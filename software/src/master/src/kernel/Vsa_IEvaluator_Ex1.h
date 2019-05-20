#ifndef Vsa_IEvaluator_Ex1_Interface
#define Vsa_IEvaluator_Ex1_Interface

#ifndef Vsa_IEvaluator_Ex1
#define Vsa_IEvaluator_Ex1 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluator.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_AT_LEVEL (Vsa,IEvaluator_Ex1,2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluator_Ex1, IEvaluator)
        VINTERFACE_ROLE_5 (
            EvaluateExpression_Ex, IEvaluatorClient*, VString const&, VString const&, VString const&, VString const&
        );
        VINTERFACE_ROLE_6 (
            EvaluateURL_Ex, IEvaluatorClient*, VString const&, VString const&, VString const&, VString const&, VString const&
        );
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluator_Ex1, IEvaluator);
        VINTERFACE_METHOD_5 (
            EvaluateExpression_Ex, IEvaluatorClient*, VString const&, VString const&, VString const&, VString const&
        );
        VINTERFACE_METHOD_6 (
            EvaluateURL_Ex, IEvaluatorClient*, VString const&, VString const&, VString const&, VString const&, VString const&
        );
    VINTERFACE_END
}


#endif
