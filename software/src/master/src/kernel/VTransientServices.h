#ifndef VTransientServices_Interface
#define VTransientServices_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VString.h"
#include "V_VApplicationLog.h"

/*************************
 *****  Definitions  *****
 *************************/

class V_API VTransientServices : public virtual VTransient {
    DECLARE_FAMILY_MEMBERS (VTransientServices, VTransient);

//  Construction
public:
    VTransientServices ();

//  Destruction
public:
    ~VTransientServices ();

//  Access
public:
    void logFilePath (VString &rLogFilePath) const {
	m_iLogFile.logFilePath (rLogFilePath);
    }
    void logDelimiter (VString &rLogDelimiter) const {
	m_iLogFile.logDelimiter (rLogDelimiter);
    }
    bool logSwitch () const {
	return m_iLogFile.logSwitch ();
    }
    unsigned int logFileSize () const {
	return m_iLogFile.logFileSize ();
    }
    unsigned int logFileBackups () const {
	return m_iLogFile.logFileBackups ();
    }
    virtual void usageLogFilePath (VString &rLogFilePath) const;
    virtual void usageLogDelimiter (VString &rLogDelimiter) const;
    virtual bool usageLogSwitch () const;
    virtual unsigned int usageLogFileSize () const;
    virtual unsigned int usageLogFileBackups () const;

//  Update
public:
    void updateBackgroundSwitch (bool bIsBackground);
    void updateLogFile () {
	m_iLogFile.updateLogFile ();
    }
    void updateLogSwitch (bool bSwitch) {
	m_iLogFile.updateLogSwitch (bSwitch);
    }
    void updateLogFilePath (char const *pPath) {
	m_iLogFile.updateLogFilePath (pPath);
    }
    void updateLogFileSize (unsigned int iSize) {
	m_iLogFile.updateLogFileSize (iSize);
    }
    void updateLogFileBackups (unsigned int iBackups) {
	m_iLogFile.updateLogFileBackups (iBackups);
    }
    void updateLogTag (char const *pTag) {
	m_iLogFile.updateLogTag (pTag);
    }
    void updateLogDelimiter (char const *pDelimiter) {
	m_iLogFile.updateLogDelimiter (pDelimiter);
    }
    virtual void updateUsageLogFile ();
    virtual void updateUsageLogSwitch (bool bSwitch);
    virtual void updateUsageLogFilePath (char const *pPath);
    virtual void updateUsageLogSize (unsigned int iSize);
    virtual void updateUsageLogBackups (unsigned int iBackups);
    virtual void updateUsageLogTag (char const *pTag);
    virtual void updateUsageLogDelimiter (char const *pDelimiter);

//  Query
public:
    bool isBackground () const {
	return m_bIsBackground;
    }

//  Exception Generation
public:
    virtual void vraiseApplicationException (
	char const *pMessage, va_list pArguments
    );
    virtual void vraiseSystemException (
	char const *pMessage, va_list pArguments
    );
    virtual void vraiseUnimplementedOperationException (
	char const *pMessage, va_list pArguments
    );
    virtual void vraiseException (
	ERRDEF_ErrorCode xErrorCode, char const *pMessage, va_list pArguments
    );

//  Memory Management
public:
    virtual void*allocate	(size_t cBytes);
    virtual void*reallocate	(void *pMemory, size_t cNewBytes);
    virtual void deallocate	(void *pMemory);

//  Message Formatting
public:
    virtual int vdisplay (char const *pMessage, va_list pArguments);

    virtual char const *vstring (char const *pMessage, va_list pArguments);

//  Resource Reclamation
public:
    //  Routines return true when resources are reclaimed, false otherwise.
    virtual bool garbageCollected ();
    virtual bool mappingsReclaimed ();

//  Daemon Creation Methods	    
private:
    bool createDaemon ();
    void redirect_stdin ();
    void redirect_stdout_stderr ();

//  Log Routines
public:
    void log (char const *pFormat, ...);
    void vlog (char const *pFormat, va_list ap) {
	m_iLogFile.vlog (pFormat, ap);
    }
    virtual void logUsageInfo (char const *pFormat, ...);
    virtual void vlogUsageInfo (char const *pFormat, va_list ap);

//  State
protected:
    bool		    m_bIsBackground;
    VApplicationLog	    m_iLogFile;
};


#endif
