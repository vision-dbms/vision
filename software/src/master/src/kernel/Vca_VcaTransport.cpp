/*****  VcaTransport Implementation  *****/

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

#include "Vca_VcaTransport.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VMessage.h"
#include "Vca_VcaConnection.h"
#include "Vca_VcaOIDL.h"
#include "Vca_VcaRouteStatistics.h"
#include "Vca_VcaSelf.h"
#include "Vca_VcaSerializer.h"
#include "Vca_VClassInfoHolder.h"
#include "Vca_VTrigger.h"
#include "Vca_VTimer.h"

#include "VkSocketAddress.h"


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VcaSerializerForMessage  *****
 *****                                *****
 ******************************************
 ******************************************/

namespace Vca {
    class VcaSerializerForMessage : public VcaSerializer {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VcaSerializerForMessage, VcaSerializer);

    //  Sequencer
    public:
	typedef VTrigger<ThisClass> Sequencer;

    //  Construction
    public:
	VcaSerializerForMessage (
	    VcaTransportOutbound *pTransport, VMessage *pMessage
	);
	VcaSerializerForMessage (VcaTransportInbound *pTransport);

    //  Destruction
    protected:
	~VcaSerializerForMessage () {
	}

    //  Sequencer Actions
    protected:
	void doApplicable      (Sequencer *pSequencer);
	void doArguments       (Sequencer *pSequencer);
	void doSequenceNumber  (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	void localizeData ();
	void getData ();
	void putData ();
	void wrapupIncomingSerialization ();
	void wrapupOutgoingSerialization ();

    //  State
    protected:
	VMessage::Reference	m_pMessage;
	VInterfaceMember const*	m_pApplicable;
	unsigned int		m_xArgument;
	unsigned int		m_xMessage;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializerForMessage::VcaSerializerForMessage (
    VcaTransportOutbound *pTransport, VMessage *pMessage
)
: VcaSerializer		(pTransport)
, m_pMessage		(pMessage)
, m_pApplicable		(pMessage->applicable_())
, m_xArgument		(0)
, m_xMessage		(VMessage::SequenceNumber (pMessage))
{
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doSequenceNumber));
}

Vca::VcaSerializerForMessage::VcaSerializerForMessage (VcaTransportInbound *pTransport)
: VcaSerializer		(pTransport)
, m_pApplicable		(0)
, m_xArgument		(0)
, m_xMessage		(0)
{
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doSequenceNumber));
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializerForMessage::doSequenceNumber (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doApplicable);
    transferData ();
}

void Vca::VcaSerializerForMessage::doApplicable (Sequencer *pSequencer) {
    pSequencer->setActionTo (&ThisClass::doArguments);
    start (this, New_VcaSerializer_(this, m_pApplicable));
}

void Vca::VcaSerializerForMessage::doArguments (Sequencer *pSequencer) {
    if (m_pMessage.isNil () && m_pApplicable)
	m_pMessage.setTo (m_pApplicable->newMessage (m_xMessage));

    if (m_pMessage.isNil ())
	abort ();
    else if (m_xArgument <= m_pApplicable->parameterCount ())
	start (this, m_pMessage->serializer (this, m_xArgument++));
    else
        clearSequencer ();
}


