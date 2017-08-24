#pragma once  /* make directive - do not remove this line */

#ifndef _H_VCA_IINFOSERVER
#define _H_VCA_IINFOSERVER

#ifndef Vca_IInfoServer
#define Vca_IInfoServer extern
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

#include "VkDynamicArrayOf.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca, IInfoServer)

namespace Vca {
    VINTERFACE_ROLE (IInfoServer, IVUnknown)
	VINTERFACE_ROLE_3 (Notify, IClient*, int, VkDynamicArrayOf<VString> const &);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IInfoServer, IVUnknown);
        VINTERFACE_METHOD_3 (Notify, IClient*, int, VkDynamicArrayOf<VString> const &);
    VINTERFACE_END
}

#endif /* _H_VCA_IINFOSERVER */
