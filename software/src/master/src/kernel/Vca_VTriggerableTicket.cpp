/*****  Vca_VTriggerableTicket Implementation  *****/

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

#include "Vca_VTriggerableTicket.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::VTriggerableTicket::List  *****
 *****                                 *****
 *******************************************
 *******************************************/

void Vca::VTriggerableTicket::List::triggerAll () {
    VTriggerableTicket::Reference pUntriggeredTickets (head ());
    while (pUntriggeredTickets) {
	pUntriggeredTickets->triggerFromList (pUntriggeredTickets);
    }
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VTriggerableTicket  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTriggerableTicket::VTriggerableTicket (
    VReferenceable *pListOwner, List &rList
) : BaseClass (pListOwner, rList) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VTriggerableTicket::~VTriggerableTicket () {
}

/************************
 ************************
 *****  Triggering  *****
 ************************
 ************************/

void Vca::VTriggerableTicket::triggerFromList (ThisClass::Reference &rpUntriggeredTickets) {
//  If this ticket isn't active, ...
    if (isntActive ())
	rpUntriggeredTickets.setTo (successor ());	//  ... just update the list of untriggered tickets.

//  Otherwise, ...
    else {
	Reference iRetainer (this);			//  ... protect this ticket from reclamation,
	rpUntriggeredTickets.setTo (successor ());	//  ... update the list of untriggered tickets,
	trigger ();					//  ... and trigger the callback.
    }
}
