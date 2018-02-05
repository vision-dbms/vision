/*****  Vca_VActivity Implementation  *****/

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

#include "Vca_VActivity.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivityFilter.h"
#include "Vca_IDirectory.h"
#include "Vca_IEventSink.h"

#include "Vca_VcaGofer.h"

#include "V_VRTTI.h"


//################################################################
//################################################################
//################################################################

/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::Gofer::IActivitySink::Filter  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

namespace Vca {
    namespace Gofer {
	namespace IActivitySink {
	    typedef VGoferInstance<Vca::IActivitySink*> gofer_base_t;

	    class Filter : public gofer_base_t {
		DECLARE_CONCRETE_RTTLITE (Filter, gofer_base_t);

	    //  Construction
	    public:
		Filter (gofer_base_t *pConsumerGofer);

	    //  Destruction
	    private:
		~Filter () {
		}

	    //  Callbacks and Triggers
	    private:
		void onData () OVERRIDE;

	    //  Gofering
	    private:
		Gofering* newGofering_() OVERRIDE {
		    return 0;
		}

	    //  State
	    private:
		gofer_base_t::Reference const m_pConsumerGofer;
	    };
	}
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Gofer::IActivitySink::Filter::Filter (gofer_base_t *pConsumerGofer) : m_pConsumerGofer (pConsumerGofer) {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Gofer::IActivitySink::Filter::onData () {
    Vca::IActivitySink::Reference pIActivitySink;
    (new VActivityFilter (m_pConsumerGofer))->getRole (pIActivitySink);
    setTo (pIActivitySink);
}


//################################################################
//################################################################
//################################################################

/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VActivity::Event  *****
 *****                         *****
 ***********************************
 ***********************************/

/**********************
 **********************
 *****  Tracking  *****
 **********************
 **********************/

bool Vca::VActivity::g_bTrackingActivities  = V::GetEnvironmentBoolean ("VcaActivityMonitoring", false);
bool Vca::VActivity::g_bFilteringActivities = V::GetEnvironmentBoolean ("VcaActivityFiltering" , true);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::Event::Event (VActivity *pActivity) {
    Reference &rCurrentEvent = pActivity->m_pCurrentEvent;
    if (rCurrentEvent)
	rCurrentEvent->m_pSuccessor.setTo (this);
    else
	pActivity->m_pFirstEvent.setTo (this);
    rCurrentEvent.setTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::Event::~Event () {
}

/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivity::Event::sendTo_(EndErrSink const &rObserver) {
}

void Vca::VActivity::Event::sendTo_(EventSink const &rObserver) {
    IEvent::Reference pMySelf;
    getRole (pMySelf);

    VString iMyDescription;
    getDescription (iMyDescription);

    rObserver->OnEvent (pMySelf, pMySelf, timestamp (), iMyDescription);
}

/****************/
void Vca::VActivity::Event::sendTo (EndErrSink const &rObserver, Reference &rpMySelf) {
    rpMySelf.setTo (m_pSuccessor);
    sendTo (rObserver);
}

void Vca::VActivity::Event::sendTo (EndErrSink const &rObserver) {
    if (rObserver.isntNil ())
	sendTo_(rObserver);
}

void Vca::VActivity::Event::sendTo (EventSink const &rObserver, Reference &rpMySelf) {
    rpMySelf.setTo (m_pSuccessor);
    sendTo (rObserver);
}

void Vca::VActivity::Event::sendTo (EventSink const &rObserver) {
    if (rObserver.isntNil ())
	sendTo_(rObserver);
}


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VActivity::StartEvent  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::StartEvent::StartEvent (VActivity *pActivity) : BaseClass (pActivity) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::StartEvent::~StartEvent () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VActivity::StartEvent::getDescription_(VString &rsDescription) const {
    BaseClass::getDescription_(rsDescription);
    rsDescription << "Start";
}


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VActivity::ErrorEvent  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::ErrorEvent::ErrorEvent (
    VActivity *pActivity, IError *pInterface, VString const &iMessage
) : BaseClass (pActivity), m_pInterface(pInterface), m_iMessage(iMessage) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::ErrorEvent::~ErrorEvent () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VActivity::ErrorEvent::getDescription_(VString &rsDescription) const {
    BaseClass::getDescription_(rsDescription);
    rsDescription.printf ("Error: %s", m_iMessage.content ());
}

/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivity::ErrorEvent::sendTo_(EndErrSink const &rObserver) {
    IEvent::Reference pMySelf;
    getRole (pMySelf);
    rObserver->OnErrorEvent (pMySelf, pMySelf, timestamp (), m_pInterface, m_iMessage);
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VActivity::FinishEvent  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::FinishEvent::FinishEvent (VActivity *pActivity) : BaseClass (pActivity) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::FinishEvent::~FinishEvent () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VActivity::FinishEvent::getDescription_(VString &rsDescription) const {
    BaseClass::getDescription_(rsDescription);
    rsDescription << "Finish:";
}

/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivity::FinishEvent::sendTo_(EndErrSink const &rObserver) {
    IEvent::Reference pMySelf;
    getRole (pMySelf);
    rObserver->OnEndEvent (pMySelf, pMySelf, timestamp ());
}

void Vca::VActivity::FinishEvent::sendTo_(EventSink const &rObserver) {
    IEvent::Reference pMySelf;
    getRole (pMySelf);
    rObserver->OnEndEvent (pMySelf, pMySelf, timestamp ());
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VActivity::SuccessEvent  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::SuccessEvent::SuccessEvent (VActivity *pActivity) : BaseClass (pActivity) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::SuccessEvent::~SuccessEvent () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VActivity::SuccessEvent::getDescription_(VString &rsDescription) const {
    BaseClass::getDescription_(rsDescription);
    rsDescription << "OK";
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VActivity::FailureEvent  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::FailureEvent::FailureEvent (
    VActivity *pActivity, IError *pInterface, VString const &iMessage
) : BaseClass (pActivity), m_pInterface(pInterface), m_iMessage(iMessage) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::FailureEvent::~FailureEvent () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VActivity::FailureEvent::getDescription_(VString &rsDescription) const {
    BaseClass::getDescription_(rsDescription);
    rsDescription.printf ("Fail: %s", m_iMessage.content ());
}

/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivity::FailureEvent::sendTo_(EndErrSink const &rObserver) {
    IEvent::Reference pMySelf;
    getRole (pMySelf);
    rObserver->OnFailureEvent (pMySelf, pMySelf, timestamp (), m_pInterface, m_iMessage);
}


/****************************
 ****************************
 *****                  *****
 *****  Vca::VActivity  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivity::parent_gofer_t* Vca::VActivity::ParentGoferFor (IClient *pClient) {
    return pClient ? new Gofer::Queried<IActivity> (pClient) : static_cast<parent_gofer_t*>(0);
}

Vca::VActivity::VActivity (
    VCohort *pCohort, parent_gofer_t *pParentGofer
) : BaseClass (pCohort), m_pParentGofer (pParentGofer), m_pIActivity (this) {
}

Vca::VActivity::VActivity (
    parent_gofer_t *pParentGofer
) : m_pParentGofer (pParentGofer), m_pIActivity (this) {
}

Vca::VActivity::VActivity (
    IClient *pClient
) : m_pParentGofer (ParentGoferFor (pClient)), m_pIActivity (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivity::~VActivity () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VActivity::getAuthority_(IActivity::Reference &rpAuthority) {
    getRole (rpAuthority);
}

void Vca::VActivity::getDescription_(VString &rsDescription) const {
    V::VRTTI iRTTI (typeid (*this));
    rsDescription << iRTTI.name ();
}

void Vca::VActivity::getFormattedDescription (VString &rResult) const {
    VString iDescription;
    getDescription (iDescription);
    if (iDescription.index ('\n', 1) < 0) {	//  If there are no newlines in description, ...
    // ... use it as is:
	rResult << iDescription;
    } else {
    // ... otherwise, decorate it:
	rResult << "\n################\n" << iDescription   << "\n################";
    }
}

/****************/

bool Vca::VActivity::getLocalStartTime_(VTime &rResult) const {
    if (m_pStartEvent) {
	rResult.setTo (m_pStartEvent->timestamp ());
	return true;
    }
    return false;
}

bool Vca::VActivity::getLocalEndTime_(VTime &rResult) const {
    if (m_pFinishEvent) {
	rResult.setTo (m_pFinishEvent->timestamp ());
	return true;
    }
    return false;
}

bool Vca::VActivity::getLocalEndTimeEstimate_(VTime &rResult) const {
    if (m_pFinishEvent) {
	rResult.setTo (m_pFinishEvent->timestamp ());
	return true;
    }
    if (m_pStartEvent) {
	rResult.setTo (m_pStartEvent->timestamp ());
	adjustedEndTime (rResult);
	return true;
    }
    return false;
}

/****************/

bool Vca::VActivity::getRemoteStartTime_(VTime &rResult) const {
    return getLocalStartTime (rResult);
}

bool Vca::VActivity::getRemoteEndTime_(VTime &rResult) const {
    return getLocalEndTime (rResult);
}

bool Vca::VActivity::getRemoteEndTimeEstimate_(VTime &rResult) const {
    return getLocalEndTimeEstimate (rResult);
}

/****************/

bool Vca::VActivity::getLocalStartTime (VString &rResult) const {
    VTime iTime;
    return getLocalStartTime (iTime) && iTime.asString (rResult);
}

bool Vca::VActivity::getLocalEndTime (VString &rResult) const {
    VTime iTime;
    return getLocalEndTime (iTime) && iTime.asString (rResult);
}

bool Vca::VActivity::getLocalEndTimeEstimate (VString &rResult) const {
    VTime iTime;
    return getLocalEndTimeEstimate (iTime) && iTime.asString (rResult);
}

bool Vca::VActivity::getRemoteStartTime (VString &rResult) const {
    VTime iTime;
    return getRemoteStartTime (iTime) && iTime.asString (rResult);
}

bool Vca::VActivity::getRemoteEndTime (VString &rResult) const {
    VTime iTime;
    return getRemoteEndTime (iTime) && iTime.asString (rResult);
}

bool Vca::VActivity::getRemoteEndTimeEstimate (VString &rResult) const {
    VTime iTime;
    return getRemoteEndTimeEstimate (iTime) && iTime.asString (rResult);
}


/************************
 ************************
 *****  Adjustment  *****
 ************************
 ************************/

V::VTime const &Vca::VActivity::adjustedEndTime(VTime &rEndTimeEstimate) const {
    rEndTimeEstimate += 10 * static_cast<U64>(VTime::OneMM); // == 10 seconds
    return rEndTimeEstimate;
}


/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivity::onEvent (Event *pEvent) const {
    if (pEvent == m_pCurrentEvent)
	onEvent ();
}

void Vca::VActivity::onEvent () const {
    if (m_pCurrentEvent) {
	m_pCurrentEvent->sendTo (EndErrSink (m_pubEndErrSink));
	m_pCurrentEvent->sendTo (EventSink  (m_pubEventSink));
    }
}

void Vca::VActivity::onGoferValue (IActivitySink *pActivitySink) {
    sendTo (pActivitySink);
}

void Vca::VActivity::onGoferError (IError *pInterface, VString const &rMessage) {
}

void Vca::VActivity::getSinkGofer (sink_gofer_t::Reference &rpGofer) const {
    static sink_gofer_t::Reference s_pGofer;
    if (TrackingActivities () && s_pGofer.isNil ()) {
	s_pGofer.interlockedSetIfNil (newSinkGofer ());
    }
    rpGofer.setTo (s_pGofer);
}

Vca::VActivity::sink_gofer_t::Reference Vca::VActivity::newSinkGofer () const {
    VString iValue;
    sink_gofer_t::Reference pSinkGofer (
	new Gofer::Named<IActivitySink,IDirectory> (
	    V::GetEnvironmentString (iValue, "VcaActivityMonitorName", "Activity_Monitor")
	)
    );
    if (FilteringActivities ()) pSinkGofer.setTo (
	new Gofer::IActivitySink::Filter (pSinkGofer)
    );
    return pSinkGofer;
}

void Vca::VActivity::sendToMonitor () {
    sink_gofer_t::Reference pSinkGofer;
    getSinkGofer (pSinkGofer);
    sendTo (pSinkGofer);
}

void Vca::VActivity::sendTo (sink_gofer_t *pActivitySinkGofer) {
    if (pActivitySinkGofer) {
	pActivitySinkGofer->supplyMembers (this, &ThisClass::onGoferValue, &ThisClass::onGoferError);
    }
}

void Vca::VActivity::sendTo (IActivitySink *pActivitySink) {
    if (pActivitySink) {
	IActivity::Reference pAuthority;
	getAuthority (pAuthority);

	IActivity::Reference pMySelf;
	getRole (pMySelf);

	VTime const iStartTime (
	    m_pStartEvent ? m_pStartEvent->timestamp () : m_pFirstEvent ? m_pFirstEvent->timestamp () : VTime()
	);
	VTime iExpectedEndTime (iStartTime);

	VString iMyDescription;
	getDescription (iMyDescription);

	pActivitySink->OnActivity (pAuthority, pMySelf, iStartTime, adjustedEndTime (iExpectedEndTime), iMyDescription);
    }
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

 void Vca::VActivity::SupplyEvents (
    IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
) {
    if (pObserver) {
	m_pubEventSink.addSubscriber (pRequestSink, pObserver);

	EventSink iObserver (pObserver);
	Event::Reference pEvent (m_pFirstEvent);
	while (pEvent) {
	    pEvent->sendTo (iObserver, pEvent);
	}
    }
}

void Vca::VActivity::SupplyEndErr (
    IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
) {
    if (pObserver) {
	m_pubEndErrSink.addSubscriber (pRequestSink, pObserver);

	EndErrSink iObserver (pObserver);
	Event::Reference pEvent (m_pFirstEvent);
	while (pEvent) {
	    pEvent->sendTo (iObserver, pEvent);
	}
    }
}

void Vca::VActivity::GetSSID (IActivity *pRole, IVReceiver<ssid_t> *pClient) {
    if (pClient)
	pClient->OnData (ssid ());
}

void Vca::VActivity::GetParent (IActivity *pRole, IVReceiver<IActivity*> *pClient) {
    if (pClient) {
	if (m_pParentGofer)
	    m_pParentGofer->supplyReceiver (pClient);
	else {
	    pClient->OnData (0);
	}
    }
}

void Vca::VActivity::GetDescription (IActivity *pRole, IVReceiver<VString const&> *pClient) {
    if (pClient) {
	VString iResult;
	getDescription (iResult);
	pClient->OnData (iResult);
    }
}


/*****************************
 *****************************
 *****  Event Reporting  *****
 *****************************
 *****************************/

/*******************
 *****  Start  *****
 *******************/

Vca::VActivity::StartEvent* Vca::VActivity::newStartEvent_() {
    return new StartEvent (this);
}

void Vca::VActivity::onActivityStart () {
    if (m_pStartEvent.isNil ()) {
	m_pStartEvent.setTo (newStartEvent_());
	sendToMonitor ();
	onEvent ();
    }
}

/*******************
 *****  Error  *****
 *******************/

Vca::VActivity::ErrorEvent* Vca::VActivity::newErrorEvent_(
    IError *pInterface, VString const &rMessage
) {
    return new ErrorEvent (this, pInterface, rMessage);
}

void Vca::VActivity::onActivityError (IError *pInterface, VString const &rMessage) {
    newErrorEvent_(pInterface, rMessage);
    onEvent ();
}

/*********************
 *****  Success  *****
 *********************/

Vca::VActivity::SuccessEvent* Vca::VActivity::newSuccessEvent_() {
    return new SuccessEvent (this);
}

void Vca::VActivity::onActivitySuccess () {
    if (m_pFinishEvent.isNil ()) {
	m_pFinishEvent.setTo (newSuccessEvent_());
	onEvent ();
    }
}

/*********************
 *****  Failure  *****
 *********************/

Vca::VActivity::FailureEvent* Vca::VActivity::newFailureEvent_(
    IError *pInterface, VString const &rMessage
) {
    return new FailureEvent (this, pInterface, rMessage);
}

void Vca::VActivity::onActivityFailure (IError *pInterface, VString const &rMessage) {
    if (m_pFinishEvent.isNil ()) {
	m_pFinishEvent.setTo (newFailureEvent_(pInterface, rMessage));
	onEvent ();
    }
}


/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivity::onMonitorCreation_(VActivityMonitor *pMonitor) {
}

void Vca::VActivity::onMonitorDeletion_(VActivityMonitor *pMonitor) {
}

void Vca::VActivity::onMonitorCompletion_(VActivityMonitor *pMonitor, VTime const &rEndTime) {
}

void Vca::VActivity::sendTo_(VActivityMonitor *pMonitor, IActivitySink *pActivitySink) {
}

void Vca::VActivity::supplyEvents_(VActivityMonitor *pMonitor, IRequestSink *pRequestSink) {
}

void Vca::VActivity::supplyEndErrEvents_(VActivityMonitor *pMonitor, IRequestSink *pRequestSink) {
}
