#ifndef Vca_VcaConnection_Interface
#define Vca_VcaConnection_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VInterfaceSource_.h"

#include "V_VPointer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IAckReceiver.h"
#include "Vca_IPeer.h"

#include "Vca_VcaSerializer.h"
#include "Vca_VcaSite.h"
#include "Vca_VcaOffer.h"

#include "Vca_VTimer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VBSConsumer;
    class VBSProducer;

    class VcaPeer;
    class VcaSerializerForPlumbing;

    class VcaTransportInbound;
    class VcaTransportOutbound;

    class Vca_API VcaConnection : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VcaConnection, VRolePlayer);

	friend class VcaPeer;
	friend class VcaSerializerForPlumbing;
	friend class VcaTransportInbound;
	friend class VcaTransportOutbound;

    //  Object Receiver
    public:
	typedef IVReceiver<IVUnknown*> IQueryClient;

    //  Offer Types
    public:
	typedef VcaOffer::Offering Offering;

    //  Transport Pointers
    public:
	typedef V::VPointer<VcaTransportInbound>  InboundTransportPointer;
	typedef V::VPointer<VcaTransportOutbound> OutboundTransportPointer;

    //****************************************************************
    //  Transport Statistics
    public:
	class TransportStatistics : public VTransient {
	    DECLARE_FAMILY_MEMBERS (TransportStatistics, VTransient);

	//  Construction
	public:
	    TransportStatistics () : m_cBytes (0), m_cMessages (0) {
	    }

	//  Destruction
	public:
	    ~TransportStatistics () {
	    }

	//  Display
	public:
	    void display (VString const &rWhere) const;

	//  Update
	public:
	    void incrementByteCount (size_t cBytes) {
		m_cBytes += cBytes;
	    }
	    void incrementMessageCount () {
		m_cMessages++;
	    }

	//  State
	private:
	    size_t m_cBytes;
	    unsigned int m_cMessages;
	};

    //****************************************************************
    //  Construction
    public:
	VcaConnection (
	    VBSProducer *pBSToHere, VBSConsumer *pBSToPeer, Offering const &rOffering, IAckReceiver *pAckReceiver = 0
	);

    //  Destruction
    private:
	~VcaConnection ();

    //  Ack, Nack, and Quack
    private:
	void sayAck ();
	void sayNack ();
	void sayQuack (IError *pClient, VString const &rMessage);

    //  Query
    public:
	bool hasAnIncomingTransport () const {
	    return m_pTransportInbound.isntNil ();
	}
	bool hasAnOutgoingTransport () const {
	    return m_pTransportOutbound.isntNil ();
	}

	bool offersNothing () const {
	    return m_iOffering.offersNothing ();
	}
	bool offersSomething () const {
	    return m_iOffering.offersSomething ();
	}

    //  Plumbing
    private:
	void plumb (VBSProducer *pBSToHere);
	void plumb (VBSConsumer *pBSToPeer);

	void detach (VcaTransportInbound *pTransport);
	void detach (VcaTransportOutbound *pTransport);

	void getPeer (VcaSite::Reference &rpPeer) const;
	void setPeer (VcaSite *pPeer);
	void suspend (VcaSerializer *pSerializer);

	void supplyOurside (IPeer::Reference &rpOurside);
	void onConnect (IPeer *pFarside);

    //  Service Query
    public:
	ThisClass *queryService (VInterfaceQuery const &rQuery);

    //  Statistics
    private:
	void displayStatistics () const;

	void incrementIncomingByteCount (size_t sTransfer) {
	    m_iIncomingStatistics.incrementByteCount (sTransfer);
	}
	void incrementIncomingMessageCount () {
	    m_iIncomingStatistics.incrementMessageCount ();
	}
	void incrementOutgoingByteCount (size_t sTransfer) {
	    m_iOutgoingStatistics.incrementByteCount (sTransfer);
	}
	void incrementOutgoingMessageCount () {
	    m_iOutgoingStatistics.incrementMessageCount ();
	}

    //  State
    private:
	Offering const			m_iOffering;
	IAckReceiver::Reference		m_pAckReceiver;
	VcaOffer::Reference		m_pOffer;

	InboundTransportPointer		m_pTransportInbound;
	OutboundTransportPointer	m_pTransportOutbound;

	VcaSite::Reference		m_pPeer;
	VcaSerializer::Reference	m_pPeerSuspendedSerializer;

	VInterfaceSource_<IPeer>	m_pFarside;

	TransportStatistics		m_iIncomingStatistics;
	TransportStatistics		m_iOutgoingStatistics;
    };
}


#endif
