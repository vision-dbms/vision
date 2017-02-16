/*****  Vsa_VGenericEvaluation Implementation  *****/

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

#include "Vsa_VGenericEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_IEvaluatorClient.h"

#include "Vsa_VEvaluator.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vsa::VGenericEvaluation  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VGenericEvaluation::VGenericEvaluation (
    VEvaluator*		pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery
) : BaseClass (pEvaluator, pClient, rPath, rQuery), m_pIEvaluation (this) {
    traceInfo ("Creating VGenericEvaluation");
}

Vsa::VGenericEvaluation::VGenericEvaluation (
    VEvaluator*		pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rContext
) : BaseClass (pEvaluator, pClient, rPath, rQuery), m_pIEvaluation (this) {
    traceInfo ("Creating VGenericEvaluation");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VGenericEvaluation::~VGenericEvaluation () {
    traceInfo ("Destroying VGenericEvaluation");
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************
 *****  Vsa::IEvaluation  *****
 ******************************/

void Vsa::VGenericEvaluation::getRole (IEvaluation::Reference &rpRole) {
    m_pIEvaluation.getRole (rpRole);
}

