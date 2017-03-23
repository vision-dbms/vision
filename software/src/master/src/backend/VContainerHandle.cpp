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

#include "M_CPD.h"

#include "Vdd_Store.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

#ifdef VMS_LINUX_EXPLICIT_COMPAT
template class VStoreHandle_<VContainerHandle>;
#endif


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VContainerHandle);

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


/***********************************
 ***********************************
 *****  Marking and Traversal  *****
 ***********************************
 ***********************************/

void VContainerHandle::traverseReferences (visitFunction fp) {
}

void VContainerHandle::startMark() {
    if (!m_bVisited) {
	m_bVisited = true;
	traverseReferences(&ThisClass::mark);
    }
}

void VContainerHandle::mark() {
#if defined(DEBUG_GC_CYCLE_DETECTION)
    if (!m_bVisited && m_iHandleRefCount > 0) { //bad
	fprintf(stderr, "non-visited node has aux ref count\n");
    }
#endif

    m_iHandleRefCount++;

#if defined(DEBUG_GC_CYCLE_DETECTION)
    if (m_iHandleRefCount == referenceCount()) { // good!
	fprintf(stderr, "found all references for a handle!\n");
    } else if (m_iHandleRefCount > referenceCount()) { // bad
	fprintf(stderr, "reference count exceeded for a handle\n");
    }
#endif

    startMark();
}

void VContainerHandle::unmark() {
#if defined(DEBUG_GC_CYCLE_DETECTION)
    if (!m_bVisited && m_iHandleRefCount > 0) { // bad
	fprintf(stderr, "non-visited node has aux ref count\n");
    }
#endif
    if (m_bVisited) {
	m_bVisited = false;
	m_iHandleRefCount = 0;
    }
}


/*****************************
 *****************************
 *****  Type Conversion  *****
 *****************************
 *****************************/

Vdd::Store *VContainerHandle::getStore_() {
    return 0;
}

bool VContainerHandle::LocateNameOf (Vdd::Store *pStore, M_TOP &rIdentity) const {
    VContainerHandle::Reference pStoreHandle;
    pStore->getContainerHandle (pStoreHandle);
    return LocateNameOf (pStoreHandle, rIdentity);
}

bool VContainerHandle::LocateOrAddNameOf (Vdd::Store *pStore, M_TOP &rIdentity) const {
    VContainerHandle::Reference pStoreHandle;
    pStore->getContainerHandle (pStoreHandle);
    return LocateOrAddNameOf (pStoreHandle, rIdentity);
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void VContainerHandle::describe_(bool bVerbose) {
    M_CPD *pCPD = GetCPD ();
    if (bVerbose)
	RTYPE_Print (pCPD, -1);
    else
	RTYPE_RPrint (pCPD, -1);
    pCPD->release ();
}
