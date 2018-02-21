/*****  Vxa_VTask Implementation  *****/

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

#include "Vxa_VTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VThreadedProcessor_.h"
#include "Vxa_VError.h"
#include "Vxa_VCallType1.h"
#include "Vxa_VCallType2.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vxa::VTask::LaunchRequest  *****
 *****                             *****
 ***************************************
 ***************************************/

//********************************************************************************************
//****
//****  Vxa tasks call user code that may not be a good citizen when called in a Vca event
//****  loop.  Among other things, it may block or rely on the completion of messages whose
//****  receipt it's blocking.  To keep that from happening, Vxa runs the application code
//****  specific portion of a task in a separate V::VThreadedProcessor_ thread pool thread.
//****
//********************************************************************************************
class Vxa::VTask::LaunchRequest {
    DECLARE_FAMILY_MEMBERS (LaunchRequest, void);

//  Construction
public:
    LaunchRequest (VTask *pTask) : m_pTask (pTask) {
    }
    LaunchRequest (ThisClass const &rOther) : m_pTask (rOther.m_pTask) {
    }
    LaunchRequest () {
    }

//  Destruction
public:
    ~LaunchRequest () {
    }

//  Update
public:
    ThisClass &operator= (ThisClass const &rOther) {
	m_pTask.setTo (rOther.m_pTask);
	return *this;
    }
    void clear () {
	m_pTask.clear ();
    }

//  Execution
public:
    void process () {
	if (m_pTask) {
	    m_pTask->runWithMonitor ();
	}
    }

//  State
private:
    VTask::Reference m_pTask;
};
template class Vxa_API V::VThreadedProcessor_<Vxa::VTask::LaunchRequest>;


/************************
 ************************
 *****              *****
 *****  Vxa::VTask  *****
 *****              *****
 ************************
 ************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VTask::VTask (
    VCallData const &rCallData
) : m_iCallData (rCallData), m_cSuspensions (1), m_pCursor (new VTaskCursor (rCallData.domain ())) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VTask::~VTask () {
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

bool Vxa::VTask::startUsing (VCallType1Importer &rImporter) {
    return resume ();
}

bool Vxa::VTask::startUsing (VCallType2Importer &rImporter) {
    return resume ();
}

bool Vxa::VTask::startWithMonitorUsing (VCallType1Importer &rImporter) {
    onStart ();
    return startUsing (rImporter);
}

bool Vxa::VTask::startWithMonitorUsing (VCallType2Importer &rImporter) {
    onStart ();
    return startUsing (rImporter);
}

bool Vxa::VTask::runWithMonitor () {
//--------------------------------------------------------------------------------
//---  This code is running in an arbitrary thread.  Since this is Vca code, it
//---  is quite likely that a call will be made that needs the current cohort.
//---  Make sure the cohort it gets isn't permanently locked to this thread by
//---  first attempting a non-blocking claim of the task's cohort and, if that
//---  fails, claiming a new virtual cohort before running the task.
//--------------------------------------------------------------------------------
    Vca::VCohort::Claim iCC (this, false); // ... attempt a non-blocking claim of the task's cohort
    if (iCC.isntHeld ())  //  ... if the claim didn't succeed, ...
        iCC.clear ();  //  ... clear the claim so that it's free to create a new cohort if necessary.

    bool const bSuccessful = run ();
/*
    if (bSuccessful)
	onSuccess ();
    else
	onFailure (0, "Vxa task run error!");
*/
    return bSuccessful;
}

void Vxa::VTask::onErrorEvent (VError *pError) {
    VString iMessage;
    pError->getMessage (iMessage);
    onActivityError (0, iMessage);
}

bool Vxa::VTask::launchInThreadPool () {
    static V::VThreadedProcessor_<LaunchRequest>::Reference g_pRequestProcessor;
    if (g_pRequestProcessor.isNil ()) {
	V::VThreadedProcessor_<LaunchRequest>::Reference const pRequestProcessor (
	    new V::VThreadedProcessor_<LaunchRequest> ()
	);
	pRequestProcessor->setThreadStackSize	(V::GetEnvironmentUnsigned ("VxaStackSize", 1024*1024));	//  ... a number drawn from experience
	pRequestProcessor->setWorkerLimit	(V::GetEnvironmentUnsigned ("VxaTaskLimit", 32));		//  ... a number drawn from a hat
	g_pRequestProcessor.interlockedSetIfNil (pRequestProcessor);
    }
//    LaunchRequest iLR (this);
    g_pRequestProcessor->process (LaunchRequest (this));
    return true;
}
