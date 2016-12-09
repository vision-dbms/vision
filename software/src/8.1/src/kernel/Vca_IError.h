#ifndef Vca_IError_Interface
#define Vca_IError_Interface

#ifndef Vca_IError
#define Vca_IError extern
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


/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IError)

namespace Vca {
    VINTERFACE_ROLE (IError, IVUnknown)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IError, IVUnknown);
    VINTERFACE_END
}

#endif
