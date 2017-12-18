/*****  The Persistent Storage Manager  *****/

#ifdef __VMS

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#endif

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName PS

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"
#include "VkMemory.h"

/*****  Facility  *****/
#include "gopt.h"
#include "m.h"
#include "ts.h"

#include "vdbupdate.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vunwind.h"
#include "vutil.h"

#include "VkStatus.h"

#include "V_VAllocator.h"
#include "V_VString.h"
#include "V_VTime.h"

#include "VSimpleFile.h"

/*****  Self  *****/
#include "ps.h"

#ifdef __VMS
#include <rms.h>
#include <starlet.h>
#include <unixlib.h>

#endif



/**************************
 **************************
 *****  Global State  *****
 **************************
 **************************/

/**********************
 *****  Switches  *****
 **********************/

PublicVarDef bool		PS_TracingSegmentScans		= false,
				PS_TracingNetworkAccess		= false,
				PS_ValidatingChecksums		= true;

/**********************
 *****  Counters  *****
 **********************/

PublicVarDef unsigned int	PS_SegmentScanCount		= 0,
				PS_SegmentReclamationCount	= 0,
				PS_SegmentReclamationMinimum	= INT_MAX,
				PS_SegmentReclamationMaximum	= 0,
				PS_PCTPrivatizations		= 0;


/************************
 *****  Parameters  *****
 ************************/

/*---------------------------------------------------------------------------
 *	LargeContainerSize	- a threshold used to determine how to
 *				  group containers in segments during update.
 *	MappedSegmentLimit	- the maximum number of segments that can be
 *				  mapped before a segment scan is attempted.
 *	NDFSyncRetries		- the maximum number of ndf synchronization
 *				  retries that will be attempted before giving
 *				  up on an update.
 *	SegmentSyncRetries	- the maximum number of segment synchronization
 *				  retries that will be attempted before giving
 *				  up on an update.
 *---------------------------------------------------------------------------
 */

PrivateVarDef bool		CommitsDisabled		= false;

PrivateVarDef void const*
#if defined(sun) && !defined (_LP64)
				MappedAddressThreshold	= (void const*)0x80000000;
#else
				MappedAddressThreshold	= 0;
#endif

PrivateVarDef unsigned int	MappedSegmentLimit	= INT_MAX,
				NDFSyncRetries		= 10,
				SegmentSyncRetries	= 10,
				SegmentOpenRetries	= 5,
				SegmentOpenRetryDelay	= 10,
				MaxCompactionSegments	= INT_MAX;
PrivateVarDef bool		CompactSmallSegments	= false;
PrivateVarDef size_t		LargeContainerSize	= 10000,
				MaxSegmentSize		= 0x2000000,
				MaxWriteChunk		= INT_MAX;

/**********************
 *****  Counters  *****
 **********************/
PrivateVarDef unsigned int	SuccessfulSegmentMaps	= 0;


/***************************
 *****  Other Globals  *****
 ***************************/

/*---------------------------------------------------------------------------
 *	TransactionId		- the ID of the current transaction.
 *---------------------------------------------------------------------------
 */
PrivateVarDef PS_TID	TransactionId;


/***************************************
 ***************************************
 *****  Error Signaling Utilities  *****
 ***************************************
 ***************************************/

PrivateFnDef void SignalUnixError (char const *pErrorMessage) {
    char message[4096];

    char const *pSystemErrorMessage = strerror (errno);
    STD_sprintf	(
	message,
	"%s: '%s'",
	pErrorMessage,
	pSystemErrorMessage ? pSystemErrorMessage : UTIL_FormatMessage ("Error#%d", errno)
    );

    ERR_SignalFault (EC__SystemFault, message);
}

#define TrapUnixError(returnCode, pErrorMessage) if ((int)(returnCode) < 0)\
    SignalUnixError (pErrorMessage);\
else

#define TrapDirectoryError(returnCode, pErrorMessage) if (IsNil (returnCode))\
    SignalUnixError (pErrorMessage);\
else


/*****************************************
 *****************************************
 *****  User Id Management Routines  *****
 *****************************************
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to return a user id for this process.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	A 'PS_UID' for this process.
 *
 *****/
PrivateFnDef PS_UID CurrentUser () {
    PS_UID			cd;

    PS_UID_RealUID	(cd) = (unsigned short)getuid ();
    PS_UID_EffectiveUID	(cd) = (unsigned short)geteuid ();
    PS_UID_RealGID	(cd) = (unsigned short)getgid ();
    PS_UID_EffectiveGID	(cd) = (unsigned short)getegid ();

    return cd;
}


/********************************************
 ********************************************
 *****  Time Stamp Management Routines  *****
 ********************************************
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to access the current time.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	A 'PS_TS' for the current time.
 *
 *****/
PrivateFnDef PS_TS CurrentTime () {
    PS_TS time;

    TrapUnixError (gettime (&time), "Time Access");

    return time;
}

/*---------------------------------------------------------------------------
 *****  Format a second resolution time stamp.
 *
 *  Arguments:
 *	tsp			- the address of the time stamp to format.
 *
 *  Returns:
 *	The address of a reused buffer containing the formatted representation
 *	of the "seconds" field of the time stamp.
 *
 *****/
PrivateFnDef char const *Ctimeval (PS_TS *tsp) {
    V::VTime iTime (*tsp);
    struct tm iTimeStruct;
    struct tm *tm = iTime.localtime (iTimeStruct);

/****  using 'mm/dd/yy:hh:mm:ss' format  ****/
    static char buffer[80];
    STD_sprintf (
	buffer,
	"%02d/%02d/%4d:%02d:%02d:%02d",
	tm->tm_mon + 1,
	tm->tm_mday,
	tm->tm_year + 1900,
	tm->tm_hour,
	tm->tm_min,
	tm->tm_sec
    );

    return buffer;
}


/********************************
 ********************************
 *****  File IO Primitives  *****
 ********************************
 ********************************/

#ifdef __VMS
namespace
{
    bool vms_set_file_permissions(const char*);
}
#endif

class NDFWriter : public VTransient {
// Construction
public:
    NDFWriter (int hNDF);
    NDFWriter (char const* pNDFPathName);

// Destruction
public:
    ~NDFWriter () {
    }

// Helpers
private:
    void AscertainSizeOfNDF () {
	struct stat fileStats;
	TrapUnixError (
	    fstat (m_hNDF, &fileStats),
	    "AscertainingSize of NDF"
	    ) m_iInitialOffset = m_sNDF = fileStats.st_size;
    }

    void Write (void const* buffer, unsigned int bytesToWrite);
    PS_Type_FO Seek (PS_Type_FO fo);
    void AccumulateChecksum (void *structure, unsigned int structureSize);

public:
    void ValidateChecksum ();
   
// Published Functions
public:
    PS_Type_FO WriteStructure    (PS_Type_FO fo, void *structure, unsigned int structureSize);
    PS_Type_FO WriteCountedArray (PS_Type_FO fo, void const* array, unsigned int elementCount, unsigned int elementGrain);
    void Close  ();
    void Synchronize (const char* ndfPathName);

// State
private:
    int			m_hNDF;
    off_t		m_iInitialOffset;
    off_t		m_sNDF;
    unsigned int	m_iChecksum;

};

NDFWriter::NDFWriter (int hNDF)
: m_hNDF (hNDF)
, m_iChecksum (0)
{
    AscertainSizeOfNDF ();
}

NDFWriter::NDFWriter (char const* pNDFPathName) 
: m_iChecksum (0)
{

    m_hNDF = open (pNDFPathName, O_RDWR|O_CREAT|O_EXCL, 0666);

    TrapUnixError (
	m_hNDF, UTIL_FormatMessage (
	    "New NDF Creation [%s]", pNDFPathName
	)
    );

    AscertainSizeOfNDF ();
}
/*---------------------------------------------------------------------------
 *****  funciton to position an NDF descriptor's file pointer at a specified offset.
 *
 *  Arguments:
 *	fo			- a file offset.  'PS_FO_Nil' values of 'fo'
 *				  will seek the current end of file.
 *
 *  Returns:
 *	new file offset
 *
 *****/
PS_Type_FO NDFWriter::Seek (PS_Type_FO fo) {
    off_t actualFO;
    if (PS_FO_IsNil (fo)) fo = (PS_Type_FO) m_sNDF;
    TrapUnixError (
	actualFO = lseek (m_hNDF, fo, SEEK_SET),
	UTIL_FormatMessage ("NDF Seek To Offset [%d]", fo)
    );
    return (PS_Type_FO)actualFO;
}

/*---------------------------------------------------------------------------
 *****  Internal utility to write a structure to an NDF.
 *
 *  Arguments:
 *	fo			- the file offset at which the structure is
 *				  to be written.  If 'PS_FO_IsNil (fo)', the
 *				  structure will be written at the current
 *				  end of file.
 *	structure 		- the address of a buffer supplying the
 *				  structure to be written.
 *	structureSize		- the number of bytes in the structure.
 *
 *  Returns:
 *	The actual file offset at which the structure was written.
 *
 *****/
PS_Type_FO NDFWriter::WriteStructure (
    PS_Type_FO			fo,
    void			*pStructure,
    unsigned int		sStructure
)
{
    PS_Type_FO actualFO = Seek (fo);
    Write (pStructure, sStructure);
    if (actualFO == m_sNDF) {
	AccumulateChecksum (pStructure, sStructure);
	m_sNDF += sStructure;
    }
    return actualFO;
}

/*---------------------------------------------------------------------------
 *****  Internal utility to write a counted array to an NDF.
 *
 *  Arguments:
 *	fo			- the file offset at which the counted array
 *				  if to be written.  If 'PS_FO_IsNil (fo)',
 *				  the array will be written at the current end
 *				  of file.
 *	array			- the address of a buffer supplying the array
 *				  elements to be written.
 *	elementCount		- the number of array elements to write.
 *	elementGrain		- the size in bytes of an array element.
 *
 *  Returns:
 *	The actual file offset at which the array was written.
 *
 *****/
PS_Type_FO NDFWriter::WriteCountedArray (
    PS_Type_FO			fo,
    void const*			array,
    unsigned int		elementCount,
    unsigned int		elementGrain
)
{
    PS_Type_FO			actualFO;
    unsigned int		sPadding = 4 - ((elementCount * elementGrain) % 4);
    void*			buffer;

    if (4 == sPadding) sPadding = 0;
    unsigned int sWrite = elementCount * elementGrain + sPadding;
    buffer = UTIL_Malloc (sWrite);
    memset (buffer, 0, sWrite);
    memcpy (buffer, array, sWrite);

    actualFO = Seek (fo);
    Write (&elementCount, sizeof (int));
    Write (buffer, sWrite);

    if (actualFO == m_sNDF) {
	AccumulateChecksum (&elementCount, sizeof (int));
	AccumulateChecksum (buffer, sWrite);
	m_sNDF += sizeof (int) + sWrite;
    }
    UTIL_Free (buffer);
    return actualFO;
}

void NDFWriter::AccumulateChecksum (
    void *			pStructure,
    unsigned int		sStructure
)
{
    unsigned int		*wp, *wl;

    wp = (unsigned int *)pStructure;
    wl = wp 
       + (sStructure + sizeof (unsigned int) - 1) / sizeof (unsigned int);
    while (wp < wl) m_iChecksum ^= *wp++;
}

PrivateFnDef void NDFReadStructure (
    int	fd, PS_Type_FO fo, void	*pStructure, size_t sStructure
);

void NDFWriter::ValidateChecksum () {
    unsigned int totalAmtWritten = (unsigned int)(m_sNDF - m_iInitialOffset);
    unsigned int writtenChecksum = m_iChecksum;
    int retryCount = NDFSyncRetries;

    for (m_iChecksum = 0; m_iChecksum != writtenChecksum; ){
	m_iChecksum = 0;
	Synchronize ("ValidateChecksum");
	void* buffer = UTIL_Malloc (totalAmtWritten);
	NDFReadStructure (m_hNDF, (PS_Type_FO)m_iInitialOffset, buffer, totalAmtWritten);

	AccumulateChecksum (buffer, totalAmtWritten);
	UTIL_Free (buffer);

	if (m_iChecksum != writtenChecksum)
	    if (retryCount-- <= 0)
		ERR_SignalFault (
	         EC__SystemFault,
	         UTIL_FormatMessage (
			 "NDF(size:%u) written checksum %08x not equal to read checksum %08x",
			 m_sNDF, writtenChecksum, m_iChecksum)
	       );
	    else {
		sleep (5);
		log ("NDFWriter::ValidateChecksum: retry=%d", NDFSyncRetries - retryCount);
	    }
    }
}

void NDFWriter::Synchronize (char const *ndfPathName) {
    int retryCount = NDFSyncRetries;

    while (fsync (m_hNDF) < 0) {
	if (retryCount-- <= 0) SignalUnixError (
	    UTIL_FormatMessage ("NDF '%s' Synchronization", ndfPathName)
	);
	sleep (5);
    }
}

void NDFWriter::Close () {
    TrapUnixError (
	close (m_hNDF),
	UTIL_FormatMessage ("NDF Close")
    );
}

/****************************************
 *****  Synchronization Primitives  *****
 ****************************************/

PrivateFnDef void SynchronizeNDF (int ndfFD, char const *ndfPathName) {
    int retryCount = NDFSyncRetries;

    while (fsync (ndfFD) < 0) {
	if (retryCount-- <= 0) SignalUnixError (
	    UTIL_FormatMessage ("NDF '%s' Synchronization", ndfPathName)
	);
	sleep (5);
    }
}

PrivateFnDef void SynchronizeAndCloseSegment (
    int				segmentFD,
    int				spaceIndex,
    int				segmentIndex
)
{
    int				retryCount = SegmentSyncRetries;

    while (fsync (segmentFD) < 0)
    {
	if (retryCount-- <= 0) SignalUnixError (
	    UTIL_FormatMessage (
		"Segment '%d/%d' Synchronization", spaceIndex, segmentIndex
	    )
	);
	sleep (5);
    }

    TrapUnixError (
	close (segmentFD),
	UTIL_FormatMessage ("Segment '%d/%d' Close", spaceIndex, segmentIndex)
    );
}


/**************************************
 *****  Byte Level IO Primitives  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Function to position an NDF descriptor's file pointer.
 *
 *  Arguments:
 *	xFD		- an open NDF descriptor.
 *	xSeek		- a file offset.  
 *
 *  Returns:
 *	the absolute offset at which the file pointer is positioned
 *	as a result of this seek.
 *
 *****/
static PS_Type_FO NDFSeek (int xFD, PS_Type_FO xSeek) {
    off_t xActualFO = lseek (xFD, xSeek, SEEK_SET);
    TrapUnixError (xActualFO, UTIL_FormatMessage ("NDF Seek To Offset [%d]", xSeek));
    return (PS_Type_FO)xActualFO;
}

/*---------------------------------------------------------------------------
 *****  Function to position a SEGMENT file descriptor's file pointer.
 *
 *  Arguments:
 *	xFD		- an open NDF descriptor.
 *	xSeek		- a file offset.  'PS_FO_Nil' values of 'xSeek'
 *			  will seek the current end of file.
 *
 *  Returns:
 *	the absolute offset at which the file pointer is positioned
 *	as a result of this seek.
 *
 *****/
static PS_Type_FO SegmentSeek (int xFD, PS_Type_FO xSeek) {
    bool bEnd = PS_FO_IsNil (xSeek);
    off_t xActualFO = bEnd ? lseek (xFD, 0, SEEK_END) : lseek (xFD, xSeek, SEEK_SET);
    if (xActualFO < 0) SignalUnixError (
	bEnd ? UTIL_FormatMessage (
	    "Segment Seek To End of File"
	) : UTIL_FormatMessage (
	    "Segment Seek To Offset [%d]", xSeek
	)
    );
    return (PS_Type_FO)xActualFO;
}


/*---------------------------------------------------------------------------
 *****  Internal macro to read a collection of bytes from an NDF or Segment.
 *
 *  Arguments:
 *	xFD			- an open NDF descriptor.
 *	pBuffer			- the address of a buffer which will be filled
 *				  by the read.
 *	sBuffer			- the number of bytes to read.
 *
 *  Syntactic Context:
 *	Returns the number of bytes read.
 *
 *****/
static ssize_t Read(int xFD, void *pBuffer, size_t sBuffer, char const *pWhat) {
    ssize_t sRead = read (xFD, pBuffer, sBuffer);
    if (sRead < 0) SignalUnixError (
	UTIL_FormatMessage ("%s Read", pWhat)
    );
    return sRead;
}

static ssize_t NDFRead (int xFD, void *pBuffer, size_t sBuffer) {
    return Read (xFD, pBuffer, sBuffer, "NDF");
}

static ssize_t SegmentRead (int xFD, void *pBuffer, size_t sBuffer) {
    return Read (xFD, pBuffer, sBuffer, "Segment");
}


/*---------------------------------------------------------------------------
 *****  Internal utility to write a collection of bytes to a data base entity.
 *
 *  Arguments:
 *	isAnNDFWrite		- true == NDF, false == Segment
 *	fd			- an open NDF descriptor.
 *	pStructure		- the address of the structure to write.
 *	sStructure		- the number of bytes in the structure.
 *
 *****/
PrivateFnDef void DoWrite (
    bool isAnNDFWrite, int fd, void const* pStructure, size_t sStructure
) {
    char const *pWhat = isAnNDFWrite ? "NDF Write" : "Segment Write";

    char const*	 pUnwrittenBytes = (char const*)pStructure;
    size_t nUnwrittenBytes = sStructure;
    do {
	size_t nBytesToWrite = MaxWriteChunk < nUnwrittenBytes
	    ? MaxWriteChunk : nUnwrittenBytes;

	ssize_t nBytesWritten = write (fd, pUnwrittenBytes, nBytesToWrite);

	if (-1 == nBytesWritten)
	    SignalUnixError (pWhat);

	pUnwrittenBytes += nBytesWritten;
	nUnwrittenBytes -= nBytesWritten;
    } while (nUnwrittenBytes > 0);
}

/*---------------------------------------------------------------------------
 *****  Internal macro to write a collection of bytes to the NDF.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
 *	buffer			- the address of a buffer which will supply
 *				  the data to be written.
 *	bytesToWrite		- the number of bytes to write.
 *
 *****/
void NDFWriter::Write (void const* buffer, unsigned int bytesToWrite) {
    DoWrite (true, m_hNDF, buffer, (unsigned int)(bytesToWrite));
}
/*---------------------------------------------------------------------------
 *****  Internal macro to write a collection of bytes to a SEGMENT file.
 *
 *  Arguments:
 *	fd			- an open segment descriptor.
 *	buffer			- the address of a buffer which will supply
 *				  the data to be written.
 *	bytesToWrite		- the number of bytes to write.
 *
 *****/
#define SegmentWrite(fd, buffer, bytesToWrite) DoWrite (false, fd, buffer, (unsigned int)(bytesToWrite))


/***************************************************
 *****  Generic Structure Level IO Primitives  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to read a structure from an NDF.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
 *	fo			- the file offset at which the structure
 *				  resides.
 *	pStructure		- the address of a buffer which will be
 *				  filled with the structure.
 *	sStructure		- the size of the structure in bytes.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void NDFReadStructure (
    int fd, PS_Type_FO fo, void *pStructure, size_t sStructure
) {
    NDFSeek (fd, fo);
    NDFRead (fd, pStructure, sStructure);
}

/*---------------------------------------------------------------------------
 *****  Internal utility to read a structure from a SEGMENT file.
 *
 *  Arguments:
 *	fd			- an open segment descriptor pointer.
 *	fo			- the file offset at which the structure
 *				  resides.
 *	pStructure		- the address of a buffer which will be
 *				  filled with the structure.
 *	sStructure		- the size of the structure in bytes.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void SegmentReadStructure (
    int fd, PS_Type_FO fo, void *pStructure, size_t sStructure
) {
    SegmentSeek (fd, fo);
    SegmentRead (fd, pStructure, sStructure);
}

PrivateFnDef void AccumulateChecksum (
    unsigned int *checksum, void const *pStructure, size_t sStructure
) {
    unsigned int *wp = (unsigned int*)pStructure;
    unsigned int *wl = wp + (sStructure + sizeof (unsigned int) - 1) / sizeof (unsigned int);

    while (wp < wl) *checksum ^= *wp++;
}


/*---------------------------------------------------------------------------
 *****  Internal utility to write a structure to a SEGMENT file.
 *
 *  Arguments:
 *	fd			- an open segment descriptor.
 *	fo			- the file offset at which the structure is
 *				  to be written.  If 'PS_FO_IsNil (fo)', the
 *				  structure will be written at the current
 *				  end of file.
 *	pStructure 		- the address of a buffer supplying the
 *				  structure to be written.
 *	sStructure		- the number of bytes in the structure.
 *	checksum		- the address of a checksum to update or Nil.
 *
 *  Returns:
 *	The actual file offset at which the structure was written.
 *
 *****/
PrivateFnDef PS_Type_FO SegmentWriteStructure (
    int				fd,
    PS_Type_FO			fo,
    void const*			pStructure,
    size_t			sStructure,
    unsigned int*		checksum
) {
    if (checksum) AccumulateChecksum (
	checksum, pStructure, sStructure
    );

    PS_Type_FO actualFO = SegmentSeek (fd, fo);
    SegmentWrite (fd, pStructure, sStructure);

    return actualFO;
}


/*******************************************************
 *****  Generic Counted Array Level IO Primitives  *****
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to read a counted array from an NDF.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
 *	fo			- the file offset of the counted array.
 *	array			- the address of a buffer which will be filled
 *				  with the counted array.
 *	elementCount		- the maximum number of array elements which
 *				  can be accomodated by 'array'.
 *	elementGrain		- the size in bytes of an array element.
 *
 *  Returns:
 *	The actual number of array elements read.  This value will never
 *	exceed the value of the 'elementCount' parameter.  It will be less
 *	if fewer than 'elementCount' elements exist to be read.
 *
 *****/
PrivateFnDef int NDFReadCountedArray (
    int fd, PS_Type_FO fo, void *array, size_t elementCount, size_t elementGrain
) {
    NDFSeek (fd, fo);

    unsigned int  actualElementCount;
    NDFRead (fd, &actualElementCount, sizeof (actualElementCount));

    if (actualElementCount > elementCount)
	actualElementCount = elementCount;

    NDFRead (fd, array, actualElementCount * elementGrain);

    return actualElementCount;
}


/*---------------------------------------------------------------------------
 *****  Internal utility to read the element count counted array from an NDF.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
 *	fo			- the file offset of the counted array.
 *
 *  Returns:
 *	The number elements in the counted array.
 *
 *****/
PrivateFnDef int NDFReadCountedArrayElementCount (int fd, PS_Type_FO fo) {
    NDFSeek (fd, fo);

    int	elementCount = 0;
    NDFRead (fd, (pointer_t)&elementCount, sizeof (int));

    return elementCount;
}


/*---------------------------------------------------------------------------
 *****  Internal utility to read a counted array element from an NDF.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
 *	fo			- the file offset of the counted array.
 *	elementSubscript	- the subscript of the element to be read.  No
 *				  bounds checking is performed on this value.
 *	element			- the address of a buffer which will be filled
 *				  with the element.
 *	elementGrain		- the size in bytes of an array element.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void NDFReadCountedArrayElement (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		elementSubscript,
    void *			element,
    unsigned int		elementGrain
) {
    NDFSeek (fd, fo + sizeof (int) + elementSubscript * elementGrain);
    NDFRead (fd, element, elementGrain);
}


/*****************************************
 *****  Type Specific IO Primitives  *****
 *****************************************/

/****************
 *  Structures  *
 ****************/

/*****  Network Directory Header  *****/
PrivateFnDef void ReadNDH (int fd, PS_NDH *ndh) {
    PS_NDH_Signature2 (*ndh) = 0;

    NDFReadStructure (fd, 0, ndh, sizeof (PS_NDH));

    if (PS_NetworkDirectorySignature == PS_NDH_Signature2 (*ndh)) return;

    PS_NDH_Signature2 (*ndh) = PS_NetworkDirectorySignature;
    PS_NDH_LayoutType (*ndh) = 1;
    PS_NDH_Generation (*ndh) = 0;

    memset (
	(char *)ndh + sizeof (ndh->components),
	0,
	sizeof (PS_NDH) - sizeof (ndh->components)
    );
}

#define WriteNDH(pNDF, ndh)\
    (pNDF)->WriteStructure (0, ndh, sizeof (PS_NDH))

