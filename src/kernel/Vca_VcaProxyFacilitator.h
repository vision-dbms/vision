#ifndef VProxyFacilitator_Interface
#define VProxyFacilitator_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"
#include "VkSetOf.h"

/**************************
 *****  Declarations  *****
 **************************/

template <class Actor, class Datum> class VReceiver;


/*************************
 *****  Definitions  *****
 *************************/
namespace Vca {
    class IPeer;

    class VcaOIDR;
    class VcaPeer;

    class VcaProxyFacilitator : public VReferenceable {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VcaProxyFacilitator, VReferenceable);
    
    // Typedefs
    public:
	typedef VReceiver<ThisClass, IPeer*> IPeerReceiver;
	typedef VkSetOf<VcaOIDR*, VcaOIDR*, VcaOIDR const*> PendingProxySet;
	typedef PendingProxySet::Iterator Iterator;


    // Construction
    public:
	VcaProxyFacilitator (
	    VcaPeer *pSourcePeer, VcaPeer *pTargetPeer
	);
	VcaProxyFacilitator (
	    VcaPeer *pSourcePeer,	    VcaPeer *pTargetPeer,
	    IPeer   *pSourcePeerReflection, IPeer   *pTargetPeerReflection
	);

    // Destruction
    protected:
	~VcaProxyFacilitator ();

    //  Update
    public:
	void setSourcePeerReflection (IPeer *pPeer);
	void setTargetPeerReflection (IPeer *pPeer);
	bool removePendingProxy (VcaOIDR *pOIDR) {
	    return m_iPendingProxySet.Delete (pOIDR);
	}

    // Query
    public:
	bool reflectionsAreValid () const {
	    return m_pSourcePeerReflection.isntNil () && m_pTargetPeerReflection.isntNil ();
	}
	bool isWaitingToFacilitate (VcaOIDR *pOIDR) const {
	    return m_iPendingProxySet.Contains (pOIDR);
	}
	VcaPeer *sourcePeer () const {
	    return m_pSourcePeer;
	}
	VcaPeer *targetPeer () const {
	    return m_pTargetPeer;
	}

    //  Reflection Receivers
    public:
	void onSourcePeerReflection (IPeerReceiver *pReceiver, IPeer *pIPeer);
	void onTargetPeerReflection (IPeerReceiver *pReceiver, IPeer *pIPeer);

    //  Proxy Facilitation
    public:
	void processProxy (VcaOIDR *pOIDR);

    //  Internal Functions
    protected:
	void startup		    ();
	void onBothReflections      ();
	void delegateReflections    ();
	void delegatePendingProxies ();
	void queueProxy		    (VcaOIDR *pOIDR);
	void delegateProxy          (VcaOIDR *pOIDR);

    // State
    protected:
	VReference<VcaPeer> const m_pSourcePeer;
	VReference<VcaPeer> const m_pTargetPeer;

	VReference<IPeer> m_pSourcePeerReflection;
	VReference<IPeer> m_pTargetPeerReflection;

	PendingProxySet m_iPendingProxySet;
    };
}

#endif
