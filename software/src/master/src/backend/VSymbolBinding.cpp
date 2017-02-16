/*****  VSymbolBinding Implementation  *****/

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

#include "VSymbolBinding.h"

/************************
 *****  Supporting  *****
 ************************/


/****************************
 ****************************
 *****                  *****
 *****  VSymbolBinding  *****
 *****                  *****
 ****************************
 ****************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSymbolBinding);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSymbolBinding::VSymbolBinding (VRunTimeType *pRTT)
: m_pType		(pRTT)
, m_xSubtypeBindingLB	(USHRT_MAX)
, m_xSubtypeBindingUB	(0)
, m_pImplementation	(0)
{
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VSymbolBindingHolder &VSymbolBinding::subtypeBindingHolder (index_t xSubtype) {
    if (m_xSubtypeBindingLB > m_xSubtypeBindingUB) {
	m_iSubtypeBindings.Append (1);
	m_xSubtypeBindingLB = xSubtype;
	m_xSubtypeBindingUB = (index_t)(xSubtype + 1);
    }
    else if (m_xSubtypeBindingLB > xSubtype) {
	m_iSubtypeBindings.Prepend (m_xSubtypeBindingLB - xSubtype);
	m_xSubtypeBindingLB = xSubtype;
    }
    else if (xSubtype >= m_xSubtypeBindingUB) {
	m_iSubtypeBindings.Append (xSubtype - m_xSubtypeBindingUB + 1);
	m_xSubtypeBindingUB = (index_t)(xSubtype + 1);
    }

    return m_iSubtypeBindings[xSubtype - m_xSubtypeBindingLB];
}


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void VSymbolBinding::Display (bool bDisplaySubtypeBindings, unsigned int xLevel) const {
    if (!this)
	return;

    printf ("%*c%s%s\n", xLevel * 2, ' ', m_pType->name (), m_pImplementation ? " Y" : "");

    if (bDisplaySubtypeBindings && m_xSubtypeBindingUB > m_xSubtypeBindingLB) {
	unsigned int nSubtypes = m_xSubtypeBindingUB - m_xSubtypeBindingLB;
	for (unsigned int xSubtype = 0; xSubtype < nSubtypes; xSubtype++)
	    m_iSubtypeBindings[xSubtype]->Display (true, xLevel + 1);
    }
}
