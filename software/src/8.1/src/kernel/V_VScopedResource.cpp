/*****  V_VScopedResource Implementation  *****/

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

#include "V_VScopedResource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VAtom.h"


/********************************
 ********************************
 *****                      *****
 *****  V::VScopedResource  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (V::VScopedResource);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VScopedResource::VScopedResource () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VScopedResource::~VScopedResource () {
}


/******************************
 ******************************
 *****  Scope Management  *****
 ******************************
 ******************************/

bool V::VScopedResource::addScope (Scope *pScope) {
    if (!m_iScopeSet.Insert (pScope))
	return false;

    onScopeAddition (pScope);

    return true;
}

bool V::VScopedResource::deleteScope (Scope *pScope) {
    if (!m_iScopeSet.Delete (pScope))
	return false;

    onScopeDeletion (pScope);

    return true;
}
