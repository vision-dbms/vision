/*****  Vca::VBSProducer Implementation  *****/

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

#include "Vca_VBSProducer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaSerializer_.h"


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::VBSProducer::ProducerExportStub  *****
 *****                                        *****
 **************************************************
 **************************************************/

namespace Vca {
    class VBSProducer::ProducerExportStub : public VBS::ExportStub {
	DECLARE_CONCRETE_RTTLITE (ProducerExportStub, VBS::ExportStub);

    //  Client
    public:
	typedef IVReceiver<Data&> Client;

    //  Request
    public:
	class Request : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Request, VTransient);

	//  Construction
	public:
	    Request (
		Client *pClient, size_t sMinimum, size_t sMaximum
	    ) : m_pClient (pClient), m_iBlob (sMaximum), m_sMinimum (
		sMinimum < sMaximum ? sMinimum : sMaximum
	    ) {
	    }
	    Request (
		Request const &rOther
	    ) : m_pClient (rOther.m_pClient), m_iBlob (rOther.m_iBlob), m_sMinimum (
		rOther.m_sMinimum
	    ) {
	    }
	    Request () {
	    }

	//  Destruction
	public:
	    ~Request () {
	    }

	//  Access
	public:
	    size_t minimum () const {
		return m_sMinimum;
	    }
	    Void &voyd () {
		return m_iBlob;
	    }

	//  Query
	public:
	    bool isSatisfied () const {
		return m_iBlob.preContentSize () >= m_sMinimum;
	    }
	    bool isntSatisfied () const {
		return m_iBlob.preContentSize () < m_sMinimum;
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
		m_sMinimum = rOther.m_sMinimum;
	    }
	    Request &operator= (Request const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  State
	private:
	    VReference<Client>	m_pClient;
	    Blob		m_iBlob;
	    size_t		m_sMinimum;
	};

    //  Construction
    public:
	ProducerExportStub (BS *pBS);

    //  Destruction
    private:
	~ProducerExportStub () {
	}

    //  IBSProducer Role
    private:
	VRole<ThisClass,IBSProducer> m_pIBSProducer;
    public:
	void getRole (VReference<IBSProducer>&rpRole) {
	    m_pIBSProducer.getRole (rpRole);
	}

    //  IBSProducer Methods
    public/*private*/:
	void Get (IBSProducer *pRole, Client *pClient, U32 sMinimum, U32 sMaximum);

    //  Access/Query
    protected:
	bool active () const {
	    return m_iQueue.active ();
	}

	Request const &activeRequest () const {
	    return m_iQueue.activeRequest ();
	}
	Request &activeRequest () {
	    return m_iQueue.activeRequest ();
	}

