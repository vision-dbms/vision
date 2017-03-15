#ifndef Vsa_IEvaluatorPoolSettings_Ex3_Interface
#define Vsa_IEvaluatorPoolSettings_Ex3_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex3
#define Vsa_IEvaluatorPoolSettings_Ex3 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex2.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex3)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex3, IEvaluatorPoolSettings_Ex2)
	VINTERFACE_ROLE_1 (GetStopTimeOut, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetStopTimeOut, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex3, IEvaluatorPoolSettings_Ex2);
	VINTERFACE_METHOD_1 (GetStopTimeOut, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetStopTimeOut, Vca::U32);
    VINTERFACE_END
}


#endif
