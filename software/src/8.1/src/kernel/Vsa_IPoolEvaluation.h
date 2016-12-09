#ifndef Vsa_IPoolEvaluation_Interface
#define Vsa_IPoolEvaluation_Interface

#ifndef Vsa_IPoolEvaluation
#define Vsa_IPoolEvaluation extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluation.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolEvaluation)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IPoolEvaluation, IEvaluation)
	VINTERFACE_ROLE_0 (UseAnyGenerationWorker);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolEvaluation, IEvaluation);
	VINTERFACE_METHOD_0 (UseAnyGenerationWorker);
    VINTERFACE_END
}


#endif
