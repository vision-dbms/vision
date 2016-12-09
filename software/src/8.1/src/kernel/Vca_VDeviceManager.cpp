/*****  Vca_VDeviceManager Implementation  *****/

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

#include "Vca_VDeviceManager.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VDeviceManager  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDeviceManager::VDeviceManager (VCohort *pCohort) : BaseClass (pCohort) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDeviceManager::~VDeviceManager () {
}

/*******************************
 *******************************
 *****  Cohort Management  *****
 *******************************
 *******************************/

bool Vca::VDeviceManager::joinCohort () {
    return cohort ()->attach (this);
}
bool Vca::VDeviceManager::exitCohort () {
    return cohort ()->detach (this);
}

/******************************
 ******************************
 *****  Event Management  *****
 ******************************
 ******************************/

void Vca::VDeviceManager::incrementUseCount () {
    if (0 == m_cUses++)
	cohort ()->startEventMonitor ();
}

/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

bool Vca::VDeviceManager::processEvents (size_t sTimeout, bool &rbEventsProcessed) {
    if (hasUses () || sTimeout > 0) {
	rbEventsProcessed = processEvent_(sTimeout);
	return hasUses ();
    }
    rbEventsProcessed = false;
    return false;
}
