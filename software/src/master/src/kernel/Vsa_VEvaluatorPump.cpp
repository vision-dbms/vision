/*****  Vsa_VEvaluatorPump Implementation  *****/

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

#include "Vsa_VEvaluatorPump.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32) // MSVC Compiler Happy Pill
#include "Vca_IPipeSource.h"
#endif

#include "Vsa_IEvaluationResult.h"
#include "Vsa_IEvaluatorClient.h"

#include "Vsa_VEvaluatorPumpSource.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::VEvaluatorPumpResult  *****
 *****                             *****
 ***************************************
 ***************************************/

namespace Vsa {
    class VEvaluatorPumpResult : public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VEvaluatorPumpResult, Vca::VRolePlayer);

    //  Construction
    public:
	VEvaluatorPumpResult (
	    VString const &rTimingReport
	) : m_iTimingReport (rTimingReport), m_pIEvaluationResult (this) {
	}

    //  Destruction
    private:
	~VEvaluatorPumpResult () {
	}

    //  IEvaluationResult Role
    private:
	Vca::VRole<ThisClass,IEvaluationResult> m_pIEvaluationResult;
    public:
	void getRole (IEvaluationResult::Reference &rpRole) {
	    m_pIEvaluationResult.getRole (rpRole);
	}

    //  Interface Implementations
    public:
	void GetTimingReport (IEvaluationResult *pRole, IVReceiver<VString const&> *pReportSink) {
	    if (pReportSink)
		pReportSink->OnData (m_iTimingReport);
	}

    //  State
    private:
	VString const m_iTimingReport;
    };
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vsa::VEvaluatorPump::IQueue::Entry  *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPump::IQueue::Entry::Entry (
    VEvaluation *pRequest, unsigned int cPrompts
) : m_pRequest (pRequest), m_cPrompts (cPrompts) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorPump::IQueue::Entry::~Entry () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vsa::VEvaluatorPump::IQueue::Entry *Vsa::VEvaluatorPump::IQueue::Entry::get (
    VEvaluation::Reference &rpRequest, unsigned int &rcPrompts
) const {
    rpRequest.setTo (m_pRequest);
    rcPrompts = m_cPrompts;
    return m_pSuccessor;
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vsa::VEvaluatorPump::IQueue  *****
 *****                               *****
 *****************************************
 *****************************************/

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vsa::VEvaluatorPump::IQueue::enqueue (VEvaluation *pRequest, unsigned int cPrompts) {
    Entry *pEntry = new Entry (pRequest, cPrompts);
    if (m_pTail) 
	m_pTail->m_pSuccessor.setTo (pEntry);
    else
	m_pHead.setTo (pEntry);
    m_pTail.setTo (pEntry);
    m_sQueue++;
}

void Vsa::VEvaluatorPump::IQueue::dequeue (
    VEvaluation::Reference &rpRequest, unsigned int &rcPrompts
) {
    if (m_pHead.isNil ()) {
	rpRequest.clear ();
	rcPrompts = 0;
    }
    else {
	m_pHead.setTo (m_pHead->get (rpRequest, rcPrompts));
	if (m_pHead.isNil ())
	    m_pTail.clear ();
        m_sQueue--;
    }
}

/*******************
 *******************
 *****  Error  *****
 *******************
 *******************/

void Vsa::VEvaluatorPump::IQueue::onError (Vca::IError *pError, VString const &rMessage) {
    VEvaluation::Reference pRequest;
    unsigned int iPrompts;
    
    dequeue (pRequest, iPrompts);
    while (pRequest.isntNil ()) {
	pRequest->onError (pError, rMessage);
	pRequest.clear ();
	dequeue (pRequest, iPrompts);
    }
}


/******************************************************************
 ******************************************************************
 *****                                                        *****
 *****  Vsa::VEvaluatorPump::PumpSettings::PromptSet::Prompt  *****
 *****                                                        *****
 ******************************************************************
 ******************************************************************/

/**********************
 **********************
 *****  Location  *****
 **********************
 **********************/

bool Vsa::VEvaluatorPump::PromptSet::Prompt::locate (	// true if found, else false
    char const *pText, size_t &rxOrigin, size_t &rsSpan
) const {
    char const *pLocation = strstr (pText, m_pText);
    if (pLocation) {
	rxOrigin = pLocation - pText;
	rsSpan = m_sText;
	return true;
    }
    return false;
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vsa::VEvaluatorPump::PumpSettings::PromptSet  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPump::PumpSettings::PromptSet::Prompt *Vsa::VEvaluatorPump::PromptSet::newPromptArray (
    char const *pPrompts[]
) {
    Prompt *pResult = static_cast<Prompt*>(allocate (sizeof (Prompt) * m_cPrompts));
    for (unsigned int xPrompt = 0; xPrompt < m_cPrompts; xPrompt++)
	new (pResult + xPrompt) Prompt (pPrompts[xPrompt]);
    return pResult;
}

Vsa::VEvaluatorPump::PumpSettings::PromptSet::PromptSet (unsigned int cPrompts, char const *pPrompts[])
: m_cPrompts (cPrompts)
, m_pPrompts (newPromptArray (pPrompts))
, m_sMaximumSpan (maximumSpanCalc ())
{
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

size_t Vsa::VEvaluatorPump::PumpSettings::PromptSet::maximumSpanCalc () const {
    unsigned int sResult = 0;
    for (unsigned int xPrompt = 0; xPrompt < m_cPrompts; xPrompt++)
	if (sResult < m_pPrompts[xPrompt].length ())
	    sResult = m_pPrompts[xPrompt].length ();
    return sResult;
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vsa::VEvaluatorPump::PumpSettings::PromptSet::locate (
    char const *pText, size_t &rxOrigin, size_t &rsSpan
) const {
    bool bFound = false;
    rxOrigin = strlen (pText);
    for (unsigned int xPrompt = 0; rxOrigin > 0 && xPrompt < m_cPrompts; xPrompt++) {
        size_t xOrigin, sSpan;
	if (m_pPrompts[xPrompt].locate (pText, xOrigin, sSpan) && xOrigin < rxOrigin) {
	    bFound = true;
	    rxOrigin = xOrigin;
	    rsSpan = sSpan;
	}
    }
    return bFound;
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vsa::VEvaluatorPump::PumpSettings  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

#define PARRAY(a) sizeof (a) / sizeof (a[0]), a

Vsa::VEvaluatorPump::PromptSet const *Vsa::VEvaluatorPump::PumpSettings::standardPromptSet () {
    static char const *g_pPrompts[] = {"V> "	, "D> "};
    static PromptSet const *g_pPromptSet = 0;
    if (!g_pPromptSet)
	g_pPromptSet = new PromptSet (PARRAY (g_pPrompts));
    return g_pPromptSet;
}

Vsa::VEvaluatorPump::PromptSet const *Vsa::VEvaluatorPump::PumpSettings::extendedPromptSet () {
    static char const *g_pPrompts[] = {"V> \005\n"	, "D> \005\n"};
    static PromptSet const *g_pPromptSet = 0;
    if (!g_pPromptSet)
	g_pPromptSet = new PromptSet (PARRAY (g_pPrompts));
    return g_pPromptSet;
}

#undef PARRAY

Vsa::VPathQuery::Formatter const Vsa::VEvaluatorPump::PumpSettings::g_iQueryFormatter (
    "Interface HtmlAccess", false,
    "$$class get: $$path usingQuery: $$query for: $REMOTE_HOST at: $REMOTE_ADDR", false
);


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*****************************************
 *****  Vsa::IEvaluatorPumpSettings  *****
 *****************************************/

void Vsa::VEvaluatorPump::PumpSettings::MakeControl (
    IEvaluatorPumpSettings*	pRole,
    IVReceiver<IEvaluatorPump*>*pClient,
    Vca::VBSConsumer*		pPipeToPeer,
    Vca::VBSProducer*		pPipeToHere
) {
    if (pClient) {
	VEvaluatorPump::Reference pControl (
	    new VEvaluatorPump (this, pPipeToPeer, pPipeToHere)
	);
	IEvaluatorPump::Reference pIEvaluatorPump;
	pControl->getRole (pIEvaluatorPump);
	pClient->OnData (pIEvaluatorPump);
    }
}

void Vsa::VEvaluatorPump::PumpSettings::MakeSource (
    IEvaluatorPumpSettings*		pRole,
    IVReceiver<IEvaluatorSource*>*	pClient,
    Vca::IPipeSource*			pPipeSource
) {
    if (pClient) {
	VEvaluatorPumpSource::Reference pEvaluatorSource (
	    new VEvaluatorPumpSource (this, pPipeSource)
	);
	Vsa::IEvaluatorSource::Reference pIEvaluatorSource;
	pEvaluatorSource->getRole (pIEvaluatorSource);
	pClient->OnData (pIEvaluatorSource);
    }
}

void Vsa::VEvaluatorPump::PumpSettings::GetURLClass (
    IEvaluatorPumpSettings *pRole, IVReceiver<VString const&> *pClient
) {
    if (pClient)
	pClient->OnData (urlClass ());
}

void Vsa::VEvaluatorPump::PumpSettings::SetURLClass (
    IEvaluatorPumpSettings *pRole, VString const &rValue
) {
    setURLClassTo (rValue);
}

void Vsa::VEvaluatorPump::PumpSettings::GetURLTemplate (
    IEvaluatorPumpSettings *pRole, IVReceiver<VString const&> *pClient
) {
    if (pClient)
	pClient->OnData (urlTemplate ());
}

void Vsa::VEvaluatorPump::PumpSettings::SetURLTemplate (
    IEvaluatorPumpSettings *pRole, VString const &rValue
) {
    setURLTemplateTo (rValue);
}

void Vsa::VEvaluatorPump::PumpSettings::GetUsingExtendedPrompts (
    IEvaluatorPumpSettings *pRole, IVReceiver<bool> *pClient
) {
    if (pClient)
	pClient->OnData (usingExtendedPrompts ());
}

void Vsa::VEvaluatorPump::PumpSettings::SetUsingExtendedPrompts (
    IEvaluatorPumpSettings *pRole, bool bValue
) {
    setUsingExtendedPromptsTo (bValue);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPump::PumpSettings::PumpSettings (PumpSettings const &rOther)
: m_iQueryFormatter		(rOther.m_iQueryFormatter)
, m_bUsingExtendedPrompts	(rOther.m_bUsingExtendedPrompts)
, m_pIEvaluatorPumpSettings	(this)
{
}

Vsa::VEvaluatorPump::PumpSettings::PumpSettings ()
: m_iQueryFormatter		(g_iQueryFormatter)
, m_bUsingExtendedPrompts	(true)
, m_pIEvaluatorPumpSettings	(this)
{
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vsa::VEvaluatorPump::PumpSettings::getSetupExpression (VString &rResult) const {
    static VString const g_iExtendedPromptSetupString ("?oE+\n", false);
    if (m_bUsingExtendedPrompts)
	rResult.setTo (g_iExtendedPromptSetupString);
    else
	rResult.clear ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vsa::VEvaluatorPump::PumpSettings::getVisionExpression (
    VEvaluation *pEvaluation, VString &rResult
) const {
    pEvaluation->getVisionExpression (m_iQueryFormatter, rResult);
}


/*********************************
 *********************************
 *****                       *****
 *****  Vsa::VEvaluatorPump  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPump::VEvaluatorPump (
    PumpSettings *pSettings, Vca::VBSConsumer *pPipeToPeer, Vca::VBSProducer *pPipeToHere
) : m_pSettings			(pSettings)
,   m_pPromptSet		(pSettings->promptSet ())
,   m_pBSConsumer		(pPipeToPeer)
,   m_pBSProducer		(pPipeToHere)
,   m_cIncomingPrompts		(1)
,   m_bOutgoingBody		(false)
,   m_bOutgoingBodyNeedsNewLine	(false)
,   m_bCrankingIncomingPump	(false)
,   m_bCrankingOutgoingPump	(false)
,   m_pIEvaluatorPump		(this)
,   m_pBSConsumerClient		(this)
,   m_pBSProducerClient		(this)
{
    traceInfo ("Creating VEvaluatorPump");
    retain (); {
//  Prime the pump, ...
	pSettings->getSetupExpression (m_iOutgoingString);
	resetIncomingData ();

	resetIncomingFifo ();
	resetOutgoingFifo ();

//  ... and start the flow:
	primeIncomingPump ();
	primeOutgoingPump ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorPump::~VEvaluatorPump () {
    if (m_pBSConsumer)
	m_pBSConsumer->Close ();
    if (m_pBSProducer)
	m_pBSProducer->Close ();
    traceInfo ("Destroying VEvaluatorPump");
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*********************************
 *****  Vsa::IEvaluatorPump  *****
 *********************************/

void Vsa::VEvaluatorPump::GetSettings (
    IEvaluatorPump *pRole, IVReceiver<IEvaluatorPumpSettings*> *pClient
) {
    if (pClient) {
	IEvaluatorPumpSettings::Reference pIEvaluatorPumpSettings;
	m_pSettings->getRole (pIEvaluatorPumpSettings);
	pClient->OnData (pIEvaluatorPumpSettings);
    }
}


/***********************************
 ***********************************
 *****  Byte Stream Callbacks  *****
 ***********************************
 ***********************************/

/**********************
 *****  Transfer  *****
 **********************/

#if defined(_DEBUG)
#define DisplayAndConsume
#define DisplayAndProduce
#endif

#if defined(DisplayAndConsume)
static void displayAndConsume (
    Vsa::VEvaluatorPump *pPump, char const *pWhere, V::VFifoLite &rFifo, size_t sTransfer
) {
    if (pPump->objectsTrace ()) {
	VString iMessage (rFifo.consumerAreaSize () + 256);
	iMessage << pPump->rttName () << "::" << pWhere << ": <";

	iMessage.append (rFifo.consumerArea (), sTransfer);
	iMessage << "@";
	iMessage.append (
	    rFifo.consumerArea () + sTransfer, rFifo.consumerAreaSize () - sTransfer
	);

	iMessage << ">";
	pPump->traceInfo (iMessage);
    }
    rFifo.consume (sTransfer);
}
#endif

void Vsa::VEvaluatorPump::OnTransfer (BSConsumerClient *pRole, size_t sTransfer) {
#   if defined(DisplayAndConsume)
    displayAndConsume (this, "OnTransfer", m_iOutgoingFifo, sTransfer);
#   else
    m_iOutgoingFifo.consume (sTransfer);
#   endif
    crankOutgoingPump (pRole);
}

void Vsa::VEvaluatorPump::OnTransfer (BSProducerClient *pRole, size_t sTransfer) {
    crankIncomingPump (pRole, sTransfer);
}


/*******************
 *****  Error  *****
 *******************/

void Vsa::VEvaluatorPump::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
) {
    traceInfo ("Vsa::VEvaluatorPump::OnError");
    if (dynamic_cast<Vca::IBSProducerClient*>(pRole)) {
	if (m_pIncomingClient.isntNil ())
    	    m_pIncomingClient->onError (pError, rMessage);

	if (m_pBSProducer) {
	    m_pBSProducer->Close ();
	    m_pBSProducer.clear ();
	}
	m_pIncomingClient.clear ();
	m_iIncomingQueue.onError (pError, rMessage);

    }
    else if (dynamic_cast<Vca::IBSConsumerClient*>(pRole)) {
	onError (pError, rMessage);
	if (m_pBSConsumer) {
	    m_pBSConsumer->Close ();
	    m_pBSConsumer.clear ();
	}
    }
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPump::onQueue_() {
    if (m_pBSProducer.isNil ()) {
	m_iIncomingQueue.onError (0, "Nil Producer");
	onError (0, "Nil Producer");
	return;
    }

    if (m_pBSConsumer.isNil ()) 
	onError (0, "Nil Consumer");
    else {
	BSConsumerClient::Reference pClientRole;
	getRole (pClientRole);
	crankOutgoingPump (pClientRole);
    }
}


/***************************
 *****  Incoming Pump  *****
 ***************************/

void Vsa::VEvaluatorPump::primeIncomingPump () {
    traceInfo ("Vsa::VEvaluatorPump::primeIncomingPump");
    BSProducerClient::Reference pClientRole;
    getRole (pClientRole);
    crankIncomingPump (pClientRole, m_pBSProducer->get (pClientRole, m_iIncomingFifo, 1));
}

void Vsa::VEvaluatorPump::crankIncomingPump (IBSClient *pClientRole, size_t sTransfer) {
    if (m_bCrankingIncomingPump)
	return;
    m_bCrankingIncomingPump = true;

    while (sTransfer > 0) {
#	if defined(DisplayAndProduce)
	if (objectsTrace ()) {
	    VString iMessage (m_iIncomingFifo.consumerAreaSize () + sTransfer + 256);
	    iMessage << rttName () << "::get :<";

	    iMessage.append (
		m_iIncomingFifo.consumerArea (), m_iIncomingFifo.consumerAreaSize ()
	    );
	    iMessage << "@";
	    iMessage.append (m_iIncomingFifo.producerArea (), sTransfer);

	    iMessage << ">";
	    traceInfo (iMessage);
	}
#	endif
	m_iIncomingFifo.produce (sTransfer);
	m_iIncomingFifo.producerArea ()[0] = '\0';

	do {
	    size_t sFragment, sPrompt;
	    bool bPromptFound = m_pPromptSet->locate (
		m_iIncomingFifo.consumerArea (), sFragment, sPrompt
	    );

	    if (sFragment > incomingFreeAreaSize ()) {
		m_pIncomingString = m_iIncomingString.guarantee (
		    m_iIncomingAllocator.nextSize (m_sIncomingString + sFragment)
		);
	    }
	    if (sFragment > 0) {
		memcpy (incomingFreeArea (), m_iIncomingFifo.consumerArea (), sFragment);
		m_iIncomingFifo.consume (sFragment);

		m_sIncomingString += sFragment;
		incomingFreeArea ()[0] = '\0';
	    } else if (!bPromptFound && m_iIncomingFifo.consumerAreaSize () > 0) {
		// We have a NULL character embedded in the consumerArea
		// Consume it to prevent an infinite loop
		m_iIncomingFifo.consume (1);
	    }
	    if (bPromptFound) {
		m_iIncomingFifo.consume (sPrompt);
		switch (--m_cIncomingPrompts) {
		case 1:
		    m_iIncomingString.trim ();
		    m_iIncomingOutput.setTo (m_iIncomingString);
		    resetIncomingData ();
		    break;
		case 0:
		    //  ... do request completion processing:
		    if (m_pIncomingClient) {
			m_iIncomingString.trim ();
			VEvaluatorPumpResult::Reference pResult (
			    new VEvaluatorPumpResult (m_iIncomingString)
			);
			IEvaluationResult::Reference pRole;
			pResult->getRole (pRole);
			m_pIncomingClient->onResult (pRole, m_iIncomingOutput);
		    }
		    m_iIncomingQueue.dequeue (m_pIncomingClient, m_cIncomingPrompts);
                    if (m_pIncomingClient) {
                        // TODO: Make sure that this cancels the correct query (not the one right after, etc.), fix it in the cases that it doesn't.
                        // TODO: Could be implemented using prompt counting on both ends of the VEvaluatorPump, then reporting from IOMDriver to VEvaluatorPump the current prompt index being processed.
                        if (m_pIncomingClient->isCancelled ()) {
                            cancelCurrent ();
                        }
                    }
		    resetIncomingData ();
		}
	    }
	} while (m_iIncomingFifo.consumerAreaSize () > 0 && m_cIncomingPrompts > 0);

	if (m_pBSProducer.isNil () || 0 == m_cIncomingPrompts)
	    sTransfer = 0;
	else {
	//  Reset the incoming fifo, ...
	    resetIncomingFifo ();

    //  ... copy enough data back to the fifo so that split prompts can be found, ...
	    size_t sCopyBack = m_pPromptSet->maximumSpan () - 1;
	    if (sCopyBack > m_sIncomingString)
		sCopyBack = m_sIncomingString;

	    m_sIncomingString -= sCopyBack;
	    memcpy (m_iIncomingFifo.producerArea (), incomingFreeArea (), sCopyBack);
	    m_iIncomingFifo.produce (sCopyBack);
	    incomingFreeArea ()[0] = '\0';

    //  ... and start the next read:
	    sTransfer = m_pBSProducer->get (pClientRole, m_iIncomingFifo, 1);
	}
    }
    if (m_iIncomingFifo.producerAreaSize () == 0) {
	log ("CrankIncomingPump: producerArea is full: resetting ...");
	resetIncomingFifo ();
    }
 
    m_bCrankingIncomingPump = false;
}


/***************************
 *****  Outgoing Pump  *****
 ***************************/

void Vsa::VEvaluatorPump::primeOutgoingPump () {
    traceInfo ("Vsa::VEvaluatorPump::primeOutgoingPump");
    BSConsumerClient::Reference pClientRole;
    getRole (pClientRole);
    OnTransfer (pClientRole, m_pBSConsumer->put (pClientRole, m_iOutgoingFifo));
}

void Vsa::VEvaluatorPump::crankOutgoingPump (IBSClient *pClientRole) {
    if (m_bCrankingOutgoingPump)
	return;
    m_bCrankingOutgoingPump = true;

    VEvaluation *pRequest = thisRequest ();

    while (pRequest && outgoingPumpIsIdle ()) {
	if (m_bOutgoingBody) {
//  ... schedule query execution directive delivery:
	    m_iOutgoingString.setToStatic (
		m_bOutgoingBodyNeedsNewLine ? "\n?g\n?t\n" : "?g\n?t\n"
	    );
	    resetOutgoingFifo ();

	    m_bOutgoingBody = m_bOutgoingBodyNeedsNewLine = false;

	    pRequest = nextRequest ();
	}
	else if (pRequest) {
//  ... schedule query body delivery:
	    m_pSettings->getVisionExpression (pRequest, m_iOutgoingString);
	    resetOutgoingFifo ();

	    m_bOutgoingBodyNeedsNewLine = m_iOutgoingFifo.consumerAreaSize () > 0
		&& m_iOutgoingString.content()[m_iOutgoingFifo.consumerAreaSize() - 1] != '\n';
	    m_bOutgoingBody = true;

//    ... schedule reply receipt:
	    unsigned int cPrompts = m_iOutgoingString.lineCount () + (
		m_bOutgoingBodyNeedsNewLine ? 3 : 2
	    );
	    if (incomingPumpIsBusy ())
		m_iIncomingQueue.enqueue (pRequest, cPrompts);
	    else {
		m_pIncomingClient.setTo (pRequest);
		m_cIncomingPrompts = cPrompts;
		primeIncomingPump ();
	    }
	}
#if defined(DisplayAndConsume)
	displayAndConsume (
	    this, "crankOutgoingPump", m_iOutgoingFifo, m_pBSConsumer->put (
		pClientRole, m_iOutgoingFifo
	    )
	);
#else
	m_iOutgoingFifo.consume (m_pBSConsumer->put (pClientRole, m_iOutgoingFifo));
#endif
    }

    m_bCrankingOutgoingPump = false;
}


/**************************
 **************************
 *****  Cancellation  *****
 **************************
 **************************/

bool Vsa::VEvaluatorPump::cancel_ (VEvaluation *pEvaluation) {
    if (m_pIncomingClient != pEvaluation) return false;
    cancelCurrent ();
    return true;
}



/*******************************
 *******************************
 *****  Execution Support  *****
 *******************************
 *******************************/

void Vsa::VEvaluatorPump::resetIncomingData () {
    m_iIncomingAllocator.reset ();

    m_iIncomingString.clear ();
    m_pIncomingString = m_iIncomingString.storage ();
    m_sIncomingString = m_iIncomingString.length ();
}

void Vsa::VEvaluatorPump::resetIncomingFifo () {
    //  ... sizeof (...) - 1 leaves room to add a terminating '\0':
    m_iIncomingFifo.setToVoid (m_iIncomingBuffer, sizeof (m_iIncomingBuffer) - 1);
}

void Vsa::VEvaluatorPump::resetOutgoingFifo () {
    m_iOutgoingFifo.setToData (m_iOutgoingString.content (), m_iOutgoingString.length ());
}

/********************************************
 *****  Vsa::IEvaluatorControl Methods  *****
 ********************************************/

void Vsa::VEvaluatorPump::GetStatistics (
    IEvaluatorControl *pRole, IVReceiver<VString const &>* pReceiver 
) {
}
