/*****  VMutex Implementation  *****/

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

#include "VMutex.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VMutexClaim.h"
#include "VMutexGrant.h"


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool VMutex::acquireClaim (
    VMutexClaim& rClaim, VComputationUnit* pSupplicant, transientx_t* pResource
) {
    if (m_pGrant)
	return m_pGrant->acquireClaim (rClaim, pSupplicant);

    m_pGrant = new VMutexGrant (pSupplicant, pResource, this);
    rClaim.setTo (m_pGrant);

    return true;
}

void VMutex::release (transientx_t* pResource) {
    m_pGrant = 0;
}