/*****  Network Version Descriptor  *****/
PrivateFnDef void ReadNVD (
    int				fd,
    PS_Type_FO			fo,
    PS_NVD			*nvd
) {
    PS_NVD_Signature (*nvd) = 0;

    NDFReadStructure (fd, fo, nvd, sizeof (PS_NVD));

    if (PS_NetworkDirectorySignature == PS_NVD_Signature (*nvd)) return;

    PS_NVD_Signature		    (*nvd) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		    (*nvd) = false;
    PS_NVD_UnusedFlags		    (*nvd) = 0;
    PS_TID_High		(PS_NVD_TID (*nvd))=
    PS_TID_Low		(PS_NVD_TID (*nvd))= 0;
    PS_TID_Sequence	(PS_NVD_TID (*nvd))= fo;
    PS_NVD_MP			    (*nvd) =
    PS_NVD_TD			    (*nvd) =
    PS_NVD_Annotation		    (*nvd) =
    PS_NVD_Extensions		    (*nvd) = PS_FO_Nil;
}

#define WriteNVD(pNDF, nvd) (pNDF)->WriteStructure (PS_FO_Nil, nvd, sizeof (PS_NVD))

/*****  Space Version Descriptor  *****/
PrivateFnDef void ReadSVD (
    int				fd,
    PS_Type_FO			fo,
    PS_SVD			*svd
) {
    PS_SVD_Signature (*svd) = 0;

    NDFReadStructure (fd, fo, svd, sizeof (PS_SVD));

    if (PS_NetworkDirectorySignature == PS_SVD_Signature (*svd)) return;

    PS_SVD_Signature			(*svd) = PS_NetworkDirectorySignature;
    PS_SVD_SD				(*svd) =
    PS_SVD_MP				(*svd) =
    PS_SVD_Extensions			(*svd) = PS_FO_Nil;
}

#define WriteSVD(pNDF, svd) (pNDF)->WriteStructure (PS_FO_Nil, svd, sizeof (PS_SVD))

/*****  Segment Descriptor  *****/
#define ReadSD(fd, fo, sd) NDFReadStructure (fd, fo, sd, sizeof (PS_SD))

#define WriteSD(pNDF, sd) (pNDF)->WriteStructure (PS_FO_Nil, sd, sizeof (PS_SD))

/*****  Segment Header  *****/
#define ReadSegmentHeader(fd, sh) SegmentReadStructure (\
    fd, 0, sh, sizeof (PS_SH)\
)

#define WriteSegmentHeader(fd, sh) SegmentWriteStructure (\
    fd, 0, sh, sizeof (PS_SH), NilOf (unsigned int*)\
)

/*****  Container  *****/
PrivateFnDef PS_Type_FO WriteContainer (
    int fd, unsigned int *checksum, M_CPreamble const *preamble
) {
    PS_Type_FO			preambleFO;

    if (M_CPreamble_OSize (preamble) ||
	RTYPE_C_Undefined0 == (RTYPE_Type)M_CPreamble_RType (preamble)
    )
    {
	M_CPreamble		altPreamble;
	M_CEndMarker		altEndMarker;

	M_CEndMarker_Size (&altEndMarker)=
	M_CPreamble_NSize (&altPreamble) = M_CPreamble_OSize (preamble);
	M_CPreamble_RType (&altPreamble) = M_CPreamble_RType (preamble);
	M_CPreamble_OSize (&altPreamble) = 0;
	M_CPreamble_POP	  (&altPreamble) = M_CPreamble_POP   (preamble);

	preambleFO = SegmentWriteStructure (
	    fd, PS_FO_Nil, &altPreamble, sizeof (altPreamble), checksum
	);
	SegmentWriteStructure (
	    fd,
	    PS_FO_Nil,
	    M_CPreamble_ContainerBase (preamble),
	    M_CPreamble_OSize (preamble),
	    checksum
	);
	SegmentWriteStructure (
	    fd, PS_FO_Nil, &altEndMarker, sizeof (altEndMarker), checksum
	);
    }
    else preambleFO = SegmentWriteStructure (
	fd,
	PS_FO_Nil,
	preamble,
	M_CPreamble_NSize (preamble) + sizeof (M_CPreamble) + sizeof (M_CEndMarker),
	checksum
    );

    return preambleFO;
}


/*************
 *  Strings  *
 *************/

#define ReadString(fd, fo, buffer, bufferSize) NDFReadCountedArray (\
    fd, fo, buffer, bufferSize, sizeof (char)\
)

#define WriteString(pNDF, string) (pNDF)->WriteCountedArray (\
    PS_FO_Nil, string, 1 + strlen (string), sizeof (char)\
)


/*******************
 *  SRDs and SRVs  *
 *******************/

PrivateFnDef void Read6ByteSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		index,
    PS_SRD *			srd
) {
    struct srd6_t {
        unsigned int		role : 2;
	unsigned short		data[2];
    } altSRD;

    NDFReadCountedArrayElement (fd, fo, index, &altSRD, 6);

    memcpy (srd, altSRD.data, sizeof (altSRD.data));
    PS_SRD_Role (*srd) = altSRD.role;
}

PrivateFnDef void Read8ByteSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		index,
    PS_SRD *			srd
) {
    struct srd8_t {
        unsigned int		role : 32 - PS_FO_FieldWidth,
				     : PS_FO_FieldWidth;
	PS_Type_FO		svd;
    } altSRD;

    NDFReadCountedArrayElement (fd, fo, index, &altSRD, 8);

    PS_SRD_Role (*srd) = altSRD.role;
    PS_SRD_SVD	(*srd) = altSRD.svd;
}


PrivateFnDef int ReadSRV (
    int				fd,
    PS_Type_FO			fo,
    PS_SRD *			srv,
    unsigned int		srvSize,
    int				softwareVersion
) {
    int				srd0[2], srdCount, srdIndex;

    if (softwareVersion >= 2)
	return NDFReadCountedArray (fd, fo, srv, srvSize, sizeof (PS_SRD));

    NDFReadCountedArrayElement (fd, fo, 0, srd0, sizeof (srd0));

    srdCount = NDFReadCountedArrayElementCount (fd, fo);
    if (PS_FO_IsNil (srd0[1])) for (
	srdIndex = 0; srdIndex < srdCount; srdIndex++
    ) Read8ByteSRD (fd, fo, srdIndex, &srv[srdIndex]);
    else for (srdIndex = 0; srdIndex < srdCount; srdIndex++
    ) Read6ByteSRD (fd, fo, srdIndex, &srv[srdIndex]);

    return srdCount;
}

PrivateFnDef void ReadSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		srdIndex,
    PS_SRD *			srd,
    int				softwareVersion
) {
    if (softwareVersion >= 2) {
	NDFReadCountedArrayElement (fd, fo, srdIndex, srd, sizeof (PS_SRD));
	return;
    }

    int srd0[2];
    NDFReadCountedArrayElement (fd, fo, 0, srd0, sizeof (srd0));

    if (PS_FO_IsNil (srd0[1]))
	Read8ByteSRD (fd, fo, srdIndex, srd);
    else
	Read6ByteSRD (fd, fo, srdIndex, srd);
}

#define WriteSRV(pNDF, srdVector, srdCount) (pNDF)->WriteCountedArray (\
    PS_FO_Nil, srdVector, srdCount, sizeof (PS_SRD)\
)


/************************************************************************
 ************************************************************************
 *****  New Network Component Permission Mask Management Utilities  *****
 ************************************************************************
 ************************************************************************/

/*********************************
 *****  Default Permissions  *****
 *********************************/

#define MaskedPermission(permission) ((permission) & ~PermissionMask ())
#define SpaceCreationPermission		0777

#ifdef __VMS
#define SegmentCreationPermission	0666
#else
#define SegmentCreationPermission	0444
#endif

/*****************************
 *****  Permission Mask  *****
 *****************************/

PrivateFnDef mode_t PermissionMask () {
    static mode_t iMask = 0;
    static bool bNotInitialized = true;
    if (bNotInitialized) {
	umask (iMask = umask (0777));
	bNotInitialized = false;
    }
    return iMask;
}

PrivateFnDef void InitializePermissionMask () {
    PermissionMask ();
}


/*********************************************
 *********************************************
 *****  Path Name Construction Routines  *****
 *********************************************
 *********************************************/

/***********************************
 *****  Path Name Buffer Size  *****
 ***********************************/

#define PathNameBufferSize	1024

PrivateVarDef char PathNameBuffer[PathNameBufferSize];


/***********************************
 *****  Space Link Resolution  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to substitute any symbolic space link found in place
 *****  of a space directory.
 *
 *  Argument:
 *      spacePathName           - a string containing the space path name to
 *                                be processed.  This string will be changed
 *                                to reflect the true space path name if a
 *                                symbolic link is found.
 *
 *  Returns:
 *      Nothing
 *
 *****/
