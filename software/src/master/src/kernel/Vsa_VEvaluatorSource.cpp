/*****  Vsa_VEvaluatorSource Implementation  *****/

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

#include "Vsa_VEvaluatorSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VInterfaceEKG.h"

#include "Vsa_IEvaluatorClient.h"

#include "Vsa_VEvaluator.h"
#include "Vsa_VEvaluation.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vsa::VEvaluatorSource::Starter  *****
 *****                                  *****
 ********************************************
 ********************************************/

namespace Vsa {
    class VEvaluatorSource::Starter : public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Starter, Vca::VRolePlayer);

    //  Typedef
	typedef Vca::VInterfaceEKG<ThisClass,IEvaluator> interface_monitor_t;

    //  Construction
    public:
	Starter (VEvaluatorSource *pSource, IEvaluatorSink *pSink, IEvaluator *pEvaluator);

    //  Destruction
    private:
	~Starter () {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IEvaluatorClient Role
    private:
	Vca::VRole<ThisClass,IEvaluatorClient> m_pIEvaluatorClient;
    public:
	void getRole (IEvaluatorClient::Reference &rpRole) {
	    m_pIEvaluatorClient.getRole (rpRole);
	}

    //  Vca::IEvaluatorClient Methods
    public:
	void OnResult (
	    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
	);
	void OnAccept (
	    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
	);

	void OnChange (
	    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
	);

    //  Vca::IClient Methods
    public:
	void OnError (
	    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
	);
	void OnEnd (Vca::IClient *pRole);

    //  EKG
    private:
	void signalIEvaluatorEKG ();
	void cancelIEvaluatorEKG ();

    //  State
    private:
	VEvaluatorSource::Reference	const m_pSource;
	IEvaluatorSink::Reference	const m_pSink;
	IEvaluator::Reference		const m_pEvaluator;
	interface_monitor_t::Reference	      m_pEvaluatorEKG;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorSource::Starter::Starter (
    VEvaluatorSource *pSource, IEvaluatorSink *pSink, IEvaluator *pEvaluator
) : m_pSource (pSource), m_pSink (pSink), m_pEvaluator (pEvaluator), m_pIEvaluatorClient (this) {
    CAM_OPERATION(co) << "message" << "Vsa::VEvaluatorSource::Starter::Starter()";
    retain (); {
	traceInfo ("Sending StartupQuery");
	IEvaluatorClient::Reference pClientRole;
	getRole (pClientRole);
	m_pEvaluatorEKG.setTo (new interface_monitor_t (this, &ThisClass::signalIEvaluatorEKG, m_pEvaluator));
        if (IEvaluator_Ex1 *const pEvaluator1 = dynamic_cast<IEvaluator_Ex1*>(m_pEvaluator.referent())) {
            pEvaluator1->EvaluateExpression_Ex (pClientRole, "", pSource->startupQuery (), co.getId().c_str(), co.chainId().c_str());
        } else {
            m_pEvaluator->EvaluateExpression (pClientRole, "", pSource->startupQuery ());
        }

    } untain ();
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/***********************************
 *****  Vsa::IEvaluatorClient  *****
 ***********************************/

void Vsa::VEvaluatorSource::Starter::OnResult (
    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
) {
    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    traceInfo ("Got Startup Expression Result"); traceInfo (rOutput.content ());
    m_pSink->OnData (m_pEvaluator);
}

void Vsa::VEvaluatorSource::Starter::OnAccept (
    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
) {
}

void Vsa::VEvaluatorSource::Starter::OnChange (
    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
) {
}


/**************************
 *****  Vca::IClient  *****
 **************************/

void Vsa::VEvaluatorSource::Starter::OnEnd (Vca::IClient *pRole) {
    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    m_pSink->OnEnd ();
}

void Vsa::VEvaluatorSource::Starter::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
) {
    // Cancel the EKG.
    cancelIEvaluatorEKG ();

    m_pSink->OnError (pError, rMessage);
}

/*****************
 *****  EKG  *****
 *****************/

void Vsa::VEvaluatorSource::Starter::signalIEvaluatorEKG () {
  
    traceInfo ("IEvaluator disappeared.");
    m_pSink->OnError (0, "Failed during Startup Query.");
}

void Vsa::VEvaluatorSource::Starter::cancelIEvaluatorEKG () {
    if (m_pEvaluatorEKG.isntNil ()) {
        m_pEvaluatorEKG->cancel ();
        m_pEvaluatorEKG.clear ();
    }
}



/***********************************
 ***********************************
 *****                         *****
 *****  Vsa::VEvaluatorSource  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluatorSource::VEvaluatorSource (VString const &rStartupQuery)
 : m_iStartupQuery (rStartupQuery)
 , m_pIEvaluatorSource (this) 
 , m_pIEvaluatorSourceControl (this) {
}

Vsa::VEvaluatorSource::VEvaluatorSource () 
 : m_pIEvaluatorSource (this)
 , m_pIEvaluatorSourceControl (this) {
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/******************************************
 *****  Vsa::IEvaluatorSourceControl ******
 ******************************************/

void Vsa::VEvaluatorSource::GetStartupQuery (
    IEvaluatorSourceControl *pRole, IVReceiver<VString const &>* pClient
) {
    if (pClient) {
	pClient->OnData (m_iStartupQuery);
    }
}

void Vsa::VEvaluatorSource::SetStartupQuery (
    IEvaluatorSourceControl *pRole, VString const &rQuery
) {
    setStartupQueryTo (rQuery);
}

/*******************************
 *******************************
 *****  Evaluator Startup  *****
 *******************************
 *******************************/

void Vsa::VEvaluatorSource::startEvaluator (IEvaluatorSink *pSink, IEvaluator *pEvaluator) {
    if (m_iStartupQuery.isEmpty ()) {
		pSink->OnData (pEvaluator);
	}
    else {
		Starter::Reference pStarter (new Starter (this, pSink, pEvaluator));
    }
}
