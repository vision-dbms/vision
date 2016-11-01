#ifndef Vca_Registry_IContact_Interface
#define Vca_Registry_IContact_Interface

#ifndef Vca_Registry_IContact
#define Vca_Registry_IContact extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IRequest.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IContact)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IContact
	 *
	 *  An interface intended for use identifying and manipulating the
	 *  registration of a specific object in an object registry.
	 *  Implemented by registry implementors, used by code that adds
	 *  objects to a registry.
	 */
	Vca_Registry_INTERFACE (IContact, IRequest);
	    /**
	    * Used to retrieve the UUID for this passive call.
	    *
	    * @param p1 the receiver by which the UUID should be returned.
	    */
	    VINTERFACE_METHOD_1 (GetUUID, IVReceiver<uuid_t const&>*);
	VINTERFACE_END

	VINTERFACE_ROLE (IContact, IRequest)
	    VINTERFACE_ROLE_1 (GetUUID, IVReceiver<uuid_t const&>*);
	VINTERFACE_ROLE_END
    }
}


#endif
