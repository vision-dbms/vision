#ifndef Vsa_IEvaluatorPool_Ex4_Interface
#define Vsa_IEvaluatorPool_Ex4_Interface

#ifndef Vsa_IEvaluatorPool_Ex4
#define Vsa_IEvaluatorPool_Ex4 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPool_Ex3.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool_Ex4)

/*************************
 *****  Definitions  *****
 *************************/


namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool_Ex4, IEvaluatorPool_Ex3)
        VINTERFACE_ROLE_2 (QueryDetails, Vca::U32, IVReceiver<VString const&>*);
        VINTERFACE_ROLE_2 (CancelQuery, Vca::U32, IVReceiver<bool>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool_Ex4
     *
     * Pool interface extension 3; adds several worker and stats operations.
     */
    VsaINTERFACE (IEvaluatorPool_Ex4, IEvaluatorPool_Ex3);
        /**
         * Used to retrieve a machine-readable dump of a particular queued request.
         *
         * @param p1 the index of the request whose details ought be dumped.
         * @param p2 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_2 (QueryDetails, Vca::U32, IVReceiver<VString const&>*);

        /**
         * Used to cancel a query; removes it from the queue.
         *
         * @param p1 the index of the request to be cancelled.
         * @param p2 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_2 (CancelQuery, Vca::U32, IVReceiver<bool>*);
    VINTERFACE_END
}


#endif
