#ifndef Vca_ISSHConnectionFactory_Interface
#define Vca_ISSHConnectionFactory_Interface

#ifndef Vca_ISSHConnectionFactory
#define Vca_ISSHConnectionFactory extern
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

DECLARE_VcaINTERFACE (IPipeSourceClient)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,ISSHConnectionFactory)


/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    MakeSSHConnection (
                IPipeSourceClient *pClient,
                VString const &rHost,
                VString const &rUserName,
                VString const &rPassword,
                VString const &rCommand,
                bool bErrorChannel
    )
    Synopsis:
        Create an SSH connection to the given host using the provided
        authorization credentials. Then execute the command at the
        remote host's default shell and return the input/output/error
        bytestreams of the connection via the IPipeSourceClient.

 *==========================================================================*/

namespace Vca {
    VINTERFACE_ROLE (ISSHConnectionFactory, IVUnknown)
        VINTERFACE_ROLE_6 (MakeSSHConnection,
                                IPipeSourceClient*,
                                VString const&,
                                VString const&, VString const&,
                                VString const&, bool);
    VINTERFACE_ROLE_END

    VcaINTERFACE (ISSHConnectionFactory, IVUnknown);
        VINTERFACE_METHOD_6 (MakeSSHConnection,
                                IPipeSourceClient*,
                                VString const&,
                                VString const&, VString const&,
                                VString const&, bool);
    VINTERFACE_END
}


#endif
