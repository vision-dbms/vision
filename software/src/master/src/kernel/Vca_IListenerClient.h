#ifndef Vca_IListenerClient_Interface
#define Vca_IListenerClient_Interface

#ifndef Vca_IListenerClient
#define Vca_IListenerClient extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IListenerClient)

#ifndef Vca_IConnection
DECLARE_VcaINTERFACE (IConnection)
#endif

#ifndef Vca_IListener
DECLARE_VcaINTERFACE (IListener)
#endif

namespace Vca {
    VINTERFACE_ROLE (IListenerClient, IClient)
	VINTERFACE_ROLE_1 (OnListener, IListener*);
	VINTERFACE_ROLE_1 (OnConnection, IConnection*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IListenerClient, IClient);
	VINTERFACE_METHOD_1 (OnListener, IListener*);
	VINTERFACE_METHOD_1 (OnConnection, IConnection*);
    VINTERFACE_END
}

#endif
