/*****  Vps_IMessageSink Implementation  *****/
#define Vps_IMessageSink

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

#include "Vps_IMessageSink.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vca::Vps::IMessageSink  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::IMessageSink)

namespace Vca {
    namespace Vps {

    // {759F0A3C-7210-4991-8545-572A2EF31AB2}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IMessageSink,
	    0x759f0a3c, 0x7210, 0x4991, 0x85, 0x45, 0x57, 0x2a, 0x2e, 0xf3, 0x1a, 0xb2
	);

	VINTERFACE_MEMBERINFO_DEFINITION (IMessageSink, OnMessage, 0);
    }
}
