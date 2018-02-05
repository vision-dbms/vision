/*****  Vca::VBSConnector Implementation  *****/

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

#include "Vca_VBSConnector.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "V_VBlob.h"

#include "Vca_IBSClient.h"
#include "Vca_VStatus.h"
#include "Vca_VBSManager.h"
#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"


/**********************************
 **********************************
 *****                        *****
 *****  VBSConnector::Buffer  *****
 *****                        *****
 **********************************
 **********************************/

namespace Vca {
    class VBSConnector::Buffer : public VReferenceable {
	DECLARE_CONCRETE_RTTLITE (Buffer, VReferenceable);

    //  Friend
	friend class VBSConnector::BufferQueue;

    //  Construction
    public:
	Buffer (size_t sBuffer) : m_iStorage (sBuffer) {
	}

    //  Destruction
    private:
	~Buffer () {
	}

    //  Access
    public:
	void const *dataArea () const {
	    return m_iStorage.contentArea ();
	}
	size_t dataSize () const {
	    return m_iStorage.contentSize ();
	}

	void *voidArea () {
	    return m_iStorage.postContentArea ();
	}
	size_t voidSize () const {
	    return m_iStorage.postContentSize ();
	}
	IBSClient* getBSClient () const {
	    return m_pBSClient;
	}
      
    //  Update
    public:
	size_t produce (size_t sTransfer) {
	    return m_iStorage.incrementContentUpperBoundBy (sTransfer);
	}
	size_t consume (size_t sTransfer) {
	    return m_iStorage.incrementContentLowerBoundBy (sTransfer);
	}
	void reset () {
	    m_iStorage.setContentBoundsTo (0, 0);
	}
	void setBSClient (IBSClient *pBSClient) {
  	    m_pBSClient.setTo (pBSClient);
	}

    //  State
    private:
	Reference		m_pSuccessor;
	V::VBlob		m_iStorage;
	IBSClient::Reference	m_pBSClient;
    };
}


/***************************************
 ***************************************
 *****                             *****
 *****  VBSConnector::BufferQueue  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConnector::BufferQueue::BufferQueue () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConnector::BufferQueue::~BufferQueue () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VBSConnector::BufferQueue::enqueue (Buffer::Reference &rpBuffer) {
    if (m_pTail)
	m_pTail->m_pSuccessor.setTo (rpBuffer);
    else
	m_pHead.setTo (rpBuffer);
    m_pTail.claim (rpBuffer);
}

bool Vca::VBSConnector::BufferQueue::dequeue (Buffer::Reference &rpBuffer) {
    bool bHeaded = m_pHead.isntNil ();
    if (!bHeaded)
	rpBuffer.clear ();
    else {
	rpBuffer.setTo (m_pHead);
	m_pHead.claim (rpBuffer->m_pSuccessor);
	if (m_pHead.isNil ())
	    m_pTail.clear ();
    }
    return bHeaded;
}


/************************************
 ************************************
 *****                          *****
 *****  VBSConnector::BSClient  *****
 *****                          *****
 ************************************
 ************************************/

namespace Vca {
    class VBSConnector::BSClient : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (BSClient, VRolePlayer);

    //  Construction
    protected:
	BSClient (VBSConnector *pConnector) : m_pConnector (pConnector), m_pIBSClient (this) {
	}

    //  Destruction
    protected:
	~BSClient () {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IBSClient Role
    private:
	VRole<ThisClass,IBSClient> m_pIBSClient;
    public:
	void getRole (IBSClient::Reference &rpRole) {
	    m_pIBSClient.getRole (rpRole);
	}

    //  IBSClient Methods
    public/*private*/:
	void OnTransfer	(IBSClient *pRole, size_t sTransfer);
	void OnEnd	(IClient *pRole);
	void OnError	(IClient *pRole, IError *pError, VString const &rMessage);

    //  Access
    public:
	VBSConnector *connector () const {
	    return m_pConnector;
	}

    //  Buffer Management
    protected:
	bool acquireBuffer (Buffer::Reference &rpBuffer) const {
	    return m_pConnector->acquireBuffer (rpBuffer);
	}
	bool releaseBuffer (Buffer::Reference &rpBuffer) const {
	    return m_pConnector->releaseBuffer (rpBuffer);
	}

    //  Callback
    private:
	virtual void onEnd_() = 0;
	virtual void onError_(IError *pError, VString const &rMessage) = 0;
	virtual void onTransfer_(size_t sTransfer) = 0;
      
    //  State
    protected:
	VBSConnector::Reference const m_pConnector;
    };
}


/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

void Vca::VBSConnector::BSClient::OnEnd (IClient *pRole) {
    onEnd_();
}

void Vca::VBSConnector::BSClient::OnError (
    IClient *pRole, IError *pError, VString const &rMessage
) {
    onError_(pError, rMessage);
}

void Vca::VBSConnector::BSClient::OnTransfer (IBSClient *pRole, size_t sTransfer) {
    onTransfer_(sTransfer);
}




