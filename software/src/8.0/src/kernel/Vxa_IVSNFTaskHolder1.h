#ifndef Vxa_IVSNFTaskHolder1_Interface
#define Vxa_IVSNFTaskHolder1_Interface

#ifndef Vxa_IVSNFTaskHolder1
#define Vxa_IVSNFTaskHolder1 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_IVSNFTaskHolder.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskHolder1)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskHolder1, IVSNFTaskHolder)
	VINTERFACE_ROLE_1 (ReturnError, VString);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskHolder1, IVSNFTaskHolder);
	VINTERFACE_METHOD_1 (ReturnError, VString);
    VINTERFACE_END
}


#endif
