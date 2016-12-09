#ifndef Vsa_IEvaluatorPool_Ex2_Interface
#define Vsa_IEvaluatorPool_Ex2_Interface

#ifndef Vsa_IEvaluatorPool_Ex2
#define Vsa_IEvaluatorPool_Ex2 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPool_Ex1.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

DECLARE_VsaINTERFACE (IEvaluatorClient)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool_Ex2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool_Ex2, IEvaluatorPool_Ex1)
	VINTERFACE_ROLE_3 (BroadcastExpression, IEvaluatorClient*, VString const&, VString const&);
	VINTERFACE_ROLE_4 (BroadcastURL, IEvaluatorClient*, VString const&, VString const&, VString const&);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool_Ex2
     *
     * Pool interface extension 2; adds broadcast functionality.
     */
    VsaINTERFACE (IEvaluatorPool_Ex2, IEvaluatorPool_Ex1);
        /**
         * Sends a broadcast query to this pool.
         *
         * @param p1 the IEvaluatorClient interface for the client that should receive the results of the query.
         * @param p2 the path of the query.
         * @param p3 the query string.
         */
	VINTERFACE_METHOD_3 (BroadcastExpression, IEvaluatorClient*, VString const&, VString const&);
        /**
         * Sends a broadcast URL to this pool.
         *
         * @copydetails BroadcastExpression()
         */
	VINTERFACE_METHOD_4 (BroadcastURL, IEvaluatorClient*, VString const&, VString const&, VString const&);
    VINTERFACE_END
}


#endif
