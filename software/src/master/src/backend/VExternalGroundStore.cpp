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
) : m_pCollection (pCollection), m_pPToken (new rtPTOKEN_Handle (pSpace, sCollection)) {
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

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

unsigned int VExternalGroundStore::cardinality_ () const {
    return m_pPToken ? m_pPToken->cardinality () : 1;
}

rtPTOKEN_Handle *VExternalGroundStore::ptoken_() const {
    return m_pPToken ? static_cast<rtPTOKEN_Handle*>(m_pPToken) : M_AttachedNetwork->TheScalarPTokenHandle ();
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
