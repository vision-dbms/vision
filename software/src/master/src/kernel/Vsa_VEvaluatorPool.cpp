/*****  Vsa_VEvaluatorPool Implementation  *****/

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

#include "Vsa_VEvaluatorPool.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_CompilerHappyPill.h"
#include "VTransientServices.h"

#include "VSimpleFile.h"
#include "Vca_VTrigger.h"
#include "Vsa_VPoolWorkerGeneration.h"
#include "Vsa_VPoolBroadcast.h"
#include "Vsa_VPoolBroadcastEvaluation.h"


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vsa::VEvaluatorPool::PoolSettings  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::U32 Vsa::VEvaluatorPool::PoolSettings::g_iDefaultWorkersBeingCreatedMaximum = UINT_MAX;
Vca::U32 Vsa::VEvaluatorPool::PoolSettings::g_iDefaultWorkerMaximum = UINT_MAX;
Vca::U32 Vsa::VEvaluatorPool::PoolSettings::g_iDefaultGenerationMaximum = 100;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPool::PoolSettings::PoolSettings (PoolSettings const &rOther)
: BaseClass                     (rOther)
, m_pWorkerSource               (rOther.m_pWorkerSource)
, m_iWorkerMaximum              (rOther.m_iWorkerMaximum)
, m_iWorkerMinimum              (rOther.m_iWorkerMinimum)
, m_iWorkersBeingCreatedMaximum (rOther.m_iWorkersBeingCreatedMaximum)
, m_cEvaluationAttemptMaximum   (rOther.m_cEvaluationAttemptMaximum)
, m_cEvaluationOnErrorAttemptMaximum   (rOther.m_cEvaluationOnErrorAttemptMaximum)
, m_cEvaluationTimeOutAttemptMaximum   (rOther.m_cEvaluationTimeOutAttemptMaximum)
, m_iWorkerMinimumAvailable     (rOther.m_iWorkerMinimumAvailable)
, m_iWorkerMaximumAvailable     (rOther.m_iWorkerMaximumAvailable)
, m_iWorkerStartupQuery         (rOther.m_iWorkerStartupQuery)
, m_iWorkerQueryHistoryLimit    (rOther.m_iWorkerQueryHistoryLimit)
, m_iWorkerCreationFailureHardLimit (rOther.m_iWorkerCreationFailureHardLimit)
, m_iWorkerCreationFailureSoftLimit (rOther.m_iWorkerCreationFailureSoftLimit)
, m_iShrinkTimeOutMinutes       (rOther.m_iShrinkTimeOutMinutes)
, m_iStopTimeOutMins            (rOther.m_iStopTimeOutMins)
, m_iBCEvaluationsInProgressLimit (rOther.m_iBCEvaluationsInProgressLimit)
, m_iWorkerPIDQueryString (rOther.m_iWorkerPIDQueryString)
, m_iGenerationMaximum (rOther.m_iGenerationMaximum)
, m_iQueryLogLength (rOther.m_iQueryLogLength)
, m_iResultLogLength (rOther.m_iResultLogLength)
, m_pIObjectSource (this)
, m_pIEvaluatorPoolSettings (this)
{
    traceInfo ("Creating VEvaluatorPool::PoolSettings");
}

Vsa::VEvaluatorPool::PoolSettings::PoolSettings ()
: m_iWorkerMaximum              (100)
, m_iWorkerMinimum              (1)
, m_iWorkersBeingCreatedMaximum (g_iDefaultWorkersBeingCreatedMaximum)
, m_cEvaluationAttemptMaximum (0)
, m_cEvaluationOnErrorAttemptMaximum   (5)
, m_cEvaluationTimeOutAttemptMaximum   (0)
, m_iWorkerMinimumAvailable     (1)
, m_iWorkerMaximumAvailable     (1)
, m_iWorkerCreationFailureHardLimit (1)
, m_iWorkerCreationFailureSoftLimit (UINT_MAX)
, m_iShrinkTimeOutMinutes       (UINT_MAX)
, m_iWorkerQueryHistoryLimit    (5)
, m_iStopTimeOutMins            (0)
, m_iBCEvaluationsInProgressLimit (1)
, m_iGenerationMaximum          (g_iDefaultGenerationMaximum)
, m_iQueryLogLength (0)
, m_iResultLogLength (2048)
, m_pIObjectSource (this)
, m_pIEvaluatorPoolSettings (this)
{
    traceInfo ("Creating VEvaluatorPool::PoolSettings");

    // Get query and result log length from env.
    m_iQueryLogLength = V::GetEnvironmentUnsigned ("VsaQueryLogLength", m_iQueryLogLength);
    m_iResultLogLength = V::GetEnvironmentUnsigned ("VsaResultLogLength", m_iResultLogLength);
}


