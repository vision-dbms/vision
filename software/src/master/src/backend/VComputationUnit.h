#ifndef VComputationUnit_Interface
#define VComputationUnit_Interface

/************************
 *****  Components  *****
 ************************/

#include "VInternalGroundStore.h"

#include "VComputationTrigger.h"

#include "Vca_VActivity.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "IOMDriver.h"

#include "vsignal.h"

#include "VComputationScheduler.h"

#include "VControlPoint.h"

#include "VDescriptor.h"

class VCall;
class VComputationTrigger;

class VMutex;

class VSNFTask;
class VSelector;
class VSuspension;
class VTask;

class VWaitTaskTrigger;

/****************/
#include "Vxa_ICollection.h"
#include "Vxa_ISingleton.h"

#include "Vsa_VEvaluation.h"

#include "Vca_IDirectory.h"
#include "Vca_VcaGofer.h"
/****************/


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VComputationUnit : public VInternalGroundStore {
    DECLARE_ABSTRACT_RTT (VComputationUnit, VInternalGroundStore);

//  Aliases
public:
    typedef Vdd::Store Store;
    typedef V::VString VString;

//  Friends
    friend class VComputationList;
    friend class VComputationScheduler;
    friend class VComputationScheduler::Queue;
    friend class VComputationTrigger;

    friend class VMutex;

    friend class VCall;
    friend class VTask;
    friend class VSuspension;

    friend class VWaitTaskTrigger;

//  Meta Maker
protected:
    static void MetaMaker ();

//  Return Case Enumeration
/*---------------------------------------------------------------------------*
 *  Elements of the 'ReturnCase' enumeration denote the value returned
 *  by a call.  Valid values are:
 *
 *	Return_Unspecified	- 'commitCall' has not yet been called.
 *	Return_Intension	- the call is returning its implementation.
 *	Return_Value		- the call is returning its value.
 *	Return_Current		- the call is returning ^current.
 *---------------------------------------------------------------------------*/
public:
    enum ReturnCase {
	Return_Unspecified,
	Return_Intension,
	Return_Value,
	Return_Current
    };
    static char const *NameFor (ReturnCase xReturnCase);

//  Stage Enumeration
public:
    enum Stage {
	Stage_Unrun,
	Stage_Entry,
	Stage_Running,
	Stage_Exit,
	Stage_Completed
    };
    static char const *NameFor (Stage xStage);

//  State Enumeration
public:
    enum State {
	State_Runnable,
	State_Interrupted,
	State_Damaged,
	State_Failed,
	State_Completed
    };
    static char const *NameFor (State xState);


/****************************************************************************************/
//  class VComputationUnit::Context
/****************************************************************************************/
/*----------------------------------------------------------------------*
 *  Computation contexts provide a shallow-binding mechanism useful	*
 *  for caching frequently accessed and infrequently rebound stack	*
 *  based execution state.  They are just an optimization.  Without	*
 *  them, every piece of information they provide could be supplied	*
 *  by 'VComputationUnit' based virtual functions whose overrides	*
 *  provide the required state and whose default implementations	*
 *  delegate the request to their caller.				*
 *----------------------------------------------------------------------*/
public:
    class Context {
	DECLARE_NUCLEAR_FAMILY (Context);

/****************************************************************************************/
//  class VComputationUnit::Context::Query
/****************************************************************************************/
    public:
	class Query : public Vca::VActivity {
	    DECLARE_CONCRETE_RTTLITE (Query, VActivity);

	//  Aliases
	public:
	    typedef Vxa::ICollection ICollection;
	    typedef Vxa::ISingleton  ISingleton;

	    typedef Vca::IDataSource<ICollection*> ICollectionSource;
	    typedef Vca::IDataSource<ISingleton*>  ISingletonSource;

	    typedef Vca::VGoferInterface<ICollection> icollection_gofer_t;
	    typedef Vca::VGoferInterface<ISingleton>  isingleton_gofer_t;
	
	//  Construction
	public:
	    Query (Vsa::VEvaluation *pEvaluation);

	//  Destruction
	protected:
	    ~Query ();

	//  Gofer Order Fulfillment
	public:
	    template <typename gofer_reference_t> bool getClientObjectGofer (gofer_reference_t &rpGofer, Context *pContext) {
		return createClientObjectGofer (rpGofer, pContext);
	    }
	    bool getClientObjectGofer (icollection_gofer_t::Reference &rpGofer, Context *pContext);
	    bool getClientObjectGofer (isingleton_gofer_t::Reference &rpGofer, Context *pContext);
	private:
	    template <typename gofer_reference_t> bool cachedClientObjectGofer (
		gofer_reference_t &rpResultGofer, gofer_reference_t &rpCachedGofer, Context *pContext
	    ) {
		if (rpCachedGofer.isNil ())
		    createClientObjectGofer (rpCachedGofer, pContext);
		rpResultGofer.setTo (rpCachedGofer);
		return rpResultGofer.isntNil ();
	    }

	    template <typename gofer_reference_t> bool createClientObjectGofer (gofer_reference_t &rpGofer, Context *pContext) {
		typedef typename gofer_reference_t::ReferencedClass::interface_t interface_t;

		GoferOrder_<gofer_reference_t> myPrerequisite (rpGofer);
		pContext->fulfillPrerequisite (myPrerequisite);

		if (m_pEvaluation && pContext->objectComesFromClient ()) {
		    gofer_reference_t pObjectGofer (0);
		    if (m_pEvaluation->newClientObjectGofer (pObjectGofer)) {
			if (rpGofer) {
			    pObjectGofer->setFallbackTo (rpGofer);
			    pObjectGofer->setFallbackOnNull ();
			}
			rpGofer.claim (pObjectGofer);
		    }
		    typename Vca::VGoferInterface<Vca::IDataSource<interface_t*> >::Reference pSourceGofer;
		    if (m_pEvaluation->newClientObjectGofer (pSourceGofer)) {
			pObjectGofer.setTo (new Vca::Gofer::Sourced<interface_t> (pSourceGofer));
			if (rpGofer) {
			    pObjectGofer->setFallbackTo (rpGofer);
			    pObjectGofer->setFallbackOnNull ();
			}
			rpGofer.claim (pObjectGofer);
		    }
		}
		return rpGofer.isntNil ();
	    }

        //  State
	private:
	    Vsa::VEvaluation::Reference   const m_pEvaluation;         
	    icollection_gofer_t::Reference	m_pICollectionGofer;
	    isingleton_gofer_t::Reference	m_pISingletonGofer;
	};

/****************************************************************************************/
//  class VComputationUnit::Context::GoferOrder
/****************************************************************************************/
    public:
	class GoferOrder {
	    DECLARE_NUCLEAR_FAMILY (GoferOrder);

	//  Construction
	public:
	    GoferOrder () {
	    }

	//  Destruction
	public:
	    ~GoferOrder () {
	    }

	//  Fulfillment
	public:
	    virtual bool fulfillFromClient (Context *pContext, Query *pQuery) const;
	    virtual bool fulfillFromServer (Context *pContext) const;

	    virtual bool unfulfill () const = 0;
	};

/****************************************************************************************/
//  template <typename gofer_reference_t> class VComputationUnit::Context::GoferOrder_
/****************************************************************************************/
	template <typename gofer_reference_t> class GoferOrder_ : public GoferOrder {
	    DECLARE_FAMILY_MEMBERS (GoferOrder_<gofer_reference_t>, GoferOrder);

	//  Construction
	public:
	    GoferOrder_(gofer_reference_t &rpGofer) : m_rpGofer (rpGofer) {
	    }

	//  Destruction
	public:
	    ~GoferOrder_() {
	    }

	//  Fulfillment
	public:
	    bool fulfillFromClient (Context *pContext, Query *pQuery) const {
		return pQuery->getClientObjectGofer (m_rpGofer, pContext);
	    }
	    bool fulfillFromServer (Context *pContext) const {
		typedef typename gofer_reference_t::ReferencedClass::interface_t interface_t;
		VString iServerName;
		if (pContext->objectComesFromServer (iServerName)) {
		    m_rpGofer.setTo (
			new Vca::Gofer::Named<interface_t,Vca::IDirectory>(iServerName)
		    );
		    return true;
		}
		return unfulfill ();
	    }
	    bool unfulfill () const {
		m_rpGofer.clear ();
		return false;
	    }

	//  State
	private:
	    gofer_reference_t &m_rpGofer;
	};


/****************************************************************************************/
//  class VComputationUnit::Context::ControlPoints
/****************************************************************************************/
    public:
	class ControlPoints : public VControlPointController {
	    DECLARE_FAMILY_MEMBERS (ControlPoints, VControlPointController);

	//  Control Point Enumeration
	public:
	    enum ControlPoint {
		ControlPoint_Entry,
		ControlPoint_Exit,
		ControlPoint_Step,
		ControlPoint_Access,
		ControlPoint_Update,
		ControlPoint_Attention,
		ControlPoint_Fault,
		ControlPoint_Interrupt,
		ControlPoint_SNF,

		ControlPoint__COUNT
	    };
	    static char const *NameFor (ControlPoint xCP);

	//  Construction
	public:
	    ControlPoints (
		VComputationUnit *pController, ControlPoints *pPredecessor = 0
	    );

	//  Destruction
	public:
	    ~ControlPoints () {
	    }

	//  Controller Description
	public:
	    char const *description () const;

	//  Control Point...
	//  ...  Access
	public:
	    ControlPoint controlPoint (VControlPoint const *pControlPoint) const;

	//  ...  Description
	    char const*	descriptionOf (VControlPoint const *pControlPoint) const;

	//  ...  Query
	public:
	    bool canTrigger (VControlPoint const *pControlPoint) const;
	    FiringType	 firingType (VControlPoint const *pControlPoint) const;

	//  ...  Subscription
	public:
	    VControlPointSubscription *subscriptionTo (ControlPoint xCP) {
		return m_iControlPoints[xCP].subscription ();
	    }

	//  ... Delivery
	public:
	    void trigger (ControlPoint xCP, VComputationUnit *pSource) {
		m_iControlPoints[xCP].trigger (pSource);
		if (m_pPredecessor)
		    m_pPredecessor->trigger (xCP, pSource);
	    }

	    void triggerStepSubscriptions (VComputationUnit *pSource) {
		trigger (ControlPoint_Step, pSource);
	    }

	    void triggerEntrySubscriptions (VComputationUnit *pSource) {
		triggerStepSubscriptions (pSource);
		trigger (ControlPoint_Entry, pSource);
	    }

	    void triggerExitSubscriptions (VComputationUnit *pSource) {
		triggerStepSubscriptions (pSource);
		trigger (ControlPoint_Exit, pSource);
	    }

	//  State
	protected:
	    VComputationUnit *const	m_pController;
	    ControlPoints *const	m_pPredecessor;
	    VControlPoint		m_iControlPoints[ControlPoint__COUNT];
	};


/****************************************************************************************/
//  class VComputationUnit::Context (continued)
/****************************************************************************************/
    //  Global Tuning Parameters
    public:
	static bool g_fVerboseSelectorNotFound;
	static unsigned int g_iLargeTaskSize;

    //  Construction
    protected:
	Context (ControlPoints &rControlPoints, Context *pParent = 0);
	Context (ThisClass *pParent);

    //  Access
    public:
	Context *parent () const {
	    return m_pParent;
	}

	unsigned int largeTaskSize () const {
	    return *m_pLargeTaskSize;
	}

	virtual bool verboseSelectorNotFound () const;

    //  Control Point...
    //  ... Subscription
    public:
	VControlPointSubscription *subscriptionTo (ControlPoints::ControlPoint xCP) {
	    return m_rControlPoints.subscriptionTo (xCP);
	}

    //  ... Delivery
    public:
	void trigger (ControlPoints::ControlPoint xCP, VComputationUnit *pSource) const {
	    m_rControlPoints.trigger (xCP, pSource);
	}

	void triggerStepSubscriptions (VComputationUnit *pSource) {
	    m_rControlPoints.triggerStepSubscriptions (pSource);
	}
    
	void triggerEntrySubscriptions (VComputationUnit *pSource) {
	    m_rControlPoints.triggerEntrySubscriptions (pSource);
	}

	void triggerExitSubscriptions (VComputationUnit *pSource) {
	    m_rControlPoints.triggerExitSubscriptions (pSource);
	}

    //  Order Fulfillment
    public:
	virtual bool fulfill (GoferOrder const &rOrder);
	virtual bool fulfillPrerequisite (GoferOrder const &rOrder);


	bool objectComesFromClient () const;
	bool objectComesFromServer (VString& rServerName) const;

    //  State
    protected:
	Context *const	m_pParent;
	ControlPoints&	m_rControlPoints;
	unsigned int*	m_pLargeTaskSize;
    };


/****************************************************************************************/
//  class VComputationUnit::Extension
/****************************************************************************************/
public:
    class Profiler;
    class Extension : public VControlPointController {
	DECLARE_FAMILY_MEMBERS (Extension, VControlPointController);

    //  Friends
	friend class VComputationUnit;

    //  Control Point Enumeration
    public:
	enum ControlPoint {
	    ControlPoint_ChildEntry,
	    ControlPoint_ExteriorChildEntry,
	    ControlPoint_InteriorChildEntry,
	    ControlPoint_Entry,
	    ControlPoint_Exit,
	    ControlPoint_Step,

	    ControlPoint__COUNT
	};
	static char const *NameFor (ControlPoint xCP);

    //  Construction
    public:
	Extension (VComputationUnit *pController);

    //  Destruction
    protected:
	~Extension ();

    //  Controller Description
    public:
	char const *description () const;

    //  Control Point...
    //  ...  Access
    public:
	ControlPoint controlPoint (VControlPoint const *pControlPoint) const;

    //  ...  Description
    public:
	char const *descriptionOf (VControlPoint const *pControlPoint) const;

    //  ...  Query
    public:
	bool canTrigger (VControlPoint const *pControlPoint) const;
	FiringType firingType (VControlPoint const *pControlPoint) const;

    //  ...  Subscription
    public:
	VControlPointSubscription *subscriptionTo (Extension::ControlPoint xCP) {
	    return m_iControlPoints[xCP].subscription ();
	}

    //  ...  Delivery
    public:
	void trigger (Extension::ControlPoint xCP, VComputationUnit *pSource) {
	    m_iControlPoints[xCP].trigger (pSource);
	}

	void triggerStepSubscriptions (VComputationUnit *pSource) {
	    trigger (ControlPoint_Step, pSource);
	}

	void triggerEntrySubscriptions (VComputationUnit *pSource) {
	    triggerStepSubscriptions (pSource);
	    trigger (ControlPoint_Entry, pSource);
	    beginProfiling (pSource);
	}

	void triggerExitSubscriptions (VComputationUnit *pSource) {
	    endProfiling (pSource);
	    trigger (ControlPoint_Exit, pSource);
	    triggerStepSubscriptions (pSource);
	}

    //  ... Shutdown and Termination
    public:
	void shutdownIfUntriggerable (Extension::ControlPoint xCP);

	void sendTerminationEvents ();

    //  Profiling
    public:
	bool profilingDescendents () const {
	    return m_pDescendentProfiler.isntNil ();
	}

	Profiler *profiler () const {
	    return m_pProfiler;
	}
	Profiler *descendentProfiler () const {
	    return m_pDescendentProfiler;
	}

	void newDescendentProfiler ();
	void setDescendentProfilerTo (Profiler * pProfiler);

    private:
	void beginProfiling (VComputationUnit *pSource);
	void endProfiling (VComputationUnit *pSource);

    //  State
    protected:
	VComputationUnit *const		m_pController;
	VControlPoint			m_iControlPoints[ControlPoint__COUNT];
	VComputationTrigger		m_iTerminationTrigger;
	VkRunTimes			m_iProfile;
	VReference<Profiler> const	m_pProfiler;
	VReference<Profiler>		m_pDescendentProfiler;  // must follow m_pProfiler
    };


/************************************************************************/
//  class VComputationUnit (continued)
/************************************************************************/

//  Construction
protected:
    VComputationUnit (VComputationUnit *pCreator, VTask *pCaller, unsigned int iAttentionMask);

    VComputationUnit (Context *pContext, VDescriptor &rDatum, VComputationScheduler *pScheduler);

    void setContextTo (Context *pContext) {
	//  Remove the 'const'
	*const_cast<Context**>(&m_pContext) = pContext;
    }
    void markAsInterior () {
	if (!m_fIsInterior) {
	    m_pCreator->m_cUnrunInteriorChildren++;
	    m_fIsInterior = true;
	}
    }

//  Destruction
protected:
    void cancelUnrunChild (VComputationUnit const *pChild);

    ~VComputationUnit ();


//  Access
public:
    unsigned int attentionMask () const {
	return m_iAttentionMask;
    }

    VTask *caller () const {
	return m_pCaller;
    }

    virtual VComputationUnit *consumer_ () const = 0;

    VComputationUnit *creator () const {
	return m_pCreator;
    }

    Context *context () const {
	return m_pContext;
    }
    virtual IOMDriver *channel_ () const = 0;

    unsigned int channelIndex () const;

    bool cdatum (VDescriptor &rDatum) const;

    VDescriptor &datum () const {
	return m_rDatum;
    }
    VDescriptor &duc () const {
	return *m_pDuc;
    }

    bool getEvaluation (Vsa::VEvaluation::Reference &rpEvaluation, Vsa::request_index_t xRequestInProgress) const {
	return channel_()->getEvaluation (rpEvaluation, xRequestInProgress);
    }
    unsigned int largeTaskSize () const {
	return m_pContext->largeTaskSize ();
    }

    virtual unsigned int parameterCount_() const = 0;

    virtual ReturnCase returnCase_ () const = 0;
    char const *returnCaseName () const {
	return NameFor (returnCase_ ());
    }

    VComputationScheduler *scheduler () const {
	return m_pScheduler;
    }

    virtual VSelector const& selector_ () const = 0;
    char const *selectorName () const;

    Stage stage () const {
	return (Stage)m_xStage;
    }
    char const *stageName () const {
	return NameFor (stage ());
    }

    State state () const {
	return (State)m_xState;
    }
    char const *stateName () const {
	return NameFor (state ());
    }

    VComputationUnit *successor () const {
	return m_pSuccessor;
    }

    unsigned int unrunChildren () const {
	return m_cUnrunChildren;
    }
    unsigned int unrunExteriorChildren () const {
	return m_cUnrunChildren - m_cUnrunInteriorChildren;
    }
    unsigned int unrunInteriorChildren () const {
	return m_cUnrunInteriorChildren;
    }

    bool verboseSelectorNotFound () const {
	return m_pContext->verboseSelectorNotFound ();
    }


//  Query
public:
    virtual bool atOrAbove (VCall const *pCall) const = 0;
    virtual bool atOrAbove (VTask const *pTask) const = 0;
    virtual bool atOrAbove (VComputationUnit const *pUnit) const = 0;

    virtual bool atOrBelow (VCall const *pCall) const = 0;
    virtual bool atOrBelow (VTask const *pTask) const = 0;
    virtual bool atOrBelow (VComputationUnit const *pUnit) const = 0;

    bool consumesResultOf (VComputationUnit const *pUnit) const;
    bool suppliesResultTo (VComputationUnit const *pUnit) const {
	return pUnit->consumesResultOf (this);
    }

    bool isInterior () const {
	return m_fIsInterior;
    }

    virtual bool datumAlterable_ () const;
    virtual bool datumAvailable_ () const;

    virtual bool recipientAvailable_ () const = 0;

    bool blocked () const {
	return m_iSuspendCount != 0;
    }
    bool blockedNot () const {
	return m_iSuspendCount == 0;
    }

    bool damaged () const {
	return m_xState == State_Damaged;
    }
    bool failed () const {
	return m_xState == State_Failed;
    }
    bool interrupted () const {
	return m_xState == State_Interrupted;
    }
    bool terminated () const {
	return m_xState == State_Completed;
    }
    bool terminatedNot () const {
	return m_xState != State_Completed;
    }

    bool canSafelySuspend (VComputationUnit const *pSuspendee) const {
	return !terminated () && !consumesResultOf (pSuspendee);
    }


//  Duc Access
public:
    bool ducIsAMonotype () const {
	return  m_pDuc->isStandard ();
    }
    bool ducIsAPolytype () const {
	return !m_pDuc->isStandard ();
    }
    DSC_Descriptor &ducMonotype () const {
	return m_pDuc->contentAsMonotype ();
    }

    /*---------------------------------------------------------------------------
     *****  Utility to return a CPD for the dictionary of a monotype duc.
     *
     *  Returns:
     *	    A CPD (which must be released when no longer needed) for the
     *	    dictionary associated with the monotype collection of objects
     *	    identified by the duc.
     *
     *  Note:
     *	    This routine DOES NOT check to see that the duc contains the required
     *	    monotype.
     *
     *****/
    void getDucDictionary (VReference<rtDICTIONARY_Handle> &rpResult) const {
	ducMonotype ().getDictionary (rpResult);
    }
    void getDucDictionary (Vdd::Store::Reference &rpResult) const {
	ducMonotype ().getDictionary (rpResult);
    }

    /*---------------------------------------------------------------------------
     *****  Utility to return a CPD / RType for the store of a monotype duc.
     *
     *  Notes:
     *	  - This routine DOES NOT check to see that the duc contains the required
     *	    monotype.
     *	  - CPD's are shared with the DUC.
     *
     *****/
    Store *ducStore () const {
	return ducMonotype ().store ();
    }
    RTYPE_Type ducStoreRType () const {
	return ducMonotype ().storeRType ();
    }

    bool decodeClosureInDuc (
	VReference<rtBLOCK_Handle> &rpBlock, unsigned int& rxPrimitive, VReference<rtCONTEXT_Handle> *ppContext = 0
    ) const {
	return ducMonotype ().decodeClosure (rpBlock, rxPrimitive, ppContext);
    }

//  Duc Update
public:
    void normalizeDuc () const {
	m_pDuc->normalize ();
    }

    rtVECTOR_Handle *convertDucToVector () const {
	return m_pDuc->convertToVector ();
    }

    rtVECTOR_CType *convertDucToVectorC () const {
	return m_pDuc->convertToVectorC ();
    }

    VFragmentation& convertDucToFragmentation () const {
	return m_pDuc->convertToFragmentation ();
    }

    bool ConvertDucVectorsToMonotypeIfPossible () const {
	return m_pDuc->convertVectorsToMonotypeIfPossible ();
    }

    void loadDucWithCopied (VDescriptor const& rValue) const {
	m_pDuc->setToCopied (rValue);
    }
    void loadDucWithCopied (DSC_Descriptor const& rValue) const {
	m_pDuc->setToCopied (rValue);
    }

    void loadDucWithMoved (VDescriptor &rValue) const {
	m_pDuc->setToMoved (rValue);
    }
    void loadDucWithMoved (DSC_Descriptor &rValue) const {
	m_pDuc->setToMoved (rValue);
    }

    void loadDucWithVector (rtVECTOR_Handle *pVector) const {
	m_pDuc->setToVector (pVector);
    }
    void loadDucWithVector (rtVECTOR_CType *pVector) const {
	m_pDuc->setToVector (pVector);
    }

    void distributeDuc (M_CPD *pDistribution) const {
	m_pDuc->distribute (pDistribution);
    }
    void reorderDuc (M_CPD *pReordering) const {
	m_pDuc->reorder (pReordering);
    }
    void restrictAndReorderDuc (rtLINK_CType *pRestriction, M_CPD *pReordering) const;
    void restrictDuc (rtLINK_CType *pRestriction) const;
    void restrictDuc (DSC_Pointer &rRestriction) const;

    bool setDucFeathers (DSC_Pointer &rFeathers, VDescriptor &rValues) const;

//  Parameter Access
public:
    virtual bool getParameter (unsigned int xParameter, VDescriptor &rResult) = 0;

private:
    void parameter (VSNFTask *pAccessTask);

//  Path Access
public:
    virtual bool getPathToCaller (
	M_CPD *&rpReordering, rtLINK_CType *&rpRestriction, VTask *&rpCaller
    ) const = 0;
    virtual bool getPathToConsumer (
	M_CPD *&rpReordering, rtLINK_CType *&rpRestriction, VComputationUnit *&rpConsumer
    ) const = 0;
    virtual bool getPathToCreator (
	M_CPD *&rpReordering, rtLINK_CType *&rpRestriction, VComputationUnit *&rpCreator
    ) const = 0;

//  Execution...
protected:
    virtual void fail	() = 0;
    virtual void run	() = 0;

//  ... Scheduling
protected:
    inline void schedule ();

    inline void resume ();

    void resume (State xState);

    void resumeFailed () {
	resume (State_Failed);
    }
    void resumeOK () {
	resume (State_Runnable);
    }

    void suspend () {
	m_iSuspendCount++;
    }

    void terminate () {
	m_xStage = Stage_Completed;
	m_xState = State_Completed;
	m_iSuspendCount = UINT_MAX;
	if (m_pExtension)
	    m_pExtension->sendTerminationEvents ();    // ... fire all triggers and control points.
    }

public:
    void yield ();

//  ... State Control
protected:
    bool setStateTo (State xState);

//  Control Point...
//  ... Extension
protected:
    Extension *extension ();

//  ... Query
public:
    bool canTrigger (Context::ControlPoints::ControlPoint xCP) const;
    bool canTrigger (Extension::ControlPoint xCP) const;

    bool cantTrigger (Context::ControlPoints::ControlPoint xCP) const {
	return !canTrigger (xCP);
    }
    bool cantTrigger (Extension::ControlPoint xCP) const {
	return !canTrigger (xCP);
    }

    VControlPointFiringType firingType (Context::ControlPoints::ControlPoint xCP) const;
    VControlPointFiringType firingType (Extension::ControlPoint xCP) const;

//  ... Subscription
public:
    VControlPointSubscription *subscriptionTo (Context::ControlPoints::ControlPoint xCP);

    VControlPointSubscription *contextEntrySubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Entry);
    }
    VControlPointSubscription *contextExitSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Exit);
    }
    VControlPointSubscription *contextStepSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Step);
    }
    VControlPointSubscription *contextAccessSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Access);
    }
    VControlPointSubscription *contextUpdateSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Update);
    }
    VControlPointSubscription *contextAttentionSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Attention);
    }
    VControlPointSubscription *contextFaultSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Fault);
    }
    VControlPointSubscription *contextInterruptSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_Interrupt);
    }
    VControlPointSubscription *contextSNFSubscription () {
	return subscriptionTo (Context::ControlPoints::ControlPoint_SNF);
    }

    VControlPointSubscription *subscriptionTo (Extension::ControlPoint xCP);

    VControlPointSubscription *unitChildEntrySubscription () {
	return subscriptionTo (Extension::ControlPoint_ChildEntry);
    }
    VControlPointSubscription *unitExteriorChildEntrySubscription () {
	return subscriptionTo (Extension::ControlPoint_ExteriorChildEntry);
    }
    VControlPointSubscription *unitInteriorChildEntrySubscription () {
	return subscriptionTo (Extension::ControlPoint_InteriorChildEntry);
    }
    VControlPointSubscription *unitEntrySubscription () {
	return subscriptionTo (Extension::ControlPoint_Entry);
    }
    VControlPointSubscription *unitExitSubscription () {
	return subscriptionTo (Extension::ControlPoint_Exit);
    }
    VControlPointSubscription *unitStepSubscription () {
	return subscriptionTo (Extension::ControlPoint_Step);
    }