PrivateFnDef void ResolveSpaceLinks (char* spacePathName) {
    char linkPathName[PathNameBufferSize], *nlp;
    FILE *ls;

    while (
	STD_sprintf (
	    linkPathName, "%s/LINK", spacePathName
	), access (linkPathName, 0) == 0
    )
    {
#if defined(_WIN32)
	if (NULL == (ls = fopen (linkPathName, "rt"))) SignalUnixError (
#else
	if (NULL == (ls = fopen (linkPathName, "r"))) SignalUnixError (
#endif
	    UTIL_FormatMessage ("Space Link Open [%s]", linkPathName)
	);

	if (spacePathName != fgets (spacePathName, PathNameBufferSize, ls)
	) SignalUnixError (
	    UTIL_FormatMessage ("Space Link Read [%s]", linkPathName)
	);

	fclose (ls);

	if (IsntNil (nlp = strchr (spacePathName, '\n')))
	    *nlp = '\0';
    }
}


/************************************************
 *****  Space Path Name Formatting Routine  *****
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to access the path name of a space given an NDF
 *****  descriptor and 'svd' address.
 *
 *  Arguments:
 *	svdp			- the address of an SVD.
 *
 *  Returns:
 *	The address of a string containing the path name of the space.
 *	This string is placed in a buffer shared by 'SpacePathName',
 *	'SpacePathName', and 'SegmentPathName'.
 *
 *****/
char * PS_AND::SpacePathName (PS_SVD const &rSVD) const {
    STD_sprintf (
	PathNameBuffer, "%s/%s", OSDPathName (), PS_SVD_SpaceBaseName (rSVD)
    );

    ResolveSpaceLinks (PathNameBuffer);

    return PathNameBuffer;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to access the path name of a space given an ASD.
 *
 *  Arguments:
 *	asd			- an ASD for the space whose path name is
 *				  desired.
 *
 *  Returns:
 *	The address of a string containing the path name of the space.
 *	This string is placed in a buffer shared by 'SpacePathName',
 *	'SpacePathName', and 'SegmentPathName'.
 *
 *****/
char* PS_ASD::SpacePathName () const {
    return m_pAND->SpacePathName (SVD ());
}


/**************************************************
 *****  Segment Path Name Formatting Routine  *****
 **************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to format the path name of a segment.
 *
 *  Arguments:
 *	this			- an ASD for the space in which the segment
 *				  resides.
 *	segmentIndex		- the index of the segment whose path name is
 *				  desired.
 *
 *  Returns:
 *	The address of a string containing the path name of the space.
 *	This string is placed in a buffer shared by 'SpacePathName',
 *	'SpacePathName', and 'SegmentPathName'.
 *
 *****/
char const *PS_ASD::SegmentPathName (unsigned int segmentIndex) const {
    char *segmentPathName = SpacePathName ();
    STD_sprintf (
	segmentPathName + strlen (segmentPathName), "/%d", segmentIndex
    );
    return segmentPathName;
}


/******************************************
 ******************************************
 *****  Rights List Access Utilities  *****
 ******************************************
 ******************************************/

/*********************************************
 *****  Rights List Path Name Generator  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Utility to return the name of the global update rights list.
 *
 *  Arguments:
 *	ndfPathName		- a char * containing the name of the Network
 *				  Directory File with which the global update
 *				  rights list is associated.
 *
 *  Returns:
 *	The address of a string containing the name of the global update
 *	rights list.
 *
 *****/
PrivateFnDef char const *GlobalUpdateRightsListName (char const *ndfPathName) {
    STD_sprintf (PathNameBuffer, "%s.GURL", ndfPathName);

    return PathNameBuffer;
}


/************************************************
 *****  Rights List Initialization Utility  *****
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Utility to create a new rights list initialized to include the
 *****  current user.
 *
 *  Argument:
 *	ndfPathName		- the path name of the accessed NDF.
 *
 *  Returns:
 *	true if the rights list was successfully created, false otherwise.
 *
 *****/
PrivateFnDef bool InitializeRightsList (char const *ndfPathName) {
/*****  Obtain the name of the current user, ...  *****/
    char iBuffer[256];
    char const *userName = Vk_username (iBuffer, sizeof (iBuffer));
    if (IsNil (userName))
	return false;

/*****  ... create the rights list file, ...  *****/
    FILE* rightsListStream = fopen (GlobalUpdateRightsListName (ndfPathName), "w");
    if (IsNil (rightsListStream))
	return false;

/*****  ... initialize the rights list, ...  *****/
    STD_fprintf (rightsListStream, "%s\n", userName);

/*****  ... close the access rights stream, ...  *****/
    fclose (rightsListStream);

/*****  ... and return.  *****/
    return true;
}


/****************************************
 *****  Rights List Search Utility  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Utility to search a rights list for the presence of the current user.
 *
 *  Argument:
 *	this			- the 'pAND' for the network.
 *
 *  Returns:
 *	true if the user associated with the current process is present in the
 *	rights list, false otherwise.
 *
 *  Notes:
 *	This routine will return false if it is unable to access either the
 *	identity of the current user or the access rights list.
 *
 *****/
bool PS_AND::CurrentUserIsInRightsList () {
/*****  Initialize the resource limit retry mechanism, ...  *****/
    bool retryPossible = true;
    bool retryNecessary;

/*****  Obtain the name of the current user, ...  *****/
    char iBuffer[256];
    char const *userName;
    do {
	userName = Vk_username (iBuffer, sizeof (iBuffer));
	if (IsNil (userName) && retryPossible)
	{
	    M_ReclaimSegments ();
	    retryPossible = false;
	    retryNecessary = true;
	}
	else retryNecessary = false;
    } while (retryNecessary);

    if (IsNil (userName))
	return false;

/*****  ... open the global update access rights file, ...  *****/
    FILE *rightsListStream;
    do {
	rightsListStream = fopen (
	    GlobalUpdateRightsListName (NDFPathName ()), "r"
	);
	if (IsNil (rightsListStream) && retryPossible)
	{
	    M_ReclaimSegments ();
	    retryPossible = false;
	    retryNecessary = true;
	}
	else retryNecessary = false;
    } while (retryNecessary);

    if (IsNil (rightsListStream))
	return false;

/*****  ... search for the user in the access rights stream, ...  *****/
    bool result = false;
    char rightsEntry [80];

    while (IsntNil (fgets (rightsEntry, sizeof rightsEntry, rightsListStream))) {
	rightsEntry [sizeof (rightsEntry) - 1] = '\0';
	rightsEntry [strlen (rightsEntry) - 1] = '\0';
	if (strncmp (userName, rightsEntry, sizeof rightsEntry) == 0) {
	    result = true;
	    break;
	}
    }

/*****  ... close the access rights stream, ...  *****/
    fclose (rightsListStream);

/*****  ... and return.  *****/
    return result;
}


/******************************************************
 *****  Global Resource Manager Public Interface  *****
 ******************************************************/

PublicFnDef int PS_GRMRequestsArePending () {
#if defined(__hp9000s800) || defined(__hp9000s700)
    return VkMemory_GRMRequestCount () > 0;
#else
    return false;
#endif
}


/**************************************
 **************************************
 *****  Segment Access Utilities  *****
 **************************************
 **************************************/

/*********************************************************
 *****  Segment Mapping Vector Management Utilities  *****
 *********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to allocate a new Segment Mapping Vector.
 *
 *  Argument:
 *	rootSegment		- the index of the root segment.
 *	minSegment		- the index of the minimum segment.
 *
 *  Returns:
 *	The address of the allocated SMV.
 *
 *****/
PrivateFnDef PS_SMD *AllocateSMV (unsigned int rootSegment, unsigned int minSegment) {
    unsigned int byteCount = PS_SMV_ByteCount (rootSegment, minSegment);
    PS_SMD *result = (PS_SMD*)UTIL_Malloc (byteCount);

    memset ((void *)result, 0, byteCount);

    return result;
}


/*---------------------------------------------------------------------------
 *****  Routine to grow a Segment Mapping Vector.
 *
 *  Argument:
 *	oldSMV			- the address of the old segment mapping
 *				  vector.
 *	oldRootSegment		- the index of the old root segment.
 *	newRootSegment		- the index of the new root segment.
 *	minSegment		- the index of the minimum segment.
 *
 *  Returns:
 *	The address of the reallocated SMV.
 *
 *****/
PrivateFnDef PS_SMD *GrowSMV (
    PS_SMD*			oldSMV,
    unsigned int		oldRootSegment,
    unsigned int		newRootSegment,
    unsigned int		minSegment
)
{
    int oldByteCount = PS_SMV_ByteCount (oldRootSegment, minSegment);
    int newByteCount = PS_SMV_ByteCount (newRootSegment, minSegment);
    PS_SMD *result = (PS_SMD*)UTIL_Realloc (oldSMV, newByteCount);

    memset ((pointer_t)result + oldByteCount, 0, newByteCount - oldByteCount);

    return result;
}


void PS_ASD::GarnerNDFSegmentInfo () {
//  Re-open the NDF if necessary, ...
    int ndfd = m_pAND->DirectoryFD ();
    if (ndfd < 0) {
#ifdef __VMS
	ndfd = open (m_pAND->NDFPathName (), O_RDONLY, 0, "shr=upd,put,get,nql");
#else
	ndfd = open (m_pAND->NDFPathName (), O_RDONLY, 0);
#endif
	TrapUnixError (
	    ndfd, UTIL_FormatMessage (
		"GarnerSegmentInfo: NDF Open [%s]", m_pAND->NDFPathName ()
	    )
	);
    }

//  ... read the required segment info, ...
    PS_Type_FO sdId = PS_SVD_SD (SVD ());
    while (PS_FO_IsntNil (sdId)) {
	PS_SD sd;
	ReadSD (ndfd, sdId, &sd);
	if (PS_SD_Segment (sd) < (unsigned) MinSegment ())
	    break;

	PS_SMD_ExpectedTID (SMD (PS_SD_Segment (sd))) = PS_SD_TID (sd);
	PS_SMD_ExpectedRID (SMD (PS_SD_Segment (sd))) = PS_SD_RID (sd);

	sdId = PS_SD_Predecessor (sd);
    }

//  ... and close the NDF if it was opened locally.
    if (m_pAND->DirectoryFD () < 0) {
	TrapUnixError (
	    close (ndfd), UTIL_FormatMessage (
		"GarnerSegmentInfo: NDF Close [%s]", m_pAND->NDFPathName ()
	    )
	);
    }
}

void PS_ASD::GarnerSDCSegmentInfo () {
    for (PS_SDC* sdc = m_pUpdateSDC; sdc; sdc = PS_SDC_Successor (sdc)) {
	PS_SMD_ExpectedTID (SMD (PS_SDC_Segment (sdc))) = PS_SDC_TID (sdc);
	PS_SMD_ExpectedRID (SMD (PS_SDC_Segment (sdc))) = PS_SDC_RID (sdc);
    }
}


/**********************************************
 *****  Segment Scan Management Routines  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to retain a container.
 *
 *  Arguments:
 *	xContainer		- the index of the container being retained.
 *
 *****/
void PS_ASD::RetainContainer (unsigned int xContainer) const {
/*****
 *  Access the container table entry for the specified container, ...
 *****/
    PS_CTE* ctep = &PS_ASD_CTEntry (this, xContainer);

/*****  ... access the SMD for the referenced segment, ...  *****/
    PS_SMD *smd = SMD (PS_CTE_Segment (*ctep));

/*****  ... and update the scan generation for the segment:  *****/
    PS_SMD_ScanGeneration (smd) = m_pAND->ScanGeneration ();
}

/*---------------------------------------------------------------------------
 *****  Routine to query the retention status of a container.
 *
 *  Arguments:
 *	xContainer		- the index of the container being queried.
 *
 *  Returns:
 *	true if the container has been retained in this segment scan, false
 *	otherwise.
 *
 *****/
bool PS_ASD::ContainerHasBeenRetained (unsigned int xContainer) const {
/*****
 *  Access the container table entry for the specified container, ...
 *****/
    PS_CTE* ctep = &PS_ASD_CTEntry (this, xContainer);

/*****  ... access the SMD for the referenced segment, ...  *****/
    PS_SMD *smd = SMD (PS_CTE_Segment (*ctep));

/*****  ... and return the retention status of the container:  *****/
    return PS_SMD_ScanGeneration (smd) == m_pAND->ScanGeneration ();
}


/*---------------------------------------------------------------------------
 *****  Routine to unmap a segment.
 *
 *  Arguments:
 *	smd			- the address of the SMD to unmap.
 *
 *****/
PrivateFnDef void ReclaimSMDMapping (PS_SMD *smd) {
    PS_SMD_FileMapping (smd).Destroy ();
}


/*---------------------------------------------------------------------------
 *****  Routine to perform a segment reclamation.
 *
 *  Arguments:
 *	smv			- the address of the first entry in a segment
 *				  mapping vector.
 *	maxSegment		- the index of the newest segment in the SMV.
 *	minSegment		- the index of the oldest segment in the SMV.
 *	scanGeneration		- the index of the current scan generation.
 *
 *  Returns:
 *	The number of reclaimed segments.
 *
 *****/
PrivateFnDef unsigned int ReclaimSMVSegments (
    PS_SMD*			smv,
    unsigned int		maxSegment,
    unsigned int		minSegment,
    unsigned int		scanGeneration
)
{
    unsigned int reclamationCount = 0;
    unsigned int segment;
    for (segment = 0; segment < maxSegment - minSegment + 1; segment++) {
	PS_SMD *smd = &smv[segment];
	if (PS_SMD_SegmentIsMapped (smd) && PS_SMD_ScanGeneration (smd) < scanGeneration) {
	    ReclaimSMDMapping (smd);
	    reclamationCount++;
	}
    }

    return reclamationCount;
}


/*---------------------------------------------------------------------------
 *****  Routine to record the statistics from a reclamation.
 *
 *  Arguments:
 *	reclamationCount	- the number of segments reclaimed.
 *
 *****/
PrivateFnDef void RecordReclamationStatistics (
    unsigned int		reclamationCount
)
{
    PS_SegmentScanCount++;
    PS_SegmentReclamationCount  += reclamationCount;
    PS_SegmentReclamationMinimum = V_Min (
	PS_SegmentReclamationMinimum, reclamationCount
    );
    PS_SegmentReclamationMaximum = V_Max (
	PS_SegmentReclamationMaximum, reclamationCount
    );

    if (PS_TracingSegmentScans) IO_printf (
   "+++ Segment Scan %u: Reclaimed: %u, Total: %u, Minimum: %u, Maximum: %u\n",
	PS_SegmentScanCount,
	reclamationCount,
	PS_SegmentReclamationCount,
	PS_SegmentReclamationMinimum,
	PS_SegmentReclamationMaximum
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to reclaim mapped segments from a network.
 *
 *  Arguments:
 *
 *  Returns:
 *	The number of reclaimed segments.
 *
 *****/
unsigned int PS_AND::ReclaimSegments () {
    unsigned int reclamationCount = 0;

    m_xScanGeneration++;

    for (PS_ASD *asd = m_pASDList; asd; asd = asd->NextASD ()) {
	M_ASD* pLASD = asd->LogicalASD ();
	if (pLASD) {
	    pLASD->ScanSegments ();
#if defined(__hp9000s800) || defined(__hp9000s700)

// Since the hp architecture imposes severe constraints on the use of shared memory mapped 
// files, we take measures to free up segments kept only for access to a space's container
// table. 

	    if (PS_ASD_CTIsMapped (asd) &&
		!asd->ContainerHasBeenRetained (M_KnownCTI_ContainerTable)
	    )
	    {
    /*****
     *  Container table allocation and copy is explicit (instead of via
     *  TS_CopyContainer) to allow the container table to be retained if
     *  space for it cannot be allocated.
     *****/
		M_CPreamble* oldCTPreamble = (M_CPreamble*)(PS_ASD_CT (asd)) - 1;
		unsigned int ctsize = sizeof (M_CPreamble)
				      + sizeof (M_CEndMarker)
				      + M_CPreamble_Size (oldCTPreamble);
		M_CPreamble* newCTPreamble = (M_CPreamble*)malloc ((unsigned int)ctsize);
		if (IsNil (newCTPreamble))
		    asd->RetainContainer (M_KnownCTI_ContainerTable);
		else {
		    memcpy (newCTPreamble, oldCTPreamble, (unsigned int)ctsize);
		    PS_ASD_CT (asd) = M_CPreamble_ContainerBaseAsType (
			newCTPreamble, PS_CT
		    );
		    PS_ASD_CTIsMapped (asd) = false;
		    PS_PCTPrivatizations++;
		    pLASD->AdjustAllocation (ctsize );
		}
	    }
#else
	    asd->RetainContainer (M_KnownCTI_ContainerTable);
#endif

	    reclamationCount += ReclaimSMVSegments (
		PS_ASD_SMV (asd),
		PS_ASD_RootSegment (asd),
		asd->baseSegment (),
		m_xScanGeneration
	    );
	}
    }

    RecordReclamationStatistics (reclamationCount);

    return reclamationCount;
}


unsigned int PS_AND::ReclaimAllSegments () {
    m_xScanGeneration++;

    unsigned int reclamationCount = 0;

    for (PS_ASD *asd = m_pASDList; asd; asd = asd->NextASD ()) {
	reclamationCount += ReclaimSMVSegments (
	    PS_ASD_SMV (asd),
	    PS_ASD_RootSegment (asd),
	    asd->baseSegment (),
	    m_xScanGeneration
	);
    }

    RecordReclamationStatistics (reclamationCount);

    return reclamationCount;
}


/**************************************
 *****  Segment Mapping Routines  *****
 **************************************/

/*--------------------------------------------------------------------------
 *  Internal routine to map a segment file -- used by ValidateChecksum ()
 *  and MapSegment()
 *
 *  Arguments:
 *	segmentPath	-- the address of a buffer containing the pathname
 *			   of the segment file
 *
 *  Returns:  The address of the segment header of the mapped segment.
 *	      (Nil if not successful)
 *****/
PS_SH *PS_ASD::MapSegment (char const *segmentPath, PS_SMD *smd) const {
/*****  Attempt to map the segment, ...  *****/
/*****
 *  The retry mechanism is probably an anachronism attributable to
 *  the Apollo platform's occassional failures due to transient network 
 *  resource problems. It is left, "just in case ..." (jck 7/96)
 *  We have some evidence that isilon has taken us back to the past.
 *  increasing the default retry delay to 10 seconds ... (jck, 9/2011)
 *****/
    PS_SMD_FileMapping (smd).Initialize ();

    for (
	unsigned int retry = 0;

	!PS_SMD_FileMapping (smd).Map (segmentPath, true, segmentMemoryShare ())
	    && ENOENT == errno
	    && retry < SegmentOpenRetries;

	retry++
    ) sleep (SegmentOpenRetryDelay);

    if (IsntNil (PS_SMD_SegmentAddress (smd)) && 0 != PS_TID_High (PS_SMD_ExpectedTID (smd))) {
/***** If we know what we're looking for, make sure it's what we've got *****/
	if (PS_SMD_ExpectedTID (smd) != PS_SH_TID (PS_SMD_SegmentAddress (smd))
	||  PS_SMD_ExpectedRID (smd) != PS_SH_RID (PS_SMD_SegmentAddress (smd))
	) {
	    char const *message = UTIL_FormatMessage (
		"Map Segment; ID Mismatch [%s]\nExpected: [%x.%x.%x.%x.%x]\nActual:   [%x.%x.%x.%x.%x]",
		segmentPath,
		PS_TID_High	(PS_SMD_ExpectedTID (smd)),
		PS_TID_Low	(PS_SMD_ExpectedTID (smd)),
		PS_TID_Sequence	(PS_SMD_ExpectedTID (smd)),
		PS_RID_Type 	(PS_SMD_ExpectedRID (smd)),
		PS_RID_Sequence	(PS_SMD_ExpectedRID (smd)),
		PS_TID_High	(PS_SH_TID (PS_SMD_SegmentAddress (smd))),
		PS_TID_Low	(PS_SH_TID (PS_SMD_SegmentAddress (smd))),
		PS_TID_Sequence	(PS_SH_TID (PS_SMD_SegmentAddress (smd))),
		PS_RID_Type 	(PS_SH_RID (PS_SMD_SegmentAddress (smd))),
		PS_RID_Sequence	(PS_SH_RID (PS_SMD_SegmentAddress (smd)))
	      );

	    ReclaimSMDMapping (smd);
	    ERR_SignalFault (EC__InternalInconsistency, message);
	}
    }

/***** The address will be Nil if mapping was not successful *****/
    return PS_SMD_SegmentAddress (smd);
}


/*---------------------------------------------------------------------------
 *****  Routine to map a segment into memory.
 *
 *  Arguments:
 *	this			- the address of the ASD containing the
 *				  segment to be mapped.
 *	segmentIndex		- the index of the segment to be mapped.
 *
 *  Returns:
 *	The address of the segment header of the mapped segment.
 *
 *****/
PS_SH *PS_ASD::MapSegment (unsigned int segmentIndex) {
/*****  Attempt to remain below the segment limit, ...  *****/
    if (VkMemory::MappedFileCount () >= MappedSegmentLimit)
	M_ReclaimSegments ();

/*****  Form the segment path name ...  *****/
    char const *segmentPathName = SegmentPathName (segmentIndex);

/*****  ... map the segment ...  *****/
    PS_SH *segmentAddress = MapSegment (segmentPathName, SMD (segmentIndex));

/*****  ... attempt to remain above the address threshold ... ****/
    if (IsntNil (segmentAddress) && MappedAddressThreshold > (void const*)segmentAddress) {
	ReclaimSMDMapping (SMD (segmentIndex));
	M_ReclaimSegments ();
	segmentAddress = MapSegment (
	    segmentPathName, SMD (segmentIndex)
	);
    }
/*****  ... abort if the segment could not be mapped ...  *****/
    if (IsNil (segmentAddress)) {
	double allocTotal = 0, mappingTotal = 0;
	M_AccumulateAllocationStatistics (&allocTotal, &mappingTotal);

	SignalUnixError (
	    UTIL_FormatMessage (
		"Map Segment [%s]\nAT: %.0f, MT: %.0f, FL: %u, Details",
		segmentPathName,
		allocTotal,
		mappingTotal,
		UTIL_FreeListTotal
	    )
	);
    }
/*****  ... otherwise, return.  *****/
    SuccessfulSegmentMaps++;
    return segmentAddress;
}


/*************************************
 *************************************
 *****  Error Logging Interface  *****
 *************************************
 *************************************/

/*---------------------------------------------------------------------------
 *****  Routine to make a 'vprintf' style error log entry.
 *
 *  Arguments:
 *	format			- a 'printf' style format string.
 *	rArgList		- a 'varargs' argument pointer supplying the
 *				  parameters specified by 'format'.
 *
 *  Returns:
 *	The number of characters generated by 'format' and 'rArgList'.
 *
 *****/
void PS_AND::VLogError (char const *format, VArgList const &rArgList) {
    V::VTime iNow;
    char iTimeStringBuffer[26];
    char const *timestring = iNow.ctime (iTimeStringBuffer);

    PS_UID userid = CurrentUser ();
    char iBuffer[256];
    char const *username = Vk_username (iBuffer, sizeof (iBuffer));

    FILE *file = fopen (NLFPathName (), "a");
    if (IsNil (file) && (ENFILE == errno || EMFILE == errno || EIO == errno)) {
	M_ReclaimSegments ();
	file = fopen (NLFPathName (), "a");
    }

    unsigned int retries = 10;
    while (IsNil (file) && retries-- > 0) {
	sleep (1);
	file = fopen (NLFPathName (), "a");
    }
    
    if (file) {
	UNWIND_Try {
	    IO_fprintf (
		file,
		"----------------------------------------------------------------\n\
***** %08X.%08X.%08X %*.*s %s [%d %d %d %d]\n",
		PS_TID_High		(TransactionId),
		PS_TID_Low		(TransactionId),
		PS_TID_Sequence	(TransactionId),
		strlen (timestring) - 1,
		strlen (timestring) - 1,
		timestring,
		IsntNil (username) ? username : "User?",
		PS_UID_RealUID	(userid),
		PS_UID_EffectiveUID	(userid),
		PS_UID_RealGID	(userid),
		PS_UID_EffectiveGID	(userid)
	    );
	    VArgList iArgList (rArgList);
	    IO_vfprintf (file, format, iArgList.list ());

	    fflush (file);
	} UNWIND_Finally {
	    fclose (file);
	} UNWIND_EndTryFinally;
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to make a 'printf' style error log entry.
 *
 *  Arguments:
 *	- like 'printf'.
 *
 *  Returns:
 *	Nothing
 *
 *****/
void __cdecl PS_AND::LogError (char const *format, ...) {
    V_VARGLIST (iArgList, format);
    VLogError (format, iArgList);
}


/*****************************
 *****************************
 *****  Access Routines  *****
 *****************************
 *****************************/

/**************************************************
 *****  Network Version Specification Syntax  *****
 **************************************************/

/*---------------------------------------------------------------------------
 *  Version Specification Syntax:
 *
 *	<Spec>		:= <Restart><Version><Offset>
 *	<Restart>	:= R |
 *	<Version>	:= <ResourceId> | <TransactionId>
 *	<ResourceId>	:= S<SpaceId><SegmentId>
 *	<SpaceId>	:= <NonNegativeInteger>
 *	<SegmentId>	:= /<NonNegativeInteger> |
 *	<TransactionId>	:= <NonNegativeInteger>  |
 *	<Offset>	:= -<NonNegativeInteger> |
 *
 *  Version Specification Interpretation:
 *
 *	A Version Specification names a data base version.  A version
 *	specification consists of two parts - a required specification
 *	for an absolute version and an optional offset from that version
 *	to a previous version.  An absolute version is named either by a
 *	data base segment created in the version or the absolute version
 *	number assigned to that version in the NDF.
 *
 *	The following are sample absolute version specifications:
 *
 *				... the empty version specification names
 *				    the newest available version in the
 *				    data base.  This is the version opened
 *				    by default.
 *		78234		... a version specification consisting of
 *				    just a non-negative integer names the
 *				    version whose NDF assigned id equals
 *				    this number.  These version numbers are
 *				    reported by 'ndftool' and 'viewndf'.
 *		S32/179		... a version specification prefixed by 'S'
 *				    names a version by data base segment.
 *				    In this example, the version named is the
 *				    version which created segment 179 in object
 *				    space 32.
 *		S32		... a version specification which names a
 *				    version by segment but includes only the
 *				    name of the space names the version that
 *				    created the newest segment in the space.
 *				    In this example, the version named is the
 *				    version that created the newest segment
 *				    in object space 32.
 *
 *	All version specifications can be followed by an optional offset.
 *	The offset specifies a number of versions before the specified
 *	absolute version.  For the preceeding examples:
 *
 *		-5		... the version 5 versions before the newest
 *				    network version.
 *		78234-5		... the version 5 versions before the version
 *				    whose NDF id is '78234'.
 *		S32/179-5	... the version 5 versions before the version
 *				    that created segment 32/179.  The five
 *				    versions are counted relative to all
 *				    updates and are not restricted to just
 *				    object space 32 updates.
 *		S32-5		... the version 5 versions before the version
 *				    that created the newest segment in object
 *				    space 32.  As in the preceeding example,
 *				    the five versions are counted relative to
 *				    all updates and are not restricted to just
 *				    object space 32 updates.
 *
 *	All version specifications can be preceeded by the letter 'R' to
 *	denote session restart (e.g., R, R78234, RS32, RS32/179, RS32/179-5,
 *	etc.).	The session restart flag modifies the use of the version by
 *	the current process.  Normally, the current process treats the version
 *	it is opening as the serialized (merged) result of multiple updates.
 *	All updates merged to produce the version become visible to the
 *	current process.  The session restart flag requests that the current
 *	process ignore the serialized updates and restrict itself to see only
 *	the updates visible to the creator of the version.  In effect, the
 *	current process becomes a continuation of the creator of the version,
 *	seeing exactly the same data base state seen by the creator immediately
 *	after the version was written.
 *---------------------------------------------------------------------------
 */


/************************************************
 *****  Network Version Location Utilities  *****
 ************************************************/

PrivateFnDef int VersionDidNotCreateSegment (
    int				ndfd,
    PS_Type_FO			versionId,
    PS_NVD			*versionNVD,
    int				xSpace,
    int				xSegment
)
{
    PS_SVD			svd;
    PS_SRD			srd;

    if (PS_TracingNetworkAccess) IO_printf (
	"...... trying version %d\n", versionId
    );
    if (xSpace >= NDFReadCountedArrayElementCount (ndfd, PS_NVD_SRV (*versionNVD)))
	PS_SRD_Role (srd) = PS_Role_Nonextant;
    else ReadSRD (
	ndfd,
	PS_NVD_SRV (*versionNVD),
	xSpace,
	&srd,
	PS_NVD_SoftwareVersion (*versionNVD)
    );

    if (PS_Role_Nonextant == PS_SRD_Role (srd)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Space %d Does Not Exist As Of Version %d", xSpace, versionId
	)
    );

/*****
 *****  If this version didn't update the desired space and more versions
 *****  exist, continue looking at versions...
 *****/
    if (PS_SRD_Role (srd) != PS_Role_Modified &&
	PS_FO_IsntNil (PS_NVD_PreviousVersion (*versionNVD))
    ) return true;

/*****  Found the most recent update of space...  *****/
    if (xSegment < 0) return false;

/*****  Otherwise, read the SVD ...  *****/
    ReadSVD (ndfd, PS_SRD_SVD (srd), &svd);
/*****  ...and validate the segment.  *****/
    if (xSegment > PS_SVD_RootSegment (svd)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Segment %d/%d Does Not Exist As Of Version %d",
	    xSpace, xSegment, versionId
	)
    );

/*****
 *****  If no previous SVD exists or the segment is greater than the root
 *****  segment of the previous SVD, we have found our version...
 *****/
    if (PS_FO_IsNil (PS_SVD_PreviousSVD (svd)))
	return false;
    ReadSVD (ndfd, PS_SVD_PreviousSVD (svd), &svd);
    if (xSegment > PS_SVD_RootSegment (svd))
	return false;
    return true;
}


PrivateFnDef void AdjustNVDToMatchSegmentSpecification (
    int				ndfd,
    char const*			versionSpecification,
    PS_Type_FO			*versionId,
    PS_NVD			*versionNVD,
    int				*desiredOffset
)
{
    int				desiredSpace, desiredSegment;

    desiredSpace   = *desiredOffset = 0;
    desiredSegment = -1;
    if (STD_sscanf (
	    versionSpecification,
	    "%d / %d - %d",
	    &desiredSpace, &desiredSegment, desiredOffset
	) == 1
    ) STD_sscanf (versionSpecification, "%d - %d", &desiredSpace, desiredOffset);

    if (0 == desiredSpace) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Invalid Segment Specification '%s'", versionSpecification
	)
    );

    if (PS_TracingNetworkAccess) IO_printf (
	"... looking for creator of segment %d/%d:\n", desiredSpace, desiredSegment
    );
    while (
	VersionDidNotCreateSegment (
	    ndfd, *versionId, versionNVD, desiredSpace, desiredSegment
	)
    )
    {
	*versionId = PS_NVD_PreviousVersion (*versionNVD);
	ReadNVD (ndfd, *versionId, versionNVD);
    }
}


PrivateFnDef void AdjustNVDToMatchBaseVersionSpecification (
    int				ndfd,
    char const*			versionSpecification,
    PS_Type_FO			*versionId,
    PS_NVD			*versionNVD,
    int				*desiredOffset
)
{
    *desiredOffset = 0;

    STD_sscanf (versionSpecification, " - %d", desiredOffset);
    if (*desiredOffset < 0) *desiredOffset = 0;

    if (PS_TracingNetworkAccess) IO_printf (
	"... looking for base version:\n"
    );
    while (
	!PS_NVD_BaseVersion (*versionNVD) &&
	PS_FO_IsntNil (PS_NVD_PreviousVersion (*versionNVD))
    )
    {
	*versionId = PS_NVD_PreviousVersion (*versionNVD);
	if (PS_TracingNetworkAccess) IO_printf (
	    "...... trying version %d\n", *versionId
	);
	ReadNVD (ndfd, *versionId, versionNVD);
    }
}


PrivateFnDef void AdjustNVDToMatchVersionSpecification (
    int				ndfd,
    char const*			versionSpecification,
    PS_Type_FO			*versionId,
    PS_NVD			*versionNVD,
    int				*desiredOffset
)
{
    int desiredVersion = *desiredOffset = 0;

    STD_sscanf (versionSpecification, "%d - %d", &desiredVersion, desiredOffset);
    if (desiredVersion < 0) {
	*desiredOffset -= desiredVersion;
	desiredVersion  = 0;
    }
    if (*desiredOffset < 0)
	*desiredOffset = 0;

    if (desiredVersion == 0)
	desiredVersion = *versionId;

    if (PS_TracingNetworkAccess) IO_printf (
	"... looking for version %d:\n", desiredVersion
    );
    while ((PS_Type_FO)desiredVersion != *versionId) {
	*versionId = PS_NVD_PreviousVersion (*versionNVD);
	if (PS_FO_IsNil (*versionId)) ERR_SignalFault (
	    EC__UsageError, UTIL_FormatMessage (
		"Network Version %d Not Found", desiredVersion
	    )
	);
	if (PS_TracingNetworkAccess) IO_printf (
	    "...... trying version %d\n", *versionId
	);
	ReadNVD (ndfd, *versionId, versionNVD);
    }
}


/*---------------------------------------------------------------------------
 *****  Internal routine to locate an NDF version given a specification for the
 *****  version.
 *
 *  Arguments:
 *	ndfd			- an open NDF descriptor.
 *	versionSpecification	- an optional character string (Nil if absent)
 *				  describing the network version to open.  The
 *				  syntax and interpretation of this string are
 *				  described above.
 *	initialVersionId	- the id of the current network version.
 *	versionId		- the address at which the version id of the
 *				  accessed version will be stored.
 *	versionNVD		- the address at which the accessed NVD will be
 *				  stored.
 *	restart			- the address at which the session restart flag
 *				  will be stored.
 *
 *  Returns:
 *	NOTHING.
 *
 *****/
PrivateFnDef void ReadNVDMatchingSpecification (
    int				ndfd,
    char const*			versionSpecification,
    PS_Type_FO			initialVersionId,
    PS_Type_FO			*versionId,
    PS_NVD			*versionNVD,
    bool			*restart
)
{
    int				desiredOffset, currentOffset;

/*****  Initialize the return arguments, ...  *****/
    if (PS_TracingNetworkAccess) IO_printf (
	"+++  Beginning network access at version %d...\n", initialVersionId
    );
    *restart	= false;
    *versionId	= initialVersionId;
    ReadNVD (ndfd, *versionId, versionNVD);

/*****  ... and return if no specification was provided:  *****/
    if (IsNil (versionSpecification)) return;

/*****  Otherwise, parse and process the version specification, ...  *****/
    if ('R' == *versionSpecification || 'r' == *versionSpecification) {
	*restart = true;
	versionSpecification++;
    }

    if ('S' == *versionSpecification || 's' == *versionSpecification
    ) AdjustNVDToMatchSegmentSpecification (
	ndfd, versionSpecification + 1, versionId, versionNVD, &desiredOffset
    );
    else if ('B' == *versionSpecification || 'b' == *versionSpecification
    ) AdjustNVDToMatchBaseVersionSpecification (
	ndfd, versionSpecification + 1, versionId, versionNVD, &desiredOffset
    );
    else AdjustNVDToMatchVersionSpecification (
	ndfd, versionSpecification, versionId, versionNVD, &desiredOffset
    );

    for (currentOffset = 0; currentOffset < desiredOffset; currentOffset++) {
	*versionId = PS_NVD_PreviousVersion (*versionNVD);
	if (PS_FO_IsNil (*versionId)) ERR_SignalFault (
	    EC__UsageError, UTIL_FormatMessage (
		"Network Version At Offset %d Not Found", desiredOffset
	    )
	);
	if (PS_TracingNetworkAccess) IO_printf (
	    "... backing up to version %d\n", *versionId
	);
	ReadNVD (ndfd, *versionId, versionNVD);
    }
}


/*---------------------------------------------------------------------------
 *****  Internal routine to update an SRV to make it appropriate for restart.
 *
 *  Arguments:
 *	ndfd			- an open NDF descriptor.
 *	nvdId			- the id of the version being restarted.
 *	nvd			- the NVD of the version being restarted.
 *	srv			- the SRV being adjusted.  This SRV is assumed
 *				  to contain the SRV of the version being
 *				  restarted.
 *
 *  Returns:
 *	NOTHING
 *
 ****************************************************************************/
PrivateFnDef void AdjustSRVForRestart (
    int				ndfd,
    PS_Type_FO			nvdId,
    PS_NVD			nvd,
    int				spaceCount,
    PS_SRD			*srv
)
{
    PS_SRD			isrv[M_POP_MaxObjectSpace + 1];
    int				ispaceCount, spaceIndex;

    PS_Type_FO const accessedVersion = PS_NVD_AccessedVersion (nvd);
    while (accessedVersion != nvdId && PS_FO_IsntNil (PS_NVD_UpdateThread (nvd))) {
	nvdId = PS_NVD_UpdateThread (nvd);
	ReadNVD (ndfd, nvdId, &nvd);
	ispaceCount = ReadSRV (
	    ndfd,
	    PS_NVD_SRV (nvd),
	    isrv,
	    M_POP_MaxObjectSpace + 1,
	    PS_NVD_SoftwareVersion (nvd)
	);
	for (spaceIndex = 0; spaceIndex < ispaceCount; spaceIndex++)
	    if (PS_Role_Modified != PS_SRD_Role (srv[spaceIndex]))
		srv[spaceIndex] = isrv[spaceIndex];
	while (spaceIndex < spaceCount)
	    if (PS_Role_Modified != PS_SRD_Role (srv[spaceIndex++]))
		PS_SRD_Role (srv[spaceIndex - 1]) = PS_Role_Nonextant;
    }
}


PrivateFnDef PS_SDC*	CreateSDCList	(int ndfd, PS_SVD const& svd);
PrivateFnDef PS_SDC*	FreeSDCList	(PS_SDC* sdcList);
PrivateFnDef PS_Type_FO CreateSDList	(PS_SDC* sdcList, NDFWriter *pNDF, PS_Type_FO sdfo);


void PS_AND::CreateNewNDF (PS_NDH ndh) {
/*****  create the new NDF  *****/
    char newNdfPathName [PathNameBufferSize];
    STD_sprintf (newNdfPathName, "%s.new", NDFPathName ());
    NDFWriter iNDFWriter (newNdfPathName);

/*****  Write a temporary version of the ndh ...  *****/
    WriteNDH (&iNDFWriter, &ndh);
    WriteString (&iNDFWriter, OSDPathName ());

/*****  ... Loop through the spaces  ...  *****/
    PS_SRD srv[M_POP_MaxObjectSpace + 1];
    unsigned int srdCount = m_iSpaceCount;
    PS_SRD_SVD  (srv[0]) = PS_FO_Nil;
    PS_SRD_Role (srv[0]) = PS_Role_Nonextant;
    for (unsigned int srdIndex = 1; srdIndex < srdCount; srdIndex++) {
	PS_SVD svd = SVD (srdIndex);

	PS_SDC* sdcList = CreateSDCList (DirectoryFD (), svd);

	/*****  Amend the svd  *****/
	PS_SVD_PreviousSVD	(svd) = PS_FO_Nil;
	PS_SVD_SD		(svd) = CreateSDList (sdcList, &iNDFWriter, PS_FO_Nil);
	FreeSDCList (sdcList);

	PS_SVD_MP		(svd) =
	PS_SVD_Extensions	(svd) = PS_FO_Nil;

	PS_SRD_SVD  (srv[srdIndex]) = WriteSVD (&iNDFWriter, &svd);
	PS_SRD_Role (srv[srdIndex]) = PS_AND_Role (this, srdIndex);

    }

/*****  ... amend the new network version state, ...  *****/
    PS_NVD nvd = m_iUpdateNVD;

    PS_NVD_SRV			(nvd) = WriteSRV (&iNDFWriter, srv, srdCount);
    PS_NVD_PreviousVersion	(nvd) = 
    PS_NVD_UpdateThread		(nvd) = 
    PS_NVD_AccessedVersion	(nvd) = PS_FO_Nil;
    PS_NVD_Signature		(nvd) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		(nvd) = true;
    PS_NVD_UnusedFlags		(nvd) = 0;
    PS_NVD_Annotation		(nvd) = WriteString (
	&iNDFWriter, "Edited NDF: single version only"
    );

    PS_NVD_TD			(nvd) =
    PS_NVD_MP			(nvd) =
    PS_NVD_Extensions		(nvd) = PS_FO_Nil;

    PS_NDH_CurrentNetworkVersion	(ndh) = WriteNVD (&iNDFWriter, &nvd);
    PS_NDH_ModificationTimeStamp	(ndh) = CurrentTime ();
    PS_NDH_AlternateVersionChainHead	(ndh) =
    PS_NDH_AlternateVersionChainMark	(ndh) =
    PS_NDH_Generation			(ndh) = 0;

    iNDFWriter.ValidateChecksum ();
    WriteNDH (&iNDFWriter, &ndh);

    iNDFWriter.Synchronize (newNdfPathName);
    iNDFWriter.Close ();
}


/************************************
 *****  Network Access Routine  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to access an object network.
 *
 *  Arguments:
 *	pDatabaseNDF		- the address of the database NDF access object.
 *	versionSpecification	- an optional character string (Nil if absent)
 *				  specifying the network version to open.  The
 *				  syntax and interpretation of a version
 *				  specification is described above.
 *
 *  Returns:
 *	The address of an Accessed Network Descriptor for the network.
 *
 *****/
PS_AND* PS_AND::AccessNetwork (
    VDatabaseNDF	*pDatabaseNDF,
    char const		*versionSpecification,
    bool		makingNewNdf
)
{
/*****  Open the NDF, ...  *****/
    char const *ndfPathName = pDatabaseNDF->ndfPathName ();
#ifdef __VMS
    int ndfd = open (ndfPathName, O_RDONLY, 0, "shr=upd,put,get,nql");
#else
    int ndfd = open (ndfPathName, O_RDONLY, 0);
#endif
    TrapUnixError (
	ndfd, UTIL_FormatMessage ("Network Directory File Open [%s]", ndfPathName)
    );

/*****  ... validate that it is a network directory, ...  *****/
    PS_NDH ndh;
    ReadNDH (ndfd, &ndh);
    if (PS_NetworkDirectorySignature != PS_NDH_Signature (ndh)) ERR_SignalFault (
	EC__UsageError,
	UTIL_FormatMessage ("Not a Network Directory File: [%s]", ndfPathName)
    );

/*****  ... if we can't read this version of NDFs ...  *****/
    if (PS_NDFVersionNumber != PS_NDH_NDFVersion (ndh)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Network Directory File Version Error: [%s:%d]:%d",
	    ndfPathName, PS_NDH_NDFVersion (ndh), PS_NDFVersionNumber
	)
    );

/*****  ... read the Network Version Descriptor ...  *****/
    PS_Type_FO	nvdId;
    PS_NVD	nvd;
    bool	restart;
    ReadNVDMatchingSpecification (
	ndfd,
	versionSpecification,
	PS_NDH_CurrentNetworkVersion (ndh),
	&nvdId,
	&nvd,
	&restart
    );

/*****  ... if the network was saved with software newer than this ... *****/
    if (PS_SoftwareVersion < PS_NVD_SoftwareVersion (nvd)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Version %d Software Cannot Access The Version %d Network %s",
	    PS_SoftwareVersion, PS_NVD_SoftwareVersion (nvd), ndfPathName
	)
    );

/*****  ... read the OSDPathName from the NDF, ...  *****/
    char const*	osdPathNameSpec;
    char*	osdPathName;
    if (makingNewNdf && IsntNil (osdPathNameSpec = GOPT_GetValueOption ("OSDPathName"))) {
	unsigned int osdPathNameLength = strlen (osdPathNameSpec) + 1;
	osdPathName = (char *)UTIL_Malloc (
	    osdPathNameLength > 1024 ? osdPathNameLength : 1024
	);
	strcpy (osdPathName, osdPathNameSpec);
    }
    else {
	unsigned int osdPathNameLength = NDFReadCountedArrayElementCount (
	    ndfd, PS_NVD_NewSpacePathName (nvd)
	);
	osdPathName = (char *)UTIL_Malloc (
	    osdPathNameLength > 1024 ? osdPathNameLength : 1024
	);
	
	ReadString (
	    ndfd, PS_NVD_NewSpacePathName (nvd), osdPathName, osdPathNameLength
	);
    }

    return new PS_AND (
	pDatabaseNDF, osdPathName, makingNewNdf, ndfd, ndh, nvdId, nvd, restart
    );
}

