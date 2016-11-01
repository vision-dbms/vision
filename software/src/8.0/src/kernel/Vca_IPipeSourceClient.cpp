/*****  Vca_IPipeSourceClient Implementation  *****/

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

#define   Vca_IPipeSourceClient
#include "Vca_IPipeSourceClient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************************
 ************************************
 *****                          *****
 *****  Vca::IPipeSourceClient  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPipeSourceClient)

namespace Vca {
// {5134E8CF-EDF4-4bb0-AC41-8BCC957A9688}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPipeSourceClient,
	0x5134e8cf, 0xedf4, 0x4bb0, 0xac, 0x41, 0x8b, 0xcc, 0x95, 0x7a, 0x96, 0x88
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPipeSourceClient, OnData, 0);
}
