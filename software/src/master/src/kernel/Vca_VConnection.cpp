/*****  Vca_VConnection Implementation  *****/

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

#include "Vca_VConnection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VDevice.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VConnection  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VConnection::VConnection (VDevice *pDevice)
: m_pDevice		(pDevice)
, m_pIConnection	(this)
, m_pIVBSConsumerSource	(this)
, m_pIVBSProducerSource	(this)
{
    traceInfo ("Creating VConnection");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VConnection::~VConnection () {
    traceInfo ("Destroying VConnection");
}

/*****************************
 *****************************
 *****  Local Suppliers  *****
 *****************************
 *****************************/

bool Vca::VConnection::supply (
    VBSProducer::Reference &rpBSProducer, VBSConsumer::Reference &rpBSConsumer
) {
    return m_pDevice->supply (rpBSProducer, rpBSConsumer);
}

bool Vca::VConnection::supply (VBSProducer::Reference &rpUser) {
    return m_pDevice->supply (rpUser);
}

bool Vca::VConnection::supply (VBSConsumer::Reference &rpUser) {
    return m_pDevice->supply (rpUser);
}


/*******************************
 *******************************
 *****  Interface Methods  *****
 *******************************
 *******************************/

void Vca::VConnection::SupplyBSConsumer (IConnection *pRole, IVBSConsumerSink *pClient) {
    if (pClient) {
	VBSConsumer::Reference pBS;
	if (supply (pBS))
	    pClient->OnData (pBS);
	else
	    communicateFailureTo (pClient);
    }
}

void Vca::VConnection::SupplyBSProducer (IConnection *pRole, IVBSProducerSink *pClient) {
    if (pClient) {
	VBSProducer::Reference pBS;
	if (supply (pBS))
	    pClient->OnData (pBS);
	else
	    communicateFailureTo (pClient);
    }
}

void Vca::VConnection::Supply (IVBSConsumerSource *pRole, IVBSConsumerSink *pClient) {
    if (pClient) {
	VBSConsumer::Reference pBS;
	if (supply (pBS))
	    pClient->OnData (pBS);
	else
	    communicateFailureTo (pClient);
    }
}

void Vca::VConnection::Supply (IVBSProducerSource *pRole, IVBSProducerSink *pClient) {
    if (pClient) {
	VBSProducer::Reference pBS;
	if (supply (pBS))
	    pClient->OnData (pBS);
	else
	    communicateFailureTo (pClient);
    }
}
