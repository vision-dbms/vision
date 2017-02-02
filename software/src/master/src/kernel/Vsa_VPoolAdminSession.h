#ifndef VPoolAdminSession_Interface
#define VPoolAdminSession_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vsa_IEvaluatorClient.h"

#include "VReceiver.h"

#include "Vca_IGetter_Ex2.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    class VTimer;
}

namespace Vsa {
    class IEvaluator;
    class IEvaluatorPool;
    class IEvaluatorPoolSettings;
    class IPoolApplication_Ex1;
    class VPoolAdmin;

    /**
     * Serves as a conduit between VPoolAdmin and a VPool, handling command processing, queueing, etc.
     */
    class VPoolAdminSession : public VReferenceable {
    //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (VPoolAdminSession, VReferenceable);

    //  Friend
        friend class VPoolAdmin;

    public:
        /**
         * Possible values for m_iState.
         */
        enum State {
            State_Plumbing,
            State_Failure,
            State_Initialized
        };

        /**
         * Possible commands/operations. Used for queueing.
         */
        enum Command {
            //  Command Operations
            Command_Suspend,
            Command_Resume,
            Command_Restart,
            Command_HardRestart,
            Command_Statistics,
            Command_AppSettings,
            Command_Stop,
            Command_HardStop,
            Command_FlushWorkers,
            Command_Shutdown,
            Command_TotalWorkers,
            Command_SetLogSwitchOn,
            Command_SetLogSwitchOff,
            Command_IsWorkerCreationFailureLmtReached,
            Command_CurrentGenerationTotalWorkers,
            Command_GetMinWorkers,
            Command_GetMaxWorkers,
            Command_GetQueryCount,
            Command_GetClientCount,
            Command_GetQueueLength,
            Command_QueryDetails,
            Command_StatisticsDump,
            Command_QuickStatDumpHeader,
            Command_QuickStatDump,
            Command_WorkerStatDumpHeader,
            Command_WorkerStatDump,
            Command_GenerationStatDumpHeader,
            Command_GenerationStatDump,
            Command_Workers,

			Command_StatSum,
			Command_StatMax,
			Command_StatMin,
			Command_StatDivide,
			
            //  Query Operations
            Command_Query,

            //  Value Operations
            Command_SetBroadcastInProgLimit,
            Command_SetEvaluationTimeout,
            Command_SetWorkerMaximum,
            Command_SetWorkerMinimum,
            Command_SetGenerationMaximum,
            Command_CancelQuery,

            //  Worker Operations
            Command_TakeWorkerOffline,
            Command_BringWorkerOnline,
            Command_RetireOfflineWorker,
            Command_WorkerStat
        };

        /**
         * Enumeration of error/output messages.
         */
        enum Message {
            Pool_Operation_Not_Supported,
            Null_Evaluator,
            End_Of_Data,
            Query_Timedout,
            Connection_Error,
            No_Object_At_Address,
            Null_IPoolApplication,
            Null_IPoolApplication_Ex1,
            Null_IEvaluatorPool,
            Null_IEvaluator,
            Null_IEvaluatorPoolSettings,
            Null_WorkerInterface,
            Invalid_Broadcast_File,
            Invalid_Worker_Startup_File,
            Invalid_Workerpid_Query_File,
            Invalid_Query_File,
            Operation_Failed,
            Worker_Creation_Limit_Reached,
            Message_Sent,
            Operation_Succeeded,
            Worker_Creation_Limit_Not_Reached,
            Null_IGetter,
            Invalid_Msg
        };

      //  Globals
    private:
      /**
       * Actual error message contents, in the order specified in Message.
       */
      static char const *g_pMessages[Invalid_Msg+1];

    public:
        class Query : public Vca::VRolePlayer {
        //  Run Time Type
            DECLARE_CONCRETE_RTTLITE (Query, Vca::VRolePlayer);

        //  Typedefs
            typedef VPoolAdminSession Session;

        public:
            enum Type {
                Query_Ping,
                Query_Broadcast,
                Query_WorkerStartup,
                Query_Offline_Worker
            };

        public:
            Query (
                Session *pSession,
                VString const &rQuery,
                Type iType,
                bool bAnyData = false,
                Vca::U64 iTimeOutSeconds = U64_MAX,
                VString const &iWorkerId="", bool bIsPID=false
            );

