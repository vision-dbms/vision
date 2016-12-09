#ifndef Vsa_IPoolApplication_Ex1_Interface
#define Vsa_IPoolApplication_Ex1_Interface

#ifndef Vsa_IPoolApplication_Ex1
#define Vsa_IPoolApplication_Ex1 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IServerApplication.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolApplication_Ex1)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IPoolApplication_Ex1, Vca::IServerApplication)
	VINTERFACE_ROLE_0 (Stop);
	VINTERFACE_ROLE_1 (GetPoolWorkerProcessSource, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetPoolWorkerProcessSource, VString const &);
   VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolApplication_Ex1, Vca::IServerApplication);
	VINTERFACE_METHOD_0 (Stop);
	VINTERFACE_METHOD_1 (GetPoolWorkerProcessSource, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetPoolWorkerProcessSource, VString const &);
    VINTERFACE_END
}


#endif
