#ifndef VWhileTrueController_Interface
#define VWhileTrueController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VPrimitiveTaskController.h"

#include "VDescriptor.h"
#include "VLinkCReference.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VWhileTrueController : public VPrimitiveTaskController<VWhileTrueController> {
//  Run Time Type
    DECLARE_CONCRETE_RTT (
	VWhileTrueController, VPrimitiveTaskController<VWhileTrueController>
    );

// Construction
public:
    VWhileTrueController (
	ConstructionData const&	rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    );

//  Continuations
protected:
    void startup		();
    void bodyContinuation	();
    void conditionContinuation	();

//  Utilities
protected:
    void restrict (rtLINK_CType* pSubset);
    void restrict (rtLINK_CType* pSubset, VDescriptor& rDatum);

    void scheduleCondition () {
	schedule (&VWhileTrueController::conditionContinuation, m_iCondition);
    }
    void scheduleBody () {
	schedule (&VWhileTrueController::bodyContinuation, m_iBody);
    }
    void schedule (Continuation pContinuation, VDescriptor& rDatum);

//  State
protected:
    VDescriptor		m_iCondition;
    VDescriptor		m_iBody;
    VLinkCReference	m_pSubset;
};

#endif
