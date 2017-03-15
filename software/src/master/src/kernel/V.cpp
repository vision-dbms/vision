/*****  V/V.cpp  *****/

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

#include "V.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VConfig.h"
#if defined (__linux__) && defined (VMS_LINUX_EXPLICIT_COMPAT)
#include "get_logical.h"
#endif

#include "V_VString.h"
#include "V_VThreadSpecific.h"
#include "V_VThread.h"	//  Windows braindamage


/********************************
 ********************************
 *****  Environment Access  *****
 ********************************
 ********************************/

bool V::GetEnvironmentBoolean (char const *pName, bool bDefaultValue) {
    return GetEnvironmentValue (pName, bDefaultValue);
}
int V::GetEnvironmentInteger (char const* pName, int iDefaultValue) {
    return GetEnvironmentValue (pName, iDefaultValue);
}
char const* V::GetEnvironmentString  (VString &rResult, char const* pName, char const* pDefaultValue) {
    return GetEnvironmentValue (rResult, pName, pDefaultValue);
}
unsigned int V::GetEnvironmentUnsigned (char const *pName, unsigned int iDefaultValue) {
    return GetEnvironmentValue (pName, iDefaultValue);
}

bool V::GetEnvironmentValue (char const *pName, bool bDefaultValue) {
    char const *const pValue = getenv (pName);
    return !pValue
	? bDefaultValue
	: strcasecmp (pValue, "TRUE" ) == 0 || strcasecmp (pValue, "ON" ) == 0
	? true
	: strcasecmp (pValue, "FALSE") == 0 || strcasecmp (pValue, "OFF") == 0
	? false
	: atoi (pValue) != 0;
}

char const* V::GetEnvironmentValue (VString &rResult, char const* pName, char const* pDefaultValue) {
    rResult = getenv (pName);
#if defined (__linux__) && defined (VMS_LINUX_EXPLICIT_COMPAT)
    if (0 == rResult.length ()) {
	char iBuffer[512];
	if (get_logical (pName, iBuffer, sizeof (iBuffer)) > 0)
	    rResult = iBuffer;
    }
#endif
    return rResult.length () > 0 ? rResult : rResult = pDefaultValue;
}

int V::GetEnvironmentValue (char const* pName, int iDefaultValue) {
    char const* const pValue = getenv (pName);
    return pValue ? atoi (pValue) : iDefaultValue;
}

double V::GetEnvironmentValue (char const *pName, double iDefaultValue) {
    char const *const pValue = getenv (pName); double iValue;
    return pValue && 1 == sscanf (pValue, " %lg", &iValue) ? iValue : iDefaultValue;
}

unsigned int V::GetEnvironmentValue (char const *pName, unsigned int iDefaultValue) {
    char const *const pValue = getenv (pName); unsigned int iValue;
    return pValue && 1 == sscanf (pValue, " %u", &iValue) ? iValue : iDefaultValue;
}


/**********************************
 **********************************
 *****  Exit Code Management  *****
 **********************************
 **********************************/

#ifdef __VMS
#include <stsdef.h>

static int g_xExitCode = SS$_NORMAL;
static int g_xExitErrorCode = STS$K_ERROR;
#else
static int g_xExitCode = 0;
static int g_xExitErrorCode = 1;
#endif

int V::ExitCode () {
    return g_xExitCode;
}

int V::ExitErrorCode () {
    return g_xExitErrorCode;
}

int V::SetExitCodeTo (int xExitCode) {
    int xOldExitCode = g_xExitCode;
    g_xExitCode = xExitCode;
    return xOldExitCode;
}

int V::SetExitCodeToError () {
    return SetExitCodeTo (g_xExitErrorCode);
}

int V::SetExitErrorCodeTo (int xExitCode) {
    int xOldExitCode = g_xExitErrorCode;
    g_xExitErrorCode = xExitCode;
    return xOldExitCode;
}


/*********************
 *********************
 *****  Utility  *****
 *********************
 *********************/

bool V::SetCloseOnExec (unsigned int fileHandle) {
#if defined(_WIN32)	
    return SetHandleInformation (
	(HANDLE) _get_osfhandle (fileHandle), HANDLE_FLAG_INHERIT, 0
    ) != 0;
#else
    return fcntl (fileHandle, F_SETFD, FD_CLOEXEC) != -1;
#endif
}


/*********************
 *********************
 *****  DllMain  *****
 *********************
 *********************/

#if defined(_WIN32)

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD xCallReason, LPVOID lpReserved) {
    switch (xCallReason) {
    case DLL_PROCESS_ATTACH:
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	V::VThreadSpecific::Key::OnThreadTermination ();
	break;
    case DLL_PROCESS_DETACH:
	break;
    }
    return true;
}

#endif
