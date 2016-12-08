/*****  Vsa_IEvaluatorSettings Implementation  *****/
#define Vsa_IEvaluatorSettings

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

#include "Vsa_IEvaluatorSettings.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorSettings  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorSettings)

namespace Vsa {
    // {F66795DB-6996-43e9-A3E7-E52891CB9DB3}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorSettings,
	0xf66795db, 0x6996, 0x43e9, 0xa3, 0xe7, 0xe5, 0x28, 0x91, 0xcb, 0x9d, 0xb3
    );
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorSettings, GetEvaluationTimeOut, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorSettings, SetEvaluationTimeOut, 1);

//  VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorSettings, Foo, 0);
}
