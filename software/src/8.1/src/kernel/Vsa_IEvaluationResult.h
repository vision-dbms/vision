#ifndef Vsa_IEvaluationResult_Interface
#define Vsa_IEvaluationResult_Interface

#ifndef Vsa_IEvaluationResult
#define Vsa_IEvaluationResult extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluationResult)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluationResult, IVUnknown)
	VINTERFACE_ROLE_1 (GetTimingReport, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluationResult
     *
     * Interface sent by evaluators to their clients when results for an evaluation are available.
     * Provides a method by which the client can ask for a timing report for the completed evaluation.
     */
    VsaINTERFACE (IEvaluationResult, IVUnknown);
        /**
         * Requests a timing report for the completed evaluation.
         *
         * @param p1 the receiver to which the requested timing result should be sent.
         */
	VINTERFACE_METHOD_1 (GetTimingReport, IVReceiver<VString const&>*);
    VINTERFACE_END
}


#endif
