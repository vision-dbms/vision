/*****  Vca_VFilterDeviceFactory Implementation  *****/

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

#include "Vca_VFilterDeviceFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPipeSource.h"
#include "Vca_IPipeSourceClient.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VDevice.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::VFilterDeviceFactory  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VFilterDeviceFactory::VFilterDeviceFactory () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VFilterDeviceFactory::~VFilterDeviceFactory () {
    traceInfo ("Destroying VFilterDeviceFactory");
}

/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

void Vca::VFilterDeviceFactory::supplyClient (IPipeSourceClient *pClient, VDevice *pDevice) const {
    if (pClient) {
	VReference<VBSProducer> pBSProducer; VReference<VBSConsumer> pBSConsumer;
	if (pDevice && pDevice->supply (pBSProducer, pBSConsumer))
	    pClient->OnData (pBSConsumer, pBSProducer, 0);
	else {
	    static VString iNoDeviceMessage ("No Device", false);
	    pClient->OnError (0, iNoDeviceMessage);
	}
    }
}
