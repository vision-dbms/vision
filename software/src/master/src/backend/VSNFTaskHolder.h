#ifndef VSNFTaskHolder_Interface
#define VSNFTaskHolder_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

#include "Vxa_ICaller.h"
#include "Vxa_IVSNFTaskHolder2.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTrigger.h"
#include "Vca_VTriggerTicket.h"

#include "VSNFTask.h"

namespace Vxa {
    class ICollection;
    class ISingleton;
}

/*************************
 *****  Definitions  *****
 *************************/

class VSNFTaskHolder : public Vca::VActivity {
    DECLARE_CONCRETE_RTT (VSNFTaskHolder, VActivity);

//  Aliases
public:
    typedef Reference TaskHolderReference;
    typedef Vca::VTrigger<ThisClass> Trigger;

    typedef Vxa::cardinality_t cardinality_t;

    typedef Vxa::object_reference_t object_reference_t;
    typedef Vxa::object_reference_array_t object_reference_array_t;

    typedef Vxa::ICaller ICaller;
    typedef Vxa::ICollection ICollection;
    typedef Vxa::ISingleton ISingleton;

    typedef Vxa::IVSNFTaskHolder IVSNFTaskHolder;
    typedef Vxa::IVSNFTaskHolder1 IVSNFTaskHolder1;
    typedef Vxa::IVSNFTaskHolder2 IVSNFTaskHolder2;

    typedef Vxa::IVSNFTaskImplementation IVSNFTaskImplementation;

    typedef IVSNFTaskHolder2 IVSNFTaskHolder_T;
    typedef IVSNFTaskHolder2::BaseClass IVSNFTaskHolderNC_T;

//  Scheduler
public:
    class Scheduler {
	DECLARE_NUCLEAR_FAMILY (Scheduler);

    //  class Entry
    public:
	class Entry : public VReferenceable {
	    DECLARE_ABSTRACT_RTTLITE (Entry, VReferenceable);

	    friend class Scheduler;

	//  Construction
	protected:
	    Entry (VSNFTaskHolder *pTaskHolder);

	//  Destruction
	protected:
	    ~Entry ();

	//  Linkage
	private:
	    bool setSuccessorTo (ThisClass *pSuccessor) {
		return m_pSuccessor.interlockedSetIfNil (pSuccessor);
	    }

	//  Execution
	protected:
	    template <typename T> void startIt (T *pT) const { // that's what friends are for :-)
		m_pTaskHolder->start (pT);
	    }
	public:
	    virtual void start () const = 0;

	//  State
	private:
	    TaskHolderReference const m_pTaskHolder;
	    Reference m_pSuccessor;
	};

    //  template <typename T> class TEntry
    public:
	template <typename T> class TEntry : public Entry {
	    DECLARE_CONCRETE_RTTLITE (TEntry, Entry);

	//  Construction
	public:
	    TEntry (VSNFTaskHolder *pTaskHolder, T *pT) : BaseClass (pTaskHolder), m_pT (pT) {
	    }

	//  Destruction
	protected:
	    ~TEntry () {
	    }

	//  Execution
	public:
	    void start () const {
		startIt (m_pT.referent ());
	    }

	//  State
	private:
	    typename T::Reference const m_pT;
	};

    //  Construction
    public:
	Scheduler (unsigned int sConcurrentTaskLimit = 1);

    //  Destruction
    public:
	~Scheduler () {
	}

    //  Concurrency
    public:
	unsigned int getConcurrentTaskLimit () const {
	    return m_sConcurrentTaskLimit;
	}
	void setConcurrentTaskLimit (unsigned int sLimit) {
	    m_sConcurrentTaskLimit = sLimit > 0 ? sLimit : 1;
	}

    //  Linkage
    private:
	void insertTail (Entry *pEntry);
	bool removeHead (Entry::Reference &rpEntry);

    //  Scheduling
    private:
	void startNextTask ();
    public:
	template <typename T> void schedule (VSNFTaskHolder *pTaskHolder, T *pT) {
	    if (m_cRunningTasks < m_sConcurrentTaskLimit)
		pTaskHolder->start (pT);
	    else {
		insertTail (new TEntry<T> (pTaskHolder, pT));
	    }
	}
	void incrementTaskCount () {
	    m_cRunningTasks++;
	}
	void decrementTaskCount () {
	    if (0 == --m_cRunningTasks)
		startNextTask ();
	}

    //  State
    private:
	unsigned int m_sConcurrentTaskLimit;
	unsigned int m_cRunningTasks;
    
	Entry::Reference m_pQueueHead;
	Entry::Reference m_pQueueTail;
    };
    friend class Scheduler::Entry;
    friend class Scheduler;

//  Construction
public:
    template <typename T> VSNFTaskHolder (
	VSNFTask *pSNFTask, T *pT
    ) : m_pSNFTask (pSNFTask), m_pICaller (this), m_pIVSNFTaskHolder (this), m_pIVSNFTaskHolderNC (this) {
	retain (); {
	    onStart ();
	    g_iScheduler.schedule (this, pT);
	} untain ();
    }

//  Destruction
private:
    ~VSNFTaskHolder ();

//  Helpers
private:
    template <typename Source_T, typename Result_T> void ReturnArray (
	VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
    );
    template <typename Source_T> void ReturnArray (
	VkDynamicArrayOf<Source_T> const &rSourceArray
    );

    template <typename Source_T, typename Result_T> void ReturnSegment (
	object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
    );
    template <typename Source_T> void ReturnSegment (
	object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray
    );

