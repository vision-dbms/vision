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
    )
    : VPrimitiveTaskBase (rTCData, pDescriptor, iFlags)
    , m_pContinuation (pContinuation)
    {
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

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VPrimitiveTaskController.i")
#else
#include "VPrimitiveTaskController.i"
#endif


#endif
