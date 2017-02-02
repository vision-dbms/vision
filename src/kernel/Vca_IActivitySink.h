#ifndef Vca_IActivitySink_Interface
#define Vca_IActivitySink_Interface

#ifndef Vca_IActivitySink
#define Vca_IActivitySink extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IActivity.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IActivitySink)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
    * @class Vca::IActivitySink
    *
    * Interface representing an activity receiver.  Provides the receiver with knowledge of the activity's
    * existence and inception.  Implementations of this interface can query the activity for additional
    * information and/or subscribe to various information streams from the activity.
    */
    VcaINTERFACE (IActivitySink, IVUnknown)
	/**
	 *  Receive a presumably new activity.
	 *
	 *  @param p1 The IActivity interface for the original activity.
	 *  @param p2 The IActivity interface for the source of this information.
	 *  @param p3 The start time of the activity.
	 *  @param p4 The expected end time of the activity.
	 *  @param p5 A brief description of the activity.
	 */
	VINTERFACE_METHOD_5 (OnActivity,IActivity*,IActivity*,V::VTime const&,V::VTime const&,VString const&);
    VINTERFACE_END

    VINTERFACE_ROLE (IActivitySink, IVUnknown)
	VINTERFACE_ROLE_5 (OnActivity,IActivity*,IActivity*,V::VTime const&,V::VTime const&,VString const&);
    VINTERFACE_ROLE_END
}


#endif
