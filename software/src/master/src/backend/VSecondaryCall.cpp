/*****  VSecondaryCall Implementation  *****/

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

#include "VSecondaryCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VTask.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSecondaryCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VSecondaryCall::MetaMaker () {
    CSym ("isASecondaryCall")->set (RTT, &g_siTrue);
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSecondaryCall::VSecondaryCall (VTask* pCaller, rtLINK_CType* pCallerSubset)
: VCall (pCaller, pCallerSubset, pCaller->call()->type(), pCaller->parameterCount ())
{
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

rtBLOCK_Handle *VSecondaryCall::boundBlock () const {
    return m_pCaller->call()->boundBlock ();
}

VSelector const& VSecondaryCall::selector_ () const {
    return m_pCaller->selector ();
}
