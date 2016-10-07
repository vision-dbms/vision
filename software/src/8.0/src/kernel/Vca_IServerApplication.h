#ifndef Vca_IServerApplication_Interface
#define Vca_IServerApplication_Interface

#ifndef Vca_IServerApplication
#define Vca_IServerApplication extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IApplication.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IServerApplication)

namespace Vca {
    VINTERFACE_ROLE (IServerApplication, IApplication)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IServerApplication, IApplication)
    VINTERFACE_END
}

#endif
