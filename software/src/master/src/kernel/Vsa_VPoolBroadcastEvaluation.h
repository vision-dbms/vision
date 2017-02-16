
#ifndef Vsa_VPoolBroadcastEvaluation_Interface
#define Vsa_VPoolBroadcastEvaluation_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VPoolEvaluation.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {

    class VPoolBroadcast;

    /****************************
     *----  VPoolBroadcast  ----*
     ****************************/

    class Vsa_API VPoolBroadcastEvaluation : public VPoolEvaluation {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VPoolBroadcastEvaluation, VPoolEvaluation);

    //  Construction
    public:
	VPoolBroadcastEvaluation (	//  ... Expression
	    VPoolBroadcast*     pBroadcast,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    Vca::U32		iValidGeneration = 0,
	    Vca::U32		iWorkerId = 0
	);
	VPoolBroadcastEvaluation (	//  ... URL
	    VPoolBroadcast*	pBroadcast,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    VString const&	rEnvironment,
	    Vca::U32		iValidGeneration = 0,
	    Vca::U32		iWorkerId = 0
	);

    //  Destruction
    private:
	~VPoolBroadcastEvaluation ();

    //  Query
    public:
	VPoolBroadcast* poolBroadcast () const {
	    return m_pPoolBroadcast;
	}

        /**
         * Returns false; all broadcast evaluations are never retryable.
         */
        virtual bool retryable() const {
            return false;
        }

    //  State
    protected:
	VReference<VPoolBroadcast>  m_pPoolBroadcast;
    };
}


#endif

