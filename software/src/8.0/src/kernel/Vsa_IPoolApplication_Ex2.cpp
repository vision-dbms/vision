/*****  Vsa_IPoolApplication_Ex2 Implementation  *****/
#define Vsa_IPoolApplication_Ex2

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

#include "Vsa_IPoolApplication_Ex2.h"

/************************
 *****  Supporting  *****
 ************************/


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::IPoolApplication_Ex2  *****
 *****                             *****
 ***************************************
 ***************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolApplication_Ex2)

namespace Vsa {
    // {7C81F8AE-E38B-49D7-95A2-43A89011FC60}

    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolApplication_Ex2,
	0x7c81f8ae, 0xe38b, 0x49d7, 0x95, 0xa2, 0x43, 0xa8, 0x90, 0x11, 0xfc, 0x60
    );
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication_Ex2, HardStop, 0);
}
