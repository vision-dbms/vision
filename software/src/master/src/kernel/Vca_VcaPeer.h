#ifndef Vca_VcaPeer_Interface
#define Vca_VcaPeer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaSite.h"

#include "Vca_INeedYou.h"

#include "Vca_VcaRouteStatistics.h"
#include "Vca_VTimer.h"
#include "Vca_VTriggerTicket.h"

#include "VkSetOfReferences.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VGoferInterface.h"
#include "Vca_VGoferExogenousInterface.h"

#include "Vca_VMessage.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaProxyFacilitator;

    class VcaPeer : public VcaSite {
	DECLARE_CONCRETE_RTTLITE (VcaPeer, VcaSite);

    //  Aliases
    public:
	typedef VcaOID::Export	Export;
	typedef VcaOIDR::Import	Import;

	typedef VGoferExogenousInterface<IVUnknown> RemoteReflectionGofer_T;

    //  Callback Request Classes
    public:
	class CallbackRequest;
	class CallMeRequest;
	class CallPeerRequest;

    //  Site Info Class
    public:
	class SiteInfo;

    //  Friends
	friend class CallMeRequest;
	friend class SiteInfo;

	friend class ::IVUnknown;
	friend class VcaOID::Export;
	friend class VcaOIDR;
	friend class VcaOIDR::Import;
	friend class VcaProxyFacilitator;
	friend class VcaSite;

    //  Construction
    public:
	VcaPeer (VcaSite* pParent, Characteristics const &rCharacteristics);

    //  Destruction
    private:
	~VcaPeer ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  INeedYou Role
    private:
	VRole<ThisClass,INeedYou> m_pINeedYou;
    protected:
	void getRole (INeedYou::Reference &rpRole) {
	    m_pINeedYou.getRole (rpRole);
	}

    //  INeedYou Methods
    public:
	void PleaseCall (
	    INeedYou*		pRole,
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    uuid_t const&	rTargetID
	);

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

    //  Connectivity
    private:
	void incrementConnectionRequestCount ();
	void decrementConnectionRequestCount () {
	    decrementConnectionRequestCount (false);
	}
	void decrementConnectionRequestCountWithDefunctingOption () {
	    decrementConnectionRequestCount (true);
	}
	void decrementConnectionRequestCount (bool bDefunctingAllowed);

	bool isConnected_() const {
	    return isConnected ();
	}
	bool isUsable_() const {
	    return isUsable ();
	}
	void processCallbackRequest_(
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    ConnectKind		xConnectKind
	);
    private:
	void closeConnections ();

	void establishConnection ();

	void forwardCallbackRequest (CallPeerRequest *pCallbackRequest);

        bool requestNoRouteToPeerCallback (
	    VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended = false
	);

	void scheduleConnectionCleanup ();

        void triggerNoRouteToPeerCallbacks ();
    public:
	bool isConnected () const {
	    return outgoingTransportCount () > 0;
	}
	bool isConnecting () const {
	    return m_cConnectionRequests > 0;
	}
	bool isConnectable () const {
	    return !isntConnectable ();
	}
	bool isDisconnectable () const;

	bool isUsable () const {
	    return isConnected () || isConnecting () || isConnectable ();
	}

	bool isntConnected () const {
	    return !isConnected ();
	}
	bool isntConnecting () const {
	    return !isConnecting ();
	}
	bool isntConnectable () const {
	    return m_bUnconnectable;
	}
	bool isntDisconnectable () const {
	    return !isDisconnectable ();
	}
	bool isntUsable () const {
	    return !isUsable ();
	}

    //  Transport
    private:
	void attach_(VcaTransportInbound *pTransport);
	void attach_(VcaTransportOutbound *pTransport);
	void detach_(VcaTransportInbound *pTransport);
	void detach_(VcaTransportOutbound *pTransport);

	void startNextSerializerOn_(VcaTransportInbound *pTransport);
	void startNextSerializerOn_(VcaTransportOutbound *pTransport);
	bool getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport);

	void deleteEvaluators ();

    public:
	unsigned int incomingTransportCount () const {
	    return m_iIncomingTransportSet.cardinality ();
	}
	unsigned int outgoingTransportCount () const {
	    return m_iOutgoingTransportSet.cardinality ();
	}

    //  Evaluation
    private:
        void evaluateOutgoing_ (VMessage *pMessage);
      
    //  Control
    private:
	virtual /*override*/ void markDefunct_();
	virtual /*override*/ void onShutdown ();

    //  Evaluation
    private:
	void enqueue (VMessage *pMessage);
	void dequeue (VMessage::Reference &pMessage);

    public:
	bool invocationQueueIsEmpty () const {
	    return m_pMessageQueueHead.isNil ();
	}
	bool invocationQueueIsntEmpty () const {
	    return m_pMessageQueueHead.isntNil ();
	}

    //  Object Export
    private:
	void createExportOf (IVUnknown *pObject, bool bWeak);
	void createExportOf (VcaOID *pOID, bool bWeak);
	void deleteExportOf (VMessageHolder<IPeer_Ex2> const& rMessage, VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages);
	bool weakenExportOf (VcaOID *pOID);
	bool weakenRemoteImportOf (VcaOID *pOIDR);
    protected:
	virtual /*override*/ void onExportCountIsZero ();
	virtual /*override*/ void onExportCountWasZero ();

    //  Object Import
    private:
	void createImportOf (VcaOID *pOID, bool bWeak);
	void deleteImportOf (VcaOID *pOID);
	bool weakenImportOf (VcaOID *pOID);
	bool weakenRemoteExportOf (VcaOIDR *pOIDR);

	bool receive_(
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rSSID,
	    VTypeInfo*		pType,
	    VcaSite*		pSource,
	    bool		bWeak
	);
	bool receive_(
	    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
	);

    protected:
	virtual /*override*/ void onImportCountIsZero ();
	virtual /*override*/ void onImportCountWasZero ();

    public:
	bool receive (
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rSSID,
	    VTypeInfo*		pType,
	    VcaSite*		pSource,
	    bool		bWeak
	);
	bool receive (
	    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
	) {
	    return getObject (rpOID, rSSID);
	}

    //  Remote Export
    private:
        void deleteRemoteExport (
	    VcaOIDR const *pOIDR, U32 cExports, U32 cWeakImports, U32 cMessages
	) const;

    //  Remote Import
    private:
	void createRemoteImportOf (VcaOID  *pOID) const;
        void deleteRemoteImportOf (VcaOIDR *pOIDR) const;

    //  Remote Fake Export-Import
    private:
	void fakeRemoteExportTo   (VcaOIDR const *pOIDR, VcaPeer const *pTargetPeer) const;
	void fakeRemoteImportFrom (VcaOIDR const *pOIDR, VcaPeer const *pSourcePeer) const;
	
	void setRemoteRemoteReflectionFor (VcaPeer const *pPeer, IPeer *pIPeer) const;

    //  Remote Interface Retrieval
    private:
	void getRemoteLocalInterfaceFor (VcaPeer *pPeer, IVReceiver<IPeer*>* pReceiver) const;

    //  Remote Reflection
    private:
	void getReflection_(IPeer::Reference &rpReflection) {
	    getReflection (rpReflection);
	}
	void setReflection (IPeer *pReflection);

	RemoteReflectionGofer_T *reflectionGofer () const {
	    return m_pRemoteReflectionGofer;
	}
    public:
	void getReflection (IPeer::Reference &rpReflection) {
	    rpReflection.setTo (m_pRemoteReflection);
	}

    //  Routing
    public:	
	typedef VkMapOf<
	    VcaPeer::Reference, VcaPeer*, VcaPeer const*, VcaRouteStatistics
	> RouteTable;

	void addRoute   (VcaPeer *pPeer, VcaRouteStatistics const &rMetrics);
	void addRouteTo (VcaSite *pSite, VcaRouteStatistics const &rMetrics) {
	    pSite->addRoute (this, rMetrics);
	}

	bool deleteRoute (VcaPeer *pPeer);

	void getBestRouteMetrics (VcaRouteStatistics &rMetrics) const {
	    rMetrics = m_iBestRouteMetrics;
	}

    //  Self
    private:
	bool isSelf_() const {
	    return isSelf ();
	}
    public:
	bool isSelf () const {
	    return false;
	}

    //  Tracing and Display
    public:
	void displayPendingMessageCounts () const;
	void displayRoutes () const;    // Displays routes to reach this peer

    //  Site Info
    private:
	VSiteInfo* getSiteInfo_(VSiteInfo::Reference& rpInfo) const;

