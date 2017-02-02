#ifndef VPrimitiveTaskController_Interface
#define VPrimitiveTaskController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VPrimitiveTask.h"

/*************************
 *****  Definitions  *****
 *************************/

template <class DerivedClass> class VPrimitiveTaskController : public VPrimitiveTaskBase {
    DECLARE_ABSTRACT_RTTLITE (VPrimitiveTaskController<DerivedClass>, VPrimitiveTaskBase);

//  Types
protected:
    typedef void (DerivedClass::*Continuation) ();

//  Construction
protected:
    VPrimitiveTaskController (
	ConstructionData const& rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags,
	Continuation		pContinuation
    ) : BaseClass (rTCData, pDescriptor, iFlags), m_pContinuation (pContinuation) {
    }

//  Execution
protected:
    void run ();

    void clearContinuation () {
	m_pContinuation = 0;
    }
    void setContinuationTo (Continuation pContinuation) {
	m_pContinuation = pContinuation;
    }

//  State
protected:
    Continuation m_pContinuation;
};


/*****************************************
 *****  Template Member Definitions  *****
 *****************************************/

template <class DerivedClass> void VPrimitiveTaskController<DerivedClass>::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    if (m_pContinuation) {
		Continuation pContinuation = m_pContinuation;
		m_pContinuation = NilOf (Continuation);

		(((DerivedClass*)this)->*pContinuation) ();
	    }
	    else if (pausedOnExit ())
		return;
	    else
		exit ();
	}
	break;

    default:
	exit ();
	break;
    }
}


#endif
