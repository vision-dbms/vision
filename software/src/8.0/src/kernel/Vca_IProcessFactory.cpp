/*****  Vca_IProcessFactory Implementation  *****/

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

#define   Vca_IProcessFactory
#include "Vca_IProcessFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vca::IProcessFactory  *****
 *****                        *****
 **********************************
 **********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IProcessFactory)

namespace Vca {
// {A34C0A07-27BA-418a-B4DF-F28D87EC5795}
    VINTERFACE_TYPEINFO_DEFINITION (
	IProcessFactory,
	0xa34c0a07, 0x27ba, 0x418a, 0xb4, 0xdf, 0xf2, 0x8d, 0x87, 0xec, 0x57, 0x95
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IProcessFactory, MakeProcess, 0);
}
