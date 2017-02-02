/*****  Vsa_IEvaluationResult Implementation  *****/
#define Vsa_IEvaluationResult

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

#include "Vsa_IEvaluationResult.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vsa::IEvaluationResult  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluationResult)

namespace Vsa {
    // {E5D397C4-34C7-44fd-830C-5432C1106959}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluationResult,
	0xe5d397c4, 0x34c7, 0x44fd, 0x83, 0xc, 0x54, 0x32, 0xc1, 0x10, 0x69, 0x59
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluationResult, GetTimingReport, 0);
}
