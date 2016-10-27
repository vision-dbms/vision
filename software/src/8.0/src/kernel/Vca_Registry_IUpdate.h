#ifndef Vca_Registry_IUpdate_Interface
#define Vca_Registry_IUpdate_Interface

#ifndef Vca_Registry_IUpdate
#define Vca_Registry_IUpdate extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry_IUpdater.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IUpdate)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IUpdate
	 *
	 * An interface for obtaining an interface to a registered object.
	 */
	Vca_Registry_INTERFACE (IUpdate, IVUnknown);
	    /**
	     * Obtain a pointer to an interface of a registered object.
	     *
	     * @param p1    A pointer to the IUpdater interface that should receive
	     *              the registration and identity information for the newly
	     *              registered object.
	     * @param p2    A pointer to an interface of the object being registered.
	     *
	     */
	    VINTERFACE_METHOD_2 (AddObject,IUpdater*,IVUnknown*);
	VINTERFACE_END

	VINTERFACE_ROLE (IUpdate, IVUnknown)
	    VINTERFACE_ROLE_2 (AddObject,IUpdater*,IVUnknown*);
	VINTERFACE_ROLE_END
    }
}


#endif
