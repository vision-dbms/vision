#ifndef Vps_ISubscription_Interface
#define Vps_ISubscription_Interface

#ifndef Vps_ISubscription
#define Vps_ISubscription extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,ISubscription)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    Cancel ();
    Synopsis:
	Cancels the subscription to this ISubscription token

    Suspend ();
    Synopsis:
	Suspends the subscription.

    Resume ();
    Synopsis:
	Resumes the subscription.

 *==========================================================================*/

namespace Vca {
    namespace Vps {
	VINTERFACE_ROLE (ISubscription, IVUnknown)
	    VINTERFACE_ROLE_0 (Suspend);
	    VINTERFACE_ROLE_0 (Resume);
	    VINTERFACE_ROLE_0 (Cancel);
	VINTERFACE_ROLE_END

	VpsINTERFACE (ISubscription, IVUnknown);
	    VINTERFACE_METHOD_0 (Suspend);
	    VINTERFACE_METHOD_0 (Resume);
	    VINTERFACE_METHOD_0 (Cancel);
	VINTERFACE_END
    }
}


#endif
