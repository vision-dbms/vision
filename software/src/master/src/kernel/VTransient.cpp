/*****  V_VTransient Implementation  *****/

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

#include "VTransient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VAllocator.h"
#include "V_VArgList.h"

#include "VTransientServices.h"


/************************
 ************************
 *****              *****
 *****  VTransient  *****
 *****              *****
 ************************
 ************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

VTransient::ObjectTracing VTransient::g_bTracingObjects = Tracing_NotInitialized;

/*****************************************
 *****************************************
 *****  Transient Services Provider  *****
 *****************************************
 *****************************************/

VTransientServices *VTransient::g_pTSP = 0;

static VTransientServices g_iDefaultTSP;

void VTransient::attachTransientServicesProvider (VTransientServices *pTSP) {
    if (!g_pTSP || g_pTSP == &g_iDefaultTSP)
	g_pTSP = pTSP;
}

void VTransient::detachTransientServicesProvider (VTransientServices *pTSP) {
    if (g_pTSP == pTSP)
	g_pTSP = 0;
}


/*******************************
 *******************************
 *****  Memory Allocation  *****
 *******************************
 *******************************/

void *VTransient::operator new (size_t sObject) {
    return V::VThreadSafeAllocator::Data.allocate (sObject);
}

void VTransient::operator delete (void *pObject, size_t sObject) {
    V::VThreadSafeAllocator::Data.deallocate (pObject, sObject);
}

void *VTransient::allocate (size_t cBytes) {
    return g_pTSP ? g_pTSP->allocate (cBytes) : ::malloc (cBytes);
}

void *VTransient::reallocate (void *pMemory, size_t cBytes) {
    return g_pTSP ? g_pTSP->reallocate (pMemory, cBytes) : 0;
}

void VTransient::deallocate (void *pMemory) {
    if (g_pTSP)
	g_pTSP->deallocate (pMemory);
}

void VTransient::DisplayAllocationStatistics () {
    display ("\n====  Standard Allocator:");
    V::VThreadDumbAllocator::Data.displayCounts ();
    display ("\n====  Thread Safe Allocator:");
    V::VThreadSafeAllocator::Data.displayCounts ();
}


/********************************
 ********************************
 *****  Message Formatting  *****
 ********************************
 ********************************/

int __cdecl VTransient::display (char const *pMessage, ...) {
    V_VARGLIST (pArguments, pMessage);
    return vdisplay (pMessage, pArguments);
}

int VTransient::vdisplay (char const *pMessage, va_list pArguments) {
    return g_pTSP ? g_pTSP->vdisplay (pMessage, pArguments) : 0;
}

char const * __cdecl VTransient::string (char const *pMessage, ...) {
    V_VARGLIST (pArguments, pMessage);
    return vstring (pMessage, pArguments);
}

char const *VTransient::vstring (char const *pMessage, va_list pArguments) {
    return g_pTSP ? g_pTSP->vstring (pMessage, pArguments) : "";
}


/**********************************
 **********************************
 *****  Resource Reclamation  *****
 **********************************
 **********************************/

bool VTransient::garbageCollected () {
    return g_pTSP && g_pTSP->garbageCollected ();
}

bool VTransient::mappingsReclaimed () {
    return g_pTSP && g_pTSP->mappingsReclaimed ();
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void __cdecl VTransient::raiseApplicationException (char const *pMessage, ...) const {
    V_VARGLIST (pArguments, pMessage);
    vraiseApplicationException (pMessage, pArguments);
}

void __cdecl VTransient::raiseSystemException (char const *pMessage, ...) const {
    V_VARGLIST (pArguments, pMessage);
    vraiseSystemException (pMessage, pArguments);
}

void __cdecl VTransient::raiseUnimplementedOperationException (char const *pMessage, ...) const {
    V_VARGLIST (pArguments, pMessage);
    vraiseUnimplementedOperationException (pMessage, pArguments);
}

void __cdecl VTransient::raiseException (
    ERRDEF_ErrorCode xErrorCode, char const *pMessage, ...
) const {
    V_VARGLIST (pArguments, pMessage);
    vraiseException (xErrorCode, pMessage, pArguments);
}

void VTransient::vraiseApplicationException (char const *pMessage, va_list pArguments) const {
    if (g_pTSP)
	g_pTSP->vraiseApplicationException (pMessage, pArguments);
}

void VTransient::vraiseSystemException (char const *pMessage, va_list pArguments) const {
    if (g_pTSP)
	g_pTSP->vraiseSystemException (pMessage, pArguments);
}

void VTransient::vraiseUnimplementedOperationException (
    char const *pMessage, va_list pArguments
) const {
    if (g_pTSP)
	g_pTSP->vraiseUnimplementedOperationException (pMessage, pArguments);
}

void VTransient::vraiseException (
    ERRDEF_ErrorCode xErrorCode, char const *pMessage, va_list pArguments
) const {
    if (g_pTSP)
	g_pTSP->vraiseException (xErrorCode, pMessage, pArguments);
}

/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void VTransient::traceInfo (char const *pWhat) const {
    if (objectsTrace ()) {
#if 0
	display ("\n+++ %-50s: %p", pWhat, this);
#else
	fprintf (stderr, "\n+++ %-50s: %p", pWhat, this);
	fflush (stderr);
#endif
    }
}

/*********************
 *********************
 *****  Logging  *****
 *********************
 *********************/

void VTransient::log (char const *pFormat, ...) const {
    V_VARGLIST (ap, pFormat);
    vlog (pFormat, ap);
}

void VTransient::vlog (char const *pFormat, va_list ap) const {
    if (g_pTSP)
	g_pTSP->vlog (pFormat, ap);
}

/**************************
 **************************
 *****  Notification  *****
 **************************
 **************************/

void VTransient::notify (int xEvent, char const *pFormat, ...) const {
    V_VARGLIST (ap, pFormat);
    notify (false, xEvent, pFormat, ap);
}

void VTransient::notify (bool bWaitingForResp, int xEvent, char const *pFormat, ...) const {
    V_VARGLIST (ap, pFormat);
    notify (bWaitingForResp, xEvent, pFormat, ap);

}

void VTransient::notify (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap) const {
    if (g_pTSP)
	g_pTSP->notify (bWaitingForResp, xEvent, pFormat, ap);
}

