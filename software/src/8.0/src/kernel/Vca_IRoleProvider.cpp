/*****  Vca_IRoleProvider Implementation  *****/

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

#define   Vca_IRoleProvider
#include "Vca_IRoleProvider.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::IRoleProvider  *****
 *****                      *****
 ********************************
 ********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IRoleProvider)

namespace Vca {
// {494C7469-C761-4a6c-B434-6E74B3231452}
    VINTERFACE_TYPEINFO_DEFINITION (
	IRoleProvider,
	0x494c7469, 0xc761, 0x4a6c, 0xb4, 0x34, 0x6e, 0x74, 0xb3, 0x23, 0x14, 0x52
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IRoleProvider, QueryRole, 0);
}
