/*****  Vca_IBinding Implementation  *****/

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

#define   Vca_IBinding
#include "Vca_IBinding.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::IBinding  *****
 *****                 *****
 ***************************
 ***************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IBinding)

namespace Vca {
// {DF2108EA-B0A9-4c9c-A2B8-065694BF55AE}
    VINTERFACE_TYPEINFO_DEFINITION (
	IBinding,
	0xdf2108ea, 0xb0a9, 0x4c9c, 0xa2, 0xb8, 0x6, 0x56, 0x94, 0xbf, 0x55, 0xae
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IBinding, GetNamespace, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IBinding, GetName, 1);
}