        protected:
            ~Query ();

        //  IEvaluatorClient Role
        private:
            Vca::VRole<ThisClass,Vsa::IEvaluatorClient> m_pIEvaluatorClient;
        public:
            void getRole (VReference<Vsa::IEvaluatorClient>&rpRole) {
                m_pIEvaluatorClient.getRole (rpRole);
            }

        //  Vca::IEvaluatorClient Methods
        public:
            void OnResult (
                Vsa::IEvaluatorClient *pRole, Vsa::IEvaluationResult *pResult, VString const &rOutput
            );
            void  OnAccept (
                Vsa::IEvaluatorClient *pRole, Vsa::IEvaluation *pEvaluation, Vca::U32 xQueuePosition
            );
            void OnChange (
                Vsa::IEvaluatorClient *pRole, Vca::U32 xQueuePosition
                ) {
            }

        //  Vca::IClient Methods
        public:
            void OnError (
                Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
            );
            void OnEnd (Vca::IClient *pRole);

        //  Operations
        public:
            void start ();

        //  Timeout
        public:
            bool setTimeout ();
            void onTimeout (Vca::VTrigger<ThisClass> *pTrigger);

        //  Linkage
        private:
            void link ();
            void unlink ();

        protected:
            VString                     m_iQuery;
            Vca::U64                    m_iTimeOut; // Seconds
            bool                        m_bTimedOut;
            VReference<Vca::VTimer>     m_pTimer;
            Session              *const m_pSession;
            bool                        m_bAnyData;
            Type                        m_iType;
            VString                     m_iWorkerId;
            bool                        m_bIsPID;

            VReference<Query>   m_pSuccessor;
            VReference<Query>   m_pPredecessor;
        };


    //  Friends
        friend class Query;

        class Operation : public VReferenceable {
        //  Run Time Type
            DECLARE_CONCRETE_RTTLITE (Operation, VReferenceable);

        //  Friend
            friend class VPoolAdminSession;

        public:
            Operation (Command iCommand): m_iCommand (iCommand) {
            }
        protected:
            ~Operation () {
            }

        protected:
            Command const m_iCommand;
            VReference<Operation>  m_pSuccessor;
        };

		class OdometerOperation : public Operation {
		//  Run Time Type
        DECLARE_CONCRETE_RTTLITE (OdometerOperation, Operation);

        //  Construction
        public:
            OdometerOperation (Command iCommand, VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange)
                : Operation (iCommand)
				, m_iName (name)
                , m_iTimeBefore (timeBefore)  
				, m_iTimeRange (timeRange) {
            }

        //  Destruction
        protected:
            ~OdometerOperation () {
            }
			
        public:
            VString const name () const {
                return m_iName;
            }
			
			Vca::U64 timeBefore () const {
				return m_iTimeBefore;
			}
			
			Vca::U64 timeRange () const {
				return m_iTimeRange;
			}
			
		//  State
        private:
            VString const m_iName;
			Vca::U64 m_iTimeBefore;
			Vca::U64 m_iTimeRange;
        };
		
		class StatDivideOperation : public OdometerOperation {
		//  Run Time Type
        DECLARE_CONCRETE_RTTLITE (StatDivideOperation, OdometerOperation);

        //  Construction
        public:
            StatDivideOperation (VString const &name1, VString const &name2, Vca::U64 timeBefore, Vca::U64 timeRange)
                : OdometerOperation (Command_StatDivide, name1, timeBefore, timeRange)
				, m_iName2 (name2) {
            }

        //  Destruction
        protected:
            ~StatDivideOperation () {
            }
			
        public:
            VString const name2 () const {
                return m_iName2;
			}
			
		//  State
        private:
            VString const m_iName2;
        };
		
        class QueryOperation : public Operation {
        //  Run Time Type
            DECLARE_CONCRETE_RTTLITE (QueryOperation, Operation);

        //  Construction
        public:
            QueryOperation (Query *pQuery)
                : Operation (Command_Query)
                , m_pQuery (pQuery)  {
            }

        //  Destruction
        protected:
            ~QueryOperation () {
            }

        //  Query
        public:
            Query *query () const {
                return m_pQuery;
            }

        //  State
        private:
            VReference<Query>  const m_pQuery;
        };

        class ValueOperation: public Operation {
        //  Run Time Type
            DECLARE_CONCRETE_RTTLITE (ValueOperation, Operation);

