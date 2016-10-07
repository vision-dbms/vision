/*****  VcaSamples_IPublication Implementation  *****/
#define VcaSamples_IPublication

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

#include "vca_samples_ipublication.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************************
 **************************************
 *****	   	                  *****
 *****  VcaSamples::IPublication  *****
 *****                            *****
 **************************************
 **************************************/

VINTERFACE_TEMPLATE_EXPORTS (VcaSamples::IPublication)

namespace VcaSamples {
// {33AC051E-EC3D-4e14-8E9F-BD486FF30FB4}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPublication,
	0x33ac051e, 0xec3d, 0x4e14, 0x8e, 0x9f, 0xbd, 0x48, 0x6f, 0xf3, 0xf, 0xb4
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPublication, Subscribe, 0);
}
