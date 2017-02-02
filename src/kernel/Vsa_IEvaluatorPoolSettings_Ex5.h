#ifndef Vsa_IEvaluatorPoolSettings_Ex5_Interface
#define Vsa_IEvaluatorPoolSettings_Ex5_Interface

#ifndef Vsa_IEvaluatorPoolSettings_Ex5
#define Vsa_IEvaluatorPoolSettings_Ex5 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPoolSettings_Ex4.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings_Ex5)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings_Ex5, IEvaluatorPoolSettings_Ex4)
	VINTERFACE_ROLE_1 (GetWorkerCreationFailureSoftLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerCreationFailureSoftLimit, Vca::U32);
	VINTERFACE_ROLE_1 (GetWorkerPIDQuery, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetWorkerPIDQuery, VString const &);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings_Ex5, IEvaluatorPoolSettings_Ex4);
	VINTERFACE_METHOD_1 (GetWorkerCreationFailureSoftLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerCreationFailureSoftLimit, Vca::U32);
	VINTERFACE_METHOD_1 (GetWorkerPIDQuery, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetWorkerPIDQuery, VString const &);
    VINTERFACE_END
}


#endif
