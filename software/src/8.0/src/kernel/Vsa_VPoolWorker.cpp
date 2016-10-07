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

#include "Vsa_VPoolWorker.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32) //  MSVC Compiler Happy Pill
#include "Vsa_CompilerHappyPill.h"
#endif

#include "VSimpleFile.h"
#include "Vca_VTrigger.h"

#include "Vsa_VEvaluatorPool.h"
#include "Vsa_VPoolWorkerGeneration.h"

#include "Vsa_VPoolBroadcastEvaluation.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Vsa::VPoolEvaluationResult  *****
 *****                              *****
 ****************************************
 ****************************************/

#include "Vsa_IEvaluationResult.h"

namespace Vsa {
    class VPoolEvaluationResult : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (VPoolEvaluationResult, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef IVReceiver<VString const&> IStringSink;

    //  TimingReportGofer
    public:
        class TimingReportGofer : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (TimingReportGofer, Vca::VRolePlayer);

        //  Construction
        public:
            TimingReportGofer (
                IStringSink *pStringSink, IEvaluationResult *pStringSource
            ) : m_pStringSink (pStringSink), m_pIStringSink (this) {
                retain (); {
                    IStringSink::Reference pIStringSinkRole;
                    getRole (pIStringSinkRole);
                    pStringSource->GetTimingReport (pIStringSinkRole);
                } untain ();
            }

        //  Destruction
        private:
            ~TimingReportGofer () {
            }

        //  IStringSink Role
        private:
            Vca::VRole<ThisClass,IStringSink> m_pIStringSink;
        public:
            void getRole (IStringSink::Reference &rpRole) {
                m_pIStringSink.getRole (rpRole);
            }

        //  IStringSink Methods
        public:
            void OnData (IStringSink *pRole, VString const &rString) {
                m_pStringSink->OnData (rString);
            }

        //  Role Player Overrides
        private:
            void OnError_(Vca::IError *pError, VString const &rMessage) {
                m_pStringSink->OnError (pError, rMessage);
            }

        //  State
        private:
            IStringSink::Reference const m_pStringSink;
        };

    //  Construction
    public:
        VPoolEvaluationResult (
            IEvaluationResult *pWorkerResult
        ) : m_pWorkerResult (pWorkerResult), m_pIEvaluationResult (this) {
        }

    //  Destruction
    private:
        ~VPoolEvaluationResult () {
        }

    //  IEvaluationResult Role
    private:
        Vca::VRole<ThisClass,IEvaluationResult> m_pIEvaluationResult;
    public:
        void getRole (IEvaluationResult::Reference &rpRole) {
            m_pIEvaluationResult.getRole (rpRole);
        }

    //  Interface Implementations
    public:
        void GetTimingReport (IEvaluationResult *pRole, IVReceiver<VString const&> *pReportSink) {
            if (pReportSink) {
                TimingReportGofer::Reference pGofer (new TimingReportGofer (pReportSink, m_pWorkerResult));
            }
        }

    //  State
    private:
        IEvaluationResult::Reference const m_pWorkerResult;
    };
}


