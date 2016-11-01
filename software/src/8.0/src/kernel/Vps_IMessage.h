#ifndef Vps_IMessage_Interface
#define Vps_IMessage_Interface

#ifndef Vps_IMessage
#define Vps_IMessage extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,IMessage)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    SetWanted (
	bool bWanted
    );
 *==========================================================================*/

namespace Vca {
    namespace Vps {
	VINTERFACE_ROLE (IMessage,IVUnknown)
	    VINTERFACE_ROLE_1 (SetWanted, bool);
	VINTERFACE_ROLE_END

	VpsINTERFACE (IMessage,IVUnknown);
	    VINTERFACE_METHOD_1 (SetWanted, bool);
	VINTERFACE_END
    }
}


#endif
