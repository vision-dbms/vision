/*****  VkStreamAgent Implementation  *****/

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

#include "VkStreamAgent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkSocketAddress.h"

#include "Vca_ITrigger.h"

#if defined(_WIN32)
#pragma comment (lib, "ws2_32.lib")
#endif


/***************************
 ***************************
 *****                 *****
 *****  Stream Thread  *****
 *****                 *****
 ***************************
 ***************************/

#if defined(_WIN32)
class VkStreamThread : public VReferenceable {
    DECLARE_CONCRETE_RTT (VkStreamThread, VReferenceable);

//  Globals
private:
    static CRITICAL_SECTION	g_iFreeListMutex;
    static VReference<ThisClass>g_pFreeListHead;

//  Thread Procedure
private:
    static DWORD WINAPI ThreadProcedure (VkStreamThread *pThread) {
	return pThread->run ();
    }
    DWORD run ();

//  Construction
private:
    VkStreamThread ();

//  Destruction
private:
    ~VkStreamThread ();

//  Query
public:
    bool isValid () const {
	return IsntNil (this)
	    && SystemHandleIsValid (m_hSemaphore)
	    && SystemHandleIsValid (m_hThread);
    }

//  Scheduling
public:
    static bool Service (VkStreamAgent *pActivity);

private:
    static bool GetIdleThread (VReference<ThisClass>&rpThread);
    bool service (VkStreamAgent *pActivity);

//  State
protected:
    VReference<ThisClass>	m_pFreeListLink;
    VkStreamAgent*		m_pActivity;
    HANDLE const		m_hSemaphore;
    HANDLE const		m_hThread;
    DWORD			m_fThread;
};


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VkStreamThread);

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

