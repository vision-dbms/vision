/*****  VComputationScheduler Implementation  *****/

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

#include "VComputationScheduler.h"

/************************
 *****  Supporting  *****
 ************************/

#include "batchvision.h"

#include "vunwind.h"

#include "IOMDriver.h"

#include "VComputationUnit.h"

#include "VTopTaskBase.h"


/***********************************
 ***********************************
 *****                         *****
 *****  VComputationScheduler  *****
 *****                         *****
 ***********************************
 ***********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VComputationScheduler);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationScheduler::VComputationScheduler () : m_pTheMan (Batchvision::Instance ()), m_iTimeSliceStart (0), m_iCumulativeTime (0), m_sPollingResidue (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VComputationScheduler::~VComputationScheduler () {
}
/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

IOMDriver* VComputationScheduler::channel () const {
    return m_pActiveUnit ? m_pActiveUnit->channel_ () : 0;
}


/***********************************
 ***********************************
 *****  Time Slice Management  *****
 ***********************************
 ***********************************/

void VComputationScheduler::endTimeSlice (VkRunTimes& rCurrentTime) {
    rCurrentTime.refresh ();

    VkRunTimes iTimeSliceDuration (rCurrentTime);
    iTimeSliceDuration -= m_iTimeSliceStart;
    m_iCumulativeTime += iTimeSliceDuration;

    m_iTimeSliceStart = rCurrentTime;
}


/**************************************
 **************************************
 *****  Execution And Scheduling  *****
 **************************************
 **************************************/

/***********************
 *****  Execution  *****
 ***********************/

void VComputationScheduler::processCUnits () {
//  Establish an error handler, ...
    UNWIND_TryAndCatch {
    //  ... that responds to errors by, ...
	if (UNWIND_IsRestarting) {
	//  ... resetting the interrupt manager, ...
	    SIGNAL_ResetInterrupt ();

	//  ... trying to launch an inspector for the culprit, ...
	    if (m_pActiveUnit.isntNil ()) {
		VComputationUnit::Reference pFailedUnit;
		pFailedUnit.claim (m_pActiveUnit);

//		if (pFailedUnit->terminatedNot ()) {
//		    pFailedUnit->setStateTo (VComputationUnit::State_Damaged);
//		    pFailedUnit->triggerFaultSubscriptions ();
//		}
		pFailedUnit->setStateTo (VComputationUnit::State_Damaged);
		if (pFailedUnit->blockedNot ())
		    pFailedUnit->schedule ();
	    }

	//  ... and finally re-enabling interrupts:
	    SIGNAL_TurnOnInterruptHandling ();
	}

    //  and otherwise processes the queue by, ...
	while (m_iActiveQueue.isntEmpty () && m_sPollingResidue > 0) {

	//  ... getting the CU to run, ...
	    m_iActiveQueue.dequeue (m_pActiveUnit);

	//  ... processing it, ...
	    switch (m_pActiveUnit->state ()) {
	    case VComputationUnit::State_Runnable:
		m_pActiveUnit->run ();
		break;

	    case VComputationUnit::State_Damaged:
		m_pActiveUnit->triggerFaultSubscriptions ();
		break;

	    case VComputationUnit::State_Interrupted:
		m_pActiveUnit->triggerInterruptSubscriptions ();
		break;

	    case VComputationUnit::State_Failed:
		m_pActiveUnit->fail ();
		break;

	    default:
		break;
	    }

	//  ... disposing of it, ...
	    if (m_pActiveUnit->blocked ())
		m_pActiveUnit.clear ();
	    else {
		VComputationUnit::Reference pYieldingUnit;
		pYieldingUnit.claim (m_pActiveUnit);
		pYieldingUnit->schedule ();
	    }

	//  ... checking for interrupts, ...
	    if (SIGNAL_InInterruptState) {
		SIGNAL_ResetInterrupt ();

		VComputationUnit* pQueuedUnit = m_iActiveQueue;
		while (pQueuedUnit) {
		    pQueuedUnit->setStateTo (VComputationUnit::State_Interrupted);
		    pQueuedUnit = pQueuedUnit->successor ();
		}

		SIGNAL_TurnOnInterruptHandling ();
	    }

	//  ... and doing it all over again (until it's time to stop):
	    m_sPollingResidue--;
	}
    } UNWIND_EndTryAndCatch;
}

