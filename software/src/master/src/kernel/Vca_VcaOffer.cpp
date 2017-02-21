/*****  Vca::VcaOffer Implementation  *****/

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

#include "Vca_VcaOffer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaSite.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::VcaOffer  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOffer::VcaOffer (
    VcaSite *pSiteObject, Offering const &rOffering
) : m_pIPeer (this), m_pSiteObject (pSiteObject), m_iOffering (rOffering) {
    aggregate (pSiteObject);	// ... provides access to INeedYou and other peer specific interfaces
//  'm_pSiteInterface' is 'const' because it is a structural property of this
//  object; however, initializing it requires that we temporarily the 'const'.
    traceInfo ("Creating VcaOffer");
    pSiteObject->getRole (*const_cast<IPeer2::Reference*>(&m_pSiteInterface));

    incrementOfferCount ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOffer::~VcaOffer () {
    traceInfo ("Destroying VcaOffer");
}

/******************************
 ******************************
 *****  Offer Management  *****
 ******************************
 ******************************/

void Vca::VcaOffer::incrementOfferCount () {
    if (m_bOffered.setIfClear ())
	m_iOffering.incrementOfferCount (this);
}

void Vca::VcaOffer::decrementOfferCount () {
    if (m_bOffered.clearIfSet ())
	m_iOffering.decrementOfferCount (this);
}


/****************************
 ****************************
 *****  IPeer2 Methods  *****
 ****************************
 ****************************/

void Vca::VcaOffer::ReleaseExportEx (
    IPeer2 *pRole, uuid_t const &rObjectSite, VcaSSID const &rObjectSSID,
    U32 cExports, U32 cWeakExports, U32 cMessages
) {
    m_pSiteInterface->ReleaseExportEx (rObjectSite, rObjectSSID, cExports, cWeakExports, cMessages);
}

/****************************
 ****************************
 *****  IPeer1 Methods  *****
 ****************************
 ****************************/

void Vca::VcaOffer::WeakenExport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID) {
    m_pSiteInterface->WeakenExport (rPeerUUID, rObjectSSID);
}

void Vca::VcaOffer::WeakenImport (IPeer1* pRole, uuid_t const& rPeerUUID, VcaSSID const& rObjectSSID) {
    m_pSiteInterface->WeakenImport (rPeerUUID, rObjectSSID);
}

void Vca::VcaOffer::GetProcessInfo (IPeer1* pRole, uuid_t const& rPeerUUID, IProcessInfoSink* pInfoSink) {
    m_pSiteInterface->GetProcessInfo (rPeerUUID, pInfoSink);
}


/***************************
 ***************************
 *****  IPeer Methods  *****
 ***************************
 ***************************/

void Vca::VcaOffer::getRole (IPeer1::Reference &rpRole) {
    IPeer2::Reference pRoleX;
    getRole (pRoleX);
    rpRole.setTo (pRoleX);
}

void Vca::VcaOffer::getRole (IPeer::Reference &rpRole) {
    IPeer2::Reference pRoleX;
    getRole (pRoleX);
    rpRole.setTo (pRoleX);
}

void Vca::VcaOffer::SinkInterface (IPeer *pRole, IVUnknown* pUnknown) {
    m_pSiteInterface->SinkInterface (pUnknown);
}

void Vca::VcaOffer::ReleaseExport (
    IPeer*		pRole,
    uuid_t const&	rObjectSite,
    VcaSSID const&	rObjectSSID,
    U32			cExports,
    U32			cMessages
) {
    m_pSiteInterface->ReleaseExport (rObjectSite, rObjectSSID, cExports, cMessages);
}

void Vca::VcaOffer::ReleaseImport (
    IPeer *pRole, uuid_t const &rObjectSite, VcaSSID const &rObjectSSID
) {
    m_pSiteInterface->ReleaseImport (rObjectSite, rObjectSSID);
}

void Vca::VcaOffer::FakeExportTo (
    IPeer*		pRole,
    uuid_t const&	rExportPeer,
    uuid_t const&	rObjectSite,
    VcaSSID const&	rObjectSSID
) {
    m_pSiteInterface->FakeExportTo (rExportPeer, rObjectSite, rObjectSSID);
}

