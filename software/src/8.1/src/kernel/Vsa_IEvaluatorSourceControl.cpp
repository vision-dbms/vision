/*****  Vsa_IEvaluatorSourceControl Implementation  *****/
#define Vsa_IEvaluatorSourceControl

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

#include "Vsa_IEvaluatorSourceControl.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************************
 ******************************************
 *****				      *****
 *****  Vsa::IEvaluatorSourceControl  *****
 *****				      *****
 ******************************************
 ******************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorSourceControl)

namespace Vsa {
// {0b52b08a-3fce-45e5-9763-f7ce3e4ad73a}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorSourceControl,
	0x0b52b08a, 0x3fce, 0x45e5, 0x97, 0x63, 0xf7, 0xce, 0x3e, 0x4a, 0xd7, 0x3a
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorSourceControl, GetStartupQuery, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorSourceControl, SetStartupQuery, 1);
}


