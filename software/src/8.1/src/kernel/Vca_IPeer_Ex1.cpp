/*****  Vca_IPeer_Ex1 Implementation  *****/

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

#define   Vca_IPeer_Ex1
#include "Vca_IPeer_Ex1.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IPeer_Ex1  *****
 *****                  *****
 ****************************
 ****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPeer_Ex1)

namespace Vca {
// {CFB679B0-8A54-47bd-B4E7-AFF75FF8E394}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPeer_Ex1, 0xcfb679b0, 0x8a54, 0x47bd, 0xb4, 0xe7, 0xaf, 0xf7, 0x5f, 0xf8, 0xe3, 0x94
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer_Ex1, WeakenExport, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer_Ex1, WeakenImport, 1);

    VINTERFACE_MEMBERINFO_DEFINITION (IPeer_Ex1, GetProcessInfo, 2);
}
