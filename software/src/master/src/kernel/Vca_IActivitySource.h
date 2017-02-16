#ifndef Vca_IActivitySource_Interface
#define Vca_IActivitySource_Interface

#ifndef Vca_IActivitySource
#define Vca_IActivitySource extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IActivitySink.h"
#include "Vca_IRequest.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IActivitySource)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/**
 * @class IActivitySource
 *
 * Interface representing an activity source.  Provides an opportunity to subscribe to an activity stream.
 */
    VcaINTERFACE (IActivitySource, IVUnknown)
	/**
	 *  Subscribe to the activity stream.
	 *
	 *  @param p1 The receiver to which a ticket (IRequest*) for the this subscription (request) will be sent.
	 *            When supported, permits cancelation of the subscription.
	 *  @param p2 The IActivitySink to which activities will be sent.
	 */
	VINTERFACE_METHOD_2 (SupplyActivities,IVReceiver<IRequest*>*,IActivitySink*);
    VINTERFACE_END

    VINTERFACE_ROLE (IActivitySource, IVUnknown)
	VINTERFACE_ROLE_2 (SupplyActivities,IVReceiver<IRequest*>*,IActivitySink*);
    VINTERFACE_ROLE_END
}


#endif
