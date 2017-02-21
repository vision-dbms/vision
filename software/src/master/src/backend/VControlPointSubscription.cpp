/*****  VControlPointSubscription Implementation  *****/

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

#include "VControlPointSubscription.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSuspension.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************************
 ***************************************
 *****                             *****
 *****  VControlPointSubscription  *****
 *****                             *****
 ***************************************
 ***************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VControlPointSubscription);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_b0_c<VControlPointSubscription>;
template class Vsi_b0<VControlPointSubscription>;
template class Vsi_f0_c<VControlPointSubscription, char const*>;
template class Vsi_p0<VControlPointSubscription>;
template class Vsi_r0<VControlPointSubscription, VSuspension*, VGroundStore*>;
template class Vsi_tb<VControlPointSubscription>;

void VControlPointSubscription::MetaMaker () {
    static Vsi_b0_c<VControlPointSubscription> const
	si_hasExpired			(&VControlPointSubscription::hasExpired),
	si_hasSuspensions		(&VControlPointSubscription::hasSuspensions),
	si_isSingleUse			(&VControlPointSubscription::isSingleUse);

    static Vsi_b0<VControlPointSubscription> const
	si_setSingleUseOff		(&VControlPointSubscription::setSingleUseOff),
	si_setSingleUseOn		(&VControlPointSubscription::setSingleUseOn);

    static Vsi_f0_c<VControlPointSubscription, char const*> const
	si_controllerDescription	(&VControlPointSubscription::controllerDescription),
	si_controlPointDescription	(&VControlPointSubscription::controlPointDescription);

    static Vsi_p0<VControlPointSubscription> const
	si_cancel			(&VControlPointSubscription::cancel);

    static Vsi_r0<VControlPointSubscription, VSuspension*, VGroundStore*> const
	si_nextSuspension		(&VControlPointSubscription::nextSuspension);

    static Vsi_tb<VControlPointSubscription> const si_waitForSuspensions (
	(bool (VControlPointSubscription::*) (VSNFTask*))&VControlPointSubscription::waitForSuspensions
    );


    CSym ("isAControlSubscription"	)->set (RTT, &g_siTrue);

    CSym ("controllerDescription"	)->set (RTT, &si_controllerDescription);
    CSym ("controlPointDescription"	)->set (RTT, &si_controlPointDescription);

    CSym ("hasExpired"			)->set (RTT, &si_hasExpired);
    CSym ("hasSuspensions"		)->set (RTT, &si_hasSuspensions);
    CSym ("isSingleUse"			)->set (RTT, &si_isSingleUse);

    CSym ("cancel"			)->set (RTT, &si_cancel);

    CSym ("setSingleUseOff"		)->set (RTT, &si_setSingleUseOff);
    CSym ("setSingleUseOn"		)->set (RTT, &si_setSingleUseOn);

    CSym ("nextSuspension"		)->set (RTT, &si_nextSuspension);

    CSym ("waitForSuspensions"		)->set (RTT, &si_waitForSuspensions);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VControlPointSubscription::VControlPointSubscription (VControlPoint* pControlPoint)
: m_pControlPoint		(pControlPoint)
, m_pControllerDescription	(pControlPoint->controllerDescription ())
, m_pControlPointDescription	(pControlPoint->description ())
, m_pSuccessor			(pControlPoint->m_pSubscriptionListHead)
, m_pPredecessor		(0)
, m_fSingleUse			(false)
{
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor = this;

    m_pControlPoint->m_pSubscriptionListHead = this;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VControlPointSubscription::~VControlPointSubscription () {
    cancel ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VSuspension* VControlPointSubscription::nextSuspension () {
    VSuspension* pSuspension = m_pSuspensions;

    if (pSuspension) {
	pSuspension->retain ();
	m_pSuspensions.claim (pSuspension->m_pSuccessor);
	pSuspension->untain ();
    }

    return pSuspension;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void VControlPointSubscription::cancel () {
    if (m_pControlPoint) {
	if (m_pSuccessor)
	    m_pSuccessor->m_pPredecessor = m_pPredecessor;

	if (m_pPredecessor)
	    m_pPredecessor->m_pSuccessor = m_pSuccessor;
	else
	    m_pControlPoint->m_pSubscriptionListHead = m_pSuccessor;

	m_pControlPoint = 0;
	m_pPredecessor = m_pSuccessor = 0;

	m_iTrigger.triggerAll ();
    }
}

bool VControlPointSubscription::setSingleUseTo (bool iNewValue) {
    if (hasExpired ())
	return false;

    m_fSingleUse = iNewValue;
    return true;
}

bool VControlPointSubscription::setSingleUseOff () {
    return setSingleUseTo (false);
}

bool VControlPointSubscription::setSingleUseOn () {
    return setSingleUseTo (true);
}


/**********************
 **********************
 *****  Delivery  *****
 **********************
 **********************/

VControlPointSubscription* VControlPointSubscription::deliver (
    VComputationUnit* pUnit, VControlPointFiringType xFiringType
) {
    VControlPointSubscription* pNextSubscription = m_pSuccessor;

    m_pSuspensions.setTo (new VSuspension (pUnit, m_pSuspensions));
    m_iTrigger.triggerAll ();

    switch (xFiringType) {
    case VControlPoint::Controller::FiringType_Normal:
	if (m_fSingleUse)
	    cancel ();
	break;

    case VControlPoint::Controller::FiringType_Final:
	cancel ();
	break;

    default:
	break;
    }

    return pNextSubscription;
}

/******************
 ******************
 *****  Wait  *****
 ******************
 ******************/

bool VControlPointSubscription::waitForSuspensions (VComputationUnit* pSuspendee) {
    if (hasSuspensions () || hasExpired ())
	return false;

    m_iTrigger.suspend (pSuspendee);

    return true;
}

bool VControlPointSubscription::waitForSuspensions (VSNFTask* pSuspendee) {
    return waitForSuspensions ((VComputationUnit*)pSuspendee);
}
