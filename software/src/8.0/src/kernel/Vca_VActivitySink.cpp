/*****  Vca_VActivitySink Implementation  *****/

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

#include "Vca_VActivitySink.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivityMonitor.h"


/********************************
 ********************************
 *****                      *****
 *****  Vca::VActivitySink  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivitySink::VActivitySink (VActivityModel *pModel) : m_pModel (pModel ? pModel : new VActivityModel ()), m_pIActivitySink (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivitySink::~VActivitySink () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivitySink::OnActivity (
    IActivitySink* pRole, IActivity* pInterface, IActivity* pProxy, VTime const& rStartTime, VTime const& rExpectedEndTime, VString const& rDescription
) {
    ActivityRecord::Reference pActivity;
    m_pModel->createActivity (pActivity, this, pInterface, pProxy, rStartTime, rExpectedEndTime, rDescription);
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivitySink::createMonitorFor (VActivity* pActivity) const {
}
