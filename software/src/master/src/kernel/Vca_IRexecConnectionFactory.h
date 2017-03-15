#ifndef Vca_IRexecConnectionFactory_Interface
#define Vca_IRexecConnectionFactory_Interface

#ifndef Vca_IRexecConnectionFactory
#define Vca_IRexecConnectionFactory extern
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
    MakeRexecConnection (
		IPipeSourceClient *pClient, 
		VString const &rHost, 
		Vca::U32 xPort,
		VString const &rUserName,
		VString const &rPassword,
		VString const &rCommand,
		bool bErrorChannel
    )
    Synopsis:
        Create an rexec connection to the given host using the provided
	authorization credentials. Then execute the command at the 
	remote host's default shell and return the input/output/error 
	bytestreams of the connection via the IPipeSourceClient.

 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IRexecConnectionFactory)

DECLARE_VcaINTERFACE (IPipeSourceClient)

namespace Vca {
    VINTERFACE_ROLE (IRexecConnectionFactory, IVUnknown)
        VINTERFACE_ROLE_7 (MakeRexecConnection, 
				IPipeSourceClient*, 
				VString const&, Vca::U32, 
				VString const&, VString const&,
				VString const&, bool);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IRexecConnectionFactory, IVUnknown);
        VINTERFACE_METHOD_7 (MakeRexecConnection, 
				IPipeSourceClient*, 
				VString const&, Vca::U32, 
				VString const&, VString const&,
				VString const&, bool);
    VINTERFACE_END
}


#endif
