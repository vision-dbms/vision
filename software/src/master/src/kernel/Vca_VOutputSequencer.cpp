/*****  Vca::VOutputSequencer Implementation  *****/

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

#include "Vca_VOutputSequencer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IError.h"
#include "Vca_ITrigger.h"


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VOutputSequencer::Output  *****
 *****                                 *****
 *******************************************
 *******************************************/

/******************************
 ******************************
 *****  State Transition  *****
 ******************************
 ******************************/

void Vca::VOutputSequencer::Output::startThis () {	    // typically called from constructor of concrete class
    onStart ();
    m_pSequencer->queue (this);
}

void Vca::VOutputSequencer::Output::setStatusToAvailable () {
    if (statusIsBlocked ()) {
	m_xStatus = Status_Available;
	m_pSequencer->start (this);
    }
}

void Vca::VOutputSequencer::Output::setStatusToDone () {
    if (m_xStatus != Status_Final) {
	m_xStatus = Status_Done;
	onSuccess ();
    }
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VOutputSequencer::Output::onSequencerShutdown () {
    static VString const sErrorMessage ("Aborted By Shutdown", false);
    onFailure (0, sErrorMessage);
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VOutputSequencer::OutputPause  *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VOutputSequencer::OutputPause::OutputPause (
    Sequencer *pSequencer) : BaseClass (pSequencer, Status_Blocked
) {
    retain (); {
	startThis ();
    } untain ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VOutputSequencer::OutputPause::load (Fifo &rOutputFifo) {
    rOutputFifo.clear ();
    setStatusToDone ();
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VOutputSequencer::OutputTerminator  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VOutputSequencer::OutputTerminator::OutputTerminator (
    Sequencer *pSequencer) : BaseClass (pSequencer, Status_Final
) {
    retain (); {
	startThis ();
    } untain ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VOutputSequencer::OutputTerminator::load (Fifo &rOutputFifo) {
    rOutputFifo.clear ();
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::VOutputSequencer::BlobOutput  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VOutputSequencer::BlobOutput::BlobOutput (
    Sequencer *pSequencer, Blob const &rBlob
) : BaseClass (pSequencer, Status_Available), m_iBlob (rBlob) {
    retain (); {
	startThis ();
    } untain ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VOutputSequencer::BlobOutput::load (Fifo &rOutputFifo) {
    rOutputFifo.setToData (m_iBlob.contentArea (), m_iBlob.contentSize ());
    setStatusToDone ();
}


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VOutputSequencer::StringOutput  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VOutputSequencer::StringOutput::StringOutput (
    Sequencer *pSequencer, VString const &rString
) : BaseClass (pSequencer, Status_Available), m_iString (rString) {
    retain (); {
	startThis ();
    } untain ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VOutputSequencer::StringOutput::load (Fifo &rOutputFifo) {
    rOutputFifo.setToData (m_iString, m_iString.length ());
    setStatusToDone ();
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VOutputSequencer  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VOutputSequencer::VOutputSequencer (
     VBSConsumer *pBS
) : Schedulable (this), m_ppOutputQueueTail (&m_pOutputQueueHead), m_pBS (pBS), m_pBSClient (this) {
    traceInfo ("Creating Sequencer");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VOutputSequencer::~VOutputSequencer () {
    traceInfo ("Destroying Sequencer");
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VOutputSequencer::OnTransfer (IBSClient *pRole, size_t sTransfer) {
    m_iOutputFifo.consume (sTransfer);
    if (m_iOutputFifo.consumerAreaIsEmpty ())
	resume ();
}

void Vca::VOutputSequencer::OnError (
    IClient *pRole, IError *pError, VString const &rMessage
) {
    m_pFinalError.setTo (pError);
    m_iFinalErrorMessage.setTo (rMessage);

    m_pBS.clear ();
    if (suspended ())
	resume ();
}


/*******************************
 *******************************
 *****  Output Generation  *****
 *******************************
 *******************************/

void Vca::VOutputSequencer::outputBlob (BlobOutput::Reference &rpBlobOutput, Blob const &rBlob) {
    if (rBlob.contentSize () > 0)
	rpBlobOutput.setTo (new BlobOutput (this, rBlob));
    else
	rpBlobOutput.clear ();
}

void Vca::VOutputSequencer::outputBlob (Blob const &rBlob) {
    BlobOutput::Reference pBlobOutput;
    outputBlob (pBlobOutput, rBlob);
}

void Vca::VOutputSequencer::outputString (StringOutput::Reference &rpStringOutput, VString const &rString) {
    if (rString.length () > 0)
	rpStringOutput.setTo (new StringOutput (this, rString));
    else
	rpStringOutput.clear ();
}

void Vca::VOutputSequencer::outputString (VString const &rString) {
    StringOutput::Reference pStringOutput;
    outputString (pStringOutput, rString);
}

void Vca::VOutputSequencer::outputPause (OutputPause::Reference &rpOutputPause) {
    rpOutputPause.setTo (new OutputPause (this));
}

void Vca::VOutputSequencer::outputTerminator (OutputTerminator::Reference &rpOutputTerminator) {
    rpOutputTerminator.setTo (new OutputTerminator (this));
}

void Vca::VOutputSequencer::outputTerminator () {
    OutputTerminator::Reference pOutputTerminator;
    outputTerminator (pOutputTerminator);
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vca::VOutputSequencer::queue (Output *pOutput) {
    m_ppOutputQueueTail->setTo (pOutput);
    m_ppOutputQueueTail = &pOutput->m_pSuccessor;
    start (pOutput);
}

void Vca::VOutputSequencer::start (Output *pOutput) {
    if (pOutput == m_pOutputQueueHead)
	resume ();
}

void Vca::VOutputSequencer::schedule_() {
    m_iScheduler.schedule (this);
}

void Vca::VOutputSequencer::run_() {
    IBSClient::Reference pBSClient;
    getRole (pBSClient);
    while (!suspended () && m_pBS.isntNil ()) {
	if (m_pOutputQueueHead.isNil ())
	    suspend ();
	else switch (m_pOutputQueueHead->status ()) {
	case Output::Status_Available:
	    m_pOutputQueueHead->load (m_iOutputFifo);
	    m_iOutputFifo.consume (m_pBS->put (pBSClient, m_iOutputFifo));
	    if (m_iOutputFifo.consumerAreaIsntEmpty ())
		suspend ();
	    break;
	case Output::Status_Blocked:
	    suspend ();
	    break;
	case Output::Status_Final:
	    m_pOutputQueueHead->onSuccess ();
	    m_pBS->Close ();
	    OnEnd (pBSClient);
	    /*****  No BREAK  *****/
	default:
	    m_pOutputQueueHead.claim (m_pOutputQueueHead->m_pSuccessor);
	    if (m_pOutputQueueHead.isNil ())
		m_ppOutputQueueTail = &m_pOutputQueueHead;
	    break;
	}
    }
    if (m_pBS.isNil ()) {
	while (m_pOutputQueueHead) {
	    m_pOutputQueueHead->onSequencerShutdown ();
	    m_pOutputQueueHead.claim (m_pOutputQueueHead->m_pSuccessor);
	}
	m_ppOutputQueueTail = &m_pOutputQueueHead;
	if (!suspended ())
	    suspend ();
	m_pOnEndTriggers.trigger ();
    }
}

/********************************
 ********************************
 *****  Event Subscription  *****
 ********************************
 ********************************/

void Vca::VOutputSequencer::onEndTrigger (ITrigger *pTrigger) {
    m_pOnEndTriggers.Insert (pTrigger);
}
