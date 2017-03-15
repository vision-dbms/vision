#ifndef Vxa_IVSNFTaskImplementation3_Interface
#define Vxa_IVSNFTaskImplementation3_Interface

#ifndef Vxa_IVSNFTaskImplementation3
#define Vxa_IVSNFTaskImplementation3 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_IVSNFTaskImplementation2.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskImplementation3)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskImplementation3, IVSNFTaskImplementation2)
	VINTERFACE_ROLE_2 (OnParameterError, unsigned int, VString const&);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskImplementation3, IVSNFTaskImplementation2);
	VINTERFACE_METHOD_2 (OnParameterError, unsigned int, VString const&);
    VINTERFACE_END
}


#endif
