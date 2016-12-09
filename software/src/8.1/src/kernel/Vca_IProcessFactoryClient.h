#ifndef Vca_IProcessFactoryClient_Interface
#define Vca_IProcessFactoryClient_Interface

#ifndef Vca_IProcessFactoryClient
#define Vca_IProcessFactoryClient extern
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
	IProcess    *pProcess,
	VBSConsumer *pPipeToPeer,
	VBSProducer *pPipeToHere,
	VBSProducer *pErrorPipeToHere
    );

    Synopsis:
	Receives the result of a process creation.    One or more of the
	pointers may be NULL or refer to the same stream depending on the
	source and operating system supplying the pipes.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IProcessFactoryClient)

DECLARE_VcaINTERFACE (IProcess)

namespace Vca {
    VINTERFACE_ROLE (IProcessFactoryClient, IClient)
	VINTERFACE_ROLE_4 (OnData, IProcess*, VBSConsumer*, VBSProducer*, VBSProducer*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IProcessFactoryClient, IClient);
	VINTERFACE_METHOD_4 (OnData, IProcess*, VBSConsumer*, VBSProducer*, VBSProducer*);
    VINTERFACE_END
}


#endif
