#ifndef Vca_VcaSelf_Interface
#define Vca_VcaSelf_Interface 

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaSite.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaSelf : public VcaSite {
	DECLARE_CONCRETE_RTTLITE (VcaSelf, VcaSite);

    //  class SiteInfo
    public:
	class SiteInfo;

    //  Friends
	friend class SiteInfo;

	friend class VcaOID;
	friend class VcaSite;

    //  Construction
    public:
	VcaSelf ();

    //  Destruction
    private:
	~VcaSelf ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  Connectivity
    private:
	bool isConnected_() const OVERRIDE {;
	    return isConnected ();
	}
	bool isUsable_() const OVERRIDE {;
	    return isUsable ();
	}
	void processCallbackRequest_(
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    ConnectKind         xConnectKind
	) OVERRIDE;
    public:
	bool isConnected () const {
	    return true;
	}
	bool isntConnected () const {
	    return false;
	}

	bool isUsable () const {
	    return true;
	}
	bool isntUsable () const {
	    return false;
	}

    //  Connection (Transport) Management
    private:
	void attach_(VcaTransportInbound *pTransport) OVERRIDE;
	void attach_(VcaTransportOutbound *pTransport) OVERRIDE;
	void detach_(VcaTransportInbound *pTransport) OVERRIDE;
	void detach_(VcaTransportOutbound *pTransport) OVERRIDE;
	void startNextSerializerOn_(VcaTransportInbound *pTransport) OVERRIDE;
	void startNextSerializerOn_(VcaTransportOutbound *pTransport) OVERRIDE;
	bool getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport) OVERRIDE;

    //  Evaluation
    private:
	void evaluateOutgoing_ (VMessage *pMessage) OVERRIDE {
	}

    //  Lifetime
    private:
	void markDefunct_() OVERRIDE;

    public:
	Vca_API count_t keepaliveCount () const;
	Vca_API void incrementKeepaliveCount ();
	Vca_API void decrementKeepaliveCount ();

    private:
	void onFirstKeepalive ();
	void onFinalKeepalive ();

    public:
	Vca_API bool keepalivesIncludeExports () const;
	Vca_API void setKeepalivesIncludeExports ();
	Vca_API void clearKeepalivesIncludeExports ();

    public:
	Vca_API bool keepalivesManageShutdown () const;
	Vca_API void setKeepalivesManageShutdown ();
	Vca_API void clearKeepalivesManageShutdown ();

    //  Object Access/Creation
    public:
	using BaseClass::getObject;
	bool getObject (VcaOIDL::Reference& rpOIDL, IVUnknown *pObject);

    //  Object Export
    private:
    /*------------------------------------------------------------------------*
     *	In Vca, import and export refer to the relationship between objects
     *	in this process and the site modeled by a particular VcaSite object.
     *	The two VcaSite subclasses implement the export and import operations
     *	to reflect who they are and ensure that inter-site object references
     *	are protected.  The one and only instance of the VcaSelf class models
     *	this process.  By definition, all of the objects in this process are
     *	already managed by this process, making the implementations of these
     *	operations appropriately empty.
     *------------------------------------------------------------------------*/
	void createExportOf (IVUnknown *pObject, bool bWeak) OVERRIDE;
	void createExportOf (VcaOID *pOID, bool bWeak) OVERRIDE;
	void deleteExportOf (
	    VMessageHolder<IPeer_Ex2> const& rMessage, VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages
	) OVERRIDE;
	bool weakenExportOf (VcaOID *pOID) OVERRIDE;
    protected:
	void onExportCountIsZero () OVERRIDE;
	void onExportCountWasZero () OVERRIDE;

    //  Object Import
    private:
	void createImportOf (VcaOID *pOID, bool bWeak) OVERRIDE;
	void deleteImportOf (VcaOID *pOID) OVERRIDE;
	bool weakenImportOf (VcaOID *pOID) OVERRIDE;

	bool receive_(
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rObjectSSID,
	    VTypeInfo*		pObjectType,
	    VcaSite*		pSource,
	    bool		bWeak
	) OVERRIDE {
	    return receive (rpOID, rObjectSSID, pObjectType, pSource, bWeak);
	}
	bool receive_(
	    VcaOID::Reference& rpOID, VcaSSID const &rObjectSSID, VcaSite *pSource, bool bWeak
	) OVERRIDE {
	    return receive (rpOID, rObjectSSID, pSource, bWeak);
	}
    protected:
	void onImportCountIsZero () OVERRIDE;
	void onImportCountWasZero () OVERRIDE;

    public:
	bool receive (
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rObjectSSID,
	    VTypeInfo*		pObjectType,
	    VcaSite*		pSource,
	    bool		bWeak
	) {
	    return receive (rpOID, rObjectSSID, pSource, bWeak);
	}
	bool receive (
	    VcaOID::Reference& rpOID, VcaSSID const &rObjectSSID, VcaSite *pSource, bool bWeak
	);

    //  Object Loopback
    public:
	typedef VkMapOf<VcaOID::Reference,VcaOID*,VcaOID*,count_t> LoopbackSet;
    private:
	void createLoopbackOf (VcaOID* pOID, bool bWeak);
	void deleteLoopbackOf (VcaOID* pOID, bool bWeak);

    //  Remote Import
    private:
	void createRemoteImportOf (VcaOID *pOID) const OVERRIDE {
	}
        void deleteRemoteImportOf (VcaOIDR *pOIDR) const OVERRIDE {
	}

    //  Remote Reflection
    private:
	void getReflection_(IPeer::Reference& rpReflection) OVERRIDE {
	    getReflection (rpReflection);
	}
	void setReflection (IPeer *pReflection) OVERRIDE {
	}
    public:
	void getReflection (IPeer::Reference& rpReflection) {
	    getRole (rpReflection);
	}

    //  Routing
    public:
	void addRoute (VcaPeer *pPeer, VcaRouteStatistics const &rMetrics) OVERRIDE;
	void addRouteTo (VcaSite *pSite, VcaRouteStatistics const &rMetrics) OVERRIDE;
	void getBestRouteMetrics (VcaRouteStatistics &rMetrics) const OVERRIDE;

    //  Self
    private:
	bool isSelf_() const OVERRIDE {
	    return isSelf ();
	}
    public:
	bool isSelf () const {
	    return true;
	}

    //  Site Info
    private:
	VSiteInfo* getSiteInfo_(VSiteInfo::Reference& rpInfo) const OVERRIDE;

    //  Tracing and Display
    public:
	void displayPendingMessageCounts () const OVERRIDE;
	void displayRoutes () const OVERRIDE;

    //  State
    private:
	LoopbackSet	m_iLoopbackSet;
	counter_t	m_cKeepalives;
	V::VTwiddler	m_bKeepalivesIncludeExports;
	V::VTwiddler	m_bKeepalivesManageShutdown;
    };
}


#endif
