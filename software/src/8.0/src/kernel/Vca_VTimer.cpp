/*****  Vision Kernel Timer Implementation  *****/

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

#include "Vca_VTimer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"

#include "V_VRandom.h"


/*************************
 *************************
 *****               *****
 *****  Vca::VTimer  *****
 *****               *****
 *************************
 *************************/

/***********************
 ***********************
 *****  Constants  *****
 ***********************
 ***********************/

VString const Vca::VTimer::g_sDefaultPurpose ("UNKNOWN", false);

/***************************
 ***************************
 *****  Construction   *****
 ***************************
 ***************************/

Vca::VTimer::VTimer (VCohort *pCohort, VString const& rPurpose, ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating)
: BaseClass	(pCohort)
, m_iPurpose	(rPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay1)
, m_sDelay2	(sDelay2)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (VCohort *pCohort, VString const& rPurpose, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating)
: BaseClass	(pCohort)
, m_iPurpose	(rPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay)
, m_sDelay2	(sDelay)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (VCohort *pCohort, ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating)
: BaseClass	(pCohort)
, m_iPurpose	(g_sDefaultPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay1)
, m_sDelay2	(sDelay2)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (VCohort *pCohort, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating)
: BaseClass	(pCohort)
, m_iPurpose	(g_sDefaultPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay)
, m_sDelay2	(sDelay)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (VString const& rPurpose, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating)
: m_iPurpose	(rPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay)
, m_sDelay2	(sDelay)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating)
: m_iPurpose	(g_sDefaultPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay1)
, m_sDelay2	(sDelay2)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

Vca::VTimer::VTimer (ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating)
: m_iPurpose	(g_sDefaultPurpose)
, m_pTrigger	(pTrigger)
, m_sDelay1	(sDelay)
, m_sDelay2	(sDelay)
, m_sDelay	(0)
, m_bExpected	(false)
, m_bRepeating	(bRepeating)
{
    defaultLogger().printf ("+++ %p: %s construction\n", this, m_iPurpose.content ());

    traceInfo ("Creating Timer");
}

/*************************
 *************************
 *****  Destruction  *****
  *************************
 *************************/

Vca::VTimer::~VTimer () {
    defaultLogger().printf ("+++ %p: %s destruction\n", this, m_iPurpose.content ());

    traceInfo ("Destroying Timer");
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

size_t Vca::VTimer::adjustedTimeout (size_t sMilliTimeout) const {
    microseconds_t sMicroTTE = timeToExpire ();
    size_t sMilliTTE = sMicroTTE < U64_MAX ? static_cast<size_t>(sMicroTTE / 1000) : (size_t)-1;
    return sMilliTTE < sMilliTimeout ? sMilliTTE : sMilliTimeout;
}

Vca::U64 Vca::VTimer::timeToExpire () const {
    VTime iNow;
    return m_iExpirationTime > iNow ? m_iExpirationTime - iNow : 0;
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VTimer::start (bool bRequested) {
    if (bRequested)
	m_bExpected = true;

    if (!m_bRunning) {
	m_iStartTime.setToNow ();
	m_sDelay = V::VRandom::BoundedValue (m_sDelay1, m_sDelay2);
	m_iExpirationTime = m_iStartTime + m_sDelay;
	cohort ()->attach (this);
    }
}

void Vca::VTimer::stop (bool bRequested) {
    if (bRequested)
	m_bExpected = false;

    Reference iRetainer (this);
    cohort ()->detach (this);
}

/******************************************************************************
 * Routine: cancel
 *  This routine stops the timer and breaks the cyclic reference existing 
 *  between this Timer and the Trigger. Calling this function might end 
 *  up releasing the last reference to the Timer and garbage collecting it
 *******************************************************************************/

void Vca::VTimer::cancel () {
    Reference iRetainer (this);
    stop ();
    m_pTrigger.clear ();
}

void Vca::VTimer::restart () {
    Reference iRetainer (this);
    stop ();
    start ();
}


/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

bool Vca::VTimer::attach (Reference&rpListHead) {
    if (!m_bRunning.setIfClear ())
	return false;

    VTimer *pSuccessor = rpListHead;
    VTimer *pPredecessor = 0;

    while (pSuccessor && pSuccessor->expiresBefore (this)) {
	pPredecessor = pSuccessor;
	pSuccessor = pSuccessor->m_pSuccessor;
    }

    m_pPredecessor.setTo (pPredecessor);
    m_pSuccessor.setTo (pSuccessor);

    if (pPredecessor)
	pPredecessor->m_pSuccessor.setTo (this);
    else
	rpListHead.setTo (this);

    if (pSuccessor)
	pSuccessor->m_pPredecessor.setTo (this);

    return true;
}

bool Vca::VTimer::detach (Reference&rpListHead) {
    if (!m_bRunning.clearIfSet ())
	return false;

    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else
	rpListHead.setTo (m_pSuccessor);

    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);

    m_pPredecessor.clear ();
    m_pSuccessor.clear ();

    return true;
}


/************************
 ************************
 *****  Triggering  *****
 ************************
 ************************/

/******************************************************************************
 * Routine: triggerIfExpired
 *  This routine is called to trigger timers which have expired.
 *  Repeating timers are rescheduled.
 ******************************************************************************/

bool Vca::VTimer::triggerIfExpired () {
    if (!hasExpired ())
	return false;

    //  stop and trigger 
    Reference iRetainer (this);

    stop (false);
    if (m_pTrigger) 
	m_pTrigger->Process ();
    if (m_bRepeating && m_bExpected)
    	start (false);

    return true;
}