	size_t activeMinimum () {
	    return activeRequest ().minimum ();
	}
	Void &activeVoid () {
	    return activeRequest ().voyd ();
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

Vca::VBSProducer::ProducerExportStub::ProducerExportStub (BS *pBS) : BaseClass (pBS), m_pIBSProducer (this) {
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************
 *****  Vca::IBSProducer  *****
 ******************************/

void Vca::VBSProducer::ProducerExportStub::Get (
    IBSProducer *pRole, Client *pClient, U32 sMinimumTransfer, U32 sMaximumTransfer
) {
    if (pClient) {
	Request iRequest (pClient, sMinimumTransfer, sMaximumTransfer);
	if (m_iQueue.enqueue (iRequest))
	    get (activeVoid (), activeMinimum ());
    }
}


/***************************
 ***************************
 *****  Communication  *****
 ***************************
 ***************************/

void Vca::VBSProducer::ProducerExportStub::onEnd () {
    activeRequest ().onEnd ();
    nextRequest ();
}

void Vca::VBSProducer::ProducerExportStub::onError (IError *pError, VString const &rMessage) {
    activeRequest ().onError (pError, rMessage);
    nextRequest ();
}

void Vca::VBSProducer::ProducerExportStub::onTransfer (size_t sTransfer) {
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

bool Vca::VBSProducer::ProducerExportStub::nextRequest () {
    return m_iQueue.dequeue ();
}


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  Vca::VBSProducer::ProducerExportStub::Request  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

/**********************************
 **********************************
 *****  Client Communication  *****
 **********************************
 **********************************/

void Vca::VBSProducer::ProducerExportStub::Request::onEnd () {
    if (m_pClient)
	m_pClient->OnEnd ();
}

void Vca::VBSProducer::ProducerExportStub::Request::onError (IError *pError, VString const &rMessage) {
    if (m_pClient)
	m_pClient->OnError (pError, rMessage);
}

//  Returns the size of the transfer absorbed by this request...
size_t Vca::VBSProducer::ProducerExportStub::Request::onTransfer (size_t sTransfer) {
    size_t sAdjustment = m_iBlob.incrementContentUpperBoundBy (sTransfer);
    m_pClient->OnData (m_iBlob);
    m_iBlob.incrementContentLowerBoundBy (sAdjustment);
    return sAdjustment;
}


/******************************
 ******************************
 *****                    *****
 *****  Vca::VBSProducer  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSProducer::VBSProducer (VCohort *pCohort) : BaseClass (pCohort) {
}

/********************
 ********************
 *****  Export  *****
 ********************
 ********************/

void Vca::VBSProducer::getExportStub (VReference<ProducerExportStub>&rpStub) {
    rpStub.setTo (new ProducerExportStub (this));
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*************************
 *****  IBSProducer  *****
 *************************/

void Vca::VBSProducer::getRole (VReference<IBSProducer>&rpRole) {
    VReference<ProducerExportStub> pExportStub;
    getExportStub (pExportStub);
    pExportStub->getRole (rpRole);
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

//  returns the amount of data size that needs to be produced....
size_t Vca::VBSProducer::transferPending () const {
    return activeRequestProducerAreaSize ();
}

/***********************
 ***********************
 *****  Scheduling  ****
 ***********************
 ***********************/

void Vca::VBSProducer::run_() {
    suspend ();
    onGetContinuation ();
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VBSProducerProxy  *****
 *****                         *****
 ***********************************
 ***********************************/

namespace Vca {
    class VBSProducerProxy : public VBSProducer {
	DECLARE_CONCRETE_RTTLITE (VBSProducerProxy, VBSProducer);

    //  ExportStubClient
    public:
	typedef ProducerExportStub::Client ExportStubClient;

    //  Construction
    public:
	VBSProducerProxy (
	    IBSProducer *pRemote
	) : BaseClass (VCohort::Here ()), m_pVoid (0), m_sVoid (0), m_pRemote (pRemote), m_pResultSink (this) {
	}

    //  Destruction
    private:
	~VBSProducerProxy () {
	}

    //  ProducerExportStub::Client Role
    private:
	VRole<ThisClass,ExportStubClient> m_pResultSink;
    public:
	using BaseClass::getRole;
	void getRole (VReference<ExportStubClient>&rpRole) {
	    m_pResultSink.getRole (rpRole);
	}

    //  ProducerExportStub::Client Methods
    public/*private*/:
	/*-------------------------------------------------------------*
	 *  The content area of the blob of data received will never
	 *  exceed the total data requested from the remote stream.
	 *-------------------------------------------------------------*/
	void OnData (ExportStubClient *pRole, Data &rData) {
	    if (m_iData.contentSize () > 0) {
		fprintf (
		    stderr, "%s %p:OnData: Orphaned %lu data bytes\n", rttName ().content (), this,
		    m_iData.contentSize ()
		);
		fflush (stderr);
	    }
	    m_iData.setTo (rData);
	    onGetContinuation ();
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
	void transferData () OVERRIDE {
	    if (m_sVoid > 0 && m_iData.contentSize () > 0) {
		size_t sCopy = m_iData.contentSize ();
		if (sCopy > m_sVoid)
		    sCopy = m_sVoid;
		memcpy (m_pVoid, m_iData.contentArea (), sCopy);
		onGetCompletion (sCopy);
		m_iData.incrementContentLowerBoundBy (sCopy);
		m_sVoid -= sCopy;
		m_pVoid += sCopy;
	    };
	    if (m_iData.contentSize () == 0 && getArea (m_pVoid, m_sVoid)) {
		VReference<ExportStubClient> pClient;
		getRole (pClient);
		m_pRemote->Get (pClient, 1, m_sVoid);
	    }
	}

    //  State
    private:
	VReference<IBSProducer> const	m_pRemote;
	Blob				m_iData;	//  ... data from remote
	char*				m_pVoid;
	size_t				m_sVoid;
    };
}


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::VcaSerializer_<Vca::VBSProducer*>  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaSerializer_<Vca::VBSProducer*>::VcaSerializer_(
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

Vca::VcaSerializer_<Vca::VBSProducer*>::~VcaSerializer_() {
}


/*******************************
 *******************************
 *****  Sequencer Actions  *****
 *******************************
 *******************************/

void Vca::VcaSerializer_<Vca::VBSProducer*>::doInterface (Sequencer *pSequencer) {
    clearSequencer ();
    start (this, new VcaSerializerForInterface_<IBSProducer>(this, m_pInterface));
}

/*********************************
 *********************************
 *****  Transport Callbacks  *****
 *********************************
 *********************************/

void Vca::VcaSerializer_<Vca::VBSProducer*>::wrapupIncomingSerialization () {
    if (m_rDatum)
	m_rDatum->release ();
    m_rDatum = new VBSProducerProxy (m_pInterface);
    m_rDatum->retain ();
}
