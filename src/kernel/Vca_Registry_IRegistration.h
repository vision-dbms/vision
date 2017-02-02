#ifndef Vca_Registry_IRegistration_Interface
#define Vca_Registry_IRegistration_Interface

#ifndef Vca_Registry_IRegistration
#define Vca_Registry_IRegistration extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IRegistration)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IRegistration
	 *
	 *  An interface intended for use identifying and manipulating the
	 *  registration of a specific object in an object registry.
	 *  Implemented by registry implementors, used by code that adds
	 *  objects to a registry.
	 */
	Vca_Registry_INTERFACE (IRegistration, IVUnknown);
	    /**
	     * Used to retrieve the UUID for this passive call.
	     *
	     * @param p1 the receiver by which the UUID should be returned.
	     */
	    VINTERFACE_METHOD_0 (Revoke);
	VINTERFACE_END

	VINTERFACE_ROLE (IRegistration, IVUnknown)
	    VINTERFACE_ROLE_0 (Revoke);
	VINTERFACE_ROLE_END
    } // namespace Registry
} // namespace Vca


#endif
