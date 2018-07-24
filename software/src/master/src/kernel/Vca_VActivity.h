#ifndef Vca_VActivity_Interface
#define Vca_VActivity_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VPublication.h"

#include "Vca_IActivity.h"
#include "Vca_IRequest.h"

#include "Vca_VEvent.h"

#include "V_VCount.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IActivitySink.h"
#include "Vca_IEventSink.h"

#include "Vca_IError.h"

#include "Vca_VGoferInstance.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VActivityMonitor;

    class Vca_API VActivity : public virtual VRolePlayer {
        DECLARE_ABSTRACT_RTTLITE (VActivity, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<IRequest*> IRequestSink;

	typedef Reference ActivityReference;
	typedef V::VTime VTime;

	typedef VGoferInstance<IActivity*,IActivity::Reference> parent_gofer_t;
	typedef VGoferInstance<IActivitySink*,IActivitySink::Reference> sink_gofer_t;

    //  Tracking Info
    private:
	static bool g_bTrackingActivities;
	static bool g_bFilteringActivities;
    public:
	static bool TrackingActivities () {
	    return g_bTrackingActivities;
	}
	static bool FilteringActivities () {
	    return g_bFilteringActivities;
	}

    //********************************************************************************************/
    //  EventSink
	class EventSink {
	public:
	    EventSink (IEventSink *pSink) : m_pSink (pSink) {
	    }
	    ~EventSink () {
	    }
	    bool isNil () const {
		return m_pSink.isNil ();
	    }
	    bool isntNil () const {
		return m_pSink.isntNil ();
	    }
	    IEventSink *operator-> () const {
		return m_pSink;
	    }
	private:
	    IEventSink::Reference const m_pSink;
	};

    //********************************************************************************************/
    //  EndErrSink
	class EndErrSink : public EventSink {
	public:
	    EndErrSink (IEventSink *pSink) : EventSink (pSink) {
	    }
	    ~EndErrSink () {
	    }
	};

    //********************************************************************************************/
    //  Event
    public:
	/**
	* The VEvent subclass used to record an event in the life of a VActivity.
	*/
	class Vca_API Event : public VEvent {
	    DECLARE_ABSTRACT_RTTLITE (Event, VEvent);

	//  Construction
	public:
	    /** Constructor associates this event with a VActivity. */
	    Event (VActivity *pActivity);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~Event ();

	//  Distribution
	protected:
	    virtual void sendTo_(EndErrSink const &rObserver);
	    virtual void sendTo_(EventSink  const &rObserver);
	public:
	    void sendTo (EndErrSink const &rObserver, Reference &rpMySelf);
	    void sendTo (EndErrSink const &rObserver);

	    void sendTo (EventSink const &rObserver, Reference &rpMySelf);
	    void sendTo (EventSink const &rObserver);

	//  State
	private:
	    Reference m_pSuccessor;
	};
	friend class Event;

    //********************************************************************************************/
    //  StartEvent
    public:
	/**
	* The VActivity::Event subclass used to record the start of a VActivity.
	*/
	class Vca_API StartEvent : public Event {
	    DECLARE_CONCRETE_RTTLITE (StartEvent, Event);

	//  Construction
	public:
	    /** Constructor associates this event with a VActivity. */
	    StartEvent (VActivity *pActivity);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~StartEvent ();

	//  Description
	protected:
	    virtual void getDescription_(VString &rsDescription) const OVERRIDE;
	};

    //********************************************************************************************/
    //  ErrorEvent
    public:
	/**
	* The VActivity::Event subclass used to record the start of a VActivity.
	*/
	class Vca_API ErrorEvent : public Event {
	    DECLARE_CONCRETE_RTTLITE (ErrorEvent, Event);

	//  Construction
	public:
	    /** Constructor associates this event with a VActivity. */
	    ErrorEvent (VActivity *pActivity, IError *pInterface, VString const &rMessage);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~ErrorEvent ();

	//  Description
	protected:
	    virtual void getDescription_(VString &rsDescription) const OVERRIDE;

	//  Distribution
	protected:
	    using BaseClass::sendTo_;
	    virtual void sendTo_(EndErrSink const &rObserver) OVERRIDE;

	//  State
	private:
	    /** The error message. */
	    const VString m_iMessage;
	    /** The error interface, if any. */
	    const IError::Reference m_pInterface;
	};

    //********************************************************************************************/
    //  FinishEvent
    public:
	/**
	* The VActivity::Event subclass used to record the completion of a VActivity.
	*/
	class Vca_API FinishEvent : public Event {
	    DECLARE_ABSTRACT_RTTLITE (FinishEvent, Event);

	//  Construction
	public:
	    /** Constructor associates this event with a VActivity. */
	    FinishEvent (VActivity *pActivity);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~FinishEvent ();

	//  Description
	protected:
	    virtual void getDescription_(VString &rsDescription) const OVERRIDE;

	//  Distribution
	protected:
	    virtual void sendTo_(EndErrSink const &rObserver) OVERRIDE;
	    virtual void sendTo_(EventSink const &rObserver) OVERRIDE;
	};

    //********************************************************************************************/
    //  SuccessEvent
    public:
	/**
	* The FinishEvent subclass used to record the successful completion of a VActivity.
	*/
	class Vca_API SuccessEvent : public FinishEvent {
	    DECLARE_CONCRETE_RTTLITE (SuccessEvent, FinishEvent);

	//  Construction
	public:
	    SuccessEvent (VActivity *pActivity);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~SuccessEvent ();

	//  Description
	protected:
	    virtual void getDescription_(VString &rsDescription) const OVERRIDE;
	};

    //********************************************************************************************/
    //  FailureEvent
    public:
	/**
	* The FinishEvent subclass used to record the successful completion of a VActivity.
	*/
	class Vca_API FailureEvent : public FinishEvent {
	    DECLARE_CONCRETE_RTTLITE (FailureEvent, FinishEvent);

	//  Construction
	public:
	    FailureEvent (VActivity *pActivity, IError *pInterface, VString const &iMessage);

	//  Destruction
	protected:  // ... 'protected' destructor allows further derivation.
	    ~FailureEvent ();

	//  Description
	protected:
	    virtual void getDescription_(VString &rsDescription) const OVERRIDE;

	//  Distribution
	protected:
	    using BaseClass::sendTo_;
	    virtual void sendTo_(EndErrSink const &rObserver) OVERRIDE;

	//  State
	private:
	    /** The error message. */
	    const VString m_iMessage;
	    /** The error interface, if any. */
	    const IError::Reference m_pInterface;
	};

    //********************************************************************************************/
    //  Construction
    public:
	static parent_gofer_t *ParentGoferFor (IClient *pClient);
    protected:
	VActivity (VCohort *pCohort, parent_gofer_t *pParentGofer = 0);
	VActivity (parent_gofer_t *pParentGofer);
	VActivity (IClient *pClient = 0);

    //  Destruction
    protected:
	~VActivity ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IActivity
    private:
	VRole<ThisClass,IActivity> m_pIActivity;
    public:
	void getRole (IActivity::Reference &rpRole) {
	    m_pIActivity.getRole (rpRole);
	}

    //  IActivity Implementation
    public:
	void SupplyEvents (
	    IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
	);
	void SupplyEndErr (
	    IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
	);
	void GetSSID (IActivity *pRole, IVReceiver<ssid_t> *pClient);
	void GetParent (IActivity *pRole, IVReceiver<IActivity*> *pClient);
	void GetDescription (IActivity *pRole, IVReceiver<VString const&> *pClient);

    //  Access
    protected:
	virtual void getAuthority_(IActivity::Reference &rpAuthority);
	virtual void getDescription_(VString &rsDescription) const;

	virtual bool getLocalStartTime_(VTime &rResult) const;
	virtual bool getLocalEndTime_(VTime &rResult) const;
	virtual bool getLocalEndTimeEstimate_(VTime &rResult) const;

	virtual bool getRemoteStartTime_(VTime &rResult) const;
	virtual bool getRemoteEndTime_(VTime &rResult) const;
	virtual bool getRemoteEndTimeEstimate_(VTime &rResult) const;
    public:
	void getAuthority (IActivity::Reference &rpAuthority) {
	    getAuthority_(rpAuthority);
	}
	void getDescription (VString &rsDescription) const {
	    return getDescription_(rsDescription);
	}
	void getFormattedDescription (VString &rsDescription) const;

	bool getLocalStartTime (VString &rResult) const;
	bool getLocalStartTime (VTime &rResult) const {
	    return getLocalStartTime_(rResult);
	}

	bool getLocalEndTime (VString &rResult) const;
	bool getLocalEndTime (VTime &rResult) const {
	    return getLocalEndTime_(rResult);
	}

	bool getLocalEndTimeEstimate (VString &rResult) const;
	bool getLocalEndTimeEstimate (VTime &rResult) const {
	    return getLocalEndTimeEstimate_(rResult);
	}

	bool getRemoteStartTime (VString &rResult) const;
	bool getRemoteStartTime (VTime &rResult) const {
	    return getRemoteStartTime_(rResult);
	}

	bool getRemoteEndTime (VString &rResult) const;
	bool getRemoteEndTime (VTime &rResult) const {
	    return getRemoteEndTime_(rResult);
	}

	bool getRemoteEndTimeEstimate (VString &rResult) const;
	bool getRemoteEndTimeEstimate (VTime &rResult) const {
	    return getRemoteEndTimeEstimate_(rResult);
	}

	bool hasStarted () const {
	    return m_pStartEvent.isntNil ();
	}
	bool hasFinished () const {
	    return m_pFinishEvent.isntNil ();
	}

    //  Adjustments
    protected:
	virtual VTime const &adjustedEndTime (VTime &rTimeToAdjust) const;

    //  Distribution
    private:
	/**
	 * Virtual expected to return a gofer for the IActivitySink to which this activity
	 * should report.  The default implementation of this virtual returns a gofer for
	 * a sink suitable for use in this process.
	 */
	virtual void getSinkGofer (VReference<sink_gofer_t> &rpGofer) const;

	sink_gofer_t::Reference newSinkGofer () const;
    protected:
	void onEvent (Event *pEvent) const;
    private:
	void onEvent () const;

	void sendToMonitor ();
	void sendTo (sink_gofer_t *pSinkGofer);
	void sendTo (IActivitySink *pActivitySink);

    //  Distribution Callbacks
    private:
	void onGoferValue (IActivitySink *pSink);
	void onGoferError (IError *pInterface, VString const &rMessage);

    //  Event Recording
    public:
	StartEvent *startEvent () const {
	    return m_pStartEvent;
	}
	FinishEvent *completionEvent () const {
	    return m_pFinishEvent;
	}
	Event *currentEvent () const {
	    return m_pCurrentEvent;
	}

    //  Event Recording
    //  ... Start
    private:
	/**
	 * Virtual expected to return a start event for this activity.  Override this virtual
	 * in subclasses that want something other than the generic StartEvent object created
	 * by the default implementation of this virtual.
	 *
	 *  @return a new StartEvent or one of its subclasses.
	 */
	virtual StartEvent *newStartEvent_();
    protected:
        /**
         * To be called by subclasses when starting work; creates a StartEvent for this VActivity.
         */
        void onActivityStart ();

        void onStart () {
	    onActivityStart ();
	}

    //  Event Recording
    //  ... Error
    private:
	/**
	 * Virtual expected to return an error event for this activity.  Override this virtual
	 * in subclasses that want something other than the generic ErrorEvent object created
	 * by the default implementation of this virtual.
	 *
	 *  @return a new ErrorEvent or one of its subclasses.
	 */
	virtual ErrorEvent *newErrorEvent_(IError *pInterface, VString const &rMessage);
    protected:
        /**
         * Creates an ErrorEvent for this VActivity.
	 *
         */
        void onActivityError (IError *pInterface, VString const &rMessage);

    //  Event Recording
    //  ... Success
    private:
	/**
	 * Virtual expected to return a success event for this activity.  Override this virtual
	 * in subclasses that want something other than the generic SuccessEvent object created
	 * by the default implementation of this virtual.
	 *
	 *  @return a new SuccessEvent or one of its subclasses.
	 */
	virtual SuccessEvent *newSuccessEvent_();
    protected:
        /**;
         * Creates a SuccessEvent for this VActivity.
         */
        void onActivitySuccess ();

        void onSuccess () {
	    onActivitySuccess ();
	}

    //  Event Recording
    //  ... Failure
    private:
	/**
	 * Virtual expected to return a failure event for this activity.  Override this virtual
	 * in subclasses that want something other than the generic FailureEvent object created
	 * by the default implementation of this virtual.
	 *
	 *  @return a new FailureEvent or one of its subclasses.
	 */
	virtual FailureEvent *newFailureEvent_(IError *pInterface, VString const &rMessage);
    protected:
        /**
         * Creates a FailureEvent for this VActivity.
         */
        void onActivityFailure (IError *pInterface, VString const &rMessage);

        void onFailure (IError *pInterface, VString const &rMessage) {
	    onActivityFailure (pInterface, rMessage);
	}

    //  Event Publication
    private:
	VPublication<IEventSink> m_pubEndErrSink;
	VPublication<IEventSink> m_pubEventSink;

    //  Monitoring
    protected:
	virtual void onMonitorCreation_(VActivityMonitor *pMonitor);
	virtual void onMonitorCompletion_(VActivityMonitor *pMonitor, VTime const &rEndTime);
	virtual void onMonitorDeletion_(VActivityMonitor *pMonitor);
	virtual void sendTo_(VActivityMonitor *pMonitor, IActivitySink *pActivitySink);
	virtual void supplyEvents_(VActivityMonitor *pMonitor, IRequestSink *pRequestSink);
	virtual void supplyEndErrEvents_(VActivityMonitor *pMonitor, IRequestSink *pRequestSink);
    public:
	void onMonitorCreation (VActivityMonitor *pMonitor) {
	    onMonitorCreation_(pMonitor);
	}
	void onMonitorCompletion (VActivityMonitor *pMonitor, VTime const &rEndTime) {
	    onMonitorCompletion_(pMonitor, rEndTime);
	}
	void onMonitorDeletion (VActivityMonitor *pMonitor) {
	    onMonitorDeletion_(pMonitor);
	}
	void sendTo (VActivityMonitor *pMonitor, IActivitySink *pActivitySink) {
	    sendTo_ (pMonitor, pActivitySink);
	}
	void supplyEvents (VActivityMonitor *pMonitor, IRequestSink *pRequestSink) {
	    supplyEvents_(pMonitor, pRequestSink);
	}
	void supplyEndErrEvents (VActivityMonitor *pMonitor, IRequestSink *pRequestSink) {
	    supplyEndErrEvents_(pMonitor, pRequestSink);
	}

    //  State
    private:
	VReference<parent_gofer_t>const m_pParentGofer;
	Event::Reference		m_pFirstEvent;
	StartEvent::Reference		m_pStartEvent;
	FinishEvent::Reference		m_pFinishEvent;
        Event::Reference		m_pCurrentEvent;
    };
}

#endif
