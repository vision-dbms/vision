#ifndef VApplicationLogger_Interface
#define VApplicationLogger_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VSimpleFile.h"
#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VApplicationLog {

    //  Construction
    public:
	VApplicationLog ();

    //  Destruction
    public:
	~VApplicationLog () {
	}
    //  Access
    public:
	void logFilePath (VString &rLogFilePath) const {
	    rLogFilePath = m_iLogFilePath;
	}
	void logDelimiter (VString &rLogDelimiter) const {
	    rLogDelimiter = m_iLogDelimiter;
	}
	bool logSwitch () const {
	    return m_bLogSwitch;
	}
	bool redirectingStdOut () const {
	    return m_bRedirectingStdOut;
	}
	bool redirectingStdErr () const {
	    return m_bRedirectingStdErr;
	}
	unsigned int logFileSize () const {
	    return m_iLogFileSize;
	}
	unsigned int logFileBackups () const {
	    return m_iLogFileBackups;
	}

    //  Update
    public:
	void updateLogFile () {
	    m_iLogFile.close ();
	    initLogFile ();
	}
	void updateLogSwitch (bool bSwitch) {
	    m_bLogSwitch = bSwitch;
	}
	void updateLogFilePath (char const *pPath) {
	    m_iLogFilePath.setTo (pPath);
	}
	void updateLogFileSize (unsigned int iSize) {
	    m_iLogFileSize = iSize;
	}
	void updateLogFileBackups (unsigned int iBackups) {
	    m_iLogFileBackups = iBackups;
	}
	void updateLogTag (char const *pTag) {
	    m_iLogTag.setTo (pTag);
	}
	void updateLogDelimiter (char const *pDelimiter) {
	    m_iLogDelimiter.setTo (pDelimiter);
	}
    public:
	void redirectStdOutToLog ();
	void redirectStdErrToLog ();

    //  Query
    private:
	bool logFileExceedsSize () const {
	    //  logFileSize () returns size in Megabytes
	    return m_iLogFile.Size () >= logFileSize () * 1024 * 1024;
	}

    //  LogFile Management
    private:
	void initLogFile   ();
	void rotateLogFiles ();


    //  Log Routines
    public:
	void log (char const *pFormat, ...);
	void vlog (char const *pFormat, va_list ap);


    //  State
    private:
	bool		m_bLogSwitch;
	bool		m_bRedirectingStdOut;
	bool		m_bRedirectingStdErr;
	VString		m_iLogFilePath;
	VString		m_iLogTag;
	VString		m_iLogDelimiter;
	unsigned int    m_iLogFileSize;    //  MegaBytes
	unsigned int    m_iLogFileBackups;
	VSimpleFile	m_iLogFile;
    };
}
using V::VApplicationLog;


#endif
