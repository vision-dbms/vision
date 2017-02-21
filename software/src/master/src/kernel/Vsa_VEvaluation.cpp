/*****  Vsa_VEvaluation Implementation  *****/

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

#include "Vsa_VEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_IEvaluatorClient.h"
#include "Vsa_VEvaluator.h"
#include "Vca_VMessageManager.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::VEvaluation::Manager  *****
 *****                             *****
 ***************************************
 ***************************************/

namespace Vsa {
  class VEvaluation::Manager : public Vca::VMessageManager {
    DECLARE_CONCRETE_RTTLITE (Manager, Vca::VMessageManager);
    
  public:
    Manager (Vsa::IEvaluatorClient *pClient);
    
  protected:
    ~Manager ();
    
    //  VMessageManager Methods
  public:
    void onEnd ();
    void onError (Vca::IError *pError, VString const &rMessage);
    void onSent ();
    
  protected:
    Vsa::IEvaluatorClient::Reference m_pClient;
  };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluation::Manager::Manager (
    Vsa::IEvaluatorClient *pClient
) : m_pClient (pClient) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluation::Manager::~Manager () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vsa::VEvaluation::Manager::onEnd () {
  m_pClient->OnEnd ();
}

void Vsa::VEvaluation::Manager::onError (Vca::IError *pError, VString const &rMessage) {
  m_pClient->OnError (pError, rMessage);
}

void Vsa::VEvaluation::Manager::onSent () {
}


/******************************
 ******************************
 *****                    *****
 *****  Vsa::VEvaluation  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluation::VEvaluation (
    VEvaluator*		pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery
)
: BaseClass		(pClient)
, m_pEvaluator		(pEvaluator)
, m_xRequest		(pEvaluator->newRequestIndex ())
, m_pClient		(pClient)
, m_xQueryType		(QueryType_Expression)
, m_pQuery		(new VPathQuery (rPath, rQuery))
, m_xEvaluationAttempt  (0)
, m_pIEvaluatorClientEKG (new interface_monitor_t (this, &ThisClass::signalIEvaluatorClientEKG))
, m_bCancelled          (false)
{
    retain (); {
	m_pIEvaluatorClientEKG->monitor (pClient);
    } untain ();
    traceInfo ("Creating VEvaluation");
}

Vsa::VEvaluation::VEvaluation (
    VEvaluator*		pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rContext
)
: BaseClass		(pClient)
, m_pEvaluator		(pEvaluator)
, m_xRequest		(pEvaluator->newRequestIndex ())
, m_pClient		(pClient)
, m_xQueryType		(QueryType_URL)
, m_pQuery		(new VPathQuery (rPath, rQuery, rContext))
, m_xEvaluationAttempt  (0)
, m_pIEvaluatorClientEKG (new interface_monitor_t (this, &ThisClass::signalIEvaluatorClientEKG))
, m_bCancelled          (false)
{
    retain (); {
	m_pIEvaluatorClientEKG->monitor (pClient);
    } untain ();
    traceInfo ("Creating VEvaluation");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluation::~VEvaluation () {
    traceInfo ("Destroying VEvaluation");
}


/******************************
 ******************************
 *****  IRequest Methods  *****
 ******************************
 ******************************/

void Vsa::VEvaluation::cancel () {
    m_bCancelled = true;
    if (m_pEvaluator) m_pEvaluator->cancel (this);
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vsa::VEvaluation::getDescription_(VString &rResult) const {
    BaseClass::getDescription_(rResult);
    rResult << ": " << queryString ();
}

void Vsa::VEvaluation::getVisionExpression (
    VPathQuery::Formatter const &rFormatter, VString &rResult
) {
    switch (m_xQueryType) {
    case QueryType_URL:
	m_pQuery->getVisionExpression (rFormatter, rResult);
	break;
    default:
	rResult.setTo (queryString ());
    }
}

/********
 * Note:
 *	Returns the time taken for the evaluation in milliseconds. If the 
 * evaluation hasnt completed, returns zero
 ****/
Vca::U64 Vsa::VEvaluation::evaluationTime () const {
    return m_iEndTime > m_iStartTime ? (m_iEndTime - m_iStartTime) / 1000 : 0;
}

/***************************
 ***************************
 *****  Client Access  *****
 ***************************
 ***************************/

bool Vsa::VEvaluation::fulfill (GoferOrder &rOrder) {
    return rOrder.fulfillUsing (this);
}

/************************
 ************************
 *****  Delegation  *****
 ************************
 ************************/

void Vsa::VEvaluation::delegateTo (IEvaluatorClient *pClient, IEvaluator *pEvaluator) const {

  Manager::Reference const pManager (new Manager (pClient));
  switch (m_xQueryType) {
  case QueryType_URL:
    pEvaluator->EvaluateURL (pClient, pathString (), queryString (), environmentString (), pManager);
    break;
  default:
    pEvaluator->EvaluateExpression (pClient, pathString (), queryString (), pManager);
  }
}


/*******************************
 *******************************
 *****  Result Generation  *****
 *******************************
 *******************************/

void Vsa::VEvaluation::onAccept (Vca::U32 xQueuePosition) {
    if (m_xEvaluationAttempt > 0)
	m_pClient->OnChange (xQueuePosition);
    else {
	onStart ();

	IEvaluation::Reference pRole;
	getRole (pRole);
	m_pClient->OnAccept (pRole, xQueuePosition);
    }
}

void Vsa::VEvaluation::onChange (Vca::U32 xQueuePosition) {
    m_pClient->OnChange (xQueuePosition);
}

void Vsa::VEvaluation::onError (Vca::IError *pError, VString const &rText) {
    cancelIEvaluatorClientEKG ();
    onFailure (pError, rText);

    m_iEndTime.setToNow ();
    m_pClient->OnError (pError, rText);
}

void Vsa::VEvaluation::onComplete () {
    cancelIEvaluatorClientEKG ();
    m_iEndTime.setToNow ();
    m_pEvaluator->updateQueryStatistics (1, evaluationTime ());
}

void Vsa::VEvaluation::onResult (
    IEvaluationResult *pResult, VString const &rOutput
) {
    onSuccess ();

    onComplete ();
    m_pClient->OnResult (pResult, rOutput);
}


/*****************
 *****  EKG  *****
 *****************/

void Vsa::VEvaluation::signalIEvaluatorClientEKG () {
    traceInfo ("IEvaluatorClient disappeared.");

    // Cancel the EKG.
    cancelIEvaluatorClientEKG ();

    // Handle option to suppress cancellation.
    if (IsntNil (getenv ("VsaNoCancelOnDetach"))) return;

    // Cancel the evaluation.
    log ("IEvaluatorClient for VEvaluation (%d) no longer connected; cancelling query.", m_xRequest);
    cancel ();
}

void Vsa::VEvaluation::cancelIEvaluatorClientEKG () {
    if (m_pIEvaluatorClientEKG.isntNil ()) {
        m_pIEvaluatorClientEKG->cancel ();
        m_pIEvaluatorClientEKG.clear ();
    }
}
