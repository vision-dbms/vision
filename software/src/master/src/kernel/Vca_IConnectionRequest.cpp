/*****  Vca_IConnectionRequest Implementation  *****/

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

#define   Vca_IConnectionRequest
#include "Vca_IConnectionRequest.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::IConnectionRequest  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IConnectionRequest)

namespace Vca {
// {3DC78095-C234-4653-A723-4385E4C6AA0B}
    VINTERFACE_TYPEINFO_DEFINITION (
	IConnectionRequest, 0x3DC78095, 0xC234, 0x4653, 0xA7, 0x23, 0x43, 0x85, 0xE4, 0xC6, 0xAA, 0x0B
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IConnectionRequest, onListenerAddress, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IConnectionRequest, onDone, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IConnectionRequest, onFailure, 2);
}
