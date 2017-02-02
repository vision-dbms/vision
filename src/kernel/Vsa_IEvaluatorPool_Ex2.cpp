/*****  Vsa_IEvaluatorPool_Ex2 Implementation  *****/
#define Vsa_IEvaluatorPool_Ex2

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

#include "Vsa_IEvaluatorPool_Ex2.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorPool_Ex2  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool_Ex2)

namespace Vsa {
// {A7D2E826-D749-4291-8A87-267EDDD765E7}

    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool_Ex2,
	0xa7d2e826, 0xd749, 0x4291, 0x8a, 0x87, 0x26, 0x7e, 0xdd, 0xd7, 0x65, 0xe7
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex2, BroadcastExpression	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex2, BroadcastURL , 1);
}
