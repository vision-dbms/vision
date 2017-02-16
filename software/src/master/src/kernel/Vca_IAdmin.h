#ifndef Vca_IAdmin_Interface
#define Vca_IAdmin_Interface

#ifndef Vca_Main_IAdmin
#define Vca_Main_IAdmin extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(Vca_Main,Vca,IAdmin)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    VINTERFACE_ROLE (IAdmin, IVUnknown)
    VINTERFACE_ROLE_END

    VcaMainINTERFACE (IAdmin, IVUnknown);
    VINTERFACE_END
}


#endif
