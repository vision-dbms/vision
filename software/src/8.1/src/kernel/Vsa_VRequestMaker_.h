#ifndef Vsa_VRequestMaker__Interface
#define Vsa_VRequestMaker__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VRequestMaker.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    template <class ResultClass> class VRequestMaker_ : public VRequestMaker {
	DECLARE_FAMILY_MEMBERS (VRequestMaker_<ResultClass>, VRequestMaker);

    //  Aliases
    public:
	typedef typename ResultClass::Builder	ResultBuilder;
	typedef typename ResultClass::Reference ResultReference;

    //  Construction
    public:
	VRequestMaker_(VString const &rEvaluatorName) : BaseClass (rEvaluatorName) {
	}
	VRequestMaker_(BaseClass::evaluator_gofer_t *pEvaluatorGofer) : BaseClass (pEvaluatorGofer) {
	}

    //  Destruction
    public:
	~VRequestMaker_() {
	}

    //  Use
    public:
	void makeRequest (ResultReference &rpResult, VString const &rQuery) const {
	    Vca::VCohortClaim cohortClaim (gofer (), false);
	    rpResult.setTo (new ResultClass ());
	    typename ResultBuilder::Reference const pResultBuilder (new ResultBuilder (rpResult));
	    BaseClass::makeRequest (pResultBuilder, rQuery);
	}
	template <typename T> void makeRequest (ResultReference &rpResult, VString const &rQuery, T pT) const {
	    Vca::VCohortClaim iCohortClaim (gofer (), false);
	    rpResult.setTo (new ResultClass ());
	    typename ResultBuilder::Reference const pResultBuilder (new ResultBuilder (rpResult, pT));
	    BaseClass::makeRequest (pResultBuilder, rQuery);
	}
    };
}


#endif
