/*****  Vsa_IUpDownPublisher Implementation  *****/
#define Vsa_IUpDownPublisher

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

#include "Vsa_IUpDownPublisher.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************************
 ***********************************
 *****	   	               *****
 *****  Vsa::IUpDownPublisher  *****
 *****			       *****
 ***********************************
 ***********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IUpDownPublisher)

namespace Vsa {
// {9DE8DB8F-C7AD-4836-9AE7-282F52129073}
    VINTERFACE_TYPEINFO_DEFINITION (
	IUpDownPublisher,
	0x9de8db8f, 0xc7ad, 0x4836, 0x9a, 0xe7, 0x28, 0x2f, 0x52, 0x12, 0x90, 0x73
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IUpDownPublisher, Subscribe, 0);
}
