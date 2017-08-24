/*****  Vsa_VPoolAdminInterpreter Implementation  *****/

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

#include "Vsa_VPoolAdminInterpreter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_VPoolAdmin.h"
#include "Vca.h"


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

char const *Vsa::VPoolAdminInterpreter::g_pCommandUsage[] = {
    "open <n/w address> - Opens an admin session with the pool",
    "stop               - Stops the pool after all clients have disconnected",
    "hardStop           - Stops the pool after finishing already running queries",
    "suspend            - Suspends the pool",
    "resume             - Resumes the pool",
    "restart            - Restarts the pool",
    "hardRestart        - HardRestarts (suspend,flush,restart) the pool",
    "flushWorkers       - Flushes all the online workers",
    "statistics         - Gets the pool statistics",
    "stat               - Gets the pool statistics",
    "statisticsDump     - Dumps pool statistics in machine-readable format",
    "quickStat          - Dumps pool statistics in one-line machine-readable format",
    "queryDetails       <id> - Dumps queued request in machine-readable format",
    "workers            - Lists all workers in the pool",
    "workerStat         <pid|*id> - Gets a worker statistics",
    "workerStatDump     - Dumps worker statistics in a machine-readable format.",
    "generationStat     <id> - Gets generation statistics for a given generation",
    "generationStatDump - Dumps all generation statistics in a machine-readable format.",
    "setWorkerMinimum   <count>   - Sets the worker minimum",
    "setWorkerMaximum   <count>   - Sets the worker maxumum",
    "setWorkerMinimumAvailable <count> - Sets worker minimum available",
    "setWorkerMaximumAvailable <count> - Sets worker maximum available",
    "setWorkersBeingCreated <count> - Sets workers being created limit",
    "setWorkerCreationFailureHardLimit <count> - Sets worker creation failure HARD limit",
    "setWorkerCreationFailureSoftLimit <count> - Sets worker creation failure SOFT limit",
    "setWorkerProcessSource <workerSource> - Sets worker process source string",
    "setGenerationMaximum <count> - Sets the maximum number of old generations to keep data on,",
    "appSettings        - Gets the application settings",
    "setLogFilePath     <path>  - Sets pool debug log path",
    "setLogSwitchOn     - Turns pool debug log on",
    "setLogSwitchOff    - Turns pool debug log off",
    "setLogFileSize     <size>  - Sets the log file size (Megabytes)",
    "setLogFileBackups  <count> - Sets the log file backup count",
    "setEvaluationTimeout <timeout_secs> - Sets the Pool's Evaluation Timeout value (secs)",
	"setEvaluationAttempts <count> - Sets the maximum evaluation attempt count",
    "setEvaluationOnErrorAttempts <count> - Sets the maximum evaluation attempt count when error occurs",
    "setEvaluationTimeOutAttempts <count> - Sets the maximum evaluation attempt count when time out occurs",
    "setShrinkTimeOut   <timeout> - Sets shrink timeout for new workers (Minutes)",
    "setStopTimeOut     <timeout> - Sets stop timeout for the pool application(Mins)",
    "setWorkerStartupFromFile <vision file> - Sets worker startup query file",
    "setWorkerPIDQueryFromFile <vision file> - Sets worker pid query file",
    "broadcastFile      <File> [timeout] - Broadcasts file to current online workers",
    "setBroadcastInProgressLimit <lmt>- Sets broadcast evaluations inprogress lmt",
    "setWorkerQueryHistoryLimit <count> - Sets Worker Query History limit",
    "takeWorkerOffline  <pid|*id> - Takes an online available worker, offline",
    "bringWorkerOnline  <pid|*id> - Brings an offline available worker, online",
    "evaluateUsingOfflineWorker <File><pid|*id>-Evaluate file with offline worker",
    "retireOfflineWorker <pid|*id> - Retires an offline availableworker",
    "cancelQuery        <id> - Cancels a queued query",
    "dumpWorker         <pid|*id> <filepath> - Dumps the worker query history to file",
    "dumpWorkingWorkers <filepath> - Dumps the InUse workers info to file",
    "dumpPoolQueue      <filepath> [numQueries] - Dumps the pool query queue to file",
    "displaySessions    - Display all open sessions",
    "help               - VcaAdmin interpreter commands",
    "exit               - Terminates this admin program",
    0
};

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPoolAdminInterpreter::VPoolAdminInterpreter (
    VPoolAdmin *pAdmin, Vca::VBSProducer *pBSToHere
) : BaseClass (pBSToHere), m_pAdmin (pAdmin) {
    displayPrompt ();
    start ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolAdminInterpreter::~VPoolAdminInterpreter () {
}


/*****************************
 *****************************
 ***** Command Processor *****
 *****************************
 *****************************/

#define MAX_ARG_SIZE 6

bool Vsa::VPoolAdminInterpreter::processCommand (char *pFullCommand) {
    static char const *const commandList[] = {
           "open",
           "stop",
           "hardStop",
           "suspend",
           "resume",
           "restart",
           "hardRestart",
           "flushWorkers",
           "statistics",
           "stat",
           "statisticsDump",
           "quickStat",
           "queryDetails",
           "workers",
           "workerStat",
           "workerStatDump",
           "generationStat",
           "generationStatDump",
           "setWorkerMinimum",
           "setWorkerMaximum",
           "setWorkerMinimumAvailable",
           "setWorkerMaximumAvailable",
           "setWorkersBeingCreated",
           "setWorkerCreationFailureHardLimit",
           "setWorkerCreationFailureSoftLimit",
           "setWorkerProcessSource",
           "setGenerationMaximum",
           "appSettings",
           "setLogFilePath",
           "setLogSwitchOn",
           "setLogSwitchOff",
           "setLogFileSize",
           "setLogFileBackups",
           "setEvaluationTimeout",
		   "setEvaluationAttempts",
           "setEvaluationOnErrorAttempts",
		   "setEvaluationTimeOutAttempts",
           "setShrinkTimeOut",
           "setStopTimeOut",
           "setWorkerStartupFromFile",
           "setWorkerPIDQueryFromFile",
           "broadcastFile",
           "setBroadcastInProgressLimit",
           "setWorkerQueryHistoryLimit",
           "takeWorkerOffline",
           "bringWorkerOnline",
           "evaluateUsingOfflineWorker",
           "retireOfflineWorker",
           "cancelQuery",
           "dumpWorker",
           "dumpWorkingWorkers",
           "dumpPoolQueue",
           "displaySessions",
           "help",
           "exit",
    };

    static Command const enumCommand[] = {
            COpen,
            CStop,
            CHardStop,
            CSuspend,
            CResume,
            CRestart,
            CHardRestart,
            CFlushWorkers,
            CServiceStatistics,
            CStatistics,
            CStatisticsDump,
            CQuickStatistics,
            CQueryDetails,
            CWorkersStat,
            CWorkerStat,
            CWorkerStatDump,
            CGenerationStat,
            CGenerationStatDump,
            CSetWorkerMinimum,
            CSetWorkerMaximum,
            CSetWorkerMinimumAvailable,
            CSetWorkerMaximumAvailable,
            CSetWorkersBeingCreated,
            CSetWorkerCreationFailureHardLimit,
            CSetWorkerCreationFailureSoftLimit,
            CSetWorkerProcessSource,
            CSetGenerationMaximum,
            CAppSettings,
            CSetLogFilePath,
            CSetLogSwitchOn,
            CSetLogSwitchOff,
            CSetLogFileSize,
            CSetLogFileBackups,
            CSetEvaluationTimeout,
			CSetEvaluationAttempts,
            CSetEvaluationOnErrorAttempts,
			CSetEvaluationTimeOutAttempts,
            CSetShrinkTimeOut,
            CSetStopTimeOut,
            CSetWorkerStartupFromFile,
            CSetWorkerPIDQueryFromFile,
            CBroadcastFile,
            CSetBroadcastInProgressLimit,
            CSetWorkerQueryHistoryLimit,
            CTakeWorkerOffline,
            CMakeWorkerOnline,
            CEvaluateUsingOfflineWorker,
            CRetireOfflineWorker,
            CCancelQuery,
            CDumpWorker,
            CDumpWorkingWorkers,
            CDumpPoolQueue,
            CDisplaySessions,
            CHelp,
            CExit
    };

    bool displayingPrompt = true;
    bool notDone = true;

    if(strcmp(pFullCommand,"")) {
        char *pArgs[MAX_ARG_SIZE], *pInterpreterCommand = strtok(pFullCommand," ");

        unsigned int iArgCount = 0;
        while (iArgCount < MAX_ARG_SIZE && (pArgs[iArgCount] = strtok(NULL, " ")))
            iArgCount++;

        Command command = CInvalid;
        for(unsigned int i=0; i < CommandCount (); i++) {
            if(strcmp(commandList[i], pFullCommand)==0)
                command = enumCommand[i];
        }

        switch(command) {
        case COpen:
            if(iArgCount == 2)
                m_pAdmin->Open (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->Open (pArgs[0]);
            else
                displayCommand (COpen);
            break;

        case CStop:
            if(iArgCount == 1)
                m_pAdmin->Stop (pArgs[0]);
            else
                m_pAdmin->Stop ();
            break;

        case CHardStop:
            if (iArgCount == 1)
                m_pAdmin->HardStop (pArgs[0]);
            else
                m_pAdmin->HardStop ();
            break;

        case CSuspend:
            if (iArgCount == 1)
                m_pAdmin->Suspend (pArgs[0]);
            else
                m_pAdmin->Suspend ();
            break;

        case CResume:
            if (iArgCount == 1)
                m_pAdmin->Resume (pArgs[0]);
            else
                m_pAdmin->Resume ();
            break;

        case CRestart:
            if (iArgCount == 1)
                m_pAdmin->Restart (pArgs[0]);
            else
                m_pAdmin->Restart ();
            break;

        case CHardRestart:
            if (iArgCount == 1)
                m_pAdmin->HardRestart (pArgs[0]);
            else
                m_pAdmin->HardRestart ();
            break;

        case CFlushWorkers:
            if (iArgCount == 1)
                m_pAdmin->FlushWorkers (pArgs[0]);
            else
                m_pAdmin->FlushWorkers ();
            break;

        case CServiceStatistics:
        case CStatistics:
            if (iArgCount == 1)
                m_pAdmin->ServiceStatistics (pArgs[0]);
            else
                m_pAdmin->ServiceStatistics ();
            break;

        case CStatisticsDump:
            if (iArgCount == 1)
                m_pAdmin->StatisticsDump (pArgs[0]);
            else
                m_pAdmin->StatisticsDump ();
            break;

        case CQuickStatistics:
            if (iArgCount == 1)
                m_pAdmin->QuickStatistics (pArgs[0]);
            else
                m_pAdmin->QuickStatistics ();
            break;

        case CQueryDetails:
            if (iArgCount == 2)
                m_pAdmin->QueryDetails (atoi(pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->QueryDetails (atoi(pArgs[0]));
            else
                displayCommand (CQueryDetails);
            break;

        case CWorkerStat:
            if (iArgCount == 2)
                m_pAdmin->WorkerStatistics (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->WorkerStatistics (pArgs[0]);
            else
                displayCommand (CWorkerStat);
            break;

        case CWorkersStat:
            if (iArgCount == 1)
                m_pAdmin->WorkersStatistics (pArgs[0]);
            else
                m_pAdmin->WorkersStatistics ();
            break;

        case CWorkerStatDump:
            if (iArgCount == 1)
                m_pAdmin->WorkerStatDump (pArgs[0]);
            else
                m_pAdmin->WorkerStatDump ();
            break;

        case CGenerationStat:
            if (iArgCount == 2)
                m_pAdmin->GenerationStatistics (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->GenerationStatistics (atoi (pArgs[0]));
            else
                displayCommand (CGenerationStat);
            break;

        case CGenerationStatDump:
            if (iArgCount == 1)
                m_pAdmin->GenerationStatDump (pArgs[0]);
            else
                m_pAdmin->GenerationStatDump ();
            break;

        case CSetWorkerMinimum:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerMinimum (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerMinimum (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerMinimum);
            break;

        case CSetWorkerMaximum:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerMaximum (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerMaximum (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerMaximum);
            break;

        case CSetWorkerMinimumAvailable:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerMinimumAvailable (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerMinimumAvailable (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerMinimumAvailable);
            break;

        case CSetWorkerMaximumAvailable:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerMaximumAvailable (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerMaximumAvailable (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerMaximumAvailable);
            break;

        case CSetWorkersBeingCreated:
            if (iArgCount == 2)
                m_pAdmin->SetWorkersBeingCreated (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkersBeingCreated (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkersBeingCreated);
            break;

        case CSetWorkerCreationFailureHardLimit:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerCreationFailureHardLimit (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerCreationFailureHardLimit (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerCreationFailureHardLimit);
            break;

        case CSetWorkerCreationFailureSoftLimit:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerCreationFailureSoftLimit (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerCreationFailureSoftLimit (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerCreationFailureSoftLimit);
            break;

        case CSetWorkerProcessSource:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerProcessSource (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerProcessSource (pArgs[0]);
            else
                displayCommand (CSetWorkerProcessSource);
            break;

        case CSetGenerationMaximum:
            if (iArgCount == 2)
                m_pAdmin->SetGenerationMaximum (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetGenerationMaximum (atoi (pArgs[0]));
            else
                displayCommand (CSetGenerationMaximum);
            break;

        case CAppSettings:
            if (iArgCount == 1)
                m_pAdmin->GetAppSettings (pArgs[0]);
            else
                m_pAdmin->GetAppSettings ();
            break;

        case CSetLogFilePath:
            if (iArgCount == 2)
                m_pAdmin->SetLogFilePath (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetLogFilePath (pArgs[0]);
            else
                displayCommand (CSetLogFilePath);
            break;

        case CSetLogSwitchOn:
            if (iArgCount == 1)
                m_pAdmin->SetLogSwitch (true, pArgs[0]);
            else
                m_pAdmin->SetLogSwitch (true);
            break;

        case CSetLogSwitchOff:
            if (iArgCount == 1)
                m_pAdmin->SetLogSwitch (false, pArgs[0]);
            else
                m_pAdmin->SetLogSwitch (false);
            break;

        case CSetLogFileSize:
            if (iArgCount == 2)
                m_pAdmin->SetLogFileSize (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetLogFileSize (atoi (pArgs[0]));
            else
                displayCommand (CSetLogFileSize);
            break;

        case CSetLogFileBackups:
            if (iArgCount == 2)
                m_pAdmin->SetLogFileBackups (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetLogFileBackups (atoi (pArgs[0]));
            else
                displayCommand (CSetLogFileBackups);
            break;

        case CSetEvaluationTimeout:
            if (iArgCount == 2)
                m_pAdmin->SetEvaluationTimeout (atoll (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetEvaluationTimeout (atoll (pArgs[0]));
            else
                displayCommand (CSetEvaluationTimeout);
            break;
		
		case CSetEvaluationAttempts:
            if (iArgCount == 2)
                m_pAdmin->SetEvaluationAttempts (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetEvaluationAttempts (atoi (pArgs[0]));
            else
                displayCommand (CSetEvaluationAttempts);
            break;
			
        case CSetEvaluationOnErrorAttempts:
            if (iArgCount == 2)
                m_pAdmin->SetEvaluationOnErrorAttempts (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetEvaluationOnErrorAttempts (atoi (pArgs[0]));
            else
                displayCommand (CSetEvaluationOnErrorAttempts);
            break;

        case CSetEvaluationTimeOutAttempts:
            if (iArgCount == 2)
                m_pAdmin->SetEvaluationTimeOutAttempts (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetEvaluationTimeOutAttempts (atoi (pArgs[0]));
            else
                displayCommand (CSetEvaluationTimeOutAttempts);
            break;

        case CDisplaySessions:
            m_pAdmin->displaySessions ();
            break;

        case CSetShrinkTimeOut:
            if (iArgCount == 2)
                m_pAdmin->SetShrinkTimeOut (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetShrinkTimeOut (atoi (pArgs[0]));
            else
                displayCommand (CSetShrinkTimeOut);
            break;

        case CSetWorkerStartupFromFile:
            if (iArgCount == 2)
                m_pAdmin-> SetWorkerStartupFromFile (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerStartupFromFile (pArgs[0]);
            else
                displayCommand (CSetWorkerStartupFromFile);
            break;

        case CSetWorkerPIDQueryFromFile:
            if (iArgCount == 2)
                m_pAdmin-> SetWorkerPIDQueryFromFile (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerPIDQueryFromFile (pArgs[0]);
            else
                displayCommand (CSetWorkerPIDQueryFromFile);
            break;

        case CBroadcastFile:
            if (iArgCount == 3)
                m_pAdmin->BroadcastFile (pArgs[0], atoi (pArgs[1]), pArgs[2]);
            else if (iArgCount == 2)
                m_pAdmin->BroadcastFile (pArgs[0], atoi (pArgs[1]));
            else if (iArgCount == 1)
                m_pAdmin->BroadcastFile (pArgs[0]);
            else
                displayCommand (CBroadcastFile);
            break;


        case CSetBroadcastInProgressLimit:
            if (iArgCount == 2)
                m_pAdmin->SetBroadcastInProgressLimit (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetBroadcastInProgressLimit (atoi (pArgs[0]));
            else
                displayCommand (CSetBroadcastInProgressLimit);
            break;

        case CSetWorkerQueryHistoryLimit:
            if (iArgCount == 2)
                m_pAdmin->SetWorkerQueryHistoryLimit (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetWorkerQueryHistoryLimit (atoi (pArgs[0]));
            else
                displayCommand (CSetWorkerQueryHistoryLimit);
            break;

        case CSetStopTimeOut:
            if (iArgCount == 2)
                m_pAdmin->SetStopTimeOut (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->SetStopTimeOut (atoi (pArgs[0]));
            else
                displayCommand (CSetStopTimeOut);
            break;

        case CTakeWorkerOffline:
            if (iArgCount == 2)
                m_pAdmin->TakeWorkerOffline (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->TakeWorkerOffline (pArgs[0]);
            else
                displayCommand (CTakeWorkerOffline);
            break;

        case CMakeWorkerOnline:
            if (iArgCount == 2)
                m_pAdmin->MakeWorkerOnline (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->MakeWorkerOnline (pArgs[0]);
            else
                displayCommand (CMakeWorkerOnline);
            break;

        case CEvaluateUsingOfflineWorker:
            if (iArgCount == 3)
                m_pAdmin->EvaluateUsingOfflineWorker (pArgs[0], pArgs[1], pArgs[2]);
            else if (iArgCount == 2)
                m_pAdmin->EvaluateUsingOfflineWorker (pArgs[0], pArgs[1]);
            else
                displayCommand (CEvaluateUsingOfflineWorker);
            break;

        case CRetireOfflineWorker:
            if (iArgCount == 2)
                m_pAdmin->RetireOfflineWorker (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->RetireOfflineWorker (pArgs[0]);
            else
                displayCommand (CRetireOfflineWorker);
            break;

        case CCancelQuery:
            if (iArgCount == 2)
                m_pAdmin->CancelQuery (atoi (pArgs[0]), pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->CancelQuery (atoi (pArgs[0]));
            else
                displayCommand (CCancelQuery);
            break;

        case CDumpWorker:
            if (iArgCount == 3)
                m_pAdmin->DumpWorker (pArgs[0], pArgs[1], pArgs[3]);
            else if (iArgCount == 2)
                m_pAdmin->DumpWorker (pArgs[0], pArgs[1]);
            else
                displayCommand (CDumpWorker);
            break;

        case CDumpWorkingWorkers:
            if (iArgCount == 2)
                m_pAdmin->DumpWorkingWorkers (pArgs[0], pArgs[1]);
            else if (iArgCount == 1)
                m_pAdmin->DumpWorkingWorkers (pArgs[0]);
            else
                displayCommand (CDumpWorkingWorkers);
            break;

        case CDumpPoolQueue:
            if (iArgCount == 3)
                m_pAdmin->DumpPoolQueue (pArgs[0], atoi (pArgs[1]), pArgs[2]);
            else if (iArgCount == 2)
                m_pAdmin->DumpPoolQueue (pArgs[0], atoi (pArgs[1]));
            else if (iArgCount == 1)
                m_pAdmin->DumpPoolQueue (pArgs[0]);
            else
                displayCommand (CDumpPoolQueue);
            break;

        case CHelp:
            displayCommands();
            break;

        case CExit:
            displayingPrompt = notDone = false;
            break;

        default:
            display ("Invalid command.\nType 'help' for a list of commands.\n");
            break;
        }
    }
    if (displayingPrompt)
        displayPrompt ();
    fflush (stdout);

    return notDone;
}


bool Vsa::VPoolAdminInterpreter::onData_(char const *pLine, size_t sLine) {
    static char const *const pWhitespace = " \t\r\n";

    VString iCommand;
    iCommand.setTo (pLine, sLine);

    char *pCommand = iCommand.storage ();

//  Trim leading ...
    pCommand += strspn (pCommand, pWhitespace);

//  ... and trailing whitespace, ...
    char *pCommandTail = pCommand;
    size_t sCommandTail = strlen (pCommandTail);

    while (pCommandTail[0] != '\0') {
        size_t sChunk = strcspn (pCommandTail, pWhitespace); // ... non-whitespace

        pCommandTail += sChunk;
        sCommandTail -= sChunk;

        if (sCommandTail > 0) {
            sChunk = strspn (pCommandTail, pWhitespace); // ... whitespace
            if (sCommandTail == sChunk)
                *pCommandTail = '\0';
            else {
            //  More text follows this whitespace...
                pCommandTail += sChunk;
                sCommandTail -= sChunk;
            }
        }
    }

//  ... and process what's left:
    return processCommand (pCommand);
}

void Vsa::VPoolAdminInterpreter::onDone_() {
    m_pAdmin->stop ();
}


/*******************
 *******************
 ***** Utility *****
 *******************
 *******************/

void Vsa::VPoolAdminInterpreter::displayCommands() {
    unsigned int xDescription = 0;
    display ("\nUsage: Command <parameters> [session]\n");

    while (g_pCommandUsage[xDescription]) {
        char const *pDescription = g_pCommandUsage[xDescription++];
        display ("\n%2d. %s", xDescription, pDescription);
    }
    display ("\n");
}

void Vsa::VPoolAdminInterpreter::displayCommand (Command iCommand) {
    display ("Usage:%s\n", g_pCommandUsage[iCommand]);
}

void Vsa::VPoolAdminInterpreter::displayPrompt () {
    display ("\nVcaAdmin> ");
}
