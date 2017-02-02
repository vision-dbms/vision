/*****  Vca_IListener Implementation  *****/

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

#define   Vca_IListener
#include "Vca_IListener.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IListener  *****
 *****                  *****
 ****************************
 ****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IListener)

namespace Vca {
    // {60721124-ACDF-4300-9330-B2F8D34AF95A}
    VINTERFACE_TYPEINFO_DEFINITION (
	IListener,
	0x60721124, 0xacdf, 0x4300, 0x93, 0x30, 0xb2, 0xf8, 0xd3, 0x4a, 0xf9, 0x5a
    );

    //  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IListener, SupplyConnections, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IListener, SupplyByteStreams, 1);
}
