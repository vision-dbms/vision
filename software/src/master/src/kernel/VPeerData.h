#ifndef VPeerData_Interface
#define VPeerData_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkUUID.h"
#include "VkSetOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    typedef V::uuid_t uuid_t;

    class Vca_API VPeerData : public VTransient {
	DECLARE_FAMILY_MEMBERS (VPeerData, VTransient);

    public:
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ListenerDomains;
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ConnectorDomains; 
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ConnectedPeers;

    // Construction
    public:
	VPeerData (uuid_t const &peer) : m_iPeer (peer), m_iInFinalPath(false) {
	}
	VPeerData () : m_iInFinalPath(false) {
	}

    // Destruction
    public:
	~VPeerData() {
	}

    // Access 
    public:
	uuid_t const &peerUUID() const {
	     return m_iPeer;
	}

	void setPeerUUID(uuid_t const &peer)  {
	    m_iPeer = peer;
	}

	bool addListenerDomain (uuid_t const &domain) {
	    return m_iListenerDomains.Insert (domain);
	}

	bool addConnectorDomain (uuid_t const &domain) {
	    return m_iConnectorDomains.Insert (domain);
	}

	bool addConnectedPeer (uuid_t const &peer) {
	    return m_iConnectedPeers.Insert (peer);
	}

	void setListenerDomains(ListenerDomains const &rOther) {
	    m_iListenerDomains.DeleteAll();
	    for(unsigned i=0; i<rOther.cardinality(); i++) 
		m_iListenerDomains.Insert(rOther[i]);
	}

	void setConnectorDomains(ConnectorDomains const &rOther) {
	    m_iConnectorDomains.DeleteAll();
	    for(unsigned int i=0; i<rOther.cardinality(); i++) 
		m_iConnectorDomains.Insert(rOther[i]);
	}

	void setConnectedPeers(ConnectedPeers const  &rOther) {
	    m_iConnectedPeers.DeleteAll();
	    for(unsigned int i=0; i<rOther.cardinality(); i++) 
		m_iConnectedPeers.Insert(rOther[i]); 
	}

	ListenerDomains const &listenerDomains() const {
	    return m_iListenerDomains;
	}
    
	ConnectorDomains const &connectorDomains() const {
	    return m_iConnectorDomains;
	}

	ConnectedPeers const &connectedPeers() const {
	    return m_iConnectedPeers;
	}

	unsigned int numListenerDomains() const {
	    return m_iListenerDomains.cardinality();
	}

	unsigned int numConnectorDomains() const {
	    return m_iConnectorDomains.cardinality();
	}

	unsigned int numConnectedPeers() const {
	    return m_iConnectedPeers.cardinality();
	}

	bool isConnectedToPeer(uuid_t const &peerUUID) const {
	    for(unsigned int i=0; i< m_iConnectedPeers.cardinality(); i++)  {
		if(VkUUID::eq(peerUUID, m_iConnectedPeers[i]))
		    return true;
	    }
	    return false;
	}

	bool inFinalPath() const {
	    return m_iInFinalPath;
	}

	void setInFinalPath(bool value) {
	    m_iInFinalPath = value;
	}

	VPeerData &operator= (VPeerData const &rOther) {
	    if(&rOther != this) {
		m_iPeer = rOther.peerUUID();
		m_iInFinalPath = rOther.inFinalPath();
		setListenerDomains   (rOther.listenerDomains());
		setConnectorDomains  (rOther.connectorDomains());
		setConnectedPeers    (rOther.connectedPeers());
	    }
	    return *this;
	}

	void deleteAllConnectedPeers() {
	    m_iConnectedPeers.DeleteAll();
	}

	// Testing
    public:
	void displayInfo () const;

    public/*private*/:	// can't declare friend class VcaSerializer_<VPeerData>
	ListenerDomains		m_iListenerDomains;
	ConnectorDomains	m_iConnectorDomains;
	ConnectedPeers		m_iConnectedPeers;
	bool			m_iInFinalPath;
	uuid_t			m_iPeer;
    };
}


#endif
