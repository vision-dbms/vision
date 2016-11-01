/*****  Vca_IPipeSource Implementation  *****/

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

#define   Vca_IPipeSource
#include "Vca_IPipeSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IPipeSource  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPipeSource)

namespace Vca {
// {3B1AA5A0-EF3D-4b60-AC97-A9D01A22BFDC}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPipeSource,
	0x3b1aa5a0, 0xef3d, 0x4b60, 0xac, 0x97, 0xa9, 0xd0, 0x1a, 0x22, 0xbf, 0xdc
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPipeSource, Supply, 0);
}
