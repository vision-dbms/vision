/*****  V::VThreadSpecific Implementation  *****/

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

#include "V_VThreadSpecific.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VThread.h"


/*************************************
 *************************************
 *****                           *****
 *****  V::VThreadSpecific::Key  *****
 *****                           *****
 *************************************
 *************************************/

namespace {
    static V::VThreadSpecific::Key::Pointer pKeyListHead;
}

V::VThreadSpecific::Key::Pointer &V::VThreadSpecific::Key::KeyListHead () {
    return pKeyListHead;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VThreadSpecific::Key::Key () {
#if !defined(V_THREAD_ENABLED)
    m_hKey = UINT_MAX;
    m_bKey = false;
#elif defined(_WIN32)
    m_hKey = TlsAlloc ();
    m_bKey = m_hKey != TLS_OUT_OF_INDEXES;
#else
    m_bKey = pthread_key_create (&m_hKey, (pthread_key_destructor_t*)&V::VThreadSpecific::OnThreadTermination) == 0;
#endif

    Pointer &rKeyListHead = KeyListHead ();
    m_pSuccessor.setTo (rKeyListHead);
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (this);
    rKeyListHead.setTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VThreadSpecific::Key::~Key () {
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else
	KeyListHead ().setTo (m_pSuccessor);

    if (m_bKey) {
#if !defined(V_THREAD_ENABLED)
#elif defined(_WIN32)
	TlsFree (m_hKey);
#else
	pthread_key_delete (m_hKey);
#endif
    }
}

/********************************
 ********************************
 *****  Thread Association  *****
 ********************************
 ********************************/

void V::VThreadSpecific::Key::OnThreadTermination () {
//  This code should use a cursor that detects key deletions...
    Pointer pKey (KeyListHead ());
    while (pKey) {
	VReference<ThreadSpecific> pSpecific;
	if (pKey->getSpecific (pSpecific))
	    pSpecific->onThreadDetach ();
	pKey.setTo (pKey->m_pSuccessor);
    }
}

bool V::VThreadSpecific::Key::getSpecific (VReference<VThreadSpecific> &rpSpecific) const {
    if (!m_bKey)
	rpSpecific.clear ();
    else 
#if !defined(V_THREAD_ENABLED)
	rpSpecific.clear ();
#elif defined(_WIN32)
	rpSpecific.setTo (reinterpret_cast<VThreadSpecific*>(TlsGetValue (m_hKey)));
#else
	rpSpecific.setTo (reinterpret_cast<VThreadSpecific*>(pthread_getspecific (m_hKey)));
#endif
    return rpSpecific.isntNil ();
}

bool V::VThreadSpecific::Key::setSpecific (VThreadSpecific *pNewSpecific) const {
    if (!m_bKey)
	return false;

#if !defined(V_THREAD_ENABLED)
    VThreadSpecific *pOldSpecific = 0;
    bool bSuccessful = false;
#elif defined(_WIN32)
    VThreadSpecific *pOldSpecific = reinterpret_cast<VThreadSpecific*> (TlsGetValue (m_hKey));
    bool bSuccessful = 0 != TlsSetValue (m_hKey, pNewSpecific);
#else
    VThreadSpecific *pOldSpecific = reinterpret_cast<VThreadSpecific*> (pthread_getspecific (m_hKey));
    bool bSuccessful = 0 == pthread_setspecific (m_hKey, pNewSpecific);
#endif
    if (bSuccessful && pOldSpecific != pNewSpecific) {
	if (pOldSpecific)
	    pOldSpecific->onThreadDetach ();
	pNewSpecific->onThreadAttach ();
    }
    return bSuccessful;
}


/********************************
 ********************************
 *****                      *****
 *****  V::VThreadSpecific  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (V::VThreadSpecific);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VThreadSpecific::VThreadSpecific (Thread *pThread) : m_pThread (pThread) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VThreadSpecific::~VThreadSpecific () {
}

/********************************
 ********************************
 *****  Thread Association  *****
 ********************************
 ********************************/

void V::VThreadSpecific::OnThreadTermination (ThisClass *pThis) {
    pThis->onThreadDetach ();
}

void V::VThreadSpecific::onThreadAttach () {
    retain ();
}

void V::VThreadSpecific::onThreadDetach () {
    release ();
}
