/*****  Vsa_IEvaluatorPoolSettings_Ex2 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex2

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

#include "Vsa_IEvaluatorPoolSettings_Ex2.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex2  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex2)

namespace Vsa {
    // {8D4B73FE-C7CA-4C3F-979A-D2D3733A0B65}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex2,
	0x8d4b73fe, 0xc7ca, 0x4c3f, 0x97, 0x9a, 0xd2, 0xd3, 0x73, 0x3a, 0x0b, 0x65
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex2, GetBCEvaluationsInProgressLimit, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex2, SetBCEvaluationsInProgressLimit, 1);
}
