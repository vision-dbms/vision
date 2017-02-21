/*****  Vca_IClient Implementation  *****/

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

#define   Vca_IClient
#include "Vca_IClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**************************
 **************************
 *****                *****
 *****  Vca::IClient  *****
 *****                *****
 **************************
 **************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IClient)

namespace Vca {
// {85FDC734-9242-432c-A416-BDED68C738F7}
    VINTERFACE_TYPEINFO_DEFINITION (
	IClient,
	0x85fdc734, 0x9242, 0x432c, 0xa4, 0x16, 0xbd, 0xed, 0x68, 0xc7, 0x38, 0xf7
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IClient, OnEnd, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IClient, OnError, 0);
}
