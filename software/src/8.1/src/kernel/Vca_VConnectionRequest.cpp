/*****  Vca_VConnectionRequest Implementation  *****/

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

#include "Vca_VConnectionRequest.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VcaConnection.h"

#include "Vca_VDeviceFactory.h"
#include "Vca_VStatus.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/***************************************
 ***************************************
 *****  Interface Implementations  *****
 ***************************************
 ***************************************/

/*************************************
 *****  Vca::IConnectionRequest  *****
 *************************************/

void Vca::VConnectionRequest::onListenerAddress (IConnectionRequest *pRole, VString const &rAddress) {
    //  Need to handle error cases
    VStatus iStatus; VBSProducer::Reference pBSProducer; VBSConsumer::Reference pBSConsumer;
    if (VDeviceFactory::Supply (iStatus, pBSProducer, pBSConsumer, rAddress, Default::TcpNoDelaySetting ())) {
	VcaOffer::Offering const iOffering;
	VcaConnection::Reference pConnection (new VcaConnection (pBSProducer, pBSConsumer, iOffering));
    }
}

void Vca::VConnectionRequest::onDone (IConnectionRequest *pRole) {
    m_bDone = true;
}

void Vca::VConnectionRequest::onFailure (IConnectionRequest *pRole) {
    display ("\nFailure message received....\n");
}
