/*****  Vxa_ICaller2 Implementation  *****/

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

#define   Vxa_ICaller2
#include "Vxa_ICaller2.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  Vxa::ICaller2  *****
 *****                *****
 **************************
 **************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::ICaller2)

namespace Vxa {
// {D87C5A8A-5FF1-45ec-849B-A95E5A924CCB}
    VINTERFACE_TYPEINFO_DEFINITION (
	ICaller2,
        0x3a169023, 0x0143, 0x4916, 0xb5, 0xb4, 0xa1, 0x6e, 0xcf, 0x31, 0x48, 0x49
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller2, Suspensions, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller2, Suspend    , 1);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller2, Resume     , 2);
}
