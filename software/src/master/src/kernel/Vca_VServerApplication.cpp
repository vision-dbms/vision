/*****  Vca_VServerApplication Implementation  *****/

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

#include "Vca_VServerApplication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_IDirectory.h"
#include "Vca_IListener.h"
#include "Vca_IListenerFactory.h"
#include "Vca_IPipeFactory.h"

#include "Vca_VcaConnection.h"
#include "Vca_VcaGofer.h"
#include "Vca_VcaGofer_Weakened.h"
#include "Vca_VcaListener.h"

#include "Vca_VClassInfoHolder.h"
#include "Vca_VListenerFactory.h"

#include "Vca_VPassiveAgent.h"
#include "Vca_VPassiveCallback.h"

#include "Vca_VTrigger.h"

#include "VReceiver.h"

#include "VSimpleFile.h"


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VServerApplication::ControlRequest  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::ControlRequest::ControlRequest (
    ThisClass *pSuccessor, IPauseResume *pIPauseResume, IStop *pIStop
) : m_pSuccessor (pSuccessor), m_pIPauseResume (pIPauseResume), m_pIStop (pIStop), m_pIRequest (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServerApplication::ControlRequest::~ControlRequest () {
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/**********************
 *****  IRequest  *****
 **********************/

void Vca::VServerApplication::ControlRequest::Cancel (IRequest *pRole) {

}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

Vca::VServerApplication::ControlRequest *Vca::VServerApplication::ControlRequest::stop (bool bHardStop) const {
    if (m_pIStop)
	m_pIStop->Stop (bHardStop);
    return m_pSuccessor;
}

Vca::VServerApplication::ControlRequest *Vca::VServerApplication::ControlRequest::pause () const {
    if (m_pIPauseResume)
	m_pIPauseResume->Pause ();
    return m_pSuccessor;
}

Vca::VServerApplication::ControlRequest *Vca::VServerApplication::ControlRequest::resume () const {
    if (m_pIPauseResume)
	m_pIPauseResume->Resume ();
    return m_pSuccessor;
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VServerApplication::OfferTracker  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

namespace Vca {
    class VServerApplication::OfferTracker : public VcaOffer::Tracker {
	DECLARE_CONCRETE_RTTLITE (OfferTracker, Tracker);

    //****************************************************************
    //  CountEvent
    public:
	class CountEvent : public Event {
	    DECLARE_CONCRETE_RTTLITE (CountEvent, Event);

	//  Construction
	public:
	    CountEvent (VServerApplication* pServer, bool bIncrement);

	//  Destruction
	private:
	    ~CountEvent () {
	    }

	//  Description
	protected:
	    void getDescription_(VString& rResult) const OVERRIDE;

	//  State
	private:
	    count_t const m_cOffersNow;
	    bool    const m_bIncrement;
	};

    //****************************************************************
    //  Construction
    public:
	OfferTracker (VServerApplication *pServer) : m_pServer (pServer) {
	}

    //  Destruction
    private:
	~OfferTracker () {
	}

    //  Callbacks
    private:
	void onIncrement (VcaOffer* pOffer) OVERRIDE {
	    m_pServer->onOfferCountIncrement (this, pOffer);
	}
	void onDecrement (VcaOffer* pOffer) OVERRIDE {
	    m_pServer->onOfferCountDecrement (this, pOffer);
	}

    //  State
    private:
	VServerApplication::Reference const m_pServer;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::OfferTracker::CountEvent::CountEvent (
    VServerApplication* pServer, bool bIncrement
) : BaseClass (pServer), m_cOffersNow (pServer->activeOfferCount ()), m_bIncrement (bIncrement) {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VServerApplication::OfferTracker::CountEvent::getDescription_(VString& rResult) const {
    BaseClass::getDescription_(rResult);

    if (m_bIncrement) {
	rResult << "Offer count incremented to " << m_cOffersNow + 1;
    } else {
	rResult << "Offer count decremented to " << m_cOffersNow - 1;
    }
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VServerApplication::ServerActivity  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::ServerActivity::ServerActivity (VServerApplication *pServer) : BaseClass (pServer) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServerApplication::ServerActivity::~ServerActivity () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VServerApplication::ServerActivity::OnError_(IError *pInterface, VString const &rMessage) {
    onActivityFailure (pInterface, rMessage);
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::VServerApplication::Callback  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

namespace Vca {
    class VServerApplication::Callback : public ServerActivity {
	DECLARE_CONCRETE_RTTLITE (Callback, ServerActivity);

    //  Construction
    public:
	Callback (VServerApplication *pServer, VString const &rCallerAddress, uuid_t const &rCallerID);

    //  Destruction
    private:
	~Callback () {
	}

    //  Access
    protected:
	void getDescription_(VString &rResult) const OVERRIDE;

    //  Roles
    public:
	using BaseClass::getRole;

    //  IAckReceiver Role
    private:
	VRole<ThisClass,IAckReceiver> m_pIAckReceiver;
    public:
	void getRole (IAckReceiver::Reference &rpRole) {
	    m_pIAckReceiver.getRole (rpRole);
	}

    //  IAckReceiver Methods
    public:
	void Ack (IAckReceiver *pRole) {
	    onDone ();
	}

    //  State
    private:
	VString const m_iCallerAddress;
	VkUUIDHolder const m_iCallerID;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::Callback::Callback (
    VServerApplication *pServer, VString const &rCallerAddress, uuid_t const &rCallerID
) : BaseClass (pServer), m_iCallerAddress (rCallerAddress), m_iCallerID (rCallerID), m_pIAckReceiver (this) {
    retain (); {
	onStart ();
    } untain ();
}

void Vca::VServerApplication::Callback::getDescription_(VString& rResult) const {
    BaseClass::getDescription_(rResult);
    rResult << " ";
    m_iCallerID.GetString (rResult);
    rResult << "@";
    rResult << m_iCallerAddress;
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::VServerApplication::Listener  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

namespace Vca {
    VClassInfoHolder &VServerApplication::Listener::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addBase (BaseClass::ClassInfo ())

		.addProperty ("ListenerName"    , &ThisClass::listenerName)
		.addProperty ("RegistrationName", &ThisClass::registrationName)
		.addProperty ("ServerFile"      , &ThisClass::registrationName)
		.addProperty ("RegistrationData", &ThisClass::registrationData)

		.markCompleted ();
	}
	return iClassInfoHolder;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::Listener::Listener (
    VServerApplication *pServer, IListenerFactory *pFactory, VString const &rAddress
) : BaseClass (pServer), m_pIListenerSink (this), m_pITrigger (this), m_iRegistrationName (commandStringValue ("serverFile")) {
    retain (); {
	onStart ();

	pServer->attach (this);

	IListenerSink::Reference pMySelf;
	getRole (pMySelf);
	pFactory->SupplyListener (pMySelf, rAddress);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServerApplication::Listener::~Listener () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VServerApplication::Listener::getDescription_(VString &rResult) const {
    BaseClass::getDescription_(rResult);
    rResult << " " << "[" << m_iRegistrationName;
    if (m_iRegistrationData.isntEmpty ())
	rResult << ": " << m_iRegistrationData;
    else if (m_iListenerName.isntEmpty ())
	rResult << ": " << m_iListenerName;
    rResult << "]";
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VServerApplication::Listener::OnData (IListenerSink *pRole, IListener *pListener) {
    if (pListener && m_pIListener.setIfNil (pListener)) {
	VcaOffer::Offering const iOffering (application (), new OfferTracker (application ()));
	(new VcaListener (pListener, iOffering))->getName (
	    this, &ThisClass::onListenerName, &ThisClass::onFailure
	);
    }
}

void Vca::VServerApplication::Listener::Process (ITrigger *pRole) {
    if (hasFinished ())
	return;

    VSimpleFile iRegistration;
    VString iRegistrationData;
    if (iRegistration.GetLine (iRegistrationData, m_iRegistrationName) && iRegistrationData == m_iRegistrationData) {
	startWatching ();
    } else {
	stopListening ();
    }
}

void Vca::VServerApplication::Listener::onFailure (IError *pInterface, VString const &rMessage) {
    BaseClass::onFailure (pInterface, rMessage);
    stopListening ();
}

void Vca::VServerApplication::Listener::onListenerName (VString const &rName) {
//  Record the listener name, ...
    m_iListenerName.setTo (rName);

//  ... adjust the permissions on UNIX domain sockets, ...
    if (strchr (rName, '/')) {
	mode_t iPermissions = 0666;
	if (commandSwitchValue ("excludeGroup"))
	    iPermissions -= (mode_t)0060;
	if (commandSwitchValue ("excludeOther"))
	    iPermissions -= (mode_t)0006;
	chmod (rName, iPermissions);
    }

//  ... and register the listener in a server file if appropriate:
    if (m_iRegistrationName.isntEmpty ()) {
	VSimpleFile iRegistrationFile;
	if (iRegistrationFile.PutLine (rName, m_iRegistrationName)) {
	    m_iRegistrationData.setTo (rName);
	    startWatching ();
	} else {
	    VString iMessage;
	    iMessage.printf ("Cannot create server file '%s'.", m_iRegistrationName.content ());
	    onFailure(0, iMessage);
	}
    }
}

void Vca::VServerApplication::Listener::retract () {
    stopListening ();
}

void Vca::VServerApplication::Listener::startWatching () {
    timeout_minutes_t const sTimeout = application()->watchTimeout ();
    if (sTimeout > 0) {
	ITrigger::Reference pMySelf;
	getRole (pMySelf);

	VTimer::Reference const pWatchTimer (new VTimer ("Listener Watch Timeout", pMySelf, sTimeout * 60 * 1000 * 1000));
	pWatchTimer->start ();
    }
}

void Vca::VServerApplication::Listener::stopListening () {
    Reference const iRetainer (this);
    if (m_pIListener) {
	m_pIListener->Close ();
	m_pIListener.clear ();
    }
    application()->detach (this);
    onDone ();
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VServerApplication::Registration  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

namespace Vca {
    VClassInfoHolder &VServerApplication::Registration::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addBase (BaseClass::ClassInfo ())

		.addProperty ("RegistrationName", &ThisClass::registrationName)

		.markCompleted ();
	}
	return iClassInfoHolder;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::Registration::Registration (
    VServerApplication *pServer, registry_gofer_t *pRegistryGofer, VString const &rRegistrationName
) : BaseClass (pServer), m_pIRegistrationSink (this), m_iRegistrationName (rRegistrationName) {
    retain (); {
	onStart ();

	pServer->attach (this);

	pRegistryGofer->supplyMembers (this, &ThisClass::onRegistry, &ThisClass::onFailure);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServerApplication::Registration::~Registration () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VServerApplication::Registration::getDescription_(VString &rResult) const {
    BaseClass::getDescription_(rResult);
    rResult << " " << "[" << m_iRegistrationName;
    rResult << "]";
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VServerApplication::Registration::OnData (IRegistrationSink *pRole, IRegistration *pRegistration) {
    if (pRegistration && m_pIRegistration.setIfNil (pRegistration)) {
//	VcaOffer::Offering const iOffering (application (), new OfferTracker (application ()));
	m_pMonitor->monitor (pRegistration);
    }
}

void Vca::VServerApplication::Registration::monitor (IVUnknown *pInterface) {
    if (m_pMonitor)
	m_pMonitor->monitor (pInterface);
    else {
	m_pMonitor.setTo (new interface_monitor_t (this, &ThisClass::onDisconnect, pInterface));
    }
}

void Vca::VServerApplication::Registration::onDisconnect () {
    onFailure (0, "Disconnect");
}

void Vca::VServerApplication::Registration::onFailure (IError *pInterface, VString const &rMessage) {
    BaseClass::onFailure (pInterface, rMessage);
    retract ();
}

void Vca::VServerApplication::Registration::onRegistry (IRegistry *pRegistry) {
    if (pRegistry) {
	monitor (pRegistry);

	IRegistrationSink::Reference pMySink;
	getRole (pMySink);

	IVUnknown::Reference pMySelf;
	application ()->getRole (pMySelf);

	pRegistry->SetObject (pMySink, m_iRegistrationName, pMySelf);
    } else {
	onFailure (0, "No Registry");
    }
}

void Vca::VServerApplication::Registration::retract () {
    Reference const iRetainer (this);
    if (m_pMonitor) {
	m_pMonitor->cancel ();
	m_pMonitor.clear ();
    }
    if (m_pIRegistration) {
	m_pIRegistration->Close ();
	m_pIRegistration.clear ();
    }
    application()->detach (this);
    onDone ();
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::VServerApplication::Plumber  *****
 *****                                    *****
 **********************************************
 **********************************************/

namespace Vca {
    class VServerApplication::Plumber : public ServerActivity {
	DECLARE_CONCRETE_RTTLITE (Plumber, ServerActivity);

    //  Construction
    public:
	Plumber (VServerApplication *pServer);

    //  Destruction
    private:
	~Plumber () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IAckReceiver Role
    private:
	VRole<ThisClass,IAckReceiver> m_pIAckReceiver;
    public:
	void getRole (IAckReceiver::Reference &rpRole) {
	    m_pIAckReceiver.getRole (rpRole);
	}

    //  IAckReceiver Methods
    public:
	void Ack (IAckReceiver *pRole);
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::Plumber::Plumber (VServerApplication *pServer) : BaseClass (pServer), m_pIAckReceiver (this) {
    retain (); {
	onStart ();
    } untain ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VServerApplication::Plumber::Ack (IAckReceiver *pRole) {
    onDone ();
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VServerApplication  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServerApplication::VServerApplication (Context *pContext)
    : BaseClass			(pContext)
    , m_sOfferLimit		(UINT_MAX)
    , m_sStopTimeout		(0)
    , m_sWatchTimeout		(0)
    , m_bStoppingOnIdle		(commandSwitchValue ("stopOnIdle"))
    , m_bWaitingForBuddy	(false)
    , m_iBuddySource		(commandStringValue ("buddySource", "VcaBuddySource"))
    , m_pIBuddy 		(this)
    , m_pIPassiveServer		(this)
    , m_pIServerApplication	(this)
{
    size_t iOptionValue;
    if (getCommandValue (iOptionValue, "offerLimit", "VcaOfferLimit"))
	m_sOfferLimit = static_cast<count_t>(iOptionValue);

    if (getCommandValue (iOptionValue, "stopTimeout", "VcaStopTimeout"))
	m_sStopTimeout = static_cast<timeout_minutes_t>(iOptionValue);

    if (getCommandValue (iOptionValue, "watchTimeout", "VcaWatchTimeout"))
	m_sWatchTimeout = static_cast<timeout_minutes_t>(iOptionValue);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServerApplication::~VServerApplication () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VServerApplication::getListenerName (VString &rName) const {
    if (m_pListeners.cardinality () == 0)
	return false;

    rName << m_pListeners[0]->listenerName ();
    return true;
}

VString Vca::VServerApplication::listenerName () const {
    VString iResult;
    getListenerName (iResult);
    return iResult;
}


/****************************
 ****************************
 *****  IBuddy Methods  *****
 ****************************
 ****************************/

void Vca::VServerApplication::ControlThese (
    IBuddy *pRole, IRequestReceiver *pRequestReceiver, IPauseResume *pIPauseResume, IStop *pIStop
) {
//  References to the interfaces passed to this routine must be weakened so that the old server
//  that sent them to us isn't kept alive by them...
    if (pRequestReceiver)
	pRequestReceiver->weakenImport ();
    if (pIPauseResume)
	pIPauseResume->weakenImport ();
    if (pIStop)
	pIStop->weakenImport ();

    m_pControlRequests.setTo (new ControlRequest (m_pControlRequests, pIPauseResume, pIStop));
}

/************************************
 ************************************
 *****  IPassiveServer Methods  *****
 ************************************
 ************************************/

void Vca::VServerApplication::RequestCallback (
    IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
) {
    onCallbackRequest (pCall, rCallerAddress, rCallID);
}


/*********************************
 *********************************
 *****  Activity Accounting  *****
 *********************************
 *********************************/

void Vca::VServerApplication::onActivityCountWasZero () {
    log ("New offers (connections)...");

    VString iDeviceLogMessage;
    VString *const pDeviceLogMessage = defaultLogger().isActive () ? &iDeviceLogMessage : 0;

    if (isAwaitingStop ()) {
	if (pDeviceLogMessage)
	    pDeviceLogMessage->printf (" isAwaitingStop, canceling timer");
	cancelStopTimer ();
    }
    if (pDeviceLogMessage)
	defaultLogger().printf ("+++ onOfferCountWasZero%s.\n", pDeviceLogMessage->content ());
}

void Vca::VServerApplication::onActivityCountIsZero () {
    onStopCondition ();
}

void Vca::VServerApplication::onStopCondition () {
    if (activeOfferCount () > 0 || activityCount () > passiveOfferCount ()) 
	return;

    log ("Zero offers (connections)...");

    VString iDeviceLogMessage;
    VString *const pDeviceLogMessage = defaultLogger().isActive () ? &iDeviceLogMessage : 0;

    if (isAwaitingStop ()) {
	U64 const sTimeout = stopTimeout ();
	if (pDeviceLogMessage)
	    pDeviceLogMessage->printf (" isAwaitingStop hardstop timeout = %llu", sTimeout);
	if (sTimeout == 0) {
	    hardstop ();
	} else {
	    m_pStopTimer.setTo (
	        new VTimer (
		    "Stop Timeout", new VTrigger<ThisClass> (this, &ThisClass::onStopTimer), sTimeout*60*1000*1000
	        )
	    );
	    m_pStopTimer->start ();	    
	}
    } else if (stoppingOnIdle () && isntStopping () && isntStopped ()) {
	if (pDeviceLogMessage)
	    pDeviceLogMessage->printf (" stoppingOnIdle && isntStopping && isntStopped");
      stop ();
    }
    if (pDeviceLogMessage)
	defaultLogger().printf ("+++ onOfferCountIsZero%s.\n", pDeviceLogMessage->content ());
}


/**************************
 **************************
 *****  Connectivity  *****
 **************************
 **************************/

void Vca::VServerApplication::onBuddy (IBuddy *pBuddy) {
    m_bWaitingForBuddy = false;

//  Register and transfer control to our buddy...
    if (pBuddy) {
	m_pBuddy.setTo (pBuddy);

	IPauseResume::Reference pMyPauseResume;
	getRole (pMyPauseResume);

	IStop::Reference pMyStop;
	getRole (pMyStop);

	pBuddy->ControlThese (0, pMyPauseResume, pMyStop);
    }

//  ... and, because our buddy has taken over from us, set our own stop on idle:
    m_bStoppingOnIdle = true;
}

void Vca::VServerApplication::onBuddyGoferError (IError *pInterface, VString const &rMessage) {
    m_bWaitingForBuddy = false;

//    fprintf (stderr, "+++ Buddy Gofer Error '%s'\n", rMessage.content ());
}

void Vca::VServerApplication::offerSelf (VString const &rAddress) {
    IListenerFactory::Reference pListenerFactory;
    if (supply (pListenerFactory)) {
	(new Listener (this, pListenerFactory, rAddress))->discard ();
    }
}

void Vca::VServerApplication::offerSelf (char const* pAddress) {
    VString const sAddress (pAddress);
    offerSelf (sAddress);
}

/**
 * @todo Allow for multiple address offers.
 * @todo (Perhaps) obtain an IDirectory interface to some directory (potentially remote) and add an offer of ourselves to that directory.
 */
bool Vca::VServerApplication::offerSelf (bool bOfferMadeAtAddress) {
//  As appropriate, offer this object through...
    bool bOfferMade = false;

// ... the plumbing:
    if (commandSwitchValue ("plumbed")) {
	getStandardChannels2 ();
	bOfferMade = true;
    }

// ... at an address:
    if (char const *const pAddress = commandStringValue ("address")) {
	offerSelf (pAddress);
	bOfferMade = bOfferMadeAtAddress = true;
    }

// ... in one or more registries:
    CommandCursor iCommandCursor (this);
    while (char const *const pRegistrationData = iCommandCursor.nextString ("registration")) {
	VString const iRegistrationData (pRegistrationData);
	VString iRegistryName, iEntryName;
	if (iRegistrationData.getPrefix (':', iRegistryName, iEntryName) && iRegistryName.isntEmpty () && iEntryName.isntEmpty ()) {
	    (new Registration (this, new Gofer::Named<IDirectory,IDirectory>(iRegistryName), iEntryName))->discard ();
	    bOfferMade = true;
	}
    }

// ... and passively:
    iCommandCursor.reset ();
    while (char const *const pCallbackRequest = iCommandCursor.nextString ("callback")) {
	if (onCallbackRequest (pCallbackRequest))
	    bOfferMade = true;
    }

//  ... returning true if at least one offer was made:
    return bOfferMade;
}

bool Vca::VServerApplication::onCallbackRequest (char const *pRequestData) {
    VPassiveAgent::CallbackRequest iRequest (pRequestData);
    if (iRequest.isValid ()) {
	onCallbackRequest (0, iRequest.callerAddress (), iRequest.callID ());
	return true;
    }
    return false;
}

void Vca::VServerApplication::onCallbackRequest (IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID) {
    VcaOffer::Offering const iOffering (this, new OfferTracker (this));

    IPipeFactory::Reference pPipeFactory;
    supply (pPipeFactory);

    IAckReceiver::Reference pController;
    (new Callback (this, rCallerAddress, rCallID))->getRole (pController);

    VPassiveCallback::Reference pCallback (
	new VPassiveCallback (
	    this, pCall, rCallID, new Gofer::Named<IPassiveClient,IPipeFactory> (
		iOffering, rCallerAddress, pPipeFactory.referent ()
	    ), pController.referent ()
	)
    );
}

void Vca::VServerApplication::onStandardChannels (VBSProducer *pStdin, VBSConsumer *pStdout) {
    VcaOffer::Offering const iOffering (this, new OfferTracker (this));

    IAckReceiver::Reference pAckReceiver;
    (new Plumber (this))->getRole (pAckReceiver);

    (new VcaConnection (pStdin, pStdout, iOffering, pAckReceiver))->discard ();
}

void Vca::VServerApplication::onStandardChannels (VBSProducer *pStdin, VBSConsumer *pStdout, VBSConsumer *pStderr) {
    onStandardChannels (pStdin, pStdout);
}

bool Vca::VServerApplication::attach (Listener *pListener) {
    return m_pListeners.Insert (pListener);
}

bool Vca::VServerApplication::detach (Listener *pListener) {
    return m_pListeners.Delete (pListener);
}

bool Vca::VServerApplication::attach (Registration *pRegistration) {
    return m_pRegistrations.Insert (pRegistration);
}

bool Vca::VServerApplication::detach (Registration *pRegistration) {
    return m_pRegistrations.Delete (pRegistration);
}


/*************************
 *************************
 *****  Transitions  *****
 *************************
 *************************/

bool Vca::VServerApplication::stop_(bool bHardStop) {
    if (BaseClass::stop_(bHardStop) && !bHardStop) {
	for (
	    ControlRequest::Reference pControlRequest (m_pControlRequests);
	    pControlRequest;
	    pControlRequest.setTo (pControlRequest->stop (bHardStop))
	);
	while (m_pRegistrations.cardinality () > 0)
	    m_pRegistrations[0]->retract ();

	log ("Gracefully stopping server.....");

	setRunStateIf (RunState_Stopping, RunState_AwaitingStop);
	onStopCondition ();
	}

    return isStopping (bHardStop);
}

bool Vca::VServerApplication::pause_() {
    if (BaseClass::pause_()) {
	for (
	    ControlRequest::Reference pControlRequest (m_pControlRequests);
	    pControlRequest;
	    pControlRequest.setTo (pControlRequest->pause ())
	);
    }
    return isPausing ();
}

bool Vca::VServerApplication::resume_() {
    if (BaseClass::resume_()) {
	for (
	    ControlRequest::Reference pControlRequest (m_pControlRequests);
	    pControlRequest;
	    pControlRequest.setTo (pControlRequest->resume ())
	);
    }
    return isResuming ();
}


/**********************
 **********************
 *****  Triggers  *****
 **********************
 **********************/

void Vca::VServerApplication::onOfferCountIncrement (OfferTracker *pTracker, VcaOffer* pOffer) {
    onEvent (new OfferTracker::CountEvent (this, true));

    count_t const cOffers = ++m_cOffers;
    defaultLogger().printf ("+++ onOfferCountIncrement to %u\n", cOffers);

    if (cOffers == 1)
	onOfferCountWasZero ();
    else if (cOffers >= m_sOfferLimit && m_iBuddySource.isntEmpty () && !m_bWaitingForBuddy) {
    //  We're working too hard, get a buddy...
	m_bWaitingForBuddy = true;
	Gofer::Named<IBuddy,IDirectory>::Reference pBuddyGofer (new Gofer::Named<IBuddy,IDirectory> (m_iBuddySource));
	pBuddyGofer->supplyMembers (this, &ThisClass::onBuddy, &ThisClass::onBuddyGoferError);
    }
}

void Vca::VServerApplication::onOfferCountDecrement (OfferTracker *pTracker, VcaOffer* pOffer) {
    onEvent (new OfferTracker::CountEvent (this, false));

    if (m_cOffers.decrementIsZero ())
	onOfferCountIsZero ();
    else {
	defaultLogger().printf ("+++ onOfferCountDecrement to %u\n", m_cOffers.value ());
    }
}

void Vca::VServerApplication::onOfferCountIsZero () {
    decrementActivityCount ();
    if (activityCount () > 0 && activityCount () == passiveOfferCount ())
	onStopCondition ();
}

void Vca::VServerApplication::onOfferCountWasZero () {
    incrementActivityCount ();
}

void Vca::VServerApplication::onStopTimer (VTrigger<ThisClass> *pTrigger) {
    log ("Stop Timer triggered");

    if (isAwaitingStop () && activeOfferCount () == 0) {
	cancelStopTimer ();
	hardstop ();
    }
}

void Vca::VServerApplication::cancelStopTimer () {
    VTimer::Reference pTimer;
    pTimer.claim (m_pStopTimer);   // claim removes cyclic reference between timer and app.
    if (pTimer)
	pTimer->cancel ();
}


/*************************************
 *************************************
 *****  Tracking and Trackables  *****
 *************************************
 *************************************/

Vca::VTrackable_count_t const &Vca::VServerApplication::activeOfferCountTrackable () {
    return m_cOffers.trackable ();
}

// Local Variables:
// c-basic-offset: 4
// c-comment-only-line-offset: -4
// c-file-offsets: ((arglist-intro . +) (arglist-close . 0))
// End:
