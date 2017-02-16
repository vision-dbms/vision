/*****  Vca_VGofer Implementation  *****/

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

#include "Vca_VGofer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VGofering.h"

#include "Vca_VcaThreadState.h"

#include "V_VRTTI.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::VGofering  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VGofering::VGofering (VGofer* pGofer) : m_pGofer (pGofer) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VGofering::~VGofering () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VGofering::getDescription_(VString& rResult) const {
    m_pGofer->getDescription (rResult);
}


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VGofer::RetrySettings  *****
 *****                              *****
 ****************************************
 ****************************************/

Vca::VGofer::RetrySettings::RetrySettings (count_t cRetries, delay_t sDelay1, delay_t sDelay2) {
    setTo (cRetries, sDelay1, sDelay2);
}

Vca::VGofer::RetrySettings::RetrySettings (ThisClass const& rOther) {
    setTo (rOther);
}

Vca::VGofer::RetrySettings::RetrySettings () : m_cRetries (0), m_sDelay1 (0), m_sDelay2 (0) {
}

Vca::VGofer::RetrySettings::~RetrySettings () {
}

void Vca::VGofer::RetrySettings::setTo (ThisClass const& rOther) {
    m_cRetries = rOther.retryCount ();
    m_sDelay1 = rOther.delay1 ();
    m_sDelay2 = rOther.delay2 ();
}

void Vca::VGofer::RetrySettings::setTo (count_t cRetries, delay_t sDelay1, delay_t sDelay2) {
    m_cRetries = cRetries;
    m_sDelay1 = sDelay1;
    m_sDelay2 = sDelay2;
}

void Vca::VGofer::RetrySettings::getTimer (timer_t::Reference& rpTimer, ITrigger* pTrigger) const {
    rpTimer.setTo (
	new timer_t (
	    pTrigger, static_cast<microseconds_t>(m_sDelay1) * 1000, static_cast<microseconds_t>(m_sDelay2) * 1000
	)
    );
}


/*************************
 *************************
 *****               *****
 *****  Vca::VGofer  *****
 *****               *****
 *************************
 *************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

bool Vca::VGofer::g_bKeepingErrors = V::GetEnvironmentBoolean ("VcaGofersKeepErrors");
bool Vca::VGofer::g_bTracingGofers = V::GetEnvironmentBoolean ("TracingVcaGofers");

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VGofer::VGofer () : m_cDependencies (1), m_bTriggered (false) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VGofer::~VGofer () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VGofer::getDescription_(VString &rDescription) const {
    V::VRTTI iRTTI (typeid (*this));
    rDescription << iRTTI.name ();
}

/*********************************
 *********************************
 *****  Gofering (Activity)  *****
 *********************************
 *********************************/

Vca::VGofering* Vca::VGofer::newGofering () {
    Gofering::Reference const pNewGofering (VActivity::TrackingActivities () ? newGofering_() : 0);
    if (pNewGofering) {
//	VString iD;
//	getDescription (iD);
//	fprintf (stderr, "+++ %p: Start %s\n", this, iD.content ());

	pNewGofering->onActivityStart ();
    }
    return pNewGofering;
}

Vca::VGofering* Vca::VGofer::newGofering_() {
    return 0;
}

void Vca::VGofer::onGoferingError (IError* pInterface, VString const& rMessage) {
    if (m_pGofering) {
	VString iD;
	getDescription (iD);
// 	fprintf (stderr, "+++ %p: Error %s\n", this, iD.content ());

	m_pGofering->onActivityError (pInterface, rMessage);
    }
}

void Vca::VGofer::onGoferingFailure (IError* pInterface, VString const& rMessage) {
    if (m_pGofering) {
	VString iD;
	getDescription (iD);
// 	fprintf (stderr, "+++ %p: Fail  %s\n", this, iD.content ());

	m_pGofering->onActivityFailure (pInterface, rMessage);
	m_pGofering.clear ();
    }
}

void Vca::VGofer::onGoferingSuccess () {
    if (m_pGofering) {
	VString iD;
	getDescription (iD);
// 	fprintf (stderr, "+++ %p: Succd %s\n", this, iD.content ());

	m_pGofering->onActivitySuccess ();
	m_pGofering.clear ();
    }
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VGofer::OnError_(IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}

void Vca::VGofer::onNeed () {
    if (m_bTriggered.setIfClear ()) {
	if (TracingGofers ()) {
	    VString iD;
	    getDescription (iD);
	    fprintf (stderr, "+++ %s: onNeed\n", iD.content ());
	}
	m_pGofering = newGofering ();
	resume ();
    }
}

void Vca::VGofer::onReset () {
}

void Vca::VGofer::onValueNeeded () {
    onNeed ();
}

void Vca::VGofer::onResetNeeded () {
    resetGofer ();
}

void Vca::VGofer::suspend () {
    m_cDependencies.increment ();
}

void Vca::VGofer::resume () {
    if (m_cDependencies.decrementIsZero ()) {
	onData ();
    }
}

void Vca::VGofer::resetGofer () {
    if (m_bTriggered.clearIfSet ()) {
	m_cDependencies.setTo (1);
	onReset ();
    }
}

bool Vca::VGofer::validatesError (IError *pInterface, VString const &rMessage) const {
    return KeepingErrors ();
}
