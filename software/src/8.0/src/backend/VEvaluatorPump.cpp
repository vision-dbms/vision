/*****  VEvaluatorPump Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VEvaluatorPump.h"

/************************
 *****  Supporting  *****
 ************************/

#include "vsignal.h"


/****************************************
 ****************************************
 *****                              *****
 *****  VEvaluatorPump::Evaluation  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VEvaluatorPump::Evaluation::Evaluation (
    VEvaluator *pEvaluator, IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
) : BaseClass (pEvaluator, pClient, rPath, rExpression) {
}

VEvaluatorPump::Evaluation::Evaluation (
    VEvaluator *pEvaluator, IEvaluatorClient* pClient, VString const& rPath, VString const& rQuery, VString const& rEnvironment
) : BaseClass (pEvaluator, pClient, rPath, rQuery, rEnvironment) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VEvaluatorPump::Evaluation::~Evaluation () {
}

/**********************************
 **********************************
 *****  Client Object Gofers  *****
 **********************************
 **********************************/

bool VEvaluatorPump::Evaluation::fulfill (icollection_gofer_t::Reference& rpGofer) {
    return cachedClientObjectGofer (rpGofer, m_pICollectionGofer);
}

bool VEvaluatorPump::Evaluation::fulfill (isingleton_gofer_t::Reference& rpGofer) {
    return cachedClientObjectGofer (rpGofer, m_pISingletonGofer);
}

bool VEvaluatorPump::Evaluation::createClientObjectGofer (icollection_gofer_t::Reference &rpGofer) {
    return createClientRootObjectGofer (rpGofer);
}

bool VEvaluatorPump::Evaluation::createClientObjectGofer (isingleton_gofer_t::Reference &rpGofer) {
    return createClientRootObjectGofer (rpGofer);
}


/****************************
 ****************************
 *****                  *****
 *****  VEvaluatorPump  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VEvaluatorPump::VEvaluatorPump (
    PumpSettings *pSettings, Vca::VBSConsumer *pPipeToPeer, Vca::VBSProducer *pPipeToHere
) : BaseClass (pSettings, pPipeToPeer, pPipeToHere) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VEvaluatorPump::~VEvaluatorPump () {
}

/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

VEvaluatorPump::VEvaluation *VEvaluatorPump::createEvaluation (
    IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
) {
    return new Evaluation (this, pClient, rPath, rExpression);
}

VEvaluatorPump::VEvaluation *VEvaluatorPump::createEvaluation (
    IEvaluatorClient* pClient, VString const& rPath, VString const& rQuery, VString const& rEnvironment
) {
    return new Evaluation (this, pClient, rPath, rQuery, rEnvironment);
}

/*************************
 *************************
 *****  Cancelation  *****
 *************************
 *************************/

void VEvaluatorPump::cancelCurrent () {
    SIGNAL_HandleInterrupt(0);
}
