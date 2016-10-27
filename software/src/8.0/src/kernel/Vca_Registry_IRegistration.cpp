/*****  Vca_Registry_IRegistration Implementation  *****/

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

#define   Vca_Registry_IRegistration
#include "Vca_Registry_IRegistration.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::IRegistration  *****
 *****                      *****
 ********************************
 ********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IRegistration)

namespace Vca {
    namespace Registry {
    // {46ABA7BD-35BB-4173-928C-2319948AE7DF}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IRegistration,
	    0x46aba7bd, 0x35bb, 0x4173, 0x92, 0x8c, 0x23, 0x19, 0x94, 0x8a, 0xe7, 0xdf
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IRegistration, Revoke, 0);
    }  // namespace Registry
} // namespace Vca
