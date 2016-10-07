/*****  VContainerHandle Implementation  *****/

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

#include "VContainerHandle.h"

/************************
 *****  Supporting  *****
 ************************/


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VContainerHandle);

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void VContainerHandle::AcquireAccessLock () {
    if (m_fOwnsAnUnusedAccessLock)
	m_fOwnsAnUnusedAccessLock = false;
    else if (0 == m_iAccessCount++)
	m_pDCTE->retain ();
}

void VContainerHandle::ReleaseAccessLock () {
    m_fOwnsAnUnusedAccessLock = false;
    if (0 == --m_iAccessCount) {
	if (g_fPreservingHandles || m_pTransientExtension.isntEmpty ())
	    createReference ();
	else {
	    m_pDCTE->setToContainerAddress (m_pContainerAddress, m_fIsReadWrite);
	}
	m_pDCTE->release (m_pASD, ContainerIndex ());
	deleteReference ();
    }
}

transientx_t *VContainerHandle::TransientExtensionIfA (VRunTimeType const& rRTT) const {
    return TransientExtensionIsA (rRTT) ? TransientExtension () : static_cast<transientx_t*>(0);
}

bool VContainerHandle::TransientExtensionIsA (VRunTimeType const& rRTT) const {
    VRunTimeType *const pRTT = m_pTransientExtension ? m_pTransientExtension->rtt () : static_cast<VRunTimeType*>(0);
    return pRTT && pRTT->isA (rRTT);
}

/**********************************
 **********************************
 *****  Enumeration Handlers  *****
 **********************************
 **********************************/

typedef bool (*CallF) (VContainerHandle*, V::VArgList const&);
typedef void (*CallP) (VContainerHandle*, V::VArgList const&);

bool VContainerHandle::CallFunction (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    CallF const pFunction = iArgList.arg<CallF>();
    return pFunction (this, iArgList);
}

bool VContainerHandle::CallProcedure (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    CallP const pProcedure = iArgList.arg<CallP> ();
    pProcedure (this, iArgList);
    return true;
}

bool VContainerHandle::DoNothing (VArgList const&) {
    return true;
}
