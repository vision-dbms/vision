/*****  Vca_ISSHConnectionFactory Implementation  *****/

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

#define   Vca_ISSHConnectionFactory
#include "Vca_ISSHConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

VINTERFACE_TEMPLATE_EXPORTS (Vca::ISSHConnectionFactory)

 namespace Vca {
// {943f527c-f463-4c4d-aa02-9a996cb9dc87}
    VINTERFACE_TYPEINFO_DEFINITION (
	ISSHConnectionFactory,
        0x943f527c, 0xf463, 0x4c4d, 0xaa, 0x02, 0x9a, 0x99, 0x6c, 0xb9, 0xdc, 0x87
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (ISSHConnectionFactory, MakeSSHConnection, 0);
}
