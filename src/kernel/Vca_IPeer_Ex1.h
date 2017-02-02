#ifndef Vca_IPeer_Ex1_Interface
#define Vca_IPeer_Ex1_Interface

#ifndef Vca_IPeer_Ex1
#define Vca_IPeer_Ex1 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IPeer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IProcessInfoSink.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPeer_Ex1)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    VcaINTERFACE (IPeer_Ex1, IPeer);
	VINTERFACE_METHOD_2 (WeakenExport,uuid_t const&,VcaSSID const&)
	VINTERFACE_METHOD_2 (WeakenImport,uuid_t const&,VcaSSID const&)
	VINTERFACE_METHOD_2 (GetProcessInfo,uuid_t const&, IProcessInfoSink*)
    VINTERFACE_END

    VINTERFACE_ROLE (IPeer_Ex1, IPeer)
	VINTERFACE_ROLE_2 (WeakenExport,uuid_t const&,VcaSSID const&)
	VINTERFACE_ROLE_2 (WeakenImport,uuid_t const&,VcaSSID const&)
	VINTERFACE_ROLE_2 (GetProcessInfo,uuid_t const&,IProcessInfoSink*)
    VINTERFACE_ROLE_END

    typedef IPeer_Ex1 IPeer1;
}

#endif
