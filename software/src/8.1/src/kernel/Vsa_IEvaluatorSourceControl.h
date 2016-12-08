#ifndef Vsa_IEvaluatorSourceControl_Interface
#define Vsa_IEvaluatorSourceControl_Interface

#ifndef Vsa_IEvaluatorSourceControl
#define Vsa_IEvaluatorSourceControl extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorSourceControl)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorSourceControl, IVUnknown)
	VINTERFACE_ROLE_1 (SetStartupQuery, VString const &);
	VINTERFACE_ROLE_1 (GetStartupQuery, IVReceiver<VString const &>*);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorSourceControl, IVUnknown);
	VINTERFACE_METHOD_1 (SetStartupQuery, VString const &);
	VINTERFACE_METHOD_1 (GetStartupQuery, IVReceiver<VString const &>*);
    VINTERFACE_END
}


#endif