//  ... Triggering
private:
    void triggerChildEntrySubscriptions_(VComputationUnit *pChild);

    void triggerChildEntrySubscriptions (VComputationUnit *pChild) {
	m_cUnrunChildren--;
	if (m_pExtension)
	    triggerChildEntrySubscriptions_ (pChild);
	else if (pChild->isInterior ())
	    m_cUnrunInteriorChildren--;
    }

    void triggerEntrySubscriptions () {
	m_pContext->triggerEntrySubscriptions (this);
	if (m_iAttentionMask & 04)
	    m_pContext->trigger (Context::ControlPoints::ControlPoint_Access, this);
	if (m_pExtension)
	    m_pExtension->triggerEntrySubscriptions (this);
	if (m_pCreator)
	    m_pCreator->triggerChildEntrySubscriptions (this);
    }

    void triggerExitSubscriptions () {
	m_pContext->triggerExitSubscriptions (this);
	if (m_pExtension)
	    m_pExtension->triggerExitSubscriptions (this);
    }

    void triggerStepSubscriptions () {
	m_pContext->triggerStepSubscriptions (this);
	if (m_pExtension)
	    m_pExtension->triggerStepSubscriptions (this);
    }

protected:
    void triggerFaultSubscriptions () {
	m_pContext->trigger (Context::ControlPoints::ControlPoint_Fault, this);
	if (blockedNot ())
	    fail ();
    }

    void triggerInterruptSubscriptions () {
	m_pContext->trigger (Context::ControlPoints::ControlPoint_Interrupt, this);
	if (blockedNot ())
	    fail ();
    }

    void triggerSNFSubscriptions () {
	m_pContext->trigger (Context::ControlPoints::ControlPoint_SNF, this);
    }

    bool pausedOnEntry () {
	m_xStage = Stage_Entry;
	triggerEntrySubscriptions();
	return blocked ();
    }

    bool pausedOnExit () {
	m_xStage = Stage_Exit;
	triggerExitSubscriptions ();
	return blocked ();
    }

    bool pausedOnStep () {
	if (m_fAtStepPoint) {
	    m_fAtStepPoint = false;
	    triggerStepSubscriptions ();
	}
	return blocked ();
    }

    bool runnable () const {
	return blockedNot () && !SIGNAL_InInterruptState;
    }
    bool runnable () {
	return blockedNot () && !SIGNAL_InInterruptState && !pausedOnStep ();
    }

