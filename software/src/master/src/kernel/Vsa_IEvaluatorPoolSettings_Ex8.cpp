/*****  Vsa_IEvaluatorPoolSettings_Ex8 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex8

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

#include "Vsa_IEvaluatorPoolSettings_Ex8.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex8  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex8)

namespace Vsa {

    // {69139d35-2509-4325-9dec-5e20b1607427}

    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex8,
        0x69139d35, 0x2509, 0x4325, 0x9d, 0xec, 0x5e, 0x20, 0xb1, 0x60, 0x74, 0x27
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex8, GetEvaluationOnErrorAttemptMaximum, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex8, SetEvaluationOnErrorAttemptMaximum, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex8, GetEvaluationTimeOutAttemptMaximum,2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex8, SetEvaluationTimeOutAttemptMaximum, 3);
}
