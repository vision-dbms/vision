/*****  Vca_VEvent Implementation  *****/

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

#include "Vca_VEvent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IEventSink.h"


/*************************
 *************************
 *****               *****
 *****  Vca::VEvent  *****
 *****               *****
 *************************
 *************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VEvent::VEvent () : m_pIEvent (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VEvent::~VEvent () {
}

/***********************************
 ***********************************
 *****  Callbacks and Defaults *****
 ***********************************
 ***********************************/

void Vca::VEvent::getDescription_(VString& rsDescription) const {
}

void Vca::VEvent::GetDescription (IEvent* pRole, IVReceiver<VString const&>* pClient) {
    if (pClient) {
	VString sResult;
	getDescription (sResult);
	pClient->OnData (sResult);
    }
}

void Vca::VEvent::GetTimestamp (IEvent* pRole, IVReceiver<V::VTime const&>* pClient) {
    if (pClient)
	pClient->OnData (timestamp ());
}

void Vca::VEvent::GetSSID (IEvent* pRole, IVReceiver<ssid_t>* pClient) {
    if (pClient)
	pClient->OnData (ssid ());
}
