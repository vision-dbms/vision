/**
 * @file
 * Defines V::VCondvar class.
 */

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

#include "V_VCondvar.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"


/*************************
 *************************
 *****               *****
 *****  V::VCondvar  *****
 *****               *****
 *************************
 *************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VCondvar::VCondvar () {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    m_hEvent = CreateEvent (NULL, false, false, NULL);
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    pthread_cond_init (&m_iCondvar, 0);
#endif
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VCondvar::~VCondvar () {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    CloseHandle (m_hEvent);
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    pthread_cond_destroy (&m_iCondvar);
#endif
}

/***********************
 ***********************
 *****  Operation  *****
 ***********************
 ***********************/

void V::VCondvar::signal () {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    SetEvent (m_hEvent);
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    pthread_cond_signal (&m_iCondvar);
#endif
}

bool V::VCondvar::wait() {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    HANDLE handles[] = {m_hEvent, m_hMutex};
    enum {
	handleCount = sizeof (handles) / sizeof (handles[0])
    };

    DWORD xResult = WaitForMultipleObjects (handleCount, handles, true, INFINITE);
    return xResult >= WAIT_OBJECT_0 && xResult < WAIT_OBJECT_0 + handleCount;
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    return pthread_cond_wait (&m_iCondvar, &m_iMutex) == 0;
#endif
}

V::VCondvar::WaitStatus V::VCondvar::waitUntil (VTime const& rLimit) {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    HANDLE handles[] = {m_hEvent, m_hMutex};
    enum {
	handleCount = sizeof (handles) / sizeof (handles[0])
    };

    DWORD xResult = WaitForMultipleObjects (handleCount, handles, true, INFINITE);
    return xResult >= WAIT_OBJECT_0 && xResult < WAIT_OBJECT_0 + handleCount ? WaitStatus_Success : WaitStatus_Failure;
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    timespec iLimit;
    rLimit.asValue (iLimit);
    switch (pthread_cond_timedwait (&m_iCondvar, &m_iMutex, &iLimit)) {
    case 0:
	return WaitStatus_Success;
    case ETIMEDOUT:
	return WaitStatus_Timeout;
    default:
	return WaitStatus_Failure;
    }
#endif
}
