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

void VContainerHandle::visitReferencesUsing (visitFunction visitor) {
}

void VContainerHandle::cdMark() {
    m_iHandleRefCount++;
    generateLogRecord ("CDMark");

    if (m_bVisited)
	return;

    m_bVisited = true;
    if (hasNoIdentity ()) { // no identity -> no CTE -> can't be queued -> must visit now
	visitReferencesUsing (&ThisClass::cdMark);
    } else if (!m_pDCTE->cdVisited ()) {
	m_pDCTE->cdVisited (true);
	m_pASD->GCQueueInsert (containerIndex ());
    }
}

void VContainerHandle::gcMark () {
    generateLogRecord ("GCMark");

    if (hasNoIdentity ()) { // no identity -> no CTE -> can't be queued -> must visit now
/*****************
 *  Because containers with no identity have no container table entry to mark as
 *  'gcVisited', the following 'visitReferencesUsing' will be executed once for
 *  each handle that references this handle.  While redundant, it is safe because
 *  of the idempotency of the queue insertion of 'else if' clause that follows and
 *  that ultimately terminates the recusion.  Adding a gc marking guard such as a
 *  gc generation number to handles (which is something we ultimately ought to do)
 *  will eliminate the wasteful visits, but for now...
 *****************/
	visitReferencesUsing (&ThisClass::gcMark);
    } else if (!m_pDCTE->gcVisited ()) {
	m_pDCTE->gcVisited (true);
	m_pASD->GCQueueInsert (containerIndex ());
    }
}

void VContainerHandle::unmark() {
    generateLogRecord ("Unmark");

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
	bValid = pLogName && sFile.OpenForTextAppend (pLogName) && sFile.PutLine ("================");
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
