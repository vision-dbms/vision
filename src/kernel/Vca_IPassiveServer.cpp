/*****  Vca_IPassiveServer Implementation  *****/

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

#define   Vca_IPassiveServer
#include "Vca_IPassiveServer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IPassiveServer  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPassiveServer)

namespace Vca {
// {D4E38FA8-6B81-4bf8-A79E-984012A2D8FC}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPassiveServer,
	0xd4e38fa8, 0x6b81, 0x4bf8, 0xa7, 0x9e, 0x98, 0x40, 0x12, 0xa2, 0xd8, 0xfc
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPassiveServer, RequestCallback, 0);
}
