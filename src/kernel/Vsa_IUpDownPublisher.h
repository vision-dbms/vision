#ifndef Vsa_IUpDownPublisher_Interface
#define Vsa_IUpDownPublisher_Interface

#ifndef Vsa_IUpDownPublisher
#define Vsa_IUpDownPublisher extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

#if defined (_WIN32)
#include "Vsa_IUpDownSubscriber.h"
#endif


/**************************
 *****  Declarations  *****
 **************************/

DECLARE_VsaINTERFACE (IUpDownSubscriber)
DECLARE_VsaINTERFACE (ISubscription)

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IUpDownPublisher)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    Subscribe (Vsa::IUpDownSubscriber*, IVReceiver<Vsa::ISubscription>*);

    Synopsis:
	Subscribe the Vsa::IUpDownSubscriber for the UpDown event changes,
	and return the subscription token to IVReceiver<Vsa::ISubscription>*.
 *==========================================================================*/

namespace Vsa {
    typedef IVReceiver<ISubscription*> ISubscriptionSink;

    VINTERFACE_ROLE (IUpDownPublisher, IVUnknown)
	VINTERFACE_ROLE_2 (Subscribe, IUpDownSubscriber*, ISubscriptionSink*);
    VINTERFACE_ROLE_END

    VsaINTERFACE (IUpDownPublisher, IVUnknown);
	VINTERFACE_METHOD_2 (Subscribe, IUpDownSubscriber*, ISubscriptionSink*);
    VINTERFACE_END
}


#endif
