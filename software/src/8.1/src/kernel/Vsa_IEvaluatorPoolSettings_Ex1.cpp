/*****  Vsa_IEvaluatorPoolSettings_Ex1 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex1

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

#include "Vsa_IEvaluatorPoolSettings_Ex1.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex1  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex1)

namespace Vsa {
    // {B907E368-A5E1-4050-BEA0-F28C35675148}
    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex1,
	0xb907e368, 0xa5e1, 0x4050, 0xbe, 0xa0, 0xf2, 0x8c, 0x35, 0x67, 0x51, 0x48
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex1, GetWorkerMaximumAvailable, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex1, SetWorkerMaximumAvailable, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex1, GetShrinkTimeOut, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex1, SetShrinkTimeOut, 3);
}
