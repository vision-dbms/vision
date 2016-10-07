#ifndef Vca_IDevice_Interface
#define Vca_IDevice_Interface

#ifndef Vca_IDevice
#define Vca_IDevice extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IResource.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IDevice)

namespace Vca {
    VINTERFACE_ROLE (IDevice, IResource)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IDevice, IResource);
    VINTERFACE_END
}

#endif
