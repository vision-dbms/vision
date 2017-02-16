
#ifndef Vsa_IEvaluatorPoolSettings_Ex6_Interface
#define Vsa_IEvaluatorPoolSettings_Ex6_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex6
#define Vsa_IEvaluatorPoolSettings_Ex6 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex5.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex6)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex6, IEvaluatorPoolSettings_Ex5)
	VINTERFACE_ROLE_1 (GetGenerationMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetGenerationMaximum, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex6, IEvaluatorPoolSettings_Ex5);
	VINTERFACE_METHOD_1 (GetGenerationMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetGenerationMaximum, Vca::U32);
    VINTERFACE_END
}


#endif

