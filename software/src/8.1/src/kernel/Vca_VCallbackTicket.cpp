/*****  Vca_VCallbackTicket Implementation  *****/

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

#include "Vca_VCallbackTicket.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VCohort.h"


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::VCallbackTicket::List::Mutex  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

Vca::VCallbackTicket::List::Mutex::Mutex () {
}

Vca::VCallbackTicket::List::Mutex::~Mutex () {
}

/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VCallbackTicket::List  *****
 *****                              *****
 ****************************************
 ****************************************/

Vca::VCallbackTicket::List::MutexImplementation& Vca::VCallbackTicket::List::mutex () {
    if (m_pMutex.isNil ()) {
	Mutex::Reference const pMutex (new Mutex ());
	m_pMutex.interlockedSetIfNil (pMutex);
    }
    return m_pMutex->implementation ();
}

void Vca::VCallbackTicket::List::insert (VCallbackTicket *pTicket) {
    if (pTicket) {
	MutexClaim m_iLock (mutex ());
	pTicket->linkTo (*this);
    }
}

void Vca::VCallbackTicket::List::remove (VCallbackTicket *pTicket) {
    if (pTicket) {
	MutexClaim m_iLock (mutex ());
	pTicket->unlinkFrom (*this);
    }
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VCallbackTicket  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCallbackTicket::VCallbackTicket (
    VReferenceable *pListOwner, List &rList
) : m_pListOwner (pListOwner), m_rList (rList), m_xStatus (Status_Active) {
    rList.insert (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VCallbackTicket::~VCallbackTicket () {
    cancel ();
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VCallbackTicket::cancel () {
    if (isntCanceled ()) {
	VCohort::DefaultLogger().printf ("+++ %p: Callback cancelled.\n", this);
	m_xStatus = Status_Canceled;
	m_rList.remove (this);
    }
}

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vca::VCallbackTicket::linkTo (Pointer &rListHead) {
    m_pSuccessor.setTo (rListHead);
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (this);
    rListHead.setTo (this);
}

void Vca::VCallbackTicket::unlinkFrom (Pointer &rListHead) {
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else
	rListHead.setTo (m_pSuccessor);

    m_pPredecessor.clear ();
    m_pSuccessor.clear ();
}
