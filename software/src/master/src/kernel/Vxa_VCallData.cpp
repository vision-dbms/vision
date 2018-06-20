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
    VCollection *pCluster, VString const &rSelectorName, cardinality_t cParameters, cardinality_t cTask, bool bIntensional
) : m_pCluster     (pCluster)
  , m_iSelector    (rSelectorName, cParameters)
  , m_pDomain      (new VFiniteSet (cTask))
  , m_bIntensional (bIntensional)
{
}

Vxa::VCallData::VCallData (
    ThisClass const &rOther
) : m_pCluster     (rOther.m_pCluster)
  , m_iSelector    (rOther.m_iSelector)
  , m_pDomain      (rOther.m_pDomain)
  , m_bIntensional (rOther.m_bIntensional)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallData::~VCallData () {
}


/**************************************
 **************************************
 *****                            *****
 *****  Vxa::VCallData::Selector  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallData::Selector::Selector (
    VString const &rName, cardinality_t cParameters
) : m_iName (rName), m_aComponents (cParameters) {
    VString iNameResidue (rName); VString iNextResidue;
    for (cardinality_t xComponent = 0;
         xComponent < cParameters && iNameResidue.getPrefix (
             ':', const_cast<VString&>(m_aComponents[xComponent]), iNextResidue
         );
         xComponent++
    ) {
        iNameResidue.setTo (iNextResidue);
    }
}

Vxa::VCallData::Selector::Selector (
    Selector const &rOther
) : m_iName (rOther.m_iName), m_aComponents (rOther.m_aComponents) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallData::Selector::~Selector () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

V::VString const &Vxa::VCallData::Selector::component (cardinality_t xComponent) const {
    return xComponent < parameterCount () ? m_aComponents[xComponent] : m_iName;
}

bool Vxa::VCallData::Selector::component (VString &rComponent, unsigned int xComponent) const {
    if (xComponent < parameterCount ()) {
        rComponent.setTo (m_aComponents[xComponent]);
        return true;
    }
    return false;
}
