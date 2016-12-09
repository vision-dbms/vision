/*****  Vca_IRequest Implementation  *****/

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

#define   Vca_IRequest
#include "Vca_IRequest.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::IRequest  *****
 *****                 *****
 ***************************
 ***************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IRequest)

namespace Vca {
    // {59167A4A-E27D-450b-8421-A451B61090F0}
    VINTERFACE_TYPEINFO_DEFINITION (
	IRequest,
	0x59167a4a, 0xe27d, 0x450b, 0x84, 0x21, 0xa4, 0x51, 0xb6, 0x10, 0x90, 0xf0
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IRequest, Cancel, 0);
}
