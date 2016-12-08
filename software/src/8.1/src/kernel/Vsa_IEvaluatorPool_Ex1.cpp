/*****  Vsa_IEvaluatorPool_Ex1 Implementation  *****/
#define Vsa_IEvaluatorPool_Ex1

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

#include "Vsa_IEvaluatorPool_Ex1.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorPool_Ex1  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool_Ex1)

namespace Vsa {
// {F9E057AB-42F4-42AA-8263-06BCFA805D39}

    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool_Ex1,
	0xf9e057ab, 0x42f4, 0x42aa, 0x82, 0x63, 0x06, 0xbc, 0xfa, 0x80, 0x5d, 0x39
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, HardRestart	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, IsWorkerCreationFailureLmtReached , 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, GetCurrentGenerationTotalWorkers  , 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, GetWorkerInterface, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, GetTotalWorkers   , 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex1, HardStop   , 5);
}
