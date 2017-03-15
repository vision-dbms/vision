/*****  Vca_VTriggerTicket Implementation  *****/

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

#include "Vca_VTriggerTicket.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VTriggerTicket  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTriggerTicket::VTriggerTicket (
    VReferenceable *pListOwner, List &rList, ITrigger *pTrigger, bool bTriggered
) : BaseClass (pListOwner, rList), m_pTrigger (pTrigger) {
    VCohort::DefaultLogger().printf ("+++ %p: Callback %p created.\n", this, pTrigger);
    if (bTriggered && isActive ()) {
	retain (); {
	    trigger ();
	} untain ();
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VTriggerTicket::~VTriggerTicket () {
}

/********************
 ********************
 *****  Firing  *****
 ********************
 ********************/

void Vca::VTriggerTicket::trigger () const {
    VCohort::DefaultLogger().printf ("+++ %p: Callback %p triggered.\n", this, m_pTrigger.referent ());
    if (m_pTrigger)
	m_pTrigger->Process ();
}
