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
    VString const &rSelectorName, cardinality_t cParameters, cardinality_t cTask, bool bIntensional
) : m_iSelector    (rSelectorName, cParameters)
  , m_pDomain      (new VFiniteSet (cTask))
  , m_bIntensional (bIntensional)
{
}

Vxa::VCallData::VCallData (
    ThisClass const &rOther
) : m_iSelector    (rOther.m_iSelector)
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
    VString rNameResidue (rName);
    for (cardinality_t xComponent = 0;
         xComponent < cParameters && rNameResidue.getPrefix (
             ':', const_cast<VString&>(m_aComponents[xComponent]), rNameResidue
         );
         xComponent++
    );
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

VString const &Vxa::VCallData::Selector::component (cardinality_t xComponent) const {
    if (xComponent < parameterCount ())
        return m_aComponents[xComponent];

    static VString g_iEmptyString;
    return g_iEmptyString;
}

bool Vxa::VCallData::Selector::component (VString &rComponent, unsigned int xComponent) const {
    if (xComponent < m_aComponents.elementCount ()) {
        rComponent.setTo (m_aComponents[xComponent]);
        return true;
    }
    return false;
}
