/*****  Vxa_IVSNFTaskHolder Implementation  *****/

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

#define   Vxa_IVSNFTaskHolder
#include "Vxa_IVSNFTaskHolder.h"

/************************
 *****  Supporting  *****
 ************************/


/**********************************
 **********************************
 *****                        *****
 *****  Vxa::IVSNFTaskHolder  *****
 *****                        *****
 **********************************
 **********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::IVSNFTaskHolder)

namespace Vxa {
    // {8BDC13DC-81BF-49f5-959D-08172C94449A}
    VINTERFACE_TYPEINFO_DEFINITION (
	IVSNFTaskHolder, 0x8bdc13dc, 0x81bf, 0x49f5, 0x95, 0x9d, 0x8, 0x17, 0x2c, 0x94, 0x44, 0x9a
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, SetOutput, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnString, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnInteger, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, GetParameter, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, TurnBackSNFTask, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnImplementationHandle, 8);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnIntegerArray, 9);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnDoubleArray, 10);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnStringArray, 11);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnObjectArray, 12);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnFloatArray, 13);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskHolder, ReturnBooleanArray, 14);
}
