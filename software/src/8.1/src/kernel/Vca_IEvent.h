#ifndef Vca_IEvent_Interface
#define Vca_IEvent_Interface

#ifndef Vca_IEvent
#define Vca_IEvent extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IEvent)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/**
 * @class Vca::IEvent
 *
 * Interface representing an event in a sequence of events.  Implementors should consider supporting
 * additional interfaces - possibly one or more that extend this interface - to provide more specific
 * information about the event.
 */
    VcaINTERFACE (IEvent, IVUnknown)
    /**
     *  Request a brief description of the event.
     *
     *  @param p1 The receiver to which the requested description should be sent.
     */
	VINTERFACE_METHOD_1 (GetDescription, IVReceiver<VString const&>*);

    /**
     *  Request the event's time of occurrence.
     *
     *  @param p1 The receiver to which the requested timestamp will be sent.
     */
	VINTERFACE_METHOD_1 (GetTimestamp, IVReceiver<V::VTime const&>*);

    /**
     *  Request the activity's SSID (Site-Specific-ID).
     *
     *  @param p1 The receiver to which the requested IEvent will be sent.
     */
	VINTERFACE_METHOD_1 (GetSSID, IVReceiver<ssid_t>*);
    VINTERFACE_END

    VINTERFACE_ROLE (IEvent, IVUnknown)
	VINTERFACE_ROLE_1 (GetDescription, IVReceiver<VString const&>*);
	VINTERFACE_ROLE_1 (GetTimestamp, IVReceiver<V::VTime const&>*);
	VINTERFACE_ROLE_1 (GetSSID, IVReceiver<ssid_t>*);
    VINTERFACE_ROLE_END
}


#endif
