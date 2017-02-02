/*****  Vps_IWatermarkablePublication Implementation  *****/
#define Vps_IWatermarkablePublication

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

#include "Vps_IWatermarkablePublication.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::Vps::IWatermarkablePublication  *****
 *****                                       *****
 *************************************************
 *************************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::IWatermarkablePublication)

namespace Vca {
    namespace Vps {

    // {7133E3EE-3AE0-4c3f-AED9-408C021FEF24}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IWatermarkablePublication,
	    0x7133e3ee, 0x3ae0, 0x4c3f, 0xae, 0xd9, 0x40, 0x8c, 0x2, 0x1f, 0xef, 0x24
	);

	VINTERFACE_MEMBERINFO_DEFINITION (IWatermarkablePublication, SubscribeWithWatermark, 0);
    }
}
