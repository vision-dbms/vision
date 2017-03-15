#ifndef Vca_IPeer_Interface
#define Vca_IPeer_Interface

#ifndef Vca_IPeer
#define Vca_IPeer extern
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

#include "VPeerDataArray.h"

#ifndef Vca_IConnection
DECLARE_VcaINTERFACE (IConnection)
#endif

#ifndef Vca_IConnectionRequest
DECLARE_VcaINTERFACE (IConnectionRequest)
#endif

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IPeer)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    VINTERFACE_ROLE (IPeer, IVUnknown)
	VINTERFACE_ROLE_1 (SinkInterface, IVUnknown*);
	VINTERFACE_ROLE_4 (ReleaseExport, uuid_t const&, VcaSSID const&, U32, U32);
	VINTERFACE_ROLE_2 (GetRootInterface, VTypeInfo*, IVReceiver<IVUnknown*>*);
	VINTERFACE_ROLE_5 (GatherConnectionData, IConnectionRequest*, VPeerDataArray const&, VPeerDataArray const&, uuid_t const&, bool);
	VINTERFACE_ROLE_3 (CreateConnection, VPeerDataArray const&, uuid_t const&, uuid_t const&);
	VINTERFACE_ROLE_2 (ReleaseImport, uuid_t const&, VcaSSID const&);
	VINTERFACE_ROLE_2 (GetLocalInterfaceFor, VcaSite*, IVReceiver<IPeer*>*);
	VINTERFACE_ROLE_3 (FakeExportTo, uuid_t const&, uuid_t const&, VcaSSID const&);
	VINTERFACE_ROLE_3 (FakeImportFrom, uuid_t const&, uuid_t const&, VcaSSID const&);
	VINTERFACE_ROLE_2 (SetRemoteReflectionFor, uuid_t const&, IPeer*);
	VINTERFACE_ROLE_0 (OnDone);
	VINTERFACE_ROLE_2 (MakeConnection, IVReceiver<IConnection*>*, VString const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IPeer, IVUnknown);
	VINTERFACE_METHOD_1 (SinkInterface, IVUnknown*);
	VINTERFACE_METHOD_4 (ReleaseExport, uuid_t const&, VcaSSID const&, U32, U32);
	VINTERFACE_METHOD_2 (GetRootInterface, VTypeInfo*, IVReceiver<IVUnknown*>*);
	VINTERFACE_METHOD_5 (GatherConnectionData, IConnectionRequest*, VPeerDataArray const&, VPeerDataArray const&, uuid_t const&, bool);
	VINTERFACE_METHOD_3 (CreateConnection, VPeerDataArray const&, uuid_t const&, uuid_t const&);
	VINTERFACE_METHOD_2 (ReleaseImport, uuid_t const&, VcaSSID const&);
	VINTERFACE_METHOD_2 (GetLocalInterfaceFor, VcaSite*, IVReceiver<IPeer*>*);
	VINTERFACE_METHOD_3 (FakeExportTo, uuid_t const&, uuid_t const&, VcaSSID const&);
	VINTERFACE_METHOD_3 (FakeImportFrom, uuid_t const&, uuid_t const&, VcaSSID const&);
	VINTERFACE_METHOD_2 (SetRemoteReflectionFor, uuid_t const&, IPeer*);
	VINTERFACE_METHOD_0 (OnDone);
	VINTERFACE_METHOD_2 (MakeConnection, IVReceiver<IConnection*>*, VString const&);
    VINTERFACE_END
}

#endif
