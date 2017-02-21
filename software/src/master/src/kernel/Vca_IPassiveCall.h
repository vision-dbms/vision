#ifndef Vca_IPassiveCall_Interface
#define Vca_IPassiveCall_Interface

#ifndef Vca_IPassiveCall
#define Vca_IPassiveCall extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPassiveCall)

/**
 * @class Vca::IPassiveCall
 *
 * Call metadata interface provided by passive clients (or their agents) to the servers that they're requesting callbacks from.
 *
 * Typically, consumers will not need to write implementations of this interface; it's already got a well fleshed-out implementation in Vca::VPassiveConnector::Call.
 */

namespace Vca {

    VINTERFACE_ROLE (IPassiveCall, IClient)
	VINTERFACE_ROLE_1 (GetUUID, IVReceiver<uuid_t const&>*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPassiveCall, IClient);
        /**
         * Used to retrieve the UUID for this passive call.
         *
         * @param p1 the receiver by which the UUID should be returned.
         */
	VINTERFACE_METHOD_1 (GetUUID, IVReceiver<uuid_t const&>*);
    VINTERFACE_END
}

#endif
