/*****  VSelector Implementation  *****/

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

#include "VSelector.h"

/************************
 *****  Supporting  *****
 ************************/

#include "selector.h"

#include "verrdef.h"

#include "RTseluv.h"
#include "RTstring.h"
#include "RTvstore.h"

#include "VByteCodeScanner.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSelectorGeneral::VSelectorGeneral (M_CPD* pMessageName) : VSelector (
    SELECTOR_StringToKSI (rtSTRING_CPD_Begin (pMessageName)), rtSTRING_CPD_Begin (pMessageName)
)
{
}

VSelectorGeneral::VSelectorGeneral (char const *pMessageName) : VSelector (
    SELECTOR_StringToKSI (pMessageName), pMessageName
)
{
}

VSelector::VSelector (int xMessageName, char const *pMessageName) {
    if (xMessageName < 0) {
	m_xType = String;
	m_iValue.as_pMessageName = pMessageName;
    }
    else {
	m_xType = Known;
	m_iValue.as_xMessageName = xMessageName;
    }
}

VSelector::VSelector (VByteCodeScanner const &rMessageSource) {
    m_xType = Block;
    m_iValue.as_iBlockMessage.construct (
	rMessageSource.blockHandle (), rMessageSource.fetchedSelectorIndex ()
    );
}

VSelector::VSelector (char const *pMessageName) {
    m_xType = String;
    m_iValue.as_pMessageName = pMessageName;
}

VSelector::VSelector (unsigned int xMessageName) {
    m_xType = Known;
    m_iValue.as_xMessageName = xMessageName;
}

VSelector::VSelector () {
    m_xType = Value;
    m_iValue.as_xMessageName = KS__Value;
}

VSelector::~VSelector () {
    switch (m_xType) {
    case Block:
	m_iValue.as_iBlockMessage.destroy ();
	break;
    default:
	break;
    }
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VSelector::getKSData (unsigned int& xSelector) const {
    if (isAKnownSelector () || isTheValueCallSelector ()) {
	xSelector = m_iValue.as_xMessageName;
	return true;
    }
    return false;
}

bool VSelector::getBSData (rtBLOCK_Handle::Reference &rpBlock, unsigned int& rxSelector) const {
    if (isABlockSelector ()) {
	rpBlock.setTo (m_iValue.as_iBlockMessage.block ());
	rxSelector = m_iValue.as_iBlockMessage.m_xSelector;
	return true;
    }
    return false;
}

char const* VSelector::messageName () const {
    switch (m_xType) {
    case Value:
    case Known:
 	return KS__ToString ((unsigned short)m_iValue.as_xMessageName);
    case Block:
	return m_iValue.as_iBlockMessage.messageName ();
    case String:
	return m_iValue.as_pMessageName;
    default:
	raiseSelectorTypeException ();
        return 0;
    }
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool VSelector::insertIn (rtSELUV_Set& rSet, unsigned int& rxElement) const {
    switch (m_xType) {
    case Known:
    case Value:
	return rSet.Insert (m_iValue.as_xMessageName, rxElement);
    case Block:
	return rSet.Insert (
	    m_iValue.as_iBlockMessage.messageName (), rxElement
	);
    case String:
	return rSet.Insert (m_iValue.as_pMessageName, rxElement);
    default:
	raiseSelectorTypeException ();
	break;
    }

    return false;
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VSelector::raiseSelectorTypeException () const {
    raiseException (EC__InternalInconsistency, "VSelector::Unknown type %d\n", m_xType);
}
