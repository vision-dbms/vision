#ifndef Vca_IPipeSourceClient_Interface
#define Vca_IPipeSourceClient_Interface

#ifndef Vca_IPipeSourceClient
#define Vca_IPipeSourceClient extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    OnData (
	VBSConsumer *pPipeToPeer,
	VBSProducer *pPipeToHere,
	VBSProducer *pErrorPipeToHere
    );

    Synopsis:
	Receives pipes from a pipe source.  One or more of the byte stream
	pointers may be NULL or refer to the same stream depending on the
	source and operating system supplying the pipes.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPipeSourceClient)

namespace Vca {
    VINTERFACE_ROLE (IPipeSourceClient, IClient)
	VINTERFACE_ROLE_3 (OnData, VBSConsumer*, VBSProducer*, VBSProducer*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPipeSourceClient, IClient);
	VINTERFACE_METHOD_3 (OnData, VBSConsumer*, VBSProducer*, VBSProducer*);
    VINTERFACE_END
}

#endif
