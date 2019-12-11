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


/***************************************
 ***************************************
 *****                             *****
 *****  Vxa::VTask::RemoteControl  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VTask::RemoteControl::RemoteControl (
    ICaller2 *pRemoteInterface
) : m_pRemoteInterface (pRemoteInterface), m_cSuspensions (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VTask::RemoteControl::~RemoteControl () {
}

/****************************
 ****************************
 *****  Remote Control  *****
 ****************************
 ****************************/

bool Vxa::VTask::RemoteControl::suspend () {
    return 0 == m_cSuspensions++ && sendRemoteSuspend ();
}

bool Vxa::VTask::RemoteControl::resume () {
    return m_cSuspensions > 0 && 0 == --m_cSuspensions && sendRemoteResume ();
}

bool Vxa::VTask::RemoteControl::sendRemoteSuspend () const {
    Vca::VCohortClaim iCohortClaim;

    if (m_pRemoteInterface.isNil ())
        return false;
    m_pRemoteInterface->Suspend ();
    return true;
}

bool Vxa::VTask::RemoteControl::sendRemoteResume () const {
    Vca::VCohortClaim iCohortClaim;

    if (m_pRemoteInterface.isNil ())
        return false;
    m_pRemoteInterface->Resume ();
    return true;
}


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
    wrapupRemoteControl ();
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

/*****
 *  Remove the protective remote suspension set during remote control creation...
 *****/
    wrapupRemoteControl ();

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

bool Vxa::VTask::launch () {
    return m_iCallData.launchTask (this);
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


/****************************
 ****************************
 *****  Remote Control  *****
 ****************************
 ****************************/

Vxa::VTask::RemoteControl *Vxa::VTask::getRemoteControl (
    Vxa::ICaller2 *pRemoteInterface
) {
    if (m_pRemoteControl.isNil ()) {
        RemoteControl::Reference const pRemoteControl (
            new RemoteControl (pRemoteInterface)
        );
        if (m_pRemoteControl.interlockedSetIfNil (pRemoteControl))
        /*****
         *  Tasks send a protective remote suspension on newly minted remote controls
         *  which they remove on task completion/finalization...
         *****/
            m_pRemoteControl->suspend ();
    }
    return m_pRemoteControl;
}

void Vxa::VTask::wrapupRemoteControl () {
    if (m_pRemoteControl) {
        m_pRemoteControl->resume ();
        m_pRemoteControl.clear ();
    }
}