/******************************
 ******************************
 *****                    *****
 *****  Vsa::VPoolWorker  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolWorker::VPoolWorker (
    Vca::U32 iIdentifier, VString const &rPID, VPoolWorkerGeneration *pGeneration, IEvaluator *pEvaluator
) : m_iIdentifier (iIdentifier),
    m_iPID (rPID),
    m_pEvaluator  (pEvaluator),
    m_pGeneration (pGeneration),
    m_pPool       (pGeneration->pool ()),
    m_cQueriesProcessed (0),
    m_cTotalQueryTime (0),
    m_iStatus (Worker_InStartup),
    m_iMode   (Worker_Offline),
    m_iRetirementCause (Worker_Employed),
    m_bAboutToRetire (false),
    m_pIEvaluatorEKG (new interface_monitor_t (this, &ThisClass::signalIEvaluatorEKG)),
    m_bToggleMode (false),
    m_pIPoolWorker (this)
{
    retain (); {
	m_pIEvaluatorEKG->monitor (pEvaluator);
    } untain ();
    traceInfo ("Creating VPoolWorker");
    log ("Creating Worker %s [%d] (Generation %d)", m_iPID.content (), m_iIdentifier, generationId ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolWorker::~VPoolWorker () {
    traceInfo ("Destroying VPoolWorker");
    log ("Destroying Worker %s [%d] (Generation %d)", m_iPID.content (), m_iIdentifier, generationId ());
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

Vca::U32 Vsa::VPoolWorker::generationId () const {
    return m_pGeneration->generationId ();
}

Vsa::IEvaluator* Vsa::VPoolWorker::evaluator () const {
    return m_pEvaluator;
}

Vsa::VPoolWorkerGeneration *Vsa::VPoolWorker::generation () const {
    return m_pGeneration;
}

bool Vsa::VPoolWorker::isFlushed () const {
    return identifier () <= m_pPool->lastToBeFlushedWorker ();
}

void Vsa::VPoolWorker::getWorkerStatus (VString &rStatus) const {
  switch (m_iStatus) {
  case Worker_InStartup:
    rStatus << "In Startup";
    break;
  case Worker_Available:
    rStatus << "Available";
    break;
  case Worker_InUse:
    rStatus << "In Use";
    break;
  case Worker_GoneFishing:
    rStatus << "Gone Fishing";
    break;
  default:
    rStatus << "???";
    break;
  }
}

void Vsa::VPoolWorker::getWorkerMode (VString &rMode) const {
  switch (m_iMode) {
  case Worker_Online:
    rMode << "Online";
    break;
  case Worker_Offline:
    rMode << "Offline";
    break;
  case Worker_Retired:
    rMode << "Retired";
    break;
  }
}

char const *Vsa::VPoolWorker::getWorkerMode () const {
  switch (m_iMode) {
  case Worker_Online:
    return "Online";
  case Worker_Offline:
    return "Offline";
  case Worker_Retired:
    return "Retired";
  }
  return "Unknown";
}

void Vsa::VPoolWorker::getWorkerRetirementCause (VString &rRetirementCause) const {
    switch (m_iRetirementCause) {
    case Worker_Employed:
        rRetirementCause << "N/A";
        break;
    case Worker_Cutback:
        rRetirementCause << "Cutback";
        break;
    case Worker_Failed:
        rRetirementCause << "Failure";
        break;
    }
}

bool Vsa::VPoolWorker::canProcess (VPoolEvaluation const *pEvaluation) const {
    bool bFound = false;
    Vca::U32 iWorkerId = pEvaluation->workerId ();
    if (iWorkerId == 0)
        bFound = true;
    else
        bFound = (iWorkerId == m_iIdentifier);
    return bFound;
}

bool Vsa::VPoolWorker::hasId (VString const &rId, bool bIsPID) const {
    if (bIsPID)
        return m_iPID == rId;
    else
        return m_iIdentifier == atoi (rId);
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vsa::VPoolWorker::setLastQuery (VEvaluation *pRequest) {

    switch (pRequest->queryType ()) {
        case VEvaluation::QueryType_Expression:
            m_iLastQuery.setTo (pRequest->queryString ());
            break;
        case VEvaluation::QueryType_URL:
        default:
            break;
    }
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vsa::VPoolWorker::evaluate (
    IEvaluatorClient *pClient, VPoolEvaluation *pRequest
) {
    setLastQuery (pRequest);
    log (
        "Worker(%d) Generation (%d) processing query (%d).",
        m_iIdentifier,
        generationId (),
        pRequest->index ()
    );
    pRequest->delegateTo (pClient, m_pEvaluator);
}

void Vsa::VPoolWorker::process (VPoolEvaluation *pRequest) {
    VEvaluatorPool::Reference const pPool (pool ());
    Vca::U32 const cLim = pPool->workerQueryHistoryLimit ();

    if (m_iHistoryQueue.size () >= cLim)
      m_iHistoryQueue.dequeue ();
    m_pCurrentCacheUnit = (new CacheUnit (pRequest->query (), pRequest->index ()));
    m_iHistoryQueue.enqueue (m_pCurrentCacheUnit);

    m_pWIP = (new WorkInProgress (this, pRequest));

    setStatus (Worker_InUse);
    if (m_iMode==Worker_Online)
      m_pGeneration->incrOnlineWIPs ();
    else
      m_pGeneration->incrOfflineWIPs ();
    m_pWIP->start ();

}

/*********************
 *****  Linkage  *****
 *********************/

void Vsa::VPoolWorker::removeFromGeneration () {
    m_pGeneration->retire (this);
}

void Vsa::VPoolWorker::returnToGeneration () {
    if (m_pWIP) m_pWIP.clear();
    m_pGeneration->returnWorker (this);
}

