#ifndef Vca_IStdPipeSourceClient_Interface
#define Vca_IStdPipeSourceClient_Interface

#ifndef Vca_IStdPipeSourceClient
#define Vca_IStdPipeSourceClient extern
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
	VBSProducer *pPipeToHere,
	VBSConsumer *pPipeToPeer,
	VBSConsumer *pErrorPipeToPeer
    );

    Synopsis:
	Receives byte streams from a standard (stdin, stdout, stderr) pipe
	source.  One or more of the byte stream pointers may be NULL or refer
	to the same stream depending on the source and operating system
	supplying the streams.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IStdPipeSourceClient)

namespace Vca {
    VINTERFACE_ROLE (IStdPipeSourceClient, IClient)
	VINTERFACE_ROLE_3 (OnData, VBSProducer*, VBSConsumer*, VBSConsumer*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IStdPipeSourceClient, IClient);
	VINTERFACE_METHOD_3 (OnData, VBSProducer*, VBSConsumer*, VBSConsumer*);
    VINTERFACE_END
}


#endif