CRITICAL_SECTION VkStreamThread::g_iFreeListMutex;
VReference<VkStreamThread> VkStreamThread::g_pFreeListHead;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VkStreamThread::VkStreamThread () : m_pActivity (0), m_hSemaphore (
    CreateSemaphore (NULL, 0, 1, NULL)
), m_hThread (
    CreateThread (
	NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, this, CREATE_SUSPENDED, &m_fThread
    )
) {
    ResumeThread (m_hThread);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VkStreamThread::~VkStreamThread () {
    TerminateThread (m_hThread, (DWORD)-1);
    CloseHandle (m_hSemaphore);
    CloseHandle (m_hThread);
}


/******************************
 ******************************
 *****  Thread Procedure  *****
 ******************************
 ******************************/

DWORD VkStreamThread::run () {
//  Wait for the first activity, ...
    WaitForSingleObject (m_hSemaphore, INFINITE);

//  ... and, while there's work to be done, ...
    while (m_pActivity) {

//  ... get it done, ...
	m_pActivity->CompleteThreadedOperation ();

//  ... mark the thread as inactive, ...
	m_pActivity = 0;

	__try {
	    EnterCriticalSection (&g_iFreeListMutex);

//  ... return the thread to the thread pool, ...
	    m_pFreeListLink.claim (g_pFreeListHead);
	    g_pFreeListHead.setTo (this);

//  ... remove the active thread reference, ...
	    untain ();
	} __finally {
	    LeaveCriticalSection (&g_iFreeListMutex);
	}

//  ... and wait for more to do:
	WaitForSingleObject (m_hSemaphore, INFINITE);
    }
    return 0;
}


/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

bool VkStreamThread::GetIdleThread (VReference<ThisClass>&rpThread) {
    static bool g_bFreeListMutexUninitialized = true;
    if (g_bFreeListMutexUninitialized) {
	InitializeCriticalSection (&g_iFreeListMutex);
	g_bFreeListMutexUninitialized = false;
    }

    __try {
	EnterCriticalSection (&g_iFreeListMutex);
	rpThread.claim (g_pFreeListHead);
	if (rpThread.isntNil ())
	    g_pFreeListHead.claim (rpThread->m_pFreeListLink);
    } __finally {
	LeaveCriticalSection (&g_iFreeListMutex);
    }
    return rpThread.isntNil ();
}

bool VkStreamThread::Service (VkStreamAgent *pActivity) {
    VReference<ThisClass> pThread;
    while (GetIdleThread (pThread) && !pThread->service (pActivity));

    if (pThread.isntNil ())
	return true;

    pThread.setTo (new ThisClass ());
    return pThread->isValid () && pThread->service (pActivity);
}

bool VkStreamThread::service (VkStreamAgent *pActivity) {
//  Record the activity to be performed, ...
    m_pActivity = pActivity;

//  ... create a reference to keep the thread alive while it's active, ...
    retain ();

//  ... resume the thread, ...
    if (ReleaseSemaphore (m_hSemaphore, 1, NULL))
	return true;

//  ... allowing threads that won't resume to be reclaimed:
    release ();

    return false;
}
#endif	//  defined(_WIN32)


/**************************
 **************************
 *****                *****
 *****  Stream Agent  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

#if defined(_WIN32)
#define EventFromStream(pStream) CreateEvent (NULL, true, true, NULL)
#else
#define EventFromStream(pStream) ((pStream)->m_hStream)
#endif

VkStreamAgent::VkStreamAgent (
    VkStream *pStream
) : m_pStream (pStream), m_iEvent (EventFromStream (pStream)) {
}

#undef EventFromStream

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

void VkStreamAgent::Destroy () {
    m_iEvent.clearHandler (m_iEventHH);
    m_iEventHH.clear ();
    m_pStream.clear ();
}

VkStreamAgent::~VkStreamAgent () {
//    Destroy ();
}


/*******************************
 *******************************
 *****  Operation Startup  *****
 *******************************
 *******************************/

void VkStreamAgent::start (Vca::ITrigger *pEventHandler) {
    m_iEventHH.setTo (pEventHandler);
}

bool VkStreamAgent::StartOperation (
    VkEvent::State xEventState, VkStatus *pStatusReturn
) {
#if defined(_WIN32)
    ResetEvent (m_iEvent.handle ());
    m_iDeferredStatus.MakeSuccessStatus ();

    m_iEvent.claimHandler (m_iEventHH, xEventState);
    pStatusReturn->MakeBlockedStatus ();

    if (VkStreamThread::Service (this))
	return true;

    m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
    pStatusReturn->MakeErrorStatus ();

    return false;
#else
    m_iEvent.claimHandler (m_iEventHH, xEventState);
//    pStatusReturn->MakeBlockedStatus ();  ???

    return true;
#endif
}


/************************************
 ************************************
 *****  Win32 Specific Support  *****
 ************************************
 ************************************/

#if defined(_WIN32)

/*******************************
 *****  Thread Management  *****
 *******************************/

void VkStreamAgent::CompleteThreadedOperation () {
    switch (m_iEvent.state ()) {
    case VkEvent::State_AcceptWait:
	if (SocketHandleIsInvalid (
		*(SOCKET*)m_pData = accept ((SOCKET)StreamHandle (), NULL, NULL)
	    )
	) {
	    m_iDeferredStatus.MakeErrorStatus ();
	}
	break;

    case VkEvent::State_ReadWait:
	SetLastError (NO_ERROR);
	if (!ReadFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, 0))
	    m_iDeferredStatus.MakeErrorStatus ();
	else if (0 == m_sData) {
	    DWORD xLastError = GetLastError ();
    /*-----------------------------------------------------------------------*
     *  ERROR_OPERATION_ABORTED is returned in response to Control-C/Break.  *
     *  It isn't an error in those cases...                                  *
     *-----------------------------------------------------------------------*/
	    if (ERROR_OPERATION_ABORTED == xLastError)
		m_iDeferredStatus.MakeSuccessStatus ();
	    else {
		SetLastError (ERROR_HANDLE_EOF);
		m_iDeferredStatus.MakeErrorStatus ();
	    }
	}
	break;

    case VkEvent::State_WriteWait:
 	if (!WriteFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, 0))
	    m_iDeferredStatus.MakeErrorStatus ();
	break;

    default:
	break;
    }
    SetEvent (m_iEvent.handle ());
}
#endif


/****************************
 ****************************
 *****  I/O Completion  *****
 ****************************
 ****************************/

