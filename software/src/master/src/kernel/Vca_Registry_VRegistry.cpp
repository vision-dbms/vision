/*****  Vca_Registry_VRegistry Implementation *****/

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

#include "Vca_Registry_VRegistry.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_Registry_IContact.h"

#include "Vca_IPassiveCallback.h"

#include "Vca_VInstanceOfInterface.h"
#include "Vca_VInterfaceQuery.h"
#include "Vca_VPassiveConnector.h"

#include "Vca_VcaSelf.h"


/*******************************************************
 *******************************************************
 *****                                             *****
 *****  Vca::Registry::VRegistry::Contact::Ticket  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

namespace Vca {
    namespace Registry {
	class VRegistry::Contact::Ticket : public Vca::VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Ticket, VRolePlayer);

	//  Construction
	public:
	    Ticket (VRegistry* pRegistry, Contact* pContact);

	//  Destruction
	private:
	    ~Ticket ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IContact Role
	private:
	    VRole<ThisClass,IContact> m_pIContact;
	public:
	    void getRole (IContact::Reference& rpRole) {
		m_pIContact.getRole (rpRole);
	    }

	//  IContact Methods
	public:
	    void GetUUID (IContact* pRole, IVReceiver<uuid_t const&> *pUUIDReceiver);

	//  IRequest Methods
	public:
	    void Cancel (IRequest* pRole);

	//  Access
	public:
	    uuid_t const& uuid () const {
		return m_pContact->uuid ();
	    }

	//  State
	private:
	    VRegistry::Reference const m_pRegistry;
	    Contact::Reference const m_pContact;
	};
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VRegistry::Contact::Ticket::Ticket (
    VRegistry* pRegistry, Contact* pContact
) :m_pRegistry (pRegistry), m_pContact (pContact), m_pIContact (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Registry::VRegistry::Contact::Ticket::~Ticket () {
}
/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Registry::VRegistry::Contact::Ticket::GetUUID (IContact *pRole, IVReceiver<uuid_t const&> *pUUIDReceiver) {
    if (pUUIDReceiver)
	pUUIDReceiver->OnData (uuid ());
}

void Vca::Registry::VRegistry::Contact::Ticket::Cancel (IRequest *pRole) {
    m_pContact->disconnectFrom (m_pRegistry);
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::Registry::VRegistry::Contact  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VRegistry::Contact::Contact (
    VRegistry* pRegistry, uuid_t const &rUUID, unsigned int cUses
) : m_iUUID (rUUID), m_cUsesRemaining (cUses) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Registry::VRegistry::Contact::~Contact () {
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::Registry::VRegistry::Contact::disconnectFrom (VRegistry* pRegistry) {
    pRegistry->deleteContact (uuid ());
}

void Vca::Registry::VRegistry::Contact::onCallback (IPassiveCallback *pCallback) {
}

void Vca::Registry::VRegistry::Contact::onQuery (IObjectSink* pObjectSink, VTypeInfo* pObjectType) {
    if (pObjectSink)
	pObjectSink->OnError (0, "No Object");
}


/*******************************************************
 *******************************************************
 *****                                             *****
 *****  Vca::Registry::VRegistry::CallbackContact  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

namespace Vca {
    namespace Registry {
	class VRegistry::CallbackContact : public Contact {
	    DECLARE_CONCRETE_RTTLITE (CallbackContact, Contact);

	//  Aliases
	public:
	    typedef Reference CallbackContactReference;

	//  CallMaker
	public:
	    class CallMaker {
		DECLARE_NUCLEAR_FAMILY (CallMaker);

	    //  Construction
	    public:
		CallMaker (CallbackContact *pContact, IPassiveServer *pServer);
		CallMaker (ThisClass const &rOther);

	    //  Destruction
	    public:
		~CallMaker ();

	    //  Callback
	    public:
		void operator () (VString const &rName) const;
		void operator () (IError *pInterface, VString const &rMessage) const;

	    //  State
	    private:
		CallbackContactReference  const m_pCall;
		IPassiveServer::Reference const m_pServer;
	    };
	    friend class CallMaker;

	//  Construction
	public:
	    CallbackContact (
		VRegistry* pRegistry, uuid_t const &rUUID, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
	    );
	    
	//  Destruction
	private:
	    ~CallbackContact () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IPassiveCall Role
	private:
	    VRole<ThisClass,IPassiveCall> m_pIPassiveCall;
	public:
	    void getRole (IPassiveCall::Reference &rpRole) {
		m_pIPassiveCall.getRole (rpRole);
	    }

	//  IPassiveCall Methods
	public:
	    void GetUUID (IPassiveCall *pRole, IVReceiver<uuid_t const&> *pUUIDReceiver);

	//  ITrigger Role
	private:
	    VRole<ThisClass,ITrigger> m_pITrigger;
	public:
	    void getRole (ITrigger::Reference &rpRole) {
		m_pITrigger.getRole (rpRole);
	    }

	//  ITrigger Methods
	public:
	    void Process (ITrigger *pRole);

	//  Implementation
	private:
	    void OnEnd_();
	    void OnError_(IError *pInterface, VString const &rMessage);

	    bool onCallEnd (IPassiveCallbackReceiver::Reference &rpCallbackReceiver);
	    void onCallback (IPassiveCallback *pCallback);

	    void onEnd ();
	    void onError (IError *pInterface, VString const &rMessage);
	    void onTimeout ();

	//  State
	private:
	    IPassiveCallbackReceiver::Reference m_pCallbackReceiver;
	    VTimer::Reference m_pTimeoutTimer;
	};
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VRegistry::CallbackContact::CallbackContact (
    VRegistry* pRegistry, uuid_t const &rUUID, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
) : BaseClass (pRegistry, rUUID, cUses), m_pCallbackReceiver (pCallbackReceiver), m_pIPassiveCall (this), m_pITrigger (this) {
    retain (); {
	ITrigger::Reference pTrigger;
	getRole (pTrigger);

	VString sTimerPurpose; {
	    VString sCallUUID; {
		VkUUIDHolder iUUID(rUUID);
		iUUID.GetString (sCallUUID);
	    }
	    sTimerPurpose.printf ("Contact %s Timeout", sCallUUID.content ());
	}
	m_pTimeoutTimer.setTo (new VTimer (sTimerPurpose, pTrigger, sTimeout));
	m_pTimeoutTimer->start ();

	CallMaker iCallMaker (this, pServer);
	pConnector->getName (iCallMaker);

    } untain ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Registry::VRegistry::CallbackContact::OnEnd_() {
    onEnd ();
}
void Vca::Registry::VRegistry::CallbackContact::OnError_(IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}

void Vca::Registry::VRegistry::CallbackContact::GetUUID (IPassiveCall *pRole, IVReceiver<uuid_t const&> *pUUIDReceiver) {
    if (pUUIDReceiver)
	pUUIDReceiver->OnData (uuid ());
}

void Vca::Registry::VRegistry::CallbackContact::Process (ITrigger *pRole) {
    onTimeout ();
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

bool Vca::Registry::VRegistry::CallbackContact::onCallEnd (IPassiveCallbackReceiver::Reference &rpCallbackReceiver) {
//  Stop the timer now...
    if (m_pTimeoutTimer) {
	m_pTimeoutTimer->cancel ();
	m_pTimeoutTimer.clear ();
    }

//  ... and return the callback receiver:
    rpCallbackReceiver.claim (m_pCallbackReceiver);
    return rpCallbackReceiver.isntNil ();
}

void Vca::Registry::VRegistry::CallbackContact::onCallback (IPassiveCallback *pCallback) {
    IPassiveCallbackReceiver::Reference pCallbackReceiver;
    if (onCallEnd (pCallbackReceiver))
	pCallbackReceiver->OnData (pCallback);
}

void Vca::Registry::VRegistry::CallbackContact::onTimeout () {
    IPassiveCallbackReceiver::Reference pCallbackReceiver;
    if (onCallEnd (pCallbackReceiver))
	pCallbackReceiver->OnError (0, "Timeout");
}

void Vca::Registry::VRegistry::CallbackContact::onEnd () {
    IPassiveCallbackReceiver::Reference pCallbackReceiver;
    if (onCallEnd (pCallbackReceiver))
	pCallbackReceiver->OnEnd ();
}
void Vca::Registry::VRegistry::CallbackContact::onError (IError *pInterface, VString const &rMessage) {
    IPassiveCallbackReceiver::Reference pCallbackReceiver;
    if (onCallEnd (pCallbackReceiver))
	pCallbackReceiver->OnError (pInterface, rMessage);
}


/******************************************************************
 ******************************************************************
 *****                                                        *****
 *****  Vca::Registry::VRegistry::CallbackContact::CallMaker  *****
 *****                                                        *****
 ******************************************************************
 ******************************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VRegistry::CallbackContact::CallMaker::CallMaker (
    CallbackContact *pContact, IPassiveServer *pServer
) : m_pCall (pContact), m_pServer (pServer) {
}

Vca::Registry::VRegistry::CallbackContact::CallMaker::CallMaker (
    ThisClass const &rOther
) : m_pServer (rOther.m_pServer), m_pCall (rOther.m_pCall) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Registry::VRegistry::CallbackContact::CallMaker::~CallMaker () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Registry::VRegistry::CallbackContact::CallMaker::operator () (VString const &rName) const {
    IPassiveCall::Reference pCall;
    m_pCall->getRole (pCall);
    m_pServer->RequestCallback (pCall, rName, m_pCall->uuid ());
}
void Vca::Registry::VRegistry::CallbackContact::CallMaker::operator () (IError *pInterface, VString const &rMessage) const {
    m_pCall->onError (pInterface, rMessage);
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::Registry::VRegistry::ObjectContact  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

namespace Vca {
    namespace Registry {
	class VRegistry::ObjectContact : public Contact, public object_gofer_t {
	    DECLARE_CONCRETE_RTTLITE (ObjectContact, Contact);

	//  Query
	public:
	    class Query : public VRolePlayer {
		DECLARE_CONCRETE_RTTLITE (Query, VRolePlayer);

	    //  Construction
	    public:
		Query (ObjectContact* pObject, IObjectSink *pObjectSink, VTypeInfo *pObjectType) : m_iInterfaceQuery (pObjectSink, pObjectType) {
		    retain (); {
			pObject->supplyMembers (this, &ThisClass::onInterfaceSource, &ThisClass::onInterfaceError);
		    } untain ();
		}

	    //  Destruction
	    private:
		~Query () {
		}

	    //  Callbacks
	    public:
		void onInterfaceSource (IVUnknown *pInterfaceSource) {
		    m_iInterfaceQuery.getObject (pInterfaceSource);
		}
		void onInterfaceError (IError *pInterface, VString const &rMessage) {
		    m_iInterfaceQuery.onError (pInterface, rMessage);
		}

	    //  State
	    private:
		VInterfaceQuery const m_iInterfaceQuery;
	    };

	//  Construction
	private:
	    void makeTicket (VRegistry *pRegistry, IUpdater *pUpdater);
	public:
	    template <class object_t> ObjectContact (
		VRegistry* pRegistry, uuid_t const &rUUID, unsigned int cUses, IUpdater *pUpdater, object_t *pOffering
	    ) : BaseClass (pRegistry, rUUID, cUses), m_pOffering (pOffering) {
		makeTicket (pRegistry, pUpdater);
	    }

	//  Destruction
	private:
	    ~ObjectContact () {
	    }

	//  Callbacks
	private:
	    void onNeed () {
		m_pOffering.materializeFor (this);
		object_gofer_t::onNeed ();
	    }
	    void onData () {
		setTo (m_pOffering);
	    }

	//  Implementation
	private:
	    void onQuery (IObjectSink* pObjectSink, VTypeInfo* pObjectType);

	//  State
	private:
	    VInstanceOfInterface<IVUnknown> m_pOffering;
	};
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

void Vca::Registry::VRegistry::ObjectContact::makeTicket (VRegistry *pRegistry, IUpdater *pUpdater) {
    if (pUpdater) {
	retain (); {
	    IContact::Reference pIContact;
	    (new Ticket (pRegistry, this))->getRole (pIContact);
	    pUpdater->OnContact (pIContact, uuid ());
	} untain ();
    }
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::Registry::VRegistry::ObjectContact::onQuery (IObjectSink* pObjectSink, VTypeInfo* pObjectType) {
    (new Query (this, pObjectSink, pObjectType))->discard ();
}


/**************************************
 **************************************
 *****                            *****
 *****  Vca::Registry::VRegistry  *****
 *****                            *****
 **************************************
 **************************************/

