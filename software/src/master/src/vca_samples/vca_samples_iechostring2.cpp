/*****  VcaSamples_IEchoString2 Implementation  *****/
#define VcaSamples_IEchoString2

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

#include "vca_samples_iechostring2.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************************
 **************************************
 *****	   	                  *****
 *****  VcaSamples::IEchoString2  *****
 *****                            *****
 **************************************
 **************************************/

VINTERFACE_TEMPLATE_EXPORTS (VcaSamples::IEchoString2)

namespace VcaSamples {
// {92314C89-6DA4-4bc9-88AF-70E57E7BF060}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEchoString2,
	0x92314c89, 0x6da4, 0x4bc9, 0x88, 0xaf, 0x70, 0xe5, 0x7e, 0x7b, 0xf0, 0x60
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEchoString2, GetChild, 0);
}
