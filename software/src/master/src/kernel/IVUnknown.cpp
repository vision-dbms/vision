/*****  Vca_IVUnknown Implementation  *****/

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

#define   Vca_IVUnknown
#include "IVUnknown.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaPeer.h"
#include "Vca_VcaSelf.h"
#include "Vca_VcaSerializer.h"

#include "Vca_VTrigger.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VcaSerializerForInterface  *****
 *****                                  *****
 ********************************************
 ********************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

// DEFINE_VIRTUAL_RTT(Vca::VcaSerializerForInterface);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

// Initially the object to be sent or received is set as unknown kind 

Vca::VcaSerializerForInterface::VcaSerializerForInterface (
    VcaSerializer *pCaller, bool bWeak
) : VcaSerializer (pCaller), m_bWeak (bWeak), m_xObjectSite (ObjectSite_Unknown) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaSerializerForInterface::~VcaSerializerForInterface () {
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializerForInterface::doData (Sequencer *pSequencer) {
    switch (m_xObjectSite) {
    case ObjectSite_Unknown:
	break;
    case ObjectSite_Nil:
	//  ObjectSite_Nil{} ()
	clearSequencer ();
	return;
    case ObjectSite_Receiver:
	//  ObjectSite_Receiver{SSID yourSSID} ()
	clearSequencer ();
	break;
    case ObjectSite_Sender:
	//  ObjectSite_Sender{SSID mySSID} (VTypeInfo *pObjectType)
	pSequencer->setActionTo (&ThisClass::doType);
	break;
    case ObjectSite_Other:
	//  ObjectSite_Other{SSID mySSID} (VcaSite *pObjectSite, VTypeInfo *pObjectType)
	pSequencer->setActionTo (&ThisClass::doSite);
	break;
    default:
	clearSequencer ();
	break;
    }
    transferData ();
}

void Vca::VcaSerializerForInterface::doSite (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doType);
    start (this, New_VcaSerializer_(this, m_pObjectSite));
}

void Vca::VcaSerializerForInterface::doType (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, New_VcaSerializer_(this, m_pObjectType));
}


/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/**************************
 *----  Localization  ----*
 **************************/

void Vca::VcaSerializerForInterface::localizeData () {
    if (objectKindIsKnown ())
	peer ()->localize (m_iObjectSSID);
}

/*****************
 *----  Get  ----*
 *****************/

void Vca::VcaSerializerForInterface::getData () {
    if (objectKindIsKnown ()) 
	get (&m_iObjectSSID, sizeof (m_iObjectSSID));
    else 
	get (&m_xObjectSite, sizeof (m_xObjectSite));
}

/*****************
 *----  Put  ----*
 *****************/

void Vca::VcaSerializerForInterface::putData () {
    if (objectKindIsKnown ()) 
	put (&m_iObjectSSID, sizeof (m_iObjectSSID));
    else {
	IVUnknown *pUnknown = interface_();
	if (IsNil (pUnknown))
	    m_xObjectSite = ObjectSite_Nil;
	else {
	    VcaSite *pTransportPeer = peer ();
	    VcaOIDR *pOIDR = pUnknown->oidr ();
	    if (pOIDR) {				// ... object implemented at another site:
		m_iObjectSSID = pOIDR->objectSSID ();
		m_pObjectSite = pOIDR->objectSite ();
		m_pObjectType = pOIDR->objectType ();

		if (pTransportPeer == m_pObjectSite)	// ... object implemented at this transport's peer:
		//  ObjectSite_Receiver{SSID objectSSID} ()
		    m_xObjectSite = ObjectSite_Receiver;
		else {					// ... object implemented elsewhere:
		//  ObjectSite_Other{SSID objectSSID} (
		//	VcaSite *pObjectSite, VTypeInfo *pObjectType
		//  )
		    pTransportPeer->createExportOf (pOIDR, m_bWeak);
		    m_xObjectSite = ObjectSite_Other;
		}
	    } else {					// ... object implemented here:
		//  ObjectSite_Sender{SSID objectSSID} (VTypeInfo *pObjectType)
		pTransportPeer->createExportOf (pUnknown, m_bWeak);
		pUnknown->getSSID (m_iObjectSSID);

		m_pObjectType = pUnknown->typeInfo_();
		m_xObjectSite = ObjectSite_Sender;
	    }
	}
	put (&m_xObjectSite, sizeof (m_xObjectSite));
    }
}


/********************
 *----  Wrapup  ----*
 ********************/

