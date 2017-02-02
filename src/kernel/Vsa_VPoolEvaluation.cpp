/*****  Vsa_VPoolEvaluation Implementation  *****/

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

#include "Vsa_VPoolEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32) //  MSVC Compiler Happy Pill
#include "Vca_VTimer.h"
#include "Vca_VTrigger.h"
#include "Vsa_VPoolBroadcast.h"
#include "Vsa_VPoolWorkerGeneration.h"
#endif

#include "Vsa_IEvaluatorClient.h"

#include "Vsa_VEvaluatorPool.h"





/**********************************
 **********************************
 *****                        *****
 *****  Vsa::VPoolEvaluation  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/ 

Vsa::VPoolEvaluation::VPoolEvaluation (
    VEvaluatorPool*	pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    Vca::U32		iValidGeneration,
    Vca::U32		iWorkerId
) : BaseClass (pEvaluator, pClient, rPath, rQuery)
, m_pIPoolEvaluation    (this)
, m_iValidGeneration	(iValidGeneration)
, m_iWorker		(iWorkerId)
, m_bAnyData            (false)
, m_bRetryable          (true)
{
    traceInfo ("Creating VPoolEvaluation");
}

Vsa::VPoolEvaluation::VPoolEvaluation (
    VEvaluatorPool*	pEvaluator,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rContext,
    Vca::U32		iValidGeneration,
    Vca::U32		iWorkerId
) : BaseClass (pEvaluator, pClient, rPath, rQuery, rContext)
, m_pIPoolEvaluation    (this)
, m_iValidGeneration	(iValidGeneration)
, m_iWorker		(iWorkerId)
, m_bAnyData            (false)
, m_bRetryable          (true)
{
    traceInfo ("Creating VPoolEvaluation");
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************
 *****  Vsa::IEvaluation  *****
 ******************************/

void Vsa::VPoolEvaluation::getRole (IEvaluation::Reference &rpRole) {
    IPoolEvaluation_Ex1::Reference pRole;
    m_pIPoolEvaluation.getRole (pRole);
    rpRole.setTo (pRole);
}

/**********************************
 *****  Vsa::IPoolEvaluation  *****
 **********************************/

void Vsa::VPoolEvaluation::getRole (IPoolEvaluation_Ex1::Reference &rpRole) {
    m_pIPoolEvaluation.getRole (rpRole);
}


void Vsa::VPoolEvaluation::UseAnyGenerationWorker (IPoolEvaluation *pRole) {
    m_bAnyData = true;
    VEvaluatorPool::Reference const pPool (evaluatorPool ());
    if (pPool) {
        Vca::U32 iOldestGeneration = pPool->oldestGenerationId ();
        if (m_iValidGeneration != iOldestGeneration) {
            m_iValidGeneration = iOldestGeneration;
            log ("Pool Request %d's valid generation changed to %d", m_xRequest, m_iValidGeneration);
            pPool->onAnyDataUpdateFor (this);
        }
    }
}

void Vsa::VPoolEvaluation::SetRetryable (IPoolEvaluation_Ex1 *pRole, bool bRetryable) {
    m_bRetryable = bRetryable;
    VEvaluatorPool::Reference const pPool (evaluatorPool ());
    if (pPool) {
        pPool->onRetryableUpdateFor (this);
    }
}

Vsa::VEvaluatorPool* Vsa::VPoolEvaluation::evaluatorPool () const {
  return dynamic_cast<VEvaluatorPool*> (m_pEvaluator.referent ());
}
