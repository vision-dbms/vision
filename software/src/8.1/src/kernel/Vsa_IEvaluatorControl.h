#ifndef Vsa_IEvaluatorControl_Interface
#define Vsa_IEvaluatorControl_Interface

#ifndef Vsa_IEvaluatorControl
#define Vsa_IEvaluatorControl extern
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

#include "Vca_IDataSource.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorControl)

/*************************
 *****  Definitions  *****
 *************************/

DECLARE_VsaINTERFACE (IEvaluator)

namespace Vsa {
    typedef IVReceiver<IEvaluator*> IEvaluatorSink;
    typedef Vca::IDataSource<IEvaluator*> IEvaluatorSource;

    VINTERFACE_ROLE (IEvaluatorControl, IVUnknown)
	VINTERFACE_ROLE_1 (GetEvaluator, IEvaluatorSink*);
	VINTERFACE_ROLE_1 (Suspend, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (Resume, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (Restart, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (GetStatistics, IVReceiver<VString const&>*);
	VINTERFACE_ROLE_2 (GetWorker, IVReceiver<IVUnknown*>*, Vca::U32);
	VINTERFACE_ROLE_1 (Stop, IVReceiver<bool>*);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorControl, IVUnknown);
	VINTERFACE_METHOD_1 (GetEvaluator, IEvaluatorSink*);
	VINTERFACE_METHOD_1 (Suspend, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (Resume, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (Restart, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (GetStatistics, IVReceiver<VString const&>*);
	VINTERFACE_METHOD_2 (GetWorker, IVReceiver<IVUnknown*>*, Vca::U32);
	VINTERFACE_METHOD_1 (Stop, IVReceiver<bool>*);
    VINTERFACE_END
}


#endif
