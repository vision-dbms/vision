#ifndef Vsa_IUpDownSubscriber_Interface
#define Vsa_IUpDownSubscriber_Interface

#ifndef Vsa_IUpDownSubscriber
#define Vsa_IUpDownSubscriber extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IUpDownSubscriber)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    OnUp ();

    Synopsis:
	The publishers status is now Up

    OnDown ();

    Synopsis:
	The publishers status is now Down
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IUpDownSubscriber, IVUnknown)
	VINTERFACE_ROLE_0 (OnUp);
	VINTERFACE_ROLE_0 (OnDown);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IUpDownSubscriber, IVUnknown);
	VINTERFACE_METHOD_0 (OnUp);
	VINTERFACE_METHOD_0 (OnDown);
    VINTERFACE_END
}


#endif
