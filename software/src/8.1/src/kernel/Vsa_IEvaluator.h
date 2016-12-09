#ifndef Vsa_IEvaluator_Interface
#define Vsa_IEvaluator_Interface

#ifndef Vsa_IEvaluator
#define Vsa_IEvaluator extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

VINTERFACE_TEMPLATE_INSTANTIATIONS_AT_LEVEL (Vsa,IEvaluator,2)

/*************************
 *****  Definitions  *****
 *************************/

DECLARE_VsaINTERFACE (IEvaluatorClient)

namespace Vsa {
    VINTERFACE_ROLE (IEvaluator, IVUnknown)
        VINTERFACE_ROLE_3 (
            EvaluateExpression, IEvaluatorClient*, VString const&, VString const&
        );
        VINTERFACE_ROLE_4 (
            EvaluateURL, IEvaluatorClient*, VString const&, VString const&, VString const&
        );
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluator
     *
     * Evaluation engine interface providing methods for evaluating queries.
     * Query results are passed back to an IEvaluatorClient.
     *
     * @see IEvaluatorClient
     */
    VsaINTERFACE (IEvaluator, IVUnknown);
        /**
         * Evaluates an expression within a given context.
         * Results, including appropriate intermediate status, are returned using the interface methods of the given IEvaluatorClient.
         *
         * Future enhancements may include negotiated result return arranged with evaluators that support result negotiation by passing an appropriate subclass of IEvaluatorClient.
         *
         * @param p1 the client that will receive results.
         * @param p2 the expression context.
         * @param p3 the expression/query to evaluate.
         */
        VINTERFACE_METHOD_3 (
            EvaluateExpression, IEvaluatorClient*, VString const&, VString const&
        );
        /**
         * Evaluates a URL-based query within a given context.
         *
         * @copydetail EvaluateExpression()
         * @param p4 the environment to use when evaluating the given query.
         */
        VINTERFACE_METHOD_4 (
            EvaluateURL, IEvaluatorClient*, VString const&, VString const&, VString const&
        );
    VINTERFACE_END
}


#endif
