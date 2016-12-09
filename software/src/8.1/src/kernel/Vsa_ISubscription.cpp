/*****  Vsa_ISubscription Implementation  *****/
#define Vsa_ISubscription

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

#include "Vsa_ISubscription.h"

/************************
 *****  Supporting  *****
 ************************/


/********************************
 ********************************
 *****	   	            *****
 *****  Vsa::ISubscription  *****
 *****			    *****
 ********************************
 ********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::ISubscription)

namespace Vsa {
// {18F2CD71-B16B-4982-A22F-B77F9A804E1D}
    VINTERFACE_TYPEINFO_DEFINITION (
	ISubscription,
	0x18f2cd71, 0xb16b, 0x4982, 0xa2, 0x2f, 0xb7, 0x7f, 0x9a, 0x80, 0x4e, 0x1d
    );

    VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Unsubscribe, 0);
}
