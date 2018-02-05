#ifndef VMutexController_Interface
#define VMutexController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VPrimitiveTaskController.h"

#include "VMutexClaim.h"

#include "Vdd_Store.h"

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
    virtual void fail () OVERRIDE;

//  Continuations
protected:
    void Start ();
    void AcquireMutex ();
    void ReleaseMutex ();

//  State
protected:
    VMutexClaim			m_iMutexClaim;
    Vdd::Store::Reference	m_pProtectedContainer;
    VPrimitiveDescriptor*	m_pProtectedPrimitive;
};


#endif
