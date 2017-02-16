#ifndef Vca_Registry_IAuthority_Interface
#define Vca_Registry_IAuthority_Interface

#ifndef Vca_Registry_IAuthority
#define Vca_Registry_IAuthority extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IRequest.h"
#include "Vca_Registry_IRegistration.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IAuthority)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	typedef IRequest IRegistrationTicket;
	typedef IVReceiver<IRegistrationTicket*> IRegistrationTicketSink;

	/**
	 * @class IAuthority
	 *
	 *  An interface used to receive registrar and identity information
	 *  from implementations of IUpdate::Insert.
	 *
	 */
	Vca_Registry_INTERFACE (IAuthority, IClient);
	    /**
	     * Subscribe to the stream of events pertinent to a registration (e.g., revocation).
	     *
	     * @param p1    A pointer to a receiver for the subscription ticket (request) for
	     *              the stream.
	     * @param p2    A pointer to the registration to which the events should be sent.
	     */
	    VINTERFACE_METHOD_2 (SupplyEvents,IRegistrationTicketSink*,IRegistration*);
	VINTERFACE_END

	VINTERFACE_ROLE (IAuthority, IClient)
	    VINTERFACE_ROLE_2 (SupplyEvents,IRegistrationTicketSink*,IRegistration*);
	VINTERFACE_ROLE_END
    } // namespace Registry
} // namespace Vca


#endif
