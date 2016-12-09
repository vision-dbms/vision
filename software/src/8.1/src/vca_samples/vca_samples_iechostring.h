#ifndef VcaSamples_IEchoString_Interface
#define VcaSamples_IEchoString_Interface

#ifndef VcaSamples_IEchoString
#define VcaSamples_IEchoString extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "vca_samples.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (VcaSamples,IEchoString)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
 *==========================================================================*/

namespace VcaSamples {
    VINTERFACE_ROLE (IEchoString, IVUnknown)
	VINTERFACE_ROLE_2 (Echo, IVReceiver<VString const&>*, VString const&);
    VINTERFACE_ROLE_END

    VcaSamplesINTERFACE (IEchoString, IVUnknown);
	VINTERFACE_METHOD_2 (Echo, IVReceiver<VString const&>*, VString const&);
    VINTERFACE_END
}


#endif
