/*****  Vca_VDevice Implementation  *****/

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

#include "Vca_VDevice.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VConnection.h"
#include "Vca_VListener.h"
#include "Vca_VStatus.h"
#include "Vca_VStreamSink.h"
#include "Vca_VStreamSource.h"


/**************************
 **************************
 *****                *****
 *****  Vca::VDevice  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDevice::VDevice (VCohort *pCohort) : BaseClass (pCohort) {
    traceInfo ("Creating VDevice");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDevice::~VDevice () {
    traceInfo ("Destroying VDevice");
}


/******************************
 ******************************
 *****  Interface Supply  *****
 ******************************
 ******************************/

bool Vca::VDevice::supplyBSProducerConsumer_(
    VBSProducer::Reference &rpBSProducer, VBSConsumer::Reference &rpBSConsumer
) {
    bool bProducer = supplyBSProducer_(rpBSProducer);
    bool bConsumer = supplyBSConsumer_(rpBSConsumer);
    return bProducer && bConsumer;
}

bool Vca::VDevice::supplyBSConsumer_(VBSConsumer::Reference &rpBSConsumer) {
    rpBSConsumer.clear ();
    return false;
}

bool Vca::VDevice::supplyBSProducer_(VBSProducer::Reference &rpBSProducer) {
    rpBSProducer.clear ();
    return false;
}

bool Vca::VDevice::supplyConnection_(VConnection::Reference &rpConnection) {
    rpConnection.clear ();
    return false;
}

bool Vca::VDevice::supplyListener_(VListener::Reference &rpListener) {
    rpListener.clear ();
    return false;
}

bool Vca::VDevice::supply (IConnection::Reference &rpInterface) {
    VConnection::Reference  pUse;
    return supply (pUse) && pUse->supply (rpInterface);
}

bool Vca::VDevice::supply (IListener::Reference &rpInterface) {
    VListener::Reference  pUse;
    return supply (pUse) && pUse->supply (rpInterface);
}

bool Vca::VDevice::supply (IPipeSource::Reference &rpInterface) {
    VListener::Reference  pUse;
    return supply (pUse) && pUse->supply (rpInterface);
}


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VDevice::Use  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDevice::Face::User::Use::Use () : m_xRunState (RunState_Idle) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDevice::Use::~Use () {
}

/**********************
 **********************
 *****  Callback  *****
 ***********************
 ***********************/

void Vca::VDevice::Use::triggerUser () {
    user()->trigger ();
}


/************************
 ************************
 *****  Completion  *****
 ************************
 ************************/

