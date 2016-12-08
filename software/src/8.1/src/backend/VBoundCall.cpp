/*****  VBoundCall Implementation  *****/

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

#include "VBoundCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VBlockTask.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VBoundCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VBoundCall::MetaMaker () {
    CSym ("isABoundCall")->set (RTT, &g_siTrue);
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VBoundCall::VBoundCall (
    VTask* pCaller, rtLINK_CType* pCallerSubset, unsigned int iParameterCount, M_CPD *pBlock
) : VUtilityCall (pCaller, pCallerSubset, Type_Bound, iParameterCount), m_pBoundBlock (pBlock) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VBoundCall::~VBoundCall () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

rtBLOCK_Handle *VBoundCall::boundBlock () const {
    return static_cast<rtBLOCK_Handle*>(m_pBoundBlock->containerHandle ());
}
