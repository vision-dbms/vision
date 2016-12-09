#ifndef Vca_IStdPipeSource_Interface
#define Vca_IStdPipeSource_Interface

#ifndef Vca_IStdPipeSource
#define Vca_IStdPipeSource extern
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
    Supply (IStdPipeSourceClient *pClient)

    Synopsis:
	Create a set of pipes for consumption by the client.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IStdPipeSource)

DECLARE_VcaINTERFACE (IStdPipeSourceClient)

namespace Vca {
    VINTERFACE_ROLE (IStdPipeSource, IVUnknown)
	VINTERFACE_ROLE_1 (Supply, IStdPipeSourceClient*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IStdPipeSource, IVUnknown);
	VINTERFACE_METHOD_1 (Supply, IStdPipeSourceClient*);
    VINTERFACE_END
}


#endif
