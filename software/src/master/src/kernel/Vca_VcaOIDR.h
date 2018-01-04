#ifndef Vca_VcaOIDR_Interface
#define Vca_VcaOIDR_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaOID.h"

#include "Vca_VTriggerTicket.h"
#include "Vca_VMessageStarter.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VTypeInfo.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class ITrigger;

    class VcaOIDR;
    class VcaProxyFacilitator;
    class VcaSerializerForInterface;
    class VMessageScheduler;
    class VSiteInfo;
    class VTimer;

/*********************************************************************/

    class Vca_API VcaOIDX : public VcaOID, protected VMessageStarter {
	DECLARE_ABSTRACT_RTTLITE (VcaOIDX, VcaOID);

    //  Vise
    public:
	class Vise : public Reference {
	    DECLARE_FAMILY_MEMBERS (Vise, Reference);

	//  Construction
	public:
	    explicit Vise (VcaOIDX *pOIDX) : BaseClass (pOIDX) {
	    }

	//  Destruction
	public:
	    ~Vise () {
	    }

	//  Access
	public:
	    operator VcaOIDR* () const {
		return m_pReferent ? m_pReferent->oidr () : 0;
	    }

	//  Query
	public:
	    bool isConnected () const {
		return isNil () || m_pReferent->isConnected ();
	    }
	    bool isUsable () const {
		return isNil () || m_pReferent->isUsable ();
	    }

	//  Grip
	public:
	    void detachInterface (IVUnknown *pProxy) const {
                if (isntNil ())
                    m_pReferent->detachInterface (pProxy);
	    }
	};
	friend class Vise;

    //  Construction
    protected:
	VcaOIDX (SSID const &rObjectSSID, VTypeInfo *pObjectType);

    //  Destruction
    protected:
	~VcaOIDX () {
	}

    //  Access
    private:
	virtual VcaOIDR *oidr_();
	virtual VTypeInfo *objectType_() const;
    public:
	VcaOIDR *oidr () {
	    return oidr_();
	}
	VTypeInfo *objectType () const {
	    return m_pObjectType;
	}

    //  Query
    private:
	virtual bool isConnected_() const = 0;
	virtual bool isUsable_() const = 0;
    public:
	bool isConnected () const {
	    return isConnected_();
	}
	bool isUsable () const {
	    return isUsable_();
	}

    //  Interface Management
    protected:
	virtual void supplyInterface_(VReference<IVUnknown>&rpInterface); // override VcaOID
	virtual bool detachInterface (IVUnknown *pProxy);

    //  Message Scheduling
    public:
	virtual bool defersTo (VMessageScheduler &rScheduler);

    //  State
    private:
	VTypeInfo::Reference	const	m_pObjectType;
	V::VAggregatePointer<IVUnknown>	m_pProxy;
    };

