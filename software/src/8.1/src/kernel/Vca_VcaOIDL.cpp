/*****  Vca::VcaOIDL Implementation  *****/

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

#include "Vca_VcaOIDL.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "IVUnknown.h"

#include "Vca_VcaPeer.h"
#include "Vca_VcaSelf.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VcaOIDL::Evaluator  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDL::Evaluator::Evaluator (VcaOIDL *pOIDL, VcaSite *pSite)
    : m_pOIDL			(pOIDL)
    , m_pSite			(pSite)
    , m_pSiteListSuccessor	(pSite->m_pEvaluators)
    , m_pMessagesToAck		(&m_pMessages)
    , m_pMessagesToRun		(&m_pMessages)
    , m_xNextMessage		(0)
{
    if (m_pSiteListSuccessor)
	m_pSiteListSuccessor->m_pSiteListPredecessor = this;
    m_pSite->m_pEvaluators = this;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOIDL::Evaluator::~Evaluator () {
    removeFromSiteList ();
}

/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaOIDL::Evaluator::getInfo (VString &rResult, VString const &rPrefix) const {
    rResult << rPrefix << "Evaluator: "; {
	VkUUIDHolder const iUUID (m_pSite->uuid ());
	iUUID.GetString (rResult);
    }
    rResult.printf (", NextSeq: %u", m_xNextMessage);
}


/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

/****************************************************************************
 * Routine: evaluate
 * This method checks the sequence number of the the message to determine
 * if it the next one expected.  If it is, it is evaluated along with
 * any messages in the queue blocked by its absence.  If the message has
 * a sequence number greater than that of the next message expected, it is
 * queued, or, if it has a sequence number less than or equal to the last
 * message evaluated, it is discarded.
 *
 * When a message is evaluated, the following steps are performed:
 * 1. Evaluate the message.
 * 2. Bookkeeping (incrementing various counters).
 * 3. Loop until no more messages can be evaluated.
 *
 ****************************************************************************
 *  This routine has been written to anticipate message acknowledgments.
 *  See the discussion attached to the implementations of 'addToList' and 
 *  'successor' to understand how this double indirection works.
 ****************************************************************************/

unsigned int Vca::VcaOIDL::Evaluator::evaluate (VMessage *pMessage) {
//  null guard...
    if (!pMessage)
	return 0;

//  message sequence number bug fix...
    if (pMessage->sequenceNumber () == 0 && m_xNextMessage != 0){
        VString iString;
        iString.printf ("Message Sequence Number Reset: Was %u",m_xNextMessage);
        m_pOIDL->logAsInteresting (iString);
	m_xNextMessage = 0;
    }

//  message accounting
    count_t const xFirstMessage = m_xNextMessage;

//  accept only new messages, silently discarding old ones...
    if (m_xNextMessage <= pMessage->sequenceNumber ()) {
	pMessage->addToList (m_pMessagesToRun);
	while (VMessage::SequenceNumber (*m_pMessagesToRun) == m_xNextMessage) {
	    (*m_pMessagesToRun)->evaluate (m_pMessagesToRun);
	    m_xNextMessage++;
	}
    } else {
        VString iString;
        iString.printf ("Message Discard: Expecting %u, Discarding %u", m_xNextMessage, pMessage->sequenceNumber ());
        m_pOIDL->logAsInteresting (iString);
    }


//  here's how to collect acknowledgment data
    while (m_pMessagesToAck != m_pMessagesToRun) {
	// (*m_pMessagesToAck)->acknowledge (); or something smarter!!!
	m_pMessagesToAck = VMessage::Successor (*m_pMessagesToAck, m_pMessages);
    }

//  adjust the lists to reclaim the last acknowledged message:
    m_pMessages.setTo (*m_pMessagesToAck);
    m_pMessagesToAck = m_pMessagesToRun = &m_pMessages;

    if (*m_pMessagesToRun) {
        VString iString;
        iString.printf ("Message Gap: Expecting %u Received %u", m_xNextMessage, (*m_pMessagesToRun)->sequenceNumber ());
        m_pOIDL->logAsInteresting (iString);
    }

    return m_xNextMessage - xFirstMessage;
}

void Vca::VcaOIDL::Evaluator::onDefunctPeer () {
    removeFromSiteList ();

    m_pOIDL->detach (this);
//  No member access beyond this point - 'this' may have just been reclaimed !!!

#ifdef TracingVcaMemoryLeaks
    log("PoolMemoryLeak VcaOIDL::Evaluator::onDefunctPeer() referenceCount: %u ", referenceCount());
#endif
}

void Vca::VcaOIDL::Evaluator::removeFromSiteList () {
    if (m_pSiteListPredecessor)
	m_pSiteListPredecessor->m_pSiteListSuccessor.setTo (m_pSiteListSuccessor);
    else
	m_pSite->m_pEvaluators.setIf (m_pSiteListSuccessor, this);

    if (m_pSiteListSuccessor) {
	m_pSiteListSuccessor->m_pSiteListPredecessor.claim (m_pSiteListPredecessor);
	m_pSiteListSuccessor.clear ();
    }
    m_pSiteListPredecessor.clear();
}


/********************************
 ********************************
 *****                      *****
 *****  Vca::VcaOIDL::Vise  *****
 *****                      *****
 ********************************
 ********************************/

/********************
 ********************
 *****  Export  *****
 ********************
 ********************/

Vca::VcaOID::count_t Vca::VcaOIDL::Vise::exportCount () const {
    return m_pOIDL ? m_pOIDL->exportCount () : 0;
}

Vca::VcaOID::count_t Vca::VcaOIDL::Vise::weakExportCount () const {
    return m_pOIDL ? m_pOIDL->weakExportCount () : 0;
}
	
bool Vca::VcaOIDL::Vise::isExported () const {
    return m_pOIDL &&  m_pOIDL->isExported ();
}
bool Vca::VcaOIDL::Vise::isExportedTo (VcaPeer const *pPeer) const {
    return m_pOIDL && m_pOIDL->isExportedTo (pPeer);
}

/******************
 ******************
 *****  OIDL  *****
 ******************
 ******************/

void Vca::VcaOIDL::Vise::getOIDL (VReference<VcaOIDL>&rpOIDL) const {
    rpOIDL.setTo (m_pOIDL);
}

void Vca::VcaOIDL::Vise::getOIDL (VReference<VcaOIDL>&rpOIDL, IVUnknown *pObject) {
    if (m_pOIDL)
	rpOIDL.setTo (m_pOIDL);
    else {
	self ()->getObject (rpOIDL, pObject);
	m_pOIDL = rpOIDL;
    }
}

void Vca::VcaOIDL::Vise::getSSID (VcaSSID &rSSID) {
    if (m_pOIDL)
	rSSID = m_pOIDL->objectSSID ();
    else
	rSSID.generateSSID ();
}


/**************************
 **************************
 *****                *****
 *****  Vca::VcaOIDL  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDL::VcaOIDL (VcaSSID const &rObjectSSID, IVUnknown *pObject)
    :	VcaOID			(rObjectSSID)
    ,	m_pObject		(pObject)
    ,   m_cMessagesReceived	(0)
    ,   m_cMessagesProcessed	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOIDL::~VcaOIDL () {
    logIfInteresting ("DTOR");

//  Detach the OIDL from its site, ...
    self ()->detach (this);

//  ... and object:
    m_pObject->detach (this);
}


/***************************
 ***************************
 *****  Object Access  *****
 ***************************
 ***************************/

uuid_t const& Vca::VcaOIDL::objectSiteUUID_() const {
    return self ()->uuid ();
}

VTypeInfo *Vca::VcaOIDL::objectType_() const {
    return m_pObject->typeInfo_();
}

void Vca::VcaOIDL::supplyInterface_(VReference<IVUnknown>&rpInterface) {
    rpInterface.setTo (m_pObject);
}

/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void Vca::VcaOIDL::onFinalExport () {
#ifdef TracingVcaMemoryLeaks
    if (dynamic_cast<IPeer*>(m_pObject.referent()) != 0) {
	log("PoolMemoryLeak VcaOIDL::onFinalExport() referenceCount: %u evaluators: %u reported: %u processed: %u received: %u", 
	    referenceCount(), m_iEvaluators.cardinality(), m_cMessagesReported, m_cMessagesProcessed, m_cMessagesReceived);
    }
#endif

    if (allMessagesProcessed ()) {
	Reference const iRetainer (this);
	m_iEvaluators.DeleteAll ();
    }
}


/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

/****************************************************************************
 * Routine: evaluateIncomingFrom
 *  This routine is only called from VcaSerializerForMessage on arrival
 *  of a message. This routine gets (or creates) an export record for
 *  that peer and delegates the operation to the export record.
 ****************************************************************************/

void Vca::VcaOIDL::evaluateIncomingFrom (VMessage *pMessage, VcaSite *pSite) {

    updateMessageCounts (0, 1, 0);

    unsigned int xSite;
    if (m_iEvaluators.Insert (pSite, xSite))
	m_iEvaluators[xSite] = new Evaluator (this, pSite);
    updateMessageCounts (0, 0, m_iEvaluators[xSite]->evaluate (pMessage));

    if (importerCount () == 0)
	onFinalExport ();
}

void Vca::VcaOIDL::detach (Evaluator *pEvaluator) {
    m_iEvaluators.Delete (pEvaluator->site ());
}

/*****************************
 *****************************
 *****  Message Counts   *****
 *****************************
 *****************************/

void Vca::VcaOIDL::updateMessageCounts (count_t cReported, count_t cReceived, count_t cProcessed) {
    bool const bAllProcessedBefore = allMessagesProcessed ();
    m_cMessagesReported  += cReported;
    m_cMessagesReceived  += cReceived;
    m_cMessagesProcessed += cProcessed;

    bool const bAllProcessedNow = allMessagesProcessed ();
    if (bAllProcessedBefore != bAllProcessedNow) {
	if (bAllProcessedNow)
	    release ();
	else
	    retain ();
    }
}

/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaOIDL::displayInfo () const {
    BaseClass::displayInfo ();
    display (
	" Received: %u, Processed: %u", m_cMessagesReceived, m_cMessagesProcessed
    );
}

void Vca::VcaOIDL::getInfo (VString &rResult, const VString &rPrefix) const {
    BaseClass::getInfo (rResult, rPrefix);
    rResult << "\n" << rPrefix  << "  Interface: ";
    objectType ()->getName (rResult);
    rResult.printf (
	", Received: %u, Processed: %u", m_cMessagesReceived, m_cMessagesProcessed
    );
    if (m_iEvaluators.cardinality () > 0) {
	rResult << "\n";
	getEvaluatorTable (rResult, VString (rPrefix) << "  ");
    }
}

void Vca::VcaOIDL::getEvaluatorTable (VString &rResult, VString const &rPrefix) const {
    bool bNeedsNewLine = false;
    for (unsigned int xEvaluator = 0; xEvaluator < m_iEvaluators.cardinality (); xEvaluator++) {
	if (bNeedsNewLine)
	    rResult << "\n";
	bNeedsNewLine = true;
	m_iEvaluators[xEvaluator]->getInfo (rResult, rPrefix);
    }
}
