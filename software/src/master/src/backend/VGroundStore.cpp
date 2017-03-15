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
 *****  Construction  *****
 **************************
 **************************/

VGroundStore::VGroundStore () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VGroundStore::~VGroundStore () {
}

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


/**********************************
 **********************************
 *****  Surrogate Management  *****
 **********************************
 **********************************/

void VGroundStore::detachSurrogate (rtVSTORE_Handle *pSurrogate) {
    if (m_pSurrogate->Names (pSurrogate))
	m_pSurrogate.clear ();
}

void VGroundStore::getSurrogate (rtVSTORE_Handle::Reference &rpResult) {
    rtPTOKEN_Handle *pPPT = ptoken_();

    static_cast<rtVSTORE_Handle*>(pPPT->TheObjectPrototype ().ObjectHandle ())->clone (rpResult, pPPT);

    m_pSurrogate.setTo (rpResult);
    m_pSurrogate->setTransientExtensionTo (this);
}
