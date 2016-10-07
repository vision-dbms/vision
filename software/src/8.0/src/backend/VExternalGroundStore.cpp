/*****  VExternalGroundStore Implementation  *****/

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

#include "VExternalGroundStore.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSNFTask.h"


/**********************************
 **********************************
 *****                        *****
 *****  VExternalGroundStore  *****
 *****                        *****
 **********************************
 **********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VExternalGroundStore);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VExternalGroundStore::VExternalGroundStore (
    M_ASD *pSpace, Vxa::ICollection *pCollection, unsigned int sCollection
) : m_pCollection (pCollection), m_pPToken (rtPTOKEN_New (pSpace, sCollection)) {
}

VExternalGroundStore::VExternalGroundStore (Vxa::ISingleton *pCollection) : m_pCollection (pCollection) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VExternalGroundStore::~VExternalGroundStore () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

unsigned int VExternalGroundStore::cardinality_ () const {
    return m_pPToken ? rtPTOKEN_CPD_BaseElementCount (m_pPToken) : 1;
}

M_CPD *VExternalGroundStore::ptoken_() const {
    return m_pPToken ? static_cast<M_CPD*>(m_pPToken) : M_AttachedNetwork->TheScalarPToken ();
}

/*********************************
 *********************************
 *****  Task Implementation  *****
 *********************************
 *********************************/

bool VExternalGroundStore::groundImplementationSucceeded (VSNFTask* pTask) {
    pTask->startExternalInvocation (m_pCollection);
    return true;
}
