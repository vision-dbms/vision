/*****  Vsa_VEvaluatorPumpSource Implementation  *****/

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

#include "Vsa_VEvaluatorPumpSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_CompilerHappyPill.h"

#include "Vca_IPipeSource.h"
#include "Vca_VcaGofer.h"

#include "Vsa_IEvaluatorClient.h"
#include "Vsa_VEvaluatorPump.h"


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vsa::VEvaluatorPumpSource::Agent  *****
 *****                                    *****
 **********************************************
 **********************************************/

namespace Vsa {
    class VEvaluatorPumpSource::Agent : public Vca::VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (Agent, Vca::VRolePlayer);

    //  Construction
    protected:
	Agent (VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink);

    //  Destruction
    protected:
	~Agent ();

    //  Vca::IClient Methods
    public:
	void OnError (
	    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
	);
	void OnEnd (Vca::IClient *pRole);

    //  State
    protected:
	VEvaluatorPumpSource::Reference	const m_pSource;
	IEvaluatorSink::Reference	const m_pSink;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPumpSource::Agent::Agent (
    VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink
) : m_pSource (pSource), m_pSink (pSink) {
    traceInfo ("Creating Vsa::VEvaluatorPumpSource::Agent");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorPumpSource::Agent::~Agent () {
    traceInfo ("Destroying Vsa::VEvaluatorPumpSource::Agent");
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPumpSource::Agent::OnEnd (Vca::IClient *pRole) {
    m_pSink->OnEnd ();
}

void Vsa::VEvaluatorPumpSource::Agent::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
) {
    m_pSink->OnError (pError, rMessage);
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vsa::VEvaluatorPumpSource::Plumber  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace Vsa {
    class VEvaluatorPumpSource::Plumber : public Agent {
	DECLARE_CONCRETE_RTTLITE (Plumber, Agent);

    //  Construction
    public:
	Plumber (
	    VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink, Vca::IPipeSource *pPipeSource
	);

    //  Destruction
    private:
	~Plumber () {
	}

    //  IObjectSink Role
    private:
	Vca::VRole<ThisClass,Vca::IObjectSink> m_pIObjectSink;
    public:
	void getRole (Vca::IObjectSink::Reference &rpRole) {
	    m_pIObjectSink.getRole (rpRole);
	}

    //  IObjectSink Methods
    public:
	void OnData (Vca::IObjectSink *pRole, IVUnknown *pObject);
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPumpSource::Plumber::Plumber (
    VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink, Vca::IPipeSource *pPipeSource
) : Agent (pSource, pSink), m_pIObjectSink (this) {
    retain (); {
        Vca::IObjectSink::Reference pObjectSink;
	getRole (pObjectSink);
	Vca::VQueryInterface<IEvaluator> iQuery (pObjectSink);
	Vca::Gofer::Supply (iQuery, pPipeSource);
    } untain ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPumpSource::Plumber::OnData (
    Vca::IObjectSink *pRole, IVUnknown *pObject
) {
    m_pSource->onData (m_pSink, dynamic_cast<IEvaluator*>(pObject));
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vsa::VEvaluatorPumpSource::Pumper  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

namespace Vsa {
    class VEvaluatorPumpSource::Pumper : public Agent {
	DECLARE_CONCRETE_RTTLITE (Pumper, Agent);

    //  Aliases
    public:
	typedef Vca::IPipeSourceClient IPipeSink;

    //  Construction
    public:
	Pumper (VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink, Vca::IPipeSource *pPipeSource);

    //  Destruction
    private:
	~Pumper () {
	}

    //  IPipeSink Role
    private:
	Vca::VRole<ThisClass,Vca::IPipeSourceClient> m_pIPipeSink;
    public:
	void getRole (IPipeSink::Reference &rpRole) {
	    m_pIPipeSink.getRole (rpRole);
	}

    //  IPipeSink Methods
    public:
	void OnData (
	    IPipeSink *pRole,
	    Vca::VBSConsumer *pPipeToPeer,
	    Vca::VBSProducer *pPipeToHere,
	    Vca::VBSProducer *pErrorPipeToHere
	);
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPumpSource::Pumper::Pumper (
    VEvaluatorPumpSource *pSource, IEvaluatorSink *pSink, Vca::IPipeSource *pPipeSource
) : Agent (pSource, pSink), m_pIPipeSink (this) {
    retain (); {
	Vca::IPipeSourceClient::Reference pPipeSink;
	getRole (pPipeSink);
	pPipeSource->Supply (pPipeSink);
    } untain ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vsa::VEvaluatorPumpSource::Pumper::OnData (
    IPipeSink *pRole,
    Vca::VBSConsumer *pPipeToPeer,
    Vca::VBSProducer *pPipeToHere,
    Vca::VBSProducer *pErrorPipeToHere
) {
    m_pSource->onData (m_pSink, pPipeToPeer, pPipeToHere, pErrorPipeToHere);
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::VEvaluatorPumpSource  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorPumpSource::VEvaluatorPumpSource (
    EvaluatorSettings *pEvaluatorSettings, Vca::IPipeSource *pPipeSource, bool bPlumbed
) : m_pEvaluatorSettings (pEvaluatorSettings), m_pPipeSource (pPipeSource), m_bPlumbed (bPlumbed) {
    traceInfo ("Creating VEvaluatorPumpSource");
}

Vsa::VEvaluatorPumpSource::VEvaluatorPumpSource (
    EvaluatorSettings *pEvaluatorSettings, Vca::IPipeSource *pPipeSource, 
    VString const &rStartupExpression, bool bPlumbed
) : VEvaluatorSource (rStartupExpression)
,   m_pEvaluatorSettings (pEvaluatorSettings)
,   m_pPipeSource (pPipeSource)
,   m_bPlumbed (bPlumbed) {
    traceInfo ("Creating VEvaluatorPumpSource");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluatorPumpSource::~VEvaluatorPumpSource () {
    traceInfo ("Destroying VEvaluatorPumpSource");
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vsa::VEvaluatorPumpSource::Supply (IEvaluatorSource *pRole, IEvaluatorSink *pSink) {
    if (m_bPlumbed) {
	Plumber::Reference pPlumber (new Plumber (this, pSink, m_pPipeSource));
    } else {
	Pumper::Reference pPumper (new Pumper (this, pSink, m_pPipeSource));
    }
}

/*****************************
 *****************************
 *****  Agent Callbacks  *****
 *****************************
 *****************************/

void Vsa::VEvaluatorPumpSource::onData (
    IEvaluatorSink *pEvaluatorSink, IEvaluator *pEvaluator
) {
    if (pEvaluator)
	startEvaluator (pEvaluatorSink, pEvaluator);
    else
	pEvaluatorSink->OnData (0);
}

void Vsa::VEvaluatorPumpSource::onData (
    IEvaluatorSink *pEvaluatorSink,
    Vca::VBSConsumer *pPipeToPeer,
    Vca::VBSProducer *pPipeToHere,
    Vca::VBSProducer *pErrorPipeToHere
) {
    VEvaluatorPump::Reference pEvaluatorPump (
    	new VEvaluatorPump (evaluatorSettings (), pPipeToPeer, pPipeToHere)
    );

    IEvaluator::Reference pEvaluator;
    pEvaluatorPump->getRole (pEvaluator);

    startEvaluator (pEvaluatorSink, pEvaluator);
}
