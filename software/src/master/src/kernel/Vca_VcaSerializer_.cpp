/*****  Vca_VcaSerializer_ Implementation  *****/

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

#include "Vca_VcaSerializer_.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaSerializerForVkDynamicArrayOf_.h"
#include "Vca_VcaSerializerForVkSetOf_.h"
#include "Vca_VcaSite.h"
#include "Vca_VcaTransport.h"

#include "Vca_VInterfaceMember.h"
#include "VPeerDataArray.h"
#include "VReceiver.h"
#include "VTypeInfo.h"


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VcaSerializer_<Vca::VcaSite*>  *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VcaSite*>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum), m_xPeerKind (PeerKind_Unknown) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

/****************************************************************************
 * If the VcaSite has already been moved over the transport, then move 
 * only SSID and VcaRouteStatistics, else move SSID, Characteristics, 
 * and VcaRouteStatistics
****************************************************************************/

void Vca::VcaSerializer_<Vca::VcaSite*>::doData (Sequencer *pSequencer) {
    switch (m_xPeerKind) {
    case PeerKind_Unknown:
 	transferData ();
        break;
    
    case PeerKind_New:
        pSequencer->setActionTo(&ThisClass::doCharacteristics);
        start (this, New_VcaSerializer_(this, m_iSSID));
        break;

    case PeerKind_Old:
        clearSequencer ();
        start (this, New_VcaSerializer_(this, m_iSSID));
        break;

    default:
	abort ();
	break;
    }
}

void Vca::VcaSerializer_<Vca::VcaSite*>::doCharacteristics (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doRoutingInformation); 
    start (this, New_VcaSerializer_(this, m_iCharacteristics));
}

