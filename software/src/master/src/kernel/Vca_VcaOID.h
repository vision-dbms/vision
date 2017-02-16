#ifndef Vca_VcaOID_Interface
#define Vca_VcaOID_Interface 

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

class IVUnknown;

#include "VkUUID.h"

/*************************
 *****  Definitions  *****
 *************************/
namespace Vca {
    typedef V::uuid_t uuid_t;

    class VMessage;

    class VcaPeer;
    class VcaSelf;
    class VcaSite;

    /**
     *  VcaOID is an abstract class whose instances represent application objects
     *  known to Vca.  Specifically, those are the local objects exported from
     *  this process and all remote objects imported into this process.  Two
     *  concrete subclasses, VcaOIDL and VcaOIDR, respectively represent these
     *  two kinds of objects.
     *
     *  All objects, whether local or imported can be exported to other sites.
     *  This class implements most the machinery that tracks those exports.
     *
     * @see VcaOIDL
     * @see VcaOIDR
     */
    class ABSTRACT VcaOID : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VcaOID, VReferenceable);

	friend class ::IVUnknown;
	friend class VcaPeer;
	friend class VcaSelf;

    //  Aliases
    public:
	typedef Reference OIDReference;

	typedef U32 count_t;

    //  Export
    public:
	class Export : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (Export, VReferenceable);

	    friend class VcaOID;
	    friend class VcaPeer;

	//  Construction
	protected:
	    Export (VcaOID *pOID, VcaPeer *pImporter, bool bWeak);

	//  Destruction
	private:
	    ~Export ();

	//  Access
	public:
	    VcaPeer *importer () const {
		return m_pImporter;
	    }
	private:
	    Export *importerListPredecessor () const {
		return m_pImporterListPredecessor;
	    }
	    Export *importerListSuccessor () const {
		return m_pImporterListSuccessor;
	    }
	public:
	    VcaOID *oid () const {
		return m_pOID;
	    }

	//  Object Export
	public:
	    count_t exportCount () const {
		return m_cExports;
	    }
	    count_t weakExportCount () const {
		return m_cWeakExports;
	    }
	private:
	    void clearExportCount ();
	    void clearWeakExportCount ();

	    void incrementExportCount (bool bWeak);
	    void decrementExportCount (count_t cExports, count_t cWeakExports);

	    void incrementWeakExportCount ();
	    bool weakenExport ();

	    void onDefunctPeer ();

	    void onFinalExport ();

	    void onShutdown ();

	//  Display
	public:
	    using BaseClass::displayInfo;
	    void displayInfo () const;
	    void getInfo (VString &rResult, const VString &rPrefix) const;

	//  State
	protected:
	    OIDReference	const	m_pOID;
	    VReference<VcaPeer>	const	m_pImporter;
	    Pointer			m_pImporterListPredecessor;
	    Pointer			m_pImporterListSuccessor;
	    count_t			m_cExports;
	    count_t			m_cWeakExports;
	};
	friend class Export;

    //  Construction
    protected:
	VcaOID (VcaSSID const &rSSID);

    //  Destruction
    protected:
	~VcaOID ();

    //  Interface Access
    private:
	virtual void supplyInterface_(VReference<IVUnknown>&rpInterface) = 0;
    public:
	void supplyInterface (VReference<IVUnknown>&rpInterface) {
	    supplyInterface_(rpInterface);
	}

    //  Connectivity
    public:
	virtual bool isConnected () const = 0;
	bool isntConnected () const {
	    return !isConnected ();
	}

    //  Object Access
    private:
	virtual uuid_t const& objectSiteUUID_() const = 0;
	virtual VTypeInfo *objectType_() const = 0;
    public:
	VcaSSID const &objectSSID () const {
	    return m_iSSID;
	}
	uuid_t const& objectSiteUUID () const {
	    return objectSiteUUID_();
	}
	VTypeInfo *objectType () const {
	    return objectType_();
	}

    //  Message Count Update
    protected:
	virtual void updateMessageCounts (count_t cReported, count_t cReceived, count_t cProcessed);

    //  Object Export
    protected:
	typedef VkMapOf<
	    VReference<VcaPeer>, VcaPeer*, VcaPeer const*, Export::Reference
	> ExportTable;

	virtual void onFinalExport ();

    private:
	void createExportTo (	//  ... called from VcaPeer and IVUnknown
	    VcaPeer *pPeer, bool bWeak
	);
	void createExportTo (	//  ... called from IVUnknown
	    VcaSelf *pSelf, bool bWeak
	);
	void deleteExportTo (	//  ... called from VcaPeer
	    VcaPeer *pPeer, count_t cExports, count_t cWeakExports, count_t cMessages
	);
	bool weakenExportTo (	//  ... called from VcaPeer
	    VcaPeer *pPeer
	);
	void detach (Export *pExport);

    protected:
	count_t exportCount () const;
	count_t weakExportCount () const;
	ExportTable const &exportTable () const {
	    return m_iExportTable;
	}

    public:
	count_t importerCount () const {
	    return m_iExportTable.cardinality ();
	}

	bool isExported () const {
	    return m_iExportTable.cardinality () > 0;
	}
	bool isExportedTo (VcaPeer const *pPeer) const {
	    return m_iExportTable.Contains (pPeer);
	}

	bool isntExported () const {
	    return !isExported ();
	}
	bool isntExportedTo (VcaPeer const *pPeer) const {
	    return !isExportedTo (pPeer);
	}

    //  Object Import
    private:
        virtual void createImportFrom (VcaPeer *pPeer, bool bWeak) = 0;
	virtual void deleteImportFrom (VcaPeer *pPeer) = 0;
	virtual bool weakenImportFrom (VcaPeer *pPeer) = 0;
    public:
	void createRemoteImportFrom (VcaSite *pSite);

    //  Interesting Objects
    public:
	bool isInteresting () const {
	    return m_bInteresting;
	}
	bool isntInteresting () const {
	    return !m_bInteresting;
	}
	void setInteresting (bool bInteresting = true) {
	    m_bInteresting = bInteresting;
	}
	Vca_API void logAsInteresting (char const *pWhere);
	Vca_API void logIfInteresting (char const *pWhere) const;
	Vca_API void logIt (char const *pWhere) const;

    //  Tracing and Display
    public:
	using BaseClass::displayInfo;
	void displayInfo () const;
	void getInfo (VString &rResult, const VString &rPrefix) const;

	void displayExportTable () const;
	void getExportTable (VString &rResult, const VString &rPrefix) const;

    //  State
    private:
        /**
         * Unique identifier, at the site where the instance is implemented, for the object instance that this OID references.
         */
	VcaSSID const	m_iSSID;
        /**
         * The places to which this object has been exported by this process.
         */
	ExportTable	m_iExportTable;
    protected:
	count_t		m_cMessagesReported;
    private:
	bool		m_bInteresting;
    };
}


#endif
