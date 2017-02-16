#ifndef Vsa_IPoolWorker_Interface
#define Vsa_IPoolWorker_Interface

#ifndef Vsa_IPoolWorker
#define Vsa_IPoolWorker extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolWorker)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    GetStatistics (IVReceiver<VString const&>*);

    Synopsis:
	Gets the statistics pertaining to this PoolWorker
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IPoolWorker, IVUnknown)
	VINTERFACE_ROLE_1 (GetStatistics, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolWorker, IVUnknown);
	VINTERFACE_METHOD_1 (GetStatistics, IVReceiver<VString const&>*);
    VINTERFACE_END
}


#endif
