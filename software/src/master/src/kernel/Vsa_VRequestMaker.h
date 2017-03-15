#ifndef Vsa_VRequestMaker_Interface
#define Vsa_VRequestMaker_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vsa_IEvaluator.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VEvaluatorClient;

    class Vsa_API VRequestMaker : public VTransient {
	DECLARE_FAMILY_MEMBERS (VRequestMaker, VTransient);

    //  Aliases
    public:
	typedef Vca::VGoferInterface<IEvaluator> evaluator_gofer_t;

    //  Request
    public:
	class Request;

    //  Construction
    private:
	static evaluator_gofer_t* NewEvaluatorGofer (VString const& rEvaluatorName);
    protected:
	VRequestMaker (VString const &rEvaluatorName);
	VRequestMaker (evaluator_gofer_t *pEvaluatorGofer);

    //  Destruction
    protected:
	~VRequestMaker ();

    //  Access
    protected:
	evaluator_gofer_t* gofer () const {
	    return m_pGofer;
	}

    //  Use
    protected:
	void makeRequest (VEvaluatorClient *pResultBuilder, VString const &rQuery) const;
	
    //  State
    private:
	evaluator_gofer_t::Reference const m_pGofer;
    };
}


#endif