/****************************************************************************************
 * /////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * ////////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * ///////////////////////////////////////    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\    ///////////////////////////////////////// *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  ////////////////////////////////////////// *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////////////////// *
 ****************************************************************************************/

    //  Connection Data Gathering
    private:
	void GatherConnectionData (
	    IConnectionRequest*,
	    VPeerDataArray const&, VPeerDataArray const&, uuid_t const&, bool bForward
	);
	void CreateConnection (VPeerDataArray const&, uuid_t const &, uuid_t const &);

    public:
	bool containsRoutingPeer (VcaPeer *pRoutingPeer) const;
	bool containsRoutingPeer (VcaSite *pRoutingPeer) const;

	bool getHopCountFor (VcaPeer *pRoutingPeer, unsigned int &rHopCount) const;

    private:
	bool getNextHop (IPeer::Reference &rpNextHop);

	void setBestRoutingStatistics ();

    //  Transport Maintenance
    private:
	void addFreeOutboundTransport (VcaTransportOutbound *pFreeTransport);
	void getFreeOutboundTransport (VReference<VcaTransportOutbound> &rpFreeTransport);

	bool freeTransportListIsntEmpty () const {
	    return m_pTransportHead.isntNil ();
	}

	unsigned int freeTransportCount () const;
   
    //  Proxy Facilitation Methods
    public:
	typedef VkMapOf<
	    VcaPeer::Reference, VcaPeer*, VcaPeer const*, VReference<VcaProxyFacilitator> 
	> ProxyFacilitationTable;
	typedef ProxyFacilitationTable::Iterator PFTIterator;

    public:
	void facilitateProxy (VcaOIDR *pOIDR, VcaPeer *pImporter);

    private:    
	VcaProxyFacilitator *createProxyFacilitatorFor (VcaPeer *pImporter);
	VcaProxyFacilitator *createProxyFacilitatorFor (
	    VcaPeer *pImporter, IPeer *pSourcePeerReflection, IPeer *pImporterReflection
	);
	bool deleteProxyFacilitatorFor (VcaPeer *pImporter);
	
	bool  getProxyFacilitatorFor (
	    VcaPeer *pImporter, VcaProxyFacilitator *&rpFacilitator
	) const;

	bool isWaitingToFacilitate (VcaOIDR *pOIDR);
	void refacilitateProxyWith (VcaOIDR *pOIDR, VcaPeer *pNewSourcePeer);
	void refacilitateProxyTo (
	    VcaOIDR *pOIDR, VcaPeer *pNewImporterPeer, VcaPeer *pOldImporterPeer
	);

    //  Transport Policy (for choosing freely available transports)
    public:
	enum TransportPolicy { LIFO, FIFO } ;
    private:
	static TransportPolicy g_xTransportPolicy; 
    public:
	static Vca_API void setGlobalTransportPolicyTo (TransportPolicy xPolicy);
	static Vca_API TransportPolicy transportPolicy ();

