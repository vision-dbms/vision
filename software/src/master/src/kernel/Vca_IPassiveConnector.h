#ifndef Vca_IPassiveConnector_Interface
#define Vca_IPassiveConnector_Interface

#ifndef Vca_IPassiveConnector
#define Vca_IPassiveConnector extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPassiveConnector)

DECLARE_VcaINTERFACE (IPassiveCallback)
DECLARE_VcaINTERFACE (IPassiveServer)

/**
 * @class Vca::IPassiveConnector
 * Used by passive clients to initiate a callback request.
 *
 * Typically, consumers will not need to write implementations of this interface; it's already got a well fleshed-out implementation in Vca::VPassiveConnector.
 */

namespace Vca {

    /** Convenience definition for a receiver for the Vca::IPassiveCallback from the server. */
    typedef IVReceiver<IPassiveCallback*> IPassiveCallbackReceiver;

    VINTERFACE_ROLE (IPassiveConnector, IVUnknown)
	VINTERFACE_ROLE_3 (RequestCallback, IPassiveCallbackReceiver*, IPassiveServer*, U64);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPassiveConnector, IVUnknown);
        /**
         * Used by passive clients to initiate a callback request.
         *
         * @param p1 The client-side receiver for the callback (Vca::IPassiveCallback) expected from the server.
         * @param p2 The server interface (or its local surrogate) that the implementor of this interface should use to request the callback.
         * @param p3 The number of microseconds to wait before the call should time out.
         */
	VINTERFACE_METHOD_3 (RequestCallback, IPassiveCallbackReceiver*, IPassiveServer*, U64);
    VINTERFACE_END
}

#endif
