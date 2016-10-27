#ifndef Vxa_IVSNFTaskHolder2_Interface
#define Vxa_IVSNFTaskHolder2_Interface

#ifndef Vxa_IVSNFTaskHolder2
#define Vxa_IVSNFTaskHolder2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_IVSNFTaskHolder1.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VSerializer2DynArray.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskHolder2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskHolder2, IVSNFTaskHolder1)
	VINTERFACE_ROLE_1 (ReturnDouble, double);
	VINTERFACE_ROLE_1 (ReturnS2Booleans, bool_s2array_t const&);
	VINTERFACE_ROLE_1 (ReturnS2Integers, i32_s2array_t const&);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskHolder2, IVSNFTaskHolder1);
	VINTERFACE_METHOD_1 (ReturnDouble, double);
	VINTERFACE_METHOD_1 (ReturnS2Booleans, bool_s2array_t const&);
	VINTERFACE_METHOD_1 (ReturnS2Integers, i32_s2array_t const&);
    VINTERFACE_END
}

#endif
