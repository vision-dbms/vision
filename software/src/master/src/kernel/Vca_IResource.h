#ifndef Vca_IResource_Interface
#define Vca_IResource_Interface

#ifndef Vca_IResource
#define Vca_IResource extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IResource)

namespace Vca {
    VINTERFACE_ROLE (IResource, IVUnknown)
	VINTERFACE_ROLE_0 (Close);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IResource, IVUnknown)
	VINTERFACE_METHOD_0 (Close);
    VINTERFACE_END
}

#endif
