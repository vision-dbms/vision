/*****  VcaProxyFacilitator Implementation  *****/

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

#include "Vca_VcaProxyFacilitator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "VReceiver.h"
#include "Vca_IPeer.h"
#include "Vca_VcaPeer.h"

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaProxyFacilitator::VcaProxyFacilitator (
    VcaPeer *pSourcePeer, VcaPeer *pTargetPeer
) : m_pSourcePeer (pSourcePeer), m_pTargetPeer (pTargetPeer) {
    startup ();
}

Vca::VcaProxyFacilitator::VcaProxyFacilitator (
    VcaPeer *pSourcePeer, VcaPeer *pTargetPeer,
    IPeer *pSourcePeerReflection, IPeer *pTargetPeerReflection
) : m_pSourcePeer (pSourcePeer)
  , m_pTargetPeer (pTargetPeer)
  , m_pSourcePeerReflection (pSourcePeerReflection)
  , m_pTargetPeerReflection (pTargetPeerReflection) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaProxyFacilitator::~VcaProxyFacilitator () {
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VcaProxyFacilitator::setSourcePeerReflection (IPeer *pPeer) {
    m_pSourcePeerReflection.setTo (pPeer);
}

void Vca::VcaProxyFacilitator::setTargetPeerReflection (IPeer *pPeer) {
    m_pTargetPeerReflection.setTo (pPeer);
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

/****************************************************************************
 * Routine: startup
 * Operation:
 *	This method does the intial startup process, for a 
 * VcaFacilitator object to be used. It sends messages to both the source
 * as well as the target peer, with the information about each other, 
 * also requests local interfaces for the images created by them. 
****************************************************************************/

void Vca::VcaProxyFacilitator::startup () {
    if (!reflectionsAreValid ()) {
	VReference<IPeerReceiver> pReceiver1 (
	    new IPeerReceiver (this, &ThisClass::onTargetPeerReflection)
	);
	VReference<IPeerReceiver> pReceiver2 (
	    new IPeerReceiver (this, &ThisClass::onSourcePeerReflection)
	);

	traceInfo ("\nSending Peers for exchanging");
	m_pSourcePeer->getRemoteLocalInterfaceFor (m_pTargetPeer, pReceiver1);
	m_pTargetPeer->getRemoteLocalInterfaceFor (m_pSourcePeer, pReceiver2);
    }
}

/****************************************************************************
 * Routine: onSourcePeerReflection
 * Operation:
 *	On getting the request for constructing an image for source peer, the 
 * target peer creates a source peer image and returns a local interface 
 * pointer by calling this routine. This method checks to see whether the other 
 * reflection has also arrived if so triggers the exchange of the 
 * reflections obtained
 * Precondition: pIPeer, the reflection returned is a valid non-null pointer
****************************************************************************/

void Vca::VcaProxyFacilitator::onSourcePeerReflection (
    IPeerReceiver *pReceiver, IPeer *pIPeer
) {
    m_pSourcePeerReflection.setTo (pIPeer);
    if (reflectionsAreValid ())
	onBothReflections ();
}

void Vca::VcaProxyFacilitator::onTargetPeerReflection (
    IPeerReceiver *pReceiver, IPeer *pIPeer
) {
    m_pTargetPeerReflection.setTo (pIPeer);
    if (reflectionsAreValid ())
	onBothReflections ();
}

/****************************************************************************
 * Routine: onBothReflections
 * Operation:
 *	Once both reflections are obtained, by this facilitator it needs to
 * send messages to both the source and target peer indicating their reflections
 * at the other end. This method completes the phase of creating a virtual
 * plumbing between the source and the target peer by the facilitator object.
 * Now it can start the facilitations.
****************************************************************************/

void Vca::VcaProxyFacilitator::onBothReflections () {
    traceInfo ("\nExchanging Reflections");

    m_pSourcePeer->setRemoteRemoteReflectionFor (m_pTargetPeer, m_pSourcePeerReflection);
    m_pTargetPeer->setRemoteRemoteReflectionFor (m_pSourcePeer, m_pTargetPeerReflection);

    delegateReflections ();
    delegatePendingProxies ();
}

/****************************************************************************
 * Routine: delegateReflections
 * Operation:
 *    This method delegates the reflections obtained from the source and the
 *   target peer. The reflections are being referenced from this facilitator
 *   object. So they wont be facilitated automatically as there is an internal
 *   usage. But to relieve this intermediary peer from serving as intermediary
 *   for the reflections we delegate the reflections also as part of the 
 *   startup process. 
 *   Out of the two reflections, the TargetPeer reflection can be facilitated
 *   by the current facilitator, but the SourcePeer reflection has to be
 *   facilitated by creating a new facilitator object which facilitates 
 *   proxies going from the Target peer to the Source peer.
 ****************************************************************************/

void Vca::VcaProxyFacilitator::delegateReflections () {
    traceInfo ("\nDelegating Reflection1");
    delegateProxy (m_pTargetPeerReflection->oidr ());

    traceInfo ("\nDelegating Reflection2");
    VcaProxyFacilitator *pFacilitator;
    pFacilitator = m_pTargetPeer->createProxyFacilitatorFor (
	m_pSourcePeer, m_pTargetPeerReflection, m_pSourcePeerReflection
    );
    pFacilitator->processProxy (m_pSourcePeerReflection->oidr ());
}

void Vca::VcaProxyFacilitator::delegatePendingProxies () {
    if (!reflectionsAreValid ())
	return; 

    Iterator iterator (m_iPendingProxySet);
    while (iterator.isNotAtEnd ()) {
	VcaOIDR *pOIDR = *iterator;

	//  remove from pending list and then delegate...
	iterator.Delete ();
	pOIDR->deleteFacilitationTo (m_pTargetPeer);
	delegateProxy (pOIDR);
    }
}

/****************************************************************************
 * Routine: processProxy
 * Operation: 
 *    When a proxy is being requested to be processed, first a  check is made
 * to determine whether the reflection are valid. If so then the proxy is 
 * directly delegated, else it is inserted into the pending proxy set.
****************************************************************************/

void Vca::VcaProxyFacilitator::processProxy (VcaOIDR *pOIDR) {
    if (reflectionsAreValid ()) 
	delegateProxy (pOIDR);
    else 
	queueProxy (pOIDR);
}

void Vca::VcaProxyFacilitator::queueProxy (VcaOIDR *pOIDR) {
    m_iPendingProxySet.Insert (pOIDR);
    pOIDR->addFacilitationPeers (m_pTargetPeer, m_pSourcePeer);
}

/****************************************************************************
 * Routine: delegateProxy
 * Operation: 
 *  This method performs the work of eliminating this process as a middle man
 *  in an import/export relationship between two adjacent peers.  This method
 *  operates by manipulating the peer models of its neighbors.  It is called
 *  after this facilitator has constructed appropriate models of the one-hop
 *  removed peers at those neighbors and performs its work in three steps:
 *  .  
 *   1. Send a "Fake Export" message to the upstream object exporter
 *	requesting that it 'export' the object to the downstream importer.
 *   2. Send a "Fake Import" message to the downstream object importer
 *	requesting that it 'import' the object from the upstream exporter.
 *   3. Send a "ReleaseImport" message to the downstream importer requesting
 *	that it stop importing the object from this process.  This message,
 *	when processed by the downstream peer, will eventually result in a
 *	ReleaseExport message from the downstream peer absolving this process
 *	of the export responsibilities it is trying to free itself of.
 *
****************************************************************************/

void Vca::VcaProxyFacilitator::delegateProxy (VcaOIDR *pOIDR) {
    traceInfo ("\nDelegating Proxy. Calling FakeExport, FakeImport,deleteRemoteImport");
   
    m_pSourcePeer->fakeRemoteExportTo	(pOIDR, m_pTargetPeer);
    m_pTargetPeer->fakeRemoteImportFrom	(pOIDR, m_pSourcePeer);
    m_pTargetPeer->deleteRemoteImportOf	(pOIDR);
}
