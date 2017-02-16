#ifndef Vca_ITrigger_Interface
#define Vca_ITrigger_Interface

#ifndef Vca_ITrigger
#define Vca_ITrigger extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,ITrigger)

namespace Vca {
    VINTERFACE_ROLE (ITrigger, IVUnknown)
	VINTERFACE_ROLE_0 (Process);
    VINTERFACE_ROLE_END

    VcaINTERFACE (ITrigger, IVUnknown);
	VINTERFACE_METHOD_0 (Process);
    VINTERFACE_END
}


#endif
