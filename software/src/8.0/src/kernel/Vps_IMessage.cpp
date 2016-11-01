/*****  Vps_IMessage Implementation  *****/
#define Vps_IMessage

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

#include "Vps_IMessage.h"

/************************
 *****  Supporting  *****
 ************************/


/********************************
 ********************************
 *****                      *****
 *****  Vca::Vps::IMessage  *****
 *****                      *****
 ********************************
 ********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::IMessage)

namespace Vca {
    namespace Vps {

    // {1FEB5CBB-73C5-486e-B44D-3C4461522CF5}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IMessage,
	    0x1feb5cbb, 0x73c5, 0x486e, 0xb4, 0x4d, 0x3c, 0x44, 0x61, 0x52, 0x2c, 0xf5
	);

	VINTERFACE_MEMBERINFO_DEFINITION (IMessage, SetWanted, 0);
    }
}
