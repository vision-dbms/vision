/*****  Vca::VcaSite Implementation  *****/

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

#include "Vca_VcaSite.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_VcaPeer.h"

#include "Vca_VConnectionFactory.h"


/**************************
 **************************
 *****                *****
 *****  Vca::VcaSite  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

unsigned int Vca::VcaSite::m_iCreatedCount = 0;
unsigned int Vca::VcaSite::m_iCurrentCount = 0;

Vca::VcaSite::VcaSite (
    VcaSite* pParent, Characteristics const &rCharacteristics
) : m_pParent (pParent), m_iCharacteristics (rCharacteristics), m_pIPeer (this), m_iReleaseMessageSequenceNumber(0){
    m_iCreatedCount++;
    m_iCurrentCount++;
}

Vca::VcaSite::VcaSite () : m_pParent (this), m_pIPeer (this), m_iReleaseMessageSequenceNumber (0){
    unsigned int xInstance;
    m_iSiteSet.Insert (uuid (), xInstance);
    m_iSiteSet[xInstance].setTo (this);

    m_iCreatedCount++;
    m_iCurrentCount++;

}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaSite::~VcaSite () {
    m_iCurrentCount--;
}


/****************************
 ****************************
 *****  IPeer2 Methods  *****
 ****************************
 ****************************/

/****************************************************************************
 *  Synopsis:
 *	ReleaseExport (rObjectSite, rObjectSSID, cExports, cMessages
 *
 *  Arguments:
 *	this		- the site releasing the export.
 *	rObjectSite	- the exported object's site.
 *	rObjectSSID	- the exported object's ssid.
 *	cExports	- the number of 'exports' being released.
 *	cMessages	- the number of messages being 'released'.
 *
 *  Notes:
 *
 ****************************************************************************/
void Vca::VcaSite::ReleaseExportEx (
    VMessageHolder<IPeer_Ex2> const& rMessage, uuid_t const &rObjectSite, 
    VcaSSID const &rObjectSSID, U32 cExports, U32 cWeakExports, U32 cMessages
) {
    VcaOID::Reference pOID;
    if (getObject (pOID, rObjectSSID, rObjectSite))
	deleteExportOf (rMessage, pOID, cExports, cWeakExports, cMessages); 
}


/****************************
 ****************************
 *****  IPeer1 Methods  *****
 ****************************
 ****************************/

void Vca::VcaSite::getRole (IPeer1::Reference &rpRole) {
    IPeer2::Reference pRoleX;
    getRole (pRoleX);
    rpRole.setTo (pRoleX);
}

void Vca::VcaSite::WeakenExport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID) {
    VcaOID::Reference pOID;
    if (getObject (pOID, rObjectSSID, rPeerUUID)) {
	weakenExportOf (pOID);
    }
}

void Vca::VcaSite::WeakenImport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID) {
    VcaOID::Reference pOID;
    if (getObject (pOID, rObjectSSID, rPeerUUID)) {
	weakenImportOf (pOID);
    }
}

void Vca::VcaSite::GetProcessInfo (IPeer1* pRole, uuid_t const& rPeerUUID, IProcessInfoSink* pInfoSink) {
    if (VcaSite* const pTargetSite = sibling (rPeerUUID))
	pTargetSite->supplyProcessInfo (pInfoSink);
    else if (pInfoSink)
	pInfoSink->OnError (0, "Unknown Peer");
}


/***************************
 ***************************
 *****  IPeer Methods  *****
 ***************************
 ***************************/

void Vca::VcaSite::getRole (IPeer::Reference &rpRole) {
    IPeer1::Reference pRoleX;
    getRole (pRoleX);
    rpRole.setTo (pRoleX);
}

void Vca::VcaSite::SinkInterface (IPeer *pRole, IVUnknown* pUnknown) {
//  That's right, do nothing !!!!
}

/****************************************************************************
 *  Synopsis:
 *	ReleaseExport (rObjectSite, rObjectSSID, cExports, cMessages
 *
 *  Arguments:
 *	this		- the site releasing the export.
 *	rObjectSite	- the exported object's site.
 *	rObjectSSID	- the exported object's ssid.
 *	cExports	- the number of 'exports' being released.
 *	cMessages	- the number of messages being 'released'.
 *
 *  Notes:
 *
 ****************************************************************************/
