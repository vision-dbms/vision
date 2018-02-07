/*****  Vxa_VCallData Implementation  *****/

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

#include "Vxa_VCallData.h"

/************************
 *****  Supporting  *****
 ************************/


/****************************
 ****************************
 *****                  *****
 *****  Vxa::VCallData  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallData::VCallData (
    VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask
) : m_iMethodName (rMethodName), m_cParameters (cParameters), m_pDomain (new VFiniteSet (cTask)) {
}

Vxa::VCallData::VCallData (
    ThisClass const &rOther
) : m_iMethodName (rOther.m_iMethodName), m_cParameters (rOther.m_cParameters), m_pDomain (rOther.m_pDomain) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallData::~VCallData () {
}
