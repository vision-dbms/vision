#ifndef VPoolAdminInterpreter_Interface
#define VPoolAdminInterpreter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VLineGrabber.h"

/**************************
 *****  Declarations  *****
 **************************/


/***************************************
 ***************************************
 ****  Vsa::VPoolAdminInterpreter   ****
 ***************************************
 ***************************************/

namespace Vsa {
    class VPoolAdmin;

    class VPoolAdminInterpreter : public Vca::VLineGrabber {
    //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (VPoolAdminInterpreter, Vca::VLineGrabber);

    // Construction
    public:
        VPoolAdminInterpreter (VPoolAdmin *pAdmin, Vca::VBSProducer *pBSToHere);

    // Destruction
    protected:
        ~VPoolAdminInterpreter ();

    // Interpreter Command List
    private:
        enum Command {
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
            CSetWorkerQueryHistoryLimit,
            CBroadcastFile,
            CSetBroadcastInProgressLimit,
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
            CExit,
            CInvalid
        };

    //  Globals
    private:
        static char const *g_pCommandUsage[CInvalid+1];

    private:
        static void displayCommands ();
        static void displayCommand (Command iCommand);
        static size_t CommandCount () {
            return CInvalid;
        }
    public:
        static void displayPrompt ();

    //  Command Processor
    private:
        bool onData_(char const *pLine, size_t sLine) OVERRIDE;
        void onDone_() OVERRIDE;

        bool processCommand (char *pCommand);

    //  State
    protected:
        VReference<VPoolAdmin> const m_pAdmin;
    };
}


#endif
