/**
 * @file
 * Provides definition for IGetter interface.
 */
#define Vca_IGetter

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

#include "Vca_IGetter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**************************
 **************************
 *****                *****
 *****  Vca::IGetter  *****
 *****                *****
 **************************
 **************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IGetter)

namespace Vca {
//  IGetter
    VINTERFACE_TYPEINFO_DEFINITION (
	IGetter,
	0xdf0723be, 0x639e, 0x11dd, 0xb5, 0x3d, 0x00, 0x1b, 0x78, 0xd2, 0xb6, 0xaf
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IGetter, GetStringValue, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IGetter, GetU32Value, 1);
}
