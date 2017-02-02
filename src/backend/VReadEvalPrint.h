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

#include "IOMDriver.h"
#include "Vca_VTimer.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VTimer;
    class ITrigger;
    template<class Actor> class VTrigger;
}

class VReadEvalPrintController : public VPrimitiveTaskController<VReadEvalPrintController> {
//  Run Time Type
    DECLARE_CONCRETE_RTT (
	VReadEvalPrintController,VPrimitiveTaskController<VReadEvalPrintController>
    );

//  Construction
public:
    VReadEvalPrintController (
	ConstructionData const&	rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    );

//  Query
protected:
    bool isAController () const;

//  Execution
protected:
    void exit ();
    void fail ();

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
    VOutputBuffer::Reference const	m_pInitialOutputBuffer;
    VCPDReference const			m_pSelfDictionary;
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
