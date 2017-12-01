/*****  Vca_VcaConnection Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VcaConnection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength happy pill needed here
#include "Vca_VDevice.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_IError.h"
#include "Vca_ITrigger.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VcaTransport.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::VcaConnection  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaConnection::VcaConnection (
    VBSProducer *pConnectionToHere, VBSConsumer *pConnectionToPeer, Offering const &rOffering, IAckReceiver *pAckReceiver
) : m_iOffering (rOffering), m_pAckReceiver (pAckReceiver) {
    traceInfo ("Creating VcaConnection");
    retain (); {
	plumb (pConnectionToHere);
	plumb (pConnectionToPeer);
    } untain ();
}

/****
 * Note: When a VcaConnection object is destroyed the offer count on the object being exposed is decremented. 
 *       (When connections are abnormally terminated, transport heartbeats would detect the abormal connection
 *       termination and help in destroying the VcaConnection object)
 ****/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaConnection::~VcaConnection () {
    if (!isAtExit ())
	defaultLogger().printf ("+++ VcaConnection[%p]::~VcaConnection\n", this);
    traceInfo ("Destroying VcaConnection");
}

/****************************
 ****************************
 *****  Ack/Nack/Quack  *****
 ****************************
 ****************************/

void Vca::VcaConnection::sayAck () {
    if (m_pAckReceiver) {
	m_pAckReceiver->Ack ();
	m_pAckReceiver.clear ();
    }
}

void Vca::VcaConnection::sayNack () {
    sayQuack (0, "Nack");
}

void Vca::VcaConnection::sayQuack (IError *pInterface, VString const &rMessage) {
    m_pFarside.setTo (0, rMessage);
    if (m_pAckReceiver) {
	m_pAckReceiver->OnError (pInterface, rMessage);
	m_pAckReceiver.clear ();
    }
}


/**********************
 **********************
 *****  Plumbing  *****
 **********************
 **********************/

/*---------------------------------------------------------------------------*
 *  Until this routine checks that the transport is still 'attached' there
 *  is a potential inconsistency if the transport detachs itself before its
 *  constructor returns.
 *---------------------------------------------------------------------------*/
void Vca::VcaConnection::plumb (VBSProducer *pConnectionToHere) {
    m_pTransportInbound.setTo (new VcaTransportInbound (this, pConnectionToHere));
    m_pTransportInbound->plumbThyself ();
}

/*---------------------------------------------------------------------------*
 *  Until this routine checks that the transport is still 'attached' there
 *  is a potential inconsistency if the transport detachs itself before its
 *  constructor returns.
 *---------------------------------------------------------------------------*/
void Vca::VcaConnection::plumb (VBSConsumer *pConnectionToPeer) {
    m_pTransportOutbound.setTo (new VcaTransportOutbound (this, pConnectionToPeer));
    m_pTransportOutbound->plumbThyself ();
}

void Vca::VcaConnection::detach (VcaTransportInbound *pTransport) {
    traceInfo ("Entering Vca::VcaConnection::detach(I)");
    if (m_pTransportInbound.clearIf (pTransport)) {
	displayStatistics ();
	if (m_pPeer)
	    m_pPeer->detach (pTransport);
	if (m_pPeerSuspendedSerializer) {
	  // plumbing has failed, so tell the outbound to stop ...
	    VcaSerializer::Reference pSuspendedSerializer;
	    pSuspendedSerializer.claim (m_pPeerSuspendedSerializer);
	    pSuspendedSerializer->abort ();
	}
	if (m_pTransportOutbound)
	    m_pTransportOutbound->onShutdownInProgress ();
	else {
	    if (m_pOffer)
		m_pOffer->decrementOfferCount ();
	    sayQuack (0,"Incoming Transport Detached");
	}
    }
    traceInfo ("Exiting Vca::VcaConnection::detach(I)");
}

void Vca::VcaConnection::detach (VcaTransportOutbound *pTransport) {
    traceInfo ("Entering Vca::VcaConnection::detach(O)");
    if (m_pTransportOutbound.clearIf (pTransport)) {
	displayStatistics ();
	if (m_pPeer)
	    m_pPeer->detach (pTransport);
	if (m_pTransportInbound)
	    m_pTransportInbound->onShutdownInProgress ();
	else {
	    if (m_pOffer)
		m_pOffer->decrementOfferCount ();
	    sayQuack (0, "Outgoing Transport Detached");
	}
    }
    traceInfo ("Exiting Vca::VcaConnection::detach(O)");
}

void Vca::VcaConnection::getPeer (VcaSite::Reference &rpPeer) const {
    rpPeer.setTo (m_pPeer);
}

void Vca::VcaConnection::setPeer (VcaSite *pPeer) {
    m_pPeer.setTo (pPeer);
    m_pOffer.setTo (new VcaOffer (pPeer, m_iOffering));

    if (m_pPeerSuspendedSerializer) {
	VcaSerializer::Reference pSuspendedSerializer;
	pSuspendedSerializer.claim (m_pPeerSuspendedSerializer);
	pSuspendedSerializer->resume ();
    }
}

void Vca::VcaConnection::suspend (VcaSerializer *pSerializer) {
    if (!hasAnIncomingTransport ())
	pSerializer->abort ();  // ... plumbing has failed, so give up.
    else {
	pSerializer->suspend ();
	m_pPeerSuspendedSerializer.setTo (pSerializer);
    }
}

void Vca::VcaConnection::supplyOurside (IPeer::Reference &rpOurside) {
    m_pOffer->getRole (rpOurside);
}

void Vca::VcaConnection::onConnect (IPeer *pFarside) {
    m_pPeer->setReflection (pFarside);
    if (m_pTransportInbound)
	m_pPeer->attach (m_pTransportInbound);
    if (m_pTransportOutbound)
	m_pPeer->attach (m_pTransportOutbound);
    m_pFarside.setTo (pFarside);
    sayAck ();
}

/***************************
 ***************************
 *****  Service Query  *****
 ***************************
 ***************************/

Vca::VcaConnection *Vca::VcaConnection::queryService (VInterfaceQuery const &rQuery) {
    if (rQuery.wantsResult ())
	m_pFarside.supply (rQuery, &IPeer::GetRootInterface);
    return this;
}


/************************
 ************************
 *****  Statistics  *****
 ************************
 ************************/

void Vca::VcaConnection::displayStatistics () const {
    if (objectsTrace ()) {
	VString iIncomingLabel;
	iIncomingLabel.printf ("VcaConnection %p Incoming", this);
	m_iIncomingStatistics.display (iIncomingLabel);

	VString iOutgoingLabel;
	iOutgoingLabel.printf ("VcaConnection %p Outgoing", this);
	m_iOutgoingStatistics.display (iOutgoingLabel);
    }
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VcaConnection::TransportStatistics  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

void Vca::VcaConnection::TransportStatistics::display (VString const &rWhere) const {
    if (objectsTrace ()) {
        VString iMessage;
        iMessage.printf ("%s Bytes: %6u, Messages: %u", rWhere.content (), m_cBytes, m_cMessages);
        traceInfo (iMessage);
    }
}
