/*****  VDatabaseActivation Implementation  *****/

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

#include "VDatabaseActivation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VDatabaseFederator.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VDatabaseActivation);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VDatabaseActivation::VDatabaseActivation (VDatabaseFederator *pFederator)
: m_pFederator			(pFederator)
, m_pActivationListSuccessor	(pFederator->m_pActivationListHead)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VDatabaseActivation::~VDatabaseActivation () {
}
