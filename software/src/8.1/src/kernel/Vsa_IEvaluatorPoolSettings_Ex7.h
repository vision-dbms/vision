#ifndef Vsa_IEvaluatorPoolSettings_Ex7_Interface
#define Vsa_IEvaluatorPoolSettings_Ex7_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex7
#define Vsa_IEvaluatorPoolSettings_Ex7 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex6.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex7)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex7, IEvaluatorPoolSettings_Ex6)
	VINTERFACE_ROLE_1 (GetQueryLogLength, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetQueryLogLength, Vca::U32);
	VINTERFACE_ROLE_1 (GetResultLogLength, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetResultLogLength, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex7, IEvaluatorPoolSettings_Ex6);
	VINTERFACE_METHOD_1 (GetQueryLogLength, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetQueryLogLength, Vca::U32);
	VINTERFACE_METHOD_1 (GetResultLogLength, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetResultLogLength, Vca::U32);
    VINTERFACE_END
}


#endif
