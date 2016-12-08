/*****  Vxa_IVSNFTaskImplementation Implementation  *****/

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

#define   Vxa_IVSNFTaskImplementation
#include "Vxa_IVSNFTaskImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

/******************************************
 ******************************************
 *****                                *****
 *****  Vxa::IVSNFTaskImplementation  *****
 *****                                *****
 ******************************************
 ******************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::IVSNFTaskImplementation)

namespace Vxa {
    // {DB2CE47B-0C42-42a1-8F51-550EB8278638}
    VINTERFACE_TYPEINFO_DEFINITION (
	IVSNFTaskImplementation, 0xdb2ce47b, 0xc42, 0x42a1, 0x8f, 0x51, 0x55, 0xe, 0xb8, 0x27, 0x86, 0x38
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskImplementation, SetToInteger, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskImplementation, SetToDouble, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskImplementation, SetToVString, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskImplementation, SetToObjects, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IVSNFTaskImplementation, PopulateVariantCompleted, 5);
}