void Vca::VcaSite::ReleaseExport (
    VMessageHolder<IPeer_Ex2> const& rMessage, uuid_t const &rObjectSite, 
    VcaSSID const &rObjectSSID, U32 cExports, U32 cMessages
) {
    VcaOID::Reference pOID;
    if (getObject (pOID, rObjectSSID, rObjectSite))
	deleteExportOf (rMessage, pOID, cExports, 0, cMessages);

    // In Vca interactions involving more than two sites, this is better
    // characterized as "late" than "bogus" ...

    //    else /* raiseApplicationException */ display (
    //	"Bogus export release: Not Exported"
    //    );
}

/****************************************************************************
 *  Synopsis:
 *	ReleaseImport (rObjectSite, rObjectSSID, cExports, cMessages
 *
 *  Arguments:
 *	this		- the site releasing the import.
 *	rObjectSite	- the imported object's site.
 *	rObjectSSID	- the imported object's ssid.
 *
 *  Notes:
 *	None here.
 ****************************************************************************/
void Vca::VcaSite::ReleaseImport (
    IPeer *pRole, uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
) {
    VcaOID::Reference pOID;
    if (getObject (pOID, rObjectSSID, rObjectSite))
	deleteImportOf (pOID);

    // In Vca interactions involving more than two sites, this is better
    // characterized as "late" than "bogus" ...

    //     else /* raiseApplicationException */ display (
    // 	"Bogus import release: Not imported"
    //     );
}

/*********************************************************************
 * Note: The Fake & SetRemoteReflectionFor operations are not
 * performed on the direct peer, rather we determine the peer from the 
 * input peer argument.....
 **********************************************************************/

/****************************************************************************
 * Routine: FakeExportTo
 * Operation:
 *   Fakes an export after checking whether the object being asked to 
 * be exported is a valid one
 * Note (refacilitation):  
 *	For client to client fake export, the function checks to see whether
 * the proxy is about to be facilitated to the peer which is doing this
 * Fake Export. If so tries to refacilitate the proxy to a new target peer
 ****************************************************************************/

void Vca::VcaSite::FakeExportTo (
    IPeer *pRole,
    uuid_t const &rExportPeer, uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
) {
    VcaSite *const pExportPeer = sibling (rExportPeer);
    VcaOID::Reference pOID;
    if (pExportPeer && getObject (pOID, rObjectSSID, rObjectSite)) {
	pExportPeer->createExportOf (pOID, false);

#if 0
    //  refacilitation
	pOID->refacilitateTo (pExportPeer, this);
#endif
    }
}

/****************************************************************************
 * Routine: FakeImportFrom
 * Operation:
 *	Fakes an import on an object after verifying the validity of the 
 * object
 * Note (refacilitation):
 *	Also checks whether the proxy which is being fake imported is 
 * about to be facilitated with "this" peer. If so refacilitation with
 * a new source peer is initiated.
 ****************************************************************************/

void Vca::VcaSite::FakeImportFrom (
    IPeer *pRole, uuid_t const& rImportPeerUUID, 
    uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
) {
    VcaSite *const pImportPeer = sibling (rImportPeerUUID);
    VcaOID::Reference pOID;
    if (pImportPeer && getObject (pOID, rObjectSSID, rObjectSite)) {
	pImportPeer->traceInfo ("Faking Import");
	pImportPeer->createImportOf (pOID, false);
#if 0
	if (isWaitingToFacilitate (pOID))
	    refacilitateProxyWith (pOID, pImportPeer);
#endif
    }
}

void Vca::VcaSite::GatherConnectionData (
    IPeer*		    pRole,
    IConnectionRequest*	    pRequest,
    VPeerDataArray const&   rForwardData,
    VPeerDataArray const&   rReverseData,
    uuid_t const&	    targetPeer,
    bool		    bForward
) {
#if 0
    GatherConnectionData (pRequest, rForwardData, rReverseData, targetPeer, bForward);
#endif
}

void Vca::VcaSite::CreateConnection (
    IPeer *pRole, VPeerDataArray const &peerDataArray, uuid_t const &targetUUID, uuid_t const &finalUUID
) {
#if 0
    CreateConnection (peerDataArray, targetUUID, finalUUID);
#endif
}