bool VkStreamAgent::Complete (VkStatus *pStatusReturn, VkEvent::State xPartialSuccessState) {
    if (m_pStream.isNil ()) {
	pStatusReturn->MakeClosedStatus ();
	return false;
    }

#if defined(_WIN32)
/**************************
 *  WIN32 Implementation  *
 **************************/

    switch (m_pStream->Mode ()) {
    case VkStreamMode_Synchronous:
	return true;

    case VkStreamMode_Overlapped:
	switch (m_iEvent.state ()) {
	case VkEvent::State_Signaled:
	    return true;

	case VkEvent::State_ReadWait:
	case VkEvent::State_WriteWait:
	    if (CompleteOverlappedOperation ()) {
		m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
	        OnCompletion ();
		return true;
	    }

	    m_iDeferredStatus.MakeErrorStatus ();	// m_iStatus
	    if (!m_iDeferredStatus.isBlocked ())	// m_iStatus
		m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);

	    *pStatusReturn = m_iDeferredStatus;	// m_iStatus
	    break;

	default:
	    break;
	}
	break;

    case VkStreamMode_Threaded:
	if (!m_iEvent.isSignaled (0))
	    pStatusReturn->MakeBlockedStatus ();
	else switch (m_iEvent.state ()) {
	case VkEvent::State_Signaled:
	    return true;

	case VkEvent::State_Failed:
	    pStatusReturn->MakeErrorStatus ();
	    m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	    break;

	case VkEvent::State_AcceptWait:
	    if (m_iDeferredStatus.isCompleted ()) {
		m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
		return true;
	    }
	    m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	    *pStatusReturn = m_iDeferredStatus;
	    break;

	case VkEvent::State_ReadWait:
	case VkEvent::State_WriteWait:
	    if (m_iDeferredStatus.isCompleted ()) {
		m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
	        OnCompletion ();
		return true;
	    }
	    m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	    *pStatusReturn = m_iDeferredStatus;
	    break;
	}
	break;
    }

    return false;


#else
/*************************
 *  UNIX Implementation  *
 *************************/

    if (!m_iEvent.isSignaled (0))
	 pStatusReturn->MakeBlockedStatus ();
    else switch (m_iEvent.state ()) {
    case VkEvent::State_Signaled:
	return true;

    case VkEvent::State_Failed:
	m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	pStatusReturn->MakeErrorStatus ();
	break;

    case VkEvent::State_AcceptWait: {
	    SOCKET &rhSocket = *(SOCKET*)m_pData;
	    rhSocket = accept (m_iEvent.handle (), NULL, NULL);
	    if (SocketHandleIsValid (rhSocket)) {
#if defined (sun) || defined (_AIX)
		int noDelay = false; socklen_t noDelaySize = sizeof (noDelay);
		getsockopt (
		    m_iEvent.handle (), IPPROTO_TCP, TCP_NODELAY, (char *)&noDelay, &noDelaySize
		);
		if (noDelay) setsockopt (
		    rhSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&noDelay, noDelaySize
		);
#endif
		m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
		return true;
	    }
	}
//  Added in 6.2.0 ...
	m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	m_iDeferredStatus.MakeErrorStatus ();
	*pStatusReturn = m_iDeferredStatus;
	break;

    case VkEvent::State_ReadWait:
#if defined(__VMS)
	if (m_sData > 65535)
	    m_sData = 65535;
#endif
	m_sData = read (m_iEvent.handle (), m_pData, m_sData);
	if (-1 == (int)m_sData)
	    m_iDeferredStatus.MakeErrorStatus ();
	else if (0 == m_sData)
	    m_iDeferredStatus.MakeClosedStatus ();
        else {
	    m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
	    OnCompletion ();
	    return true;
	}
	m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	*pStatusReturn = m_iDeferredStatus;
	break;

    case VkEvent::State_WriteWait:
#if defined(__VMS)
	if (m_sData > 65535)
	    m_sData = 65535;
#endif
	m_sData = write (m_iEvent.handle (), m_pData, m_sData);
	if (-1 == (int)m_sData)
	    m_iDeferredStatus.MakeErrorStatus ();
        else {
	    m_iEvent.clearHandler (m_iEventHH, VkEvent::State_Signaled);
	    OnCompletion ();
	    return true;
	}
	m_iEvent.clearHandler (m_iEventHH, xPartialSuccessState);
	*pStatusReturn = m_iDeferredStatus;
	break;

    default:
	break;
    }

    return false;
#endif
}


/******************************
 ******************************
 *****                    *****
 *****  VkStreamListener  *****
 *****                    *****
 ******************************
 ******************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

void VkStreamListener::Destroy () {
    VkSocketAddress iSocketAddress; VkStatus iStatus;
    bool bSocketAddressValid = iSocketAddress.GetSocketName (SocketHandle (), &iStatus) >= 0;

    BaseClass::Destroy ();

    if (bSocketAddressValid && iSocketAddress.familyIsUnix ())
	unlink (iSocketAddress.unixPathName ());
}

/************************
 *****  Connection  *****
 ************************/

void VkStreamListener::OnCompletion () {
}

