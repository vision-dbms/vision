/*****  Vca_VSubscriptionManager Implementation  *****/

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

#include "Vca_VSubscriptionManager.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VRolePlayer.h"


/***********************************************
 ***********************************************
 *****                                     *****
 *****  Vca::VSubscriptionManager::Ticket  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

namespace Vca {
    class VSubscriptionManager::Ticket : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Ticket, VRolePlayer);

    //  Construction
    public:
	Ticket (VSubscriptionManager* pManager, Subscription* pSubscription);

    //  Destruction
    private:
	~Ticket () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IRequest Role
    private:
	VRole<ThisClass,IRequest> m_pIRequest;
    public:
	void getRole (IRequest::Reference& rpRole) {
	    m_pIRequest.getRole (rpRole);
	}

    //  IRequest Implementation
    public:
	void Cancel (IRequest* pRole);

    //  State
    private:
	VSubscriptionManager::Reference const m_pManager;
	Subscription::Reference const m_pSubscription;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSubscriptionManager::Ticket::Ticket (
    VSubscriptionManager* pManager, Subscription* pSubscription
) : m_pManager (pManager), m_pSubscription (pSubscription), m_pIRequest (this) {
}

void Vca::VSubscriptionManager::Ticket::Cancel (IRequest* pRole) {
    m_pManager->cancel (m_pSubscription);
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::VSubscriptionManager  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSubscriptionManager::VSubscriptionManager () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VSubscriptionManager::~VSubscriptionManager () {
}

/*******************************
 *******************************
 *****  Ticket Generation  *****
 *******************************
 *******************************/

void Vca::VSubscriptionManager::makeTicket (IRequest::Reference& rpTicketReceiver, Subscription* pSubscription) {
    (new Ticket (this, pSubscription))->getRole (rpTicketReceiver);
}

void Vca::VSubscriptionManager::makeTicket (IVReceiver<IRequest*>* pTicketReceiver, Subscription* pSubscription) {
    if (pTicketReceiver) {
	IRequest::Reference pTicket;
	makeTicket (pTicket, pSubscription);
	pTicketReceiver->OnData (pTicket);
    }
}

/*******************************
 *******************************
 *****  Ticket Processing  *****
 *******************************
 *******************************/

void Vca::VSubscriptionManager::cancel (Subscription const* pSubscription) {
    cancel_(pSubscription);
}
