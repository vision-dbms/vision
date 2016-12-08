#ifndef Vxa_IVSNFTaskImplementation3NC_Interface
#define Vxa_IVSNFTaskImplementation3NC_Interface

#ifndef Vxa_IVSNFTaskImplementation3NC
#define Vxa_IVSNFTaskImplementation3NC extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_IVSNFTaskImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskImplementation3NC)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskImplementation3NC, IVSNFTaskImplementation)
	VINTERFACE_ROLE_2 (OnParameterError, unsigned int, VString const&);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskImplementation3NC, IVSNFTaskImplementation);
	VINTERFACE_METHOD_2 (OnParameterError, unsigned int, VString const&);
    VINTERFACE_END
}


#endif
