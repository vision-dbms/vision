#ifndef Vsa_IPoolApplication_Ex2_Interface
#define Vsa_IPoolApplication_Ex2_Interface

#ifndef Vsa_IPoolApplication_Ex2
#define Vsa_IPoolApplication_Ex2 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IPoolApplication_Ex1.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IPoolApplication_Ex2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IPoolApplication_Ex2, IPoolApplication_Ex1)
	VINTERFACE_ROLE_0 (HardStop);
   VINTERFACE_ROLE_END

    VsaINTERFACE (IPoolApplication_Ex2, IPoolApplication_Ex1);
	VINTERFACE_METHOD_0 (HardStop);
    VINTERFACE_END
}


#endif
