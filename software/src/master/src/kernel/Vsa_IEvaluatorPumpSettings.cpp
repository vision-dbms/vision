/*****  Vsa_IEvaluatorPumpSettings Implementation  *****/
#define Vsa_IEvaluatorPumpSettings

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

#include "Vsa_IEvaluatorPumpSettings.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************************
 *****************************************
 *****                               *****
 *****  Vsa::IEvaluatorPumpSettings  *****
 *****                               *****
 *****************************************
 *****************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPumpSettings)

namespace Vsa {
    // {C22D4AB1-1439-4718-9873-8ECB40CA4D2D}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPumpSettings,
	0xc22d4ab1, 0x1439, 0x4718, 0x98, 0x73, 0x8e, 0xcb, 0x40, 0xca, 0x4d, 0x2d
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, MakeControl	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, MakeSource	, 1);

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, GetURLClass	, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, SetURLClass	, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, GetURLTemplate	, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, SetURLTemplate	, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, GetUsingExtendedPrompts, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPumpSettings, SetUsingExtendedPrompts, 7);
}
