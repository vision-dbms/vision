/*****  VPoolAdmin main  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VPoolAdmin.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "VkStatus.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VDeviceFactory.h"

#include "Vsa_VPoolAdminSession.h"
#include "Vsa_VPoolAdminInterpreter.h"


/*****************************
 *****************************
 *****  Vsa::VPoolAdmin  *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolAdmin::VPoolAdmin (
    Context *pContext
) : BaseClass (pContext), m_iCommandCursor (this), m_pIAdmin (this) {
    traceInfo ("Creating Admin");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolAdmin::~VPoolAdmin () {
    traceInfo ("Destroying Admin");
}


/***********************
 ***********************
 *****  Operation  *****
 ***********************
 ***********************/
bool Vsa::VPoolAdmin::getDefaultServerName (VString &rServerName) const {
    CommandCursor iCommandCursor (this);
    rServerName.setTo (iCommandCursor.firstToken ());

    return rServerName.isntEmpty ();
}


bool Vsa::VPoolAdmin::start_() {
    if (!BaseClass::start_())
        return false;

    VString iServerName;
    if (getServerName (iServerName)) {
	incrementActivityCount ();
        processCommandLineArgs (iServerName);
    } else {
        display ("Invalid Server Address");

        displayUsagePatterns (
            "<connection-details> <pool-parameter-setting-options> <pool-control-options> <pool-query-options>",
            static_cast<char const*>(0)
        );
        displayOptions (
            "  Pool Connection Details:",
            "      -server = <poolServer> | -serverFile = <poolServer-file>",
            "      -interactive",
            static_cast<char const*>(0)
        );
        displayOptions (
            "  Pool Parameter Setting Options:",
            "      -workerStartupQuery    = <vision query>",
            "      -workerStartupFromFile = <vision script file>",
            "      -bcastInProgLmt= <count>",
            "      -evaluationTimeout = <timeout_secs>",
            "      -poolLogSwitchOn|Off",
            static_cast<char const*>(0)
        );
        displayOptions (
            "  Pool Control Options:",
            "      -restart",
            "      -hardRestart",
            "      -suspend",
            "      -resume",
            "      -stop",
            "      -hardStop",
            "      -flushWorkers",
            "      -broadcastQuery= <vision query> [-timeout=<seconds>]",
            "      -broadcastFile = <vision script file> [-timeout=<seconds>]",
            "      -takeWorkerOffline=<*workerId|processId>",
            "      -bringWorkerOnline=<*workerId|processId>",
            "      -retireOfflineWorker=<*workerId|processId>",
            "      [-timeout=<seconds>] -ping=<query> [-anyData]",
            static_cast<char const*>(0)
        );

        displayOptions (
            "  Pool Query Options:",
            "      -stat",
            "      -appSettings",
            "      -totalWorkers",
            "      -currentGenerationTotalWorkers",
            "      -isWorkerCreationFailureLmtReached",
            "      -maxWorkers",
            "      -minWorkers",
            "      -queryCount",
            "      -clientCount",
            "      -queueLength",
            static_cast<char const*>(0)
        );

        displayStandardOptions ();
        setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

/*******************************
 ****   Admin Operations    ****
 *******************************/

Vsa::VPoolAdminSession* Vsa::VPoolAdmin::Open (
    VString const &rAddress, VString const &rSession
)  {
    Session::Reference pSession (new Session (rAddress, rSession, this));
    if (pSession->isntFailed ()) {
        if (!insertSession (pSession))
            display ("A session with the above name already exists....\n");
        return pSession;
    }
    return 0;
}

/********************
 ****   Control  ****
 ********************/

void Vsa::VPoolAdmin::Stop (VString const &rSession) {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        pSession->Stop ();
	//        m_iSessionTable.Delete (rSession.content ());
    }
}
void Vsa::VPoolAdmin::Suspend (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->Suspend ();
}

void Vsa::VPoolAdmin::Resume (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->Resume ();
}

void Vsa::VPoolAdmin::Restart (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->Restart ();
}

void Vsa::VPoolAdmin::HardRestart (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->HardRestart ();
}

void Vsa::VPoolAdmin::FlushWorkers (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->FlushWorkers ();
}

void Vsa::VPoolAdmin::HardStop (VString const &rSession) {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        pSession->HardStop ();
	//        m_iSessionTable.Delete (rSession.content ());
    }
}
/******************************
 ****   Parameter Setting  ****
 ******************************/

