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
