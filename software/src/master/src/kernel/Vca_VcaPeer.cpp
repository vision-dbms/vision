/*****  Vca::VcaPeer Implementation  *****/

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

#include "Vca_VcaPeer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_IAckReceiver.h"
#include "Vca_INeedYou.h"
#include "Vca_IPassiveCall.h"
#include "Vca_IPassiveCallback.h"
#include "Vca_IPassiveConnector_Gofers.h"
#include "Vca_IPassiveServer.h"

#include "Vca_VcaTransport.h"
#include "Vca_VcaConnection.h"
#include "Vca_VcaGofer.h"
#include "Vca_VcaGofer_Weakened.h"
#include "Vca_VcaProxyFacilitator.h"
#include "Vca_VcaSelf.h"

#include "Vca_VConnectionRequest.h"
#include "Vca_VInterfaceEKG.h"
#include "Vca_VPassiveCallback.h"

#include "V_VLogger.h"
#include "V_VString.h"


/*********************
 *********************
 *****           *****
 *****  Globals  *****
 *****           *****
 *********************
 *********************/

/**************************
 **************************
 *****  Connectivity  *****
 **************************
 **************************/

namespace {

    /****************/
    Vca::VcaSite::ConnectKind CallMePolicy () {
	VString iToken;
	V::GetEnvironmentValue (iToken, "VcaCallMePolicy", "RouteTablePlus");
	return strcasecmp (iToken, "RouteTablePlus") == 0
	    ? Vca::VcaSite::ConnectKind_RouteTablePlus

	    : strcasecmp (iToken, "RouteTable") == 0
	    ? Vca::VcaSite::ConnectKind_RouteTable

	    : strcasecmp (iToken, "ConnectedSiblings") == 0
	    ? Vca::VcaSite::ConnectKind_ConnectedSiblings

	    : strcasecmp (iToken, "Siblings") == 0
	    ? Vca::VcaSite::ConnectKind_Siblings

	    : strcasecmp (iToken, "None") == 0
	    ? Vca::VcaSite::ConnectKind_None

	    : Vca::VcaSite::ConnectKind_RouteTablePlus;
    }

    /****************/
    Vca::VcaSite::ConnectKind const s_xCallMePolicy = CallMePolicy ();
}


/********************
 ********************
 *****  Logger  *****
 ********************
 ********************/

namespace {
    bool TracingAtDt () {
	static bool s_bTracingAtDt (V::GetEnvironmentBoolean ("TracingAtDt"));
	return s_bTracingAtDt;
    }
    V::VLogger const &Logger () {
	static V::VLogger const iLogger (VString () << "p_" << getpid () << "_atdt.log");
	static V::VTwiddler bInitialized;
	if (bInitialized.setIfClear ()) {
	    VString iMyName;
	    Vca::self ()->getUUID (iMyName);
	    iLogger.printf (
		"****************\n*** Self: %s, Call Me Policy: %s\n",
		iMyName.content (), Vca::VcaPeer::DescriptionFor (s_xCallMePolicy)
	    );
	}
	return iLogger;
    }
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VcaPeer::CallbackRequest  *****
 *****                                 *****
 *******************************************
 *******************************************/

namespace Vca {
    class VcaPeer::CallbackRequest : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (CallbackRequest, VRolePlayer);

    //  Aliases
    public:
	typedef VGoferInterface<INeedYou> gofer_ineedyou_t;

    //  Construction
    protected:
	CallbackRequest ();

    //  Destruction
    protected:
	~CallbackRequest ();

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

    //  IAckReceiver Callbacks
    public:
	void Ack (IAckReceiver *pRole);

    //  Callbacks
    private:
	virtual /*override*/ void OnError_(IError *pInterface, VString const &rMessage);

    //  Actions
    protected:
	virtual void onAck_() = 0;
	virtual void onNack_() = 0;

	void onError (IError *pInterface, VString const &rMessage);

	void onRequest ();
	void onResponse ();

