/*****  Vca::VcaOIDR Implementation  *****/

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

#include "Vca_VcaOIDR.h" 

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"

#include "Vca_VcaPeer.h"
#include "Vca_VcaSelf.h"
#include "Vca_VTimer.h"


/**************************
 **************************
 *****                *****
 *****  Vca::VcaOIDX  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDX::VcaOIDX (
    SSID const &rObjectSSID, VTypeInfo *pObjectType
) : BaseClass (rObjectSSID), MessageStarter (this), m_pObjectType (pObjectType) {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VcaOIDR *Vca::VcaOIDX::oidr_() {
    return 0;
}

VTypeInfo *Vca::VcaOIDX::objectType_() const {
    return m_pObjectType;
}

bool Vca::VcaOIDX::defersTo (VMessageScheduler &rScheduler) {
    return true;
}

/******************************
 ******************************
 *****  Proxy Management  *****
 ******************************
 ******************************/

void Vca::VcaOIDX::supplyInterface_(IVUnknown::Reference &rpProxy) {
    IVUnknown::Reference pNewProxy;
    rpProxy.setTo (m_pProxy);
    while (rpProxy.isNil ()) {
	if (pNewProxy.isNil ())
	    pNewProxy.setTo (m_pObjectType->proxy (this));
	rpProxy.setTo (
	    m_pProxy.interlockedSetIfNil (pNewProxy) ? pNewProxy.referent () : m_pProxy.referent ()
	);
    }
}

