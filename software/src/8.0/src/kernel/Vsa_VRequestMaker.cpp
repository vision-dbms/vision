/*****  Vsa_VEvaluationResult Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VRequestMaker.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_IDirectory.h"

#include "Vca_VcaGofer.h"

#include "Vsa_VEvaluatorClient.h"


/*****************************************
 *****************************************
 *****                               *****
 *****  Vsa::VRequestMaker::Request  *****
 *****                               *****
 *****************************************
 *****************************************/

namespace Vsa {
    class VRequestMaker::Request {
	DECLARE_NUCLEAR_FAMILY (Request);

    //  Construction
    public:
	Request (VEvaluatorClient *pClient, VString const &rQuery) : m_pClient (pClient), m_iQuery (rQuery) {
	}
	Request (ThisClass const &rOther) : m_pClient (rOther.m_pClient), m_iQuery (rOther.m_iQuery) {
	}
    //  Destruction
    public:
	~Request () {
	}

    //  Callback
    public:
	void operator () (IEvaluator *pEvaluator) const {
	    m_pClient->onQuery (pEvaluator, m_iQuery);
	}
	void operator () (Vca::IError *pInterface, VString const &rMessage) const {
	    m_pClient->onError (pInterface, rMessage);
	}

    //  State
    private:
	VEvaluatorClient::Reference const	m_pClient;
	VString const				m_iQuery;
    };
}


/********************************
 ********************************
 *****                      *****
 *****  Vsa::VRequestMaker  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VRequestMaker::evaluator_gofer_t* Vsa::VRequestMaker::NewEvaluatorGofer (VString const& rEvaluatorName) {
    Vca::VCohortClaim cohortClaim;	// ... give the gofer a home.
    return new Vca::Gofer::Named<IEvaluator,Vca::IDirectory> (rEvaluatorName);
}

Vsa::VRequestMaker::VRequestMaker (VString const &rEvaluatorName) : m_pGofer (NewEvaluatorGofer (rEvaluatorName)) {
}

Vsa::VRequestMaker::VRequestMaker (evaluator_gofer_t *pEvaluatorGofer) : m_pGofer (pEvaluatorGofer) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VRequestMaker::~VRequestMaker () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vsa::VRequestMaker::makeRequest (VEvaluatorClient *pClient, VString const& rQuery) const {
    Request const iRequest (pClient, rQuery);
    m_pGofer->supply (iRequest);
}
