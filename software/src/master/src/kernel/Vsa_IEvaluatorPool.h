#ifndef Vsa_IEvaluatorPool_Interface
#define Vsa_IEvaluatorPool_Interface

#ifndef Vsa_IEvaluatorPool
#define Vsa_IEvaluatorPool extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorControl.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool)

/*************************
 *****  Definitions  *****
 *************************/

DECLARE_VsaINTERFACE (IEvaluatorPoolSettings)

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool, IEvaluatorControl)
	VINTERFACE_ROLE_1 (GetSettings, IVReceiver<IEvaluatorPoolSettings*>*);
	VINTERFACE_ROLE_1 (AddEvaluator, IEvaluator*);
	VINTERFACE_ROLE_1 (FlushWorkers, IVReceiver<bool>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool
     *
     * Pool interface.
     */
    VsaINTERFACE (IEvaluatorPool, IEvaluatorControl);
        /**
         * Retrieves an IEvaluatorPoolSettings interface for this pool.
         */
	VINTERFACE_METHOD_1 (GetSettings, IVReceiver<IEvaluatorPoolSettings*>*);
        /**
         * Adds an evaluator to this pool.
         */
	VINTERFACE_METHOD_1 (AddEvaluator, IEvaluator*);
        /**
         * Retires all current workers.
         */
	VINTERFACE_METHOD_1 (FlushWorkers, IVReceiver<bool>*);
    VINTERFACE_END
}


#endif
