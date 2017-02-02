/*****  Vca::VBS Implementation  *****/

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

#include "Vca_VBS.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#include "Vca_VBSManager.h"
#include "Vca_ITrigger.h"

#include "Vca_VStatus.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VCheckPoint  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCheckPoint::VCheckPoint (size_t sCheckPoint, bool bBlocking = false, 
			       ITrigger *pTrigger = 0
)   : m_sCheckPoint (sCheckPoint)
    , m_bBlocking (bBlocking)
    , m_pTrigger  (pTrigger) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/


Vca::VCheckPoint::~VCheckPoint () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VCheckPoint::decrementSize (size_t sTransfer) {
    if (m_sCheckPoint >= sTransfer)
	m_sCheckPoint -= sTransfer;
    else 
	log ("ERROR in Vca::VCheckPoint::decrementSize (size_t) - Invalid data transfer amount...");

}

/*********************
 *********************
 *****  Trigger  *****
 *********************
 *********************/

void Vca::VCheckPoint::trigger () {
    if (m_pTrigger) {
        m_pTrigger->Process ();
	m_pTrigger.clear ();
    }
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VBS::ExportStub  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBS::ExportStub::ExportStub (VBS *pBS) : m_pBS (pBS), m_pIBSClient (this) {
}

/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VBS::ExportStub::get (Void &rVoid, size_t sMinimum) {
    IBSClient::Reference pClient;
    getRole (pClient);
    onTransfer (m_pBS->get (pClient, rVoid, sMinimum));
}

void Vca::VBS::ExportStub::put (Data &rData) {
    IBSClient::Reference pClient;
    getRole (pClient);
    onTransfer (m_pBS->put (pClient, rData));
}


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VBS::Request  *****
 *****                     *****
 *******************************
 *******************************/

/***************************
 ***************************
 *****  Communication  *****
 ***************************
 ***************************/

void Vca::VBS::Request::onEnd () const {
    if (m_pClient)
	m_pClient->OnEnd ();
}

void Vca::VBS::Request::onError (IError *pError, VString const &rMessage) const {
    if (m_pClient)
	m_pClient->OnError (pError, rMessage);
}

bool Vca::VBS::Request::onStatus (VStatus const &rStatus) const {
    return rStatus.communicateTo (m_pClient);
}

bool Vca::VBS::Request::wrapupGet (size_t sOriginalArea, size_t &rsTransfer) const {
    rsTransfer = sOriginalArea - fifoProducerAreaSize ();
    return fifoSatisfiesGet ();
}

bool Vca::VBS::Request::wrapupPut (size_t sOriginalArea, size_t &rsTransfer) const {
    rsTransfer = sOriginalArea - fifoConsumerAreaSize ();
    return fifoSatisfiesPut ();
}

bool Vca::VBS::Request::wrapupGet (
    size_t sOriginalArea, size_t &rsTransfer, IBSClient::Reference &rpClient
) const {
    rpClient.setTo (m_pClient);
    return wrapupGet (sOriginalArea, rsTransfer);
}

bool Vca::VBS::Request::wrapupPut (
    size_t sOriginalArea, size_t &rsTransfer, IBSClient::Reference &rpClient
) const {
    rpClient.setTo (m_pClient);
    return wrapupPut (sOriginalArea, rsTransfer);
}


/**********************
 **********************
 *****            *****
 *****  Vca::VBS  *****
 *****            *****
 **********************
 **********************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

void Vca::VBS::setGlobalBufferSizeTo (size_t sLimit) {
    g_sBufferLimit = sLimit;
}

void Vca::VBS::setGlobalProxyTransferSizeTo (size_t sLimit) {
    g_sProxyTransfer = sLimit;
}

size_t Vca::VBS::g_sBufferLimit (4096);
size_t Vca::VBS::g_sProxyTransfer (UINT_MAX);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBS::VBS (VCohort *pCohort)
: BaseClass 			(pCohort)
, Schedulable			(this)
, m_pActiveFifo			(0)
, m_pBuffer			(0)
, m_sBuffer			(0)
, m_sBufferLimit		(UINT_MAX)
, m_sBytesRequested             (0)
, m_sBytesAcked                 (0) 
, m_bUnbuffered                 (false)
, m_bWaitingToClose		(false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBS::~VBS () {
    if (m_pBuffer)
	deallocate (m_pBuffer);
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VBS::incrementBytesRequested (Vca::U32 sTransfer) {
  checkForByteCountOverflow (m_sBytesRequested, sTransfer);
  m_sBytesRequested += sTransfer;
}

void Vca::VBS::incrementBytesAcked (Vca::U32 sAcked) {
  checkForByteCountOverflow (m_sBytesAcked, sAcked);
  m_sBytesAcked += sAcked;
}

/*****
 * Note: When incrementing ByteCounts...if it exceeds a threshold, then we scale down the byte count values
 *       and also indicate the manager (indirectly transports) about the rescaling 
 *       so that it updates its structures (list of messages) which would have used these bytecounts.
 *       Here we have used 4 billion as threshold for Vca::U32 datatype which can store a max of
 *       4294967296.
 *****/

void Vca::VBS::checkForByteCountOverflow (Vca::U32 sCurrent, Vca::U32 sBytes) {
  Vca::U32 const sThreshold = 4000000000U;
  if (sCurrent >= sThreshold - sBytes) {
    
    Vca::U32 sAdjustment = m_sBytesAcked < m_sBytesRequested ? m_sBytesAcked : m_sBytesRequested;
    m_sBytesRequested -= sAdjustment;
    m_sBytesAcked -= sAdjustment;

    if (isManaged ()) 
      m_pManager->onChange (sAdjustment);
  }
}


/**********************
 **********************
 *****  Queueing  *****
 **********************
 **********************/

void Vca::VBS::schedule_() {
    cohort ()->schedule (this);
}

bool Vca::VBS::startRequest (VCohortClaim const &rClaim) {
    if (rClaim.isHeld ())
	return true;

    resume ();

    return false;
}

bool Vca::VBS::startRequest (
    VCohortClaim const &rClaim, IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum
) {
    Request iRequest (pClient, pVoid, sMinimum, sMaximum);
    return m_iQueue.enqueue (iRequest) && startRequest (rClaim);
}

bool Vca::VBS::startRequest (
    VCohortClaim const &rClaim, IBSClient *pClient, void const *pData, size_t sData
) {
    Request iRequest (pClient, pData, sData);
    return m_iQueue.enqueue (iRequest) && startRequest (rClaim);
}

bool Vca::VBS::nextRequest () {
    return m_iQueue.dequeue ();
}


/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

/************************
 *****  Initiation  *****
 ************************/

size_t Vca::VBS::get (IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum) {
    VCohortClaim iCohortClaim (this, false);

//  Schedule the request, ...
    size_t sTransfer = 0;
    incrementBytesRequested (sMaximum);   

    if (startRequest (iCohortClaim, pClient, pVoid, sMinimum, sMaximum)) {

    //  ... transfer data if we're at the front of the queue, ...
	transferData ();

    //  ... determine what we're completed, ...
	wrapupGet (sMaximum, sTransfer);
    }

//  ... and return the amount of data transfered:
    return sTransfer;
}

size_t Vca::VBS::put (IBSClient *pClient, void const *pData, size_t sData) {
    VCohortClaim iCohortClaim (this, false);

//  Schedule the request, ...
    size_t sTransfer = 0;
    incrementBytesRequested (sData);     
   
    if (startRequest (iCohortClaim, pClient, pData, sData)) {

    //  ... transfer data if we're at the front of the queue, ...
	transferData ();

    //  ... determine what we've completed, ...
	wrapupPut (sData, sTransfer);
    }

//  ... and return the amount of data transfered:
    return sTransfer;
}

bool Vca::VBS::putBufferedData () {
    size_t sTransfer = 0;

    VCohortClaim iCohortClaim (this, false);
    if (iCohortClaim.isHeld ()) {
	bool bUnbuffered = m_bUnbuffered;
	m_bUnbuffered = true;
	sTransfer = onPutContinuation ();
	m_bUnbuffered = bUnbuffered;
    }

    return sTransfer > 0;
}


/*******************************
 *****  Buffer Management  *****
 *******************************/

bool Vca::VBS::buffering (size_t sTransfer) {
//  If data is already buffered, a buffered operation must consume it, ...
    if (m_iBufferFifo.consumerAreaIsntEmpty ())
	return true;

//  Otherwise, transfers larger than the buffer size limit are unbuffered, ...
    size_t sBuffer = bufferSize ();
    if (sTransfer >= sBuffer || unbuffered ())
	return false;

//  ... and all others are processed using an appropriately sized buffer:
    if (!m_pBuffer || m_sBuffer != sBuffer) { 
	if (m_pBuffer)
	    deallocate (m_pBuffer);
	m_pBuffer = allocate (sBuffer);
	m_sBuffer = sBuffer;
    }

    m_iBufferFifo.setTo (m_pBuffer, m_sBuffer);

    return true;
}


/*---------------------------------------------------------------------------
 *  Notes:
 *	Called from Vca::VBSConsumer subclasses (i.e., 'put' implementations).
 *****/
bool Vca::VBS::getArea (char const *&rpData, size_t &rsData) {
//  Only one area at a time can be active, ...
    if (busy ()) 
	return false;

//  Set up the next transfer if one isn't active, ...
    Fifo *pNextFifo = 0;
    if (!buffering (activeRequestConsumerAreaSize ()))
	pNextFifo = &activeRequestFifo ();
    else {
	activeRequest().moveDataTo (m_iBufferFifo);
	pNextFifo = &m_iBufferFifo;
    }

/*---------------------------------------------------------------------------*
 *  The intent of the following test is the preventation of empty and
 *  premature writes.  Empty writes are obvious - if there's no fifo
 *  (consumer area) content, there is nothing to write.  Deferrable
 *  (premature) writes occur when data is being staged in the internal
 *  buffer.  Technically, this requires that the test be written as
 *  'pNextFifo == &m_iBufferFifo && pNextFifo->producerAreaIsntEmpty...';
 *  however, in this case, the first part of the test can be omitted
 *  because the producer area of a client buffer is always empty since
 *  only the internal buffer can have empty space in its producer area:
 *	(Client fifo=>empty producer area)
 *		    <=>
 *	(non-empty producer area => not client (a.k.a. internal) fifo)
 *---------------------------------------------------------------------------*/
//  ... suppressing empty and deferrable transfers, ...
    if (isBlocked () || pNextFifo->consumerAreaIsEmpty () ||
	pNextFifo->producerAreaIsntEmpty () && !unbuffered ()
    ) {
	return false;
    }

//  ... and allowing all others:
    rpData = pNextFifo->consumerArea ();
    rsData = pNextFifo->consumerAreaSize ();

    size_t sCheckPoint = 0;
    if (isCheckPointed (sCheckPoint) && rsData > sCheckPoint)
	rsData = sCheckPoint;

    m_pActiveFifo = pNextFifo;

    return true;
}


/*---------------------------------------------------------------------------
 *  Notes:
 *	Called from Vca::VBSProducer subclasses (i.e., 'get' implementations)
 *****/
bool Vca::VBS::getArea (char *&rpVoid, size_t &rsVoid) {
//  Only one area at a time can be active, ...
    if (busy ())
	return false;

    activeRequest().moveDataFrom (m_iBufferFifo);

//  ... suppressing read ahead.. if client requirement is fulfilled
    if (activeRequestFifoSatisfiesGet ()) 
	return false;

//  Set up the next transfer if one isn't active, ...
    Fifo *pNextFifo = buffering (
	activeRequestProducerAreaSize ()
    ) ? &m_iBufferFifo : &activeRequestFifo ();

//  ... suppressing empty and deferrable transfers, ...
    if (pNextFifo->producerAreaIsEmpty ()) 
	return false;	//  ... empty:  no room for more data

    rpVoid = pNextFifo->producerArea ();
    rsVoid = pNextFifo->producerAreaSize ();

    m_pActiveFifo = pNextFifo;

    return true;
}


/*************************************
 *****  Completion/Continuation  *****
 *************************************/

void Vca::VBS::onGetCompletion (size_t sTransfer) {
    incrementBytesAcked (sTransfer);
    m_pActiveFifo->produce (sTransfer);
    m_pActiveFifo = 0;

    if (isManaged ()) 
	m_pManager->onTransfer (sTransfer);
}

size_t Vca::VBS::onGetContinuation () {
/*-----------------------------------------------------------------------*
 *  This routine is called exclusively by event handlers.  Because the
 *  events being processed may hold the last references to this stream,
 *  it is possible that the stream may be destroyed before this routine
 *  has finished its business.  To prevent that, a protective reference
 *  is created here to defer that collection until after the last event
 *  has been processed and control returned to this scope.
 *-----------------------------------------------------------------------*/
    Reference const iRetainer (this);

    size_t sTotalTransfer = 0;
    size_t sVoid;
    do {
	sVoid = activeRequestProducerAreaSize ();
	transferData ();
    } while (onGetContinuation (sVoid, sTotalTransfer));

    if (m_bWaitingToClose)
	Close ();

    return sTotalTransfer;
}

bool Vca::VBS::onGetContinuation (size_t sOriginalArea, size_t &rsTotalTransfer) {
    size_t sTransfer = 0; IBSClient::Reference pClient;
    bool bDoneWithThis = !active () || activeRequest().wrapupGet (
	sOriginalArea, sTransfer, pClient
    );

    rsTotalTransfer += sTransfer;

    if (pClient && sTransfer > 0)
	pClient->OnTransfer (sTransfer);

    return bDoneWithThis && nextRequest ();
}

/******************************************************************************
 * Routine: onPutCompletion
 *	After updating the active fifo on the data transfer size. Updates
 * the checkpoints (if any) on the data transfer and processes them..
 ******************************************************************************/

void Vca::VBS::onPutCompletion (size_t sTransfer) {
    incrementBytesAcked (sTransfer);
    m_pActiveFifo->consume (sTransfer);
    m_pActiveFifo = 0;

    if (isManaged ()) 
	m_pManager->onTransfer (sTransfer);

    if (isCheckPointed ()) {
	updateCheckPoints (sTransfer);
	processCheckPoint ();
    }
}   

size_t Vca::VBS::onPutContinuation () {
/*-----------------------------------------------------------------------*
 *  This routine is called exclusively by event handlers.  Because the
 *  events being processed may hold the last references to this stream,
 *  it is possible that the stream may be destroyed before this routine
 *  has finished its business.  To prevent that, a protective reference
 *  is created here to defer that collection until after the last event
 *  has been processed and control returned to this scope.
 *-----------------------------------------------------------------------*/
    Reference const iRetainer (this);

    size_t sTotalTransfer = 0;
    size_t sData;
    do {
	sData = activeRequestConsumerAreaSize ();
	transferData ();
    } while (onPutContinuation (sData, sTotalTransfer));

    if (m_bWaitingToClose)
	Close ();

    return sTotalTransfer;
}

bool Vca::VBS::onPutContinuation (size_t sOriginalArea, size_t &rsTotalTransfer) {
    size_t sTransfer = 0; IBSClient::Reference pClient;
    bool bDoneWithThis = !active () || activeRequest().wrapupPut (
	sOriginalArea, sTransfer, pClient
    );

    rsTotalTransfer += sTransfer;

    if (pClient && sTransfer > 0)
	pClient->OnTransfer (sTransfer);

    return bDoneWithThis && nextRequest ();
}


/***************************
 ***************************
 *****  Communication  *****
 ***************************
 ***************************/

void Vca::VBS::onEnd () {
    activeRequest().onEnd ();
    m_pActiveFifo = 0;
    nextRequest ();
}

void Vca::VBS::onError (IError *pError, VString const &rMessage) {
    activeRequest().onError (pError, rMessage);
    m_pActiveFifo = 0;
    nextRequest ();
}


/*****
 * Note: Vca::VBS::onStatus () calls on Request/Manager objects
 *       can result in the Transport being aborted/closed
 *       resulting in the destruction of "this" VBS object. Hence
 *       the retain/release protection.
 *	 
 *       If an error has happened, then if there is an active 
 *       request, propagate the error to it, else if there
 *       is a manager associated with the VBS, 
 *       then propagate the error to the manager. If both
 *       (active request and manager) are nil, then Abort the BS.
 ****/

void Vca::VBS::onStatus (VStatus const &rStatus) {
    Reference const iRetainer (this);

    if (activeRequest ().onStatus (rStatus)) {
	m_pActiveFifo = 0;
	nextRequest ();
    } else if (isManaged () && m_pManager->onStatus (rStatus)) {
	m_pActiveFifo = 0;
	nextRequest ();
    } else if (rStatus.isClosed () || rStatus.isFailed ()) {
	Abort ();
    }
}

void Vca::VBS::wrapupGet (size_t sOriginalArea, size_t &rsTransfer) {
    if (activeRequest().wrapupGet (sOriginalArea, rsTransfer))
	nextRequest ();
}

void Vca::VBS::wrapupPut (size_t sOriginalArea, size_t &rsTransfer) {
    if (activeRequest().wrapupPut (sOriginalArea, rsTransfer))
	nextRequest ();
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/******************************************************************************
 * Routine: Close
 *	When performing a graceful Close on the VBS if the data transfer is 
 * still in progress then the Close is temporarily suspended using a boolean flag.
 * Later again when data is transferred Close operation is tried again.
 ******************************************************************************/

void Vca::VBS::Close () {
    if (closeDeferred ()) {
	if (objectsTrace ()) traceInfo (
	    string ("%s::Close: Deferred %u", rttName ().content (), transferPending ())
	);
	if (!m_bWaitingToClose) {
	    m_bWaitingToClose = true;
	    retain ();
	}
    } else {
	if (objectsTrace ()) traceInfo (
	    string ("%s::Close: %u%s", rttName ().content (), referenceCount (), m_bWaitingToClose ? "W":"")
	);
	Abort ();
    }
}

/******************************************************************************
 * Routine: Abort
 *   Abort is forceful closing of the connection. Hence endTransfers () 
 * is called directly.
 ******************************************************************************/

void Vca::VBS::Abort () {
    endTransfers ();
    if (m_bWaitingToClose) {
	m_bWaitingToClose = false;
	release ();
    }
}


/************************
 ************************
 *****  BS Manager  *****
 ************************
 ************************/

void Vca::VBS::registerManager (VBSManager *pManager) {
    m_pManager.setTo (pManager);
}

bool Vca::VBS::unregisterManager (VBSManager *pManager) {
    return m_pManager.clearIf (pManager);
}

/*******************************
 *******************************
 *****  VBS Checkpointing  *****
 *******************************
 *******************************/

bool Vca::VBS::isCheckPointed () const {
    return m_pCheckPointHead.isntNil ();
}

bool Vca::VBS::isBlocked () const {
    return isCheckPointed () && m_pCheckPointHead->size () == 0  && m_pCheckPointHead->isBlocking ();
}

bool  Vca::VBS::isntBlocked () const {
    return !isBlocked ();
}

/******************************************************************************
 * Routine:  setCheckPoint
 *	Inserts the checkpoint in the queue and also calls the process function
 * to check for any processeable checkpoints. e.g. if the checkpoint being 
 * inserted has a size zero and the queue is empty then the inserted checkpoint
 * is processed immediately
******************************************************************************/

void Vca::VBS::setCheckPoint (VCheckPoint *pCheckPoint) {
    queueCheckPoint (pCheckPoint);
    processCheckPoint ();
}

void Vca::VBS::updateCheckPoints (size_t sTransfer) {

    VCheckPoint::Reference pCheckPoint (m_pCheckPointHead);
    while (pCheckPoint.isntNil ()) {
	pCheckPoint->decrementSize (sTransfer);
	pCheckPoint.setTo (pCheckPoint->m_pSuccessor);
    }
}

/******************************************************************************
 * Routine:  processCheckPoint
 *	This routine gets the first head checkpoint and if its size is zero
 * then triggers it. (If it is a non blocking CP then it is also removed from
 * the queue)
******************************************************************************/

void Vca::VBS::processCheckPoint () {
    VCheckPoint::Reference pCheckPoint (m_pCheckPointHead);
    if (pCheckPoint.isntNil () && pCheckPoint->size () == 0) {
	if (pCheckPoint->isntBlocking ()) 
	    dequeueCheckPoint ();
	pCheckPoint->trigger ();
    }
}

/******************************************************************************
 * Routine: releaseBlockingCheckPoint
 *  If the BS is blocked then removes the blocking CheckPoint and 
 *  triggers to process the next checkpoint (if possible)
******************************************************************************/

void Vca::VBS::releaseBlockingCheckPoint () {
    if (isBlocked ()) {
	dequeueCheckPoint ();
	processCheckPoint ();
    }
    else
	log ("ERROR in Vca::VBS::releaseBlockingCheckPoint - Lost a releaseBlockingCheckPoint callback");	    
}

bool Vca::VBS::isCheckPointed (size_t &rCheckPointSize) const {
    if (isCheckPointed ()) {
	rCheckPointSize = m_pCheckPointHead->size ();
	return true;
    }
    return false;
}

void  Vca::VBS::queueCheckPoint (VCheckPoint *pCheckPoint) {
    if (m_pCheckPointHead.isNil ()) {
	m_pCheckPointHead.setTo (pCheckPoint);
	m_pCheckPointTail.setTo (pCheckPoint);
    }
    else {
	m_pCheckPointTail->m_pSuccessor.setTo (pCheckPoint);
	pCheckPoint->m_pPredecessor.setTo (m_pCheckPointTail);
	m_pCheckPointTail.setTo (pCheckPoint);
    }    
}

void  Vca::VBS::dequeueCheckPoint () {
    if (m_pCheckPointHead.isntNil ()) {
	m_pCheckPointHead.setTo  (m_pCheckPointHead->m_pSuccessor);
	if (m_pCheckPointHead.isntNil ())
	    m_pCheckPointHead->m_pPredecessor.clear ();
    
	if (m_pCheckPointHead.isNil ())
	    m_pCheckPointTail.setTo (m_pCheckPointHead);
    }
}



void Vca::VBS::displayDataAtTransferPoint (
    char const *pData, size_t sData, char const *pDir
) const {
    if (sData > 0) {

	fprintf (stderr, "[Modified] %s %p:", pDir, this);
	char iBuffer[20 * 3 + 1];
	char *pIP = iBuffer;
	for (unsigned int xByte = 0; xByte < sData; xByte++) {
	    if (0 == xByte % 20) {
		if (iBuffer == pIP)
		    fprintf (stderr, "\n   ");
		else {
		    *pIP = 0;
		    fprintf (stderr, " <%s>\n   ", iBuffer);
		    pIP = iBuffer;
		}
	    }
	    unsigned char iChar = pData[xByte];
	    fprintf (stderr, " %02x", 0xff & iChar);
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
	fprintf (stderr, " <%s>\n", iBuffer);
	fflush (stderr);
    }
}
