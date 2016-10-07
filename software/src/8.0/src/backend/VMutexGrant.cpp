/*****  VMutexGrant Implementation  *****/

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

#include "VMutexGrant.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VComputationUnit.h"

#include "VMutex.h"
#include "VMutexClaim.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VMutexGrant);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VMutexGrant::VMutexGrant (VComputationUnit *pOwner, transientx_t *pResource, VMutex *pMutex)
: m_pOwner (pOwner), m_pMutex (pResource, pMutex), m_cClaims (1)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VMutexGrant::~VMutexGrant () {
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool VMutexGrant::acquireClaim (VMutexClaim& rClaim, VComputationUnit* pSupplicant) {
    if (rClaim.grant () == this)
	return true;

    if (m_pOwner->consumesResultOf (pSupplicant)) {
	m_cClaims++;
	rClaim.setTo (this);

	return true;
    }

    m_iQueue.suspend (pSupplicant);

    return false;
}

void VMutexGrant::releaseClaim () {
//  If this is the final claim on the grant, release the mutex and resume its suspendees:
    if (0 == --m_cClaims) {
	m_pMutex->release (m_pMutex.container ());

	m_iQueue.triggerAll ();
    }
}