void Vsa::VPoolAdmin::SetWorkerMinimum (
    Vca::U32 workerCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerMinimum (workerCount);
}

void Vsa::VPoolAdmin::SetWorkerMaximum (
    Vca::U32 workerCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerMaximum (workerCount);
}

void Vsa::VPoolAdmin::SetWorkerMinimumAvailable (
    Vca::U32 workerCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerMinimumAvailable (workerCount);
}

void Vsa::VPoolAdmin::SetWorkerMaximumAvailable (
    Vca::U32 workerCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerMaximumAvailable (workerCount);
}

void Vsa::VPoolAdmin::SetWorkersBeingCreated (
    Vca::U32 workerCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkersBeingCreatedMaximum (workerCount);
}

void Vsa::VPoolAdmin::SetWorkerStartupQuery (
    VString const &rQuery, VString const &rSession
) const{
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerStartupQuery (rQuery);
}

void  Vsa::VPoolAdmin::SetWorkerStartupFromFile  (
    VString const &rFile, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerStartupFile (rFile);
}

void  Vsa::VPoolAdmin::SetWorkerPIDQueryFromFile  (
    VString const &rFile, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerPIDQueryFromFile (rFile);
}

void Vsa::VPoolAdmin::SetWorkerCreationFailureHardLimit (
    Vca::U32 count, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerCreationFailureHardLimit (count);
}

void Vsa::VPoolAdmin::SetWorkerCreationFailureSoftLimit (
    Vca::U32 count, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerCreationFailureSoftLimit (count);
}

void Vsa::VPoolAdmin::SetWorkerProcessSource (
    VString const &source, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerProcessSource (source);
}

void Vsa::VPoolAdmin::SetGenerationMaximum (
    Vca::U32 count, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetGenerationMaximum (count);
}

void Vsa::VPoolAdmin::GetAppSettings  (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->GetAppSettings ();
}

void Vsa::VPoolAdmin::SetLogFilePath (
    VString const &rPath, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetLogFilePath (rPath);
}

void Vsa::VPoolAdmin::SetLogSwitch  (
    bool bTraceSwitch, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetLogSwitch (bTraceSwitch);
}

void  Vsa::VPoolAdmin::SetLogFileSize (
    Vca::U32 iSize,  VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetLogFileSize (iSize);
}

void  Vsa::VPoolAdmin::SetLogFileBackups (
    Vca::U32 iCount, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetLogFileBackups (iCount);
}

void  Vsa::VPoolAdmin::SetEvaluationAttempts  (
    Vca::U32 cEvaluationAttempt, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetEvaluationAttempts (cEvaluationAttempt);
}

void  Vsa::VPoolAdmin::SetShrinkTimeOut  (
    Vca::U32 iTimeOutMinutes, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetShrinkTimeOut (iTimeOutMinutes);
}

void  Vsa::VPoolAdmin::SetStopTimeOut  (
    Vca::U32 iTimeOutMins, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetStopTimeOut (iTimeOutMins);
}

void  Vsa::VPoolAdmin::SetBroadcastInProgressLimit  (
    Vca::U32 iLimit, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetBroadcastInProgressLimit (iLimit);
}

void  Vsa::VPoolAdmin::SetEvaluationTimeout  (
    Vca::U64 sTimeout, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
                pSession->SetEvaluationTimeout (sTimeout);
}
void  Vsa::VPoolAdmin::SetWorkerQueryHistoryLimit  (
    Vca::U32 iLimit, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->SetWorkerQueryHistoryLimit (iLimit);
}

void Vsa::VPoolAdmin::TakeWorkerOffline (VString const& iWorkerId, VString const &rSession) const {
    VString ID (iWorkerId);
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        if (*iWorkerId.content () == '*') {
            bIsPID = false;
            ID.setTo (ID.content ()+1);
        }
        pSession->TakeWorkerOffline (ID, bIsPID);
    }
}

void Vsa::VPoolAdmin::MakeWorkerOnline (VString const & iWorkerId, VString const &rSession) const {
    VString ID (iWorkerId);
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        if (*iWorkerId.content () == '*') {
            bIsPID = false;
            ID.setTo (ID.content ()+1);
        }
        pSession->MakeWorkerOnline (ID, bIsPID);
    }
}

