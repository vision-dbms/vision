/*****  Vxa_IVSNFTaskHolder2 Implementation  *****/

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

#define   Vxa_IVSNFTaskHolder2
#include "Vxa_IVSNFTaskHolder2.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************************
 ***********************************
 *****                         *****
 *****  Vxa::IVSNFTaskHolder2  *****
 *****                         *****
 ***********************************
 ***********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::IVSNFTaskHolder2)

namespace Vxa {
    // {6F826325-33C0-427f-8AEF-99D6DB832C09}
    VINTERFACE_TYPEINFO_DEFINITION (
	IVSNFTaskHolder2, 0x6f826325, 0x33c0, 0x427f, 0x8a, 0xef, 0x99, 0xd6, 0xdb, 0x83, 0x2c, 0x9
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder2, ReturnDouble, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder2, ReturnS2Booleans, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder2, ReturnS2Integers, 2);
}
