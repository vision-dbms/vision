/*****  Vsa_IEvaluatorPump Implementation  *****/
#define Vsa_IEvaluatorPump

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

#include "Vsa_IEvaluatorPump.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************
 *********************************
 *****                       *****
 *****  Vsa::IEvaluatorPump  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPump)

namespace Vsa {
// {DCE80E8D-797C-46f8-A50D-D467F5BB0ED2}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPump,
	0xdce80e8d, 0x797c, 0x46f8, 0xa5, 0xd, 0xd4, 0x67, 0xf5, 0xbb, 0xe, 0xd2
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPump, GetSettings, 0);
}
