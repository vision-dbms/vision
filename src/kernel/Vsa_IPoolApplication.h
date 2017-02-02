#ifndef Vsa_IPoolApplication_Interface
#define Vsa_IPoolApplication_Interface

#ifndef Vsa_IPoolApplication
#define Vsa_IPoolApplication extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolApplication)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IPoolApplication, IVUnknown)
	VINTERFACE_ROLE_0 (Stop);
	VINTERFACE_ROLE_1 (GetPoolWorkerProcessSource, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetPoolWorkerProcessSource, VString const &);
   VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolApplication, IVUnknown);
	VINTERFACE_METHOD_0 (Stop);
	VINTERFACE_METHOD_1 (GetPoolWorkerProcessSource, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetPoolWorkerProcessSource, VString const &);
    VINTERFACE_END
}


#endif