        //  Construction
        public:
            ValueOperation(Command iCommand, Vca::U64 iValue)
                : Operation (iCommand)
                , m_iValue (iValue)  {
            }

        //  Destruction
        protected:
            ~ValueOperation() {
            }

        //  Query
        public:
            Vca::U64 value () const {
                return m_iValue;
            }

        //  State
        private:
            Vca::U64 const m_iValue;
        };
        class WorkerOperation: public Operation {
        //  Run Time Type
            DECLARE_CONCRETE_RTTLITE (WorkerOperation, Operation);

        //  Construction
        public:
            WorkerOperation(Command iCommand, VString const & iWorkerId, bool bIsPID)
                : Operation (iCommand), m_iWorkerId (iWorkerId), m_bIsPID (bIsPID)  {
            }

        //  Destruction
        protected:
            ~WorkerOperation() {
            }

        //  Query
        public:
            VString const & workerId () const {
                return m_iWorkerId;
            }
            bool isWorkerPID () const {
                return m_bIsPID;
            }
        //  State
        private:
            VString const m_iWorkerId;
            bool           m_bIsPID;
        };


    //  Typedefs
    public:
        typedef VReceiver<ThisClass, IVUnknown*> IVUnknownReceiver;
        typedef VReceiver<ThisClass, bool>       BoolReceiver;
        typedef VReceiver<ThisClass, Vca::U32>   U32Receiver;
		typedef VReceiver<ThisClass, Vca::S64> 	 S64Receiver;
        typedef VReceiver<ThisClass, VString const&> VStringReceiver;
        typedef VReceiver<ThisClass, Vsa::IEvaluatorPoolSettings*> PoolSettingsReceiver;
        typedef VReceiver<ThisClass, Vsa::IEvaluator*> EvaluatorReceiver;
		typedef VReceiver<ThisClass, Vca::F64> 	F64Receiver;

    //  Construction
    public:
        VPoolAdminSession (
            VString const &rAddress, VString const &rSessionName, VPoolAdmin *pAdmin
        );

    //  Destruction
    protected:
        ~VPoolAdminSession ();

    //  Query
    public:
        Vsa::IEvaluator *evaluator () const {
            return m_pEvaluator;
        }
        Vsa::IEvaluatorPool *poolEvaluator () const {
            return m_pPool;
        }
        Vsa::IEvaluatorPoolSettings *poolSettings () const {
            return m_pPoolSettings;
        }

    //  Session Management
    private:
    //  Control
        void Stop           (); /**< Stops pool gracefully. */
        void HardStop       (); /**< Stops pool immediately. */
        void Suspend        (); /**< Suspends pool, preventing new queries from being processed. */
        void Resume         (); /**< Resumes normal operation of suspended pool. */
        void Restart        (); /**< Stops gracefully, then starts pool. */
        void HardRestart    (); /**< Stops immediately, then starts pool. */
        void FlushWorkers   (); /**< @copydoc Vsa::IEvaluatorPool::FlushWorkers() */

        /**
         * @copydoc Stop()
         * @deprecated
         */
        void Stop_Old ();
        /**
         * @copydoc HardStop()
         * @deprecated
         */
        void HardStop_Old ();

