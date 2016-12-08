/*****  Vsa_IEvaluation Implementation  *****/
#define Vsa_IEvaluation

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

#include "Vsa_IEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************
 ******************************
 *****                    *****
 *****  Vsa::IEvaluation  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluation)

namespace Vsa {
    // {A01431A3-989F-40fe-9499-4D30F3D873DF}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluation,
	0xa01431a3, 0x989f, 0x40fe, 0x94, 0x99, 0x4d, 0x30, 0xf3, 0xd8, 0x73, 0xdf
    );

//    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluation, Foo, 0);
}
