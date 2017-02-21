#ifndef Vsa_ISubscription_Interface
#define Vsa_ISubscription_Interface

#ifndef Vsa_ISubscription
#define Vsa_ISubscription extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,ISubscription)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    Unsubscribe ();

    Synopsis:
	Unsubscribes the Subscriber pertaining to this ISubscription token
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (ISubscription, IVUnknown)
	VINTERFACE_ROLE_0 (Unsubscribe);
    VINTERFACE_ROLE_END

    VsaINTERFACE (ISubscription, IVUnknown);
	VINTERFACE_METHOD_0 (Unsubscribe);
    VINTERFACE_END
}


#endif
