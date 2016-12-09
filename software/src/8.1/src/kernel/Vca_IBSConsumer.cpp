/*****  Vca_IBSConsumer Implementation  *****/

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

#define   Vca_IBSConsumer
#include "Vca_IBSConsumer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IBSConsumer  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IBSConsumer)

namespace Vca {
    // {50264F18-EF77-485b-91E5-33C6F1DEF768}
    VINTERFACE_TYPEINFO_DEFINITION (
	IBSConsumer,
	0x50264f18, 0xef77, 0x485b, 0x91, 0xe5, 0x33, 0xc6, 0xf1, 0xde, 0xf7, 0x68
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IBSConsumer, Put, 0);
}
