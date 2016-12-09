/*****  V::VSemaphore Implementation  *****/

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

#include "V_VSemaphore.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"


/***************************
 ***************************
 *****                 *****
 *****  V::VSemaphore  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VSemaphore::VSemaphore (value_t iInitialValue) {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    m_hSemaphore = CreateSemaphore (NULL, iInitialValue, INT_MAX, NULL);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    sem_init (&m_iSemaphore, 0, iInitialValue);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    m_iValue = iInitialValue;
    pthread_mutex_init (&m_iMutex, 0);
    pthread_cond_init  (&m_iCondvar, 0);
#endif
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VSemaphore::~VSemaphore () {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    CloseHandle (m_hSemaphore);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    sem_destroy (&m_iSemaphore);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    pthread_mutex_destroy (&m_iMutex);
    pthread_cond_destroy  (&m_iCondvar);
#endif
}

/***********************
 ***********************
 *****  Operation  *****
 ***********************
 ***********************/

void V::VSemaphore::produce () {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    ReleaseSemaphore (m_hSemaphore, 1, 0);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    sem_post (&m_iSemaphore);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    pthread_mutex_lock (&m_iMutex);
    m_iValue++;
    pthread_mutex_unlock (&m_iMutex);
    pthread_cond_signal (&m_iCondvar);
#endif
}

void V::VSemaphore::consume () {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    WaitForSingleObject (m_hSemaphore, INFINITE);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    bool bNotDone = false;
    do {
	if (sem_wait (&m_iSemaphore)) switch (errno) {
	case EINTR:
	    bNotDone = true;
	    break;
	default:
	    break;
	}
    } while (bNotDone);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    pthread_mutex_lock (&m_iMutex);
    while (0 == m_iValue)
	pthread_cond_wait (&m_iCondvar, &m_iMutex);
    m_iValue--;
    pthread_mutex_unlock (&m_iMutex);
#endif
}

bool V::VSemaphore::consumeTry () {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    return WAIT_TIMEOUT != WaitForSingleObject (m_hSemaphore, 0);
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    bool bSuccessful = true;
    bool bNotDone = false;
    do {
	if (sem_trywait (&m_iSemaphore)) {
	    switch (errno) {
	    case EINTR:
		bNotDone = true;
		break;
	    default:
		bSuccessful = false;
		break;
	    }
	}
    } while (bNotDone);
    return bSuccessful;
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    bool bSuccessful = false;
    if (pthread_mutex_lock (&m_iMutex) == 0) {
	if (m_iValue > 0) {
	    m_iValue--;
	    bSuccessful = true;
	}
	pthread_mutex_unlock (&m_iMutex);
    }
    return bSuccessful;
#endif
}

V::VSemaphore::WaitStatus V::VSemaphore::consumeTry (VTime const &rLimit) {
#if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
    return WAIT_TIMEOUT != WaitForSingleObject (m_hSemaphore, 0) ?
      WaitStatus_Success : WaitStatus_Timeout;
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
    timespec iLimit;
    rLimit.asValue (iLimit);

    bool bNotDone = false;
    do {
	if (sem_timedwait (&m_iSemaphore, &iLimit)) {
	    switch (errno) {
	    case EINTR:
		bNotDone = true;
		break;
	    case ETIMEDOUT:
		return WaitStatus_Timeout;
	    default:
		return WaitStatus_Failure;
	    }
	}
    } while (bNotDone);
    return WaitStatus_Success;
#elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
    timespec iLimit;
    rLimit.asValue (iLimit);

    if (pthread_mutex_lock (&m_iMutex) != 0)
	return WaitStatus_Failure;

    WaitStatus xResultStatus = WaitStatus_Success;
    bool bNotDone = false;
    do {
	switch (pthread_cond_timedwait (&m_iCondvar, &m_iMutex, &iLimit)) {
	case 0:
	    if (m_iValue > 0) {
		m_iValue--;
	    } else {
		bNotDone = true;
	    }
	    break;
	case ETIMEDOUT:
	    xResultStatus = WaitStatus_Timeout;
	    break;
	default:
	    xResultStatus = WaitStatus_Failure;
	    break;
	}
    } while (bNotDone);

    pthread_mutex_unlock (&m_iMutex);

    return xResultStatus;
#endif
}
