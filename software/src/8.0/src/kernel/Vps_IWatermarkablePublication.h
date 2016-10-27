#ifndef Vps_IWatermarkablePublication_Interface
#define Vps_IWatermarkablePublication_Interface

#ifndef Vps_IWatermarkablePublication
#define Vps_IWatermarkablePublication extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vps_IPublication.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vps_IWatermarkedDataSink.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,IWatermarkablePublication)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    SubscribeWithWatermark (
	IVReceiver<ISubscription*>* pSubscriber,
	IWatermarkedDataSink*	    pRecipient,
	VString const&		    rWatermark,
	bool			    bSuspended
    );
 *==========================================================================*/

namespace Vca {
    namespace Vps {
	VINTERFACE_ROLE (IWatermarkablePublication, IPublication)
	    VINTERFACE_ROLE_4 (SubscribeWithWatermark, ISubscriber*, IWatermarkedDataSink*, VString const&, bool);
	VINTERFACE_ROLE_END

	VpsINTERFACE (IWatermarkablePublication, IPublication);
	    VINTERFACE_METHOD_4 (SubscribeWithWatermark, ISubscriber*, IWatermarkedDataSink*, VString const&, bool);
	VINTERFACE_END
    }
}


#endif
