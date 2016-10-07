/*****  VUtilityCall Implementation  *****/

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

#include "VUtilityCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSelector.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VUtilityCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VUtilityCall::MetaMaker () {
    CSym ("isAUtilityCall")->set (RTT, &g_siTrue);
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VSelector const& VUtilityCall::selector_ () const {
    static VSelector const iKS__Value	    (KS__Value);
    static VSelector const iKS__Environment (KS__Environment);
    return m_xReturnCase != Return_Current ? iKS__Value : iKS__Environment;
}
