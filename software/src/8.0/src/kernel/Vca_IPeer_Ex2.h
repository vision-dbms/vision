#ifndef Vca_IPeer_Ex2_Interface
#define Vca_IPeer_Ex2_Interface

#ifndef Vca_IPeer_Ex2
#define Vca_IPeer_Ex2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IPeer_Ex1.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPeer_Ex2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    VcaINTERFACE (IPeer_Ex2, IPeer_Ex1);
	VINTERFACE_METHOD_5 (ReleaseExportEx, uuid_t const&, VcaSSID const&, U32, U32, U32);
    VINTERFACE_END

    VINTERFACE_ROLE (IPeer_Ex2, IPeer_Ex1)
	VINTERFACE_ROLE_5 (ReleaseExportEx, uuid_t const&, VcaSSID const&, U32, U32, U32);
    VINTERFACE_ROLE_END

    typedef IPeer_Ex2 IPeer2;
}

#endif
