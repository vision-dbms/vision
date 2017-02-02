#ifndef Vca_VSubscriptionManager_Interface
#define Vca_VSubscriptionManager_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IRequest.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VSubscriptionManager : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VSubscriptionManager, VReferenceable);

    //  Aliases
    public:
	typedef VReferenceable Subscription;

    //  Ticket
    public:
	class Ticket;

    //  Construction
    protected:
	VSubscriptionManager ();

    //  Destruction
    protected:
	~VSubscriptionManager ();

    //  Ticket Generation
    protected:
	void makeTicket (IRequest::Reference& rpTicketReceiver, Subscription* pSubscription);
	void makeTicket (IVReceiver<IRequest*>* pTicketReceiver, Subscription* pSubscription);

    //  Ticket Processing
    private:
	virtual void cancel_(Subscription const* pSubscription) = 0;
    public:
	void cancel (Subscription const* pSubscription);
    };
}


#endif