/*********************************************************************/

    class Vca_API VcaOIDR : public VcaOIDX {
	DECLARE_CONCRETE_RTTLITE (VcaOIDR, VcaOIDX);

	friend class ::IVUnknown;
	friend class VcaPeer;
	friend class VcaProxyFacilitator;
	friend class VcaSerializerForInterface;
	friend class VMessage;
	friend class VMessageScheduler;

    //  Import
    public:
	class Import : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (Import, VReferenceable);

	    friend class VcaOIDR;
	    friend class VcaPeer;

	//  Construction
	protected:
	    Import (VcaOIDR *pOIDR, VcaPeer *pExporter, bool bWeak);

	//  Destruction
	private:
	    ~Import ();

	//  Access
	public:
	    VcaPeer *exporter () const {
		return m_pExporter;
	    }
	private:
	    Import *exporterListPredecessor () const {
		return m_pExporterListPredecessor;
	    }
	    Import *exporterListSuccessor () const {
		return m_pExporterListSuccessor;
	    }
	public:
	    VcaOIDR *oidr () const {
		return m_pOIDR;
	    }

	//  Object Import
	public:
	    count_t importCount () const {
		return m_cImports;
	    }
	    count_t weakImportCount () const {
		return m_cWeakImports;
	    }
	private:
	    void clearImportCount ();
	    void clearWeakImportCount ();

	    void incrementImportCount (bool bWeak);
	    void incrementWeakImportCount ();

	    void deleteRemoteExport (count_t cMessages);

	    bool weakenImport ();

	    void onDefunctPeer ();

	    void onFinalImport ();

	    void onShutdown ();

	//  Display
	public:
	    using BaseClass::displayInfo;
	    void displayInfo () const;
	    void getInfo (VString &rResult, const VString &rPrefix) const;

	//  State
	private:
	    VcaOIDR*			m_pOIDR;
	    VReference<VcaPeer> const	m_pExporter;
	    Pointer			m_pExporterListPredecessor;
	    Pointer			m_pExporterListSuccessor;
	    count_t			m_cImports;
	    count_t			m_cWeakImports;
	};
	friend class Import;

    //  Manager
    public:
	class Manager;
	friend class Manager;
    private:
	static VReference<Manager> g_pTheMan;
	static Manager *TheMan ();

    //  Imported From Peers
    public:
	typedef VkMapOf<
	    VReference<VcaPeer>, VcaPeer*, VcaPeer const*, VReference<Import>
	> ImportTable;
	typedef ImportTable::Iterator Iterator;

    //  Pending Facilitation Map (TargetPeer, SourcePeer)
    public:
	typedef VkMapOf<
	    VReference<VcaPeer>, VcaPeer*, VcaPeer const*, VReference<VcaPeer>
	> PendingFacilitationMap;

    //  Construction
    private:
	VcaOIDR (SSID const &rObjectSSID, VTypeInfo *pObjectType, VcaPeer *pObjectSite);

    //  Destruction
    private:
	~VcaOIDR (); 

    //  Access
    private:
	virtual VcaOIDR *oidr_();
	VcaPeer *objectSite () const {
	    return m_pObjectSite;
	}
	uuid_t const& objectSiteUUID_() const {
	    return objectSiteUUID ();
	}
    public:
	VcaOIDR *oidr () {
	    return this;
	}
	uuid_t const &objectSiteUUID () const;

	unsigned int proxyList () const {
	    return m_xProxyList;
	}

    //  Connectivity
    protected:
	virtual /*override*/ bool isConnected_() const;
	virtual /*override*/ bool isUsable_() const;
    public:
	bool isConnected () const;
	bool isntConnected () const {
	    return !isConnected ();
	}
	bool isUsable () const;
	bool isntUsable () const {
	    return !isUsable ();
	}
	bool requestNoRouteToPeerCallback (
	    VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended = false
	) const;

    //  Control
    private:
	static void onDefunctPeer (VcaPeer *pPeer);	// ... used by the manager to accelerate cleanup.
    public:
	static void onShutdown ();

    //  Message Bookkeeping
    protected:
	count_t messagesToReport ();

    //  Message Scheduling
    public:
	virtual bool defersTo (VMessageScheduler &rScheduler);

    //  Message Starting
    private:
	void start_(VMessage *pMessage);

    //  Object Access
    private:
	virtual void supplyInterface_(VReference<IVUnknown>&rpInterface);	// override VcaOID
	virtual bool detachInterface (IVUnknown *pProxy);			// override VcaOIDX

    //  Object Import
    private:
        void createImportFrom (VcaPeer *pPeer, bool bWeak);
	void deleteImportFrom (VcaPeer *pPeer);
	bool weakenImportFrom (VcaPeer *pPeer);

	void detach (Import *pImport);

	void eliminateRedundantImports ();

    public:
        bool importedFrom (VcaPeer *pPeer) const {
	    return m_iImportTable.Contains (pPeer);
	}
	bool importCountForPeer (
	    VcaPeer *pImportedFromPeer, count_t& rImportCount
	) const;
	bool weakenImport ();

    //  Object Reclamation
    private:
	enum {
	    InactiveListCount = 1
	};
	enum List {
	    List_0,
	    List_Active = InactiveListCount,
	    List_Facilitation,
	    List_Dead,
	    List_None
	};
	static VString &GetListName (VString &rName, List xList);
    public:
	VString const &getListName (VString &rName) const {
	    return GetListName (rName, m_xProxyList);
	}

    //  Garbage Collection
    private:
	void clean ();

	void moveToProxyList (List xList);

    //  Proxy Facilitation
    private:
	bool initiateFacilitation ();

	void addFacilitationPeers (VcaPeer *pTargetPeer, VcaPeer *pSourcePeer);
	bool sourceFacilitationPeerFor (VcaPeer *pTargetPeer, VcaPeer *&rpSourcePeer);
	bool deleteFacilitationTo (VcaPeer *pTargetPeer);
	void refacilitateTo (VcaPeer *pNewTargetPeer, VcaPeer *pOldTargetPeer);

	VcaPeer *getBestImportedFromPeer ();

    //  Trace Control
    private:
        enum GCTracing {
          Tracing_NotInitialized,
          Tracing_On,
          Tracing_Off
        };
	static GCTracing g_bTracingGC;

    public:
        static bool initializeTraceOption () {
          if (g_bTracingGC == Tracing_NotInitialized) {
            char const *pEnvValue = getenv ("TraceProxyGC");
            g_bTracingGC = pEnvValue ? (strcasecmp (pEnvValue, "TRUE")? Tracing_Off : Tracing_On) : Tracing_Off;
          }
          return true;
        }

	static bool gcTrace () {
	    return initializeTraceOption () && g_bTracingGC==Tracing_On;
	}
	static void setGCTrace (bool bTracingGC) {
	    g_bTracingGC = bTracingGC ? Tracing_On : Tracing_Off;
	}

	void displayInfo () const;
	void displayInfo (char const *pWhat) const;
	void getInfo (VString &rResult, const VString &rPrefix) const;

	void displayImportTable () const;
	void getImportTable (VString &rResult, const VString &rPrefix) const;

    //  State
    private:
	//  This should be a VReference<Manager>, but MSVC can't compile it...
	Manager *const			m_pManager;
	VReference<VcaPeer> const	m_pObjectSite;
	ImportTable			m_iImportTable;
	List				m_xProxyList;
	Pointer				m_pPrevious;
	Pointer				m_pNext;
	counter_t			m_cMessagesWeSent;
	count_t				m_cMessagesWeReported;
	PendingFacilitationMap		m_iFacilitationMap;
    };
}


#endif
