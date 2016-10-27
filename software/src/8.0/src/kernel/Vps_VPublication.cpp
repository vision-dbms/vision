/*****  Vps_VPublication Implementation  *****/

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

#include "Vps_VPublication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"


/***********************************************
 ***********************************************
 *****					   *****
 *****  Vca::Vps::VPublication::Watermark  *****
 *****					   *****
 ***********************************************
 ***********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VPublication::Watermark::Watermark (
    VString const &rWatermarkData
) : m_iWatermarkPreamble (WatermarkPreamble (rWatermarkData)) {
}

VString Vca::Vps::VPublication::Watermark::WatermarkPreamble (VString const &rWatermarkData) {
    VString iWatermarkPreamble (32);
    iWatermarkPreamble << "VcaNewsWM Dst:" << rWatermarkData << ",Src:";
    iWatermarkPreamble.printf ("%08X", getpid ());
    iWatermarkPreamble << ",Now:";

    return iWatermarkPreamble;
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VString Vca::Vps::VPublication::Watermark::watermark () const {
    VString iWatermark (m_iWatermarkPreamble);

    V::VTime iNow;
    iNow.asString (iWatermark);

    return iWatermark;
}


/**************************************************
 **************************************************
 *****					      *****
 *****  Vca::Vps::VPublication::Subscription  *****
 *****					      *****
 **************************************************
 **************************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT(Vca::Vps::VPublication::Subscription);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VPublication::Subscription::Subscription (
    VPublication *pPublication, ISubscriber *pSubscriber, IRecipient *pRecipient, Watermark *pWatermark, bool bSuspended
)   : m_pPublication (pPublication)
    , m_pRecipient (pRecipient)
    , m_pWatermarkableRecipient (dynamic_cast<IWatermarkedDataSink*>(pRecipient))
    , m_pMessageSink(dynamic_cast<IMessageSink*>(pRecipient))
    , m_pWatermark (pWatermark)
    , m_xState (bSuspended ? State_Suspended : State_Active)
    , m_pSuccessor (pPublication->m_pSubscriptions)
    , m_pISubscription (this)
{
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (this);
  
    pPublication->m_pSubscriptions.setTo (this);

    retain (); {
	ISubscription::Reference pRole;
	getRole (pRole);
	pSubscriber->OnData (pRole);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Vps::VPublication::Subscription::~Subscription () {
// check if the subscription has not been canceled yet....
    if (m_xState != State_Canceled) {
	m_xState  = State_Canceled;
	unlink ();
    }
}

/*************************
 *************************
 *****  Maintenance  *****
 *************************
 *************************/

