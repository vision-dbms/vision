#ifndef Vsa_IEvaluatorPool_Ex1_Interface
#define Vsa_IEvaluatorPool_Ex1_Interface

#ifndef Vsa_IEvaluatorPool_Ex1
#define Vsa_IEvaluatorPool_Ex1 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPool.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool_Ex1)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool_Ex1, IEvaluatorPool)
	VINTERFACE_ROLE_1 (HardRestart, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (IsWorkerCreationFailureLmtReached, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (GetCurrentGenerationTotalWorkers, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_2 (GetWorkerInterface, IVReceiver<IVUnknown*>*, Vca::U32);
	VINTERFACE_ROLE_1 (GetTotalWorkers, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (HardStop, IVReceiver<bool>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool_Ex1
     *
     * Pool interface extension 1.
     */
    VsaINTERFACE (IEvaluatorPool_Ex1, IEvaluatorPool);
        /**
         * Send hard restart directive to this pool.
         *
         * @param p1 receiver for the return status of the directive.
         */
	VINTERFACE_METHOD_1 (HardRestart, IVReceiver<bool>*);
        /**
         * Query this pool about its worker creation failure limit.
         *
         * @param p1 receiver for the status of the worker creation failure limit.
         */
	VINTERFACE_METHOD_1 (IsWorkerCreationFailureLmtReached, IVReceiver<bool>*);
        /**
         * Query this pool about its current generation's worker count.
         *
         * @param p1 receiver for the worker count to be returned by the pool.
         */
	VINTERFACE_METHOD_1 (GetCurrentGenerationTotalWorkers, IVReceiver<Vca::U32>*);
        /**
         * Request a worker interface for a worker in this pool.
         *
         * @param p1 receiver for the worker interface to be returned by the pool.
         * @param p2 the worker ID of the worker to which an interface should be returned.
         */
	VINTERFACE_METHOD_2 (GetWorkerInterface, IVReceiver<IVUnknown*>*, Vca::U32);
        /**
         * Query this pool about its total worker count.
         *
         * @param p1 receiver for the worker count to be returned by the pool.
         */
	VINTERFACE_METHOD_1 (GetTotalWorkers, IVReceiver<Vca::U32>*);
        /**
         * Send hard stop directive to this pool.
         *
         * @param p1 receiver for the return status of the directive.
         */
	VINTERFACE_METHOD_1 (HardStop, IVReceiver<bool>*);
    VINTERFACE_END
}


#endif
