/*****  VcaListener Implementation  *****/

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

#include "Vca_VcaListener.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IListener.h"

#include "Vca_VcaConnection.h"
#include "Vca_VcaSite.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VcaListener  *****
 *****                    *****
 ******************************
 ******************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VcaListener::Reference Vca::VcaListener::g_pListenerListHead;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaListener::VcaListener (IListener *pListener, Offering const &rOffering)
    : m_pIPipeSourceClient	(this)
    , m_pINameSink		(this)
    , m_pListener		(pListener)
    , m_iOffering		(rOffering)
    , m_cConnectionsProcessed	(0)
{
    retain (); {
	addToListenerList ();

	IPipeSourceClient::Reference pPipeSourceClientRole;
	getRole (pPipeSourceClientRole);
	m_pListener->SupplyByteStreams (pPipeSourceClientRole, UINT_MAX);

	INameSink::Reference pNameSink;
	getRole (pNameSink);
	m_pListener->GetName (pNameSink);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaListener::~VcaListener () {
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

Vca::U32 Vca::VcaListener::activeOfferCount () const {
    return m_iOffering.offerCount ();
}

/*********************************
 *********************************
 *****  Role Implementation  *****
 *********************************
 *********************************/

void Vca::VcaListener::OnData (INameSink *pRole, VString const &rName) {
    m_iNameSource.setTo (rName);
}

void Vca::VcaListener::OnData (
    IPipeSourceClient *pRole, VBSConsumer *pBSToPeer, VBSProducer *pBSToHere, VBSProducer *pErrToHere
) {
    if (pBSToPeer && pBSToHere) {
	VcaConnection::Reference pVcaConnection (new VcaConnection (pBSToHere, pBSToPeer, m_iOffering));
	if (UINT_MAX == ++m_cConnectionsProcessed) {
	    m_pListener->SupplyByteStreams (pRole, UINT_MAX);
	    m_cConnectionsProcessed = 0;
	}
    }
}

void Vca::VcaListener::OnError (IClient *pRole, IError *pError, VString const &rMessage) {
//    BaseClass::OnError (pRole, pError, rMessage);

    removeFromListenerList ();
    m_pListener.clear ();

    m_iNameSource.setTo (pError, rMessage);
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/*---------------------------------------------------------------------------*
 *  This operation applies to all Vca resource manager derived classes, not
 *  just this one.
 *---------------------------------------------------------------------------*/
//  Note: Stopping a listener also removes it from the global Listener list
//	  being maintained in the system....

void Vca::VcaListener::stop () {
    if (m_pListener) {
	m_pListener->Close ();
	m_pListener.clear ();
    }
    removeFromListenerList ();
}


/******************************
 ******************************
 *****  List Maintenance  *****
 ******************************
 ******************************/

void Vca::VcaListener::addToListenerList () {
    m_pSuccessor.setTo (g_pListenerListHead);
    if (g_pListenerListHead) 
	g_pListenerListHead->m_pPredecessor.setTo (this);
    g_pListenerListHead.setTo (this);
}

void Vca::VcaListener::removeFromListenerList () {
    retain ();

    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else if (g_pListenerListHead.referent () == this)
	g_pListenerListHead.setTo (m_pSuccessor);

    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);

    m_pPredecessor.clear ();
    m_pSuccessor.clear ();

    release ();
}

/******************************************************************************
 * Routine: CloseListeners
 *	This static method is used during the shutdown of a peer to close all the 
 * listeners which are still active ...
 *****************************************************************************/

void Vca::VcaListener::CloseListeners  () {

    // Note: stop () removes the listener from the global list resulting in a 
    // new list head

    while (g_pListenerListHead) 
	g_pListenerListHead->stop ();
}