void Vsa::VPoolWorker::disable () {
    m_pGeneration->disable (this);
}

/************************
 *****  Statistics  *****
 ************************/

void Vsa::VPoolWorker::getStatistics (VString &rResult, Vca::S32 cQueryLines, Vca::S32 cQueries) const {

    VString rState;
    getWorkerMode (rState); rState << ":";
    getWorkerStatus (rState);
    Vca::U32 iAvgQueryTime = queriesProcessed () > 0 ?
                             ((Vca::U32)totalQueryTime ()/ queriesProcessed ()) : 0;
    VString iHistory; getQueryHistory (iHistory, cQueryLines, cQueries);
    VString iTimeStr; V::VTime iTime;
    iTime.asString (iTimeStr);
    Vca::U32 cQueriesInHistory = (cQueries == -1) ? m_iHistoryQueue.size (): cQueries;

    rResult << "\nWorker Statistics (at " << iTimeStr.content () <<")";
    rResult << "\nProcess Id: " << m_iPID;
    rResult << "\nPool Worker Id: " << m_iIdentifier;
    rResult << "\nState: " << rState;
    if (m_iMode == Worker_Retired) {
        VString iRetirementCause;
        getWorkerRetirementCause(iRetirementCause);
        rResult << "\nRetirement Cause           : " << iRetirementCause;
    }
    rResult << "\nWorker Generation          : " << generationId ();
    rResult << "\nNumber of Queries Processed: " << queriesProcessed ();
    rResult << "\nAverage Time Per Query     : " << iAvgQueryTime << " milliseconds";
    if (cQueriesInHistory > 1)
      rResult << "\n" << cQueriesInHistory  << " most recent queries in chronological order: " << iHistory;
    else if (cQueriesInHistory == 1)
	rResult << "\nMost recent query:" << iHistory;
    rResult << "\n";
}

/****
 * getStat: Returns statistics in line format for the pool generation to display in the worker list command
 *
 ***/

void Vsa::VPoolWorker::getStat (VString &rResult) const {

    VString iGen, iGenPadded;
    iGen << generationId (); iGenPadded.printf ("%-4s",iGen.content ());

    VString iID (m_iPID), iIDPadded;
    iID << "[" << m_iIdentifier << "]";
    iIDPadded.printf ("%-12s", iID.content ());

    VString iMode, iModePadded;
    getWorkerMode (iMode);
    iModePadded.printf ("%-8s", iMode.content ());

    VString iStatus, iStatusPadded;
    getWorkerStatus (iStatus);
    iStatusPadded.printf ("%-9s", iStatus.content ());

    Vca::U32 iQueriesProcessed = queriesProcessed ();
    VString iQueriesProcessedPadded;
    iQueriesProcessedPadded.printf ("%-10d", iQueriesProcessed);

    Vca::U32 iAvgQueryTime = queriesProcessed () > 0 ?
                             ((Vca::U32)totalQueryTime ()/ queriesProcessed ()) : 0;
    VString iAvgQueryTimePadded;
    iAvgQueryTimePadded.printf ("%-8d", iAvgQueryTime);

    rResult << "\n" << iGenPadded << "  ";
    rResult << iIDPadded << "  ";
    rResult << iModePadded << "  ";
    rResult << iStatusPadded << "  ";
    rResult << iQueriesProcessedPadded << "  ";
    rResult << iAvgQueryTimePadded;
}

void Vsa::VPoolWorker::quickStats (VString &rResult) const {
    // Get our data.
    VString iMode, iStatus;
    getWorkerMode (iMode);
    getWorkerStatus (iStatus);
    Vca::U32 iAvgQueryTime = queriesProcessed () > 0 ?
                             ((Vca::U32)totalQueryTime ()/ queriesProcessed ()) : 0;

    // Format our data.
    VString iResult;
    iResult.printf(
            "%5d | %5d | %5s | %10s | %12s | %10d | %10d",
            generationId (),
            m_iIdentifier,
            m_iPID.content (),
            iMode.content (),
            iStatus.content (),
            queriesProcessed (),
            iAvgQueryTime);

    // Insert newline as necessary.
    if (rResult.length () > 0) rResult << "\n";

    // Put our data into the result string.
    rResult << iResult;
}

