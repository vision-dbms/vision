#ifndef Vca_IPeek_Interface
#define Vca_IPeek_Interface

#ifndef Vca_IPeek
#define Vca_IPeek extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IRequest.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPeek)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/**
 * @class IPeek
 *
 * Interface used to inspect and subscribe to the properties of an object.  Implemented by VRolePlayer.
 */
    VcaINTERFACE (IPeek, IVUnknown)
	/**
	 *  Obtain the value of an object property.
	 *
	 *  @param p1 The name of the property to examine.
	 *  @param p2 The IClient whose specialization will receive the current value of the
	 *            requested property.
	 *  @param p3 An optional receiver an IRequest ticket.  If the property identified by
	 *            parameter 1 supports update tracking and this parameter's value is non-
	 *            null, a ticket for an update tracking subscription will be returned via
	 *            this parameter and the IClient identified by parameter 2 will receive
	 *            an 'OnData' message for each subsequent change to the property's value.
	 *            If the requested property does not support update tracking and this
	 *            parameter is specified, a null will be passed as the value the tracking
	 *            returned via this receiver to indicate that a tracking subscription was
	 *            not created.
	 */
	VINTERFACE_METHOD_3 (GetValue,VString const&,IClient*,IVReceiver<IRequest*>*);
    VINTERFACE_END

    VINTERFACE_ROLE (IPeek, IVUnknown)
	VINTERFACE_ROLE_3 (GetValue,VString const&,IClient*,IVReceiver<IRequest*>*);
    VINTERFACE_ROLE_END
}


#endif