/************************
 *****  Scheduling  *****
 ************************/

void VComputationScheduler::yield () {
    m_sPollingResidue = 1;
}


/************************************************************
 ************************************************************
 *****                                                  *****
 *****  VComputationScheduler::Manager::EventProcessor  *****
 *****                                                  *****
 ************************************************************
 ************************************************************/

class VComputationScheduler::Manager::EventProcessor : public Vca::VCohortManager {
    DECLARE_FAMILY_MEMBERS (EventProcessor, Vca::VCohortManager);

//  Aliases
public:
    typedef VComputationScheduler::Manager SchedulerManager;

//  Construction
public:
    EventProcessor (SchedulerManager* pManager) : BaseClass (Vca::VCohort::Vca ()), m_pManager (pManager) {
    }

//  Destruction
public:
    ~EventProcessor () {
    }

//  Callbacks
private:
    virtual bool onEmptyCohort_() {
	return true;
    }

//  Timeout Adjustment
private:
    virtual size_t adjustedTimeout_(size_t sTimeout) {
	return m_pManager->queueIsntEmpty () ? 0 : sTimeout;
    }

//  State
private:
    SchedulerManager::Reference const m_pManager;
};


/********************************************
 ********************************************
 *****                                  *****
 *****  VComputationScheduler::Manager  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationScheduler::Manager::Manager () 
    : m_sPollingInterval (10000)
    , m_sBlockingWait (1000)
    , m_sGCTimeout (5000000 /* 5 second default */)
    , m_bDebuggerEnabled (false)
    , m_bStopped (false)
{
    setGCTimeout (V::GetEnvironmentUnsigned ("VisionGCTimeout", 5));
}

/********************************
 ********************************
 *****  Garbage Collection  *****
 ********************************
 ********************************/

/** Callback which runs garbage collection upon timeout
 */
void VComputationScheduler::Manager::onGCTimeOut (Vca::VTrigger<ThisClass> *pTrigger) {
    log ("Batchvision[%u]: GC timer expired", getpid ());
    M_DisposeOfSessionGarbage ();
}

/** Procedure to start the garbage collection timer. 
 *  Called when an REP operation completes
 */
void VComputationScheduler::Manager::enableGC () {
    if (0 == m_cGCInhibits || 0 == --m_cGCInhibits) {
	if (m_pGCTimer) {
	    log ("Batchvision[%u]: Starting gc timer ...", getpid ());
	    m_pGCTimer->restart ();
	} else {
	    log("Batchvision[%u]: Creating new gc timer.", getpid ());
	    Vca::VTrigger<ThisClass>::Reference pTrigger (
		new Vca::VTrigger<ThisClass> (this, &ThisClass::onGCTimeOut)
	    );
	    m_pGCTimer.setTo (
		new Vca::VTimer ("GCtimer", pTrigger, m_sGCTimeout)
	    );
	    m_pGCTimer->start ();	    
	}
    }
}

/** Procedure to stop the timer. Does nothing if timer does not exist.
 *  Called when an REP starts so garbage collection will not run while
 *  a vision query is being executed.
 */
void VComputationScheduler::Manager::disableGC() {
    if (m_cGCInhibits++ > 0 || m_pGCTimer.isNil ())
	return;
    log("Batchvision[%u]: Stopping gc timer.", getpid ());
    m_pGCTimer->stop();
}

/** Procedure to change the timeout for the GC Timer
 *
 * \param sTimeout - the new timeout value (in seconds)
 */
