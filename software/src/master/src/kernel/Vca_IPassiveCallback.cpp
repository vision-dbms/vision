/*****  Vca_IPassiveCallback Implementation  *****/

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

#define   Vca_IPassiveCallback
#include "Vca_IPassiveCallback.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::IPassiveCallback  *****
 *****                         *****
 ***********************************
 ***********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPassiveCallback)

namespace Vca {
// {FE1092F2-2F55-431d-8711-6E73C18BFFB6}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPassiveCallback,
	0xfe1092f2, 0x2f55, 0x431d, 0x87, 0x11, 0x6e, 0x73, 0xc1, 0x8b, 0xff, 0xb6
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPassiveCallback, QueryService, 0);
}
