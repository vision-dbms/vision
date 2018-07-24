#ifndef VPoolAdmin_Interface
#define VPoolAdmin_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VClientApplication.h"

#include "Vca_IAdmin.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

template<class Actor, class Datum> class VReceiver;

#include "Vsa_VPoolAdminSession.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    /**
     * Main class for the VPoolAdmin utility.
     *
     * Uses a VPoolAdminInterpreter instance for getting user input and doing
     * command validity. Maintains a table of sessions with pools
     * (i.e.poolname -> session object). When connecting to the pool, the
     * session is in the "Plumbing" state, after which it moves to the
     * QueryInterface state wherein it queries the ServiceInterface of the
     * Pool application for IPoolApplication, IEvaluatorPool, and
     * IEvaluatorPoolSettings interfaces.
     */
    class VPoolAdmin : public Vca::VClientApplication {
        DECLARE_CONCRETE_RTTLITE (VPoolAdmin, Vca::VClientApplication);

    //  Friends
        friend class VPoolAdminSession;

    //  Typedefs
        typedef VPoolAdminSession Session;
        typedef VkMapOf<VString, char const*, char const*, Session::Reference> SessionTable;
        typedef SessionTable::Iterator Iterator;

    //  Construction
    public:
        VPoolAdmin (Context *pContext);

    //  Destruction
    protected:
        ~VPoolAdmin ();

    //  Base Class Roles
    public:
        using BaseClass::getRole;

    //  IAdmin Role
    private:
        Vca::VRole<ThisClass,Vca::IAdmin> m_pIAdmin;
    public:
        void getRole (Vca::IAdmin::Reference &rpRole);

    // Admin Operations
    public:
        Session* Open       (VString const &rAddress, VString const &rSession = "default");

    //  Control
        void Stop          (VString const &rSession = "default");
        void Suspend       (VString const &rSession = "default") const;
        void Resume        (VString const &rSession = "default") const;
        void Restart       (VString const &rSession = "default") const;
        void FlushWorkers  (VString const &rSession = "default") const;
        void HardRestart   (VString const &rSession = "default") const;
        void HardStop      (VString const &rSession = "default") ;
		
    //  Parameter Setting
        void SetWorkerMinimum   (Vca::U32 workerCount,  VString const &rSession = "default") const;
        void SetWorkerMaximum   (Vca::U32 workerCount,  VString const &rSession = "default") const;
        void SetWorkerProcessSource    (VString const &source, VString const &rSession = "default") const;
        void SetWorkerMinimumAvailable (Vca::U32 workerCount, VString const &rSession = "default") const;
        void SetWorkerMaximumAvailable (Vca::U32 workerCount, VString const &rSession = "default") const;
        void SetWorkerStartupQuery     (VString const &rQuery, VString const &rSession = "default") const;
        void SetWorkerStartupFromFile  (VString const &rFile, VString const &rSession = "default") const;
        void SetWorkerPIDQueryFromFile     (VString const &rFile, VString const &rSession = "default") const;
        void SetWorkersBeingCreated    (Vca::U32 workerCount, VString const &rSession = "default") const;
        void SetWorkerCreationFailureHardLimit  (Vca::U32 count,  VString const &rSession = "default") const;
        void SetWorkerCreationFailureSoftLimit  (Vca::U32 count,  VString const &rSession = "default") const;
        void SetGenerationMaximum (Vca::U32 count, VString const &rSession = "default") const;
        void GetAppSettings  (VString const &rSession = "default") const;
        void SetLogFilePath  (VString const &rPath, VString const &rSession = "default") const;
        void SetLogSwitch  (bool bTraceSwitch, VString const &rSession = "default") const;
        void SetLogFileSize    (Vca::U32 iSize,  VString const &rSession = "default") const;
        void SetLogFileBackups (Vca::U32 iCount, VString const &rSession = "default") const;
        void SetEvaluationTimeout    (Vca::U64 sTimeout, VString const &rSession = "default") const;
		void SetEvaluationAttempts    (Vca::U32 cEvaluationAttempt, VString const &rSession = "default") const;
        void SetEvaluationOnErrorAttempts    (Vca::U32 cEvaluationOnErrorAttempt, VString const &rSession = "default") const;
		void SetEvaluationTimeOutAttempts    (Vca::U32 cEvaluationTimeOutAttempt, VString const &rSession = "default") const;

        void SetShrinkTimeOut  (Vca::U32 iTimeOutMinutes, VString const &rSession = "default") const;
        void SetBroadcastInProgressLimit  (Vca::U32 iLimit, VString const &rSession = "default") const;
        void SetWorkerQueryHistoryLimit  (Vca::U32 iLimit, VString const &rSession = "default") const;
        void SetStopTimeOut  (Vca::U32 iTimeOutMins, VString const &rSession = "default") const;
        void TakeWorkerOffline (VString const& iWorkerId, VString const &rSession = "default") const;
        void MakeWorkerOnline (VString const & iWorkerId, VString const &rSession = "default") const;
        void EvaluateUsingOfflineWorker (VString const &rQueryFile, VString const& iWorkerId, VString const &rSession = "default") const;
        void RetireOfflineWorker (VString const& iWorkerId, VString const &rSession = "default") const;
        void CancelQuery (Vca::U32 iID, VString const &rSession = "default") const;
        void DumpWorker (VString const & iWorkerId, VString const &rFile, VString const &rSession = "default") const;
        void DumpWorkingWorkers (VString const &rFile, VString const &rSession = "default") const;
        void DumpPoolQueue (VString const &rFile, Vca::U32 cQueries=0, VString const &rSession = "default") const;

    //  Pool Query
        void GetTotalWorkers (VString const &rSession = "default") const;
        void IsWorkerCreationFailureLmtReached (VString const &rSession = "default") const;
        void GetCurrentGenerationTotalWorkers (VString const &rSession = "default") const;
        void GetMaxWorkers (VString const &rSession = "default") const;
        void GetMinWorkers (VString const &rSession = "default") const;
        void GetQueryCount (VString const &rSession = "default") const;
        void GetQueueLength (VString const &rSession = "default") const;
        void GetClientCount (VString const &rSession = "default") const;

    //  Statistics
		void StatSum			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange, VString const &rSession = "default") const;
		void StatMin			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange, VString const &rSession = "default") const;
		void StatMax			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange, VString const &rSession = "default") const;
		void StatDivide			(VString const &names, Vca::U64 timeBefore, Vca::U64 timeRange, VString const &rSession = "default") const;
        void ServiceStatistics  (VString const &rSession = "default") const;
        void QueryDetails       (Vca::U32 iID, VString const &rSession = "default") const;
        void StatisticsDump     (VString const &rSession = "default") const;
        void QuickStatistics    (VString const &rSession = "default") const;
        void WorkerStatistics   (VString const &rID, VString const &rSession = "default") const;
        void WorkersStatistics  (VString const &rSession = "default") const;
        void WorkerStatDump     (VString const &rSession = "default") const;
        void GenerationStatistics (Vca::U32 iID, VString const &rSession = "default") const;
        void GenerationStatDump (VString const &rSession = "default") const;

    //  Ping Session
        void PingSession        (VString const &rQuery, bool bAnyData, Vca::U64 iTimeOut = U64_MAX, VString const &rSession = "default") const;

    // Broadcast
        void Broadcast (VString const &rQuery, Vca::U64 iTimeOut = U64_MAX, VString const &rSession = "default") const;
        void BroadcastFile (VString const &rFile, Vca::U64 iTimeOut = U64_MAX, VString const &rSession = "default") const;


    //  SessionTable Operations
    private:
        bool insertSession      (Session *pSession);
        bool removeSession      (Session *pSession);
        bool getSession         (VString const &rSession, Session::Reference &rpSession) const;

    //  Shutdown
    private:
        virtual bool stop_(bool bHardStop) OVERRIDE;

    //  Start
    private:
        virtual bool start_() OVERRIDE;
        void processCommandLineArgs (VString const &rServerName);
        virtual bool getDefaultServerName (VString &rServerName) const OVERRIDE;

    //  Utility
    public:
        void displaySessions ();
        static void displayResult (VString const &rResult);
        static bool g_bInteractive;

    //  State
    protected:
        SessionTable  m_iSessionTable;
        CommandCursor m_iCommandCursor;
    };
}

#endif
