/*****  Vca_IListenerClient Implementation  *****/

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

#define   Vca_IListenerClient
#include "Vca_IListenerClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vca::IListenerClient  *****
 *****                        *****
 **********************************
 **********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IListenerClient)

namespace Vca {
    // {2E03E086-22E2-4c29-B13B-5A0758B356C5}
    VINTERFACE_TYPEINFO_DEFINITION (
	IListenerClient,
	0x2e03e086, 0x22e2, 0x4c29, 0xb1, 0x3b, 0x5a, 0x7, 0x58, 0xb3, 0x56, 0xc5
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IListenerClient, OnListener, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IListenerClient, OnConnection, 1);
}
