#ifndef Vca_IAckReceiver_Interface
#define Vca_IAckReceiver_Interface

#ifndef Vca_IAckReceiver
#define Vca_IAckReceiver extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IAckReceiver)

namespace Vca {
    VINTERFACE_ROLE (IAckReceiver, IClient)
	VINTERFACE_ROLE_0 (Ack);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IAckReceiver, IClient);
	VINTERFACE_METHOD_0 (Ack);
    VINTERFACE_END
}


#endif