void Vca::VcaSite::GetLocalInterfaceFor (
    IPeer *pRole, VcaSite *pPeer, IVReceiver<IPeer*>* pReceiver
) {
    if (pReceiver) {
	IPeer::Reference pInterface;
	pPeer->getRole (pInterface);
	pReceiver->OnData (pInterface);
    }
}	

/*----------------------------------------------------------------------------*
 *  OnDone is the last message a peer sends prior to exiting and is a
 *  request that we terminate our use of and connections to the peer.
 *----------------------------------------------------------------------------*/
void Vca::VcaSite::OnDone (VMessageHolder<IPeer_Ex2> const& rMessage) {
    Vca::VcaPeer* pPeer = dynamic_cast<Vca::VcaPeer*>(this);
    if (m_iReleaseMessageSequenceNumber > 0 && pPeer) {
#ifdef TracingVcaMemoryLeaks
	log("PoolMemoryLeak VcaSite::OnDone reported message increment by %u", 
	    rMessage.message()->sequenceNumber() - m_iReleaseMessageSequenceNumber + 1); 
#endif
	// update the number of ReleaseExportEx messages that were involved 
	// in tearing down this connection.  Their message count will not have
	// been kept up to date once the remote reflection was itself Release(ExportEx)ed
	rMessage.vinterface()->oid()->deleteExportTo (pPeer, 0, 0,
	    rMessage.message()->sequenceNumber() - m_iReleaseMessageSequenceNumber + 1);
    }

    markDefunct ();
}


void Vca::VcaSite::MakeConnection (
    IPeer *pRole, IVReceiver<IConnection*> *pClient, VString const &rDestination
) {
    VConnectionFactory::Supply (pClient, rDestination, Default::TcpNoDelaySetting ());
}

void Vca::VcaSite::GetRootInterface (
    IPeer *pRole, VTypeInfo *pInterfaceType, IVReceiver<IVUnknown*>* pInterfaceReceiver
) {
    if (pInterfaceReceiver)
	pInterfaceReceiver->OnData (0);
}

void Vca::VcaSite::SetRemoteReflectionFor (
    IPeer *pRole, uuid_t const &rSiteUUID, IPeer *pReflection
) {
    if (VcaSite *const pSite = sibling (rSiteUUID))
	pSite->setReflection (pReflection);
}


/**************************
 **************************
 *****  Connectivity  *****
 **************************
 **************************/


char const *Vca::VcaSite::DescriptionFor (ConnectKind xConnectKind) {
    switch (xConnectKind) {
    case ConnectKind_RouteTable:
	return "RouteTable";
    case ConnectKind_RouteTablePlus:
	return "RouteTablePlus";
    case ConnectKind_ConnectedOthers:
	return "ConnectedOthers";
    case ConnectKind_ConnectedSiblings:
	return "ConnectedSiblings";
    case ConnectKind_Siblings:
	return "Siblings";
    case ConnectKind_None:
	return "None";
    }
    return "Unknown";
}

