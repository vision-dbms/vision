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
#include "V_VCamLog.h"
#include "VkSetOf.h"
#include "VkDynamicArrayOf.h"
#include "VkMapOf.h"


/*************************
 *****  Definitions  *****
 *************************/

class V_API VTransientServices : public virtual VTransient {
    DECLARE_FAMILY_MEMBERS (VTransientServices, VTransient);

    typedef VkSetOf<VString, VString const&, char const*> NSTarget_t;
    typedef VkSetOf<int,int> EventSet_t;
    typedef VkSetOf<VString, VString const&, char const*> ActionInfos_t;
    typedef VkMapOf<VString, VString const&, char const*, ActionInfos_t *> ActionSet_t; 

//  Construction
public:
    VTransientServices ();

//  Destruction
public:
    ~VTransientServices ();

//  Access
public:
    void infoServerEntry (VString &rInfoServer) const {
        rInfoServer = m_iInfoServer;
    }
    VString infoLogger () const {
        return m_iInfoLogger;
    }
    VString infoSubscriber () const {
        return m_iInfoSubscriber;
    }

    bool getNSTargets (VkDynamicArrayOf<VString> &rResult, int xEvent, int xType) const;

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
    VString getNSMessage () const;

//  Update
public:
    void updateInfoServerEntry (char const *pInfoServer) {
        m_iInfoServer.setTo (pInfoServer);
    }
    void updateInfoLogger (char const *pInfoLogger) {
        m_iInfoLogger.setTo (pInfoLogger);
    }
    void updateInfoSubscriber (char const *pInfoSubscriber) {
        m_iInfoSubscriber.setTo (pInfoSubscriber);
    }
    void parseNSTFile (char const *pNSTFileName);
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
    void updateCamLogPath (char const *pPath);
    virtual void updateUsageLogFile ();
    virtual void updateUsageLogSwitch (bool bSwitch);
    virtual void updateUsageLogFilePath (char const *pPath);
    virtual void updateUsageLogSize (unsigned int iSize);
    virtual void updateUsageLogBackups (unsigned int iBackups);
    virtual void updateUsageLogTag (char const *pTag);
    virtual void updateUsageLogDelimiter (char const *pDelimiter);
    virtual void setNSServer (char const *pServer);
    virtual void setNSMessage (char const *pMsg);

//  Query
public:
    bool isBackground () const {
	return m_bIsBackground;
    }

// Plan Entry
public:
    class PlanEntry :public VReferenceable {
	DECLARE_CONCRETE_RTTLITE (PlanEntry, VReferenceable);

    // Construction
    public:
	PlanEntry (){
	}

    // Destruction
	~PlanEntry (){
	}

    // Access
    public:
	VString const &name () const {
	    return m_iPlanName;
	}
	void getActionInfos (int xEvent, ActionInfos_t *pActionInfos);

    // Updating
    public:
	bool updateEvents (int xEvent);
	bool updateActions (int xIdx, VString const &rAction);
	void setName (VString const &rName);

    // State
    private:
	VString         m_iPlanName;
	EventSet_t      m_iEvents;
	ActionInfos_t   m_iActionInfos[5];
    };

    typedef VkMapOf<VString, VString const&, char const*, PlanEntry::Reference> PlanEntries_t;
    typedef PlanEntries_t::Iterator PlanIterator;

// NS Entry
public:
    class V_API NSEntry :public VReferenceable {
	DECLARE_CONCRETE_RTTLITE (NSEntry, VReferenceable);

    // Construction
    public:
	NSEntry (){
	}

    // Destruction
	~NSEntry (){
	}

    // Access
    public:
	/** Returns the name of this entry. */
	VString const &name () const {
	    return m_iName;
	}
	VString const &infoServer () const {
	    return m_iInfoServer;
	}
	PlanEntries_t &planEntries () {
	    return m_iPlanEntries;
	}
	void getActionInfos (int xEvent, ActionInfos_t *pActionInfos);

    // Updating
    public:
	bool insertEntry (PlanEntry *pEntry);
	void setName (VString const &rName);
	void setServer (VString const &rServer);

    // State
    private:
	VString m_iName;
	VString m_iInfoServer;
	PlanEntries_t m_iPlanEntries;
    };

    typedef VkMapOf<VString, VString const&, char const*, NSEntry::Reference> NSEntries_t;

public:
    bool parseNotificationTemplate (VSimpleFile &rSessionsFile) ;
    bool parsePlanTemplate (VSimpleFile &rSessionsFile, PlanEntry *pEntry);

//  Globals
private:
    static char const* const g_pRestOfLineBreakSet;
    static char const* const g_pWhitespaceBreakSet;
    static const VString g_iEventNames[24];

// Parse State
    enum ParseState {
	ParseState_Error,
	ParseState_ExpectingTag,
	ParseState_ExpectingLogPath,
	ParseState_ExpectingLogEventId,
	ParseState_ExpectingEmail,
	ParseState_ExpectingEmailEventId,
	ParseState_ExpectingPage,
	ParseState_ExpectingPageEventId,
	ParseState_ExpectingMonster,
	ParseState_ExpectingMonsterEventId,
	ParseState_ExpectingRPD,
	ParseState_ExpectingRPDEventId,
	ParseState_ExpectingNotificationName,
	ParseState_ExpectingPlanSentinel,
	ParseState_ExpectingNotificationSentinel,
	ParseState_ExpectingNotificationServerName,
	ParseState_ExpectingEventId,
	ParseState_ExpectingActionTag,
	ParseState_ExpectingPlanName
    };

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

//  Notification
public:
    virtual void notify (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap);
    bool getNSEntry (VString const &rName, NSEntry::Reference &rEntry) const;

//  State
protected:
    bool		    m_bIsBackground;
    VApplicationLog	    m_iLogFile;
    VCamLog	            m_iCamLog;
    VString                 m_iInfoServer;
    VString                 m_iInfoLogger;
    VString                 m_iInfoSubscriber;
    NSTarget_t              m_iNST[1001][5]; // Event #0: Dummy Event. 
    VString                 m_iNSServer;
    VString                 m_iNSMessage;
    NSEntries_t             m_iNSEntries;
};


#endif
