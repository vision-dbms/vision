#ifndef Vca_IListenerFactory_Interface
#define Vca_IListenerFactory_Interface

#ifndef Vca_IListenerFactory
#define Vca_IListenerFactory extern
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

/*************************************
 *-----  Vca::IListenerFactory  -----*
 *************************************/

/*===========================================================================
 *  Members:
 *	SupplyByteStreams (
 *	    IPipeSourceClient *pClient, VString const &rNetName, U32 cConnections
 *	);
 *	SupplyConnections (
 *	    IListenerClient *pClient, VString const &rNetName, U32 cConnections
 *	);
 *	SupplyListener (
 *	    IVReceiver<IListener*>*pClient, VString const &rNetName
 *	);
 *==========================================================================*/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IListenerFactory)

DECLARE_VcaINTERFACE (IListener)
DECLARE_VcaINTERFACE (IListenerClient)
DECLARE_VcaINTERFACE (IPipeSourceClient)

namespace Vca {
    VINTERFACE_ROLE (IListenerFactory, IVUnknown)
	VINTERFACE_ROLE_3 (
	    SupplyByteStreams, IPipeSourceClient*, VString const&, U32
	);
	VINTERFACE_ROLE_3 (
	    SupplyConnections, IListenerClient*, VString const&, U32
	);
	VINTERFACE_ROLE_2 (
	    SupplyListener, IVReceiver<IListener*>*, VString const&
	);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IListenerFactory, IVUnknown);
	VINTERFACE_METHOD_3 (
	    SupplyByteStreams, IPipeSourceClient*, VString const&, U32
	);
	VINTERFACE_METHOD_3 (
	    SupplyConnections, IListenerClient*, VString const&, U32
	);
	VINTERFACE_METHOD_2 (
	    SupplyListener, IVReceiver<IListener*>*, VString const&
	);
    VINTERFACE_END
}

#endif
