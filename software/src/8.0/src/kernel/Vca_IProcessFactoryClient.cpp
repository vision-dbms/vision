/*****  Vca_IProcessFactoryClient Implementation  *****/

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

#define   Vca_IProcessFactoryClient
#include "Vca_IProcessFactoryClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::IProcessFactoryClient  *****
 *****                              *****
 ****************************************
 ****************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IProcessFactoryClient)

namespace Vca {
// {577CB33A-8450-4281-853A-D566C8613450}
    VINTERFACE_TYPEINFO_DEFINITION (
	IProcessFactoryClient,
	0x577cb33a, 0x8450, 0x4281, 0x85, 0x3a, 0xd5, 0x66, 0xc8, 0x61, 0x34, 0x50
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IProcessFactoryClient, OnData, 0);
}
