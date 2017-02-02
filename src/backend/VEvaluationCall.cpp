/*****  VEvaluationCall Implementation  *****/

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

#include "VEvaluationCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VEvaluationCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VEvaluationCall::MetaMaker () {
    CSym ("isAnEvaluationCall")->set (RTT, &g_siTrue);
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VEvaluationCall::VEvaluationCall (
    VTask* pCaller, rtLINK_CType* pCallerSubset, unsigned int iParameterCount
) : VUtilityCall (pCaller, pCallerSubset, Type_Evaluation, iParameterCount)
{
}
