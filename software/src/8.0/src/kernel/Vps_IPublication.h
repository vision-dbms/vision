#ifndef Vps_IPublication_Interface
#define Vps_IPublication_Interface

#ifndef Vps_IPublication
#define Vps_IPublication extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vps_ISubscription.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,IPublication)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    Subscribe (
	IVReceiver<ISubscription*>* pSubscriber,
	IClient*		    pRecipient,
	bool			    bSuspended
    );
 *==========================================================================*/

namespace Vca {
    namespace Vps {
        typedef IVReceiver<ISubscription*> ISubscriber;
        typedef Vca::IClient IRecipient;

	VINTERFACE_ROLE (IPublication, IVUnknown)
	    VINTERFACE_ROLE_3 (Subscribe, ISubscriber*, IRecipient*, bool);
	VINTERFACE_ROLE_END

	VpsINTERFACE (IPublication, IVUnknown);
	    VINTERFACE_METHOD_3 (Subscribe, ISubscriber*, IRecipient*, bool);
	VINTERFACE_END
    }
}


#endif