    //  State
    private:
	unsigned int m_cUnansweredRequests;
	bool m_bAcked;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeer::CallbackRequest::CallbackRequest () : m_cUnansweredRequests (1), m_bAcked (false), m_pIAckReceiver (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaPeer::CallbackRequest::~CallbackRequest () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VcaPeer::CallbackRequest::Ack (IAckReceiver *pRole) {
    if (!m_bAcked) {
	m_bAcked = true;
	onResponse ();
	onAck_();
    }
}

void Vca::VcaPeer::CallbackRequest::OnError_(IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}


/*********************
 *********************
 *****  Actions  *****
 *********************
 *********************/

void Vca::VcaPeer::CallbackRequest::onError (IError *pInterface, VString const &rMessage) {
    onResponse ();
}

void Vca::VcaPeer::CallbackRequest::onRequest () {
    m_cUnansweredRequests++;
}

void Vca::VcaPeer::CallbackRequest::onResponse () {
    if (0 == --m_cUnansweredRequests && !m_bAcked)
	onNack_();
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VcaPeer::CallMeRequest  *****
 *****                               *****
 *****************************************
 *****************************************/

namespace Vca {
    class VcaPeer::CallMeRequest : public CallbackRequest {
	DECLARE_CONCRETE_RTTLITE (CallMeRequest, CallbackRequest);

    //  Aliases
    public:
	typedef IVReceiver<VString const&> INameSink;
	typedef IDataSource<VString const&> INameSource;

    //  Construction
    public:
	CallMeRequest (VcaPeer *pTarget);

    //  Destruction
    private:
	~CallMeRequest ();

    //  Access
    public:
	VcaPeer *target () const {
	    return m_pTarget;
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  INameSink Role
    private:
	VRole<ThisClass,INameSink> m_pINameSink;
    public:
	void getRole (INameSink::Reference &rpRole) {
	    m_pINameSink.getRole (rpRole);
	}

    //  INameSink Methods
    public:
	void OnData (INameSink *pRole, VString const &rCallbackAddress);

    //  IPassiveServer Role
    private:
	VRole<ThisClass,IPassiveServer> m_pIPassiveServer;
    public:
	void getRole (IPassiveServer::Reference &rpRole) {
	    m_pIPassiveServer.getRole (rpRole);
	}
	
    //  IPassiveServer Callbacks
    public:
	void RequestCallback (
	    IPassiveServer *pRole, IPassiveCall *pCallbackRequest, VString const &rCallbackAddress, uuid_t const &rCallbackID
	);

    //  Implementation
    private:
	void onAddressError (IError *pInterface, VString const &rMessage);
	void onAddressSource (INameSource *pAddressSource);
	void onAddress (VString const &rAddress);

    private:
	void makeRequest ();
	void retryRequestUsing (ConnectKind xConnectKind);

	void onConnectorError (IError *pInterface, VString const &rMessage);
	void onConnector (IPassiveConnector *pConnector);

	void onAck_();
	void onNack_();

	void saySomething (char const *pWhat) const;

    //  State
    private:
	VcaPeer::Reference const m_pTarget;
	IPassiveCall::Reference  m_pCallbackRequest;
	VString                  m_iCallbackAddress;
	VkUUIDHolder             m_iCallbackID;
	ConnectKind              m_xConnectKind;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeer::CallMeRequest::CallMeRequest (
    VcaPeer *pTarget
) : m_pTarget (pTarget), m_xConnectKind (s_xCallMePolicy), m_pINameSink (this), m_pIPassiveServer (this) {
    retain (); {
	m_pTarget->incrementConnectionRequestCount ();

    //  This seems to be necessary to work around misbehavior of the VMS compiler (never seen this before)...
	VGoferInterface<IPassiveConnector>::Reference const pIPC (IPassiveConnector_Gofer::DefaultConnector ());
	VGoferInterface<INameSource>::Reference const pINS (new Gofer::Queried<INameSource,IPassiveConnector> (pIPC));

	pINS->supplyMembers (
	    this, &ThisClass::onAddressSource, &ThisClass::onAddressError
	);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaPeer::CallMeRequest::~CallMeRequest () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VcaPeer::CallMeRequest::OnData (INameSink *pRole, VString const &rCallbackAddress) {
    onAddress (rCallbackAddress);
}

void Vca::VcaPeer::CallMeRequest::RequestCallback (
    IPassiveServer *pRole, IPassiveCall *pCallbackRequest, VString const &rCallbackAddress, uuid_t const &rCallbackID
) {
//  Save the callback's details, ...
    m_pCallbackRequest.setTo (pCallbackRequest);
    m_iCallbackAddress.setTo (rCallbackAddress);
    m_iCallbackID.setTo (rCallbackID);

//  ... and make the request:
    makeRequest ();
}

void Vca::VcaPeer::CallMeRequest::makeRequest () {
    IAckReceiver::Reference pAckReceiver;
    getRole (pAckReceiver);
    m_pTarget->processCallbackRequest (
	pAckReceiver, 0, m_pCallbackRequest, m_iCallbackAddress, m_iCallbackID, m_xConnectKind
    );
}

void Vca::VcaPeer::CallMeRequest::retryRequestUsing (ConnectKind xConnectKind) {
    m_xConnectKind = xConnectKind;
    onRequest ();
    makeRequest ();
}

void Vca::VcaPeer::CallMeRequest::onAddressError (IError *pInterface, VString const &rMessage) {
//  Obtain a passive connector for this callback...
    IPassiveConnector_Gofer::DefaultConnector ()->supplyMembers (this, &ThisClass::onConnector, &ThisClass::onConnectorError);
}

void Vca::VcaPeer::CallMeRequest::onAddressSource (INameSource *pAddressSource) {
    if (!pAddressSource) {
	onAddressError (0, "No Address Source");
    } else {
	INameSink::Reference pMe;
	getRole (pMe);
	pAddressSource->Supply (pMe);
    }
}

void Vca::VcaPeer::CallMeRequest::onAddress (VString const &rAddress) {
//  Save the callback's details, ...
    m_iCallbackAddress.setTo (rAddress);
    m_iCallbackID.setTo (m_pTarget->uuid ());

//  ... and make the request:
    makeRequest ();
}

void Vca::VcaPeer::CallMeRequest::onConnectorError (IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}

void Vca::VcaPeer::CallMeRequest::onConnector (IPassiveConnector *pConnector) {
    if (!pConnector)
	onError (0, "No Connector");
    else {
    //  Use the passive connector just obtained to initiate a callback request...
	IPassiveServer::Reference pIPassiveServerRole;
	getRole (pIPassiveServerRole);
	pConnector->RequestCallback (0, pIPassiveServerRole, 120 * static_cast<unsigned __int64>(1000000)); // == 2 minutes
    }
}

void Vca::VcaPeer::CallMeRequest::onAck_() {
    saySomething ("ACK");
    m_pTarget->decrementConnectionRequestCount ();
}

void Vca::VcaPeer::CallMeRequest::onNack_() {
    saySomething ("NACK");
    switch (m_xConnectKind) {
    case ConnectKind_RouteTablePlus:
	retryRequestUsing (ConnectKind_ConnectedOthers);
	break;

    default:
	m_pTarget->decrementConnectionRequestCountWithDefunctingOption ();
	break;
    }
}

void Vca::VcaPeer::CallMeRequest::saySomething (char const *pWhat) const {
    if (TracingAtDt ()) {
	VString iTargetName;
	target()->getUUID (iTargetName);
	Logger().printf ("%s: CallMeRequest: %s, Connect Kind: %s\n", iTargetName.content (), pWhat, DescriptionFor (m_xConnectKind));
    }
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VcaPeer::CallPeerRequest  *****
 *****                                 *****
 *******************************************
 *******************************************/

namespace Vca {
    class VcaPeer::CallPeerRequest : public CallbackRequest {
	DECLARE_CONCRETE_RTTLITE (CallPeerRequest, CallbackRequest);

    //  Aliases
    public:
	typedef VInterfaceEKG<ThisClass,INeedYou> forwarder_ekg_t;

    //  Construction
    public:
	CallPeerRequest (
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    uuid_t const&	rTargetID
	);

    //  Destruction
    private:
	~CallPeerRequest ();

    //  Callbacks
    private:
	void onAck_();
	void onNack_();
    private:
	void onForwarder (INeedYou *pINeedYou);

    //  Actions
    private:
	void cancelEKG ();
    public:
	void requestCallbackUsing (gofer_ineedyou_t *pForwarder);
    public:
	void onFormulation () {
	    BaseClass::onResponse ();
	}
    protected:
	void onError (IError *pInterface, VString const &rMessage) { // gcc needs this to compile the 'supplyMembers' call below
	    BaseClass::onError (pInterface, rMessage);
	}

    //  State
    private:
	IAckReceiver::Reference		const m_pAckReceiver;
	ICallbackReceiver::Reference	const m_pCallbackReceiver;
	IPassiveCall::Reference		const m_pCallbackRequest;
	VString				const m_iCallbackAddress;
	uuid_t				const m_iCallbackID;
	uuid_t				const m_iTargetID;
	forwarder_ekg_t::Reference	      m_pForwarderEKG;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeer::CallPeerRequest::CallPeerRequest (
    IAckReceiver*	pAckReceiver,
    ICallbackReceiver*	pCallbackReceiver,
    IPassiveCall*	pCallbackRequest,
    VString const&	rCallbackAddress,
    uuid_t const&	rCallbackID,
    uuid_t const&	rTargetID
) : m_pAckReceiver	(pAckReceiver)
  , m_pCallbackReceiver (pCallbackReceiver)
  , m_pCallbackRequest	(pCallbackRequest)
  , m_iCallbackAddress	(rCallbackAddress)
  , m_iCallbackID	(rCallbackID)
  , m_iTargetID		(rTargetID)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaPeer::CallPeerRequest::~CallPeerRequest () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VcaPeer::CallPeerRequest::onAck_() {
    cancelEKG ();
    if (m_pAckReceiver)
	m_pAckReceiver->Ack ();
}

void Vca::VcaPeer::CallPeerRequest::onNack_() {
    cancelEKG ();
    if (m_pAckReceiver)
	m_pAckReceiver->OnError (0, "No Route To Peer");
}

void Vca::VcaPeer::CallPeerRequest::onForwarder (INeedYou *pForwarder) {
    if (!pForwarder)
	onError (0, "No Forwarder");
    else {
	if (VcaOIDR *const pOIDR = pForwarder->oidr ())
	    pOIDR->logAsInteresting ("INeedYou");
	IAckReceiver::Reference pAckReceiver;
	getRole (pAckReceiver);

	pForwarder->PleaseCall (
	    pAckReceiver, m_pCallbackReceiver, m_pCallbackRequest, m_iCallbackAddress, m_iCallbackID, m_iTargetID
	);
	m_pForwarderEKG.setTo (new forwarder_ekg_t (this, &ThisClass::onNack_, pForwarder));
    }
}

/*********************
 *********************
 *****  Actions  *****
 *********************
 *********************/

void Vca::VcaPeer::CallPeerRequest::requestCallbackUsing (gofer_ineedyou_t *pForwarder) {
    if (pForwarder) {
	onRequest ();
	pForwarder->supplyMembers (this, &ThisClass::onForwarder, &ThisClass::onError);
    }
}

void Vca::VcaPeer::CallPeerRequest::cancelEKG () {
    if (m_pForwarderEKG) {
	m_pForwarderEKG->cancel ();
	m_pForwarderEKG.clear ();
    }
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::VcaPeer::SiteInfo  *****
 *****                          *****
 ************************************
 ************************************/

namespace Vca {
    class VcaPeer::SiteInfo : public VSiteInfo {
	DECLARE_CONCRETE_RTTLITE (SiteInfo, VSiteInfo);

    //  Aliases
    public:
	typedef Reference SiteInfoReference;

    //  ProcessInfoRequest
    public:
	class ProcessInfoRequest : public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (ProcessInfoRequest, VRolePlayer);

	//  Construction
	public:
	    ProcessInfoRequest (SiteInfo* pClient) : m_pClient (pClient), m_pIProcessInfoSink (this) {
	    }

	//  Destruction
	private:
	    ~ProcessInfoRequest () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IProcessInfoSink Role
	private:
	    VRole<ThisClass,IProcessInfoSink> m_pIProcessInfoSink;
	public:
	    void getRole (IProcessInfoSink::Reference& rpRole) {
		m_pIProcessInfoSink.getRole (rpRole);
	    }

	//  IProcessInfoSink Methods
	public:
	    void OnBasics (
		IProcessInfoSink*	pRole,
		IProcessInfo*		pProcessInfo,
		ProcessInfo::pid_t	iPID,
		ProcessInfo::uid_t	iUID,
		ProcessInfo::gid_t	iGID,
		VString const&		rHostName,
		VString const&		rUserName,
		VString const&		rGroupName
	    ) {
		m_pClient->onProcessInfo (
		    pProcessInfo, iPID, iUID, iGID, rHostName, rUserName, rGroupName
		);
	    }
 
	//  IClient Related Methods
	private:
	    void OnError_(IError* pInterface, VString const& rMessage) {
		m_pClient->onProcessInfoError (pInterface, rMessage);
	    }

	//  State
	private:
	    SiteInfoReference const m_pClient;
	};

    //  Construction
    public:
	SiteInfo (VcaPeerCharacteristics const& rCharacteristics, IPeer1 *pProcessInfoSource);

    //  Destruction
    private:
	~SiteInfo () {
	}

    //  Requests
    private:
	void onAuthoritativeSourceNeeded_();
	void onPIDNeeded_();
	void onUIDNeeded_();
	void onGIDNeeded_();
	void onHostNameNeeded_();
	void onUserNameNeeded_();
	void onGroupNameNeeded_();
    private:
	void requestProcessInfo ();

    //  Replies
    public:
	void onProcessInfo (
	    IProcessInfo*	pProcessInfo,
	    ProcessInfo::pid_t	iPID,
	    ProcessInfo::uid_t	iUID,
	    ProcessInfo::gid_t	iGID,
	    VString const&	rHostName,
	    VString const&	rUserName,
	    VString const&	rGroupName
	);
	void onProcessInfoError (
	    IError* pInterface, VString const&rMessage
	);
    private:
	void onProcessInfoReply ();

    //  Status
    public:
	bool processInfoRequestInProgress () const {
	    return m_bProcessInfoRequestInProgress;
	}

    //  State
    private:
	IPeer1::Reference const m_pProcessInfoSource;
	bool m_bProcessInfoRequestInProgress;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeer::SiteInfo::SiteInfo (
    VcaPeerCharacteristics const& rCharacteristics, IPeer1 *pProcessInfoSource
) : BaseClass (rCharacteristics), m_pProcessInfoSource (pProcessInfoSource), m_bProcessInfoRequestInProgress (false) {
    retain (); {
	cacheProcessInfo ();
    } untain ();
}


/**********************
 **********************
 *****  Requests  *****
 **********************
 **********************/

void Vca::VcaPeer::SiteInfo::onAuthoritativeSourceNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onPIDNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onUIDNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onGIDNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onHostNameNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onUserNameNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::onGroupNameNeeded_() {
    requestProcessInfo ();
}

void Vca::VcaPeer::SiteInfo::requestProcessInfo () {
    if (processInfoRequestInProgress ())
	return;

    m_bProcessInfoRequestInProgress = true;
    if (!m_pProcessInfoSource)
	onProcessInfoError (0, "Not Available");
    else {
	IProcessInfoSink::Reference pProcessInfoSink;
	(new ProcessInfoRequest (this))->getRole (pProcessInfoSink);
	m_pProcessInfoSource->GetProcessInfo (siteUUID (), pProcessInfoSink);
    }
}


/*********************
 *********************
 *****  Replies  *****
 *********************
 *********************/

void Vca::VcaPeer::SiteInfo::onProcessInfo (
    IProcessInfo*	pProcessInfo,
    ProcessInfo::pid_t	iPID,
    ProcessInfo::uid_t	iUID,
    ProcessInfo::gid_t	iGID,
    VString const&	rHostName,
    VString const&	rUserName,
    VString const&	rGroupName
) {
    if (processInfoRequestInProgress ()) {
	PID::setTo (iPID);
	UID::setTo (iUID);
	GID::setTo (iGID);
	HostName::setTo (rHostName);
	UserName::setTo (rUserName);
	GroupName::setTo (rGroupName);

	onProcessInfoReply ();
    }
}

void Vca::VcaPeer::SiteInfo::onProcessInfoError (
    IError* pInterface, VString const& rMessage
) {
    if (processInfoRequestInProgress ()) {
	PID::setTo (pInterface, rMessage);
	UID::setTo (pInterface, rMessage);
	GID::setTo (pInterface, rMessage);
	HostName::setTo (pInterface, rMessage);
	UserName::setTo (pInterface, rMessage);
	GroupName::setTo (pInterface, rMessage);

	onProcessInfoReply ();
    }
}

void Vca::VcaPeer::SiteInfo::onProcessInfoReply () {
    m_bProcessInfoRequestInProgress = false;
}


/**************************
 **************************
 *****                *****
 *****  Vca::VcaPeer  *****
 *****                *****
 **************************
 **************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

namespace Vca {
    VcaPeer::TransportPolicy VcaPeer::g_xTransportPolicy = LIFO;

    void VcaPeer::setGlobalTransportPolicyTo (TransportPolicy xPolicy) {
	g_xTransportPolicy = xPolicy;
    }
    VcaPeer::TransportPolicy VcaPeer::transportPolicy () {
	return g_xTransportPolicy;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeer::VcaPeer (
    VcaSite* pParent, Characteristics const &rCharacteristics
) : BaseClass (pParent, rCharacteristics), m_pRemoteReflectionGofer (
    new RemoteReflectionGofer_T ()
), m_pINeedYouGofer (
    new Gofer::Weakened<INeedYou> (new Gofer::Queried<INeedYou> (reflectionGofer ()))
), m_cConnectionRequests (0), m_bUnconnectable (false), m_bOnDoneSent (false), m_pINeedYou (this), m_pITrigger (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaPeer::~VcaPeer () {
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VcaPeer::PleaseCall (
    INeedYou*		pRole,
    IAckReceiver*	pAckReceiver,
    ICallbackReceiver*	pCallbackReceiver,
    IPassiveCall*	pCallbackRequest,
    VString const&	rCallbackAddress,
    uuid_t const&	rCallbackID,
    uuid_t const&	rTargetID
) {
    VcaSite::Reference const pTargetSite (sibling (rTargetID));
    if (pTargetSite)
	pTargetSite->processCallbackRequest (pAckReceiver, pCallbackReceiver, pCallbackRequest, rCallbackAddress, rCallbackID);
    else if (pAckReceiver)
	pAckReceiver->OnError (0, "Unknown Site");
}

void Vca::VcaPeer::Process (ITrigger *pRole) {
    if (isDisconnectable ())
	closeConnections ();
}


/**************************
 **************************
 *****  Connectivity  *****
 **************************
 **************************/

void Vca::VcaPeer::incrementConnectionRequestCount () {
    m_cConnectionRequests++;
}

void Vca::VcaPeer::decrementConnectionRequestCount (bool bDefunctingAllowed) {
    if (0 == --m_cConnectionRequests && bDefunctingAllowed && isntConnected () && isntDisconnectable ()) {
	markDefunct ();
    }
}

bool Vca::VcaPeer::isDisconnectable () const {
    if (TracingAtDt ()) {
	VString iName;
	getUUID (iName);
	Logger().printf ("%s: isDisconnectable? %u %u\n", iName.content (), importCount (), exportCount ());
	if (m_pImportListHead) {
	    Logger().printf ("+++ Imports:\n");
	    for (Import *pImport = m_pImportListHead; pImport; pImport = pImport->exporterListSuccessor ()) {
		VString iInfo;
		if (VcaOIDR *const pOID = pImport->oidr())
		    pOID->getInfo (iInfo, "      ");
		Logger().printf ("%s\n", iInfo.content ());
	    }
	}
	if (m_pExportListHead) {
	    Logger().printf ("+++ Exports:\n");
	    for (Export *pExport = m_pExportListHead; pExport; pExport = pExport->importerListSuccessor ()) {
		VString iInfo;
		if (VcaOID *const pOID = pExport->oid())
		    pOID->getInfo (iInfo, "      ");
		Logger().printf ("%s\n", iInfo.content ());
	    }
	}
    }
    return importCount () == 0 && exportCount () == 0;
}

/****************************************************************************
 * Routine: closeConnections
 *	Checks whether all the transports are free. If so takes each 
 * transport from the outbound free transport list and Closes it.
 * If not (some transports are being used), this routine just returns.
 * When the busy transports are returned to the free list, this routine
 * will be called again.
 ****************************************************************************/

void Vca::VcaPeer::closeConnections () {
    traceInfo ("closeConnections: Close");
    while (freeTransportListIsntEmpty ()) {
	VcaTransportOutbound::Reference pFreeTransport;
	getFreeOutboundTransport (pFreeTransport);
	pFreeTransport->Close ();
    }
}

void Vca::VcaPeer::establishConnection () {
    if (isConnected () || isConnecting () || ShutdownCalled ()) {
    } else if (isntConnectable () || isDefunct ()) {
	markDefunct ();
    } else {
	CallMeRequest::Reference const pCallbackRequest (new CallMeRequest (this));
    }
}

void Vca::VcaPeer::processCallbackRequest_(
    IAckReceiver*	pAckReceiver,
    ICallbackReceiver*	pCallbackReceiver,
    IPassiveCall*	pCallbackRequest,
    VString const&	rCallbackAddress,
    uuid_t const&	rCallbackID,
    ConnectKind         xConnectKind
) {
    CallPeerRequest::Reference const pNextHopRequest (
	new CallPeerRequest (pAckReceiver, pCallbackReceiver, pCallbackRequest, rCallbackAddress, rCallbackID, uuid ())
    );
    if (isConnected ())
	forwardCallbackRequest (pNextHopRequest);
    else switch (xConnectKind) {
    case ConnectKind_RouteTable:
    case ConnectKind_RouteTablePlus:
	for (unsigned int xRoute = 0; xRoute < m_iRouteTable.cardinality (); xRoute++)
	    m_iRouteTable.key(xRoute)->forwardCallbackRequest (pNextHopRequest);
	break;
    case ConnectKind_ConnectedOthers:
	for (unsigned int xSibling = 0; xSibling < siblings ().cardinality (); xSibling++) {
	    if (VcaPeer *const pSibling = dynamic_cast<VcaPeer*>(siblings ()[xSibling].referent())) {
		if (!m_iRouteTable.Contains (pSibling) && pSibling->isConnected ())
		    pSibling->forwardCallbackRequest (pNextHopRequest);
	    }
	}
	break;
    case ConnectKind_ConnectedSiblings:
	for (unsigned int xSibling = 0; xSibling < siblings ().cardinality (); xSibling++) {
	    if (VcaPeer *const pSibling = dynamic_cast<VcaPeer*>(siblings ()[xSibling].referent())) {
		if (pSibling->isConnected ())
		    pSibling->forwardCallbackRequest (pNextHopRequest);
	    }
	}
	break;
    case ConnectKind_Siblings:
	for (unsigned int xSibling = 0; xSibling < siblings ().cardinality (); xSibling++) {
	    if (VcaPeer *const pSibling = dynamic_cast<VcaPeer*>(siblings ()[xSibling].referent())) {
		pSibling->forwardCallbackRequest (pNextHopRequest);
	    }
	}
	break;
    default:
	break;
    }
    pNextHopRequest->onFormulation ();
}

void Vca::VcaPeer::forwardCallbackRequest (CallPeerRequest *pCallbackRequest) {
    if (isntDefunct ())
	pCallbackRequest->requestCallbackUsing (m_pINeedYouGofer);
}

bool Vca::VcaPeer::requestNoRouteToPeerCallback (
    VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended
) {
    if (defaultLogger().isActive ()) {
	VString iMyName;
	getUUID (iMyName);
	defaultLogger().printf (
	    "+++ %s: requestNoRouteToPeerCallback: %p %u\n", iMyName.content (), pEventSink, isntUsable ()
	);
    }
    rpTicket.setTo (
	new VTriggerTicket (this, m_pNoRouteToPeerCallbacks, pEventSink, isntUsable ())
    );
    return true;
}

void Vca::VcaPeer::scheduleConnectionCleanup () {
    if (isDefunct ())
	return;

    V::VTime iNow;
    if (m_iCleanupTime < iNow) {
	static bool bUninitialized = true;
	static U64 sCleanupDelay = 0;
	if (bUninitialized) {
	    sCleanupDelay = static_cast<U64>(
		V::GetEnvironmentValue ("VcaPeCoClDelay", static_cast<double>(3)) // default == 3 seconds.
	    ) * V::VTime::OneMM;
	    bUninitialized = false;
	}
	m_iCleanupTime = iNow;
	m_iCleanupTime += sCleanupDelay;

	if (m_pCleanupTimer) {
	    m_pCleanupTimer->setDelay (m_iCleanupTime - iNow);
	    m_pCleanupTimer->restart ();
	} else {
	    ITrigger::Reference pMySelf;
	    getRole (pMySelf);

	    m_pCleanupTimer.setTo (new VTimer ("Peer Cleanup", pMySelf, m_iCleanupTime - iNow));
	    m_pCleanupTimer->start ();
	}
    }
}

void Vca::VcaPeer::triggerNoRouteToPeerCallbacks () {
    if (defaultLogger().isActive ()) {
	VString iMyName;
	getUUID (iMyName);
	defaultLogger().printf ("+++ %s: triggerNoRouteToPeerCallbacks.\n", iMyName.content ());
    }
    m_bUnconnectable = true;

//  Abort all undelivered messages...
    VMessage::Reference pMsg;
    dequeue (pMsg);
    while (pMsg) {
	if (IVUnknown *const pRecipient = pMsg->recipient ()) {
	    if (VcaOIDR *const pOIDR = pRecipient->oidr ())
		pOIDR->logAsInteresting ("Aborted: No Route To Peer");
	}
	pMsg->onError (0, "\nError: Message not delivered to target Peer\n");
	dequeue (pMsg);
    }

//  ... trigger and remove all callbacks, ...
    m_pNoRouteToPeerCallbacks.triggerAll ();

//  ... and say so in our logs:
    traceInfo ("\n+++\n+++ No Route To Peer Callbacks Triggered\n+++\n+++");
}


/***********************
 ***********************
 *****  Transport  *****
 ***********************
 ***********************/

/****************************************************************************
 * Routine: attach
 *	Each site object maintains sets of inbound and outbound transports
 *  in contact with the actual site.  These methods maintain those sets.
 ****************************************************************************/

void Vca::VcaPeer::attach_(VcaTransportInbound *pTransport) {
    if (TracingAtDt ()) {
	VString iMyName;
	getUUID (iMyName);
	Logger().printf ("%s: incoming transport attached (ccr=%u).\n", iMyName.content (), m_cConnectionRequests);
    }
    m_iIncomingTransportSet.Insert (pTransport);

    pTransport->onAttach (this);
}

void Vca::VcaPeer::attach_(VcaTransportOutbound *pTransport) {
    if (TracingAtDt ()) {
	VString iMyName;
	getUUID (iMyName);
	Logger().printf ("%s: outgoing transport attached (ccr=%u).\n", iMyName.content (), m_cConnectionRequests);
    }
    m_iOutgoingTransportSet.Insert (pTransport);

    pTransport->onAttach (this); // was pTransport->startHeartBeat ();
}

void Vca::VcaPeer::detach_(VcaTransportInbound *pTransport) {
    pTransport->onDetach (this);

    if (TracingAtDt () && !isAtExit ()) {
	VString iMyName;
	getUUID (iMyName);
	Logger().printf ("%s: incoming transport detached (ccr=%u).\n", iMyName.content (), m_cConnectionRequests);
    }
    m_iIncomingTransportSet.Delete (pTransport);

    /**
     * The following code resulted in peers being marked defunct too soon, when they might be needed again later.
     * If this disconnect is indicative of a true demise of the peer, that will be recognized if a future message
     * (for example, a heartbeat) can't be sent. The peer will be marked defunct at that point.
     */
//    if (incomingTransportCount () == 0 && isntDisconnectable ())
//	markDefunct ();
    if (isDefunct()) 
	deleteEvaluators();
}

void Vca::VcaPeer::detach_(VcaTransportOutbound *pTransport) {
    pTransport->onDetach (this);

    if (TracingAtDt () && !isAtExit ()) {
	VString iMyName;
	getUUID (iMyName);
	Logger().printf ("%s: outgoing transport detached (ccr=%u).\n", iMyName.content (), m_cConnectionRequests);
    }
    m_iOutgoingTransportSet.Delete (pTransport);

    if (isntDisconnectable ())
	establishConnection ();
}

void Vca::VcaPeer::startNextSerializerOn_(VcaTransportInbound *pTransport) {
    pTransport->startMessage ();
}

void Vca::VcaPeer::startNextSerializerOn_(VcaTransportOutbound *pTransport) {
    if (freeTransportListIsntEmpty () || invocationQueueIsEmpty ()) {
	addFreeOutboundTransport (pTransport);
    } else {
	VMessage::Reference pNextMessage;

	pNextMessage.claim (m_pMessageQueueHead);
	m_pMessageQueueHead.claim (pNextMessage->m_pSuccessor);
	if (m_pMessageQueueHead.isNil ())
	    m_pMessageQueueTail.clear ();

	pTransport->startMessage (pNextMessage);
    }
}

bool Vca::VcaPeer::getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport) {
    if (m_pTransportHead.isNil ())
	return false;

    if (pTransport == m_pTransportHead) { // found at head
	m_pTransportHead.claim (m_pTransportHead->m_pSuccessor);
	if (m_pTransportHead.isNil ())
	    m_pTransportTail.clear ();
	return true;
    }

    VcaTransportOutbound *pPrev = m_pTransportHead, *pCurrent = pPrev->m_pSuccessor;

    while (pCurrent) {

	//  Found required transport...
	if (pTransport == pCurrent) {

	    if (pCurrent == m_pTransportTail) { // found at tail
		m_pTransportTail.setTo (pPrev);
		pPrev->m_pSuccessor.clear ();
	    }
	    else { //  found in middle
		pPrev->m_pSuccessor.claim (pCurrent->m_pSuccessor);
	    }
	    return true;
	}

	pPrev = pCurrent;
	pCurrent = pCurrent->m_pSuccessor;
    }

    return false;
}

/*******************************************************************************/

void Vca::VcaPeer::addFreeOutboundTransport (VcaTransportOutbound *pFreeTransport) {
    switch(transportPolicy()) {
    case FIFO:
	if (m_pTransportTail.isntNil()) 
	    m_pTransportTail->m_pSuccessor.setTo (pFreeTransport);
	else 
	    m_pTransportHead.setTo (pFreeTransport);

	m_pTransportTail.setTo (pFreeTransport);
	break;

    case LIFO:
	if (m_pTransportTail.isNil ())
	    m_pTransportTail.setTo (pFreeTransport);
	pFreeTransport->m_pSuccessor.claim (m_pTransportHead);
	m_pTransportHead.setTo (pFreeTransport);
	break;
    }

    //  Trigger for terminating connections.....
    if (isDefunct ())
	closeConnections ();
}

void Vca::VcaPeer::getFreeOutboundTransport(VcaTransportOutbound::Reference &rpFreeTransport) {
    switch(transportPolicy()) {
    case FIFO:
    case LIFO:
	rpFreeTransport.claim  (m_pTransportHead);
	m_pTransportHead.claim (rpFreeTransport->m_pSuccessor);
	if (m_pTransportHead.isNil ())
	    m_pTransportTail.clear ();
	break;
    }
}

unsigned int Vca::VcaPeer::freeTransportCount () const {
    unsigned int count = 0;
    for (VcaTransportOutbound *pTransport = m_pTransportHead; pTransport; pTransport = pTransport->m_pSuccessor)
	count++;
    return count;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VcaPeer::deleteEvaluators () {
    if (incomingTransportCount () == 0) {
	while (m_pEvaluators) {
	    Evaluator::Reference pEvaluator (m_pEvaluators);
	    pEvaluator->onDefunctPeer ();
	}
    }
}

void Vca::VcaPeer::markDefunct_() {
    if (TracingAtDt ()) {
	VString iMyName;
	getUUID (iMyName);
	Logger().printf (
	    "%s: marked defunct: Imports: %u+%u, Exports: %u+%u\n",
	    iMyName.content (), importCount (), weakImportCount (), exportCount (), weakExportCount ()
	);
    }
    traceInfo ("\n+++\n+++ markDefunct (VcaPeer)\n+++\n+++");

//  Cancel the cleanup timer if there is one...
    if (m_pCleanupTimer) {
	m_pCleanupTimer->cancel ();
	m_pCleanupTimer.clear ();
    }

//  Mark all exports to this peer as unneeded...
    while (m_pExportListHead) {
	Export::Reference pExport (m_pExportListHead);
	pExport->onDefunctPeer ();
    }

//  Remove this peer as a source of imports...
    while (m_pImportListHead) {
	Import::Reference pImport (m_pImportListHead);
	pImport->onDefunctPeer ();
    }

//  And cleanup everything else...
    triggerNoRouteToPeerCallbacks ();

// clear remote reflection
    m_pRemoteReflection.clear();
    m_pRemoteReflectionV1.clear();
    m_pRemoteReflectionV2.clear();
    m_pRemoteReflectionGofer->setTo(0);

    VcaOIDR::onDefunctPeer (this);

    unstance ();

    closeConnections ();
    deleteEvaluators ();

    BaseClass::markDefunct_();
#ifdef TracingVcaMemoryLeaks
    log("PoolMemoryLeak VcaSite::markDefunct referenceCount: %u exportCount: %u exportedInterfaceCount: %u instantiatedInterfaceCount: %u referencedInterfaceCount: %u", 
	referenceCount(), exportCount(), exportedInterfaceCount(), instantiatedInterfaceCount(), referencedInterfaceCount());
#endif
}

/*--------------------------------------------*
 *  THIS ROUTINE IS A MAJOR WORK IN PROGRESS  *
 *--------------------------------------------*/
void Vca::VcaPeer::onShutdown () {
//  Give importers a chance to find different paths to their imports...
    traceInfo ("shutdown: exports");
    Export::Reference pExport (m_pExportListHead);
    while (pExport) {
	Export::Reference pNextExport (pExport->importerListSuccessor ());
	pExport->onShutdown ();
	pExport.claim (pNextExport);
    }

//  Give exporters a chance to learn their message counts...
    traceInfo ("shutdown: imports");
    Import::Reference pImport (m_pImportListHead);
    while (pImport) {
	Import::Reference pNextImport (pImport->exporterListSuccessor ());
	pImport->onShutdown ();
	pImport.claim (pNextImport);
    }

    if (objectsTrace ())
	traceInfo (string ("shutdown: OnDone: %p", m_pRemoteReflection.referent ()));
    if (m_pRemoteReflection && (isConnected () || isConnecting ())) {
	m_pRemoteReflection->OnDone ();
	m_bOnDoneSent = true;
    }

    closeConnections ();
}


/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

/****************************************************************************
 * FreeOutboundTransportList   PendingMessageQueue    Action
 *
 * 1. NonEmpty		    NonEmpty		    Shouldnt arise
 * 2. NonEmpty		    Empty		    Remove free tr. and invoke
 * 3. Empty		    NonEmpty		    Enqueue Message
 * 4. Empty		    Empty		    Enqueue Message, 
 *						    establish dynamic connection			
****************************************************************************/

void Vca::VcaPeer::evaluateOutgoing_(VMessage *pMessage) {
    bool bUndeliverable = false;
    if (isntUsable ()) {
	if (VcaOIDR *const pOIDR = pMessage->recipient ()->oidr ())
	    pOIDR->logAsInteresting ("Dead Message");
	
	bUndeliverable = true;
	traceInfo ("DeadMessage: Unconnectable peer");
    } else if (m_bOnDoneSent) {
	if (VcaOIDR *const pOIDR = pMessage->recipient ()->oidr ())
	    pOIDR->logAsInteresting ("Late Message");

	bUndeliverable = true;
	traceInfo ("LateMessage: OnDone message already sent");
    } else if (freeTransportListIsntEmpty ()) {		//case 2
	VcaTransportOutbound::Reference pTransport;
	getFreeOutboundTransport (pTransport);
	if (m_pCleanupTimer) {
	    if (VcaOIDR *const pOIDR = pMessage->recipient ()->oidr ()) {
		VString iMessage;
		iMessage.printf ("Connection Cleanup Window: %s", pMessage->applicable_()->name ());
		pOIDR->logAsInteresting (iMessage);
	    }
	}
	pTransport->startMessage (pMessage);
    } else {						//case 4
	if (VcaOIDR *const pOIDR = pMessage->recipient ()->oidr ()) {
	    VString iMessage;
	    iMessage.printf ("Connection Trigger: %s", pMessage->applicable_()->name ());
	    pOIDR->logAsInteresting (iMessage);
	}

	enqueue (pMessage);
	establishConnection ();
    }
    if (bUndeliverable)
	pMessage->onError (0, "\nError: Message not delivered to target Peer\n");
}

void Vca::VcaPeer::enqueue (VMessage *pMessage) {
    if (m_pMessageQueueHead.isNil ())
	m_pMessageQueueHead.setTo (pMessage);
    else 
	m_pMessageQueueTail->m_pSuccessor.setTo (pMessage);
  
    m_pMessageQueueTail.setTo (pMessage);
}

void Vca::VcaPeer::dequeue (VMessage::Reference &pMsg) {
    pMsg.setTo (m_pMessageQueueHead);
    if (pMsg) {
	m_pMessageQueueHead.claim (pMsg->m_pSuccessor);
	if (m_pMessageQueueHead.isNil ())
	    m_pMessageQueueTail.clear ();
    }
}


/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void Vca::VcaPeer::onExportCountIsZero () {
    if (isDisconnectable ())
	scheduleConnectionCleanup ();

    BaseClass::onExportCountIsZero ();
}

void Vca::VcaPeer::onExportCountWasZero () {
    BaseClass::onExportCountWasZero ();
}

void Vca::VcaPeer::createExportOf (IVUnknown *pObject, bool bWeak) {
    pObject->createExportTo (this, bWeak && supportsWeak ());
}

void Vca::VcaPeer::createExportOf (VcaOID *pOID, bool bWeak) {
    pOID->createExportTo (this, bWeak && supportsWeak ());
}

void Vca::VcaPeer::deleteExportOf (
    VMessageHolder<IPeer_Ex2> const& rMessage, VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages
) {
    if (rMessage.vinterface()->oid() == pOID) {
#ifdef TracingVcaMemoryLeaks
	log("PoolMemoryLeak VcaPeer::deleteExportOf 'this' message sequence number capture"); 
#endif
	m_iReleaseMessageSequenceNumber = rMessage.message()->sequenceNumber();
    } 

    pOID->deleteExportTo (this, cExports, cWeakExports, cMessages);
}

bool Vca::VcaPeer::weakenExportOf (VcaOID *pOID) {
    return pOID->weakenExportTo (this);
}

bool Vca::VcaPeer::weakenRemoteImportOf (VcaOID* pOID) {
    if (m_pRemoteReflectionV1) {
	m_pRemoteReflectionV1->WeakenImport (pOID->objectSiteUUID (), pOID->objectSSID ());
	return true;
    }
    return false;
}

/***************************
 ***************************
 *****  Object Import  *****
 ***************************
 ***************************/

void Vca::VcaPeer::onImportCountIsZero () {
    if (isDisconnectable ())
	scheduleConnectionCleanup ();

    BaseClass::onImportCountIsZero ();
}

void Vca::VcaPeer::onImportCountWasZero () {
    BaseClass::onImportCountWasZero ();
}

void Vca::VcaPeer::createImportOf (VcaOID *pOID, bool bWeak) {
    pOID->createImportFrom (this, bWeak && supportsWeak ());
}

void Vca::VcaPeer::deleteImportOf (VcaOID *pOID) {
    pOID->deleteImportFrom (this);
}

bool Vca::VcaPeer::weakenImportOf (VcaOID *pOID) {
    return pOID->weakenImportFrom (this);
}

bool Vca::VcaPeer::receive_(
    VcaOID::Reference&	rpOID,
    VcaSSID const&	rSSID,
    VTypeInfo*		pType,
    VcaSite*		pSource,
    bool		bWeak
) {
    return receive (rpOID, rSSID, pType, pSource, bWeak);
}

bool Vca::VcaPeer::receive_(
    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
) {
    return receive (rpOID, rSSID, pSource, bWeak);
}

bool Vca::VcaPeer::receive (
    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VTypeInfo *pType, VcaSite *pSource, bool bWeak
) {
    unsigned int xObject;
    if (m_iObjectTable.Insert (rSSID, xObject))
	m_iObjectTable[xObject] = new VcaOIDR (rSSID, pType, this);
    rpOID.setTo (m_iObjectTable[xObject]);
    pSource->createImportOf (rpOID, bWeak);
    return true;
}

bool Vca::VcaPeer::weakenRemoteExportOf (VcaOIDR* pOIDR) {
    if (m_pRemoteReflectionV1) {
	m_pRemoteReflectionV1->WeakenExport (pOIDR->objectSiteUUID (), pOIDR->objectSSID ());
	return true;
    }
    return false;
}


/***************************************
 ***************************************
 *****  Remote Export Maintenance  *****
 ***************************************
 ***************************************/

void Vca::VcaPeer::deleteRemoteExport (
    VcaOIDR const *pOIDR, U32 cImports, U32 cWeakImports, U32 cMessages
) const {
    if (m_pRemoteReflectionV2) {
    //  Try to suppress release of the remote reflection so old versions of the code don't break...
	if (m_pRemoteReflection->oidr () != pOIDR);
	else if (cImports > 0)
	    cImports--;
	else if (cWeakImports > 0)
	    cWeakImports--;

    //  ... and only send messages that have something to say:
	if (0 == cImports && 0 == cWeakImports && 0 == cMessages)
	    traceInfo ("ReleaseExport suppressed");
	else {
	    m_pRemoteReflectionV2->ReleaseExportEx (
		pOIDR->objectSiteUUID (), pOIDR->objectSSID (), cImports, cWeakImports, cMessages
	    );
	}
    } else if (m_pRemoteReflection) {
    //  Try to suppress release of the remote reflection so old versions of the code don't break...
	if (m_pRemoteReflection->oidr () != pOIDR);
	else if (cImports > 0)
	    cImports--;

    //  ... and only send messages that have something to say:
	if (0 == cImports && 0 == cMessages)
	    traceInfo ("ReleaseExport suppressed");
	else m_pRemoteReflection->ReleaseExport (
	    pOIDR->objectSiteUUID (), pOIDR->objectSSID (), cImports, cMessages
	);
    }
}

/***************************************
 ***************************************
 *****  Remote Import Maintenance  *****
 ***************************************
 ***************************************/

void Vca::VcaPeer::createRemoteImportOf (VcaOID *pOID) const {
    if (!pOID->isExportedTo (this) && m_pRemoteReflection) {
	IVUnknown::Reference pObject;
	pOID->supplyInterface_(pObject);
	m_pRemoteReflection->SinkInterface (pObject);
    }
}

void Vca::VcaPeer::deleteRemoteImportOf (VcaOIDR *pOIDR) const {
    if (m_pRemoteReflection)
	m_pRemoteReflection->ReleaseImport (pOIDR->objectSiteUUID (), pOIDR->objectSSID ());
}

/***************************************
 ***************************************
 *****  Remote Fake Export/Import  *****
 ***************************************
 ***************************************/

void Vca::VcaPeer::fakeRemoteExportTo (
    VcaOIDR const *pOIDR, VcaPeer const *pTargetPeer
) const {
    m_pRemoteReflection->FakeExportTo (
	pTargetPeer->uuid (), pOIDR->objectSiteUUID (), pOIDR->objectSSID ()
    );
}

void Vca::VcaPeer::fakeRemoteImportFrom (   
    VcaOIDR const *pOIDR, VcaPeer const *pSourcePeer
) const {
    m_pRemoteReflection->FakeImportFrom (
	pSourcePeer->uuid (), pOIDR->objectSiteUUID (), pOIDR->objectSSID ()
    );
}

void Vca::VcaPeer::setRemoteRemoteReflectionFor (VcaPeer const *pPeer, IPeer *pIPeer) const {
    m_pRemoteReflection->SetRemoteReflectionFor (pPeer->uuid (), pIPeer);
}

/*******************************
 *******************************
 *****  Remote Reflection  *****
 *******************************
 *******************************/

void Vca::VcaPeer::setReflection (IPeer *pReflection) {
    if (m_pRemoteReflection.isNil () && !supplierOf (pReflection)) {
	m_pRemoteReflection.setTo (pReflection);
	m_pRemoteReflectionV1.setTo (dynamic_cast<IPeer1*>(pReflection));
	m_pRemoteReflectionV2.setTo (dynamic_cast<IPeer2*>(pReflection));
	m_pRemoteReflectionGofer->setTo (pReflection);
    }
}


/*********************
 *********************
 *****  Routing  *****
 *********************
 *********************/

/****************************************************************************
 * Routine: addRoute
 * Operation:
 *	This routine sets routing information to reach "this" peer via the 
 * argument peer and the statistics (hop count). If the argument routing peer 
 * exists already, then alters the existing statistics information else creates 
 * a new entry. Then calls setBestRoutingStatistics() to calculate the updated 
 * latest best route to "this" peer.
*****************************************************************************/

void Vca::VcaPeer::addRoute (
    VcaPeer *pRoutingSite, VcaRouteStatistics const &rStatistics
) {
    unsigned int xElement;
    m_iRouteTable.Insert (pRoutingSite, xElement);
    m_iRouteTable[xElement] = rStatistics;
    setBestRoutingStatistics();
}

// Deletes an existing routing peer entry

bool Vca::VcaPeer::deleteRoute (VcaPeer *pVcaPeer) {
    return m_iRouteTable.Delete (pVcaPeer);
}

bool Vca::VcaPeer::containsRoutingPeer (VcaPeer *pRoutingPeer) const {
    return pRoutingPeer && m_iRouteTable.Contains (pRoutingPeer);
}

bool Vca::VcaPeer::containsRoutingPeer (VcaSite *pRoutingPeer) const {
    return containsRoutingPeer (dynamic_cast<VcaPeer*>(pRoutingPeer));
}

/****************************************************************************
 * Routine : setBestRoutingStatistics
 * Operation:
 *	Iterates through the Vca Routing Peer entries and sets the best 
 * routing information. This routine sets the m_iBestRouteMetrics
 * VcaPeer member variable. This variable is used during serialization process
 * to indicate to other peers the best route that the host peer can
 * offer to reach "this" peer. Also sets the best Peer  offering the route.
 * 
 * Precondition: 
 *  There will be atleast one entry in the RoutingPeersTable, This function 
 *  is only called from addRoute ().
****************************************************************************/

void Vca::VcaPeer::setBestRoutingStatistics () {
    unsigned int xBestRoute = 0;
    unsigned int cBestHopCount = m_iRouteTable[xBestRoute].hopCount();

    for (unsigned int i=1; i<m_iRouteTable.cardinality(); ++i) {
        VcaRouteStatistics statistics (m_iRouteTable[i]);
        if (statistics.hopCount() < cBestHopCount) {
	    xBestRoute = i;
	    cBestHopCount = statistics.hopCount();
	}
    }
    m_pBestRoute.setTo (m_iRouteTable.key(xBestRoute));
    m_iBestRouteMetrics = m_iRouteTable[xBestRoute];
    
    // Need to place logic which checks whether routing information to this peer has
    // changed from the earlier one. If so need to send the new routing statistics
    // information to all the peers in the RoutingPeersTable
}

/****************************************************************************
 * Routine : getHopCountFor
 * Operation:
 *	 This method retrieves the hop count that it takes to reach "this" 
 * peer from the "pRoutingPeer"
 ***************************************************************************/

bool Vca::VcaPeer::getHopCountFor (VcaPeer *pRoutingPeer, unsigned int &rHopCount) const {
    if (m_iRouteTable.Contains (pRoutingPeer)) {
	unsigned int xIndex;
	m_iRouteTable.Locate (pRoutingPeer, xIndex);
	rHopCount = m_iRouteTable.value (xIndex).hopCount();
	return true;
    }
    return false;
}

bool Vca::VcaPeer::getNextHop (IPeer::Reference &rpPeer) {
    if (m_pBestRoute) {
	m_pBestRoute->getReflection (rpPeer);
	return true;
    }
    return false;
}


/***************************************
 ***************************************
 ******  Remote Peer Maintenance  ******
 ***************************************
 ***************************************/

void Vca::VcaPeer::getRemoteLocalInterfaceFor (
    VcaPeer *pPeer, IVReceiver<IPeer*>*pReceiver
) const {
    m_pRemoteReflection->GetLocalInterfaceFor (pPeer, pReceiver);
}   


/*******************************************
 *******************************************
 *****  Peer Connection Data Gathering *****
 *******************************************
 *******************************************/

/****************************************************************************
  * Routine: GatherConnectionData
  * Operation:
  *	Copies the incoming PeerDataArrays(forward and reverse) and  based 
  * on the direction, inserts the local peer data value in the appropriate 
  * data structure(forward or reverse). If the local peer is the destination 
  * then runs the setFinalPathPeers method, which sets the final peers that 
  * could be used for establishing connection
****************************************************************************/

void Vca::VcaPeer::GatherConnectionData (
    IConnectionRequest*	    pRequest,
    VPeerDataArray const&   rOldForwardData,
    VPeerDataArray const&   rOldReverseData,
    uuid_t const&	    targetPeer,
    bool		    bForward
) {
#if 0
    VPeerDataArray iNewData (bForward ? rOldForwardData : rOldReverseData);
    iNewData.appendLocalData (self ());

    VPeerDataArray const &rNewForwardData = bForward ? iNewData : rOldForwardData;
    VPeerDataArray const &rNewReverseData = bForward ? rOldReverseData : iNewData;

    if (targetPeer->isntSelf () ())) {
    // reached intermediate peer
	display ("\nForwarding \n");
	VcaPeer *pTargetPeer = sibling (targetPeer);
	if (pTargetPeer) {
	    IPeer::Reference pNextHop;
	    if (pTargetPeer->getNextHop (pNextHop)) pNextHop->GatherConnectionData (
		pRequest, rNewForwardData, rNewReverseData, targetPeer, bForward
	    );
	}
	else {
	    // fail the connection request
	}
    }

    else if (bForward) {
    //  end of forward path 
	GatherConnectionData (
	    new VConnectionRequest (), rNewForwardData, rNewReverseData,
	    iNewData[0].peerUUID (), false
	);
    }
    else {
    // end of reverse path
//	onConnectionData (iForwardData, iNewData);

	rNewForwardData.setFinalPathPeers (
	    targetPeer, rNewForwardData[rNewForwardData.cardinality ()-1].peerUUID ()
	); 
	rNewForwardData.display ("Forward Route Path:");

	rNewReverseData.setFinalPathPeers (targetPeer, rNewReverseData[0].peerUUID()); 
	rNewReverseData.display ("Forward Route Path:");

	/*uuid_t nextTarget;
	if (rNewReverseData.pathLength () < rNewForwardData.pathLength ()) {
	    rNewReverseData.getNextFinalPathPeer (
		targetPeer, nextTarget, rNewReverseData[0].peerUUID ()
	    );
	    VcaPeer *pPeer = sibling (nextTarget);

	    CreateConnection (rNewReverseData, nextTarget, rNewReverseData[0].peerUUID ());
	}
	else {
	    rNewForwardData.getNextFinalPathPeer (
		targetPeer, nextTarget, 
		rNewForwardData[rNewForwardData.cardinality()-1].peerUUID ()
	    );
	    CreateConnection (
		rNewForwardData, nextTarget, 
		rNewForwardData[rNewForwardData.cardinality()-1].peerUUID ()
	    );
	}*/
    }
#endif
}

void Vca::VcaPeer::CreateConnection (
    VPeerDataArray const &peerDataArray, uuid_t const &targetUUID, uuid_t const &finalUUID
) {
#if 0
    peerDataArray.display ("Received VPeerDataArray");
    uuid_t const &localUUID = self ()->uuid () ();

    VcaPeer *pTargetPeer = 0; uuid_t nextTarget;

    // If I am the destination peer
    if(VkUUID::eq (targetUUID, localUUID)) {
	display ("\nReached a  Peer in the FinalPath");

	unsigned int index;
	peerDataArray.locatePeer (localUUID,index);

	if (VkUUID::eq (finalUUID, localUUID))
	    display ("\n Reached the final destination");
	else {
	    peerDataArray.getNextFinalPathPeer (localUUID, nextTarget,finalUUID);

	    pTargetPeer = sibling (nextTarget);
	    if (!pTargetPeer) {
		uuid_t nextPeer;
		if (peerDataArray.getNextPeer (localUUID, nextPeer,finalUUID))
		    pTargetPeer = sibling (nextPeer); 
	    }
	}
    }

    else {	// If I am an intermediate peer then delegate the request
	pTargetPeer = sibling (targetUUID);
	if (!pTargetPeer) {
	    unsigned int index;

	    if (peerDataArray.locatePeer (localUUID, index)) {
		uuid_t nextPeer;
		peerDataArray.getNextPeer (localUUID, nextPeer,finalUUID);
		pTargetPeer = sibling (nextPeer);
		nextTarget = targetUUID;
	    }
	}
    }
    if (pTargetPeer) {
	IPeer::Reference pNextHop;
	if (pTargetPeer->getNextHop (pNextHop)) pNextHop->CreateConnection (
	    peerDataArray, nextTarget, finalUUID
	);
    }
#endif
}


/***************************************
 ***************************************
 ***** Proxy Facilitation Routines *****
 ***************************************
 ***************************************/

/****************************************************************************
 * Routine: createProxyFacilitatorFor
 * Operation: 
 *	This method is called when a proxy needs to be facilitated between
 * "this" peer and the target (argument peer) and there is no facilitator 
 * already existing in the facilitation table of this peer. This construction 
 * of the Facilitator object also involves initiating the startup process
 * (exchange of reflections) of the facilitator object.
****************************************************************************/

Vca::VcaProxyFacilitator* Vca::VcaPeer::createProxyFacilitatorFor (VcaPeer *pTargetPeer) {
    VcaProxyFacilitator *pFacilitator = new VcaProxyFacilitator (this, pTargetPeer);
    unsigned int xFacilitator;
    m_iProxyFacilitationTable.Insert (pTargetPeer, xFacilitator);
    m_iProxyFacilitationTable[xFacilitator] = pFacilitator;
    return pFacilitator;
}

/****************************************************************************
 * Overloaded Routine: createProxyFacilitatorFor
 * Operation: 
 *	This method is called when a proxy needs to be facilitated between
 * "this" peer and the target (argument peer) and there is no facilitator 
 * already existing in the facilitation table of this peer. But there is a 
 * facilitator object which serves objects between the target peer and "this"
 * peer (reverse direction). This method takes as arguments the Remote 
 * Reflections (retreived from the other facilitator) as arguments so that 
 * the newly created Facilitator object doesnt have to perform the startup 
 * process.
****************************************************************************/

Vca::VcaProxyFacilitator* Vca::VcaPeer::createProxyFacilitatorFor (
    VcaPeer *pTargetPeer, IPeer *pSourcePeerReflection, IPeer *pTargetPeerReflection
) { 
    VcaProxyFacilitator *pFacilitator = new VcaProxyFacilitator (
	this, pTargetPeer, pSourcePeerReflection, pTargetPeerReflection
    );
    unsigned int xFacilitator;
    m_iProxyFacilitationTable.Insert (pTargetPeer, xFacilitator);
    m_iProxyFacilitationTable[xFacilitator] = pFacilitator;
    return pFacilitator;
}

bool Vca::VcaPeer::getProxyFacilitatorFor (
    VcaPeer *pTargetPeer, VcaProxyFacilitator *&rpFacilitator
) const {
    if (m_iProxyFacilitationTable.Contains (pTargetPeer)) {
	unsigned int xFacilitator;
	m_iProxyFacilitationTable.Locate (pTargetPeer, xFacilitator);
	rpFacilitator =  m_iProxyFacilitationTable[xFacilitator];
	return true;
    }
    return false;
}

/****************************************************************************
 * Routine: facilitateProxy
 * Operation: 
 *	This method is called to facilitate a proxy to a downstream peer
 * If "this" upstream peer already has a ProxyFacilitator for the target 
 * downstream peer in its ProxyFacilitationTable, uses it to facilitate
 * the proxy between "this" peer and the target peer, else creates a new 
 * proxy facilitator for the target peer and then facilitates
****************************************************************************/

void Vca::VcaPeer::facilitateProxy (VcaOIDR *pOIDR, VcaPeer *pTargetPeer) {
    VcaProxyFacilitator *pFacilitator;
    if (!getProxyFacilitatorFor (pTargetPeer, pFacilitator)) {
	pFacilitator = createProxyFacilitatorFor (pTargetPeer);
    }
    pFacilitator->processProxy (pOIDR);
}


/****************************************************************************
 * Routine: deleteProxyFaciliatorFor
 * Operation:
 *   This routine should be called from peer defuncting process to clean up
 * the facilitation objects associated with the defuncted peer.
 * Facilitation objects as such are deleted from the table, only during the
 * the peer defuncting process. Otherwise they live facilitating proxies as 
 * and when they become "passing" proxies without any internal usage
****************************************************************************/
 
bool Vca::VcaPeer::deleteProxyFacilitatorFor (VcaPeer *pTargetPeer) {
    return m_iProxyFacilitationTable.Delete (pTargetPeer);
}

/****************************************************************************
 * Routine: isWaitingToFacilitate
 * Operation:
 *	This routine is used to check whether a faciliation for a proxy is 
 * scheduled and waiting.
****************************************************************************/

bool Vca::VcaPeer::isWaitingToFacilitate (VcaOIDR *pOIDR) {
    PFTIterator iterator (m_iProxyFacilitationTable);
    while (iterator.isNotAtEnd ()) {
        VcaProxyFacilitator  *pFacilitator = iterator.value ();
	if (pFacilitator->isWaitingToFacilitate (pOIDR))
	    return true;
        ++iterator;
    }
    return false;
}

/****************************************************************************
 * Routine: refacilitateProxyWith
 * Operation:
 *	This method is used to optimize the Proxy Facilitation process.
 * A facilitation is done between a source peer and one or more target peers.
 * But if the source peer itself is also doing facilitation then we can 
 * convert the existing facilitation from the current source to the new 
 * source peer. This helps in creating shorter links faster.
****************************************************************************/

void Vca::VcaPeer::refacilitateProxyWith (VcaOIDR *pOIDR, VcaPeer *pNewSourcePeer) {
    PFTIterator iterator (m_iProxyFacilitationTable);
    while (iterator.isNotAtEnd ()) {
        VcaProxyFacilitator  *pFacilitator = iterator.value ();
	if (pFacilitator->isWaitingToFacilitate (pOIDR)) {
	    VcaPeer *pTargetPeer = pFacilitator->targetPeer ();
	    pFacilitator->removePendingProxy (pOIDR);
	    pNewSourcePeer->facilitateProxy (pOIDR, pTargetPeer);
	}
        ++iterator;
    }
}

/****************************************************************************
 * Routine: refacilitateProxyTo
 * Operation:
 *	This method is also used to optimize the Proxy Facilitation process.
 * If a target peer itself is also doing facilitation then we can 
 * convert the existing facilitation from the current target to the new 
 * target peer. 
****************************************************************************/

void Vca::VcaPeer::refacilitateProxyTo (
    VcaOIDR *pOIDR, VcaPeer *pNewTargetPeer, VcaPeer *pOldTargetPeer
) {
    VcaProxyFacilitator *pOldFacilitator;
    if (getProxyFacilitatorFor (pOldTargetPeer, pOldFacilitator)) {
	pOldFacilitator->removePendingProxy (pOIDR);
	facilitateProxy (pOIDR, pNewTargetPeer);
    }
}


/***********************
 ***********************
 *****  Site Info  *****
 ***********************
 ***********************/

Vca::VSiteInfo* Vca::VcaPeer::getSiteInfo_(VSiteInfo::Reference& rpInfo) const {
    rpInfo.setTo (new SiteInfo (characteristics (), m_pRemoteReflectionV1));
    return rpInfo;
}

/************************************
 ************************************
 *****  Test Framework Support  *****
 ************************************
 ************************************/

//  Displays all the routes (peers) to reach "this" peer from the local peer

void Vca::VcaPeer::displayRoutes () const {
    for(unsigned int i=0; i<m_iRouteTable.cardinality(); i++) {
	m_iRouteTable.key(i)->displayInfo ();
	display(" Distance :%u", m_iRouteTable[i].hopCount());
    }
    display("\n Best Hop count: %u\n", m_iBestRouteMetrics.hopCount());
}

void Vca::VcaPeer::displayPendingMessageCounts () const {
    unsigned int cMessages = 0;
    VMessage *pMessage = m_pMessageQueueHead;
    while (pMessage) {
	++cMessages;
	pMessage = pMessage->m_pSuccessor;
    }
    displayInfo ();
    display (" Messages pending = %u", cMessages);
}
