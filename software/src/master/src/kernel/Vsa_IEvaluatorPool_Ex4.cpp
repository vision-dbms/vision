/*****  Vsa_IEvaluatorPool_Ex4 Implementation  *****/
#define Vsa_IEvaluatorPool_Ex4

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

#include "Vsa_IEvaluatorPool_Ex4.h"

/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorPool_Ex4  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool_Ex4)

namespace Vsa {
// {1c401396-e420-4e25-bef1-7d726dd37aec}

    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool_Ex4,
        0x1c401396, 0xe420, 0x4e25, 0xbe, 0xf1, 0x7d, 0x72, 0x6d, 0xd3, 0x7a, 0xec
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex4, QueryDetails, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex4, CancelQuery, 1);
}


