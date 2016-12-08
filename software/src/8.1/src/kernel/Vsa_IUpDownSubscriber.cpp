/*****  Vsa_IUpDownSubscriber Implementation  *****/
#define Vsa_IUpDownSubscriber

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

#include "Vsa_IUpDownSubscriber.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****	   	                *****
 *****  Vsa::IUpDownSubscriber  *****
 *****			        *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IUpDownSubscriber)

namespace Vsa {
// {D8E0D956-4DC5-4181-913F-0C18F6170887}
    VINTERFACE_TYPEINFO_DEFINITION (
	IUpDownSubscriber,
	0xd8e0d956, 0x4dc5, 0x4181, 0x91, 0x3f, 0x0c, 0x18, 0xf6, 0x17, 0x08, 0x87
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IUpDownSubscriber, OnUp, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IUpDownSubscriber, OnDown, 1);
}
