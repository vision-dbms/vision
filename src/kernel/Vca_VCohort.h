#ifndef Vca_VCohort_Interface
#define Vca_VCohort_Interface

/**
 * @page CohortsPage Cohorts
 *
 * Placeholder.
 * @todo Flesh out Cohorts page.
 * @see Vca::VCohort
 * @see Vca::VcaThreadState
 * @see Vca::VRolePlayer
 */

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "Vca_VMessageStarter.h"

#include "V_VAggregatePointer.h"
#include "V_VCount.h"
#include "V_VSchedulable.h"
#include "V_VThreadedProcessor_.h"
#include "V_VTime.h"

#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VLogger.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaThreadState;
    class VDeviceManager;
    class VMessageScheduler;
    class VRolePlayer;
    class VTimer;

    /**
     * Class representing a grouping of RolePlayer objects.
     * Each RolePlayer object belongs to exactly one Cohort. Each Cohort references zero or more RolePlayer members.
     */
    class Vca_API VCohort : public VReferenceable, private VMessageStarter {
	DECLARE_CONCRETE_RTTLITE (VCohort, VReferenceable);

	friend class VDeviceManager;
	friend class VMessageScheduler;
	friend class VRolePlayer;
	friend class VTimer;

    //  Aliases
    public:
	typedef VDeviceManager			DM;
	typedef V::VAggregatePointer<DM>	DMPointer;

	typedef V::VSchedulable			Message;
	typedef Message::Queue 			MessageQueue;
	typedef Message::Reference		MessageReference;

	typedef VcaThreadState			Thread;
	typedef V::VAggregatePointer<Thread>	ThreadPointer;

	typedef Reference			CohortReference;

    //  Cache
    public:
	class Vca_API Cache {
	//  Array
	public:
	    typedef VkDynamicArrayOf<V::VPointer</*void*/char const> > Array;

	//  Index
	public:
	    class Vca_API Index {
	    //  Index Generator
	    private:
		static count_t NextIndex ();

	    //  Construction
	    public:
		Index ();

	    //  Destruction
	    public:
		~Index () {
		}

	    //  Access
	    public:
		operator count_t () const {
		    return m_xIndex;
		}

	    //  State
	    private:
		count_t const m_xIndex;
	    };

	//  Construction
	public:
	    Cache ();

	//  Destruction
	public:
	    ~Cache ();

	//  Access
	private:
	    bool getElementIndex (Index const &rIndex, unsigned int &rxElement) const;

	public:
	    void *value (Index const &rIndex) const;

	//  Update
	public:
	    bool attach (Index const &rIndex, void const *pValue);
	    bool detach (Index const &rIndex, void const *pValue);

	//  State
	private:
	    Array	m_iArray;
	    count_t	m_xArrayLB;
	    count_t	m_xArrayUB;
	};
	typedef Cache::Index Index;

    //  Claim
    public:
        /**
         * VCohort::Claim objects are used for VCohort locking as well as for setting VCohort contexts for new RolePlayers.
         *
         * Common usage:
         *  - Construct a new VCohort and use it as the context for new RolePlayers.
         *  - Use an existing VCohort as the context for new RolePlayers, optionally locking said VCohort until the Claim is destroyed.
         *
         * Unless otherwise noted, when instructed to lock a cohort, a claim will wait until a lock can be obtained on the given cohort. Claims waiting on a lock for a particular cohort are placed on a claim stack kept by the cohort being locked.
         * @see VCohort::m_pBlockedClaims
         */
	class Vca_API Claim {
	    friend class VcaThreadState;
	    friend class VCohort;

	//  Family
            DECLARE_NUCLEAR_FAMILY(Claim);

	//  Construction
	public:
            /**
             * Lays claim to, and optionally locks, a VCohort given a VRolePlayer belonging to it.
             *
             * @param pRolePlayer a VRolePlayer belonging to the VCohort that should be claimed.
             * @param bWait true if this claim should lock its VCohort for the duration of its lifetime, false otherwise.
             */
	    Claim (VRolePlayer *pRolePlayer, bool bWait = true);

            /**
             * Lays claim to, and optionally locks, a VCohort.
             *
             * @param pCohort the VCohort that should be claimed.
             * @param bWait true if this claim should lock its VCohort for the duration of its lifetime, false otherwise.
             */
	    Claim (VCohort *pCohort, bool bWait = true);

            /**
             * Lays claim to a VCohort by either creating a new VCohort or locking the newest existing VCohort referenced in this thread's VcaThreadState.
             *
             * @param bClaimingThreadCohort true if this claim should lock the newest VCohort referenced in this thread's VcaThreadState for the duration of its lifetime, false if instead a new VCohort should be created.
             */
	    Claim (bool bClaimingThreadCohort = false);
	private:
            /**
             * Creates a new claim within a given thread that references a yet-to-be-created cohort.
             */
	    Claim (Thread *pThread);

	//  Destruction
	public:
	    ~Claim ();

	//  Access
	public:
            /** @copydoc cohort()const */
	    operator VCohort* () const {
		return cohort ();
	    }
            /**
             * Returns the cohort that this claim references.
             */
	    VCohort *cohort () const {
		return m_pCohort;
	    }

            /** @copydoc cohort() */
	    operator VCohort* () {
		return cohort ();
	    }
            /**
             * Returns the cohort that this claim references, creating one as necessary.
             */
	    VCohort *cohort ();

            /**
             * Returns the thread within which this claim was instantiated.
             * @see m_pThread
             */
	    Thread *thread () const {
		return m_pThread;
	    }

            /**
             * Returns the pointer to the next claim waiting to acquire the referenced cohort.
             * @see m_pThreadLink
             */
	    ThisClass *threadLink () const {
		return m_pThreadLink;
	    }

	//  Query
	public:
            /**
             * Used to check if this claim references and locks a given cohort.
             *
             * @return true if the given cohort is referenced and locked by this claim.
             */
	    bool claims (VCohort const *pCohort) const {
		return m_pCohort.referent () == pCohort && m_bHeld;
	    }

            /**
             * Returns true if this claim is locking.
             */
	    bool isHeld () const {
		return m_bHeld;
	    }

            /**
             * Returns true if this claim is non-locking.
             */
	    bool isntHeld () const {
	        return !m_bHeld;
	    }

	//  Update
	public:
            /**
             * @copybreif setTo(VCohort,bool)
             *
             * @param pCohort the cohort that should be referenced and locked by this claim.
             * @see setTo(VCohort,bool)
             */
	    ThisClass &operator= (VCohort *pCohort) {
		setTo (pCohort);
		return *this;
	    }

            /**
             * @copybreif setTo(VRolePlayer,bool)
             *
             * @param pObject a VRolePlayer whose cohort should be referenced and locked by this claim.
             * @see setTo(VRolePlayer,bool)
             */
	    ThisClass &operator= (VRolePlayer *pObject) {
		setTo (pObject);
		return *this;
	    }

            /**
             * Clears this claim, setting its cohort to nil and unlocking any existing lock.
             */
	    void clear ();

            /**
             * Used to set the cohort that this claim references, locking the cohort in the process.
             *
             * @param pCohort the cohort that should be referenced and locked by this claim.
             * @param bWait true if this claim should lock its VCohort for the duration of its lifetime, false otherwise.
             * @return true if acquired, false otherwise.
             */
	    bool setTo (VCohort *pCohort, bool bWait = true);

            /**
             * @copybrief setTo(VCohort,bool)
             *
             * @param pObject a VRolePlayer whose cohort should be referenced and locked by this claim.
             * @param bWait true if this claim should lock its VCohort for the duration of its lifetime, false otherwise.
             * @return true if acquired, false otherwise.
             */
	    bool setTo (VRolePlayer *pObject, bool bWait = true);

            /**
             * Sets the cohort that this claim references, attempting locking the cohort in the process but not waiting for a lock if one cannot be immediately acquired.
             * Equivelant to calling setTo(VCohort,bool) with false as the second parameter.
             *
             * @return true if acquired, false otherwise.
             */
	    bool setTry (VCohort *pCohort) {
		return setTo (pCohort, false);
	    }

            /**
             * @copybrief setTry(VCohort)
             * Equivelant to calling setTo(VRolePlayer,bool) with false as the second parameter.
             *
             * @return true if acquired, false otherwise.
             */
	    bool setTry (VRolePlayer *pObject) {
		return setTo (pObject, false);
	    }

	//  Management
	private:
	    /**
	     * Temporarily relinquish this claim to allow other threads to proceed when this one blocks.
	     *
	     * @param rpNextClaim A Claim::Pointer set this claim's successor in this thread's list of claims.
	     */
	    void relinquish (Pointer &rpNextClaim);

	    /**
	     * Re-acquire (i.e., un-relinquish) a cohort released by Claim::relinquish.
	     *
	     * @param rpNextClaim A Claim::Pointer set this claim's successor in this thread's list of claims.
	     * @see relinquish
	     */
	    bool unlinquish (Pointer &rpNextClaim);

	public:
            /**
             * Used to turn a soft claim into a hard claim by actually acquiring a lock on the referenced cohort.
             *
             * @param bWait true if this claim should wait to lock its VCohort if already locked, false if it should instead give up.
             * @return true if claim acquired, false otherwise.
             */
	    bool acquire (bool bWait = true);

            /**
             * Equivelant to calling acquire(false).
             * @return true if claim acquired, false otherwise.
             */
	    bool acquireTry ();

            /**
             * Turns a hard claim into a soft claim by releasing its lock on the referenced cohort.
             * When invoked on soft claims, nothing happens.
             */
	    void release ();

	//  State
	private:
            /**
             * The thread within which this claim was instantiated.
             */
	    VReference<Thread>	const	m_pThread;

            /**
             * Pointer to the next claim in this thread's claim stack (toward the bottom of the stack).
             */
	    Pointer		const	m_pThreadLink;

            /**
             * Cohort that we're claiming (or at least referencing, in the case of a soft claim).
             */
	    CohortReference		m_pCohort;

            /**
             * Pointer to the next claim waiting to acquire the referenced cohort.
             * @see VCohort::m_pBlockedClaims
             * @see VCohort::releaseClaim()
             */
	    Pointer			m_pCohortLink;

            /**
             * Indicates whether or not this claim is locking.
             *
             * True if locking, false otherwise.
             */
	    bool			m_bHeld;

	    /**
	     * Indicates whether an un-held claim was voluntarily relinquished.
	     *
	     * When a blocking claim fails to acquire a cohort, all of the cohorts previously
	     * held by the blocking thread are relinquished. This prevents deadlocks with other
	     * running threads that attempt to acquire one of the cohorts held by this thread.
	     * When the cohort referenced by this claim eventually becomes available, the
	     * cohorts relinquished by this thread are reacquired along with this claim's
	     * cohort and processing proceeds.
	     */
	    bool			m_bRelinquished;
	};
	friend class Claim;

    //  Manager
    public:
	class Vca_API Manager {
	    DECLARE_NUCLEAR_FAMILY (Manager);

	    friend class VCohort;

	//  Construction
	public:
	    Manager (VCohort *pCohort);
	    Manager ();

	//  Destruction
	public:
	    ~Manager ();

	//  Access
	public:
	    operator VCohort* () const {
		return cohort ();
	    }
	    operator VCohort* () {
		return cohort ();
	    }
	    VCohort *cohort () const {
		return m_pCohort;
	    }
	    VCohort *cohort ();

	//  Event Notification
	public:
	    bool attachCohortEvents () {
		return m_pCohort.isntNil () && m_pCohort->attach (this);
	    }
	    bool detachCohortEvents () {
		return m_pCohort.isntNil () && m_pCohort->detach (this);
	    }
	protected:
	    virtual bool onCohortEvent_();	// return true if event handled
	    virtual bool onEmptyCohort_();	// return true if event handled
	private:
	    bool onCohortEvent () {
		return onCohortEvent_();
	    }
	    bool onEmptyCohort () {
		return onEmptyCohort_();
	    }

	//  Event Processing
	private:
	    virtual size_t adjustedTimeout_(size_t sTimeout) {
		return sTimeout;
	    }
	    virtual size_t adjustedTimeoutNDM_(size_t sTimeout) { // NDM = No Device Manager
		return 0;
	    }
	public:
	    size_t adjustedTimeout (size_t sTimeout) {
		return adjustedTimeout_(sTimeout);
	    }
	    size_t adjustedTimeoutNDM (size_t sTimeout) {
		return adjustedTimeoutNDM_(sTimeout);
	    }

	public:
	    bool doEvents (size_t sTimeOut, bool &rbEventsHandled);
	    bool doEvents (size_t sTimeout);
	    void doEvents ();

	    bool postInterrupt () {
		return m_pCohort->postInterrupt ();
	    }

	//  State
	private:
	    CohortReference const m_pCohort;
	};
	typedef Manager::Pointer EPPointer;
	friend class Manager;

    //  Processor Request
    public:
	class ProcessorRequest {
	    DECLARE_FAMILY_MEMBERS (ProcessorRequest, void);

	//  Construction
	public:
	    ProcessorRequest (ThisClass const &rOther) : m_pCohort (rOther.m_pCohort), m_bCounted (false) {
		incrementProcessorRequestCount ();
	    }
	    ProcessorRequest (VCohort *pCohort) : m_pCohort (pCohort), m_bCounted (false) {
		incrementProcessorRequestCount ();
	    }
	    ProcessorRequest () : m_bCounted (false) {
	    }

	//  Destruction
	public:
	    ~ProcessorRequest () {
		decrementProcessorRequestCount ();
	    }

	//  Access
	public:
	    VCohort *cohort () const {
		return m_pCohort;
	    }

	//  Update
	public:
	    void clear () {
		decrementProcessorRequestCount ();
		m_pCohort.clear ();
	    }
	    void setTo (ThisClass const &rOther) {
		decrementProcessorRequestCount ();
		m_pCohort.setTo (rOther.m_pCohort);
		if (rOther.m_bCounted)
		    incrementProcessorRequestCount ();
	    }
	    ThisClass &operator= (ThisClass const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  Execution
	private:
	    void incrementProcessorRequestCount () {
		if (!m_bCounted && m_pCohort) {
		    m_bCounted = true;
		    m_pCohort->incrementProcessorRequestCount ();
		}
	    }
	    void decrementProcessorRequestCount () {
		if (m_bCounted && m_pCohort) {
		    m_bCounted = false;
		    m_pCohort->decrementProcessorRequestCount ();
		}
	    }

	public:
	    void process ();

	//  State
	private:
	    CohortReference m_pCohort;
	    bool m_bCounted;
	};
	friend class ProcessorRequest;

    //  Processor
        /** Convenience definition for thread pool managers. */
	typedef V::VThreadedProcessor_<ProcessorRequest> Processor;
    private:
        /** The thread pool used to process cohort events. */
	static Processor::Reference g_pProcessor;

    //  Construction
    private:
	VCohort ();

    //  Destruction
    private:
	~VCohort ();

    //  Access
    private:
        /**
         * The single cohort to which Vca's internal role players belong.
         * Including, but not limited to:
         *  - VcaPeer
         *  - VcaSelf
         *  - VcaTransport
         *  - VcaConnection
         */
	static Reference g_pVcaCohort;
    public:
        /**
         * Returns the current cohort to which new role players will be attached.
         * Can be changed by creating a new Claim.
         *
         * @see Claim
         */
	static VCohort *Here ();

        /**
         * Returns the cohort to which Vca's internal role players belong.
         * Cannot be changed.
         */
	static VCohort *Vca ();

        /**
         * Returns the thread that currently has a lock on this cohort, if any.
         */
	Thread *claimHolder () const {
	    return m_pClaimHolder;
	}

	/**
         * Returns the device logger for this cohort.
         */
	static V::VLogger &DefaultLogger ();
	V::VLogger &defaultLogger () const;

        /**
         * Returns the device manager for this cohort.
         */
	DM *deviceManager () const {
	    return m_pDeviceManager;
	}

        /**
         * Returns true if this cohort is the cohort to which Vca's internal role players belong.
         */
	bool isVca () const {
	    return this == g_pVcaCohort.referent ();
	}

        /**
         * Returns false if this cohort is the cohort to which Vca's internal role players belong.
         */
	bool isntVca () const {
	    return this != g_pVcaCohort.referent ();
	}

    /**
     * @name Cache Management
     * These methods are related to VcaValue() and support its functionality.
     * Intended for Vca internal use.
     */
    //@{
    public:
        /**
         * Low-level convenience function for associating additional thread-specific Vca data.
         */
	static void *VcaValue (Index const &rIndex) {
	    return GetValue (Vca (), rIndex);
	}
        static void *GetValue (VCohort const* pCohort, Index const &rIndex) {
            return pCohort ? pCohort->getValue (rIndex) : 0;
        }
        void *getValue (Index const &rIndex) const {
	    return m_iCache.value (rIndex);
        }
	bool attach (Index const &rIndex, void const *pValue) {
	    return m_iCache.attach (rIndex, pValue);
	}
	bool detach (Index const &rIndex, void const *pValue) {
	    return m_iCache.detach (rIndex, pValue);
	}
    //@}

    /// @name Claim Management
    //@{
    private:
	void attemptClaim (bool &rbHeld, Thread *pThread);
	void releaseClaim (bool &rbHeld, Thread *pThread);

	void onReleaseSignal (Claim *pClaim);
    //@}

    /**
     * @name Event Source Management
     *
     * Cohorts accept the following event generators:
     *  - Messages (external events)
     *  - Timers (internal events)
     *  - Devices and Device Managers (I/O events)
     */
    //@{
    public:
        /**
         * Schedules a Vca message for delivery to an object belonging to this cohort.
         * Indended for Vca internal use only.
         * @internal
         *
         * @param pMessage the message to be scheduled for delivery.
         */
	void schedule (Message *pMessage);

    private:
	bool attach (VTimer *pTimer);
	bool detach (VTimer *pTimer);

        /**
         * Attempts to attach a device manager to this cohort.
         * At most one device manager can be attached to a cohort at a time.
         *
         * @param pManager the device manager to attach to this cohort.
         * @return true if the attach was successful, false if this cohort is already attached to a device manager.
         */
	bool attach (VDeviceManager *pManager);

        /**
         * Attempts to detach a device manager from this cohort.
         *
         * @param pManager the device manager to detach from this device.
         * @return true if the detach was successful, false if this cohort is not attached the given manager.
         */
	bool detach (VDeviceManager *pManager);
    //@}

    /**
     * @name Event Processing.
     */
    //@{
    private:
        /**
         * Attempts to attach a cohort manager to this cohort.
         * At most one cohort manager can be attached to a cohort at a time.
         *
         * @param pManager the cohort manager to attach to this cohort.
         * @return true if the attach was successful, false if this cohort is already attached to a cohort manager.
         */
	bool attach (Manager *pManager);

        /**
         * Attempts to detach a cohort manager from this cohort.
         *
         * @param pManager the cohort manager to detach from this cohort.
         * @return true if the detach was successful, false if this cohort is not attached the given manager.
         */
	bool detach (Manager *pManager);

        /**
         * Used to check if there are entities or conditions associated with this cohort that may require attention.
         * The following conditions are checked. If any are met, true is returned.
         *  - The message queue (m_iMessageQueue) is not empty.
         *  - There are not-yet-expired timers (m_cPendingTimers).
         *  - Our device manager has active devices.
         *
         * @return true if there's something potentially requiring attention, false otherwise.
         */
	bool needsTending () const;
        bool doesntNeedTending () const {
            return !needsTending();
        }

        /**
         * Implements VMessageStarter::start_().
         */
	virtual void start_(VMessage *pMessage) OVERRIDE;

        /**
         * Starts an event monitor as necessary. Wakes an existing event monitor if one already exists. May result in the creation of a new thread.
         */
	void startEventMonitor ();

    public:
        /**
         * Interrupts the current wait (if there is one) on the device manager.
         * @see m_pDeviceManager
         */
	bool postInterrupt ();

    private:
	void incrementProcessorRequestCount () {
	    m_cProcessorRequests.increment ();
	}
	void decrementProcessorRequestCount ();

        /** Main event loop. */
	bool processEvents (
	    Manager *pEP, size_t sTimeout /* in milliseconds  */, bool &rbEventsProcessed
	);

    public:
	bool hasTimers () const {
	    return m_cPendingTimers > 0;
	}
	/**
	 * Returns true if process events has timers that should be counted as external event sources.  Currently true when this cohort has timers and shutdown hasn't been called.
	 */
	bool hasPrimaryTimers () const {
	    return hasTimers () && timersArePrimary ();
	}
	static bool timersArePrimary () {
	    return !ShutdownCalled ();
	}

	count_t processorRequestCount () const {
	    return m_cProcessorRequests;
	}
	count_t pendingTimerCount () const {
	    return m_cPendingTimers;
	}

    //  Event Processing Utilities
    private:
	void drainMessageQueue ();
	void drainOutputQueue ();
	void drainQueues ();
	bool triggerTimers ();
    //@}

    //  Membership
    private:
        /** Increments the member count. */
	void incrementMemberCount () {
	    m_cMembers.increment ();
	}
        /** Decrements the member count. Automatically calls onEmptyCohort() as needed. */
	void decrementMemberCount () {
	    if (m_cMembers.decrementIsZero ())
		onEmptyCohort ();
	}
        /** Called when this cohort has no members. */
	void onEmptyCohort ();
    public:
	count_t memberCount () const {
	    return m_cMembers;
	}

    //  Shutdown control
    public:
	static void CallExitHandler ();

    //  State
    private:
	Cache			m_iCache;
	ThreadPointer		m_pClaimHolder;
	counter_t		m_sHolderClaim;

        /**
         * Head of the list of claims that have attempted, failed and are waiting to acquire a locking claim on this cohort.
         * @see VCohort::Claim::m_pCohortLink
         */
	Claim::Pointer		m_pBlockedClaims;
        /** Used to interface with the system and to handle lower-level event wait control. */
	DMPointer		m_pDeviceManager;
        /** Used by cohorts to process events. */
	Manager::Pointer	m_pManager;
	MessageQueue		m_iMessageQueue;
	VReference<VTimer>	m_pPendingTimers;
	counter_t		m_cPendingTimers;
	counter_t		m_cProcessorRequests;
	counter_t		m_cMembers;
	V::VTime		m_tLastEvent;
    };
    typedef VCohort::Claim	VCohortClaim;
    typedef VCohort::Index	VCohortIndex;
    typedef VCohort::Manager	VCohortManager;
}


#endif
