/*****  Vca_IRoleProvider2 Implementation  *****/

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

#define   Vca_IRoleProvider2
#include "Vca_IRoleProvider2.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IRoleProvider2  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IRoleProvider2)

namespace Vca {
// {7BB22132-4F4A-48bc-B536-AF19CB21D08C}
    VINTERFACE_TYPEINFO_DEFINITION (
	IRoleProvider2,
	0x7bb22132, 0x4f4a, 0x48bc, 0xb5, 0x36, 0xaf, 0x19, 0xcb, 0x21, 0xd0, 0x8c
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IRoleProvider2, QueryRole2, 0);
}
