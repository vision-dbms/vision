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
	bool isConnected_() const {;
	    return isConnected ();
	}
	bool isUsable_() const {;
	    return isUsable ();
	}
	void processCallbackRequest_(
	    IAckReceiver*	pAckReceiver,
	    ICallbackReceiver*	pCallbackReceiver,
	    IPassiveCall*	pCallbackRequest,
	    VString const&	rCallbackAddress,
	    uuid_t const&	rCallbackID,
	    ConnectKind         xConnectKind
	);
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
	void attach_(VcaTransportInbound *pTransport);
	void attach_(VcaTransportOutbound *pTransport);
	void detach_(VcaTransportInbound *pTransport);
	void detach_(VcaTransportOutbound *pTransport);
	void startNextSerializerOn_(VcaTransportInbound *pTransport);
	void startNextSerializerOn_(VcaTransportOutbound *pTransport);
	bool getSpecificFreeOutboundTransport_ (VcaTransportOutbound *pTransport);

    //  Evaluation
    private:
	void evaluateOutgoing_ (VMessage *pMessage) {
	}

    //  Lifetime
    private:
	virtual /*override*/ void markDefunct_();

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
	void createExportOf (IVUnknown *pObject, bool bWeak);
	void createExportOf (VcaOID *pOID, bool bWeak);
	void deleteExportOf (
	    VcaOID *pOID, U32 cExports, U32 cWeakExports, U32 cMessages
	);
	bool weakenExportOf (VcaOID *pOID);
    protected:
	virtual /*override*/ void onExportCountIsZero ();
	virtual /*override*/ void onExportCountWasZero ();

    //  Object Import
    private:
	void createImportOf (VcaOID *pOID, bool bWeak);
	void deleteImportOf (VcaOID *pOID);
	bool weakenImportOf (VcaOID *pOID);

	bool receive_(
	    VcaOID::Reference&	rpOID,
	    VcaSSID const&	rObjectSSID,
	    VTypeInfo*		pObjectType,
	    VcaSite*		pSource,
	    bool		bWeak
	) {
	    return receive (rpOID, rObjectSSID, pObjectType, pSource, bWeak);
	}
	bool receive_(
	    VcaOID::Reference& rpOID, VcaSSID const &rObjectSSID, VcaSite *pSource, bool bWeak
	) {
	    return receive (rpOID, rObjectSSID, pSource, bWeak);
	}
    protected:
	virtual /*override*/ void onImportCountIsZero ();
	virtual /*override*/ void onImportCountWasZero ();

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
	void createRemoteImportOf (VcaOID *pOID) const {
	}
        void deleteRemoteImportOf (VcaOIDR *pOIDR) const {
	}

    //  Remote Reflection
    private:
	void getReflection_(IPeer::Reference& rpReflection) {
	    getReflection (rpReflection);
	}
	void setReflection (IPeer *pReflection) {
	}
    public:
	void getReflection (IPeer::Reference& rpReflection) {
	    getRole (rpReflection);
	}

    //  Routing
    public:
	void addRoute (VcaPeer *pPeer, VcaRouteStatistics const &rMetrics);
	void addRouteTo (VcaSite *pSite, VcaRouteStatistics const &rMetrics);
	void getBestRouteMetrics (VcaRouteStatistics &rMetrics) const;

    //  Self
    private:
	bool isSelf_() const {
	    return isSelf ();
	}
    public:
	bool isSelf () const {
	    return true;
	}

    //  Site Info
    private:
	VSiteInfo* getSiteInfo_(VSiteInfo::Reference& rpInfo) const;

    //  Tracing and Display
    public:
	void displayPendingMessageCounts () const;
	void displayRoutes () const;

    //  State
    private:
	LoopbackSet	m_iLoopbackSet;
	counter_t	m_cKeepalives;
	V::VTwiddler	m_bKeepalivesIncludeExports;
	V::VTwiddler	m_bKeepalivesManageShutdown;
    };
}


#endif
