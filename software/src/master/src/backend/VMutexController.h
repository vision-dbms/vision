#ifndef VMutexController_Interface
#define VMutexController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VPrimitiveTaskController.h"

#include "VCPDReference.h"
#include "VMutexClaim.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VMutexController : public VPrimitiveTaskController<VMutexController> {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VMutexController,VPrimitiveTaskController<VMutexController>);

//  Construction
public:
    VMutexController (
	ConstructionData const&	rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    );

//  Destruction
protected:
    ~VMutexController ();

//  Execution
protected:
    void fail ();

//  Continuations
protected:
    void Start ();
    void AcquireMutex ();
    void ReleaseMutex ();

//  State
protected:
    VMutexClaim			m_iMutexClaim;
    VContainerHandle::Reference	m_pProtectedContainer;
    VPrimitiveDescriptor*	m_pProtectedPrimitive;
};


#endif
