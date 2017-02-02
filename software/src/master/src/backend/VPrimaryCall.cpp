/*****  VPrimaryCall Implementation  *****/

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

#include "VPrimaryCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VByteCodeScanner.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VPrimaryCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VPrimaryCall::MetaMaker () {
    CSym ("isAPrimaryCall")->set (RTT, &g_siTrue);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VPrimaryCall::VPrimaryCall (
    VTask *pCaller, rtLINK_CType *pCallerSubset, VByteCodeScanner const &rMessageSource
) : VCall (pCaller, pCallerSubset, Type_Message, rMessageSource.fetchedSelectorValence ()), m_iSelector	(rMessageSource) {
}

VPrimaryCall::VPrimaryCall (
    VTask *pCaller, rtLINK_CType *pCallerSubset, char const * pMessageName, unsigned int iParameterCount
) : VCall (pCaller, pCallerSubset, Type_Message, iParameterCount), m_iSelector (pMessageName) {
}

VPrimaryCall::VPrimaryCall (
    VTask *pCaller, rtLINK_CType *pCallerSubset, unsigned int xMessageName
) : VCall (pCaller, pCallerSubset, Type_Message, SELECTOR_KSValence (xMessageName)), m_iSelector (xMessageName) {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VSelector const &VPrimaryCall::selector_() const {
    return m_iSelector;
}