void Vsa::VPoolAdmin::EvaluateUsingOfflineWorker (
     VString const &rQueryFile, VString const& iWorkerId, VString const &rSession
) const {
    VString ID (iWorkerId);
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        if (*iWorkerId.content () == '*') {
            bIsPID = false;
            ID.setTo (ID.content ()+1);
        }
        pSession->EvaluateUsingOfflineWorker (rQueryFile, ID, bIsPID);
    }
}

void Vsa::VPoolAdmin::RetireOfflineWorker (VString const & iWorkerId, VString const &rSession) const {
    VString ID (iWorkerId);
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        if (*iWorkerId.content () == '*') {
            bIsPID = false;
            ID.setTo (ID.content ()+1);
        }
        pSession->RetireOfflineWorker (ID, bIsPID);
    }
}

void Vsa::VPoolAdmin::CancelQuery (Vca::U32 iID, VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->CancelQuery (iID);
}

void Vsa::VPoolAdmin::DumpWorker (VString const &iWorkerId, VString const &rFile, VString const &rSession) const {
    VString ID (iWorkerId);
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        if (*iWorkerId.content () == '*') {
            bIsPID = false;
            ID.setTo (ID.content ()+1);
        }
        pSession->DumpWorker (ID, bIsPID, rFile);
    }
}

void Vsa::VPoolAdmin::DumpWorkingWorkers (VString const &rFile, VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->DumpWorkingWorkers (rFile);
}

void Vsa::VPoolAdmin::DumpPoolQueue (VString const &rFile, Vca::U32 cQueries, VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->DumpPoolQueue (rFile, cQueries);
}

/*****************
 ****   Ping  ****
 *****************/

void  Vsa::VPoolAdmin::PingSession  (
    VString const &rQuery, bool bAnyData, Vca::U64 iTimeOutSeconds, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->PingSession (rQuery, bAnyData, iTimeOutSeconds);
}

/**********************
 ****   Broadcast  ****
 **********************/

void Vsa::VPoolAdmin::Broadcast (VString const &rQuery, Vca::U64 iTimeOutSeconds,
                                 VString const &rSession
) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
      pSession->Broadcast (rQuery, iTimeOutSeconds);
}

void Vsa::VPoolAdmin::BroadcastFile (VString const &rFile, Vca::U64 iTimeOutSeconds,
                                 VString const &rSession
) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
      pSession->BroadcastFile (rFile, iTimeOutSeconds);
}

/***********************
 ****   Statistics  ****
 ***********************/

void Vsa::VPoolAdmin::ServiceStatistics (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->ServiceStatistics ();
}

void Vsa::VPoolAdmin::QueryDetails (Vca::U32 iID, VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->QueryDetails (iID);
}

void Vsa::VPoolAdmin::StatisticsDump (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->StatisticsDump ();
}

void Vsa::VPoolAdmin::QuickStatistics (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        pSession->QuickStatDumpHeader ();
        pSession->QuickStatDump ();
    }
}

void Vsa::VPoolAdmin::WorkerStatistics (
    VString const &rID, VString const &rSession
) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        bool bIsPID = true;
        VString iID (rID);

        if (*rID.content () == '*') {
          bIsPID = false;
          iID.setTo (rID.content ()+1);
        }
        pSession->WorkerStatistics (iID, bIsPID);
    }
}

void Vsa::VPoolAdmin::WorkersStatistics (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->WorkersStatistics ();
}

void Vsa::VPoolAdmin::WorkerStatDump (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        pSession->WorkerStatDumpHeader ();
        pSession->WorkerStatDump ();
    }
}

void Vsa::VPoolAdmin::GenerationStatistics (Vca::U32 iID, VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession))
        pSession->GenerationStatistics (iID);
}

void Vsa::VPoolAdmin::GenerationStatDump (VString const &rSession) const {
    Session::Reference pSession;
    if (getSession (rSession, pSession)) {
        pSession->GenerationStatDumpHeader ();
        pSession->GenerationStatDump ();
    }
}

/******************************
 ****   Session Helpers  ****
 ******************************/

void Vsa::VPoolAdmin::GetTotalWorkers (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetTotalWorkers ();
}

void Vsa::VPoolAdmin::IsWorkerCreationFailureLmtReached (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->IsWorkerCreationFailureLmtReached ();
}

void Vsa::VPoolAdmin::GetCurrentGenerationTotalWorkers (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetCurrentGenerationTotalWorkers ();
}

void Vsa::VPoolAdmin::GetMaxWorkers (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetMaxWorkers ();
}

void Vsa::VPoolAdmin::GetMinWorkers (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetMinWorkers ();
}

