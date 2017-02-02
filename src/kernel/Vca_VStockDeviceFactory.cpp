/*****  Vca_VStockDeviceFactory Implementation  *****/

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

#include "Vca_VStockDeviceFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPipeSourceClient.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerConsumerDevice_.h"
#include "Vca_VConnection.h"
#include "Vca_VListener.h"


/**************************************
 **************************************
 *****                            *****
 *****  Vca::VStockDeviceFactory  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VStockDeviceFactory::VStockDeviceFactory ()
    : m_pIBSConsumerSource (this)
    , m_pIBSProducerSource (this)
    , m_pIConnectionSource (this)
    , m_pIListenerSource   (this)
    , m_pIPipeSource (this)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VStockDeviceFactory::~VStockDeviceFactory () {
}

/**********************************
 **********************************
 *****  Role Implementations  *****
 **********************************
 **********************************/

void Vca::VStockDeviceFactory::Supply (IBSConsumerSource *pRole, IBSConsumerSink *pClient) {
    supply (pClient);
}

void Vca::VStockDeviceFactory::Supply (IBSProducerSource *pRole, IBSProducerSink *pClient) {
    supply (pClient);
}

void Vca::VStockDeviceFactory::Supply (IConnectionSource *pRole, IConnectionSink *pClient) {
    supply (pClient);
}

void Vca::VStockDeviceFactory::Supply (IListenerSource *pRole, IListenerSink *pClient) {
    supply (pClient);
}

void Vca::VStockDeviceFactory::Supply (IPipeSource *pRole, IPipeSourceClient *pClient) {
    supply (pClient);
}

/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

bool Vca::VStockDeviceFactory::supply (
    VReference<VBSProducer>&rpBSProducer, VReference<VBSConsumer>&rpBSConsumer
) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpBSProducer, rpBSConsumer);
}

bool Vca::VStockDeviceFactory::supply (VReference<VBSProducer>&rpBSProducer) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpBSProducer);
}

bool Vca::VStockDeviceFactory::supply (VReference<VBSConsumer>&rpBSConsumer) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpBSConsumer);
}

bool Vca::VStockDeviceFactory::supply (VReference<VConnection>&rpConnection) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpConnection);
}

bool Vca::VStockDeviceFactory::supply (VReference<IConnection>&rpConnection) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpConnection);
}

bool Vca::VStockDeviceFactory::supply (VReference<VListener>&rpListener) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpListener);
}

bool Vca::VStockDeviceFactory::supply (VReference<IListener>&rpListener) {
    VReference<VDevice> pDevice;
    return supply (pDevice) && pDevice->supply (rpListener);
}

void Vca::VStockDeviceFactory::supply (IBSConsumerSink *pClient) {
    if (pClient) {
	VReference<VBSConsumer> pBS;
	pClient->OnData (supply (pBS) ? pBS.referent () : NilOf (VBSConsumer*));
    }
}

void Vca::VStockDeviceFactory::supply (IBSProducerSink *pClient) {
    if (pClient) {
	VReference<VBSProducer> pBS;
	pClient->OnData (supply (pBS) ? pBS.referent () : NilOf (VBSProducer*));
    }
}

void Vca::VStockDeviceFactory::supply (IConnectionSink *pClient) {
    if (pClient) {
	VReference<IConnection> pConnection;
	pClient->OnData (supply (pConnection) ? pConnection.referent () : NilOf (IConnection*));
    }
}

void Vca::VStockDeviceFactory::supply (IListenerSink *pClient) {
    if (pClient) {
	VReference<IListener> pListener;
	pClient->OnData (supply (pListener) ? pListener.referent () : NilOf (IListener*));
    }
}

void Vca::VStockDeviceFactory::supply (IPipeSourceClient *pClient) {
    if (pClient) {
	VReference<VBSProducer> pBSProducer; VReference<VBSConsumer> pBSConsumer;
	if (supply (pBSProducer, pBSConsumer))
	    pClient->OnData (pBSConsumer, pBSProducer, 0);
	else
	    pClient->OnData (0,0,0);
    }
}
