/*****  VSimpleFile Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/
#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VSimpleFile.h"

/************************
 *****  Supporting  *****
 ************************/

#define VMS_LINUX_EXPLICIT_COMPAT_FILENAMES 1
#include "VConfig.h"

#include "V_VArgList.h"
#include "V_VString.h"

/***********************
 *****  Constants  *****
 ***********************/

#if defined(_WIN32)

char const * const V::VSimpleFile::g_pTextReadMode = "rt";
char const * const V::VSimpleFile::g_pTextWriteMode = "wt";
char const * const V::VSimpleFile::g_pTextAppendMode = "a+t";

#else

char const * const V::VSimpleFile::g_pTextReadMode = "r";
char const * const V::VSimpleFile::g_pTextWriteMode = "w";
char const * const V::VSimpleFile::g_pTextAppendMode = "a";

#endif

/* VMS:
 * fopen is redefined to include "shr=get" parameter to allow external reads when a file is in use (write/append)
 * (A file being opened in write/append mode in VMS is not readable by others by default.
 */

#if defined(__VMS)
#define fopen(filename_char_ptr, mode_char_ptr) \
        fopen(filename_char_ptr, mode_char_ptr, "shr=get")  
#endif

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool V::VSimpleFile::Open (char const *pFileName, char const *pFileMode) {
    close ();
#if defined (__linux__) && defined (VMS_LINUX_EXPLICIT_COMPAT)
    String iFileName(FileSpecTranslation::translateVMSFileSpec (pFileName));
    pFileName = iFileName.c_str();
#endif

    m_pStream = fopen (pFileName, pFileMode);
    if (m_pStream)
	V::SetCloseOnExec (fileno (m_pStream));
    return IsntNil (m_pStream);
}

bool V::VSimpleFile::OpenUniqueTemp (VString& rFileName, char const *pPrefix) {
    close ();
#if defined (__linux__) && defined (VMS_LINUX_EXPLICIT_COMPAT)
    String iFileName(FileSpecTranslation::translateVMSFileSpec (pPrefix));
    pPrefix = iFileName.c_str();
#endif

#if defined (__linux__)
    VString fileName(pPrefix);
    fileName << ".XXXXXX";
    int fd = -1;
    m_pStream = NULL;

    if (((fd = mkstemp(fileName.storage())) == -1) ||
           ((m_pStream = fdopen(fd, g_pTextWriteMode)) == NULL)) {

        // shared cleanup code
        if (fd != -1) {
            unlink(fileName.storage());
            ::close(fd);
        }
        m_pStream = NULL; // set to closed state
    }
    if (m_pStream) {
	V::SetCloseOnExec (fileno (m_pStream));
        rFileName = fileName;
    }
    return IsntNil (m_pStream);
#else
    return false;
#endif
}

bool V::VSimpleFile::GetLine (VString &rLine) const {
    if (IsNil (m_pStream))
	return false;

    bool bGotSomething = false;
    char iInputBuffer[1024];
    while (fgets (iInputBuffer, sizeof (iInputBuffer), m_pStream)) {
	bGotSomething = true;
	if (char *pNewLine = strchr (iInputBuffer, '\n')) {
	    rLine.append (iInputBuffer, pNewLine - iInputBuffer);
	    break;
	}
	rLine.append (iInputBuffer);
    }
    return bGotSomething;
}

bool V::VSimpleFile::GetContents (VString &rContents) const {
    if (IsNil (m_pStream))
	return false;

//  First try to read everything in one preallocated gulp...
    unsigned int const sFile = Size ();
    size_t const sPrefix = rContents.length ();
    char * const pStorage = rContents.storage (sPrefix + sFile);
    size_t const sRead = fread (pStorage + sPrefix, sizeof (char), sFile, m_pStream);
    pStorage[sPrefix + sRead] = '\0';

//  ... finishing up with a loop if necessary:
    char iInputBuffer[1024];
    while (fgets (iInputBuffer, sizeof (iInputBuffer), m_pStream)) {
        rContents << iInputBuffer;
    }
    return true;
}


bool V::VSimpleFile::PutLine (char const *pLine) const {
    if (IsNil (m_pStream))
	return false;

    STD_fprintf (m_pStream, "%s\n", pLine);

    return true;
}

bool V::VSimpleFile::PutString (char const *pString) const {
    if (IsNil (m_pStream) || IsNil (pString))
	return false;

    return fputs (pString, m_pStream) >= 0;
}

int __cdecl V::VSimpleFile::printf (char const *pFormat, ...) const {
    V_VARGLIST (pFormatArgs, pFormat);
    return vprintf (pFormat, pFormatArgs);
}

int V::VSimpleFile::vprintf (char const *pFormat, va_list pFormatArgs) const {
    V::VArgList iFormatArgs (pFormatArgs);
    int const cBytesTransfered = m_pStream ? STD_vfprintf (m_pStream, pFormat, iFormatArgs) : 0;
    if (m_bAutoFlushEnabled)
	flush ();
    return cBytesTransfered;
}

void V::VSimpleFile::flush () const {
    if (m_pStream)
	fflush (m_pStream);
}

void V::VSimpleFile::close () {
    if (m_pStream) {
    	fclose (m_pStream);
	m_pStream = 0;
    }
}

unsigned int V::VSimpleFile::Size () const {
    unsigned int size = 0;
    if (m_pStream) {
	int handle = fileno (m_pStream);
	struct stat stats;
        if (fstat (handle, &stats) >= 0)
	    size = (size_t)stats.st_size;
    }
    return size;
}

/******************
 ******************
 *****  Dup2  *****
 ******************
 ******************/

int V::VSimpleFile::dup2 (int filedes2) const {
    if (m_pStream)
	return ::dup2 (fileno (m_pStream), filedes2);
    return -1;
}
