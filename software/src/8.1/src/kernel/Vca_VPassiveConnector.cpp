/*****  Vca_VPassiveConnector Implementation *****/

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

#include "Vca_VPassiveConnector.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

/************************************
 ************************************
 *****                          *****
 *****  Vca::VPassiveConnector  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaListener::Reference Vca::VPassiveConnector::newListener (IListener *pListener) {
    VcaOffer::Offering const iOffering (m_pRegistry);
    VcaListener::Reference pVcaListener (new VcaListener (pListener, iOffering));
    return pVcaListener;
}

Vca::VPassiveConnector::VPassiveConnector (
    IListener *pListener
) : m_pINameSource (this), m_pIPassiveConnector (this), m_pRegistry_IUpdate (this), m_pRegistry (
    new Registry::VRegistry ()
), m_pListener (newListener (pListener)) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VPassiveConnector::~VPassiveConnector () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VPassiveConnector::AddObject (
    Registry::IUpdate *pRole, Registry::IUpdater* pUpdater, IVUnknown* pObject
) {
    m_pRegistry->addObject (pUpdater, pObject);
}

void Vca::VPassiveConnector::RequestCallback (
    IPassiveConnector *pRole, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
) {
    m_pRegistry->requestCallback (this, pCallbackReceiver, pServer, sTimeout);
}

void Vca::VPassiveConnector::Supply (INameSource *pRole, INameSink *pSink) {
    getName (pSink);
}
