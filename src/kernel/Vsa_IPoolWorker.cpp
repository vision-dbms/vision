/*****  Vsa_IPoolWorker Implementation  *****/
#define Vsa_IPoolWorker

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

#include "Vsa_IPoolWorker.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************
 ******************************
 *****	   	          *****
 *****  Vsa::IPoolWorker  *****
 *****			  *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolWorker)

namespace Vsa {
// {18276de0-10c7-4c05-a2ef-cb7fff1fb37f}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolWorker,
	0x18276de0, 0x10c7, 0x4c05, 0xa2, 0xef, 0xcb, 0x7f, 0xff, 0x1f, 0xb3, 0x7f
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPoolWorker, GetStatistics, 0);
}
