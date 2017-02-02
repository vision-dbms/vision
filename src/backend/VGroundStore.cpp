/*****  VGroundStore Implementation  *****/

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

#include "VGroundStore.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTvstore.h"


/**************************
 **************************
 *****                *****
 *****  VGroundStore  *****
 *****                *****
 **************************
 **************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VGroundStore);


/**************************
 **************************
 *****  Access/Query  *****
 **************************
 **************************/

bool VGroundStore::allowsInsert () const {
    return false;
}

bool VGroundStore::allowsDelete () const {
    return false;
}


/********************************************************
 ********************************************************
 *****  Surrogate / Transient Extension Management  *****
 ********************************************************
 ********************************************************/

void VGroundStore::EndRoleAsTransientExtensionOf (VContainerHandle* pContainerHandle) {
    if (m_pSurrogateHandle == pContainerHandle)
	m_pSurrogateHandle = 0;
}

M_CPD *VGroundStore::getCPDForNewSurrogate () {
    M_CPD *pPPT = ptoken_();
    M_CPD *pSurrogateCPD = rtVSTORE_NewCluster (pPPT, pPPT->TheObjectPrototype ());

    m_pSurrogateHandle = pSurrogateCPD->containerHandle ();
    m_pSurrogateHandle->SetTransientExtensionTo (this);

    return pSurrogateCPD;
}
