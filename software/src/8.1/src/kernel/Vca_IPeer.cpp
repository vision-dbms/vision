/*****  Vca_IPeer Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#define   Vca_IPeer
#include "Vca_IPeer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************
 ************************
 *****              *****
 *****  Vca::IPeer  *****
 *****              *****
 ************************
 ************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPeer)

namespace Vca {
// {4E0AF9D5-9890-4101-916C-221BC92EB86E}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPeer, 0x4e0af9d5, 0x9890, 0x4101, 0x91, 0x6c, 0x22, 0x1b, 0xc9, 0x2e, 0xb8, 0x6e
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, SinkInterface, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, ReleaseExport, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, GetRootInterface, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, GatherConnectionData, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, CreateConnection, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, ReleaseImport, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, GetLocalInterfaceFor, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, FakeExportTo, 7);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, FakeImportFrom, 8);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, SetRemoteReflectionFor, 9);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, OnDone, 10);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer, MakeConnection, 11);
}
