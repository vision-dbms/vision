/*****  Vsa_VEvaluationResult Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VEvaluationResult.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"

#include "Vca_IError.h"

#include "Vsa_VEvaluatorClient.h"
#include "Vsa_VResultBuilder_.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vsa::VEvaluationResult::Waiter  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluationResult::Waiter::Waiter (Result* pResult) : m_pResult (pResult) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluationResult::Waiter::~Waiter () {
}

/*************************
 *************************
 *****  Result Wait  *****
 *************************
 *************************/

bool Vsa::VEvaluationResult::Waiter::wait (unsigned int sMillisecondTimeout) {
    Reference const iRetainer (this);

    V::VTime iEndTime;
    iEndTime += static_cast<unsigned __int64>(sMillisecondTimeout) * 1000;

    onWaitStarted_(); {
	V::VCondvar::Claim iCVL (m_iResultWaitCV);
	while (notCompleted () && m_iResultWaitCV.waitUntil (iEndTime) == V::VCondvar::WaitStatus_Success) {
	    onWaitSignaled_();
	}
    } onWaitFinished_();
	
    return hasCompleted ();
}

void Vsa::VEvaluationResult::Waiter::signal () {
    m_iResultWaitCV.signal ();
}

void Vsa::VEvaluationResult::Waiter::complete (State xCompletionState) {
    if (notCompleted ()) {
	V::VCondvar::Claim iCVL (m_iResultWaitCV);
	m_pResult->setStateTo (xCompletionState);
	signal ();
    }
}

/***********************************
 ***********************************
 *****  Result Wait Callbacks  *****
 ***********************************
 ***********************************/

void Vsa::VEvaluationResult::Waiter::onWaitStarted_() {
}

void Vsa::VEvaluationResult::Waiter::onWaitSignaled_() {
}

void Vsa::VEvaluationResult::Waiter::onWaitFinished_() {
}


/************************************
 ************************************
 *****                          *****
 *****  Vsa::VEvaluationResult  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VEvaluationResult::VEvaluationResult () : m_xState (State_New), m_xQueuePosition (UINT_MAX) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VEvaluationResult::~VEvaluationResult () {
}


/*******************************
 *******************************
 *****  Builder Callbacks  *****
 *******************************
 *******************************/

void Vsa::VEvaluationResult::onAccept (IEvaluation *pEvaluation, Vca::U32 xQueuePosition) {
    if (isNew ()) {
	m_pEvaluation.setTo (pEvaluation);
	m_xQueuePosition = xQueuePosition;
	m_xState = State_Running;
    }
}

void Vsa::VEvaluationResult::onChange (Vca::U32 xQueuePosition) {
    if (isRunning ()) {
	m_xQueuePosition = xQueuePosition;
    }
}

void Vsa::VEvaluationResult::onResult (IEvaluationResult *pResult, VString const &rOutput) {
    if (isRunning ()) {
	m_pEvaluationResult.setTo (pResult);
	m_iEvaluationOutput.setTo (rOutput);
	onCompletion (State_Succeeded);
    }
}

void Vsa::VEvaluationResult::onError (Vca::IError *pError, VString const &rErrorMessage) {
    if (notCompleted ()) {
	m_pError.setTo (pError);
	m_iErrorMessage.setTo (rErrorMessage);
	onCompletion (State_Failed);
    }
}

void Vsa::VEvaluationResult::onCompletion (State xCompletionState) {
    if (m_pWaiter) {
	m_pWaiter->complete (xCompletionState);
	m_pWaiter.clear ();
    }
}

void Vsa::VEvaluationResult::setStateTo (State xCompletionState) {
    m_xState = xCompletionState;
}


/*************************
 *************************
 *****  Result Wait  *****
 *************************
 *************************/

bool Vsa::VEvaluationResult::waitForResult (unsigned int sMillisecondTimeout) {
    if (m_pWaiter.isNil ()) {
	Waiter::Reference const pWaiter (newWaiter ());
	m_pWaiter.interlockedSetIfNil (pWaiter);
    }
    return m_pWaiter->wait (sMillisecondTimeout);
}

Vsa::VEvaluationResult::Waiter *Vsa::VEvaluationResult::newWaiter () {
    return new Waiter (this);
}

/*************************************
 *************************************
 *****  Template Instantiations  *****
 *************************************
 *************************************/

template class Vsa_API Vsa::VResultBuilder_<Vsa::VEvaluationResult>;
