/*****  VcaSamples_IEchoString Implementation  *****/
#define VcaSamples_IEchoString

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

#include "vca_samples_iechostring.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****	   	                 *****
 *****  VcaSamples::IEchoString  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (VcaSamples::IEchoString)

namespace VcaSamples {
// {E62CDF44-F7EF-45b0-86AF-7F04C95CD16B}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEchoString,
	0xe62cdf44, 0xf7ef, 0x45b0, 0x86, 0xaf, 0x7f, 0x4, 0xc9, 0x5c, 0xd1, 0x6b
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEchoString, Echo, 0);
}
