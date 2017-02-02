/*****  Vca::VMessageScheduler Implementation  *****/

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

#include "Vca_VMessageScheduler.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VMessage.h"


/************************************
 ************************************
 *****                          *****
 *****  Vca::VMessageScheduler  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMessageScheduler::VMessageScheduler () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VMessageScheduler::~VMessageScheduler () {
}


/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

bool Vca::VMessageScheduler::mustSchedule (VCohort *pCohort, VMessageStarter *pStarter) {
    m_pStarter.setTo (pStarter);
    return !m_pCohort.setTry (pCohort);
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void Vca::VMessageScheduler::schedule (VMessage *pMessage) {
    pMessage->resumeUsing (m_pStarter);
}
