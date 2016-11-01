#ifndef Vsa_IEvaluatorPoolSettings_Interface
#define Vsa_IEvaluatorPoolSettings_Interface

#ifndef Vsa_IEvaluatorPoolSettings
#define Vsa_IEvaluatorPoolSettings extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorSettings.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPoolSettings)

DECLARE_VsaINTERFACE (IEvaluatorPool)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPoolSettings, IEvaluatorSettings)
	VINTERFACE_ROLE_1 (MakeControl, IVReceiver<IEvaluatorPool*>*);

	VINTERFACE_ROLE_1 (GetWorkerSource, IVReceiver<IEvaluatorSource*>*);
	VINTERFACE_ROLE_1 (SetWorkerSource, IEvaluatorSource*);
	VINTERFACE_ROLE_1 (GetWorkersBeingCreatedMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkersBeingCreatedMaximum, Vca::U32);
	VINTERFACE_ROLE_1 (GetWorkerMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerMaximum, Vca::U32);
	VINTERFACE_ROLE_1 (GetWorkerMinimum, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerMinimum, Vca::U32);
  	VINTERFACE_ROLE_1 (GetWorkerGenerationLimit, IVReceiver<Vca::U32>*);
  	VINTERFACE_ROLE_1 (SetWorkerGenerationLimit, Vca::U32);
	VINTERFACE_ROLE_1 (GetEvaluationAttemptMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetEvaluationAttemptMaximum, Vca::U32);
	VINTERFACE_ROLE_1 (GetWorkerMinimumAvailable, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerMinimumAvailable, Vca::U32);
	VINTERFACE_ROLE_1 (GetWorkerStartupQuery, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetWorkerStartupQuery, VString const &);
	VINTERFACE_ROLE_1 (GetWorkerCreationFailureLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetWorkerCreationFailureLimit, Vca::U32);
	VINTERFACE_ROLE_1 (GetDebugFilePathName, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetDebugFilePathName, VString const &);
	VINTERFACE_ROLE_1 (GetDebugSwitch, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (SetDebugSwitch, bool);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPoolSettings, IEvaluatorSettings);
	VINTERFACE_METHOD_1 (MakeControl, IVReceiver<IEvaluatorPool*>*);

	VINTERFACE_METHOD_1 (GetWorkerSource, IVReceiver<IEvaluatorSource*>*);
	VINTERFACE_METHOD_1 (SetWorkerSource, IEvaluatorSource*);
	VINTERFACE_METHOD_1 (GetWorkersBeingCreatedMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkersBeingCreatedMaximum, Vca::U32);
	VINTERFACE_METHOD_1 (GetWorkerMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerMaximum, Vca::U32);
	VINTERFACE_METHOD_1 (GetWorkerMinimum, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerMinimum, Vca::U32);
  	VINTERFACE_METHOD_1 (GetWorkerGenerationLimit, IVReceiver<Vca::U32>*);
  	VINTERFACE_METHOD_1 (SetWorkerGenerationLimit, Vca::U32);
	VINTERFACE_METHOD_1 (GetEvaluationAttemptMaximum, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetEvaluationAttemptMaximum, Vca::U32);
	VINTERFACE_METHOD_1 (GetWorkerMinimumAvailable, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerMinimumAvailable, Vca::U32);
	VINTERFACE_METHOD_1 (GetWorkerStartupQuery, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetWorkerStartupQuery, VString const &);
	VINTERFACE_METHOD_1 (GetWorkerCreationFailureLimit, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetWorkerCreationFailureLimit, Vca::U32);
	VINTERFACE_METHOD_1 (GetDebugFilePathName, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetDebugFilePathName, VString const &);
	VINTERFACE_METHOD_1 (GetDebugSwitch, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (SetDebugSwitch, bool);
    VINTERFACE_END
}


#endif
