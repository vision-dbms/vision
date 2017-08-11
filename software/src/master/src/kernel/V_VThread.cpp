/*****  V::VThread Implementation  *****/

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

#include "V_VThread.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "V_VUnmanagedThread.h"


/************************
 ************************
 *****              *****
 *****  V::VThread  *****
 *****              *****
 ************************
 ************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (V::VThread);

/*************************
 *************************
 *****  Context Key  *****
 *************************
 *************************/

V::VThread::EternalKey V::VThread::g_iTLSKey;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VThread::VThread (
    pthread_t hThread, pthread_id_t xThread
) : m_xState (State_Set), m_hThread (hThread), m_xThread (xThread) {
}

V::VThread::VThread () : m_xState (State_Startable) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VThread::~VThread () {
#if !defined(V_THREAD_ENABLED)
#elif defined(_WIN32)
    switch (m_xState) {
    case State_Set:
    case State_Started:
	CloseHandle (m_hThread);
	break;
    default:
	break;
    }
#else
    switch (m_xState) {
    case State_Started:
	pthread_detach (m_hThread);
	break;
    default:
	break;
    }
#endif
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

pthread_t V::VThread::CurrentThread () {
#if !defined (V_THREAD_ENABLED)
    return 0;
#elif defined(_WIN32)
    return GetCurrentThread ();	// dup?
    HANDLE hProcess = GetCurrentProcess ();
    HANDLE hThread;
    return DuplicateHandle (
	hProcess, GetCurrentThread (), hProcess, &hThread, 0, false, DUPLICATE_SAME_ACCESS
    ) ? hThread : NULL;
#else
    return pthread_self ();
#endif
}

pthread_id_t V::VThread::CurrentThreadId () {
#if !defined (V_THREAD_ENABLED)
    return 0;
#elif defined(_WIN32)
    return GetCurrentThreadId ();
#elif defined(__VMS)
    return pthread_getsequence_np (pthread_self ());
#else
    static pthread_id_t g_xNextThread = 1;
    return g_xNextThread++;
#endif
}

V::VThread::Reference V::VThread::Here () {
    BaseClass::Reference pSpecific; Reference pThisInstance;
    if (g_iTLSKey->getSpecific (pSpecific) && pSpecific.isntNil ())
	pThisInstance.setTo (static_cast<ThisClass*>(pSpecific.referent ()));
    else
	pThisInstance.setTo (new VUnmanagedThread ());

    return pThisInstance;
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool V::VThread::start (pthread_routine_t *pThreadProcedure, void *pThreadData) {
    if (isntStartable ())
	return false;

    retain ();		//  ... protect this thread from garbage collection while it is running.
			//  ... This protection MUST be removed by the thread procedure when the
    			//  ... thread exits.

#if !defined(V_THREAD_ENABLED)
    bool bStarted = false;
#elif defined(_WIN32)
    m_hThread = CreateThread (NULL, 0, pThreadProcedure, pThreadData, 0, &m_xThread);
    bool bStarted = SystemHandleIsValid (m_hThread);
#else
    pthread_attr_t iTCA;
    pthread_attr_init (&iTCA);
#ifdef __VMS
    pthread_attr_setname_np (&iTCA, rttName ().content (), 0);
#endif

    
    //  set minimum stack size....
#if defined(__VMS) || defined (sun) || defined (__linux__) || defined(__APPLE__)
    size_t sStack=0;
    pthread_attr_getstacksize (&iTCA, &sStack);
    adjustStackSize (sStack);
    if (sStack < STACKSIZE_MIN)
	sStack = STACKSIZE_MIN;
    pthread_attr_setstacksize (&iTCA, sStack);
#endif

    bool bStarted = pthread_create (&m_hThread, &iTCA, pThreadProcedure, pThreadData) == 0;
#endif
    if (bStarted)
	m_xState = State_Started;
    else
	release ();	//  ... remove the GC reclamation protection if the thread didn't start.

    return bStarted;
}

bool V::VThread::stop () {
    if (isntInitialized ())
	return false;

#if !defined(V_THREAD_ENABLED)
    return false;
#elif defined(_WIN32)
    return TerminateThread (m_hThread, (DWORD)-1) ? true : false;
#else
    return pthread_cancel (m_hThread) == 0;
#endif
}
