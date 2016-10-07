/*****  Vsa_IEvaluatorPoolSettings_Ex4 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex4

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

#include "Vsa_IEvaluatorPoolSettings_Ex4.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex4  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex4)

namespace Vsa {

    // {66ACD425-458B-4DBE-A247-5E12D9E4AC7D}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex4,
	0x66acd425, 0x458b, 0x4dbe, 0xa2, 0x47, 0x5e, 0x12, 0xd9, 0xe4, 0xac, 0x7d
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex4, GetWorkerQueryHistoryLimit, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex4, SetWorkerQueryHistoryLimit, 1);
}
