/*****  VMutexController Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VMutexController.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTblock.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VPrimitiveTaskController<VMutexController>;


/******************************
 ******************************
 *****                    *****
 *****  VMutexController  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VMutexController);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VMutexController::VMutexController (
    ConstructionData const&	rTCData,
    VPrimitiveDescriptor*	pDescriptor,
    unsigned short		iFlags
)
: VPrimitiveTaskController<VMutexController> (
    rTCData, pDescriptor, iFlags, &VMutexController::Start
), m_pProtectedPrimitive (0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VMutexController::~VMutexController () {
    ReleaseMutex ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void VMutexController::fail () {
    ReleaseMutex ();
}


/***************************
 ***************************
 *****  Continuations  *****
 ***************************
 ***************************/

/*******************
 *****  Start  *****
 *******************/

void VMutexController::Start () {
    setContinuationTo (&VMutexController::AcquireMutex);

    rtBLOCK_Handle::Reference pProtectedBlock; unsigned int xProtectedPrimitive;
    switch (flags ()) {
    case 0:
	getCurrent().decodeClosure (pProtectedBlock, xProtectedPrimitive);
	if (pProtectedBlock)
	    m_pProtectedContainer.setTo (pProtectedBlock);
	else
	    m_pProtectedPrimitive = PrimitiveDescriptor (xProtectedPrimitive);
	break;
    case 1:
	m_pProtectedContainer.setTo (getSelf().store ());
	break;
    default:
	raiseUnimplementedAliasException ("VMutexController::Start");
	break;
    }
}

/**************************
 *****  AcquireMutex  *****
 **************************/

void VMutexController::AcquireMutex () {
    bool acquired = m_pProtectedContainer
	? m_pProtectedContainer->AcquireMutex (m_iMutexClaim, this)
	: m_pProtectedPrimitive->AcquireMutex (m_iMutexClaim, this);

    if (!acquired)
	setContinuationTo (&VMutexController::AcquireMutex);
    else {
	setContinuationTo (&VMutexController::ReleaseMutex);

	switch (flags ()) {
	default:
	case 0:
	    beginValueCall	 (parameterCount ());

	    commitRecipient	 (VMagicWord_Current);

	    commitParameters	 ();
	    commitCall		 ();
	    break;

	case 1:
	    beginValueCall	 (parameterCount () - 1);

	    loadDucWithParameter (0);
	    commitRecipient	 ();

	    commitParameters	 (1);
	    commitCall		 ();
	    break;
	}
    }
}

/**************************
 *****  ReleaseMutex  *****
 **************************/

void VMutexController::ReleaseMutex () {
    m_iMutexClaim.releaseClaim ();
}
