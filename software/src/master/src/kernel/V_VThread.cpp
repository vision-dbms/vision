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
#include "V_VRandom.h"


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

V::VThreadSpecific::Key const V::VThread::g_iTLSKey;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VThread::VThread (
    pthread_t hThread, pthread_id_t xThread
) : m_xState (State_Set), m_hThread (hThread), m_xThread (xThread), m_bAllocatorFlushed (true) {
}

V::VThread::VThread () : m_xState (State_Startable), m_bAllocatorFlushed (true) {
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

bool V::VThread::GetHere (Reference &rpHere) {
    BaseClass::Reference pSpecific;
    if (g_iTLSKey.getSpecific (pSpecific) && pSpecific) {
	rpHere.setTo (static_cast<ThisClass*>(pSpecific.referent ()));
	return true;
    }
    return false;
}

V::VThread::Reference V::VThread::Here () {
    Reference pHere;
    if (!GetHere (pHere))
	pHere.setTo (new VUnmanagedThread ());
    return pHere;
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

    CheckSingleCoreExecution();

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

bool const V::VThread::m_VSingleCoreExecution = V::GetEnvironmentBoolean ("VSingleCoreExecution");
bool       V::VThread::m_VSingleCoreExecutionChecked = false;



/*******************************
 *******************************
 *****  Memory Management  *****
 *******************************
 *******************************/

V::VThreadSafeAllocator V::VThread::GlobalAllocator;

void *V::VThread::Allocate (size_t sObject) {
    Reference pHere;
    return GetHere (pHere) ? pHere->allocateLocal (sObject) : AllocateGlobal (sObject);
}

void *V::VThread::AllocateGlobal (size_t sObject) {
    NoHazardPointer pNoHazard;
    return AllocateGlobal (sObject, pNoHazard);
}

void V::VThread::Deallocate (void *pObject, size_t sObject) {
    Reference pHere;
    if (GetHere (pHere))
	pHere->deallocateLocal (pObject, sObject);
    else {
	DeallocateGlobal (pObject, sObject);
    }
}

void V::VThread::FlushAllocator () {
    Reference pHere;
    if (GetHere (pHere))
	pHere->flushAllocator ();
}

void V::VThread::flushAllocator () {
    m_bAllocatorFlushed = true;
    m_iTLA.flush ();
}

void V::VThread::DisplayAllocationStatistics () {
    display ("\n====  Thread Local Allocator:"); {
	Reference pHere;
	if (GetHere (pHere))
	    pHere->displayAllocationStatistics ();
    }
    display ("\n====  Process Global Allocator:");
    GlobalAllocator.displayCounts ();
}

void V::VThread::displayAllocationStatistics () const {
    m_iTLA.displayCounts ();
}


void V::VThread::CheckSingleCoreExecution () {
    if (!m_VSingleCoreExecutionChecked) {
        if (m_VSingleCoreExecution) {
            SetSingleCoreExecution();
        }
    }

    m_VSingleCoreExecutionChecked = true;
}

int V::VThread::SetSingleCoreExecution () {
    bool success = -1;

#if defined(V_THREAD_ENABLED) && defined (__linux__)

    // Determine how many CPUs exist in order to randomly assign appropriately
    unsigned int cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    
    // get our random-ish CPU index
    int cpu = V::VRandom::BoundedValue(cpu_count);

    // Configure a CPU mask that only includes the specified CPU.
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(cpu, &cpu_mask);
    
    // Tell the scheduler to only run the process on the CPU set defined
    // in the mask. Passing zero for the first argument operates on the
    // calling process. 
    success = sched_setaffinity(0, sizeof(cpu_mask), &cpu_mask);
#endif

    return success;
}
