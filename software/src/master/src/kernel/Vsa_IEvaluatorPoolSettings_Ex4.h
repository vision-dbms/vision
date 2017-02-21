#ifndef Vsa_IEvaluatorPoolSettings_Ex4_Interface
#define Vsa_IEvaluatorPoolSettings_Ex4_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex4
#define Vsa_IEvaluatorPoolSettings_Ex4 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex3.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex4)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex4, IEvaluatorPoolSettings_Ex3)
	VINTERFACE_ROLE_1 (GetWorkerQueryHistoryLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerQueryHistoryLimit, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex4, IEvaluatorPoolSettings_Ex3);
	VINTERFACE_METHOD_1 (GetWorkerQueryHistoryLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerQueryHistoryLimit, Vca::U32);
    VINTERFACE_END
}


#endif
