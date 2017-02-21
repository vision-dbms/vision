#ifndef Vca_IProcess_Interface
#define Vca_IProcess_Interface

#ifndef Vca_IProcess
#define Vca_IProcess extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IProcess)

namespace Vca {
    VINTERFACE_ROLE (IProcess, IVUnknown)
	VINTERFACE_ROLE_1 (WaitForTermination, IVReceiver<S32>*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IProcess, IVUnknown);
	VINTERFACE_METHOD_1 (WaitForTermination, IVReceiver<S32>*);
    VINTERFACE_END
}

#endif