void Vca::VcaSite::processCallbackRequest (
    IAckReceiver*	pAckReceiver,
    ICallbackReceiver*	pCallbackReceiver,
    IPassiveCall*	pCallbackRequest,
    VString const&	rCallbackAddress,
    uuid_t const&	rCallbackID,
    ConnectKind         xConnectKind
) {
    processCallbackRequest_(
	pAckReceiver, pCallbackReceiver, pCallbackRequest, rCallbackAddress, rCallbackID, xConnectKind
    );
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VcaSite::markDefunct () {
    if (m_bDefunct.setIfClear ()) {
	if (defaultLogger().isActive ()) {
	    VString iMyName; {
		VkUUIDHolder const iUUID (uuid ());
		iUUID.GetString (iMyName);
	    }
	    defaultLogger().printf ("+++ %s: marked defunct%s.\n", iMyName.content (), isSelf () ? ":-(That's Me)-:" : "");
	}
	markDefunct_();
    }
}

void Vca::VcaSite::markDefunct_() {
    for (unsigned int i=0; i<m_iSiteSet.cardinality (); i++) {
	m_iSiteSet[i]->traceInfo ("shutdownAll: shutdown");
	m_iSiteSet[i]->onShutdown ();
    }
}

/******************************************************************************
 *  Routine: onShutdown
 *	During a graceful shutdown, the exiting process calls this member for
 *  each site in the site set to report the impending demise of the process.
 *****************************************************************************/
void Vca::VcaSite::onShutdown () {
}

/***************************
 ***************************
 *****  Object Access  *****
 ***************************
 ***************************/

bool Vca::VcaSite::getObject (VcaOID::Reference& rpOID, VcaSSID const &rObjectSSID) const {
    unsigned int xObject;
    if (m_iObjectTable.Locate (rObjectSSID, xObject))
	rpOID.setTo (m_iObjectTable[xObject]);
    else
	rpOID.clear ();
    return rpOID.isntNil ();
}

bool Vca::VcaSite::getObject (
    VcaOID::Reference &rpOID, VcaSSID const &rObjectSSID, uuid_t const &rObjectSite
) {
    VcaSite* const pSite = sibling (rObjectSite);
    return pSite && pSite->getObject (rpOID, rObjectSSID);
}

/***************************
 ***************************
 *****  Object Detach  *****
 ***************************
 ***************************/

void Vca::VcaSite::detach (VcaOID *pOID) {
    m_iObjectTable.Delete (pOID->objectSSID ());
}

/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void Vca::VcaSite::incrementExportCount (bool bWeak) {
    if (bWeak)
	incrementWeakExportCount ();
    else if (0 == m_cExports++)
	onExportCountWasZero ();
}

void Vca::VcaSite::decrementExportCount () {
    if (m_cExports.decrementIsZero ())
	onExportCountIsZero ();
}

void Vca::VcaSite::onExportCountIsZero () {
    m_pParent->decrementExportCount ();
}

void Vca::VcaSite::onExportCountWasZero () {
    m_pParent->incrementExportCount (false);
}

/***************************
 ***************************
 *****  Object Import  *****
 ***************************
 ***************************/

void Vca::VcaSite::incrementImportCount (bool bWeak) {
    if (bWeak)
	incrementWeakImportCount ();
    else if (0 == m_cImports++)
	onImportCountWasZero ();
}

void Vca::VcaSite::decrementImportCount () {
    if (m_cImports.decrementIsZero ())
	onImportCountIsZero ();
}

void Vca::VcaSite::onImportCountIsZero () {
    m_pParent->decrementImportCount ();
}

void Vca::VcaSite::onImportCountWasZero () {
    m_pParent->incrementImportCount (false);
}


/*********************
 *********************
 *****  Routing  *****
 *********************
 *********************/

void Vca::VcaSite::getLocalPeerData (VPeerData &rLocalData) {
    rLocalData.setPeerUUID (uuid ());
    rLocalData.deleteAllConnectedPeers();
    for (unsigned int i=0; i < m_iSiteSet.cardinality(); i++) {
	VcaSite *const pSite = m_iSiteSet[i];
	if (pSite->isntSelf () && pSite->isConnected ()) 
	    rLocalData.addConnectedPeer (pSite->uuid());
    }
}

void Vca::VcaSite::setRoutingInformation (
    VcaSite *pRoutingSite, VcaRouteStatistics const &rStatistics
) {
}


/*******************
 *******************
 *****  Sites  *****
 *******************
 *******************/

/**********************
 *****  Children  *****
 **********************/

Vca::VcaSite::SiteSet const &Vca::VcaSite::children () const {
    return m_iSiteSet;
}

Vca::VcaSite *Vca::VcaSite::child (Characteristics const &rCharacteristics) {
    unsigned int xInstance;
    if (m_iSiteSet.Insert (rCharacteristics, xInstance)) {
	m_iSiteSet[xInstance] = new VcaPeer (this, rCharacteristics);

#ifdef TracingVcaMemoryLeaks
	log("PoolMemoryLeak current: %u vs. created: %u vs. site set: %u", 
	    m_iCurrentCount, m_iCreatedCount, m_iSiteSet.cardinality());
#endif	    
    }
    return m_iSiteSet[xInstance];
}

Vca::VcaSite *Vca::VcaSite::child (uuid_t const &rUUID) {
    unsigned int xInstance;
    return m_iSiteSet.Locate (rUUID, xInstance) ? m_iSiteSet[xInstance].referent () : NilOf (VcaSite*);
}

Vca::VcaSite *Vca::VcaSite::child (char const *pUUID) {
    VkUUIDHolder iUUID (pUUID);
    return child (iUUID);
}

/**********************
 *****  Siblings  *****
 **********************/

Vca::VcaSite::SiteSet const &Vca::VcaSite::siblings () const {
    return m_pParent->children ();
}

Vca::VcaSite *Vca::VcaSite::sibling (Characteristics const &rCharacteristics) {
    return m_pParent->child (rCharacteristics);
}

Vca::VcaSite *Vca::VcaSite::sibling (uuid_t const &rUUID) {
    return m_pParent->child (rUUID);
}

Vca::VcaSite *Vca::VcaSite::sibling (char const *pUUID) {
    return m_pParent->child (pUUID);
}

/**********************
 *****  Unstance  *****
 **********************/

bool Vca::VcaSite::unstance (uuid_t const &rUUID) {
    return m_iSiteSet.Delete (rUUID);
}

bool Vca::VcaSite::unstance () {
    return m_pParent->unstance (uuid ());
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaSite::displayUUID () const {
    VString sUUID; {
	VkUUIDHolder const iUUID (uuid ());
	iUUID.GetString (sUUID);
    }
    display (" %s", sUUID.content ());
}

void Vca::VcaSite::getUUID (VString &rResult) const {
    VString sTemp;
    VkUUID::GetString (sTemp, uuid());
    rResult << sTemp;
}

void Vca::VcaSite::displayInfo () const {
    BaseClass::displayInfo ();
    displayUUID ();
}

void Vca::VcaSite::getInfo (VString &rResult, const VString &rPrefix) const {
    BaseClass::getInfo (rResult, rPrefix);
    rResult << ": ";
    getUUID (rResult);
}

void Vca::VcaSite::displayObjectTable () const {
    displayInfo (" Object Table:");

    for (unsigned int xObject = 0; xObject < m_iObjectTable.cardinality (); xObject++)
	m_iObjectTable[xObject]->displayInfo ();
}

void Vca::VcaSite::getObjectTable (VString &rResult, const VString &rPrefix) const {
    bool bNeedsNewline = false;
    for (unsigned int xObject = 0; xObject < m_iObjectTable.cardinality (); xObject++) {
        if (bNeedsNewline) rResult << "\n";
        bNeedsNewline = true;
	m_iObjectTable[xObject]->getInfo (rResult, rPrefix);
    }
}

void Vca::VcaSite::displayPendingMessageCounts () const {
}

void Vca::VcaSite::displayRoutes () const {
}

void Vca::VcaSite::displayInfoForAll () {
    for (unsigned int i = 0; i < m_iSiteSet.cardinality (); i++) {
	m_iSiteSet[i]->displayInfo ();
    }
    display ("\n");
}

void Vca::VcaSite::getInfoForAll (VString &rResult, const VString &rPrefix) {
    bool bNeedsNewline = false;
    for (unsigned int i = 0; i < m_iSiteSet.cardinality (); i++) {
        if (bNeedsNewline) rResult << "\n";
        bNeedsNewline = true;
	m_iSiteSet[i]->getInfo (rResult, rPrefix);
    }
}

void Vca::VcaSite::displayObjectTableForAll () {
    for (unsigned int i = 0; i < m_iSiteSet.cardinality (); i++) {
	m_iSiteSet[i]->displayObjectTable ();
    }
    display ("\n");
}

void Vca::VcaSite::getObjectTableForAll (VString &rResult, const VString &rPrefix) {
    bool bNeedsNewline = false;
    for (unsigned int i = 0; i < m_iSiteSet.cardinality (); i++) {
        if (bNeedsNewline) rResult << "\n";
        bNeedsNewline = true;
	m_iSiteSet[i]->getObjectTable (rResult, rPrefix);
    }
}

void Vca::VcaSite::displayPendingMessageCountsForAll () {
    for (unsigned int i=0; i < m_iSiteSet.cardinality (); i++) {
	m_iSiteSet[i]->displayPendingMessageCounts ();
    } 
    display ("\n");
}
