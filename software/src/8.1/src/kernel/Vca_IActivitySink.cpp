/*****  Vca_IActivitySink Implementation  *****/

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

#define   Vca_IActivitySink
#include "Vca_IActivitySink.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::IActivitySink  *****
 *****                      *****
 ********************************
 ********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IActivitySink)

namespace Vca {
// {E4E15234-1570-436c-9BB4-F7AE2DDEF354}
    VINTERFACE_TYPEINFO_DEFINITION (
	IActivitySink,
	0xe4e15234, 0x1570, 0x436c, 0x9b, 0xb4, 0xf7, 0xae, 0x2d, 0xde, 0xf3, 0x54
);

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IActivitySink, OnActivity, 0);
}
