#ifndef Vps_IMessageSink_Interface
#define Vps_IMessageSink_Interface

#ifndef Vps_IMessageSink
#define Vps_IMessageSink extern
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

#include "Vps_IMessage.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vps,Vca::Vps,IMessageSink)

/*************************
 *****  Definitions  *****
 *************************/

/*==========================================================================*
    OnMessage (
	IMessage *pMessage, VString const &rData
    );
 *==========================================================================*/

namespace Vca {
    namespace Vps {
	VpsINTERFACE_NEST (IMessageSink, IVReceiver<VString const&>)

	    VINTERFACE_ROLE_NEST
		VINTERFACE_ROLE_2 (OnMessage, IMessage*, VString const&);
	    VINTERFACE_ROLE_NEST_END

	    VINTERFACE_METHOD_2 (OnMessage, IMessage*, VString const&);
	VINTERFACE_END
    }
}


#endif
