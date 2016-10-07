/*****  Vsa_IEvaluatorPool Implementation  *****/
#define Vsa_IEvaluatorPool

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

#include "Vsa_IEvaluatorPool.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************
 *********************************
 *****                       *****
 *****  Vsa::IEvaluatorPool  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool)

namespace Vsa {
// {FF386D5A-2FA4-4529-A16B-1C5DA7710B66}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool,
	0xff386d5a, 0x2fa4, 0x4529, 0xa1, 0x6b, 0x1c, 0x5d, 0xa7, 0x71, 0xb, 0x66
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool, GetSettings	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool, AddEvaluator	, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool, FlushWorkers	, 2);
}
