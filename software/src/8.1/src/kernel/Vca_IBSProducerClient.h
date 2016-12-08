#ifndef Vca_IBSProducerClient_Interface
#define Vca_IBSProducerClient_Interface

#ifndef Vca_IBSProducerClient
#define Vca_IBSProducerClient extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IBSClient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBSProducerClient)

namespace Vca {
    class VBSProducer;

    VINTERFACE_ROLE (IBSProducerClient, IBSClient)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBSProducerClient, IBSClient)
    VINTERFACE_END
}

#endif
