/*****  Vca_VPassiveCallback Implementation  *****/

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

#include "Vca_VPassiveCallback.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPassiveClient.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VPassiveCallback  *****
 *****                         *****
 ***********************************
 ***********************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VPassiveCallback::~VPassiveCallback () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VPassiveCallback::QueryService (
    IPassiveCallback *pRole, IObjectSink *pObjectSink, VTypeInfo *pObjectType
) {
    m_pOffering->supplyInterface (pObjectSink, pObjectType);
}

void Vca::VPassiveCallback::onNeed () {
    m_pController.materializeFor (this);
    m_pCaller.materializeFor (this);
    BaseClass::onNeed ();
}

void Vca::VPassiveCallback::onData () {
    if (m_pCaller) {
	IPassiveCallback::Reference pCallback;
	getRole (pCallback);
	m_pCaller->AcceptCallback (pCallback, m_iCallID);
    }
    m_pCall.clear ();
    if (m_pController)
	m_pController->Ack ();
}

void Vca::VPassiveCallback::onError (IError *pInterface, VString const &rMessage) {
    if (m_pCall)
	m_pCall->OnError (pInterface, rMessage);
    if (m_pController)
	m_pController->OnError (pInterface, rMessage);
}
