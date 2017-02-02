/*****  Vca_Registry_IAuthority Implementation  *****/

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

#define   Vca_Registry_IAuthority
#include "Vca_Registry_IAuthority.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****                   *****
 *****  Vca::IAuthority  *****
 *****                   *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IAuthority)

namespace Vca {
    namespace Registry {
    // {7EECE73C-CDB6-4e79-A3B1-DCDDD37087B3}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IAuthority,
	    0x7eece73c, 0xcdb6, 0x4e79, 0xa3, 0xb1, 0xdc, 0xdd, 0xd3, 0x70, 0x87, 0xb3
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IAuthority, SupplyEvents, 0);
    } // namespace Registry
} // namespace Vca
