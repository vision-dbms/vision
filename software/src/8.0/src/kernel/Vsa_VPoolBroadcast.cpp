/*****  Vsa_VPoolBroadcast Implementation  *****/

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

#include "Vsa_VPoolBroadcast.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_VEvaluatorPool.h"
#include "Vsa_VPoolBroadcastEvaluation.h"

#if defined(_WIN32) //  MSVC Compiler Happy Pill
#include "Vca_VTimer.h"			// omit:Linux omit:SunOS
#include "Vca_VTrigger.h"		// omit:Linux omit:SunOS
#include "Vsa_VPoolWorkerGeneration.h"	// omit:Linux omit:SunOS
#endif

/*********************************
 *********************************
 *****                       *****
 *****  Vsa::VPoolBroadcast  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/ 

Vsa::VPoolBroadcast::VPoolBroadcast (
    VEvaluatorPool*	pPool,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery
) 
: m_pPool      (pPool)
, m_pClient    (pClient)
, m_xQueryType (QueryType_Expression)
, m_cExpectedReplies (0)
, m_pIEvaluatorClient (this)
{
    traceInfo ("Creating Vsa::VPoolBroadcast");
    m_iPathQuery.setTo (new VPathQuery (rPath, rQuery));
}

Vsa::VPoolBroadcast::VPoolBroadcast (
    VEvaluatorPool*	pPool,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rContext
)
: m_pPool   (pPool)
, m_pClient (pClient)
, m_xQueryType (QueryType_URL)
, m_cExpectedReplies (0)
, m_pIEvaluatorClient (this)
{
    traceInfo ("Creating Vsa::VPoolBroadcast");
    m_iPathQuery.setTo (new VPathQuery (rPath, rQuery, rContext));
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/ 

Vsa::VPoolBroadcast::~VPoolBroadcast () {
    traceInfo ("Destroying Vsa::VPoolBroadcast");
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/***********************************
 *****  Vsa::IEvaluatorClient  *****
 ***********************************/

void Vsa::VPoolBroadcast::getRole (VReference<IEvaluatorClient>&rpRole) {
    m_pIEvaluatorClient.getRole (rpRole);
}

void Vsa::VPoolBroadcast::OnResult (
    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
) {
    m_iResult << rOutput;
    m_cExpectedReplies--;
    m_pPool->decrementBCEvaluationsInProgress ();

    if (m_cExpectedReplies == 0)
	onComplete ();
}


void Vsa::VPoolBroadcast::OnAccept (
    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
) {
}

void Vsa::VPoolBroadcast::OnChange (
    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
) {
}


void Vsa::VPoolBroadcast::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rText
) {
    m_cExpectedReplies--;
    m_iResult << rText;
    log ("Pool Broadcast Evaluation Error");
    m_pPool->decrementBCEvaluationsInProgress ();

    if (m_cExpectedReplies == 0)
	onComplete ();

}

/*******************************************
 *****  Evaluation-Cancelled CallBack  *****
 *******************************************/

void Vsa::VPoolBroadcast::OnEvaluationCancelled (VString const& rMessage) {
    m_cExpectedReplies--;
    m_iResult << rMessage;
    log ("Pool Broadcast Evaluation Cancelled");

    if (m_cExpectedReplies == 0)
	onComplete ();
}

/******************************
 *****  VPoolEvaluations  *****
 ******************************/


Vsa::VPoolEvaluation* Vsa::VPoolBroadcast::createBCEvaluationFor (Vca::U32 iWorkerId) {
    VReference<IEvaluatorClient> pLocalClient;
    getRole (pLocalClient);
	
    VPoolEvaluation *pEvaluation;

    switch (m_xQueryType) {
    case QueryType_Expression:
	pEvaluation = new VPoolBroadcastEvaluation (this, pLocalClient,
    			   m_iPathQuery->pathString (), m_iPathQuery->queryString ());
	pEvaluation->setWorker (iWorkerId);

	break;
    case QueryType_URL:
	pEvaluation = new VPoolBroadcastEvaluation (this, pLocalClient,
			    m_iPathQuery->pathString (), m_iPathQuery->queryString (),
			    m_iPathQuery->environmentString () );
	pEvaluation->setWorker (iWorkerId);

	break;
    default:
	break;
    } //  end switch

    return pEvaluation;
}

/************************
 *****  onComplete  *****
 ************************/


void Vsa::VPoolBroadcast::onComplete () {
    m_pClient->OnResult (0, m_iResult);
    m_pPool->removeBCObject (this);
    //  this should be destroyed here....
}
