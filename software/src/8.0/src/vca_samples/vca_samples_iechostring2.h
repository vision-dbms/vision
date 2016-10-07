#ifndef VcaSamples_IEchoString2_Interface
#define VcaSamples_IEchoString2_Interface

#ifndef VcaSamples_IEchoString2
#define VcaSamples_IEchoString2 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "vca_samples.h"

/************************
 *****  Components  *****
 ************************/

#include "vca_samples_iechostring.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (VcaSamples,IEchoString2)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace VcaSamples {
    class IEchoString2;

    VINTERFACE_ROLE (IEchoString2, IEchoString)
	VINTERFACE_ROLE_2 (GetChild, IVReceiver<IEchoString2*>*, VString const&);
    VINTERFACE_ROLE_END

    VcaSamplesINTERFACE (IEchoString2, IEchoString);
	VINTERFACE_METHOD_2 (GetChild, IVReceiver<IEchoString2*>*, VString const&);
    VINTERFACE_END
}


#endif
