#ifndef VcaSamples_IPublication_Interface
#define VcaSamples_IPublication_Interface

#ifndef VcaSamples_IPublication
#define VcaSamples_IPublication extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "vca_samples.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

DECLARE_VcaSamplesINTERFACE (ISubscription)

VINTERFACE_TEMPLATE_INSTANTIATIONS (VcaSamples,IPublication)

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

namespace VcaSamples {
    typedef IVReceiver<ISubscription*> ISubscriber;
    typedef Vca::IClient IRecipient;

    VINTERFACE_ROLE (IPublication, IVUnknown)
	VINTERFACE_ROLE_3 (Subscribe, ISubscriber*, IRecipient*, bool);
    VINTERFACE_ROLE_END

    VcaSamplesINTERFACE (IPublication, IVUnknown);
	VINTERFACE_METHOD_3 (Subscribe, ISubscriber*, IRecipient*, bool);
    VINTERFACE_END
}


#endif
