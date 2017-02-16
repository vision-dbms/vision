#ifndef Vsa_IEvaluatorPoolSettings_Ex1_Interface
#define Vsa_IEvaluatorPoolSettings_Ex1_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex1
#define Vsa_IEvaluatorPoolSettings_Ex1 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex1)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex1, IEvaluatorPoolSettings)
	VINTERFACE_ROLE_1 (GetWorkerMaximumAvailable, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerMaximumAvailable, Vca::U32);
	VINTERFACE_ROLE_1 (GetShrinkTimeOut, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetShrinkTimeOut, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex1, IEvaluatorPoolSettings);
	VINTERFACE_METHOD_1 (GetWorkerMaximumAvailable, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerMaximumAvailable, Vca::U32);
	VINTERFACE_METHOD_1 (GetShrinkTimeOut, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetShrinkTimeOut, Vca::U32);
    VINTERFACE_END
}


#endif
