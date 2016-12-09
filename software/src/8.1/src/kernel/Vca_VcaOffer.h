#ifndef Vca_VcaOffer_Interface
#define Vca_VcaOffer_Interface 

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPeer_Ex2.h"

#include "Vca_VTriggerSet.h"

#include "V_VReferenceable_.h"
#include "V_VTwiddler.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaSite;

    class VcaOffer : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VcaOffer, VRolePlayer);

    //  Aliases
    public:
	typedef VReference<VcaSite> SiteReference;

    //  Offering
    /****
     *     - Holds the object being offered. 
     *     - Holds a tracker that records the number of active consumers of this offering and
     *       allows for zero/non-zero offer triggers to be set that will be triggered when the
     *       offer count transitions to zero or non-zero respectively.
     ****/
    public:
	class Vca_API Offering  {
	    DECLARE_NUCLEAR_FAMILY (Offering);

	    friend class VcaOffer;

	//  Tracker
	public:
	    class Vca_API Tracker : virtual public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Tracker, VReferenceable);

		friend class Offering;

	    //  Construction
	    public:
		Tracker ();

	    //  Destruction
	    protected:
		~Tracker ();

	    //  Access
	    public:
		U32 offerCount () const {
		    return m_cOffers;
		}

	    //  Update
	    private:
		virtual void onIncrement (VcaOffer* pOffer);
		virtual void onDecrement (VcaOffer* pOffer);
	    private:
		void incrementOfferCount (VcaOffer* pOffer);
		void decrementOfferCount (VcaOffer* pOffer);
	   
	    //  State
	    private:
		U32 m_cOffers;
	    };

	//  Construction
	public:
	    Offering (VRolePlayer *pObject, Tracker *pTracker);
	    Offering (VRolePlayer *pObject = 0);
	    Offering (Tracker *pTracker);
	    Offering (ThisClass const &rOther);
	   
	//  Destruction
	public:
	    ~Offering () {
	    }

	//  Access
	public:
	    void supplyInterface (IVReceiver<IVUnknown*>* pInterfaceReceiver, VTypeInfo *pInterfaceType) const;

	    VRolePlayer *object () const {
		return m_pObject;
	    }
	    U32 offerCount () const {
		return m_pTracker->offerCount ();
	    }
	    Tracker *tracker () const {
		return m_pTracker;
	    }

	//  Query
	public:
	    bool offersNothing () const {
		return m_pObject.isNil ();
	    }
	    bool offersSomething () const {
		return m_pObject.isntNil ();
	    }

	//  Update
	private:
	    void incrementOfferCount (VcaOffer* pOffer) const {
		m_pTracker->incrementOfferCount (pOffer);
	    }
	    void decrementOfferCount (VcaOffer* pOffer) const {
		m_pTracker->decrementOfferCount (pOffer);
	    }
	   
	//  State
	private:
	    VRolePlayer::Reference	const m_pObject;
	    Tracker::Reference		const m_pTracker;
	};
	typedef Offering::Tracker Tracker;

    //  Construction
    public:
	VcaOffer (VcaSite *pPeer, Offering const &rOffering);

    //  Destruction
    private:
	~VcaOffer () ;

    //  Access
    public:
	VcaSite* site () const {
	    return m_pSiteObject;
	}

    //  Offer Management
    public:
	void incrementOfferCount ();
	void decrementOfferCount ();

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
	    VMessageHolder<IPeer_Ex2> const& rMessage, uuid_t const &rObjectSite, 
	    VcaSSID const &rObjectSSID, U32 cExports, U32 cWeakExports, U32 cMessages
	);

    //  IPeer1 Role
    public:
	void getRole (IPeer1::Reference &rpRole);

    //  IPeer1 Methods
    public:
	void WeakenExport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID);
	void WeakenImport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID);
	void GetProcessInfo (IPeer1* pRole, uuid_t const& rPeerUUID, IProcessInfoSink* pInfoSink);

    //  IPeer Role
    public:
	void getRole (IPeer::Reference &rpRole);

    //  IPeer Methods
    public/*private*/:
	void SinkInterface (IPeer *pRole, IVUnknown *pObject);

	void ReleaseExport (
	    VMessageHolder<IPeer_Ex2> const& rMessage, uuid_t const &rPeerUUID, 
	    VcaSSID const &rObjectSSID, U32 cExports, U32 cMessages
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

	void OnDone (VMessageHolder<IPeer_Ex2>const& rMessage);

    //  State
    private:
	SiteReference		const	m_pSiteObject;
	IPeer2::Reference	const	m_pSiteInterface;
	Offering		const	m_iOffering;
	V::VTwiddler			m_bOffered;
    };
}


#endif
