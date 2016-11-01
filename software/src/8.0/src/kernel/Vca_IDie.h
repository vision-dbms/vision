#ifndef Vca_IDie_Interface
#define Vca_IDie_Interface

#ifndef Vca_Main_IDie
#define Vca_Main_IDie extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

//  pIDie->Stop (bHardStop)

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(Vca_Main,Vca,IDie)

namespace Vca {
    VINTERFACE_ROLE (IDie, IVUnknown)
	VINTERFACE_ROLE_0 (Die)
    VINTERFACE_ROLE_END

    VcaMainINTERFACE (IDie, IVUnknown)
	VINTERFACE_METHOD_0 (Die)
    VINTERFACE_END
}

#endif
