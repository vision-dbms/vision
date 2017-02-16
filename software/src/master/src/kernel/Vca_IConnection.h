#ifndef Vca_IConnection_Interface
#define Vca_IConnection_Interface

#ifndef Vca_IConnection
#define Vca_IConnection extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    SupplyBSConsumer (IVReceiver<VBSConsumer*> *pConsumerClient);

    Synopsis:
	Obtain (via pConsumerClient->OnData') a byte stream consumer
	(sink) that can be used to write data to this connection.
 *==========================================================================*
    SupplyBSProducer (IVReceiver<VBSProducer*> *pProducerClient);

    Synopsis:
	Obtain (via pProducerClient->OnData') a byte stream producer
	(source) that can be used to read data from this connection.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IConnection)

namespace Vca {
    typedef IVReceiver<VBSConsumer*> IVBSConsumerSink;
    typedef IVReceiver<VBSProducer*> IVBSProducerSink;

    VINTERFACE_ROLE (IConnection, IVUnknown)
	VINTERFACE_ROLE_1 (SupplyBSConsumer, IVBSConsumerSink*);
	VINTERFACE_ROLE_1 (SupplyBSProducer, IVBSProducerSink*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IConnection, IVUnknown)
	VINTERFACE_METHOD_1 (SupplyBSConsumer, IVBSConsumerSink*);
	VINTERFACE_METHOD_1 (SupplyBSProducer, IVBSProducerSink*);
    VINTERFACE_END
}

#endif
