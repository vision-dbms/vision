/*****  Vca_INamespace Implementation  *****/

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

#define   Vca_INamespace
#include "Vca_INamespace.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****                   *****
 *****  Vca::INamespace  *****
 *****                   *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::INamespace)

namespace Vca {
// {F642500F-62C4-433d-8646-63C39DB8C63E}
    VINTERFACE_TYPEINFO_DEFINITION (
	INamespace,
	0xf642500f, 0x62c4, 0x433d, 0x86, 0x46, 0x63, 0xc3, 0x9d, 0xb8, 0xc6, 0x3e
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (INamespace, GetBinding, 0);
}
