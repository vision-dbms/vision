#ifndef VReadEvalPrint_Interface
#define VReadEvalPrint_Interface

/************************
 *****  Components  *****
 ************************/

#include "VPrimitiveTaskController.h"

#include "VkRunTimes.h"
#include "VMutexClaim.h"
#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VReadEvalPrintController : public VPrimitiveTaskController<VReadEvalPrintController> {
    DECLARE_CONCRETE_RTT (
	VReadEvalPrintController,VPrimitiveTaskController<VReadEvalPrintController>
    );

/************************************************************************/
//  class VReadEvalPrintController::QueryContext
/************************************************************************/
public:
    class QueryContext : public Context {
	DECLARE_FAMILY_MEMBERS (QueryContext, Context);

    //  Construction
    public:
	QueryContext (VTask *pCaller);

    //  Destruction
    public:
	~QueryContext ();

    //  Fulfillment
    public:
	bool fulfill (GoferOrder const &rOrder);

    //  Transitions
    public:
	void onQueryInProgress (VReadEvalPrintController *pTask);
	void onQueryCompleted (VReadEvalPrintController *pTask);

    //  State
    private:
	Query::Reference m_pQuery;
        Vsa::request_index_t m_xRequestInProgress;
    };

/************************************************************************/
//  class VReadEvalPrintController
/************************************************************************/
//  Construction
public:
    VReadEvalPrintController (
	ConstructionData const&	rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    );

//  Destruction
private:
    ~VReadEvalPrintController () {
    }

//  Query
protected:
    bool isAController () const;

//  Execution
protected:
    void exit ();
    virtual void fail () OVERRIDE;

//  Execution Support
protected:
    void ProcessCommand (char const *pLine);

    void ScheduleEvaluation ();
    void ConcludeEvaluation (bool fDisplayingOutput = true);

//  Continuations
protected:
    void REPStart   ();
    void REPRestart ();
    void REPRead    ();
    void REPDebug   ();
    void REPEval    ();
    void REPRepeat  ();
    void REPWrap    ();

// Usage Logging Support
public:
    void logUsage ();

// Garbage Collection Control
private:
    void enableGC  ();
    void disableGC ();

//  State
protected:
    QueryContext                        m_iQueryContext;
    VOutputBuffer::Reference const	m_pInitialOutputBuffer;
    VReference<rtDICTIONARY_Handle>const m_pSelfDictionary;
    VString				m_iLastSource;
    VString				m_iThisSource;
    VCPDReference			m_pBlock;
    VkRunTimes				m_iStartTime;
    VkRunTimes				m_iMidTime;
    VkRunTimes				m_iEndTime;
    VMutexClaim				m_iAutoMutexClaim;
    unsigned int			m_iExecutionsRequested;
    unsigned int			m_iExecutionsRemaining;
    bool				m_bDebuggerEnabled,
					m_bEchoingInput,
					m_bExecutionLogged,
					m_bNeedingSetup,
					m_bVerboseStats,
					m_bGCEnabled;
};


#endif
