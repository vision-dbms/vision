#ifndef Vca_IActivity_Interface
#define Vca_IActivity_Interface

#ifndef Vca_IActivity
#define Vca_IActivity extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IEventSink.h"
#include "Vca_IRequest.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IActivity)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/**
 * @class IActivity
 *
 * Interface representing an activity.  Provides access to basic information about the activity.  Implementors should
 * consider supporting additional interfaces - possibly one or more that extend this interface - to provide specific
 * information and controls pertinent to the activity represented by this interface.
 */
    VcaINTERFACE (IActivity, IVUnknown)
	/**
	 *  Subscribe to the activity's events.
	 *
	 *  @param p1 The receiver to which a ticket (IRequest*) for the this subscription (request) will be sent.
	 *            When supported, permits cancelation of the subscription.
	 *  @param p2 The IEventSink to which events will be sent.
	 */
	VINTERFACE_METHOD_2 (SupplyEvents,IVReceiver<IRequest*>*,IEventSink*);

	/**
	 *  Subscribe to the activity's events.
	 *
	 *  @param p1 The receiver to which a ticket (IRequest*) for the this subscription (request) will be sent.
	 *            When supported, permits cancelation of the subscription.
	 *  @param p2 The IClient to which end/err notifications will be sent.
	 */
	VINTERFACE_METHOD_2 (SupplyEndErr,IVReceiver<IRequest*>*,IEventSink*);

	/**
	 *  Request the activity's SSID (Site-Specific-ID).
	 *
	 *  @param p1 The receiver to which the requested IEvent will be sent.
	 */
	VINTERFACE_METHOD_1 (GetSSID, IVReceiver<ssid_t>*);

	/**
	 *  Request the activity's parent IActivity.
	 *
	 *  @param p1 The receiver to which the requested IActivity will be sent.
	 */
	VINTERFACE_METHOD_1 (GetParent, IVReceiver<IActivity*>*);

	/**
	 *  Request the activity's parent IActivity.
	 *
	 *  @param p1 The receiver to which the requested IActivity will be sent.
	 */
	VINTERFACE_METHOD_1 (GetDescription, IVReceiver<VString const&>*);
    VINTERFACE_END

    VINTERFACE_ROLE (IActivity, IVUnknown)
	VINTERFACE_ROLE_2 (SupplyEvents,IVReceiver<IRequest*>*,IEventSink*);
	VINTERFACE_ROLE_2 (SupplyEndErr,IVReceiver<IRequest*>*,IEventSink*);

	VINTERFACE_ROLE_1 (GetIdentity, IVReceiver<ssid_t>*);
	VINTERFACE_ROLE_1 (GetParent, IVReceiver<IActivity*>*);
	VINTERFACE_ROLE_1 (GetDescription, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END
}


#endif