VkStream *VkStreamListener::GetConnection (VkStatus *pStatusReturn) {
    while (Complete (pStatusReturn)) {
	VkStream *pStream;
	if (SocketHandleIsInvalid (m_hSocket)) {
	    m_pData = (void*)&m_hSocket;
	    StartOperation (VkEvent::State_AcceptWait, pStatusReturn);
	}
	else if (IsntNil (pStream = VkStream::New (m_hSocket, false, false))) {
	    m_hSocket = INVALID_SOCKET;
	    return pStream;
	}
	else {
	    pStatusReturn->MakeErrorStatus ();
	    return NilOf (VkStream*);
	}
    };

    return NilOf (VkStream*);
}


/*******************************
 *******************************
 *****                     *****
 *****  Stream Data Agent  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VkStreamDataAgent::VkStreamDataAgent (VkStream *pStream) : VkStreamAgent (pStream) {
#if defined(_WIN32)
    m_iOverlapped.Offset = m_iOverlapped.OffsetHigh = 0;
    m_iOverlapped.hEvent = m_iEvent.handle ();
#endif
}


/************************************
 ************************************
 *****  Win32 Specific Support  *****
 ************************************
 ************************************/

/***************************************
 *****  Overlapped I/O Management  *****
 ***************************************/

#if defined(_WIN32)
bool VkStreamDataAgent::RegisterOverlappedOperation (
    VkEvent::State xEventState, VkStatus *pStatusReturn
) {
    pStatusReturn->MakeErrorStatus ();
    if (!pStatusReturn->isBlocked ())
	return false;

    m_iEvent.claimHandler (m_iEventHH, xEventState);
    return true;
}
#endif


/************************************
 ************************************
 *****                          *****
 *****  VkStreamReceivingAgent  *****
 *****                          *****
 ************************************
 ************************************/

/*******************
 *****  Input  *****
 *******************/

bool VkStreamReceivingAgent::StartRead (
    void *pData, size_t sData, VkStatus *pStatusReturn
) {
    m_pData = pData;
    m_sData = sData;

    if (m_pStream.isNil ()) {
	pStatusReturn->MakeClosedStatus ();
	return false;
    }

#if defined(_WIN32)
    switch (m_pStream->Mode ()) {
    case VkStreamMode_Synchronous:
	if (!ReadFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, 0)) {
	    pStatusReturn->MakeErrorStatus ();
	    return false;
	}

	if (0 == m_sData) {
	    SetLastError (ERROR_HANDLE_EOF);
	    pStatusReturn->MakeErrorStatus ();
	    return false;
	}

	OnCompletion ();
	break;

    case VkStreamMode_Overlapped:
	if (!ReadFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, &m_iOverlapped))
	    return RegisterOverlappedOperation (VkEvent::State_ReadWait, pStatusReturn);

	if (0 == m_sData) {
	    SetLastError (ERROR_HANDLE_EOF);
	    pStatusReturn->MakeErrorStatus ();
	    return false;
	}

	OnCompletion ();
	break;

    default:
	return StartOperation (VkEvent::State_ReadWait, pStatusReturn);
    }
    return true;

#else
    return StartOperation (VkEvent::State_ReadWait, pStatusReturn);
#endif
}


/***********************************
 ***********************************
 *****                         *****
 *****  VkStreamShippingAgent  *****
 *****                         *****
 ***********************************
 ***********************************/

/********************
 *****  Output  *****
 ********************/

bool VkStreamShippingAgent::StartWrite (
    void const *pData, size_t sData, VkStatus *pStatusReturn
) {
    m_pData = (void*)pData;
    m_sData = sData;

    if (m_pStream.isNil ()) {
	pStatusReturn->MakeClosedStatus ();
	return false;
    }

#if defined(_WIN32)
    switch (m_pStream->Mode ()) {
    case VkStreamMode_Synchronous:
	if (!WriteFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, 0)) {
	    pStatusReturn->MakeErrorStatus ();
	    return false;
	}
	OnCompletion ();
	break;

    case VkStreamMode_Overlapped:
	if (!WriteFile (StreamHandle (), m_pData, m_sData, (LPDWORD)&m_sData, &m_iOverlapped))
	    return RegisterOverlappedOperation (VkEvent::State_WriteWait, pStatusReturn);

	OnCompletion ();
	break;

    default:
	m_sData = V_Min (0x8000, sData);
	return StartOperation (VkEvent::State_WriteWait, pStatusReturn);
    }
    return true;

#else
    return StartOperation (VkEvent::State_WriteWait, pStatusReturn);
#endif
}
