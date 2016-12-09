/*****  Vca_IPauseResume Implementation  *****/

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

#define   Vca_Main_IPauseResume
#include "Vca_IPauseResume.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::IPauseResume  *****
 *****                     *****
 *******************************
 *******************************/


VINTERFACE_TEMPLATE_EXPORTS (Vca::IPauseResume)

namespace Vca {
    VINTERFACE_TYPEINFO_DEFINITION (
	IPauseResume,
	0xbef08b81, 0x7e5b, 0x4b60, 0x9d, 0x96, 0x1d, 0xbb, 0xd7, 0xee, 0x5d, 0x81
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPauseResume, Pause, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IPauseResume, Resume, 1);
}
