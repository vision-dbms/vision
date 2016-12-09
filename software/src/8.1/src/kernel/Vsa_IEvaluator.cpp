/*****  Vsa_IEvaluator Implementation  *****/
#define Vsa_IEvaluator

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

#include "Vsa_IEvaluator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****  Vsa::IEvaluator  *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS_AT_LEVEL (Vsa::IEvaluator,2)

namespace Vsa {
    // {C9FB74C9-C8CE-4e30-A52C-54D07F3B86C1}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluator,
	0xc9fb74c9, 0xc8ce, 0x4e30, 0xa5, 0x2c, 0x54, 0xd0, 0x7f, 0x3b, 0x86, 0xc1
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluator, EvaluateExpression	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluator, EvaluateURL		, 1);
}
