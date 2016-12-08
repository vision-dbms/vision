/**
 * @file
 * Provides IGetter interface declaration.
 */
#ifndef Vca_IGetter_Interface
#define Vca_IGetter_Interface

#ifndef Vca_IGetter
#define Vca_IGetter extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

/**
 * @class Vca::IGetter
 *
 * Interface for retrieving values given a specific key.
 *
 * IGetter methods are strictly typed, so you'll need to know the type of the value you want to fetch.
 * This also means that it is possible to return different values for the same key depending on the type of value requested.
 */

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IGetter)

namespace Vca {
    VcaINTERFACE (IGetter, IVUnknown);
        /**
         * Retrieves a string value given a key.
         *
         * @param p1[out] the receiver to which the result will be passed.
         * @param p2[in] the key for which a value is to be fetched.
         */
	VINTERFACE_METHOD_2 (GetStringValue, IVReceiver<VString const &>*, VString const &);

        /**
         * Retrieves a U32 value given a key.
         * @copydetail GetStringValue()
         */
	VINTERFACE_METHOD_2 (GetU32Value, IVReceiver<U32>*, VString const &);
    VINTERFACE_END

    VINTERFACE_ROLE (IGetter, IVUnknown)
	VINTERFACE_ROLE_2 (GetStringValue, IVReceiver<VString const &>*, VString const &);
	VINTERFACE_ROLE_2 (GetU32Value, IVReceiver<U32>*, VString const &);
    VINTERFACE_ROLE_END
}

#endif
