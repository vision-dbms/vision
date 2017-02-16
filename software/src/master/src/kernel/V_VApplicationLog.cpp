/*****  VApplicationLog Implementation  *****/

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

#include "V_VApplicationLog.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "V_VTime.h"


#if defined(_WIN32)
static char const *const g_pNullDeviceName = "NUL:";
#elif defined(__VMS)
static char const *const g_pNullDeviceName = "NL:";
#else
static char const *const g_pNullDeviceName = "/dev/null";
#endif


V::VApplicationLog::VApplicationLog ()
    : m_bLogSwitch		(false)
    , m_bRedirectingStdOut	(false)
    , m_bRedirectingStdErr	(false)
    , m_iLogFilePath		(g_pNullDeviceName, false)
    , m_iLogFileSize		(1)
    , m_iLogFileBackups		(0)
    , m_iLogTag			()
    , m_iLogDelimiter		(" ", false) 
{
}

void V::VApplicationLog::log (char const *pFormat, ...) {
  if (m_bLogSwitch) {
    V_VARGLIST (ap, pFormat);
    vlog (pFormat, ap);
  }
}

void V::VApplicationLog::vlog (char const *pFormat, va_list ap) {
  if (m_bLogSwitch) {
	initLogFile ();
	if (logFileBackups () > 0 && logFileExceedsSize ()) 
	    rotateLogFiles ();

	V::VTime iNow; 
        VString iTimestring;
	iNow.asString (iTimestring);
        m_iLogFile.printf ("%s%s", iTimestring.content (), m_iLogDelimiter.content ());
        m_iLogFile.vprintf (pFormat, ap);
	m_iLogFile.printf ("%s%s\n", m_iLogDelimiter.content (), m_iLogTag.content ());
	m_iLogFile.flush ();
  }
}

void V::VApplicationLog::initLogFile () {
    if (m_iLogFile.isntOpen ()) {
	if (!m_iLogFile.OpenForTextAppend (m_iLogFilePath))
	    m_iLogFile.OpenForTextAppend (g_pNullDeviceName);
    }
}

void V::VApplicationLog::rotateLogFiles () {

    //  close current log file
    m_iLogFile.flush ();
    m_iLogFile.close ();
    initLogFile ();
    if (logFileExceedsSize ()) {
	m_iLogFile.flush ();
	m_iLogFile.close ();
	//  if redirecting, temporarily dupe 0 (null-dev in background) to stdout/stderr,
	//  so that stdout/stderr dont point to the log-file, enabling renaming

	if (redirectingStdOut ()) dup2 (0, 1);
	if (redirectingStdErr ()) dup2 (0, 2);

	unsigned int xIndex = logFileBackups ();
	VString iLog;
	logFilePath (iLog);

	//  rotate
	while (xIndex > 0) {
	    VString iRecentLog (iLog), iOldLog (iLog);
	    iOldLog << "-" << xIndex;
	    if (xIndex-1 > 0)
		iRecentLog << "-" << xIndex-1;

	    VSimpleFile iFile;
	    if (iFile.OpenForTextRead (iRecentLog)) {
		iFile.close ();
		remove (iOldLog.content ());
		rename (iRecentLog.content (), iOldLog.content ());
	    }
	    xIndex--; 
	}
	initLogFile ();
    }
    if (redirectingStdOut ()) redirectStdOutToLog ();
    if (redirectingStdErr ()) redirectStdErrToLog ();
}

void V::VApplicationLog::redirectStdOutToLog () {
    initLogFile ();

    //   dup logfile desc to stdout, set its close-on-exec flag
    m_iLogFile.dup2 (1);
    V::SetCloseOnExec (1);

    m_bRedirectingStdOut = true;
}

void V::VApplicationLog::redirectStdErrToLog () {
    initLogFile ();

    //   dup logfile desc to stdout, set its close-on-exec flag
    m_iLogFile.dup2 (2);
    V::SetCloseOnExec (2);

    m_bRedirectingStdErr = true;
}

