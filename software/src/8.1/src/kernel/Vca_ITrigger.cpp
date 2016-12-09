/*****  Vca_ITrigger Implementation  *****/

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

#define   Vca_ITrigger
#include "Vca_ITrigger.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::ITrigger  *****
 *****                 *****
 ***************************
 ***************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::ITrigger)

namespace Vca {
// {5A0C0F45-B24B-460d-89C4-17ACF263AD06}
    VINTERFACE_TYPEINFO_DEFINITION (
	ITrigger,
	0x5a0c0f45, 0xb24b, 0x460d, 0x89, 0xc4, 0x17, 0xac, 0xf2, 0x63, 0xad, 0x6
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (ITrigger, Process, 0);
}
