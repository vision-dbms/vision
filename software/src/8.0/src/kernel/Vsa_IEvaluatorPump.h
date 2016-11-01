#ifndef Vsa_IEvaluatorPump_Interface
#define Vsa_IEvaluatorPump_Interface

#ifndef Vsa_IEvaluatorPump
#define Vsa_IEvaluatorPump extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorControl.h"

/**************************
 *****  Declarations  *****
 **************************/

DECLARE_VsaINTERFACE (IEvaluatorPumpSettings)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPump)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPump, IEvaluatorControl)
	VINTERFACE_ROLE_1 (GetSettings, IVReceiver<IEvaluatorPumpSettings*>*);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPump, IEvaluatorControl);
	VINTERFACE_METHOD_1 (GetSettings, IVReceiver<IEvaluatorPumpSettings*>*);
    VINTERFACE_END
}


#endif
