/*****  Vca_IProcessInfoSink Implementation  *****/

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

#define   Vca_IProcessInfoSink
#include "Vca_IProcessInfoSink.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::IProcessInfoSink  *****
 *****                         *****
 ***********************************
 ***********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IProcessInfoSink)

namespace Vca {
// {B0170334-61C1-473a-89AA-9EFB03416DEB}
    VINTERFACE_TYPEINFO_DEFINITION (
	IProcessInfoSink,
	0xb0170334, 0x61c1, 0x473a, 0x89, 0xaa, 0x9e, 0xfb, 0x3, 0x41, 0x6d, 0xeb
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IProcessInfoSink, OnBasics, 0);
}
