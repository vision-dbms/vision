#ifndef Vsa_IEvaluatorClient_Interface
#define Vsa_IEvaluatorClient_Interface

#ifndef Vsa_IEvaluatorClient
#define Vsa_IEvaluatorClient extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorClient)

/*************************
 *****  Definitions  *****
 *************************/

DECLARE_VsaINTERFACE (IEvaluation)
DECLARE_VsaINTERFACE (IEvaluationResult)

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorClient, Vca::IClient)
        VINTERFACE_ROLE_2 (OnAccept, IEvaluation*, Vca::U32);
        VINTERFACE_ROLE_1 (OnChange, Vca::U32);
        VINTERFACE_ROLE_2 (OnResult, IEvaluationResult*, VString const&);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorClient
     *
     * Client for evaluators providing methods for receiving evaluation responses.
     * Intended to be used in conjunction with IEvaluator and IEvaluation objects.
     *
     * @see IEvaluator
     * @see IEvaluation
     */
    VsaINTERFACE (IEvaluatorClient, Vca::IClient);
        /**
         * Invoked by an evaluator to acknowledge the acceptance of an evaluation.
         * Evaluators provide an IEvaluation interface that can then be used to control and/or query the execution process.
         * Evaluators may or may not send this message on receipt of a request.
         *
         * @param p1 the IEvaluation instance representing the evaluation in progress.
         * @param p2 the queue position of the given evaluation within the evaluator.
         */
        VINTERFACE_METHOD_2 (OnAccept, IEvaluation*, Vca::U32);

        /**
         * Invoked by an IEvaluator to acknowledge evaluation progress.
         * Evaluators may or many not send this message. When they do, by default they use this message to acknowledge the start of an evaluation (i.e., queue position 0). Note that queue position isn't guaranteed to decrease monotonically since evaluations can be aborted and restarted at the discretion of the evaluator.
         *
         * @param p1 the queue position of the given evaluation within the evaluator.
         */
        VINTERFACE_METHOD_1 (OnChange, Vca::U32);

        /**
         * Invoked by an evaluator when the result of an evaluation is available.
         * Sending an IEvaluationResult instance by way of this message is optional and at the discretion of the evaluator.
         *
         * @param p1 the result of the evaluation, optionally null at the discretion of the evaluator.
         * @param p2 the output of the evaluation.
         */
        VINTERFACE_METHOD_2 (OnResult, IEvaluationResult*, VString const&);
    VINTERFACE_END
}


#endif