//  Profiling
public:
    bool creatorProfilingDescendents () const {
	return m_pCreator && m_pCreator->profilingDescendents ();
    }
    Profiler *creatorDescendentProfiler () const {
	return m_pCreator ? m_pCreator->descendentProfiler () : 0;
    }
    Profiler *descendentProfiler () const {
	return m_pExtension ? m_pExtension->descendentProfiler () : 0;
    }

    bool profilingDescendents () const {
	return m_pExtension && m_pExtension->profilingDescendents ();
    }
    Profiler *profiler () const {
	return m_pExtension ? m_pExtension->profiler () : 0;
    }

    void newDescendentProfiler () {
	extension ()->newDescendentProfiler ();
    }
    void setDescendentProfilerTo (Profiler * pProfiler) {
	extension ()->setDescendentProfilerTo (pProfiler);
    }

//  Event Wait
public:
    bool suspendOnQueue (
	VComputationUnit *pSuspendee, VComputationTrigger &(VComputationUnit::*pQueueAccessor)()
    );

//  Completion Wait
protected:
    VComputationTrigger &completionQueue ();

public:
    bool untilCompletionSuspend (VComputationUnit *pSuspendee) {
	return suspendOnQueue (pSuspendee, &VComputationUnit::completionQueue);
    }

    bool untilCompletionSuspend (VSNFTask *pSuspendee);

