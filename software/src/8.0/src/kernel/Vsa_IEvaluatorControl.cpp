/*****  Vsa_IEvaluatorControl Implementation  *****/
#define Vsa_IEvaluatorControl

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

#include "Vsa_IEvaluatorControl.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vsa::IEvaluatorControl  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorControl)

namespace Vsa {
// {2A17C652-A642-413e-935A-EB26F3050C64}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorControl,
	0x2a17c652, 0xa642, 0x413e, 0x93, 0x5a, 0xeb, 0x26, 0xf3, 0x5, 0xc, 0x64
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, GetEvaluator	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, Suspend	, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, Resume		, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, Restart	, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, GetStatistics	, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, GetWorker	, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorControl, Stop		, 6);
}
