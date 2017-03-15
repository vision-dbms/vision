/*****  Vca_Registry_IRegistry Implementation  *****/

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

#define   Vca_Registry_IRegistry
#include "Vca_Registry_IRegistry.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****                   *****
 *****  Vca::IRegistry  *****
 *****                   *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IRegistry)

namespace Vca {
    namespace Registry {
    // {3AD41662-62E7-41ed-8341-D002617B3AB9}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IRegistry,
	    0x3ad41662, 0x62e7, 0x41ed, 0x83, 0x41, 0xd0, 0x2, 0x61, 0x7b, 0x3a, 0xb9
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IRegistry, RegisterObject, 0);
    } // namespace Registry
} // namespace Vca
