/*****  Vca_IStdPipeSourceClient Implementation  *****/

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

#define   Vca_IStdPipeSourceClient
#include "Vca_IStdPipeSourceClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::IStdPipeSourceClient  *****
 *****                             *****
 ***************************************
 ***************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IStdPipeSourceClient)

namespace Vca {
// {5AF033A3-88E4-441b-84B7-C1080C0EE977}
    VINTERFACE_TYPEINFO_DEFINITION (
	IStdPipeSourceClient,
	0x5af033a3, 0x88e4, 0x441b, 0x84, 0xb7, 0xc1, 0x8, 0xc, 0xe, 0xe9, 0x77
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IStdPipeSourceClient, OnData, 0);
}
