/*****  Vca::VBSConsumer Implementation  *****/

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

#include "Vca_VBSConsumer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaSerializer_.h"

#include "V_VSpinlock.h"


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::VBSConsumer::ConsumerExportStub  *****
 *****                                        *****
 **************************************************
 **************************************************/

namespace Vca {
    class VBSConsumer::ConsumerExportStub : public VBS::ExportStub {
	DECLARE_CONCRETE_RTTLITE (ConsumerExportStub, VBS::ExportStub);

    //  Client
    public:
	typedef IVReceiver<U32> Client;

    //  Request
    public:
	class Request : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Request, VTransient);

	//  Construction
	public:
	    Request (Client *pClient, Data &rData)
		: m_pClient (pClient), m_iBlob (rData)
	    {
	    }
	    Request (Request const &rOther)
		: m_pClient (rOther.m_pClient), m_iBlob (rOther.m_iBlob)
	    {
	    }
	    Request () {
	    }

	//  Destruction
	public:
	    ~Request () {
	    }

	//  Access
	public:
	    Data &data () const {
		return m_iBlob;
	    }

	//  Query
	public:
	    bool isSatisfied () const {
		return m_iBlob.contentSize () == 0;
	    }
	    bool isntSatisfied () const {
		return m_iBlob.contentSize () > 0;
	    }

	//  Client Communication
	public:
	    void onEnd ();
	    void onError (IError *pError, VString const &rMessage);

	    size_t onTransfer (size_t sTransfer);

	//  Update
	public:
	    void clear () {
		m_pClient.clear ();
		m_iBlob.clear ();
	    }
	    void setTo (Request const &rOther) {
		m_pClient.setTo (rOther.m_pClient);
		m_iBlob.setTo (rOther.m_iBlob);
	    }
	    Request &operator= (Request const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  State
	private:
	    VReference<Client>	m_pClient;
	    Blob		m_iBlob;
	};

    //  Construction
    public:
	ConsumerExportStub (BS *pBS);

    //  Destruction
    private:
	~ConsumerExportStub () {
	}

    //  IBSConsumer Role
    private:
	VRole<ThisClass,IBSConsumer> m_pIBSConsumer;
    public:
	void getRole (VReference<IBSConsumer>&rpRole) {
	    m_pIBSConsumer.getRole (rpRole);
	}

    //  IBSConsumer Methods
    public/*private*/:
	void Put (
	    IBSConsumer *pRole, Client *pClient, V::VBlob const &rData
	);

    //  Access/Query
    protected:
	bool active () const {
	    return m_iQueue.active ();
	}

	Data &activeData () const {
	    return activeRequest().data ();
	}

	Request const &activeRequest () const {
	    return m_iQueue.activeRequest ();
	}
	Request &activeRequest () {
	    return m_iQueue.activeRequest ();
	}

    //  Communication
    private:
	void onEnd () OVERRIDE;
	void onError (IError *pError, VString const &rMessage) OVERRIDE;
	void onTransfer (size_t sTransfer) OVERRIDE;

    //  Queueing
    private:
	bool nextRequest ();

