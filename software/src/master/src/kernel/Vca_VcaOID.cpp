/*****  Vca::VcaOID Implementation  *****/

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

#include "Vca_VcaOID.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VInterfaceMember.h"

#include "Vca_VcaPeer.h"
#include "Vca_VcaSelf.h"

#include "V_VLogger.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VcaOID::Export  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOID::Export::Export (VcaOID *pOID, VcaPeer *pImporter, bool bWeak)
:   m_pOID			(pOID)
,   m_pImporter			(pImporter)
,   m_pImporterListSuccessor	(m_pImporter->m_pExportListHead)
,   m_cExports			(bWeak ? 0 : 1)
,   m_cWeakExports		(bWeak ? 1 : 0)
{
    if (m_pImporterListSuccessor)
	m_pImporterListSuccessor->m_pImporterListPredecessor = this;
    m_pImporter->m_pExportListHead = this;
    m_pImporter->incrementExportCount (bWeak);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOID::Export::~Export () {
    clearExportCount ();
    clearWeakExportCount ();
}


/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

void Vca::VcaOID::Export::clearExportCount () {
    if (m_cExports > 0) {
	m_cExports = 0;
	m_pImporter->decrementExportCount ();
    }
}

void Vca::VcaOID::Export::clearWeakExportCount () {
    if (m_cWeakExports > 0) {
	m_cWeakExports = 0;
	m_pImporter->decrementWeakExportCount ();
    }
}

void Vca::VcaOID::Export::incrementExportCount (bool bWeak) {
    if (!bWeak)
	m_cExports++;
    else {
	incrementWeakExportCount ();
    }
}

void Vca::VcaOID::Export::incrementWeakExportCount () {
    if (0 == m_cWeakExports++)
	m_pImporter->incrementWeakExportCount ();
}

void Vca::VcaOID::Export::decrementExportCount (count_t cExports, count_t cWeakExports) {
    if (m_cExports > cExports)
	m_cExports -= cExports;
    else {
	clearExportCount ();
    }
    if (m_cWeakExports > cWeakExports)
	m_cWeakExports -= cWeakExports;
    else {
	clearWeakExportCount ();
    }
    if (0 == m_cExports && 0 == m_cWeakExports)
	onFinalExport ();
}

/**  This actually strengthens the reference because the original export count is
 *   preserved AND the object won't be reclaimed as long a weak reference exists.
 *   This apparent contradiction in terms is simply because 'weak'ness in this
 *   case applies to the lifetime of the process exporting this object, not the
 *   object itself.
 */
bool Vca::VcaOID::Export::weakenExport () {
//    m_pOID->logAsInteresting ("Weaken Export");
    incrementWeakExportCount ();
    return true;
}

void Vca::VcaOID::Export::onDefunctPeer () {
    clearWeakExportCount ();
    clearExportCount ();
    onFinalExport ();
}

void Vca::VcaOID::Export::onFinalExport () {
    if (m_pImporterListPredecessor)
	m_pImporterListPredecessor->m_pImporterListSuccessor = m_pImporterListSuccessor;
    else
	m_pImporter->m_pExportListHead = m_pImporterListSuccessor;
    if (m_pImporterListSuccessor)
	m_pImporterListSuccessor->m_pImporterListPredecessor = m_pImporterListPredecessor;
    m_pImporterListPredecessor = m_pImporterListSuccessor = 0;

    m_pOID->detach (this);
//  No member access beyond this point - 'this' may have just been reclaimed !!!
}

void Vca::VcaOID::Export::onShutdown () {
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaOID::Export::displayInfo () const {
    BaseClass::displayInfo ();
    VString sUUID;
    if (m_pImporter) {
	VkUUIDHolder const iUUID (m_pImporter->uuid ());
	iUUID.GetString (sUUID);
    }
    display (" Exports To %s: %u", sUUID.content (), m_cExports);
}

void Vca::VcaOID::Export::getInfo (VString &rResult, const VString &rPrefix) const {
    VString sUUID;
    if (m_pImporter) {
	VkUUIDHolder const iUUID (m_pImporter->uuid ());
	iUUID.GetString (sUUID);
    }
    rResult << rPrefix;
    rResult.printf ("Export To: %s, Strong: %u, Weak: %u", sUUID.content (), m_cExports, m_cWeakExports);
}


/*************************
 *************************
 *****               *****
 *****  Vca::VcaOID  *****
 *****               *****
 *************************
 *************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaOID::VcaOID (VcaSSID const &rSSID) : m_iSSID (rSSID), m_cMessagesReported (0), m_bInteresting (false) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaOID::~VcaOID () {
}

/*****************************
 *****************************
 *****  Message Counts   *****
 *****************************
 *****************************/

void Vca::VcaOID::updateMessageCounts (count_t cReported, count_t cReceived, count_t cProcessed) {
    m_cMessagesReported  += cReported;
}


/***************************
 ***************************
 *****  Object Export  *****
 ***************************
 ***************************/

Vca::count_t Vca::VcaOID::exportCount () const {
    count_t cExports = 0;
    for (unsigned int xElement = 0; xElement < m_iExportTable.cardinality (); xElement++)
	cExports += m_iExportTable[xElement]->exportCount ();
    return cExports;
}

Vca::count_t Vca::VcaOID::weakExportCount () const {
    count_t cWeakExports = 0;
    for (unsigned int xElement = 0; xElement < m_iExportTable.cardinality (); xElement++)
	cWeakExports += m_iExportTable[xElement]->weakExportCount ();
    return cWeakExports;
}

void Vca::VcaOID::createExportTo (VcaPeer *pPeer, bool bWeak) {
    unsigned int xExport;
    if (!m_iExportTable.Insert (pPeer, xExport))
	m_iExportTable[xExport]->incrementExportCount (bWeak);
    else {
	if (objectsTrace ())
	    traceInfo (string ("%s %p: On first export %p.", rttName ().content (), this, pPeer));
	m_iExportTable[xExport] = new Export (this, pPeer, bWeak);
	if (dynamic_cast<VcaOIDR*>(this)) {
	    logAsInteresting ("Re-Export");
	}
    }
}

void Vca::VcaOID::createExportTo (VcaSelf *pSelf, bool bWeak) {
    pSelf->createLoopbackOf (this, bWeak);
}

void Vca::VcaOID::deleteExportTo (
    VcaPeer *pPeer, count_t cExports, count_t cWeakExports, count_t cMessages
) {
    Reference const iRetainer (this);
//  Adjust the OID's expected message count, ...
//  m_cMessagesReported += cMessages;
    updateMessageCounts (cMessages, 0, 0);

//  ... and the peer's export count, ...
    unsigned int xPeer;
    if (m_iExportTable.Locate (pPeer, xPeer))
	m_iExportTable[xPeer]->decrementExportCount (cExports, cWeakExports);
    else if (m_iExportTable.isEmpty ())
	onFinalExport ();

    if (isInteresting ()) {
	VString iLabel;
	iLabel.printf ("DEx: %u %u", cExports, cMessages);
	logIt (iLabel);
    }
}

bool Vca::VcaOID::weakenExportTo (VcaPeer *pPeer) {
//  Adjust the peer's export count, ...
    unsigned int xPeer;
    return m_iExportTable.Locate (pPeer, xPeer) && m_iExportTable[xPeer]->weakenExport ();
}

void Vca::VcaOID::detach (Export *pExport) {
    if (objectsTrace ()) traceInfo (
	string ("%s %p: On final export %p.", rttName ().content (), this, pExport->importer ())
    );

//  Protect this OID from reclamation, ...
    Reference const iRetainer (this);

//  ... remove the export from from the export table (should trigger the Export's destruction), ...
    m_iExportTable.Delete (pExport->importer ());

//  ... signal final export for the benefit of any subclasses, ...
    if (m_iExportTable.isEmpty ())
	onFinalExport ();

//  ... and allow the OID to be reclaimed if its time has come.
}

void Vca::VcaOID::onFinalExport () {
}


/*********************************
 *********************************
 *****  Tracing and Display  *****
 *********************************
 *********************************/

void Vca::VcaOID::displayInfo () const {
    BaseClass::displayInfo ();
    display (
	" Importers: %u, Exports: %u, Messages Reported: %u",
	m_iExportTable.cardinality (), exportCount (), m_cMessagesReported
    );
    displayExportTable ();
}

void Vca::VcaOID::getInfo (VString &rResult, const VString &rPrefix) const {
    BaseClass::getInfo (rResult, rPrefix);
    rResult.printf (
	" SSID: %u, Exports: %u+%u, Messages Reported: %u",
	m_iSSID.value (), exportCount (), weakExportCount (), m_cMessagesReported
    );
    VString iPrefix;
    iPrefix << rPrefix << "  ";
    if (m_iExportTable.cardinality () > 0) {
	rResult << "\n";
	getExportTable (rResult, VString (rPrefix) << "  ");
    }
}

void Vca::VcaOID::displayExportTable () const {
    for (unsigned int xExport = 0; xExport < m_iExportTable.cardinality (); xExport++)
	m_iExportTable[xExport]->displayInfo ();
}

void Vca::VcaOID::getExportTable (VString &rResult, const VString &rPrefix) const {
    bool bNeedsNewline = false;
    for (unsigned int xExport = 0; xExport < m_iExportTable.cardinality (); xExport++) {
        if (bNeedsNewline) rResult << "\n";
        bNeedsNewline = true;
	m_iExportTable[xExport]->getInfo (rResult, rPrefix);
    }
}

void Vca::VcaOID::logAsInteresting (char const *pWhere) {
    setInteresting ();
    logIt (pWhere);
}

void Vca::VcaOID::logIfInteresting (char const *pWhere) const {
    if (isInteresting ())
	logIt (pWhere);
}

void Vca::VcaOID::logIt (char const *pWhere) const {
    static bool bTracingOIDs (V::GetEnvironmentBoolean ("TracingOIDs"));
    if (bTracingOIDs && isInteresting () && !isAtExit ()) {
	static V::VLogger const iLogger (VString () << "p_" << getpid () << "_oid.log");

	static V::VTwiddler bInitialized;
	if (bInitialized.setIfClear ()) {
	    VString iUUIDText; {
		VkUUIDHolder const iUUID (self ()->uuid ());
		iUUID.GetString (iUUIDText);
	    }
	    iLogger.printf ("****************\n*** Self: %s\n", iUUIDText.content ());
	}

	VString iInfo;
	getInfo (iInfo, "  ");

	iLogger.printf ("%s\n%s\n", pWhere, iInfo.content ());
    }
}
