#ifndef Vsa_IEvaluatorPoolSettings_Ex8_Interface
#define Vsa_IEvaluatorPoolSettings_Ex8_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex8
#define Vsa_IEvaluatorPoolSettings_Ex8 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex7.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex8)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex8, IEvaluatorPoolSettings_Ex7)
		VINTERFACE_ROLE_1 (GetEvaluationOnErrorAttemptMaximum, IVReceiver<Vca::U32>*);
		VINTERFACE_ROLE_1 (SetEvaluationOnErrorAttemptMaximum, Vca::U32);
		VINTERFACE_ROLE_1 (GetEvaluationTimeOutAttemptMaximum, IVReceiver<Vca::U32>*);
		VINTERFACE_ROLE_1 (SetEvaluationTimeOutAttemptMaximum, Vca::U32);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex8, IEvaluatorPoolSettings_Ex7);
		VINTERFACE_METHOD_1 (GetEvaluationOnErrorAttemptMaximum, IVReceiver<Vca::U32>*);
		VINTERFACE_METHOD_1 (SetEvaluationOnErrorAttemptMaximum, Vca::U32);
		VINTERFACE_METHOD_1 (GetEvaluationTimeOutAttemptMaximum, IVReceiver<Vca::U32>*);
    	VINTERFACE_METHOD_1 (SetEvaluationTimeOutAttemptMaximum, Vca::U32);
    VINTERFACE_END
}


#endif