void Vca::VcaSerializerForInterface::wrapupIncomingSerialization () {
    VcaOID::Reference pOID;
    switch (m_xObjectSite) {
    case ObjectSite_Nil:
	break;

    case ObjectSite_Receiver:  // ... one of my local objects:
	self ()->receive (pOID, m_iObjectSSID, peer (), m_bWeak);
	break;

    case ObjectSite_Sender:   // ... one of the transport peer's local objects:
	peer ()->receive (pOID, m_iObjectSSID, m_pObjectType, m_bWeak);
	break;

    case ObjectSite_Other:  // ... somebody else's object:
	m_pObjectSite->receive (pOID, m_iObjectSSID, m_pObjectType, peer (), m_bWeak);
	break;

    default: {
	    RTTI iRTTI (this);
	    // Is this really worth dying for???
	    //	    raiseUnimplementedOperationException (
	    log (
		"%s::wrapupIncomingSerialization: Unrecognized Site # %u", iRTTI.name (), m_xObjectSite
	    );
	 }
	break;
    }
    if (!pOID)
	setInterfaceTo_(0);
    else {
	IVUnknown::Reference pInterface;
	pOID->supplyInterface (pInterface);
	setInterfaceTo_(pInterface);
    }
}


/***********************
 ***********************
 *****             *****
 *****  IVUnknown  *****
 *****             *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (IVUnknown)

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IVUnknown::~IVUnknown () {
}

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

// {AC4FD60D-F94E-407d-916F-6776A9BCFEB7}
VINTERFACE_TYPEINFO_DEFINITION (
    IVUnknown,
    0xac4fd60d, 0xf94e, 0x407d, 0x91, 0x6f, 0x67, 0x76, 0xa9, 0xbc, 0xfe, 0xb7
);

/*************************
 *************************
 *****  Member Info  *****
 *************************
 *************************/

VINTERFACE_MEMBERINFO_DEFINITION (IVUnknown, QueryInterface, 0);

/*******************
 *******************
 *****  HACKS  *****
 *******************
 *******************/

void IVUnknown::closePeer () {
    VcaOIDR *const pOIDR = oidr ();
    if (pOIDR)
	pOIDR->objectSite ()->onShutdown ();
}


/*************************************
 *************************************
 *****  Default Implementations  *****
 *************************************
 *************************************/

void IVUnknown::QI (VTypeInfo *pTypeInfo, IVReceiver<IVUnknown*> *pReceiver) {
    pReceiver->OnData (typeInfo_()->specializes (pTypeInfo) ? this : 0);
}

bool IVUnknown::isConnected_() const {
    return true;
}

bool IVUnknown::isUsable_() const {
    return true;
}

bool IVUnknown::requestNoRouteToPeerCallback (
    VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended
) const {
    if (VcaOIDR const *const pOIDR = oidr ())
	return pOIDR->requestNoRouteToPeerCallback (rpTicket, pEventSink, bSuspended);

    rpTicket.clear ();
    return false;
}

Vca::VcaSite* IVUnknown::site () const {
    VcaOIDR const* const pOIDR = oidr ();
    if (pOIDR)
	return pOIDR->objectSite ();
    return Vca::self ();
}

Vca::VSiteInfo* IVUnknown::siteInfo () const {
    return site ()->info ();
}


/***************************
 ***************************
 *****  Object Access  *****
 ***************************
 ***************************/

Vca::VcaOID *IVUnknown::oid () const {
    VcaOIDR *pOID = oidr ();
    return pOID ? pOID : static_cast<VcaOID*>(m_iOIDLVise);
}

/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void IVUnknown::createExportTo (VcaPeer *pPeer, bool bWeak) {
    VcaOIDL::Reference pOIDL;
    m_iOIDLVise.getOIDL(pOIDL, this);
    pOIDL->createExportTo (pPeer, bWeak);
}

void IVUnknown::createExportTo (VcaSelf *pSelf, bool bWeak) {
    VcaOIDL::Reference pOIDL;
    m_iOIDLVise.getOIDL(pOIDL, this);
    pOIDL->createExportTo (pSelf, bWeak);
}

/***************************
 ***************************
 *****  Object Import  *****
 ***************************
 ***************************/

bool IVUnknown::weakenImport () const {
    VcaOIDR* const pOIDR = oidr ();
    return pOIDR && pOIDR->weakenImport ();
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void IVUnknown::displayInfo () const {
    BaseClass::displayInfo ();

    VcaOIDR *pOIDR = oidr ();
    if (!pOIDR)
        display (" Local Object");
    else display (
        " Remote Object: %08x:%08x %s",
        pOIDR->objectSSID ().u32High (),
        pOIDR->objectSSID ().u32Low (),
        pOIDR->objectType ()->name ().content ()
    );
    display (" %s\n", typeInfo_()->name ().content());

    fflush (stdout);
}

void IVUnknown::displayExportTable () const {
    VcaOIDL::Reference pOIDL;
    m_iOIDLVise.getOIDL(pOIDL);
    if (pOIDL)
	pOIDL->displayExportTable ();
}
