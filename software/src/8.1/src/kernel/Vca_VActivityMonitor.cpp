/*****  Vca_VActivityMonitor Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VActivityMonitor.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivitySink.h"


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VActivityMonitor::RequestSink  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace Vca {
    class VActivityMonitor::RequestSink : virtual public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (RequestSink, VRolePlayer);

    //  Aliases
    public:
	typedef void (VActivityMonitor::*monitor_callback_t) (IRequest*);

    //  Construction
    public:
	RequestSink (VActivityMonitor* pMonitor, monitor_callback_t pMonitorCallback);

    //  Destruction
    private:
	~RequestSink ();

    //  IRequestSink Role
    private:
	VRole<ThisClass,IRequestSink> m_pIRequestSink;
    public:
	void getRole (IRequestSink::Reference& rpRole) {
	    m_pIRequestSink.getRole (rpRole);
	}

    //  IRequestSink Callbacks
    public:
	void OnData (IRequestSink* pRole, IRequest* pTicket);

    //  IClient Callbacks
    private:
	void OnError_(IError* pInterface, VString const& rMessage);

    //  State
	VActivityMonitor::Reference const m_pMonitor;
	monitor_callback_t const m_pMonitorCallback;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityMonitor::RequestSink::RequestSink (
    VActivityMonitor* pMonitor, monitor_callback_t pMonitorCallback
) : m_pMonitor (pMonitor), m_pMonitorCallback (pMonitorCallback), m_pIRequestSink (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityMonitor::RequestSink::~RequestSink () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityMonitor::RequestSink::OnData (IRequestSink* pRole, IRequest* pRequest) {
    if (pRequest) {
	(m_pMonitor.referent()->*m_pMonitorCallback) (pRequest);
    }
}

void Vca::VActivityMonitor::RequestSink::OnError_(IError *pInterface, VString const &rMessage) {
    m_pMonitor->OnError_(pInterface, rMessage);
}


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VActivityMonitor::TicketHolder  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityMonitor::TicketHolder::TicketHolder () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityMonitor::TicketHolder::~TicketHolder () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VActivityMonitor::TicketHolder::setTo (VActivityMonitor *pMonitor, monitor_member_t pMonitorMember, IRequest *pTicket) {
    IRequest *const pOldTicket = ticket ();
    if (pTicket != pOldTicket) {
	if (pTicket)
	    pMonitor->onActivation ();
	if (m_pEKG) {
	    m_pEKG->cancel ();
	    if (pOldTicket)
		pOldTicket->Cancel ();
	    pMonitor->onDeactivation ();
	    m_pEKG.clear ();
	}
	if (pTicket)
	    m_pEKG.setTo (new EKG (pMonitor, pMonitorMember, pTicket));
    }
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VActivityMonitor  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityMonitor::VActivityMonitor (
    VActivity *pActivity
) : m_pActivity (pActivity), m_pIEventSink (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityMonitor::~VActivityMonitor () {
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityMonitor::OnEnd_() {
    onCompletion ();
}

void Vca::VActivityMonitor::OnError_(IError* pInterface, VString const& rMessage) {
}

void Vca::VActivityMonitor::OnEvent (
    IEventSink* pRole, IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription
) {
    OnEvent_(pEvent, pProxy, rTimestamp, rDescription);
}

void Vca::VActivityMonitor::OnEndEvent (IEventSink* pRole, IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp) {
    OnEndEvent_(pEvent, pProxy, rTimestamp);
    onCompletion (rTimestamp);
}

void Vca::VActivityMonitor::OnErrorEvent (
    IEventSink* pRole, IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage
) {
    OnErrorEvent_(pEvent, pProxy, rTimestamp, pInterface, rMessage);
}

void Vca::VActivityMonitor::OnFailureEvent (
    IEventSink* pRole, IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage
) {
    OnFailureEvent_(pEvent, pProxy, rTimestamp, pInterface, rMessage);
    onCompletion (rTimestamp);
}

void Vca::VActivityMonitor::OnEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription) {
}

void Vca::VActivityMonitor::OnEndEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp) {
    OnEvent_(pEvent, pProxy, rTimestamp, "End");
}

void Vca::VActivityMonitor::OnErrorEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage) {
    VString iDescription ("Error: ");
    iDescription << rMessage;
    OnEvent_(pEvent, pProxy, rTimestamp, iDescription);
}

void Vca::VActivityMonitor::OnFailureEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage) {
    VString iDescription ("Failure: ");
    iDescription << rMessage;
    OnEvent_(pEvent, pProxy, rTimestamp, iDescription);
}

void Vca::VActivityMonitor::onCompletion (VTime const& rEndTime) {
    m_pActivity->onMonitorCompletion (this, rEndTime);
    onCompletion ();
}

void Vca::VActivityMonitor::onCompletion () {
    onEventTicket (0);
    onEndErrTicket (0);
    onCompletion_();
}

void Vca::VActivityMonitor::onCompletion_() {
}


/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivityMonitor::onActivation () {
    m_pActivity->onMonitorCreation (this);
}

void Vca::VActivityMonitor::onDeactivation () {
    m_pActivity->onMonitorDeletion (this);
}

void Vca::VActivityMonitor::onDisconnectedEventTicket () {
    onEventTicket (0);
}

void Vca::VActivityMonitor::onDisconnectedEndErrTicket () {
    onEndErrTicket (0);
}

void Vca::VActivityMonitor::onEventTicket (IRequest *pTicket) {
    m_pEventRequest.setTo (this, &ThisClass::onDisconnectedEventTicket, pTicket);
}

void Vca::VActivityMonitor::onEndErrTicket (IRequest *pTicket) {
    m_pEventRequest.setTo (this, &ThisClass::onDisconnectedEndErrTicket, pTicket);
}

void Vca::VActivityMonitor::requestEvents () {
    IRequestSink::Reference pRequestSink;
    (new RequestSink (this, &ThisClass::onEventTicket))->getRole (pRequestSink);
    m_pActivity->supplyEvents (this, pRequestSink);
}

void Vca::VActivityMonitor::requestEndErrEvents () {
    IRequestSink::Reference pRequestSink;
    (new RequestSink (this, &ThisClass::onEndErrTicket))->getRole (pRequestSink);
    m_pActivity->supplyEndErrEvents (this, pRequestSink);
}
