/*****  Vca_IBuddy Implementation  *****/

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

#define   Vca_Main_IBuddy
#include "Vca_IBuddy.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************
 *************************
 *****               *****
 *****  Vca::IBuddy  *****
 *****               *****
 *************************
 *************************/


VINTERFACE_TEMPLATE_EXPORTS (Vca::IBuddy)

namespace Vca {
    VINTERFACE_TYPEINFO_DEFINITION (
	IBuddy,
	0x4aa8d160, 0x2cc5, 0x11dd, 0xb7, 0x87, 0x00, 0x18, 0xfe, 0x28, 0x43, 0xb5
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IBuddy, ControlThese, 0);
}
