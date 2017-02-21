/*****  Vca_VPassiveDirectiveAgent Implementation *****/

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

#include "Vca_VPassiveDirectiveAgent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPipeSourceClient.h"

#include "Vca_VBSProducer.h"
#include "Vca_VBSConsumer.h"

#include "Vca_VLineGrabber.h"

#include "Vca_VOutputSequencer.h"


/*********************************************************
 *********************************************************
 *****                                               *****
 *****  class Vca::VPassiveDirectiveAgent::PipeSink  *****
 *****                                               *****
 *********************************************************
 *********************************************************/

namespace Vca {
    class VPassiveDirectiveAgent::PipeSink : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (PipeSink, VRolePlayer);

    //  Construction
    public:
	PipeSink (
	    VPassiveDirectiveAgent *pAgent, CallData const &rCallData, PipeSourceGofer *pPipeSourceGofer
	) : m_pIPipeSourceClient (this), m_pAgent (pAgent), m_iCallData (rCallData) {
	    retain (); {
		pPipeSourceGofer->supplyMembers (this, &ThisClass::onPipeSource, &ThisClass::onError);
	    } untain ();
	}

    //  Destruction
    private:
	~PipeSink () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IPipeSourceClient Role
    private:
	VRole<ThisClass,IPipeSourceClient> m_pIPipeSourceClient;
	void getRole (IPipeSourceClient::Reference &rpRole) {
	    m_pIPipeSourceClient.getRole (rpRole);
	}

    //  IPipeSourceClient Methods
    public:
	void OnData (
	    IPipeSourceClient *pRole, VBSConsumer *pStdBSToPeer, VBSProducer *pStdBSToHere, VBSProducer *pErrBSToHere
	);

    //  Callbacks
    private:
	void OnError_(IError *pInterface, VString const &rMessage) {
	    onError (pInterface, rMessage);
	}
	void onError (IError *pInterface, VString const &rMessage);
	void onPipeSource (IPipeSource *pPipeSource);

    //  State
    private:
	VPassiveDirectiveAgent::Reference m_pAgent;
	CallData const m_iCallData;
    };
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VPassiveDirectiveAgent::PipeSink::OnData (
    IPipeSourceClient *pRole, VBSConsumer *pStdBSToPeer, VBSProducer *pStdBSToHere, VBSProducer *pErrBSToHere
) {
    m_pAgent->onPlumbing (m_iCallData, pStdBSToPeer, pStdBSToHere, pErrBSToHere);
}

void Vca::VPassiveDirectiveAgent::PipeSink::onPipeSource (IPipeSource *pPipeSource) {
    if (pPipeSource) {
	IPipeSourceClient::Reference pPipeSourceClient;
	getRole (pPipeSourceClient);
	pPipeSource->Supply (pPipeSourceClient);
    }
}

void Vca::VPassiveDirectiveAgent::PipeSink::onError (IError *pInterface, VString const &rMessage) {
    m_iCallData.OnError (pInterface, rMessage);
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  class Vca::VPassiveDirectiveAgent  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

namespace Vca {
    template class VLineGrabber_<VPassiveDirectiveAgent>;

    bool const VPassiveDirectiveAgent::g_bTracingLineGrabber = getenv ("VcaTracingPDALG") ? true : false;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPassiveDirectiveAgent::VPassiveDirectiveAgent (VString const &rDirectivePrefix) : m_iDirectivePrefix (rDirectivePrefix) {
}

/*************************
 ************************* 
 *****  Destruction  *****
 ************************* 
 *************************/

Vca::VPassiveDirectiveAgent::~VPassiveDirectiveAgent () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VPassiveDirectiveAgent::processCallbackRequest (CallData const &rCallData) {
    if (m_pPipeSourceGofer.isNil ())
	rCallData.OnEnd ();
    else {
	m_pPipeSourceGofer->resetGofer ();
	PipeSink::Reference pPipeSink (new PipeSink (this, rCallData, m_pPipeSourceGofer));
    }
}

bool Vca::VPassiveDirectiveAgent::supplyPassiveServerGofer (
    PassiveServerGofer::Reference &rpGofer, PipeSourceGofer *pPipeSourceGofer
) {
    return m_pPipeSourceGofer.setIfNil (pPipeSourceGofer)
	&& BaseClass::supplyPassiveServerGofer (rpGofer);
}

void Vca::VPassiveDirectiveAgent::startLineGrabber (VBSProducer *pBSProducer) {
    if (pBSProducer)
	(new VLineGrabber_<ThisClass> (pBSProducer, this))->discard ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VPassiveDirectiveAgent::onPlumbing (
    CallData const &rCallData, VBSConsumer *pStdBSToPeer, VBSProducer *pStdBSToHere, VBSProducer *pErrBSToHere
) {
    if (pStdBSToHere)
	startLineGrabber (pStdBSToHere);
    if (pErrBSToHere)
	startLineGrabber (pErrBSToHere);
    if (pStdBSToPeer) {
	VOutputSequencer::Reference pOutputSequencer (new VOutputSequencer (pStdBSToPeer));
	pOutputSequencer->outputTerminator ();
    }
}

bool Vca::VPassiveDirectiveAgent::onInputLine (char const *pLine, size_t sLine) {
    if (g_bTracingLineGrabber)
	fprintf (stderr, "+++ Got Input: <%*.*s>\n", sLine, sLine, pLine);
    return true;
}
void Vca::VPassiveDirectiveAgent::onInputDone () {
    if (g_bTracingLineGrabber)
	fprintf (stderr, "+++ Got End Of Input\n");
}
