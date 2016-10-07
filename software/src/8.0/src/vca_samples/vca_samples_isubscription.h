#ifndef VcaSamples_ISubscription_Interface
#define VcaSamples_ISubscription_Interface

#ifndef VcaSamples_ISubscription
#define VcaSamples_ISubscription extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "vca_samples.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (VcaSamples,ISubscription)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace VcaSamples {
    VINTERFACE_ROLE (ISubscription, IVUnknown)
	VINTERFACE_ROLE_0 (Suspend);
	VINTERFACE_ROLE_0 (Resume);
	VINTERFACE_ROLE_0 (Cancel);
    VINTERFACE_ROLE_END

    VcaSamplesINTERFACE (ISubscription, IVUnknown);
	VINTERFACE_METHOD_0 (Suspend);
	VINTERFACE_METHOD_0 (Resume);
	VINTERFACE_METHOD_0 (Cancel);
    VINTERFACE_END
}


#endif