    template <typename Source_T> void ReturnSingleton (Source_T iSingleton);

//  Roles
public:
    using BaseClass::getRole;

//  IVSNFTaskHolder_T Role
private:
    Vca::VRole<ThisClass,IVSNFTaskHolder_T> m_pIVSNFTaskHolder;
public:
    void getRole (IVSNFTaskHolder_T::Reference &rpRole) {
	m_pIVSNFTaskHolder.getRole (rpRole);
    }

//  IVSNFTaskHolderNC_T Role
private:
    Vca::VRole<ThisClass,IVSNFTaskHolderNC_T> m_pIVSNFTaskHolderNC;
public:
    void getRole (IVSNFTaskHolderNC_T::Reference &rpRole) {
	m_pIVSNFTaskHolderNC.getRole (rpRole);
    }

//  IVSNFTaskHolder Methods
public:
    void ReturnImplementationHandle (
	IVSNFTaskHolder *pRole, 
	IVSNFTaskImplementation *pImplementation, 
	VkDynamicArrayOf<ISingleton::Reference> const &
    );
    void GetParameter 		(IVSNFTaskHolder *pRole, unsigned int);
    void ReturnBooleanArray	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<bool> const &);
    void ReturnIntegerArray 	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<int> const &);
    void ReturnFloatArray	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<float> const &);
    void ReturnDoubleArray 	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<double> const &);
    void ReturnStringArray 	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<VString> const &);
    void ReturnObjectArray 	(IVSNFTaskHolder *pRole, VkDynamicArrayOf<ISingleton::Reference> const &);
    void TurnBackSNFTask 	(IVSNFTaskHolder *pRole);
    void ReturnInteger 		(IVSNFTaskHolder *pRole, int);
    void ReturnString 		(IVSNFTaskHolder *pRole, char const *);
    void SetOutput 		(IVSNFTaskHolder *pRole, VkDynamicArrayOf<VString> const &);

//  IVSNFTaskHolder1 Methods
public:
    void ReturnError		(IVSNFTaskHolder1 *pRole, const VString &rMessage);

//  IVSNFTaskHolder2 Methods
public:
    void ReturnDouble		(IVSNFTaskHolder2 *pRole, double);
    void ReturnS2Booleans	(IVSNFTaskHolder2 *pRole, Vxa::bool_s2array_t const&);
    void ReturnS2Integers	(IVSNFTaskHolder2 *pRole, Vxa::i32_s2array_t const&);

//  ICaller Role
private:
    Vca::VRole<ThisClass,ICaller> m_pICaller;
public:
    void getRole (ICaller::Reference &rpRole) {
	m_pICaller.getRole (rpRole);
    }

//  ICaller Methods
public:
    void GetSelfReference (ICaller *pRole, IVReceiver<object_reference_t> *pResultSink);
    void GetSelfReferences (ICaller *pRole, IVReceiver<object_reference_array_t const&> *pResultSink);

    void ReturnObjectReference (ICaller *pRole, ICollection *pCluster, object_reference_t sCluster, object_reference_t xObject);
    void ReturnObjectReferences (ICaller *pRole, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects);

    void ReturnS08Array (ICaller *pRole, VkDynamicArrayOf<char> const &rData);
    void ReturnU08Array (ICaller *pRole, VkDynamicArrayOf<unsigned char> const &rData);

    void ReturnS16Array (ICaller *pRole, VkDynamicArrayOf<short> const &rData);
    void ReturnU16Array (ICaller *pRole, VkDynamicArrayOf<unsigned short> const &rData);

    void ReturnS32Array (ICaller *pRole, VkDynamicArrayOf<int> const &rData);
    void ReturnU32Array (ICaller *pRole, VkDynamicArrayOf<unsigned int> const &rData);

    void ReturnS64Array (ICaller *pRole, VkDynamicArrayOf<__int64> const &rData);
    void ReturnU64Array (ICaller *pRole, VkDynamicArrayOf<unsigned __int64> const &rData);

    void ReturnNA (ICaller *pRole);
    void ReturnNASegment (ICaller *pRole, object_reference_array_t const &rInjector);

    void ReturnBooleanSegment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<bool> const &rData);

    void ReturnS08Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<char> const &rData);
    void ReturnU08Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned char> const &rData);

    void ReturnS16Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<short> const &rData);
    void ReturnU16Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned short> const &rData);

    void ReturnS32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<int> const &rData);
    void ReturnU32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned int> const &rData);

    void ReturnS64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<__int64> const &rData);
    void ReturnU64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned __int64> const &rData);

    void ReturnF32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<float> const &rData);
    void ReturnF64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<double> const &rData);

    void ReturnStringSegment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<VString> const &rData);
    void ReturnObjectSegment (ICaller *pRole, object_reference_array_t const &rInjector, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rData);

    void ReturnSegmentCount (ICaller *pRole, cardinality_t cSegments);

//  Access/Query
protected:
    void getDescription_(VString& rResult) const;
public:
    unsigned int taskCardinality () const {
	return m_pSNFTask ? m_pSNFTask->cardinality () : 0;
    }
    unsigned int taskId () const {
	return m_pSNFTask ? m_pSNFTask->tuid32 () : 0;
    }

//  Monitoring and Control
private:
    void start (ISingleton *pExternalObject);
    void start (ICollection *pExternalObject);

    void onDeadPeer (Trigger *pTrigger);
    void onEnd ();
    void onError (Vca::IError *pInterface, VString const &rMessage);
    void onSent ();

    void wrapup ();

//  Utilities
private:
    void signalParameterError (
	IVSNFTaskHolder *pRole, IVSNFTaskImplementation *pImplementation, unsigned int xParameter, VString const & rError
    );


//  Adapter type
public:
    static unsigned int g_iAdapterType;

//  State
private:
    static Scheduler g_iScheduler;

    VSNFTask::Reference m_pSNFTask;
    Vca::VTriggerTicket::Reference m_pNoRouteToPeerTriggerTicket;
};


#endif
