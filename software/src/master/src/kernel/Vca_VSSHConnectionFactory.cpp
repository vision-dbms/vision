/*****  Vca_VSSHConnectionFactory Implementation  *****/

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

#include "Vca_VSSHConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32)     // Compiler Happy Pill Lite...
#include "V_VThread.h"  // omit:Linux omit:SunOS
#endif

#include "VReceiver.h"
#include "Vca_VStatus.h"
#include "Vca_VListener.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VSSHDeviceImplementation.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VBSProducerConsumerDevice_.h"
#include "Vca_VBSProducerDevice_.h"

#include "Vca_IPipeSourceClient.h"

#include "Vca_CompilerHappyPill.h"


/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vca::VSSHConnectionFactory::PipeSource  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSSHConnectionFactory::PipeSource::PipeSource (
    VSSHConnectionFactory *pFactory,
    VString const &rHost, U32 xPort,
    VString const &rUsername, VString const &rPassword,
    VString const &rCommand, bool bErrorChannel, bool bNoDelay
) : m_pFactory (pFactory)
  , m_iHost (rHost)
  , m_xPort (xPort)
  , m_iUsername (rUsername)
  , m_iPassword (rPassword)
  , m_iCommand (rCommand)
  , m_bErrorChannel (bErrorChannel)
  , m_bNoDelay (bNoDelay)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VSSHConnectionFactory::PipeSource::~PipeSource () {
    traceInfo ("Destroying VSSHConnectionFactory::PipeSource");
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VSSHConnectionFactory::PipeSource::supply_(IPipeSourceClient *pClient) {
    m_pFactory->supply (
        pClient, m_iHost, m_iUsername, m_iPassword, m_iCommand, m_bErrorChannel, m_bNoDelay, m_xPort
    );
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VSSHConnectionFactory  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSSHConnectionFactory::VSSHConnectionFactory (
        VDeviceFactory *pDeviceFactory
) : BaseClass (pDeviceFactory->cohort ()), m_pDeviceFactory (pDeviceFactory), m_pISSHConnectionFactory (this) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VSSHConnectionFactory::~VSSHConnectionFactory () {
    traceInfo ("Destroying Vca::VSSHConnectionFactory");

    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VSSHConnectionFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VSSHConnectionFactory::Supply (Reference&rpInstance) {
    rpInstance.setTo (
        reinterpret_cast<ThisClass*> (VCohort::VcaValue (CohortIndex ()))
    );
    if (rpInstance.isNil ()) {
        VDeviceFactory::Reference pDeviceFactory;
        if (VDeviceFactory::Supply (pDeviceFactory))
            rpInstance.setTo (new ThisClass (pDeviceFactory));
    }
    return rpInstance.isntNil ();
}

bool Vca::VSSHConnectionFactory::Supply (ThisInterface::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
        pInstance->getRole (rpRole);
    else
        rpRole.clear ();
    return rpRole.isntNil ();
}

/*********************************************
 *********************************************
 *****  ISSHConnectionFactory Methods  *****
 *********************************************
 *********************************************/

void Vca::VSSHConnectionFactory::MakeSSHConnection (
    ISSHConnectionFactory *pRole,
    IPipeSourceClient *pClient,
    VString const &rHost,
    VString const &rUsername, VString const &rPassword,
    VString const &rCommand, bool bErrorChannel
) {
    supply (
        pClient, rHost, rUsername, rPassword, rCommand, bErrorChannel, Default::TcpNoDelaySetting ()
    );
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

bool Vca::VSSHConnectionFactory::Supply (
    IPipeSourceClient *pClient,
    VString const &rHostname, VString const &rUserName, VString const &rPassword, VString const &rCommand,
    bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
        pClient, rHostname, rUserName, rPassword, rCommand, bErrorChannel, bNoDelay, xPort
    );
}

/**
 * @todo Make non-blocking.
 */
bool Vca::VSSHConnectionFactory::supply (
    IPipeSourceClient *pClient,
    VString const &rHostname, VString const &rUserName, VString const &rPassword, VString const &rCommand,
    bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    bool bSuccess = true;
    if (pClient) {
#ifdef HAS_LIBSSH2
        // Get our socket.
        VDevice::Reference pSocket;
        VString iServer = rHostname;
        iServer << ":" << xPort;
        VStatus iStatus;
        if (!m_pDeviceFactory->supply (iStatus, pSocket, iServer, bNoDelay)) {
            iStatus.communicateTo (pClient);
            pClient->OnError (0, "Connection to SSH server failed.");
            return false;
        }

        // Construct and initialize channel.
        VSSHDeviceImplementation::Channel::Reference pChannel (new VSSHDeviceImplementation::Channel (pSocket, rUserName, rPassword, rCommand));
        return pChannel->connect (pClient);
#else
	pClient->OnError (0, "SSH Not Available On This Platform");
#endif
    }
    return true;
}

bool Vca::VSSHConnectionFactory::Supply (
    PipeSource::Reference &rpCPS,
    VString const &rHostname,
    VString const &rUsername, VString const &rPassword,
    VString const &rCommand, bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
        rpCPS, rHostname, rUsername, rPassword, rCommand, bErrorChannel, bNoDelay, xPort
    );
}

bool Vca::VSSHConnectionFactory::supply (
    PipeSource::Reference &rpCPS,
    VString const &rHostname,
    VString const &rUsername, VString const &rPassword,
    VString const &rCommand, bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    rpCPS.setTo (
        new PipeSource (
            this, rHostname, xPort, rUsername, rPassword, rCommand, bErrorChannel, bNoDelay
        )
    );
    return true;
}
