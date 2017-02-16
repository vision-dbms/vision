/*****  Vca::VCohort Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VpSocket.h"		//  Needed for waitpid.
#include "VConfig.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VCohort.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VRolePlayer.h"
#include "Vca_VTimer.h"

#include "Vca_VcaThreadState.h"

/*********************
 *****  Logging  *****
 *********************/

/*************************************************/
namespace {
    char const *LogFileName () {
	static char const *pLogFileName = getenv ("VcaDeviceLog");
	static bool bNotInitialized = true;
	if (!pLogFileName && bNotInitialized) {
	    bNotInitialized = false;
	    if (V::GetEnvironmentBoolean ("TracingDevices")) {
		static VString iLogFileName (VString () << "p_" << getpid () << "_device.log");
		pLogFileName = iLogFileName;
	    }
	}
	return pLogFileName;
    }
    V::VLogger g_iDeviceLogger (LogFileName ());
}

V::VLogger &Vca::VCohort::defaultLogger () const {
    return g_iDeviceLogger;
}

V::VLogger &Vca::VCohort::DefaultLogger () {
    return g_iDeviceLogger;
}
/*************************************************/
namespace Vca {
    bool s_bDeviceManagerPollEnabled = V::GetEnvironmentInteger ("VcaDeviceManagerPollEnabled", 1) != 0;

    bool TracingCohortWaitStates () {
	static bool const bTracingCohortWaitStates = V::GetEnvironmentInteger ("VcaTracingCohortWaitStates", 0) != 0;
	return bTracingCohortWaitStates;
    }
}
/*************************************************/


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VCohort::Cache::Index  *****
 *****                              *****
 ****************************************
 ****************************************/

/*****************************
 *****************************
 *****  Index Generator  *****
 *****************************
 *****************************/

namespace {
    Vca::counter_t g_xNextIndex;
    bool g_bAtExit = false;
    bool g_bHalted = false;
}

