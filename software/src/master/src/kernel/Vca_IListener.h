#ifndef Vca_IListener_Interface
#define Vca_IListener_Interface

#ifndef Vca_IListener
#define Vca_IListener extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IBinding.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IListener)

#ifndef Vca_IListenerClient
DECLARE_VcaINTERFACE (IListenerClient)
#endif

#ifndef Vca_IPipeSourceClient
DECLARE_VcaINTERFACE (IPipeSourceClient)
#endif

namespace Vca {
    VINTERFACE_ROLE (IListener, IBinding)
	VINTERFACE_ROLE_2 (SupplyConnections, IListenerClient*, U32);
	VINTERFACE_ROLE_2 (SupplyByteStreams, IPipeSourceClient*, U32);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IListener, IBinding);
	VINTERFACE_METHOD_2 (SupplyConnections, IListenerClient*, U32);
	VINTERFACE_METHOD_2 (SupplyByteStreams, IPipeSourceClient*, U32);
    VINTERFACE_END
}

#endif