/**************************
 *****  Construction  *****
 **************************/

PS_AND::PS_AND (
    VDatabaseNDF	*pDatabaseNDF,
    char		*osdPathName,
    bool		 makingNewNdf,
    int			 ndfd,
    PS_NDH		&ndh,
    PS_Type_FO		 nvdId,
    PS_NVD		&nvd,
    bool		 restart
)
: m_pDatabaseNDF	(pDatabaseNDF)
, m_iNetworkHandle	(pDatabaseNDF->ndfPathName (), osdPathName, -1)
, m_pASDList		(0)
, m_iGeneration		(PS_NDH_Generation (ndh))
, m_pUpdateAnnotation	(0)
, m_iSpaceCount		(NDFReadCountedArrayElementCount (ndfd, PS_NVD_SRV (nvd)))
, m_pSRDArray		((PS_SRD*)UTIL_Malloc (sizeof (PS_SRD) * m_iSpaceCount))
, m_pSVDArray		((PS_SVD*)UTIL_Malloc (sizeof (PS_SVD) * m_iSpaceCount))
, m_xScanGeneration	(0)
, m_xSegmentMemoryShare	(VkMemory::Share_Private)
{
/*****  ... initialize it, ...  *****/
    ReadSRV (
	ndfd,
	PS_NVD_SRV (nvd),
	m_pSRDArray,
	m_iSpaceCount,
	PS_NVD_SoftwareVersion (nvd)
    );
    if (restart)
	AdjustSRVForRestart (ndfd, nvdId, nvd, m_iSpaceCount, m_pSRDArray);

    for (unsigned int spaceIndex = 0; spaceIndex < m_iSpaceCount; spaceIndex++) {
	if (PS_AND_Role (this, spaceIndex) != PS_Role_Nonextant) {
	    if (PS_TracingNetworkAccess) IO_printf (
		"+++  Using space %d version %d.\n",
		spaceIndex,
		PS_AND_SVDFO (this, spaceIndex)
	    );
	    ReadSVD (ndfd, PS_AND_SVDFO (this, spaceIndex), &SVD (spaceIndex));
	    if (PS_TracingNetworkAccess) {
		PS_SD sd;
		for (
		    PS_Type_FO sdId = PS_SVD_SD (SVD (spaceIndex));
		    PS_FO_IsntNil (sdId);
		    sdId = PS_SD_Predecessor (sd)
		)
		{
		    ReadSD (ndfd, sdId, &sd);
		    if (PS_SD_Segment (sd) <
			(unsigned int)PS_SVD_MinSegment (SVD (spaceIndex))
		    ) break;
		    IO_printf (
			"+++  Segment Descriptor: %d/%d %d %08x.%08x.%08x %d %d\n",
			spaceIndex,
			PS_SD_Segment (sd),
			sdId,
			PS_TID_High	(PS_SD_TID (sd)),
			PS_TID_Low	(PS_SD_TID (sd)),
			PS_TID_Sequence (PS_SD_TID (sd)),
			PS_RID_Type	(PS_SD_RID (sd)),
			PS_RID_Sequence (PS_SD_RID (sd))
		    );
		}
	    }
	}
    }

    m_iAccessFO			= restart ? PS_NVD_AccessedVersion (nvd) : nvdId;
    m_iUpdateFO			= nvdId;
    m_iUpdateNVD		= nvd;
    m_bTracingCompaction	=
    m_bTracingUpdate		=
    m_bDoingCompaction		=
    m_bMakingBaseVersion	= false;
    m_bValidatingChecksums	= PS_ValidatingChecksums;

    m_iLicense.construct (getenv ("VisionLicenseFile"));

    if (makingNewNdf) {
	SetDirectoryFDTo (ndfd);
	CreateNewNDF (ndh);
	ResetDirectoryFD ();
    }

/*****  ... close the NDF, ...  *****/
    TrapUnixError (
	close (ndfd), UTIL_FormatMessage (
	    "Network Directory File Close [%s]", pDatabaseNDF->ndfPathName ().content ()
	)
    );

/*****  ... and, finally, look for segment mapping hints:  *****/
    VString iText (OSDPathName ());
    iText.append ("/.hints");

    VSimpleFile iFile;
    if (iFile.OpenForTextRead (iText) && iFile.GetContents (iText)) {
	char const * const pMappingHintTag = "MappingHint:";
	char const *pMappingHint = strstr (iText, pMappingHintTag);
	if (pMappingHint) {
	    pMappingHint += strlen (pMappingHintTag);
	    bool bNotDone = true; bool bS = false, bG = false, bP = true, bValue = true;
	    while (bNotDone) {
		switch (*pMappingHint++) {
		case ' ':
		case '\t':
		    break;
		case '+':
		    bValue = true;
		    break;
		case '-':
		    bValue = false;
		    break;
		case 's':
		case 'S':
		    bS = bValue;
		    break;
		case 'g':
		case 'G':
		    bG = bValue;
		    break;
		case 'p':
		case 'P':
		    bP = bValue;
		    break;
		default:
		    bNotDone = false;
		    break;
		}
	    }
	    VkMemory::Share xShare = VkMemory::Share_None;
	    if (bS)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_System);
	    if (bG)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_Group);
	    if (bP)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_Private);
	    if (xShare != VkMemory::Share_None)
		m_xSegmentMemoryShare = xShare;
	}
    }
}


/***********************************
 *****  Space Access Routines  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to access an object space.
 *
 *  Argument:
 *      rpResult                - a reference to a PS_ASD* that will be
 *                                set to point to the PS_ASD for the space
 *                                if it was successfully accessed.
 *	spaceIndex		- the index of the space to be accessed.
 *
 *  Returns:
 *	True if the access succeeded (rpResult can be used), false otherwise.
 *      Access will fail if the space index is 0.
 *
 *  Notes:
 *	This routine accesses the space version consistent with the network
 *	version attached by 'PS_AND::AccessNetwork'.  Versions of the specified
 *	space associated with newer versions of the network will NOT be
 *	accessed by this routine.
 *
 *****/
bool PS_AND::AccessSpace (PS_ASD *&rpResult, unsigned int spaceIndex) {
/*****  Abort if an attempt is made to map an out-of-bounds space...  *****/
    if (spaceIndex >= m_iSpaceCount) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
	        "%s:\n            Space Index '%d' Exceeds Space Count '%d'",
		NDFPathName (), spaceIndex, m_iSpaceCount - 1
	    )
	);
    }
/*****  Return if an attempt is made to map a non-persistent space...  *****/
    if (PS_AND_Role (this, spaceIndex) == PS_Role_Nonextant)
	return false;
    
/*****  Return the space if it already exists, ...  *****/
    for (PS_ASD *asd = m_pASDList; asd; asd = asd->NextASD ()) {
	if (spaceIndex == asd->spaceIndex ()) {
	    rpResult = asd;
            return true;
        }
    }

#if defined(sun) && !defined (_LP64)
    // For 32-bit Solaris, try to reduce fragmentation of the upper half of address space by strongly suggesting
    // that the segments holding ContainerTables (which never get unmapped) get stashed way on the upper end ...
    void const *savedThreshold = MappedAddressThreshold;
    MappedAddressThreshold = (void const *)0xe0000000;
#endif

/*****  Otherwise, allocate, ...  *****/
    m_pASDList = new PS_ASD (this, spaceIndex);

#if defined(sun) && !defined (_LP64)
    MappedAddressThreshold = savedThreshold;
#endif
    rpResult = m_pASDList;
    return true;
}


/******************************
 *****  ASD Construction  *****
 ******************************/

PS_ASD::PS_ASD (PS_AND *pAND, unsigned int xSpace, PS_CT* pCT)
: m_pAND		(pAND)
, m_pNextASD		(pAND->m_pASDList)
, m_xSpace		(xSpace)
, m_pLogicalASD		(0)
, m_xBaseSegment	(MinSegment ())
, m_bInCompaction	(true)
, m_bCompactingAll	(false)
, m_bCompactingSmall	(CompactSmallSegments)
, m_bMinSMDSet		(false)
, m_xMinSMD		(0)
, m_iMaxCompactionSegs	(MaxCompactionSegments)
, m_iMaxSegmentSize	(MaxSegmentSize)
, m_iLargeContainerSize	(LargeContainerSize)
, m_iCopyCoefficient	(1.0)
, m_iReclaimCoefficient	(1.0)
, m_pSMV		(AllocateSMV (RootSegment (), m_xBaseSegment))
, m_bCTIsMapped		(true)
, m_pCT			(pCT)
, m_pUpdateCT		(pCT)
, m_pUpdateSDC		(0)
, m_xUpdateFD		(-1)
{
    if (m_pCT) {
	char const *pSpacePathName = SpacePathName ();
	TrapUnixError (
	    mkdir (pSpacePathName, SpaceCreationPermission),
	    UTIL_FormatMessage ("Space Creation [%s]", pSpacePathName)
	);
    }
    else {
	GarnerNDFSegmentInfo ();
	PS_SH* rootSegment = MapSegment (RootSegment ());
	m_pCT = PS_SH_ContainerBaseAsType (rootSegment, CTOffset (), PS_CT);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to calculate the total mapped size of an object space.
 *
 *  Argument:
 *	this			- the address of an Accessed Space Descriptor
 *      segCount		- a pointer to an integer to be set to the
 *				  number of segments which are currently
 *				  mapped in this space.
 *  Returns:
 *	The total size of all mapped segments in the specified space.
 *
 *****/
unsigned long PS_ASD::MappedSizeOfSpace (unsigned int *segCount) const {
    *segCount = 0;

    unsigned long totalSize = 0;
    unsigned int totalSegments = TotalSegments ();

    for (unsigned int segmentIndex = 0; segmentIndex < totalSegments; segmentIndex++) {
	PS_SMD *smd = m_pSMV + segmentIndex;

	if (PS_SMD_SegmentIsMapped (smd)) {
	    totalSize += PS_SMD_SegmentSize (smd);
	    (*segCount)++;
	}
    }
	    
    return totalSize;
}

void PS_ASD::dumpCTSegment () {
    VString iDumpName ("ctDump_");
    iDumpName << m_xSpace << "_" << RootSegment () << "." << getpid ();
    unlink (iDumpName);
    int fd = open (iDumpName, O_CREAT | O_RDWR, 0644);

    if (-1 != fd) {
	write (fd, PS_SMD_SegmentAddress (SMD (RootSegment ())), PS_SMD_SegmentSize (SMD (RootSegment ())));
	close (fd);
    }
    else
	IO_printf ("%d:%s\n", errno, strerror (errno));

}
/**************************************
 *****  Container Access Routine  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to access a container.
 *
 *  Arguments:
 *	this			- the address of an ASD associated with an
 *				  accessed space.
 *	xContainer		- the index of the container to be accessed.
 *	hasBeenAccessed		- a boolean which, when true, denotes that
 *				  this container has been accessed at least
 *				  once before in this session.  Used to
 *				  avoid double counting containers in the
 *				  segment utilization statistics for a space.
 *
 *  Returns:
 *	The mapped address of the container specified by the arguments.
 *
 *****/
M_CPreamble* PS_ASD::AccessContainer (unsigned int xContainer, bool hasBeenAccessed) {
/*****
 *  Access the container table entry for the specified container, ...
 *****/
    PS_CTE cte = PS_ASD_CTEntry (this, xContainer);

/*****  ... access the SMD for the referenced segment, ...  *****/
    if (PS_CTE_Segment (cte) > RootSegment () || PS_CTE_Segment (cte) < MinSegment ()) {
	dumpCTSegment ();
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
	        "%s: AccessContainer [%u:%u], Segment %u Out of Range: '%u-%u'",
		Database ()->NDFPathName (), m_xSpace, xContainer, PS_CTE_Segment (cte), MinSegment (), RootSegment ()
	    )
	);
    }
    PS_SMD *smd = SMD (PS_CTE_Segment (cte));

/*****  ... map that segment if necessary, ...  *****/
    if (PS_SMD_SegmentIsntMapped (smd))
	MapSegment (PS_CTE_Segment (cte));

    // If the offset is out of bounds, the container table's segment is likely corrupted.
    // Dump what we have mapped, and signal an error.
    if (PS_CTE_Offset (cte) >= PS_SMD_SegmentSize(smd) - sizeof (M_CPreamble)- sizeof (M_CEndMarker)) {
	dumpCTSegment ();
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
	        "%s: AccessContainer [%u:%u], Segment %u:\n             Offset: '%u' Exceeds Segment Size: '%u'",
		Database ()->NDFPathName (), m_xSpace, xContainer, PS_CTE_Segment (cte), PS_CTE_Offset (cte), PS_SMD_SegmentSize(smd)
	    )
	);
    }


/*****  ... and return the address of the container.  *****/
    M_CPreamble* container = PS_SMD_ContainerAddress (smd, PS_CTE_Offset (cte));
    if (hasBeenAccessed)
	return container;

    PS_SMD_SegmentUtilization (smd) += M_CPreamble_Size (container)
	+ sizeof(M_CPreamble)
	+ sizeof(M_CEndMarker);

    return container;
}


/*---------------------------------------------------------------------------
 *****  Routine to release a container.
 *
 *  Arguments:
 *	this			- the address of an ASD associated with an
 *				  accessed space.
 *	xContainer		- the index of the container to be released.
 *	sContainer		- the size of the container (redundant but
 *				  available).  Excludes the preamble and
 *				  end marker.
 *
 *  Returns:
 *	NOTHING
 *
 ******/
void PS_ASD::ReleaseContainer (unsigned int xContainer, unsigned int sContainer) {
/*****
 *  Access the container table entry for the specified container, ...
 *****/
    PS_CTE* ctep = &PS_ASD_CTEntry (this, xContainer);

/*****  ... access the SMD for the referenced segment, ...  *****/
    PS_SMD *smd = SMD (PS_CTE_Segment (*ctep));

/*****  ... and decrement the segment utilization statistic.  *****/
    PS_SMD_SegmentUtilization (smd) -= sContainer
	+ sizeof(M_CPreamble)
	+ sizeof(M_CEndMarker);
}


/**************************
 **************************
 *****  Lock Manager  *****
 **************************
 **************************/

/*---------------------------------------------------------------------------
 *****************************  Locking Strategy  ***************************
 *
 * Two types of lockable resource exist - Networks and Spaces.  Possession of
 * a network lock is required to modify a Network Directory File, create a new
 * object space, or lock an exist object space.  Possession of an Space lock
 * is required to update that space.  Because there is only one Network lock,
 * processes contending for that lock will execute serially.  In order to
 * maximize update concurrency, the network lock is held while a process
 * determines if its update is consistent under the rules of read-write
 * serializability and, if it is, while the space locks are obtained.  A
 * Network lock is NOT held while spaces are being updated.  Consequently,
 * contention during space update is limited to processes attempting to update
 * the same space.
 *---------------------------------------------------------------------------
 */


/*************************************
 *****  Basic Locking Utilities  *****
 *************************************/

/*************************
 *  Symbolic Parameters  *
 *************************/

#define WaitForLock	    	VdbNetworkLockOperation_Lock
#define DontWaitForLock		VdbNetworkLockOperation_TLock

/*************************
 *  Symbolic Predicates  *
 *************************/

#define LockAcquired(acquisitionResult)	(acquisitionResult)


/********************************
 *  Low Level Lock Acquisition  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to perform a lock operation.
 *
 *  Arguments:
 *	xLock			- the lock to select.  A negative 'xLock'
 *				  selects all possible locks.
 *	xOperation		- the lock operation to be performed.
 *
 *  Returns:
 *	true if the operation completed successfully, false if a 'TLock' was
 *	requested which did not procur the requested lock.
 *
 *****/
bool PS_AND::LockOp (int xLock, VdbNetworkLockOperation xOperation) {
    VkStatus iStatus;

    m_iNetworkHandle.Lock (xLock, xOperation, &iStatus);

    switch (iStatus.Type()) {
    case VkStatusType_Completed:
	return true;
    case VkStatusType_Blocked:
	return false;
    }

    ERR_SignalFault (
	EC__SystemFault, UTIL_FormatMessage (
	    "Lock Select [%d:%d]: %s", xLock, xOperation, iStatus.CodeDescription ()
	)
    );

    return false;
}


/***********************************************
 *****  Network Lock Management Utilities  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to acquire a network lock.
 *
 *  Arguments:
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	When this routine returns during a healthy update, the caller is
 *	assured exclusive write access to the network directory and the
 *	exclusive opportunity to lock additional resources.
 *
 *****/
void PS_AND::LockNetwork () {
    if (UpdateIsntWell ())
	return;

/****  Lock the network, ...  *****/
    LockOp (0, WaitForLock);

/*****  ...re-open the NDF...  *****/
    int ndfd = DirectoryFD ();
#ifdef __VMS
    if (ndfd >= 0) {
	ResetDirectoryFD ();
	close (ndfd);
    }
    ndfd = open (NDFPathName (), O_RDWR, 0, "shr=get,nql");
    TrapUnixError (
	ndfd, UTIL_FormatMessage (
	    "Network Directory File Re-Open [%s]", NDFPathName ()
	)
    );
#endif

/*****  ...and confirm that it holds the update thread of this process:  *****/
    PS_NDH ndh;
    ReadNDH (ndfd, &ndh);

    if (m_bTracingUpdate) IO_printf (
	"+++  AcquireUpdateResources: Attempting to locate version %d ...\n",
	m_iUpdateFO
    );

    PS_Type_FO nvdfo; PS_NVD nvd;
    bool reopenFailed = true;
    if (m_iGeneration == PS_NDH_Generation (ndh)) {
	nvdfo = m_iUpdateFO;
	if (m_bTracingUpdate) IO_printf (
	    "... verifying version %d\n", nvdfo
	);
	ReadNVD (ndfd, nvdfo, &nvd);
	if (PS_NVD_TID			(nvd) == PS_NVD_TID		(m_iUpdateNVD)
	&&  PS_NVD_CommitTimeStamp	(nvd) == PS_NVD_CommitTimeStamp	(m_iUpdateNVD)
	) reopenFailed = false;
    }
    else {
	PS_Type_FO tnvdfo; PS_NVD tnvd;
	for (
	    tnvdfo = PS_NDH_CurrentNetworkVersion (ndh);
	    reopenFailed && PS_FO_IsntNil (tnvdfo);
	    tnvdfo = PS_NVD_PreviousVersion (nvd)
	) {
	    if (m_bTracingUpdate) IO_printf (
		"... trying version %d\n", tnvdfo
	    );
	    nvdfo = tnvdfo;
	    ReadNVD (ndfd, nvdfo, &nvd);
	    if (PS_NVD_TID		(nvd) == PS_NVD_TID		(m_iUpdateNVD)
	    &&  PS_NVD_CommitTimeStamp	(nvd) == PS_NVD_CommitTimeStamp	(m_iUpdateNVD)
	    ) reopenFailed = false;
	}

	if (nvdfo != m_iUpdateFO && !reopenFailed) {
	    for (
		tnvdfo = PS_NVD_MP (nvd), reopenFailed = true;
		PS_FO_IsntNil (tnvdfo) && reopenFailed;
		tnvdfo = PS_NVD_MP (tnvd)
	    ) if (tnvdfo != m_iUpdateFO)
		ReadNVD (ndfd, tnvdfo, &tnvd);
	    else {
		m_iAccessFO	= PS_NVD_AccessedVersion (nvd);
		m_iUpdateNVD	= nvd;
		m_iUpdateFO	= nvdfo;
		m_iGeneration	= PS_NDH_Generation (ndh);
		reopenFailed	= false;
	    }
	}
    }

    if (reopenFailed) {
	close (ndfd);
	m_xUpdateStatus = PS_UpdateStatus_Inconsistent;
	return;
    }

    if (m_bTracingUpdate) IO_printf (
	"+++  AcquireUpdateResources: Successfully located version as %d\n",
	nvdfo
    );

#ifdef __VMS
    SetDirectoryFDTo (ndfd);
#endif
}

