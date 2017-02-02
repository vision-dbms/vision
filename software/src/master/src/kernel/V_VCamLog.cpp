/*****  VCamLog Implementation  *****/

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

#include "V_VCamLog.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "V_VTime.h"
#include "cam.h"


#if defined(_WIN32)
static char const *const g_pNullDeviceName = "NUL:";
#elif defined(__VMS)
static char const *const g_pNullDeviceName = "NL:";
#else
static char const *const g_pNullDeviceName = "/dev/null";
#endif


V::VCamLog::VCamLog ()
    : m_iLogFileNamePrefix	(g_pNullDeviceName, false)
    , m_iLogFileSize		(1)
    , m_iLogFileBackups		(10)
{
    CAM::ProcessLevel::setCamDestination(&camLogCastAndWriteMessage, this);
}

void V::VCamLog::log (char const *message) {
    initLogFile ();
    if (logFileExceedsSize ()) 
        rotateLogFiles ();

    m_iLogFile.PutString (message);
    m_iLogFile.flush ();
}

void V::VCamLog::initLogFile () {
    if (m_iLogFile.isntOpen ()) {
	if (!m_iLogFile.OpenUniqueTemp (m_iLogFileName, m_iLogFileNamePrefix.content())) {
            /* silently swallow errors and log to the null device */
	    m_iLogFile.OpenForTextAppend (g_pNullDeviceName);
        }
    }

}

void V::VCamLog::rotateLogFiles () {
    if (logFileExceedsSize ()) {
        if (m_iLogFileName.isntEmpty()) {
            m_iLogFilePriorNames.enqueue(m_iLogFileName);
        }

        //  close current log file
        initLogFile ();
	m_iLogFile.flush ();
	m_iLogFile.close ();

        // open next log file
        initLogFile();

        // delete an old log file?
        if (m_iLogFileBackups <= m_iLogFilePriorNames.size()) {
            VString deleteName;
            m_iLogFilePriorNames.dequeue(deleteName);
            remove(deleteName);
        }
    }
}


void V::camLogCastAndWriteMessage(void* object, const char* message) {
  V::VCamLog* logger(static_cast<V::VCamLog*>(object));
  logger->log(message);
}