//  Construction
Vca::Registry::VRegistry::VRegistry () : m_pIPassiveClient (this), m_pRegistry_IAccess (this) {
}

//  Destruction
Vca::Registry::VRegistry::~VRegistry () {
}

//  Registry::IAccess Callbacks
void Vca::Registry::VRegistry::GetObject (
    IAccess* pRole, IObjectSink* pObjectSink, uuid_t const& rObjectUUID, VTypeInfo* pObjectType
) {
    if (pObjectSink) {
	Contact::Reference pContact;
	if (getOldContact (pContact, rObjectUUID))
	    pContact->onQuery (pObjectSink, pObjectType);
	else {
	    VString iErrorMessage ("Object ", false); {
		VString iUUIDString; {
		    VkUUIDHolder iObjectUUID (rObjectUUID);
		    iObjectUUID.GetString (iUUIDString);
		}
		iErrorMessage << iUUIDString << " Not Found";
	    }
	    pObjectSink->OnError (0, iErrorMessage);
	}
    }
}

//  IPassiveClient Callbacks
void Vca::Registry::VRegistry::AcceptCallback (
    IPassiveClient *pRole, IPassiveCallback *pCallback, uuid_t const &rCallUUID
) {
    if (pCallback) {
	Contact::Reference pContact;
    //  If we initiated this request, complete it:
	if (getOldContact (pContact, rCallUUID))
	    pContact->onCallback (pCallback);
    //  If we initiated an unregistered INeedYou callback handled by an earlier version of the code, quietly ignore it:
	else if (self ()->sibling (rCallUUID));

    //  If none of the above, let the caller know we don't have clue about this request:
	else {
	    VString iErrorMessage ("Contact Request ", false); {
		VString iUUIDString; {
		    VkUUIDHolder iCallUUID (rCallUUID);
		    iCallUUID.GetString (iUUIDString);
		}
		iErrorMessage << iUUIDString << " Not Found";
	    }
	    pCallback->OnError (0, iErrorMessage);
	}
    }
}

