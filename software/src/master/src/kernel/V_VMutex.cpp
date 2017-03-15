/*****  V::VMutex Implementation  *****/

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

#include "V_VMutex.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************
 ***********************
 *****             *****
 *****  V::VMutex  *****
 *****             *****
 ***********************
 ***********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VMutex::VMutex () {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    m_hMutex = CreateMutex (NULL, false, NULL);
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    pthread_mutexattr_t iAttributes;
    pthread_mutexattr_init (&iAttributes);
    pthread_mutexattr_settype (&iAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&m_iMutex, &iAttributes);
    pthread_mutexattr_destroy (&iAttributes);
#endif
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VMutex::~VMutex () {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    CloseHandle (m_hMutex);
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    pthread_mutex_destroy (&m_iMutex);
#endif
}

/***********************
 ***********************
 *****  Operation  *****
 ***********************
 ***********************/

bool V::VMutex::lock (Claim *pClaim, bool bWait) {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    return WaitForSingleObject (m_hMutex, bWait ? INFINITE : 0) == WAIT_OBJECT_0 && attachClaim (pClaim);

#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    return (bWait ? pthread_mutex_lock (&m_iMutex) : pthread_mutex_trylock (&m_iMutex)) == 0 && attachClaim (pClaim);
#endif
}

bool V::VMutex::unlock  (Claim *pClaim) {
#if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
    return detachClaim (pClaim) && ReleaseMutex (m_hMutex) != 0;
#elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
    return detachClaim (pClaim) && pthread_mutex_unlock (&m_iMutex) == 0;
#endif
}
