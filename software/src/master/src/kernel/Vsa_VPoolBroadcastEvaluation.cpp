
/*****  Vsa_VPoolBroadcastEvaluation Implementation  *****/

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

#include "Vsa_VPoolBroadcastEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_VPoolBroadcast.h"


#if defined(_WIN32) //  MSVC Compiler Happy Pill
#include "Vca_VTimer.h"			// omit:Linux omit:SunOS
#include "Vca_VTrigger.h"		// omit:Linux omit:SunOS
#include "Vsa_VEvaluatorPool.h"
#include "Vsa_VPoolWorkerGeneration.h"	// omit:Linux omit:SunOS
#endif

/*******************************************
 *******************************************
 *****                                 *****
 *****  Vsa::VPoolBroadcastEvaluation  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/ 

Vsa::VPoolBroadcastEvaluation::VPoolBroadcastEvaluation (
    VPoolBroadcast*	pBroadcast,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    Vca::U32		iValidGeneration,
    Vca::U32		iWorkerId
)
: VPoolEvaluation (pBroadcast->pool (), pClient, rPath, rQuery, iValidGeneration, iWorkerId)
, m_pPoolBroadcast (pBroadcast)
{
    traceInfo ("Creating VPoolBroadcastEvaluation");
}

Vsa::VPoolBroadcastEvaluation::VPoolBroadcastEvaluation (
    VPoolBroadcast*	pBroadcast,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rContext,
    Vca::U32		iValidGeneration,
    Vca::U32		iWorkerId
) 
: VPoolEvaluation (pBroadcast->pool (), pClient, rPath, rQuery, rContext, iValidGeneration, iWorkerId)
, m_pPoolBroadcast (pBroadcast)
{
    traceInfo ("Creating VPoolBroadcastEvaluation");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/ 

Vsa::VPoolBroadcastEvaluation::~VPoolBroadcastEvaluation () {
    traceInfo ("Destroying Vsa::VPoolBroadcastEvaluation");
}

