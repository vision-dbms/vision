/*****  M_KnownObjectTable Implementation  *****/

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

#include "M_KnownObjectTable.h"

/************************
 *****  Supporting  *****
 ************************/

#include "M_CPD.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (M_KnownObjectTable);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

//  See envir.cpp

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

M_KnownObjectTable::~M_KnownObjectTable () {
    m_pKOTC->release ();
}

M_KnownObjectTable::Entry::~Entry () {
    if (m_pObjectCPD) {
	m_pObjectCPD->ClearGCLock ();
	m_pObjectCPD->release ();
    }
    if (m_pPTokenCPD) {
	m_pPTokenCPD->ClearGCLock ();
	m_pPTokenCPD->release ();
    }
}
