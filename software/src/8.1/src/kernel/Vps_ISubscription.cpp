/*****  Vps_ISubscription Implementation  *****/
#define Vps_ISubscription

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

#include "Vps_ISubscription.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****			         *****
 *****  Vca::Vps::ISubscription  *****
 *****				 *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::ISubscription)

namespace Vca {
    namespace Vps {

    // {E2D4A532-E4B2-480C-9A54-EFE209A74EA4}
	VINTERFACE_TYPEINFO_DEFINITION (
	    ISubscription,
	    0xe2d4a532, 0xe4b2, 0x480c, 0x9a, 0x54, 0xef, 0xe2, 0x09, 0xa7, 0x4e, 0xa4
	);

	VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Suspend, 0);
	VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Resume, 1);
	VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Cancel, 2);
    }
}
