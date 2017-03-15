/*****  VMutexClaim Implementation  *****/

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

#include "VMutexClaim.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VMutexGrant.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VMutexClaim::VMutexClaim () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VMutexClaim::~VMutexClaim () {
    if (m_pGrant)
	m_pGrant->releaseClaim ();
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void VMutexClaim::releaseClaim () {
    if (m_pGrant) {
	m_pGrant->releaseClaim ();
	m_pGrant.clear ();
    }
}

void VMutexClaim::setTo (VMutexGrant* pNewGrant) {
    if (pNewGrant != m_pGrant) {
	if (m_pGrant)
	    m_pGrant->releaseClaim ();
	m_pGrant.setTo (pNewGrant);
    }
}