void Vca::VcaSerializer_<Vca::VcaSite*>::doRoutingInformation (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, New_VcaSerializer_(this, m_iBestRouteMetrics));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<Vca::VcaSite*>::localizeData () {
    peer ()->localize (m_xPeerKind);
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VcaSite*>::getData () {
    get (&m_xPeerKind, sizeof (m_xPeerKind));
}

/**********************
 *----  Put Data  ----*
 **********************/

/****************************************************************************
 * If the datum hasnt been exported to the peer earlier, inserts the datum 
 * into the transport optimization table. If the peer datum is new, adds the 
 * peer to the exported-to list of peers, which maintains the list of peers 
 * to which this peer datum has been sent
****************************************************************************/

void Vca::VcaSerializer_<Vca::VcaSite*>::putData () {
    VcaTransportOutbound *pTransport = transportIfOutbound ();

    if(!pTransport->addExport (m_rDatum, m_iSSID)) 
        m_xPeerKind = PeerKind_Old;
    else { 
        m_xPeerKind = PeerKind_New;
        m_iCharacteristics = m_rDatum->characteristics ();
        m_rDatum->getBestRouteMetrics (m_iBestRouteMetrics);
    }

    put (&m_xPeerKind, sizeof (m_xPeerKind));
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

/****************************************************************************
 * Checks if the peer has already been received, if so sets the datum to 
 * appropriate pointer else creates a new VcaSite instance and initializes 
 * the received values to it. In both cases, creates and sets a new 
 * VcaRouteStatistics object with hopCount equal to one more than what was 
 * received (as an extra segment(between this peer and the informer) has 
 * to be travelled)
****************************************************************************/

void Vca::VcaSerializer_<Vca::VcaSite*>::wrapupIncomingSerialization () {
    if (m_rDatum) {
	m_rDatum->release ();
	m_rDatum = NilOf (VcaSite*);
    }

//  Get the transport as the inbound transport that it is...
    VcaTransportInbound *pTransport = transportIfInbound ();

//  ... and get or create the peer:
    switch(m_xPeerKind) {
    case PeerKind_Old:
        pTransport->getImport (m_iSSID, m_rDatum);
        break;
        
    case PeerKind_New: 
        m_rDatum = peer ()->sibling (m_iCharacteristics); {
	    VcaRouteStatistics iStatistics (m_iBestRouteMetrics.hopCount()+1); 
	    peer ()->addRouteTo (m_rDatum, iStatistics); 
	}

        // inserting the datum into the transport's peer-optimization table
        pTransport->addImport (m_iSSID, m_rDatum);
        break;

      default:
	abort ();
	break;
    }
    if (m_rDatum)
	m_rDatum->retain ();
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vca::VcaSerializer_<Vca::VcaRouteStatistics>  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VcaRouteStatistics>::VcaSerializer_(
    VcaSerializer *pCaller, VcaRouteStatistics &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}

/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VcaRouteStatistics>::doData (Sequencer *pSequencer) {
    clearSequencer ();
    transferData ();
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<Vca::VcaRouteStatistics>::localizeData () {
    peer ()->localize (m_iHopCount); 
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VcaRouteStatistics>::getData () {
    get (&m_iHopCount, sizeof (m_iHopCount)); 
}  

/**********************
 *----  Put Data  ----*
 **********************/ 

void Vca::VcaSerializer_<Vca::VcaRouteStatistics>::putData () {
    m_iHopCount = m_rDatum.hopCount();
    put (&m_iHopCount, sizeof (m_iHopCount)); 
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<Vca::VcaRouteStatistics>::wrapupIncomingSerialization () {
    m_rDatum.setHopCount(m_iHopCount);
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VcaSerializer_<V::VBlob>  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<V::VBlob>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum), m_pData (0) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<V::VBlob>::doData (Sequencer *pSequencer) {
    if (movingData ())
	clearSequencer ();
    transferData ();
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<V::VBlob>::localizeData () {
    if (movingSize ()) {
	peer ()->localize (m_sData);
	if (m_sData)
	    m_pData = m_rDatum.storage (m_sData);
	else
	    clearSequencer ();
    }
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<V::VBlob>::getData () {
    if (movingData ())
	get (m_pData, m_sData);
    else // movingSize ()
	get (&m_sData, sizeof (m_sData));
}

/**********************
 *----  Put Data  ----*
 **********************/

void Vca::VcaSerializer_<V::VBlob>::putData () {
    if (movingData ())
	put (m_pData, m_sData);
    else { // movingSize ()
	m_pData = m_rDatum.contentArea ();
	m_sData = m_rDatum.contentSize ();
	put (&m_sData, sizeof (m_sData));
    }
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<V::VBlob>::wrapupIncomingSerialization () {
    m_rDatum.incrementContentUpperBoundBy (m_sData);
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VcaSerializer_<V::VTime>  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<V::VTime>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum), m_iNanosecondsSinceEpoch (rDatum.encodedValue () * 1000) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doNanosecondsSinceEpoch));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<V::VTime>::doNanosecondsSinceEpoch (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, New_VcaSerializer_(this, m_iNanosecondsSinceEpoch));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<V::VTime>::wrapupIncomingSerialization () {
    m_rDatum.setTo (m_iNanosecondsSinceEpoch / 1000);
}


/***************************************************************
 ***************************************************************
 *****                                                     *****
 *****  Vca::VcaSerializer_<Vca::VInterfaceMember const*>  *****
 *****                                                     *****
 ***************************************************************
 ***************************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VInterfaceMember const*>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}

Vca::VcaSerializer_<Vca::VInterfaceMember const*>::~VcaSerializer_() {
    if (isOutgoing ()) {
        transport()->defaultLogger().printf (
            "+++ VcaSerializerForApplicable: [%p] %s: destruction: %p\n",
            m_rDatum, m_rDatum->name (), m_pInterfaceType.referent ()
        );
    }
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::doData (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doType);
    transferData ();
}

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::doType (Sequencer *pSequencer) {
    if (isOutgoing ()) {
        transport()->defaultLogger().printf (
            "+++ VcaSerializerForApplicable: [%p] %s: doType: %p\n",
            m_rDatum, m_rDatum->name (), m_pInterfaceType.referent ()
        );
    }

    clearSequencer ();
    start (this, New_VcaSerializer_(this, m_pInterfaceType));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::localizeData () {
    peer ()->localize (m_xMember);
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::getData () {
    //  {memberIndex:U32} (VTypeInfo *pInterface)
    get (&m_xMember, sizeof (m_xMember));
}

/**********************
 *----  Put Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::putData () {
    m_xMember = m_rDatum->index ();
    m_pInterfaceType.setTo (m_rDatum->interfaceTypeInfo_());

    if (isOutgoing ()) {
        transport()->defaultLogger().printf (
            "+++ VcaSerializerForApplicable: [%p] %s: doData: %u\n",
            m_rDatum, m_rDatum->name (), m_xMember
        );
    }

    //  {memberIndex:U32} (VTypeInfo *pInterface)
    put (&m_xMember, sizeof (m_xMember));
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<Vca::VInterfaceMember const*>::wrapupIncomingSerialization () {
    m_rDatum = m_pInterfaceType ? m_pInterfaceType->interfaceMember (m_xMember) : 0;
}


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::VcaSerializer_<Vca::VPeerData>   *****
 *****                                        *****
 **************************************************
 **************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VPeerData>::VcaSerializer_(
    VcaSerializer *pCaller, VPeerData &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum) {
   m_pSequencer.setTo (new Sequencer (this, &ThisClass::doInFinalPath));
}

/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VPeerData>::doInFinalPath (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doData); 
    transferData ();
}

void Vca::VcaSerializer_<Vca::VPeerData>::doData (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doListenerDomains); 
    start (this, New_VcaSerializer_(this, m_rDatum.m_iPeer));
}

void Vca::VcaSerializer_<Vca::VPeerData>::doListenerDomains (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doConnectorDomains); 
    start (this, New_VcaSerializerForVkSetOf_(this, m_rDatum.m_iListenerDomains));
}

void Vca::VcaSerializer_<Vca::VPeerData>::doConnectorDomains (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doConnectedPeers); 
    start (this, New_VcaSerializerForVkSetOf_(this, m_rDatum.m_iConnectorDomains));
}

void Vca::VcaSerializer_<Vca::VPeerData>::doConnectedPeers  (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, New_VcaSerializerForVkSetOf_(this, m_rDatum.m_iConnectedPeers));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<Vca::VPeerData>::localizeData () {
    peer ()->localize (m_rDatum.m_iInFinalPath);
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VPeerData>::getData () {
    get (&m_rDatum.m_iInFinalPath, sizeof (m_rDatum.m_iInFinalPath));
}  

/**********************
 *----  Put Data  ----*
 **********************/ 

void Vca::VcaSerializer_<Vca::VPeerData>::putData () {
    put (&(m_rDatum.m_iInFinalPath), sizeof (m_rDatum.m_iInFinalPath));
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<Vca::VPeerData>::wrapupIncomingSerialization () {
}


/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vca::VcaSerializer_<Vca::VPeerDataArray>  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************/

Vca::VcaSerializer_<Vca::VPeerDataArray>::VcaSerializer_(
    VcaSerializer *pCaller, VPeerDataArray &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum), m_cElements (UINT_MAX), m_xElement (0) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doCount));
}

/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VPeerDataArray>::doCount (Sequencer *pSequencer) {
    if (isIncoming ())
        pSequencer->setActionTo(&ThisClass::doPeerDataSetup); 
    else
        pSequencer->setActionTo(&ThisClass::doPeerData);

    transferData ();
}

void Vca::VcaSerializer_<Vca::VPeerDataArray>::doPeerDataSetup (Sequencer *pSequencer) {
    pSequencer->setActionTo(&ThisClass::doPeerData); 
    m_rDatum.append (m_cElements);
}

void Vca::VcaSerializer_<Vca::VPeerDataArray>::doPeerData (Sequencer *pSequencer) {
    if(m_cElements==0)
	clearSequencer ();
    else {
	if (m_xElement == (m_cElements-1))
	    clearSequencer ();
	start (this, New_VcaSerializer_(this, m_rDatum[m_xElement]));
	m_xElement++;
    }
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<Vca::VPeerDataArray>::localizeData () {
    peer ()->localize (m_cElements); 
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<Vca::VPeerDataArray>::getData () {
    get (&m_cElements, sizeof (m_cElements)); 
}  

/**********************
 *----  Put Data  ----*
 **********************/ 

void Vca::VcaSerializer_<Vca::VPeerDataArray>::putData () {
    m_cElements = m_rDatum.cardinality();
    put (&m_cElements, sizeof (m_cElements)); 
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<Vca::VPeerDataArray>::wrapupIncomingSerialization () {
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VcaSerializer_<VString>  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<V::VString>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum), m_pData (0) {
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<V::VString>::doData (Sequencer *pSequencer) {
    if (movingData ())
	clearSequencer ();
    transferData ();
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<V::VString>::localizeData () {
    if (movingSize ()) {
	peer ()->localize (m_sData);
	m_pData = m_rDatum.storage (m_sData);
    }
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<V::VString>::getData () {
    if (movingData ())
	get (m_pData, m_sData + 1);
    else // movingSize ()
	get (&m_sData, sizeof (m_sData));
}

/**********************
 *----  Put Data  ----*
 **********************/

void Vca::VcaSerializer_<V::VString>::putData () {
    if (movingData ())
	put (m_pData, m_sData + 1);
    else { // movingSize ()
	m_sData = m_rDatum.length ();
	m_pData = m_rDatum.storage ();
	put (&m_sData, sizeof (m_sData));
    }
}


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vca::VcaSerializer_<VTypeInfo*>  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<VTypeInfo*>::VcaSerializer_(VcaSerializer *pCaller, VTypeInfo *&rDatum)
: VcaSerializer	(pCaller)
, m_rDatum	(rDatum)
, m_pScope	(VTypeInfo::ground ())
, m_xParameter	(0)
, m_xTypeKind	(TypeKind_Unknown)
{
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<VTypeInfo*>::doData (Sequencer *pSequencer) {
    switch (m_xTypeKind) {
    case TypeKind_Unknown:
	transferData ();
	break;

    //  TypeKind_Nil{} ()
    case TypeKind_Nil:
	clearSequencer ();
	break;

    //  TypeKind_Reference{this:SSID} ()
    case TypeKind_Reference:
	clearSequencer ();
	transferData ();
	break;

    //  TypeKind_Simple{this:SSID} (uuid_t iUUID)
    case TypeKind_Simple:
	pSequencer->setActionTo (&ThisClass::doSymbolName);
	transferData ();
	break;

    //  TypeKind_Subtype{this:SSID} (VTypeInfo *pSuper, uuid_t iUUID)
    case TypeKind_Subtype:
	pSequencer->setActionTo (&ThisClass::doSuper);
	transferData ();
	break;

    //  TypeKind_Template{this:SSID} (
    //	U32			n,  // m_cParameters
    //	VTypeInfo		*pP1, ... *pPn,
    //	VTypeInfo		*pSuper,
    //	uuid_t		iUUID
    //  )
    case TypeKind_Template:
	pSequencer->setActionTo (&ThisClass::doParameterCount);
	transferData ();
	break;

    //  TypeKind_General{this:SSID} (
    //	VTypeInfo		*pScope,
    //	U32			n,  // m_cParameters
    //	VTypeInfo		*pP1, ... *pPn,
    //	VTypeInfo		*pSuper,
    //	uuid_t		iUUID
    //  )
    case TypeKind_General:
	pSequencer->setActionTo (&ThisClass::doScope);
	transferData ();
	break;

    default:
	abort ();
	break;
    }
}

void Vca::VcaSerializer_<VTypeInfo*>::doParameterCount (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doParameters);
    start (this, New_VcaSerializer_(this, m_cParameters));
}

void Vca::VcaSerializer_<VTypeInfo*>::doParameters (Sequencer *pSequencer) {
    if (m_pParameters.isNil ())
	m_pParameters.setTo (0, new ParameterSignature (m_cParameters));

    if (m_xParameter >= m_cParameters)
	pSequencer->setActionTo (&ThisClass::doSuper);
    else {
	start (this, New_VcaSerializer_(this, m_pParameters->operator[] (m_xParameter++)));
    }
}

void Vca::VcaSerializer_<VTypeInfo*>::doScope (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doParameterCount);
    start (this, New_VcaSerializer_(this, m_pScope));
}

void Vca::VcaSerializer_<VTypeInfo*>::doSuper (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doSymbolName);
    start (this, New_VcaSerializer_(this, m_pSuper));
}

void Vca::VcaSerializer_<VTypeInfo*>::doSymbolName (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doSymbolUUID);
    start (this, New_VcaSerializer_(this, m_iTypeName));
}

void Vca::VcaSerializer_<VTypeInfo*>::doSymbolUUID (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, New_VcaSerializer_(this, m_iTypeUUID));
}


/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializer_<VTypeInfo*>::localizeData () {
    if (typeKindIsKnown ())
	peer ()->localize (m_iSSID);
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializer_<VTypeInfo*>::getData () {
    if (typeKindIsKnown ())
	get (&m_iSSID, sizeof (m_iSSID));
    else
	get (&m_xTypeKind, sizeof (m_xTypeKind));
}

/**********************
 *----  Put Data  ----*
 **********************/

void Vca::VcaSerializer_<VTypeInfo*>::putData () {
    if (typeKindIsKnown ())
	put (&m_iSSID, sizeof (m_iSSID));
    else {
    //  Generate a 'Nil' type kind if VTypeInfo* is Nil...
	if (IsNil (m_rDatum))
	    m_xTypeKind = TypeKind_Nil;

    //  Generate a 'Reference' type kind for already shipped types ('export' returns false):
	else if (!transportIfOutbound ()->addExport (m_rDatum, m_iSSID))
	    m_xTypeKind = (typekind_wire_t)TypeKind_Reference;

    //  Otherwise, unpack and decode the existing VTypeInfo object:
	else {
	    m_pSuper.setTo (m_rDatum->super ());
	    m_iTypeName.setTo (m_rDatum->name ());
	    m_iTypeUUID = m_rDatum->uuid ();
	    if (m_rDatum->isSimple ())
		m_xTypeKind = m_pSuper ? (typekind_wire_t)TypeKind_Subtype : (typekind_wire_t)TypeKind_Simple;
	    else {
		m_pScope.setTo (m_rDatum->scope ());
		m_pParameters.setTo (
		    m_rDatum,
		    const_cast<VTypeInfo::ParameterSignature*>(&m_rDatum->parameters ())
		);
		m_cParameters = m_pParameters->elementCount ();
		m_xTypeKind = m_pScope->isGround () ? (typekind_wire_t)TypeKind_Template : (typekind_wire_t)TypeKind_General;
	    }
	}
	put (&m_xTypeKind, sizeof (m_xTypeKind));
    }
}


/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializer_<VTypeInfo*>::wrapupIncomingSerialization () {
    if (m_rDatum) {
	m_rDatum->release ();
	m_rDatum = NilOf (VTypeInfo*);
    }

//  Get the transport as the inbound transport that it is...
    VcaTransportInbound *pTransport = transportIfInbound ();

//  Handle the different type kinds...
    switch (m_xTypeKind) {
    case TypeKind_Nil:
	return;

    case TypeKind_Reference:
	if (pTransport->getImport (m_iSSID, m_rDatum))
	    m_rDatum->retain ();
        else
            raiseApplicationException ("Received Unknown Type ID: %08x:%08x", m_iSSID.u32High (), m_iSSID.u32Low ());
	return;

    default:
	break;
    }

//  Locate or create the type:
    switch (m_xTypeKind) {
    case TypeKind_Simple:
    case TypeKind_Subtype:
	m_rDatum = m_pScope ? m_pScope->instance (
	    m_iTypeName, m_iTypeUUID, m_pSuper
	) : VTypeInfo::ground (m_iTypeName, m_iTypeUUID, m_pSuper);
	break;

    case TypeKind_Template:
    case TypeKind_General:
	m_rDatum = m_pScope ? m_pScope->instance (
	    m_iTypeName, m_iTypeUUID, m_pSuper, *m_pParameters
	) : VTypeInfo::ground (m_iTypeName, m_iTypeUUID, m_pSuper, *m_pParameters);
	break;

    default:
	abort ();
	break;
    }

    if (!m_pParameters.container ()) {
	delete m_pParameters.component ();
	m_pParameters.clear ();
    }
    if (m_rDatum) {
	m_rDatum->retain ();
	pTransport->addImport (m_iSSID, m_rDatum);
    }
}
