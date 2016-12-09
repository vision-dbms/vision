#ifndef Vsa_IEvaluation_Interface
#define Vsa_IEvaluation_Interface

#ifndef Vsa_IEvaluation
#define Vsa_IEvaluation extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IRequest.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluation)

namespace Vsa {
    VINTERFACE_ROLE (IEvaluation, Vca::IRequest)
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluation
     *
     * Interface representing a queued or running evaluation on an evaluator.
     * Typically provided by an evaluator to its client in order to acknowledge receipt of an evaluation.
     * Intended to be used in conjunction with IEvaluator and IEvaluatorClient.
     * Should be subclassed for particular types of IEvaluator and IEvaluatorClient types, such as is done in IPoolEvaluation.
     *
     * @see IEvaluator
     * @see IEvaluatorClient
     */
    VsaINTERFACE (IEvaluation, Vca::IRequest);
    VINTERFACE_END
}


#endif
