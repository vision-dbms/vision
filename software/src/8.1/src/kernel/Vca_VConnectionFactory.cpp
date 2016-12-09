/*****  Vca_VConnectionFactory Implementation  *****/

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

#include "Vca_VConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDevice.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_IPipeSourceClient.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VConnection.h"
#include "Vca_VConnectionPipeSource.h"

#include "Vca_VDeviceFactory.h"

#include "Vca_VStatus.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VConnectionFactory  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VConnectionFactory::VConnectionFactory (
    VDeviceFactory *pDeviceFactory
) : BaseClass		(pDeviceFactory->cohort ())
, m_pDeviceFactory	(pDeviceFactory)
, m_pIConnectionFactory (this)
, m_pIPipeFactory	(this)
{
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VConnectionFactory::~VConnectionFactory () {
    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VConnectionFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VConnectionFactory::Supply (Reference &rpInstance) {
    rpInstance.setTo (reinterpret_cast<ThisClass*> (VCohort::VcaValue (CohortIndex ())));
    if (rpInstance.isNil ()) {
	VDeviceFactory::Reference pDeviceFactory;
	if (VDeviceFactory::Supply (pDeviceFactory))
	    rpInstance.setTo (new ThisClass (pDeviceFactory));
    }
    return rpInstance.isntNil ();
}

bool Vca::VConnectionFactory::Supply (IConnectionFactory::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}

bool Vca::VConnectionFactory::Supply (IPipeFactory::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}


/****************************************
 ****************************************
 *****  IConnectionFactory Methods  *****
 ****************************************
 ****************************************/

void Vca::VConnectionFactory::MakeConnection (
    IConnectionFactory *pRole, IConnectionSink *pClient, VString const &rName
) {
    supply (pClient, rName, Default::TcpNoDelaySetting ());
}

/**********************************
 **********************************
 *****  IPipeFactory Methods  *****
 **********************************
 **********************************/

void Vca::VConnectionFactory::MakeConnection (
    IPipeFactory *pRole, IPipeSourceClient *pClient, VString const &rName, bool bTwoPipe
) {
    supply (pClient, rName, Default::TcpNoDelaySetting ());
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

bool Vca::VConnectionFactory::Supply (IConnectionSink *pClient, char const *pName, bool bNoDelay) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (pClient, pName, bNoDelay);
}

bool Vca::VConnectionFactory::supply (IConnectionSink *pClient, char const *pName, bool bNoDelay) {
    bool bStarted = false;
    if (pClient) {
	VStatus iStatus; VConnection::Reference pConnection;
	if (!m_pDeviceFactory->supply (iStatus, pConnection, pName, bNoDelay))
	    iStatus.communicateTo (pClient);
	else {
	    IConnection::Reference pInterface;
	    pConnection->getRole (pInterface);
	    pClient->OnData (pInterface);
	    bStarted = true;
	}
    }
    return bStarted;
}

bool Vca::VConnectionFactory::Supply (IPipeSourceClient *pClient, char const *pName, bool bNoDelay) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (pClient, pName, bNoDelay);
}

bool Vca::VConnectionFactory::supply (IPipeSourceClient *pClient, char const *pName, bool bNoDelay) {
    bool bStarted = false;
    if (pClient) {
	VStatus iStatus;
	VBSProducer::Reference pBSProducer;
	VBSConsumer::Reference pBSConsumer;
	if (!m_pDeviceFactory->supply (iStatus, pBSProducer, pBSConsumer, pName, bNoDelay))
	    iStatus.communicateTo (pClient);
	else {
	    pClient->OnData (pBSConsumer, pBSProducer, 0);
	    bStarted = true;
	}
    }
    return bStarted;
}

bool Vca::VConnectionFactory::Supply (
    VConnectionPipeSource::Reference &rpCPS, VString const &rDestination, bool bIsFileName, bool bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rpCPS, rDestination, bIsFileName, bNoDelay);
}

bool Vca::VConnectionFactory::supply (
    VConnectionPipeSource::Reference &rpCPS, VString const &rDestination, bool bIsFileName, bool bNoDelay
) {
    rpCPS.setTo (new VConnectionPipeSource (this, rDestination, bIsFileName, bNoDelay));
    return true;
}

