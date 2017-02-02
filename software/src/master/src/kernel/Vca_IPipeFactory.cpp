/*****  Vca_IPipeFactory Implementation  *****/

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

#define   Vca_IPipeFactory
#include "Vca_IPipeFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::IPipeFactory  *****
 *****                     *****
 *******************************
 *******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPipeFactory)

namespace Vca {
// {92362320-F271-466c-A75A-C26814A9C22D}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPipeFactory,
	0x92362320, 0xf271, 0x466c, 0xa7, 0x5a, 0xc2, 0x68, 0x14, 0xa9, 0xc2, 0x2d
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPipeFactory, MakeConnection, 0);
}
