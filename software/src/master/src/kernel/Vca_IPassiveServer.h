#ifndef Vca_IPassiveServer_Interface
#define Vca_IPassiveServer_Interface

#ifndef Vca_IPassiveServer
#define Vca_IPassiveServer extern
#endif

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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPassiveServer)

DECLARE_VcaINTERFACE (IPassiveCall)

namespace Vca {
    //****************************************************************************************
    //
    //  RequestCallback (
    //        IPassiveCall *pCall, VString const &rAddress, uuid_t const &rCallID
    //	);
    //
    //  Parameters:
    //        pCall		... an interface for the passive call being returned.  This
    //				    could be an interface for the actual call, but is usually
    //				    an interface for a local surrogate for the call.
    //        rCallerAddress	... the caller's address.
    //        rCallID		... the caller's UUID for this call.
    //
    //****************************************************************************************

    VINTERFACE_ROLE (IPassiveServer, IVUnknown)
	VINTERFACE_ROLE_3 (RequestCallback, IPassiveCall*, VString const&, uuid_t const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPassiveServer, IVUnknown);
	VINTERFACE_METHOD_3 (RequestCallback, IPassiveCall*, VString const&, uuid_t const&);
    VINTERFACE_END
}

#endif
