#ifndef Vca_IConnectionRequest_Interface
#define Vca_IConnectionRequest_Interface

#ifndef Vca_IConnectionRequest
#define Vca_IConnectionRequest extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IConnectionRequest)

namespace Vca {
    VINTERFACE_ROLE (IConnectionRequest, IVUnknown)
	VINTERFACE_ROLE_1 (onListenerAddress, VString const &);
	VINTERFACE_ROLE_0 (onDone);
	VINTERFACE_ROLE_0 (onFailure);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IConnectionRequest, IVUnknown);
	VINTERFACE_METHOD_1 (onListenerAddress, VString const &);
	VINTERFACE_METHOD_0 (onDone);
	VINTERFACE_METHOD_0 (onFailure);
    VINTERFACE_END
}

#endif
