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

#include "VSimpleFile.h"

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

/*********************************
 *********************************
 *****  Lifetime Management  *****
 *********************************
 *********************************/

bool VContainerHandle::onDeleteThis () {
    if (m_pDCTE.isntNil () && (m_pDCTE->isntReferenced () || !g_bPreservingHandles && m_pContainer && !m_bPrecious)) {
	if (M_AND::GarbageCollectionRunning ()) {
	    generateLogRecord ("DeleteInGC");
	}
	m_pDCTE->setToContainerAddress (m_pContainer, m_bReadWrite);
	m_pDCTE->discard (m_pASD, containerIndexNC ());
	m_pDCTE.clear ();
    }
    return m_pDCTE.isNil ();
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


/***********************************
 ***********************************
 *****  Marking and Traversal  *****
 ***********************************
 ***********************************/

void VContainerHandle::visitReferencesUsing (Visitor *visitor) {
}

void VContainerHandle::cdMarkFor(M_ASD::GCVisitCycleDetect *visitor) {
    m_iGCState.incrementInterhandleReferenceCount ();
    generateLogRecord ("CDMark");

    if (m_iGCState.onFirstCDVisit ()) {
	if (hasNoIdentity ()) {
	    // no identity -> no CTE -> can't be queued -> must visit now
	    visitReferencesUsing (visitor);
	} else if (!m_pDCTE->cdVisited ()) {
	    m_pDCTE->cdVisited (true);
	    m_pASD->GCQueueInsert (containerIndex ());
	}
    }
}

void VContainerHandle::gcMarkFor (M_ASD::GCVisitMark *visitor) {
    generateLogRecord ("GCMark");

    if (m_iGCState.onFirstGCVisit ()) {
	if (hasNoIdentity ()) {
	    // no identity -> no CTE -> can't be queued -> must visit now
	    visitReferencesUsing (visitor);
	} else if (!m_pDCTE->gcVisited ()) {
	    m_pDCTE->gcVisited (true);
	    m_pASD->GCQueueInsert (containerIndex ());
	}
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

/*************************************
 *************************************
 *****  Description and Logging  *****
 *************************************
 *************************************/

void VContainerHandle::describe_(bool bVerbose) {
    M_CPD *pCPD = GetCPD ();
    if (bVerbose)
	RTYPE_Print (pCPD, -1);
    else
	RTYPE_RPrint (pCPD, -1);
    pCPD->release ();
}

void VContainerHandle::generateLogRecord (char const *pWhere) const {
    static VSimpleFile sFile;
    static bool bFirst = true;
    static bool bValid = false;
    if (bFirst) {
	bFirst = false;
	char const *pLogName = getenv ("VisionHandleLog");
	bValid = pLogName
	    && sFile.OpenForTextAppend (pLogName)
	    && sFile.PutString ("================ ")
	    && sFile.PutLine (Space ()->UpdateAnnotation ());
    }
    if (bValid) {
	sFile.printf ("%-12s:", pWhere);
	generateReferenceReport (sFile, 0);
	sFile.flush ();
    }
}

void VContainerHandle::generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const {
    if (xLevel > 0) {
	rOutputFile.printf ("%12u:", xLevel);
    }

    rOutputFile.printf ("%2s", hasAContainer () ? "C" : "");

    if (m_pDCTE) { // Implies 'hasAnIdentity'
	rOutputFile.printf (
	    "[%2u:%5u] CRC:%u%s",
	    spaceIndex (), containerIndex (),
	    m_pDCTE->referenceCount (),
	    m_pDCTE->gcVisited () ? " GCV" : ""
	);
    } else {
	rOutputFile.printf ("[%2u]", spaceIndex ());
    }

    rOutputFile.printf (
	"%s%s RC:%u CDRC:%u: %s\n",
	cdVisited () ? " CDV" : "",
	exceededReferenceCount () ? " EXCESS" : foundAllReferences () || m_pDCTE && m_pDCTE->foundAllReferences () ? " FA+" : " FA-",
	referenceCount (),
	cdReferenceCount (),
	RTypeName ()
    );
}
