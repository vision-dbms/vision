#ifndef Vsa_IEvaluatorPoolSettings_Ex2_Interface
#define Vsa_IEvaluatorPoolSettings_Ex2_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex2
#define Vsa_IEvaluatorPoolSettings_Ex2 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex1.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex2)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex2, IEvaluatorPoolSettings_Ex1)
	VINTERFACE_ROLE_1 (GetBCEvaluationsInProgressLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetBCEvaluationsInProgressLimit, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex2, IEvaluatorPoolSettings_Ex1);
	VINTERFACE_METHOD_1 (GetBCEvaluationsInProgressLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetBCEvaluationsInProgressLimit, Vca::U32);
    VINTERFACE_END
}


#endif
