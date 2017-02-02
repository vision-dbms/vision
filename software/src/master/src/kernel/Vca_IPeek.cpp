/**
 * @file
 * Provides definition for IPeek interface.
 */
#define Vca_IPeek

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

#include "Vca_IPeek.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************
 ************************
 *****              *****
 *****  Vca::IPeek  *****
 *****              *****
 ************************
 ************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPeek)

namespace Vca {
// {0B933950-2E07-44ed-9496-59D576836DC0}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPeek,
	0xb933950, 0x2e07, 0x44ed, 0x94, 0x96, 0x59, 0xd5, 0x76, 0x83, 0x6d, 0xc0
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPeek, GetValue, 0);
}