/********************************************
 ********************************************
 *****                                  *****
 *****  VBSConnector::BSConsumerClient  *****
 *****                                  *****
 ********************************************
 ********************************************/

namespace Vca {
    class VBSConnector::BSConsumerClient : public VBSConnector::BSClient {
	DECLARE_CONCRETE_RTTLITE (BSConsumerClient, VBSConnector::BSClient);

    //  Manager
	class Manager : public VBSManager {
	    DECLARE_CONCRETE_RTTLITE (Manager, VBSManager);
    
	//  Construction  
	public:	
	    Manager (BSConsumerClient *pClient);

	//  Destruction
	private:
	    ~Manager ();
	
	//  Callbacks
	public:
	    virtual void onEnd      () OVERRIDE {}
	    virtual void onError    (IError *pError, VString const &rMessage) OVERRIDE {}
	    virtual void onTransfer (size_t sTransfer) OVERRIDE {}
	    virtual bool onStatus   (VStatus const &rStatus) OVERRIDE;
	    virtual void onChange   (U32 sChange) OVERRIDE {}
    
	//  Query
	public:
	    BSConsumerClient* client () const {
		return m_pBSConsumerClient;
	    }

	//  State  
	protected:
	    BSConsumerClient *m_pBSConsumerClient; // maintain a soft reference, to prevent cyclic referencing
	};

  
    //  Construction
    public:
	BSConsumerClient (VBSConnector *pConnector, VBSConsumer *pConsumer);

    //  Destruction
    private:
	~BSConsumerClient ();

    //  Access
    public:
	VBSConsumer *byteStream () const {
	    return m_pConsumer;
	}

    //  Callback
    private:
	virtual void onEnd_() OVERRIDE;
	virtual void onError_(IError *pError, VString const &rMessage) OVERRIDE;
	virtual void onTransfer_(size_t sTransfer) OVERRIDE;

    //  Execution
    public:
	void consume (Buffer::Reference &rpBuffer);
    private:
	void consume (size_t sTransfer);

	void run ();

	void stop ();

