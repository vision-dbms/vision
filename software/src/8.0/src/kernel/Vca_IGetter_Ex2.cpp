/**
 * @file
 * Provides definition for IGetter_Ex2 interface.
 */
#define Vca_IGetter_Ex2

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

#include "Vca_IGetter_Ex2.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IGetter_Ex2  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IGetter_Ex2)

namespace Vca {
//  IGetter_Ex2
// {123773A2-E307-4c9b-A4BF-EC7290F2D4E3}
    VINTERFACE_TYPEINFO_DEFINITION (
	IGetter_Ex2,
	0x123773a2, 0xe307, 0x4c9b, 0xa4, 0xbf, 0xec, 0x72, 0x90, 0xf2, 0xd4, 0xe3
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IGetter_Ex2, GetU64Value, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IGetter_Ex2, GetF32Value, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IGetter_Ex2, GetF64Value, 2);
}
