/*****  Vsa_VPoolWorker Implementation  *****/

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

#include "Vsa_VPoolWorkerGeneration.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32) //  MSVC Compiler Happy Pill
#include "Vsa_CompilerHappyPill.h"
#endif

#include "VSimpleFile.h"
#include "Vsa_VEvaluatorPool.h"
#include "Vsa_VPoolWorker.h"
#include "Vsa_VPoolEvaluation.h"
#include "Vsa_VPoolBroadcast.h"

/****************************************
 ****************************************
 *****                              *****
 *****  Vsa::VPoolWorkerGeneration  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolWorkerGeneration::VPoolWorkerGeneration (
    VEvaluatorPool *pEvaluatorPool, Vca::U32 iGeneration, Vca::U32 iNextWorker
)
: m_iGeneration                 (iGeneration)
, m_iNextWorker                 (iNextWorker)
, m_bHiring                     (false)
, m_pPool                       (pEvaluatorPool)
, m_cOnlineWorkers              (0)
, m_cOnlineAvailWorkers         (0)
, m_cOnlineWIPs                 (0)
, m_cOnlineExcessWorkers        (0)
, m_cWorkersBeingCreated        (0)
, m_cDisabledWorkers            (0)
, m_cWorkerCreationHardFailures (0)
, m_cWorkerCreationSoftFailures (0)
, m_cWorkersRetired             (0)
, m_cOfflineWorkers             (0)
, m_cOfflineAvailWorkers        (0)
, m_cOfflineWIPs                (0)
, m_cOfflineExcessWorkers       (0)
, m_pSuccessor                  (0)
, m_pPredecessor                (0)
{
    traceInfo ("Creating VPoolWorkerGeneration");
    m_pPool->attach (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolWorkerGeneration::~VPoolWorkerGeneration () {
    traceInfo ("Destroying VPoolWorkerGeneration");
    m_pPool->detach (this);
}

/*******************
 *****  Query  *****
 *******************/

bool Vsa::VPoolWorkerGeneration::isCurrent () const {
    return m_pPool->currentGenerationId () == generationId ();
}

/**
 * Indicates whether this worker generation is capable of hiring worker based on the pool parameter settings
 */
bool Vsa::VPoolWorkerGeneration::canHireWorker () const {
    return
        //  Pool Hiring Boundary Cond.
        m_pPool->isOperational ()  &&
        m_pPool->workerCreationSupported () &&
        m_pPool->workersBeingCreated () < m_pPool->workersBeingCreatedMaximum () &&

        //  Generation Hiring Boundary Cond.
        isCurrent () &&
        isWorkerCreationFailureHardLmtNotReached () &&
        m_cOnlineWorkers + m_cWorkersBeingCreated < m_pPool->workerMaximum ();
}

/**
 * Indicates whether this worker generation should hire a new worker based on the pool parameter settings.
 * Always returns false if canHireWorker() returns false.
 * @todo Add hire reason to worker metadata.
 */
bool Vsa::VPoolWorkerGeneration::shouldHireWorker () const {
    // Return false right away if we can't hire at all.
    if (!canHireWorker ()) return false;

    //  Reason for Hiring
    if (m_cOnlineAvailWorkers + m_cWorkersBeingCreated < m_pPool->workerMinimumAvailable ()) {
        // Hiring to maintain minimum available workers.
        return true;
    }
    if (m_cOnlineWorkers + m_cWorkersBeingCreated < m_pPool->workerMinimum ()) {
        // Hiring to maintain minimum workers.
        return true;
    }
    if (m_cOnlineAvailWorkers + m_cWorkersBeingCreated < m_pPool->queueLength ()) {
        // Hiring to process requests in queue.
        log ("Hiring worker to process %d requests in queue.", m_pPool->queueLength());
        return true;
    }

    // No particular reason to hire found.
    return false;
}

/**
 * Indicates whether a free available worker can be employed in this pool generation.
 * Depends on the generation and the workers current attributes.
 */
bool Vsa::VPoolWorkerGeneration::isWorkerEmployable (Worker *pWorker) const {
    bool bEmployable = false;
    switch (pWorker->workerMode ()) {
    case Worker::Worker_Online:
      bEmployable = (m_cOnlineExcessWorkers > 0 || pWorker->isFlushed () || pWorker->isAboutToRetire ()) ? false : true;
      break;
    case Worker::Worker_Offline:
      bEmployable = (m_cOfflineExcessWorkers > 0 || pWorker->isAboutToRetire ()) ? false : true;
      break;
    }
    return bEmployable;
}

bool Vsa::VPoolWorkerGeneration::isWorkerCreationFailureHardLmtNotReached () const {
  return !isWorkerCreationFailureHardLmtReached ();
}

bool Vsa::VPoolWorkerGeneration::isWorkerCreationFailureHardLmtReached () const {
  return m_cWorkerCreationHardFailures >= m_pPool->workerCreationFailureHardLimit ();
}

bool Vsa::VPoolWorkerGeneration::isWorkerCreationFailureSoftLmtNotReached () const {
  return !isWorkerCreationFailureSoftLmtReached ();
}

bool Vsa::VPoolWorkerGeneration::isWorkerCreationFailureSoftLmtReached () const {
  return m_cWorkerCreationSoftFailures >= m_pPool->workerCreationFailureSoftLimit ();
}

bool Vsa::VPoolWorkerGeneration::canProcess (VPoolEvaluation const *pEvaluation) const {
  return pEvaluation->anyData () || m_iGeneration >= pEvaluation->validGeneration ();
}

bool Vsa::VPoolWorkerGeneration::isProcessing (Vca::U32 xEvaluation, VReference<WIP> &rpWIP) const {
    bool bIsProcessing = false;
    VReference<WIP> pWIP (m_pOnlineWIPHead);
    while (!bIsProcessing && pWIP) {
        if (xEvaluation == pWIP->evaluationIndex ()) {
            rpWIP.setTo (pWIP);
            bIsProcessing = true;
        }
        pWIP.setTo (pWIP->m_pSuccessor);
    }
    return bIsProcessing;
}

bool Vsa::VPoolWorkerGeneration::isProcessing (VPoolEvaluation const *pEvaluation, VReference<WIP> &rpWIP) const {
    if (pEvaluation) return isProcessing (pEvaluation->index (), rpWIP);
    return false;
}

Vca::U32 Vsa::VPoolWorkerGeneration::WIPsWithWorkerMode (WIPList iList, Worker::Worker_Mode iMode) const {
  VReference <WIP> pWIP = (iList == WIPList_Online) ? m_pOnlineWIPHead : m_pOfflineWIPHead;
  Vca::U32 cWIPs = 0;

  while (pWIP) {
     if (pWIP->workerMode () == iMode)
       cWIPs++;

     pWIP.setTo (pWIP->m_pSuccessor);
  }
  return cWIPs;
}

bool Vsa::VPoolWorkerGeneration::isOnlineWIP (WIP const *pWIP) const {
  VReference <WIP> pOnlineWIP (m_pOnlineWIPHead);
  while (pOnlineWIP) {
    if (pOnlineWIP == pWIP)
      return true;
    pOnlineWIP.setTo (pOnlineWIP->m_pSuccessor);
  }
  return false;
}

bool Vsa::VPoolWorkerGeneration::hasActiveWIPs () const {
  //  treat worker creation as an active WIP ...
  if (workersBeingCreated () > 0) return true;

  //  check whether any online wip is still active (running, not timedout)
  VReference<WIP> pWIP (m_pOnlineWIPHead);
  while (pWIP) {
    if (!pWIP->isTimedOut ())
      return true;
    pWIP.setTo (pWIP->m_pSuccessor);
  }

  //  check whether any offline wip is still active (running, not timedout)
  pWIP.setTo (m_pOfflineWIPHead);
  while (pWIP) {
    if (!pWIP->isTimedOut ())
      return true;
    pWIP.setTo (pWIP->m_pSuccessor);
  }
  return false;
}

