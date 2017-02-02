/*****  Vsa_IPoolApplication_Ex1 Implementation  *****/
#define Vsa_IPoolApplication_Ex1

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

#include "Vsa_IPoolApplication_Ex1.h"

/************************
 *****  Supporting  *****
 ************************/


/***************************************
 ***************************************
 *****                             *****
 *****  Vsa::IPoolApplication_Ex1  *****
 *****                             *****
 ***************************************
 ***************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolApplication_Ex1)

namespace Vsa {
    // {99736C0F-726B-41DA-9BC7-AC3BECBBA183}

    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolApplication_Ex1,
	0x99736c0f, 0x726b, 0x41da, 0x9b, 0xc7, 0xac, 0x3b, 0xec, 0xbb, 0xa1, 0x83
    );
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication_Ex1, Stop, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication_Ex1, GetPoolWorkerProcessSource, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication_Ex1, SetPoolWorkerProcessSource, 2);
}
