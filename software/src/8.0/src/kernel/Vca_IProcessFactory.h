#ifndef Vca_IProcessFactory_Interface
#define Vca_IProcessFactory_Interface

#ifndef Vca_IProcessFactory
#define Vca_IProcessFactory extern
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
    MakeProcess (
	IProcessFactoryClient *pClient, VString const &rCommand, bool bTwoPipe
    )

    Synopsis:
	Create a new process, sending the process and its standard input,
	output, and optional error output pipes to the process factory client.
	MakeProcess combines stdout and stderr into a single pipe if 'bTwoPipe'
	is true, otherwise separate output and error pipes are plumbed.
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IProcessFactory)

DECLARE_VcaINTERFACE (IProcessFactoryClient)

namespace Vca {
    VINTERFACE_ROLE (IProcessFactory, IVUnknown)
	VINTERFACE_ROLE_3 (MakeProcess, IProcessFactoryClient*, VString const&, bool);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IProcessFactory, IVUnknown);
	VINTERFACE_METHOD_3 (MakeProcess, IProcessFactoryClient*, VString const&, bool);
    VINTERFACE_END
}

#endif