    //  Query (Ping/Broadcast)
        void QuerySession   (Query *pQuery);
        /**
         * Sends a Ping query.
         *
         * @param rQuery the query to be sent along as the ping query.
         * @param bAnyData whether or not to allow workers from older generations to respond to this ping.
         * @param iTimeOutSecs the timeout for this query in seconds.
         */
        void PingSession   (VString const &rQuery, bool bAnyData = false, Vca::U64 iTimeOutSecs = U64_MAX);
        /**
         * Broadcasts a query to all workers.
         *
         * @param rQuery the query to be sent along to all workers.
         * @param iTimeOutSecs the timeout for this query in seconds.
         */
        void Broadcast     (VString const &rQuery, Vca::U64 iTimeOutSecs = U64_MAX);
        /**
         * Broadcasts a query, sourced from a file, to all workers.
         *
         * @param rQueryFile the file containing the query to send along to all workers.
         * @param iTimeOutSecs the timeout for this query in seconds.
         */
        void BroadcastFile (VString const &rQueryFile, Vca::U64 iTimeOutSecs = U64_MAX);
        /**
         * Cancels a query.
         *
         * @param iID the index of the query to be canceled.
         */
        void CancelQuery   (Vca::U32 iID);
        /**
         * Sets the status of a given worker to offline, preventing it from automatically being used to process queries.
         *
         * @param iWorkerId the workerId of the worker to be taken offline.
         * @param bIsPID true if the WorkerId is in PID form.
         */
        void TakeWorkerOffline (VString const &iWorkerId, bool bIsPID);
        /**
         * Sets the status of a given worker to online, allowing it to be used automatically for processing queries.
         *
         * @param iWorkerId the workerId of the worker to be taken offline.
         * @param bIsPID true if the WorkerId is in PID form.
         */
        void MakeWorkerOnline (VString const &iWorkerId, bool bIsPID);
        /**
         * Evaluates a query, sourced from a file, using a particular offline worker.
         *
         * @param rQueryFile the file containing the query to send along to all workers.
         * @param iWorkerId the workerId of the worker to be taken offline.
         * @param bIsPID true if the WorkerId is in PID form.
         */
        void EvaluateUsingOfflineWorker (VString const &rQueryFile, VString const &iWorkerId, bool bIsPID);
        /**
         * Retires a given worker already in an offline state.
         *
         * @param iWorkerId the workerId of the worker to be taken offline.
         * @param bIsPID true if the WorkerId is in PID form.
         */
        void RetireOfflineWorker (VString const &iWorkerId, bool bIsPID);
        void DumpWorker (VString const &iWorkerId, bool bIsPID, VString const &rFile);
        void DumpWorkingWorkers (VString const &rFile);
        void DumpPoolQueue (VString const &rFile, Vca::U32 cQueries);


    //  Parameter Setting
        void SetWorkerMinimum   (Vca::U32 workerCount);
        void SetWorkerMaximum   (Vca::U32 workerCount);
        void SetWorkerProcessSource    (VString const &source);
        void SetWorkerMinimumAvailable (Vca::U32 workerCount);
        void SetWorkerMaximumAvailable (Vca::U32 workerCount);
        void SetWorkerStartupQuery         (VString const &rQuery);
        void SetWorkerStartupFile          (VString const &rStartupFile);
        void SetWorkerPIDQueryFromFile     (VString const &rStartupFile);
        void SetWorkersBeingCreatedMaximum  (Vca::U32 workerCount);
        void SetWorkerCreationFailureHardLimit  (Vca::U32 count);
        void SetWorkerCreationFailureSoftLimit  (Vca::U32 count);
        void SetLogFilePath     (VString const &rPath);
        void SetLogSwitch       (bool bLogSwitch);
        void SetLogFileSize     (Vca::U32 iSize);
        void SetLogFileBackups  (Vca::U32 iCount);
		void SetEvaluationAttempts  (Vca::U32 cEvaluationAttempt);
        void SetEvaluationOnErrorAttempts  (Vca::U32 cEvaluationOnErrorAttempt);
		void SetEvaluationTimeOutAttempts  (Vca::U32 cEvaluationTimeOutAttempt);
        void SetShrinkTimeOut (Vca::U32 iTimeOutMinutes);
        void SetBroadcastInProgressLimit (Vca::U32 iLimit);
        void SetWorkerQueryHistoryLimit (Vca::U32 iLimit);
        void SetStopTimeOut (Vca::U32 iTimeOutMins);
        void SetEvaluationTimeout (Vca::U64 iTimeoutSecs);
        void SetGenerationMaximum (Vca::U32 count);
		void SetQueryLogLength (Vca::U32 iLogLength);
		void SetResultLogLength (Vca::U32 iResultLength);