/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializerForMessage::localizeData () {
    peer ()->localize (m_xMessage); 
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializerForMessage::getData () {
    get (&m_xMessage, sizeof (m_xMessage)); 
}  

/**********************
 *----  Put Data  ----*
 **********************/ 

void Vca::VcaSerializerForMessage::putData () {
    put (&m_xMessage, sizeof (m_xMessage)); 
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializerForMessage::wrapupIncomingSerialization () {
    m_pMessage->evaluateIncomingFrom (peer ());
}

/*****************************
 *----  Outgoing Wrapup  ----*
 *****************************/

void Vca::VcaSerializerForMessage::wrapupOutgoingSerialization () {
    m_pMessage->setStatusToSent ();
    m_pTransport->onWrapupSerialization ();
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VcaSerializerForPlumbing  *****
 *****                                 *****
 *******************************************
 *******************************************/

namespace Vca {
    class VcaSerializerForPlumbing : public VcaSerializer {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VcaSerializerForPlumbing, VcaSerializer);

    //  Characteristics
    public:
	typedef VcaSite::Characteristics Characteristics;

    //  Sequencer
    public:
	typedef VTrigger<ThisClass> Sequencer;

    //  Construction
    public:
	VcaSerializerForPlumbing (VcaTransport *pTransport, VcaConnection *pConnection);

    //  Destruction
    protected:
	~VcaSerializerForPlumbing () {
	}

    //  Peer Management
    public:
	bool plumbed () const {
	    return m_pTransport->plumbed ();
	}

	void findPeer (VcaSite::Reference &rpPeer) const {
	    m_pConnection->getPeer (rpPeer);
	}

	void makePeer (VcaSite::Reference &rpPeer) const {
	    VcaSite *pPeer = self()->child (m_iCharacteristics);
	    VcaRouteStatistics iStatistics (1);
	    pPeer->setRoutingInformation (pPeer,iStatistics);
	    rpPeer.setTo(pPeer);
	    m_pConnection->setPeer (rpPeer);
	}

    //  Sequencer Actions
    protected:
	void doData		(Sequencer *pSequencer);
	void doPeer		(Sequencer *pSequencer);
	void doReflection	(Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	void localizeData ();

	void getData ();
	void putData ();

	void wrapupIncomingSerialization ();

    //  State
    protected:
	VcaConnection::Reference const	m_pConnection;
	Characteristics			m_iCharacteristics;
	IPeer::Reference		m_pReflection;
	bool				m_bNoGetYet;
	bool				m_bDataDone;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

// Sets the local peers characteristics as the default one 

Vca::VcaSerializerForPlumbing::VcaSerializerForPlumbing (
    VcaTransport *pTransport, VcaConnection *pConnection
) : VcaSerializer	(pTransport)
,   m_pConnection	(pConnection)
,   m_iCharacteristics	(self ()->characteristics ())
,   m_bNoGetYet		(true)
,   m_bDataDone		(false)
{
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
}

 
/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

// Initiates peer-characteristics data transfer through appropriate 
// transports (VcaTransportInbound/VcaTransportOutbound) 

void Vca::VcaSerializerForPlumbing::doData (Sequencer *pSequencer) {
    if (m_bDataDone)
	pSequencer->setActionTo (&ThisClass::doPeer);
    else
	transferData ();
}

// Creates or sets peer based on inbound or outbound 
// also sets the reflection reference. if inbound sets the reflection
// from the peer charactersitics obtained, if outbound sets the reflection
// from the characteristics of the host.

void Vca::VcaSerializerForPlumbing::doPeer (Sequencer *pSequencer) {
    m_pTransport->setPeerFrom (this);

    if (!plumbed ())
	m_pConnection->suspend (this);
    else {
	m_pConnection->supply (m_pReflection);
	pSequencer->setActionTo (&ThisClass::doReflection);
    }
}

void Vca::VcaSerializerForPlumbing::doReflection (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, new VcaSerializerForInterface_<IPeer>(this, m_pReflection, true /* => weak */));
}


/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

/*******************************
 *----  Data Localization  ----*
 *******************************/

void Vca::VcaSerializerForPlumbing::localizeData () {
    if (m_bDataDone)
	m_iCharacteristics.localize ();
}

/**********************
 *----  Get Data  ----*
 **********************/

void Vca::VcaSerializerForPlumbing::getData () {
    unsigned char *pDataBytes = reinterpret_cast<unsigned char*>(&m_iCharacteristics);

    if (m_bNoGetYet || m_iCharacteristics.architectureIsUnreasonable ()) {
	m_bNoGetYet = false;
	get (pDataBytes, 1);
    }
    else {
	m_bDataDone = true;
	get (pDataBytes + 1, sizeof (m_iCharacteristics) - 1);
    }
}

/**********************
 *----  Put Data  ----*
 **********************/

void Vca::VcaSerializerForPlumbing::putData () {
    m_bDataDone = true;
    put (&m_iCharacteristics, sizeof (m_iCharacteristics));
}

/*****************************
 *----  Incoming Wrapup  ----*
 *****************************/

void Vca::VcaSerializerForPlumbing::wrapupIncomingSerialization () {
    m_pConnection->onConnect (m_pReflection);
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VcaTransport::BSManager  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaTransport::BSManager::BSManager (VcaTransport *pTransport) : m_pTransport (pTransport) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaTransport::BSManager::~BSManager () {
}

/*********************************
 *********************************
 *****  BSManager Callbacks  *****
 *********************************
 *********************************/

void Vca::VcaTransport::BSManager::onTransfer (size_t sTransfer) {
    m_pTransport->onBytesTransferred ();
}

bool Vca::VcaTransport::BSManager::onStatus (VStatus const &rStatus) {
    bool const bClosedOrFailed = rStatus.isClosed () || rStatus.isFailed ();
    if (bClosedOrFailed) {
	m_pTransport->defaultLogger().printf ("+++ VcaTransportOutbound[%p]::BSManager[%p]::onStatus %s\n",
 				m_pTransport.referent(), this, rStatus.CodeDescription ());
	log ("\nVca::VcaTransport::BSManager::onStatus...Aborting Transport...");

	VcaTransport::Reference const pTransport (m_pTransport);
	pTransport->Abort ();
	pTransport->onFailedOrClosed ();
    }
    return bClosedOrFailed;
}

void Vca::VcaTransport::BSManager::onChange (U32 sChange) {
    m_pTransport->onChange (sChange);
}


/*************************************************
**************************************************
 *****                                       *****
 *****  Vca::VcaTransport::InTransitMessage  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaTransport::InTransitMessage::InTransitMessage (
   VcaTransport *pTransport, VMessage *pMessage, U32 xStartByteOffset
) : m_pTransport (pTransport)
  , m_pMessage (pMessage)
  , m_xStartByteOffset (xStartByteOffset)
  , m_sSize (UINT_MAX) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaTransport::InTransitMessage::~InTransitMessage () {
}


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VcaTransport  *****
 *****                     *****
 *******************************
 *******************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VcaTransport::TransportTracing  Vca::VcaTransport::g_bTracingTransfers = Tracing_NotInitialized;

bool Vca::VcaTransport::TracingTransfers () {
    if (Tracing_NotInitialized == g_bTracingTransfers) {
	char const *const pEnvValue = getenv ("TraceTransports");
	g_bTracingTransfers = !pEnvValue || strcasecmp (pEnvValue, "TRUE") ? Tracing_Off : Tracing_On;
    }
    return Tracing_On == g_bTracingTransfers;
}

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

namespace Vca {
    VClassInfoHolder &VcaTransport::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addBase (BaseClass::ClassInfo ())

		.addProperty ("peerSiteInfo", &ThisClass::getPeerSiteInfo)

		.markCompleted ();
	}
	return iClassInfoHolder;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaTransport::VcaTransport (VcaConnection *pConnection)
  : m_pConnection (pConnection)
  , m_sInTransitMessage (0)
  , m_bTracingTransfers (false)
  , m_cBytesTransfered (0)
  , m_bShutdownInProgress (false)
  , m_pIBSClient (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaTransport::~VcaTransport () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VcaTransport::getPeerSiteInfo (IVReceiver<VString const&> *pResultSink) const {
    if (peer())
	peer()->info()->supplyProcessInfo (pResultSink);
    else {
	pResultSink->OnData ("---");
    }
}

void Vca::VcaTransport::getDescription_(VString &rResult) const {
    switch (directionCode ()[0]) {
    case 'I':
	rResult << "Incoming";
	break;
    case 'O':
	rResult << "Outbound";
	break;
    default:
	rResult << "Uncoming";
	break;
    }
    rResult << " Transport At "; {
	VkUUIDHolder const iPeerUUID (self ()->uuid ());
	iPeerUUID.GetString (rResult);
    }
    rResult << " ["; {
	char iBuffer[256];
	rResult << VkSocketAddress::hostname (iBuffer, sizeof (iBuffer));
    }
    rResult << ", " << getpid () << "] To "; {
	VkUUIDHolder const iPeerUUID (peer ()->uuid ());
	iPeerUUID.GetString (rResult);
    }
}

char const *Vca::VcaTransport::directionCode () const {
    return isIncoming () ? "I" : isOutgoing () ? "O" : "-";
}

Vca::VcaTransportInbound *Vca::VcaTransport::transportIfInbound () {
    raiseApplicationException (
	"VcaTransport::transportIfInbound: %s is not an inbound transport", rttName ().content ()
    );
    return 0;
}

Vca::VcaTransportOutbound *Vca::VcaTransport::transportIfOutbound () {
    raiseApplicationException (
	"VcaTransport::transportIfOutbound: %s is not an outbound transport", rttName ().content ()
    );
    return 0;
}


/**************************
 **************************
 *****  Data Display  *****
 **************************
 **************************/
namespace {
    using namespace Vca;

    U64 transferDisplayLimit () {
	static U64 sTraceLimit = UINT64_MAX;
	static bool bNotInitialized = true;

	if (bNotInitialized) {
	    bNotInitialized = false;
	    if (char const * const pEnvValue = getenv ("TraceTransportLimit"))
		sscanf (pEnvValue, "%llu", &sTraceLimit);
	}
	return sTraceLimit;
    }

    FILE *displayFile () {
	static FILE *pDisplayFile = NULL;
	static bool bNotInitialized = true;

	if (bNotInitialized) {
	    bNotInitialized = false;
	    if (char const * const pEnvValue = getenv ("TraceTransportPath"))
		pDisplayFile = fopen (pEnvValue, "a");
	}
	return pDisplayFile;
    }
}

void Vca::VcaTransport::displayDataAtTransferPoint (char const *pData, size_t sData) const {
    FILE *pFile = displayFile ();
// Output to the specified file, or to standard error ...
    displayDataAtTransferPoint (pData, sData, pFile ? pFile :stderr);
}

void Vca::VcaTransport::displayDataAtTransferPoint (
    char const *pData, size_t sData, FILE *pFile
) const {
    m_cBytesTransfered += sData;
    if (sData > 0 && IsntNil (pFile) && tracingTransfers () && transferDisplayLimit () > m_cBytesTransfered) {
	RTTI iSerializerRTTI (m_pInTransitSerializer);
	fprintf (
	    pFile, "%s %p: %p: %s:", directionCode (), this,
	    m_pInTransitSerializer.referent (), iSerializerRTTI.name ()
	);
	char iBuffer[20 * 3 + 1];
	char *pIP = iBuffer;
	for (unsigned int xByte = 0; xByte < sData; xByte++) {
	    if (0 == xByte % 20) {
		if (iBuffer == pIP)
		    fprintf (pFile, "\n   ");
		else {
		    *pIP = 0;
		    fprintf (pFile, " <%s>\n   ", iBuffer);
		    pIP = iBuffer;
		}
	    }
	    unsigned char iChar = pData[xByte];
	    fprintf (pFile, " %02x", 0xff & iChar);
	    if (iChar >= ' ' && iChar < 0x7f)
		*pIP++ = iChar;
	    else {
		*pIP++ = '\\';
		switch (iChar) {
		case '\n':
		    *pIP++ = 'n';
		    break;
		case '\r':
		    *pIP++ = 'r';
		    break;
		case '\t':
		    *pIP++ = 't';
		    break;
		case '\0':
		    *pIP++ = '0';
		    break;
		default:
		    *pIP++ = '-';
		    break;
		}
	    }
	}
	*pIP = 0;
	fprintf (pFile, " <%s>\n", iBuffer);
	fflush (pFile);
    }
}


/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VcaTransport::getData (VcaSerializer *pSerializer, void *pVoid, size_t sVoid) {
}

void Vca::VcaTransport::putData (
    VcaSerializer *pSerializer, void const *pData, size_t sData
) {
}

/**********************
 **********************
 *****  Plumbing  *****
 **********************
 **********************/

void Vca::VcaTransport::plumbThyself () {
    (new VcaSerializerForPlumbing (this, m_pConnection))->resume ();
}

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void Vca::VcaTransport::schedule (VcaSerializer *pSerializer) {
    m_iScheduler.schedule (pSerializer);
}


/***************************
 ***************************
 *****  Serialization  *****
 ***************************
 ***************************/

void Vca::VcaTransport::resumeSerialization (bool notSuspended) {
    if (notSuspended)
	m_pInTransitSerializer.clear ();
    else {
	VcaSerializer::Pointer const pSuspendedSerializer (m_pInTransitSerializer);
	pSuspendedSerializer->retain ();
	m_pInTransitSerializer.clear ();
	pSuspendedSerializer->untain ();
	pSuspendedSerializer->resume ();
    }
}

/******************************************************************************
 * Note: 
 * If an error occurs when reading or writing it triggers this 
 * abortSerialization () method to release the current serializer and also
 * its parent chain of serializers.
 ******************************************************************************/

void Vca::VcaTransport::abortSerialization () {
    Reference const iRetainer (this);
    if (m_pInTransitSerializer.isntNil ()) {
	m_pInTransitSerializer->abort ();
	m_pInTransitSerializer.clear ();
    }
    Abort ();
}


/*******************************
 *******************************
 *****  InTransit Message  *****
 *******************************
 *******************************/

void Vca::VcaTransport::enqueue (InTransitMessage *pMessage) {
    if (m_pInTransitMsgTail.isntNil ()) 
	m_pInTransitMsgTail->m_pSuccessor.setTo (pMessage);
    else
	m_pInTransitMsgHead.setTo (pMessage);

    m_pInTransitMsgTail.setTo (pMessage);
}

bool Vca::VcaTransport::dequeue (InTransitMessage::Reference &rpMsg) {
    rpMsg.setTo (m_pInTransitMsgHead);
    if (rpMsg) {
	m_pInTransitMsgHead.setTo (m_pInTransitMsgHead->m_pSuccessor);
	if (m_pInTransitMsgHead.isNil ())
	    m_pInTransitMsgTail.clear ();
    }
    return rpMsg.isntNil ();
}

/**********************
 **********************
 *****  Shutdown  *****
 **********************
 **********************/

void Vca::VcaTransport::onShutdownInProgress () {
    m_bShutdownInProgress = true;
}


/**************************************
 **************************************
 *****                            *****
 *****  Vca::VcaTransportInbound  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaTransportInbound::VcaTransportInbound (
    VcaConnection *pConnection, BS *pBS
) : BaseClass (pConnection), m_pBS (pBS) {
    traceInfo ("Creating VcaTransportInbound");
}

/************************* 
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaTransportInbound::~VcaTransportInbound () {
    m_pConnection->detach (this);
    traceInfo ("Destroying VcaTransportInbound");
}

/*******************************
 *******************************
 *****  Import Management  *****
 *******************************
 *******************************/

bool Vca::VcaTransportInbound::addImport (SSID const &rSSID, VcaSite *pPeer) {
    unsigned int xElement;
    if(!m_iSiteTable.Insert(rSSID, xElement))
	return false;
    m_iSiteTable[xElement] = pPeer;
    return true;
}

bool Vca::VcaTransportInbound::getImport (SSID const &rSSID, VcaSite *&rpPeer) const {
    unsigned int xElement;
    if(!m_iSiteTable.Locate(rSSID, xElement))
	return false;
    rpPeer = m_iSiteTable[xElement];     
    return true;
}

bool Vca::VcaTransportInbound::addImport (SSID const &rSSID, VTypeInfo *pObject) {
    unsigned int xElement;
    if (!m_iTypeTable.Insert (rSSID, xElement))
	return false;

    m_iTypeTable[xElement] = pObject;
    return true;
}

bool Vca::VcaTransportInbound::getImport (SSID const &rSSID, VTypeInfo *&rpObject) const {
    unsigned int xElement;
    if (!m_iTypeTable.Locate (rSSID, xElement))
	return false;

    rpObject = m_iTypeTable[xElement];
    return true;
}


/****************************************
 ****************************************
 *****  Byte Stream Client Support  *****
 ****************************************
 ****************************************/

void Vca::VcaTransportInbound::OnTransfer (IBSClient *pRole, size_t sTransfer) {
    m_pConnection->incrementIncomingByteCount (sTransfer);

    displayDataAtTransferPoint (m_pInTransitDatum.producerArea (), sTransfer);
    m_pInTransitDatum.produce (sTransfer);

    if (transferDeficit () == 0) {
	m_pInTransitSerializer->localizeData ();
	resumeSerialization (IsNil (pRole));
    }
    else if (IsNil (pRole) && m_pInTransitSerializer.isntNil ())
	m_pInTransitSerializer->suspend ();
}

void Vca::VcaTransportInbound::OnError (
    IClient *pRole, IError *pError, VString const &rMessage
) {
    if (m_pBS) {
	defaultLogger().printf ("+++ VcaTransportInbound[%p]::OnError %s\n", this, rMessage.content ());

//	onActivityError (pError, rMessage);
	abortSerialization ();
    }
}


/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VcaTransportInbound::getData (
    VcaSerializer *pSerializer, void *pVoid, size_t sVoid
) {
    m_pInTransitSerializer.setTo (pSerializer);
    m_pInTransitDatum.setTo (pVoid, sVoid);

    if (m_pBS.isNil ())
	OnTransfer (0, 0);
    else {
	IBSClient::Reference pClientRole;
	getRole (pClientRole);
	OnTransfer (0, m_pBS->get (pClientRole, pVoid, sVoid));
    }
}

void Vca::VcaTransportInbound::transferDataFor (VcaSerializer *pSerializer) {
    pSerializer->getData ();
}

/**********************
 **********************
 *****  Plumbing  *****
 **********************
 **********************/

void Vca::VcaTransportInbound::onAttach (VcaPeer *pPeer) {
    onActivityStart ();
}

void Vca::VcaTransportInbound::onDetach (VcaPeer *pPeer) {
    onActivitySuccess ();
}

void Vca::VcaTransportInbound::setPeerFrom (VcaSerializerForPlumbing *pSerializer) {
    pSerializer->makePeer (m_pPeer);
}

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void Vca::VcaTransportInbound::startMessage () {
    m_pConnection->incrementIncomingMessageCount ();

    (new VcaSerializerForMessage (this))->resume ();
}

void Vca::VcaTransportInbound::startNextSerializer () {
    m_pPeer->startNextSerializerOn (this);
}

void Vca::VcaTransportInbound::wrapup (VcaSerializer *pSerializer) {
    pSerializer->wrapupIncomingSerialization ();
}


/**********************
 **********************
 *****  Shutdown  *****
 **********************
 **********************/

/******************************************************************************
 * Routine: VcaTransportInbound::Close
 *  This method 
 *    1. Closes and releases the BSProducer 
 *****************************************************************************/ 

void Vca::VcaTransportInbound::Close () {
    defaultLogger().printf ("+++ VcaTransportInbound[%p]::Close\n", this);
    if (m_pBS) {
	m_pBS->Close ();
	m_pBS.clear ();
    }
    m_pConnection->detach (this);
}

void Vca::VcaTransportInbound::Abort () {
    defaultLogger().printf ("+++ VcaTransportInbound[%p]::Abort\n", this);
    if (m_pBS) {
	m_pBS->Abort ();
	m_pBS.clear ();
    }
    m_pConnection->detach (this);
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::VcaTransportOutbound  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaTransportOutbound::VcaTransportOutbound (
    VcaConnection *pVcaConnection, BS *pBS 
) : VcaTransport (pVcaConnection), m_pBS (pBS), m_sHeartBeatStall (0), m_pITrigger (this) {
    retain (); {
	m_pBSManager.setTo (new BSManager (this));
	m_pBS->registerManager (m_pBSManager);
    } untain ();
    traceInfo ("Creating VcaTransportOutbound");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaTransportOutbound::~VcaTransportOutbound () {
    m_pConnection->detach (this);
    traceInfo ("Destroying VcaTransportOutbound");
}


/*******************************
 *******************************
 *****  Export Management  *****
 *******************************
 *******************************/

bool Vca::VcaTransportOutbound::addExport (VcaSite *pObject, SSID &rSSID) {
    unsigned int xElement;
    bool bAdded = m_iSiteTable.Insert (pObject, xElement);
    if (bAdded)
	m_iSiteTable[xElement].generateSSID ();
    rSSID = m_iSiteTable[xElement];
    return bAdded;
}

bool Vca::VcaTransportOutbound::addExport (VTypeInfo *pObject, SSID &rSSID) {
    unsigned int xElement;
    bool bAdded = m_iTypeTable.Insert (pObject, xElement);
    if (bAdded)
	m_iTypeTable[xElement].generateSSID ();
    rSSID = m_iTypeTable[xElement];
    return bAdded;
}


/****************************************
 ****************************************
 *****  Byte Stream Client Support  *****
 ****************************************
 ****************************************/

void Vca::VcaTransportOutbound::OnTransfer (IBSClient *pRole, size_t sTransfer) {

    m_pConnection->incrementOutgoingByteCount (sTransfer);

    displayDataAtTransferPoint (m_pInTransitDatum.consumerArea (), sTransfer);
    m_pInTransitDatum.consume (sTransfer);

    if (transferDeficit () == 0)
	resumeSerialization (IsNil (pRole));
    else if (IsNil (pRole) && m_pInTransitSerializer.isntNil ()) 
	m_pInTransitSerializer->suspend ();
}

void Vca::VcaTransportOutbound::OnError (
    IClient *pRole, IError *pError, VString const &rMessage
) {
    if (m_pBS) {
	defaultLogger().printf ("+++ VcaTransportOutbound[%p]::OnError %s\n", this, rMessage.content ());

//	onActivityError (pError, rMessage);
	abortSerialization ();
    }
}


/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

// Sets the data and data size to bVcae transferred out and initiates
// the data transfer

void Vca::VcaTransportOutbound::putData (
    VcaSerializer *pSerializer, void const *pData, size_t sData
) {
    m_pInTransitSerializer.setTo (pSerializer);
    m_pInTransitDatum.setTo (pData, sData);

    if (m_pBS.isNil ())
	OnTransfer (0, 0);
    else {
	IBSClient::Reference pClientRole;
	getRole (pClientRole);
	m_sInTransitMessage += sData;
	OnTransfer (0, m_pBS->put (pClientRole, pData, sData));
    }
}

void Vca::VcaTransportOutbound::transferDataFor (VcaSerializer *pSerializer) {
    pSerializer->putData ();
}


/**********************
 **********************
 *****  Plumbing  *****
 **********************
 **********************/

void Vca::VcaTransportOutbound::onAttach (VcaPeer *pPeer) {
    onActivityStart ();
    startHeartBeat ();
}

void Vca::VcaTransportOutbound::onDetach (VcaPeer *pPeer) {
    onActivitySuccess ();
}

void Vca::VcaTransportOutbound::setPeerFrom (VcaSerializerForPlumbing *pSerializer) {
    pSerializer->findPeer (m_pPeer);
}

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void Vca::VcaTransportOutbound::startMessage (VMessage *pMessage) {
    if (m_pBS) {
	Message::Reference pInTransitMsg (new Message (this, pMessage, m_pBS->bytesRequested ()+1));
	m_sInTransitMessage = 0;
	m_pInTransitMessage.setTo (pInTransitMsg);
	enqueue (pInTransitMsg);

	m_iLastMessageTick.setToNow ();
	m_pConnection->incrementOutgoingMessageCount ();
	(new VcaSerializerForMessage (this, pMessage))->resume ();
    }
}

void Vca::VcaTransportOutbound::startNextSerializer () {
    m_pPeer->startNextSerializerOn (this);
}

void Vca::VcaTransportOutbound::wrapup (VcaSerializer *pSerializer) {
    pSerializer->wrapupOutgoingSerialization ();
}


/*********************************
 *********************************
 *****  BSManager Callbacks  *****
 *********************************
 *********************************/

/****
 * Note: When BS acknowledges that it has transferred data, we update
 *       the InTransitMessage queue to reflect the current status
 ****/

void Vca::VcaTransportOutbound::onBytesTransferred () {
    InTransitMessage::Reference pMessage (m_pInTransitMsgHead);
    while (pMessage && pMessage->isSizeSet () && m_pBS && m_pBS->bytesAcked () >= pMessage->startByteOffset ()+pMessage->size ()-1) {
	VMessage::Reference const pMsg (pMessage->message ());
	pMsg->onSent ();
	dequeue (pMessage);
	pMessage.setTo (m_pInTransitMsgHead);
    }
}

/****
 * Note:
 *	If an outbound transport fails, all messages that haven't been
 *	acknowledged must be scheduled for retransmission to the peer.
 ****/

void Vca::VcaTransportOutbound::onFailedOrClosed () {
    VcaSite::Reference const pSite (peer ());
    if (pSite) {
	InTransitMessage::Reference pMsg;
	while (dequeue (pMsg)) {
	    VMessage::Reference const pMessage (pMsg->message ());
	    pSite->evaluateOutgoing (pMessage);
	}
    }
}

void Vca::VcaTransportOutbound::onChange (U32 sChange) {
    InTransitMessage::Reference pMsg (m_pInTransitMsgHead);
    while (pMsg) {
	pMsg->decrementStartByteOffset (sChange);
	pMsg.setTo (pMsg->m_pSuccessor);
    }
}


/****
 * Note: In certain cases, there is a possibility of onBytesTransferred called 
 *       from BS failing to dequeue messages, if that call happens before the message size is set
 *       To overcome that lost update, we have an extra call to onBytesTransferred once
 *       serialization of a message is complete.
 ****/

void Vca::VcaTransportOutbound::onWrapupSerialization () {
    m_pInTransitMessage->setSize (m_sInTransitMessage);
    m_pInTransitMessage.clear ();
    onBytesTransferred ();
}


/***********************
 ***********************
 *****  HeartBeat  *****
 ***********************
 ***********************/

void Vca::VcaTransportOutbound::startHeartBeat () {
    if (m_pHeartBeatTimer)
	m_pHeartBeatTimer->restart ();
    else {
	ITrigger::Reference pTrigger;
	getRole (pTrigger);

	VString sTimerPurpose;
	sTimerPurpose.printf ("Connection %p Heartbeat", m_pConnection.referent());

	m_pHeartBeatTimer.setTo (new VTimer (VCohort::Vca (), sTimerPurpose, pTrigger, HEARTBEAT_VALUE, true));

	m_pHeartBeatTimer->start ();
    }
}

void Vca::VcaTransportOutbound::stopHeartBeat () {
    VTimer::Reference pHeartBeatTimer;
    pHeartBeatTimer.claim (m_pHeartBeatTimer);

    if (pHeartBeatTimer)
	pHeartBeatTimer->cancel ();
}

/*******************************
 **** Vca::ITrigger Methods ****
 *******************************/

/****
 * Heartbeat Notes:
 *      Each VcaTransportOutbound has a Heartbeat sent periodically via its channel to detect errors
 *      proactively. For this, a VMessage object for a "IPeer->SinkInterface (0)" method invocation
 *      is created explicitly and invoked through "this" transport.
 *
 *      This routine sends a heartbeat only if no messages have been sent in the past HEARTBEAT_VALUE
 *	microseconds.
 ****/

void Vca::VcaTransportOutbound::Process (ITrigger *pRole) {
//  generate a heart beat if one is required....
    if (isConnected () && plumbed ()) {
	V::VTime const iCurrentTick;
	S64 const uSecondsSinceLastMessage = iCurrentTick - m_iLastMessageTick;
	if (uSecondsSinceLastMessage >= HEARTBEAT_VALUE) {
	    m_sHeartBeatStall = 0;

	    IPeer::Reference pPeer;
	    m_pPeer->getReflection (pPeer);

	    VMessageScheduler iScheduler;
	    if (pPeer && pPeer->defersTo (iScheduler) && m_pPeer->getSpecificFreeOutboundTransport (this)) {
		VMessage::Reference const pMessage (
		    new IPeer::VINTERFACE_MEMBERTYPE (SinkInterface)::Message (
			iScheduler, &IPeer::VINTERFACE_MEMBERINFO (SinkInterface), pPeer, 0, 0
		    )
		);
		startMessage (pMessage);
	    } else if (shutdownInProgress () && uSecondsSinceLastMessage >= 10 * HEARTBEAT_VALUE) {
		defaultLogger().printf ("+++ VcaTransportOutbound[%p]: Heartbeat aborting stalled shutdown.\n", this);
		Abort ();
	    } else if (0 == m_sHeartBeatStall++) {
		defaultLogger().printf ("+++ VcaTransportOutbound[%p]: Heartbeat transport busy.\n", this);
	    }
	}
    }
}


/**********************
 **********************
 *****  Shutdown  *****
 **********************
 **********************/

void Vca::VcaTransportOutbound::Close () {
    defaultLogger().printf ("+++ VcaTransportOutbound[%p]::Close\n", this);
    stopHeartBeat ();
    if (m_pBS) {
        m_pBS->unregisterManager (m_pBSManager);
	m_pBS->Close ();
	m_pBS.clear ();
    }
    m_pConnection->detach (this);
}

void Vca::VcaTransportOutbound::Abort () {
    defaultLogger().printf ("+++ VcaTransportOutbound[%p]::Abort\n", this);
    stopHeartBeat ();
    if (m_pBS) {
        m_pBS->unregisterManager (m_pBSManager);
	m_pBS->Abort ();
	m_pBS.clear ();
    }
    m_pConnection->detach (this);
}
