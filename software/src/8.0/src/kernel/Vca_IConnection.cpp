/*****  Vca_IConnection Implementation  *****/

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

#define   Vca_IConnection
#include "Vca_IConnection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IConnection  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IConnection)

namespace Vca {
// {B0E54F79-C0B5-46cd-B0BB-34296DD9164C}
    VINTERFACE_TYPEINFO_DEFINITION (
	IConnection,
	0xb0e54f79, 0xc0b5, 0x46cd, 0xb0, 0xbb, 0x34, 0x29, 0x6d, 0xd9, 0x16, 0x4c
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IConnection, SupplyBSConsumer, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IConnection, SupplyBSProducer, 3);
}
