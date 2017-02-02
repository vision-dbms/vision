#ifndef VCamLogger_Interface
#define VCamLogger_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VSimpleFile.h"
#include "V_VString.h"
#include "V_VQueue.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VCamLog {

    //  Construction
    public:
	VCamLog ();

    //  Destruction
    public:
	~VCamLog () {
	}
    //  Access
    public:
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
	void updateLogFilePath (char const *pPath) {
	    m_iLogFileNamePrefix.setTo (pPath);
	}
	void updateLogFileSize (unsigned int iSize) {
	    m_iLogFileSize = iSize;
	}
	void updateLogFileBackups (unsigned int iBackups) {
	    m_iLogFileBackups = iBackups;
	}

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
	void log (char const *message);


    //  State
    private:
	VString		m_iLogFileNamePrefix;
	unsigned int    m_iLogFileSize;    //  MegaBytes
	unsigned int    m_iLogFileBackups;
	VSimpleFile	m_iLogFile;
	VString		m_iLogFileName;
        VQueue<VString> m_iLogFilePriorNames;
    };

    void camLogCastAndWriteMessage(void* object, const char* message);
}
using V::VCamLog;


#endif
