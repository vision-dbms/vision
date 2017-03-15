#ifndef Vca_IConnectionFactory_Interface
#define Vca_IConnectionFactory_Interface

#ifndef Vca_IConnectionFactory
#define Vca_IConnectionFactory extern
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

/*==========================================================================*
    MakeConnection (
	IVReceiver<IConnection*> *pClient, VString const &rDestination
    );

    Synopsis:
	Open a connection to the network address named by 'rDestination'
	and return the connection to 'pClient'.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IConnectionFactory)

DECLARE_VcaINTERFACE (IConnection)

namespace Vca {
    VINTERFACE_ROLE (IConnectionFactory, IVUnknown)
	VINTERFACE_ROLE_2 (MakeConnection, IVReceiver<IConnection*>*, VString const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IConnectionFactory, IVUnknown);
	VINTERFACE_METHOD_2 (MakeConnection, IVReceiver<IConnection*>*, VString const&);
    VINTERFACE_END
}

#endif
