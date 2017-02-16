/*****  I/O Manager File Driver Interface  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IOMFileFacility

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "IOMFile.h"

/*****  Supporting  *****/
#include "vfac.h"

#include "IOMHandle.h"

#include "V_VArgList.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (IOMFile);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMFile::IOMFile (
    Options const& rOptions, int fd, char const *pFileName, MappingMode mappingMode
)
: IOMDriver		(rOptions)
, m_filefd		(fd)
, m_mappingMode		(MappingMode_None)
, m_mappingSize		(0)
, m_mappingBase		(0)
, m_mappingCursor	(0)
{
    bool mappedOk;
    switch (mappingMode) {
    case IOMFile::MappingMode_ReadOnly:
	mappedOk = m_fileMapping.Map (pFileName, true, VkMemory::Share_Group);
	break;
    case IOMFile::MappingMode_ReadWrite:
	mappedOk = m_fileMapping.Map (pFileName, false, VkMemory::Share_Group);
	break;
    default:
	mappedOk = false;
    }
    
    if (mappedOk) {
	m_mappingMode	= mappingMode;
	m_mappingSize	= m_fileMapping.RegionSize ();
	m_mappingBase	=
	m_mappingCursor	= (pointer_t)m_fileMapping.RegionAddress ();
    }
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IOMFile::~IOMFile () {
    Close ();
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

IOMChannelType IOMFile::channelType () const {
    return IOMChannelType_File;
}

int IOMFile::getTcpNodelay (int* fOnOff) {
    *fOnOff = false;
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void IOMFile::Close ()
{
//  Do nothing if the channel is already closed, ...
    if (isClosing ())
	return;

//  ... otherwise, flush buffered output if we can, ...
    PutBufferedData ();

//  ... mark the channel as closing, ...
    SetStateToClosing ();

//  ... and release resources:
    if (m_mappingBase) {
	m_fileMapping.Destroy ();

	m_mappingMode	= MappingMode_None;
	m_mappingSize	= 0;
	m_mappingBase	=
	m_mappingCursor	= NilOf (pointer_t);
    }

    close (m_filefd);
}

int IOMFile::EndTransmission () {
    m_iStatus.MakeFailureStatus ();
    return -1;
}

int IOMFile::EndReception () {
    m_iStatus.MakeFailureStatus ();
    return -1;
}

int IOMFile::Seek (IOMHandle const* pHandle, size_t *pOffset) {
    if (IsntNil (m_mappingBase)) {
	*pOffset = pHandle->seekOffset () > 0
	    ? pHandle->seekOffset () - 1
	    : m_mappingCursor - m_mappingBase;

	if (*pOffset > m_mappingSize) {
	    *pOffset = (size_t)-1;
	    errno = EINVAL;
	}
	else m_mappingCursor = m_mappingBase + *pOffset;

    }
    else if (m_filefd < 0)
	*pOffset = (size_t)-1;
    else *pOffset = pHandle->seekOffset () > 0 ? (size_t)lseek (
	m_filefd, pHandle->seekOffset () - 1, SEEK_SET
    ) : (size_t)lseek (m_filefd, 0, SEEK_CUR);

    if ((size_t)-1 == *pOffset) {
	m_iStatus.MakeErrorStatus ();
	return false;
    }

    return true;
}

int IOMFile::SetTcpNodelay (int Unused (fOnOff)) {
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/*******************
 *******************
 *****  Input  *****
 *******************
 *******************/

/*---------------------------------------------------------------------------
 *****  'GetByteCount' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.
 *
 *  Returns:
 *	The number of bytes actually available for reading.  Always zero for this
 *	driver.
 *
 *****/
size_t IOMFile::GetByteCount (IOMHandle const* pHandle) {
    size_t seekOffset;
    if (!Seek (pHandle, &seekOffset))
	return 0;

    if (IsntNil (m_mappingBase))
	return m_mappingSize - seekOffset;
		  
#if defined(__VMS) || defined(_WIN32) || defined(sun) || defined (__linux__) || defined(__APPLE__)
    struct stat stats;
    return fstat (m_filefd, &stats) >= 0
	&& (size_t)stats.st_size > seekOffset
    ? (size_t)(stats.st_size - seekOffset)
    : 0;
#else
    int bytesAvailable;
    return ioctl (m_filefd, FIONREAD, &bytesAvailable) >= 0
    ? bytesAvailable
    : 0;
#endif
}


/*---------------------------------------------------------------------------
 *****  'GetData' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.
 *	pBuffer			- the address of the buffer to fill.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns zero if at least 'cbMin'
 *				  bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.
 *
 *  Returns:
 *	The actual number of bytes returned in '*pBuffer'.  Always zero for this
 *	driver.
 *
 *****/
size_t IOMFile::GetData (
    IOMHandle const* pHandle, void *pBuffer, size_t cbMin, size_t cbMax
) {
    size_t seekOffset, bytesRead;

    if (!Seek (pHandle, &seekOffset))
	return 0;

    if (IsntNil (m_mappingBase))
    {
	bytesRead = m_mappingSize - seekOffset;
	if (bytesRead < cbMin)
	    return 0;
	if (bytesRead > cbMax)
	    bytesRead = cbMax;

	memcpy (pBuffer, m_mappingCursor, bytesRead);
	m_mappingCursor += bytesRead;

	return bytesRead;
    }

#ifdef __VMS
    char* pbuf = static_cast<char*>(pBuffer);
    bytesRead = 0;
    ssize_t tmpBytesRead;
    do {
	tmpBytesRead = read(m_filefd, pbuf + bytesRead, cbMax - bytesRead);
	if (tmpBytesRead <= 0)
	    break;
	bytesRead += tmpBytesRead;
    } while (bytesRead < cbMax);

    if (tmpBytesRead < 0) {
	m_iStatus.MakeErrorStatus();
	return 0;
    }

    // note: bytesRead may be less than cbMax even on successful reads,
    //       due to translations between VMS file formats -- read()
    //       converts all records to '\n'-delimited lines, which may
    //       be slightly smaller than the original binary data (eg,
    //       variable-length record format)

#else

    bytesRead = read (m_filefd, pBuffer, cbMax);
    if ((size_t)-1 == bytesRead) {
	m_iStatus.MakeErrorStatus ();
	return 0;
    }

#endif  // __VMS

    return bytesRead < cbMin ? 0 : bytesRead;
}


/*---------------------------------------------------------------------------
 *****  'GetString' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns a failure code if at least
 *				  'cbMin' bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.  If 'cbMax' is less than 'cbMin', all
 *				  currently available data will be read and
 *				  and returned.
 *	ppString		- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  data read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'UTIL_Free' on its address when it is no
 *				  longer needed.  '*ppString' is set to NULL if
 *				  no data was read.
 *	psString		- the length of the string returned in
 *				  '**ppString', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	The driver status at the end of the call.
 *
 *****/
VkStatusType IOMFile::GetString (
    IOMHandle const*		pHandle,
    size_t			cbMin,
    size_t			cbMax,
    char **			ppString,
    size_t *			psString
)
{
/*****  Initialize the return arguments, ...  *****/
    *ppString = NilOf (char *);
    *psString = 0;

/*****  ...  determine the maximum amount of data to transfer, ...  *****/
    if (cbMax < cbMin)
	cbMax = GetByteCount (pHandle);

/*****  ...  return immediately if no data is to be transferred, ...  *****/
    if (cbMax < cbMin)
	return VkStatusType_Failed;

/*****  ... otherwise transfer up to 'cbMax' bytes of string data:  *****/
    *ppString = (char *)allocate (cbMax + 1);
    *psString = GetData (pHandle, *ppString, cbMin, cbMax);
    (*ppString) [*psString] = '\0';

    return VkStatusType_Completed;
}


/*---------------------------------------------------------------------------
 *****  'GetLine' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.
 *	zPrompt			- the address of a NULL terminated string used
 *				  as an input prompt.  May be NULL if no prompt
 *				  is desired.  Always ignored by this driver.
 *	ppLine			- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  line read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'UTIL_Free' on its address when it is no
 *				  longer needed.  '*ppLine' is set to NULL if
 *				  no data was read.
 *	psLine			- the length of the string returned in
 *				  '**ppLine', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	One of the following VkStatusType values:
 *		Completed
 *		Failed
 *
 *****/
VkStatusType IOMFile::GetLine (
    IOMHandle const*		pHandle,
    char const*			Unused(zPrompt),
    char**			ppLine,
    size_t *			psLine
)
{
    size_t			seekOffset;
    char *			lineBreak;
    char *			pLine;
    size_t			sLine,
				scanLength;
    char			scanBuffer[4096];

/*****  Initialize the return arguments to their default value, ...  *****/
    *ppLine = NilOf (char *);
    *psLine = 0;

/*****  ... fail if the file pointer could not be positioned, ...  *****/
    if (!Seek (pHandle, &seekOffset))
	return VkStatusType_Failed;

/*****  ... handle the trivial case of a mapped file, ...  *****/
    if (IsntNil (m_mappingBase))
    {
	sLine = m_mappingSize - seekOffset;
	lineBreak = (char*)memchr (m_mappingCursor, '\n', sLine);
	if (IsntNil (lineBreak))
	    sLine = (size_t)(lineBreak - m_mappingCursor) + 1;

	pLine = (char*)allocate (sLine + 1);
	memcpy (pLine, m_mappingCursor, sLine);
	pLine[sLine] = '\0';

	m_mappingCursor += sLine;

	*ppLine = pLine;
	*psLine = sLine;

	return VkStatusType_Completed;
    }

/*****  ... and read data until a '\n' or end of file is found, ...  *****/
    for (
	sLine = 0;

	(ssize_t)(
	    scanLength = read (m_filefd, scanBuffer, sizeof (scanBuffer))
	) > 0 && IsNil (lineBreak = (char*)memchr (scanBuffer, '\n', scanLength));

	sLine += scanLength
    );

    if ((ssize_t)scanLength > 0) sLine += IsntNil (
	lineBreak
    ) ? (size_t)(lineBreak - scanBuffer) + 1 : scanLength;

/*****  ... reseek the start of the line, ...  *****/
    if ((ssize_t)lseek (m_filefd, seekOffset, SEEK_SET) < 0)
	return VkStatusType_Failed;

/*****  ... allocate the result buffer, ...  *****/
    pLine = (char*)allocate (sLine + 1);

/*****  ... re-read the line,  ...  *****/
    if (sLine != (size_t)read (m_filefd, pLine, sLine)) {
	deallocate (pLine);
	return VkStatusType_Failed;
    }
    pLine[sLine] = '\0';

/*****  ... and return it:  *****/
    *ppLine = pLine;
    *psLine = sLine;

    return VkStatusType_Completed;
}


/********************
 ********************
 *****  Output  *****
 ********************
 ********************/

/******************
 *****  Flush  ****
 ******************/

bool IOMFile::PutBufferedData () {
    if (IsntNil (m_mappingBase))
	m_fileMapping.Sync ();
    else if (m_filefd >= 0)
	fsync (m_filefd);
    return true;
}


/*****************
 *****  Raw  *****
 *****************/

size_t IOMFile::PutData (IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer) {
    size_t seekOffset, bytesWritten;

    if (!Seek (pHandle, &seekOffset))
	return 0;

    if (MappingMode_ReadWrite == m_mappingMode)
    {
	bytesWritten = m_mappingSize - seekOffset;
	if (bytesWritten > sBuffer)
	    bytesWritten = sBuffer;

	memcpy (m_mappingCursor, pBuffer, bytesWritten);
	m_mappingCursor += bytesWritten;

	return bytesWritten;
    }

    bytesWritten = write (m_filefd, pBuffer, sBuffer);
    if ((size_t)-1 == bytesWritten) {
	m_iStatus.MakeErrorStatus ();
	return 0;
    }

    return bytesWritten;
}


/***********************
 *****  Character  *****
 ***********************/

size_t IOMFile::PutString (char const *string) {
    IOMHandle handle;
    handle.construct (this);
    return PutData (&handle, string, strlen (string));
}

size_t IOMFile::VPrint (size_t Unused(sData), char const* pFormat, va_list pFormatArgList) {
/*****  Determine the byte count to be written...  *****/
    int byteCount = 0; {
	FILE *const sink = fopen ("/dev/null", "a");
	if (!sink) {
	    m_iStatus.MakeErrorStatus ();
	    return 0;
	}
	V::VArgList pArgListCopy (pFormatArgList);
	byteCount = vfprintf (sink, pFormat, pArgListCopy);
	fclose (sink);
    }

/*****  ... allocate a work buffer if necessary, ...  *****/
    if (byteCount <= 0)
	return 0;

/*****  ... re-format and display the result, ...  *****/
    char *workBuffer = reinterpret_cast<char*>(allocate (byteCount + 1));
    vsprintf (workBuffer, pFormat, pFormatArgList);
    PutString (workBuffer);

/*****  ... and cleanup and return:  *****/
    deallocate (workBuffer);
    return byteCount;
}


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (IOMFile);
    FAC_FDFCase_FacilityDescription ("I/O File Driver");
    default:
        break;
    }
}
