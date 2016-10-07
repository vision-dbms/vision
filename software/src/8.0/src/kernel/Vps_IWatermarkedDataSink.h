#ifndef Vps_IWatermarkedDataSink_Interface
#define Vps_IWatermarkedDataSink_Interface

#ifndef Vps_IWatermarkedDataSink
#define Vps_IWatermarkedDataSink extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "IVReceiver.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,IWatermarkedDataSink)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    OnData (
	VString const &rWatermark, VString const &rData
    );
 *==========================================================================*/

namespace Vca {
    namespace Vps {
	VpsINTERFACE_NEST (IWatermarkedDataSink, IVReceiver<VString const&>)

	    VINTERFACE_ROLE_NEST
		VINTERFACE_ROLE_2 (OnWatermarkedData, VString const&, VString const&);
	    VINTERFACE_ROLE_NEST_END

	    VINTERFACE_METHOD_2 (OnWatermarkedData, VString const&, VString const&);
	VINTERFACE_END
    }
}


#endif
