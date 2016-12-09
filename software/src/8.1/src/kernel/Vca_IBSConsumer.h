#ifndef Vca_IBSConsumer_Interface
#define Vca_IBSConsumer_Interface

#ifndef Vca_IBSConsumer
#define Vca_IBSConsumer extern
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
    Put (VBlob const &rData);

    Synopsis:
	Consume the data found in 'rData'.
 *==========================================================================*
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBSConsumer)

namespace Vca {
    VINTERFACE_ROLE (IBSConsumer, IResource)
	VINTERFACE_ROLE_2 (Put, IVReceiver<U32>*, V::VBlob const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBSConsumer, IResource)
	VINTERFACE_METHOD_2 (Put, IVReceiver<U32>*, V::VBlob const&);
    VINTERFACE_END
}

#endif
