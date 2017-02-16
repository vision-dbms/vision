#ifndef Vsa_VEvaluator_Interface
#define Vsa_VEvaluator_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vca_VTrackableCounter.h"

#include "V_VTime.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IEvaluator_Ex1.h"
#include "Vsa_IEvaluatorControl.h"
#include "Vsa_IEvaluatorSettings.h"
#include "Vsa_IUpDownPublisher.h"
#include "Vsa_ISubscription.h"
#include "Vsa_IUpDownSubscriber.h"
#include "Vsa_VOdometer.h" 

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VEvaluation;
    class ISubscription;

    class Vsa_API VEvaluator : public Vca::VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VEvaluator, Vca::VRolePlayer);

    //  Aliases
    public:
	typedef Reference EvaluatorReference;

    class UpDownSubscriber : public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (UpDownSubscriber, Vca::VRolePlayer);
    
        //  Friend
	friend class VEvaluator;
    
        //  Construction
      public:
	UpDownSubscriber (VEvaluator *pEvaluator, IUpDownSubscriber *pSubscriber);
    
	//  Destruction
      private:
	~UpDownSubscriber ();
    
	//  Access
      public:
	IUpDownSubscriber* getIUpDownSubscriber () const {
	  return m_pSubscriber;
	}
    
	//  ISubscription Role
      private:
	Vca::VRole<ThisClass,ISubscription> m_pISubscription;
      public:
	void getRole (VReference<ISubscription>&rpRole) {
	  m_pISubscription.getRole (rpRole);
	}
    
	//  Interface Implementations
      public:
	void Unsubscribe (ISubscription *pRole);
    
	//  State
      private:
	EvaluatorReference		m_pEvaluator;
	IUpDownSubscriber::Reference    m_pSubscriber;
	bool                             m_bIsSubscribed;
	Pointer				m_pPredecessor;
	Pointer				m_pSuccessor;
    };


    //  Typedef
        typedef UpDownSubscriber Subscriber;
        typedef IVReceiver<ISubscription*> ISubscriptionSink;

    //  Friends
        friend class UpDownSubscriber;
	friend class VEvaluation;

    //  Status
	enum Status {
	    Status_Running,
	    Status_Suspended,
	    Status_Stopping,
	    Status_Stopped
	};

    //  Queue
    public:
	class Queue {
	    DECLARE_NUCLEAR_FAMILY (Queue);

	//  Aliases
	public:
	    typedef V::counter32_t counter_t;
	    typedef counter_t::value_t count_t;

	    typedef Vca::VTrackableCounter<counter_t> trackable_counter_t;
	    typedef Vca::VTrackable<count_t> trackable_count_t;

	//  Entry
	public:
	    class Iterator;
	    class Entry : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Entry, VReferenceable);

	    //  Friends
		friend class Queue;
		friend class Iterator;

	    //  Construction
	    public:
		Entry (VEvaluation *pRequest);

	    //  Destruction
	    private:
		~Entry ();

	    //  Access
	    public:
		VEvaluation *request () const {
		    return m_pRequest;
		}
		unsigned int requestIndex () const;

	    //  Linkage
	    private:
		void unlink (Queue *pQueue, VReference<VEvaluation> &rpEvaluation);

	    //  State
	    protected:
		VReference<VEvaluation>	const	m_pRequest;
		Reference			m_pPredecessor;
		Reference			m_pSuccessor;
	    };

	//  Iterator 
	public:
	    class Iterator {
	    public:
		Iterator (Queue &rQueue) : m_rQueue (rQueue), m_xElement (0) {
		}   
		
	    public:
		bool isNotAtEnd () {
		    return m_xElement < m_rQueue.length ();
		}
		Vca::U32 index () const {
		    return m_xElement;
		}
		Iterator& operator++ () {
    		    if (m_xElement < m_rQueue.length ())
    			m_xElement++;
		    return *this;
		}
		bool Delete () {
		    if (isNotAtEnd ()) {
			m_rQueue.unlink ( operator*()->request ());
			return true;
		    }
		    return false;
		}

		Entry const *operator* () const;

	    protected:
		Queue &m_rQueue;
		count_t m_xElement;
	    };

	//  Friends
	    friend class Entry;
	    friend class Iterator;

	//  Construction
	public:
	    Queue () {
	    }

	//  Destruction
	public:
	    ~Queue () {
	    }

	//  Access
	public:
	    trackable_count_t const &lengthTrackable () {
		return m_sQueue.trackable ();
	    }
	    count_t length () const {
		return m_sQueue;
	    }
	    count_t wait () const {
		return 0;
	    }

	//  Query
	public:
	    bool empty () const {
		return m_pHead.isNil ();
	    }
	    bool contains (VEvaluation *pRequest, VReference<Entry> &rpEntry) const;

	//  Linkage
	public:
	    void enqueue (VEvaluation *pRequest);
	    void dequeue (VReference<VEvaluation> &rpRequest);

	    bool unlink (VEvaluation *pRequest);

	//  State
	private:
	    Entry::Reference	m_pHead;
	    Entry::Reference	m_pTail;
	    trackable_counter_t m_sQueue;
	};
	typedef Queue::count_t count_t;
	typedef Queue::trackable_count_t trackable_count_t;


    //  Settings
    public:
	class Vsa_API Settings : public Vca::VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Settings, Vca::VRolePlayer);

	//  Construction
	protected:
	    Settings (Settings const &rOther) 
		: m_cEvaluationTimeOut (rOther.m_cEvaluationTimeOut)
		, m_pIEvaluatorSettings (this) {
	    }
	    Settings ()
		: m_cEvaluationTimeOut (U64_MAX)
		, m_pIEvaluatorSettings (this) {
	    }

	//  Destruction
	protected:
	    ~Settings () {
	    }

	//  IEvaluatorSettings Role
	private:
	    Vca::VRole<ThisClass,IEvaluatorSettings> m_pIEvaluatorSettings;
	public:
	    void getRole (VReference<IEvaluatorSettings>&rpRole);

	//  IEvaluatorSettings Methods
	public:
	    void GetEvaluationTimeOut (IEvaluatorSettings *pRole, IVReceiver<Vca::U64>*);
	    void SetEvaluationTimeOut (IEvaluatorSettings *pRole, Vca::U64);
	
	//  Update
	public:
	    void setEvaluationTimeOutTo (Vca::U64 sTimeOut) {
		m_cEvaluationTimeOut = sTimeOut;
	    }

	//  Access
	public:
	    Vca::U64  evaluationTimeOut () const {
		return m_cEvaluationTimeOut;
	    }
	    Vca::U64  evaluationTimeOutInSecs () const {
		return m_cEvaluationTimeOut/1000000;
	    }
            void dumpSettings (VString &rResult) const;

	//  State
	protected:
	    Vca::U64	m_cEvaluationTimeOut;	//  microseconds
	};


    //  Construction
    protected:
	VEvaluator ();

    //  Destruction
    protected:
	~VEvaluator ();

    //  IEvaluator Role
    private:
	Vca::VRole<ThisClass,IEvaluator_Ex1> m_pIEvaluator;
    public:
	void getRole (VReference<IEvaluator_Ex1>&rpRole);

    //  IUpDownPublisher Role
    private:
	Vca::VRole<ThisClass,IUpDownPublisher> m_pIUpDownPublisher;
    public:
	void getRole (VReference<IUpDownPublisher>&rpRole);

    //  IEvaluator Methods:
    public:
	void EvaluateExpression (
	    IEvaluator*		pRole,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rExpression
	);

	void EvaluateURL (
	    IEvaluator*		pRole,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    VString const&	rEnvironment
	);

        void EvaluateExpression_Ex (
            IEvaluator*		pRole,
            IEvaluatorClient*	pClient,
            VString const&	rPath,
            VString const&	rExpression,
            VString const&      rID,
            VString const&      rCID
        );

        void EvaluateURL_Ex (
            IEvaluator*		pRole,
            IEvaluatorClient*	pClient,
            VString const&	rPath,
            VString const&	rQuery,
            VString const&	rEnvironment,
            VString const&      rID,
            VString const&      rCID
        );

    //  IEvaluatorControl Methods:

    //  As of now the implementation of operations like 
    //	Suspend, Resume, Restart, Stop are independent of
    //  each other. Issue to be thought. 

    public:
	void GetEvaluator (IEvaluatorControl*, IEvaluatorSink*);
	void Suspend (IEvaluatorControl*, IVReceiver<bool>*);
	void Resume (IEvaluatorControl*, IVReceiver<bool>*);
	void Restart (IEvaluatorControl*, IVReceiver<bool>*);
	void Stop    (IEvaluatorControl*, IVReceiver<bool>*);
	void GetStatistics (IEvaluatorControl*, IVReceiver<VString const &>*);
	void GetWorker	   (IEvaluatorControl*, IVReceiver<IVUnknown*>*, Vca::U32);

    //  IUpDownPublisher Methods:
    public:
      void Subscribe (IUpDownPublisher* pRole, 
		      IUpDownSubscriber *pSubscriber,
		      ISubscriptionSink* pSubscriptionSink
	);

    //  Access
    public:
	trackable_count_t const &queueLengthTrackable () {
	    return m_iQueue.lengthTrackable ();
	}
	
	trackable_count_t const &busynessTrackable () {
//	    return busyness();
//  make busyness a real trackable when it is more than just queue length
	    return m_iQueue.lengthTrackable ();
	}

	count_t queueLength () const {
	    return m_iQueue.length () + (m_pThisRequest.isntNil () ? 1 : 0);
	}
	count_t queueWait () const {
	    return m_iQueue.wait ();
	}
	VEvaluation *thisRequest () const {
	    return m_pThisRequest;
	}

    //  Evaluation
    public:
	void evaluate (
	    IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
	);
	void evaluate (
	    IEvaluatorClient* pClient,
	    VString const& rPath, VString const& rQuery, VString const& rEnvironment
	);
    protected:
	virtual VEvaluation* createEvaluation (
	    IEvaluatorClient* pClient, VString const& rPath, VString const& rExpression
	);
	virtual VEvaluation* createEvaluation (
	    IEvaluatorClient* pClient,
	    VString const& rPath, VString const& rQuery, VString const& rEnvironment
	);

    //  Execution
    protected:
	void onError (Vca::IError *pError, VString const &rMessage);
	void onQueue ();
	virtual void onQueue_ () = 0;

    private:
	virtual void onRequest (VEvaluation *pRequest) {
	}

    protected:
	void schedule (VEvaluation *pRequest);

    protected:
	VEvaluation *nextRequest ();

    //  Cancellation
    public:
        bool cancel (VEvaluation *pEvaluation);
    protected:
        virtual bool cancel_ (VEvaluation *pEvaluation) { return false; };

    //  Request Index Allocation
    private:
	Vca::U32 newRequestIndex () {
	    return m_xNextRequest++;
	}

    //  Query
    public:
	Vca::U32 busyness ();
	
	Vca::U32 queriesProcessed () const {
	    return m_cQueriesProcessed;
	}
	Vca::U64 totalQueryTime () const {
	    return m_cTotalQueryTime + m_cTotalQueueTime; 	//  milli seconds
	}
	Vca::U64 totalQueueTime () const {
		return m_cTotalQueueTime;
	}
	Vca::U64 totalEvalTime () const {
		return m_cTotalQueryTime;
	}
	Status status () const {
	    return m_iStatus;
	}
	V::VTime const &started () const {
	    return m_iStarted;
	}
	V::VTime const &lastRestarted () const {
	    return m_iLastRestarted;
	}
	V::VTime const &lastUsed () const {
	    return m_iLastUsed;
        }
	bool isRunning () const {
	    return m_iStatus == Status_Running;
	}
	bool isntRunning () const {
	    return !isRunning ();
	}
	bool isStopping () const {
	    return m_iStatus == Status_Stopping;
	}
	bool isStopped () const {
	    return m_iStatus == Status_Stopped;
	}
	bool isSuspended () const {
	    return m_iStatus == Status_Suspended;
	}
	bool isOperational () const {
	    return m_iStatus == Status_Running || 
		   m_iStatus == Status_Stopping;
	}
	bool isntOperational () const {
	    return ! isOperational ();
	}
	void getStatistics (VString &rResult) const;
        void dumpStats (VString &rResult) const;

        Vca::U32 subscriberCount () const {
            return m_cSubscribers;
        }


    //  Update
    public:
      void updateQueryStatistics (Vca::U32 cQueries, Vca::U64 iQueryTime, Vca::U64 iQueueTime);
      
    //  Control
    protected:
	void suspend ();
	void resume  ();
	void stop    ();
	void restart ();
	void hardstop ();

    //  Subscription Impl 
    private:
      void notifyUpDownSubscriber (VReference<UpDownSubscriber> pSubscriber, bool bEvaluatorUp) const;
      void notifyAllUpDownSubscribers (bool bEvaluatorUp) const;

    //  Subscriber Linkage
    private:
	void link (Subscriber *const pSubscriber);
	void unlink (Subscriber *const pSubscriber);

    //  State
    protected:
	Vca::U32		m_xNextRequest;
	Queue			m_iQueue;
    bool                     m_bProcessingQueue;
	VReference<VEvaluation>	m_pThisRequest;

	VOdometer<Vca::U32>		m_cQueriesProcessed;
	VOdometer<Vca::U64>		m_cTotalQueryTime;
	VOdometer<Vca::U64>		m_cTotalQueueTime;
	Status			m_iStatus;
	V::VTime    const	m_iStarted;
	V::VTime		m_iLastRestarted;
	V::VTime		m_iLastUsed; // for evaluating queries
      
	Subscriber::Pointer     m_pSubscriberList;
    Vca::U32                m_cSubscribers;
    };
}


#endif
