/*****  Vsa_VEvaluatorClient Implementation  *****/

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

#include "Vsa_VEvaluatorClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_IEvaluator_Ex1.h"
#include "Vsa_IPoolEvaluation.h"
#include "cam.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vsa::VEvaluatorClient  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorClient::VEvaluatorClient (
    Vca::VCohort *pCohort
) : BaseClass (pCohort), m_bUsingAnyData (true), m_pIEvaluatorClient (this) {
}

Vsa::VEvaluatorClient::VEvaluatorClient () : m_bUsingAnyData (true), m_pIEvaluatorClient (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorClient::~VEvaluatorClient () {
}

/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

bool Vsa::VEvaluatorClient::onQuery (IEvaluator *pEvaluator, VString const &rPath, VString const &rQuery, VString const &rEnvironment) {
    CAM_OPERATION(co) << "message" << "VEvaluatorClient::onQuery()";

    IEvaluatorClient::Reference pClientRole;
    if (!goodToGo (pEvaluator, pClientRole))
	return false;

    if (IEvaluator_Ex1 *const pEvaluator1 = dynamic_cast<IEvaluator_Ex1*>(pEvaluator)) {
        pEvaluator1->EvaluateURL_Ex (pClientRole, rPath, rQuery, rEnvironment, co.getId().c_str(), co.chainId().c_str());
    } else {
        pEvaluator->EvaluateURL (pClientRole, rPath, rQuery, rEnvironment);
    }
    return true;
}

bool Vsa::VEvaluatorClient::onQuery (IEvaluator *pEvaluator, VString const &rPath, VString const &rQuery) {
    CAM_OPERATION(co) << "message" << "VEvaluatorClient::onQuery()";

    IEvaluatorClient::Reference pClientRole;
    if (!goodToGo (pEvaluator, pClientRole))
	return false;

    if (IEvaluator_Ex1 *const pEvaluator1 = dynamic_cast<IEvaluator_Ex1*>(pEvaluator)) {
        pEvaluator1->EvaluateExpression_Ex (pClientRole, rPath, rQuery, co.getId().c_str(), co.chainId().c_str());
    } else {
        pEvaluator->EvaluateExpression (pClientRole, rPath, rQuery);
    }
    return true;
}

bool Vsa::VEvaluatorClient::onQuery (IEvaluator *pEvaluator, VString const &rQuery) {
    return onQuery (pEvaluator, VString (), rQuery);
}

bool Vsa::VEvaluatorClient::goodToGo (IEvaluator *pEvaluator, IEvaluatorClient::Reference &rpRole) {
    if (pEvaluator) {
	onStart ();
	monitorInterface (pEvaluator);

	getRole (rpRole);
	return true;
    }
    VString const iNoData ("Evaluator Not Available", false);
    onError (0, iNoData);
    return false;
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vsa::VEvaluatorClient::monitorInterface (IVUnknown *pInterface) {
    m_pInterfaceMonitor.setTo (new interface_monitor_t (this, &ThisClass::signalInterfaceMonitor, pInterface));
}

void Vsa::VEvaluatorClient::cancelInterfaceMonitor () {
    interface_monitor_t::Reference pInterfaceMonitor;
    pInterfaceMonitor.claim (m_pInterfaceMonitor);
    if (pInterfaceMonitor)
	pInterfaceMonitor->cancel ();
}

void Vsa::VEvaluatorClient::signalInterfaceMonitor () {
    onError (0, "Disconnected");
}


/**********************************
 **********************************
 *****  Role Implementations  *****
 **********************************
 **********************************/

/*******************************
 ***  Vsa::IEvaluatorClient  ***
 *******************************/

void Vsa::VEvaluatorClient::OnAccept (Vsa::IEvaluatorClient *pRole, Vsa::IEvaluation *pEvaluation, Vca::U32 xQueuePos){
    if (!usingAnyData ()) {
    } else if (IPoolEvaluation *const pPoolEvaluation = dynamic_cast<IPoolEvaluation*>(pEvaluation)) {
	pPoolEvaluation->UseAnyGenerationWorker ();
    }
    OnAccept_(pEvaluation, xQueuePos);
}

void Vsa::VEvaluatorClient::OnChange (Vsa::IEvaluatorClient *pRole, Vca::U32 xQueuePosition){
    OnChange_(xQueuePosition);
}

void Vsa::VEvaluatorClient::OnResult (Vsa::IEvaluatorClient *pRole, Vsa::IEvaluationResult *pResult, VString const &rOutput){
    cancelInterfaceMonitor ();
    OnResult_(pResult, rOutput);
    onSuccess ();
}

/*********************************
 ***  Default Implementations  ***
 *********************************/

void Vsa::VEvaluatorClient::OnAccept_(IEvaluation *pEvaluation, Vca::U32 xQueuePosition) {
}

void Vsa::VEvaluatorClient::OnChange_(Vca::U32 xQueuePosition) {
}

void Vsa::VEvaluatorClient::OnError_(Vca::IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}

void Vsa::VEvaluatorClient::onError (Vca::IError *pInterface, VString const &rMessage) {
    cancelInterfaceMonitor ();

    onFailure (pInterface, rMessage);
}
