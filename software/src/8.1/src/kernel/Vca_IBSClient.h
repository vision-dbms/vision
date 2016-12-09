#ifndef Vca_IBSClient_Interface
#define Vca_IBSClient_Interface

#ifndef Vca_IBSClient
#define Vca_IBSClient extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBSClient)

namespace Vca {
    VINTERFACE_ROLE (IBSClient, IClient)
	VINTERFACE_ROLE_0 (OnEnd);
	VINTERFACE_ROLE_1 (OnTransfer, U32);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBSClient, IClient)
	VINTERFACE_METHOD_0 (OnEnd);
	VINTERFACE_METHOD_1 (OnTransfer, U32);
    VINTERFACE_END
}

#endif
