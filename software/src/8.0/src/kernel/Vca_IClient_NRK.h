/**
 * @file
 * Declares the IClient interface, a special case to work around BUILDER dependency generation bug(s).
 */
#ifndef Vca_IClient_NRK
#define Vca_IClient_NRK

#ifndef Vca_IClient
#define Vca_IClient extern
#endif

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

// Special case.
#ifndef Vca_IError
DECLARE_VcaINTERFACE (IError)
#endif

namespace Vca {
    VINTERFACE_ROLE (IClient, IVUnknown)
	VINTERFACE_ROLE_0 (OnEnd);
	VINTERFACE_ROLE_2 (OnError, IError*, VString const&);
    VINTERFACE_ROLE_END

    /**
     * @class Vca::IClient
     * @extends IVUnknown
     * Interface for clients to a particular service, offering notification capabilities for when the service has ended or errored.
     */

    VcaINTERFACE (IClient, IVUnknown);
        /**
         * Used to signal that this client's server will send no more results to this client.
         */
	VINTERFACE_METHOD_0 (OnEnd);

        /**
         * Returns an error associated with an operation.
	 * This member is a work in progress.
         *
         * @param[in] p1 currently always set to @c null, may be used in the future.
         * @param[in] p2 the error message.
         */
	VINTERFACE_METHOD_2 (OnError, IError*, VString const&);
    VINTERFACE_END
}

/***********************************
 *****  IVReceiver Definition  *****
 ***********************************
 *----------------------------------------------------------------------------*
 *  See the notes with the include of "Vca_IClient.h" in "IVUnknown.h".
 *----------------------------------------------------------------------------*/

//  Bugs in BUILDER' dependency analysis require the include of a non-recursive kernel
#include "IVReceiver_NRK.h"

#endif
