#ifndef Vxa_IVSNFTaskImplementation2_Interface
#define Vxa_IVSNFTaskImplementation2_Interface

#ifndef Vxa_IVSNFTaskImplementation2
#define Vxa_IVSNFTaskImplementation2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_IVSNFTaskImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VSerializer2DynArray.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskImplementation2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskImplementation2, IVSNFTaskImplementation)
	VINTERFACE_ROLE_2 (SetToS2Integers, unsigned int, i32_s2array_t const&);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskImplementation2, IVSNFTaskImplementation);
	VINTERFACE_METHOD_2 (SetToS2Integers, unsigned int, i32_s2array_t const&);
    VINTERFACE_END
}


#endif
