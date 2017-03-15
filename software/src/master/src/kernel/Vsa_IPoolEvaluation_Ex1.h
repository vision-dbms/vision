#ifndef Vsa_IPoolEvaluation_Ex1_Interface
#define Vsa_IPoolEvaluation_Ex1_Interface

#ifndef Vsa_IPoolEvaluation_Ex1
#define Vsa_IPoolEvaluation_Ex1 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IPoolEvaluation.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolEvaluation_Ex1)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IPoolEvaluation_Ex1, IPoolEvaluation)
	VINTERFACE_ROLE_1 (SetRetryable, bool);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolEvaluation_Ex1, IPoolEvaluation);
        /**
         * Enables or disable retries of this evaluation.
         *
         * @param p1 true if retries should be enabled, false otherwise.
         */
	VINTERFACE_METHOD_1 (SetRetryable, bool);
    VINTERFACE_END
}


#endif
