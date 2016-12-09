#ifndef Vca_IBSConsumerClient_Interface
#define Vca_IBSConsumerClient_Interface

#ifndef Vca_IBSConsumerClient
#define Vca_IBSConsumerClient extern
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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBSConsumerClient)

namespace Vca {
    class VBSConsumer;

    VINTERFACE_ROLE (IBSConsumerClient, IBSClient)
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBSConsumerClient, IBSClient)
    VINTERFACE_END
}

#endif
