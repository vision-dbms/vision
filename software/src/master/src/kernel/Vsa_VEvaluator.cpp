/*****  Vsa_VEvaluator Implementation  *****/

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

#include "Vsa_VEvaluator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_IEvaluatorClient.h"
#include "Vsa_VGenericEvaluation.h"


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vsa::VEvaluator::UpDownSubscriber  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/*************************
 *************************
 *****  Constructor  *****
 *************************
 *************************/

Vsa::VEvaluator::UpDownSubscriber::UpDownSubscriber (VEvaluator *pEvaluator, IUpDownSubscriber *pSubscriber)
: m_pEvaluator (pEvaluator)
, m_pSubscriber (pSubscriber)
, m_pISubscription (this) {
  traceInfo ("Creating VEvaluator::UpDownSubscriber");
  m_pEvaluator->link (this);
  m_bIsSubscribed = true;
}
    
/************************
 ************************
 *****  Destructor  *****
 ************************
 ************************/

Vsa::VEvaluator::UpDownSubscriber::~UpDownSubscriber () {
  traceInfo ("Destroying VEvaluator::UpDownSubscriber");
  if (m_bIsSubscribed) {
    m_bIsSubscribed = false;
    m_pEvaluator->unlink (this);
  }
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vsa::VEvaluator::UpDownSubscriber::Unsubscribe (ISubscription *pRole) {
  //  check to see if already unsubscribed...
  if (m_bIsSubscribed) {
    m_bIsSubscribed = false;
    m_pEvaluator->unlink (this);
  }

  //  clear the cyclic reference between the remote subscriber and this object
  m_pSubscriber.clear ();  
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vsa::VEvaluator::Queue::Entry  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluator::Queue::Entry::Entry (VEvaluation *pRequest) : m_pRequest (pRequest) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluator::Queue::Entry::~Entry () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

unsigned int Vsa::VEvaluator::Queue::Entry::requestIndex () const {
    return m_pRequest->index ();
}

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vsa::VEvaluator::Queue::Entry::unlink (
    Queue *pQueue, VReference<VEvaluation> &rpRequest
) {
    retain ();
    rpRequest.setTo (m_pRequest);
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else
	pQueue->m_pHead.setTo (m_pSuccessor);
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);
    else
	pQueue->m_pTail.setTo (m_pPredecessor);
    m_pPredecessor.clear ();
    m_pSuccessor.clear ();
    release ();
}


/************************************
 ************************************
 *****                          *****
 *****  Vsa::VEvaluator::Queue  *****
 *****                          *****
 ************************************
 ************************************/

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vsa::VEvaluator::Queue::contains (
    VEvaluation *pRequest, VReference<Entry> &rpEntry
) const {
    VReference<Entry> pEntry (m_pHead);
    while (pEntry) {
	if (pEntry->request () == pRequest) {
	    rpEntry.setTo (pEntry);
	    return true;
	}
	pEntry.setTo (pEntry->m_pSuccessor);
    }
    rpEntry.clear ();
    return false;
}


/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vsa::VEvaluator::Queue::enqueue (VEvaluation *pRequest) {
    Entry *pNewEntry = new Entry (pRequest);
    m_sQueue++;
    if (empty ()) {
	m_pHead.setTo (pNewEntry);
	m_pTail.setTo (pNewEntry);
	pRequest->onAccept (1);
    }
    else {
	unsigned int xRequestIndex = pRequest->index ();
	if (xRequestIndex >= m_pTail->request ()->index ()) {
	    pNewEntry->m_pPredecessor.setTo (m_pTail);
	    m_pTail->m_pSuccessor.setTo (pNewEntry);
	    m_pTail.setTo (pNewEntry);
	    pRequest->onAccept (m_sQueue);
	}
	else if (xRequestIndex < m_pHead->requestIndex ()) {
	    pNewEntry->m_pSuccessor.setTo (m_pHead);
	    m_pHead->m_pPredecessor.setTo (pNewEntry);
	    m_pHead.setTo (pNewEntry);
	    pRequest->onAccept (1);
	}
	else {
	    Vca::U32 xQueuePosition = 2;
	    Entry *pCandidate = m_pHead;
	    while (xRequestIndex > pCandidate->m_pSuccessor->requestIndex ()) {
		pCandidate = pCandidate->m_pSuccessor;
		xQueuePosition++;
	    }
	    pNewEntry->m_pPredecessor.setTo (pCandidate);
	    pNewEntry->m_pSuccessor.setTo (pCandidate->m_pSuccessor);
	    pCandidate->m_pSuccessor->m_pPredecessor.setTo (pNewEntry);
	    pCandidate->m_pSuccessor.setTo (pNewEntry);
	    pRequest->onAccept (xQueuePosition);
	}
    }
}

void Vsa::VEvaluator::Queue::dequeue (VReference<VEvaluation> &rpRequest) {
    if (m_pHead.isNil ())
	rpRequest.clear ();
    else {
	m_pHead->unlink (this, rpRequest);
	rpRequest->onChange (0);
	m_sQueue--;

	// inform remaining evaluations about change in queue position
	Vca::U32 xQueuePosition = 1;
	VReference<Entry> pEntry (m_pHead);
	while (pEntry.isntNil ()) {
	    pEntry->m_pRequest->onChange (xQueuePosition);
	    xQueuePosition++;
	    pEntry.setTo (pEntry->m_pSuccessor);
	}
    }
}

bool Vsa::VEvaluator::Queue::unlink (VEvaluation *pRequest) {
    VReference<Entry> pEntry;
    VReference<VEvaluation> pEvaluation;

    if (contains (pRequest, pEntry)) {
	pEntry->unlink (this, pEvaluation);
	m_sQueue--;
        return true;
    }
    return false;
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vsa::VEvaluator::Queue::Iterator  *****
 *****                                    *****
 **********************************************
 **********************************************/

Vsa::VEvaluator::Queue::Entry const *Vsa::VEvaluator::Queue::Iterator::operator* () const { 
    Vca::U32 xIndex = 0;
    VReference<Entry> pEntry (m_rQueue.m_pHead);
    while (pEntry.isntNil () && xIndex++ < m_xElement) 
	pEntry.setTo (pEntry->m_pSuccessor);
    return pEntry.referent ();
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::VEvaluator::Settings  *****
 *****                             *****
 ***************************************
 ***************************************/

/******************
 ******************
 *****  Role  *****
 ******************
 ******************/

/*************************************
 *****  Vsa::IEvaluatorSettings  *****
 *************************************/

void Vsa::VEvaluator::Settings::GetEvaluationTimeOut (
    IEvaluatorSettings *pRole, IVReceiver<Vca::U64>* pClient
) { 
    pClient->OnData (m_cEvaluationTimeOut);
}

void Vsa::VEvaluator::Settings::SetEvaluationTimeOut (
    IEvaluatorSettings *pRole, Vca::U64 sTimeOut
) {
    m_cEvaluationTimeOut = sTimeOut;
}


/*******************
 *******************
 *****  Access *****
 *******************
 *******************/

void Vsa::VEvaluator::Settings::dumpSettings (VString &rResult) const {
    rResult << "EvaluationTimeOut | " << evaluationTimeOutInSecs () << "\n";
}
    


/*****************************
 *****************************
 *****                   *****
 *****  Vsa::VEvaluator  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluator::VEvaluator () 
:   m_xNextRequest	(0)
,   m_bProcessingQueue  (false)
,   m_cQueriesProcessed	(0)
,   m_cTotalQueryTime	(0)
,   m_iStatus		(Status_Running)
,   m_cSubscribers      (0)
,   m_pIEvaluator	(this)
,   m_pIUpDownPublisher (this)
{
    traceInfo ("Creating VEvaluator");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluator::~VEvaluator () {
    traceInfo ("Destroying VEvaluator");
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vsa::VEvaluator::suspend () {
  notifyAllUpDownSubscribers (false);
  m_iStatus = Status_Suspended;
}

void Vsa::VEvaluator::resume  () {
  notifyAllUpDownSubscribers (true);
  m_iStatus = Status_Running;
}

void Vsa::VEvaluator::stop    () {
  m_iStatus = Status_Stopping;
}

void Vsa::VEvaluator::hardstop () {
  notifyAllUpDownSubscribers (false);
  m_iStatus = Status_Stopped;
}

void Vsa::VEvaluator::restart () {
  notifyAllUpDownSubscribers (true);
  m_iStatus = Status_Running;
  m_iLastRestarted.setToNow ();
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*****************************
 *****  Vsa::IEvaluator  *****
 *****************************/

void Vsa::VEvaluator::getRole (VReference<IEvaluator>&rpRole) {
    m_pIEvaluator.getRole (rpRole);
}

void Vsa::VEvaluator::EvaluateExpression (
    IEvaluator*		pRole,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rExpression
) {
  m_iLastUsed.setToNow ();
  if (m_iStatus == Status_Stopped) {
    if (pClient)
      pClient->OnError (0, "Evaluator is already stopped");
  }
  else 
    evaluate (pClient, rPath, rExpression);
}

void Vsa::VEvaluator::EvaluateURL (
    IEvaluator*		pRole,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rEnvironment
) {
  m_iLastUsed.setToNow ();
  if (m_iStatus == Status_Stopped) {
    if (pClient)
      pClient->OnError (0, "Evaluator is already stopped");
  }
  else 
    evaluate (pClient, rPath, rQuery, rEnvironment);
}


/********************************************
 *****  Vsa::IEvaluatorControl Methods  *****
 ********************************************/

void Vsa::VEvaluator::GetEvaluator (IEvaluatorControl *pRole, IEvaluatorSink *pSink) {
    if (pSink) {
	VReference<IEvaluator> pIEvaluator;
	getRole (pIEvaluator);
	pSink->OnData (pIEvaluator);
    }
}

void Vsa::VEvaluator::Suspend (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver) {
    suspend ();
    if (pReceiver)
	pReceiver->OnData (true);
}

void Vsa::VEvaluator::Resume (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver) {
    resume ();
    if (pReceiver)
	pReceiver->OnData (true);
}

void Vsa::VEvaluator::Restart (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver) {
    restart ();
    if (pReceiver)
	pReceiver->OnData (true);
}

void Vsa::VEvaluator::Stop (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver) {
    stop ();
    if (pReceiver)
	pReceiver->OnData (true);
}
void Vsa::VEvaluator::GetStatistics (
	IEvaluatorControl *pRole, IVReceiver<VString const&> *pReceiver
) {
    VString rResult;
    getStatistics (rResult);
    pReceiver->OnData (rResult);
}

void Vsa::VEvaluator::GetWorker (IEvaluatorControl* pControl, IVReceiver<IVUnknown*>* pReceiver, Vca::U32 cWorker) {
    //  provided for compatibility
    pReceiver->OnData (0);
}

/***********************************
 *****  Vsa::IUpDownPublisher  *****
 ***********************************/

void Vsa::VEvaluator::getRole (VReference<IUpDownPublisher>&rpRole) {
    m_pIUpDownPublisher.getRole (rpRole);
}

void Vsa::VEvaluator::Subscribe (
   IUpDownPublisher* pRole, IUpDownSubscriber *pSubscriber,
   ISubscriptionSink* pSubscriptionSink
) {
  
  //  create/link a local subscriber representing remote subscriber
  VReference<Subscriber> pLocalSub (new Subscriber (this, pSubscriber));

  //  send remote subscriber, the subscription token
  VReference<ISubscription> pSubscription;
  pLocalSub->getRole (pSubscription);
  pSubscriptionSink->OnData (pSubscription);

  //  send remote subscriber the current status
  notifyUpDownSubscriber (pLocalSub, isOperational ());
}

/*************************
 *************************
 *****  Evaluation   *****
 *************************
 *************************/

void Vsa::VEvaluator::evaluate (
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rExpression
) {
    schedule (createEvaluation (pClient, rPath, rExpression));
}

void Vsa::VEvaluator::evaluate (
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rEnvironment
) {
    schedule (createEvaluation (pClient, rPath, rQuery, rEnvironment));
}

bool Vsa::VEvaluator::cancel (VEvaluation *pEvaluation) {
    // If there's no evaluation to cancel, do nothing.
    if (!pEvaluation) return false;

    // Find the evaluation in the queue, cancel if found.
    bool bFound = m_iQueue.unlink (pEvaluation);
    if (bFound) pEvaluation->onError(0, "Cancelled.");

    // Delegate to subclass.
    else bFound = cancel_ (pEvaluation);

    return bFound;
}

Vsa::VEvaluation* Vsa::VEvaluator::createEvaluation (
    IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
) {
    return new VGenericEvaluation (this, pClient, rPath, rExpression);
}

Vsa::VEvaluation* Vsa::VEvaluator::createEvaluation (
    IEvaluatorClient* pClient,
    VString const& rPath, VString const& rQuery, VString const& rEnvironment
) {
    return new VGenericEvaluation (this, pClient, rPath, rQuery, rEnvironment);
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vsa::VEvaluator::onQueue () {

  //  this is old graceful stop implementation code....need to be removed
  if (m_iStatus == Status_Stopping && queueLength ()==0) {
    hardstop ();
    return;
  }

  switch (m_iStatus) {
  case Status_Running:
  case Status_Stopping:
  case Status_Stopped:
    if (m_bProcessingQueue == false) { //reentrant guard...
      m_bProcessingQueue = true;
      onQueue_ ();
      m_bProcessingQueue = false;
    }

    break;

  case Status_Suspended:
    //  nothing to do...
    break;
  }
}


void Vsa::VEvaluator::onError (Vca::IError *pError, VString const &rMessage) {
    VEvaluation *pRequest = thisRequest ();
    while (pRequest) {
	pRequest->onError (pError, rMessage);
	pRequest = nextRequest ();
    }
}

void Vsa::VEvaluator::schedule (VEvaluation *pRequest) {

    switch (m_iStatus) {
    case Status_Stopped:
	pRequest->onError (0, "Evaluator is stopped. Your query is not processed");
        break;

    case Status_Running:
    case Status_Stopping:
    case Status_Suspended:

      onRequest (pRequest);
      if (m_pThisRequest) {
	m_iQueue.enqueue (pRequest);
      }
      else {
	m_pThisRequest.setTo (pRequest);
	pRequest->onAccept (0);
      }
      onQueue ();
      break;
    }
}

Vsa::VEvaluation *Vsa::VEvaluator::nextRequest () {
    m_iQueue.dequeue (m_pThisRequest);
    return m_pThisRequest;
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vsa::VEvaluator::notifyUpDownSubscriber (VReference<UpDownSubscriber> pSubscriber, bool bEvaluatorUp) const {
  VReference<IUpDownSubscriber> pRemote;
  pRemote.setTo (pSubscriber->getIUpDownSubscriber ());
  if (pRemote) 
    bEvaluatorUp ? pRemote->OnUp (): pRemote->OnDown ();
}

void Vsa::VEvaluator::notifyAllUpDownSubscribers (bool bEvaluatorUp) const {
  
  if ((isOperational () && !bEvaluatorUp) || (!isOperational () && bEvaluatorUp)) {
    VReference<UpDownSubscriber> pSubscriber (m_pSubscriberList);
    while (pSubscriber) {
	notifyUpDownSubscriber (pSubscriber, bEvaluatorUp);
	pSubscriber.setTo (pSubscriber->m_pSuccessor);
    }
  }
}

void Vsa::VEvaluator::updateQueryStatistics (
    Vca::U32 cQueries, Vca::U64 iQueryTime
) {
    m_cQueriesProcessed += cQueries;
    m_cTotalQueryTime += iQueryTime;
}

/**********************
 **********************
 *****  Linking   *****
 **********************
 **********************/

// Need to move the link/unlink to Subscriber class...

void Vsa::VEvaluator::link (Subscriber *const pSubscriber) {
    if (pSubscriber) {
        m_cSubscribers++;
        pSubscriber->m_pSuccessor.setTo (m_pSubscriberList);
	if (m_pSubscriberList)
	    m_pSubscriberList->m_pPredecessor.setTo (pSubscriber);
	m_pSubscriberList.setTo (pSubscriber);
        log ("New Subscriber linked. Current subscriber count %d", m_cSubscribers);
    }
}

void Vsa::VEvaluator::unlink (Subscriber *const pSubscriber) {

    if (pSubscriber) {
        m_cSubscribers--;

	//  if head
	if (pSubscriber == m_pSubscriberList.referent ()) {
	    m_pSubscriberList.setTo (pSubscriber->m_pSuccessor);
	    if (m_pSubscriberList)
		m_pSubscriberList->m_pPredecessor.clear ();
	}
	else {
	    pSubscriber->m_pPredecessor->m_pSuccessor.setTo (pSubscriber->m_pSuccessor);
	    if (pSubscriber->m_pSuccessor)
		pSubscriber->m_pSuccessor->m_pPredecessor.setTo (pSubscriber->m_pPredecessor);
	}
	pSubscriber->m_pSuccessor.clear ();
	pSubscriber->m_pPredecessor.clear ();
        log ("Subscriber unlinked. Current subscriber count %d", m_cSubscribers);
    }
}

/************************
 *****  Statistics  *****
 ************************/

void Vsa::VEvaluator::getStatistics (VString &rResult) const {

  VString iStatus;
  switch (m_iStatus) {
  case Status_Running:
    iStatus << "Running";
    break;
  case Status_Suspended:
    iStatus << "Suspended";
    break;
  case Status_Stopping:
    iStatus << "Stopping";
    break;
  case Status_Stopped:
    iStatus << "Stopped";
    break;
  }

  //  format the start, last restart, last used times...
  char iTimestring[26]; VString iStarted ("-\n"), iLastRestarted ("-\n"), iLastUsed ("-\n");

  if (m_iStarted.ctime (iTimestring))
    iStarted.setTo (iTimestring);
  
  if (m_iLastRestarted != m_iStarted && m_iLastRestarted.ctime(iTimestring))
    iLastRestarted.setTo (iTimestring);
  
  if (m_iLastUsed != m_iStarted && m_iLastUsed.ctime (iTimestring))
    iLastUsed.setTo (iTimestring);
  
  Vca::U64 iAvgQueryTime = 0;
  if (queriesProcessed () > 0)
    iAvgQueryTime = totalQueryTime ()/ queriesProcessed ();

  rResult << "\nStarted                        : " << iStarted;
  rResult << "Last Restarted                 : " << iLastRestarted;
  rResult << "Last Query Arrived             : " << iLastUsed; 
  rResult << "Current Status                 : " << iStatus << "\n";
  rResult << "Current Queue Length           : " << queueLength () << "\n";
  rResult << "Number of Queries Processed    : " << queriesProcessed () << "\n";
  rResult << "Average Time Per Query         : " << iAvgQueryTime << " milliseconds.\n";
}

void Vsa::VEvaluator::dumpStats (VString &rResult) const {
  VString iStatus;
  switch (m_iStatus) {
  case Status_Running:
    iStatus << "Running";
    break;
  case Status_Suspended:
    iStatus << "Suspended";
    break;
  case Status_Stopping:
    iStatus << "Stopping";
    break;
  case Status_Stopped:
    iStatus << "Stopped";
    break;
  }
  
  rResult << "Status | " << iStatus << "\n";
  rResult << "QueueLength | " << queueLength () << "\n";
  rResult << "QueriesProcessed | " << queriesProcessed () << "\n";
}
