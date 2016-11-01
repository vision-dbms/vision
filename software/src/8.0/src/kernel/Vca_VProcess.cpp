/*****  Vca_VProcess Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

// ... needed for 'waitpid' on Unix derived systems:
#include "VpSocket.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VProcess.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::VProcess  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VProcess::VProcess (Info const &rProcessInfo)
    : m_pIProcess (this)
    , m_iProcessInfo (rProcessInfo)
    , m_bTerminatingOnClosedStdio (false)
    , m_bNoStdin (false)
    , m_bNoStdout (false)
    , m_bNoStderr (false)
{
    traceInfo ("Creating VProcess");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VProcess::~VProcess () {
    traceInfo ("Destroying VProcess");
#if defined(__VMS)

#elif defined(_WIN32)
    if (m_iProcessInfo.dwProcessId != 0) {
	CloseHandle (m_iProcessInfo.hProcess);
	CloseHandle (m_iProcessInfo.hThread);
    }
#else
    if (m_iProcessInfo != 0) {
	int result;
	waitpid (m_iProcessInfo, &result, WNOHANG);
    }
#endif
}


/******************************
 ******************************
 *****  IProcess Methods  *****
 ******************************
 ******************************/

void Vca::VProcess::WaitForTermination (IProcess *pRole, IVReceiver<S32> *pResultClient) {
    if (pResultClient) {
	int iResult;
	waitForTermination (&iResult);
	pResultClient->OnData (iResult);
    }
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VProcess::onClosedStdio () {
    if (m_bTerminatingOnClosedStdio && m_bNoStdin && m_bNoStdout && m_bNoStderr) {
#if defined(__VMS)

#pragma __pointer_size __save
#pragma __pointer_size __short
	unsigned int hProcess = m_iProcessInfo;
	unsigned int xStatus =  sys$delprc (static_cast<unsigned int*>(&hProcess), 0, 0);
#pragma __pointer_size __restore

#elif defined(_WIN32)
#else
	kill (m_iProcessInfo, SIGTERM);
#endif
    }
}

void Vca::VProcess::onClosedStdin () {
    m_bNoStdin = true;
    onClosedStdio ();
}

void Vca::VProcess::onClosedStdout () {
    m_bNoStdout = true;
    onClosedStdio ();
}

void Vca::VProcess::onClosedStderr () {
    m_bNoStderr = true;
    onClosedStdio ();
}

void Vca::VProcess::waitForTermination (int *pResult) {
#if defined(__VMS)
    *pResult = -1;
#elif defined(_WIN32)
    if (WAIT_OBJECT_0 == WaitForSingleObject (m_iProcessInfo.hProcess, INFINITE)
    &&	GetExitCodeProcess (m_iProcessInfo.hProcess, (DWORD*)pResult)
    )	return;
    *pResult = -1;
#else
    waitpid (m_iProcessInfo, pResult, 0);
#endif
}


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

char **Vca::VProcess::Command::setTo (char const *pCommandLine) {
//  Parse the command line, ...
    unsigned int sArgVector = 0;
    bool bTokenInProgress = false;
    bool bStringInProgress = false;
    char const *commandp = pCommandLine;
    char *pBuffer = m_iArgStorage.storage (pCommandLine ? strlen (pCommandLine) + 1 : 1);
    do {
	switch (*commandp) {
	case ' ':
	case '\t':
	case '\n':
	case '\0':
	    commandp += strspn (commandp, " \t\n");
	    if (bTokenInProgress) {
		bTokenInProgress = false;
		*pBuffer++ = '\0';
	    }
	    break;
	case '"':
	    if (!bTokenInProgress) {
		bTokenInProgress = true;
		sArgVector++; //argv[argc++] = pBuffer;
	    }
	    commandp++;
	    bStringInProgress = true;
	    while (bStringInProgress) {
		size_t span = strcspn (commandp, "\"\\");

		strncpy (pBuffer, commandp, span);
		commandp += span;
		pBuffer  += span;
		switch (*commandp) {
		case '\\':
		    if (*++commandp == '\0')
			break;
		    *pBuffer++ = *commandp++;
		    break;
		case '"':
		    commandp++;
		    bStringInProgress = false;
		    break;
		default:
		    bStringInProgress = false;
		    break;
		}
	    }
	    break;
	case '\\':
	    if (*++commandp == '\0')
		break;
	default:
	    if (!bTokenInProgress) {
		bTokenInProgress = true;
		sArgVector++; //argv[argc++] = pBuffer;
	    }
	    *pBuffer++ = *commandp++;
	    break;
	}
    } while (*commandp);
    *pBuffer = '\0';

//  ... and build the argument vector:
    m_iArgVector.Guarantee (sArgVector + 1);

    pBuffer = m_iArgStorage.storage ();
    for (unsigned int xArgument = 0; xArgument < sArgVector; xArgument++) {
	m_iArgVector[xArgument] = pBuffer;
	pBuffer += strlen (pBuffer) + 1;
    }
    m_iArgVector[sArgVector] = NilOf (char*);

    return argv ();
}
