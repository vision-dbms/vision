#ifndef Vca_Registry_IUpdater_Interface
#define Vca_Registry_IUpdater_Interface

#ifndef Vca_Registry_IUpdater
#define Vca_Registry_IUpdater extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry_IContact.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IUpdater)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IUpdater
	 *
	 *  An interface used to receive registration and identity information
	 *  from implementations of IUpdate::Insert.
	 *
	 */
	Vca_Registry_INTERFACE (IUpdater, IClient);
	    /**
	     * Receive the registration and identity for register object request.
	     *
	     * @param p1    A pointer to the IContact interface associated
	     *              with the inserted object's registry entry.
	     * @param p2    The UUID of the registry entry.  Redeemable for the
	     *              associated object using IRegistryQuery::GetObject.
	     *
	     */
	    VINTERFACE_METHOD_2 (OnContact,IContact*,uuid_t const&);
	VINTERFACE_END

	VINTERFACE_ROLE (IUpdater, IClient)
	    VINTERFACE_ROLE_2 (OnContact,IContact*,uuid_t const&);
	VINTERFACE_ROLE_END
    }
}


#endif
