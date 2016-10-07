#ifndef VTransientServicesForBatchvision_Interface
#define VTransientServicesForBatchvision_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransientServices.h"
#include "V_VApplicationLog.h"
#include "V_VPointer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VTransientServicesForBatchvision : public VTransientServices {
    DECLARE_FAMILY_MEMBERS (VTransientServicesForBatchvision, VTransientServices);

//  Construction
public:
    VTransientServicesForBatchvision ();

//  Destruction
public:
    ~VTransientServicesForBatchvision () {
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
    virtual void *allocate (size_t cBytes);
    virtual void *reallocate (void *pMemory, size_t cNewBytes);
    virtual void  deallocate (void *pMemory);

    V::U64 totalMemoryAllocation () const {
	return m_sTotalMemoryAllocation;
    }
    V::U64 totalMemoryAllocationHighWaterMark () const {
	return m_sTotalMemoryAllocationHighWaterMark;
    }

    V::U64 memoryAllocationLimit () const {
	return m_sMemoryAllocationLimit;
    }

//  Diagnostic Support
public:
    void enableStackDumps (bool bOnOff);
    void dumpStack ();

//  Message Formatting
public:
    virtual int vdisplay (char const *pMessage, va_list pArguments);

//  Resource Reclamation
public:
    //  Routines return true when resources are reclaimed, false otherwise.
    virtual bool garbageCollected ();
    virtual bool mappingsReclaimed ();

// Reclamation tuning
public:
    void setGCThreshold (unsigned int sValue) {
	m_sGCThreshold = 
	    m_sInitialGCThreshold = sValue * 1024 * 1024;
    }
    void setGCSpread (unsigned int sValue) {
	m_sGCSpread = sValue * 1024 * 1024;
    }
    unsigned int getGCThreshold () {
	return static_cast<unsigned int> (m_sGCThreshold / (1024 * 1024));
    }
    unsigned int getGCSpread () {
	return static_cast<unsigned int> (m_sGCSpread / (1024 * 1024));
    }
    
// Usage Logging
public:
    void usageLogFilePath (VString &rLogFilePath) const {
	m_iUsageLog.logFilePath (rLogFilePath);
    }
    void usageLogDelimiter (VString &rLogDelimiter) const {
	m_iUsageLog.logDelimiter (rLogDelimiter);
    }
    bool usageLogSwitch () const {
	return m_iUsageLog.logSwitch ();
    }
    unsigned int usageLogFileSize () const {
	return m_iUsageLog.logFileSize ();
    }
    unsigned int usageLogFileBackups () const {
	return m_iUsageLog.logFileBackups ();
    }
    void updateUsageLogFile () {
	m_iUsageLog.updateLogFile ();
    }
    void updateUsageLogSwitch (bool bSwitch) {
	m_iUsageLog.updateLogSwitch (bSwitch);
    }
    void updateUsageLogFilePath (char const *pPath) {
	m_iUsageLog.updateLogFilePath (pPath);
    }
    void updateUsageLogSize (unsigned int iSize) {
	m_iUsageLog.updateLogFileSize (iSize);
    }
    void updateUsageLogBackups (unsigned int iBackups) {
	m_iUsageLog.updateLogFileBackups (iBackups);
    }
    void updateUsageLogTag (char const *pTag) {
	m_iUsageLog.updateLogTag (pTag);
    }
    void updateUsageLogDelimiter (char const *pDelimiter) {
	m_iUsageLog.updateLogDelimiter (pDelimiter);
    }

    void logUsageInfo (char const *pFormat, ...);
    void vlogUsageInfo (char const *pFormat, va_list ap) {
	m_iUsageLog.vlog (pFormat, ap);
    }


//  State
private:
    V::U64		m_sTotalMemoryAllocation;
    V::U64		m_sTotalMemoryAllocationHighWaterMark;
    V::U64		m_sMemoryAllocationLimit;
    V::U64		m_sInitialGCThreshold;
    V::U64		m_sGCThreshold;
    V::U64		m_sGCSpread;
    bool		m_bStackDumpEnabled;
    int			m_iErrorCount;
    VApplicationLog 	m_iUsageLog;
};


#endif
