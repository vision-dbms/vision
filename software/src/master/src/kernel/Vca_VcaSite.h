#ifndef Vca_VcaSite_Interface
#define Vca_VcaSite_Interface 

/*---------------------------------------------------------------------------*
 *  VcaSite objects model processes that make their objects available to Vca.
 *  VcaSite is an abstract class with two concrete subclasses - VcaSelf and
 *  VcaPeer.  In any single process, there is exactly one instance of VcaSelf,
 *  the class that models this process, and any number of instances of
 *  instances of VcaPeer, the class that models other processes.  Primarily,
 *  VcaSite provides the machinery for managing the collection of objects
 *  from the site known to Vca.  The primary data structure for that data
 *  the ObjectTable, with the subclasses providing implementation details
 *  (for example, managing references to local objects in VcaSelf, managing
 *  references to remote objects in VcaPeer).
 *---------------------------------------------------------------------------*/
 
/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPeer_Ex2.h"

#include "Vca_VcaPeerCharacteristics.h"

#include "V_VTwiddler.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_INeedYou.h"

#include "Vca_VSiteInfo.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaConnection;
    class VcaOffer;
    class VcaRouteStatistics;
    class VcaSelf;
    class VcaSerializerForInterface;
    class VcaSerializerForMessage;
    class VcaSerializerForPlumbing;
    class VcaTransportInbound;
    class VcaTransportOutbound;

    class VPeerData;
    class VPeerDataArray;

    class ABSTRACT Vca_API VcaSite : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VcaSite, VRolePlayer);

    //  Aliases
    public:
	typedef VcaPeerCharacteristics	Characteristics;
	typedef VcaOIDL::Evaluator	Evaluator;
	typedef VReference<VcaSelf>	SelfReference;

    //  Friends
	friend class VcaConnection;
	friend class VcaOffer;
	friend class VcaOIDL;
	friend class VcaOIDL::Evaluator;
	friend class VcaOIDR;
	friend class VcaPeer;
	friend class VcaSelf;
	friend class VcaSerializerForInterface;
	friend class VcaSerializerForMessage;
	friend class VcaSerializerForPlumbing;
	friend class VcaTransportInbound;
	friend class VcaTransportOutbound;

    //  Construction
    protected:
	VcaSite (VcaSite* pSelf, Characteristics const &rCharacteristics);
	VcaSite ();

    //  Destruction
    protected:
	~VcaSite ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IPeer2 Role
    private:
	VRole<ThisClass,IPeer2> m_pIPeer;
    public:
	void getRole (IPeer2::Reference &rpRole) {
	    m_pIPeer.getRole (rpRole);
	}

    //  IPeer2 Methods
    public:
	void ReleaseExportEx (
	    IPeer2 *pRole, uuid_t const &rObjectSite, VcaSSID const &rObjectSSID, U32 cExports, U32 cWeakExports, U32 cMessages
	);

    //  IPeer1 Role
    protected:
	void getRole (IPeer1::Reference &rpRole);

    //  IPeer1 Methods
    public:
	void WeakenExport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID);
	void WeakenImport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID);
	void GetProcessInfo (IPeer1* pRole, uuid_t const& rPeerUUID, IProcessInfoSink* pInfoSink);

    //  IPeer Role
    protected:
	void getRole (IPeer::Reference &rpRole);

	bool supplierOf (IPeer const *pRole) const {
	    return m_pIPeer.supplies (pRole);
	}

    //  IPeer Methods
    public/*private*/:
	void SinkInterface (IPeer *pRole, IVUnknown *pObject);

	void ReleaseExport (
	    IPeer *pRole, uuid_t const &rPeerUUID, VcaSSID const &rObjectSSID,
	    U32 cExports, U32 cMessages
	);
	void ReleaseImport (IPeer *pRole, uuid_t const &rUUID, VcaSSID const &rSSID);

	void GatherConnectionData (
	    IPeer*			pRole,
	    IConnectionRequest*		pRequest,
	    VPeerDataArray const&	rForwardData,
	    VPeerDataArray const&	rReverseData,
	    uuid_t const&		rImporterUUID,
	    bool			bForward
	);
	void CreateConnection (
	    IPeer*			pRole,
	    VPeerDataArray const&	rPeerDataArray,
	    uuid_t const&		rImporterUUID,
	    uuid_t const&		rFinalUUID
	);

	void GetRootInterface (
	    IPeer *pRole, VTypeInfo *pInterfaceType, IVReceiver<IVUnknown*>* pReceiver
	);

	void GetLocalInterfaceFor (
	    IPeer *pRole, VcaSite *pSite, IVReceiver<IPeer*>* pReceiver
	);
	void SetRemoteReflectionFor (
	    IPeer *pRole, uuid_t const &rPeerUUID, IPeer *pReflection
	);

	void FakeExportTo (
	    IPeer *pRole, uuid_t const &rExportSite,
	    uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
	);
	void FakeImportFrom (
	    IPeer *pRole, uuid_t const& rImportSite, 
	    uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
	);

	void MakeConnection (
	    IPeer *pRole, IVReceiver<IConnection*> *pClient, VString const &rDestination
	);

	void OnDone (IPeer *pRole);

    private:
	void GatherConnectionData (
	    IConnectionRequest*		pRequest,
	    VPeerDataArray const&	rForwardData,
	    VPeerDataArray const&	rReverseData,
	    uuid_t const&		rImporterUUID,
	    bool			bForward
	);
	void CreateConnection (
	    VPeerDataArray const&	rPeerDataArray,
	    uuid_t const&		rImporterUUID,
	    uuid_t const&		rFinalUUID
	);

    //  Characteristics
    public:
	bool architectureIsKnown () const {
	    return m_iCharacteristics.architectureIsKnown ();
	}
	bool architectureIsLocal () const {
	    return m_iCharacteristics.architectureIsLocal ();
	}
	bool architectureIsAntiLocal () const {
	    return m_iCharacteristics.architectureIsAntiLocal ();
	}
	Characteristics const &characteristics () const {
	    return m_iCharacteristics;
	}

	bool represents (uuid_t const &rUUID) const {
	    return m_iCharacteristics.represents (rUUID);
	}

	uuid_t const &uuid () const {
	    return m_iCharacteristics.uuid ();
	}

	bool supportsWeak () const {
	    return m_iCharacteristics.supportsWeak ();
	}

    //  Localization
    public:
	void localize (void *pElements, size_t sElement, size_t cElements) const {
	    m_iCharacteristics.localize (pElements, sElement, cElements);
	}

	void localize (bool &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (S08 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (U08 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (S16 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (U16 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (S32 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (U32 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (S64 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (U64 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (F32 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (F64 &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (VcaSSID &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}
	void localize (uuid_t &rValue, size_t cElements = 1) const {
	    m_iCharacteristics.localize (rValue, cElements);
	}

	void localize (Characteristics &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<bool> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<S08> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}
	void localize (VkDynamicArrayOf<U08> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<S16> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}
	void localize (VkDynamicArrayOf<U16> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<S32> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}
	void localize (VkDynamicArrayOf<U32> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<S64> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}
	void localize (VkDynamicArrayOf<U64> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

	void localize (VkDynamicArrayOf<F32> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}
	void localize (VkDynamicArrayOf<F64> &rValue) const {
	    m_iCharacteristics.localize (rValue);
	}

    //  Connectivity
    public:
	enum ConnectKind {
	    ConnectKind_RouteTable,
	    ConnectKind_RouteTablePlus,
	    ConnectKind_ConnectedOthers,
	    ConnectKind_ConnectedSiblings,
	    ConnectKind_Siblings,
	    ConnectKind_None
	};
	static char const *DescriptionFor (ConnectKind xConnectKind);

    private:
	virtual bool isConnected_() const = 0;
	virtual bool isUsable_() const = 0;

	virtual void processCallbackRequest_(
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    ConnectKind         xConnectKind
	) = 0;

    protected:
	void processCallbackRequest (
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    ConnectKind         xConnectKind = ConnectKind_RouteTable
	);

    public:
	bool isConnected () const {
	    return isConnected_();
	}
	bool isntConnected () const {
	    return !isConnected ();
	}

	bool isUsable () const {
	    return isUsable_();
	}
	bool isntUsable () const {
	    return !isUsable ();
	}
    //  Transport
    private:
	virtual void attach_(VcaTransportInbound *pTransport) = 0;
	virtual void attach_(VcaTransportOutbound *pTransport) = 0;
	virtual void detach_(VcaTransportInbound *pTransport) = 0;
	virtual void detach_(VcaTransportOutbound *pTransport) = 0;
	virtual void startNextSerializerOn_(VcaTransportInbound *pTransport) = 0;
	virtual void startNextSerializerOn_(VcaTransportOutbound *pTransport) = 0;
	virtual bool getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport)= 0;

    private:
	void attach (VcaTransportInbound *pTransport) {
	    attach_(pTransport);
	}
	void attach (VcaTransportOutbound *pTransport) {
	    attach_(pTransport);
	}
	void detach (VcaTransportInbound *pTransport) {
	    detach_(pTransport);
	}
	void detach (VcaTransportOutbound *pTransport) {
	    detach_(pTransport);
	}

	bool getSpecificFreeOutboundTransport (VcaTransportOutbound *pTransport) {
	    return getSpecificFreeOutboundTransport_ (pTransport);
	}

	void startNextSerializerOn (VcaTransportInbound *pTransport) {
	    startNextSerializerOn_(pTransport);
	}
	void startNextSerializerOn (VcaTransportOutbound *pTransport) {
	    startNextSerializerOn_(pTransport);
	}

    //  Evaluation
    private:
	virtual void evaluateOutgoing_(VMessage *pMessage) = 0;

    private:
	void evaluateOutgoing (VMessage *pMessage) {
	    evaluateOutgoing_ (pMessage);
	}

    //  Control
    protected:
	void markDefunct ();

	virtual void markDefunct_();
	virtual void onShutdown ();

    public:
	bool isDefunct () const {
	    return m_bDefunct;
	}
	bool isntDefunct () const {
	    return !m_bDefunct;
	}

    //  Object Access/Creation
    protected:
	/*-------------------------------------------------------------------------*
	 *  For every site known to this process, Vca maintains an ObjectTable
	 *  enumerating the objects from that site known to Vca in this process.
	 *  There are two types of site objects in a process - a single object
	 *  of class VcaSelf representing the local site and zero or more objects
	 *  of class VcaPeer representing remote sites.  The object table associated
	 *  with the local site records all objects implemented by this process and
	 *  made available through Vca to other sites.  The object table associated
	 *  with each remote site identifies the objects implemented by that site
	 *  and made available through Vca for use by this site (process).
	 *-------------------------------------------------------------------------*/
	typedef VkMapOf<VcaSSID, VcaSSID const&, VcaSSID const&, VcaOID*> ObjectTable;
    public:
	bool getObject (		// returns true if found
	    VcaOID::Reference &rpOID, VcaSSID const &rObjectSSID
	) const;
	bool getObject (		// returns true if found
	    VcaOID::Reference &rpOID, VcaSSID const &rObjectSSID, uuid_t const &rObjectSite
	);

    //  Object Detach
    private:
	void detach (VcaOID *pOID);

    //  Object Export
    private:
	virtual void createExportOf (IVUnknown *pObject, bool bWeak) = 0;
	virtual void createExportOf (VcaOID *pOID, bool bWeak) = 0;
	virtual void deleteExportOf (
	    VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages
	) = 0;
	virtual bool weakenExportOf (VcaOID *pOID) = 0;

    protected:
	void incrementExportCount (bool bWeak);
	void decrementExportCount ();

	void incrementWeakExportCount () {
	    m_cWeakExports++;
	}
	void decrementWeakExportCount () {
	    m_cWeakExports--;
	}

	virtual void onExportCountIsZero ();
	virtual void onExportCountWasZero ();

    public:
	count_t exportCount () const {
	    return m_cExports;
	}
	count_t weakExportCount () const {
	    return m_cWeakExports;
	}

    //  Object Import
    private:
	virtual void createImportOf (VcaOID *pOID, bool bWeak) = 0;
	virtual void deleteImportOf (VcaOID *pOID) = 0;
	virtual bool weakenImportOf (VcaOID *pOID) = 0;

	virtual bool receive_(
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rSSID,
	    VTypeInfo*		pType,
	    VcaSite*		pSource,
	    bool		bWeak
	) = 0;
	virtual bool receive_(
	    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
	) = 0;

    protected:
	void incrementImportCount (bool bWeak);
	void decrementImportCount ();

	void incrementWeakImportCount () {
	    m_cWeakImports++;
	}
	void decrementWeakImportCount () {
	    m_cWeakImports--;
	}

	virtual void onImportCountIsZero ();
	virtual void onImportCountWasZero ();

    public:
	count_t importCount () const {
	    return m_cImports;
	}
	count_t weakImportCount () const {
	    return m_cWeakImports;
	}

	bool receive (
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rSSID,
	    VTypeInfo*		pType,
	    VcaSite*		pSource,
	    bool		bWeak
	) {
	    return receive_(rpOID, rSSID, pType, pSource, bWeak);
	}
	bool receive (
	    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VTypeInfo *pType, bool bWeak
	) {
	    return receive_(rpOID, rSSID, pType, this, bWeak);
	}
	bool receive (
	    VcaOID::Reference &rpOID, VcaSSID const &rSSID, VcaSite *pSource, bool bWeak
	) {
	    return receive_(rpOID, rSSID, pSource, bWeak);
	}

    //  Remote Import
    private:
	virtual void createRemoteImportOf (VcaOID  *pOID ) const = 0;
        virtual void deleteRemoteImportOf (VcaOIDR *pOIDR) const = 0;

    //  Remote Reflection
    private:
	virtual void getReflection_(IPeer::Reference &rpReflection) = 0;
	virtual void setReflection (IPeer *pReflection) = 0;
    public:
	void getReflection (IPeer::Reference &rpReflection) {
	    getReflection_(rpReflection);
	}

    //  Routing
    public:
	virtual void addRouteTo (VcaSite *pSite, VcaRouteStatistics const &rStatistics) = 0;
	virtual void addRoute (VcaPeer *pPeer, VcaRouteStatistics const &rStatistics) = 0;

	virtual void getBestRouteMetrics (VcaRouteStatistics &rMetrics) const = 0;

	void getLocalPeerData (VPeerData &rPeerData);

    public:
	virtual void setRoutingInformation (
	    VcaSite *pRoutingSite, VcaRouteStatistics const &rStatistics
	);

    //  Self
    private:
	virtual bool isSelf_() const = 0;
    public:
	bool isSelf () const {
	    return isSelf_();
	}
	bool isntSelf () const {
	    return !isSelf();
	}

    //  Sites
    private:
	typedef VkMapOf<VkUUIDHolder, uuid_t const&, uuid_t const&, Reference> SiteSet;
    public:
	SiteSet const &children () const;
	VcaSite *child (Characteristics const &rCharacteristics);
	VcaSite *child (uuid_t const &rUUID);
	VcaSite *child (char const *pUUID);

	SiteSet const &siblings () const;
	VcaSite *sibling (Characteristics const &rCharacteristics);
	VcaSite *sibling (uuid_t const &rUUID);
	VcaSite *sibling (char const *pUUID);
    private:
	bool unstance (uuid_t const& rUUID);
	bool unstance ();

    //  Site Info
    private:
	virtual VSiteInfo* getSiteInfo_(VSiteInfo::Reference& rpInfo) const = 0;
    public:
	VSiteInfo* info () const {
	    return m_pSiteInfo ? m_pSiteInfo.referent () : getSiteInfo_(m_pSiteInfo);
	}
	void supplyProcessInfo (IProcessInfoSink* pInfoSink) const {
	    info ()->supplyProcessInfo (pInfoSink);
	}

    //  Tracing and Display
    public:
	void displayUUID () const;
        void getUUID (VString &rResult) const;

	using BaseClass::displayInfo;
	virtual void displayInfo () const;
        virtual void getInfo (VString &rResult, const VString &rPrefix) const;

	void displayObjectTable () const;
        void getObjectTable (VString &rResult, const VString &rPrefix) const;

	virtual void displayPendingMessageCounts () const;
	virtual void displayRoutes () const;

	virtual void displayInfoForAll ();
        virtual void getInfoForAll (VString &rResult, const VString &rPrefix);
	virtual void displayObjectTableForAll ();
	virtual void getObjectTableForAll (VString &rResult, const VString &rPrefix);
	virtual void displayPendingMessageCountsForAll ();

    //  State
    protected:
	Reference	const	m_pParent;
	Characteristics const	m_iCharacteristics;
	SiteSet			m_iSiteSet;
	ObjectTable		m_iObjectTable;

	//  Message Evaluators
	Evaluator::Pointer	m_pEvaluators;

    private:
	//  Site Info
	VSiteInfo::Reference mutable m_pSiteInfo;

	//  Export/Import Counters
	counter_t		m_cExports;
	counter_t		m_cImports;
	counter_t		m_cWeakExports;
	counter_t		m_cWeakImports;
	V::VTwiddler		m_bDefunct;
    };
}


#endif
