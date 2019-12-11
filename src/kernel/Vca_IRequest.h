#ifndef Vca_IRequest_Interface
#define Vca_IRequest_Interface

#ifndef Vca_IRequest
#define Vca_IRequest extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IRequest)

/**
 * @class Vca::IRequest
 *
 * Interface optionally supported by implementors of subscriptions or activities, allowing for consumers to cancel said subscription or activity.
 */
namespace Vca {
    VINTERFACE_ROLE (IRequest, IVUnknown)
	VINTERFACE_ROLE_0 (Cancel);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IRequest, IVUnknown);
        /**
         * Cancels the notifications associated with this subscription or activity.
         */
	VINTERFACE_METHOD_0 (Cancel);
    VINTERFACE_END
}


#endif
