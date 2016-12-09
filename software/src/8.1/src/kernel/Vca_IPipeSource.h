#ifndef Vca_IPipeSource_Interface
#define Vca_IPipeSource_Interface

#ifndef Vca_IPipeSource
#define Vca_IPipeSource extern
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
    Supply (IPipeSourceClient *pClient)

    Synopsis:
	Create a set of pipes for consumption by the client.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPipeSource)

DECLARE_VcaINTERFACE (IPipeSourceClient)

namespace Vca {

    VINTERFACE_ROLE (IPipeSource, IVUnknown)
	VINTERFACE_ROLE_1 (Supply, IPipeSourceClient*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPipeSource, IVUnknown);
	VINTERFACE_METHOD_1 (Supply, IPipeSourceClient*);
    VINTERFACE_END
}

#endif
