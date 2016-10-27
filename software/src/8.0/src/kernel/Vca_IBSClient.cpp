/*****  Vca_IBSClient Implementation  *****/

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

#define   Vca_IBSClient
#include "Vca_IBSClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IBSClient  *****
 *****                  *****
 ****************************
 ****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IBSClient)

namespace Vca {
// {8E26B06D-A3EA-4075-834C-8012FF144D74}
    VINTERFACE_TYPEINFO_DEFINITION (
	IBSClient,
	0x8e26b06d, 0xa3ea, 0x4075, 0x83, 0x4c, 0x80, 0x12, 0xff, 0x14, 0x4d, 0x74
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IBSClient, OnEnd, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IBSClient, OnTransfer, 1);
}
