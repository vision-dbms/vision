/*****  Vca_Registry_IAccess Implementation  *****/

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

#define   Vca_Registry_IAccess
#include "Vca_Registry_IAccess.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************************
 ************************************
 *****                          *****
 *****  Vca::Registry::IAccess  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IAccess)

namespace Vca {
    namespace Registry {
    // {E5AE0060-ACBD-4b45-99A4-ACC5C923C8B1}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IAccess,
	    0xe5ae0060, 0xacbd, 0x4b45, 0x99, 0xa4, 0xac, 0xc5, 0xc9, 0x23, 0xc8, 0xb1
        );

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IAccess, GetObject, 0);
    } // namespace Registry
} // namespace Vca