void VComputationScheduler::Manager::setGCTimeout (unsigned int sTimeout) {
    if (sTimeout > 0)
	m_sGCTimeout = sTimeout * 1000000;
}


/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void VComputationScheduler::Manager::schedule (VComputationScheduler* pScheduler) {
    if (pScheduler == m_pActiveScheduler)
	return;

    if (m_pQueueTail)
	m_pQueueTail->m_pSuccessor.setTo (pScheduler);
    else
	m_pQueueHead.setTo (pScheduler);

    m_pQueueTail.setTo (pScheduler);

    pScheduler->setPollingResidueTo (m_sPollingInterval);
}

void VComputationScheduler::Manager::reschedule () {
    if (m_pActiveScheduler->QueueIsEmpty ())
	m_pActiveScheduler.clear ();
    else {
	SchedulerReference pActiveScheduler;
	pActiveScheduler.claim (m_pActiveScheduler);
	pActiveScheduler->schedule ();
    }
}


/*******************************
 *******************************
 *****  Master Scheduling  *****
 *******************************
 *******************************/

namespace {
    bool const s_bQueueShutdownRules = V::GetEnvironmentBoolean ("VisionQueueShutdownRules");
}

void VComputationScheduler::Manager::DoEverything () {
//  Allocate the time slice boundary time holders, ...
    VkRunTimes iTimeSliceBoundary;

    EventProcessor iEP (this);
    iEP.attachCohortEvents ();

    UNWIND_TryAndCatch {
    //  On restart, ...
	if (UNWIND_IsRestarting) {
	//  ... reset the interrupt manager, ...
	    SIGNAL_ResetInterrupt ();

	//  ... and reschedule the active scheduler:
	    if (m_pActiveScheduler.isntNil ())
		reschedule ();
	}

    //  Crank the event loop:
	do {
	    SIGNAL_TurnOnInterruptHandling ();

	    if (m_pQueueHead) {// && m_pQueueHead->pollingResidue () > 0 && !SIGNAL_InInterruptState) {
	    //  End the current time slice if the scheduler is changing, ...
		if (m_pActiveScheduler.referent () != m_pQueueHead) {
		    if (m_pActiveScheduler)
			m_pActiveScheduler->endTimeSlice (iTimeSliceBoundary);
		    m_pQueueHead->startTimeSlice (iTimeSliceBoundary);
		}

	    //  ... get the next scheduler to run, ...
		m_pQueueHead.pop (m_pActiveScheduler, &VComputationScheduler::m_pSuccessor);
		if (m_pQueueHead.isEmpty ())
		    m_pQueueTail.clear ();

	    //  ... process its c-units, ...
		m_pActiveScheduler->processCUnits ();

	    //  ... reschedule it, ...
		reschedule ();
	    }
	//  When we run out of things to do, ...
	//  ...  disable interrupts, ...
	    SIGNAL_ResetInterrupt ();

	//  ... flush buffered output, ...
	    IOM_PutBufferedData ();

	//  ... clear the active scheduler, ...
	    if (m_pActiveScheduler) {
		m_pActiveScheduler->endTimeSlice (iTimeSliceBoundary);
		m_pActiveScheduler.clear ();
	    }

	//  ... perform some maintenance tasks ...
	    M_ProcessGRMRequests ();

	//  ... and re-fill the queues, ...
	    if (m_pQueueHead.isntEmpty ())
		iEP.doEvents (0);
	    else {
		bool bEventsHandled = false;
    		while (m_cTopTasks > 0 && m_pQueueHead.isEmpty () && iEP.doEvents (m_sBlockingWait, bEventsHandled)) {
		    if (!bEventsHandled) {
			M_ProcessGRMRequests ();
		    }
		}
	    }
	} while (s_bQueueShutdownRules ? m_pQueueHead.isntEmpty () : m_cTopTasks > 0 && !m_bStopped );
    } UNWIND_EndTryAndCatch;
}

void VComputationScheduler::Manager::StopEverything () {
    m_bStopped = true;
}
