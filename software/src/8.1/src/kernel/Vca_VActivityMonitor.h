#ifndef Vca_VActivityMonitor_Interface
#define Vca_VActivityMonitor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VActivity.h"
#include "Vca_VInterfaceEKG.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VActivityMonitor : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VActivityMonitor, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<IRequest*> IRequestSink;
	typedef V::VTime VTime;

    //  RequestSink
    public:
	class RequestSink;

    //  TicketHolder
    public:
	class TicketHolder {
	    DECLARE_NUCLEAR_FAMILY (TicketHolder);

	//  Aliases
	public:
	    typedef VInterfaceEKG<VActivityMonitor,IRequest> EKG;
	    typedef void (VActivityMonitor::*monitor_member_t)();

	//  Construction
	public:
	    TicketHolder ();

	//  Destruction
	public:
	    ~TicketHolder ();

	//  Access
	public:
	    IRequest *ticket () const {
		return m_pEKG ? m_pEKG->interfase () : 0;
	    }

	//  Update
	public:
	    void setTo (VActivityMonitor *pMonitor, monitor_member_t pMonitorMember, IRequest *pTicket);

	//  State
	private:
	    EKG::Reference m_pEKG;
	};
	friend class TicketHolder;

    //  Construction
    protected:
	VActivityMonitor (VActivity *pActivity);

    //  Destruction
    protected:
	~VActivityMonitor ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IEventSink Role
    private:
	VRole<ThisClass,IEventSink> m_pIEventSink;
    public:
	void getRole (IEventSink::Reference &rpRole) {
	    m_pIEventSink.getRole (rpRole);
	}

    //  IEventSink Callbacks
    public:
	void OnEvent (IEventSink *pRole, IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, VString const &rDescription);
	void OnEndEvent (IEventSink *pRole, IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp);
	void OnErrorEvent (
	    IEventSink *pRole, IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, IError *pInterface, VString const &rMessage
	);
	void OnFailureEvent (
	    IEventSink *pRole, IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, IError *pInterface, VString const &rMessage
	);

    //  IClient Callbacks
    public:
	void OnEnd_();
	void OnError_(IError *pInterface, VString const &rMessage);

    //  Access
    public:
	VActivity *activity () const {
	    return m_pActivity;
	}
	ssid_t activitySSID () const {
	    return m_pActivity->ssid ();
	}

	void getDescription (VString &rResult) const {
	    m_pActivity->getDescription (rResult);
	}
	void getFormattedDescription (VString &rResult) const {
	    m_pActivity->getFormattedDescription (rResult);
	}

	bool getLocalStartTime (VString &rResult) const {
	    return m_pActivity->getLocalStartTime (rResult);
	}
	bool getLocalStartTime (VTime &rResult) const {
	    return m_pActivity->getLocalStartTime (rResult);
	}

	bool getLocalEndTime (VString &rResult) const {
	    return m_pActivity->getLocalEndTime (rResult);
	}
	bool getLocalEndTime (VTime &rResult) const {
	    return m_pActivity->getLocalEndTime (rResult);
	}

	bool getLocalEndTimeEstimate (VString &rResult) const {
	    return m_pActivity->getLocalEndTimeEstimate (rResult);
	}
	bool getLocalEndTimeEstimate (VTime &rResult) const {
	    return m_pActivity->getLocalEndTimeEstimate (rResult);
	}

	bool getRemoteStartTime (VString &rResult) const {
	    return m_pActivity->getRemoteStartTime (rResult);
	}
	bool getRemoteStartTime (VTime &rResult) const {
	    return m_pActivity->getRemoteStartTime (rResult);
	}

	bool getRemoteEndTime (VString &rResult) const {
	    return m_pActivity->getRemoteEndTime (rResult);
	}
	bool getRemoteEndTime (VTime &rResult) const {
	    return m_pActivity->getRemoteEndTime (rResult);
	}

	bool getRemoteEndTimeEstimate (VString &rResult) const {
	    return m_pActivity->getRemoteEndTimeEstimate (rResult);
	}
	bool getRemoteEndTimeEstimate (VTime &rResult) const {
	    return m_pActivity->getRemoteEndTimeEstimate (rResult);
	}

    //  Query
    public:
	bool activityHasCompleted () const {
	    return m_pActivity->hasFinished ();
	}

    //  Update
    private:
	virtual void onCompletion_();
	void onCompletion ();
    protected:
	void onCompletion (VTime const &rEndTime);

    //  Monitoring
    private:
	void onActivation ();
	void onDeactivation ();

	void onDisconnectedEventTicket ();
	void onDisconnectedEndErrTicket ();

	void onEventTicket (IRequest *pTicket);
	void onEndErrTicket (IRequest *pTicket);
    protected:
	void requestEvents ();
	void requestEndErrEvents ();

    //  Monitoring Hooks
    private:
	virtual void OnEvent_(IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, VString const &rDescription);
	virtual void OnEndEvent_(IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp);
	virtual void OnErrorEvent_(IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, IError *pInterface, VString const &rMessage);
	virtual void OnFailureEvent_(IEvent *pEvent, IEvent *pProxy, VTime const &rTimestamp, IError *pInterface, VString const &rMessage);

    //  State
    private:
	VActivity::Reference const	m_pActivity;
	TicketHolder			m_pEventRequest;
	TicketHolder			m_pEndErrRequest;
    };
}


#endif
