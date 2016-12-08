/**
 * @file
 * Provides IGetter_Ex2 interface declaration.
 */
#ifndef Vca_IGetter_Ex2_Interface
#define Vca_IGetter_Ex2_Interface

#ifndef Vca_IGetter_Ex2
#define Vca_IGetter_Ex2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IGetter.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IGetter_Ex2)

namespace Vca {
    /**
     * @class Vca::IGetter_Ex2
     * @copydoc Vca::IGetter
     */
    VcaINTERFACE (IGetter_Ex2, IGetter);
        /**
         * Retrieves a U64 value given a key.
         * @copydetail GetStringValue()
         */
	VINTERFACE_METHOD_2 (GetU64Value, IVReceiver<U64>*, VString const &);

        /**
         * Retrieves a F32 value given a key.
         * @copydetail GetStringValue()
         */
	VINTERFACE_METHOD_2 (GetF32Value, IVReceiver<F32>*, VString const &);

        /**
         * Retrieves a F64 value given a key.
         * @copydetail GetStringValue()
         */
	VINTERFACE_METHOD_2 (GetF64Value, IVReceiver<F64>*, VString const &);
    VINTERFACE_END

    VINTERFACE_ROLE (IGetter_Ex2, IGetter)
	VINTERFACE_ROLE_2 (GetU64Value, IVReceiver<U64>*, VString const &);
	VINTERFACE_ROLE_2 (GetF32Value, IVReceiver<F32>*, VString const &);
	VINTERFACE_ROLE_2 (GetF64Value, IVReceiver<F64>*, VString const &);
    VINTERFACE_ROLE_END
}

#endif
