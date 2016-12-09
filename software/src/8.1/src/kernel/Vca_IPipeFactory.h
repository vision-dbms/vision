#ifndef Vca_IPipeFactory_Interface
#define Vca_IPipeFactory_Interface

#ifndef Vca_IPipeFactory
#define Vca_IPipeFactory extern
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
	IPipeSourceClient *pClient, VString const &rDestination, bool bTwoPipe
    );

    Synopsis:
	Open a connection to the network address named by 'rDestination'
	and return the connection to 'pClient'.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPipeFactory)

DECLARE_VcaINTERFACE (IPipeSourceClient)

namespace Vca {

    VINTERFACE_ROLE (IPipeFactory, IVUnknown)
	VINTERFACE_ROLE_3 (MakeConnection, IPipeSourceClient*, VString const&, bool);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPipeFactory, IVUnknown);
	VINTERFACE_METHOD_3 (MakeConnection, IPipeSourceClient*, VString const&, bool);
    VINTERFACE_END
}

#endif