void Vca::VcaOffer::FakeImportFrom (
    IPeer*		pRole,
    uuid_t const&	rImportPeer, 
    uuid_t const&	rObjectSite,
    VcaSSID const&	rObjectSSID
) {
    m_pSiteInterface->FakeImportFrom (rImportPeer, rObjectSite, rObjectSSID);
}

void Vca::VcaOffer::GatherConnectionData (
    IPeer*		    pRole,
    IConnectionRequest*	    pRequest,
    VPeerDataArray const&   rForwardData,
    VPeerDataArray const&   rReverseData,
    uuid_t const&	    targetPeer,
    bool		    bForward
) {
    m_pSiteInterface->GatherConnectionData (
	pRequest, rForwardData, rReverseData, targetPeer, bForward
    );
}

void Vca::VcaOffer::CreateConnection (
    IPeer *pRole, VPeerDataArray const &peerDataArray, uuid_t const &targetUUID, uuid_t const &finalUUID
) {
    m_pSiteInterface->CreateConnection (peerDataArray, targetUUID, finalUUID);
}

void Vca::VcaOffer::GetLocalInterfaceFor (
    IPeer *pRole, VcaSite *pPeer, IVReceiver<IPeer*>* pReceiver
) {
    m_pSiteInterface->GetLocalInterfaceFor (pPeer, pReceiver);
}	

void Vca::VcaOffer::OnDone (IPeer *pRole) {
    m_pSiteInterface->OnDone ();
}

void Vca::VcaOffer::MakeConnection (
    IPeer *pRole, IVReceiver<IConnection*> *pClient, VString const &rDestination
) {
    m_pSiteInterface->MakeConnection (pClient, rDestination);
}

void Vca::VcaOffer::GetRootInterface (
    IPeer *pRole, VTypeInfo *pInterfaceType, IVReceiver<IVUnknown*>* pInterfaceReceiver
) {
    m_iOffering.supplyInterface (pInterfaceReceiver, pInterfaceType);
}

void Vca::VcaOffer::SetRemoteReflectionFor (
    IPeer *pRole, uuid_t const &rSiteUUID, IPeer *pReflection
) {
    m_pSiteInterface->SetRemoteReflectionFor (rSiteUUID, pReflection);
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VcaOffer::Offering  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOffer::Offering::Offering (VRolePlayer *pObject, Tracker *pTracker) : m_pObject (pObject), m_pTracker (pTracker) {
}

Vca::VcaOffer::Offering::Offering (VRolePlayer *pObject) : m_pObject (pObject), m_pTracker (new Tracker ()) {
}

Vca::VcaOffer::Offering::Offering (Tracker *pTracker) : m_pTracker (pTracker) {
}

Vca::VcaOffer::Offering::Offering (ThisClass const &rOther) : m_pObject (rOther.m_pObject), m_pTracker (rOther.m_pTracker) {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VcaOffer::Offering::supplyInterface (IVReceiver<IVUnknown*>* pInterfaceReceiver, VTypeInfo *pInterfaceType) const {
    if (m_pObject)
	m_pObject->supplyInterface (pInterfaceReceiver, pInterfaceType);
    else if (pInterfaceReceiver) {
	pInterfaceReceiver->OnError (0, "No Object");
    }
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::VcaOffer::Offering::Tracker  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOffer::Offering::Tracker::Tracker () : m_cOffers (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOffer::Offering::Tracker::~Tracker () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VcaOffer::Offering::Tracker::onIncrement (VcaOffer* pOffer) {
}

void Vca::VcaOffer::Offering::Tracker::onDecrement (VcaOffer* pOffer) {
}

void Vca::VcaOffer::Offering::Tracker::incrementOfferCount (VcaOffer* pOffer) {
   m_cOffers++;
   onIncrement (pOffer);
}

void Vca::VcaOffer::Offering::Tracker::decrementOfferCount (VcaOffer* pOffer) {
   m_cOffers--;
   onDecrement (pOffer);
}
