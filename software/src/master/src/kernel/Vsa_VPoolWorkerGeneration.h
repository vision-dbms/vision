#ifndef Vsa_VPoolWorkerGeneration_Interface
#define Vsa_VPoolWorkerGeneration_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vca_VInterfaceEKG.h"
#include "Vsa_IEvaluator.h"
#include "Vsa_VPoolWorker.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDataSource.h"

namespace V {
    class VSimpleFile;
}

namespace Vsa {
    class VPoolEvaluation;
    class VEvaluatorPool;
    class VPoolBroadcast;
}

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VPoolWorkerGeneration : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (VPoolWorkerGeneration, Vca::VRolePlayer);

    public:
    //  Typedef
        typedef Vca::IDataSource<IEvaluator*> IEvaluatorSource;
        typedef VPoolWorker                   Worker;
        typedef VPoolWorker::WorkInProgress   WIP;
	     
    //  Enum
        enum WorkerList {
            WorkerList_Online,
            WorkerList_Offline,
            WorkerList_Retired
        };
        enum WIPList {
            WIPList_Online,
            WIPList_Offline
        };

        //  WorkerRequest
        public:
            class WorkerRequest : public Vca::VRolePlayer {
                DECLARE_CONCRETE_RTTLITE (WorkerRequest, Vca::VRolePlayer);

            //  Typedef
                typedef IVReceiver<IEvaluator*> IEvaluatorSink;
                typedef VPoolWorkerGeneration   WorkerGeneration;
                typedef VPoolWorker             Worker;
		typedef Vca::VInterfaceEKG<ThisClass,IEvaluator> interface_monitor_t;

            //  Friends
                friend class VPoolWorkerGeneration;


            //  Construction
            public:
                WorkerRequest (Vca::U32 iWorkerId, WorkerGeneration *pGeneration);
                WorkerRequest (Vca::U32 iWorkerId, WorkerGeneration *pGeneration, IEvaluator *pEvaluator);

            //  Destruction
            protected:
                ~WorkerRequest ();

            //  IEvaluatorSink Role
            private:
                Vca::VRole<ThisClass,IEvaluatorSink> m_pIEvaluatorSink;
            public:
                void getRole (VReference<IEvaluatorSink>&rpRole);

            //  IEvaluatorSink Methods
            public:
                void OnData (IEvaluatorSink *pRole, IEvaluator *pEvaluator);

            //  Vca::IClient Methods
            public:
                void OnError (
                    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
                ) OVERRIDE;
                void OnEnd (Vca::IClient *pRole) OVERRIDE;

            //  IEvaluatorClient Role
            private:
                Vca::VRole<ThisClass,IEvaluatorClient> m_pIEvaluatorClient;
            public:
                void getRole (VReference<IEvaluatorClient>&rpRole);

            //  IEvaluatorClient Methods
            public:
                void OnAccept (
                    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
                );
                void OnChange (
                    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
                );
                void OnResult (
                    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
                );

            //  Access
            public:
                Vca::U32 workerId () const {
                    return m_iWorkerId;
                }
                Worker::Reference worker () {
                    return m_pWorker;
                }

            //   Worker Creation Operations
            private:
                void start          ();
                void handleError    (VString const &rMessage);
                /**
                 * Creates and returns a new Worker instace to the generation.
                 * Also clears any state references that aren't needed.
                 *
                 * @param rPID the PID string of the newly created Worker.
                 *
                 * @see Vsa::VPoolWorkerGeneration::onWorker()
                 */
                void returnNewWorker (VString const &rPID);

		//  EKG
            private:
		void signalIEvaluatorEKG ();
		void cancelIEvaluatorEKG ();

            //  State
            protected:

                VReference<WorkerGeneration>      m_pGeneration;
                VEvaluatorPool      const  *const m_pPool;
                Vca::U32                          m_iWorkerId;
				IEvaluator::Reference             m_pEvaluator;
                VReference<Worker>                m_pWorker;
                VReference<ThisClass>             m_pSuccessor;
                VReference<ThisClass>             m_pPredecessor;
				interface_monitor_t::Reference    m_pEvaluatorEKG;
            };

    //  Friends
        friend class VPoolWorker;
        friend class WorkerRequest;
        friend class VEvaluatorPool;
        friend class VPoolWorker::WorkInProgress;

    //  Note: Worker Generation Id and Worker Ids start from 1
    //  Construction
    public:
        VPoolWorkerGeneration (
            VEvaluatorPool *pPool, Vca::U32 iGeneration = 1, Vca::U32 iNextWorker = 1
        );

    //  Destruction
    private:
        ~VPoolWorkerGeneration ();

    //  Access
    public:
        Vca::U32 generationId () const {
            return m_iGeneration;
        }
        Vca::U32 onlineWorkerCount () const {
            return m_cOnlineWorkers;
        }
        Vca::U32 onlineAvailWorkers () const {
            return m_cOnlineAvailWorkers;
        }
        Vca::U32 onlineWIPs () const {
            return m_cOnlineWIPs;
        }
        Vca::U32 onlineExcessWorkers () const {
            return m_cOnlineExcessWorkers;
        }
        Vca::U32 workersBeingCreated () const {
            return m_cWorkersBeingCreated;
        }
        Vca::U32 disabledWorkers () const {
            return m_cDisabledWorkers;
        }
        Vca::U32 retiredWorkers () const {
            return m_cWorkersRetired;
        }
        Vca::U32 offlineWorkerCount () const {
            return m_cOfflineWorkers;
        }
        Vca::U32 offlineAvailWorkers () const {
            return m_cOfflineAvailWorkers;
        }
        Vca::U32 offlineWIPs () const {
            return m_cOfflineWIPs;
        }
        VEvaluatorPool *pool () const {
            return m_pPool;
        }
        Vca::U32 nextWorker () const {
            return m_iNextWorker;
        }
        Vca::U32 workerCreationHardFailures () const {
            return m_cWorkerCreationHardFailures;
        }
        Vca::U32 workerCreationSoftFailures () const {
            return m_cWorkerCreationSoftFailures;
        }

    //  Query
    public:
        bool isCurrent      () const;
        bool isOnlineWIP (WIP const *pWIP) const;
        bool canHireWorker () const;
        bool shouldHireWorker () const;
        bool isWorkerEmployable (Worker*) const;
        bool isWorkerCreationFailureHardLmtReached    () const;
        bool isWorkerCreationFailureHardLmtNotReached () const;
        bool isWorkerCreationFailureSoftLmtReached    () const;
        bool isWorkerCreationFailureSoftLmtNotReached () const;
        bool canProcess (VPoolEvaluation const *pEvaluation) const;
        bool isProcessing (Vca::U32 xEvaluation, VReference<WIP> &rpWIP) const;
        bool isProcessing (VPoolEvaluation const *pEvaluation, VReference<WIP> &rpWIP) const;
        Vca::U32 WIPsWithWorkerMode (WIPList iList, Worker::Worker_Mode iMode) const;
        bool hasActiveWIPs () const;

    //  Update
    private:
        void incrOnlineWorkers ();
        void decrOnlineWorkers ();

        void incrOnlineAvailWorkers ();
        void decrOnlineAvailWorkers ();

        void incrOnlineWIPs ();
        void decrOnlineWIPs ();

        void incrWorkersBeingCreated ();
        void decrWorkersBeingCreated ();

        void incrOnlineExcessWorkers ();
        void decrOnlineExcessWorkers ();
        void incrOnlineExcessWorkers (Vca::U32 cWorkers);

        void incrWorkersRetired ();
        void decrWorkersRetired ();

        void incrOfflineWorkers ();
        void decrOfflineWorkers ();

        void incrOfflineAvailWorkers ();
        void decrOfflineAvailWorkers ();

        void incrOfflineWIPs ();
        void decrOfflineWIPs ();

        void incrOfflineExcessWorkers ();
        void decrOfflineExcessWorkers ();
        void incrOfflineExcessWorkers (Vca::U32 cWorkers);


    //  WorkerGeneration Control
    private:
        void retire      (bool bOnlyOnlineWorkers=true);
        void hire        ();

    //  Worker Creation/Callbacks
    public:
        void finishHiringWorker ();
        bool hasStartedWorkers ();

    private:
        void hireNewWorker   ();
        void hireNewWorker   (IEvaluator *pEvaluator);
        void onWorker        (WorkerRequest *pRequest, Worker *pWorker);
        void onWorkerError   (WorkerRequest *pRequest, VString const &rMessage);


    //  Worker Management
    private:
        void employ         (Worker *pWorker);
        void retire         (Worker *pWorker, Worker::Worker_RetirementCause iCause = Worker::Worker_Cutback);
        void disable        (Worker *pWorker);
        void add            (IEvaluator *pEvaluator);
        bool returnWorker   (Worker *pWorker);

        void retireAvailWorkers (Vca::U32 cWorkers, bool bOnlineWorkers=true);
        void retireStartingWorkers ();

        Vca::U32 trimWorkers (Vca::U32 cWorkers, bool bIdleOnly = false);

        bool takeWorkerOffline (Worker *pWorker, IVReceiver<VString const&> *pClient);
        bool makeWorkerOnline  (Worker *pWorker, IVReceiver<VString const&> *pClient);
        void evaluateUsingOfflineWorker (Worker *pWorker, VString const &rQuery, IEvaluatorClient *pClient);
        void retireOfflineWorker (Worker *pWorker, IVReceiver<VString const&> *pClient);


    //  Worker Access
    public:
        bool GetWorkerInterface (IVReceiver<IVUnknown*>* pReceiver, Vca::U32 workerIdentifier);
        bool supplyOnlineWorkerFor (
            VPoolEvaluation const *pEvaluation, VReference<VPoolWorker> &rpWorker
        );
        bool supplyOfflineWorker (VReference<VPoolWorker> &rpWorker, Vca::U32 iWorkerId);
        bool getWorkerReference (VString const &rId, bool bIsPID, VReference<Worker> &rpWorker) const;

    private:
        bool getWorkerReference (VString const &rId, bool bIsPID, WorkerList iList, VReference<Worker> &rpWorker) const;
        bool getWorkerReference (VString const &rId, bool bIsPID, WIPList iList, VReference<Worker> &rpWorker) const;


    //  Broadcast
        void processBCObject (VPoolBroadcast *pBroadcast);


    //  List Management (Workers, WIPs, WorkerRequests)
    private:
        VReference<Worker>& getHead (WorkerList iList);
        VReference<Worker>& getTail (WorkerList iList);

        /**
         * Used to attach a worker to a worker list.
         *
         * For Online and Offline available worker lists, incrOnlineAvailWorkers() or incrOfflineAvailWorkers() is automatically called.
         *
         * @param[in] pWorker the worker to be attached.
         * @param[in] iList the list to attach pWorker to.
         */
        void attach (Worker *pWorker, WorkerList iList);

        /**
         * Used to detach a worker from a worker list.
         *
         * For Online and Offline available worker lists, decrOnlineAvailWorkers() or decrOfflineAvailWorkers() is automatically called.
         *
         * @param[in] pWorker the worker to be attached.
         * @param[in] iList the list to attach pWorker to.
         */
        void detach (Worker *pWorker, WorkerList iList);

        /**
         * Used to detach a worker from a worker list.
         *
         * Finds the appropriate worker and then delegates to detach(Worker,WorkerList).
         *
         * @param[in] iList the list to attach pWorker to.
         * @param[out] rpAvailableWorker the worker that was detached.
         * @param[in] iWorkerID the worker ID for the worker being requested. If zero, the first worker in the list is used.
         */
        bool detach (WorkerList iList, VReference<Worker> &rpAvailableWorker, Vca::U32 iWorkerId = 0);

        void attach (WIP *pWIP);
        void detach (WIP *pWIP);

        /**
         * Used to attach a worker request to this generation.
         *
         * Automatically calls incrWorkersBeingCreated().
         *
         * @param pRequest the request to be attached.
         */
        void attach (WorkerRequest *pRequest);

        /**
         * Used to detach a worker request to this generation.
         *
         * Automatically calls decrWorkersBeingCreated().
         *
         * @param pRequest the request to be detached.
         */
        void detach (WorkerRequest *pRequest);

        /**
         * Used to remove all references to retired workers so that they can be cleaned up.
         */
        void detachRetiredWorkers ();

    //  Statistics
    public:
        void getDetailedStat (VString &rResult) const;
        void getStatistics (VString &rResult) const;
        static void quickStatsHeader (VString &rResult);
        void quickStats (VString &rResult) const;
        void getWorkersStatistics (VString &rResult) const;
        void dumpWorkerStats (VString &rResult) const;
        void getWorkerLists (
            VString &rOnlineAvailList,
            VString &rOnlineWIPList,
            VString &rOfflineAvailList,
            VString &rOfflineWIPList,
            VString &rInCreationList,
            VString &rRetiredList
        ) const;

    //  Format List
    private:
        void formatWorkerList (VString &rInputList, VString &rOutputList, Vca::U32 cOutputLength) const;

    //  SnapShot
    public:
        void dumpWorkingWorkers(V::VSimpleFile &rFile) const;


    //  State
    private:
        Vca::U32                    m_iGeneration;
        Vca::U32                    m_iNextWorker;
        bool                        m_bHiring;

        VReference<VEvaluatorPool>  m_pPool;

        //  Available Workers
        VReference<Worker>          m_pOnlineAvailWorkerHead;
        VReference<Worker>          m_pOnlineAvailWorkerTail;

        VReference<Worker>          m_pOfflineAvailWorkerHead;
        VReference<Worker>          m_pOfflineAvailWorkerTail;

        //  Retired Workers
        VReference<Worker>          m_pRetiredWorkerHead;
        VReference<Worker>          m_pRetiredWorkerTail;

        //  Works in Progress
        VReference <WIP>            m_pOnlineWIPHead;
        VReference <WIP>            m_pOnlineWIPTail;

        VReference <WIP>            m_pOfflineWIPHead;
        VReference <WIP>            m_pOfflineWIPTail;

        //  Workers Being Created
        VReference<WorkerRequest>   m_pWorkerRequestHead;
        VReference<WorkerRequest>   m_pWorkerRequestTail;

        Vca::U32                    m_cOnlineWorkers;
        Vca::U32                    m_cOnlineAvailWorkers;
        Vca::U32                    m_cOnlineWIPs;
        Vca::U32                    m_cOnlineExcessWorkers;

        Vca::U32                    m_cWorkersBeingCreated;
        Vca::U32                    m_cDisabledWorkers;
        Vca::U32                    m_cWorkerCreationHardFailures;
        Vca::U32                    m_cWorkerCreationSoftFailures;
        Vca::U32                    m_cWorkersRetired;

        Vca::U32                    m_cOfflineWorkers;
        Vca::U32                    m_cOfflineAvailWorkers;
        Vca::U32                    m_cOfflineWIPs;
        Vca::U32                    m_cOfflineExcessWorkers;

        ThisClass                  *m_pSuccessor;
        ThisClass                  *m_pPredecessor;
    };
}


#endif
