#ifndef Vxa_ICaller2_Interface
#define Vxa_ICaller2_Interface

#ifndef Vxa_ICaller2
#define Vxa_ICaller2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_ICaller.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (ICaller2, ICaller)

	VINTERFACE_ROLE_1 (Suspensions, IVReceiver<Vxa::cardinality_t>*);

	VINTERFACE_ROLE_0 (Suspend);
	VINTERFACE_ROLE_0 (Resume);

    VINTERFACE_ROLE_END

    VxaINTERFACE (ICaller2, ICaller);

	VINTERFACE_METHOD_1 (Suspensions, IVReceiver<Vxa::cardinality_t>*);

	VINTERFACE_METHOD_0 (Suspend);
	VINTERFACE_METHOD_0 (Resume);

    VINTERFACE_END
}


#endif