void PS_AND::UnlockNetwork (bool reopen) {
#ifdef __VMS
    int ndfd = DirectoryFD ();
    if (ndfd >= 0) {
	ResetDirectoryFD ();
	close (ndfd);
    }
#endif

    LockOp (0, VdbNetworkLockOperation_Unlock);

#ifdef __VMS
    if (reopen) {
	ndfd = open (NDFPathName (), O_RDONLY, 0, "shr=upd,put,get,nql");
	TrapUnixError (
	    ndfd, UTIL_FormatMessage (
		"Network Directory File Re-Open [%s]", NDFPathName ()
	    )
        );
	SetDirectoryFDTo (ndfd);
    }
#endif
}


/*********************************************
 *****  Space Lock Management Utilities  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Utility to wait for the release of an object space lock.
 *
 *  Argument:
 *	this			- the ASD for the locked space to be awaited.
 *
 *  Returns:
 *	NOTHING - when the current lock on the space has been released.
 *
 *  Notes:
 *	This routine does not actually lock the requested space, it merely
 *	waits until any current locks have been released.
 *
 *****/
void PS_ASD::AwaitSpaceLockRelease () {
//  Wait for the lock to become available, ...
    AcquireSpaceLock (WaitForLock);

//  ... and immediately release it, ...
    m_pAND->LockOp (m_xSpace, VdbNetworkLockOperation_Unlock);

//  ... to avoid a deadlock while it is reacquired properly.
}

/*---------------------------------------------------------------------------
 *****  Physical space locking routine.
 *
 *  Argument:
 *	this			- the address of a physical ASD for the space
 *				  to be locked.
 *
 *  Returns:
 *	true if the space was successfully locked, false if it wasn't.
 *
 *****/
bool PS_ASD::LockSpace () {
    if (LockAcquired (AcquireSpaceLock (DontWaitForLock)))
	return true;

    m_pAND->UnlockSpaces ();
    m_pAND->m_pUngrantedASD = this;

    return false;
}


/*---------------------------------------------------------------------------
 *****  Logical space locking routine.
 *
 *  Argument:
 *	this			- the address of a Logical ASD for the space
 *				  to be locked.
 *
 *  Returns:
 *	true if the space was successfully locked, false if it wasn't.
 *
 *  Notes:
 *	This routine is intended for use as the action routine of an
 *	'M_Enumerate*Spaces' routine.
 *
 *****/
bool M_ASD::LockSpace (VArgList const&) {
    return IsNil (m_pPASD) || m_pPASD->LockSpace ();
}

/*---------------------------------------------------------------------------
 *****  Routine to lock the spaces required by an update.
 *
 *  Arguments:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	This routine attempts to lock all spaces which must be updated.  If it
 *	fails, it MAY choose to retain the locks it has acquired.  The general
 *	update algorithm will then wait for any ungranted locks.  In order to
 *	avoid DEADLOCK, if any version of this routine decides to retain locks,
 *	then ALL versions which decide to retain locks MUST lock their spaces
 *	in the same order.  Currently, this routine does NOT retain locks and
 *	does NOT lock spaces in any particular order.
 *
 *****/
void PS_AND::LockSpaces (M_AND *pLogicalAND) {
    if (UpdateIsntWell ())
	return;

    pLogicalAND->EnumerateModifiedSpaces (&M_ASD::LockSpace);
}

void PS_AND::UnlockSpaces () {
    LockOp (-1, VdbNetworkLockOperation_Unlock);
}


/**************************************
 *****  Lock Manager Initializer  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Utility to initialize the lock manager prior to an update.
 *
 *  Arguments:
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_AND::ResetLockManager () {
    m_pUngrantedASD = NilOf (PS_ASD*);
}


/*---------------------------------------------------------------------------
 *****  Routine to wait for ungranted space locks.
 *
 *  Arguments:
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	This routine is designed to be used OUTSIDE the protection of a
 *	network lock; consequently, the only guarantee that can be made
 *	is that the resource lock(s) blocking this process will have been
 *	released by its(their) original holder.
 *
 *****/
void PS_AND::WaitForUngrantedLocks () {
    if (UpdateIsntWell ()) {
	ResetLockManager ();
	return;
    }

    if (m_pUngrantedASD) {
	m_pUngrantedASD->AwaitSpaceLockRelease ();
	ResetLockManager ();
    }
}


/*******************************************
 *******************************************
 *****  Update Consistency Predicates  *****
 *******************************************
 *******************************************/

/***************************************************
 *****  Space Consistency Predicate Utilities  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to determine if the currently accessed version of a
 *****  space has been modified by another process.
 *
 *  Argument:
 *	this			- the address of an ASD for the space being
 *				  tested.
 *
 *  Returns:
 *	true if the version of the space accessed by this process has not
 *	been modified by another process since its access, false otherwise.
 *
 *****/
bool PS_ASD::AccessedSpaceVersionIsNewest () const {
    int ndfd = m_pAND->DirectoryFD ();

    PS_NDH ndh;
    ReadNDH (ndfd, &ndh);

    PS_NVD nvd;
    ReadNVD (ndfd, PS_NDH_CurrentNetworkVersion (ndh), &nvd);

    PS_SRD srd;
    ReadSRD (ndfd, PS_NVD_SRV (nvd), m_xSpace, &srd, PS_NVD_SoftwareVersion (nvd));

    return PS_ASD_SVDFO (this) == PS_SRD_SVD (srd);
}


/******************************************
 *****  Space Consistency Predicates  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to determine if the current state of a physical space
 *****  satisfies the read-write serializability condition for consistency.
 *
 *  Arguments:
 *	this			- the address of a Physical ASD for the space
 *				  to be tested.
 *
 *  Returns:
 *	true if this process' access to the space is serializable with the
 *	current network state, false if it is not.
 *
 *  Notes:
 *	If the serializability condition for the space isn't satisfied, this
 *	routine will set flag the update as inconsistent.
 *
 *****/
bool PS_ASD::ValidateSpaceConsistency () const {
    if (AccessedSpaceVersionIsNewest ())
	return true;

    m_pAND->SetUpdateStatusTo (PS_UpdateStatus_Inconsistent);

    return false;
}

/*---------------------------------------------------------------------------
 *****  Internal routine to determine if the current state of a logical space
 *****  satisfies the read-write serializability condition for consistency.
 *
 *  Arguments:
 *	logicalASD		- the address of a Logical ASD for the space
 *				  to be tested.
 *
 *  Returns:
 *	true if this process' access to the space is serializable with the
 *	current network state, false if it is not.
 *
 *  Notes:
 *	If the serializability condition for the space isn't satisfied, this
 *	routine will set flag the update as inconsistent.
 *
 *	This routine is intended for use as the action routine of an
 *	'M_Enumerate*Spaces' routine.
 *
 *****/
bool M_ASD::ValidateSpaceConsistency (VArgList const&) {
    return IsNil (m_pPASD) || m_pPASD->ValidateSpaceConsistency ();
}


/*******************************************
 *****  Network Consistency Predicate  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to determine if the update being attempted is consistent with
 *****  the currently committed version of the network.
 *
 *  Arguments:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	The current definition of consistency is based upon the notion of
 *	read-write serializability at the level of object spaces.  An update
 *	is considered consistent if no updates have been performed to any of
 *	object spaces it has read.
 *
 *****/
void PS_AND::ValidateNetworkConsistency (M_AND *pLogicalAND) {
    if (UpdateIsntWell ())
	return;

    pLogicalAND->EnumerateAccessedSpaces (&M_ASD::ValidateSpaceConsistency);
}


/**************************************************
 **************************************************
 *****  Update Resource Acquisition Routines  *****
 **************************************************
 **************************************************/

/*---------------------------------------------------------------------------
 *****  Internal procedure to acquire resources needed to perform an update.
 *
 *  Arguments:
 *
 *  Notes:
 *	This routine re-opens the NDF for writing and acquires a lock set.
 *
 *****/
void PS_AND::AcquireUpdateResources () {
#ifndef __VMS
    int ndfd = open (NDFPathName (), O_RDWR, 0);
    TrapUnixError (
	ndfd, UTIL_FormatMessage (
	    "Network Directory File Re-Open [%s]", NDFPathName ()
	)
    );
    SetDirectoryFDTo (ndfd);
#endif

    SetJournalFDTo (open (NJFPathName (), O_WRONLY | O_APPEND, 0));

    m_xNextRIDSequence = 0;
    m_iTID = TransactionId;

    PS_TID_Sequence (TransactionId)++;
}

/*---------------------------------------------------------------------------
 *****  Internal procedure to release resources acquired to perform an update.
 *
 *  Arguments:
 *	inCleanupTrap		- a boolean which, when true, indicates that
 *				  this routine is being invoked from a cleanup
 *				  trap.
 *
 *****/
void PS_AND::ReleaseUpdateResources (bool recovering) {
    int ndfd = DirectoryFD ();

    if (ndfd < 0)
	return;

    if (recovering) {
	UnlockNetwork	();
	UnlockSpaces	();
    }

    if (JournalFD () >= 0) {
	close (JournalFD ());
	ResetJournalFD ();
    }

    ResetDirectoryFD ();
    TrapUnixError (
	close (ndfd), UTIL_FormatMessage (
	    "Network Directory File Close [%s]", NDFPathName ()
	)
    );
}


/****************************
 ****************************
 *****  Journal Update  *****
 ****************************
 ****************************/

void PS_AND::WriteJournalEntry (char const *message) {
    if (JournalFD () < 0)
	return;

    if (-1 == write (JournalFD (), message, strlen (message))) SignalUnixError (
	UTIL_FormatMessage (
	    "Error Writing <%s> to Journal <%s>", message, NJFPathName ()
	)
    );

    SynchronizeNDF (JournalFD (), NJFPathName ());
}

void PS_AND::WriteMinimumSegmentJournalEntry (
    unsigned int space, unsigned int segment
)
{
//  Return if journaling is disabled, ...
    if (JournalFD () < 0)
	return;

//  ... otherwise write the entry:
    WriteJournalEntry (
	UTIL_FormatMessage (
	    "%08lx.%08lx.%08lx %-6.6s %4lu/%lu\n",
	    PS_TID_High	    (m_iTID),
	    PS_TID_Low	    (m_iTID),
	    PS_TID_Sequence (m_iTID),
	    "MinSeg",
	    space,
	    segment
	)
    );
}

void PS_AND::WriteSegmentJournalEntry (
    unsigned int space, unsigned int segment, PS_SH* sh
)
{
//  Return if journaling is disabled, ...
    if (JournalFD () < 0)
	return;

//  ... otherwise format and write the entry:
    char const *segmentType;
    switch (PS_RID_Type (PS_SH_RID (sh)))
    {
    case PS_ResourceType_UpdateSegment:
	segmentType = "U";
	break;
    case PS_ResourceType_IncorporatorSegment:
	segmentType = "I";
	break;
    default:
	segmentType = "?";
	break;
    }
    
    WriteJournalEntry (
	UTIL_FormatMessage (
	    "%08lx.%08lx.%08lx %-6.6s %4lu/%-5lu %s %lu\n",
	    PS_TID_High	    (PS_SH_TID (sh)),
	    PS_TID_Low	    (PS_SH_TID (sh)),
	    PS_TID_Sequence (PS_SH_TID (sh)),
	    "NewSeg",
	    space,
	    segment,
	    segmentType,
	    PS_RID_Sequence (PS_SH_RID (sh))
	)
    );
}

void PS_AND::WriteCommitJournalEntry (PS_Type_FO nvdfo) {
//  Return if journaling is disabled, ...
    if (JournalFD () < 0)
	return;

//  Otherwise, format the basic message, ...
    char const *pBasicCommitEntry = UTIL_FormatMessage (
	"%08lx.%08lx.%08lx %-6.6s %lu\n",
	PS_TID_High	(m_iTID),
	PS_TID_Low	(m_iTID),
	PS_TID_Sequence	(m_iTID),
	"Commit",
	nvdfo
    );

//  ... and write it if an annotation was not supplied:
    if (IsNil (m_pUpdateAnnotation))
	WriteJournalEntry (pBasicCommitEntry);
//  Otherwise, ...
    else
    {
    //  ... allocate a buffer for the annotated message, ...
	char *pAnnotatedCommitEntry = (char *)UTIL_Malloc (
	    strlen (pBasicCommitEntry) + strlen (m_pUpdateAnnotation) + 5
	);

    //  ... format the annotated message, ...
	sprintf (
	    pAnnotatedCommitEntry, "%s<%s>", pBasicCommitEntry, m_pUpdateAnnotation
	);

    //  ... convert newlines to spaces, ...
	char *pNextSubstring = pAnnotatedCommitEntry;
	while (*(pNextSubstring += strcspn (pNextSubstring, "\r\n")))
	    *pNextSubstring = ' ';

    //  ... append a terminating new line, ...
	strcpy (pNextSubstring, "\n");

    //  ... write the annotated commit message, ...
	WriteJournalEntry (pAnnotatedCommitEntry);

    //  ... and free the buffer containing it.
	UTIL_Free (pAnnotatedCommitEntry);
    }
}


/**********************************************
 **********************************************
 *****  Update Recovery Checkpoint Macro  *****
 **********************************************
 **********************************************/

#define BeginRecoverableUpdateSection \
    AcquireUpdateResources ();\
    if (UpdateIsWell ()) {\
	UNWIND_Try

#define EndRecoverableUpdateSection UNWIND_Finally {\
    ReleaseUpdateResources (UNWIND_IsRestarting || UpdateIsntWell ());\
} UNWIND_EndTryFinally; } else


/***********************************
 ***********************************
 *****  Space Update Routines  *****
 ***********************************
 ***********************************/

/*********************************
 *****  Checksum Validation  *****
 *********************************/

void PS_ASD::ValidateChecksum (char const *segmentPathName) const {
    if (TracingUpdate ()) IO_printf (
	"+++ ... validating segment checksum"
    );

    PS_SMD smd;
    PS_TID_High (PS_SMD_ExpectedTID (&smd)) = 0;

    PS_SH* segmentAddress = MapSegment (segmentPathName, &smd);
    if (IsNil (segmentAddress)) {
	double allocTotal = 0, mappingTotal = 0;
	M_AccumulateAllocationStatistics (&allocTotal, &mappingTotal);

	SignalUnixError (
	    UTIL_FormatMessage (
		"Map Segment [%s] (Checksum)\nAT: %.0f, MT: %.0f, FL: %u, Details",
		segmentPathName, allocTotal, mappingTotal, UTIL_FreeListTotal
	    )
	);
    }

    unsigned int oldChecksum = PS_SH_Checksum (segmentAddress);
    if (TracingUpdate ())
	IO_printf (" of %u ...\n", oldChecksum);
    unsigned int newChecksum = 0;
    AccumulateChecksum (
	&newChecksum, (pointer_t)(segmentAddress + 1), PS_SMD_SegmentSize (&smd) - sizeof (PS_SH)
    );
    ReclaimSMDMapping (&smd);
    if (oldChecksum != newChecksum) ERR_SignalFault (
	EC__SystemFault, UTIL_FormatMessage (
	    "Segment '%s' checksum %u not equal to computed checksum %u",
	    segmentPathName, oldChecksum, newChecksum
	)
    );
}


/******************************************************
 *****  Segment Descriptor Maintenance Utilities  *****
 ******************************************************/

PrivateFnDef PS_SDC* CreateSDC (
    PS_SDC*			sdcList,
    unsigned int		segmentNumber,
    PS_SH*			segmentHeader
)
{
    PS_SDC* sdc = (PS_SDC*)UTIL_Malloc (sizeof (PS_SDC));

    PS_SDC_Successor	(sdc) = sdcList;
    PS_SDC_TID		(sdc) = PS_SH_TID (segmentHeader);
    PS_SDC_RID		(sdc) = PS_SH_RID (segmentHeader);
    PS_SDC_Segment	(sdc) = segmentNumber;

    return sdc;
}

PrivateFnDef PS_SDC* CreateSDC (
    PS_SDC*			sdcList,
    PS_SD			sd
)
{
    PS_SDC* sdc = (PS_SDC*)UTIL_Malloc (sizeof (PS_SDC));

    PS_SDC_Successor	(sdc) = sdcList;
    PS_SDC_TID		(sdc) = PS_SD_TID (sd);
    PS_SDC_RID		(sdc) = PS_SD_RID (sd);
    PS_SDC_Segment	(sdc) = PS_SD_Segment (sd);

    return sdc;
}

PrivateFnDef PS_SDC* FreeSDCList (PS_SDC* sdcList) {
    while (IsntNil (sdcList)) {
	PS_SDC* sdc = sdcList;
	sdcList = PS_SDC_Successor (sdc);
	UTIL_Free (sdc);
    }
    return sdcList;
}

PrivateFnDef PS_Type_FO CreateSDList (
    PS_SDC*			sdcList,
    NDFWriter*			pNDF,
    PS_Type_FO			sdfo
)
{
    PS_SD sd;
    PS_SD_Extensions (sd) = PS_FO_Nil;

    while (IsntNil (sdcList)) {
	PS_SD_Predecessor	(sd) = sdfo;
	PS_SD_TID		(sd) = PS_SDC_TID	(sdcList);
	PS_SD_RID		(sd) = PS_SDC_RID	(sdcList);
	PS_SD_Segment		(sd) = PS_SDC_Segment	(sdcList);

	sdcList = PS_SDC_Successor (sdcList);
	sdfo = WriteSD (pNDF, &sd);
    }
    return sdfo;
}


/*************************************
 *****  Segment Update Routines  *****
 *************************************/

void PS_ASD::OpenUpdateSegment () {
/*****  Construct the new segment's path name, ...  *****/
    char const *segmentPathName = SegmentPathName (++m_xUpdateSegment);

/*****  ... delete any old segment of the same name, ...  *****/
    remove (segmentPathName);

/*****  ... create the segment, ...  *****/
    m_iUpdateChecksum = 0;
    m_xUpdateFD = creat (segmentPathName, SegmentCreationPermission);

    if ((int)m_xUpdateFD < 0 &&
	(EMFILE == errno || ENFILE == errno || EIO == errno)) {
	M_ReclaimSegments ();
	m_xUpdateFD = creat (segmentPathName, SegmentCreationPermission);
    }

    if ((int)m_xUpdateFD >= 0) {
    }
    else if (EACCES == errno) {
	m_pAND->SetUpdateStatusTo (PS_UpdateStatus_NotPermitted);
	return;
    }
    else SignalUnixError ("Segment Creation");

/*****  ... write a segment header, ...  *****/
    PS_SH segmentHeader;
    PS_SH_Reserved	    (&segmentHeader) = 0;
    PS_SH_DataFormat	    (&segmentHeader) = Vk_DataFormatNative;
    PS_SH_SoftwareVersion   (&segmentHeader) = PS_SoftwareVersion;
    PS_SH_CTOffset	    (&segmentHeader) = PS_FO_Nil;
    PS_SH_CTSize	    (&segmentHeader) = 0;
    WriteSegmentHeader (m_xUpdateFD, &segmentHeader);
}


void PS_ASD::CloseUpdateSegment (PS_Type_FO ctOffset, unsigned int ctSize) {
/*****  ... store the resource id of the segment just created, ...  *****/
    PS_RID rid;
    PS_RID_Type	    (rid) = PS_ResourceType_UpdateSegment;
    PS_RID_Sequence (rid) = PS_ASD_RSequence (this)++;

/*****  ... rewrite the segment header, ...  *****/
    PS_SH segmentHeader;
    PS_SH_Reserved	    (&segmentHeader) = 0;
    PS_SH_DataFormat	    (&segmentHeader) = Vk_DataFormatNative;
    PS_SH_SoftwareVersion   (&segmentHeader) = PS_SoftwareVersion;
    PS_SH_CTOffset	    (&segmentHeader) = ctOffset;
    PS_SH_CTSize	    (&segmentHeader) = ctSize;
    PS_SH_TID		    (&segmentHeader) = m_pAND->TID ();
    PS_SH_RID		    (&segmentHeader) = rid;
    PS_SH_Checksum	    (&segmentHeader) = m_iUpdateChecksum;

    WriteSegmentHeader (m_xUpdateFD, &segmentHeader);

    if (TracingUpdate ()) IO_printf (
	"+++ ... synchronizing segment %d/%d ...\n", m_xSpace, m_xUpdateSegment
    );
    SynchronizeAndCloseSegment ();

    if (m_pAND->ValidatingChecksums ())
	ValidateChecksum ();
    if (TracingUpdate ()) IO_printf (
	"+++ Done with segment %d/%d.\n", m_xSpace, m_xUpdateSegment
    );

/*****  ... write the journal entry ...  ******/
    m_pAND->WriteSegmentJournalEntry (m_xSpace, m_xUpdateSegment, &segmentHeader);

/*****  ... and register the existence of the segment.  *****/
    m_pUpdateSDC = CreateSDC (m_pUpdateSDC, m_xUpdateSegment, &segmentHeader);
}

void PS_ASD::SynchronizeAndCloseSegment() {
    int retryCount = SegmentSyncRetries;

    while (fsync (m_xUpdateFD) < 0) {
	if (retryCount-- <= 0) SignalUnixError (
	    UTIL_FormatMessage (
		"Segment '%d/%d' Synchronization", m_xSpace, m_xUpdateSegment
	    )
	);
	sleep (5);
    }

    TrapUnixError (
	close (m_xUpdateFD),
	UTIL_FormatMessage ("Segment '%d/%d' Close", m_xSpace, m_xUpdateSegment)
    );
    m_xUpdateFD = -1;

#ifdef __VMS
    char const *segmentPathName = SegmentPathName (m_xUpdateSegment);
    bool b = vms_set_file_permissions(segmentPathName);
#endif
}


/************************************************
 *****  Space Update Initialization Routine *****
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to begin the update of a space.
 *
 *  Argument:
 *	this			- the address of an ASD for the space to be
 *				  updated.
 *
 *  Returns:
 *	true if the space was properly initialized for update,
 *	false otherwise.
 *
 *****/
bool PS_ASD::BeginSpaceUpdate () {
    if (UpdateIsWell ()) {
/*****  Display trace message, ...  *****/
	if (TracingUpdate ()) IO_printf (
	    "+++ Beginning space %d update...\n", m_xSpace
	    );

/*****  ... create the initial update segment, ...  *****/
	m_xUpdateSegment = PS_ASD_MaxSegment (this);
	m_pUpdateSDC = FreeSDCList (m_pUpdateSDC);
	OpenUpdateSegment ();
    }
/*****  ... and return.  *****/
    return UpdateIsWell ();
}


/************************************************
 *****  Modified Container Update Routines  *****
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to save a modified container.
 *
 *  Argument:
 *	ap			- a 'stdargs' pointer referencing the container
 *				  threshold size.
 *
 *  Returns:
 *	true if the container was successfully saved, false otherwise.
 *
 *****/
bool VContainerHandle::StoreContainer (VArgList const&) {
/*****  Don't save the container table, ...  *****/
    M_CPreamble* cp = m_pContainerAddress;
    unsigned int ci = M_CPreamble_POPContainerIndex (cp);
    if (M_KnownCTI_ContainerTable == ci)
	return true;

/*****  ... validate the container's framing, ...  *****/
    M_CPreamble_ValidateFraming (cp);

/*****  ... saving everything else that's valid, ...  *****/
    PS_ASD *asd = m_pASD->persistentASD ();

/*****  ... starting a new segment if the old one just got too big:  *****/
    PS_Type_FO containerOffset = SegmentSeek (PS_ASD_UpdateFD (asd), PS_FO_Nil);
    if ((containerOffset
	    + M_CPreamble_NSize (cp)
	    + sizeof (M_CPreamble)
	    + sizeof (M_CEndMarker)
	    > PS_ASD_MaxSegmentSize (asd)) &&
	(containerOffset > sizeof (PS_SH))
    )
    {
	asd->CloseUpdateSegment (0, 0);
	asd->OpenUpdateSegment ();
    }

    if (asd->UpdateIsWell ()) {
	containerOffset = WriteContainer (
	    PS_ASD_UpdateFD (asd), &PS_ASD_UpdateChecksum (asd), cp
	);
	PS_CT_Entry (PS_ASD_UpdateCT (asd), ci).setSegmentAndOffset (
	    PS_ASD_UpdateSegment (asd), containerOffset, IsReadOnly ()
	);

	m_pDCTE->setReferenceCountToInfinity ();
	m_pDCTE->setMustBeRemapped ();
    }

    return asd->UpdateIsWell ();
}


/*---------------------------------------------------------------------------
 *****  Internal routine to save a modified container larger than
 *****  LargeContainerSize.
 *
 *  Argument:
 *	ap			- a 'stdargs' pointer referencing the container
 *				  threshold size.
 *
 *  Returns:
 *	true if the container was successfully processed, false otherwise.
 *
 *****/
