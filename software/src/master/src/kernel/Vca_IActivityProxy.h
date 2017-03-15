#ifndef Vca_IActivityProxy_Interface
#define Vca_IActivityProxy_Interface

#ifndef Vca_IActivityProxy
#define Vca_IActivityProxy extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IActivity.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IActivityProxy)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/**
 * @class IActivityProxy
 *
 * Interface representing a proxy for an activity.  Extends IActivity with a single message that accesses the
 * original activity.
 */
    VcaINTERFACE (IActivityProxy, IActivity)
	/**
	 *  Subscribe to the activity's events.
	 *
	 *  @param p1 The receiver to which a ticket (IRequest*) for the this subscription (request) will be sent.
	 *            When supported, permits cancelation of the subscription.
	 *  @param p2 The IEventSink to which events will be sent.
	 */
//	VINTERFACE_METHOD_1 (GetMaster,IVReceiver<IActivity*>*);
    VINTERFACE_END

    VINTERFACE_ROLE (IActivityProxy, IActivity)
//	VINTERFACE_ROLE_1 (GetMaster,IVReceiver<IActivity*>*);
    VINTERFACE_ROLE_END
}


#endif
