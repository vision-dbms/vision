/*****  Vsa_IEvaluatorPoolSettings Implementation  *****/
#define Vsa_IEvaluatorPoolSettings

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_IEvaluatorPoolSettings.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************************
 *****************************************
 *****                               *****
 *****  Vsa::IEvaluatorPoolSettings  *****
 *****                               *****
 *****************************************
 *****************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings)

namespace Vsa {
    // {412B945F-CEF7-4f82-9176-647F0D15620D}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings,
	0x412b945f, 0xcef7, 0x4f82, 0x91, 0x76, 0x64, 0x7f, 0xd, 0x15, 0x62, 0xd
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, MakeControl, 0);

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerSource, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerSource, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkersBeingCreatedMaximum, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkersBeingCreatedMaximum, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerMaximum, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerMaximum, 7);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerMinimum, 8);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerMinimum, 9);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetEvaluationAttemptMaximum, 10);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetEvaluationAttemptMaximum, 11);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerGenerationLimit, 12);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerGenerationLimit, 13);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerMinimumAvailable, 14);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerMinimumAvailable, 15);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerStartupQuery, 16);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerStartupQuery, 17);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetWorkerCreationFailureLimit, 18);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetWorkerCreationFailureLimit, 19);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetDebugFilePathName, 20);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetDebugFilePathName, 21);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, GetDebugSwitch, 22);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings, SetDebugSwitch, 23);
}
