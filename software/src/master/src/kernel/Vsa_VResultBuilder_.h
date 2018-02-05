#ifndef Vsa_VResultBuilder__Interface
#define Vsa_VResultBuilder__Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    class VCohort;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    template <class ResultClass> class VResultBuilder_ : public ResultClass::BuilderClient {
	DECLARE_CONCRETE_RTTLITE (VResultBuilder_, typename ResultClass::BuilderClient);

    //  Aliases
    public:
	typedef typename ResultClass::Reference RR;

    //  Construction
    public:
	VResultBuilder_(
	    ResultClass *pRUC	// RUC == Result Under Construction
	) : BaseClass (pRUC->cohort ()), m_pRUC (pRUC) {
	}
	template <typename T> VResultBuilder_(
	    ResultClass *pRUC, T pT
	) : BaseClass (pRUC->cohort (), pT), m_pRUC (pRUC) {
	}

    //  Destruction
    private:
	~VResultBuilder_() {
	}

    //  Callbacks
    private:
	void OnAccept_(Vsa::IEvaluation *pEvaluation, Vca::U32 xQueuePosition) OVERRIDE {
	    if (m_pRUC)
		m_pRUC->onAccept (pEvaluation, xQueuePosition);
	}
	void OnChange_(Vca::U32 xQueuePosition) OVERRIDE {
	    if (m_pRUC)
		m_pRUC->onChange (xQueuePosition);
	}
	void OnError_(Vca::IError *pInterface, VString const &rMessage) OVERRIDE {
	    if (m_pRUC) {
		m_pRUC->onError (pInterface, rMessage);
		m_pRUC.clear ();

		BaseClass::OnError_(pInterface, rMessage);
	    }
	}
	void OnResult_(Vsa::IEvaluationResult *pResult, VString const &rOutput) OVERRIDE {
	    if (m_pRUC) {
		m_pRUC->onResult (pResult, rOutput);
		m_pRUC.clear ();
	    }
	}

    //  State
    private:
	RR m_pRUC;
    };
}


#endif
