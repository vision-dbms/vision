/*****  VTransientServices Implementation  *****/

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

#include "VTransientServices.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "V_VTime.h"

#if defined(_WIN32)
static char const *const g_pNullDeviceName = "NUL:";
#elif defined(__VMS)
static char const *const g_pNullDeviceName = "NL:";
extern "C" {
    void *decc$malloc (size_t sAllocation);
    void *decc$realloc (void *pMemory, size_t sMemory);
    void *decc$free (void *pMemory);
}
#else
static char const *const g_pNullDeviceName = "/dev/null";
#endif


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTransientServices::VTransientServices () 
    : m_bIsBackground   (false) 
{
    attachTransientServicesProvider (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VTransientServices::~VTransientServices () {
    detachTransientServicesProvider (this);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void VTransientServices::updateBackgroundSwitch (bool bIsBackground) {
    if (bIsBackground)
	createDaemon ();
}

 
/*******************************
 *******************************
 *****  Memory Management  *****
 *******************************
 *******************************/

void *VTransientServices::allocate (size_t cBytes) {
#if !defined(__VMS) || !defined(_DEBUG)
    void *const pResult = ::malloc (cBytes);
#else
    void *const pResult = decc$malloc (cBytes);
#endif

    if (pResult)
	return pResult;

    raiseApplicationException ("Error Allocating %u Bytes", cBytes);

    return pResult;
}

void *VTransientServices::reallocate (void *pMemory, size_t cBytes) {
#if !defined(__VMS) || !defined(_DEBUG)
    void *const pResult = ::realloc (pMemory, cBytes);
#else
    void *const pResult = decc$realloc (pMemory, cBytes);
#endif

    if (pResult)
	return pResult;

    raiseApplicationException (
	"Error Reallocating Memory At %p To %u Bytes", pMemory, cBytes
    );

    return pResult;
}

void VTransientServices::deallocate (void *pMemory) {
#if !defined(__VMS) || !defined(_DEBUG)
    ::free (pMemory);
#else
    decc$free (pMemory);
#endif
}


/********************************
 ********************************
 *****  Message Formatting  *****
 ********************************
 ********************************/

/***********************
 *****  'display'  *****
 ***********************/

int VTransientServices::vdisplay (char const* pMessage, va_list pArgList) {
    V::VArgList iArgList (pArgList);
    int const sWrite = ::vfprintf (stdout, pMessage, iArgList);
    fflush (stdout);
    return sWrite;
}


/*************************************
 *****  Message Buffer Services  *****
 *************************************/
/*---------------------------------------------------------------------------
 * Message Buffer Services provides a pool of message buffers which are
 * automatically recycled by message buffer services.  Message buffers are
 * allocated in two ways - as a request for a buffer of a specific size
 * (GetNextMessageBuffer) and automatically by (VTransientServices::vstring).
 * Message buffers will cannot be larger than 'MessageBufferSize'.  Requests
 * for more than 'MessageBufferSize' bytes will be truncated to
 * 'MaxMessageBufferSize'.  As currently implemented, a 'MaxMessageBufferSize'
 * character buffer will be allocated by 'UTIL_FormatMessage'.
 *
 * Note that message buffers are automatically reused without warning.  The
 * only guarantee associated with their use is that they will be unchanged for
 * some SHORT period of time after their allocation.
 *---------------------------------------------------------------------------
 */

/*****  Message Buffer Pool Declarations  *****/
#define MessageBufferSize   256
#define MessageBufferCount  8
PrivateVarDef char MessageBufferPool[MessageBufferCount][MessageBufferSize];
PrivateVarDef unsigned int NextFreeMessageBuffer = 0;

/*---------------------------------------------------------------------------
 *****  Routine to obtain a temporary character string buffer
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	The address of the buffer.  Note that this buffer will eventually be
 *	reclaimed for other uses.
 *
 *****/
PrivateFnDef char *GetNextMessageBuffer () {
    char *result = MessageBufferPool[NextFreeMessageBuffer];
    NextFreeMessageBuffer = (NextFreeMessageBuffer + 1) % MessageBufferCount;

    return result;
}

/*---------------------------------------------------------------------------
 *****  Routine to format a message in a temporary buffer.
 *
 *  Arguments:
 *	Identical to 'printf'
 *
 *  Returns:
 *	The address of the buffer in which the message was formatted.  Note
 *	that this buffer will eventually be reclaimed for other uses.
 *
 *****/
char const *VTransientServices::vstring (char const* pMessage, va_list pArgList) {
    char *const pBuffer= GetNextMessageBuffer ();
    V::VArgList iArgList (pArgList);
    STD_vsprintf (pBuffer, pMessage, iArgList);
    return pBuffer;
}


/**********************************
 **********************************
 *****  Resource Reclamation  *****
 **********************************
 **********************************/

bool VTransientServices::garbageCollected () {
    return false;
}

bool VTransientServices::mappingsReclaimed () {
    return false;
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VTransientServices::vraiseApplicationException (
    char const *pMessage, va_list pArgList
) {
    fflush (stdout);
    fprintf (stderr, ">>> %s\n", vstring (pMessage, pArgList));
    exit (ErrorExitValue);
}

void VTransientServices::vraiseSystemException (
    char const *pMessage, va_list pArgList
) {
    vraiseApplicationException (pMessage, pArgList);
}

void VTransientServices::vraiseUnimplementedOperationException (
    char const *pMessage, va_list pArgList
) {
    raiseApplicationException (
	"Unimplemented Operation: %s", vstring (pMessage, pArgList)
    );
}

void VTransientServices::vraiseException (
    ERRDEF_ErrorCode xErrorCode, char const *pMessage, va_list pArgList
) {
    raiseApplicationException (
	">>> Error #%d: %s\n", xErrorCode, vstring (pMessage, pArgList)
    );
}

/*********************
 *********************
 *****  Logging  *****
 *********************
 *********************/

void VTransientServices::log (char const *pFormat, ...) {
    V_VARGLIST (ap, pFormat);
    m_iLogFile.vlog (pFormat, ap);
}
void VTransientServices::usageLogFilePath (VString &rLogFilePath) const {
}
void VTransientServices::usageLogDelimiter (VString &rLogDelimiter) const {
}
bool VTransientServices::usageLogSwitch () const {
    return false;
}
unsigned int VTransientServices::usageLogFileSize () const {
    return 0;
}
unsigned int VTransientServices::usageLogFileBackups () const {
    return 0;
}

void VTransientServices::updateUsageLogFile () {
}
void VTransientServices::updateUsageLogSwitch (bool bSwitch) {
}
void VTransientServices::updateUsageLogFilePath (char const *pPath) {
}
void VTransientServices::updateUsageLogSize (unsigned int iSize) {
}
void VTransientServices::updateUsageLogBackups (unsigned int iBackups) {
}
void VTransientServices::updateUsageLogTag (char const *pTag) {
}
void VTransientServices::updateUsageLogDelimiter (char const *pDelimiter) {
}
void VTransientServices::logUsageInfo (char const *pFormat, ...) {
}
void VTransientServices::vlogUsageInfo (char const *pFormat, va_list ap){
}



/*****************************
 *****************************
 *****  Daemon Creation  *****
 *****************************
 *****************************/

/********
 * Note: Daemon Creation in Unix 
 *    1. Fork and exit the parent. The child is now guaranteed not to be a 
 *       Process Group Leader 
 *    2. Make this child a session leader/process group leader using setsid () 
 *       to lose the controlling terminal
 *    3. Fork again and exit the parent to make the new child a non-session 
 *       group leader (to prevent the child from reacquiring the controlling 
 *       terminal) 
 *    4. Change the current working directory for the daemon to "/tmp" and unset 
 *       the masking attributes
 *    5. Dup stdin to null device, stderr, stdout to log file 
 ****/

bool VTransientServices::createDaemon () {
    if (m_bIsBackground)
	return true;

#if !defined(__VMS) && !defined (_WIN32)
    switch (fork ()) {
    case 0:
	break;
    case -1:
	return false;
    default:
	_exit (0);
    }
    if (setsid () < 0)
	return false; 

    switch (fork ()) {
    case 0:
//	chdir ("/tmp");
//	umask (0);
	break;
    case -1:
	return false;
    default:
	_exit (0);
    }
    m_bIsBackground = true;
#endif
    redirect_stdin ();
    redirect_stdout_stderr ();
    return true;
}

/****
 * If running in background mode close the stdin and redirect it to 
 * the null device
 ****/

void VTransientServices::redirect_stdin () {
    if (isBackground ()) {
	int xNullDeviceHandle = open (g_pNullDeviceName, O_RDONLY, 0);

	//  close stdin and bind null-device to stdin
	dup2 (xNullDeviceHandle, 0);
	V::SetCloseOnExec (0);

	close (xNullDeviceHandle);
    }
}

/****
 * If running in background mode redirect the stdout and stderr to 
 * the application's current log file (if it is valid)
 ****/

void VTransientServices::redirect_stdout_stderr () {
    if (isBackground ()) {
	m_iLogFile.redirectStdOutToLog ();
	m_iLogFile.redirectStdErrToLog ();
    }
}
