#ifndef Vca_IPassiveClient_Interface
#define Vca_IPassiveClient_Interface

#ifndef Vca_IPassiveClient
#define Vca_IPassiveClient extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPassiveClient)

/**
 * @class Vca::IPassiveClient
 *
 * Represents a passive client capable of accepting callbacks.
 */

DECLARE_VcaINTERFACE (IPassiveCallback)
DECLARE_VcaINTERFACE (IPassiveServer)

namespace Vca {
    /** Convenience definition for receivers of IPassiveCallback instances. */
    typedef IVReceiver<IPassiveCallback*> IPassiveCallbackSink;

    VINTERFACE_ROLE (IPassiveClient, IVUnknown)
	VINTERFACE_ROLE_2 (AcceptCallback, IPassiveCallback*, uuid_t const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPassiveClient, IVUnknown);
        /**
         * Used in completing a callback; provides an IPassiveCallback (with callback metadata) to the client.
         *
         * @param p1 A pointer to the callback interface.
         * @param p2 The UUID of the returned call.
         */
	VINTERFACE_METHOD_2 (AcceptCallback, IPassiveCallback*, uuid_t const&);
    VINTERFACE_END
}

#endif
