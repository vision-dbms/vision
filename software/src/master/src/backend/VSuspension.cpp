/*****  VSuspension Implementation  *****/

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

#include "VSuspension.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VTask.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/*************************
 *************************
 *****               *****
 *****  VSuspension  *****
 *****               *****
 *************************
 *************************/

/***********************************
 ***********************************
 *****  Description Utilities  *****
 ***********************************
 ***********************************/

char const* VSuspension::NameFor (State xElement) {
    switch (xElement) {
    case Aborted:
	return "Aborted";
    case Resumed:
	return "Resumed";
    case MustAbort:
	return "MustAbort";
    case WaitingToAbort:
	return "WaitingToAbort";
    case WaitingToResume:
	return "WaitingToResume";
    default:
	return string ("#%d???", xElement);
    }
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSuspension);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_ciref_c<VSuspension, VDescriptor>;
template class Vsi_b0_c<VSuspension>;
template class Vsi_uiref_c<VSuspension, VComputationUnit*>;
template class Vsi_b0<VSuspension>;
template class Vsi_f0_c<VSuspension, char const*>;
template class Vsi_setDucFeathers<VSuspension>;

void VSuspension::MetaMaker () {
    static Vsi_ciref_c<VSuspension, VDescriptor> const
	si_datum		(&VSuspension::cdatum);

    static Vsi_b0_c<VSuspension> const
	si_datumAlterable	(&VSuspension::datumAlterable),
	si_datumAvailable	(&VSuspension::datumAvailable);

    static Vsi_uiref_c<VSuspension, VComputationUnit*> const
	si_suspendee		(&VSuspension::suspendee);

    static Vsi_b0<VSuspension> const
	si_abort		(&VSuspension::abort),
	si_commit		(&VSuspension::commit),
	si_resume		(&VSuspension::resume);

    static Vsi_b0_c<VSuspension> const
	si_committed		(&VSuspension::committed),
	si_uncommitted		(&VSuspension::uncommitted),
	si_canAbort		(&VSuspension::canAbort),
	si_canResume		(&VSuspension::canResume),
	si_mustAbort		(&VSuspension::mustAbort),
	si_waitingToAbort	(&VSuspension::waitingToAbort),
	si_waitingToResume	(&VSuspension::waitingToResume);

    static Vsi_f0_c<VSuspension, char const*> const
	si_stateName		(&VSuspension::stateName);

    static Vsi_setDucFeathers<VSuspension> const
	si_setResultTo;

    CSym ("datum"		)->set (RTT, &si_datum);
    CSym ("datumAlterable"	)->set (RTT, &si_datumAlterable);
    CSym ("datumAvailable"	)->set (RTT, &si_datumAvailable);

    CSym ("suspendee"		)->set (RTT, &si_suspendee);

    CSym ("committed"		)->set (RTT, &si_committed);
    CSym ("uncommitted"		)->set (RTT, &si_uncommitted);
    CSym ("canAbort"		)->set (RTT, &si_canAbort);
    CSym ("canResume"		)->set (RTT, &si_canResume);
    CSym ("waitingToAbort"	)->set (RTT, &si_waitingToAbort);
    CSym ("waitingToResume"	)->set (RTT, &si_waitingToResume);
    CSym ("mustAbort"		)->set (RTT, &si_mustAbort);

    CSym ("stateName"		)->set (RTT, &si_stateName);

    CSym ("abort"		)->set (RTT, &si_abort);
    CSym ("commit"		)->set (RTT, &si_commit);
    CSym ("resume"		)->set (RTT, &si_resume);

    CSym ("setResultTo:"	)->set (RTT, &si_setResultTo);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSuspension::State VSuspension::AppropriateInitialStateFor (VComputationUnit* pSuspendee) {
    switch (pSuspendee->state ()) {
    case VComputationUnit::State_Runnable:
	return WaitingToResume;

    case VComputationUnit::State_Interrupted:
	return WaitingToAbort;

    default:
	return MustAbort;
    }
}

VSuspension::VSuspension (VComputationUnit* pSuspendee, VSuspension* pSuccessor)
: m_pSuspendee		(pSuspendee)
, m_pSuccessor		(pSuccessor)
, m_xState		(AppropriateInitialStateFor (pSuspendee))
{
    pSuspendee->suspend ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSuspension::~VSuspension () {
    commit ();
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VSuspension::datumAlterable () const {
    return m_pSuspendee->datumAlterable_ ();
}

bool VSuspension::datumAvailable () const {
    return m_pSuspendee->datumAvailable_ ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/**********************
 *****  Mandated  *****
 **********************/

unsigned int VSuspension::cardinality_() const {
    return m_pSuspendee->cardinality_();
}

M_CPD* VSuspension::ptoken_() const {
    return m_pSuspendee->ptoken_();
}

/*********************
 *****  General  *****
 *********************/

bool VSuspension::cdatum (VDescriptor& rDatum) const {
    return m_pSuspendee->cdatum (rDatum);
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool VSuspension::commit () {
    switch (m_xState) {
    default:
	return false;

    case MustAbort:
    case WaitingToAbort:
	m_xState = Aborted;
	m_pSuspendee->resumeFailed ();
	break;

    case WaitingToResume:
	m_xState = Resumed;
	m_pSuspendee->resumeOK ();
	break;
    }

    return true;
}

bool VSuspension::abort () {
    return (mustAbort () || setStateToWaitingToAbort ()) && commit ();
}

bool VSuspension::resume () {
    return setStateToWaitingToResume () && commit ();
}

bool VSuspension::setStateToWaitingToAbort () {
    if (canResume ()) {	// ... prevent MustAbort->WaitingToAbort change.
	m_xState = WaitingToAbort;
	return true;
    }

    return false;
}

bool VSuspension::setStateToWaitingToResume () {
    if (canResume ()) {
	m_xState = WaitingToResume;
	return true;
    }

    return false;
}


/********************************
 *****  Duc Feather Update  *****
 ********************************/

bool VSuspension::setDucFeathers (DSC_Pointer& rFeathers, VDescriptor& rValues) const {
    return m_pSuspendee->setDucFeathers (rFeathers, rValues);
}
