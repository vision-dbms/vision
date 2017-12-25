/*****  Vps_VRelayPublication Implementation  *****/

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

#include "Vps_VRelayPublication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_IDirectory.h"
#include "Vps_VRegistrar.h"


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::Vps::VRelayPublication  *****
 *****                               *****
 *****************************************
 *****************************************/

namespace {
    bool TracingRelay () {
	static bool s_bTracingRelay = getenv ("NEWS_TRACING_RELAY") ? true : false;
	return s_bTracingRelay;
    }
}

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (Vca::Vps::VRelayPublication);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VRelayPublication::VRelayPublication (VString const &rSubject, IPublicationSource *pPublicationSource, RelayPublicationRegistrar* registrar)
    : BaseClass (rSubject)
    , m_pPublicationSource (pPublicationSource)
    , m_bRestarted (false)
    , m_pIPublicationReceiver (this)
    , m_pISubscriptionReceiver (this)
    , m_pIStringReceiver (this)
    , m_pITrigger (this)
    , m_iMessageCount(0)
    , m_iMessagesWanted(0)
    , m_bCollectStats( false )
                        
{
    traceInfo ("Creating Vca::Vps::VRelayPublication");
    if (registrar) {
      registrar->install(this);
      m_bCollectStats = true;
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Vps::VRelayPublication::~VRelayPublication () {
    traceInfo ("Destroying Vca::Vps::VRelayPublication");
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool Vca::Vps::VRelayPublication::start () {
    if (!BaseClass::start ())
	return false;

    IPublicationReceiver::Reference pPublicationReceiver;
    getRole (pPublicationReceiver);
    m_pPublicationSource->Supply (pPublicationReceiver);

    return true;
}

bool Vca::Vps::VRelayPublication::stop () {
    if (!BaseClass::stop ())
	return false;

    cancelSubscription ();

    return true;
}

void Vca::Vps::VRelayPublication::cancelSubscription () {
    cancelSubscriptionMonitor ();
    if (m_pSubscription) {
	m_pSubscription->Cancel ();
	m_pSubscription.clear ();
    }
}

void Vca::Vps::VRelayPublication::cancelSubscriptionMonitor () {
    if (m_pDisconnectedSourceTriggerTicket) {
	m_pDisconnectedSourceTriggerTicket->cancel ();
	m_pDisconnectedSourceTriggerTicket.clear ();
    }
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

/***************************************
 *****  IVReceiver<IPublication*>  *****
 ***************************************/

void Vca::Vps::VRelayPublication::OnData (IPublicationReceiver *pRole, IPublication *pPublication) {
    if (m_bRestarted && TracingRelay ()) {
	printf ("+++ %p: Got new supplier for %s\n", this, subject ().content ());
    }

    if (!pPublication)
	OnError (pRole, 0, "No Source at Relay");
    else if (!stopped ()) {
	ISubscriptionReceiver::Reference pSubscriptionReceiver;
	getRole (pSubscriptionReceiver);

	IWatermarkablePublication::Reference const pWatermarkablePublication (
	    dynamic_cast<IWatermarkablePublication*>(pPublication)
	);
	if (pWatermarkablePublication) {
	    static VString iWatermark;
	    static bool bWatermarkNotSet = true;
	    if (bWatermarkNotSet) {
		bWatermarkNotSet = false;
		iWatermark.printf ("%08X", getpid ());
	    }

	    IWatermarkedDataSink::Reference pDataReceiver;
	    getRole (pDataReceiver);

	    pWatermarkablePublication->SubscribeWithWatermark (pSubscriptionReceiver, pDataReceiver, iWatermark, false);
	} else {
	    IVReceiver<VString const&>::Reference pDataReceiver;
	    getRole (pDataReceiver);

	    pPublication->Subscribe (pSubscriptionReceiver, pDataReceiver, false);
	}
    }
}

/****************************************
 *****  IVReceiver<ISubscription*>  *****
 ****************************************/

void Vca::Vps::VRelayPublication::OnData (ISubscriptionReceiver *pRole, ISubscription* pSubscription) {
//  Store the subscription ...
    if (pSubscription != m_pSubscription) {
	cancelSubscription ();
	m_pSubscription.setTo (pSubscription);
    }

//  If we're stopped, 
    if (stopped ())
    //  ... immediately cancel the new subscription, ...
	cancelSubscription ();
    else {
    //  ... otherwise, monitor connectivity to our publisher:
	ITrigger::Reference pDisconnectedSourceTrigger;
	getRole (pDisconnectedSourceTrigger);

	pSubscription->requestNoRouteToPeerCallback (m_pDisconnectedSourceTriggerTicket, pDisconnectedSourceTrigger);

    }
}

/********************************************
 *****  Vca::Vps::IWatermarkedDataSink  *****
 ********************************************/

void Vca::Vps::VRelayPublication::OnWatermarkedData (IWatermarkedDataSink *pRole, VString const &rWatermark, VString const &rString) {
      if (m_bCollectStats)
        publishData(rString);
      else
        publish(rString);
}

/****************************************
 *****  IVReceiver<VString const&>  *****
 ****************************************/
 
void Vca::Vps::VRelayPublication::OnData (IVReceiver<VString const &> *pRole, VString const &rString) {
      if (m_bCollectStats)
        publishData(rString);
      else
        publish(rString);
}


void Vca::Vps::VRelayPublication::publishData(VString const &rString) {
#ifdef __VMS
      ProcessStats::Event event(m_EffStats);
#endif
      m_iMessageCount++;
      Message::Reference const pMessage (new Message (this));
      publish (pMessage, rString);
      if (pMessage->Wanted()) m_iMessagesWanted++;
}

/**********************
 *****  ITrigger  *****
 **********************/

void Vca::Vps::VRelayPublication::Process (ITrigger *pRole) {
    cancelSubscriptionMonitor ();

    m_bRestarted = true;

    if (TracingRelay ())
	printf (">>> On Disconnected Relay: %s\n", subject ().content ());

    IPublicationReceiver::Reference pPublicationReceiver;
    getRole (pPublicationReceiver);
    m_pPublicationSource->Supply (pPublicationReceiver);
}
