/*****  Vca::VProxyApp Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VProxyApp.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::VProxyApp  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VProxyApp::VProxyApp (Context *pContext)
    : BaseClass			(pContext)
    , m_pIConnectionFactory	(this)
    , m_pIListenerFactory	(this)
    , m_pIPipeFactory		(this)
    , m_pIProcessFactory	(this)
    , m_pIStdPipeSource		(this)
{
}


/**********************************
 **********************************
 *****  Role Implementations  *****
 **********************************
 **********************************/

void Vca::VProxyApp::MakeConnection (
    IConnectionFactory *pRole, IVReceiver<IConnection*> *pClient, VString const &rDestination
) {
    IConnectionFactory::Reference pFactory;
    supply (pFactory);
    pFactory->MakeConnection (pClient, rDestination);
}

void Vca::VProxyApp::SupplyByteStreams (
    IListenerFactory *pRole, IPipeSourceClient *pClient, VString const &rName, U32 cConnections
) {
    IListenerFactory::Reference pFactory;
    supply (pFactory);
    pFactory->SupplyByteStreams (pClient, rName, cConnections);
}

void Vca::VProxyApp::SupplyConnections (
    IListenerFactory *pRole, IListenerClient *pClient, VString const &rName, U32 cConnections
) {
    IListenerFactory::Reference pFactory;
    supply (pFactory);
    pFactory->SupplyConnections (pClient, rName, cConnections);
}

void Vca::VProxyApp::SupplyListener (
    IListenerFactory *pRole, IVReceiver<IListener*>*pClient, VString const &rName
) {
    IListenerFactory::Reference pFactory;
    supply (pFactory);
    pFactory->SupplyListener (pClient, rName);
}

void Vca::VProxyApp::MakeConnection (
    IPipeFactory *pRole, IPipeSourceClient *pClient, VString const &rDestination, bool bTwoPipe
) {
    IPipeFactory::Reference pFactory;
    supply (pFactory);
    pFactory->MakeConnection (pClient, rDestination, bTwoPipe);
}

void Vca::VProxyApp::MakeProcess (
    IProcessFactory *pRole, IProcessFactoryClient *pClient, VString const &rCommand, bool bTwoPipe
) {
    IProcessFactory::Reference pFactory;
    supply (pFactory);
    pFactory->MakeProcess (pClient, rCommand, bTwoPipe);
}

void Vca::VProxyApp::Supply (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient 
) {
    IStdPipeSource::Reference pFactory;
    supply (pFactory);
    pFactory->Supply (pClient);
}


/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool Vca::VProxyApp::start_() {
    if (!BaseClass::start_())
	return false;

    char const *const pListenerAddress = commandStringValue ("server");
    if (pListenerAddress)
	offerSelf (pListenerAddress);
    else {
	displayUsagePattern (
	    "[-proxy=address | -proxyFile=filename] [-serverFile=filename] -server=<listenerName>"
	);
	displayStandardOptions ();
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vca::VProxyApp> iMain (argc, argv);
    return iMain.processEvents ();
}
