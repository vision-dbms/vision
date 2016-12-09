#ifndef Vsa_IEvaluatorSettings_Interface
#define Vsa_IEvaluatorSettings_Interface

#ifndef Vsa_IEvaluatorSettings
#define Vsa_IEvaluatorSettings extern
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

/*************************
 *****  Definitions  *****
 *************************/

DECLARE_VsaINTERFACE (IEvaluator)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorSettings)

namespace Vsa {
    typedef IVReceiver<IEvaluator*> IEvaluatorSink;
    typedef Vca::IDataSource<IEvaluator*> IEvaluatorSource;

    VINTERFACE_ROLE (IEvaluatorSettings, IVUnknown)
	VINTERFACE_ROLE_1 (GetEvaluationTimeOut, IVReceiver<Vca::U64>*);
	VINTERFACE_ROLE_1 (SetEvaluationTimeOut, Vca::U64);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorSettings, IVUnknown);
	VINTERFACE_METHOD_1 (GetEvaluationTimeOut, IVReceiver<Vca::U64>*);
	VINTERFACE_METHOD_1 (SetEvaluationTimeOut, Vca::U64);
    VINTERFACE_END
}


#endif
