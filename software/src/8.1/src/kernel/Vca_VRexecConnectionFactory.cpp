/*****  Vca_VRexecConnectionFactory Implementation  *****/

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

#include "Vca_VRexecConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32)	// Compiler Happy Pill Lite...
#include "V_VThread.h"	// omit:Linux omit:SunOS
#endif

#include "VReceiver.h"
#include "Vca_VStatus.h"
#include "Vca_VListener.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_IListener.h"
#include "Vca_IPipeSourceClient.h"

#include "VkSocketAddress.h"

#include "Vca_CompilerHappyPill.h"


/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vca::VRexecConnectionFactory::PipeSource  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRexecConnectionFactory::PipeSource::PipeSource (
    VRexecConnectionFactory *pFactory, 
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

Vca::VRexecConnectionFactory::PipeSource::~PipeSource () {
    traceInfo ("Destroying VRexecConnectionFactory::PipeSource");
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VRexecConnectionFactory::PipeSource::supply_(IPipeSourceClient *pClient) {
    m_pFactory->supply (
	pClient, m_iHost, m_iUsername, m_iPassword, m_iCommand, m_bErrorChannel, m_bNoDelay, m_xPort
    );
}


/*******************************************************
 *******************************************************
 *****                                             *****	
 *****  Vca::VRexecConnectionFactory::Transaction  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

namespace Vca {
    class VRexecConnectionFactory::Transaction : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Transaction, VRolePlayer);
	
    // Typedefs
    public:
	//  Byte Stream Types
	typedef VBSConsumer BSConsumer;
	typedef IBSConsumerClient BSConsumerClient;

	typedef VBSProducer BSProducer;
	typedef IBSProducerClient BSProducerClient;

	//  Listener Name Receiver	
	typedef VReceiver<ThisClass,VString const&> NameReceiver;

    //  Construction
    public:
	Transaction (
	    VDeviceFactory *pDeviceFactory,
	    IPipeSourceClient *pClient,
	    VString const &rHost, 
	    VString const &rUsername,
	    VString const &rPassword,
	    VString const &rCommand,
	    bool bErrorChannel,
	    bool bNoDelay,
	    U32 xHostPort
	);

    //  Destruction	
    protected:
	~Transaction (); 
    
    //  Transaction Methods
    private:
	void initiate ();

	bool doConnection ();
	void doDataTransfer ();
	bool setupErrorChannel ();

	void doDataWrite ();
	void doAckRead ();
	void doErrorRead ();

	void onAcknowledgement ();
	void onError (VString const &rErrorMessage);
	void onComplete ();

    //  Byte Stream Client Roles
    private:
	VRole<ThisClass,IBSConsumerClient> m_pBSConsumerClient;
	void getRole (BSConsumerClient::Reference &rpRole) {
	    m_pBSConsumerClient.getRole (rpRole);
	}

	VRole<ThisClass,IBSProducerClient> m_pBSProducerClient;
	void getRole (BSProducerClient::Reference &rpRole) {
	    m_pBSProducerClient.getRole (rpRole);
	}

    //  Byte Stream Client Methods
    public/*private*/:
	void OnTransfer (BSConsumerClient *pRole, size_t sTransfer);
	void OnTransfer (BSProducerClient *pRole, size_t sTransfer);

    //  IClient Role
    public/*private*/:
	void OnError (
	    IClient *pRole, IError *pError, VString const &rDescription
	);
	void OnEnd (IClient *pRole);

    //  IPipeSourceClient Role
    private:
	VRole<ThisClass,IPipeSourceClient> m_pIPipeSourceClient;
    public:
	void getRole (IPipeSourceClient::Reference &rpRole) {
	    m_pIPipeSourceClient.getRole (rpRole);
	}

    //  IPipeSourceClient Methods
    public/*private*/:
	void OnData (
	    IPipeSourceClient *pRole, VBSConsumer *pBSToPeer, VBSProducer *pBSToHere, VBSProducer *pErrToHere
	);

    //  Error Channel ListenerName Receivers
	void onListenerName (NameReceiver *pRole, VString const &rName);
	void onListenerNameError (
	    NameReceiver *pRole, IError *pError, VString const &rMessage
	);

    //  State
    private:
	VString	const	m_iHost;
	U32	const	m_xPort;
	VString	const	m_iUsername;
	VString	const	m_iPassword;
	VString	const	m_iCommand;
	bool	const	m_bErrorChannel;
	bool	const	m_bNoDelay;
	VString		m_iErrorChannelPort;

	IPipeSourceClient::Reference  const m_pClient;
	VDeviceFactory::Reference     const m_pDeviceFactory;

	BSConsumer::Reference	m_pBSConsumer;
	BSProducer::Reference	m_pBSProducer;
	BSProducer::Reference	m_pBSErrorProducer;

	char	m_iAckByte;
	bool	m_bReceivedAckByte;

	V::VBlob	m_iOutgoingBlob;
	V::VBlob	m_iIncomingErrorBlob;	// used for error data
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRexecConnectionFactory::Transaction::Transaction (
    VDeviceFactory *pDeviceFactory,
    IPipeSourceClient *pClient,
    VString const &rHost, 
    VString const &rUsername,
    VString const &rPassword, 
    VString const &rCommand,
    bool bErrorChannel,
    bool bNoDelay,
    U32 xHostPort
) : m_pDeviceFactory (pDeviceFactory)
  , m_pClient (pClient)
  , m_iHost (rHost)
  , m_xPort (xHostPort)
  , m_iUsername (rUsername)
  , m_iPassword (rPassword)
  , m_iCommand (rCommand)
  , m_bReceivedAckByte (false)
  , m_iAckByte ('-')
  , m_iIncomingErrorBlob (4096)
  , m_bErrorChannel (bErrorChannel)
  , m_bNoDelay (bNoDelay)
  , m_pBSConsumerClient  (this)
  , m_pBSProducerClient  (this)
  , m_pIPipeSourceClient (this) {
    traceInfo ("Creating VRexecConnectionFactory::Transaction");
    retain ();
    initiate ();
    untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRexecConnectionFactory::Transaction::~Transaction () {
    traceInfo ("Destroying VRexecConnectionFactory::Transaction");
}


/**********************************
 **********************************
 *****  Initiate Transaction  *****
 **********************************
 **********************************/

void Vca::VRexecConnectionFactory::Transaction::initiate () {
    if (doConnection ()) {
	if (m_bErrorChannel) 
	    setupErrorChannel ();
	else 
	    doDataTransfer ();
    }
}

/************************
 ************************
 *****  Connection  *****
 ************************
 ************************/

bool Vca::VRexecConnectionFactory::Transaction::doConnection () {
    VString iServer (m_iHost);
    iServer << ":" << m_xPort;

    VStatus iStatus;
    if (!m_pDeviceFactory->supply (iStatus, m_pBSProducer, m_pBSConsumer, iServer, m_bNoDelay)) {
	iStatus.communicateTo (m_pClient);
	m_pClient->OnError (0, "Connection to Rexec daemon server failed");
	return false;
    }

    m_pBSErrorProducer.setTo (m_pBSProducer);
    log ("Established connection with rexec server");
    return true;
}

/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VRexecConnectionFactory::Transaction::doDataTransfer () {
    doDataWrite ();
    doAckRead ();
}

void Vca::VRexecConnectionFactory::Transaction::doDataWrite () {
    BSConsumerClient::Reference pConsumerClient;
    getRole (pConsumerClient);

    m_iOutgoingBlob.append (m_iErrorChannelPort, m_iErrorChannelPort.length ()+1);
    m_iOutgoingBlob.append (m_iUsername, m_iUsername.length ()+1);
    m_iOutgoingBlob.append (m_iPassword, m_iPassword.length ()+1);
    m_iOutgoingBlob.append (m_iCommand, m_iCommand.length ()+1);

    OnTransfer (
	pConsumerClient, m_pBSConsumer->put (
	    pConsumerClient, m_iOutgoingBlob.contentArea (), m_iOutgoingBlob.contentSize ()
	)
    );
}

void Vca::VRexecConnectionFactory::Transaction::doAckRead () {
    BSProducerClient::Reference pProducerClient;
    getRole (pProducerClient);
    
    log ("Vca::VRexecConnectionFactory::doAckRead ...");
    OnTransfer (
	pProducerClient, m_pBSProducer->get (
	    pProducerClient, &m_iAckByte, sizeof (m_iAckByte), sizeof (m_iAckByte)
	)
    );
}

void Vca::VRexecConnectionFactory::Transaction::doErrorRead () {
    BSProducerClient::Reference pProducerClient;
    getRole (pProducerClient);

    log ("Vca::VRexecConnectionFactory::doErrorRead ...");
    OnTransfer (
	pProducerClient, m_pBSProducer->get (
	    pProducerClient,
	    m_iIncomingErrorBlob.postContentArea (), 1,
	    m_iIncomingErrorBlob.postContentSize ()
	)
    );
}

/***************************
 ***************************
 *****  Error Channel  *****
 ***************************
 ***************************/

bool Vca::VRexecConnectionFactory::Transaction::setupErrorChannel () {
    log ("Rexec client setting up error channel");

    //  ...use port 0 to generate a dynamic port
    VString iAddress;
    iAddress << 0;

    VStatus iStatus;
    VListener::Reference pListener;

    if (!(m_pDeviceFactory->supply (iStatus, pListener, iAddress, m_bNoDelay)))  {
	iStatus.communicateTo (m_pClient);
	m_pClient->OnError (0, "RexecConnectionFactory Error port listen failed");
	return false;
    }

    //  ...get the dynamic port assigned
    NameReceiver::Reference pNameReceiver (
	new NameReceiver (
	    this, &ThisClass::onListenerName, &ThisClass::onListenerNameError
	)
    );
    IListener::Reference pIListener;
    pListener->getRole (pIListener);
    pIListener->GetName (pNameReceiver);

    //  ...set the pipe source client to get the error channel connection
    IPipeSourceClient::Reference pPipeSourceClient;
    getRole (pPipeSourceClient);
    pListener->SupplyByteStreams (pPipeSourceClient, 1);
    return true;
}

/***********************************
 ***********************************
 *****  Byte Stream Callbacks  *****
 ***********************************
 ***********************************/

/**********************
 *****  Transfer  *****
 **********************/

void Vca::VRexecConnectionFactory::Transaction::OnTransfer (
    BSConsumerClient *pRole, size_t sTransfer
) {
    log ("Rexec client. OnTransfer (Consumer) %d bytes", sTransfer);
    if (sTransfer > 0) {
	m_iOutgoingBlob.incrementContentLowerBoundBy (sTransfer);
    }

}

void Vca::VRexecConnectionFactory::Transaction::OnTransfer (
    BSProducerClient *pRole, size_t sTransfer
) {
     log ("Rexec client. OnTransfer (Producer) %d bytes", sTransfer);
     if (sTransfer > 0) {

	if (!m_bReceivedAckByte && sTransfer == 1) 
	    onAcknowledgement ();

	else { //  error data
	    m_iIncomingErrorBlob.incrementContentUpperBoundBy (sTransfer);

	    if (m_pBSProducer.isntNil ()) 
		doErrorRead ();
	}
    }
}

/*******************
 *****  Error  *****
 *******************/

void Vca::VRexecConnectionFactory::Transaction::OnError (
    IClient *pRole, IError *pError, VString const &rMessage
) {
    log ("Rexec client. Transaction Error %s", rMessage.content ());
    m_pClient->OnError (0, rMessage);
}

void Vca::VRexecConnectionFactory::Transaction::OnEnd (IClient *pRole) {
    VString iErrorMessage;
    iErrorMessage.append (
	m_iIncomingErrorBlob.contentArea (), m_iIncomingErrorBlob.contentSize ()
    );

    log ("Rexec client. OnEnd %s", iErrorMessage.content ());
    m_pClient->OnError (0, iErrorMessage.content ());
}


/**************************************************
 *****  ErrorChannel Listener Name Receivers  *****
 **************************************************/


void Vca::VRexecConnectionFactory::Transaction::onListenerName (
    NameReceiver *pRole, VString const &rName
) {
    log ("Rexec error channel dynamic listener is %s", rName.content ());
    char const *pPort;
    if ((pPort = strchr (rName.content (), ':'))) {
	m_iErrorChannelPort.setTo (pPort+1);
	
	//  ...write/read data to/from the server...
	doDataTransfer ();
    }
    else
	m_pClient->OnError (0, "Rexec ErrorChannel port creation error");
}

void Vca::VRexecConnectionFactory::Transaction::onListenerNameError (
    NameReceiver *pRole, IError *pError, VString const &rName) {
    log ("Rexec client. Error receiving listener");
    m_pClient->OnError (0, rName.content ());
}


/*****************************************
 *****************************************
 *****  IPipeSourceClient CallBacks  *****
 *****************************************
 *****************************************/

void Vca::VRexecConnectionFactory::Transaction::OnData (
    IPipeSourceClient *pRole,
    VBSConsumer *pBSToPeer, VBSProducer *pBSToHere, VBSProducer *pErrToHere
) {
    log ("Rexec client. Received error channel connection from rexecd");
    m_pBSErrorProducer.setTo (pBSToHere);

    //  ...check to see if null ack byte has already been received
    if (m_iAckByte == '\0')
	onComplete ();
}

/********************
 ********************
 *****  Result  *****
 ********************
 ********************/

void Vca::VRexecConnectionFactory::Transaction::onAcknowledgement () {
    log ("Rexec client has read the ack byte %c", m_iAckByte);

    m_bReceivedAckByte = true;

    if (m_iAckByte != '\0') 
	doErrorRead ();
    else if (!m_bErrorChannel) 
	onComplete ();
    else if (m_pBSErrorProducer.referent () != m_pBSProducer) 
	onComplete ();
}

void Vca::VRexecConnectionFactory::Transaction::onComplete () {
    log ("Rexec client. Transaction complete...Sending BSs");
    m_pClient->OnData (m_pBSConsumer, m_pBSProducer, m_pBSErrorProducer);
}

void Vca::VRexecConnectionFactory::Transaction::onError (VString const &rMessage) {
    log ("Rexec client. Transaction Error %s", rMessage.content ());
    m_pClient->OnError (0, rMessage);
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VRexecConnectionFactory  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRexecConnectionFactory::VRexecConnectionFactory (
	VDeviceFactory *pDeviceFactory
) : BaseClass (pDeviceFactory->cohort ()), m_pDeviceFactory (pDeviceFactory), m_pIRexecConnectionFactory (this) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRexecConnectionFactory::~VRexecConnectionFactory () {
    traceInfo ("Destroying Vca::VRexecConnectionFactory");

    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VRexecConnectionFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VRexecConnectionFactory::Supply (Reference&rpInstance) {
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

bool Vca::VRexecConnectionFactory::Supply (ThisInterface::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}

/*********************************************
 *********************************************
 *****  IRexecConnectionFactory Methods  *****
 *********************************************
 *********************************************/

void Vca::VRexecConnectionFactory::MakeRexecConnection (
    IRexecConnectionFactory *pRole, 
    IPipeSourceClient *pClient, 
    VString const &rHost, U32 xPort, 
    VString const &rUsername, VString const &rPassword, 
    VString const &rCommand, bool bErrorChannel
) {
    supply (
	pClient, rHost, rUsername, rPassword, rCommand, bErrorChannel, Default::TcpNoDelaySetting (), xPort
    );
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

bool Vca::VRexecConnectionFactory::Supply (
    IPipeSourceClient *pClient,
    VString const &rHostname, VString const &rUserName, VString const &rPassword, VString const &rCommand,
    bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	pClient, rHostname, rUserName, rPassword, rCommand, bErrorChannel, bNoDelay, xPort
    );
}

bool Vca::VRexecConnectionFactory::supply (
    IPipeSourceClient *pClient, 
    VString const &rHostname, VString const &rUserName, VString const &rPassword, VString const &rCommand,
    bool bErrorChannel, bool bNoDelay, U32 xPort
) {
    bool bStarted = false;
    if (pClient) {
	Transaction::Reference const pTransaction (
	    new Transaction (
		m_pDeviceFactory, pClient, rHostname, rUserName, rPassword, rCommand, bErrorChannel, bNoDelay, xPort
	    )
	);
	bStarted = true;
    }
    return bStarted;
}

bool Vca::VRexecConnectionFactory::Supply (
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

bool Vca::VRexecConnectionFactory::supply (
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