bool VContainerHandle::StoreLargeContainer (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    return ContainerSize () < iArgList.arg<unsigned int>() || StoreContainer (iArgList);
}

/*---------------------------------------------------------------------------
 *****  Internal routine to save a modified container smaller than
 *****  LargeContainerSize.
 *
 *  Argument:
 *	ap			- a 'stdargs' pointer referencing the container
 *				  threshold size.
 *
 *  Returns:
 *	true if the container was successfully procesed, false otherwise.
 *
 *****/
bool VContainerHandle::StoreSmallContainer (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    return ContainerSize () >= iArgList.arg<unsigned int>() || StoreContainer (iArgList);
}

/**********************************************
 *****  Space Update Termination Routine  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to end the update of a space.
 *
 *  Argument:
 *	this			- the address of an ASD for the space being
 *				  updated.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_ASD::EndSpaceUpdate () {
/*****  Determine the true minimum segment for the space, ...  *****/
    unsigned int minSegment = m_xUpdateSegment;
    for (unsigned int cti = 1; cti < (unsigned int)PS_CT_EntryCount (m_pUpdateCT); cti++) {
	PS_CTE *ctep = &PS_CT_Entry (m_pUpdateCT, cti);
	if (PS_CTE_IsFree (*ctep) || PS_CTE_Segment (*ctep) >= minSegment)
	    continue;
	minSegment = PS_CTE_Segment (*ctep);
    }
    m_xMinSMD = minSegment - m_xBaseSegment;

/*****  Determine the current end of the segment file, ...  *****/
    PS_Type_FO ctOffset = SegmentSeek (m_xUpdateFD, PS_FO_Nil);

/*****
 *  ... update the container table container table entry, ...
 *      (The preceeding is not a typographical error!!!!)
 *****/
    PS_CT_Entry (m_pUpdateCT, M_KnownCTI_ContainerTable).setSegmentAndOffset (
	m_xUpdateSegment, ctOffset
    );
    PS_ASD_UpdateCTOffset (this) = ctOffset;

/*****  ... write the container table, ...  *****/
    M_CPreamble* ctPreamble = (M_CPreamble*)m_pUpdateCT - 1;
    WriteContainer (m_xUpdateFD, &m_iUpdateChecksum, ctPreamble);

/*****  ... and close the segment.  *****/
    CloseUpdateSegment (
	ctOffset, M_CPreamble_NSize (ctPreamble) + sizeof (M_CPreamble)
    );
}


/*****************************************************
 *****  Space Compaction Property Determination  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize the compaction plan for a space.
 *
 *  Arguments:
 *	this			- the address of the physical ASD for the space
 *				  being updated.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_ASD::DetermineCompactionPlan () {
    if (TracingCompaction ()) {
	PS_TS currentTime = CurrentTime ();
	IO_printf (
	    "\n+++  Compaction Statistics For Space %d:\n     Time Now: %s\n",
	    m_xSpace, Ctimeval (&currentTime)
	);
    }

    m_bDoingCompaction = true;
    if (m_bMinSMDSet &&
	MinSavedSegment () >= (unsigned int)PS_ASD_MinSegment  (this) &&
	MinSavedSegment () <= (unsigned int)PS_ASD_RootSegment (this) + 1
    )
    {
	if (TracingCompaction ()) IO_printf (
	    "    Forced MSS = %d\n", MinSavedSegment ()
	);
    }
    else {
	m_pLogicalASD->VisitUnaccessedContainers ();

	double copyLimit = (double)(m_iMaxCompactionSegs - 1) * (double)m_iMaxSegmentSize;
	if (TracingCompaction () && m_iMaxCompactionSegs != INT_MAX) IO_printf (
	    "     Segment Size Limit = %1.0f\n",  copyLimit
	);

	double copied		= 0.0;
	double maxCopied	= 0.0;
	double maxCopied1	= 0.0;
	double reclaimed	= 0.0;
	double maxReclaimed	= 0.0;
	double maxReclaimed1	= 0.0;
	double maxrr		= 0.0;
	unsigned int xMinSMD	= 0;
	unsigned int xMinSMD1	= 0;
	for (
	    unsigned int smdIndex = 0;
	    smdIndex < 1 + PS_ASD_RootSegment (this) - m_xBaseSegment;
	    smdIndex++
	)
	{
	    PS_SMD *smd = PS_ASD_SMV (this) + smdIndex;
	    if (0 == PS_SMD_SegmentSize (smd)) {
		char const *segmentPath = SegmentPathName (m_xBaseSegment + smdIndex);
		struct stat segmentStats;
		if (-1 == stat (segmentPath, &segmentStats)) {
		    if (ENOENT != errno) SignalUnixError (
			     UTIL_FormatMessage (
				 "Compaction Plan Generation Segment Stat [%s]",
				 segmentPath
			     )
		     );
		}
		else
		    smd->fileMapping.SetRegionSizeTo ((size_t)segmentStats.st_size);
	    }
	    copied	+= PS_SMD_SegmentUtilization	(smd);
	    reclaimed	+= PS_SMD_SegmentSize 		(smd);
	    double rr	= m_iReclaimCoefficient * (reclaimed - copied)
			- m_iCopyCoefficient * copied;
	    if (rr > maxrr) {
		maxCopied	= copied;
		maxReclaimed	= reclaimed;
		maxrr		= rr;
		xMinSMD		= smdIndex + 1;
	    }
	    if (copied < copyLimit) {
		maxCopied1	= copied;
		maxReclaimed1	= reclaimed;
		xMinSMD1	= smdIndex + 1;
	    }
	    if (TracingCompaction ()) IO_printf (
		"%7d%12.0f%9d%9d%12.0f%12.0f\n",
		m_xBaseSegment + smdIndex,
		rr,
		PS_SMD_SegmentSize		(smd),
		PS_SMD_SegmentUtilization	(smd),
		reclaimed,
		copied
	    );
	}

	m_xMinSMD = V_Min (xMinSMD, xMinSMD1);
	if (TracingCompaction ()) IO_printf (
	    "    MSS = %d [Reclaimed = %12.0f, Copied = %12.0f]\n",
	    MinSavedSegment (), V_Min (maxReclaimed, maxReclaimed1), V_Min (maxCopied,maxCopied1)
	);
    }
    m_bMinSMDSet = false;
}


/********************************************
 *****  Modified Space Update Routines  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****	Physical space reference persistence.
 *
 *  Returns:
 *	true if all references in the space's reference closure will persist.
 *
 *****/
bool PS_ASD::PersistReferences () {
    if (UpdateIsntWell ())
	return false;

    if (TracingUpdate ()) IO_printf (
	"+++ Computing space %d closure...\n", m_xSpace
    );

    m_pLogicalASD->EliminateTransientReferences ();
    if (UpdateIsntWell ())
	return false;

    if (TracingUpdate ()) IO_printf (
	"+++ ...space %d closure computed.\n", m_xSpace
    );

    return true;
}

/*---------------------------------------------------------------------------
 *****  Physical space structure persistence.
 *
 *  Returns:
 *	true if the space was updated successfully, false otherwise.
 *
 *  Notes:
 *	This is the routine that does the actual work of making structures
 *	persistent.
 *****/
bool PS_ASD::PersistStructures () {
    if (UpdateIsntWell ())
	return false;
    if (m_pAND->DoingCompaction () && m_bInCompaction)
	DetermineCompactionPlan ();
    else
	m_bDoingCompaction = false;

    m_pLogicalASD->EnablePCTModification ();

    if (BeginSpaceUpdate ()) {
	unsigned int threshold = m_iLargeContainerSize;
	m_pLogicalASD->EnumerateUpdatingContainers (
	    &VContainerHandle::StoreLargeContainer, threshold
	);

	/****
	 *  If there are no "large" containers being saved,
	 *  don't start a new segment. Check to see if a segment header is all
	 *  that is in the update segment ....
	 ****/
	PS_Type_FO filesize = SegmentSeek (m_xUpdateFD, PS_FO_Nil);
	if (sizeof (PS_SH) < filesize) {
	    CloseUpdateSegment (0, 0);
	    OpenUpdateSegment ();
	}
	if (UpdateIsWell ()) {
	    if (m_bCompactingSmall) m_bCompactingAll = true;
	    m_pLogicalASD->EnumerateUpdatingContainers (
		&VContainerHandle::StoreSmallContainer, threshold
	    );
	    m_bCompactingAll = false;
	    EndSpaceUpdate ();
	}
    }

    return UpdateIsWell ();
} 


/*---------------------------------------------------------------------------
 *****  Logical space reference/structure persistence routines.
 *
 *  Returns:
 *	true if the space was successfully updated, false otherwise.
 *
 *  Notes:
 *	These routine is intended for use as the action routine of an
 *	'M_Enumerate*Spaces' routine.
 *
 *****/
bool M_ASD::PersistReferences (VArgList const&) {
    return IsNil (m_pPASD) || m_pPASD->PersistReferences ();
}

bool M_ASD::PersistStructures (VArgList const&) {
    return IsNil (m_pPASD) || m_pPASD->PersistStructures ();
}


/********************************************
 *****  Modified Spaces Update Routine  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routines to update the modified spaces of a network.
 *
 *  Arguments:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_AND::PersistReferences (M_AND *pLogicalAND) {
    if (UpdateIsntWell ())
	return;

    pLogicalAND->EnumerateModifiedSpaces (&M_ASD::PersistReferences);
}

void PS_AND::PersistStructures (M_AND *pLogicalAND) {
    if (UpdateIsntWell ())
	return;

    pLogicalAND->EnumerateModifiedSpaces (&M_ASD::PersistStructures);
}


/***********************************
 ***********************************
 *****  Update Commit Routine  *****
 ***********************************
 ***********************************/

/*--------------------------------------------------------------------------
 *  Internal routine that writes an ascii file to a space's segment directory
 *  holding the "base name" (segment index) of the earliest segment referenced
 *  by space versions after the last compaction.  This file will be read by a
 *  routine that discards obsolete segment files.
 *
 *  Arguments:
 *	rSVD		- a reference to an SVD containing the information
 *			  to record.
 *
 *  Returns:  NOTHING - executed for side effect only
 *
 *****/
