#ifndef Vca_IStop_Interface
#define Vca_IStop_Interface

#ifndef Vca_Main_IStop
#define Vca_Main_IStop extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(Vca_Main,Vca,IStop)

namespace Vca {
    VINTERFACE_ROLE (IStop, IVUnknown)
	VINTERFACE_ROLE_1 (Stop, bool)
    VINTERFACE_ROLE_END

    VcaMainINTERFACE (IStop, IVUnknown)
	VINTERFACE_METHOD_1 (Stop, bool)
    VINTERFACE_END
}


#endif
