/*****  Vca::VcaThreadState Implementation  *****/

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

#include "Vca_VcaThreadState.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "VStaticTransient.h"	// ... temporary hack
#include "V_VThread.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VcaThreadState  *****
 *****                       *****
 *********************************
 *********************************/

/*************************
 *************************
 *****  Context Key  *****
 *************************
 *************************/

Vca::VcaThreadState::Key const Vca::VcaThreadState::g_iTLSKey;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaThreadState::VcaThreadState (
    Thread *pThread
) : BaseClass (pThread), m_pCohortClaims (&m_pDefaultCohortClaim), m_pDefaultCohortClaim (this) {
    retain (); {
	g_iTLSKey.setSpecific (this);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaThreadState::~VcaThreadState () {
}


/***************************
 ***************************
 *****  Cohort Access  *****
 ***************************
 ***************************/

Vca::VCohort *Vca::VcaThreadState::Cohort () {
    Reference pThisInstance (Here ());
    return pThisInstance ? pThisInstance->cohort () : 0;
}

void *Vca::VcaThreadState::CohortValue (VCohortIndex const &rIndex) {
    return VCohort::GetValue (Cohort (), rIndex);
}


/***************************
 ***************************
 *****  Cohort Claims  *****
 ***************************
 ***************************/

/*------------------------------------------------------------*
 *  Private routine called from a Claim constructor to push
 *  a new, stack-based, claim onto a thread's claim list.
 *------------------------------------------------------------*/
void Vca::VcaThreadState::attach (CohortClaim *pClaim, bool bWait) {
    if (m_pCohortClaims.referent () == pClaim->threadLink ()) {
	m_pCohortClaims.setTo (pClaim);
	pClaim->acquire (bWait);
    }
}

/*------------------------------------------------------------*
 *  Private routine called from the Claim destructor to pop
 *  the claim from a thread's claim list.
 *------------------------------------------------------------*/
void Vca::VcaThreadState::detach (CohortClaim *pClaim) {
    if (m_pCohortClaims.referent () == pClaim) {
	m_pCohortClaims->relinquish (m_pCohortClaims);
    }
}

bool Vca::VcaThreadState::acquireCohorts () {
    CohortClaimPointer pClaim (m_pCohortClaims);
    while (pClaim && pClaim->unlinquish (pClaim));
    return pClaim.isNil ();
}

void Vca::VcaThreadState::releaseCohorts () {
    CohortClaimPointer pClaim (m_pCohortClaims);
    while (pClaim) {
	pClaim->relinquish (pClaim);
    }
}


/*****************************
 *****************************
 *****  Synchronization  *****
 *****************************
 *****************************/

void Vca::VcaThreadState::signal () {
    m_iControlSemaphore.produce ();
}

void Vca::VcaThreadState::waitForSignal () {
    m_iControlSemaphore.consume ();
}


/********************************
 ********************************
 *****  Thread Association  *****
 ********************************
 ********************************/

Vca::VcaThreadState::Reference Vca::VcaThreadState::Here () {
    VStaticTransient::InitializeAllInstances ();	// ... temporary hack

    BaseClass::Reference pSpecific; Reference pThisInstance;
    // Check if we already have a VcaThreadState for this thread.
    if (g_iTLSKey.getSpecific (pSpecific) && pSpecific.isntNil ())
        // Found one. Use it.
	pThisInstance.setTo (static_cast<ThisClass*>(pSpecific.referent ()));
    else {
        // None existed. Make one.
	Thread::Reference const pThread (Thread::Here ());
	pThisInstance.setTo (new ThisClass (pThread));
    }
    return pThisInstance;
}

void Vca::VcaThreadState::onThreadDetach () {
//  Detach all cohort claims, ...
    while (m_pCohortClaims)
	detach (m_pCohortClaims);

//  ... and call base class detach handler:
    BaseClass::onThreadDetach ();
}