/********************
 *****  Update  *****
 ********************/

void Vsa::VPoolWorkerGeneration::incrOnlineWorkers () {
    m_cOnlineWorkers++;
    m_pPool->incrOnlineWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOnlineWorkers () {
    m_cOnlineWorkers--;
    m_pPool->decrOnlineWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOnlineAvailWorkers () {
    m_cOnlineAvailWorkers++;
    m_pPool->incrOnlineAvailWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOnlineAvailWorkers () {
    m_cOnlineAvailWorkers--;
    m_pPool->decrOnlineAvailWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOnlineWIPs () {
    m_cOnlineWIPs++;
    m_pPool->incrOnlineWIPs ();
}

void Vsa::VPoolWorkerGeneration::decrOnlineWIPs () {
    m_cOnlineWIPs--;
    m_pPool->decrOnlineWIPs ();
}

void Vsa::VPoolWorkerGeneration::incrWorkersBeingCreated () {
    m_cWorkersBeingCreated++;
    m_pPool->incrWorkersBeingCreated ();
}

void Vsa::VPoolWorkerGeneration::decrWorkersBeingCreated () {
    m_cWorkersBeingCreated--;
    m_pPool->decrWorkersBeingCreated ();
}

void Vsa::VPoolWorkerGeneration::incrOnlineExcessWorkers () {
    m_cOnlineExcessWorkers++;
    m_pPool->incrOnlineExcessWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOnlineExcessWorkers () {
    m_cOnlineExcessWorkers--;
    m_pPool->decrOnlineExcessWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOnlineExcessWorkers (Vca::U32 cWorkers) {
    m_cOnlineExcessWorkers += cWorkers;
    m_pPool->incrOnlineExcessWorkers (cWorkers);
}

void Vsa::VPoolWorkerGeneration::incrWorkersRetired () {
    m_cWorkersRetired++;
    m_pPool->incrWorkersRetired ();
}

void Vsa::VPoolWorkerGeneration::decrWorkersRetired () {
    m_cWorkersRetired--;
    m_pPool->decrWorkersRetired ();
}
void Vsa::VPoolWorkerGeneration::incrOfflineWorkers () {
    m_cOfflineWorkers++;
    m_pPool->incrOfflineWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOfflineWorkers () {
    m_cOfflineWorkers--;
    m_pPool->decrOfflineWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOfflineAvailWorkers () {
    m_cOfflineAvailWorkers++;
    m_pPool->incrOfflineAvailWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOfflineAvailWorkers () {
    m_cOfflineAvailWorkers--;
    m_pPool->decrOfflineAvailWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOfflineWIPs () {
    m_cOfflineWIPs++;
    m_pPool->incrOfflineWIPs ();
}

void Vsa::VPoolWorkerGeneration::decrOfflineWIPs () {
    m_cOfflineWIPs--;
    m_pPool->decrOfflineWIPs ();
}
void Vsa::VPoolWorkerGeneration::incrOfflineExcessWorkers () {
    m_cOfflineExcessWorkers++;
    m_pPool->incrOfflineExcessWorkers ();
}

void Vsa::VPoolWorkerGeneration::decrOfflineExcessWorkers () {
    m_cOfflineExcessWorkers--;
    m_pPool->decrOfflineExcessWorkers ();
}

void Vsa::VPoolWorkerGeneration::incrOfflineExcessWorkers (Vca::U32 cWorkers) {
    m_cOfflineExcessWorkers += cWorkers;
    m_pPool->incrOfflineExcessWorkers (cWorkers);
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/****
 * Procedure: hire ()
 * hireNewWorker method call below can either immediately return the result of worker creation
 * or via a call back later (onWorkerError, onWorker).
 * We have a boolean protection variable so that if it immediately returns error, it doesnt reenter
 * this method again.
 ***/


void Vsa::VPoolWorkerGeneration::hire () {
  if (m_bHiring == false) {  //  reentrant check
    m_bHiring = true;
    log ("Hiring worker(s) as necessary.");
    while (shouldHireWorker ())  {
      hireNewWorker ();
    }
    m_bHiring = false;
  }
}

void Vsa::VPoolWorkerGeneration::retire (bool bOnlyOnlineWorkers) {
    retireStartingWorkers ();
    retireAvailWorkers (onlineAvailWorkers (), true);
    Vca::U32 cBusyOnlineWorkers =
        WIPsWithWorkerMode (WIPList_Online, Worker::Worker_Online) + workersBeingCreated ();

    incrOnlineExcessWorkers (cBusyOnlineWorkers - m_cOnlineExcessWorkers);

    //  retire Offline workers also if needed...(like pool stop/hardstop commands)
    if (!bOnlyOnlineWorkers) {
        retireAvailWorkers (offlineAvailWorkers (), false);
        Vca::U32 cBusyOfflineWorkers =
            WIPsWithWorkerMode (WIPList_Offline, Worker::Worker_Offline);

        incrOfflineExcessWorkers (cBusyOfflineWorkers - m_cOfflineExcessWorkers);
    }
}

/***************************
 *****  Worker Access  *****
 ***************************/

bool Vsa::VPoolWorkerGeneration::supplyOnlineWorkerFor (
    VPoolEvaluation const *pEvaluation, VReference<VPoolWorker> &rpWorker
) {
    bool bFound = false;
    if (canProcess (pEvaluation)) {
        Vca::U32 iWorkerId = pEvaluation->workerId ();
        bFound =  detach (WorkerList_Online, rpWorker, iWorkerId);
    }
    return bFound;
}

bool Vsa::VPoolWorkerGeneration::supplyOfflineWorker (
    VReference<VPoolWorker> &rpWorker, Vca::U32 iWorkerId
) {
    return detach (WorkerList_Offline, rpWorker, iWorkerId);
}

bool Vsa::VPoolWorkerGeneration::getWorkerReference (
   VString const &rId, bool bIsPID, VReference<Worker> &rpWorker
) const {
  bool bFound =
           getWorkerReference (rId, bIsPID, WorkerList_Online, rpWorker)  ? true :
           getWorkerReference (rId, bIsPID, WorkerList_Offline, rpWorker) ? true :
           getWorkerReference (rId, bIsPID, WorkerList_Retired, rpWorker) ? true :
           getWorkerReference (rId, bIsPID, WIPList_Online, rpWorker) ?  true :
           getWorkerReference (rId, bIsPID, WIPList_Offline, rpWorker) ? true : false;
  return bFound;
}

bool Vsa::VPoolWorkerGeneration::getWorkerReference (
   VString const &rId, bool bIsPID, WorkerList iList, VReference<Worker> &rpWorker
) const {
  bool bFound = false;
  VReference <Worker> pWorker;

  switch (iList) {
  case WorkerList_Online:
    pWorker.setTo (m_pOnlineAvailWorkerHead);
    break;
  case WorkerList_Offline:
    pWorker.setTo (m_pOfflineAvailWorkerHead);
    break;
  case WorkerList_Retired:
      pWorker.setTo (m_pRetiredWorkerHead);
  }

  while (pWorker && !bFound) {
      if (pWorker->hasId (rId, bIsPID)) {
          rpWorker.setTo (pWorker);
          bFound = true;
      }
     pWorker.setTo (pWorker->m_pSuccessor);
  }
  return bFound;
}

bool Vsa::VPoolWorkerGeneration::getWorkerReference (
   VString const &rId, bool bIsPID, WIPList iList, VReference<Worker> &rpWorker
) const {
  bool bFound = false;
  VReference <WIP> pWIP;

  switch (iList) {
  case WIPList_Online:
    pWIP.setTo (m_pOnlineWIPHead);
    break;
  case WIPList_Offline:
    pWIP.setTo (m_pOfflineWIPHead);
    break;
  }

  while (pWIP && !bFound) {
    VReference<Worker> pWorker (pWIP->worker ());
    if (pWorker->hasId (rId, bIsPID)) {
        rpWorker.setTo (pWorker);
        bFound = true;
    }
    pWIP.setTo (pWIP->m_pSuccessor);
  }
  return bFound;
}

bool Vsa::VPoolWorkerGeneration::GetWorkerInterface (
    IVReceiver<IVUnknown*>* pReceiver, Vca::U32 workerIdentifier
) {
    VReference<IPoolWorker> rpWorker;
    if (pReceiver) {

        VReference<Worker> pWorker (m_pOnlineAvailWorkerHead);
        while (pWorker.isntNil ()) {
            if (pWorker->identifier () == workerIdentifier) {
                pWorker->getRole (rpWorker);
                pReceiver->OnData (rpWorker);
                return true;
            }
            pWorker.setTo (pWorker->m_pSuccessor);
        }
    }
    return false;
}

/*****************************
 *****************************
 *****  Worker Creation  *****
 *****************************
 *****************************/

void Vsa::VPoolWorkerGeneration::hireNewWorker () {
    log ("Hiring new worker.");
    VReference<WorkerRequest> pRequest;
    pRequest.setTo (new WorkerRequest (m_iNextWorker++, this));

    attach (pRequest);
    pRequest->start ();
}

void Vsa::VPoolWorkerGeneration::hireNewWorker (Vsa::IEvaluator *pEvaluator) {
    VReference<WorkerRequest> pRequest;
    pRequest.setTo (new WorkerRequest (m_iNextWorker++, this, pEvaluator));

    attach (pRequest);
    pRequest->start ();
}

/**
 * @warn must only be called once per worker request.
 */
void Vsa::VPoolWorkerGeneration::onWorker (WorkerRequest *pRequest, Worker *pWorker) {
    log ("Worker %d created successfully", pWorker->identifier ());
    if (m_cWorkerCreationSoftFailures !=0)
        m_cWorkerCreationSoftFailures = 0;
    if (m_cWorkerCreationHardFailures !=0)
        m_cWorkerCreationHardFailures = 0;

    // Check if we're the current generation, retire the worker right away if not.
    if (! isCurrent () && m_pPool->hasStartedWorkers () && m_pPool->queueLength () == 0) {
        // Check if there are any predecessors with online workers.
        Reference pGeneration (m_pSuccessor);
        bool bNoOlderWorkers = true;
        while (bNoOlderWorkers && pGeneration.isntNil ()) {
            if (pGeneration->onlineWorkerCount ()) bNoOlderWorkers = false;
            pGeneration = pGeneration->m_pSuccessor;
        }

        // Throw away this worker if all predecessors are empty.
        if (bNoOlderWorkers) {
            detach (pRequest);
	    retire (pWorker);
	    // ...And since we've restarted, try to hire another in the new generation.
	    m_pPool->hire ();
            return;
        }
    }

    // Trim away an old worker if there are any from a previous generation, or if we've hit our maximum.
    if (m_pPool->hasMaximumWorkers () ||
	(m_pPool->queueLength () == 0 && onlineWorkerCount () < m_pPool->onlineWorkerCount ())) {
        m_pPool->trimWorkers(1);
    }

    // Now that we've trimmed, check again if we can directly make the new worker online.
    if (m_pPool->hasMaximumWorkers()) {
        // Defer the request; keep the worker around until we can make the worker available or until this generation is retired.
        log ("Deferring worker employment.");
    } else {
        detach (pRequest);
        makeWorkerOnline (pWorker, NULL);
    }
}

void Vsa::VPoolWorkerGeneration::finishHiringWorker () {
    WorkerRequest::Reference pWorkerRequest = m_pWorkerRequestHead;
    while (pWorkerRequest) {
        // If we have a worker for this request, employ it.
        if (pWorkerRequest->worker ().isntNil ()) {
            detach (pWorkerRequest);
            makeWorkerOnline (pWorkerRequest->worker (), NULL);
            return;
        }

        // Iterate.
        pWorkerRequest.setTo (pWorkerRequest->m_pSuccessor);
    }
}

bool Vsa::VPoolWorkerGeneration::hasStartedWorkers () {
    WorkerRequest::Reference pWorkerRequest = m_pWorkerRequestHead;
    while (pWorkerRequest.isntNil ()) {
        if (pWorkerRequest->worker ().isntNil ()) return true;

        // Iterate.
        pWorkerRequest.setTo (pWorkerRequest->m_pSuccessor);
    }
    return false;
}

/**
 * @detail
 * May end up calling hire(), which can cause recursion. That's okay, though, because there's a re-entrant check on hire(). We hope.
 */
void Vsa::VPoolWorkerGeneration::onWorkerError (WorkerRequest *pRequest, VString const &rMessage) {
    log ("Worker %d creation failure: %s", pRequest->workerId (), rMessage.content ());
    detach (pRequest);
    m_cWorkerCreationHardFailures++;
    m_cWorkerCreationSoftFailures++;

    if (m_cWorkerCreationHardFailures >= m_pPool->workerCreationFailureHardLimit ())
      m_pPool->onWorkerCreationFailureHardLmt ();
    else if (m_cWorkerCreationSoftFailures >= m_pPool->workerCreationFailureSoftLimit ()) {
      m_pPool->onWorkerCreationFailureSoftLmt ();
      m_cWorkerCreationSoftFailures = 0;
    }
    else
      m_pPool->hire ();
}

/*******************************
 *******************************
 *****  Worker Management  *****
 *******************************
 *******************************/

/**
 * Returns a worker to a generation, taking appropriate action on that worker.
 * A worker which is being returned to generation may be a marked Excess or a Flushed Worker, amongst other things.
 * In either of these cases they are retired, and in all other cases employed.
 */
bool Vsa::VPoolWorkerGeneration::returnWorker (Worker *pWorker) {

    bool bReturned = false;

    //  logging info
    VString iMsg;
    iMsg << "Worker " << pWorker->pid () << "[" << pWorker->identifier () << "]";
    iMsg << " (Generation " << generationId () << ") " << pWorker->getWorkerMode ();

    //  employ or retire....
    if (isWorkerEmployable (pWorker) &&
	(isCurrent () ||
	 pWorker->status () == Worker::Worker_InStartup ||
	 !m_pPool->hasStartedWorkers ())) {
        bReturned = true;
	iMsg << ": Employed";
	switch (pWorker->status ()) {
	case Worker::Worker_InStartup:
	    incrOnlineWorkers ();
	    break;
	default:
	    break;
	}
        employ (pWorker);
    }
    else {
      bReturned = false;
        retire (pWorker);
        iMsg << ": Retired";
    }
    log (iMsg);
    if (!bReturned) {
        // We've just retired a worker. Finish hiring a pending worker as necessary.
        m_pPool->finishHiringWorker ();
    }
    m_pPool->onQueue ();
    return bReturned;
}

/**
 * @detail A worker which is employed goes to the available list of workers (online or offline accordingly)
 * If the worker needs to change mode (online or offline) it does so.
 */
void Vsa::VPoolWorkerGeneration::employ (Worker *pWorker) {

    switch (pWorker->workerMode ()) {
        case Worker::Worker_Online:

          pWorker->setStatus (Worker::Worker_Available);
          //  switch to offline mode if required....
          if (pWorker->getToggleMode ()) {
            decrOnlineWorkers ();
            pWorker->setOfflineMode ();
            incrOfflineWorkers ();
            attach (pWorker, WorkerList_Offline);
            pWorker->resetToggleMode ();
          } else {
            attach (pWorker, WorkerList_Online);
          }
          break;
        case Worker::Worker_Offline:

          pWorker->setStatus (Worker::Worker_Available);
          //  switch to online mode if required...
          if (pWorker->getToggleMode ()) {
            decrOfflineWorkers ();
            pWorker->setOnlineMode ();
            incrOnlineWorkers ();
            attach (pWorker, WorkerList_Online);
            pWorker->resetToggleMode ();
          }
          else {
            attach (pWorker, WorkerList_Offline);
          }
          break;
    }
}


void Vsa::VPoolWorkerGeneration::retire (Worker *pWorker, Worker::Worker_RetirementCause iCause) {
    // Keep the worker alive for the duration of this method.
    Worker::Reference rWorker (pWorker); // TODO: Convert pWorker parameter into reference.

    // Skip if this worker is already retired.
    if (pWorker->workerMode () == Worker::Worker_Retired) return;

    switch (pWorker->workerMode ()) {
    case Worker::Worker_Online:
	if (pWorker->status () != Worker::Worker_InStartup)
	    decrOnlineWorkers ();

	// If the worker is still available, remove it from the available list.
	if (pWorker->status () == Worker::Worker_Available) {
	    detach(pWorker, WorkerList_Online);
	}
	if (m_cOnlineExcessWorkers > 0)
	    decrOnlineExcessWorkers ();
	if (pWorker->isFlushed ())
	    m_pPool->decrementToBeFlushedWorkers ();

	break;
    case Worker::Worker_Offline:
	decrOfflineWorkers ();

	if (pWorker->status () == Worker::Worker_Available) {
	    detach(pWorker, WorkerList_Offline);
	}

	if (m_cOfflineExcessWorkers > 0)
	    decrOfflineExcessWorkers ();

	break;
    }

    // Keep the worker's corpse around.
    attach (pWorker, WorkerList_Retired);
    pWorker->retire (iCause);

    incrWorkersRetired ();
    m_pPool->removeAllBCEvaluationsFor (pWorker->identifier ());
    log ("Retiring Worker(%d) (Generation %d)", pWorker->identifier (), generationId ());
}

void Vsa::VPoolWorkerGeneration::disable (Worker *pWorker) {
    m_cDisabledWorkers++;
    retire (pWorker, Worker::Worker_Failed);
    // Well, this one just died ... let's see if we have a ready replacement
    m_pPool->finishHiringWorker ();
}

void Vsa::VPoolWorkerGeneration::add (IEvaluator *pEvaluator) {
    if (pEvaluator && shouldHireWorker ())
       hireNewWorker (pEvaluator);
}

void Vsa::VPoolWorkerGeneration::retireAvailWorkers (Vca::U32 cWorkers, bool bOnlineWorkers) {
    VReference <Worker> &pHead = bOnlineWorkers ? m_pOnlineAvailWorkerHead : m_pOfflineAvailWorkerHead;
    while (pHead && cWorkers-- > 0) retire (pHead);
}

/**
 * @detail Workers that have already started and are awaiting employment will be retired.
 * WorkerRequests that haven't yet returned will remain, and should be cleaned by the generation when they return.
 */
void Vsa::VPoolWorkerGeneration::retireStartingWorkers () {
    WorkerRequest::Reference pRequest = m_pWorkerRequestHead;
    WorkerRequest::Reference pNextRequest;
    while (pRequest) {
        // Find next request (pre-detach).
        pNextRequest.setTo (pRequest->m_pSuccessor);

        // Retire started worker if there is one.
        if (pRequest->worker ().isntNil ()) {
            retire (pRequest->worker ());
            detach (pRequest);
        }

        // Iterate.
        pRequest.setTo (pNextRequest);
    }
}

/**
 * Trims excess workers from this generation.
 * Trimming is done on online workers only.
 * Retiring all workers might end up releasing all references to this generation object. The retain/release pair ensures that the  object is kept alive till the end of this routine.
 */
Vca::U32 Vsa::VPoolWorkerGeneration::trimWorkers (Vca::U32 cExcessWorkers, bool bIdleOnly) {

    Vca::U32 cWorkers = cExcessWorkers;
    if (onlineWorkerCount () == 0) return 0;

    retain ();

    //  Prune as many Idle Available Workers
    Vca::U32 cTrim = onlineAvailWorkers () > cExcessWorkers ? cExcessWorkers : onlineAvailWorkers ();
    retireAvailWorkers (cTrim);
    cExcessWorkers -= cTrim;

    if (!bIdleOnly && cExcessWorkers > 0) {
        //  Mark as many InUse Workers as Excess
        Vca::U32 cBusyOnlineWorkers =
            WIPsWithWorkerMode (WIPList_Online, Worker::Worker_Online) + workersBeingCreated ();
        cTrim = cBusyOnlineWorkers > cExcessWorkers ? cExcessWorkers : cBusyOnlineWorkers;
        incrOnlineExcessWorkers (cTrim - m_cOnlineExcessWorkers);
        cExcessWorkers -= cTrim;
    }

    release ();
    return cWorkers-cExcessWorkers;
}


/***************************************
 ***************************************
 *****  Offline Worker Management  *****
 ***************************************
 ***************************************/

/**
 * Takes a worker from this generation offline.
 * If the provided worker is online and in use, it will be taken offline after it completes its current query. To indicate this, the worker's toggle mode flag is turned on.
 *
 * @param pWorker The worker to take offline.
 * @param pClient The destination of the result (log) string.
 */
bool Vsa::VPoolWorkerGeneration::takeWorkerOffline (Worker *pWorker, IVReceiver<VString const&> *pClient) {
    bool bTakenOffline = false;

    if (pWorker->generationId () == m_iGeneration) {

        VString iResult;
        VString iPID (pWorker->pid ());
        Vca::U32 iID (pWorker->identifier ());

        switch (pWorker->workerMode ()) {
        case Worker::Worker_Online:
            switch (pWorker->status ()) {
            case Worker::Worker_Available: {
                VReference<Worker> rpWorker;
                detach (WorkerList_Online, rpWorker, pWorker->identifier ());
                decrOnlineWorkers ();
                pWorker->setOfflineMode ();
                attach (pWorker, WorkerList_Offline);
                incrOfflineWorkers ();
                iResult.printf ("Worker %s [%d] is taken offline", iPID.content (), iID);
                bTakenOffline = true;
                break;
            }
            case Worker::Worker_InUse:
                pWorker->setToggleMode ();
                iResult.printf ("Worker %s [%d] will be offline after it finishes its current online query.", iPID.content (), iID);
                break;
            case Worker::Worker_InStartup:
                iResult.printf ("Worker %s [%d] is being created", iPID.content (), iID);
                break;
            default:
                break;
            }
            break;
        case Worker::Worker_Offline:
            iResult.printf ("Worker %s [%d] is already offline", iPID.content (), iID);
            break;
        }
        if (pClient)
            pClient->OnData (iResult);
    }
    return bTakenOffline;
}

/**
 * Takes a worker online.
 * If the worker is offline and in use, it will be brought online after its current query. To indicate this, the workers toggle mode flag is turned on.
 * For workers in startup, it is assumed that the appropriate WorkerRequest will be detached by the caller.
 *
 * @param pWorker The worker to take online.
 * @param pClient The destination of the result (log) string.
 */
bool Vsa::VPoolWorkerGeneration::makeWorkerOnline  (Worker *pWorker, IVReceiver<VString const&> *pClient) {
    bool bMadeOnline = false;

    if (pWorker->generationId () == m_iGeneration) {
        VString iResult;
        VString iPID (pWorker->pid ());
        Vca::U32 iID (pWorker->identifier ());

        switch (pWorker->workerMode ()) {
        case Worker::Worker_Offline:
            switch (pWorker->status ()) {
            case Worker::Worker_Available: {
                VReference<Worker> rpWorker;
                detach (WorkerList_Offline, rpWorker, pWorker->identifier ());
                decrOfflineWorkers ();
                incrOnlineWorkers ();
                pWorker->setOnlineMode ();

                //  do not directly attach to avail list, return the worker so that its added when
                //  if required.
                bMadeOnline = returnWorker (pWorker);
                if (bMadeOnline)
                        iResult.printf ("Worker %s [%d] is taken online", iPID.content (), iID);
                else
                        iResult.printf ("Worker %s [%d] is retired to satisfy pool settings", iPID.content (), iID);
                break;
            }
            case Worker::Worker_InUse:
                  pWorker->setToggleMode ();
                  iResult.printf ("Worker %s [%d] will be online after it finishes its current offline query.", iPID.content (), iID);
                  break;
            case Worker::Worker_InStartup:
                  iResult.printf ("Worker %s [%d] is being created", iPID.content (), iID);
                  break;
            default:
                 break;
            }
            break;
        case Worker::Worker_Online:
            if (pWorker->status () == Worker::Worker_InStartup) {
                bMadeOnline = returnWorker (pWorker);
                if (bMadeOnline)
                    iResult.printf ("Worker %s [%d] has been employed and taken online", iPID.content (), iID);
                else
                    iResult.printf ("Worker %s [%d] was employed but subsequently retired to satisfy pool settings", iPID.content (), iID);
                break;
            }
            else iResult.printf ("Worker %s [%d] is already online", iPID.content (), iID);
            break;
        }
        if (pClient)
            pClient->OnData (iResult);
    }
    return bMadeOnline;
}

/**
 * Evaluates a query using an offline worker.
 *
 * @param pWorker The worker to use for evaluation of the given query.
 * @param rQuery The query to evaluate.
 * @param pClient The destination of the evaluation result.
 */
void Vsa::VPoolWorkerGeneration::evaluateUsingOfflineWorker (
    Worker *pWorker, VString const &rQuery, IEvaluatorClient *pClient
) {

    if (pWorker->generationId () == m_iGeneration) {

        VString iResult;
        VString iPID (pWorker->pid ());
        Vca::U32 iID (pWorker->identifier ());

        switch (pWorker->workerMode ()) {
        case Worker::Worker_Offline:
            switch (pWorker->status ()) {
            case Worker::Worker_Available: {
                VReference<Worker> rpWorker;
                 supplyOfflineWorker (rpWorker, pWorker->identifier  ());
                 VReference<VEvaluation> pEvaluation (m_pPool->createEvaluation (pClient, "", rQuery));
                 pWorker->process (dynamic_cast<VPoolEvaluation*> (pEvaluation.referent ()));
            }
            break;
            case Worker::Worker_InUse:
            case Worker::Worker_InStartup: {
                VString iResult;
                iResult.printf ("Worker %s [%d] is offline, but not available for querying", iPID.content (), iID);
                if (pClient)
                    pClient->OnResult (0, iResult);
            }
            break;
            }
        break;
        case Worker::Worker_Online:
            iResult.printf ("Worker %s [%d] is online. Make it offline and try.", iPID.content (), iID);
            if (pClient)
                pClient->OnResult (0, iResult);
            break;
        }
    }

}

void Vsa::VPoolWorkerGeneration::retireOfflineWorker (Worker *pWorker, IVReceiver<VString const&> *pClient) {
    if (pWorker->generationId () == m_iGeneration) {
        VString iResult;
        VString iPID (pWorker->pid ());
        Vca::U32 iID (pWorker->identifier ());

        switch (pWorker->workerMode ()) {

        case Worker::Worker_Offline:
              switch (pWorker->status ()) {
              case Worker::Worker_Available: {
                    retire (pWorker);
                    iResult.printf ("Worker %s [%d] retired", iPID.content (), iID);
                    break;
             }
             case Worker::Worker_InUse:
                  pWorker->m_bAboutToRetire = true;
                  iResult.printf ("Worker %s [%d] will be retired after it finishes its current offline query.", iPID.content (), iID);
                  break;
             case Worker::Worker_InStartup:
                  iResult.printf ("Worker %s [%d] is being created", iPID.content (), iID);
                  break;
             default:
                  break;
              }
              break;
        case Worker::Worker_Online:
              iResult.printf ("Worker %s [%d] is online. Make it offline and then retire", iPID.content (), iID);
              break;
        }
        if (pClient)
            pClient->OnData (iResult);
    }
}



/***********************
 *****  Broadcast  *****
 ***********************/

void Vsa::VPoolWorkerGeneration::processBCObject (VPoolBroadcast *pBroadcast) {
    VReference<VPoolWorker> pWorker (m_pOnlineAvailWorkerHead);

    //  broadcast to free online workers
    while (pWorker) {
        Vca::U32 iFreeWorkerId = pWorker->identifier ();
        VReference<VPoolEvaluation> pEvaluation;
        pEvaluation.setTo (pBroadcast->createBCEvaluationFor (iFreeWorkerId));
        pBroadcast->incrementExpectedReplies ();

        m_pPool->enqueueBCEvaluation (pEvaluation);
        pWorker.setTo (pWorker->m_pSuccessor);
    }

    //  broadcast to busy online workers
    VReference<WIP> pWIP (m_pOnlineWIPHead);
    while (pWIP) {
        Vca::U32 iBusyWorkerId = pWIP->workerId ();
        VReference<VPoolEvaluation> pEvaluation;
        pEvaluation.setTo (pBroadcast->createBCEvaluationFor (iBusyWorkerId));
        pBroadcast->incrementExpectedReplies ();

        m_pPool->enqueueBCEvaluation (pEvaluation);
        pWIP.setTo (pWIP->m_pSuccessor);
    }
}

/****************************
 *****  List Management *****
 ****************************/

VReference<Vsa::VPoolWorker>& Vsa::VPoolWorkerGeneration::getHead (WorkerList iList) {
    switch (iList) {
    default:
    case WorkerList_Online:
        return m_pOnlineAvailWorkerHead;
    case WorkerList_Offline:
        return m_pOfflineAvailWorkerHead;
    case WorkerList_Retired:
        return m_pRetiredWorkerHead;
    }
}

VReference<Vsa::VPoolWorker>& Vsa::VPoolWorkerGeneration::getTail (WorkerList iList) {
    switch (iList) {
    default:
    case WorkerList_Online:
        return m_pOnlineAvailWorkerTail;
    case WorkerList_Offline:
        return m_pOfflineAvailWorkerTail;
    case WorkerList_Retired:
        return m_pRetiredWorkerTail;
    }
}

/****
 *  Note: The following routines attaches/detaches Workers/WIPs/WorkerRequests from their
 *        respective lists being maintained in the VPoolGeneration object.
 ***/

void Vsa::VPoolWorkerGeneration::attach (Worker *pWorker, WorkerList iList) {
  VReference <Worker> &pHead = getHead (iList), &pTail = getTail (iList);

  if (pTail.isNil ())
      pHead.setTo (pWorker);
  else {
      pTail->m_pSuccessor.setTo (pWorker);
      pWorker->m_pPredecessor.setTo (pTail);
  }
  pTail.setTo (pWorker);

  // Keep our counters in order.
    switch (iList) {
    case WorkerList_Online:
        incrOnlineAvailWorkers (); // may result in this worker being retired ..
        break;
    case WorkerList_Offline:
        incrOfflineAvailWorkers ();
    default:
        break;
    }
}

void Vsa::VPoolWorkerGeneration::detach (Worker *pWorker, WorkerList iList) {
    VReference <Worker> &pHead = getHead (iList), &pTail = getTail (iList);

    if (pWorker->m_pSuccessor)
        pWorker->m_pSuccessor->m_pPredecessor = pWorker->m_pPredecessor;
    else if (pWorker == pTail) { // The only way this won't be true is if pWorker isn't in the list at all.
        pTail.setTo (pWorker->m_pPredecessor);
        if (pTail)
            pTail->m_pSuccessor.clear ();
    }

    if (pWorker->m_pPredecessor)
        pWorker->m_pPredecessor->m_pSuccessor = pWorker->m_pSuccessor;
    else if (pWorker == pHead) { // The only way this won't be true is if pWorker isn't in the list at all.
        pHead.setTo (pWorker->m_pSuccessor);
        if (pHead)
            pHead->m_pPredecessor.clear ();
    }

    pWorker->m_pSuccessor.clear ();
    pWorker->m_pPredecessor.clear ();

    // Keep our counters in order.
    switch (iList) {
    case WorkerList_Online:
        decrOnlineAvailWorkers ();
        break;
    case WorkerList_Offline:
        decrOfflineAvailWorkers ();
    default:
        break;
    }
}

/*****
 * Note: If the user passes 0 as the requested workerId, then the first available worker is returned...
 *
 ****/

bool Vsa::VPoolWorkerGeneration::detach (
  WorkerList iList, VReference<Worker> &rpAvailableWorker, Vca::U32 iWorkerId
) {
    VReference <Worker> &pHead = getHead (iList), &pTail = getTail (iList);

    VReference<VPoolWorker> pWorker (pHead);

    while (pWorker) {
        // Check if we've found the worker, return as necessary.
        if (pWorker->identifier () == iWorkerId || iWorkerId == 0) {
            rpAvailableWorker.setTo (pWorker);
            detach (pWorker.referent (), iList);
            return true;
        }

        // Increment.
        pWorker.setTo (pWorker->m_pSuccessor);
    }

    // Worker not found.
    return false;
}

void Vsa::VPoolWorkerGeneration::attach (WorkerRequest *pRequest) {
    if (m_pWorkerRequestTail.isNil ())
        m_pWorkerRequestHead.setTo (pRequest);
    else {
        m_pWorkerRequestTail->m_pSuccessor.setTo (pRequest);
        pRequest->m_pPredecessor.setTo (m_pWorkerRequestTail);
    }
    m_pWorkerRequestTail.setTo (pRequest);
    incrWorkersBeingCreated ();
}


void Vsa::VPoolWorkerGeneration::detach (WorkerRequest *pRequest) {
    if (pRequest->m_pSuccessor)
        pRequest->m_pSuccessor->m_pPredecessor = pRequest->m_pPredecessor;
    else {
        m_pWorkerRequestTail.setTo (pRequest->m_pPredecessor);
        if (m_pWorkerRequestTail)
            m_pWorkerRequestTail->m_pSuccessor.clear ();
    }

    if (pRequest->m_pPredecessor)
        pRequest->m_pPredecessor->m_pSuccessor = pRequest->m_pSuccessor;
    else {
        m_pWorkerRequestHead.setTo (pRequest->m_pSuccessor);
        if (m_pWorkerRequestHead)
            m_pWorkerRequestHead->m_pPredecessor.clear ();
    }

    pRequest->m_pSuccessor.clear ();
    pRequest->m_pPredecessor.clear ();

    decrWorkersBeingCreated ();
}

void Vsa::VPoolWorkerGeneration::attach (WIP *pWIP) {

    VReference <WIP> &pHead = (pWIP->isWorkerOnline() ? m_pOnlineWIPHead : m_pOfflineWIPHead);
    VReference <WIP> &pTail = (pWIP->isWorkerOnline () ? m_pOnlineWIPTail : m_pOfflineWIPTail);

    if (pTail.isNil ())
        pHead.setTo (pWIP);
    else {
        pTail->m_pSuccessor.setTo (pWIP);
        pWIP->m_pPredecessor.setTo (pTail);
    }
    pTail.setTo (pWIP);
}

void Vsa::VPoolWorkerGeneration::detach (WIP *pWIP) {

    VReference <WIP> &pHead = (pWIP->isOnlineWIP () ? m_pOnlineWIPHead : m_pOfflineWIPHead);
    VReference <WIP> &pTail = (pWIP->isOnlineWIP () ? m_pOnlineWIPTail : m_pOfflineWIPTail);

    if (pWIP->m_pSuccessor) {
        if (pWIP == pHead) {
            pWIP->m_pSuccessor->m_pPredecessor.clear ();
            pHead.setTo (pWIP->m_pSuccessor);
        }
        else {
            pWIP->m_pSuccessor->m_pPredecessor.setTo (pWIP->m_pPredecessor);
            pWIP->m_pPredecessor->m_pSuccessor.setTo (pWIP->m_pSuccessor);
        }
    }
    else {
        if (pWIP == pHead) {
            pHead.clear ();
            pTail.clear ();
        }
        else if (pWIP->m_pPredecessor) {
            pWIP->m_pPredecessor->m_pSuccessor.clear ();
            pTail.setTo (pWIP->m_pPredecessor);
        }
    }
}

void Vsa::VPoolWorkerGeneration::detachRetiredWorkers () {
    while (m_pRetiredWorkerHead.isntNil ()) {
	VReference<Worker> pWorker = m_pRetiredWorkerTail;
        // It's actually more efficient code-wise to work from the back to the front.
        detach (pWorker, WorkerList_Retired);
    }
}

/************************
 *****  Statistics  *****
 ************************/

void Vsa::VPoolWorkerGeneration::getDetailedStat (VString &rResult) const {
    VString iOnlineAvailList, iOnlineInUseList, iInCreationList;
    VString iOfflineAvailList, iOfflineInUseList;
    VString iRetiredList;

    getWorkerLists (
        iOnlineAvailList, iOnlineInUseList, iInCreationList, iOfflineAvailList, iOfflineInUseList, iRetiredList
    );
    rResult << "\n";
    rResult << "\nGeneration Id: " << m_iGeneration;
    rResult << "\nWorkers Starting: " << iInCreationList;

    //  Online
    rResult << "\n\nOnline Workers:";
    rResult << "\nOnline Available Workers: " << iOnlineAvailList;
    rResult << "\nOnline InUse Workers: "<< iOnlineInUseList;
    rResult << "\nOnline Excess Worker Count: " << m_cOnlineExcessWorkers;

    //  Offline + Retired
    rResult << "\n\nOfflineWorkers:";
    rResult << "\nOffline Available Workers: " << iOfflineAvailList;
    rResult << "\nOffline InUse Workers: "<< iOfflineInUseList;
    rResult << "\nOffline Excess Count: " << m_cOfflineExcessWorkers;
    rResult << "\n\nRetired Workers: " << iRetiredList;
}

void Vsa::VPoolWorkerGeneration::getStatistics (VString &rResult) const {

    VString iOnlineAvailList, iOnlineInUseList, iInCreationList;
    VString iOfflineAvailList, iOfflineInUseList;
    VString iRetiredList;

    getWorkerLists (
        iOnlineAvailList, iOnlineInUseList, iInCreationList, iOfflineAvailList, iOfflineInUseList, iRetiredList
    );

    VString iOnlineAvailPadded, iOnlineInUsePadded, iCreationPadded;

    formatWorkerList (iOnlineAvailList, iOnlineAvailPadded, 16);
    formatWorkerList (iOnlineInUseList, iOnlineInUsePadded, 16);
    formatWorkerList (iInCreationList, iCreationPadded, 10);

    VString iGen; iGen << m_iGeneration << ".";
    VString iGenPadded; iGenPadded.printf ("%-5s", iGen.content ());

    VString iOnline, iOnlinePadded;
    iOnline << m_cOnlineWorkers; iOnlinePadded.printf ("%-7s", iOnline.content ());

    rResult << iGenPadded << "Online   ";
    rResult << iOnlinePadded << iOnlineAvailPadded << iOnlineInUsePadded;
    rResult << iCreationPadded << m_cOnlineExcessWorkers << "       " << m_cWorkersRetired;

    if (m_cOfflineAvailWorkers >0 || m_cOfflineWIPs >0) {
      rResult << "\n";
      VString iOfflinePadded, iOfflineAvailPadded, iOfflineInUsePadded;
      formatWorkerList (iOfflineAvailList, iOfflineAvailPadded, 16);
      formatWorkerList (iOfflineInUseList, iOfflineInUsePadded, 16);
      VString iOffline; iOffline << m_cOfflineWorkers; iOfflinePadded.printf ("%-7s", iOffline.content ());

      rResult << iGenPadded << "Offline  " << iOfflinePadded;
      rResult << iOfflineAvailPadded << iOfflineInUsePadded ;
      rResult << "          " << m_cOfflineExcessWorkers;
    }
}

void Vsa::VPoolWorkerGeneration::quickStatsHeader (VString &rResult) {
    VString iResult;
    iResult.printf(
            "%-5s | %-6s | %-16s | %-16s | %-10s | %-6s | %-7s | %-7s | %-17s | %-16s",
            "ID",
            "Online",
            "Online Available",
            "Online In Use",
            "Starting",
            "Excess",
            "Retired",
            "Offline",
            "Offline Available",
            "Offline In Use");
    rResult.setTo(iResult);
}

void Vsa::VPoolWorkerGeneration::quickStats (VString &rResult) const {
    // Get our data.
    VString iOnlineAvailList,   iOnlineAvailListFormatted,
            iOnlineInUseList,   iOnlineInUseListFormatted,
            iInCreationList,    iInCreationListFormatted,
            iOfflineAvailList,  iOfflineAvailListFormatted,
            iOfflineInUseList,  iOfflineInUseListFormatted,
            iRetiredList;
    getWorkerLists (iOnlineAvailList, iOnlineInUseList, iInCreationList, iOfflineAvailList, iOfflineInUseList, iRetiredList);
    formatWorkerList (iOnlineAvailList,  iOnlineAvailListFormatted,  16);
    formatWorkerList (iOnlineInUseList,  iOnlineInUseListFormatted,  16);
    formatWorkerList (iInCreationList,   iInCreationListFormatted,   10);
    formatWorkerList (iOfflineAvailList, iOfflineAvailListFormatted, 17);
    formatWorkerList (iOfflineInUseList, iOfflineInUseListFormatted, 16);

    // Format our result.
    VString iResult;
    iResult.printf(
            "%-5d | %-6d | %-16s | %-16s | %-10s | %-6d | %-7d | %-7d | %-17s | %-16s",
            m_iGeneration,
            m_cOnlineWorkers,
            iOnlineAvailListFormatted.content (),
            iOnlineInUseListFormatted.content (),
            iInCreationListFormatted.content (),
            m_cOnlineExcessWorkers,
            m_cWorkersRetired,
            m_cOfflineWorkers,
            iOfflineAvailListFormatted.content (),
            iOfflineInUseListFormatted.content ());

    // Insert newline as necessary.
    if (rResult.length () > 0) rResult << "\n";

    // Put our data into the result string.
    rResult << iResult;
}

void Vsa::VPoolWorkerGeneration::getWorkersStatistics (VString &rStat) const {

    VReference<Worker> pWorker (m_pOnlineAvailWorkerHead);
    while (pWorker) {
      pWorker->getStat (rStat);
      pWorker.setTo (pWorker->m_pSuccessor);
    }

    VReference<WIP> pWIP (m_pOnlineWIPHead);
    while (pWIP) {
      VReference <Worker> pWorker (pWIP->worker ());
      pWorker->getStat (rStat);
      pWIP.setTo (pWIP->m_pSuccessor);
    }

    pWorker.setTo (m_pOfflineAvailWorkerHead);
    while (pWorker) {
      pWorker->getStat (rStat);
      pWorker.setTo (pWorker->m_pSuccessor);
    }

    pWIP.setTo (m_pOfflineWIPHead);
    while (pWIP) {
      VReference <Worker> pWorker (pWIP->worker ());
      pWorker->getStat (rStat);
      pWIP.setTo (pWIP->m_pSuccessor);
    }
}

void Vsa::VPoolWorkerGeneration::dumpWorkerStats (VString &rResult) const {
    VReference<Worker> pWorker (m_pOnlineAvailWorkerHead);
    while (pWorker) {
        pWorker->quickStats (rResult);
        pWorker.setTo (pWorker->m_pSuccessor);
    }
    VReference<WIP> pWIP (m_pOnlineWIPHead);
    while (pWIP) {
      VReference<Worker> pWorker (pWIP->worker ());
      pWorker->quickStats (rResult);
      pWIP.setTo (pWIP->m_pSuccessor);
    }
    pWorker.setTo(m_pOfflineAvailWorkerHead);
    while (pWorker) {
        pWorker->quickStats (rResult);
        pWorker.setTo (pWorker->m_pSuccessor);
    }
    pWorker.setTo(m_pRetiredWorkerHead);
    while (pWorker) {
        pWorker->quickStats (rResult);
        pWorker.setTo (pWorker->m_pSuccessor);
    }
}

/****
 * Note: The getWorkerLists function returns the worker details in the following format.
 *       "WorkerCount_N [WorkerId1, WorkerId2, WorkerId3..., WorkerIdN]"
 *        for each of the Available, InUse, InCreation, Offline workers of this generation
 *        If a worker is about to change mode, it is indicated with a '#' before its id.
 ****/

void Vsa::VPoolWorkerGeneration::getWorkerLists (
   VString &rOnlineAvailList,
   VString &rOnlineWIPList,
   VString &rInCreationList,
   VString &rOfflineAvailList,
   VString &rOfflineWIPList,
   VString &rRetiredList
) const {
  // OnlineAvailList
  rOnlineAvailList << m_cOnlineAvailWorkers << "[";
  VReference<Worker> pWorker (m_pOnlineAvailWorkerHead);

  while (pWorker) {
    rOnlineAvailList << pWorker->identifier ();
    if (pWorker->m_pSuccessor)
      rOnlineAvailList << ",";
    pWorker.setTo (pWorker->m_pSuccessor);
  }
  rOnlineAvailList << "]";

  // OnlineWIPList
  rOnlineWIPList << m_cOnlineWIPs << "[";
  VReference<WIP> pWIP (m_pOnlineWIPHead);
  while (pWIP) {
    //  prefix '#' if the Worker In Use is about to change mode...after this query
    VReference<Worker> pWorker (pWIP->worker ());
    if (pWorker->getToggleMode ())
      rOnlineWIPList << "#";

    rOnlineWIPList << pWIP->workerId ();
    if (pWIP->m_pSuccessor)
      rOnlineWIPList << ",";
    pWIP.setTo (pWIP->m_pSuccessor);
  }
  rOnlineWIPList << "]";

  // InCreationList
  rInCreationList << m_cWorkersBeingCreated << "[";
  VReference<WorkerRequest> pRequest (m_pWorkerRequestHead);
  while (pRequest) {
    rInCreationList << pRequest->workerId ();
    if (pRequest->m_pSuccessor)
        rInCreationList << ",";
    pRequest.setTo (pRequest->m_pSuccessor);
  }
  rInCreationList << "]";

  // OfflineAvailList
  rOfflineAvailList << m_cOfflineAvailWorkers << "[";
  pWorker.setTo (m_pOfflineAvailWorkerHead);
  while (pWorker) {
    rOfflineAvailList << pWorker->identifier ();
    if (pWorker->m_pSuccessor)
      rOfflineAvailList << ",";
    pWorker.setTo (pWorker->m_pSuccessor);
  }
  rOfflineAvailList << "]";

  // OfflineWIPList
  rOfflineWIPList << m_cOfflineWIPs << "[";
  pWIP.setTo (m_pOfflineWIPHead);
  while (pWIP) {
    //  prefix '#' if the Worker In Use is about to change mode...after this query
    VReference<Worker> pWorker (pWIP->worker ());
    if (pWorker->getToggleMode ())
      rOfflineWIPList << "#";

    rOfflineWIPList << pWIP->workerId ();
    if (pWIP->m_pSuccessor)
      rOfflineWIPList << ",";
    pWIP.setTo (pWIP->m_pSuccessor);
  }
  rOfflineWIPList << "]";

  // RetiredList
  rRetiredList << m_cWorkersRetired << "[";
  pWorker.setTo (m_pRetiredWorkerHead);
  while (pWorker) {
    rRetiredList << pWorker->identifier ();
    if (pWorker->m_pSuccessor)
      rRetiredList << ",";
    pWorker.setTo (pWorker->m_pSuccessor);
  }
  rRetiredList << "]";
}

/**
 * Formats worker list for displaying purposes...
 * Example: [1, 2, 3, 4, 5] is formatted to [1, 2, 3,..]
 *
 * @param rInput The unformatted worker list.
 * @param rOutput The formatted (resultant) worker list.
 * @param cOutputLength The maximum length of the resultant worker list.
 */
void Vsa::VPoolWorkerGeneration::formatWorkerList (
    VString &rInput, VString &rOutput, Vca::U32 cOutputLength
) const {
    VString  iFormattedOutput, iPrefix;
    VString iTail ("..]");
    Vca::U32 sTail = iTail.length ();
    bool bAttachEllipsis = false;

    do {
      rInput.getPrefix (',', iPrefix, rInput);


      if (iFormattedOutput.length ()+1+iPrefix.length ()<=cOutputLength-sTail) {
        if (iFormattedOutput.isntEmpty ()) iFormattedOutput << ",";
        iFormattedOutput << iPrefix;
      }
      else if (rInput.isEmpty () && iFormattedOutput.length ()+1+iPrefix.length () <= cOutputLength) {
        if (iFormattedOutput.isntEmpty ()) iFormattedOutput << ",";
        iFormattedOutput << iPrefix;
      }
      else {
        bAttachEllipsis = true;
        break;
      }
    } while (rInput.isntEmpty ());

    if (bAttachEllipsis)
      iFormattedOutput << iTail;

    rOutput.printf ("%-*s", cOutputLength, iFormattedOutput.content ());
}



/**********************
 *****  Snapshot  *****
 **********************/

void Vsa::VPoolWorkerGeneration::dumpWorkingWorkers (V::VSimpleFile &rFile) const {
  rFile.printf ("\nGeneration %d: ", generationId ());

  VReference<WIP> pWIP (m_pOnlineWIPHead);
  if (pWIP)
    rFile.printf ("\nOnline InUse Workers:");
  while (pWIP) {
    VReference<Worker> pWorker (pWIP->worker());
    VString iStat;
    pWorker->getStatistics (iStat);
    rFile.printf (iStat.content ());
    pWIP.setTo (pWIP->m_pSuccessor);
  }


  pWIP.setTo (m_pOfflineWIPHead);
  if (pWIP)
    rFile.printf ("\nOffline InUse Workers:");
  while (pWIP) {
    VReference<Worker> pWorker (pWIP->worker());
    VString iStat;
    pWorker->getStatistics (iStat);
    rFile.printf (iStat.content ());
    pWIP.setTo (pWIP->m_pSuccessor);
  }

}

/*******************************************************
 *******************************************************
 *****                                             *****
 *****  Vsa::VPoolWorkerGeneration::WorkerRequest  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolWorkerGeneration::WorkerRequest::WorkerRequest (
    Vca::U32 iWorkerId, WorkerGeneration *pGeneration
)
 : m_pGeneration (pGeneration)
 , m_pPool       (pGeneration->pool ())
 , m_iWorkerId   (iWorkerId)
 , m_pIEvaluatorSink (this)
 , m_pIEvaluatorClient (this) {
    traceInfo ("Creating WorkerRequest");
}

Vsa::VPoolWorkerGeneration::WorkerRequest::WorkerRequest (
    Vca::U32 iWorkerId, WorkerGeneration *pGeneration, IEvaluator *pEvaluator
)
 : m_pGeneration (pGeneration)
 , m_pPool       (pGeneration->pool ())
 , m_iWorkerId   (iWorkerId)
 , m_pEvaluator  (pEvaluator)
 , m_pIEvaluatorSink (this)
 , m_pIEvaluatorClient (this) {
    traceInfo ("Creating WorkerRequest");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolWorkerGeneration::WorkerRequest::~WorkerRequest () {
    traceInfo ("Destroying WorkerRequest");
}

/***************************
 *****  Create Worker  *****
 ***************************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::start () {
  if (m_pEvaluator.isNil ()) {
    VReference<IEvaluatorSink> pIEvaluatorSink;
    getRole (pIEvaluatorSink);
    VReference<IEvaluatorSource> pSource (m_pPool->workerSource ());
    pSource->Supply (pIEvaluatorSink);
  }
  else {
    VString iPIDQuery;
    m_pPool->workerPIDQuery (iPIDQuery);
    if (iPIDQuery.isEmpty () ||
	m_pPool->isStopped ()) {
      returnNewWorker ("");
    }
    else {
      VReference<IEvaluatorClient> pClient;
      getRole (pClient);

      //  get pid of worker...
      m_pEvaluatorEKG.setTo (new interface_monitor_t (this, &ThisClass::signalIEvaluatorEKG, m_pEvaluator));
      m_pEvaluator->EvaluateExpression (pClient, "", iPIDQuery);
    }
  }
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::returnNewWorker(VString const &rPID) {
      m_pWorker.setTo (new Worker (m_iWorkerId, rPID, m_pGeneration, m_pEvaluator));
      // Set worker mode to online, signifying that the worker has started up.
      m_pWorker->setOnlineMode ();

      // Send the worker to the generation.
      m_pGeneration->onWorker (this, m_pWorker);

      // Clear state references that we won't need anymore.
      m_pGeneration.clear ();
      m_pEvaluator.clear ();
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/**************************
 *****  Vsa::IClient  *****
 **************************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
) {
    handleError (rMessage);
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::OnEnd (Vca::IClient *pRole) {
    handleError ("Connection Termination Error");
}

/*********************************
 *****  Vsa::IEvaluatorSink  *****
 *********************************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::getRole (
    VReference<IEvaluatorSink>&rpRole
) {
    m_pIEvaluatorSink.getRole (rpRole);
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::OnData (
    IEvaluatorSink *pRole, IEvaluator *pEvaluator
) {
    if (pEvaluator) {
        m_pEvaluator.setTo (pEvaluator);
        start();
    } else {
        handleError ("Null Evaluator");
    }
}

/***********************************
 *****  Vsa::IEvaluatorClient  *****
 ***********************************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::getRole (VReference<IEvaluatorClient>&rpRole) {
    m_pIEvaluatorClient.getRole (rpRole);
}


void Vsa::VPoolWorkerGeneration::WorkerRequest::OnAccept (
    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
) {
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::OnChange (
    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
) {
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::OnResult (
    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
) {
    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    //  Remove the space and new line characters
    VString iOutput (rOutput);
    iOutput.replaceSubstring ("\n", "");
    iOutput.replaceSubstring (" ", "");

    //  VMS has PIDs displayed in Hexadecimal
#if defined(__VMS)
    Vca::U32 pid = atoi (iOutput);
    iOutput.clear ();
    iOutput.printf ("%x", pid);
#endif
    //  Create Worker...
    returnNewWorker (iOutput);
}


/**************************
 *****  ErrorHandler  *****
 **************************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::handleError (
    VString const &rMessage
) {
    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    m_pGeneration->onWorkerError (this, rMessage);
    m_pGeneration.clear ();
}


/*****************
 *****  EKG  *****
 *****************/

void Vsa::VPoolWorkerGeneration::WorkerRequest::signalIEvaluatorEKG () {
  
    traceInfo ("IEvaluator disappeared.");
    log ("IEvaluator for worker %d creation (Generation %d) no longer connected.", m_iWorkerId, m_pGeneration->generationId ());
    handleError ("Worker creation failed.");
}

void Vsa::VPoolWorkerGeneration::WorkerRequest::cancelIEvaluatorEKG () {
    if (m_pEvaluatorEKG.isntNil ()) {
        m_pEvaluatorEKG->cancel ();
        m_pEvaluatorEKG.clear ();
    }
}

