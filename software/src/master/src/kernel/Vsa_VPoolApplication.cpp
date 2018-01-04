/*****  Vsa_VPoolApplication Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VPoolApplication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"
#include "Vca_VClassInfoHolder.h"

#include "VSimpleFile.h"

#include "Vca_VTrigger.h"
#include "Vca_VcaListener.h"
#include "Vca_VConnectionFactory.h"
#include "Vca_VConnectionPipeSource.h"
#include "Vca_VProcessFactory.h"
#include "Vca_VRexecConnectionFactory.h"

#include "Vsa_IPoolApplication_Ex2.h"
#include "Vsa_VEvaluatorPump.h"
#include "Vsa_VEvaluatorPumpSource.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vsa::VPoolApplication  *****
 *****                         *****
 ***********************************
 ***********************************/

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

Vca::VClassInfoHolder &Vsa::VPoolApplication::ClassInfo () {
    static Vca::VClassInfoHolder_<ThisClass> iClassInfoHolder;
    if (iClassInfoHolder.isntComplete ()) {
	iClassInfoHolder
	    .addBase (BaseClass::ClassInfo ())

	    .addProperty ("queueLength", &ThisClass::queueLengthTrackable)

	    .markCompleted ();
    }
    return iClassInfoHolder;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolApplication::VPoolApplication (Context *pContext)
: BaseClass (pContext)
, m_bHardStopPending (false)   // to maintain compatibility with old vers
, m_pIPoolApplication (this)   // to maintain compatibility with old vers
, m_bPlumbingWorkers (commandSwitchValue ("plumbWorkers", "VPoolPlumbWorkers"))
, m_pIPoolApplication_Ex(this)
, m_pIEvaluator (this)
, m_pIEvaluatorPool (this)
, m_pIUpDownPublisher (this) {
  traceInfo ("Creating VPoolApplication");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolApplication::~VPoolApplication () {
  traceInfo ("Destroying VPoolApplication");
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/***********************************
 *****  Vsa::IPoolApplication  *****
 ***********************************/

/********************************************************************
 ****  Supported solely for the purpose of maintaining          *****
 ****  compatibility with the older pools	                *****
 ********************************************************************/

void Vsa::VPoolApplication::getRole (IPoolApplication::Reference &rpRole) {
    m_pIPoolApplication.getRole (rpRole);
}

void Vsa::VPoolApplication::Stop (IPoolApplication *pRole) {
    stop ();
}

void Vsa::VPoolApplication::GetPoolWorkerProcessSource (
    IPoolApplication *pRole, IVReceiver<VString const &> *pClient
) {
}

void Vsa::VPoolApplication::SetPoolWorkerProcessSource (
    IPoolApplication *pRole, VString const &iValue
) {
    setPoolWorkerProcessSourceTo (iValue);
}

/*******************************
 *****  Vsa::IApplication  *****
 *******************************/

void Vsa::VPoolApplication::GetStatistics (
    Vca::IApplication *pRole, IVReceiver<VString const &> *pClient
) {
    if (pClient) {
	VString result;
	getStatistics (result);
        pClient->OnData (result);
    }
}
/***************************************
 *****  Vsa::IPoolApplication_Ex1  *****
 ***************************************/

/****
 * Note:   Graceful Stop:
 *         This new PoolApp's Stop method does a graceful stop of the application.
 *	 If the new PoolApp gets a Stop from an Old client (which expects a hardStop),
 *	 we currently check for it and provide the hardstop. But in future PoolApp
 *	 versions they will go out and the PoolApp's Stop will just do graceful stop
 *	 of the Application alone.
 ****/

void Vsa::VPoolApplication::Stop (IPoolApplication_Ex1 *pRole) {
    stop (m_bHardStopPending);  // to support old pooladmins HardStop 
    m_bHardStopPending = false;
}

void Vsa::VPoolApplication::GetPoolWorkerProcessSource (
    IPoolApplication_Ex1 *pRole, IVReceiver<VString const &> *pClient
) {
}

void Vsa::VPoolApplication::SetPoolWorkerProcessSource (
    IPoolApplication_Ex1 *pRole, VString const &iValue
) {
    setPoolWorkerProcessSourceTo (iValue);
}

/***************************************
 *****  Vsa::IPoolApplication_Ex2  *****
 ***************************************/

void Vsa::VPoolApplication::getRole (IPoolApplication_Ex2::Reference &rpRole) {
    m_pIPoolApplication_Ex.getRole (rpRole);
}

/****
 * Note:   Hard Stop:
 *         Stops the pool application, waiting for current work to finish and then 
 *         terminates the connections
 ****/

void Vsa::VPoolApplication::HardStop (IPoolApplication_Ex2 *pRole) {
    hardstop ();
}

/*****************************
 *****  Vsa::IEvaluator  *****
 *****************************/

void Vsa::VPoolApplication::getRole (IEvaluator::Reference &rpRole) {
  m_pIEvaluator.getRole (rpRole);
}

void Vsa::VPoolApplication::EvaluateExpression (
    IEvaluator*		pRole,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rExpression
) {
    if (m_pEvaluatorPool)
	m_pEvaluatorPool->EvaluateExpression (pRole, pClient, rPath, rExpression);
    else if (pClient)
	pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::EvaluateURL (
    IEvaluator*		pRole,
    IEvaluatorClient*	pClient,
    VString const&	rPath,
    VString const&	rQuery,
    VString const&	rEnvironment
) {
    if (m_pEvaluatorPool)
	m_pEvaluatorPool->EvaluateURL (pRole, pClient, rPath, rQuery, rEnvironment);
    else if (pClient)
	pClient->OnError (0, "Evaluator not initialized");
}

/***********************************
 *****  Vsa::IUpDownPublisher  *****
 **********************************/

void Vsa::VPoolApplication::getRole (IUpDownPublisher::Reference &rpRole) {
    m_pIUpDownPublisher.getRole (rpRole);
}

void Vsa::VPoolApplication::Subscribe (IUpDownPublisher *pRole, IUpDownSubscriber *pSub, IVReceiver<ISubscription*> *pRec) {
    if (m_pEvaluatorPool)
	m_pEvaluatorPool->Subscribe (pRole, pSub, pRec);
}

/*********************************
 *****  Vsa::IEvaluatorPool  *****
 *********************************/

void Vsa::VPoolApplication::GetSettings (
    IEvaluatorPool *pRole, IVReceiver<IEvaluatorPoolSettings*>*pClient
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->GetSettings (pRole, pClient);
    else if (pClient)
	pClient->OnData (0);
}

void Vsa::VPoolApplication::AddEvaluator (
    IEvaluatorPool *pRole, IEvaluator *pEvaluator
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->AddEvaluator (pRole, pEvaluator);
}

void Vsa::VPoolApplication::FlushWorkers (
    IEvaluatorPool *pRole, IVReceiver<bool>*pClient
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->FlushWorkers (pRole, pClient);
    else if (pClient)
	pClient->OnData (false);
}

/*************************************
 *****  Vsa::IEvaluatorPool_Ex1  *****
 *************************************/

void Vsa::VPoolApplication::IsWorkerCreationFailureLmtReached (
    IEvaluatorPool_Ex1* pRole, IVReceiver<bool>* pReceiver
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->IsWorkerCreationFailureLmtReached (pRole, pReceiver);
    else if (pReceiver)
	pReceiver->OnData (false);
}

void Vsa::VPoolApplication::GetCurrentGenerationTotalWorkers (
    IEvaluatorPool_Ex1* pRole, IVReceiver<Vca::U32>* pReceiver
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->GetCurrentGenerationTotalWorkers (pRole, pReceiver);
    else if (pReceiver)
	pReceiver->OnData (0);
}

void Vsa::VPoolApplication::HardRestart (
    IEvaluatorPool_Ex1* pRole, IVReceiver<bool>* pReceiver
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->HardRestart (pRole, pReceiver);
    else if (pReceiver)
	pReceiver->OnData (false);
}

void Vsa::VPoolApplication::GetWorkerInterface (
    IEvaluatorPool_Ex1 *pRole, IVReceiver<IVUnknown*>* pReceiver, Vca::U32 connection
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->GetWorkerInterface (pRole, pReceiver, connection);
    else if (pReceiver)
	pReceiver->OnData (0);
}

void Vsa::VPoolApplication::GetTotalWorkers (
    IEvaluatorPool_Ex1* pRole, IVReceiver<Vca::U32>* pReceiver
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->GetTotalWorkers (pRole, pReceiver);
    else if (pReceiver)
	pReceiver->OnData (0);
}

/****
 * Note: HardStop - Implementation changed in new version of PoolApp.
 *       New versions of PoolApps ignore direct IEvaluatorPool based HardStops.
 *       HardStops have to go via IPoolApplication which does a hard stop of the 
 *       entire pool application (listeners, pool etc)
 ****/

/********************************************************************
 ****  Supported solely for the purpose of maintaining          *****
 ****  compatibility with the older pools	                *****
 ********************************************************************/

void Vsa::VPoolApplication::HardStop (
    IEvaluatorPool_Ex1* pRole, IVReceiver<bool>* pReceiver
) {
  if (m_pEvaluatorPool) {	// to support old clients
      m_bHardStopPending = true;
      if (pReceiver)
          pReceiver->OnData (true);
  } else if (pReceiver)
      pReceiver->OnData (false);
}   

/*************************************
 *****  Vsa::IEvaluatorPool_Ex2  *****
 *************************************/

void Vsa::VPoolApplication::BroadcastExpression (
   IEvaluatorPool_Ex2* pRole, IEvaluatorClient* pClient, VString const &rPath, VString const & rQuery
) {
    if (m_pEvaluatorPool) 
	m_pEvaluatorPool->BroadcastExpression (pRole, pClient, rPath, rQuery);
    else if (pClient)
	pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::BroadcastURL (
   IEvaluatorPool_Ex2* pRole, IEvaluatorClient* pClient, VString const & rPath, VString const &rExpression, VString const &rContext
) {
    if (m_pEvaluatorPool)
	m_pEvaluatorPool->BroadcastURL (pRole, pClient, rPath, rExpression, rContext);
    else if (pClient)
	pClient->OnError (0, "Evaluator not initialized");
}

/*************************************
 *****  Vsa::IEvaluatorPool_Ex3  *****
 *************************************/

void Vsa::VPoolApplication::TakeWorkerOffline (
   IEvaluatorPool_Ex3* pRole, VString const & iWorkerId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->TakeWorkerOffline (pRole, iWorkerId, bIsPID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::MakeWorkerOnline (
   IEvaluatorPool_Ex3* pRole, VString const & iWorkerId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->MakeWorkerOnline (pRole, iWorkerId, bIsPID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::EvaluateUsingOfflineWorker (
   IEvaluatorPool_Ex3* pRole, VString const &rQuery, VString const & iWorkerId, bool bIsPID, IEvaluatorClient* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->EvaluateUsingOfflineWorker (pRole, rQuery, iWorkerId, bIsPID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::RetireOfflineWorker (
   IEvaluatorPool_Ex3* pRole, VString const & iWorkerId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->RetireOfflineWorker (pRole, iWorkerId, bIsPID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::DumpWorker (
    IEvaluatorPool_Ex3* pRole, VString const & iWorkerId, bool bIsPID, VString const &rFile, 
    IVReceiver<VString const &>*pClient
) {
  if (m_pEvaluatorPool) 
    m_pEvaluatorPool->DumpWorker (pRole, iWorkerId, bIsPID, rFile, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::DumpPoolQueue (
   IEvaluatorPool_Ex3* pRole, VString const &rFile, Vca::U32 cQueries, IVReceiver<VString const &>* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->DumpPoolQueue (pRole, rFile, cQueries, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}
        
void Vsa::VPoolApplication::DumpWorkingWorkers (
   IEvaluatorPool_Ex3* pRole, VString const &rFile, IVReceiver<VString const &>* pClient
) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->DumpWorkingWorkers (pRole, rFile, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");
}

void Vsa::VPoolApplication::GetWorker_Ex  (IEvaluatorPool_Ex3* pRole, VString const &rID, bool bIsPID, IVReceiver<IVUnknown*>* pClient) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->GetWorker_Ex (pRole, rID, bIsPID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");  
}

void Vsa::VPoolApplication::GetWorkersStatistics (IEvaluatorPool_Ex3* pRole, IVReceiver<VString const &>* pClient) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->GetWorkersStatistics (pRole, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");  
}

void Vsa::VPoolApplication::GetGenerationStatistics (IEvaluatorPool_Ex3* pRole, Vca::U32 iID, IVReceiver<VString const &>* pClient) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->GetGenerationStatistics (pRole, iID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");  
}

/*************************************
 *****  Vsa::IEvaluatorPool_Ex4  *****
 *************************************/

void Vsa::VPoolApplication::getRole (IEvaluatorPool_Ex4::Reference &rpRole) {
    if (m_pEvaluatorPool)
	m_pIEvaluatorPool.getRole (rpRole);
}

void Vsa::VPoolApplication::QueryDetails (IEvaluatorPool_Ex4* pRole, Vca::U32 iID, IVReceiver<VString const &>* pClient) {
  if (m_pEvaluatorPool)
    m_pEvaluatorPool->QueryDetails (pRole, iID, pClient);
  else if (pClient)
    pClient->OnError (0, "Evaluator not initialized");  
}

void Vsa::VPoolApplication::CancelQuery (IEvaluatorPool_Ex4* pRole, Vca::U32 iID, IVReceiver<bool>* pClient) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->CancelQuery (pRole, iID, pClient);
}

/************************************
 *****  Vsa::IEvaluatorControl  *****
 ************************************/

void Vsa::VPoolApplication::Restart (
    IEvaluatorControl* pRole, IVReceiver<bool>* pReceiver
) {
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->Restart (pRole, pReceiver);
    else if (pReceiver)
	pReceiver->OnData (false);
}

/**************************
 *****  Vca::IGetter  *****
 **************************/

bool Vsa::VPoolApplication::GetU32Value_(
    IVReceiver<Vca::U32> *pResult, VString const &rKey
) {
    if (rKey.equalsIgnoreCase("QueryCount")) pResult->OnData(m_pEvaluatorPool->queriesProcessed ());
    else if (rKey.equalsIgnoreCase ("MinWorkers")) pResult->OnData(m_pEvaluatorPool->workerMinimum ());
    else if (rKey.equalsIgnoreCase ("MaxWorkers")) pResult->OnData(m_pEvaluatorPool->workerMaximum ());
    else if (rKey.equalsIgnoreCase ("QueueLength")) pResult->OnData(m_pEvaluatorPool->queueLength ());
    else if (rKey.equalsIgnoreCase ("ShrinkTimeRemaining")) pResult->OnData(m_pEvaluatorPool->shrinkTimeRemaining());
    else if (rKey.equalsIgnoreCase ("AssertionsFailed")) pResult->OnData(m_pEvaluatorPool->checkAssertions());
    else return BaseClass::GetU32Value_(pResult, rKey);
    return true;
}

bool Vsa::VPoolApplication::GetStringValue_(
    IVReceiver<VString const &> *pResult, VString const &rKey
) {
    if (rKey.equalsIgnoreCase ("statDump")) {
        VString rResult;
        m_pEvaluatorPool->dumpStats (rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("queueDump")) {
        VString rResult;
        m_pEvaluatorPool->queueDump(rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("queueDumpHeader")) {
        VString rResult;
        m_pEvaluatorPool->queueDumpHeader(rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("settingsDump")) {
        VString rResult;
        m_pEvaluatorPool->dumpSettings (rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("quickStatsHeader")) {
        VString rResult;
        char iAppendToResult[16];
        snprintf(iAppendToResult, sizeof(iAppendToResult), " | %10s", "Clients");
        m_pEvaluatorPool->quickStatsHeader (rResult);
        rResult << iAppendToResult;
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("quickStats")) {
        VString rResult;
        char iAppendToResult[16];
        snprintf(iAppendToResult, sizeof(iAppendToResult), " | %10d", activeOfferCount ());
        m_pEvaluatorPool->quickStats (rResult);
        rResult << iAppendToResult;
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("workerStatsHeader")) {
        VString rResult;
        m_pEvaluatorPool->dumpWorkerStatsHeader (rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("workerStats")) {
        VString rResult;
        m_pEvaluatorPool->dumpWorkerStats (rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("generationStatsHeader")) {
        VString rResult;
        m_pEvaluatorPool->dumpGenerationStatsHeader (rResult);
        pResult->OnData (rResult);
    } else if (rKey.equalsIgnoreCase ("generationStats")) {
        VString rResult;
        m_pEvaluatorPool->dumpGenerationStats (rResult);
        pResult->OnData (rResult);
    } else return BaseClass::GetStringValue_(pResult, rKey);
    return true;
}

/****
 * Note: New versions of PoolApps ignore direct IEvaluator based Stops.
 *       Stop has to go via IPoolApplication which does a graceful
 *       stop of the entire pool application (listeners, pool etc)
 ****/
 
/********************************************************************
 ****  Supported solely for the purpose of maintaining          *****
 ****  compatibility with the older pools	                *****
 ********************************************************************/

void Vsa::VPoolApplication::Stop (
    IEvaluatorControl* pRole, IVReceiver<bool>* pReceiver
) {
/////////////
//  For consistency with the other definitions of Stop, this could also be...
//  bool const bTrue = stop () || isStopping () || isAwaitingStop ();
    bool const bTrue = true;
    if (pReceiver)
	pReceiver->OnData (bTrue);
}

void Vsa::VPoolApplication::Suspend (
    IEvaluatorControl* pRole, IVReceiver<bool>* pReceiver
) {
    bool const bTrue = pause () || isPausing ();
    if (pReceiver)
	pReceiver->OnData (bTrue);
}

void Vsa::VPoolApplication::Resume (
    IEvaluatorControl* pRole, IVReceiver<bool>* pReceiver
) {
    bool const bTrue = resume () || isResuming ();
    if (pReceiver)
	pReceiver->OnData (bTrue);
}

void Vsa::VPoolApplication::GetStatistics (
    IEvaluatorControl* pRole, IVReceiver<VString const&>* pReceiver
) {
    if (m_pEvaluatorPool) {
	if (pReceiver) {
	  VString iStat; 
          //  Alerts
	  if (isAwaitingStop ()) 
	    iStat << "\nALERT: Stop received. Pool waiting for " << activeOfferCount () << " connections to go down....";
          if (m_pEvaluatorPool->isWorkerCreationFailureHardLmtReached ())
            iStat << "\nALERT: Pool Worker Creation Failure HARD Limit Reached...RESTART POOL";

          char iTimeString[26]; VString iTimeStr ("-\n"); V::VTime iCurrentTime;
          if (iCurrentTime.ctime (iTimeString))
            iTimeStr.setTo (iTimeString);
          iStat << "\nPool Statistics as of          : " << iTimeStr;
          iStat << "Number of clients connected    : " << activeOfferCount () << "\n";

	  m_pEvaluatorPool->getStatistics (iStat);
	  pReceiver->OnData (iStat);
	}
    }
    else if (pReceiver)
	pReceiver->OnData ("Control Interface not Initialized");
}

void Vsa::VPoolApplication::GetEvaluator (
    IEvaluatorControl* pRole, IEvaluatorSink* pSink
) {
    IEvaluator::Reference pIEvaluatorRole;
    getRole (pIEvaluatorRole);
    if (pSink)
	pSink->OnData (pIEvaluatorRole);
}
void Vsa::VPoolApplication::GetWorker (
    IEvaluatorControl *pRole, IVReceiver<IVUnknown*>* pReceiver, Vca::U32 connection
) {
    //  provided for compatibility with older pool admin versions
    if (m_pEvaluatorPool)
        m_pEvaluatorPool->GetWorkerInterface (0, pReceiver, connection);
    else if (pReceiver)
	pReceiver->OnData (0);
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vsa::VPoolApplication::setPoolWorkerProcessSourceTo (VString const &rWorkerSource) {
    Vca::VProcessFactory::PipeSource::Reference pProcessSource;
    if (Vca::VProcessFactory::Supply (pProcessSource, rWorkerSource, true)) {
	Vca::IPipeSource::Reference pPipeSource;
	pProcessSource->getRole (pPipeSource);

	//  Create settings for the pumps, ...
	VEvaluatorPump::PumpSettings::Reference pPumpSettings (
	    new VEvaluatorPump::PumpSettings ()
	);
	//  Create a pump source, ...
	VEvaluatorPumpSource::Reference pPumpSource (
	    new VEvaluatorPumpSource (pPumpSettings, pPipeSource, m_bPlumbingWorkers)
	);

	IEvaluatorSource::Reference pIEvaluatorSource;
	pPumpSource->getRole (pIEvaluatorSource);

	if (m_pEvaluatorPool.isntNil ())
	    m_pEvaluatorPool->setWorkerSourceTo (pIEvaluatorSource);
    }
}


/*************************
 *************************
 *****  Transitions  *****
 *************************
 *************************/

bool Vsa::VPoolApplication::start_() {
    if (!BaseClass::start_())
	return false;

    CommandCursor iCommandCursor (this);
    char const *const pListenerAddress = iCommandCursor.nextToken ();
    char const *const pWhat = iCommandCursor.nextToken ();
    if (pListenerAddress && pWhat) {
	if (strcmp (pWhat, "cpool") == 0 ||
	    strcmp (pWhat, "epool") == 0 ||
	    strcmp (pWhat, "rpool") == 0) {
	    char const *pPositionalCommand = iCommandCursor.nextToken ();

	//  Max Worker
	    char const *pCommand;
	    Vca::U32 cMaxWorkers = 1;
	    if (pCommand = commandStringValue ("maxWorkers", "MaxWorkers"))
		cMaxWorkers = atoi (pCommand);
	    else if (pPositionalCommand)
		cMaxWorkers = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Workers Being Created
	    Vca::U32 cWorkersBeingCreated = 1;
	    if (pCommand = commandStringValue ("workersBeingCreated", "WorkersInCreation"))
		cWorkersBeingCreated = atoi (pCommand);
	    else if (pPositionalCommand)
		cWorkersBeingCreated = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Worker Source
	    VString iWorkerSource;
	    if (pCommand = commandStringValue ("workerSource", "WorkerSource"))
		iWorkerSource = pCommand;
	    else if (pPositionalCommand)
		iWorkerSource = pPositionalCommand ;
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Create a pipe source used to create new workers, ...
	    Vca::IPipeSource::Reference pPipeSource;
	    if (strcmp (pWhat, "epool") == 0) {
	    //  ... as a process launcher:
		Vca::VProcessFactory::PipeSource::Reference pProcessSource;
		if (m_bPlumbingWorkers)
		    iWorkerSource << " -- -plumbed";
		Vca::VProcessFactory::Supply (pProcessSource, iWorkerSource, true);
		pProcessSource->getRole (pPipeSource);
	    }
	    else if (strcmp (pWhat, "cpool") == 0) {
	    //  ... a connection maker:
		Vca::VConnectionPipeSource::Reference pConnectionSource;
		Vca::VConnectionFactory::Supply (pConnectionSource, iWorkerSource, false, Vca::Default::TcpNoDelaySetting ());
		pConnectionSource->getRole (pPipeSource);
	    }
	    else {
	    //  ... or an rexec connection maker:

	    //  HostName
		VString iHostName;
		if (pCommand = commandStringValue ("hostName", "HostName"))
		    iHostName = pCommand;
		else if (pPositionalCommand)
		    iHostName = pPositionalCommand ;
		pPositionalCommand = iCommandCursor.nextToken ();

	    //  PortNumber
		Vca::U32 cPortNumber = 512;
		if (pCommand = commandStringValue ("portNumber", "PortNumber"))
		    cPortNumber = atoi (pCommand);
		else if (pPositionalCommand)
		    cPortNumber = atoi (pPositionalCommand);
		pPositionalCommand = iCommandCursor.nextToken ();

	    //  UserName
		VString iUserName;
		if (pCommand = commandStringValue ("userName", "UserName"))
		    iUserName = pCommand;
		else if (pPositionalCommand)
		    iUserName = pPositionalCommand ;
		pPositionalCommand = iCommandCursor.nextToken ();

	    //  Password
		VString iPassword;
		if (pCommand = commandStringValue ("password", "Password"))
		    iPassword = pCommand;
		else if (pPositionalCommand)
		    iPassword = pPositionalCommand ;
		pPositionalCommand = iCommandCursor.nextToken ();

	    //  set the PipeSource
		Vca::VRexecConnectionFactory::PipeSource::Reference pRexecConnectionSource;
		if (m_bPlumbingWorkers)
		    iWorkerSource << " -- -plumbed";
		Vca::VRexecConnectionFactory::Supply (
		    pRexecConnectionSource,
		    iHostName, iUserName, iPassword, iWorkerSource,
		    commandSwitchValue ("rexecErrorChannel"), Vca::Default::TcpNoDelaySetting (),
		    cPortNumber
		);
		pRexecConnectionSource->getRole (pPipeSource);
	    }

	//  Min Workers
	    Vca::U32 cMinWorkers = 1;
	    if (pCommand = commandStringValue ("minWorkers", "MinWorkers"))
		cMinWorkers = atoi (pCommand);
	    else if (pPositionalCommand)
		cMinWorkers = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Min Workers Available
	    Vca::U32 cMinWorkersAvailable = cMinWorkers-1;
	    if (pCommand = commandStringValue ("minWorkersAvailable", "MinWorkersAvailable"))
		cMinWorkersAvailable = atoi (pCommand);
	    else if (pPositionalCommand)
		cMinWorkersAvailable = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Max Workers Available
	    Vca::U32 cMaxWorkersAvailable = cMinWorkers;
	    if (pCommand = commandStringValue ("maxWorkersAvailable", "MaxWorkersAvailable"))
		cMaxWorkersAvailable = atoi (pCommand);
	    else if (pPositionalCommand)
		cMaxWorkersAvailable = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Worker Creation Failure Hard Limit
	    Vca::U32 cWorkerCreationFailureHardLimit = 5; // default
	    if (pCommand = commandStringValue ("workerSpawnErrorLmt", "WorkerSpawnErrorLmt"))
		cWorkerCreationFailureHardLimit = atoi (pCommand);
	    else if (pCommand = commandStringValue ("workerSpawnHardErrorLmt", "WorkrSpawnHardErrLt"))
                cWorkerCreationFailureHardLimit = atoi (pCommand);
            else if (pPositionalCommand)
                cWorkerCreationFailureHardLimit = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Worker Creation Failure Soft Limit
	    Vca::U32 cWorkerCreationFailureSoftLimit = UINT_MAX; // default
	    if (pCommand = commandStringValue ("workerSpawnSoftErrorLmt", "WorkrSpawnSoftErrLt"))
                cWorkerCreationFailureSoftLimit = atoi (pCommand);
            else if (pPositionalCommand)
                cWorkerCreationFailureSoftLimit = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();


        //  Evaluation Timeout
            Vca::U64 cEvaluationTimeoutSecs = U64_MAX; // default
	    if (pCommand = commandStringValue ("evaluationTimeout", "EvaluationTimeout"))
                cEvaluationTimeoutSecs = atoll (pCommand);
            else if (pPositionalCommand)
                cEvaluationTimeoutSecs = atoll (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();
            
            //  overf flow check...
            Vca::U64 cEvaluationTimeoutMicroSecs;
            if (cEvaluationTimeoutSecs > U64_MAX/1000000) 
              cEvaluationTimeoutMicroSecs = U64_MAX;
            else
              cEvaluationTimeoutMicroSecs = cEvaluationTimeoutSecs * 1000000;


	//  Evaluation Attempts is set to Maxworkers + 1 to try all existing
	//  workers and a new worker as a last attempt
	    Vca::U32 cEvaluationAttempts = cMaxWorkers+1; // default
	    if (pCommand = commandStringValue ("evaluationAttempts", "EvaluationAttempts"))
		cEvaluationAttempts = atoi (pCommand);
	    else if (pPositionalCommand)
		cEvaluationAttempts = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Broadcast InProg Limit
	    Vca::U32 cBroadcastInProgLmt = 1; // default
	    if (pCommand = commandStringValue ("bcastInProgLmt", "BcastInProgLmt"))
		cBroadcastInProgLmt = atoi (pCommand);
	    else if (pPositionalCommand)
		cBroadcastInProgLmt = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Stop TimeOut
	    Vca::U32 cStopTimeOut = 0;
	    if (pCommand = commandStringValue ("stopTimeOut", "StopTimeOut"))
		cStopTimeOut = atoi (pCommand);
	    else if (pPositionalCommand)
		cStopTimeOut = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Shrink TimeOut
	    Vca::U32 cShrinkTimeOut = UINT_MAX;
	    if (pCommand = commandStringValue ("shrinkTimeOut", "ShrinkTimeOut"))
		cShrinkTimeOut = atoi (pCommand);
	    else if (pPositionalCommand)
		cShrinkTimeOut = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

        //  Max Generations
	    Vca::U32 cMaxGenerations = VEvaluatorPool::PoolSettings::g_iDefaultGenerationMaximum;
	    if (pCommand = commandStringValue ("maxGenerations", "MaxGenerations"))
		cMaxGenerations = atoi (pCommand);
	    else if (pPositionalCommand)
		cMaxGenerations = atoi (pPositionalCommand);
	    pPositionalCommand = iCommandCursor.nextToken ();

	//  Worker PID Query [String|File]
	    VString iPIDQuery;
	    if (pCommand = commandStringValue ("workerPIDQueryStr", "WorkerPIDQueryStr"))
		iPIDQuery = pCommand;
	    else if (pPositionalCommand)
		iPIDQuery = pPositionalCommand;
	    pPositionalCommand = iCommandCursor.nextToken ();

	    VString iPIDQueryFile;
	    if (pCommand = commandStringValue ("workerPIDQueryFile", "WorkerPIDQueryFile"))
		iPIDQueryFile = pCommand;
	    else if (pPositionalCommand)
		iPIDQueryFile = pPositionalCommand;
	    pPositionalCommand = iCommandCursor.nextToken ();


            if (iPIDQuery.isEmpty () && iPIDQueryFile.isntEmpty ()) {
              VSimpleFile iFile;
              iFile.GetContents (iPIDQuery, iPIDQueryFile);
            }

	//  Create an evaluator source, ...
	    IEvaluatorSource::Reference pIEvaluatorSource; 
	    IEvaluatorSourceControl::Reference pIEvaluatorSourceControl; 
	    VString iStartupQuery; {
		VEvaluatorSource::Reference pEvaluatorSource; {
		//  Create settings for the pumps, ...
		    VEvaluatorPump::PumpSettings::Reference pPumpSettings (
			new VEvaluatorPump::PumpSettings ()
		    );
		    if (pCommand = commandStringValue ("urlClass", "UrlClass"))
			pPumpSettings->setURLClassTo (pCommand);
    		    else if (pPositionalCommand)
			pPumpSettings->setURLClassTo (pPositionalCommand);
		    pPositionalCommand = iCommandCursor.nextToken ();

    
		    if (pCommand = commandStringValue ("urlTemplate", "UrlTemplate"))
			pPumpSettings->setURLTemplateTo (pCommand);
    		    else if (pPositionalCommand)
			pPumpSettings->setURLTemplateTo (pPositionalCommand);
		    pPositionalCommand = iCommandCursor.nextToken ();

		//  Create a pump source, ...
		    VEvaluatorPumpSource::Reference pPumpSource (
			new VEvaluatorPumpSource (pPumpSettings, pPipeSource, m_bPlumbingWorkers)
		    );
		    pEvaluatorSource.setTo (pPumpSource);
		}

		//  Complete the evaluator source initialization, ...
		if (pEvaluatorSource) {
		    pEvaluatorSource->getRole (pIEvaluatorSource);
		    pEvaluatorSource->getRole (pIEvaluatorSourceControl);

		    //  Worker Startup Query
		    if (pCommand = commandStringValue ("workerStartupQuery", "WorkerStartupQuery"))
			iStartupQuery = pCommand;
		    else if (pPositionalCommand)
			iStartupQuery = pPositionalCommand;
		    pPositionalCommand = iCommandCursor.nextToken ();

		    //  Worker StartupQuery From File
		    VString iStartupQueryFile;
		    if (pCommand = commandStringValue ("workerStartupFromFile", "WStartupFromFile"))
			iStartupQueryFile = pCommand;
		    else if (pPositionalCommand)
			iStartupQueryFile = pPositionalCommand;

		    if (iStartupQuery.isEmpty () && iStartupQueryFile.isntEmpty ()) {
			VSimpleFile iFile;
			iFile.GetContents (iStartupQuery, iStartupQueryFile);
		    }
		    if (iStartupQuery.isntEmpty ())
			pEvaluatorSource->setStartupQueryTo (iStartupQuery);
		}
	    }


	//  Create settings for the pool, ...
	    VEvaluatorPool::PoolSettings::Reference pPoolSettings (
		new VEvaluatorPool::PoolSettings ()
	    );

	//  Set the Pool Settings
	    pPoolSettings->setWorkerMaximumTo (cMaxWorkers);
	    pPoolSettings->setWorkersBeingCreatedMaximumTo (cWorkersBeingCreated);
	    pPoolSettings->setWorkerMinimumTo (cMinWorkers);
	    pPoolSettings->setEvaluationAttemptMaximumTo (cEvaluationAttempts);
	    pPoolSettings->setWorkerMinimumAvailableTo (cMinWorkersAvailable);
	    pPoolSettings->setWorkerMaximumAvailableTo (cMaxWorkersAvailable);
	    pPoolSettings->setWorkerCreationFailureHardLimitTo (cWorkerCreationFailureHardLimit);
	    pPoolSettings->setWorkerCreationFailureSoftLimitTo (cWorkerCreationFailureSoftLimit);
	    pPoolSettings->setWorkerSourceTo (pIEvaluatorSource);
	    pPoolSettings->setWorkerSourceControlTo (pIEvaluatorSourceControl);
	    pPoolSettings->setWorkerStartupQueryTo (iStartupQuery);
	    pPoolSettings->setBCEvaluationsInProgressLimit (cBroadcastInProgLmt);
	    pPoolSettings->setStopTimeOutTo (cStopTimeOut);
            pPoolSettings->setShrinkTimeOutTo (cShrinkTimeOut);
            pPoolSettings->setGenerationMaximumTo (cMaxGenerations);
	    pPoolSettings->setWorkerPIDQueryString (iPIDQuery);
	    pPoolSettings->setEvaluationTimeOutTo (cEvaluationTimeoutMicroSecs);

	//  Create and the pool, ...
	    m_pEvaluatorPool.setTo (new VEvaluatorPool (pPoolSettings));
	}
    }

    if (m_pEvaluatorPool) {
	offerSelf (pListenerAddress);
	offerSelf (true);	//  an address offer was made, tell 'offerSelf' so it doesn't set stop on idle
    } else {
	displayUsagePatterns (
	    "<listener> cpool <value-options> <switch-options> <trace-options>",
	    "<listener> epool <value-options> <switch-options> <trace-options>",
	    "<listener> rpool <value-options> <switch-options> <trace-options>",
            static_cast<char const*>(0)
	);

	displayOptions (
	    " Value Options:",
	    "     <MaxWorkers>                   | -maxWorkers=<workerCount>",
	    "     <WorkersBeingCreatedMaximum>   | -workersBeingCreatedMaximum=<workerCount>",
	    "     <WorkerSource>                 | -workerSource = <command>",
	    "     <MinWorkers>                   | -minWorkers =<workerCount>",
	    "     <MinWorkersAvailable>          | -minWorkersAvailable=<workerCount>",
	    "     <MaxWorkersAvailable>          | -maxWorkersAvailable=<workerCount>",
	    "     <WorkrSpawnHardErrLmt>         | -workerSpawnHardErrorLmt=<count>",
	    "     <WorkrSpawnSoftErrLmt>         | -workerSpawnSoftErrorLmt=<count>",
            "     <EvaluationTimeout>            | -evaluationTimeout=<timeout_secs>",
	    "     <EvaluationAttempts>           | -evaluationAttempts=<cAttempts>",
	    "     <BcastInProgLmt>               | -bcastInProgLmt=<cLmt>",
	    "     <StopTimeOut>                  | -stopTimeOut=<cTimeOut>",
	    "     <UrlClass>                     | -urlClass=<urlClass>",
	    "     <UrlTemplate>                  | -urlTemplate=<urlTemplate>",
	    "     <WorkerStartup[Query|FromFile]>| -workerStartup[Query|FromFile]=<query|file>",
            "     <WorkerPIDQuery[Str|File]>     | -workerPIDQuery[Str|File]",
	    "                                    | -logFilePath=<logFilePath>",
            "                                    | -logFileSize=<logFileSize>",
            "                                    | -logFileBackups=<logFileBackups>",
            "                                    | -serverFile=<serverFile>",
            static_cast<char const*>(0)
	);
	displayOptions (
	    " Switch Options:",
	    "     -plumbWorkers",
	    "     -logSwitch[On/Off]",
	    static_cast<char const*>(0)
	);
	displayStandardOptions ();
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}
/****
 * Note: Graceful Stop
 *       Stops the application/pool only if there are no active offers 
 *       (connections) and the stop timeout is zero, else it registers trigger for zero offer 
 *       call back.
 *       If there are active connections, pool is unaware of the graceful stop.
 *       When all connections drop, the pool application automatically triggers a hardstop 
 *       on the pool.
 ****/

bool Vsa::VPoolApplication::stop_(bool bHardStop) {
    if (BaseClass::stop_(bHardStop) && bHardStop) {
/****
 * Note: PoolApplication's hardstop () call does the following
 *       1. Immediately stops the listener to prevent incoming connections
 *       2. Calls the pools hardstop () passing in a call back pointer when the 
 *          pool WIPs are not active and pool is stopped
 *       3. On callback, Vca::Stop () is called to terminate existing connections
 ****/
//	if (m_pListener)
//	    m_pListener->stop ();

	Vca::VTrigger<VPoolApplication>::Reference pTrigger;
	pTrigger.setTo (new Vca::VTrigger<ThisClass> (this, &ThisClass::onPoolStopped));

	if (m_pEvaluatorPool) 
	    m_pEvaluatorPool->hardstop (pTrigger);

	m_pEvaluatorPool.clear ();	// cleared to cause future Pool Interface methods to fail.
    }
    return isStopping (bHardStop);
}

bool Vsa::VPoolApplication::pause_() {
    if (BaseClass::pause_() && m_pEvaluatorPool)
	m_pEvaluatorPool->suspend ();
    return isPausing ();
}

bool Vsa::VPoolApplication::resume_() {
    if (BaseClass::resume_() && m_pEvaluatorPool)
	m_pEvaluatorPool->resume ();
    return isResuming ();
}

void Vsa::VPoolApplication::onPoolStopped (Vca::VTrigger<ThisClass> *pTrigger) {
    //  stop the application (including existing client connections)
    BaseClass::stop ();
    log ("Pool Application STOP - Completed.");
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vsa::VPoolApplication> iMain (argc, argv);
    return iMain.processEvents ();
}