void Vsa::VEvaluatorPool::PoolSettings::AddOdometerSetting(OdometerSettings *settings) {
	int index = m_Odometers.cardinality();
	m_Odometers.Append(1);
	m_Odometers[index] = settings;
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*****************************************
 *****  Vsa::IEvaluatorPoolSettings  *****
 *****************************************/

void Vsa::VEvaluatorPool::PoolSettings::MakeControl (
    IEvaluatorPoolSettings *pRole, IVReceiver<IEvaluatorPool*> *pClient
) {
    if (pClient) {
        VReference<VEvaluatorPool> pControl (new VEvaluatorPool (this));
        VReference<IEvaluatorPool_Ex5> pIEvaluatorPool;
        pControl->getRole (pIEvaluatorPool);
        pClient->OnData (pIEvaluatorPool);
    }
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerSource (
    IEvaluatorPoolSettings *pRole, IVReceiver<IEvaluatorSource*> *pClient
) {
    if (pClient)
        pClient->OnData (m_pWorkerSource);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerSource (
    IEvaluatorPoolSettings *pRole, IEvaluatorSource *pWorkerSource
) {
    setWorkerSourceTo (pWorkerSource);
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkersBeingCreatedMaximum (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkersBeingCreatedMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkersBeingCreatedMaximum (
    IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    setWorkersBeingCreatedMaximumTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerMaximum (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerMaximum (
    IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    setWorkerMaximumTo (iValue);
}
void Vsa::VEvaluatorPool::PoolSettings::GetWorkerMinimum (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerMinimum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerMinimum (
    IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    setWorkerMinimumTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetEvaluationAttemptMaximum (
	IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_cEvaluationAttemptMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetEvaluationAttemptMaximum (
	IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
	setEvaluationAttemptMaximumTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerGenerationLimit (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnError (0, "GenerationLimit Parameter not supported in this Pool version");
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerGenerationLimit (
    IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    //  Operation Not Supported in this Pool version
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerMinimumAvailable (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerMinimumAvailable);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerMinimumAvailable (
    IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    setWorkerMinimumAvailableTo (iValue);
}
void Vsa::VEvaluatorPool::PoolSettings::GetWorkerStartupQuery (
    IEvaluatorSettings *pRole, IVReceiver<VString const &>* pClient
) {
    if (pClient) {
        VString query;
        workerStartupQuery (query);
        pClient->OnData (query);
    }
}
void Vsa::VEvaluatorPool::PoolSettings::SetWorkerStartupQuery (
    IEvaluatorSettings *pRole, VString const &rQuery
) {
    setWorkerStartupQueryTo (rQuery);
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerCreationFailureLimit (
    IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerCreationFailureHardLimit);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerCreationFailureLimit (
     IEvaluatorPoolSettings *pRole, Vca::U32 iValue
) {
    setWorkerCreationFailureHardLimitTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetDebugFilePathName (
    IEvaluatorPoolSettings *pRole, IVReceiver<VString const &> *pClient
) {
    //  support for older vpooladmin
    if (pClient) {
        VString iPath;
        transientServicesProvider ()->logFilePath (iPath);
        pClient->OnData (iPath);
    }
}

void Vsa::VEvaluatorPool::PoolSettings::SetDebugFilePathName (
     IEvaluatorPoolSettings *pRole, VString const &iValue
) {
     //  support for older vpooladmin
     transientServicesProvider ()->updateLogFilePath (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetDebugSwitch (
    IEvaluatorPoolSettings *pRole, IVReceiver<bool> *pClient
) {
    //  support for older vpooladmin
    if (pClient) {
        pClient->OnData (transientServicesProvider ()->logSwitch ());
    }
}

void Vsa::VEvaluatorPool::PoolSettings::SetDebugSwitch (
     IEvaluatorPoolSettings *pRole, bool iValue
 ) {
     //  support for older vpooladmin
     transientServicesProvider ()->updateLogSwitch (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex1  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerMaximumAvailable (
    IEvaluatorPoolSettings_Ex1 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerMaximumAvailable);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerMaximumAvailable (
    IEvaluatorPoolSettings_Ex1 *pRole, Vca::U32 iValue
) {
    setWorkerMaximumAvailableTo (iValue);
}
void Vsa::VEvaluatorPool::PoolSettings::GetShrinkTimeOut (
    IEvaluatorPoolSettings_Ex1 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iShrinkTimeOutMinutes);
}
void Vsa::VEvaluatorPool::PoolSettings::SetShrinkTimeOut (
     IEvaluatorPoolSettings_Ex1 *pRole, Vca::U32 iValue
) {
    setShrinkTimeOutTo (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex2  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetBCEvaluationsInProgressLimit (
    IEvaluatorPoolSettings_Ex2 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iBCEvaluationsInProgressLimit);
}

void Vsa::VEvaluatorPool::PoolSettings::SetBCEvaluationsInProgressLimit (
    IEvaluatorPoolSettings_Ex2 *pRole, Vca::U32 iValue
) {
    setBCEvaluationsInProgressLimit (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex3  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetStopTimeOut (
    IEvaluatorPoolSettings_Ex3 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iStopTimeOutMins);
}

void Vsa::VEvaluatorPool::PoolSettings::SetStopTimeOut (
    IEvaluatorPoolSettings_Ex3 *pRole, Vca::U32 iValue
) {
    setStopTimeOutTo (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex4  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerQueryHistoryLimit (
    IEvaluatorPoolSettings_Ex4 *pRole, IVReceiver<Vca::U32> *pClient
    ) {
    if (pClient)
        pClient->OnData (m_iWorkerQueryHistoryLimit);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerQueryHistoryLimit (
    IEvaluatorPoolSettings_Ex4 *pRole, Vca::U32 iValue
) {
    setWorkerQueryHistoryLimitTo (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex5  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerCreationFailureSoftLimit (
     IEvaluatorPoolSettings_Ex5 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iWorkerCreationFailureSoftLimit);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerCreationFailureSoftLimit (
     IEvaluatorPoolSettings_Ex5 *pRole, Vca::U32 iValue) {
    setWorkerCreationFailureSoftLimitTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetWorkerPIDQuery (
    IEvaluatorPoolSettings_Ex5 *pRole, IVReceiver<VString const &> *pClient
    ) {
    if (pClient)
        pClient->OnData (m_iWorkerPIDQueryString);
}

void Vsa::VEvaluatorPool::PoolSettings::SetWorkerPIDQuery (
    IEvaluatorPoolSettings_Ex5 *pRole, VString const &iValue
    ) {
    setWorkerPIDQueryString (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex6  *****
 *********************************************/

void Vsa::VEvaluatorPool::PoolSettings::GetGenerationMaximum (
    IEvaluatorPoolSettings_Ex6 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iGenerationMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetGenerationMaximum (
    IEvaluatorPoolSettings_Ex6 *pRole, Vca::U32 iValue) {
    setGenerationMaximumTo (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex7  *****
 *********************************************/
 
void Vsa::VEvaluatorPool::PoolSettings::GetQueryLogLength (
    IEvaluatorPoolSettings_Ex7 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iQueryLogLength);
}

void Vsa::VEvaluatorPool::PoolSettings::SetQueryLogLength (
    IEvaluatorPoolSettings_Ex7 *pRole, Vca::U32 iValue) {
    setQueryLogLengthTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::GetResultLogLength (
    IEvaluatorPoolSettings_Ex7 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_iResultLogLength);
}

void Vsa::VEvaluatorPool::PoolSettings::SetResultLogLength (
    IEvaluatorPoolSettings_Ex7 *pRole, Vca::U32 iValue) {
    setResultLogLengthTo (iValue);
}

/*********************************************
 *****  Vsa::IEvaluatorPoolSettings_Ex8  *****
 *********************************************/
 
void Vsa::VEvaluatorPool::PoolSettings::getRole (VReference<IEvaluatorPoolSettings_Ex8>&rpRole) {
    m_pIEvaluatorPoolSettings.getRole (rpRole);
}

void Vsa::VEvaluatorPool::PoolSettings::GetEvaluationOnErrorAttemptMaximum (
    IEvaluatorPoolSettings_Ex8 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_cEvaluationOnErrorAttemptMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::GetEvaluationTimeOutAttemptMaximum (
    IEvaluatorPoolSettings_Ex8 *pRole, IVReceiver<Vca::U32> *pClient
) {
    if (pClient)
        pClient->OnData (m_cEvaluationTimeOutAttemptMaximum);
}

void Vsa::VEvaluatorPool::PoolSettings::SetEvaluationOnErrorAttemptMaximum (
    IEvaluatorPoolSettings_Ex8 *pRole, Vca::U32 iValue
) {
    setEvaluationOnErrorAttemptMaximumTo (iValue);
}

void Vsa::VEvaluatorPool::PoolSettings::SetEvaluationTimeOutAttemptMaximum (
    IEvaluatorPoolSettings_Ex8 *pRole, Vca::U32 iValue
) {
    setEvaluationTimeOutAttemptMaximumTo (iValue);
}

VkDynamicArrayOf<Vsa::VEvaluatorPool::OdometerSettings::Reference> Vsa::VEvaluatorPool::PoolSettings::GetOdometerSettings () const {
	return m_Odometers;
}

/***********************************
 ***********************************
 *****  IObjectSource Methods  *****
 ***********************************
 ***********************************/

 /****
  * Note: Declaring the following Supply method in .h file and defining it in .cpp results in an undefined
  * Supply symbol problem when linking against Vsa library.
  * elfdump and nm shows that libVsa.so.1 has 2 entries for Vsa::VEvaluatorPool::settings::Supply, each having differant mangled names (??)
  * To fix it, we currently define the weak symbol which appears to be undefined with the defined symbol name
  ****/


#if defined (__sparc)
#pragma weak "__1cDVsaOVEvaluatorPoolMPoolSettingsGSupply6MpnDVcaLIDataSource4CpnJIVUnknown___pnKIVReceiver4C5___v_" = "__1cDVsaOVEvaluatorPoolMPoolSettingsGSupply6MpnDVcaLIDataSource4CpnJIVUnknown___pnKIVReceiver4Cp4___v_"
#endif

void Vsa::VEvaluatorPool::PoolSettings::Supply (Vca::IObjectSource *pRole, Vca::IObjectSink *pClient) {
  if (pClient) {
    VReference<VEvaluatorPool> pPool (new VEvaluatorPool (this));

    VReference<IEvaluator_Ex1> pIEvaluator;
    pPool->getRole (pIEvaluator);
    pClient->OnData (pIEvaluator);
  }
}

/************************
 *****  Statistics  *****
 ************************/

void Vsa::VEvaluatorPool::PoolSettings::getStatistics (VString &rResult) const {
    VString iQuery;
    if (m_iWorkerStartupQuery.lineCount ()<=2)
        iQuery.setTo (m_iWorkerStartupQuery);
    else {
        VString iFirstLine, iRemQuery;
        char const *pDelimiter = "\n";
        m_iWorkerStartupQuery.getPrefix (pDelimiter, iFirstLine, iRemQuery);
        iQuery << iFirstLine << pDelimiter;
        iRemQuery.getPrefix (pDelimiter, iFirstLine, iRemQuery);
        iQuery << iFirstLine << ".....";
    }

    VString iEvalTimeoutSecs;
    if (m_cEvaluationTimeOut == U64_MAX)
      iEvalTimeoutSecs.setTo ("NIL");
    else
      iEvalTimeoutSecs << evaluationTimeOutInSecs ();

    rResult << "\nWorker Maximum Lmt              : " << m_iWorkerMaximum;
    rResult << "\nWorker Minimum Lmt              : " << m_iWorkerMinimum;
    rResult << "\nWorker Minimum Available Lmt    : " << m_iWorkerMinimumAvailable;
    rResult << "\nWorker Maximum Available Lmt    : " << m_iWorkerMaximumAvailable;
    rResult << "\nWorkers In Creation Lmt         : " << m_iWorkersBeingCreatedMaximum;
    rResult << "\nWorkerCreation Failure HARD Lt  : " << m_iWorkerCreationFailureHardLimit;
    rResult << "\nWorkerCreation Failure SOFT Lt  : " << m_iWorkerCreationFailureSoftLimit;
    rResult << "\nWorker Query History Lmt        : " << m_iWorkerQueryHistoryLimit;
    rResult << "\nEvaluation Timeout (secs)       : " << iEvalTimeoutSecs;
	rResult << "\nEvaluation Attempt Lmt          : " << m_cEvaluationAttemptMaximum;
    rResult << "\nEvaluation On Error Attempt Lmt : " << m_cEvaluationOnErrorAttemptMaximum;
    rResult << "\nEvaluation Time Out Attempt Lmt : " << m_cEvaluationTimeOutAttemptMaximum;
    rResult << "\nShrink Timeout (mins)           : " << m_iShrinkTimeOutMinutes;
    rResult << "\nStop Timeout (Minutes)          : " << m_iStopTimeOutMins;
    rResult << "\nBroadcasts InProgress Lmt       : " << m_iBCEvaluationsInProgressLimit;
    rResult << "\nWorker Startup Query            : " << iQuery;
    rResult << "\nWorker PID Query                : " << m_iWorkerPIDQueryString;
    rResult << "\nGeneration Maximum              : " << m_iGenerationMaximum;
    rResult << "\nQuery Log Length                : " << m_iQueryLogLength;
    rResult << "\nResult Log Length               : " << m_iResultLogLength;
}

void Vsa::VEvaluatorPool::PoolSettings::dumpSettings (VString &rResult) const {
    BaseClass::dumpSettings (rResult);

    VString iPIDQuery;
    workerPIDQuery (iPIDQuery);

    VString iWorkerStartupQueryShortened;
    m_iWorkerStartupQuery.getSummary (iWorkerStartupQueryShortened, queryLogLength () ? queryLogLength () : 50); // 50 chars if log length not set.

    rResult << "WorkerMaximum | "                   << workerMaximum () << "\n";
    rResult << "WorkerMinimum | "                   << workerMinimum () << "\n";
    rResult << "WorkerMinimumAvailable | "          << workerMinimumAvailable () << "\n";
    rResult << "WorkerMaximumAvailable | "          << workerMaximumAvailable () << "\n";
    rResult << "WorkersBeingCreatedMaximum | "      << workersBeingCreatedMaximum () << "\n";
    rResult << "WorkerCreationFailureHardLimit | "  << workerCreationFailureHardLimit () << "\n";
    rResult << "WorkerCreationFailureSoftLimit | "  << workerCreationFailureSoftLimit () << "\n";
    rResult << "WorkerQueryHistoryLimit | "         << workerQueryHistoryLimit () << "\n";
    rResult << "EvaluationAttemptMaximum | "        << evaluationAttemptMaximum () << "\n";	
    rResult << "EvaluationOnErrorAttemptMaximum | "        << evaluationOnErrorAttemptMaximum () << "\n";
    rResult << "EvaluationTimeOutAttemptMaximum | "        << evaluationTimeOutAttemptMaximum () << "\n";
    rResult << "ShrinkTimeOut | "                   << shrinkTimeOut () << "\n";
    rResult << "StopTimeOut | "                     << getStopTimeOut () << "\n";
    rResult << "BCEvaluationsInProgressLimit | "    << BCEvaluationsInProgressLimit () << "\n";
    rResult << "WorkerStartUpQuery | "              << iWorkerStartupQueryShortened << "\n";
    rResult << "WorkerPIDQuery | "                  << iPIDQuery << "\n";
    rResult << "GenerationMaximum | "               << generationMaximum () << "\n";
    rResult << "Query Log Length | "                << queryLogLength () << "\n";
    rResult << "Result Log Length | "               << resultLogLength ();
}



/*********************************
 *********************************
 *****                       *****
 *****  Vsa::VEvaluatorPool  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPool::VEvaluatorPool (PoolSettings *pSettings)
: m_cOnlineWorkers          (0)
, m_cOnlineAvailWorkers     (0)
, m_cOnlineWIPs             (0)
, m_cWorkersBeingCreated    (0)
, m_cOnlineExcessWorkers    (0)
, m_cWorkersRetired         (0)
, m_cOfflineWorkers         (0)
, m_cOfflineAvailWorkers    (0)
, m_cOfflineWIPs            (0)
, m_cOfflineExcessWorkers   (0)
, m_iLastToBeFlushedWorker  (0)
, m_cToBeFlushedWorkers     (0)
, m_cTempWorkerMinimum      (pSettings->workerMinimum ())
, m_cShrinkTimerExpirations (0)
, m_bExplicitlySuspended    (false)
, m_pSettings               (pSettings)
, m_pGenerationTail         (0)
, m_cBCEvaluationsInProgress (0)
, m_pIEvaluatorPool         (this)
{
    traceInfo ("Creating VEvaluatorPool");
    log ("POOL STARTED");
	setupOdometers();
    VReference<WorkerGeneration> pGeneration (new WorkerGeneration (this));
    hire ();
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorPool::~VEvaluatorPool () {
    traceInfo ("Destroying VEvaluatorPool");
    log ("POOL STOPPED\n>>>>>>>>>><<<<<<<<<<");
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

Vca::U32 Vsa::VEvaluatorPool::currentGenerationId () const {
    return m_pGenerationHead->generationId ();
}

Vca::U32 Vsa::VEvaluatorPool::oldestGenerationId () const {
    return m_pGenerationTail->generationId ();
}

bool Vsa::VEvaluatorPool::hasActiveWIPs () const {
  bool bActiveWIPs = false;
  VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
  while (pGeneration) {
    bActiveWIPs = pGeneration->hasActiveWIPs ();
    if (bActiveWIPs)
      break;
    pGeneration.setTo (pGeneration->m_pSuccessor);
  }
  return bActiveWIPs;
}

bool Vsa::VEvaluatorPool::hasStartedWorkers () const {
  VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
  while (pGeneration) {
      if  (pGeneration->hasStartedWorkers ())
	  return true;
    pGeneration.setTo (pGeneration->m_pSuccessor);
  }
  return false;
}

/*************************
 *************************
 *****  Evaluation   *****
 *************************
 *************************/

Vsa::VEvaluation* Vsa::VEvaluatorPool::createEvaluation (
    IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
) {
    return new VPoolEvaluation (this, pClient, rPath, rExpression, currentGenerationId ());
}

Vsa::VEvaluation* Vsa::VEvaluatorPool::createEvaluation (
    IEvaluatorClient* pClient, VString const& rPath, VString const& rQuery, VString const& rEnvironment
) {
    return new VPoolEvaluation (this, pClient, rPath, rQuery, rEnvironment, currentGenerationId ());
}

/**************************
 *****  Pool Control  *****
 **************************/

void Vsa::VEvaluatorPool::restart () {
    traceInfo ("VEvaluatorPool::restart");
    log ("RESTARTING POOL");

    BaseClass::restart ();

    // Create new generation.
    WorkerGeneration::Reference prevGeneration = m_pGenerationHead;
    Vca::U32 iNextWorker     = m_pGenerationHead->nextWorker ();
    Vca::U32 iNextGeneration = m_pGenerationHead->generationId () + 1;
    VReference<WorkerGeneration> pGeneration (
        new WorkerGeneration (this, iNextGeneration, iNextWorker)
    );
    if (pGeneration.isNil ()) {
	notify (15, "#15: VEvaluatorPool::restart: unable to obtain new generation for pool restart\n");		
		log ("ERROR RESTARTING POOL (unable to obtain new generation).");
		return;
    }

    // Retire any started workers that aren't employed yet in the old generation.
    prevGeneration->retireStartingWorkers ();

    // Re-hire as many workers as we have right away.
    setTempWorkerMinimumTo(m_cOnlineWorkers);
    hire ();
}

void Vsa::VEvaluatorPool::hardRestart () {
    traceInfo ("VEvaluatorPool::hardRestart");
    log ("HARD RESTARTING POOL");

	if (m_iStatus != Status_Suspended) 
		suspend (false);

	Vca::VTrigger<VEvaluatorPool> *pOnFlushTrigger;
	pOnFlushTrigger = new Vca::VTrigger<VEvaluatorPool> (this, &VEvaluatorPool::resumeHardRestart);
	flushWorkers (pOnFlushTrigger);
}


void Vsa::VEvaluatorPool::resumeHardRestart (Vca::VTrigger<ThisClass> *pTrigger) {
    m_pOnFlushTrigger.clear ();
    restart ();

    if (!m_bExplicitlySuspended)
		resume (false);
}

void Vsa::VEvaluatorPool::triggerOnFlush () {
    if (m_pOnFlushTrigger.isntNil ())
        m_pOnFlushTrigger->Process ();
}


/********
 * Note: stop routine sets the status to Status_Stopping. If there are
 *       no pending queries then does a hard stop....Else it waits and
 *       does a hard stop after all queries are processed.
 ****/

void Vsa::VEvaluatorPool::stop () {
    traceInfo ("VEvaluatorPool::Stop");
    log ("STOPPING POOL");

    BaseClass::stop ();

    //  if nothing is pending...
    if (queueLength () == 0)
        hardstop ();
}

void Vsa::VEvaluatorPool::flushWorkers (
    Vca::VTrigger<VEvaluatorPool> *pOnFlushTrigger
) {
    traceInfo ("VEvaluatorPool::flushWorkers");
    log ("Flushing Pool Workers");

    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    while (pGeneration.isntNil ()) {
        pGeneration->retire ();
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }

    m_iLastToBeFlushedWorker = (m_pGenerationHead->m_iNextWorker)-1;
    m_cToBeFlushedWorkers = onlineWIPs () + workersBeingCreated ();

    if (m_cToBeFlushedWorkers == 0 && pOnFlushTrigger)
        pOnFlushTrigger->Process ();
    else
        m_pOnFlushTrigger.setTo (pOnFlushTrigger);
}

void Vsa::VEvaluatorPool::suspend () {
    log ("EXPLICITLY SUSPENDING POOL");
    suspend(true);
}
void Vsa::VEvaluatorPool::suspend (bool explicitly) {
    traceInfo ("VEvaluatorPool::suspend");
    log ("SUSPENDING POOL");

    if (explicitly)
        m_bExplicitlySuspended = true;

    BaseClass::suspend ();
}

void Vsa::VEvaluatorPool::resume () {
    log ("EXPLICITLY RESUMING POOL");
    resume(true);
}
void Vsa::VEvaluatorPool::resume (bool explicitly) {
    traceInfo ("VEvaluatorPool::resume");
    log ("RESUMING POOL");

    if (explicitly)
        m_bExplicitlySuspended = false;

    BaseClass::resume ();
    onQueue ();
}

/********
 * Stops the pool immediately, emitting errors to queued clients.
 *
 * Specifically, does the following in order:
 *  - Calls VEvaluator::hardstop (), which sends OnDown message to all subscribers
 *  - Stops the shrink timer if there is one.
 *  - Retires all free workers in all genreations
 *  - Sends error messages to all pending queries in the pools queue
 *  - Sets retry limit to 0 in order to prevent further retries of running queries
 *  - Checks if pool has active WorkInProgress still, if so store the timer for later use, else trigger immediately
 *  - Detaches all generation objects from pool
 ****/

void Vsa::VEvaluatorPool::hardstop (Vca::ITrigger *pTrigger) {
    traceInfo ("VEvaluatorPool::HardStop");
    log ("HARD STOPPING POOL");
    notify (2, "#2: VEvaluatorPool::hardstop: pool hard stopped\n");

    BaseClass::hardstop ();

    // Destroy our timer explicitly, since otherwise we'll have to wait until it fires, which could also cause a segfault.
    stopShrinkTimer ();

    //  retire all free workers in each generation...
    VReference<WorkerGeneration> pGeneration (m_pGenerationTail);
    while (pGeneration) {
        bool bRetireOnlineOnly = false;
        pGeneration->retire (bRetireOnlineOnly);
        pGeneration.setTo (pGeneration->m_pPredecessor);
    }

    //  send error message to pending queries in queue....
    if (m_pThisRequest.isntNil ()) {
        m_pThisRequest->onError (0, "Pool Stopped. Evaluation not processed");
        m_pThisRequest.clear ();
    }
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd ()) {
        VEvaluation *pEvaluation = (*iterator)->request ();
	notify (2, "#2: VEvaluatorPool::hardstop: unprocessed query string - %s\n", pEvaluation->queryString ().content ());		
        pEvaluation->onError (0, "Pool Stopped. Evaluation not processed");
        //  remove this entry
        iterator.Delete ();
    }
	
    //  disable future retries of timedout queries....
	setEvaluationAttemptMaxTo (0);
	
    //  disable future retries of error queries....
    setEvaluationOnErrorAttemptMaxTo (0);

    //  disable future retries of timedout queries....
    setEvaluationTimeOutAttemptMaxTo (0);


    //  if there is active WIPs wait for its completion...
    if (hasActiveWIPs ()) {
      log ("Pool has ActiveWIPs (HardStop called). Registering Trigger...");
      if (pTrigger)
        m_pHardStopTrigger.setTo (pTrigger);
      else
        detachAllGenerations ();
    }
    else {
      detachAllGenerations ();
      if (pTrigger)
        pTrigger->Process ();
    }
}

void Vsa::VEvaluatorPool::add (IEvaluator *pEvaluator) {
    if (m_pGenerationHead.isntNil ())
        m_pGenerationHead->add (pEvaluator);
}

void Vsa::VEvaluatorPool::cancelQuery (Vca::U32 iID, BoolReceiver *pClient) {
    // Check if the query is the current one(between WIP and the queries in the queue).
    if (m_pThisRequest && m_pThisRequest->index () == iID) {
		VEvaluation::Reference pThisRequest; 
		pThisRequest.claim (m_pThisRequest);
		VPoolEvaluation::Reference pEvaluation (static_cast <VPoolEvaluation*> (pThisRequest.referent ()));
		pEvaluation->onError (0, "Query cancelled.");

		VString sLog;
		sLog << "Cancelled current query (" << iID << ").";
		log (sLog);
		if (pClient) pClient->OnData (true);
			return;
    }
    
    // Look in the queue for the query.
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd ()) {
        if ((*iterator)->requestIndex () == iID) {
            VPoolEvaluation::Reference pEvaluation (static_cast <VPoolEvaluation*> ((*iterator)->request ()));
			VString iQuery (pEvaluation->queryString ());
			notify (17, "#17: VEvaluatorPool::cancelQuery: cancelled queued query - %s\n", iQuery.content ());
            iterator.Delete ();
            pEvaluation->onError (0, "Query cancelled.");
            VString sLog;
            sLog << "Cancelled queued query (" << iID << ").";
            log (sLog);
            if (pClient) pClient->OnData (true);
            return;
        }
        ++iterator;
    }

    // Look in the current WIP list for the query.
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    VReference<WIP> pWIP;
    IEvaluation *pRemoteIEvaluation;
    while (pGeneration) {
        if (pGeneration->isProcessing (iID, pWIP)) {
	    if (pWIP) {	
		VString iQuery (pWIP->evaluation ()->queryString ());
		notify (17, "#17: VEvaluatorPool::cancelQuery: cancelled running query - %s\n", iQuery.content ());

		pRemoteIEvaluation = pWIP->remoteIEvaluation ();
		if (pRemoteIEvaluation) {
		    pWIP->cancelQuery ();
		}

	    }
	    VString sLog;
	    sLog << "Cancelled running query (" << iID << ").";
	    log (sLog);
	    if (pClient) pClient->OnData (true);
	    return;
        }
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }

    // Query not found in queue nor in WIP list. Error, err, non-error-completion-state.
    VString iError;
    iError << "Query (" << iID << ") not found in queue nor work-in-progress list.";
    if (pClient) pClient->OnError (0, iError);
}

bool Vsa::VEvaluatorPool::cancel_ (VEvaluation *pEvaluation) {
    // Cast to VPoolEvaluation.
    VPoolEvaluation *pPoolEvaluation = dynamic_cast<VPoolEvaluation*> (pEvaluation);
    if (!pPoolEvaluation) return false;

    // Look in the current WIP list for the query.
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    VReference<WIP> pWIP;
    IEvaluation *pRemoteIEvaluation;
    while (pGeneration) {
        if (pGeneration->isProcessing (pPoolEvaluation, pWIP)) {
	    if (pWIP) { 	
		pRemoteIEvaluation = pWIP->remoteIEvaluation ();	
		if (pRemoteIEvaluation && (!pWIP->remoteIEvaluationCancelled ())) 
		    pWIP->cancelQuery ();
		VString sLog;
		sLog << "Cancelled running query (" << pWIP->evaluationIndex () << ") successfully.";
		log (sLog);
		return true;
	    } 
        } 
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }

    // Not found.
    return false;
}

/*******************************
 *****  Worker Operations  *****
 *******************************/

/********
 * Note: If the pool already has maximum workers in use then supplyWorkerFor
 *       routine fails. A PoolEvaluation can specify both Generation identifier and
 *       Worker identifier. If both are zero, it means any worker from any generation
 *       can process the PoolEvaluation. This routine removes a free worker from
 *       the ONLINE free worker list and returns the worker for use.
 ****/

bool Vsa::VEvaluatorPool::supplyOnlineWorkerFor (
    VPoolEvaluation *pEvaluation, VReference<VPoolWorker> &rpWorker
) const {

    if (m_cOnlineWIPs >= workerMaximum ())
        return false;

    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    bool bFoundWorker = false;

    while (pGeneration && !bFoundWorker) {
        bFoundWorker = pGeneration->supplyOnlineWorkerFor (pEvaluation, rpWorker);
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    return bFoundWorker;
}

void Vsa::VEvaluatorPool::finishHiringWorker () {
  VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
  while (pGeneration) {
      if  (pGeneration->hasStartedWorkers ()) {
	  pGeneration->finishHiringWorker ();
	  if (!pGeneration->isCurrent ())
	      hire ();
	  return;
      }
    pGeneration.setTo (pGeneration->m_pSuccessor);
  }
}

/******
 * Note: getWorkerReference/getWorkerInterface just gets the worker reference
 *       and interface of the worker id specified. The worker may be free or working.
 ****/

bool Vsa::VEvaluatorPool::getWorkerReference (
    VString const & iWorkerId, bool bIsPID, VReference<Worker> &rpWorker
) const {
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    bool bFound = false;
    while (pGeneration && !bFound) {
      if (pGeneration->getWorkerReference (iWorkerId, bIsPID, rpWorker))
        bFound = true;
      pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    return bFound;
}

void Vsa::VEvaluatorPool::getWorkerInterface (
    IVReceiver<IVUnknown*>* pReceiver, Vca::U32 iWorkerId
) const {
  VReference <Worker> pWorker;
  VString iId; iId<<iWorkerId;
  bool bIsPID = false;
  if (getWorkerReference (iId, bIsPID, pWorker)) {
     VReference <IPoolWorker> IWorker;
     pWorker->getRole (IWorker);
     pReceiver->OnData (IWorker);
  }
  else {
    VString iMsg;
    iMsg << "Worker " << iWorkerId << " not available in the pool";
    pReceiver->OnError (0, iMsg);
  }
}


/****************************
 *****  GenerationList  *****
 ****************************/

void Vsa::VEvaluatorPool::attach (WorkerGeneration *pGeneration) {
    pGeneration->m_pSuccessor = m_pGenerationHead;
    if (m_pGenerationHead.isntNil ())
        m_pGenerationHead->m_pPredecessor = pGeneration;
    else
        m_pGenerationTail = pGeneration;
    m_pGenerationHead.setTo (pGeneration);

    // Clean up old generations past our generation limit.
    if (m_pGenerationHead->generationId () - m_pGenerationTail->generationId () >= m_pSettings->generationMaximum ()) {
        m_pGenerationTail->detachRetiredWorkers ();
    }
    // The above 'if' was changed to a 'while' which sometimes resulted in an infinite loop. For a quick fix,
    //   reverting to earlier 'if' version and logging the occurrence.
    if (m_pGenerationHead->generationId () - m_pGenerationTail->generationId () >= m_pSettings->generationMaximum ()) {
	log ("Note: Generations not cleaned up. head:%u, tail:%u, max:%u",
	     m_pGenerationHead->generationId (), m_pGenerationTail->generationId (), m_pSettings->generationMaximum ());
    }
}

void Vsa::VEvaluatorPool::detach (WorkerGeneration *pGen) {
    if (pGen->m_pSuccessor)
        pGen->m_pSuccessor->m_pPredecessor = pGen->m_pPredecessor;
    else
        m_pGenerationTail = pGen->m_pPredecessor;

    if (pGen->m_pPredecessor)
        pGen->m_pPredecessor->m_pSuccessor = pGen->m_pSuccessor;
    else
        m_pGenerationHead.setTo (pGen->m_pSuccessor);

    pGen->m_pSuccessor = 0;
    pGen->m_pPredecessor = 0;
}

void Vsa::VEvaluatorPool::detachAllGenerations () {
  VReference<WorkerGeneration> pGeneration (m_pGenerationTail);
  while (pGeneration) {
    detach (pGeneration);
    pGeneration.setTo (m_pGenerationTail);
  }
}


/************************
 ***** Odometers   ******
 ***********************/
 void Vsa::VEvaluatorPool::setupOdometers () {
	unsigned int xIndex;
	VOdometer<Vca::U32>::Reference pOdometer (&m_cOnlineWorkers);
    if (m_iOdometers32.Insert ("Workers", xIndex)) {
        m_iOdometers32[xIndex] = pOdometer;
	}
	
	VOdometer<Vca::U32>::Reference pOdometer2 (&m_cOnlineAvailWorkers);
    if (m_iOdometers32.Insert ("AvailWorkers", xIndex)) {
        m_iOdometers32[xIndex] = pOdometer2;
	}
	
	VOdometer<Vca::U32>::Reference pOdometer3 (&m_cOnlineWIPs);
    if (m_iOdometers32.Insert ("WIPs", xIndex)) {
        m_iOdometers32[xIndex] = pOdometer3;
	}
	
	VOdometer<Vca::U32>::Reference pOdometer4 (&m_cQueriesProcessed);
    if (m_iOdometers32.Insert ("QueriesProcessed", xIndex)) {
        m_iOdometers32[xIndex] = pOdometer4;
	}
	
	VOdometer<Vca::U64>::Reference pOdometer5 (&m_cTotalQueryTime);
    if (m_iOdometers64.Insert ("QueryTime", xIndex)) {
        m_iOdometers64[xIndex] = pOdometer5;
	}
	
	VOdometer<Vca::U64>::Reference pOdometer6 (&m_cTotalQueueTime);
    if (m_iOdometers64.Insert ("QueueTime", xIndex)) {
        m_iOdometers64[xIndex] = pOdometer6;
	}
	
	for(int i = 0; i < m_pSettings->GetOdometerSettings().cardinality(); i++) {
		OdometerSettings::Reference odo = m_pSettings->GetOdometerSettings()[i];
		VString name = odo->name;
		unsigned int xInstance;
		if (m_iOdometers32.Locate (name, xInstance)) {
			 VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
			 odometer->setUncompressedTotalTimeRange(odo->uncompressedTotalTimeRange);
			 odometer->setTimerTime(odo->timerTime);
			 odometer->setTotalTimeRange(odo->totalTimeRange);
			 odometer->setCompressedBucketSize(odo->compressedBucketSize);
			 odometer->setTimer();
		} else if (m_iOdometers64.Locate (name, xInstance)) {
			 VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
			 odometer->setUncompressedTotalTimeRange(odo->uncompressedTotalTimeRange);
			 odometer->setTimerTime(odo->timerTime);
			 odometer->setTotalTimeRange(odo->totalTimeRange);
			 odometer->setCompressedBucketSize(odo->compressedBucketSize);
			 odometer->setTimer();
		}
	}
 }


/*****************************
 *****************************
 *****  Query Execution  *****
 *****************************
 *****************************/


void Vsa::VEvaluatorPool::onQueue_ () {
  //  if hardstopped...
  if (Status_Stopped == m_iStatus && m_pHardStopTrigger.isntNil ()) {
    if (!hasActiveWIPs () ) {
      // complete the local portion of the hardstop ...
      detachAllGenerations ();
      // ... and pull the trigger on the rest ...
      m_pHardStopTrigger->Process ();
      m_pHardStopTrigger.clear ();
    }
  } else {
    if (isWorkerCreationFailureHardLmtReached () && m_pGenerationHead->onlineWorkerCount () == 0) {
        VString iMsg;
	// Applications check for the phrase "Worker Creation Failure". Don't change this message until we have a better way of 
	//   getting this information to the end user ....
        iMsg << "\nWorker Creation Failure HARD Limit (" << m_pGenerationHead->workerCreationHardFailures () <<") reached.\n";
	notify (4, "#4: VEvaluatorPool::onQueue: %s\n", iMsg.content ());

        flushQueue (iMsg);
    }
    bool bProcessedQueries = false;
    do {
      hire ();
      bProcessedQueries = processQueries ();
    } while (bProcessedQueries);
  }
  checkAssertions ();
}

void Vsa::VEvaluatorPool::onRequest (VEvaluation *pRequest) {
    VPoolEvaluation *pEvaluation = static_cast <VPoolEvaluation*> (pRequest);
    VString query (pRequest->queryString ()), queryShortened;
    if (queryLogLength ()) query.getSummary (queryShortened, queryLogLength ());
    log ("Request(%d) (With valid generation %d) arrived: %s",
        pRequest->index (),
        pEvaluation->validGeneration (),
        queryLogLength () ? queryShortened.content () : query.content ()
    );
}

bool Vsa::VEvaluatorPool::processQueries () {
  bool bProcessedBCQueries = processBCEvaluations ();
  bool bProcessedQueries = processQueryEvaluations ();

  return bProcessedBCQueries || bProcessedQueries;
}

bool Vsa::VEvaluatorPool::processQueryEvaluations () {

    bool bProcessed = false;

    //  process queries with available workers...
    VPoolEvaluation::Reference pRequest;
    pRequest.setTo (dynamic_cast <VPoolEvaluation*> (thisRequest ()));
    VReference<VPoolWorker> pWorker;
    while (pRequest && supplyOnlineWorkerFor (pRequest, pWorker)) {
        VPoolEvaluation::Reference pThisRequest (pRequest);
        pRequest.setTo (dynamic_cast<VPoolEvaluation*> (nextRequest ()));
        pWorker->process (pThisRequest);
        bProcessed = true;
    }

    //  scan for processing the remaining queries
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd () && onlineAvailWorkers () > 0) {
        VPoolEvaluation::Reference pEvaluation (dynamic_cast <VPoolEvaluation*> ((*iterator)->request ()));
        if (pEvaluation && supplyOnlineWorkerFor (pEvaluation, pWorker)) {
            iterator.Delete ();
            pWorker->process (pEvaluation);
            bProcessed = true;
        }
        else
            ++iterator;
    }
    return bProcessed;
}

bool Vsa::VEvaluatorPool::processBCEvaluations () {
    bool bProcessed = false;

    Queue::Iterator iterator (m_iBCEvaluationQueue);
    while (iterator.isNotAtEnd () && onlineAvailWorkers () > 0
        && !hasMaxBCEvaluationsInProgress ()
    ) {
        VPoolEvaluation::Reference pEvaluation (static_cast<VPoolEvaluation*>((*iterator)->request ()));
        VReference<VPoolWorker> pWorker;

        if (supplyOnlineWorkerFor (pEvaluation, pWorker)) {
            iterator.Delete ();
            incrementBCEvaluationsInProgress ();
            pWorker->process (pEvaluation);
            bProcessed = true;
        }
        else
            ++iterator;
    }
    return bProcessed;
}

void Vsa::VEvaluatorPool::onAnyDataUpdateFor (VPoolEvaluation *pEvaluation) {

    //  if PoolEvaluation is in queue, allow it to be reprocessed...
    onQueue ();

    //  if PoolEvaluation is in WIP list of a generation...
    //  i.e. currently being evaluated either by a a direct batchvision worker or by another pool (pool of pool case)

    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    bool bProcessed = false;

    while (pGeneration && !bProcessed) {
      VReference<WIP> pWIP;
      if (pGeneration->isProcessing (pEvaluation, pWIP)) {
        pWIP->onAnyDataUpdate ();
        bProcessed = true;
      }
      pGeneration.setTo (pGeneration->m_pSuccessor);
    }
}

void Vsa::VEvaluatorPool::onRetryableUpdateFor (VPoolEvaluation *pEvaluation) {
    //  if PoolEvaluation is in WIP list of a generation...
    //  i.e. currently being evaluated either by a a direct batchvision worker or by another pool (pool of pool case)

    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    bool bProcessed = false;

    while (pGeneration && !bProcessed) {
      VReference<WIP> pWIP;
      if (pGeneration->isProcessing (pEvaluation, pWIP)) {
        pWIP->onRetryableUpdate ();
        bProcessed = true;
      }
      pGeneration.setTo (pGeneration->m_pSuccessor);
    }
}

/********************************************
 *****  Vsa::IEvaluatorControl Methods  *****
 ********************************************/

void Vsa::VEvaluatorPool::Suspend (
    IEvaluatorControl *pRole, IVReceiver<bool>* pReceiver
) {
    suspend ();
    if (pReceiver)
        pReceiver->OnData (true);
}

void Vsa::VEvaluatorPool::Resume (
    IEvaluatorControl *pRole, IVReceiver<bool>* pReceiver
) {
    resume ();
    if (pReceiver)
        pReceiver->OnData (true);
}

void Vsa::VEvaluatorPool::Restart (
    IEvaluatorControl *pRole, IVReceiver<bool>* pReceiver
) {
    restart ();
    if (pReceiver)
        pReceiver->OnData (true);
}

void Vsa::VEvaluatorPool::Stop (
    IEvaluatorControl *pRole, IVReceiver<bool>* pReceiver
) {
    stop ();
    if (pReceiver)
        pReceiver->OnData (true);
}

void Vsa::VEvaluatorPool::GetStatistics (
    IEvaluatorControl *pRole, IVReceiver<VString const &>* pReceiver
) {
    if (pReceiver) {
        VString result;
        getStatistics (result);
        pReceiver->OnData (result);
    }
}

void Vsa::VEvaluatorPool::GetWorker (
    IEvaluatorControl *pRole, IVReceiver<IVUnknown*> *pReceiver,
    Vca::U32 workerIdentifier
) {
    //  provided for compatibility with older pool admin versions
    getWorkerInterface (pReceiver, workerIdentifier);
}

/*********************************
 *****  Vsa::IEvaluatorPool  *****
 *********************************/

void Vsa::VEvaluatorPool::GetSettings (
    IEvaluatorPool *pRole, IVReceiver<IEvaluatorPoolSettings*> *pClient
) {
    if (pClient) {
        VReference<IEvaluatorPoolSettings_Ex8> pIEvaluatorPoolSettings;
        m_pSettings->getRole (pIEvaluatorPoolSettings);
        pClient->OnData (pIEvaluatorPoolSettings);
    }
}
void Vsa::VEvaluatorPool::AddEvaluator (
    IEvaluatorPool *pRole, IEvaluator *pEvaluator
) {
    add (pEvaluator);
}

/********
 * Note: FlushWorkers operation notes the Last Worker which is supposed
 *       to be flushed (m_iLastToBeFlushedWorker). Retires existing workers.
 *       Later when workers that need to be flushed arrive...they are not
 *       returned to the worker generation
 ****/

void Vsa::VEvaluatorPool::FlushWorkers (
    IEvaluatorPool *pRole, IVReceiver<bool> *pClient
) {
    flushWorkers ();
    if (pClient)
        pClient->OnData (true);
}


/*********************************************
 *****  Vsa::IEvaluatorPool_Ex1 Methods  *****
 *********************************************/

void Vsa::VEvaluatorPool::HardRestart (
    IEvaluatorPool_Ex1 *pRole, IVReceiver<bool>* pReceiver
) {
    hardRestart ();
    if (pReceiver)
        pReceiver->OnData (true);
}

void Vsa::VEvaluatorPool::IsWorkerCreationFailureLmtReached (
    IEvaluatorPool_Ex1* pRole, IVReceiver<bool> *pReceiver
) {
    pReceiver->OnData (isWorkerCreationFailureHardLmtReached ());
}

void Vsa::VEvaluatorPool::GetCurrentGenerationTotalWorkers (
    IEvaluatorPool_Ex1* pRole, IVReceiver<Vca::U32> *pReceiver
) {
    Vca::U32 iWorkerCount = m_pGenerationHead->onlineWorkerCount ();
    pReceiver->OnData (iWorkerCount);
}

/********
 * Note: GetTotalWorkers returns the total number of workers in the entire
 *       pool both free workers and in use workers
 ****/

void Vsa::VEvaluatorPool::GetTotalWorkers (
    IEvaluatorPool_Ex1* pRole, IVReceiver<Vca::U32> *pReceiver
) {
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    Vca::U32 cWorkerCount =0;

    while (pGeneration.isntNil ()) {
        cWorkerCount += pGeneration->onlineWorkerCount ();
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    pReceiver->OnData (cWorkerCount);
}

/********
 * Note: GetWorkerInterface method returns the Worker Interface of the
 *       required worker. First checks the list of WIPs for the worker.
 *       Then checks all the generations for the worker.
 ****/

void Vsa::VEvaluatorPool::GetWorkerInterface (
    IEvaluatorPool_Ex1 *pRole, IVReceiver<IVUnknown*> *pReceiver,
    Vca::U32 workerIdentifier
) {
    getWorkerInterface (pReceiver, workerIdentifier);
}

void Vsa::VEvaluatorPool::HardStop (
    IEvaluatorPool_Ex1 *pRole, IVReceiver<bool>* pReceiver
) {
    hardstop ();
    if (pReceiver)
        pReceiver->OnData (true);
}


/*********************************************
 *****  Vsa::IEvaluatorPool_Ex2 Methods  *****
 *********************************************/

void Vsa::VEvaluatorPool::BroadcastExpression (
    IEvaluatorPool_Ex2 *pRole, IEvaluatorClient *pClient,
    VString const& rPath, VString const& rExpression
) {
    if (onlineWorkerCount ()>0) {
        VReference<VPoolBroadcast> pBroadcast;
        pBroadcast.setTo (new VPoolBroadcast (this, pClient, rPath, rExpression));
        processBCObject (pBroadcast);
    }
    else
        pClient->OnResult (0, "");
}


void Vsa::VEvaluatorPool::BroadcastURL (
    IEvaluatorPool_Ex2 *pRole, IEvaluatorClient *pClient,
    VString const& rPath, VString const& rQuery, VString const& rEnvironment
 ) {
    if (onlineWorkerCount () >0) {
        VReference<VPoolBroadcast> pBroadcast;
        pBroadcast.setTo (new VPoolBroadcast (this, pClient, rPath, rQuery, rEnvironment));
        processBCObject (pBroadcast);
    }
    else
        pClient->OnResult (0, "");
}

/*********************************************
 *****  Vsa::IEvaluatorPool_Ex3 Methods  *****
 *********************************************/

void Vsa::VEvaluatorPool::TakeWorkerOffline (
   IEvaluatorPool_Ex3* pRole, VString const &rId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  takeWorkerOffline (rId, bIsPID, pClient);
}

void Vsa::VEvaluatorPool::MakeWorkerOnline (
   IEvaluatorPool_Ex3* pRole, VString const &rId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  makeWorkerOnline (rId, bIsPID, pClient);
}

void Vsa::VEvaluatorPool::EvaluateUsingOfflineWorker (
   IEvaluatorPool_Ex3* pRole, VString const &rQuery, VString const &rId, bool bIsPID, IEvaluatorClient *pClient
) {
   evaluateUsingOfflineWorker (rQuery, rId, bIsPID, pClient);
}

void Vsa::VEvaluatorPool::RetireOfflineWorker (
   IEvaluatorPool_Ex3* pRole, VString const &rId, bool bIsPID, IVReceiver<VString const &>* pClient
) {
  retireOfflineWorker (rId, bIsPID, pClient);
}

void Vsa::VEvaluatorPool::DumpWorker (
    IEvaluatorPool_Ex3* pRole, VString const &rId, bool bIsPID, VString const &rFile,
    IVReceiver<VString const &>*pClient
) {
  dumpWorker (rId, bIsPID, rFile, pClient);
}

void Vsa::VEvaluatorPool::DumpPoolQueue (
   IEvaluatorPool_Ex3* pRole, VString const &rFile, Vca::U32 cQueries, IVReceiver<VString const &>* pClient
) {
  dumpPoolQueue (rFile, cQueries, pClient);
}

void Vsa::VEvaluatorPool::DumpWorkingWorkers (
   IEvaluatorPool_Ex3* pRole, VString const &rFile, IVReceiver<VString const &>* pClient
) {
  dumpWorkingWorkers (rFile, pClient);
}

void Vsa::VEvaluatorPool::GetWorker_Ex (
   IEvaluatorPool_Ex3* pRole, VString const &rID, bool bIsPID, IVReceiver<IVUnknown *>* pClient
) {
    VReference<Worker> pWorker;
    if (pClient) {
        if (getWorkerReference (rID, bIsPID, pWorker)) {
            VReference <IPoolWorker> pIWorker;
            pWorker->getRole (pIWorker);
            pClient->OnData (pIWorker);
        }
        else
            pClient->OnError (0, "Worker not available in the pool");
    }
}

void Vsa::VEvaluatorPool::GetWorkersStatistics (
   IEvaluatorPool_Ex3* pRole, IVReceiver<VString const &>* pClient
) {
  if (pClient) {
    VString iStat;
    getWorkersStatistics (iStat);
    pClient->OnData (iStat);
  }
}

void Vsa::VEvaluatorPool::GetGenerationStatistics (
    IEvaluatorPool_Ex3* pRole, Vca::U32 iID, IVReceiver<VString const &>* pClient
) {
  if (pClient) {
    VString iStat;
    getGenerationStatistics (iID, iStat);
    pClient->OnData (iStat);
  }
}

/*********************************************
 *****  Vsa::IEvaluatorPool_Ex4 Methods  *****
 *********************************************/

void Vsa::VEvaluatorPool::QueryDetails (IEvaluatorPool_Ex4 *pRole, Vca::U32 iID, IVReceiver<VString const &>* pClient) {
    if (pClient) {
        VString iResult;
        queryDetails (iID, iResult);
        pClient->OnData (iResult);
    }
}

void Vsa::VEvaluatorPool::CancelQuery (IEvaluatorPool_Ex4 *pRole, Vca::U32 iID, BoolReceiver *pClient) {
    cancelQuery (iID, pClient);
}

/*********************************************
 *****  Vsa::IEvaluatorPool_Ex5 Methods  *****
 *********************************************/
void Vsa::VEvaluatorPool::getRole (VReference<IEvaluatorPool_Ex5>&rpRole) {
    m_pIEvaluatorPool.getRole (rpRole);
}

void Vsa::VEvaluatorPool::StatSum (IEvaluatorPool_Ex5 *pRole, VString name, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<VString const &> *pClient) {
	unsigned int xInstance;
    if (m_iOdometers32.Locate (name, xInstance)) {
         VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		 try {
			Vca::S64 resultInt = odometer->statSum(timeBefore, timeRange); 
			VString result;
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			pClient->OnData(result);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name, xInstance)) {
         VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		 try {
			Vca::S64 resultInt = odometer->statSum(timeBefore, timeRange); 
			VString result;
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			pClient->OnData(result);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name"); 
	}
}

void Vsa::VEvaluatorPool::StatDivide (IEvaluatorPool_Ex5 *pRole, VString name1, VString name2, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<Vca::F64> *pClient) {
	unsigned int xInstance1;
	unsigned int xInstance2;
	Vca::F64 sum1;
	Vca::F64 sum2;
	if (m_iOdometers32.Locate (name1, xInstance1)) {
		VOdometer<Vca::U32>::Reference odometer1 = m_iOdometers32[xInstance1];
		try {
			sum1 = odometer1->statSum(timeBefore, timeRange);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name1, xInstance1)) {
		VOdometer<Vca::U64>::Reference odometer1 = m_iOdometers64[xInstance1];
		try {
			sum1 = odometer1->statSum(timeBefore, timeRange);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
		return;
	}
	
	if (m_iOdometers32.Locate (name2, xInstance2)) {
		VOdometer<Vca::U32>::Reference odometer2 = m_iOdometers32[xInstance2];
		try {
			sum2 = odometer2->statSum(timeBefore, timeRange);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name2, xInstance2)) {
		VOdometer<Vca::U64>::Reference odometer2 = m_iOdometers64[xInstance2];
		try {
			sum2 = odometer2->statSum(timeBefore, timeRange);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
		return;
	}
	pClient->OnData(sum1 / sum2);
}

void Vsa::VEvaluatorPool::StatMax (IEvaluatorPool_Ex5 *pRole, VString name, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<Vca::U32> *pClient, IVReceiver<V::VTime const &> *pClient2) {
	unsigned int xInstance;
    if (m_iOdometers32.Locate (name, xInstance)) {
		VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 result = odometer->statMax(timeBefore, timeRange, time);
			pClient->OnData(result);
			pClient2->OnData(time);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name, xInstance)) {
         VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 result = odometer->statMax(timeBefore, timeRange, time);
			pClient->OnData(result);
			pClient2->OnData(time);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
	}
}

void Vsa::VEvaluatorPool::StatMin (IEvaluatorPool_Ex5 *pRole, VString name, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<Vca::U32> *pClient, IVReceiver<V::VTime const &> *pClient2) {
	unsigned int xInstance;
    if (m_iOdometers32.Locate (name, xInstance)) {
		VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 result = odometer->statMin(timeBefore, timeRange, time);
			pClient->OnData(result);
			pClient2->OnData(time);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name, xInstance)) {
         VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 result = odometer->statMin(timeBefore, timeRange, time);
			pClient->OnData(result);
			pClient2->OnData(time);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
	}
}

void Vsa::VEvaluatorPool::StatMinString (IEvaluatorPool_Ex5 *pRole, VString name, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<VString const &> *pClient) {
	unsigned int xInstance;
	if (m_iOdometers32.Locate (name, xInstance)) {
         VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 resultInt = odometer->statMin(timeBefore, timeRange, time);
			VString result;
			VString timeString;
			time.asString(timeString);
			result << "Min value of " << name << " was ";
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			result << " occuring at " << timeString;
			pClient->OnData(result);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name, xInstance)) {
         VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 resultInt = odometer->statMin(timeBefore, timeRange, time);
			VString result;
			VString timeString;
			time.asString(timeString);
			result << "Min value of " << name << " was ";
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			result << " occuring at " << timeString;
			pClient->OnData(result);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
	}
}

void Vsa::VEvaluatorPool::StatMaxString (IEvaluatorPool_Ex5 *pRole, VString name, Vca::U64 timeBefore, Vca::U64 timeRange, IVReceiver<VString const &> *pClient) {
	unsigned int xInstance;
    if (m_iOdometers32.Locate (name, xInstance)) {
         VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 resultInt = odometer->statMax(timeBefore, timeRange, time);
			VString result;
			VString timeString;
			time.asString(timeString);
			result << "Max value of " << name << " was ";
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			result << " occuring at " << timeString;
			pClient->OnData(result);
		} catch (VOdometer<Vca::U32>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else if (m_iOdometers64.Locate (name, xInstance)) {
         VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		 V::VTime time;
		 try {
			Vca::U64 resultInt = odometer->statMax(timeBefore, timeRange, time);
			VString result;
			VString timeString;
			time.asString(timeString);
			result << "Max value of " << name << " was ";
			if(odometer->displayMultiplier() == 1)
				result << resultInt;
			else
				result << resultInt * odometer->displayMultiplier();	
			result << " occuring at " << timeString;
			pClient->OnData(result);
		} catch (VOdometer<Vca::U64>::OdometerImproperInputException e) {
			pClient->OnError(0, e.getMessage());
			return;
		}
	} else {
		pClient->OnError(0, "Pool does not possess an odometer with that name");
	}
}

void Vsa::VEvaluatorPool::AllStatValues (IEvaluatorPool_Ex5 *pRole, VString name, IVReceiver<VkDynamicArrayOf<Vca::U64> const &> *valuesClient, 
							IVReceiver<VkDynamicArrayOf<V::VTime> const &> *timesClient, IVReceiver<VkDynamicArrayOf<Vca::U64> const &> *maxsClient, 
							IVReceiver<VkDynamicArrayOf<Vca::U64> const &> *minsClient, IVReceiver<VkDynamicArrayOf<Vca::F64> const &> *sigmasClient) {
	unsigned int xInstance;
	VkDynamicArrayOf<Vca::U64> values, maxs, mins;
	VkDynamicArrayOf<V::VTime> times;		
	VkDynamicArrayOf<Vca::F64> sigmas;
	if (m_iOdometers32.Locate (name, xInstance)) {
		VOdometer<Vca::U32>::Reference odometer = m_iOdometers32[xInstance];
		odometer->boxMonsterValues(values, times, maxs, mins, sigmas);
	} else if (m_iOdometers64.Locate(name, xInstance)){
		VOdometer<Vca::U64>::Reference odometer = m_iOdometers64[xInstance];
		odometer->boxMonsterValues(values, times, maxs, mins, sigmas);
	} else  {
		valuesClient->OnError(0, "Pool does not possess an odometer with that name");
		timesClient->OnError(0, "Pool does not possess an odometer with that name");
		maxsClient->OnError(0, "Pool does not possess an odometer with that name");
		minsClient->OnError(0, "Pool does not possess an odometer with that name");
		sigmasClient->OnError(0, "Pool does not possess an odometer with that name");
		return;
	}
	valuesClient->OnData(values);
	timesClient->OnData(times);
	maxsClient->OnData(maxs);
	minsClient->OnData(mins);	
	sigmasClient->OnData(sigmas);
}

/**************************
 *****  Trim Workers  *****
 **************************/

/**
 * Note: By default trimWorkers trims only old (generation) workers.
 */

void Vsa::VEvaluatorPool::trimWorkers (Vca::U32 cWorkers, bool bOldOnly) {
	// First retire idle worker
    VReference<WorkerGeneration> pGeneration (m_pGenerationTail);
    while (pGeneration && cWorkers > 0) {
        if (bOldOnly && pGeneration->isCurrent ())
            break;
        cWorkers -= pGeneration->trimWorkers (cWorkers, true);
        pGeneration.setTo (pGeneration->m_pPredecessor);
    }
	// Retire worker in use, mark as excessWorker
    pGeneration.setTo (m_pGenerationTail);
    while (pGeneration && cWorkers > 0) {
        if (bOldOnly && pGeneration->isCurrent ())
            break;
        cWorkers -= pGeneration->trimWorkers (cWorkers, false);
        pGeneration.setTo (pGeneration->m_pPredecessor);
    }
}

/**
 * Callback invoked when shrink timer fires.
 *
 * Gets rid of shrink timer, invokes trimWorkers().
 */
void Vsa::VEvaluatorPool::onShrinkTimeOut (Vca::VTrigger<ThisClass> *pTrigger) {
    log ("Shrink timer fired. Shrinking.");
    m_cShrinkTimerExpirations++;
    m_pShrinkTimer->cancel ();
    m_pShrinkTimer.clear ();
    Vca::U32 cWorkers = excessIdleWorkers ();
    if (m_cTempWorkerMinimum > m_pSettings->workerMinimum ()) {
        // if we are restarting, we won't need as many new workers ...
		setTempWorkerMinimumTo (m_cTempWorkerMinimum - cWorkers);
    }
    trimWorkers (cWorkers, false);
}

/**
 * Sets the shrink timer. Does nothing if timer was already set.
 */
void Vsa::VEvaluatorPool::setShrinkTimer () {
    if (m_pShrinkTimer.isntNil ()) return;

    if (shrinkTimeOut () != UINT_MAX) {
        log("Setting new shrink timer.");
        VReference<Vca::VTrigger<ThisClass> > pTrigger (
            new Vca::VTrigger<ThisClass> (this, &ThisClass::onShrinkTimeOut)
        );
        m_pShrinkTimer.setTo (
            new Vca::VTimer ("VPool shrink timer", pTrigger, static_cast <Vca::U64> (shrinkTimeOut ())*60*1000*1000)
        );
        m_pShrinkTimer->start ();
    }
}

/**
 * Stops the shrink timer. Does nothing if shrink timer does not exist or is not set.
 */
void Vsa::VEvaluatorPool::stopShrinkTimer() {
    if (m_pShrinkTimer.isNil ()) return;
    log("Stopping existing shrink timer.");
    m_pShrinkTimer->cancel();
    m_pShrinkTimer.clear();
}

/***********************
 ***********************
 *****  Broadcast  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPool::processBCObject (VPoolBroadcast *pBroadcast) {

    appendBCObject (pBroadcast);

    VReference<VPoolWorkerGeneration> pGeneration (m_pGenerationHead);

    //  broadcast to each generation
    while (pGeneration) {
        pGeneration->processBCObject (pBroadcast);
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    onQueue ();
}


void Vsa::VEvaluatorPool::appendBCObject (VPoolBroadcast *pBroadcast) {
    if (m_pBCObjectListHead.isntNil ()) {
        pBroadcast->m_pSuccessor.setTo (m_pBCObjectListHead);
        m_pBCObjectListHead->m_pPredecessor.setTo (pBroadcast);
    }
    m_pBCObjectListHead.setTo (pBroadcast);
}

void Vsa::VEvaluatorPool::removeBCObject (VPoolBroadcast *pBroadcast) {

    if (pBroadcast == m_pBCObjectListHead) {
        m_pBCObjectListHead.claim (m_pBCObjectListHead->m_pSuccessor);
        if (m_pBCObjectListHead.isntNil ())
            m_pBCObjectListHead->m_pPredecessor.clear ();
    }
    else {
        pBroadcast->m_pPredecessor->m_pSuccessor.setTo (pBroadcast->m_pSuccessor);
        if (pBroadcast->m_pSuccessor.isntNil ())
            pBroadcast->m_pSuccessor->m_pPredecessor.setTo (pBroadcast->m_pPredecessor);
    }
}


void Vsa::VEvaluatorPool::enqueueBCEvaluation (VPoolEvaluation *pEvaluation) {
    m_iBCEvaluationQueue.enqueue (pEvaluation);
}

/****
 * removeAllBCEvaluationsFor :
 * This routine is called when a worker retires. When a worker retires all broadcasts which are
 * pending on the worker is removed.
 ****/


void Vsa::VEvaluatorPool::removeAllBCEvaluationsFor (Vca::U32 iWorkerId) {
    Queue::Iterator iterator (m_iBCEvaluationQueue);
    while (iterator.isNotAtEnd ()) {
        VReference<VPoolBroadcastEvaluation> pEvaluation (static_cast<VPoolBroadcastEvaluation*>((*iterator)->request ()));

        if (pEvaluation->workerId () == iWorkerId) {

            // communicate to the broadcast object, via IEvaluatorClient
            VReference<VPoolBroadcast> pBroadcast (pEvaluation->poolBroadcast ());
            VString errMessage ("Worker retired/disabled: Broadcast \"");
            errMessage << pEvaluation->queryString () << "\" cancelled to worker.\n";
            pBroadcast->OnEvaluationCancelled (errMessage);

            iterator.Delete ();
        }
        else
            ++iterator;
    }
}

/***************************************
 ***************************************
 *****  Offline Worker Management  *****
 ***************************************
 ***************************************/

void Vsa::VEvaluatorPool::takeWorkerOffline (
    VString const &rWorkerId, bool bIsPID, IVReceiver<VString const &> *pClient
) {
    if (pClient) {
        VReference <Worker> pWorker;
        if (getWorkerReference (rWorkerId, bIsPID, pWorker)) {
            VReference <WorkerGeneration> pGeneration (pWorker->generation ());
            pGeneration->takeWorkerOffline (pWorker, pClient);
            onQueue ();
        }
        else
            sendWorkerNotAvailableMsg (rWorkerId, bIsPID, pClient);
    }
}


void Vsa::VEvaluatorPool::makeWorkerOnline (
    VString const &rWorkerId, bool bIsPID, IVReceiver<VString const &> *pClient
) {
    if (pClient) {
        VReference <Worker> pWorker;
        if (getWorkerReference (rWorkerId, bIsPID, pWorker)) {
            VReference <WorkerGeneration> pGeneration (pWorker->generation ());
            pGeneration->makeWorkerOnline (pWorker, pClient);
            onQueue ();
        }
        else
            sendWorkerNotAvailableMsg (rWorkerId, bIsPID, pClient);
    }
}

void Vsa::VEvaluatorPool::evaluateUsingOfflineWorker (
    VString const &rQuery, VString const &rWorkerId, bool bIsPID, IEvaluatorClient *pClient
) {
    if (pClient) {
        VReference <Worker> pWorker;
        if (getWorkerReference (rWorkerId, bIsPID, pWorker)) {
            VReference <WorkerGeneration> pGeneration (pWorker->generation ());
            pGeneration->evaluateUsingOfflineWorker (pWorker, rQuery, pClient);
        }
        else if (pClient) {
            VString iResult;
            VString iTag = bIsPID ? "PID: " : "ID: ";
            iResult << "Worker " << iTag << rWorkerId << " is not currently in the pool";
            pClient->OnError (0, iResult);
        }
    }
}

void Vsa::VEvaluatorPool::retireOfflineWorker (
    VString const &rWorkerId, bool bIsPID, IVReceiver<VString const &> *pClient
) {
    if (pClient) {
        VReference <Worker> pWorker;
        if (getWorkerReference (rWorkerId, bIsPID, pWorker)) {
            VReference <WorkerGeneration> pGeneration (pWorker->generation ());
            pGeneration->retireOfflineWorker (pWorker, pClient);
        }
        else
            sendWorkerNotAvailableMsg (rWorkerId, bIsPID, pClient);
    }
}


/***********************
 ***********************
 *****  Snapshots  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPool::dumpWorker (
   VString const &rWorkerId, bool bIsPID, VString const &rFile, VStringReceiver* pClient
) const {
    VReference<Worker> pWorker;
    if (getWorkerReference (rWorkerId, bIsPID, pWorker))
        pWorker->dumpHistory (rFile, pClient);
    else
        sendWorkerNotAvailableMsg (rWorkerId, bIsPID, pClient);
}

void Vsa::VEvaluatorPool::dumpPoolQueue (
    VString const &rFile, Vca::U32 cQueries, VStringReceiver* pClient
) {
    V::VSimpleFile iFile; VString iResult;

    if (iFile.Open (rFile.content (), "a")) {
        VString iDump, iTimeStr; V::VTime iTime;
        iTime.asString (iTimeStr);
        VString iQuerySeparator;
        iQuerySeparator.setTo ("\n================================================================================\n");

        iDump << "\nPool Queue (Queries pending execution) (at " << iTimeStr.content () <<")";
        Vca::U32 cCount=1;
        if (m_pThisRequest) {
            iDump << iQuerySeparator << "Query "<< cCount++ <<".";
            iDump << m_pThisRequest->query ()->queryString ();
        }
        Queue::Iterator iterator (m_iQueue);
        while (iterator.isNotAtEnd ()) {
            VPoolEvaluation::Reference pEvaluation (static_cast <VPoolEvaluation*> ((*iterator)->request ()));
            iDump << iQuerySeparator << "Query " << cCount++ << "." << pEvaluation->queryString ();
            ++iterator;
        }
        iFile.printf (iDump.content ());
        iFile.close ();
        iResult.printf ("Wrote Pool Queue to %s", rFile.content ());
    }
    else
        iResult.printf ("Error opening file %s", rFile.content ());

    if (pClient)
        pClient->OnData (iResult);
}

void Vsa::VEvaluatorPool::queueDumpHeader (VString &rResult) {
    char iResult[42];
    sprintf (iResult,
             "%8s | %5s | %7s | %12s",
             "Index",
             "Retry",
             "AnyData",
             "Query String");
    rResult.setTo (iResult);
}

void Vsa::VEvaluatorPool::queueDumpItem (VPoolEvaluation::Reference &pRequest, VString &rResult) {
    char iResult[40];
    VString iQuery (pRequest->queryString ()), iQueryShortened;
    iQuery.getSummary (iQueryShortened, 80);
    sprintf (iResult,
             "%8d | %5d | %7s | ",
             pRequest->index (),
             pRequest->evaluationAttempt (),
             pRequest->anyData () ? "any" : "latest" );
    rResult << iResult << iQueryShortened.content();
}

void Vsa::VEvaluatorPool::queryDetails (VPoolEvaluation::Reference &pRequest, VString &rResult, WIP *pWIP) {
    VString iArrivalTime;
    pRequest->arrivalTime ().asString (iArrivalTime);
    rResult.clear ();
    rResult << "Index | " << pRequest->index () << "\n"
            << "Retry | " << pRequest->evaluationAttempt () << "\n"
            << "AnyData | " << pRequest->anyData () << "\n"
	    << "Valid Gen | " << pRequest->validGeneration () << "\n"
	    << "Arrival Time | " << iArrivalTime << "\n"
            << "State | " << (pWIP ? "Running" : "Queued") << "\n"
            << "Query | " << pRequest->queryString () << "\n";
}

void Vsa::VEvaluatorPool::queryDetails (Vca::U32 xRequest, VString &rResult) {
    VPoolEvaluation::Reference pRequest;
    // Search local member.
    if (m_pThisRequest && m_pThisRequest->index () == xRequest) {
        pRequest = static_cast <VPoolEvaluation*> (m_pThisRequest.referent ());
        queryDetails (pRequest, rResult);
        return;
    }

    // Search queue.
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd ()) {
        if ((*iterator)->requestIndex () == xRequest) {
            pRequest = static_cast <VPoolEvaluation*> ((*iterator)->request ());
            queryDetails (pRequest, rResult);
            return;
        }
        ++iterator;
    }

    // Search WIPs.
    WorkerGeneration::Reference pGeneration = m_pGenerationHead;
    WIP::Reference pWIP;
    while (pGeneration.isntNil ()) {
        // Search online WIPs.
        pWIP = pGeneration->m_pOnlineWIPHead;
        while (pWIP.isntNil()) {
            pRequest = pWIP->evaluation();
            if (pRequest->index () == xRequest) {
                queryDetails (pRequest, rResult, pWIP);
                return;
            }
            pWIP = pWIP->m_pSuccessor;
        }

        // Search offline WIPs.
        pWIP = pGeneration->m_pOfflineWIPHead;
        while (pWIP.isntNil()) {
            pRequest = pWIP->evaluation();
            if (pRequest->index () == xRequest) {
                queryDetails (pRequest, rResult, pWIP);
                return;
            }
            pWIP = pWIP->m_pSuccessor;
        }

        // Increment.
        pGeneration = pGeneration->m_pSuccessor;
    }

    // Not found.
    rResult = "Request not found in queue nor in works in progress.";
}

void Vsa::VEvaluatorPool::queueDump (VString &rResult) {
    rResult.clear();

    Vca::U32 cCount = 0;
    if (m_pThisRequest) {
        cCount++;
        VPoolEvaluation::Reference pRequest (static_cast <VPoolEvaluation*> (m_pThisRequest.referent ()));
        queueDumpItem (pRequest, rResult);
    }
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd ()) {
        cCount++;
        if (cCount > 1) rResult << "\n";
        VPoolEvaluation::Reference pRequest (static_cast <VPoolEvaluation*> ((*iterator)->request ()));
        queueDumpItem (pRequest, rResult);
        ++iterator;
    }
}

void Vsa::VEvaluatorPool::dumpWorkingWorkers (
    VString const &rFile, VStringReceiver* pClient
) const {
    V::VSimpleFile iFile; VString iResult;

    if (iFile.Open (rFile.content (), "a")) {
        VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
        while (pGeneration) {
            pGeneration->dumpWorkingWorkers (iFile);
            pGeneration.setTo (pGeneration->m_pSuccessor);
        }
        iResult.printf ("Wrote working worker stats to %s", rFile.content ());
        iFile.close ();
    }
    else
        iResult.printf ("Error opening file %s", rFile.content ());

    if (pClient)
        pClient->OnData (iResult);
}



/*****************************************
 *****  Worker Creation Failure Lmt  *****
 *****************************************/

void Vsa::VEvaluatorPool::flushQueue (VString const &rErrorMsg) {
    if (m_pThisRequest) {
        m_pThisRequest->onError (0, rErrorMsg.content ());
        m_pThisRequest.clear ();
    }
    Queue::Iterator iterator (m_iQueue);
    while (iterator.isNotAtEnd ()) {
        ((*iterator)->request ())->onError (0, rErrorMsg.content ());
        iterator.Delete ();
        ++iterator;
    }
}

bool Vsa::VEvaluatorPool::isWorkerCreationFailureHardLmtReached () const {
  return  m_pGenerationHead->isWorkerCreationFailureHardLmtReached ();
}

bool Vsa::VEvaluatorPool::isWorkerCreationFailureSoftLmtReached () const {
  return  m_pGenerationHead->isWorkerCreationFailureSoftLmtReached ();
}

void Vsa::VEvaluatorPool::onWorkerCreationFailureHardLmt () {
    VString iMsg;
    // Applications check for the phrase "Worker Creation Failure". Don't change this message until we have a better way of 
    //   getting this information to the end user  ....
    iMsg << "\nWorker Creation Failure HARD Limit (" << m_pGenerationHead->workerCreationHardFailures () <<") reached.\n";
    if (m_pGenerationHead->onlineWorkerCount () == 0) {
	notify (4, "#4: VEvaluatorPool::onWorkerCreationFailureHardLmt: %s\n", iMsg.content ());
	flushQueue (iMsg);
    }
}

void Vsa::VEvaluatorPool::onWorkerCreationFailureSoftLmt () {
    if (m_pThisRequest.isntNil ()) {
        VString iMsg;
	// Applications check for the phrase "Worker Creation Failure". Don't change this message until we have a better way of 
	//   getting this information to the end user ....

	iMsg << "\nWorker Creation Failure SOFT Limit (" << m_pGenerationHead->workerCreationSoftFailures () <<") reached.\n";
	notify (5, "#5: VEvaluatorPool::onWorkerCreationFailureSoftLmt: %s\n", iMsg.content ());
        m_pThisRequest->onError (0, iMsg);
        m_pThisRequest.clear ();
    }
}

/*************************
 *************************
 *****  Statistics   *****
 *************************
 *************************/

void Vsa::VEvaluatorPool::sendWorkerNotAvailableMsg (
    VString const &rWorkerId, bool bIsPID, IVReceiver<VString const &> *pClient
) const {
    if (pClient) {
        VString iResult;
        VString iTag = bIsPID ? "PID: " : "ID: ";
        iResult << "Worker " << iTag << rWorkerId << " is not currently in the pool";
        pClient->OnData (iResult);
    }
}

/*************************
 *************************
 *****  Statistics   *****
 *************************
 *************************/

void Vsa::VEvaluatorPool::getStatistics (VString &rResult) const {

    BaseClass::getStatistics (rResult);

    Vca::U32 cHardFailures = m_pGenerationHead->workerCreationHardFailures (); 
    rResult << "WorkerCreation HARD Failures    : " << cHardFailures << " (for generation " << currentGenerationId ();
    rResult << "\nTemporary Worker Minimum        : " << m_cTempWorkerMinimum;
    rResult << "\nShrink Operations               : " << m_cShrinkTimerExpirations;
    m_pSettings->getStatistics (rResult);

    //  format the online workers count stats...
    VString iOnline, iOnlinePadded, iOnlineAvail, iOnlineAvailPadded, iInUse;
    VString iInUsePadded, iStarting, iStartingPadded, iOnlineExcess, iOnlineExcessPadded;

    iOnline << m_cOnlineWorkers; iOnlinePadded.printf ("%-7s", iOnline.content ());
    iOnlineAvail << m_cOnlineAvailWorkers; iOnlineAvailPadded.printf ("%-16s", iOnlineAvail.content ());
    iInUse << m_cOnlineWIPs; iInUsePadded.printf ("%-16s", iInUse.content ());
    iStarting << m_cWorkersBeingCreated; iStartingPadded.printf ("%-10s", iStarting.content ());
    iOnlineExcess << m_cOnlineExcessWorkers; iOnlineExcessPadded.printf ("%-8s", iOnlineExcess.content ());

    //  print the online workers count stats....
    rResult << "\n\nWorker Information:";
    rResult << "\nMode          Total  Available       InUse           Starting  Excess  Retired";
    rResult << "\nOnline        ";
    rResult << iOnlinePadded << iOnlineAvailPadded << iInUsePadded << iStartingPadded;
    rResult << iOnlineExcessPadded << m_cWorkersRetired;

    //  format and print offline workers stat...
    if (m_cOfflineWorkers > 0) {
       VString iOffline, iOfflinePadded, iOfflineAv, iOfflineAvPadded, iOfflineInUse, iOfflineInUsePadded;
       iOffline << m_cOfflineWorkers; iOfflinePadded.printf ("%-7s", iOffline.content ());
       iOfflineAv << m_cOfflineAvailWorkers; iOfflineAvPadded.printf ("%-16s", iOfflineAv.content ());
       iOfflineInUse << m_cOfflineWIPs; iOfflineInUsePadded.printf ("%-26s", iOfflineInUsePadded.content ());
       rResult << "\nOffline       " << iOfflinePadded << iOfflineAvPadded << iOfflineInUsePadded << m_cOfflineExcessWorkers;
    }


    rResult << "\n\nWorker Generation Information:";
    rResult << "\n# - indicates worker will change mode after its current query";
    rResult << "\nGen/Mode      Total  Available       InUse           Starting  Excess  Retired\n";

    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    Vca::U32 cSkippedGenerations = 0;
    while (pGeneration) {
        if (pGeneration->isCurrent () ||
	        pGeneration->onlineWorkerCount () > 0 ||
	        pGeneration->offlineWorkerCount () > 0 ||
	        pGeneration->workersBeingCreated () > 0) {
            pGeneration->getStatistics (rResult);
            rResult << "\n";
        } else {
            cSkippedGenerations++;
        }
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    if (cSkippedGenerations > 0) {
        rResult << "(Hiding " << cSkippedGenerations << " generation";
        if (cSkippedGenerations > 1) rResult << "s";
        rResult << " with zero employed workers.)";
    }
}

void Vsa::VEvaluatorPool::getWorkersStatistics (VString &rResult) const {
    rResult << "\nPool Workers Statistics:";
    if (m_cOnlineWorkers >0 || m_cOfflineWorkers>0) {
      rResult << "\nGen.  PID[ID]       Mode      Status     Queries     Avg Time\n";
      VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
      while (pGeneration.isntNil ()) {
	size_t sStartingResultLength = rResult.length ();
        pGeneration->getWorkersStatistics (rResult);
        pGeneration.setTo (pGeneration->m_pSuccessor);
        if (sStartingResultLength < rResult.length ())
	    rResult << "\n";
      }
    }
    else
      rResult << "\nPool is currently empty with no workers";
}

void Vsa::VEvaluatorPool::getGenerationStatistics (Vca::U32 iID, VString &rResult) const {
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    bool bFound = false;
    while (pGeneration && !bFound) {
        if (pGeneration->generationId () == iID) {
            bFound = true;
            pGeneration->getDetailedStat (rResult);
            return;
        }
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
    rResult << "Generation " << iID << " is currently not in the Pool.";
}

void Vsa::VEvaluatorPool::dumpStats (VString &rResult) const {
    BaseClass::dumpStats (rResult);

    rResult << "OnlineWorkers | "           << onlineWorkerCount () << "\n";
    rResult << "OnlineAvailWorkers | "      << onlineAvailWorkers () << "\n";
    rResult << "OnlineWIPs | "              << onlineWIPs () << "\n";
    rResult << "OnlineExcessWorkers | "     << onlineExcessWorkers () << "\n";
    rResult << "RetiredWorkers | "          << retiredWorkers () << "\n";
    rResult << "OfflineWorkers | "          << offlineWorkerCount () << "\n";
    rResult << "OfflineAvailWorkers | "     << offlineAvailWorkers () << "\n";
    rResult << "OfflineWIPs | "             << offlineWIPs () << "\n";
    rResult << "OfflineExcessWorkers | "    << offlineExcessWorkers () << "\n";
    rResult << "TempWorkerMinimum | "       << m_cTempWorkerMinimum << "\n";
    rResult << "WorkersBeingCreated | "     << workersBeingCreated ();
}

void Vsa::VEvaluatorPool::dumpSettings (VString &rResult) const {
    m_pSettings->dumpSettings (rResult);
}

void Vsa::VEvaluatorPool::quickStatsHeader (VString &rResult){
    char iResult[144];
    sprintf(iResult,
            "%24s | %10s | %10s | %10s | %24s | %24s | %10s",
            "Start Date/Time",
            "Queries",
            "Workers",
            "Queue",
            "Last Query Date/Time",
            "Last Restart Date/Time",
            "Avg Time");
    rResult.setTo (iResult);
}

void Vsa::VEvaluatorPool::quickStats (VString &rResult) const {
    // Get started time, store in iStarted.
    char iTimestring[26];
    VString iStarted ("-");
    if (m_iStarted.ctime (iTimestring)) {
        iTimestring[24] = '\0'; // Clear last character, which ought to be '\n'.
        iStarted.setTo (iTimestring);

    }

    // Get last query time, store in iLastUsed.
    VString iLastUsed ("-");
    if (m_iLastUsed.ctime (iTimestring)) {
        iTimestring[24] = '\0'; // Clear last character, which ought to be '\n'.
        iLastUsed.setTo (iTimestring);
    }

    // Get last restart time, store in iLastRestarted.
    VString iLastRestarted ("-");
    if (m_iLastRestarted.ctime(iTimestring)) {
        iTimestring[24] = '\0'; // Clear last character, which ought to be '\n'.
        iLastRestarted.setTo (iTimestring);
    }

    // Get average query time, store in iAvgQueryTime.
    Vca::U64 iAvgQueryTime = 0;
    if (queriesProcessed () > 0)
        iAvgQueryTime = totalQueryTime ()/ queriesProcessed ();

    char iResult[144];
    sprintf (iResult,
             "%24s | %10d | %10d | %10d | %24s | %24s | %10llu",
             iStarted.content (),
             queriesProcessed (),
             onlineWorkerCount () + offlineWorkerCount (),
             queueLength (),
             iLastUsed.content (),
             iLastRestarted.content (),
             iAvgQueryTime);
    rResult.setTo (iResult);
}

void Vsa::VEvaluatorPool::dumpWorkerStatsHeader (VString &rResult) {
    Worker::quickStatsHeader(rResult);
}

void Vsa::VEvaluatorPool::dumpWorkerStats (VString &rResult) const {
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    while (pGeneration.isntNil ()) {
        pGeneration->dumpWorkerStats (rResult);
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
}

void Vsa::VEvaluatorPool::dumpGenerationStatsHeader (VString &rResult) {
    WorkerGeneration::quickStatsHeader(rResult);
}

void Vsa::VEvaluatorPool::dumpGenerationStats (VString &rResult) const {
    VReference<WorkerGeneration> pGeneration (m_pGenerationHead);
    while (pGeneration.isntNil ()) {
        pGeneration->quickStats (rResult);
        pGeneration.setTo (pGeneration->m_pSuccessor);
    }
}

/**
 * Checks pool assertions (structural/configuration invariants).
 *
 * @return number of assertions that failed.
 */
Vca::U32 Vsa::VEvaluatorPool::checkAssertions () const {
    Vca::U32 toReturn = 0;
    static bool errored = false;

    if (onlineWIPs () > workerMaximum ()) {
        log ("ASSERTION FAILED: WorkersInUse (%d) greater than pool's limit (%d)",
              onlineWIPs (), workerMaximum ()
        );
	notify (12, "#12: VEvaluatorPool-checkAssertions: WorkersInUse (%d) greater than pool's limit (%d)\n", 
		onlineWIPs (), workerMaximum ());
        toReturn++;
    }

    if (m_cWorkersBeingCreated > workersBeingCreatedMaximum ()) {
        log ("ASSERTION FAILED: WorkersBeingCreated (%d) greater than pool's limit (%d)",
              m_cWorkersBeingCreated, workersBeingCreatedMaximum ()
        );
	notify (13, "#13: VEvaluatorPool::checkAssertions: WorkersBeingCreated (%d) greater than pool's limit (%d)",
		m_cWorkersBeingCreated, workersBeingCreatedMaximum ());
        toReturn++;
    }
    //effective worker count may be negative if a starting worker is marked as excess ...
    int cEffectiveWorkers = m_cOnlineWorkers - m_cOnlineExcessWorkers;
    int sWorkerMaximum = workerMaximum ();
    if (cEffectiveWorkers > sWorkerMaximum) {
        log ("ASSERTION FAILED: Total Effective Workers (%d) in the pool greater than pool's limit (%d)",
            cEffectiveWorkers, sWorkerMaximum
        );
	notify (14, "#14: VEvaluatorPool::checkAssertions: Total Effective Workers (%d) in the pool greater than pool's limit (%d)",
		cEffectiveWorkers, sWorkerMaximum);
        toReturn++;
    }

    if (m_cBCEvaluationsInProgress > BCEvaluationsInProgressLimit ()) {
        log ("ASSERTION FAILED: Total Broadcast Evaluations in progress (%d) is greater than pool's limit (%d)",
            m_cBCEvaluationsInProgress, BCEvaluationsInProgressLimit ()
        );
	notify (14, "#14: VEvaluatorPool::checkAssertions: Total Broadcast Evaluations in progress (%d) is greater than pool's limit (%d)",
		m_cBCEvaluationsInProgress, BCEvaluationsInProgressLimit ());
        toReturn++;
    }

    if (errored && (toReturn == 0)) {
        log ("Assertion passed after previous failure.");
    }

    errored = (toReturn > 0);
    return toReturn;
}
