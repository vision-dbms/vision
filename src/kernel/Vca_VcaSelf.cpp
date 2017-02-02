/*****  Vca::VcaSelf Implementation  *****/

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

#include "Vca_VcaSelf.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IAckReceiver.h"
#include "Vca_IPipeFactory.h"

#include "Vca_VcaGofer.h"
#include "Vca_VcaListener.h"
#include "Vca_VcaOIDL.h"
#include "Vca_VcaOIDR.h"
#include "Vca_VcaPeer.h"
#include "Vca_VcaRouteStatistics.h"
#include "Vca_VcaTransport.h"

#include "Vca_VPassiveCallback.h"

#include "VPeerData.h"

#include "VkSocketAddress.h"


/************************************
 ************************************
 *****                          *****
 *****  Vca::VcaSelf::SiteInfo  *****
 *****                          *****
 ************************************
 ************************************/

namespace Vca {
    class VcaSelf::SiteInfo : public VSiteInfo {
	DECLARE_CONCRETE_RTTLITE (SiteInfo, VSiteInfo);

    //  Construction
    public:
	SiteInfo (VcaPeerCharacteristics const& rCharacteristics);

    //  Destruction
    private:
	~SiteInfo () {
	}

    //  Implementation
    private:
	void onAuthoritativeSourceNeeded_();
	void onPIDNeeded_();
	void onUIDNeeded_();
	void onGIDNeeded_();
	void onHostNameNeeded_();
	void onUserNameNeeded_();
	void onGroupNameNeeded_();
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSelf::SiteInfo::SiteInfo (VcaPeerCharacteristics const& rCharacteristics) : BaseClass (rCharacteristics) {
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VcaSelf::SiteInfo::onAuthoritativeSourceNeeded_() {
    IProcessInfo::Reference pMySelf;
    getRole (pMySelf);
    AuthoritativeSource::setTo (pMySelf);
}

void Vca::VcaSelf::SiteInfo::onPIDNeeded_() {
    PID::setTo (getpid ());
}

void Vca::VcaSelf::SiteInfo::onUIDNeeded_() {
    UID::setTo (getuid ());
}

void Vca::VcaSelf::SiteInfo::onGIDNeeded_() {
    GID::setTo (getgid ());
}

void Vca::VcaSelf::SiteInfo::onHostNameNeeded_() {
    char iBuffer[128];
    HostName::setTo (VkSocketAddress::hostname (iBuffer,sizeof(iBuffer)));
}

void Vca::VcaSelf::SiteInfo::onUserNameNeeded_() {
    char iBuffer[128];
    UserName::setTo (Vk_username (iBuffer, sizeof (iBuffer)));
}

#ifdef _WIN32
void Vca::VcaSelf::SiteInfo::onGroupNameNeeded_() {
    GroupName::setTo ("NoGroup");
}
#else
#include <grp.h>

void Vca::VcaSelf::SiteInfo::onGroupNameNeeded_() {
    typedef struct group group_t;
    char iBuffer[128];
    group_t iGroup, *pGroup = 0;
    GroupName::setTo (getgrgid_r (getgid (), &iGroup, iBuffer, sizeof(iBuffer), &pGroup) == 0 && pGroup ? pGroup->gr_name : "UNKNOWN");
}
#endif


/**************************
 **************************
 *****                *****
 *****  Vca::VcaSelf  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSelf::VcaSelf () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaSelf::~VcaSelf () {
}


/**************************
 **************************
 *****  Connectivity  *****
 **************************
 **************************/

namespace {
    using namespace Vca;

    template <class Interface_T, class Namespace_T> class DontAsk : public VGoferInterface<Interface_T> {
	typedef DontAsk<Interface_T,Namespace_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VGoferInterface<Interface_T>);

    //  Aliases
    public:
	typedef Interface_T interface_t;
	typedef typename Namespace::VTraits<Namespace_T>::interface_t namespace_t;
	typedef typename Namespace::VTraits<Namespace_T>::name_t name_t;

    //  Construction
    public:
	template <typename namme_t, typename namespase_t> DontAsk (
	    namme_t iObjectName, namespase_t pNamespace
	) : m_iObjectName (iObjectName), m_pNamespace (pNamespace) {
	}

	template <typename namme_t> DontAsk (
	    namme_t iObjectName
	) : m_iObjectName (iObjectName), m_pNamespace (static_cast<namespace_t*>(0)) {
	}

    //  Destruction
    private:
	~DontAsk () {
	}

    //  Callbacks and Triggers
    private:
	void onNeed () {
	    m_iObjectName.materializeFor (this);
	    m_pNamespace.materializeFor (this);
	    BaseClass::onNeed ();
	}

	void onData () {
	    VQueryInterface<Interface_T> iQuery (0);
	    VcaOffer::Offering iOffering;
	    Gofer::Supply (iQuery,iOffering,m_iObjectName,m_pNamespace);
	    BaseClass::setTo (0);
	}

    //  State
    private:
	VInstanceOf<name_t> m_iObjectName;
	VInstanceOfInterface<Namespace_T> m_pNamespace;
    };
}

void Vca::VcaSelf::processCallbackRequest_(
    IAckReceiver*	pAckReceiver,
    ICallbackReceiver*	pCallbackReceiver,
    IPassiveCall*	pCallbackRequest,
    VString const&	rCallbackAddress,
    uuid_t const&	rCallbackID,
    ConnectKind         xConnectKind
) {
    typedef VGoferInterface<IPassiveClient> client_gofer_t;

//  ... respond to the request,
    VPassiveCallback::Reference const pCallback (
	new VPassiveCallback (
	    0, pCallbackRequest, rCallbackID,
	    represents (rCallbackID) ? static_cast<client_gofer_t*> (
		new DontAsk<IPassiveClient,IPipeFactory> (rCallbackAddress)
	    ) : static_cast<client_gofer_t*> (
		new Gofer::Named<IPassiveClient,IPipeFactory> (rCallbackAddress)
	    ), pAckReceiver
	)
    );

// ... if requested, return the IPassiveCallback interface of the callback:
    if (pCallbackReceiver) {
	IPassiveCallback::Reference pCallbackInterface;
	pCallback->getRole (pCallbackInterface);
	pCallbackReceiver->OnData (pCallbackInterface);
    }
}

void Vca::VcaSelf::attach_(VcaTransportInbound *pTransport) {
}

void Vca::VcaSelf::attach_(VcaTransportOutbound *pTransport) {
}

void Vca::VcaSelf::detach_(VcaTransportInbound *pTransport) {
}

void Vca::VcaSelf::detach_(VcaTransportOutbound *pTransport) {
}

void Vca::VcaSelf::startNextSerializerOn_(VcaTransportInbound *pTransport) {
    pTransport->Close ();
}

void Vca::VcaSelf::startNextSerializerOn_(VcaTransportOutbound *pTransport) {
    pTransport->Close ();
}

bool Vca::VcaSelf::getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport) {
    return false;
}


/**********************
 **********************
 *****  Lifetime  *****
 **********************
 **********************/

void Vca::VcaSelf::markDefunct_() {
    traceInfo ("markDefunct (VcaSelf)");

    VcaListener::CloseListeners ();
    VcaOIDR::onShutdown ();

    BaseClass::markDefunct_();
}

bool Vca::VcaSelf::keepalivesIncludeExports () const {
    return m_bKeepalivesIncludeExports;
}

void Vca::VcaSelf::setKeepalivesIncludeExports () {
//  When switched from OFF to ON and exports exist, ...
    if (m_bKeepalivesIncludeExports.setIfClear () && exportCount () > 0) {
	incrementKeepaliveCount ();     //  ... start counting them now.
    }
}

void Vca::VcaSelf::clearKeepalivesIncludeExports () {
//  When switched from ON to OFF and exports exist, ...
    if (m_bKeepalivesIncludeExports.clearIfSet () && exportCount () > 0) {
	decrementKeepaliveCount ();	//  ... stop counting them now.
    }
}

bool Vca::VcaSelf::keepalivesManageShutdown () const {
    return m_bKeepalivesManageShutdown;
}

void Vca::VcaSelf::setKeepalivesManageShutdown () {
    m_bKeepalivesManageShutdown.setIfClear ();
}

void Vca::VcaSelf::clearKeepalivesManageShutdown () {
    m_bKeepalivesManageShutdown.clearIfSet ();
}

count_t Vca::VcaSelf::keepaliveCount () const {
    return m_cKeepalives;
}

void Vca::VcaSelf::incrementKeepaliveCount () {
    if (0 == m_cKeepalives++)
	onFirstKeepalive ();
}

void Vca::VcaSelf::decrementKeepaliveCount () {
    if (m_cKeepalives.decrementIsZero ())
	onFinalKeepalive ();
}

void Vca::VcaSelf::onFirstKeepalive () {
}

void Vca::VcaSelf::onFinalKeepalive () {
    if (objectsTrace ())
	traceInfo (string ("%s %p: On Final Keepalive.", rttName ().content (), this));
    if (keepalivesManageShutdown ())
	Shutdown ();
}


/***************************
 ***************************
 *****  Object Access  *****
 ***************************
 ***************************/

bool Vca::VcaSelf::getObject (VcaOIDL::Reference& rpOIDL, IVUnknown *pObject) {
    VcaSSID iSSID;
    pObject->getSSID (iSSID);

    unsigned int xObject;
    if (m_iObjectTable.Insert (iSSID, xObject))
	m_iObjectTable[xObject] = new VcaOIDL (iSSID, pObject);
    rpOIDL.setTo (static_cast<VcaOIDL*>(m_iObjectTable[xObject]));

    return rpOIDL.isntNil ();
}

/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void Vca::VcaSelf::createExportOf (IVUnknown *pObject, bool bWeak) {
    pObject->createExportTo (this, bWeak);
}

void Vca::VcaSelf::createExportOf (VcaOID *pObject, bool bWeak) {
    pObject->createExportTo (this, bWeak);
}

void Vca::VcaSelf::deleteExportOf (
    VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages
) {
}

bool Vca::VcaSelf::weakenExportOf (VcaOID *pOID) {
    return false;
}

void Vca::VcaSelf::onExportCountIsZero () {
    if (keepalivesIncludeExports ())
	decrementKeepaliveCount ();
}

void Vca::VcaSelf::onExportCountWasZero () {
    if (keepalivesIncludeExports ())
	incrementKeepaliveCount ();
}

/***************************
 ***************************
 *****  Object Import  *****
 ***************************
 ***************************/

void Vca::VcaSelf::createImportOf (VcaOID *pOID, bool bWeak) {
}

void Vca::VcaSelf::deleteImportOf (VcaOID *pOID) {
}

bool Vca::VcaSelf::weakenImportOf (VcaOID *pOID) {
    return false;
}

void Vca::VcaSelf::onImportCountIsZero () {
}

void Vca::VcaSelf::onImportCountWasZero () {
}

bool Vca::VcaSelf::receive (
    VcaOID::Reference& rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
) {
    if (getObject (rpOID, rSSID)) {
	pSource->createRemoteImportOf (rpOID);
	if (this == pSource)
	    deleteLoopbackOf (rpOID, bWeak);
	return true;
    }
    return false;
}

/*****************************
 *****************************
 *****  Object Loopback  *****
 *****************************
 *****************************/

void Vca::VcaSelf::createLoopbackOf (VcaOID* pOID, bool bWeak) {
    unsigned int xLoopback;
    if (m_iLoopbackSet.Insert (pOID, xLoopback))
	m_iLoopbackSet[xLoopback] = 1;
    else
	m_iLoopbackSet[xLoopback]++;
}

void Vca::VcaSelf::deleteLoopbackOf (VcaOID* pOID, bool bWeak) {
    unsigned int xLoopback;
    if (m_iLoopbackSet.Locate (pOID, xLoopback) && 0 == --m_iLoopbackSet[xLoopback])
	m_iLoopbackSet.Delete (pOID);
}


/*********************
 *********************
 *****  Routing  *****
 *********************
 *********************/

void Vca::VcaSelf::addRoute (VcaPeer *pPeer, VcaRouteStatistics const &rMetrics) {
}

void Vca::VcaSelf::addRouteTo (VcaSite *pSite, VcaRouteStatistics const &rMetrics) {
}

void Vca::VcaSelf::getBestRouteMetrics (VcaRouteStatistics &rMetrics) const {
    static VcaRouteStatistics const iReallyGoodRoutingMetrics (0);
    rMetrics = iReallyGoodRoutingMetrics;
}

/*******************
 *******************
 *****  Sites  *****
 *******************
 *******************/

Vca::VSiteInfo* Vca::VcaSelf::getSiteInfo_(VSiteInfo::Reference& rpInfo) const {
    rpInfo.setTo (new SiteInfo (characteristics ()));
    return rpInfo;
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaSelf::displayPendingMessageCounts () const {
    display ("\nNever had them, never will.");
}

void Vca::VcaSelf::displayRoutes () const {
    display ("\nYou are there.");
}