//  Client Object Access
public:
    template <typename gofer_reference_t> bool getClientObjectGofer (gofer_reference_t& rpGofer) const {
	Context::GoferOrder_<gofer_reference_t> const myOrder (rpGofer);
	return m_pContext && m_pContext->fulfill (myOrder);
    }

//  State
protected:
    Reference const			m_pCreator;
    VTask *const			m_pCaller;	// ... transitively referenced via creator.
    Context *const			m_pContext;
    VDescriptor&			m_rDatum;
    VDescriptor*			m_pDuc;
    Extension*				m_pExtension;
    VComputationScheduler *const	m_pScheduler;
    Reference				m_pSuccessor;
    unsigned int			m_iSuspendCount;
    unsigned int			m_cUnrunChildren;
    unsigned int			m_cUnrunInteriorChildren;
    Stage				m_xStage	: 8;
    State				m_xState	: 8;
    unsigned int			m_iAttentionMask: 3;
    unsigned int			m_fAtStepPoint	: 1;
    unsigned int			m_fIsInterior	: 1;
};


/*********************************
 *****  In-Line Definitions  *****
 *********************************/

/*----------------------------*
 *----  VComputationList  ----*
 *----------------------------*/

void VComputationList::pop (VReference<VComputationUnit>& rpCU) {
    rpCU.claim (*this);
    claim (rpCU->m_pSuccessor);
}