void PS_AND::SaveMinSegIndexInFile (PS_SVD const& rSVD) {
    char const *segDirPath = SpacePathName (rSVD);
    char  	filePath [PathNameBufferSize];
    FILE	*mssfp;
    int		retryPossible, retryNecessary;

/*****  form the path for the new file and create it *****/
    strcpy (filePath, segDirPath);
    strcat (filePath, "/MSS");
    remove (filePath);

    retryPossible = true;
    do {
	mssfp = fopen (filePath, "w");
	if (IsNil (mssfp) && retryPossible &&
	    (EMFILE == errno || ENFILE == errno || EIO == errno)) {
	    M_ReclaimSegments ();
	    retryPossible = false;
	    retryNecessary = true;
	}
	else retryNecessary = false;
    } while (retryNecessary);

    TrapUnixError (
	IsntNil (mssfp) ? 0 : -1,
	UTIL_FormatMessage ("MSS File Creation [%s]", filePath)
    );

/*****  fill it in from oldestSVD.minSegment in spaceInfoList  *****/
    STD_fprintf (mssfp, "%d\n", PS_SVD_MinSegment (rSVD));
    fclose (mssfp);

    WriteMinimumSegmentJournalEntry (
	PS_SVD_SpaceIndex (rSVD), PS_SVD_MinSegment (rSVD)
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to commit a network update.
 *
 *  Argument:
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_AND::CommitUpdate () {
    PS_ASD	*asd;
    unsigned int srdIndex;

    if (UpdateIsntWell ())
	return;

/*****  Display trace message, ...  *****/
    if (m_bTracingUpdate) IO_printf (
	"+++ Starting commit processing with annotation <%s>...\n",
	m_pUpdateAnnotation ? m_pUpdateAnnotation : ""
    );

/*****  Lock the object network, ...  *****/
    LockNetwork ();

/*****  ... load the current network version state, ...  *****/
    int ndfd = DirectoryFD ();
    NDFWriter* pNDF = new NDFWriter (ndfd);

    PS_NDH ndh;
    ReadNDH (ndfd, &ndh);

    PS_NVD nvd;
    ReadNVD (ndfd, PS_NDH_CurrentNetworkVersion (ndh), &nvd);

    PS_SRD srv[M_POP_MaxObjectSpace + 1];
    unsigned int srdCount = ReadSRV (
	ndfd, PS_NVD_SRV (nvd), srv, M_POP_MaxObjectSpace + 1, PS_NVD_SoftwareVersion (nvd)
    );

/*****  ... set the roles of all extant spaces to 'Unaccessed', ...  *****/
    for (srdIndex = 0; srdIndex < srdCount; srdIndex++) {
	if (PS_Role_Nonextant != PS_SRD_Role (srv[srdIndex]))
	    PS_SRD_Role (srv[srdIndex]) = PS_Role_Unaccessed;
    }

/*****  ... update the roles of all accessed and modified spaces, ...  *****/
    for (asd = m_pASDList; asd; asd = asd->NextASD ()) {
	srdIndex = asd->spaceIndex ();
	if (PS_ASD_Updated (asd)) {
	    PS_SVD svd = PS_ASD_SVD (asd);

	/*****  if new spaces are being created, initialize their SRDs  *****/
	    while (srdCount <= srdIndex) {
		PS_SRD_Role (srv[srdCount  ]) = PS_Role_Nonextant;
		PS_SRD_SVD  (srv[srdCount++]) = PS_FO_Nil;
	    }
	/*****  update the previous-svd chain  *****/
	    PS_SVD_PreviousSVD		(svd) = PS_SRD_SVD (srv[srdIndex]);
	    PS_SVD_MinSegment		(svd) = asd->baseSegment () + asd->MinSMDIndex ();
	    PS_SVD_OrigRootSegment	(svd) =
	    PS_SVD_RootSegment		(svd) = PS_ASD_UpdateSegment (asd);
	    if (PS_SVD_MaxSegment	(svd) < PS_ASD_UpdateSegment (asd))
		PS_SVD_MaxSegment	(svd) = PS_ASD_UpdateSegment (asd);
	    PS_SVD_CTOffset		(svd) = PS_ASD_UpdateCTOffset (asd);

	    PS_SVD_SD			(svd) = CreateSDList (
		PS_ASD_UpdateSDC (asd), pNDF, PS_SVD_SD (svd)
	    );
	    PS_SVD_MP			(svd) =
	    PS_SVD_Extensions		(svd) = PS_FO_Nil;

	    PS_SRD_SVD  (srv[srdIndex]) = WriteSVD (pNDF, &svd);
	    PS_SRD_Role (srv[srdIndex]) = PS_Role_Modified;

	    if (m_bMakingBaseVersion)
		SaveMinSegIndexInFile (svd);
	}
	else PS_SRD_Role (srv[srdIndex]) = PS_Role_Read;
    }
    if (m_bMakingBaseVersion) for (
	srdIndex = 0; srdIndex < srdCount; srdIndex++
    ) if (
	PS_Role_Read	    == PS_SRD_Role (srv[srdIndex]) ||
	PS_Role_Unaccessed  == PS_SRD_Role (srv[srdIndex])
    ) {
	PS_SVD svd;
	ReadSVD (ndfd, PS_SRD_SVD (srv[srdIndex]), &svd);
	SaveMinSegIndexInFile (svd);
    }

/*****  ... create a new network version state, ...  *****/
    PS_NVD_Creator		(nvd) = CurrentUser ();
    PS_NVD_CommitTimeStamp	(nvd) = CurrentTime ();
    PS_NVD_SoftwareVersion	(nvd) = PS_SoftwareVersion;
    PS_NVD_SRV			(nvd) = WriteSRV (pNDF, srv, srdCount);
    PS_NVD_PreviousVersion	(nvd) = PS_NDH_CurrentNetworkVersion (ndh);
    PS_NVD_UpdateThread		(nvd) = m_iUpdateFO;
    PS_NVD_AccessedVersion	(nvd) = m_iAccessFO;
    PS_NVD_DirectoryVersion	(nvd) = PS_NDH_DirectoryVersion (ndh);
    PS_NVD_Signature		(nvd) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		(nvd) = m_bMakingBaseVersion;
    PS_NVD_UnusedFlags		(nvd) = 0;
    PS_NVD_TID			(nvd) = m_iTID;


    if (m_pUpdateAnnotation)
	PS_NVD_Annotation	(nvd) = WriteString (pNDF, m_pUpdateAnnotation);
    else
	PS_NVD_Annotation	(nvd) = PS_FO_Nil;

    PS_NVD_TD			(nvd) =
    PS_NVD_MP			(nvd) =
    PS_NVD_Extensions		(nvd) = PS_FO_Nil;

    PS_NDH_CurrentNetworkVersion(ndh) = WriteNVD (pNDF, &nvd);
    PS_NDH_ModificationTimeStamp(ndh) = CurrentTime ();

    pNDF->ValidateChecksum ();
    WriteNDH		(pNDF, &ndh);
    pNDF->Synchronize	(NDFPathName ());
    delete pNDF;

/*****  ... unlock the object network (and reopen NDF in read-only mode), ...  *****/
    UnlockNetwork (true);

/*****  ... write the commit journal entry, ...  *****/
    WriteCommitJournalEntry (PS_NDH_CurrentNetworkVersion (ndh));

/*****  ... set the update status to successful, ...  *****/
    m_xUpdateStatus = PS_UpdateStatus_Successful;

/*****  ... display the trace message, ...  *****/
    if (m_bTracingUpdate) IO_printf (
	"+++ Done with commit processing.\n"
    );

/*****  ... clear the annotation, ...  *****/
    if (m_pUpdateAnnotation) {
	UTIL_Free ((void*)m_pUpdateAnnotation);
	m_pUpdateAnnotation = NilOf (char const*);
    }

/*****  ... update the process update thread, ...  *****/
    m_iUpdateFO	 = PS_NDH_CurrentNetworkVersion (ndh);
    m_iUpdateNVD = nvd;

/*****  ... and update the network's ASDs to reference the new version.  *****/
    for (asd = m_pASDList; asd; asd = asd->NextASD ()) {
	if (!PS_ASD_Updated (asd))
	    continue;

	PS_ASD_SRD (asd) = srv[asd->spaceIndex ()];

	PS_SVD svd;
	ReadSVD (ndfd, PS_ASD_SVDFO (asd), &svd);

	PS_ASD_SMV (asd) = GrowSMV (
	    PS_ASD_SMV (asd),
	    PS_ASD_RootSegment (asd),
	    PS_SVD_RootSegment (svd),
	    asd->baseSegment ()
	);
	PS_ASD_SVD (asd) = svd;
	asd->GarnerSDCSegmentInfo ();

	PS_SH *rootSegment = asd->MapSegment (PS_ASD_RootSegment (asd));
	if (!PS_ASD_CTIsMapped (asd))
	    free ((M_CPreamble*)(PS_ASD_CT (asd)) - 1);

	PS_ASD_CT		(asd) = PS_SH_ContainerBaseAsType (
	    rootSegment, PS_ASD_CTOffset (asd), PS_CT
	);
	PS_ASD_CTIsMapped	(asd) = true;
	PS_ASD_UpdateCT		(asd) = NilOf (PS_CT*);

	for (unsigned int smdIndex = 0; smdIndex < asd->MinSMDIndex (); smdIndex++)
	    PS_SMD_SegmentUtilization (PS_ASD_SMV (asd) + smdIndex) = 0;

	if (asd->LogicalASD ())
	    asd->LogicalASD ()->RemapSpace ();
    }
}


/*****************************************
 *****  Master Space Update Routine  *****
 *****************************************/

void PS_AND::UpdateSpace (PS_ASD *pASD) {
    if (!m_iLicense.isValid ()) {
	m_xUpdateStatus = PS_UpdateStatus_NoLicense;
	return;
    }
    if (CommitsDisabled) {
	m_xUpdateStatus = PS_UpdateStatus_Disabled;
	return;
    }

    M_DisposeOfSessionGarbage ();

    ResetUpdateStatus	();
    ResetLockManager	();

    BeginRecoverableUpdateSection {
	do {
	    WaitForUngrantedLocks ();
	    LockNetwork ();
	    pASD->ValidateSpaceConsistency ();
	    pASD->LockSpace ();
	    UnlockNetwork ();
	} while (UngrantedLocksExist ());
	pASD->PersistReferences ();
	pASD->PersistStructures ();
	CommitUpdate ();
	UnlockSpaces ();
    } EndRecoverableUpdateSection;
}


/*******************************************
 *****  Master Network Update Routine  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to update a persistent object network.
 *
 *  Argument:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *
 *  Returns:
 *	NOTHING explicitly; however, 'm_xUpdateStatus' will be
 *	set to the appropriate 'PS_UpdateStatus' code.
 *
 *****/
void PS_AND::UpdateNetwork (M_AND *pLogicalAND) {
    if (!m_iLicense.isValid ()) {
	m_xUpdateStatus = PS_UpdateStatus_NoLicense;
	return;
    }
    if (!CurrentUserIsInRightsList ()) {
	m_xUpdateStatus = PS_UpdateStatus_NotPermitted;
	return;
    }
    if (CommitsDisabled) {
	m_xUpdateStatus = PS_UpdateStatus_Disabled;
	return;
    }

    M_DisposeOfSessionGarbage ();

    ResetUpdateStatus	();
    ResetLockManager	();

    BeginRecoverableUpdateSection {
	do {
	    WaitForUngrantedLocks ();
	    LockNetwork ();
	    ValidateNetworkConsistency (pLogicalAND);
	    LockSpaces (pLogicalAND);
	    UnlockNetwork ();
	} while (UngrantedLocksExist ());
	PersistReferences (pLogicalAND);
	PersistStructures (pLogicalAND);
	CommitUpdate ();
	UnlockSpaces ();
    } EndRecoverableUpdateSection;
}


/*******************************
 *******************************
 *****  Creation Routines  *****
 *******************************
 *******************************/

/**************************************
 *****  Space Creation Utilities  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a pseudo container table.
 *
 *  Argument:
 *	spaceIndex		- the index of the space to be associated with
 *				  the container table.
 *
 *  Returns:
 *	The address of the pseudo container table.
 *
 *  Notes:
 *	This routine reuses an internal static structure with each call.
 *
 *****/
PrivateFnDef PS_CT* PseudoCT (unsigned int spaceIndex) {
    typedef struct M_CPreamble M_CPreamble;
    typedef struct PS_CT PS_CT;
    typedef struct M_CEndMarker M_CEndMarker;

    static struct pseudoct_t {
	M_CPreamble	p;
	PS_CT		t;
	M_CEndMarker	e;
    } pseudoCT;
    M_CPreamble*	p = &pseudoCT.p;
    PS_CT*		t = &pseudoCT.t;
    M_CEndMarker*	e = &pseudoCT.e;

/*****  Initialize the 'pseudo' container, ...  *****/
    M_CEndMarker_Size			(e) =
    M_CPreamble_NSize			(p) = sizeof (PS_CT);
    M_CPreamble_RType			(p) = (unsigned char)RTYPE_C_PCT;
    M_CPreamble_OSize			(p) = 0;
    M_CPreamble_POPObjectSpace		(p) = spaceIndex;
    M_CPreamble_POPContainerIndex	(p) = M_KnownCTI_ContainerTable;

/*****  ... initialize the 'pseudo' container table, ...  *****/
    PS_CT_SpaceIndex	(t) = spaceIndex;
    PS_CT_SegmentIndex	(t) = 0;
    PS_CT_EntryCount	(t) = 2;
    PS_CT_FreeList	(t) = M_KnownCTI_SpaceRoot;

    PS_CTE &rCTE = PS_CT_Entry (t, M_KnownCTI_SpaceRoot);
    PS_CTE_IsCopy   (rCTE) = false;
    PS_CTE_IsFree   (rCTE) = true;
    PS_CTE_NextFree (rCTE) = PS_CT_FreeListNil;

/*****  ... and return the 'pseudo' container table address.  *****/
    return t;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to create an ASD for a new, empty space.
 *
 *  Argument:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *
 *  Returns:
 *	The address of an ASD for the new space.
 *
 *****/
PS_ASD *PS_AND::NewSpaceASD (M_AND *pLogicalAND) {
/*****  Access the current network version state, ...  *****/
    int ndfd = DirectoryFD ();

    PS_NDH ndh;
    ReadNDH (ndfd, &ndh);

    PS_NVD nvd;
    ReadNVD (ndfd, PS_NDH_CurrentNetworkVersion (ndh), &nvd);

/*****  ... determine the index of the new space, ...  *****/
    unsigned int spaceCount = NDFReadCountedArrayElementCount (ndfd, PS_NVD_SRV (nvd));
    unsigned int spaceIndex;
    for (spaceIndex = 1; spaceIndex < spaceCount; spaceIndex++) {
	PS_SRD srd;
	ReadSRD (ndfd, PS_NVD_SRV (nvd), spaceIndex, &srd, PS_NVD_SoftwareVersion (nvd));
	if (PS_Role_Nonextant == PS_SRD_Role (srd))
	    break;
    }

/*****  ... abort if that index conflicts with an attached space, ...  *****/
    if (IsntNil (pLogicalAND->m_iASDVector[spaceIndex])) {
	m_xUpdateStatus = PS_UpdateStatus_Inconsistent;
	return NilOf (PS_ASD*);
    }

/*****  ... reallocate/align network space cache arrays, ...  *****/
    if (spaceIndex >= m_iSpaceCount) {
	spaceCount = spaceIndex + 1;
	m_pSRDArray = (PS_SRD *)UTIL_Realloc (m_pSRDArray, sizeof (PS_SRD) * spaceCount);
	m_pSVDArray = (PS_SVD *)UTIL_Realloc (m_pSVDArray, sizeof (PS_SVD) * spaceCount);
	m_iSpaceCount = spaceCount;

	if (pLogicalAND->m_cActiveSpaces < m_iSpaceCount)
	    pLogicalAND->m_cActiveSpaces = m_iSpaceCount;
    }

/*****  ... initialize the network space cache structures, ...  *****/
    PS_AND_Role		(this, spaceIndex) = PS_Role_Nonextant;
    PS_AND_SVDFO	(this, spaceIndex) = PS_FO_Nil;

    PS_SVD *svdp = &SVD (spaceIndex);
    PS_SVD_SpaceIndex		(*svdp) = spaceIndex;
    PS_SVD_MinSegment		(*svdp) = 0;
    PS_SVD_MaxSegment		(*svdp) =
    PS_SVD_OrigRootSegment	(*svdp) =
    PS_SVD_RootSegment		(*svdp) = -1;
    PS_SVD_PreviousSVD		(*svdp) = PS_FO_Nil;
    PS_SVD_SpacePathName	(*svdp) = PS_NVD_NewSpacePathName (nvd);
    STD_sprintf (PS_SVD_SpaceBaseName (*svdp), "%d", spaceIndex);
    PS_SVD_Signature		(*svdp) = PS_NetworkDirectorySignature;
    PS_SVD_SD			(*svdp) =
    PS_SVD_MP			(*svdp) =
    PS_SVD_Extensions		(*svdp) = PS_FO_Nil;

/*****  ... and return an ASD for the space:  *****/

    return m_pASDList = new PS_ASD (this, spaceIndex, PseudoCT (spaceIndex));
}


/************************************
 *****  Space Creation Routine  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create and access a new object space.
 *
 *  Argument:
 *	pLogicalAND		- the address of the M_AND initiating this
 *				  update.
 *
 *  Returns:
 *	The address of an 'Accessed Space Descriptor' for the new space.  If
 *	the space could not be created, this routine will return 'Nil' and set
 *	'm_xUpdateStatus' to the appropriate 'PS_UpdateStatus'
 *	code.
 *
 *****/
PS_ASD *PS_AND::CreateSpace (M_AND *pLogicalAND) {
    if (!m_iLicense.isValid ()) {
	m_xUpdateStatus = PS_UpdateStatus_NoLicense;
	return NilOf (PS_ASD*);
    }
    if (!CurrentUserIsInRightsList ()) {
	m_xUpdateStatus = PS_UpdateStatus_NotPermitted;
	return NilOf (PS_ASD*);
    }
    if (CommitsDisabled) {
	m_xUpdateStatus = PS_UpdateStatus_Disabled;
	return NilOf (PS_ASD*);
    }

    ResetUpdateStatus	();
    ResetLockManager	();

    PS_ASD *newSpace = NilOf (PS_ASD*);
    BeginRecoverableUpdateSection {
	LockNetwork ();
	newSpace = NewSpaceASD (pLogicalAND);
	if (newSpace) {
	    newSpace->BeginSpaceUpdate ();
	    newSpace->EndSpaceUpdate ();
	    CommitUpdate ();
	}
    } EndRecoverableUpdateSection;

    return newSpace;
}


/****************************************
 *****  Network Creation Utilities  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to enumerate the network components in a directory.
 *
 *  Arguments:
 *	directoryPathName	- the address of a string containing the path
 *				  name of the directory to be searched for
 *				  network components.
 *	componentProcessingFn	- the address of a function which will be
 *				  called to process each component found.
 *				  This function will be presented with three
 *				  arguments - 'directoryPathName', the
 *				  numeric index of the component, and a
 *				  <varargs.h> argument pointer positioned at
 *				  the next argument.  This function is
 *				  expected to return a non-zero integer to
 *				  request the next element in the enumeration
 *				  and 0 to request that the enumeration be
 *				  terminated.
 *	componentProcessingFnArg1, ...
 *				- a series of optional arguments that will be
 *				  passed on to the 'componentProcessingFn'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void __cdecl EnumNetComponentsInDirectory (
    char const *directoryPathName, int (*componentProcessor) (
	char const*directoryPathName, int directoryEntry, V::VArgList iArgList
    ), ...
) {
    DIR* dirPointer = opendir (directoryPathName);

    TrapDirectoryError (
	dirPointer, UTIL_FormatMessage ("Directory Open '%s'", directoryPathName)
    );

    unsigned int okToProceed = true;
    STD_DirectoryEntryType* dirEntry;
    while (IsntNil (dirEntry = readdir (dirPointer)) && okToProceed) {
#ifdef __VMS
        int sNumericString = strspn (STD_DirectoryEntryName (dirEntry), "0123456789");
        char *pPostNumericString = STD_DirectoryEntryName (dirEntry) + sNumericString;
	if (sNumericString == STD_DirectoryEntryNameLen (dirEntry) ||
            0 == strcmp (pPostNumericString, ".")) { // segments have a trailing '.' on VMS
#else
	if (strspn (STD_DirectoryEntryName (dirEntry), "0123456789") == STD_DirectoryEntryNameLen (dirEntry)) {
#endif
	    V_VARGLIST (componentProcessorArgs, componentProcessor);
	    okToProceed = componentProcessor (
		directoryPathName,
		atoi (STD_DirectoryEntryName (dirEntry)),
		componentProcessorArgs
	    );
	}
    }
    closedir (dirPointer);
}


/*---------------------------------------------------------------------------
 *****  Internal 'EnumNetComponentsInDirectory' component processor
 *****  to determine the minimum and maximum segments present in a space:
 *
 *	*min  -- the lowest segment index,
 *	*max  -- the highest segment index.
 *
 *  Arguments:
 *	segDirPath		- the address of a string containing the path
 *				  name of the segment directory being searched
 *				  (the space whose segs are to be partitioned)
 *	segIndex		- the index of the segment file to scan
 *	ap			- a <varargs.h> argument pointer positioned at:
 *				   o  the address of the min. seg. index,
 *				   o  address of the max. seg. index,
 *  Returns: true
 *  Note:  *min, *max must be initialized to -1 before this routine is
 *	   used via the enumeration routine.
 *****/
PrivateFnDef int ExamineSegments (char const *, int segIndex, V::VArgList iArgList) {
/*****  collect varargs parameters  *****/
    int *const min = iArgList.arg<int *>();
    int *const max = iArgList.arg<int *>();

/*****  set min, max  *****/
    *min = (*min == -1)? segIndex : V_Min (*min, segIndex);
    *max = (*max == -1)? segIndex : V_Max (*max, segIndex);

    return true;
}


/*--------------------------------------------------------------------------
 *  linked list node used by the space compactor to store copies of
 *  container tables.
 *****/
struct PS_CTList {
    M_CPreamble		preamble;
    PS_CT*		ct;
};

/*****  Access Macros  *****/
#define PS_CTList_Preamble(ctList)\
    (ctList).preamble

#define PS_CTList_Ct(ctList)\
    (ctList).ct

#define PS_CTListPtr_Preamble(ctList)\
    (ctList)->preamble

#define PS_CTListPtr_Ct(ctList)\
    (ctList)->ct

/*--------------------------------------------------------------------------
*  Internal routine to read a Container Table (including it's preamble)
*  from a space version into a buffer it has allocated
*
*  Arguments:
*	segDirPath	-- null-terminated string holding the name of the
*			   segment directory containing the segment
*	svdPtr		-- address of a copy of the Space Version Descriptor
*		   	   for the space
*
*  Returns:  address of a list node containing the copied table and it's
*	     preamble.  The link field is set to Nil
*****/
PrivateFnDef PS_CTList* ReadContainerTable (char const *segDirPath, PS_SVD *svdPtr) {
/*****  form segfile name; open the file *****/
    char segmentPath[PathNameBufferSize];
    strcpy  (segmentPath, segDirPath);
    STD_sprintf(segmentPath + strlen(segmentPath), "/%d", svdPtr->rootSegment);

    int segFd = open (segmentPath, O_RDONLY, 0);
    TrapUnixError (
	segFd, UTIL_FormatMessage ("Open Segment [%s]", segmentPath)
    );

/*****  read the header and compute the container table offset  *****/
    PS_SH segHeader;
    ReadSegmentHeader (segFd, &segHeader);

    PS_Type_FO ctfo = PS_SVD_CTOffset (*svdPtr);
    if (PS_SH_IsASmallContainerSegment (&segHeader))
	ctfo -= sizeof (unsigned int);

/*****  allocate the container table list element structure  *****/
    PS_CTList* ctList = (PS_CTList*) UTIL_Malloc (sizeof (PS_CTList));

/*****  read the preamble  *****/
    SegmentReadStructure (segFd, ctfo, &ctList->preamble, sizeof(M_CPreamble));
    M_CPreamble_FixSize (&ctList->preamble);

/*****  alloc space for Container Table *****/
    ctList->ct = (PS_CT*) UTIL_Malloc (
	M_CPreamble_NSize (&ctList->preamble) + sizeof (M_CEndMarker)
    );

/*****  copy the entire Container Table, except for the preamble *****/
    SegmentReadStructure (
	segFd,
	ctfo + sizeof(M_CPreamble),
	ctList->ct,
	M_CPreamble_NSize (&ctList->preamble) + sizeof (M_CEndMarker)
    );
    M_CEndMarker_Size (
	(M_CEndMarker*)(
	    (pointer_t)(ctList->ct) + M_CPreamble_NSize (&ctList->preamble)
	)
    ) = M_CPreamble_NSize (&ctList->preamble);

    TrapUnixError (close (segFd), "Close Segment");
    return ctList;
}


/*--------------------------------------------------------------------------
*  Internal routine finds minimum segment referenced by the container table
*  found in the supplied SVD root segment, and plugs it into the MinSegment
*  field of the SVD.
*
*  Arguments:
*	spaceDirName	-- the segment directory as a string
*	svdptr		-- address of the Space Version Descriptor of the
*			   space whose minimum segment is sought.
*
*  Returns:  NOTHING
*****/
PrivateFnDef void SetMinSegFromMinCTESegReference (
    char const *spaceDirName, PS_SVD *svdptr
)
{
/*****  collect the space's container table  *****/
    PS_CTList *ctList = ReadContainerTable (spaceDirName, svdptr);
    PS_CT* ct = PS_CTListPtr_Ct (ctList);

/*****  find minimum segment referenced by CTEs  *****/
    int minseg = INT_MAX;
    for (int cteIndex = 0; cteIndex < PS_CT_EntryCount (ct); cteIndex++) {
	if (PS_CTE_IsFree (PS_CT_Entry (ct, cteIndex)))
	    continue;

	minseg = V_Min (minseg, (int) PS_CTE_Segment (PS_CT_Entry (ct, cteIndex)));
    }
/*****  update the SVD and discard the Container Table copy  *****/
    PS_SVD_MinSegment (*svdptr) = minseg;
    UTIL_Free (PS_CTListPtr_Ct (ctList));
    UTIL_Free (ctList);
}


/*---------------------------------------------------------------------------
 *****  Internal routine to obtain the header of a segment
 *
 *  Arguments:
 *	segDirPath		- the path name of a space.
 *	segIndex		- the index of a segment within that space.
 *
 *  Returns:
 *	The header for the segment
 *
 *****/
PrivateFnDef PS_SH AccessSegmentHeader (char const *segDirPath, int segIndex) {
/*****  open the segment file  *****/
    char segPathName [PathNameBufferSize];
    STD_sprintf (segPathName, "%s/%d", segDirPath, segIndex);

    int segFD = open (segPathName, O_RDONLY, 0);
    TrapUnixError (
	segFD, UTIL_FormatMessage ("Open Segment [%s]", segPathName)
    );

/*****  read the ctOffset into the SVD  *****/
    PS_SH result;
    ReadSegmentHeader (segFD, &result);

/*****  close the segment file  *****/
    TrapUnixError (
	close (segFD), UTIL_FormatMessage ("Close Segment [%s]", segPathName)
    );

/*****  and return the segmentHeader.  *****/
    return result;
}


/*--------------------------------------------------------------------------
*  Internal routine fills in the "ctOffset" field of an SVD by reading the
*  "ctOffset" field of the root segment file.
*
*  Arguments:
*	svd		-- address of the SVD to be updated
*	segDirPath	-- the address of a string containing the path name
*			   of the segment directory containing the root
*			   segment.
*  Returns:  NOTHING
*
*****/
PrivateFnDef void FillInCTOffsetFromSegHeader (char const *segDirPath, PS_SVD *svd) {
    PS_SH header = AccessSegmentHeader (segDirPath, PS_SVD_RootSegment (*svd));
    PS_SVD_CTOffset (*svd) = PS_SH_CTOffset (&header);

}


PS_SDC* CreateSDCList (char const *segDirPath, PS_SVD *svd) {
    PS_SDC* result = NilOf (PS_SDC*);
    for (
	int segIndex = PS_SVD_MaxSegment (*svd);
	segIndex >= PS_SVD_MinSegment (*svd);
	segIndex--
    ) {
	PS_SH segmentHeader = AccessSegmentHeader (segDirPath, segIndex);
	result = CreateSDC (result, segIndex, &segmentHeader);
    }
    return result;
}

PS_SDC* CreateSDCList (int ndfd, PS_SVD const& svd) {
    PS_SDC* result = NilOf (PS_SDC*);

    PS_SD sd;
    for (
	PS_Type_FO sdId = PS_SVD_SD (svd);
	PS_FO_Nil != sdId;
	sdId = PS_SD_Predecessor (sd)
    ) {
	ReadSD (ndfd, sdId, &sd);
	if (PS_SD_Segment (sd) >=  (unsigned int)PS_SVD_MinSegment (svd) &&
	    PS_SD_Segment (sd) <=  (unsigned int)PS_SVD_MaxSegment (svd)
	)
	{
	    result = CreateSDC (result, sd);
	}
    }
    return result;
}


/*---------------------------------------------------------------------------
 *****  Internal 'EnumNetComponentsInDirectory' component processor
 *****  to create a new SRD for the space.
 *
 *  Arguments:
 *	spaceDirPathName	- the address of a string containing the path
 *				  name of the object space directory being
 *				  searched for spaces.
 *	componentNumber		- the name of the space to be processed
 *				  as an integer
 *	ap		- a <varargs.h> argument pointer positioned at
 *				   o  address of SRV for the new net version
 *				   o  address of the count of SRDs in SRVs
 *				   o  file descr. for NDF being built
 *				   o  file offset at which the space directory
 *				      path was written in that NDF
 *  Returns:  true
 *****/
PrivateFnDef int CreateInitialSRD (
    char const *spaceDirPathName, int componentNumber, V::VArgList iArgList
) {
/*****  Acquire the parameters to this routine, ...  *****/
    PS_SRD	*const	newSRV		= iArgList.arg<PS_SRD *>();
    int		*const	srdCount	= iArgList.arg<int *>();
    NDFWriter	*const	pNDF		= iArgList.arg<NDFWriter *>();
    PS_Type_FO	 const	osdPathNameFO	= iArgList.arg<PS_Type_FO>();

/*****  Ignore any space '0' ...  *****/
    if (0 == componentNumber || M_POP_MaxObjectSpace < componentNumber)
	return true;

/***** ... determine the min, max segments contained in this space, ... *****/
    char spacePathName [PathNameBufferSize];
    STD_sprintf (spacePathName, "%s/%d", spaceDirPathName, componentNumber);
    ResolveSpaceLinks (spacePathName);

    int minSeg = -1;
    int maxSeg = -1;
    EnumNetComponentsInDirectory (
	spacePathName, ExamineSegments, &minSeg, &maxSeg
    );


/*****  ... fill in SRDs if necessary, init current SRDs, update srdCount  ***/
    while (componentNumber >= *srdCount) {
	PS_SRD_Role	(newSRV[ *srdCount   ]) = PS_Role_Nonextant;
	PS_SRD_SVD	(newSRV[(*srdCount)++]) = PS_FO_Nil;
    }

    if (minSeg != -1) {
	/*****  This Space actual has segments, so let's record it ...  *****/
	PS_SVD newSVD;

	PS_SVD_SpaceIndex		(newSVD) = componentNumber;
	PS_SVD_SpacePathName		(newSVD) = osdPathNameFO;
	STD_sprintf (PS_SVD_SpaceBaseName (newSVD), "%d", componentNumber);
	PS_SVD_MaxSegment		(newSVD) =
	PS_SVD_RootSegment		(newSVD) =
	PS_SVD_OrigRootSegment		(newSVD) = maxSeg;
	PS_SVD_PreviousSVD		(newSVD) =
	PS_SVD_SD			(newSVD) =
	PS_SVD_MP			(newSVD) =
	PS_SVD_Extensions		(newSVD) = PS_FO_Nil;
	PS_SVD_Signature		(newSVD) = PS_NetworkDirectorySignature;
	FillInCTOffsetFromSegHeader (spacePathName, &newSVD);
	SetMinSegFromMinCTESegReference (spacePathName, &newSVD);
	if (minSeg > PS_SVD_MinSegment (newSVD))
	{
	    ERR_SignalFault (
		EC__InternalInconsistency,
		    UTIL_FormatMessage (
		    "Missing Segment: %s/%d\n", 
		    spacePathName, 
		    PS_SVD_MinSegment (newSVD)
		)
	    );
	}
	PS_SDC* sdcList = CreateSDCList (spacePathName, &newSVD);
	PS_SVD_SD	(newSVD) = CreateSDList (sdcList, pNDF, PS_FO_Nil);
	FreeSDCList	(sdcList);
        PS_SRD_Role     (newSRV[componentNumber]) = PS_Role_Modified;
        PS_SRD_SVD      (newSRV[componentNumber]) = WriteSVD (pNDF, &newSVD);
    }
    return true;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to create the initial SRV for the network
 *	version created during a network rebuild
 *
 *  Arguments:
 *	fd			- a file descriptor for the new NDF being
 *				  created.
 *	lookForExistingSpaces	- a boolean which, when true, instructs this
 *				  routine to look for existing spaces in the
 *				  new space directory.
 *	spaceDirectoryPathName	- the address of a string containing the path
 *				  name of the object space directory to be
 *				  searched for existing spaces.
 *	spaceDirectoryPathNameFO- the network directory file offset at which
 *				  'spaceDirectoryPathName' was written.
 *	newSRVFO		- address of variable in which to store the
 *				  file offset of the newly created SRV for
 *				  the new network versions.
 *  Returns:  NOTHING
 *****/
PrivateFnDef void CreateInitialSRV (
    NDFWriter*			pNDF,
    int				lookForExistingSpaces,
    char const*			spaceDirectoryPathName,
    PS_Type_FO			spaceDirectoryPathNameFO,
    PS_Type_FO			*newSRVFO
)
{
    PS_SRD newSRV [M_POP_MaxObjectSpace];

    int highestExistingSRDIndex	= -1;
    int srdCount		= 0;

    if (lookForExistingSpaces) {
	EnumNetComponentsInDirectory (
	    spaceDirectoryPathName,
	    CreateInitialSRD,
	    newSRV,
	    &srdCount,
	    pNDF,
	    spaceDirectoryPathNameFO
	);
	for (int i = 0; i < srdCount; i++) {
	    if (PS_SRD_Role (newSRV [i]) != PS_Role_Nonextant)
		highestExistingSRDIndex = i;
	}
    }
    *newSRVFO = WriteSRV (pNDF, newSRV, highestExistingSRDIndex + 1);
}


/**************************************
 *****  Network Creation Routine  *****
 **************************************/

/*--------------------------------------------------------------------------
* USAGE:  <vision-system> -R
*
*
* pertinent environment: 	OSDPathName -- object space directory path,
* 				NDFPathName -- Network Dir. File path & name.
*
*     The Rebuilder scans the current network directory (defined by environment
* variable OSDPathName) and constructs a brand-new Network Directory File (NDF).
* o  a new net version is built, whose timestamp is the time at which the
*    rebuilder ran;  space versions constructed using the Container
*    Table found in the most recent Segment in each space directory;
*    space Role = "modified",
*
* o  The rebuilder's purpose is the creation of a new NDF for an existing
*    network (maybe the old NDF was lost or stolen!). It may also have 
*    grown too large.
*--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 *****  Routine to create a new network directory file for a network.  If
 *   "lookingForExistingSpaces" is true, it builds a network directory file
 *   which consists of Space Version Descriptors
 *   constructed by scanning the Container Table found in the most
 *   recent segment file of each space in the directory;  VISION will
 *   run fine on this NVD, Space Roles are set "modified". The NVD is
 *   marked as a base version.
 *
 *  If "lookingForExistingSpaces" is false, the NDF will have one NVD, with an
 *  empty Space Role Vector.
 *
 *  Arguments:
 *	networkDirectoryFilePath
 *				- the address of a string containing the path
 *				  name of the network directory file to be
 *				  created.
 *	newSpaceDirectoryPath	- the address of a string containing the path
 *				  name of the directory in which new spaces
 *				  associated with this network are to be
 *				  created.
 *	lookingForExistingSpaces- a boolean which, when true, instructs this
 *				  routine to look for existing spaces in the
 *				  new space directory.
 *  Returns:
 *	NOTHING
 *****/
PublicFnDef void PS_CreateNetwork (
    char const*			networkDirectoryFilePath,
    char const*			newSpaceDirectoryPath,
    int				lookingForExistingSpaces
)
{
    PS_NDH			ndh;
    PS_NVD			newNVD;

    if (IsNil (newSpaceDirectoryPath)) ERR_SignalFault (
	EC__UsageError, "Create Network Error: Object Space Directory not specified"
    );

/*****  Create the network directory file, ...  *****/
    NDFWriter* 			pNDF = new NDFWriter (networkDirectoryFilePath);

/*****  ... write a dummy network directory header, ...  *****/
    memset ((void *)&ndh, 0, sizeof (PS_NDH));
    WriteNDH		(pNDF, &ndh);
    pNDF->Synchronize	(networkDirectoryFilePath);

/*****  ... write the new space creation directory path name, ...  *****/
    PS_NVD_NewSpacePathName (newNVD) = WriteString (pNDF, newSpaceDirectoryPath);

/*****  ... create SRV for the new net version, ...  *****/
    CreateInitialSRV (
	pNDF,
	lookingForExistingSpaces,
	newSpaceDirectoryPath,
	PS_NVD_NewSpacePathName (newNVD),
	&PS_NVD_SRV (newNVD)
    );

/*****  ... initialize the rest of the initial NVD, ...  *****/
    PS_NVD_PreviousVersion	(newNVD) =
    PS_NVD_PreviousNVDChain	(newNVD) =
    PS_NVD_UpdateThread		(newNVD) =
    PS_NVD_AccessedVersion	(newNVD) =
    PS_NVD_TD			(newNVD) =
    PS_NVD_MP			(newNVD) =
    PS_NVD_Extensions		(newNVD) = PS_FO_Nil;

    PS_NVD_DirectoryVersion	(newNVD) = 0;
    PS_NVD_TID			(newNVD) = TransactionId;
    PS_NVD_Signature		(newNVD) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		(newNVD) = true;
    PS_NVD_UnusedFlags		(newNVD) = 0;
    PS_NVD_CommitTimeStamp	(newNVD) = CurrentTime ();
    PS_NVD_Creator		(newNVD) = CurrentUser ();
    PS_NVD_SoftwareVersion	(newNVD) = PS_SoftwareVersion;
    PS_NVD_Annotation (newNVD) = WriteString (
	pNDF, lookingForExistingSpaces ? "Initial Version After NDF Rebuild" : "Bootstrap"
    );

/*****  ... write the initial NVD, ...  *****/

    PS_NDH_CurrentNetworkVersion (ndh) = WriteNVD (pNDF, &newNVD);

/*****  ... initialize the rest of the NDH, ...  *****/
    PS_NDH_Signature		 (ndh) =
    PS_NDH_Signature2		 (ndh) = PS_NetworkDirectorySignature;
    PS_NDH_NDFVersion		 (ndh) = PS_NDFVersionNumber;
    PS_NDH_DirectoryVersion	 (ndh) = PS_NVD_DirectoryVersion (newNVD);
    PS_NDH_ModificationTimeStamp (ndh) = CurrentTime ();
    PS_NDH_LayoutType		 (ndh) = 1;
    PS_NDH_AlternateVersionChainHead (ndh) =
    PS_NDH_AlternateVersionChainMark (ndh) =
    PS_NDH_Generation		 (ndh) = 0;

/*****  ... re-write it, ...  *****/
    pNDF->ValidateChecksum ();
    WriteNDH		(pNDF, &ndh);
    pNDF->Synchronize	(networkDirectoryFilePath);

/*****  ... clean up, ...  *****/
    pNDF->Close ();
    delete pNDF;

/*****  ... and create an initial 'Global Update Rights List'.  *****/
    InitializeRightsList (networkDirectoryFilePath);
}


/***************************************
 ***************************************
 *****  External Update Interface  *****
 ***************************************
 ***************************************/

/***********************************
 *****  External Update Setup  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to output the current persistent address of a container for
 *****  use by the bulk update facility.
 *
 *  Arguments:
 *	this			- an ASD for the space of the container.
 *				  No output is produced if this argument is
 *				  'Nil'.
 *	xContainer		- the index of the container in the space.
 *	replace			- the boolean 'replacement' flag required by
 *				  the incorporation phase of the bulk updater.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_ASD::WriteDBUpdateInfo (unsigned int xContainer, bool replace) {
    char *spacePathName = SpacePathName ();
    char *p = strrchr (spacePathName, '/');
    if (IsntNil (p))
        *p = '\0';

    DBUPDATE_OutputHeaderRecord (
        spacePathName,
        m_xSpace,
        PS_ASD_RootSegment (this),
        xContainer,
        replace,
        m_pAND->NDFPathName (),
        m_pAND->UpdateFO ()
    );
}


/*******************************************
 *****  External Update Incorporation  *****
 *******************************************/

/*****************************************************************
 *  External Update Incorporation Specification Type Definition  *
 *****************************************************************/

class XUIS : public VTransient {
//  Construction
public:
    XUIS (char const *pSpecFilePathName);

//  Destruction
public:
    ~XUIS ();

//  Access
public:
    unsigned int spaceIndex () const {
	return m_xSpace;
    }
    unsigned int SegmentOrigin () const {
	return m_iSegmentOrigin;
    }
    unsigned int SegmentCount () const {
	return m_iSegmentCount;
    }

//  Segment Enumeration
public:
    void EnumerateExternalSegments (
	PS_ASD *pASD, void (PS_ASD::*componentProcessor) (char const *segmentPathName)
    );

//  State
protected:
    FILE* const		m_pFile;
    PS_Type_FO		m_iPathNamesFO;
    unsigned int	m_xSpace,
			m_iSegmentOrigin,
			m_iSegmentCount;
};


/**************************************************************************
 *  External Update Incorporation Specification Construction/Destruction  *
 **************************************************************************/

/*---------------------------------------------------------------------------*
 *  Argument:
 *	xuSpecPathName		- a string containing the path name of an
 *---------------------------------------------------------------------------*/
XUIS::XUIS (char const *xuSpecPathName) : m_pFile (fopen (xuSpecPathName, "r")) {
    if (IsNil (m_pFile)) ERR_SignalFault (
	EC__MError,
	UTIL_FormatMessage (
	    "Error Opening External Update Specification File: '%s'", xuSpecPathName
	)
    );

    char buffer [80];
    if (IsNil (fgets (buffer, sizeof buffer, m_pFile)) ||
	3 != STD_sscanf (
	    buffer, "XUISpecFile:%u:%u:%u", &m_xSpace, &m_iSegmentOrigin, &m_iSegmentCount
	)
    ) ERR_SignalFault (
	EC__MError, UTIL_FormatMessage (
	    "Not An External Update Specification File: '%s'", xuSpecPathName
	)
    );

    m_iPathNamesFO = ftell (m_pFile);
}

XUIS::~XUIS () {
    fclose (m_pFile);
}


/*********************************************************************
 *  External Update Incorporation Specification Segment Enumeration  *
 *********************************************************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to enumerate the external segments defined by a XUIS.
 *
 *  Arguments:
 *	pASD			- the 
 *	componentProcessor	- the address of an ASD member function called
 *				  with the address of a string containing
 *				  the path name of the segment.
 *  Returns:
 *	NOTHING
 *
 *****/
void XUIS::EnumerateExternalSegments (
    PS_ASD *pASD, void (PS_ASD::*componentProcessor) (char const *segmentPathName)
)
{
    TrapUnixError (
	fseek (m_pFile, m_iPathNamesFO, SEEK_SET) ? -1 : 0,
	"External Update Specification File Rewind"
    );

    for (unsigned int i = 0; i < m_iSegmentCount; i++) {
	char buffer[PathNameBufferSize];
	if (IsNil (fgets (buffer, sizeof buffer, m_pFile))) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "External Update Segment List Prematurely Exhausted"
	);

	char *nl = strchr (buffer, '\n');
	if (nl)
	    *nl = '\0';

	(pASD->*componentProcessor) (buffer);
    }
}


/**************************************
 *  External Segment Linking Routine  *
 **************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to link an external segment to a space.
 *
 *  Arguments:
 *	pathName		- a string containing the path name to be
 *				  linked.
 *	ap			- a <varargs.h> argument pointer positioned
 *				  at the following sequence of arguments:
 *				    - the ASD for the space being updated
 *				    - the address of a string containing the
 *				      path name of the space into which the
 *				      segments are to be linked.
 *
 *  Returns:
 *	Nothing
 *
 *****/
void PS_ASD::LinkExternalSegmentToSpace (char const *pathName) {
/*****  Move the segment into place, ...  *****/
    char const *pSegmentPathName = SegmentPathName (++m_xUpdateSegment);
    remove (pSegmentPathName);
    TrapUnixError (
	rename (pathName, pSegmentPathName), UTIL_FormatMessage (
	    "External Segment Link [%s->%s]", pathName, pSegmentPathName
	)
    );

/*****  Open the source file, ...  *****/
    m_xUpdateFD = open (pSegmentPathName, O_RDWR, 0);
    TrapUnixError (
	m_xUpdateFD, UTIL_FormatMessage ("External Segment Open '%s'", pSegmentPathName)
    );

/*****  ... set its transaction ID, ...  *****/
    PS_RID rid;
    PS_RID_Type	    (rid) = PS_ResourceType_IncorporatorSegment;
    PS_RID_Sequence (rid) = PS_ASD_RSequence (this)++;

    PS_SH segmentHeader;
    ReadSegmentHeader (m_xUpdateFD, &segmentHeader);

    PS_SH_TID	(&segmentHeader) = m_pAND->TID ();
    PS_SH_RID	(&segmentHeader) = rid;

    WriteSegmentHeader	(m_xUpdateFD, &segmentHeader);

/*****  ... flush it to disk and close it, ...  *****/
    SynchronizeAndCloseSegment ();

    if (m_pAND->ValidatingChecksums ())
	ValidateChecksum (pSegmentPathName);

#ifndef __VMS
/*****  ... change its permission, ...  *****/
    if (chmod (pSegmentPathName, MaskedPermission (SegmentCreationPermission)) < 0) {
	perror (
	    UTIL_FormatMessage (
		">>> Warning: Error Setting External Segment Permission '%s'",
		pSegmentPathName
	    )
	);
    }
#endif

/*****  ... register its existence, ...  *****/
    m_pAND->WriteSegmentJournalEntry (m_xSpace, m_xUpdateSegment, &segmentHeader);

    m_pUpdateSDC = CreateSDC (m_pUpdateSDC, m_xUpdateSegment, &segmentHeader);
}


/*---------------------------------------------------------------------------
 *****  Internal routine to link a collection of external segments to a space.
 *
 *  Arguments:
 *	xuis			- an XUIS naming the external segments to be
 *				  linked.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void PS_ASD::LinkExternalSegmentsToSpace (XUIS& rXUIS) {
/*****  ... initialize the ASD for update, ...  *****/
    m_xUpdateSegment = PS_ASD_RootSegment (this);
    m_pUpdateCT = m_pCT;
    m_pUpdateSDC = FreeSDCList (m_pUpdateSDC);

/*****  ... process the segments, ...  *****/
    rXUIS.EnumerateExternalSegments (this, &PS_ASD::LinkExternalSegmentToSpace);

/*****  ... and determine the address of the new container table.  *****/
    PS_SH header = AccessSegmentHeader (SpacePathName (), m_xUpdateSegment);
    PS_ASD_UpdateCTOffset (this) = PS_SH_CTOffset (&header);
}


/**************************************************
 *  Master External Update Incorporation Routine  *
 **************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to incorporate the result of a bulk data base update into a
 *****  network.
 *
 *  Arguments:
 *	xuSpecPathName		- the path name of an external update
 *				  specification file.
 *
 *  Returns:
 *	A new ASD for the space with which the update was associated.
 *	If the update failed, this routine will return 'Nil' and set
 *	'm_xUpdateStatus' to the appropriate 'PS_UpdateStatus'
 *	code.
 *
 *****/
PS_ASD *PS_AND::IncorporateExternalUpdate (char const *xuSpecPathName) {
/*****
 *  Access the external update specification and its associated space, ...
 *****/
    XUIS iXUIS (xuSpecPathName);

/*****
 *  ... validate that the external update is consistent with the current
 *	state of the space, ...
 *****/
    PS_ASD *asd = 0;
    if (!AccessSpace (asd, iXUIS.spaceIndex ()) || PS_ASD_RootSegment (asd) + 1 != (int)iXUIS.SegmentOrigin ()) {
	m_xUpdateStatus = PS_UpdateStatus_Inconsistent;
	return NilOf (PS_ASD*);
    }

/*****  ...  and perform the update, ...  *****/
    InitializePermissionMask ();

    ResetUpdateStatus	();
    ResetLockManager	();

    BeginRecoverableUpdateSection {
	do {
	    WaitForUngrantedLocks ();
	    LockNetwork ();
	    asd->ValidateSpaceConsistency ();
	    asd->LockSpace ();
	    UnlockNetwork ();
	} while (UngrantedLocksExist ());

	asd->LinkExternalSegmentsToSpace (iXUIS);

	CommitUpdate ();
	UnlockSpaces ();
    } EndRecoverableUpdateSection;

    return UpdateWasSuccessful () ? asd : NilOf (PS_ASD*);
}


/***********************************
 ***********************************
 *****  Module Initialization  *****
 ***********************************
 ***********************************/

PublicFnDef void PS_Initialize () {
    static int alreadyInitialized = false;
    char *			estring;
    unsigned int		evalue;

    if (alreadyInitialized)
	return;

/*---------------------------------------------------------------------------
 *****  Environment Variable Initialization
 *---------------------------------------------------------------------------
 */
    if (IsntNil (estring = getenv ("VisionLargeContainerSize")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) LargeContainerSize = evalue;
    /***** set a lower bound on LargeContainerSize *****/
    if (LargeContainerSize < 100) LargeContainerSize = 100;

    if (IsntNil (estring = getenv ("VisionMappingLimit")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) MappedSegmentLimit = evalue;

    if (IsntNil (estring = getenv ("VisionAddressThreshold")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) MappedAddressThreshold = (void const*)evalue;

    if (IsntNil (estring = getenv ("VisionNSyncRetries")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) NDFSyncRetries = evalue;

    if (IsntNil (estring = getenv ("VisionSSyncRetries")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) SegmentSyncRetries = evalue;

    if (IsntNil (estring = getenv ("VisionMaxSegSize")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) MaxSegmentSize = evalue;

    if (IsntNil (estring = getenv ("VisionMaxWriteChunk")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) MaxWriteChunk = evalue;

    if (IsntNil (estring = getenv ("VisionMaxCompSegs")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) MaxCompactionSegments = evalue;
/***** There must be at least 2 segments in the compaction *****/
    if (MaxCompactionSegments == 1) MaxCompactionSegments = 2;

    if (IsntNil (estring = getenv ("VisionCompactSmallSegs")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) CompactSmallSegments = true;

    if (IsntNil (estring = getenv ("VisionCommitsDisabled")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) CommitsDisabled = true;

    if (IsntNil (estring = getenv ("VisionSOpenRetries")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) SegmentOpenRetries = evalue;

    if (IsntNil (estring = getenv ("VisionSORetryDelay")) &&
	(evalue = (unsigned int)strtoul (estring, (char **)NULL, 0)) > 0
    ) SegmentOpenRetryDelay = evalue;

    if (IsntNil (estring = getenv ("VisionNetOpenTrace"))
    ) PS_TracingNetworkAccess = strtol (estring, (char **)NULL, 0) ? true : false;

    if (IsntNil (estring = getenv ("VisionCheckCheckSM"))
    ) PS_ValidatingChecksums = strtol (estring, (char **)NULL, 0) ? true : false;

/*---------------------------------------------------------------------------
 ***** Transaction id initialization code.
 *---------------------------------------------------------------------------
 */
    time_t timeval;
    time (&timeval);

    struct tm iTimeStruct;
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
    struct tm *timeparts = gmtime_r (static_cast<time_t*>(&timeval), static_cast<struct tm*>(&iTimeStruct));
#ifdef __VMS
#pragma __pointer_size __restore
#endif

    PS_TID_High		(TransactionId) = getpid () + (unsigned int)65536 * (
	timeparts->tm_yday + 366 * (timeparts->tm_year - 92)
    );
    PS_TID_Low		(TransactionId) = gethostid ();
    PS_TID_Sequence	(TransactionId) = 0;

/*---------------------------------------------------------------------------
 *****  Global Reclamation Manager Initialization
 *---------------------------------------------------------------------------
 */
    VkMemory::StartGRM (
	"batchvision",
	NilOf (char const *),
	(VkMemoryTraceCallback)IO_printf,
	(VkMemoryErrorCallback)SignalUnixError
    );
}


PublicFnDef void PS_SetMappingLimit (int limit) {
    if (limit > 0)
	MappedSegmentLimit = limit;
}

PublicFnDef void PS_SetAddressThreshold (unsigned int threshold) {
    MappedAddressThreshold = (void const*)threshold;
}

PublicFnDef void PS_SetNSyncRetries (int retries) {
    if (retries >= 0)
	NDFSyncRetries = retries;
}

PublicFnDef void PS_SetSSyncRetries (int retries) {
    if (retries >= 0)
	SegmentSyncRetries = retries;
}

PublicFnDef void PS_SetSOpenRetries (int retries) {
    if (retries >= 0)
	SegmentOpenRetries = retries;
}

PublicFnDef void PS_SetSORetryDelay (int delay) {
    if (delay >= 0)
	SegmentOpenRetryDelay = delay;
}

PublicFnDef void PS_SetNetOpenTrace (int onOff) {
    PS_TracingNetworkAccess = onOff ? true : false;
}

PublicFnDef void PS_SetMaxWriteChunk (unsigned int iMaxWriteChunk) {
    MaxWriteChunk =
	iMaxWriteChunk > INT_MAX ? INT_MAX :
	iMaxWriteChunk < 1	 ? 1	   :
	iMaxWriteChunk;
}

PublicFnDef void PS_DisableCommits (bool value) {
    if (value) CommitsDisabled = true;
}


/*******************************
 *******************************
 *****  Facility Services  *****
 *******************************
 *******************************/

/******************************
 *****  Debugger Methods  *****
 ******************************/

/***************************
 *  State Display Methods  *
 ***************************/

IOBJ_DefineUnaryMethod (DisplayParametersDM)
{
    IO_printf ("Environmental Parameters:\n");
    IO_printf ("-------------------------\n");
    IO_printf ("VisionLargeContainerSize = %08x %12d\n",
	       LargeContainerSize, LargeContainerSize);
    IO_printf ("VisionMappingLimit       = %08x %12d\n",
	       MappedSegmentLimit, MappedSegmentLimit);
    IO_printf ("VisionAddressThreshold   = %08x %12d\n",
	       MappedAddressThreshold, MappedAddressThreshold);
    IO_printf ("VisionNSyncRetries       = %08x %12d\n",
	       NDFSyncRetries, NDFSyncRetries);
    IO_printf ("VisionSSyncRetries       = %08x %12d\n",
	       SegmentSyncRetries, SegmentSyncRetries);
    IO_printf ("VisionMaxSegSize         = %08x %12d\n",
	       MaxSegmentSize, MaxSegmentSize);
    IO_printf ("VisionMaxCompSegs        = %08x %12d\n",
	       MaxCompactionSegments, MaxCompactionSegments);
    IO_printf ("VisionSOpenRetries       = %08x %12d\n",
	       SegmentOpenRetries, SegmentOpenRetries);
    IO_printf ("VisionSORetryDelay       = %08x %12d\n",
	       SegmentOpenRetryDelay, SegmentOpenRetryDelay);
    IO_printf ("\n");
    IO_printf ("VisionNetOpenTrace       = %s\n",
	       PS_TracingNetworkAccess ? "TRUE" : "FALSE");
    IO_printf ("VisionCheckCheckSM       = %s\n",
	       PS_ValidatingChecksums ? "TRUE" : "FALSE");
    IO_printf ("VisionCommitsDisabled    = %s\n",
	       CommitsDisabled ? "TRUE" : "FALSE");
    return self;
}

IOBJ_DefineUnaryMethod (DisplayCountsDM)
{
    IO_printf ("Successful Segment Mappings    = %u\n",
	       SuccessfulSegmentMaps);
    return self;
}

IOBJ_DefineUnaryMethod (ResetCountsDM)
{
    SuccessfulSegmentMaps = 0;
    return self;
}

/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    static int alreadyInitialized = false;

    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("displayParameters"	, DisplayParametersDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
	IOBJ_MDE ("resetCounts"		, ResetCountsDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (PS);
    FAC_FDFCase_FacilityDescription ("The Persistent Storage Manager");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    FAC_FDFCase_FacilityMD (methodDictionary);

    case FAC_DoStartupInitialization:
    /*****  Check for multiple initialization requests  *****/
	if (alreadyInitialized)
	{
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}

	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


#ifdef __VMS
/*************************
 *************************
 *****  VMS SUPPORT  *****
 *************************
 *************************/

namespace {
    bool vms_set_file_permissions(const char* unixname) {
	// translate from unix-style
	VString vsFilename;
	if (!V::OS::ConvertUnixFileNameToVms (vsFilename, unixname))
	    return false;

	char iFileName[255];
	size_t sFileName = vsFilename.length ();
	if (sFileName > sizeof (iFileName))
	    return false;
	memcpy (iFileName, vsFilename.content (), sFileName);

#pragma __pointer_size __save
#pragma __pointer_size 32
	FAB fab = cc$rms_fab;
	fab.fab$l_fna = static_cast<char*>(iFileName);
	fab.fab$b_fns = static_cast<unsigned char>(sFileName);
	fab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_UPD;

	XABPRO prot = cc$rms_xabpro;
	fab.fab$l_xab = static_cast<XABPRO*>(&prot);

	unsigned int sts = sys$open(static_cast<FAB*>(&fab));
	if (!(sts & 1))
	    return false;

	prot.xab$v_sys = prot.xab$v_own = prot.xab$v_grp = ~(XAB$M_NOREAD | XAB$M_NODEL);
	prot.xab$v_wld = ~XAB$M_NOREAD;

	sys$close (static_cast<FAB*>(&fab));
#pragma __pointer_size __restore
	return true;
    }
}
#endif // __VMS


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  ps.c 1 replace /users/mjc/system
  870215 22:56:26 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  ps.c 2 replace /users/mjc/system
  870326 19:28:02 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  ps.c 3 replace /users/cmm/system
  870519 16:11:49 cmm Release compactor

 ************************************************************************/
/************************************************************************
  ps.c 4 replace /users/mjc/translation
  870524 09:40:51 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  ps.c 5 replace /users/mjc/system
  870607 03:14:36 mjc Begin conversion to 'stdoss' utilities

 ************************************************************************/
/************************************************************************
  ps.c 6 replace /users/mjc/system
  870614 20:14:43 mjc Replaced UNIX specific include files with 'stdoss.h' equivalents

 ************************************************************************/
/************************************************************************
  ps.c 7 replace /users/cmm/system
  870821 11:02:37 cmm compactor: added omitlist, minseg formula

 ************************************************************************/
/************************************************************************
  ps.c 8 replace /users/cmm/system
  870823 12:49:42 cmm fixed segment unmapping bug

 ************************************************************************/
/************************************************************************
  ps.c 9 replace /users/cmm/system
  870824 13:21:42 cmm added min-saved-seg statistics printer in space compactor

 ************************************************************************/
/************************************************************************
  ps.c 10 replace /users/cmm/system
  870824 14:27:42 cmm ...more on the MSS problem

 ************************************************************************/
/************************************************************************
  ps.c 11 replace /users/cmm/system
  870825 13:33:32 cmm ...more on MSS trouble; this should do it (yeh, sure...)

 ************************************************************************/
/************************************************************************
  ps.c 12 replace /users/mjc/system
  870924 16:04:47 mjc Added 'MSS<space>' environment variable to tune compactor operation

 ************************************************************************/
/************************************************************************
  ps.c 13 replace /users/cmm/system
  871006 14:43:18 cmm added stats-only option to compactor, initial MSS back to orig rt. seg of previous SVD, mjc's MSS override

 ************************************************************************/
/************************************************************************
  ps.c 14 replace /users/cmm/system
  871009 13:25:51 cmm fixed incorrect init. of SVD previousSVDFO field in Commitupdate(), reorganize GURL check wrt local and global updates

 ************************************************************************/
/************************************************************************
  ps.c 15 replace /users/jck/system/tmp
  880127 11:27:44 jck Fixed Compactor Bug

 ************************************************************************/
/************************************************************************
  ps.c 16 replace /users/jck/system/tmp
  880211 15:39:17 jck Concurrency Control bug in Private saves

 ************************************************************************/
/************************************************************************
  ps.c 17 replace /users/jad/system/LINK
  880314 14:04:12 jad Include more version info -> NDF Version 2

 ************************************************************************/
/************************************************************************
  ps.c 18 replace /users/jad/system/LINK
  880314 15:45:34 jad fix typo with versions

 ************************************************************************/
/************************************************************************
  ps.c 19 replace /users/jck/system
  880315 14:38:08 jck ps.d

 ************************************************************************/
/************************************************************************
  ps.c 20 replace /users/jad/system
  880329 10:47:12 jad fixed typo in the compactor output 'MMS' to 'MSS'

 ************************************************************************/
/************************************************************************
  ps.c 21 replace /users/jck/system
  880330 22:01:18 jck Installing new standard interface

 ************************************************************************/
/************************************************************************
  ps.c 22 replace /users/jck/system
  880407 11:01:26 jck Cleaned Up VAX interface

 ************************************************************************/
/************************************************************************
  ps.c 23 replace /users/m2/dbup
  880421 17:39:53 m2 Apollo port

 ************************************************************************/
/************************************************************************
  ps.c 24 replace /users/mjc/Workbench/portbackend
  880515 12:23:01 mjc Installed cross disk network support

 ************************************************************************/
/************************************************************************
  ps.c 25 replace /users/mjc/Workbench/port/backend
  880519 23:01:30 mjc Convert NDF access to use file server calls

 ************************************************************************/
/************************************************************************
  ps.c 26 replace /users/mjc/Workbench/port/backend
  880521 17:25:26 mjc Fixed compactor logic bug exposed on Apollo, made some minor code reorganizations

 ************************************************************************/
/************************************************************************
  ps.c 27 replace /users/jck/system
  880526 14:38:40 jck Interfaced with the new Apollo lock server

 ************************************************************************/
/************************************************************************
  ps.c 28 replace /users/jck/system
  880915 12:12:51 jck Modify lock-set acquisition protocol

 ************************************************************************/
/************************************************************************
  ps.c 29 replace /users/m2/backend
  890503 15:25:46 m2 STD_printf(), memallc(), lint fixes

 ************************************************************************/
/************************************************************************
  ps.c 30 replace /users/m2/backend
  890828 17:11:09 m2 Changed scanf() of %u to %d, memset bug, ValidateContainerFraming, NDF_ stuff

 ************************************************************************/
