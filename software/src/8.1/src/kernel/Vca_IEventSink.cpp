/*****  Vca_IEventSink Implementation  *****/

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

#define   Vca_IEventSink
#include "Vca_IEventSink.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****                   *****
 *****  Vca::IEventSink  *****
 *****                   *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IEventSink)

namespace Vca {
// {3D3A797C-35AD-4713-A2C8-F29DFB452339}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEventSink,
	0x3d3a797c, 0x35ad, 0x4713, 0xa2, 0xc8, 0xf2, 0x9d, 0xfb, 0x45, 0x23, 0x39
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IEventSink, OnEvent, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEventSink, OnEndEvent, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEventSink, OnErrorEvent, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEventSink, OnFailureEvent, 3);
}
