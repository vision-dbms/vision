/*****  Vca_Registry_IContact Implementation  *****/

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

#define   Vca_Registry_IContact
#include "Vca_Registry_IContact.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::Registry::IContact  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IContact)

namespace Vca {
    namespace Registry {
    // {47544A6D-DD02-48d1-9214-78C3F8539D36}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IContact,
	    0x47544a6d, 0xdd02, 0x48d1, 0x92, 0x14, 0x78, 0xc3, 0xf8, 0x53, 0x9d, 0x36
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IContact, GetUUID, 0);
    } // namespace Registry
} // namespace Vca
