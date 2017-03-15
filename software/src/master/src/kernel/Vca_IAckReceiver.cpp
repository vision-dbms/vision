/*****  Vca_IAckReceiver Implementation  *****/

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

#define   Vca_IAckReceiver
#include "Vca_IAckReceiver.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::IAckReceiver  *****
 *****                     *****
 *******************************
 *******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IAckReceiver)

namespace Vca {
// {9AE38416-B71F-44bb-8567-7D762DF6752D}
    VINTERFACE_TYPEINFO_DEFINITION (
	IAckReceiver,
	0x9ae38416, 0xb71f, 0x44bb, 0x85, 0x67, 0x7d, 0x76, 0x2d, 0xf6, 0x75, 0x2d
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IAckReceiver, Ack, 0);
}
