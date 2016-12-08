#ifndef Vsa_IEvaluatorPumpSettings_Interface
#define Vsa_IEvaluatorPumpSettings_Interface

#ifndef Vsa_IEvaluatorPumpSettings
#define Vsa_IEvaluatorPumpSettings extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorSettings.h"

/**************************
 *****  Declarations  *****
 **************************/

DECLARE_VcaINTERFACE (IPipeSource)

DECLARE_VsaINTERFACE (IEvaluatorPump)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPumpSettings)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPumpSettings, IEvaluatorSettings)
	VINTERFACE_ROLE_3 (
	    MakeControl, IVReceiver<IEvaluatorPump*>*, Vca::VBSConsumer*, Vca::VBSProducer*
	);
	VINTERFACE_ROLE_2 (MakeSource, IVReceiver<IEvaluatorSource*>*, Vca::IPipeSource*);
	VINTERFACE_ROLE_1 (GetURLClass, IVReceiver<VString const&>*);
	VINTERFACE_ROLE_1 (SetURLClass, VString const&);
	VINTERFACE_ROLE_1 (GetURLTemplate, IVReceiver<VString const&>*);
	VINTERFACE_ROLE_1 (SetURLTemplate, VString const&);
	VINTERFACE_ROLE_1 (GetUsingExtendedPrompts, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (SetUsingExtendedPrompts, bool);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IEvaluatorPumpSettings, IEvaluatorSettings);
	VINTERFACE_METHOD_3 (
	    MakeControl, IVReceiver<IEvaluatorPump*>*, Vca::VBSConsumer*, Vca::VBSProducer*
	);
	VINTERFACE_METHOD_2 (MakeSource, IVReceiver<IEvaluatorSource*>*, Vca::IPipeSource*);
	VINTERFACE_METHOD_1 (GetURLClass, IVReceiver<VString const&>*);
	VINTERFACE_METHOD_1 (SetURLClass, VString const&);
	VINTERFACE_METHOD_1 (GetURLTemplate, IVReceiver<VString const&>*);
	VINTERFACE_METHOD_1 (SetURLTemplate, VString const&);
	VINTERFACE_METHOD_1 (GetUsingExtendedPrompts, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (SetUsingExtendedPrompts, bool);
    VINTERFACE_END
}


#endif
