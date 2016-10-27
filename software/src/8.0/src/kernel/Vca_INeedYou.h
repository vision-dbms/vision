#ifndef Vca_INeedYou_Interface
#define Vca_INeedYou_Interface

#ifndef Vca_INeedYou
#define Vca_INeedYou extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IAckReceiver.h"
#include "Vca_IPassiveCall.h"
#include "Vca_IPassiveCallback.h"


/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,INeedYou)

namespace Vca {
    typedef IVReceiver<IPassiveCallback*> ICallbackReceiver;

    //****************************************************************************************
    //
    //  PleaseCall (
    //        IAckReceiver*                  pAckReceiver
    //        IVReceiver<IPassiveCallback*>* pCallbackReceiver,
    //        IPassiveCall*                  pCallbackRequest,
    //        VString const&                 rCallbackAddress,
    //        uuid_t const&                  rCallbackID,
    //        uuid_t const&                  rTargetID
    //	);
    //
    //  Parameters:
    //        pAckReceiver      ... a pointer to an IAckReceiver interface to which either an
    //                              acknowledgement of successful processing or an error
    //                              should be sent.  Usually implemented independently
    //                              by the initiator and forwarders of this request to
    //                              receive positive confirmation that their request
    //                              has been fully processed by all of the peers to
    //                              which the request has been delegated.
    //        pCallbackReceiver ... a pointer to an IPassiveCallback receiver (i.e.,
    //                              a IVReceiver<IPassiveCallback*>*).  This argument is
    //                              optional (NULL if not required).  If specified, the
    //                              target peer will use it to return a pointer to the
    //                              IPassiveCallback interface of the object it is using
    //                              to respond to this request.  Of potential use for
    //                              querying application objects and negotiating additional
    //                              connectivity options such as authentication.
    //        pCallbackRequest  ... a pointer to the passive call interface of the object
    //                              used to initiate this request.  Of potential use for
    //                              negotiating addtional connectivity options such as
    //                              authentication.
    //        rCallbackAddress	... the network address to which the callback should be
    //                              sent.
    //        rCallbackID       ... the ID of this callback as supplied by the callback
    //                              requestor.
    //        rTargetID         ... the ID of the peer targeted by this request.
    //
    //****************************************************************************************

    VINTERFACE_ROLE (INeedYou, IClient)
	VINTERFACE_ROLE_6 (PleaseCall, IAckReceiver*, ICallbackReceiver*, IPassiveCall*, VString const&, uuid_t const&, uuid_t const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (INeedYou, IClient);
	VINTERFACE_METHOD_6 (PleaseCall, IAckReceiver*, ICallbackReceiver*, IPassiveCall*, VString const&, uuid_t const&, uuid_t const&);
    VINTERFACE_END
}

#endif
