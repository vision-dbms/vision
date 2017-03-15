/*****  Vca_INeedYou Implementation  *****/

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

#define   Vca_INeedYou
#include "Vca_INeedYou.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::INeedYou  *****
 *****                 *****
 ***************************
 ***************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::INeedYou)

namespace Vca {
// {C8FA0746-8B09-42c6-AA59-3DB85B149F7B}
    VINTERFACE_TYPEINFO_DEFINITION (
	INeedYou,
	0xc8fa0746, 0x8b09, 0x42c6, 0xaa, 0x59, 0x3d, 0xb8, 0x5b, 0x14, 0x9f, 0x7b
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (INeedYou, PleaseCall, 0);
}