bool Vca::VcaOIDX::detachInterface (IVUnknown *pProxy) {
//  Don't argue with the next three lines of nastiness --  they work around an ugly
//  little bug in the VMS compiler's peephole optimization of atomic operations.
    bool bThis = this ? true : false;
    bool bCleared = bThis && m_pProxy.interlockedClearIf (pProxy);
    return bCleared;
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VcaOIDR::Import  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDR::Import::Import (VcaOIDR *pOIDR, VcaPeer *pExporter, bool bWeak)
:   m_pOIDR			(pOIDR)
,   m_pExporter			(pExporter)
,   m_pExporterListSuccessor	(m_pExporter->m_pImportListHead)
,   m_cImports			(bWeak ? 0 : 1)
,   m_cWeakImports		(bWeak ? 1 : 0)
{
    if (m_pExporterListSuccessor)
	m_pExporterListSuccessor->m_pExporterListPredecessor.setTo (this);
    m_pExporter->m_pImportListHead = this;
    m_pExporter->incrementImportCount (bWeak);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOIDR::Import::~Import () {
    clearImportCount ();
    clearWeakImportCount ();
}


/***************************
 ***************************
 *****  Object Import  *****
 ***************************
 ***************************/

void Vca::VcaOIDR::Import::clearImportCount () {
    if (m_cImports > 0) {
	m_cImports = 0;
	m_pExporter->decrementImportCount ();
    }
}

void Vca::VcaOIDR::Import::clearWeakImportCount () {
    if (m_cWeakImports > 0) {
	m_cWeakImports = 0;
	m_pExporter->decrementWeakImportCount ();
    }
}

void Vca::VcaOIDR::Import::incrementImportCount (bool bWeak) {
    if (!bWeak)
	m_cImports++;
    else {
	incrementWeakImportCount ();
    }
}

void Vca::VcaOIDR::Import::incrementWeakImportCount () {
    if (0 == m_cWeakImports++)
	m_pExporter->incrementWeakImportCount ();
}

void Vca::VcaOIDR::Import::deleteRemoteExport (count_t cMessages) {
    m_pExporter->deleteRemoteExport (m_pOIDR, m_cImports, m_cWeakImports, cMessages);
    clearWeakImportCount ();
    clearImportCount ();
    onFinalImport ();
}

bool Vca::VcaOIDR::Import::weakenImport () {
    if (m_cImports > 0 && m_pExporter->weakenRemoteExportOf (m_pOIDR)) {
	m_pOIDR->logAsInteresting ("Weaken Remote Import");
	incrementWeakImportCount ();
	m_pExporter->deleteRemoteExport (m_pOIDR, m_cImports, 0, 0);
	clearImportCount ();
	return true;
    }
    return false;
}

void Vca::VcaOIDR::Import::onDefunctPeer () {
    clearWeakImportCount ();
    clearImportCount ();
    onFinalImport ();
}

void Vca::VcaOIDR::Import::onFinalImport () {
    if (m_pExporterListPredecessor)
	m_pExporterListPredecessor->m_pExporterListSuccessor = m_pExporterListSuccessor;
    else
	m_pExporter->m_pImportListHead = m_pExporterListSuccessor;
    if (m_pExporterListSuccessor)
	m_pExporterListSuccessor->m_pExporterListPredecessor = m_pExporterListPredecessor;
    m_pExporterListPredecessor = m_pExporterListSuccessor = 0;

    VcaOIDR *const pOIDR = m_pOIDR;
    if (pOIDR) {
	m_pOIDR = 0;
	pOIDR->detach (this);
    }

//  No member access beyond this point - in normal operation 'this' was just reclaimed !!!
}

void Vca::VcaOIDR::Import::onShutdown () {
    if (m_pOIDR) {
	count_t const cMessages = m_pOIDR->messagesToReport ();
	if (objectsTrace ())
	    traceInfo (string ("onShutdown: %u", cMessages));
	if (cMessages > 0)
	    deleteRemoteExport (cMessages);
    }
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaOIDR::Import::displayInfo () const {
    BaseClass::displayInfo ();
    VString sUUID;
    if (m_pExporter) {
	VkUUIDHolder const iUUID (m_pExporter->uuid ());
	iUUID.GetString (sUUID);
    }
    display (" Import From %s: %u", sUUID.content (), m_cImports);
}

void Vca::VcaOIDR::Import::getInfo (VString &rResult, const VString &rPrefix) const {
    VString sUUID;
    if (m_pExporter) {
	VkUUIDHolder const iUUID (m_pExporter->uuid ());
	iUUID.GetString (sUUID);
    }
    rResult << rPrefix;
    rResult.printf ("Import From: %s, Strong: %u, Weak: %u", sUUID.content (), m_cImports, m_cWeakImports);
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VcaOIDR::Manager  *****
 *****                         *****
 ***********************************
 ***********************************/

namespace Vca {
    class VcaOIDR::Manager : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Manager, VRolePlayer);

    //  Constants
    public:
	enum {
	    GCTick = 500000
	};
	enum {
	    TotalListCount = List_None
	};

    //  Lock
    private:
	typedef V::VSpinlock Lock;

    //  Construction
    public:
	Manager ();

    //  Destruction
    private:
	~Manager () {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  ITrigger Role
    private:
	VRole<ThisClass,ITrigger> m_pITrigger;
    public:
	void getRole (ITrigger::Reference& rpRole) {
	    m_pITrigger.getRole (rpRole);
	}

    //  ITrigger Method
    public:
	void Process (ITrigger *pRole) {
	    cleanInactiveLists ();
	}

    //  Control
	bool isStopping () const {
	    return m_pGCTimer.isNil ();
	}
	void stop ();

    //  Garbage Collection
    private:
	void cleanInactiveLists (bool bAccelerated = false);
	void cleanList (List xList);

    public:
	List activeInactiveListIndex () const {
	    return m_xActiveInactiveList;
	}
	List gcListIndex () const {
	    return (List)((m_xActiveInactiveList + InactiveListCount - 1) %  InactiveListCount);
	}
	bool hasInactiveProxies ();

	void onDefunctPeer (VcaPeer *pPeer);

    //  Linkage
    public:
	void relink (VcaOIDR *pOIDR, List xList);

    //  Tracing
    public:
	void displayProxyLists (); 
	void displayProxyList (List xList);
	size_t proxyListSize (List xList);

    //  State
    private:
	Lock 			m_iProxyMovementLock;
	VTimer::Reference	m_pGCTimer;
	VcaOIDR::Pointer	m_pProxyLists[TotalListCount];
	List			m_xActiveInactiveList;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDR::Manager::Manager () : BaseClass (VCohort::Vca ()), m_xActiveInactiveList (List_0), m_pITrigger (this) {
    retain (); {
	ITrigger::Reference pTrigger;
	getRole (pTrigger);
	m_pGCTimer.setTo (new VTimer (cohort (), "GC Timer", pTrigger, GCTick, true));
    } untain ();
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VcaOIDR::Manager::stop () {
    if (m_pGCTimer.isntNil ()) {
	m_pGCTimer->cancel ();
	m_pGCTimer.clear ();
    }

/*******
 *---  Fire this object's trigger to delete inactive proxies.  The trigger must be used
 *---  instead of direct call to ensure that the processing is done in a thread with a
 *---  claim on the cohort.
 *******/
    ITrigger::Reference pTrigger;
    getRole (pTrigger);
    pTrigger->Process ();
}
 

/********************************
 ********************************
 *****  Garbage Collection  *****
 ********************************
 ********************************/

/****************************************************************************
 * Routine: cleanInactiveLists
 *   Processes each proxy present in the GC Proxy list. Before
 * procesing the proxy it also cleans up any defunct exported/imported peers.
 *
 * Two possibilities exist for each proxy
 *   Case 1: (Proxy is in GC List)
 *	If such a proxy is not exported to any other peers, then it has aged 
 *	and is reclaimable, so it is deleted from the VcaOIDR's ObjectTable, 
 *   Case 2: (Proxy is in GC List)
 *	If such a proxy has been exported to some other peer then it needs to be 
 *	facilitated before it can be reclaimed.
 *	Case 2a: (Proxy Facilitation Success)
 *		 Facilitation requirement is met. (Successfully created or 
 *		 found a Facilitator object). In this case the 
 *		 Proxy is added to Facilitation list and faciliated.
 *
 *	Case 2b: (Proxy Facilitation Failure)
 *		 Facilitation requirement fails. (Unable to find or create a 
 *		 Facilitator object). Proxy is added to Dead list of proxies.
*******************************************************************************/

void Vca::VcaOIDR::Manager::cleanInactiveLists (bool bAccelerated) {
    displayProxyLists ();

    if (m_pGCTimer && !bAccelerated) {	//  ... a sweep in the normal aging process
	m_pGCTimer->setRepeating (hasInactiveProxies ());
	m_xActiveInactiveList = gcListIndex ();
    }
    else {		//  ... shutdown
        if (m_pGCTimer)
          m_pGCTimer->stop ();

	List xLast = gcListIndex ();
	while (m_xActiveInactiveList != xLast) {
	    cleanList (m_xActiveInactiveList);
	    m_xActiveInactiveList = (List)((m_xActiveInactiveList+1) % InactiveListCount);
	}
    }
    cleanList (m_xActiveInactiveList);
}

void Vca::VcaOIDR::Manager::cleanList (List xList) {
    VcaOIDR *pOIDR = m_pProxyLists[xList];
    while (pOIDR) {
    	VcaOIDR *pNextOID = pOIDR->m_pNext;
	pOIDR->clean ();
	pOIDR = pNextOID;
    }
}

bool Vca::VcaOIDR::Manager::hasInactiveProxies () {
    List xList = m_xActiveInactiveList;
    List xLast = gcListIndex ();
    while (xList != xLast) {
	if (m_pProxyLists [xList])
	    return true;
        xList = (List)((xList+1) % InactiveListCount);
    }
    return false;
}

void Vca::VcaOIDR::Manager::onDefunctPeer (VcaPeer *pPeer) {
    cleanInactiveLists (true);
}


/*******************************
 *******************************
 *****  OIDR List Linkage  *****
 *******************************
 *******************************/

void Vca::VcaOIDR::Manager::relink (VcaOIDR *pOIDR, List xList) {
    VcaOIDR::Pointer &rpPrev = pOIDR->m_pPrevious;
    VcaOIDR::Pointer &rpNext = pOIDR->m_pNext;
    VcaOIDR::List    &rxList = pOIDR->m_xProxyList;

/*>>>>>  BEGIN CRITICAL SECTION  <<<<<*/
    Lock::Claim iLock (m_iProxyMovementLock);

    bool bUnlinked = rxList != List_None;
    if (bUnlinked) {
	if (rpPrev)  
    	    rpPrev->m_pNext.setTo (rpNext);
	else
	    m_pProxyLists[rxList].setTo (rpNext);

	if (rpNext) 
	    rpNext->m_pPrevious.claim (rpPrev);
	else
	    rpPrev.clear ();

	rpNext.clear ();
	rxList = List_None;
    }

    bool bRelinked = xList != List_None;
    if (bRelinked) {
	bRelinked = true;
	if (m_pProxyLists [xList]) {
	    m_pProxyLists [xList]->m_pPrevious.setTo (pOIDR);
	    rpNext.setTo (m_pProxyLists [xList]);
	}
	m_pProxyLists [xList].setTo (pOIDR);
	rxList = xList;

	if (xList < List_Active && m_pGCTimer->isntRunning ()) {
	    m_pGCTimer->setRepeating (true);
	    m_pGCTimer->start ();
	}
    }

    iLock.release ();
/*>>>>>  END CRITICAL SECTION  <<<<<*/

    if (bUnlinked != bRelinked) {
	if (bRelinked)
	    pOIDR->retain ();
	else
	    pOIDR->release ();
    }
}


/*********************
 *********************
 *****  Tracing  *****
 *********************
 *********************/

void Vca::VcaOIDR::Manager::displayProxyLists () {
    if (gcTrace ()) {
        display ("\n****Proxy Life Cycle****");
	display ("\nActive Proxy Count :%u", proxyListSize (List_Active));
	displayProxyList (List_Active);

	display ("\nInactive Proxy Count:");
	List xList = m_xActiveInactiveList;
	do {
	    display ("\n%u  :%u", xList, proxyListSize (xList));
	    displayProxyList (xList);
	    xList = (List)((xList+1) % InactiveListCount);
	} while (xList != m_xActiveInactiveList);

	display ("\nFacilitation Proxy :%u", proxyListSize (List_Facilitation));
	displayProxyList (List_Facilitation);
	display ("\nDead Proxy Count :%u", proxyListSize (List_Dead));
	displayProxyList (List_Dead);
    }
}

void Vca::VcaOIDR::Manager::displayProxyList (List xList) {
    VcaOIDR const *pOIDR = m_pProxyLists[xList];
    while (pOIDR) {
	pOIDR->displayInfo ("");
	pOIDR = pOIDR->m_pNext;
    }
}

size_t Vca::VcaOIDR::Manager::proxyListSize (List xList) {
    size_t count = 0;
    VcaOIDR const *pOIDR = m_pProxyLists[xList];
    while (pOIDR) {
	count++;
	pOIDR = pOIDR->m_pNext;
    }
    return count;
}


/***************************
 ***************************
 *****                 *****
 *****  Vca::VcaOIDR   *****
 *****                 *****
 ***************************
 ***************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VcaOIDR::GCTracing Vca::VcaOIDR::g_bTracingGC = Tracing_NotInitialized;

/*********************
 *********************
 *****  Manager  *****
 *********************
 *********************/

Vca::VcaOIDR::Manager::Reference Vca::VcaOIDR::g_pTheMan;

Vca::VcaOIDR::Manager *Vca::VcaOIDR::TheMan () {
    if (g_pTheMan.isNil ()) {
	Manager::Reference pTheCandidate (new Manager ());
	if (!g_pTheMan.interlockedSetIfNil (pTheCandidate))
	    pTheCandidate->stop ();
    }
    return g_pTheMan;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOIDR::VcaOIDR (
    SSID const &rObjectSSID, VTypeInfo *pObjectType, VcaPeer *pObjectSite
) : BaseClass (rObjectSSID, pObjectType), m_pManager (TheMan ()), m_pObjectSite (pObjectSite), m_xProxyList (List_None), m_cMessagesWeReported (0) {
    //  This should be done using a Manager::Reference, but MSVC can't compile it...
    m_pManager->retain ();
}

/**************************
 **************************
 *****  Destruction   *****
 **************************
 **************************/

Vca::VcaOIDR::~VcaOIDR () {
    logIfInteresting ("DTOR-1");

    displayInfo ("GarbageCollecting Proxy");

//  Detach the OIDR from the site...
    m_pObjectSite->detach (this);

//  ... and delete all remaining imports:
    Iterator iterator (m_iImportTable);
    while (iterator.isNotAtEnd ()) {
	Import::Reference const pImport (iterator);

    //*******************
    //  To ensure that this loop exits, the call to 'deleteRemoteExport' MUST remove
    //  the import referenced by 'pImport' from this OIDR's import table.  That will
    //  only happen if the import's weak import count is zero.  Make sure it is...
    //*******************
	pImport->clearWeakImportCount ();
	pImport->deleteRemoteExport (messagesToReport ());
    }

    logIfInteresting ("DTOR-2");

    //  This should be done using a Manager::Reference, but MSVC can't compile it...
    m_pManager->release ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VString &Vca::VcaOIDR::GetListName (VString &rName, List xList) {
    if (static_cast<int>(xList) < static_cast<int>(InactiveListCount))
	rName.printf ("List %u", xList);
    else switch (xList) {
    case List_Active:
	rName << "Active";
	break;
    case List_Facilitation:
	rName << "Facilitation";
	break;
    case List_Dead:
	rName << "Dead";
	break;
    case List_None:
	rName << "None";
	break;
    default:
	rName.printf ("Unknown %u", xList);
	break;
    }
    return rName;
}

Vca::VcaOIDR *Vca::VcaOIDR::oidr_() {
    return oidr ();
}

uuid_t const &Vca::VcaOIDR::objectSiteUUID () const {
    return m_pObjectSite->uuid ();
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VcaOIDR::importCountForPeer (
    VcaPeer *pImportedFromPeer, count_t &rImportCount
) const {
    unsigned int xPeer;
    if (m_iImportTable.Locate (pImportedFromPeer, xPeer)) {
	rImportCount = m_iImportTable[xPeer]->importCount ();
	return true;
    }
    return false;
}

bool Vca::VcaOIDR::isConnected_() const {
    return isConnected ();
}

bool Vca::VcaOIDR::isUsable_() const {
    return isUsable ();
}

bool Vca::VcaOIDR::isConnected () const {
    return m_pObjectSite && m_pObjectSite->isConnected ();
}

bool Vca::VcaOIDR::isUsable () const {
    return m_pObjectSite && m_pObjectSite->isUsable ();
}

bool Vca::VcaOIDR::requestNoRouteToPeerCallback (
    VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended
) const {
    return m_pObjectSite && m_pObjectSite->requestNoRouteToPeerCallback (rpTicket, pEventSink, bSuspended);
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VcaOIDR::onDefunctPeer (VcaPeer *pPeer) {
    if (g_pTheMan)
	g_pTheMan->onDefunctPeer (pPeer);
}

void Vca::VcaOIDR::onShutdown () {
    if (g_pTheMan)
	g_pTheMan->stop ();
}


/*******************************
 *******************************
 *****  Import Management  *****
 *******************************
 *******************************/

void Vca::VcaOIDR::createImportFrom (VcaPeer *pPeer, bool bWeak) {
    unsigned int xElement;
    if (m_iImportTable.Insert (pPeer, xElement)) {
	if (objectsTrace ()) traceInfo (
	    string ("%s %p: On first import %p.", rttName ().content (), this, pPeer)
	);
	m_iImportTable[xElement] = new Import (this, pPeer, bWeak);
    }
    else 
	m_iImportTable[xElement]->incrementImportCount (bWeak);
}

/****************************************************************************
 * Routine: deleteImportFrom
 * This method is called when a peer releases imports done by another peer.
 * It:
 *  1. Deletes the imported peer from the importedFrom set of this OID
 *  2. Does a deleteRemoteExport (with the import count that is stored) 
 *     to remove the object from the senders side. (Note flushes
 *     the accumulated Unaccounted Message count.)
 *  Note: 
 *	This routine might have to be adapted when Peer Shutdown is 
 *	implemented. Probably need to propagate the ReleaseImport
 *	further downstream.
****************************************************************************/

void Vca::VcaOIDR::deleteImportFrom (VcaPeer *pPeer) {
    traceInfo ("Releasing Import");
    unsigned int xPeer;
    if (m_iImportTable.Locate (pPeer, xPeer))
	m_iImportTable[xPeer]->deleteRemoteExport (messagesToReport ());
}

bool Vca::VcaOIDR::weakenImportFrom (VcaPeer *pPeer) {
    traceInfo ("Weakening Import");
    unsigned int xPeer;
    return m_iImportTable.Locate (pPeer, xPeer) && m_iImportTable[xPeer]->weakenImport ();
}

bool Vca::VcaOIDR::weakenImport () {
    return weakenImportFrom (objectSite ());
}

void Vca::VcaOIDR::detach (Import *pImport) {
    traceInfo (
	string ("%s %p: On final import %p.", rttName ().content (), this, pImport->exporter ())
    );

    m_iImportTable.Delete (pImport->exporter ());
}


/******************************
 ******************************
 *****  Proxy Management  *****
 ******************************
 ******************************/

void Vca::VcaOIDR::supplyInterface_(IVUnknown::Reference &rpProxy) {
    moveToProxyList (List_Active);
    BaseClass::supplyInterface_(rpProxy);
}

bool Vca::VcaOIDR::detachInterface (IVUnknown *pProxy) {
    bool bCleared = BaseClass::detachInterface (pProxy);
    if (bCleared) {
	if (m_pManager->isStopping () || m_pObjectSite->isDefunct ())
    	    clean ();
	else {
	    moveToProxyList (m_pManager->activeInactiveListIndex ());
	}
    }
    return bCleared;
}

/*********************************
 *********************************
 *****  Message Bookkeeping  *****
 *********************************
 *********************************/

Vca::count_t Vca::VcaOIDR::messagesToReport () {
    count_t const cMessagesToReport = m_cMessagesReported + m_cMessagesWeSent - m_cMessagesWeReported;
    m_cMessagesWeReported += cMessagesToReport;
    return cMessagesToReport;
}

/*******************************
 *******************************
 *****  Message Scheduler  *****
 *******************************
 *******************************/

bool Vca::VcaOIDR::defersTo (VMessageScheduler &rScheduler) {
//  Use this object as the message starter if its cohort can be locked, ...
    if (rScheduler.mustSchedule (m_pObjectSite->cohort (), this)) {
//  ... otherwise let the cohort start the message:
	rScheduler.mustSchedule (m_pObjectSite->cohort ());
    }

//  In any case, remote objects always defer to a scheduler:
    return true;
}

/*****************************
 *****************************
 *****  Message Starter  *****
 *****************************
 *****************************/

void Vca::VcaOIDR::start_(VMessage *pMessage) {
    m_pObjectSite->evaluateOutgoing (pMessage);
    m_cMessagesWeSent.increment (); 
}
 

/********************************
 ********************************
 *****  Garbage Collection  *****
 ********************************
 ********************************/

void Vca::VcaOIDR::clean () {
    if (isntExported ())
	moveToProxyList (List_None);
    else if (initiateFacilitation ()) {	//  success
	moveToProxyList (List_Facilitation);
	displayInfo ("Facilitating OIDR");
    } else {				//  failure  
//	moveToProxyList (List_Dead);
	moveToProxyList (List_None);
	displayInfo ("Dead OIDR");
    }
}


/****************************************************************************
 * Routine: eliminateRedundantImports
 *	If there is a direct import from the server site, this function 
 * removes all other imports.
****************************************************************************/

void Vca::VcaOIDR::eliminateRedundantImports () {
    if (importedFrom (m_pObjectSite)) {
	Iterator iterator (m_iImportTable);

	while (iterator.isNotAtEnd ()) {
	    if (m_pObjectSite.referent () == iterator.key ())
		++iterator;
	    else {
		Import::Reference const pImport (iterator);
		pImport->deleteRemoteExport (0);
	    }
	}
    }
}

/****************************************************************************
 * Routine: getBestImportedFromPeer
 * Operation:
 *   This function returns a peer from the importedFrom set which is the best
 * among the set of imported peers, interms of routing hopcount to the oid's 
 * object site. This routine is utilized when redirecting downstream import 
 * peers to use an upstream export peer during the process of facilitation.
 *
 * Case 1: The object site of "this" OID, is one of the importer, return the 
 *	   object site peer 
 * Case 2: The importedFrom peer set and the object site's routing peers table
 *	   have some peers in common. Returns an importedFrom peer which has
 *	   the best routing distance to the object site.
 * Case 3: The importedFrom peer set and the object site's routing peers table 
 *	   are disjoint. Returns an importedFrom peer (0th element) as we
 *	   cant determine which peer is the best in the import table set.
 * Case 4: Return NULL, if there is no importedFromPeer
****************************************************************************/

Vca::VcaPeer* Vca::VcaOIDR::getBestImportedFromPeer () {
    VcaPeer *pObjectSite = m_pObjectSite;

    if (importedFrom (pObjectSite))
	return pObjectSite;			// case 1

    unsigned int bestHopCount = UINT_MAX;
    VcaPeer *pBestImportedFromPeer = NULL;

    Iterator iterator (m_iImportTable);	
    while (iterator.isNotAtEnd ()) {		//  case 2 
	VcaPeer *pImportedFromPeer = iterator.key ();   
	if (pObjectSite->containsRoutingPeer (pImportedFromPeer)) {
	    unsigned int rHopCount;
	    pObjectSite->getHopCountFor (pImportedFromPeer, rHopCount);
	    if (rHopCount < bestHopCount)
		pBestImportedFromPeer = pImportedFromPeer;
	    ++iterator;
	}
    }

    if (IsNil (pBestImportedFromPeer) && m_iImportTable.isntEmpty ()) {	//  case 3
	pBestImportedFromPeer = m_iImportTable[0]->exporter ();
    }
    return pBestImportedFromPeer;
}


/************************************
 ************************************
 *****  Proxy List Maintenance  *****
 ************************************
 ************************************/

/****************************************************************************
 * Routine: moveToProxyList
 * Operation: 
 *     This method move "this" VcaOIDR to the Proxy List specified by the 
 * argument. Also updates the proxy's list-index member to reflect the new 
 * list in which it is present. 
 * Precondition: xList is a valid List Index.
 *
 *  The presence of an OID in one of the proxy lists creates a reference to
 *  the OID.  Given that OID's move from one list to another, the reference
 *  count management mechanism is streamlined.  Rather than use explicit
 *  references to link the OID's to their lists, at most one reference,
 *  maintained by this routine is kept.  The strategy is simple - moving an
 *  object to a list creates a reference, removing it from a list removes
 *  the reference, and moving it from one list to another or from nowhere
 *  to nowhere does nothing.
****************************************************************************/

void Vca::VcaOIDR::moveToProxyList (List xList) {
    if (m_xProxyList != xList)
	m_pManager->relink (this, xList);
}


/***************************************
 ***************************************
 ***** Proxy Facilitation Routines *****
 ***************************************
 ***************************************/

/****************************************************************************
 * Routine: initiateFacilitation
 * Operation:
 *	 This function is called on proxy objects. This function is called
 * when a proxy just passes through the host peer. This function initiates the 
 * process of creating a virtual export-import relationship between a "best" 
 * importedFrom peer and all the exportedToPeers. An iterator iterates through
 * the exportedToSet of peers and for each combination of the best 
 * "upstreamimportedFromPeer" and the "downstreamExportPeer" sends a message
 * to the upstream peer to faciliate this proxy to the target downstream peer.
 * 
 * Error Cases: "this" is not a proxy OR there is importedFrom peer.
****************************************************************************/

bool Vca::VcaOIDR::initiateFacilitation () {
//     VcaPeer *pImportedFromPeer = getBestImportedFromPeer ();
//     if (pImportedFromPeer) {
// 	ExportTable const &rExportTable = exportTable ();
// 	for (unsigned int xExport = 0; xExport < rExportTable.cardinality (); xExport++) {
// 	    Export::Reference const pExport (rExportTable[xExport]);
// 	    pImportedFromPeer->facilitateProxy (this, pExport->importer ());
// 	}
// 	return true;
//     }
    return false;
}

void Vca::VcaOIDR::addFacilitationPeers (VcaPeer *pTargetPeer, VcaPeer *pSourcePeer) {
    unsigned int xEntry;
    m_iFacilitationMap.Insert (pTargetPeer, xEntry);
    m_iFacilitationMap[xEntry] = pSourcePeer;
}

bool Vca::VcaOIDR::deleteFacilitationTo (VcaPeer *pTargetPeer) {
    return m_iFacilitationMap.Delete (pTargetPeer);
}

bool Vca::VcaOIDR::sourceFacilitationPeerFor (VcaPeer *pTargetPeer, VcaPeer *&rpSourcePeer) {
    unsigned int xEntry;
    if (m_iFacilitationMap.Locate (pTargetPeer, xEntry)) {
	rpSourcePeer = m_iFacilitationMap [xEntry];
	return true;
    }
    return false;
}

/****************************************************************************
 * Routine: refacilitateTo
 * Operation:
 *	This method determines the source faciliation peer and asks it
 * to refacilitate this proxy to a new target. Also deletes the facilitation
 * record from the Pending Facilitation Map
****************************************************************************/

void Vca::VcaOIDR::refacilitateTo (VcaPeer *pNewTargetPeer, VcaPeer *pOldTargetPeer) {
    traceInfo ("Faking Export");
    if (m_iFacilitationMap.Contains (pOldTargetPeer)) {
	VcaPeer *pSourcePeer;
	if (sourceFacilitationPeerFor (pOldTargetPeer, pSourcePeer)) {
	    pSourcePeer->refacilitateProxyTo (this, pNewTargetPeer, pOldTargetPeer);
	    deleteFacilitationTo (pOldTargetPeer);
	}
    }
}


/*********************
 *********************
 *****  Tracing  *****
 *********************
 *********************/

void Vca::VcaOIDR::displayInfo () const {
    BaseClass::displayInfo ();
//     display (
// 	" Exporters: %u, Exports: %u, Messages Reported: %u",
// 	m_iImportTable.cardinality (), exportCount (), m_cMessagesWeSent
//     );
    displayImportTable ();
}

void Vca::VcaOIDR::displayInfo (char const *pWhat) const {
    if (gcTrace ())
	BaseClass::displayInfo (pWhat);
}

void Vca::VcaOIDR::getInfo (VString &rResult, const VString &rPrefix) const {
    BaseClass::getInfo (rResult, rPrefix);
    rResult << ", MsgsWeSent: " << m_cMessagesWeSent << ", MsgsWeReported: " << m_cMessagesWeReported << ", List: ";
    getListName (rResult);
    rResult << "\n" << rPrefix << "  Interface: ";
    objectType()->getName (rResult);
    VString sUUID; {
	VkUUIDHolder const iUUID (objectSiteUUID ());
	iUUID.GetString (sUUID);
    }
    rResult << ", Site: " << sUUID;
    if (m_iImportTable.cardinality () > 0) {
	rResult << "\n";
	getImportTable (rResult, VString (rPrefix) << "  ");
    }
}

void Vca::VcaOIDR::displayImportTable () const {
    for (unsigned int xImport = 0; xImport < m_iImportTable.cardinality (); xImport++)
	m_iImportTable[xImport]->displayInfo ();
}

void Vca::VcaOIDR::getImportTable (VString &rResult, const VString &rPrefix) const {
    bool bNeedsNewline = false;
    for (unsigned int xImport = 0; xImport < m_iImportTable.cardinality (); xImport++) {
        if (bNeedsNewline) rResult << "\n";
        bNeedsNewline = true;
	m_iImportTable[xImport]->getInfo (rResult, rPrefix);
    }
}
