/*****  Vca_IPassiveCall Implementation  *****/

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

#define   Vca_IPassiveCall
#include "Vca_IPassiveCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::IPassiveCall  *****
 *****                     *****
 *******************************
 *******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPassiveCall)

namespace Vca {
// {027E71A2-AF35-452b-8DFE-F4CDDA848219}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPassiveCall,
	0x27e71a2, 0xaf35, 0x452b, 0x8d, 0xfe, 0xf4, 0xcd, 0xda, 0x84, 0x82, 0x19
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPassiveCall, GetUUID, 0);
}