		//  Pool Query
		void StatSum			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange);
		void StatMax			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange);
		void StatMin			(VString const &name, Vca::U64 timeBefore, Vca::U64 timeRange); 
		void StatDivide			(VString const &name1, VString const &name2, Vca::U64 timeBefore, Vca::U64 timeRange);
        void ServiceStatistics  ();
        void QueryDetails       (Vca::U32 iID);
        void StatisticsDump     ();
        void QuickStatDumpHeader ();
        void QuickStatDump      ();
        void WorkerStatistics   (VString const &rID, bool bIsPID);
        void WorkersStatistics  ();
        void WorkerStatDumpHeader ();
        void WorkerStatDump     ();
        void GenerationStatistics (Vca::U32 iID);
        void GenerationStatDumpHeader ();
        void GenerationStatDump ();
        void GetAppSettings ();
        void GetTotalWorkers ();
        void IsWorkerCreationFailureLmtReached ();
        void GetCurrentGenerationTotalWorkers ();
        void GetMinWorkers ();
        void GetMaxWorkers ();
        void GetQueryCount ();
        void GetQueueLength ();
        void GetClientCount ();


    //  Shutdown
    private:
        void Shutdown       ();

    //  Receivers
    private:
        void onConnect               (IVUnknown *pObject);
        void onConnectError          (Vca::IError *pIError, VString const &rMessage);

        void onEvaluator             (EvaluatorReceiver *pReceiver, Vsa::IEvaluator *pEvaluator);
        void onEvaluatorPool         (IVUnknownReceiver *pReceiver, IVUnknown *pUnknown);
        void onEvaluatorPoolSettings (PoolSettingsReceiver *pReceiver, Vsa::IEvaluatorPoolSettings *);
        void onPoolApplication       (IVUnknownReceiver *pReceiver, IVUnknown *pUnknown);
        void onGetter                (IVUnknownReceiver *pReceiver, IVUnknown *pUnknown);
        void onWorkerInterface       (IVUnknownReceiver *pReceiver, IVUnknown*);
        void onWorkerError           (IVUnknownReceiver *pReceiver, Vca::IError *pError, VString const &rErrorTxt);
        void onOperationResult       (BoolReceiver *pReceiver, bool operationResult);
        void onOperationError        (BoolReceiver *pReceiver, Vca::IError *pError, VString const& rText);
        void onResult                (VStringReceiver *pReceiver, VString const& rResult);
        void onError                 (VStringReceiver *pReceiver, Vca::IError *pError, VString const& rText);
        void onAppSettings               (VStringReceiver *pReceiver, VString const& rStat); 
        void onWorkerCreationFailureLmtResult (BoolReceiver *pReceiver, bool rResult);
        void onU32Result             (U32Receiver *pReceiver, Vca::U32);
        void onU32Error              (U32Receiver *pReceiver, Vca::IError *pError, VString const& rText);
		void onS64Result			 (S64Receiver *pReceiver, Vca::S64);
		void onS64Error				 (S64Receiver *pReceiver, Vca::IError *pError, VString const& rText);
		void onF64Result			(F64Receiver *pReceiver, Vca::F64);
		void onF64Error              (F64Receiver *pReceiver, Vca::IError *pError, VString const& rText);

    private:
        void onQIComplete ();

    //  Callbacks
    private:
        void clear ();
        void enqueue (Operation *pOperation);
        void triggerQueuedOperations ();
        void onOperationComplete ();

    //  Access
    private:
        char const* name () const {
            return m_iSessionName.content ();
        }
        char const* address () const {
            return m_iAddress.content ();
        }

    //  Query
    private:
        bool isFailed () const {
            return m_iState == State_Failure;
        }
        bool isntFailed () const {
            return !isFailed ();
        }
        bool isInitialized () const {
            return m_iState == State_Initialized;
        }
        bool isntInitialized () const {
            return !isInitialized ();
        }

    /// @name Display
    //@{
    private:
        /**
         * Generic display callback for string results.
         * Delegates to VPoolAdmin::displayResult(VString const &).
         */
        void displayResult (VString const &rResult);

        /**
         * Generic display callback for error results.
         * Delegates to displayResult() with appropriate error prefix.
         */
        void displayError (VString const &rText);
    //@}

    //  State
    protected:
        VPoolAdmin *const                       m_pAdmin;
        VString const                           m_iSessionName;
        VString const                           m_iAddress;
        VReference<IVUnknown>                   m_pFarside;
        VReference<IEvaluator>                  m_pEvaluator;
        VReference<IEvaluatorPool>              m_pPool;
        VReference<IEvaluatorPoolSettings>      m_pPoolSettings;
        VReference<IPoolApplication_Ex1>        m_pPoolApp;
        VReference<Vca::IGetter>                m_pGetter;
        VReference<Operation>                   m_pPendingOperation;
        VReference<Query>                       m_pQueryHead;
        State                                   m_iState;
        bool                                    m_bSuspend;
    };
}

#endif
