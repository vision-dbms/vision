/*****  Vsa_IPoolApplication Implementation  *****/
#define Vsa_IPoolApplication

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

#include "Vsa_IPoolApplication.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************************
 ***********************************
 *****                         *****
 *****  Vsa::IPoolApplication  *****
 *****                         *****
 ***********************************
 ***********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolApplication)

namespace Vsa {
    // {C3C754D9-296F-40d0-A918-9B28468943D0}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolApplication,
	0xc3c754d9, 0x296f, 0x40d0, 0xa9, 0x18, 0x9b, 0x28, 0x46, 0x89, 0x43, 0xd0
    );
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication, Stop, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication, GetPoolWorkerProcessSource, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IPoolApplication, SetPoolWorkerProcessSource, 2);
}