Vca::count_t Vca::VCohort::Cache::Index::NextIndex () {
    return g_xNextIndex++;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCohort::Cache::Index::Index () : m_xIndex (NextIndex ()) {
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VCohort::Cache  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCohort::Cache::Cache () : m_xArrayLB (UINT_MAX), m_xArrayUB (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VCohort::Cache::~Cache () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VCohort::Cache::getElementIndex (Index const &rIndex, unsigned int &rxElement) const {
    rxElement = rIndex - m_xArrayLB;
    return m_xArrayLB <= rIndex && rIndex <= m_xArrayUB;
}

void *Vca::VCohort::Cache::value (Index const &rIndex) const {
    unsigned int xElement;
    return getElementIndex (rIndex, xElement) ? const_cast<char*>(m_iArray[xElement].referent ()) : 0;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vca::VCohort::Cache::attach (Index const &rIndex, void const *pValue) {
    unsigned int xElement;
    if (getElementIndex (rIndex, xElement)) {
	if (m_iArray[xElement].isntNil ())
	    return false;
	m_iArray[xElement].setTo ((char const*)pValue);
	return true;
    }
    if (m_iArray.elementCount () == 0) {
	m_iArray.Guarantee (1);
	m_xArrayLB = m_xArrayUB = rIndex;
	m_iArray[0].setTo ((char const*)pValue);
	return true;
    }
    if (m_xArrayLB > rIndex) {
	m_iArray.Prepend (m_xArrayLB - rIndex);
	m_xArrayLB = rIndex;
	m_iArray[0].setTo ((char const*)pValue);
	return true;
    }
    m_iArray.Append (rIndex - m_xArrayUB);
    m_xArrayUB = rIndex;
    m_iArray[m_xArrayUB - m_xArrayLB].setTo ((char const*)pValue);
    return true;
}

bool Vca::VCohort::Cache::detach (Index const &rIndex, void const *pValue) {
    unsigned int xElement;
    if (getElementIndex (rIndex, xElement) && m_iArray[xElement].referent () == (char const*)pValue) {
	m_iArray[xElement] = 0;
	return true;
    }
    return false;
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VCohort::Claim  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCohort::Claim::Claim (
    VRolePlayer *pObject, bool bWait
) : m_pThread (Thread::Here ()), m_pThreadLink (m_pThread->cohortClaims ()), m_pCohort (pObject->cohort ()), m_bHeld (false), m_bRelinquished (false) {
    m_pThread->attach (this, bWait);
}

Vca::VCohort::Claim::Claim (
    VCohort *pCohort, bool bWait
) : m_pThread (Thread::Here ()), m_pThreadLink (m_pThread->cohortClaims ()), m_pCohort (pCohort), m_bHeld (false), m_bRelinquished (false) {
    m_pThread->attach (this, bWait);
}

Vca::VCohort::Claim::Claim (
    bool bClaimingThreadCohort	// true for event processors, false for factories
) : m_pThread (Thread::Here ()), m_pThreadLink (m_pThread->cohortClaims ()), m_pCohort (
    bClaimingThreadCohort ? m_pThread->cohort () : 0
), m_bHeld (false), m_bRelinquished (false) {
    m_pThread->attach (this, bClaimingThreadCohort);
}

Vca::VCohort::Claim::Claim (Thread *pThread) : m_pThread (pThread), m_bHeld (false), m_bRelinquished (false) {
    m_pThread->attach (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VCohort::Claim::~Claim () {
    m_pThread->detach (this);
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VCohort *Vca::VCohort::Claim::cohort () {
    if (m_pCohort.isNil ()) {
	m_bHeld = false;
	m_pCohort.setTo (new VCohort ());
	acquire ();	//  will never block.
    }
    return m_pCohort;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VCohort::Claim::clear () {
    if (m_pCohort) {
	release ();
	m_pCohort.clear ();
	m_bHeld = true;
    }
}

bool Vca::VCohort::Claim::setTo (VCohort *pCohort, bool bWait) {
    if (pCohort != m_pCohort) {
	release ();
	m_pCohort.setTo (pCohort);
    }
    return acquire (bWait);
}

bool Vca::VCohort::Claim::setTo (VRolePlayer *pObject, bool bWait) {
    return setTo (pObject->cohort (), bWait);
}


/************************
 ************************
 *****  Management  *****
 ************************
 ************************/

bool Vca::VCohort::Claim::acquire (bool bWait) {
    if (!acquireTry () && bWait) {
	do {
	    if (TracingCohortWaitStates ()) printf (
		"+++ Thread %llp waiting for cohort %llp%s.\n",
		m_pThread.referent (), m_pCohort.referent (), m_pCohort->isVca () ? " (Vca)" : ""
	    );

	    //  release all claims and block
	    m_pThread->releaseCohorts ();
	    m_pCohort->onReleaseSignal (this);
	    m_pThread->waitForSignal ();

	    if (TracingCohortWaitStates ())
		printf ("+++ Thread %llp resumed.\n", m_pThread.referent ());
	} while (!acquireTry () || !m_pThread->acquireCohorts ());
    }
    return m_bHeld;
}

bool Vca::VCohort::Claim::acquireTry () {
    if (m_pCohort.isNil ())
	m_bHeld = true;
#ifdef VMS_LINUX_EXPLICIT_COMPAT // ... Some builds allow a thread to lock its default cohort:
    else if (!m_bHeld)
	m_pCohort->attemptClaim (m_bHeld, m_pThread);
    return m_bHeld;
#else			// ... Most builds explicitly disallow it:
    else if (m_pThreadLink.isNil ()) // ... claims with no thread link successor provide a thread's default cohort which they will NEVER lock:
 	m_bHeld = false;
    else if (!m_bHeld)
	m_pCohort->attemptClaim (m_bHeld, m_pThread);
    return m_bHeld || m_pThreadLink.isNil ();
#endif
}

void Vca::VCohort::Claim::release () {
    if (m_pCohort.isNil ())
	m_bHeld = false;
    else if (m_bHeld)
	m_pCohort->releaseClaim (m_bHeld, m_pThread);
}

void Vca::VCohort::Claim::relinquish (Pointer &rpSuccessor) {
    if (m_bHeld) {
	m_bRelinquished = true;
	release ();
    }
    rpSuccessor.setTo (m_pThreadLink);
}

bool Vca::VCohort::Claim::unlinquish (Pointer &rpSuccessor) {
    if (m_bRelinquished && !acquireTry ())
	return false;

    m_bRelinquished = false;
    rpSuccessor.setTo (m_pThreadLink);
    return true;
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VCohort::Manager  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCohort::Manager::Manager (VCohort *pCohort) : m_pCohort (pCohort) {
}

Vca::VCohort::Manager::Manager () : m_pCohort (Vca ()) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VCohort::Manager::~Manager () {
    m_pCohort->detach (this);
}

/*****************************
 *****************************
 *****  Cohort Creation  *****
 *****************************
 *****************************/

Vca::VCohort *Vca::VCohort::Manager::cohort () {
    return m_pCohort;
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

bool Vca::VCohort::Manager::onCohortEvent_() {
    return m_pCohort->postInterrupt ();
}

bool Vca::VCohort::Manager::onEmptyCohort_() {
    return true;
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/******************************************************************************
 * This is the main Vca event loop.
 ******************************************************************************/

void Vca::VCohort::Manager::doEvents () {
    while (doEvents (m_pCohort->needsTending () ? Vca_InfiniteWait : 1000));
}

/******************************************************************************
 * Routine:  doEvents
 *	This method is used to wait on the global event set for the specified 
 * period of time. Since there could be global active timers 
 * ...this method takes them into consideration on deciding on the time to wait
 * i.e. if there is a timer with expiry time lesser than the argument of this 
 * function then the wait is done on the timer's expiry time (lesser time).... 
 * After waiting on the minimum time it triggers the timers that are active 
 * and if there was an event that was fired ..handles it and tries to drain the
 * event set.
 * Return Value:
 *	Boolean indicating whether there is some more events or timers waiting
 * to be handled.
 ******************************************************************************/

bool Vca::VCohort::Manager::doEvents (size_t sTimeout) {
    bool bEventsHandled = false;
    return doEvents (sTimeout, bEventsHandled);
}

bool Vca::VCohort::Manager::doEvents (size_t sTimeout, bool &rbEventsHandled) {
    return m_pCohort->processEvents (this, sTimeout, rbEventsHandled);
}


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VCohort::ProcessorRequest  *****
 *****                                  *****
 ********************************************
 ********************************************/

template class Vca_API V::VThreadedProcessor_<Vca::VCohort::ProcessorRequest>;

Vca::VCohort::Processor::Reference Vca::VCohort::g_pProcessor;
static Vca::VCohort::Processor *g_pProcessorToo = 0;

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void Vca::VCohort::ProcessorRequest::process () {
    Claim iClaim (m_pCohort, false);
    decrementProcessorRequestCount ();	//  ... remove the processor claim

    if (iClaim.isHeld ()) {
	Manager iManager (m_pCohort);
	if (iManager.attachCohortEvents ()) {
	    do {
		iManager.doEvents (Vca_InfiniteWait);
	    } while (m_pCohort->needsTending () && !g_bHalted && (!g_bAtExit || m_pCohort->isntVca ())); // If the process is exiting and this loop is processing the Vca cohort, we exit the loop, so that the exit handler can pick up the Vca cohort and process the exit events within.
	}
    }
}


/**************************
 **************************
 *****                *****
 *****  Vca::VCohort  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VCohort::VCohort () : MessageStarter (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VCohort::~VCohort () {
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VCohort *Vca::VCohort::Here () {
    return VcaThreadState::Cohort ();
}

Vca::VCohort::Reference Vca::VCohort::g_pVcaCohort;

extern "C" {
    static void AtExitHandler () {
	if (g_bAtExit)	// ... only do this once.
	    return;

	g_bAtExit = true;

	if (Vca::ExitCalled ()) {
	    g_bHalted = true;
	    return;
	}

	//  initiate a shutdown...
	Vca::Shutdown ();

	//  ... wait for it to happen, ...
	Vca::VCohort::Reference pVca (Vca::VCohort::Vca ());
	pVca->postInterrupt ();

	Vca::VCohort::Claim iShutdownClaim (pVca);

	//  ... and run the event loop just to be sure.
	Vca::VCohort::Manager iShutdownManager (pVca);
	if (iShutdownManager.attachCohortEvents ()) {
	    Vca::VCohort::DefaultLogger().printf ("At Exit Handler Running\n");

	    V::VTime iNow;
	    V::VTime iLimit (iNow);
	    iLimit += 1000000 * static_cast<unsigned __int64>(
		V::GetEnvironmentValue ("VcaAtExitTimeout", static_cast<double>(3.0)) // default == 3 seconds
	    );

	    bool bNotDone = true;
	    bool bUninformed = true;
	    while (bNotDone && iLimit > iNow) {
		bNotDone = iShutdownManager.doEvents ((size_t)((iLimit - iNow)/ 1000));
		iNow.setToNow ();
	    }
	}

	g_bHalted = true;
	if (g_pProcessorToo)
	    g_pProcessorToo->stop ();

	Vca::VCohort::DefaultLogger().printf ("At Exit Handler Exiting\n");
    }
}

void Vca::VCohort::CallExitHandler () {
#if defined (sun)
    AtExitHandler ();
#endif
}

Vca::VCohort *Vca::VCohort::Vca () {
    if (g_pVcaCohort.isNil ()) {
	Claim iFactoryClaim (false);
	if (g_pVcaCohort.interlockedSetIfNil (iFactoryClaim.cohort ())) {
#ifdef _WIN32
/*************************************************************************************************
 *>>>>  There's no point in using an atexit handler in a Windows DLL because the threads     <<<<*
 *>>>>  this handler needs to gracefully stop have been unceremoniously blown away before    <<<<*
 *>>>>  the handler is called.  In the world of Windows, the only atexit handlers that see   <<<<*
 *>>>>  the process' threads are those established by the main (.exe) program.  After these  <<<<*
 *>>>>  handlers are run, the main calls ExitProcess which terminates all threads but the    <<<<*
 *>>>>  main thread and aborts all outstanding I/O's on those threads before calling the     <<<<*
 *>>>>  termination code for any DLLs.  Not much point in trying to exit gracefully in that  <<<<*
 *>>>>  world, is there?                                                                     <<<<*
 *************************************************************************************************/
#else
	    atexit (&AtExitHandler);
#endif
	}
    }
    return g_pVcaCohort;
}


/******************************
 ******************************
 *****  Claim Management  *****
 ******************************
 ******************************/

void Vca::VCohort::attemptClaim (bool &rbHeld, Thread *pThread) {
    if (pThread == m_pClaimHolder || m_pClaimHolder.interlockedSetIfNil (pThread)) {
	rbHeld = true;
	if (0 == m_sHolderClaim++) {
	    drainMessageQueue ();
	}
    }
}

//------
//  This routine is always called by the thread holding the claim.
//------
void Vca::VCohort::releaseClaim (bool &rbHeld, Thread *pThread) {
    // Make sure we're in the thread that holds the claim.
    if (pThread == m_pClaimHolder) {
	rbHeld = false;
        // Decrement the claim.
	if (m_sHolderClaim.decrementIsZero ()) {
            // Claim was last remaining claim for this cohort.
	    Claim::Pointer pBlockedClaim;
	    if (m_pBlockedClaims.interlockedPop (pBlockedClaim, &Claim::m_pCohortLink)) {
                // There's another thread waiting for this cohort.
                // Hand this cohort to said thread and wake it up.
		m_pClaimHolder.setTo (pBlockedClaim->thread ()); // Hand-off
		m_pClaimHolder->signal (); // Wake-up
	    } else {
                // There's no other thread waiting for this cohort.
		m_pClaimHolder.clear (); // Clear claim.
		startEventMonitor (); // Start event monitor for the newly released cohort.
	    }
	}
    }
}

void Vca::VCohort::onReleaseSignal (Claim *pClaim) {
    m_pBlockedClaims.interlockedPush (pClaim, &Claim::m_pCohortLink);
}


/******************************
 ******************************
 *****  Event Management  *****
 ******************************
 ******************************/

void Vca::VCohort::schedule (Message *pMessage) {
    if (m_iMessageQueue.enqueue (pMessage))
	startEventMonitor ();
    else
	postInterrupt ();
}

#ifdef __VMS
#define Workaround
#endif

bool Vca::VCohort::attach (Manager *pManager) {
#ifdef Workaround
    bool bThis = this != 0;
    bool bAttached = bThis && m_pManager.interlockedSetIfNil (pManager);
    if (bAttached) {
#else
    if (m_pManager.interlockedSetIfNil (pManager)) {
#endif
	m_pManager->onCohortEvent ();
	return true;
    }
    return false;
}

bool Vca::VCohort::detach (Manager *pManager) {
    return m_pManager.interlockedClearIf (pManager);
}

bool Vca::VCohort::attach (VDeviceManager *pDeviceManager) {
    return pDeviceManager && pDeviceManager->memberOf (this) && m_pDeviceManager.setIfNil (pDeviceManager);
}

bool Vca::VCohort::detach (VDeviceManager *pDeviceManager) {
    return m_pDeviceManager.clearIf (pDeviceManager);
}

bool Vca::VCohort::attach (VTimer *pTimer) {
    if (!pTimer || !pTimer->memberOf (this) || !pTimer->attach (m_pPendingTimers))
	return false;

    if (0 == m_cPendingTimers++)
	startEventMonitor ();
    else
	postInterrupt ();

    return true;
}

bool Vca::VCohort::detach (VTimer *pTimer) {
    if (!pTimer || !pTimer->memberOf (this) || !pTimer->detach (m_pPendingTimers))
	return false;

    m_cPendingTimers.decrement ();
    postInterrupt ();

    return true;
}

bool Vca::VCohort::needsTending () const {
    return m_iMessageQueue.isntEmpty () || hasPrimaryTimers () || (
	m_pDeviceManager && m_pDeviceManager->hasUses ()
    );
}

void Vca::VCohort::onEmptyCohort () {
    if (m_pManager)
	m_pManager->onEmptyCohort ();
}

void Vca::VCohort::start_(VMessage *pMessage) {
    schedule (pMessage);
}

void Vca::VCohort::startEventMonitor () {
    if (m_pManager)
        // We have a cohort manager. Let it know that we have an event to process.
	m_pManager->onCohortEvent ();
    else if (m_sHolderClaim.isntZero () || m_cProcessorRequests.isntZero () || doesntNeedTending ())
        // We _don't_ have a cohort manager and we can't start a new thread. Let's fire an interrupt on our device manager to make sure that we have everything we should from it. This is necessary when two startEventMonitor() invocations have started in separate threads simultaneously, and one has gotten to processing events first.
	postInterrupt ();	// race avoidance
    else if (!g_bHalted) {
        // We're not claimed, we haven't tried to start a new thread, there's work to be done and we're not halted. We can start a new thread.
	ProcessorRequest iRequest (this);
	if (g_pProcessor.isNil ()) {
	    Processor::Reference pProcessor (new Processor ());
	    if (g_pProcessor.interlockedSetIfNil (pProcessor))
		g_pProcessorToo = pProcessor;
	}
	g_pProcessor->process (iRequest);
    }
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

void Vca::VCohort::decrementProcessorRequestCount () {
    if (m_cProcessorRequests.decrementIsZero ()) {
	startEventMonitor ();
    }
}

bool Vca::VCohort::postInterrupt () {
    return m_pDeviceManager && m_pDeviceManager->postInterrupt ();
}

bool Vca::VCohort::processEvents (Manager *pEM, size_t sTimeout, bool &rbEventsProcessed) {
    if (!pEM)
	defaultLogger().printf ("+++ Vca::VCohort[%p]::processEvents: NO EVENT MANAGER\n", this);

    rbEventsProcessed = false;

//  Clean up zombies, ...
#if !defined(_WIN32) && !defined(__VMS)
    int childStatus;
    while (waitpid (0, &childStatus, WNOHANG) > 0);
#endif

//  ... drain the message and output queues, ...
    count_t const cMessages = m_iMessageQueue.enqueueCount () - m_iMessageQueue.dequeueCount ();
    drainQueues ();

//  ...  adjust the initial timeout for timers and event manager information, ...
    sTimeout = m_pPendingTimers ? m_pPendingTimers->adjustedTimeout (sTimeout) : sTimeout;
    sTimeout = pEM ? pEM->adjustedTimeout (sTimeout) : sTimeout;

//  ... and process any external events that come our way:
    bool bExternalSources = false;
    if (ShutdownCalled ()) {
	defaultLogger().printf (
	    "+++ VCohort[%llp]::processEvents: Shutting down with %s device manager, I/O count: %u, timer count: %u, message count: %u, %ums timeout%s.\n",
	    this,
	    m_pDeviceManager ? "a" : "no",
	    m_pDeviceManager ? m_pDeviceManager->useCount () : 0,
	    m_cPendingTimers.value (),
	    cMessages,
	    sTimeout,
	    isVca () ? " (Vca)" : ""
	);
	for (VTimer::Reference pTimer (m_pPendingTimers); pTimer; pTimer.setTo (pTimer->m_pSuccessor)) {
	    defaultLogger().printf (
		"    Timer[%p]: TTE: %7.3g seconds, Purpose: %s\n",
		pTimer.referent (), static_cast<double>(pTimer->timeToExpire ()) / 1e6, pTimer->purpose ().content ()
	    );
	}
    }
    if (EventsDisabled (sTimeout, m_tLastEvent)) {
	defaultLogger().printf (
	    "+++ VCohort[%llp]::processEvents: Events disabled with %s device manager, I/O count: %u, timer count: %u, message count: %u, %ums timeout%s.\n",
	    this,
	    m_pDeviceManager ? "a" : "no",
	    m_pDeviceManager ? m_pDeviceManager->useCount () : 0,
	    m_cPendingTimers.value (),
	    cMessages,
	    sTimeout,
	    isVca () ? " (Vca)" : ""
	);
	for (VTimer::Reference pTimer (m_pPendingTimers); pTimer; pTimer.setTo (pTimer->m_pSuccessor)) {
	    defaultLogger().printf (
		"    Timer[%p]: TTE: %7.3g seconds, Purpose: %s\n",
		pTimer.referent (), static_cast<double>(pTimer->timeToExpire ()) / 1e6, pTimer->purpose ().content ()
	    );
	}
    } else if (sTimeout > 0 || (s_bDeviceManagerPollEnabled && m_pDeviceManager.isntNil () && m_pDeviceManager->hasUses ())) {
	VDeviceManager::Reference pDM (m_pDeviceManager);
	if (pDM.isNil ()) {
	//  If there are no primary timers (no timers or we're shutting down), ...
	    if (pEM && !hasPrimaryTimers ())
	    // ... let the event manager trim the timeout:
		sTimeout = pEM ? pEM->adjustedTimeoutNDM (sTimeout) : 0;
	//  The context for ignoring the event manager when there are
	//  timers is that the timers have already set the timeout
	//  to the time the next timer is due to fire.  Absent other
	//  sources of events, trimming the timeout further (which is
	//  what the default implementation of 'adjustedTimeoutNDM'
	//  does) will just put us into CPU looping land.  If there
	//  are no timers or other sources of events, however, the
	//  value of 'sTimeout' is going to be the value passed to
	//  this routine.  That is almost certainly going to be
	//  'no timeout'.  In that case, trimming the timeout is
	//  mandatory.

	//  If we still have a timeout, ...
	    if (sTimeout > 0) {
	    //  ... create a device manager to process it, ...
		VDeviceManager::Supply (pDM, this);

	    //  ... drain our queues, ...
                drainQueues ();

	    //  ...  and re-adjust the timeout for whatever timers and events where introduced by the drain:
                sTimeout = m_pPendingTimers ? m_pPendingTimers->adjustedTimeout (sTimeout) : sTimeout;
                sTimeout = pEM ? pEM->adjustedTimeout (sTimeout) : sTimeout;
            }
	}
	bExternalSources = (
	    pDM && pDM->processEvents (sTimeout, rbEventsProcessed)
	) || hasPrimaryTimers ();

        if (triggerTimers ())
	    rbEventsProcessed = true;

	bool bEventsProcessed = false;
	do {
	    if (bEventsProcessed)
		rbEventsProcessed = true;

	    drainQueues ();
	} while (pDM && pDM->processEvents (0, bEventsProcessed) && bEventsProcessed);

	if (rbEventsProcessed) {
	    bExternalSources = true;
	    m_tLastEvent.setToNow ();
	}
    } else if (hasTimers ()) {
	bExternalSources = timersArePrimary ();
	rbEventsProcessed = triggerTimers ();
    }

    return bExternalSources;
}


/****************************************
 ****************************************
 *****  Event Processing Utilities  *****
 ****************************************
 ****************************************/

void Vca::VCohort::drainMessageQueue () {
    if (m_iMessageQueue.isntEmpty ()) do {
	MessageReference pMessage (m_iMessageQueue.head ());
	pMessage->run ();
    } while (m_iMessageQueue.dequeueAndTest ());
}

void Vca::VCohort::drainOutputQueue () {
    VBSConsumer::PutBufferedData ();
}

void Vca::VCohort::drainQueues () {
    do {
	drainMessageQueue ();
	drainOutputQueue ();
    } while (m_iMessageQueue.isntEmpty ());
}

bool Vca::VCohort::triggerTimers () {
    bool bAtLeastOneThatFired = false;
    while (m_pPendingTimers && m_pPendingTimers->triggerIfExpired ()) {
	bAtLeastOneThatFired = true;
    }
    return bAtLeastOneThatFired;
}
