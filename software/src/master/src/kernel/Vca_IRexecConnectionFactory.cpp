/*****  Vca_IRexecConnectionFactory Implementation  *****/

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

#define   Vca_IRexecConnectionFactory
#include "Vca_IRexecConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


VINTERFACE_TEMPLATE_EXPORTS (Vca::IRexecConnectionFactory)

 namespace Vca {
// {f2d68f52-4579-44d4-b199-445e0d1cec30}
    VINTERFACE_TYPEINFO_DEFINITION (
	IRexecConnectionFactory,
	0xf2d68f52, 0x4579, 0x44d4, 0xb1, 0x99, 0x44, 0x5e, 0x0d, 0x1c, 0xec, 0x30
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IRexecConnectionFactory, MakeRexecConnection, 0);
}
