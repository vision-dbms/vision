/*****  Vca_IPassiveClient Implementation  *****/

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

#define   Vca_IPassiveClient
#include "Vca_IPassiveClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IPassiveClient  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPassiveClient)

namespace Vca {
    VINTERFACE_TYPEINFO_DEFINITION (
	IPassiveClient,
	0x316ea8e, 0x902c, 0x4cf0, 0x8a, 0x7a, 0x42, 0x7d, 0x83, 0xfe, 0x5e, 0x75
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPassiveClient, AcceptCallback, 0);
}
