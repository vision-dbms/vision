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

#include "RTptoken.h"

#include "Vdd_Store.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (M_KOT);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

M_KOTE::M_KOTE () : m_pObjectCPD (0), m_pObjectHandle (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

M_KOT::~M_KOT () {
    m_pKOTC->release ();
}

M_KOTE::~M_KOTE () {
    if (m_pObjectCPD) {
	m_pObjectCPD->ClearGCLock ();
	m_pObjectCPD->release ();
    }
}
