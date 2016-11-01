/*****  Vca_VTriggerSet Implementation  *****/

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

#include "Vca_VTriggerSet.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VTriggerSet  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTriggerSet::VTriggerSet () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VTriggerSet::~VTriggerSet () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/


void Vca::VTriggerSet::trigger () {
    Iterator iterator (*this);
    for (; iterator.isNotAtEnd (); ++iterator) {	
	VReference<ITrigger> pTrigger (*iterator);
	pTrigger->Process ();
    }    
}

/****
 * Note: removeAndTrigger - For each trigger, remove it from the TriggerSet first and then trigger it...
 *       It is safer to use this function rather than {trigger () ,DeleteAll ()} method combination
 *       as the {trigger (),DeleteAll()} can introduce recursive triggering as the triggers are deleted
 *       only after triggering is completed....
 ****/
void Vca::VTriggerSet::removeAndTrigger () {
  Iterator iterator (*this);
  for (; iterator.isNotAtEnd (); ++iterator) {
    VReference<ITrigger> pTrigger (*iterator);
    iterator.Delete ();
    pTrigger->Process ();
  }
}