void Vsa::VPoolAdmin::GetQueryCount (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetQueryCount ();
}

void Vsa::VPoolAdmin::GetQueueLength (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetQueueLength ();
}

void Vsa::VPoolAdmin::GetClientCount (VString const &rSession) const {
  Session::Reference pSession;
  if (getSession (rSession, pSession))
    pSession->GetClientCount ();
}

/****************************
 ****   Session Helpers  ****
 ****************************/

bool Vsa::VPoolAdmin::insertSession (Session *pSession) {
    unsigned int xIndex;
    if (m_iSessionTable.Insert (pSession->name (), xIndex)) {
        m_iSessionTable[xIndex] = pSession;
        return true;
    }
    return false;
}

bool Vsa::VPoolAdmin::removeSession (Session *pSession) {
    if (m_iSessionTable.Contains (pSession->name ())) {
        m_iSessionTable.Delete (pSession->name ());
        return true;
    }
    return false;
}

bool Vsa::VPoolAdmin::getSession (
    VString const &rSession, Session::Reference &rpSession
) const {
    unsigned int xInstance;
    if (m_iSessionTable.Locate (rSession.content (), xInstance)) {
         rpSession = m_iSessionTable[xInstance];
         return true;
    }
    displayResult ("\nError: Invalid Session Name");
    rpSession.clear ();
    return false;
}


/*********************
 *****  Utility  *****
 *********************/
bool Vsa::VPoolAdmin::g_bInteractive = false;

void Vsa::VPoolAdmin::displayResult (VString const &rResult) {
    display ("%s", rResult.content ());
    if (g_bInteractive) Vsa::VPoolAdminInterpreter::displayPrompt ();
    else display ("\n");
    fflush (stdout);
}

/*******************************
 *****  Command Line Args  *****
 *******************************/