/****************************************************************************************
 * /////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * ////////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * ///////////////////////////////////////    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\    ///////////////////////////////////////// *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  ////////////////////////////////////////// *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////////////////// *
 ****************************************************************************************/

    //  State
    private:
	//  Gofers
	RemoteReflectionGofer_T::Reference	const m_pRemoteReflectionGofer; // ... must come before m_pINeedYouGofer
	VGoferInterface<INeedYou>::Reference	const m_pINeedYouGofer;		// ... must come after m_pRemoteReflectionGofer

	//  Transports
	VkSetOfReferences<VcaTransportInbound>		//  transport sets
				m_iIncomingTransportSet;
	VkSetOfReferences<VcaTransportOutbound>
				m_iOutgoingTransportSet;

	VReference<VcaTransportOutbound>		// outbound transport free list
				m_pTransportHead,
				m_pTransportTail;

	//  Export/Import Lists
	Export::Pointer		m_pExportListHead;	// OID::Export's to this peer
	Import::Pointer		m_pImportListHead;	// OIDR::Import's from this peer

	//  Message Queue
	VMessage::Reference	m_pMessageQueueHead;
	VMessage::Reference	m_pMessageQueueTail;

	//  Remote Reflection
	IPeer::Reference	m_pRemoteReflection;
	IPeer1::Reference	m_pRemoteReflectionV1;
	IPeer2::Reference	m_pRemoteReflectionV2;

	//  Routing and Connectivity
	RouteTable		m_iRouteTable;
	VcaPeer::Reference	m_pBestRoute;      
	VcaRouteStatistics	m_iBestRouteMetrics;

	VTriggerTicket::List	m_pNoRouteToPeerCallbacks;

	VTimer::Reference	m_pCleanupTimer;
	V::VTime		m_iCleanupTime;

	//  Facilitation
	ProxyFacilitationTable	m_iProxyFacilitationTable;    

	unsigned int		m_cConnectionRequests;
	bool			m_bUnconnectable;
	bool			m_bOnDoneSent;
    };
}


#endif