    //  State
    private:
	V::VQueue<Request> m_iQueue;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConsumer::ConsumerExportStub::ConsumerExportStub (BS *pBS) : BaseClass (pBS), m_pIBSConsumer (this) {
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************
 *****  Vca::IBSConsumer  *****
 ******************************/

void Vca::VBSConsumer::ConsumerExportStub::Put (IBSConsumer *pRole, Client *pClient, Data &rData) {
    if (pClient) {
	Request iRequest (pClient, rData);
	if (m_iQueue.enqueue (iRequest))
	    put (activeData ());
    }
}


/***************************
 ***************************
 *****  Communication  *****
 ***************************
 ***************************/

void Vca::VBSConsumer::ConsumerExportStub::onEnd () {
    activeRequest ().onEnd ();
    nextRequest ();
}

void Vca::VBSConsumer::ConsumerExportStub::onError (IError *pError, VString const &rMessage) {
    activeRequest ().onError (pError, rMessage);
    nextRequest ();
}

void Vca::VBSConsumer::ConsumerExportStub::onTransfer (size_t sTransfer) {
    bool notDone = active ();
    while (notDone && sTransfer > 0) {
	Request &rRequest = activeRequest ();
	sTransfer -= rRequest.onTransfer (sTransfer);
	notDone = rRequest.isSatisfied () && nextRequest ();
    }
}

/**********************
 **********************
 *****  Queueing  *****
 **********************
 **********************/

bool Vca::VBSConsumer::ConsumerExportStub::nextRequest () {
    return m_iQueue.dequeue ();
}


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  Vca::VBSConsumer::ConsumerExportStub::Request  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

/**********************************
 **********************************
 *****  Client Communication  *****
 **********************************
 **********************************/

void Vca::VBSConsumer::ConsumerExportStub::Request::onEnd () {
    if (m_pClient)
	m_pClient->OnEnd ();
}

void Vca::VBSConsumer::ConsumerExportStub::Request::onError (IError *pError, VString const &rMessage) {
    if (m_pClient)
	m_pClient->OnError (pError, rMessage);
}

//  Returns the size of the transfer absorbed by this request...
size_t Vca::VBSConsumer::ConsumerExportStub::Request::onTransfer (size_t sTransfer) {
    size_t sAdjustment = m_iBlob.incrementContentLowerBoundBy (sTransfer);
    m_pClient->OnData (sAdjustment);
    return sAdjustment;
}


/******************************
 ******************************
 *****                    *****
 *****  Vca::VBSConsumer  *****
 *****                    *****
 ******************************
 ******************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VBSConsumer::Pointer Vca::VBSConsumer::g_pFirstInstance;

/********************************
 ********************************
 *****  Construction Mutex  *****
 ********************************
 ********************************/

namespace {
    typedef V::VSpinlock Mutex;
    typedef Mutex::Claim MutexClaim;

    static Mutex g_iMutex;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSConsumer::VBSConsumer (VCohort *pCohort) : BaseClass (pCohort) {
    MutexClaim iClaim (g_iMutex);
    if (g_pFirstInstance) {
	m_pNextInstance.setTo (g_pFirstInstance);
	m_pNextInstance->m_pLastInstance.setTo (this);
    }
    g_pFirstInstance.setTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VBSConsumer::~VBSConsumer () {
    MutexClaim iClaim (g_iMutex);
    if (m_pLastInstance)
	m_pLastInstance->m_pNextInstance.setTo (m_pNextInstance);
    else
	g_pFirstInstance.setTo (m_pNextInstance);
    if (m_pNextInstance)
	m_pNextInstance->m_pLastInstance.claim (m_pLastInstance);
}

/********************
 ********************
 *****  Export  *****
 ********************
 ********************/

void Vca::VBSConsumer::getExportStub (VReference<ConsumerExportStub>&rpStub) {
    rpStub.setTo (new ConsumerExportStub (this));
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*************************
 *****  IBSConsumer  *****
 *************************/

void Vca::VBSConsumer::getRole (VReference<IBSConsumer>&rpRole) {
    VReference<ConsumerExportStub> pExportStub;
    getExportStub (pExportStub);
    pExportStub->getRole (rpRole);
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

//  returns the amount of data available to be consumed...
size_t Vca::VBSConsumer::transferPending () const {
    return m_iBufferFifo.consumerAreaSize () + activeRequestConsumerAreaSize ();
}

/***********************
 ***********************
 *****  Scheduling  ****
 ***********************
 ***********************/

void Vca::VBSConsumer::run_() {
    suspend ();
    onPutContinuation ();
}

/**********************
 **********************
 *****  Transfer  *****
 **********************
 **********************/

void Vca::VBSConsumer::PutBufferedData () {
    enum {
	QuiescenceCount = 2
    };
    unsigned int xPassesToGo = QuiescenceCount + (g_pFirstInstance ? g_pFirstInstance->m_cNumCheckpoints : 0);
    //   Loop until no work is done for QuiescenceCount consecutive passes...
    do {
	VReference<ThisClass> pInstance (g_pFirstInstance);
	while (pInstance) {
	    if (pInstance->putBufferedData ())
		xPassesToGo = QuiescenceCount + pInstance->m_cNumCheckpoints;
	    pInstance.setTo (pInstance->m_pNextInstance);
	}
    } while (--xPassesToGo > 0);
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VBSConsumerProxy  *****
 *****                         *****
 ***********************************
 ***********************************/

namespace Vca {
    class VBSConsumerProxy : public VBSConsumer {
	DECLARE_CONCRETE_RTTLITE (VBSConsumerProxy, VBSConsumer);

    //  ExportStubClient
    public:
	typedef ConsumerExportStub::Client ExportStubClient;

    //  Construction
    public:
	VBSConsumerProxy (
	    IBSConsumer *pRemote
	) : BaseClass (VCohort::Here ()), m_pData (0), m_sData (0), m_sVoid (0), m_pRemote (pRemote), m_pResultSink (this) {
	}

    //  Destruction
    private:
	~VBSConsumerProxy () {
	}

    //  ConsumerExportStub::Client Role
    private:
	VRole<ThisClass,ExportStubClient> m_pResultSink;
    public:
	using BaseClass::getRole;
	void getRole (VReference<ExportStubClient>&rpRole) {
	    m_pResultSink.getRole (rpRole);
	}

    //  ConsumerExportStub::Client Methods
    public/*private*/:
	void OnData (ExportStubClient *pRole, U32 sVoid) {
	    if (m_sVoid > 0) {
		fprintf (
		    stderr, "%s %p:OnData: Orphaned %lu void bytes\n", rttName ().content (), this,
		    m_sVoid
		);
		fflush (stderr);
	    }
	    m_sVoid += sVoid;
	    onPutContinuation ();
	}
	void OnEnd (IClient *pRole) OVERRIDE {
	    onEnd ();
	}
	void OnError (IClient *pRole, IError *pError, VString const &rMessage) OVERRIDE {
	    onError (pError, rMessage);
	}

    //  Control
    private:
	void endTransfers () OVERRIDE {
	    m_pRemote->Close ();
	}

    //  Data Transfer
    private:
	void transferData () OVERRIDE;

    //  State
    private:
	VReference<IBSConsumer> const	m_pRemote;
	char const*			m_pData;    //  ... outgoing from getArea
	size_t				m_sData;    //  ... outgoing from getArea
	size_t				m_sVoid;    //  ... incoming from remote
    };
}


/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VBSConsumerProxy::transferData () {
    if (m_sData > 0 && m_sVoid > 0) {
	size_t sCopy = m_sVoid;
	if (sCopy > m_sData)
	    sCopy = m_sData;
	onPutCompletion (sCopy);
	m_sVoid -= sCopy;
	m_sData -= sCopy;
	m_pData += sCopy;
    }
    if (m_sData == 0 && getArea (m_pData, m_sData)) {
	VReference<ExportStubClient> pTransferSinkRole;
	getRole (pTransferSinkRole);

	size_t sTransferLimit = proxyTransferSize ();
	char const *pRemaining = m_pData;
	size_t sRemaining = m_sData;
	while (sRemaining > 0) {
	    size_t sTransfer = sRemaining;
	    if (sTransfer > sTransferLimit)
		sTransfer = sTransferLimit;
	    V::VBlob iBlob (pRemaining, sTransfer, false);
	    m_pRemote->Put (pTransferSinkRole, iBlob);
	    pRemaining += sTransfer;
	    sRemaining -= sTransfer;
	}
    }
}


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::VcaSerializer_<Vca::VBSConsumer*>  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VBSConsumer*>::VcaSerializer_(
    VcaSerializer *pCaller, DataType &rDatum
) : VcaSerializer (pCaller), m_rDatum (rDatum) {
    if (rDatum && isOutgoing ())
	rDatum->getRole (m_pInterface);
    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doInterface));
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaSerializer_<Vca::VBSConsumer*>::~VcaSerializer_() {
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VBSConsumer*>::doInterface (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, new VcaSerializerForInterface_<IBSConsumer>(this, m_pInterface));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

void Vca::VcaSerializer_<Vca::VBSConsumer*>::wrapupIncomingSerialization () {
    if (m_rDatum)
	m_rDatum->release ();
    m_rDatum = new VBSConsumerProxy (m_pInterface);
    m_rDatum->retain ();
}

