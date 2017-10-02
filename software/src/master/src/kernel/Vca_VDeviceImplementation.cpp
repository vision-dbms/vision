/*****  Vca::VDeviceImplementation Implementation  *****/

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

#include "Vca_VDeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VDeviceImplementation  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDeviceImplementation::VDeviceImplementation (
    VReferenceable *pContainer, VDeviceImplementation const &rOther
) : BaseClass (pContainer), m_cUsers (0), m_cUses (0) {
}

Vca::VDeviceImplementation::VDeviceImplementation (
    VReferenceable *pContainer
) : BaseClass (pContainer), m_cUsers (0), m_cUses (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDeviceImplementation::~VDeviceImplementation () {
}


/******************************
 ******************************
 *****  Error Accounting  *****
 ******************************
 ******************************/

void Vca::VDeviceImplementation::onErrorDetect_(AbstractUse *pUse, VkStatus const &rStatus) {
    VString iUseName;
    if (pUse) {
	V::VRTTI iUseRTTI (typeid(*pUse));
	iUseName.setTo (iUseRTTI.name ());
    }
    VString iStatusMessage;
    rStatus.getDescription (iStatusMessage);

    VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]: %s: %s\n", this, iUseName.content (), iStatusMessage.content ());
}

void Vca::VDeviceImplementation::onErrorDetect (AbstractUse *pUse, VkStatus const &rStatus) {
    onErrorDetect_(pUse, rStatus);
}

/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

void Vca::VDeviceImplementation::onFirstReader () {
    incrementUserCount ();
}
void Vca::VDeviceImplementation::onFinalReader () {
    VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]: onFinalReader\n", this);
    decrementUserCount ();
}

void Vca::VDeviceImplementation::onFirstWriter () {
    incrementUserCount ();
}
void Vca::VDeviceImplementation::onFinalWriter () {
    VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]: onFinalWriter\n", this);
    decrementUserCount ();
}

void Vca::VDeviceImplementation::onFirstListener () {
    incrementUserCount ();
}
void Vca::VDeviceImplementation::onFinalListener () {
    decrementUserCount ();
}

//  ... onUserCountWasZero:
void Vca::VDeviceImplementation::onUserCountWasZero () {
    onUserCountWasZero_();
}
void Vca::VDeviceImplementation::onUserCountWasZero_() {
}

//  ... onUserCountIsZero:
void Vca::VDeviceImplementation::onUserCountIsZero () {
//  Immediately treat devices with no users as unused even if they have I/O operations in progress...
    if (m_cUses > 0) {
	VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]::onUserCountIsZero: use count = %u.\n", this, m_cUses);
	onUseCountIsZero_();
    }
    onUserCountIsZero_();
}
void Vca::VDeviceImplementation::onUserCountIsZero_() {
}

//  ... onUseCountWasZero:
void Vca::VDeviceImplementation::onUseCountWasZero () {
//  Only signal use count changes for devices with users...
    if (m_cUsers > 0)
	onUseCountWasZero_();
    else {
	VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]::onUseCountWasZero: no users.\n", this);
    }
}
void Vca::VDeviceImplementation::onUseCountWasZero_() {
}

//  ... onUseCountIsZero:
void Vca::VDeviceImplementation::onUseCountIsZero () {
//  Only signal use count changes for devices with users...
    if (m_cUsers > 0)
	onUseCountIsZero_();
    else {
	VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%p]:: onUseCountIsZero: no users.\n", this);
    }
}
void Vca::VDeviceImplementation::onUseCountIsZero_() {
}
