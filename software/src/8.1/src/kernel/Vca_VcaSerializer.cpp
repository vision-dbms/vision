/*****  Vca_VcaSerializer Implementation  *****/

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

#include "Vca_VcaSerializer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"

#include "Vca_VcaTransport.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::VcaSerializer  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer::VcaSerializer (
    VcaSerializer *pCaller
) : Schedulable (this), m_pCaller (pCaller), m_pTransport (pCaller->transport ()), m_bAborted (false) {
    pCaller->suspend ();
}

Vca::VcaSerializer::VcaSerializer (
    VcaTransport *pTransport
) : Schedulable (this), m_pTransport (pTransport), m_bAborted (false) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaSerializer::~VcaSerializer () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VcaSite *Vca::VcaSerializer::peer () const {
    return m_pTransport->peer ();
}

Vca::VcaTransport *Vca::VcaSerializer::transport () const {
    return m_pTransport;
}

Vca::VcaTransportInbound *Vca::VcaSerializer::transportIfInbound () const {
    return m_pTransport->transportIfInbound ();
}

Vca::VcaTransportOutbound *Vca::VcaSerializer::transportIfOutbound () const {
    return m_pTransport->transportIfOutbound ();
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VcaSerializer::isIncoming () const {
    return m_pTransport->isIncoming ();
}
bool Vca::VcaSerializer::isOutgoing () const {
    return m_pTransport->isOutgoing ();
}

/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VcaSerializer::get (void *pVoid, size_t sVoid) {
    m_pTransport->getData (this, pVoid, sVoid);
}

void Vca::VcaSerializer::put (void const *pData, size_t sData) {
    m_pTransport->putData (this, pData, sData);
}

void Vca::VcaSerializer::transferData () {
    m_pTransport->transferDataFor (this);
}


/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

//  Abort: Clears the sequencer, sets the abort flag and
//	   and recursively does it for the parent serializers...

void Vca::VcaSerializer::abort () {
    m_bAborted = true;
    m_pSequencer.clear ();
    if (m_pCaller)
	m_pCaller->abort ();
    else {
	m_pTransport->Close ();
    }
}

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void Vca::VcaSerializer::schedule_() {
    m_pTransport->schedule (this);
}

void Vca::VcaSerializer::start (VcaSerializer *pParent, VcaSerializer *pChild) {
    if (pChild)
	pChild->resume ();
    else if (pParent)
	pParent->abort ();
}

void Vca::VcaSerializer::run_() {
    while (!suspended () && !aborted ()) {
	if (m_pSequencer)
	    m_pSequencer->Process ();
	else {
	    m_pTransport->wrapup (this);
	    suspend ();

	    if (m_pCaller)
		m_pCaller->resume ();
	    else
		m_pTransport->startNextSerializer ();
	}
    }
}

void Vca::VcaSerializer::clearSequencer () {
    m_pSequencer.clear ();
}


/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

void Vca::VcaSerializer::localizeData () {
}

void Vca::VcaSerializer::getData () {
}

void Vca::VcaSerializer::putData () {
}

void Vca::VcaSerializer::wrapupIncomingSerialization () {
}

void Vca::VcaSerializer::wrapupOutgoingSerialization () {
}
