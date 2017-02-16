/*****  V::VManagedThread Implementation  *****/

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

#include "V_VManagedThread.h"

/************************
 *****  Supporting  *****
 ************************/


/*******************************
 *******************************
 *****                     *****
 *****  V::VManagedThread  *****
 *****                     *****
 *******************************
 *******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (V::VManagedThread);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VManagedThread::VManagedThread () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VManagedThread::~VManagedThread () {
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

namespace {
    void BlockSignals () {
#if defined(__unix__) || defined (__unix)
	sigset_t iMask;
	sigfillset (&iMask);
	pthread_sigmask (SIG_BLOCK, &iMask, 0);
#endif
    }
}

pthread_procedure_result_t V::VManagedThread::Run (ThisClass *pThis) {
    BlockSignals ();
    return pThis->run ();
}

pthread_procedure_result_t V::VManagedThread::run () {
    const Reference iRetainer (this);	//  ... protect this thread from garbage collection while it is running
    release ();				//  ... and remove the guard created by VThread::start ();

    becomeSpecific ();
    return run_();
}

bool V::VManagedThread::start () {
    return BaseClass::start ((pthread_routine_t*)&ThisClass::Run, this);
}