void Vca::Vps::VPublication::Subscription::unlink () {
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else if (m_pPublication->m_pSubscriptions.referent () == this) //  extra protective condition...
	m_pPublication->m_pSubscriptions.setTo (m_pSuccessor);

    m_pPredecessor.clear ();
    m_pSuccessor.clear ();

    m_pPublication->decrementSubscriptions ();
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Vps::VPublication::Subscription::Suspend (ISubscription *pRole) {
    if (m_xState != State_Canceled)
	m_xState  = State_Suspended;
}

void Vca::Vps::VPublication::Subscription::Resume (ISubscription *pRole) {
    if (m_xState != State_Canceled)
	m_xState  = State_Active;
}

void Vca::Vps::VPublication::Subscription::Cancel (ISubscription *pRole) {
    if (m_xState != State_Canceled) {
	m_xState  = State_Canceled;
	unlink ();
    }
}

/*************************
 *************************
 *****  Publication  *****
 *************************
 *************************/
Vca::Vps::VPublication::Subscription *Vca::Vps::VPublication::Subscription::publish (
    VString const &rMessage
) {
  Message::Reference const pMessage (new Message (m_pPublication.referent()));
  return publish(pMessage, rMessage);
    
}

Vca::Vps::VPublication::Subscription *Vca::Vps::VPublication::Subscription::publish (
    Message *pMessage, VString const &rMessage
) {
    if (m_pRecipient && State_Active == m_xState) {
	if (m_pWatermarkableRecipient && m_pWatermark)
	    m_pWatermarkableRecipient->OnWatermarkedData (m_pWatermark->watermark (), rMessage);
        else if (m_pMessageSink.isntNil()) {
            IMessage::Reference pIMessage;
            pMessage->getRole (pIMessage);
            m_pMessageSink->OnMessage (pIMessage, rMessage);
        }
	else
	    m_pRecipient->OnData (rMessage);
    }
    return m_pSuccessor;
}

Vca::Vps::VPublication::Subscription *Vca::Vps::VPublication::Subscription::publishError (
    IError *pError, VString const &rMessage
) {
    if (m_pRecipient) 
	m_pRecipient->OnError (pError, rMessage);
    return m_pSuccessor;
}

/**************************************************
 **************************************************
 *****					      *****
 *****  Vca::Vps::VPublication::Message       *****
 *****					      *****
 **************************************************
 **************************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT(Vca::Vps::VPublication::Message);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VPublication::Message::Message(const VPublication *pPublication) 
  : m_pPublication (const_cast< VPublication *>(pPublication)),
    m_pIMessage(this)
{

}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Vps::VPublication::Message::~Message () {

}



/************************************
 ************************************
 *****                          *****
 *****  Vca::Vps::VPublication  *****
 *****                          *****
 ************************************
 ************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (Vca::Vps::VPublication);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VPublication::VPublication (VString const &rSubject)
: m_iSubject (rSubject)
, m_cSubscriptions (0)
, m_xState (State_Stopped)
, m_pIPublication  (this)
{
    traceInfo ("Creating Vca::Vps::VPublication");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Vps::VPublication::~VPublication () {
    traceInfo ("Destroying Vca::Vps::VPublication");
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Vps::VPublication::OnError_(IError *pInterface, VString const &rMessage) {
    fprintf (stderr, ">>> %s: %s.\n", subject().content (), rMessage.content ());
    Subscription::Reference pSubscription (m_pSubscriptions);
    while (pSubscription) {
	pSubscription.setTo (pSubscription->publishError (0, rMessage));
    }
}

void Vca::Vps::VPublication::SubscribeWithWatermark (
    IWatermarkablePublication *pRole, ISubscriber *pSubscriber, IWatermarkedDataSink *pRecipient, VString const &rWatermark,  bool bSuspended
) {
    addSubscriber (pSubscriber, pRecipient, new Watermark (rWatermark), bSuspended);
}

void Vca::Vps::VPublication::Subscribe (
    IPublication *pRole, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended
) {
    addSubscriber (pSubscriber, pRecipient, 0, bSuspended);
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool Vca::Vps::VPublication::start () {
    if (!stopped ())
	return false;
    m_xState = State_Started;
    return true;
}

bool Vca::Vps::VPublication::stop () {
    if (!started ())
	return false;
    m_xState = State_Stopped;
    return true;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::Vps::VPublication::addSubscriber (ISubscriber *pSubscriber, IRecipient *pRecipient, Watermark *pWatermark, bool bSuspended) {
    if (pSubscriber) {
	Subscription::IRecipient::Reference pMessageReceiver (
	    dynamic_cast<Subscription::IRecipient*>(pRecipient)
	);
	if (pMessageReceiver.isNil ())
	    pSubscriber->OnError (0, "Invalid or Missing Recipient");
	else {
	    Subscription::Reference pSubscription (
		new Subscription (this, pSubscriber, pMessageReceiver, pWatermark, bSuspended)
	    );
	    incrementSubscriptions ();
	    start ();
	}
    }
}

void Vca::Vps::VPublication::incrementSubscriptions () {
    m_cSubscriptions.increment ();
    log ("\n+++Current Subscription Count %d", m_cSubscriptions.value ());
}

void Vca::Vps::VPublication::decrementSubscriptions () {
    if (m_cSubscriptions.decrementIsZero ())
	stop ();
    log ("\n+++Current Subscription Count %d", m_cSubscriptions.value ());
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::Vps::VPublication::publish (VString const &rMessage) const {
    if (!stopped ()) {
        Message::Reference const pMessage (new Message (this));
        publish(pMessage, rMessage);
    }
}

 void Vca::Vps::VPublication::publish (Message *pMessage, VString const &rMessage) const {
    if (!stopped ()) {
	Subscription::Reference pSubscription (subscriptionListHead ());
	while (pSubscription) {
	    pSubscription.setTo (pSubscription->publish (pMessage, rMessage));
	}
    }

}

