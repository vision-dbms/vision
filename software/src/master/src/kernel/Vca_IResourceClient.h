#ifndef Vca_IResourceClient_Interface
#define Vca_IResourceClient_Interface

#ifndef Vca_IResourceClient
#define Vca_IResourceClient extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IResourceClient)

namespace Vca {
    VINTERFACE_ROLE (IResourceClient, IClient)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IResourceClient, IClient)
    VINTERFACE_END
}


#endif