    //  State
    private:
	VBSConsumer::Reference	m_pConsumer;
	Buffer::Reference	m_pBuffer;
	BufferQueue		m_iBufferQueue;
	bool                    m_bAborting;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConnector::BSConsumerClient::BSConsumerClient (
    VBSConnector *pConnector, VBSConsumer *pConsumer
) : BaseClass (pConnector), m_pConsumer (pConsumer), m_bAborting (false) {
    traceInfo ("Creating BSConsumerClient");

    Manager::Reference pManager (new Manager (this));
    m_pConsumer->registerManager (pManager);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConnector::BSConsumerClient::~BSConsumerClient () {
    traceInfo ("Destroying BSConsumerClient");
    if (m_pConsumer)
	m_pConsumer->Close ();
}

/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

void Vca::VBSConnector::BSConsumerClient::onEnd_() {
    stop ();
}

void Vca::VBSConnector::BSConsumerClient::onError_(IError *pError, VString const &rMessage) {
    stop ();
}

void Vca::VBSConnector::BSConsumerClient::onTransfer_(size_t sTransfer) {
    consume (sTransfer);
    run ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vca::VBSConnector::BSConsumerClient::consume (Buffer::Reference &rpBuffer) {
    if (!m_bAborting) {
      m_iBufferQueue.enqueue (rpBuffer);
      run ();
    } else if (rpBuffer && rpBuffer->getBSClient ()) {
      IBSClient::Reference pClient (rpBuffer->getBSClient ());
      pClient->OnEnd ();
    }
}

void Vca::VBSConnector::BSConsumerClient::consume (size_t sTransfer) {
    if (sTransfer > 0) 
	m_pBuffer->consume (sTransfer);
}

void Vca::VBSConnector::BSConsumerClient::run () {
    while (m_pConsumer && releaseBuffer (m_pBuffer) && m_iBufferQueue.dequeue (m_pBuffer)) {
	IBSClient::Reference pRole;
	getRole (pRole);
	consume (
	    m_pConsumer->put (
		pRole, m_pBuffer->dataArea (), m_pBuffer->dataSize ()
	    )
	);
    }
}

void Vca::VBSConnector::BSConsumerClient::stop () {
  m_pConsumer.clear ();

  if (m_pBuffer && m_pBuffer->getBSClient ()) {
    IBSClient::Reference pClient (m_pBuffer->getBSClient ());
    pClient->OnEnd ();
  }
  else 
    m_bAborting = true;
}

/******************************************************
 ******************************************************
 *****                                            *****
 *****  VBSConnector::BSConsumerClient::Manager   *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConnector::BSConsumerClient::Manager::Manager (
    BSConsumerClient *pBSClient
) : m_pBSConsumerClient (pBSClient) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConnector::BSConsumerClient::Manager::~Manager () {
}

/*********************************
**********************************
 *****  BSManager Callbacks  *****
 *********************************
 *********************************/

bool Vca::VBSConnector::BSConsumerClient::Manager::onStatus (VStatus const &rStatus) {
    bool bClosedOrFailed = false;
    if (rStatus.isClosed () || rStatus.isFailed ()) {
	bClosedOrFailed = true;
	IBSClient::Reference pClient;
	m_pBSConsumerClient->getRole (pClient);
        pClient->OnEnd ();
    }
    return bClosedOrFailed;
}

/********************************************
 ********************************************
 *****                                  *****
 *****  VBSConnector::BSProducerClient  *****
 *****                                  *****
 ********************************************
 ********************************************/

namespace Vca {
    class VBSConnector::BSProducerClient : public VBSConnector::BSClient {
	DECLARE_CONCRETE_RTTLITE (BSProducerClient, VBSConnector::BSClient);

    //  Construction
    public:
	BSProducerClient (BSConsumerClient *pConsumer, VBSProducer *pProducer);

    //  Destruction
    private:
	~BSProducerClient ();

    //  Access
    public:
	VBSProducer *byteStream () const {
	    return m_pProducer;
	}
	BSConsumerClient *consumer () const {
	    return m_pConsumer;
	}

    //  Callback
    private:
	virtual void onEnd_() OVERRIDE;
	virtual void onError_(IError *pError, VString const &rMessage) OVERRIDE;
	virtual void onTransfer_(size_t sTransfer) OVERRIDE;

    //  Execution
    private:
	void produce (size_t sTransfer);

	void run ();

	void stop ();

    //  State
    private:
	VBSProducer::Reference		m_pProducer;
	Buffer::Reference		m_pBuffer;
	BSConsumerClient::Reference	m_pConsumer;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConnector::BSProducerClient::BSProducerClient (
    BSConsumerClient *pConsumer, VBSProducer *pProducer
) : BaseClass (pConsumer->connector ()), m_pConsumer (pConsumer), m_pProducer (pProducer) {
    traceInfo ("Creating BSProducerClient");
    retain (); {
	run ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConnector::BSProducerClient::~BSProducerClient () {
    traceInfo ("Destroying BSProducerClient");
    if (m_pProducer)
	m_pProducer->Close ();
}

/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

void Vca::VBSConnector::BSProducerClient::onEnd_() {
    stop ();
}

void Vca::VBSConnector::BSProducerClient::onError_(IError *pError, VString const &rMessage) {
    stop ();
}

void Vca::VBSConnector::BSProducerClient::onTransfer_(size_t sTransfer) {
    produce (sTransfer);
    run ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vca::VBSConnector::BSProducerClient::produce (size_t sTransfer) {
    if (sTransfer > 0) {
	m_pBuffer->produce (sTransfer);
	if (m_pConsumer)
	    m_pConsumer->consume (m_pBuffer);
    }
}

void Vca::VBSConnector::BSProducerClient::run () {
    while (m_pProducer && acquireBuffer (m_pBuffer)) {
	IBSClient::Reference pRole;
	getRole (pRole);
	m_pBuffer->setBSClient (pRole);
	produce (
	    m_pProducer->get (
		pRole, m_pBuffer->voidArea (), 1, m_pBuffer->voidSize ()
	    )
	);
    }
}

void Vca::VBSConnector::BSProducerClient::stop () {
    m_pProducer.clear ();
    if (m_pConsumer && m_pBuffer) {
	BSConsumerClient::Reference pConsumer = m_pConsumer;      
	m_pConsumer.clear ();
	pConsumer->consume (m_pBuffer);
    }
}

/**************************
 **************************
 *****                *****
 *****  VBSConnector  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConnector::VBSConnector (
    VBSProducer *pProducer, VBSConsumer *pConsumer
) : m_sBuffer (V_Max (pProducer->bufferSize (), pConsumer->bufferSize ())) {
    traceInfo ("Creating VBSConnector");
    retain (); {
	BSConsumerClient::Reference pConsumerClient (
	    new BSConsumerClient (this, pConsumer)
	);
	BSProducerClient::Reference pProducerClient (
	    new BSProducerClient (pConsumerClient, pProducer)
	);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConnector::~VBSConnector () {
    traceInfo ("Destroying VBSConnector");
}

/*******************************
 *******************************
 *****  Buffer Management  *****
 *******************************
 *******************************/

bool Vca::VBSConnector::acquireBuffer (Buffer::Reference &rpBuffer) {
    if (rpBuffer.isntNil ())
	return false;

    if (!m_iBufferFreeList.dequeue (rpBuffer))
	rpBuffer.setTo (new Buffer (m_sBuffer));

    return true;
}

bool Vca::VBSConnector::releaseBuffer (Buffer::Reference &rpBuffer) {
    if (rpBuffer.isNil ())
	return true;

    if (rpBuffer->dataSize () > 0)
	return false;
    
    rpBuffer->reset ();
    m_iBufferFreeList.enqueue (rpBuffer);

    return true;
}