void Vsa::VPoolAdmin::processCommandLineArgs (VString const &rServerName) {
    if (rServerName.isntEmpty ()) {
        Session *pSession = Open (rServerName, "default");

        if (pSession) {

            Vca::U32 iTimeout = UINT_MAX;
            bool bAnyData = false;

            //  iterate through the command line value options
            while (char const *pTag = m_iCommandCursor.nextTag ()) {

                char const *pValue = strchr (pTag, '=');
                //    handle tags without values
                if (!pValue) {
                    if (!strcasecmp (pTag, "restart"))
                        Restart ();
                    else if (!strcasecmp (pTag, "hardRestart"))
                        HardRestart ();
                    else if (!strcasecmp (pTag, "suspend"))
                        Suspend ();
                    else if (!strcasecmp (pTag, "resume"))
                        Resume ();
                    else if (strstr (pTag, "stat") == pTag)
                        ServiceStatistics ();
                    else if (!strcasecmp (pTag, "appSettings"))
                        GetAppSettings ();
                    else if (!strcasecmp (pTag, "stop"))
                        Stop ();
                    else if (!strcasecmp (pTag, "hardStop"))
                        HardStop ();
                    else if (!strcasecmp (pTag, "poolLogSwitchOn"))
                        SetLogSwitch (true);
                    else if (!strcasecmp (pTag, "poolLogSwitchOff"))
                        SetLogSwitch (false);
                    else if (!strcasecmp (pTag, "flushWorkers"))
                        FlushWorkers ();
                    else if (!strcasecmp (pTag, "totalWorkers"))
                        GetTotalWorkers ();
                    else if (!strcasecmp (pTag, "isWorkerCreationFailureLmtReached"))
                        IsWorkerCreationFailureLmtReached ();
                    else if (!strcasecmp (pTag, "currentGenerationTotalWorkers"))
                        GetCurrentGenerationTotalWorkers ();
                    else if (!strcasecmp (pTag, "maxWorkers"))
                        GetMaxWorkers ();
                    else if (!strcasecmp (pTag, "minWorkers"))
                        GetMinWorkers ();
                    else if (!strcasecmp (pTag, "queryCount"))
                        GetQueryCount ();
                    else if (!strcasecmp (pTag, "clientCount"))
                        GetClientCount ();
                    else if (!strcasecmp (pTag, "queueLength"))
                        GetQueueLength ();
                    else if (!strcasecmp (pTag, "anyData"))
                        bAnyData = true;
                    else if (!strcasecmp (pTag, "interactive"))
                    //  ignore interactive option as it should be processed at the end
                        g_bInteractive = true;
                    else if (!strcasecmp (pTag, "directory")) {
                    //  ... option processed by base class, ignore.
                    } else {
                        VString iInfo; iInfo << "Invalid Switch: " << pTag;
                        displayResult (iInfo);
                    }
                }

                //  handle tags with values
                else {
                    Vca::U32 cTagName=pValue-pTag;

                    if (!strncasecmp (pTag, "timeout", cTagName)) {
                        iTimeout = atol (pValue+1);
                    }
                    else if (!strncasecmp (pTag, "maxWorkers", cTagName)) {
                        SetWorkerMaximum (atol (pValue+1));
                    }
                    else if (!strncasecmp (pTag, "minWorkers", cTagName)) {
                        SetWorkerMinimum (atol (pValue+1));
                    }
                    else if (!strncasecmp (pTag, "maxGenerations", cTagName)) {
                        SetGenerationMaximum (atol (pValue+1));
                    }
                    else if (!strncasecmp (pTag, "workerStartupQuery", cTagName)) {
                        SetWorkerStartupQuery (pValue+1);
                    }
                    else if (!strncasecmp (pTag, "workerStartupFromFile", cTagName)) {
                        SetWorkerStartupFromFile (pValue+1);
                    }
                    else if (!strncasecmp (pTag, "ping", cTagName)) {
                        PingSession (pValue+1, bAnyData, iTimeout);
                    }
                    else if (!strncasecmp (pTag, "broadcastQuery", cTagName)) {
                        Broadcast (pValue+1, iTimeout);
                    }
                    else if (!strncasecmp (pTag, "broadcastFile", cTagName)) {
                        BroadcastFile (pValue+1, iTimeout);
                    }
                    else if (!strncasecmp (pTag, "bcastInProgLmt", cTagName)) {
                        SetBroadcastInProgressLimit (atol (pValue+1));
                    }
                    else if (!strncasecmp (pTag, "evaluationTimeout", cTagName)) {
                        SetEvaluationTimeout (atoll (pValue+1));
                    }
                    else if (!strncasecmp (pTag, "takeWorkerOffline", cTagName)){
                        TakeWorkerOffline (pValue+1);
                    }
                    else if (!strncasecmp (pTag, "bringWorkerOnline", cTagName)) {
                        MakeWorkerOnline (pValue+1);
                    }
                    else if (!strncasecmp (pTag, "retireOfflineWorker", cTagName)) {
                        RetireOfflineWorker (pValue+1);
                    }
                    else if (strstr (pTag, "server")) {
                    //  ... option processed by base class, ignore.
                    } else {
                        VString iInfo; iInfo << "Invalid Value Tag: " << pTag;
                        displayResult (iInfo);
                    }
              }
            }
            //  process non-interactive mode...
            if (!commandSwitchValue ("interactive"))
                 pSession->Shutdown ();
        }
    }

    VkStatus iStatus; Vca::VBSProducer::Reference pBSToHere;
    if (commandSwitchValue ("interactive") && Vca::VDeviceFactory::SupplyStdToHere (iStatus, pBSToHere)) {
        VPoolAdminInterpreter::Reference pInterpreter (
            new VPoolAdminInterpreter (this, pBSToHere)
        );
    }
}

/******************************
 *****  Display Sessions  *****
 ******************************/

void Vsa::VPoolAdmin::displaySessions () {
    Iterator iterator (m_iSessionTable);
    display ("\nOpen Sessions:\n");
    while (iterator.isNotAtEnd ()) {
        Session::Reference pSession;
        VString session = iterator.key ();
        Vca::U32 xInstance;
        m_iSessionTable.Locate (session.content (), xInstance);
        pSession = m_iSessionTable [xInstance];
        display ("\nSession Name: %s\nAddress %s", session.content (), pSession->address ());
        ++iterator;
    }
    display ("\n");
}


/********************
 ********************
 ****  Shutdown  ****
 ********************
 ********************/

bool Vsa::VPoolAdmin::stop_(bool bHardStop) {
    if (!BaseClass::stop_(bHardStop))
        return false;
    Iterator iterator (m_iSessionTable);
    for (; iterator.isNotAtEnd (); ++iterator) {
        Session *pSession = iterator.value ();
        pSession->clear ();
        m_iSessionTable.Delete (pSession->name ());
    }
    return isStopping (bHardStop);
}

/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vsa::VPoolAdmin> iMain (argc, argv);
    return iMain.processEvents ();
}