/*----------------------------------------*
 *----  VComputationScheduler::Queue  ----*
 *----------------------------------------*/

void VComputationScheduler::Queue::enqueue (VComputationUnit *pCU) {
    pCU->m_pSuccessor.claim (*m_pEnqueueReference);
    m_pEnqueueReference->setTo (pCU);
    m_pEnqueueReference = &pCU->m_pSuccessor;
}

void VComputationScheduler::Queue::dequeue (VReference<VComputationUnit>& rpCU) {
    VComputationList::pop (rpCU);
    resetEnqueueReference ();
}

/*---------------------------------*
 *----  VComputationScheduler  ----*
 *---------------------------------*/

/*--------------------------------------------------------------------------*
 *  'schedule' does not schedule the active unit for execution because the
 *  active unit's runnability cannot be known until it relinquishes control.
 *--------------------------------------------------------------------------*/
void VComputationScheduler::schedule (VComputationUnit *pCU) {
    if (m_pActiveUnit.operator-> () != pCU) {
	if (m_iActiveQueue.isEmpty ())
	    schedule ();
	m_iActiveQueue.enqueue (pCU);
    }
}


/*----------------------------*
 *----  VComputationUnit  ----*
 *----------------------------*/

/*--------------------------------------------------------------------------*
 *  'schedule' does not schedule the active unit for execution because the
 *  active unit's runnability cannot be known until it relinquishes control.
 *--------------------------------------------------------------------------*/
void VComputationUnit::schedule () {
    if (0 == m_iSuspendCount)
	scheduler ()->schedule (this);
}

void VComputationUnit::resume () {
    m_iSuspendCount--;
    schedule ();
}


#endif