bool Vca::VDevice::Use::finish (VkStatus &rStatus) {
    switch (m_xRunState) {
    case RunState_Idle:
    case RunState_Busy:
    case RunState_Recycle:
    case RunState_RecycleKeep:
	break;
    case RunState_Data:
    //  Return result only once:
	m_xRunState = RunState_Recycle;
	finish_();
	break;
    case RunState_DataKeep:
    //  Return result only once:
	m_xRunState = RunState_RecycleKeep;
	retain ();
	finish_();
	break;
    case RunState_Error:
    //  Return status only once:
	m_xRunState = RunState_Recycle;
	rStatus.setTo (m_iStatus);
	break;
    case RunState_ErrorKeep:
    //  Return status only once:
	m_xRunState = RunState_RecycleKeep;
	retain ();
	rStatus.setTo (m_iStatus);
	break;
    default:
	m_xRunState = RunState_Recycle;
	rStatus.MakeFailureStatus ();
	break;
    }
    return m_xRunState != RunState_Busy;
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

bool Vca::VDevice::Use::onData (bool bKeep) {
    switch (m_xRunState) {
    case RunState_Busy:
	m_xRunState = bKeep ? RunState_DataKeep : RunState_Data;
	return true;
    case RunState_RecycleKeep:
	if (!bKeep) {
	    m_xRunState = RunState_Recycle;
	    recycle_();
	    release ();
	}
	break;
    default:
	break;
    }
    return false;
}

bool Vca::VDevice::Use::onError (VkStatus const &rStatus, bool bKeep) {
    switch (m_xRunState) {
    case RunState_Busy:
	m_iStatus.setTo (rStatus);
	m_xRunState = bKeep ? RunState_ErrorKeep : RunState_Error;
	return true;
    case RunState_RecycleKeep:
	if (!bKeep) {
	    m_xRunState = RunState_Recycle;
	    recycle_();
	    release ();
	}
	break;
    default:
	break;
    }
    return false;
}

void Vca::VDevice::Use::onUser (User *pUser) {
    m_xRunState = RunState_Busy;
    pUser->enqueue (this);
}

void Vca::VDevice::Use::onUsed () {	//  ... called by User::finish
    if (RunState_Recycle == m_xRunState)
	recycle_();
}

void Vca::VDevice::Use::recycle () {
    m_xRunState = RunState_Idle;
}


/********************************
 ********************************
 *****                      *****
 *****  Vca::VDevice::User  *****
 *****                      *****
 ********************************
 ********************************/

/***********************
 ***********************
 *****  Use Start  *****
 ***********************
 ***********************/

bool Vca::VDevice::Face::User::start (VkStatus &rStatus) {
    if (!m_bClosed)
	return true;
    rStatus.MakeClosedStatus ();
    return false;
}

void Vca::VDevice::Face::User::enqueue (Use *pUse) {
    m_iQUIP.enqueue (Use::Reference(pUse));
}

/****************************
 ****************************
 *****  Use Completion  *****
 ****************************
 ****************************/

bool Vca::VDevice::Face::User::finish (VkStatus &rStatus) {
/*----------------------------------------------------------------------------*
 *  NOTE:
 *	The use of a local variable (bActive) allows this routine to operate
 *  correctly (i.e., without access violation) whenever the last reference
 *  to the user ('this') is held by the Use objects being processed.
 *----------------------------------------------------------------------------*/
    bool bActive = m_iQUIP.active ();
//  An active request that doesn't exist can't be busy and so must have finished
//  (not to mention that calling finish on a request that doesn't exist will
//  cause a crash)...
    while (bActive && (!m_iQUIP.activeRequest () || m_iQUIP.activeRequest ()->finish (rStatus))) {
	Use::Reference pCompletedUse;
	pCompletedUse.claim (m_iQUIP.activeRequest ());
	bActive = m_iQUIP.dequeue ();
    //  There is evidence that 'pCompletedUse' can be null when 'onUsed' is
    //  called.  A null guard prevents a crash in that case...
	if (pCompletedUse)
	pCompletedUse->onUsed ();
    }
    return !bActive;
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VDevice::BSFace  *****
 *****                        *****
 **********************************
 **********************************/

bool Vca::VDevice::BSFace::supply (VConnection::Reference &rpConnection) {
    rpConnection.setTo (new VConnection (device ()));
    return true;
}

/**************************************
 **************************************
 *****                            *****
 *****  Vca::VDevice::BSReadFace  *****
 *****                            *****
 **************************************
 **************************************/

bool Vca::VDevice::BSReadFace::supply (VBSProducer::Reference &rpBSProducer) {
    rpBSProducer.setTo (new VStreamSource (this));
    return true;
}

/************************************
 ************************************
 *****                          *****
 *****  Vca::VDevice::BSReader  *****
 *****                          *****
 ************************************
 ************************************/

bool Vca::VDevice::BSReader::start (VkStatus &rStatus, BSReader::AreaPointer pArea, size_t sArea) {
    Area const iArea (pArea, sArea);
    return start (rStatus, iArea);
}

bool Vca::VDevice::BSReader::start (VkStatus &rStatus, BSReader::Area const &rArea) {
    return BaseClass::start (rStatus)
	&& m_pFace->start_(rStatus, this, rArea);
}

bool Vca::VDevice::BSReader::start (VkStatus &rStatus) {
    return BaseClass::start (rStatus) && m_pFace->start_(rStatus, this);
}

/***************************************
 ***************************************
 *****                             *****
 *****  Vca::VDevice::BSWriteFace  *****
 *****                             *****
 ***************************************
 ***************************************/

bool Vca::VDevice::BSWriteFace::supply (VBSConsumer::Reference &rpBSConsumer) {
    rpBSConsumer.setTo (new VStreamSink (this));
    return true;
}

/************************************
 ************************************
 *****                          *****
 *****  Vca::VDevice::BSWriter  *****
 *****                          *****
 ************************************
 ************************************/

bool Vca::VDevice::BSWriter::start (VkStatus &rStatus, BSWriter::AreaPointer pArea, size_t sArea) {
    Area const iArea (pArea, sArea);
    return start (rStatus, iArea);
}

bool Vca::VDevice::BSWriter::start (VkStatus &rStatus, BSWriter::Area const &rArea) {
    return BaseClass::start (rStatus)
	&& m_pFace->start_(rStatus, this, rArea);
}

bool Vca::VDevice::BSWriter::start (VkStatus &rStatus) {
    return BaseClass::start (rStatus) && m_pFace->start_(rStatus, this);
}

/**************************************
 **************************************
 *****                            *****
 *****  Vca::VDevice::ListenFace  *****
 *****                            *****
 **************************************
 **************************************/

bool Vca::VDevice::ListenFace::supply (VListener::Reference &rpListener) {
    rpListener.setTo (new VListener (this));
    return true;
}

/**************************************
 **************************************
 *****                            *****
 *****  Vca::VDevice::ListenFace  *****
 *****                            *****
 **************************************
 **************************************/

/************************************
 ************************************
 *****                          *****
 *****  Vca::VDevice::Listener  *****
 *****                          *****
 ************************************
 ************************************/

bool Vca::VDevice::Listener::start (VkStatus &rStatus) {
    return BaseClass::start (rStatus) && m_pFace->start_(rStatus, this);
}