//  Callback Contact Creation
void Vca::Registry::VRegistry::requestCallback (
    VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout, unsigned int cUses
) {
    Contact::Reference pContact;
    getNewContact (pContact, cUses, pConnector, pCallbackReceiver, pServer, sTimeout);
}

void Vca::Registry::VRegistry::getNewContact (
    Contact::Reference &rpContact, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
) {
    rpContact.clear ();
    do {
	VkUUIDGenerate iUUID;
	// ... when insertContact fails, the UUID is in use and we must try again
	insertContact (rpContact, iUUID, cUses, pConnector, pCallbackReceiver, pServer, sTimeout);
    } while (rpContact.isNil ());
}

bool Vca::Registry::VRegistry::insertContact (
    Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
) {
    unsigned int xElement;
    if (!m_iContactMap.Insert (rUUID, xElement))
	return false; // ... when Insert returns false, the UUID is in use and we must try again

    Contact::Reference pNewContact (new CallbackContact (this, rUUID, cUses, pConnector, pCallbackReceiver, pServer, sTimeout));
    m_iContactMap.value (xElement).setTo (pNewContact);
    rpContact.claim (pNewContact);

    return true;
}

//  Object Contact Creation
bool Vca::Registry::VRegistry::insertContact (
    Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, IUpdater* pUpdater, IVUnknown* pObject
) {
    unsigned int xElement;
    if (!m_iContactMap.Insert (rUUID, xElement))
	return false; // ... when Insert returns false, the UUID is in use and we must try again

    Contact::Reference pNewContact (new ObjectContact (this, rUUID, cUses, pUpdater, pObject));
    m_iContactMap.value (xElement).setTo (pNewContact);
    rpContact.claim (pNewContact);

    return true;
}

bool Vca::Registry::VRegistry::insertContact (
    Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, IUpdater *pUpdater, object_gofer_t *pObject
) {
//     unsigned int xElement;
//     if (!m_iContactMap.Insert (rUUID, xElement))
// 	return false; // ... when Insert returns false, the UUID is in use and we must try again

//     Contact::Reference pNewContact (new ObjectContact (this, rUUID, cUses, pUpdater, pObject));
//     m_iContactMap.value (xElement).setTo (pNewContact);
//     rpContact.claim (pNewContact);

//     return true;
    return false;
}

//  Contact Map Maintenance
bool Vca::Registry::VRegistry::getOldContact (Contact::Reference &rpContact, uuid_t const &rUUID) {
    unsigned int xElement;
    if (!m_iContactMap.Locate (rUUID, xElement))
	rpContact.clear ();
    else {
	rpContact.setTo (m_iContactMap.value (xElement));
	if (rpContact->expired ())
	    rpContact.clear ();
	else if (rpContact->lastUse ())
	    deleteContact (rUUID);
    }
    return rpContact.isntNil ();
}

bool Vca::Registry::VRegistry::deleteContact (uuid_t const &rUUID) {
    return m_iContactMap.Delete (rUUID);
}
