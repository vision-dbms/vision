/*****  Vdht_VHT Implementation  *****/

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

#include "Vdht_VHT.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vdht::VHT::Entry::Authority::TicketSink  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

namespace Vdht {
    class VHT::Entry::Authority::TicketSink : virtual public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (TicketSink, VRolePlayer);

    //  Construction
    public:
	TicketSink (Authority *pAuthority);

    //  Destruction
    private:
	~TicketSink ();

    //  IRegistrationTicketSink Role
    private:
	Vca::VRole<ThisClass,IRegistrationTicketSink> m_pIRegistrationTicketSink;
    public:
	void getRole (IRegistrationTicketSink::Reference &rpRole) {
	    m_pIRegistrationTicketSink.getRole (rpRole);
	}

    //  IRegistrationTicketSink Callbacks
    public:
	void OnData (IRegistrationTicketSink *pRole, IRegistrationTicket *pTicket);

    //  State
    private:
	Authority::Reference const m_pAuthority;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vdht::VHT::Entry::Authority::TicketSink::TicketSink (
    Authority *pAuthority
) : m_pAuthority (pAuthority), m_pIRegistrationTicketSink (this) {
    retain (); {
	IRegistration::Reference pRegistration;
	pAuthority->getRole (pRegistration);

	IRegistrationTicketSink::Reference pMySelf;
	getRole (pMySelf);

//	pRegistrant->SupplyEvents (pMySelf, pRegistration);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vdht::VHT::Entry::Authority::TicketSink::~TicketSink () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vdht::VHT::Entry::Authority::TicketSink::OnData (IRegistrationTicketSink *pRole, IRegistrationTicket *pTicket) {
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vdht::VHT::Entry::Authority  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vdht::VHT::Entry::Authority::Authority (
    VHT *pHT, Entry *pEntry, VAuthority const &rAuthority
) : m_pHT (pHT), m_pEntry (pEntry), m_pIRegistration (this) {
    (new TicketSink (this))->discard ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vdht::VHT::Entry::Authority::~Authority () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vdht::VHT::Entry::Authority::Revoke (IRegistration *pRole) {
    m_pEntry->onRevocationOf (this);
}


/******************************
 ******************************
 *****                    *****
 *****  Vdht::VHT::Entry  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vdht::VHT::Entry::Entry (
    VHT *pHT, VAuthority const &rAuthority, IVUnknown *pObject
) : m_pObject (pObject) {
    retain (); {
	addAuthority (pHT, rAuthority);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vdht::VHT::Entry::~Entry () {
}

/***********************
 ***********************
 *****  Authority  *****
 ***********************
 ***********************/

void Vdht::VHT::Entry::addAuthority (VHT *pHT, VAuthority const &rAuthority) {
//     unsigned int xElement;
//     if (m_iAuthorityMap.Insert (rAuthority, xElement)) {
//     }
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vdht::VHT::Entry::onRevocationOf (Authority *pAuthority) {
}


/***********************
 ***********************
 *****             *****
 *****  Vdht::VHT  *****
 *****             *****
 ***********************
 ***********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vdht::VHT::VHT () : m_pIRegistry (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vdht::VHT::~VHT () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*****************************
 *****  Vdht::IRegistry  *****
 *****************************/

void Vdht::VHT::RegisterObject (IRegistry *pRole, VAuthority const &rAuthority, IVUnknown *pObject) {
    unsigned int xElement;
    if (m_iEntryMap.Insert (pObject, xElement)) {
	m_iEntryMap[xElement].setTo (new Entry (this, rAuthority, pObject));
    } else {
	m_iEntryMap[xElement]->addAuthority (this, rAuthority);
    }
}
