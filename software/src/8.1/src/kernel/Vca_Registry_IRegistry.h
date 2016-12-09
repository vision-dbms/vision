#ifndef Vca_Registry_IRegistry_Interface
#define Vca_Registry_IRegistry_Interface

#ifndef Vca_Registry_IRegistry
#define Vca_Registry_IRegistry extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry_VAuthority.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IRegistry)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IRegistry
	 *
	 * An interface used to register an object with an object registrar.  Implemented by
	 * object directories such as distributed hash tables.
	 *
	 */
	Vca_Registry_INTERFACE (IRegistry, IVUnknown)
	    /**
	     * Register an object.
	     *
	     * @param p1    The authority by which this object is being registered.
	     * @param p2    An interface of the object being registered.
	     */
	    VINTERFACE_METHOD_2 (RegisterObject, VAuthority const&, IVUnknown*);
	VINTERFACE_END

	VINTERFACE_ROLE (IRegistry, IVUnknown)
	    VINTERFACE_ROLE_2 (RegisterObject, VAuthority const&, IVUnknown*);
	VINTERFACE_ROLE_END
    } // namespace Registry
} // namespace Vca


#endif
