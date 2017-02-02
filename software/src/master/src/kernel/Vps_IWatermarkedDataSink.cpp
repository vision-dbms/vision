/*****  Vps_IWatermarkedDataSink Implementation  *****/
#define Vps_IWatermarkedDataSink

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

#include "Vps_IWatermarkedDataSink.h"

/************************
 *****  Supporting  *****
 ************************/


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::Vps::IWatermarkedDataSink  *****
 *****                                  *****
 ********************************************
 ********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::IWatermarkedDataSink)

namespace Vca {
    namespace Vps {

    // {F7E15F5B-2682-4698-8671-75775D707024}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IWatermarkedDataSink,
	    0xf7e15f5b, 0x2682, 0x4698, 0x86, 0x71, 0x75, 0x77, 0x5d, 0x70, 0x70, 0x24
	);

	VINTERFACE_MEMBERINFO_DEFINITION (IWatermarkedDataSink, OnWatermarkedData, 0);
    }
}
