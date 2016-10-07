#ifndef Vca_IBSProducer_Interface
#define Vca_IBSProducer_Interface

#ifndef Vca_IBSProducer
#define Vca_IBSProducer extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IResource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VBlob.h"

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    Get (IVReceiver<V::VBlob const&> *pDataReceiver, U32 sMinimum, U32 sMaximum);

    Synopsis:
	Obtain some data.
 *==========================================================================*
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBSProducer)

namespace Vca {
    VINTERFACE_ROLE (IBSProducer, IResource)
	VINTERFACE_ROLE_3 (Get, IVReceiver<V::VBlob const&>*, U32, U32);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBSProducer, IResource)
	VINTERFACE_METHOD_3 (Get, IVReceiver<V::VBlob const&>*, U32, U32);
    VINTERFACE_END
}

#endif
