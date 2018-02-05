#ifndef Vsa_VGenericEvaluation_Interface
#define Vsa_VGenericEvaluation_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluation.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {

    /********************************
     *----  VGenericEvaluation  ----*
     ********************************/

    class Vsa_API VGenericEvaluation : public VEvaluation {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VGenericEvaluation, VEvaluation);

    //  Construction
    public:
	VGenericEvaluation (	//  ... Expression
	    VEvaluator*		pEvaluator,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery
	);
	VGenericEvaluation (	//  ... URL
	    VEvaluator*		pEvaluator,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    VString const&	rEnvironment
	);

    //  Destruction
    protected:
	~VGenericEvaluation ();

    //  IEvaluation Role
    private:
	Vca::VRole<ThisClass,IEvaluation> m_pIEvaluation;
    public:
	virtual void getRole (IEvaluation::Reference &rpRole) OVERRIDE;
    };
}


#endif
