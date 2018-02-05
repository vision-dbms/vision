#ifndef Vsa_VPoolWorker_Interface
#define Vsa_VPoolWorker_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VTime.h"
#include "V_VQueue.h"
#include "Vca_VTimer.h"
#include "Vca_VInterfaceEKG.h"
#include "Vsa_IEvaluatorClient.h"
#include "Vsa_IPoolEvaluation.h"
#include "Vsa_IPoolWorker.h"
#include "Vsa_VPathQuery.h"
#include "Vsa_VPoolEvaluation.h"

namespace Vsa {
    class IEvaluator;
    class VEvaluatorPool;
    class VPoolWorkerGeneration;
}

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    /**
     * Worker class represents control and state logic for vpool abstraction of pool worker (batchvision process).
     * For use with Vsa::VPoolWorkerGeneration and Vsa::VEvaluatorPool.
     *
     * @implements IEvaluatorClient
     * @todo Check if above implements line worked.
     */
    class VPoolWorker : public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VPoolWorker, Vca::VRolePlayer);

    //  Aliases
    public:
	typedef ThisClass Worker;
	typedef Reference WorkerReference;
	typedef VPoolWorkerGeneration WorkerGeneration;
	typedef Vca::VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;


    //  Status
    public:
	enum Worker_Status {
	    Worker_InStartup, /**< Worker has not yet been accepted into a generation. */
	    Worker_Available, /**< Worker is available for new work. */
	    Worker_InUse, /**< Worker has a work in progress. */
	    Worker_GoneFishing /**< Worker is retired. */
	};
	enum Worker_Mode {
	    Worker_Online, /**< Worker is online or has finished starting up. */
	    Worker_Offline, /**< Worker is offline */
	    Worker_Retired /**< Worker is retired. */
	};
        enum Worker_RetirementCause {
            Worker_Employed, /**< Worker is not retired. */
            Worker_Cutback, /**< Worker was retired of natural causes. */
            Worker_Failed /**< Worker encountered an error resulting in retirement. */
        };

    //  WorkInProgress
    public:
	class WorkInProgress : public Vca::VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (WorkInProgress, Vca::VRolePlayer);

	//  Friends
	    friend class VPoolWorker;
	    friend class VEvaluatorPool;
  	    friend class VPoolWorkerGeneration;
    
	//  Typedefs
	    typedef VPoolWorkerGeneration WorkerGeneration;

	//  State
	public:
	    enum State {
		State_New, 
		State_Active, 
		State_Failed, 
		State_Completed
	    };

	//  Construction
	private:
	    WorkInProgress (VPoolWorker *pWorker, VPoolEvaluation *pRequest);

	//  Destruction
	private:
	    ~WorkInProgress ();

        /**
         * @name IEvaluatorClient Implementation
         * WorkInProgress implements the IEValuatorClient interface. These methods support said implementation.
         * @{
         */
	//  IEvaluatorClient Role
	private:
	    /** The Vsa::IEvaluatorClient role for this WorkInProgress. */
	    Vca::VRole<ThisClass,IEvaluatorClient> m_pIEvaluatorClient;
	public:
	    /**
	     * Retrieves the IEvaluatorClient role for this WorkInProgress.
	     *
	     * @param[out] rpRole the reference that will be used to return the IEvaluatorClient ffor this WorkInProgress.
	     */
	    void getRole (IEvaluatorClient::Reference &rpRole);

	//  IEvaluatorClient Methods
	public:
            /** Implements Vsa::IEvaluatorClient::OnAccept(). */
	    void OnAccept (
		IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
	    );

            /** Implements Vsa::IEvaluatorClient::OnChange(). */
	    void OnChange (
		IEvaluatorClient *pRole, Vca::U32 xQueuePosition
	    );

            /** Implements Vsa::IEvaluatorClient::OnResult(). */
	    void OnResult (
		IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
	    );

	//  IClient Methods
            /** Implements Vca::IClient::OnError(). */
	    void OnError (
		Vca::IClient *pRole, Vca::IError *pError, VString const &rText
	    ) OVERRIDE;
        //@}

	//  TimeOut 
	public:
	    void onTimeOut (Vca::VTrigger<ThisClass> *pTrigger);

	//  Cancel query
	bool remoteIEvaluationCancelled () const {
	    return m_bRemoteIEvaluationCancelled;
	}
	//  Update
	
	void setRemoteIEvaluationCancelled(bool cancelled) {
	    m_bRemoteIEvaluationCancelled = cancelled;
	}
			
    //  OnUpdate
	public:
	    void onAnyDataUpdate ();
	    void onRetryableUpdate ();

	//  Execution
	private:
	    void start ();
	    bool isTimedOut () const {
		return m_bTimedOut;
	    }

	//  Utilites
	private:
	    bool setTimeOut	 ();

	    bool retryEvaluation (VString);
	public:
	    void cancelQuery() ;
		
	//  Access
	public:
	    Vca::U32 workerId () const {
			return m_pWorker->identifier ();
	    }
  	    VPoolWorker* worker () const {
 	        return m_pWorker;
	    }
	    VPoolEvaluation* evaluation () const {
     	    return m_pRequest;
	    }
	    IEvaluation* remoteIEvaluation () const {
		return m_pRemoteIEvaluation.referent ();
	    }
		

	//  Query
	public:
  	    bool isWorkerOnline () const {
		return m_pWorker->isOnline ();
	    }
  	    bool isOnlineWIP () const;

	    Worker_Mode workerMode () const {
		return m_pWorker->workerMode ();
	    }
	    Vca::U32 evaluationIndex () const;

	//  State
	private:
	    WorkerReference		const	m_pWorker;
	    WorkerGeneration		       *m_pGeneration;
	    VPoolEvaluation::Reference	const	m_pRequest;
	    State				m_xState;
	    VEvaluatorPool	       *const   m_pPool;
	    IEvaluation::Reference		m_pRemoteIEvaluation;

	    bool				m_bTimedOut;
	    bool 				m_bRemoteIEvaluationCancelled;
	    Vca::VTimer::Reference		m_pTimer;
	    Reference				m_pSuccessor;
	    Reference				m_pPredecessor;
	};

    //  CacheUnit
    class CacheUnit : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (CacheUnit, VReferenceable);
          
        public:
	    CacheUnit (VPoolEvaluation *pRequest);

        private:
	    ~CacheUnit ();

        public:
	    VPathQuery* getPathQuery () const {
		return m_pRequest->query();
	    }

	    /**
	     * Returns the query's pool index
	     */
	    unsigned int getQueryId () const {
		return m_pRequest->index();
	    }

            /**
             * Retrieves start time, copying into provided VTime reference.
             * 
             * @param rStartTime reference to the VTime object in which to store the start time.
             * @return False if no start time was set, true otherwise.
             */
	    bool getStartTime (V::VTime &rStartTime) const {
		return m_pRequest->getStartTime(rStartTime);
	    }

            /**
             * Retrieves end time, copying into provided VTime reference.
             * 
             * @param rEndTime reference to the VTime object in which to store the end time.
             * @return False if no end time was set, true otherwise.
             */
	    bool getEndTime (V::VTime &rEndTime) const {
		return m_pRequest->getEndTime(rEndTime);
	    }
		
	    /**
             * Retrieves arrival time, copying into provided VTime reference.
             * 
             * @param rArrivalTime reference to the VTime object in which to store the start time.
             * @return False if no arrival time was set, true otherwise.
             */
	    bool getArrivalTime (V::VTime &rArrivalTime) const {
		return m_pRequest->getArrivalTime(rArrivalTime);
	    }

	    int queueTime() const {
		return m_pRequest->queueTime();
	    }

	    int evaluationTime() const {
		return m_pRequest->evaluationTime();
	    }
          
        private:
	    VPoolEvaluation::Reference m_pRequest;

            /**
             * Query processing start time.
             */
	    V::VTime m_iStartTime;
            
            /**
             * Query processing end time.
             */
            V::VTime m_iEndTime; 
			
	    /**
	     * Query arrival time.
	     */
	    V::VTime m_iArrivalTime;
	};

	typedef V::VQueue<CacheUnit::Reference> WorkerHistoryQueue;        
      
	//  Friends
	friend class VPoolWorkerGeneration;
	friend class VEvaluatorPool;
	friend class WorkInProgress;

	//  Construction
	public:
	    VPoolWorker (
		Vca::U32 iIdentifier, VString const &rPID,
		VPoolWorkerGeneration *pGeneration,
		IEvaluator *pEvaluator
	    );

	//  Destruction
	private:
	    ~VPoolWorker ();

	//  IPoolWorker Role
	private:
	    Vca::VRole<ThisClass,IPoolWorker> m_pIPoolWorker;
	public: 
	    void getRole  (VReference<IPoolWorker>&rpRole);

	//  IPoolWorker Methods
	public:
	    void GetStatistics (IPoolWorker *pRole, IVReceiver<VString const&>* pReceiver);

	//  Execution
	public:
	    void evaluate (IEvaluatorClient *pClient, VPoolEvaluation *pRequest);
	    void process  (VPoolEvaluation *pRequest);

	//  Linkage
	public:
	    void removeFromGeneration ();
	    void returnToGeneration ();
	    void disable (); 

	//  Access
	public:
	    Vca::U32 identifier () const {
			return m_iIdentifier;
	    }
	    VString const & pid () const {
			return m_iPID;
	    }
	    Worker_Status status () const {
			return m_iStatus;
	    }
	    Worker_Mode workerMode () const {
  	        return m_iMode;
	    }
	    Vca::U32 queriesProcessed () const {
		return m_cQueriesProcessed;
	    }
	    Vca::U64 totalQueryTime () const {
		return totalQueueTime() + totalEvalTime(); 	//  milli seconds
	    }
	    Vca::U64 totalQueueTime () const {
		return m_cTotalQueueTime; 	// milli seconds
	    }
	    Vca::U64 totalEvalTime () const {
		return m_cTotalQueryTime;
	    }
	    VEvaluatorPool *pool () const {
		return m_pPool;
	    }
	    bool isAboutToRetire () const {
		return m_bAboutToRetire;
	    }
	    bool getToggleMode () const {
		return m_bToggleMode;
	    }
	    VString const& getPID () const { 
		return m_iPID;
	    }

	//  Query
	public:
	    Vca::U32      generationId  () const;
	    VString	  createTime () const;
	    VString	  retireTime () const;
	    IEvaluator*   evaluator () const;
	    WorkerGeneration *generation () const;
	    bool  isFlushed () const;
	    bool  canProcess (VPoolEvaluation const *pEvaluation) const;
	    void  getWorkerStatus (VString &rStatus) const;
	    void  getWorkerMode (VString &rMode) const;
	    void  getWorkerRetirementCause (VString &rRetirementCause) const;
	    char const *   getWorkerMode () const;
	    Vca::U32  getHistoryLength () const;

	    bool isOnline () const {
		return m_iMode == Worker_Online; 
	    }
	    bool isOffline () const { 
		return m_iMode == Worker_Offline;
	    }
	    bool hasId (VString const &rId, bool bIsPID) const;
	    void getQueryHistory (VString &rHistory, Vca::S32 cQueryLines=-1, Vca::S32 cQueries=-1) const;
	    
	//  Update
	public:
	    void setStatus (Worker_Status iStatus) {
		m_iStatus = iStatus;
	    }
	    void updateQueryStatistics (Vca::U32 cQueries, Vca::U64 iQueryTime, Vca::U64 iQueueTime) {
		m_cQueriesProcessed += cQueries;
		m_cTotalQueryTime += iQueryTime;
		m_cTotalQueueTime += iQueueTime;
	    }
	    void pushCacheUnit (VPoolEvaluation *pRequest);
	    void setLastQuery (VEvaluation *pRequest);

	    void setOnlineMode () {
		m_iMode = Worker_Online;
	    }
	    void setOfflineMode () {
		m_iMode = Worker_Offline;
	    }
	    void setRetiredMode () {
		m_iMode = Worker_Retired;
	    }
	    void setToggleMode () {
		m_bToggleMode = true;
	    }
	    void resetToggleMode () {
		m_bToggleMode = false;
	    }

	    /**
	     * Sets this worker to a retired state.
	     * Empties the worker of its IEvaluator reference and sets internal state variables to reflect retirement reason, etc.
	     * Should really only be called from Vsa::VPoolWorkerGeneration::retire() so that generation-specific cleanup can occur first.
	     */
	    void retire (Worker_RetirementCause iCause);
 
	//  Statistics
	    void  getStatistics (VString &rResult, Vca::S32 cQueryLines=-1, Vca::S32 cQueries=-1) const;
	    void  getStat (VString &rResult) const;
	    void  quickStats (VString &rResult) const;
	    static void quickStatsHeader(VString &rResult);


	//  Snapshot
	public:
	    void dumpHistory (VString const &rFile, IVReceiver <VString const &>*pClient); 

        //  EKG
        public:
            void signalIEvaluatorEKG ();
        private:
            void cancelIEvaluatorEKG ();

	//  State
	private:
	    VString				m_iPID;
	    Vca::U32			const	m_iIdentifier;
	    VReference<IEvaluator>	        m_pEvaluator;
	    VReference<WorkerGeneration>	m_pGeneration;
	    VEvaluatorPool		*const	m_pPool;

	    /**
	     * Thread creation time.
	     */
	    V::VTime m_iCreateTime;

	    /**
	     * Thread retirement time.
	     */
	    V::VTime m_iRetireTime;

	    Vca::U32				m_cQueriesProcessed;
	    Vca::U64				m_cTotalQueryTime;
	    Vca::U64				m_cTotalQueueTime;
	    Worker_Status			m_iStatus;
	    Worker_Mode				m_iMode;
	    Worker_RetirementCause              m_iRetirementCause;
	    VString				m_iLastQuery;
	    WorkerHistoryQueue			m_iHistoryQueue;
	    VReference<CacheUnit>               m_pCurrentCacheUnit;

	    /**
	     * The currently executing WorkInProgress.
	     */
	    WorkInProgress::Reference           m_pWIP;

	    /**
	     * Keeps track of the IEvaluator interface, notifies us upon disconnect.
	     */
	    interface_monitor_t::Reference      m_pIEvaluatorEKG;

	    Reference				m_pSuccessor;
	    Reference				m_pPredecessor;

            /**
             * Used to indicate that the worker mode should be toggled at the end of the currently executing query.
             */
        bool                                m_bToggleMode;
	bool				m_bAboutToRetire;
    };
}


#endif