void Vsa::VPoolWorker::quickStatsHeader (VString &rResult) {
    VString iResult;
    iResult.printf(
            "%5s | %5s | %5s | %10s | %12s | %10s | %10s",
            "Gen",
            "ID",
            "PID",
            "Mode",
            "Status",
            "Queries",
            "Avg Time");
    rResult.setTo(iResult);
}


/**********************
 **********************
 *****  History   *****
 **********************
 **********************/

Vca::U32 Vsa::VPoolWorker::getHistoryLength () const {
  return m_iHistoryQueue.size ();
}

void Vsa::VPoolWorker::getQueryHistory (VString &rHistory, Vca::S32 cQueryLines, Vca::S32 cQueries) const {
    VEvaluatorPool::Reference const pPool (pool ());
      WorkerHistoryQueue::Iterator iterator (m_iHistoryQueue);

      Vca::U32 cLen = getHistoryLength ();
      cQueries = cQueries == -1 ? cLen : cQueries;

      if (static_cast<Vca::U32>(cQueries) > cLen)
        cQueries = cLen;
      else {
        Vca::U32 cByPass = cLen - cQueries;
        while (cByPass-- >0)
          ++iterator;
      }

      while (iterator.isNotAtEnd () && cQueries-->0) {
        CacheUnit::Reference pCache;
        if (iterator.current (pCache)) {
          // Get our start time.
          V::VTime rStartTime;
          VString iStartTimeString;
          if (pCache->getStartTime (rStartTime)) rStartTime.asString (iStartTimeString);
          else iStartTimeString = "N/A";

          // Get our stop time.
          V::VTime rEndTime;
          VString iEndTimeString;
          if (pCache->getEndTime (rEndTime)) rEndTime.asString (iEndTimeString);
          else iEndTimeString = "N/A";

          // Get our query string.
          VPathQuery::Reference const pQuery (pCache->getPathQuery ());

          VString const iQueryString (pQuery->queryString ());
          Vca::S32 const xIndex = iQueryString.index ('\n', cQueryLines);
          VString iTruncatedStr;
          if (xIndex >=0) {
            iTruncatedStr.setTo (iQueryString, xIndex);
            iTruncatedStr << "\n.........";
          }
          else
            iTruncatedStr.setTo (iQueryString);

          // Print our information.
          rHistory << "\n================================================================================\n";
          rHistory <<"Query " << pCache->getQueryId () << ":\n";
          rHistory << iTruncatedStr;
          rHistory << "\nStart Time: " << iStartTimeString;
          rHistory << "\nEnd Time: " << iEndTimeString << "\n";
        }
        ++iterator;
      }
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vsa::VPoolWorker::retire (Worker_RetirementCause iCause) {
    if (m_pWIP) m_pWIP.clear();
    setRetiredMode ();
    m_iRetirementCause = iCause;
    cancelIEvaluatorEKG ();
    m_pEvaluator.clear();
    setStatus(Worker_GoneFishing);
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************
 *****  Vsa::IPoolWorker  *****
 ******************************/

void Vsa::VPoolWorker::getRole (IPoolWorker::Reference &rpRole) {
    m_pIPoolWorker.getRole (rpRole);
}

void Vsa::VPoolWorker::GetStatistics (
    IPoolWorker *pRole, IVReceiver<VString const&>* pReceiver
) {
    if (pReceiver) {
        VString result;
        Vca::U32 cLines = 8;
        getStatistics (result, cLines);
        pReceiver->OnData (result);
    }
}


/**********************
 *****  Snapshot  *****
 **********************/

void Vsa::VPoolWorker::dumpHistory (
   VString const &rFile, IVReceiver <VString const &>*pClient
   ) {

    V::VSimpleFile iFile;
    VString iOutput;
    bool bResult = iFile.Open (rFile.content (), "a");
    if (bResult) {
      VString iResult;
      getStatistics (iResult);
      iFile.printf (iResult.content ());
      iOutput.printf ("Wrote Worker History to %s", rFile.content ());
      iFile.close ();
  }
  else
    iOutput.printf ("Error opening file %s", rFile.content ());

  if (pClient)
    pClient->OnData (iOutput);
}


/*****************
 *****  EKG  *****
 *****************/

void Vsa::VPoolWorker::signalIEvaluatorEKG () {
    traceInfo ("IEvaluator disappeared.");
    log ("IEvaluator for worker %s [%d] (Generation %d) no longer connected; disabling worker.", m_iPID.content (), m_iIdentifier, generationId ());

    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    // Continue handling of this error appropriately.
    if (m_pWIP) {
        // There's a WIP. Let it handle the error.
        m_pWIP->OnError (0, 0, "Evaluator disconnected.");
    } else {
        // There's no WIP. Handle the error ourselves.
        m_pGeneration->disable (this);
    }
}

void Vsa::VPoolWorker::cancelIEvaluatorEKG () {
    if (m_pIEvaluatorEKG.isntNil ()) {
        m_pIEvaluatorEKG->cancel ();
        m_pIEvaluatorEKG.clear ();
    }
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vsa::VPoolWorker::WorkInProgress  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolWorker::WorkInProgress::WorkInProgress (
    VPoolWorker *pWorker, VPoolEvaluation *pRequest
) : m_pWorker           (pWorker)
,   m_pGeneration       (pWorker->generation ())
,   m_pRequest          (pRequest)
,   m_xState            (State_New)
,   m_pPool             (pWorker->pool ())
,   m_pIEvaluatorClient (this)
,   m_bTimedOut         (false)
{
    aggregate (pRequest->evaluatorClient ());
    traceInfo ("Creating VPoolWorker::WorkInProgress");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolWorker::WorkInProgress::~WorkInProgress () {
    traceInfo ("Destroying VPoolWorker::WorkInProgress");
    switch (m_xState) {
    case State_New:
        m_pWorker->returnToGeneration ();
        break;
    default:
        break;
    }
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::U32 Vsa::VPoolWorker::WorkInProgress::evaluationIndex () const {
    return m_pRequest->index ();
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/***********************************
 *****  Vsa::IEvaluatorClient  *****
 ***********************************/

void Vsa::VPoolWorker::WorkInProgress::getRole (IEvaluatorClient::Reference &rpRole) {
    m_pIEvaluatorClient.getRole (rpRole);
}


void Vsa::VPoolWorker::WorkInProgress::OnAccept (
    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
) {
  m_pRemoteIEvaluation.setTo (pEvaluation);

  if (m_pRequest->anyData ())
    onAnyDataUpdate ();
}

void Vsa::VPoolWorker::WorkInProgress::OnChange (
    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
) {
}

/**
 * Callback that fires when a result is returned to this worker by its delegate process.
 *
 * Sets the state to completed, removes this WIP from the Pool's
 * WIP List. If the timer associated with this WIP is non-nil then removes
 * it from the global timer list to stop firing. If hasnt timed out calls
 * the onResult of the request and returns the worker to the generation.
 */
void Vsa::VPoolWorker::WorkInProgress::OnResult (
    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
) {
    VString iShortenedOutput;
    if (m_pPool->resultLogLength()) rOutput.getSummary (iShortenedOutput, m_pPool->resultLogLength ());
    log ("Worker(%d) (Generation %d) returned: %s",
        m_pWorker->identifier (),
        m_pWorker->generationId (),
        m_pPool->resultLogLength() ? iShortenedOutput.content () : rOutput.content ()
    );

    m_xState = State_Completed;
    m_pRemoteIEvaluation.clear ();
    if (isOnlineWIP ())
      m_pGeneration->decrOnlineWIPs();
    else
      m_pGeneration->decrOfflineWIPs ();
    m_pGeneration->detach (this);


    if (m_pTimer.isntNil ()) {
        m_pTimer->cancel ();
        m_pTimer.clear ();
    }

    if (isTimedOut ())
        m_pRequest->onComplete ();
    else {
        VPoolEvaluationResult::Reference pPoolResult (new VPoolEvaluationResult (pResult));
        IEvaluationResult::Reference pIEvaluationResult;
        pPoolResult->getRole (pIEvaluationResult);
        m_pRequest->onResult (pIEvaluationResult, rOutput);
    }

    m_pWorker->updateQueryStatistics (1, m_pRequest->evaluationTime ());
    m_pWorker->returnToGeneration ();
}

void Vsa::VPoolWorker::WorkInProgress::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rText
) {
    Reference iRetainer (this);
    //  already completed, successfully or otherwise ...
    if (State_Failed == m_xState || State_Completed == m_xState)
      return;

    log ("Worker(%d) (Generation %d) failed...disabled:%s",
        m_pWorker->identifier (), m_pWorker->generationId (), rText.content ()
    );

    m_xState = State_Failed;
    m_pRemoteIEvaluation.clear ();
    if (isOnlineWIP ())
      m_pGeneration->decrOnlineWIPs();
    else
      m_pGeneration->decrOfflineWIPs ();
    m_pGeneration->detach (this);

    if (m_pTimer.isntNil ()) {
        m_pTimer->cancel ();
        m_pTimer.clear ();
    }

    m_pWorker->disable ();
    m_pGeneration->hire();
    m_pRequest->incrementEvaluationAttempt ();

    if (!retryEvaluation ())
        m_pRequest->onError (pError, rText);

    m_pPool->onQueue ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vsa::VPoolWorker::WorkInProgress::start () {
    m_xState = State_Active;

    m_pGeneration->attach (this);
    setTimeOut ();

    IEvaluatorClient::Reference pIEvaluatorClient;
    getRole (pIEvaluatorClient);
    m_pWorker->evaluate (pIEvaluatorClient, m_pRequest);
}

/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

bool Vsa::VPoolWorker::WorkInProgress::setTimeOut () {
    Vca::U64 iPoolTimeOut = m_pPool->evaluationTimeOut ();

    if (iPoolTimeOut == U64_MAX)
        return false;
    else {
        Vca::VTrigger<ThisClass> *pTrigger =
            new Vca::VTrigger<ThisClass> (this, &ThisClass::onTimeOut);
        m_pTimer.setTo (new Vca::VTimer (pTrigger, iPoolTimeOut));
        m_pTimer->start ();
        return true;
    }
}

void Vsa::VPoolWorker::WorkInProgress::onTimeOut (
    Vca::VTrigger<ThisClass> *pTrigger
) {
    log ("WIP TimedOut (Worker %d)", m_pWorker->identifier ());

    m_bTimedOut = true;
    m_pTimer->cancel ();
    m_pTimer.clear ();
    m_pRequest->incrementEvaluationAttempt ();

    if (m_pRemoteIEvaluation.isntNil ()) {
	m_pRemoteIEvaluation->Cancel ();
	log ("WIP cancelling remote evaluation");
    }

    if (!retryEvaluation ()) {
        log ("Retry attempts completed for request %d...returning ERROR", m_pRequest->index ());
        m_pRequest->onError (0, "Evaluation Timed Out");
        m_pRemoteIEvaluation.clear ();
    }
    m_pPool->onQueue ();
}

bool Vsa::VPoolWorker::WorkInProgress::retryEvaluation () {
    //  evaluation should be a normal pool evaluation (not a broadcast evaluation and
    //  evaluation retry limit shouldnt have reached
    //  evaluation should be retryable

    if (m_pRequest->retryable() &&
        m_pRequest-> evaluationAttempt () < m_pPool->evaluationAttemptMaximum ()) {
        log ("Retrying WIP");
        m_pPool->schedule (m_pRequest);
        return true;
    }
    return false;
}

bool Vsa::VPoolWorker::WorkInProgress::isOnlineWIP () const {
  return m_pGeneration->isOnlineWIP (this);
}


/*****
 * Note: onAnyDataUpdate ()
 *       This function helps to propagate the "-anyData" information to the Pool Evaluator which is
 *       processing the query held by this WorkInProgress (WIP).
 *       If the Evaluator which is processing this WIP is not a Pool Evaluator then just ignores...
 *
 ****/


void Vsa::VPoolWorker::WorkInProgress::onAnyDataUpdate () {
    if (IPoolEvaluation_Ex1 *const pRemoteIEvaluation = dynamic_cast <IPoolEvaluation_Ex1*> (m_pRemoteIEvaluation.referent())) {
        pRemoteIEvaluation->UseAnyGenerationWorker ();
    }
}

void Vsa::VPoolWorker::WorkInProgress::onRetryableUpdate () {
    if (IPoolEvaluation_Ex1 *const pRemoteIEvaluation = dynamic_cast <IPoolEvaluation_Ex1*> (m_pRemoteIEvaluation.referent())) {
        pRemoteIEvaluation->SetRetryable(m_pRequest->retryable());
    }
}

/*****************************************
 *****************************************
 *****                               *****
 *****  Vsa::VPoolWorker::CacheUnit  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolWorker::CacheUnit::CacheUnit (VPathQuery *pQuery, unsigned int xQuery)
    : m_pPathQuery (pQuery),
      m_xQuery (xQuery),
      m_bStarted (true),
      m_bFinished (false) {
}
/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolWorker::CacheUnit::~CacheUnit () {
}
