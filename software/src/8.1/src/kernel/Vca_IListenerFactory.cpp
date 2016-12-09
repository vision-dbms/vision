/*****  Vca_IListenerFactory Implementation  *****/

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

#define   Vca_IListenerFactory
#include "Vca_IListenerFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::IListenerFactory  *****
 *****                         *****
 ***********************************
 ***********************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IListenerFactory)

namespace Vca {
    // {65003F89-55CE-4c26-9E3C-BC425AD36C3F}
    VINTERFACE_TYPEINFO_DEFINITION (
	IListenerFactory,
	0x65003f89, 0x55ce, 0x4c26, 0x9e, 0x3c, 0xbc, 0x42, 0x5a, 0xd3, 0x6c, 0x3f
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IListenerFactory, SupplyByteStreams, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IListenerFactory, SupplyConnections, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IListenerFactory, SupplyListener, 2);
}
