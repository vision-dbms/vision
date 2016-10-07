/*****  VRunTimeType Implementation  *****/

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

#include "VRunTimeType.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  VRunTimeType  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VRunTimeType::VRunTimeType (std::type_info const &rRTTI, MetaMaker pMeta, VRunTimeType *pSuper)
: m_iRTTI	(rRTTI)
, m_pMeta	(pMeta)
, m_pSuper	(pSuper)
, m_xType	(USHRT_MAX)
, m_cSubtypes	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VRunTimeType::~VRunTimeType () {
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void VRunTimeType::initialize () {
    if (USHRT_MAX == m_xType) {
	if (m_pSuper) {
	    m_pSuper->initialize ();
	    m_pSuper->addSubtype (this);
	    if (m_pMeta != m_pSuper->meta ())
		m_pMeta ();
	}
	else {
	    m_xLevel = m_xType = 0;
	    m_pMeta ();
	}
    }
}

void VRunTimeType::addSubtype (VRunTimeType *pSubtype) {
    pSubtype->m_xLevel = (level_t)(m_xLevel + 1);
    pSubtype->m_xType = (index_t)(m_cSubtypes++);
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VRunTimeType::isA (VRunTimeType const &rRTT) const {
    VRunTimeType const *pThis = this;
    while (pThis && pThis != &rRTT)
	pThis = pThis->supertype ();

    return IsntNil (pThis);
}
