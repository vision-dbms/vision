/*****  Vca_IPassiveConnector_Gofers Implementation *****/
#define Vca_IPassiveConnector_Gofers

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

#include "Vca_IPassiveConnector_Gofers.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VDeviceFactory.h"
#include "Vca_VListener.h"
#include "Vca_VPassiveConnector.h"

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::IPassiveConnector_Gofer::gofer_base_t* Vca::IPassiveConnector_Gofer::DefaultConnector () {
    static gofer_base_t::Reference pGofer (new Connector ());
    return pGofer;
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::IPassiveConnector_Gofer::Connector  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPassiveConnector_Gofer::Connector::onData () {
    VkStatus iStatus;
    VListener::Reference pListener;

    // Attempt to manufacture a listener on any TCP port.
    if (VDeviceFactory::Supply (iStatus, pListener, "0", Default::TcpNoDelaySetting ())) {
        // We have a listener. Attach a new VPassiveConnector to it.
	IPassiveConnector::Reference pIPassiveConnector; {
	    IListener::Reference pIListener;
	    pListener->getRole (pIListener);
	    VPassiveConnector::Reference pPassiveConnector (
		new VPassiveConnector (pIListener)
	    );
	    pPassiveConnector->getRole (pIPassiveConnector);
	}
        // Set this gofer's value to the IPassiveConnector instance.
	setTo (pIPassiveConnector);
    } else {
        // Listener could not be manufactured. Error.
	VString iStatusMessage;
	iStatus.getDescription (iStatusMessage);
	onError (0, iStatusMessage);
    }
}
