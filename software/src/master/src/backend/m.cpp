/*****  The Memory Manager  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName M

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "batchvision.h"

#include "ps.h"

#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vunwind.h"
#include "vutil.h"

#include "V_VAllocator.h"
typedef V::VArgList VStdargList;

#include "VDatabaseFederatorForBatchvision.h"

#include "Vdd_Store.h"

#include "RTpct.h"

/*****  Shared  *****/
#include "m.h"


/**************************
 **************************
 *****  Global State  *****
 **************************
 **************************/

/******************************
 *****  The Root Network  *****
 ******************************/

PublicVarDef M_AND::Reference M_AttachedNetwork;

/***************************************
 *****  Active Update Information  *****
 ***************************************/
/*-----------------------------------------------------------------------*
 *----  Separability information temporarily used by M_ASD::Persist  ----*
 *-----------------------------------------------------------------------*/

PrivateVarDef bool NetworkUpdateIsAdministrative = false;

/******************************
 *****  Session GC State  *****
 ******************************/

M_AND::GenIndex_t M_AND::g_xGCGeneration = 0;
bool M_AND::g_bGarbageCollectionRunning = false;
bool M_AND::g_bNetworkGarbageCollectedInSession = false;

bool M_DCTE::g_bPotentialSessionGarbage = false;

/**********************
 *****  Switches  *****
 **********************/

bool VContainerHandle::g_bPreservingHandles = true;

#if defined(_DEBUG) && !defined(INCLUDING_XDB_REFERENCE_TRACING)
#define INCLUDING_XDB_REFERENCE_TRACING
#endif

#if defined(INCLUDING_XDB_REFERENCE_TRACING)
PrivateVarDef bool TracingCrossDBReferences = false;
#endif

/* turn on for session GC debugging */
// #define DEBUG_SESSION_GC

/**********************
 *****  Counters  *****
 **********************/

PrivateVarDef unsigned int
    AllocateContainerCount		= 0,
    DeallocateContainerCount		= 0,
    ShiftContainerTailCount		= 0,
    ReallocateContainerCount		= 0,
    EnableModificationsCount		= 0;


/****************************************************
 ****************************************************
 *****  Update Error Code Translation Routines  *****
 ****************************************************
 ****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to translate an 'PS_UpdateStatus' into a descriptive
 *****  message.
 *
 *  Arguments:
 *	updateStatus		- the 'PS_UpdateStatus' to be translated.
 *	alternateSuccessText	- an optional ('Nil' if absent) string to be
 *				  substituted for the default success message.
 *
 *  Returns:
 *	A string describing the update status code.
 *
 *****/
PublicFnDef char const* M_UpdateStatusDescription (
    PS_UpdateStatus updateStatus, char const* alternateSuccessText
) {
    switch (updateStatus) {
    case PS_UpdateStatus_Successful:
	return alternateSuccessText
	    ? alternateSuccessText : "Update Operation Successful";
    case PS_UpdateStatus_Incomplete:
	return "In Progress";
    case PS_UpdateStatus_Inconsistent:
	return "Accessed Network Out Of Date";
    case PS_UpdateStatus_NotPermitted:
	return "No Permission For Attempted Operation";
    case PS_UpdateStatus_NotSeparable:
	return "Local Update Would Disrupt Network Integrity";
    case PS_UpdateStatus_NoLicense:
	return "License Does Not Permit Network Update From This Session";
    case PS_UpdateStatus_Disabled:
	return "Updates Disabled For This Session";
    default:
	return UTIL_FormatMessage (
	    "Unknown Update Status '%d'", updateStatus
	);
    }
}


/*************************
 *************************
 *****               *****
 *****  class M_RTD  *****
 *****               *****
 *************************
 *************************/

/*---------------------------------------------------------------------------
 * The memory manager maintains an internal data base which contains the
 * values of a number of representation type specific parameters which it
 * needs to perform its memory management functions.  The following
 * declarations, definitions, and utilities (along with the representation
 * type description definitions of "m.d") define that data base.
 *---------------------------------------------------------------------------
 */

/*********************************
 *****  RTD DB Declarations  *****
 *********************************/

/*****  DB Array  *****/
PublicVarDef M_RTD M_RTDDB [(int)RTYPE_C_MaxType + 1];


/*******************************************
 *****  RTD DB Initialization Routine  *****
 *******************************************/

M_RTD::M_RTD () : m_xType ((RTYPE_Type)(this - M_RTDDB)) {
    m_fReuseableCPDs		= false;
    m_iCPDPointerCount		= 1;
    m_iCPDCreateCount		=
    m_iCPDDeleteCount		=
    m_iHandleCreateCount	=
    m_iHandleDeleteCount	= 0;
    m_pHandleMaker		= &VContainerHandle::Maker;
    m_pCPDInitFn		= NilOf (M_Type_CPDInitFn);
    m_pReclaimFn		= NilOf (M_Type_ReclaimFn);
    m_pMarkFn			= NilOf (M_Type_MarkFn);
    m_pConvertFn		= NilOf (M_Type_ConvertFn);
}

void M_RTD::construct () {
/*****  Request a type description from the type handler  *****/
    RTYPE_PerformHandlerOperation (RTYPE_InitializeMData, m_xType, this);
}

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize the representation type data base used
 *****  by the memory manager.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void InitializeRTDDB () {
    for (unsigned int i = 0; i < RTYPE_C_MaxType; i++)
	M_RTDPtr (i)->construct ();
}

PrivateFnDef void GetCPCCCounts (unsigned int &rACount, unsigned int &rDCount) {
    rACount = rDCount = 0;

    for (unsigned int i = 0; i < RTYPE_C_MaxType; i++) {
	M_RTD const *pRTD = M_RTDPtr (i);
	rACount += M_RTD_HandleCreateCount (pRTD);
	rDCount += M_RTD_HandleDeleteCount (pRTD);
    }
}

PrivateFnDef void GetCPDCounts (unsigned int &rACount, unsigned int &rDCount) {
    rACount = rDCount = 0;

    for (unsigned int i = 0; i < RTYPE_C_MaxType; i++) {
	M_RTD const *pRTD = M_RTDPtr (i);
	rACount += M_RTD_CPDCreateCount (pRTD);
	rDCount += M_RTD_CPDDeleteCount (pRTD);
    }
}


/*************************
 *************************
 *****               *****
 *****  class M_CTE  *****
 *****               *****
 *************************
 *************************/

/************************
 *****  Definition  *****
 ************************/

class M_CTE {
    friend class M_AND;
    friend class M_ASD;
    friend class VContainerHandle;

//  Construction
public:
    M_CTE (M_AND *pAND, M_POP const *pReference)
	: m_pASD	(pAND->AccessASD (pReference))
	, m_xContainer	(M_POP_ContainerIndex (pReference))
	, m_pDCTE	(m_pASD->cte (m_xContainer))
    {
    }

    M_CTE (M_ASD *pASD, unsigned int xContainer)
	: m_pASD	(pASD)
	, m_xContainer	(xContainer)
	, m_pDCTE	(pASD->cte (xContainer))
    {
    }

/*---------------------------------------------------------------------------
 *  Arguments:
 *	iInitialReferenceCount	- the initial reference count for this CTE
 *				  (typically 1 for new containers, 2 for
 *				  forwarded containers).
 *	asd			- an ASD for the space in which the CTE is to
 *				  be allocated.
 *
 *
 *****/
    M_CTE (unsigned int iInitialReferenceCount, M_ASD *pASD)
	: m_pASD	(pASD)
	, m_xContainer	(pASD->allocateCTE ())
	, m_pDCTE	(pASD->cte (m_xContainer))
    {
	m_pDCTE->initializeForNewContainer (iInitialReferenceCount);
    }

    M_CTE (M_CTE const &rOther)
	: m_pASD	(rOther.m_pASD)
	, m_xContainer	(rOther.m_xContainer)
	, m_pDCTE	(rOther.m_pDCTE)
    {
    }

//  Destruction
public:

//  Access/Query
public:
    unsigned int addressType () const {
	return m_pDCTE->addressType ();
    }

    M_CPreamble *addressAsContainerAddress () const {
	return m_pDCTE->addressAsContainerAddress ();
    }
    VContainerHandle *addressAsContainerHandle () const {
	return m_pDCTE->addressAsContainerHandle ();
    }
    M_POP const &addressAsPOP () const {
	return m_pDCTE->addressAsPOP ();
    }

    unsigned int attentionMask () const {
	return m_pDCTE->attentionMask ();
    }

    M_CPreamble *containerAddress () const {
	return holdsAContainerAddress ()
	    ? addressAsContainerAddress ()
	    : holdsACPCC ()
	    ? addressAsContainerHandle ()->containerAddress ()
	    : NilOf (M_CPreamble*);
    }

    unsigned int containerIndex () const {
	return m_xContainer;
    }

    M_AND *database () const {
	return m_pASD->Database ();
    }

    M_DCTE *dcte () const {
	return m_pDCTE;
    }

    void getPOP (M_POP &rResult) const {
	M_POP_D_ObjectSpace	(rResult) = spaceIndex ();
	M_POP_D_ContainerIndex	(rResult) = containerIndex ();
    }
    bool hasBeenAccessed () const {
	return m_pDCTE->hasBeenAccessed ();
    }

    bool holdsAReadOnlyContainerAddress () const {
	return m_pDCTE->holdsAReadOnlyContainerAddress ();
    }
    bool holdsAWriteableContainerAddress () const {
	return m_pDCTE->holdsAWriteableContainerAddress ();
    }
    bool holdsAContainerAddress () const {
	return m_pDCTE->holdsAContainerAddress ();
    }
    bool holdsAContainerHandle () const {
	return m_pDCTE->holdsAContainerHandle ();
    }
    bool holdsACPCC () const {
	return m_pDCTE->holdsACPCC ();
    }
    bool holdsAForwardingPOP () const {
	return m_pDCTE->holdsAForwardingPOP ();
    }

    bool isAForwardingTarget () const {
	return m_pDCTE->isAForwardingTarget ();
    }
    bool isASweepTarget () const {
	return m_pDCTE->isASweepTarget ();
    }
    bool isATRefRescanTarget () const {
	return m_pDCTE->isATRefRescanTarget ();
    }

    bool isNew () const {
	return m_pDCTE->isNew ();
    }
    bool isntNew () const {
	return !m_pDCTE->isNew ();
    }

    bool underConstruction () const {
	return m_pDCTE->underConstruction ();
    }
    bool isntUnderConstruction () const {
	return m_pDCTE->isntUnderConstruction ();
    }

    bool isReferenced () const {
	return m_pDCTE->isReferenced ();
    }
    bool isntReferenced () const {
	return m_pDCTE->isntReferenced ();
    }

    bool mustBeRemapped () const {
	return m_pDCTE->mustBeRemapped ();
    }
    bool mustStayMapped () const {
	return m_pDCTE->mustStayMapped ();
    }

    bool namesAScratchPadObject () const {
	return m_pASD->IsTheScratchPad ();
    }
    bool namesTheSameObjectAs (M_CTE const &rOther) const {
	return m_pASD == rOther.m_pASD && m_xContainer == rOther.m_xContainer;
    }

    bool persistentlyReferenceableFrom (M_ASD const *pSpace) const {
	return m_pASD == pSpace || database () == pSpace->Database () && (
	    NetworkUpdateIsAdministrative || namesAScratchPadObject () || isntNew ()
	);
    }

    unsigned int referenceCount () const {
	return m_pDCTE->referenceCount ();
    }

    bool referenceCountIsFinite () const {
	return m_pDCTE->referenceCountIsFinite ();
    }
    bool referenceCountIsInfinite () const {
	return m_pDCTE->referenceCountIsInfinite ();
    }

    M_ASD *space () const {
	return m_pASD;
    }

    unsigned int spaceIndex () const {
	return m_pASD->Index ();
    }

//  Attribute Maintenance
public:
    void setHasBeenAccessed () const {
	m_pDCTE->setHasBeenAccessed ();
	m_pASD->AdjustLiveRefBounds (m_xContainer);
    }
    void clearHasBeenAccessed () const {
	m_pDCTE->clearHasBeenAccessed ();
    }

    void setForwardingTargetFlags () const {
	m_pDCTE->setForwardingTargetFlags ();
	m_pASD->AdjustTRBounds (m_xContainer);
    }

    void setTRefFlags () const {
	m_pDCTE->setTRefFlags ();
	m_pASD->AdjustTRBounds (m_xContainer);
    }
    void clearTRefFlags () const {
	m_pDCTE->clearTRefFlags ();
    }

    void clearIsNew () const {
	m_pDCTE->clearIsNew ();
    }

    void setMustBeRemapped () const {
	m_pDCTE->setMustBeRemapped ();
    }
    void clearMustBeRemapped () const {
	m_pDCTE->clearMustBeRemapped ();
    }
    void setMustStayMapped () const {
	m_pDCTE->setMustStayMapped ();
    }
    void clearMustStayMapped () const {
	m_pDCTE->clearMustStayMapped ();
    }

//  Forwarding
public:
    void ForwardToSpace (M_ASD* pTargetSpace);

    M_POP const *FollowForwardings ();

//  Positioning
public:
    void setTo (M_CTE const &rOther) {
	m_pASD		= rOther.m_pASD;
	m_xContainer	= rOther.m_xContainer;
	m_pDCTE		= rOther.m_pDCTE;
    }
    M_CTE &operator= (M_CTE const &rOther) {
	setTo (rOther);
	return *this;
    }

//  Container Table Entry Maintenance
protected:
    void DeallocateCTE () const {
	m_pDCTE->setToFreeListEntry (m_pASD->cteFreeListHead ());
	m_pASD->setFreeListHeadTo (m_xContainer);
    }

public:
    void retain () const {
	m_pDCTE->retain ();
    }
    void untain () const {
	m_pDCTE->untain ();
    }
    void release () const {
	m_pDCTE->release (m_pASD, m_xContainer);
    }

    void setReferenceCountToZero () const {
	m_pDCTE->setReferenceCountToZero ();
    }
    void setReferenceCountToOne () const {
	m_pDCTE->setReferenceCountToOne ();
    }
    void setReferenceCountToInfinity () const {
	m_pDCTE->setReferenceCountToInfinity ();
    }

    bool gcVisited() { 
	return m_pDCTE->gcVisited(); 
    }
    void gcVisited(bool visited) { 
	m_pDCTE->gcVisited(visited);
    }
    bool cdVisited() { 
	return m_pDCTE->cdVisited(); 
    }
    void cdVisited(bool visited) { 
	m_pDCTE->cdVisited(visited);
    }
    bool foundAllReferences() { 
	return m_pDCTE->foundAllReferences(); 
    }
    void foundAllReferences(bool foundAll) { 
	m_pDCTE->foundAllReferences(foundAll);
    }

//  Container Access
public:
/*---------------------------------------------------------------------------
 *****  Internal method to initialize the container address of a CTE.
 *
 *  Notes:
 *	The CTE must not contain a forwarding POP.  A segmentation fault will
 *	probably result if it does.
 *
 *****/
    void InitializeCTEContainerAddress () const {
	M_CPreamble *pAddress = addressAsContainerAddress ();
	if (IsNil (pAddress)) {
	    pAddress = m_pASD->persistentASD ()->AccessContainer (
		m_xContainer, m_pDCTE->hasBeenAccessed ()
	    );
	    m_pDCTE->setHasBeenAccessed ();

	    M_RTD *pRTD = M_RTDPtr (M_CPreamble_RType (pAddress));
	    if (M_RTD_ConvertFn (pRTD)) {
		pAddress = M_RTD_ConvertFn (pRTD) (m_pASD, pAddress);
		setToContainerAddress (pAddress, true);
		m_pASD->AdjustRWBounds (m_xContainer);
	    }
	    else {
		setToContainerAddress (pAddress, false);
		m_pASD->AdjustLiveRefBounds (m_xContainer);
	    }
	}
    }

private:
    VContainerHandle *MakeHandle () {
	VContainerHandle *pHandle = M_RTD_HandleMaker (
	    M_RTDPtr (M_CPreamble_RType (addressAsContainerAddress ()))
	) (*this);
	setToContainerHandle (pHandle);
	return pHandle;
    }

/*---------------------------------------------------------------------------
 *****  Internal method to attach a CPCC to a CTE.
 *
 *  Notes:
 *	This method assumes that the CTE does NOT contain a forwarding POP.
 *	A segmentation fault will probably result if it does.
 *
 *****/
    VContainerHandle *AttachHandle () {
	if (holdsACPCC ())
	    return addressAsContainerHandle ();
	if (holdsAContainerAddress ()) {
	    InitializeCTEContainerAddress ();
	    return MakeHandle ();
	}
	return NilOf (VContainerHandle*);
    }

    VContainerHandle *AttachHandle (RTYPE_Type xExpectedType) {
	FollowForwardings ();
	VContainerHandle *pHandle = AttachHandle ();

	if (xExpectedType != RTYPE_C_Any && xExpectedType != pHandle->RType()) {
	    pHandle->release ();
	    RaiseExpectedTypeException (xExpectedType);
	}

	return pHandle;
    }

    void setToContainerAddress (M_CPreamble *pAddress, bool bReadWrite) const {
	m_pDCTE->setToContainerAddress (pAddress, bReadWrite);
    }
    void clearContainerAddress () const {
	m_pDCTE->clearContainerAddress ();
    }

    void setToContainerHandle (VContainerHandle *pHandle) const {
	m_pDCTE->setToContainerHandle (pHandle);
    }

//  GC Support
public:
    bool Mark (M_ASD::GCVisitBase* pGCV) const {
	return m_pDCTE->mark (pGCV, m_pASD, m_xContainer);
    }

//  Exception Generation
protected:
    void RaiseExpectedTypeException (RTYPE_Type xType) const;

//  State
public:
    M_ASD*		m_pASD;
    unsigned int	m_xContainer;
    M_DCTE*		m_pDCTE;
};

M_POP const *M_CTE::FollowForwardings () {
  M_POP const *pPOP = 0;
  while (holdsAForwardingPOP ()) {
    pPOP = &addressAsPOP ();
    m_pASD = m_pASD->AccessASD (pPOP);
    m_xContainer = M_POP_ContainerIndex (pPOP);
    m_pDCTE = m_pASD->cte (m_xContainer);
  }
  return pPOP;
}


/*****************************
 *****  CTE Reclamation  *****
 *****************************/

void M_DCTE::reclaim (M_ASD *pASD, unsigned int xContainer) {
    if (M_CTEAddressType_CPCC == m_xAddressType) {
	VContainerHandle *pHandle = m_iAddress.asCPCC;
	if (pHandle->isReferenced ())
	    return;
	pHandle->DetachFromCTE ();
    }

    switch (m_xAddressType) {
    case M_CTEAddressType_RWContainer:
	if (gcVisited()) 
	pASD->DestroyContainer (m_iAddress.asContainerAddress);
	else
	    pASD->DeallocateContainer (m_iAddress.asContainerAddress);
	break;

    case M_CTEAddressType_ForwardingPOP:
	pASD->Release (&m_iAddress.asPOP);
	if (g_iForwardedContainerCount-- == 0) {
	    g_iForwardedContainerCount = 0;
	    ERR_SignalFault (
		EC__InternalInconsistency,
		"ReclaimForwardedCTE: Negative Forwarded Container Count"
	    );
	}
	break;
    }

    setToFreeListEntry (pASD->cteFreeListHead ());
    pASD->setFreeListHeadTo (xContainer);
}


/**********************************
 *****  Exception Generation  *****
 **********************************/

void M_CTE::RaiseExpectedTypeException (RTYPE_Type xExpectedType) const {
    RTYPE_ComplainAboutType (
	UTIL_FormatMessage (
	    "M_ASD::GetContainerHandle [%u:%u]", m_pASD->Index (), m_xContainer
	), addressAsContainerHandle ()->RType (), xExpectedType
    );
}


/***********************************
 ***********************************
 *****                         *****
 *****  class M_ASD::CT::Page  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

M_ASD::CT::Page::Page (CT const *pCT, unsigned int xPage) : m_pCT (pCT), m_xPage (xPage) {
//  Count the page size against the space's allocation:
    pCT->incrementSpaceAllocation (sizeof (Page));

//  Initialize persistent container entries to a reasonable default:
    unsigned int xLimit = pCT->persistentEntryCount ();
    if (xLimit > origin ()) {
	xLimit-= origin ();

	if (xLimit > Size)
	    xLimit = Size;

	Entry iEntry;
	iEntry.initializeForOldContainer ();

	for (unsigned int xEntry = 0; xEntry < xLimit; xEntry++)
	    m_iDCTE[xEntry] = iEntry;
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

M_ASD::CT::Page::~Page () {
    m_pCT->decrementSpaceAllocation (sizeof (Page));
}


/*****************************
 *****************************
 *****                   *****
 *****  class M_ASD::CT  *****
 *****                   *****
 *****************************
 *****************************/

/****************************************
 ****************************************
 *****  Page Vector Initialization  *****
 ****************************************
 ****************************************/

void M_ASD::CT::sizePageVector () {
//  On construction, the size of the page vector should match the entry count exactly, ...
    sizePageVector (entryPageCount ());
}

void M_ASD::CT::sizePageVector (unsigned int sPageVector) {
// The new size must be at least big enough to hold all the entries ...
    if (entryPageCount () > sPageVector)
	sPageVector = entryPageCount ();

//  ... and should be padded when the page vector is grown or for scratch pads:
    if (m_sPageVector > 0 || m_pASD->IsTheScratchPad ()) sPageVector = (
	sPageVector + PageVectorGrain - 1
    ) / PageVectorGrain * PageVectorGrain;

//  Finally, resize the vector as appropriate:
    if (sPageVector > m_sPageVector) {
	incrementSpaceAllocation (sizeof (Page*) * (sPageVector - m_sPageVector));
	m_pPageVector = (Page**)reallocate (m_pPageVector, sizeof (Page*) * sPageVector);
	while (m_sPageVector < sPageVector)
	    m_pPageVector[m_sPageVector++] = 0;
    }
    else if (sPageVector < m_sPageVector) {
	decrementSpaceAllocation (sizeof (Page*) * (m_sPageVector - sPageVector));
	while (sPageVector < m_sPageVector) {
	    Page *pPage = m_pPageVector[--m_sPageVector];
	    if (pPage)
		delete pPage;
	}
	if (sPageVector > 0) m_pPageVector = (Page**)reallocate (
	    m_pPageVector, sizeof (Page*) * sPageVector
	);
	else {
	    deallocate (m_pPageVector);
	    m_pPageVector = 0;
	}
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

M_ASD::CT::CT (M_ASD *pASD)
: m_pASD		(pASD)
, m_cEntries		(persistentEntryCount ())
, m_xFreeListHead	(FreeListNil)
, m_xFreeListBase	(0)
, m_sPageVector		(0)
, m_pPageVector		(0)
{
//  Initialize the page vector, ...
    sizePageVector ();

//  ... and initial the free list:
    PS_CT *pPCT = pASD->persistentCT ();
    if (pPCT) {
	for (
	    int xFreeListEntry = PS_CT_FreeList (pPCT);
	    PS_CT_FreeListNil != xFreeListEntry;
	    xFreeListEntry = PS_CTE_NextFree (PS_CT_Entry (pPCT, xFreeListEntry))
	)
	{
	    entry (xFreeListEntry)->setToFreeListEntry (m_xFreeListHead);
	    m_xFreeListHead = xFreeListEntry;
	}
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

M_ASD::CT::~CT () {
    for (unsigned int xPage = 0; xPage < m_sPageVector; xPage++) {
	Page *pPage = m_pPageVector[xPage];
	if (pPage)
	    delete pPage;
    }
}


/******************************
 ******************************
 *****  Entry Allocation  *****
 ******************************
 ******************************/

unsigned int M_ASD::CT::entry () {
    if (freeListIsEmpty ())
	growFreeList ();

    unsigned int xEntry = m_xFreeListHead;
    m_xFreeListHead = entry (xEntry)->addressAsCTI ();

    if (m_xFreeListHead >= entryCount () && m_xFreeListHead != UINT_MAX) {
	IO_printf ("\n+++ Bad free list entry: %u->%08x(%u)\n", xEntry, m_xFreeListHead, m_xFreeListHead);
    }

    return xEntry;
}


/***************************************
 *****  Container Table Expansion  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to expand a container table.
 *
 *  Argument:
 *	this			- the address of an ASD for the space whose
 *				  container table is to be grown.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the ASD only.
 *
 *****/
void M_ASD::CT::growFreeList () {
//  Claim any unused entries on the last page (possible for persistent spaces), ...
    unsigned int oldCount = m_cEntries;
    unsigned int newCount = entryPageCount () * Page::Size;

    if (newCount > oldCount)
	m_cEntries = newCount;
    else {
//  ... or, ...
	newCount += Page::Size;

//  ... if expansion is possible, ...
	if (newCount > M_POP_MaxContainerIndex) {
//  ... if not and a garbage collection frees some entries, whew...
	    if (M_DisposeOfSessionGarbage (true) && freeListIsntEmpty ())
		return;

	    newCount = M_POP_MaxContainerIndex;
	    if (newCount <= oldCount) ERR_SignalFault (
		EC__MError, UTIL_FormatMessage (
		    "No More Room In Container Table (%s:%u)", ndfPathName (), spaceIndex ()
		)
	    );
	}

//  ... grow by expansion:

//  make sure the page vector is large enough to hold all the required pages before setting
//  the entry count higher .... if the reallocation fails, recovery mechanisms may use
//  this container table, so it should be left consistent ...

	sizePageVector (entryPageCount (newCount));

//  make sure the newly required page is allocated, for the same reason ... to give the
//  recovery mechanisms a chance to succeed ....

	page (oldCount);
	m_cEntries = newCount;

	
    }

//  Finally, initialize the new entries, ...
    while (oldCount < newCount) {
	entry (oldCount)->setToFreeListEntry (m_xFreeListHead);
	m_xFreeListHead = oldCount++;
    }

//  ... and align the container table:
    m_pASD->AlignCT ();
}


/***************************
 ***************************
 *****  Page Creation  *****
 ***************************
 ***************************/

M_ASD::CT::Page *M_ASD::CT::page_(unsigned int xPage) const {
//  Validate the page index, ...
    if (xPage >= m_sPageVector) raiseException (
	EC__MError,
	"M_ASD::CT:: CTE Page Index %u Exceeds Limit %u: %s:%u",
	xPage, m_sPageVector, ndfPathName (), spaceIndex ()
    );

//  ... if necessary, allocate the page, ...
    if (!m_pPageVector[xPage])
	m_pPageVector[xPage] = new Page (this, xPage);

//  ... and return it:
    return m_pPageVector[xPage];
}


/**************************
 **************************
 *****                *****
 *****   class M_ASD  *****
 *****                *****
 **************************
 **************************/

/*******************************
 *****  Bounds Adjustment  *****
 *******************************/

void M_ASD::AdjustRWBounds (unsigned int xCTE) {
    m_iReadWriteRegion.include (xCTE);
}

void M_ASD::AdjustTRBounds (unsigned int xCTE) {
    m_iTRefRescanRegion.include (xCTE);
}

void M_ASD::AdjustLiveRefBounds (unsigned int xCTE) {
    m_iLiveReferenceRegion.include (xCTE);
}

void M_ASD::ResetRWBounds () {
    m_iReadWriteRegion.reset ();
}

void M_ASD::ResetTRBounds () {
    m_iTRefRescanRegion.reset ();
}

void M_ASD::ResetLiveRefBounds () {
    m_iLiveReferenceRegion.reset ();
}


/**************************
 *****  Deallocation  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to deallocate a container.
 *
 *  Argument:
 *	pContainerAddress	- the address of the container.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void M_ASD::DestroyContainer (M_CPreamble* pContainerAddress) {
    if (pContainerAddress) {
    DeallocateContainerCount++;

/*****  Erase the container ...  *****/
    M_Type_ReclaimFn reclaimer = M_RTD_ReclaimFn (
	M_RTDPtr (M_CPreamble_RType (pContainerAddress))
    );
    if (reclaimer)
	reclaimer (this, pContainerAddress);

/*****  ... and deallocate its space:  *****/
	DeallocateContainer (pContainerAddress);
    }
}

/*---------------------------------------------------------------------------
 *****  Routine to free a container in transient storage.
 *
 *  Argument:
 *	containerAddress	- the address of the transient container
 *				  to be freed.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void M_ASD::DeallocateContainer (M_CPreamble *containerAddress) {
// Check for reasonable container framing, ...
    M_CPreamble_ValidateFraming (containerAddress);

//  ... grab the size, ...
    unsigned int size = M_CPreamble_Size (containerAddress);

//  ... free the container, ...
    UTIL_Free ((pointer_t)containerAddress);

//  ... and do the accounting
    DecrementAllocation (size);
}


/******************************************
 *****  Reference Access and Testing  *****
 ******************************************/

M_AND *M_AND::ReferencedNetwork (M_POP const *pReference) {
    M_CTE cte (this, pReference);
    cte.FollowForwardings ();
    return cte.space ()->Database ();
}

bool M_AND::ReferencesAreIdentical_(
    M_POP const *pReference, M_AND *pThat, M_POP const *pThatReference
) {
    M_CTE cte1 (this, pReference);
    cte1.FollowForwardings ();

    M_CTE cte2 (pThat, pThatReference);
    cte2.FollowForwardings ();

    return cte1.namesTheSameObjectAs (cte2);
}


/***********************************************************
 ***********************************************************
 *****		The Container Table Manager	       *****
 *****  Part 1. Transient Container Table Maintenance  *****
 ***********************************************************
 ***********************************************************/

/***************************************
 *****  Container Table Alignment  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to align the persistent and transient portions of the
 *****  container table.
 *
 *  Argument:
 *	this			- the address of an ASD whose container tables
 *				  are to be brought into alignment.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void M_ASD::AlignCT () {
    if (m_pPCTCPCC) {
        m_pPCTCPCC->ReallocateContainer (PS_CT_ByteCount (cteCount ()));
	PS_ASD_UpdateCT (m_pPASD) = M_CPreamble_ContainerBaseAsType (
	    m_pPCTCPCC->containerAddress (), PS_CT
	);
	PS_CT_EntryCount (PS_ASD_UpdateCT (m_pPASD)) = cteCount ();
    }
}


/*************************
 *************************
 *****               *****
 *****	class M_AND  *****
 *****               *****
 *************************
 *************************/

/**************************
 *****  ASD Creation  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to create an ASD for a space.
 *
 *  Argument:
 *	xSpace			- the index of the space whose ASD is to be
 *				  created.
 *	aMissingSpaceIsAnError	- a boolean which, when true, instructs this
 *				  routine to abort if the underlying physical
 *				  space does not exist.
 *
 *  Returns:
 *	The address of the ASD created.
 *
 *****/
M_ASD *M_AND::CreateASD (unsigned int xSpace, bool aMissingSpaceIsAnError) {
/*****  Access the underlying physical space...  *****/
    PS_ASD *pASD = 0;
    if (!m_pPhysicalAND->AccessSpace (pASD, xSpace) && aMissingSpaceIsAnError) ERR_SignalFault (
	EC__MError, UTIL_FormatMessage (
	    "Object Space %u Is Undefined In This Network", xSpace
	)
    );

/*****  ... and create and return the new ASD...  *****/
    return pASD ? new M_ASD (this, pASD) : 0;
}


/**************************
 *****  Space Access  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the ASD associated with a space.
 *
 *  Arguments:
 *      rpResult                - a reference to an M_ASD* that will be
 *                                set to point to the M_ASD for the space
 *                                if it was successfully accessed.
 *	xSpace			- the index of the space to access.
 *
 *  Returns:
 *	True if the access succeeded (rpResult can be used), false otherwise.
 *
 *****/
bool M_AND::AccessSpace (M_ASD *&rpResult, unsigned int xSpace) {
    rpResult = xSpace < SpaceCount () ? AccessASD (xSpace, false) : NilOf (M_ASD*);
    return IsntNil (rpResult);
}

/*---------------------------------------------------------------------------
 *****  Routine to return the ASD associated with a space if it has been created
 *
 *  Arguments:
 *	xSpace			- the index of the space to access.
 *	pbSpaceValid		- a pointer to a boolean to be set indicating 
 *				  whether the requested space is in range
 *  Returns:
 *	The address of the ASD or 'Nil' if the space index is invalid, or not yet
 *	accessed.
 *
 *****/
M_ASD* M_AND::AccessSpace (unsigned int xSpace, bool *pbSpaceValid) {
    *pbSpaceValid = xSpace < ActiveSpaceCount ();
    return *pbSpaceValid ? m_iASDVector [xSpace] : NilOf (M_ASD*);
}


/*******************************
 *****  Space Enumeration  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to enumerate the non-imported spaces in a network.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each accessed space.  This
 *				  function will be called with two arguments -
 *				  the address of an ASD for the accessed space
 *				  and a <stdargs.h> argument pointer positioned
 *				  at 'elementProcessorArg1'; it should return
 *				  'true' to request the next space in the
 *				  enumeration and 'false' to terminate the
 *				  enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	Nothing.
 *
 *---------------------------------------------------------------------------
 */
void __cdecl M_AND::EnumerateSpaces (bool (M_ASD::*elementProcessor) (VArgList const&), ...) {
    V_VARGLIST (iArgList, elementProcessor);

    unsigned int spaceCount = SpaceCount ();

    bool okToProceed = true;
    for (unsigned int xSpace = 0; okToProceed && xSpace < spaceCount; xSpace++) {
	M_ASD *pASD = AccessASD (xSpace, false);
	if (pASD && pASD->Database () == this) {
	    okToProceed = (pASD->*elementProcessor) (iArgList);
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate the spaces currently accessed by this process.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each accessed space.  This
 *				  function will be called with two arguments -
 *				  the address of an ASD for the accessed space
 *				  and a <stdargs.h> argument pointer positioned
 *				  at 'elementProcessorArg1'; it should return
 *				  'true' to request the next space in the
 *				  enumeration and 'false' to terminate the
 *				  enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_AND::EnumerateAccessedSpaces (
    bool (M_ASD::*elementProcessor) (VArgList const&), ...
) const {
    V_VARGLIST (iArgList, elementProcessor);

    M_ASD *ringTerminator = 0;
    for (M_ASD *asd = m_pASDRing;

	 asd != ringTerminator && (asd->*elementProcessor) (iArgList);

	 asd = asd->successor (), ringTerminator = m_pASDRing
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate the spaces currently modified by this process.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each modified space.  This
 *				  function will be called with two arguments -
 *				  the address of an ASD for the accessed space
 *				  and a <stdargs.h> argument pointer positioned
 *				  at 'elementProcessorArg1'; it is expected
 *				  to return true to request the next space in
 *				  the enumeration and false to terminate the
 *				  enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_AND::EnumerateModifiedSpaces (
    bool (M_ASD::*elementProcessor) (VArgList const&), ...
) const {
    V_VARGLIST (iArgList, elementProcessor);

    M_ASD *ringTerminator = 0;
    for (M_ASD *asd = m_pASDRing;

	 asd != ringTerminator && (
	    asd->hasntChanged () || (asd->*elementProcessor) (iArgList)
	 );

	 asd = asd->successor (), ringTerminator = m_pASDRing
    );
}


/****************************
 *****  Network Update  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Routine to update the accessed object network.
 *
 *  Arguments:
 *	globalUpdate		- a boolean which, when true, requests that a
 *				  global update be performed.  A global update
 *				  saves all modifications made by this process.
 *				  The alternative to a global update is a local
 *				  update.  A local update saves only those
 *				  modifications made to the 'distinguished'
 *				  space of this process.
 *
 *  Returns:
 *	An 'PS_UpdateStatus' code describing the state of the update.
 *
 *****/
PS_UpdateStatus M_AND::UpdateNetwork (bool globalUpdate) {
/*-----------------------------------------------------------------------*
 *----  Separability information temporarily used by M_ASD::Persist  ----*
 *-----------------------------------------------------------------------*/
    NetworkUpdateIsAdministrative = globalUpdate;

    if (globalUpdate)
	m_pPhysicalAND->UpdateNetwork (this);
    else if (NetworkGarbageCollectedInSession ())
	m_pPhysicalAND->SetUpdateStatusTo (PS_UpdateStatus_NotSeparable);
    else
	m_pASDRing->persistentASD ()->UpdateSpace ();

    return m_pPhysicalAND->UpdateStatus ();
}


/*******************************************
 *****  External Update Incorporation  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to incorporate an external update into the network.
 *
 *  Argument:
 *	xuSpecPathName		- a string containing the path name of an
 *				  external update specification file.
 *
 *  Returns:
 *	An 'PS_UpdateStatus' code describing the state of the update.
 *
 *****/
PS_UpdateStatus M_AND::IncorporateExternalUpdate (char const * xuSpecPathName) {
/*****  Attempt the external update incorporation, ...  *****/
    PS_ASD* pPhysicalASD = m_pPhysicalAND->IncorporateExternalUpdate (xuSpecPathName);

/*****  ... abort if the update failed, ...  *****/
    if (IsNil (pPhysicalASD)) ERR_SignalFault (
	EC__MError,
	UTIL_FormatMessage (
	    "External Update Incorporation Failure: %s",
	    M_UpdateStatusDescription (
		m_pPhysicalAND->UpdateStatus (), NilOf (char const*)
	    )
	)
    );

/*****
 *  ... abort if the updated space is already accessed by this process,  ...
 *****/
    if (IsntNil (m_iASDVector [pPhysicalASD->spaceIndex ()])) ERR_SignalFault (
	EC__MError,
	"External Update Incorporation Successful But Space Already Accessed"
    );

/*****  ... otherwise, access the updated space ...  *****/
    CreateASD (pPhysicalASD->spaceIndex (), true);

/*****  ... and finally, return.  *****/
    return m_pPhysicalAND->UpdateStatus ();
} 


/************************************
 ************************************
 *****                          *****
 *****  class VContainerHandle  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 *****  Construction  *****
 **************************/

/*---------------------------------------------------------------------------
 *  Notes:
 *	Container handles are created with container access, since that is
 *	why there are being created.  Since access requires a reference, the
 *	container handle holds a reference to the underlying CTE.  If access
 *	isn't needed, release it using 'release ()' so that the
 *	container can be garbage collected.
 *****/

/*---------------------------------------------------------------------------
 *****  Container handle construction.
 *
 *  Argument:
 *	rCTE			- a reference to the CTE to which this
 *				  VContainerHandle is being attached.
 *
 *****/
VContainerHandle::VContainerHandle (M_ASD *pContainerSpace, RTYPE_Type xContainerType, size_t sContainer)
: m_pASD		(pContainerSpace)
, m_pRTD		(M_RTDPtr (xContainerType))
, m_bReadWrite		(true)
, m_bPrecious		(false)
{
    pContainerSpace->CreateContainer (xContainerType, sContainer, this); 
    M_RTD_HandleCreateCount (m_pRTD)++;
}

VContainerHandle::VContainerHandle (M_ASD *pContainerSpace, RTYPE_Type xContainerType)
: m_pASD		(pContainerSpace)
, m_pContainer		(0)
, m_pDCTE		(0)
, m_pRTD		(M_RTDPtr (xContainerType))
, m_bReadWrite		(false)
, m_bPrecious		(false)
{
    M_RTD_HandleCreateCount (m_pRTD)++;
}

VContainerHandle::VContainerHandle (M_CTE &rCTE)
: m_pASD		(rCTE.space ())
, m_pDCTE				(rCTE.dcte ())
, m_pContainer		(rCTE.addressAsContainerAddress ())
, m_pContainerIdentity	(&M_CPreamble_POP (m_pContainer))
, m_pRTD		(M_RTDPtr (M_CPreamble_RType (m_pContainer)))
, m_bReadWrite		(rCTE.addressType () == M_CTEAddressType_RWContainer)
, m_bPrecious		(false)
{
    M_RTD_HandleCreateCount (m_pRTD)++;
}

VContainerHandle *VContainerHandle::attachTo (M_CTE &rCTE) {
    m_pContainer = rCTE.addressAsContainerAddress ();
    m_pContainerIdentity.setTo (&M_CPreamble_POP (m_pContainer));
    m_pDCTE.setTo (rCTE.dcte ());
    rCTE.setToContainerHandle (this);
    return this;
}

VContainerHandle *VContainerHandle::Maker (M_CTE &rCTE) {
    return new VContainerHandle (rCTE);
}


/*************************
 *****  Destruction  *****
 *************************/

VContainerHandle::~VContainerHandle () {
    M_RTD_HandleDeleteCount (m_pRTD)++;
}


/*************************
 *****  Association  *****
 *************************/

/*---------------------------------------------------------------------------
 ******  Internal utility to associate a CPCC with a container table entry.
 *
 *  Arguments:
 *      cte			- an initialized 'lval' of type 'M_CTE'
 * 				  which will be modified to refer to 'cpcc'.
 *
 *
 *****/
void VContainerHandle::AttachToCTE (M_CTE &rCTE) {
//  Update the CTE, ...  *****/
    rCTE.setToContainerHandle (this);

//  ... remember the source, ...
    M_ASD *pSourceASD = m_pASD;

//  ... update the handle, ...
    m_pASD = rCTE.space ();
    m_pDCTE.setTo (rCTE.dcte ());

//  ... the container, ...
    if (hasAnIdentity ()) {
	M_POP *pPOP = const_cast<M_POP*>(m_pContainerIdentity.referent ());
	M_POP_ObjectSpace	(pPOP) = rCTE.spaceIndex ();
	M_POP_ContainerIndex	(pPOP) = rCTE.containerIndex ();
    }

//  ... and the space:
    if (!hasAReadOnlyContainer()) {
	m_pASD->AdjustRWBounds (rCTE.containerIndex ());

	if (m_pASD != pSourceASD) {
	    unsigned int sContainer = containerSize ();
	    pSourceASD->DecrementAllocation (sContainer);
	    m_pASD->IncrementAllocation (sContainer);
	}
    }
}

void VContainerHandle::DetachFromCTE () {
    if (m_pDCTE) {
	m_pDCTE->setToContainerAddress (m_pContainer, m_bReadWrite);
	m_pDCTE.clear ();
    }
    discard ();
}


/**********************************************
 *****  Default Callback Implementations  *****
 **********************************************/

void VContainerHandle::CheckConsistency () {
}

bool VContainerHandle::PersistReferences () {
    return true;
}


/******************************
 *****  Container Copier  *****
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to copy a container.
 *
 *  Argument:
 *	oldPreamblePtr		- a pointer to the preamble of an already
 *				  existing container
 *
 *  Returns:
 *	A pointer to the preamble of the copied container.
 *
 *****/
static M_CPreamble *CopyContainer (M_CPreamble* oldPreamblePtr) {
    size_t			size = (size_t)M_CPreamble_Size (oldPreamblePtr)
					+ sizeof (M_CPreamble)
					+ sizeof (M_CEndMarker);
    M_CPreamble*		result = (M_CPreamble*) UTIL_Malloc (size);

    memcpy (result, oldPreamblePtr, size);

    M_CPreamble_FixSizes (result);

    return result;
}

/**************************
 *****  Modification  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to enable modification of a container.
 *****/
void VContainerHandle::EnableModifications () {
    if (hasAReadOnlyContainer ()) {
	EnableModificationsCount++;

	unsigned int sContainer = containerSize ();
	AdjustContainerPointers (CopyContainer (m_pContainer), true);
	m_pASD->persistentASD ()->ReleaseContainer (containerIndexNC (), sContainer);
	m_pASD->IncrementAllocation (sContainer);
    }
}


/**************************
 *****  Reallocation  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to re-size a container in transient storage.
 *
 *  Arguments:
 *	containerAddress	- the address of the preamble of the container
 *				  to be resized.
 *	newSize			- the new size the container in bytes.
 *
 *  Returns:
 *	The new address of the container.
 *
 *****/
M_CPreamble *M_ASD::ReallocateContainer (M_CPreamble *containerAddress, size_t newSize) {
    M_CPreamble_ValidateFraming (containerAddress);

    newSize = RoundContainerSize (newSize);

    size_t oldSize = M_CPreamble_Size (containerAddress);
    M_CPreamble *result = reinterpret_cast<M_CPreamble*> (
	UTIL_Realloc (
	    reinterpret_cast<pointer_t>(containerAddress), sizeof (M_CPreamble) + sizeof (M_CEndMarker) + newSize
	)
    );
    if (newSize > oldSize)
	IncrementAllocation (newSize - oldSize);
    else
	DecrementAllocation (oldSize - newSize);

    M_CPreamble_NSize (result) = newSize;
    M_CEndMarker_Size (M_CPreamble_EndMarker (result)) = newSize;

    return result;
}


/*---------------------------------------------------------------------------
 *****  Routine to change the size of a container to a new absolute size.
 *
 *  Arguments:
 *	cpcc			- a CPCC for the container to be resized.
 *	newSize			- the new size of the container in bytes.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void VContainerHandle::ReallocateContainer (size_t newSize) {
    if (hasAContainer ()) {
    ReallocateContainerCount++;
    EnableModifications ();

	M_CPreamble *pNewContainerAddress = m_pASD->ReallocateContainer (m_pContainer, newSize);
	if (m_pContainer != pNewContainerAddress)
	    AdjustContainerPointers (pNewContainerAddress, true);
    }
    else {
	unsigned int xContainer = containerIndex ();
	m_pASD->AdjustTRBounds (xContainer);
	m_pDCTE->setTRefFlags ();

	m_pContainer = m_pASD->AllocateContainer (RType (), newSize, xContainer);
	m_pContainerIdentity.setTo (&M_CPreamble_POP (m_pContainer));
	m_bReadWrite = true;
    }
}


/************************
 *****  Relocation  *****
 ************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to adjust the physical pointers associated with a
 *****  Container Pointer after a container movement.
 *
 *  Argument:
 *	pNewAddress		- the new address of the container.
 *	bNewAddressIsReadWrite	- a boolean which indicates, when true, that
 *				  the new container address references
 *				  modifiable memory and, when false, that the
 *				  new address references read only memory.
 *
 *  Returns:
 *	NOTHING
 *
 *
 *****/
void VContainerHandle::AdjustContainerPointers (
    M_CPreamble* pNewAddress, bool bNewAddressIsReadWrite
) {
/*****  Determine the container movement amount, ...  *****/
    if (hasNoContainer ())
	return;

    pointer_diff_t addressDelta = (pointer_size_t)pNewAddress - (pointer_size_t)m_pContainer;

/*****  ...fix the container handle, ...  *****/
    m_pContainer = pNewAddress;
    m_pContainerIdentity.setTo (&M_CPreamble_POP (m_pContainer));
    m_bReadWrite = bNewAddressIsReadWrite;

/*****  ... adjust the ASD if necessary, ...  *****/
    if (bNewAddressIsReadWrite)
	m_pASD->AdjustRWBounds (containerIndexNC ());

/*****  ... and fix the CPD addresses.  *****/
    unsigned int iPointerCount = cpdPointerCount ();
    for (M_CPD *cpd = m_pCPDChainHead; cpd; cpd = M_CPD_NLink (cpd)) {
	M_CPD_PreamblePtr (cpd) = pNewAddress;

	pointer_t	*p, *pl;
	for (p = (pointer_t *)M_CPD_Pointers (cpd), pl = p + iPointerCount; p < pl; p++) {
	    if (*p)
	        *p += addressDelta;
	}
    }
}


/************************
 *****  Forwarding  *****
 ************************/

unsigned int M_DCTE::g_iForwardedContainerCount = 0;

/*---------------------------------------------------------------------------
 *****  Utility to forward a CPCC to a new space.
 *
 *  Arguments:
 *	cte			- a CTE for an existing Scratch Pad container.
 *	pTargetSpace		- an ASD for the space to which the container
 *				  will be forwarded.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void M_CTE::ForwardToSpace (M_ASD* pTargetSpace) {
//  Access the container, ...
    VContainerHandle::Reference pHandle (AttachHandle ());

//  ... allocate a CTE for the container in its new space, ...
    /*****************************************************************************
     *  CTEs used as the target of a forwarding have an initial reference count  *
     *  of 1 that reflects the reference from the original CTE in its new role   *
     *  as a forwarder.								 *
     *****************************************************************************/
    M_CTE newCTE (1, pTargetSpace);

//  ... move the container, ...
    pHandle->AttachToCTE (newCTE);

//  ... change the old CTE to a forwarder, ...
    M_TOP iName;
    m_pASD->LocateOrAddNameOf (pTargetSpace, newCTE.containerIndex (), iName);

    M_POP iPOP;
    iName.getPOP (iPOP);
    m_pDCTE->setToForwardingPOP (iPOP);

    newCTE.setForwardingTargetFlags ();
}

/*---------------------------------------------------------------------------
 *****  Utility to forward a CPCC to a new space.
 *
 *  Arguments:
 *	this			- a container handle for an existing Scratch Pad
 *				  container.
 *	pTargetSpace		- an ASD for the space to which the container
 *				  will be forwarded.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
bool VContainerHandle::forwardToSpace_(M_ASD *pTargetSpace) {
    if (pTargetSpace == m_pASD)
	return true;

/*****-------------------------------------------------------------------*****
 *****	               DISALLOW CROSS-DATABASE FORWARDING                *****
 *****                                                                   *****
 *****	Cross database forwarding requires all POPs in a container be    *****
 *****  changed to POPs valid in the namespace of the target database.   *****
 *****-------------------------------------------------------------------*****/
    if (m_pASD->Database () != pTargetSpace->Database ())
	return false;

/*****-------------------------------------------------------------------*****
 *****	               DISALLOW PERSISTENCE CHANGES                      *****
 *****-------------------------------------------------------------------*****/
    if (isntInTheScratchPad () || pTargetSpace->IsTheScratchPad ())
	return false;

    M_CTE oldCTE (m_pASD, containerIndex ());
    oldCTE.ForwardToSpace (pTargetSpace);

    return true;
}


/*************************************************************
 *************************************************************
 *****		 The Container Table Manager	         *****
 *****  Part II. Persistent Container Table Maintenance  *****
 *************************************************************
 *************************************************************/

/**************************************************************
 *****  Persistent Container Table Modification Enabling  *****
 **************************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to enable modification of the persistent portion of
 *****  a space's container table.
 *
 *  Argument:
 *	asd			- the ASD for the space whose persistent
 *				  container table is to be made modifiable.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Notes:
 *	This routine will move the persistent portion of the container table
 *	into read-write memory and align it with its transient portion.
 *
 *****/
void M_ASD::EnablePCTModification () {
    if (m_pPCTCPCC.isNil ()) {
    //  Access the persistent container table, ...
	m_pPCTCPCC.setTo (
	    static_cast<rtPCT_Handle*>(GetContainerHandle (M_KnownCTI_ContainerTable, RTYPE_C_PCT))
	);

    //  ... align it with the transient container table, ...
	AlignCT ();

    //  ... and rebuild its free list:
	PS_CT *pPCT = PS_ASD_UpdateCT (m_pPASD);
	PS_CT_FreeList (pPCT) = PS_CT_FreeListNil;

	for (
	    unsigned int xFreeEntry = cteFreeListHead ();
	    xFreeEntry != CT::FreeListNil;
	    xFreeEntry = cte (xFreeEntry)->addressAsCTI ()
	) {
	    PS_CTE &rPCTE = PS_CT_Entry (pPCT, xFreeEntry);
	    PS_CTE_IsCopy	(rPCTE) = false;
	    PS_CTE_IsFree	(rPCTE) = true;
	    PS_CTE_NextFree	(rPCTE) = PS_CT_FreeList (pPCT);
	    PS_CT_FreeList	(pPCT) = xFreeEntry;
	}
    }
}


/***********************************************
 ***********************************************
 *****	    The Object Space Manager	   *****
 *****  Part III - Enumeration and Update  *****
 ***********************************************
 ***********************************************/

/*******************************
 *****  Update Compulsion  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to force the update of a space at the next save.
 *
 *  Arguments:
 *	this			- the address of a space ASD.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void M_ASD::CompelUpdate () {
    AdjustRWBounds (M_KnownCTI_ContainerTable);
}


/*******************************
 *****  Space Distinction  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to distinguish a space for update purposes.
 *
 *  Argument:
 *	this			- the ASD for the space being distinguished.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Notes:
 *	The concept of a 'Distinguished Space' is used for two purposes by the
 *	memory manager.
 *
 *	1) the 'Distinguished Space' is always enumerated first by the space
 *	   enumeration routines.  The principal consequence of this behavior
 *	   is that the distinguished space is given the first opportunity to
 *	   find new containers during the transient reference elimination
 *	   phase of an update.
 *	2) the 'Distinguished Space' is the only space saved by a 'local'
 *	   update.
 *
 *	The 'Distinguished Space' is always the head of the ASD ring.  Changing
 *	the distinguished space involves rotating that ring.
 *
 *****/
void M_ASD::Distinguish () {
    m_pAND->m_pASDRing = this;
}


/***********************************
 *****  Container Enumerators  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to enumerate the containers in a space.
 *
 *  Arguments:
 *	this			- the address of an ASD for the space whose
 *				  containers are to be enumerated.
 *	elementProcessor	- the address of a VContainerHandle member
 *				  function that will be called for each
 *				  container.  This member takes one argument -
 *				  function will be called with two arguments -
 *				  a <stdargs.h> argument pointer positioned
 *				  at 'elementProcessorArg1'.  It is expected
 *				  to return a non-zero integer to request the
 *				  next container in the enumeration and '0' to
 *				  terminate the enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_ASD::EnumerateContainers (
    bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
) {
    V_VARGLIST (iArgList, elementProcessor);

/*****  Access the container table bounds, ...  *****/
    unsigned int lowerCTIBound = 0;
    unsigned int upperCTIBound = cteCount ();

/*****  ... and enumerate the containers:  *****/
    for (bool okToProceed = true; okToProceed && lowerCTIBound < upperCTIBound; lowerCTIBound++) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... and process the container if inappropriate:  *****/
	switch (cte.addressType ()) {
	case M_CTEAddressType_ROContainer:
	case M_CTEAddressType_RWContainer:
	case M_CTEAddressType_CPCC: {
		VContainerHandle::Reference pCPCC (cte.AttachHandle ());

		okToProceed = (pCPCC.referent ()->*elementProcessor) (iArgList);
	    }
	    break;

	default:
	    break;
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate the actively accessed containers in a space.
 *
 *  Arguments:
 *	this			- the address of an ASD for the space whose
 *				  actively accessed containers are to be
 *				  enumerated.
 *	elementProcessor	- the address of a VContainerHandle member
 *				  function that will be called for each
 *				  container.  This member takes one argument -
 *				  function will be called with two arguments -
 *				  a <stdargs.h> argument pointer positioned
 *				  at 'elementProcessorArg1'.  It is expected
 *				  to return a non-zero integer to request the
 *				  next container in the enumeration and '0' to
 *				  terminate the enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_ASD::EnumerateActiveContainers (
    bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
) {
    V_VARGLIST (iArgList, elementProcessor);

/*****  Access the container table bounds, ...  *****/
    unsigned int lowerCTIBound = 0;
    unsigned int upperCTIBound = cteCount ();

/*****  ... and enumerate the modified containers:  *****/
    for (bool okToProceed = true; okToProceed && lowerCTIBound < upperCTIBound; lowerCTIBound++) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... 'continue' if this entry is inappropriate, ...  *****/
	switch (cte.addressType ()) {
	case M_CTEAddressType_CPCC: {
		VContainerHandle::Reference pCPCC (cte.addressAsContainerHandle ());

		okToProceed = (pCPCC.referent()->*elementProcessor) (iArgList);
	    }
	    break;

	default:
	    break;
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate a space's transient reference rescan targets.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each transient referencing
 *				  container.  This function will be called with
 *				  two arguments - the address of a CPCC for the
 *				  container and a <stdargs.h> argument pointer
 *				  positioned at 'elementProcessorArg1'; it
 *				  is expected to return a non-zero integer to
 *				  request the next container in the enumeration
 *				  and '0' to terminate the enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_ASD::EnumerateTRefRescanContainers (
    bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
) {
    V_VARGLIST (iArgList, elementProcessor);

/*****  Obtain and reset the enumeration bounds, ...  *****/
    unsigned int lowerCTIBound = m_iTRefRescanRegion.lb ();
    unsigned int upperCTIBound = m_iTRefRescanRegion.ub ();

    ResetTRBounds ();

/*****  ... and enumerate the transient referencing containers:  *****/
    for (bool okToProceed = true; okToProceed && lowerCTIBound <= upperCTIBound; lowerCTIBound++) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... skip inappropriate entries, ...  *****/
	VContainerHandle::Reference pCPCC;
	switch (cte.addressType ()) {
	case M_CTEAddressType_RWContainer:
	    if (cte.isATRefRescanTarget ())
		pCPCC.setTo (cte.AttachHandle ());
	    break;

	case M_CTEAddressType_CPCC:
	    if (cte.isATRefRescanTarget ())
		pCPCC.setTo (cte.addressAsContainerHandle ());
	    break;

	default:
	    continue;
	}

    /*****  ... and process them.  *****/
	if (pCPCC) {
	    cte.clearTRefFlags ();

	    okToProceed = (pCPCC.referent ()->*elementProcessor) (iArgList);
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate a space's modified containers.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each appropriate container.  This
 *				  routine will be called with the address of a
 *				  CPCC for the container and a <stdargs.h>
 *				  pointer positioned at 'elementProcessorArg1'.
 *				  It is expected to return true to request the
 *				  next container in the enumeration and false
 *				  to terminate the enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_ASD::EnumerateModifiedContainers (
    bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
) {
    V_VARGLIST (iArgList, elementProcessor);

/*****  Access the enumeration bounds, ...  *****/
    unsigned int lowerCTIBound = m_iReadWriteRegion.lb ();
    unsigned int upperCTIBound = m_iReadWriteRegion.ub ();

/*****  ... and enumerate the modified containers:  *****/
    for (bool okToProceed = true; okToProceed && lowerCTIBound <= upperCTIBound; lowerCTIBound++) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... skip inappropriate entries, ...  *****/
	VContainerHandle::Reference pCPCC;
	switch (cte.addressType ()) {
	case M_CTEAddressType_RWContainer:
	    pCPCC.setTo (cte.AttachHandle ());
	    break;

	case M_CTEAddressType_CPCC:
	    pCPCC.setTo (cte.addressAsContainerHandle ());
	    if (pCPCC->isReadOnly ())
		continue;
	    break;

	default:
	    continue;
	}

    /*****   ... and process the rest:  *****/
	pCPCC->CheckConsistency ();

	okToProceed = (pCPCC.referent ()->*elementProcessor) (iArgList);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to enumerate the containers being saved by an update.
 *
 *  Arguments:
 *	elementProcessor	- the address of a function which will be
 *				  applied to each appropriate container.  This
 *				  routine will be called with the address of a
 *				  CPCC for the container and a <stdargs.h>
 *				  pointer positioned at 'elementProcessorArg1'.
 *				  It is expected to return true to request the
 *				  next container in the enumeration and false
 *				  to terminate the enumeration early.
 *	elementProcessorArg1...	- the first of an arbitrary collection of
 *				  optional arguments to 'elementProcessor'.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
void __cdecl M_ASD::EnumerateUpdatingContainers (
    bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
) {
    V_VARGLIST (iArgList, elementProcessor);

/*****  Cache some fixed state ...  *****/
    bool notCompactingSpace = !(m_pPASD->DoingCompaction () || m_pPASD->CompactingAll ());

/*****  ... access the container table bounds, ...  *****/
    unsigned int lowerCTIBound;
    unsigned int upperCTIBound;
    if (notCompactingSpace) {
	lowerCTIBound = m_iReadWriteRegion.lb ();
	upperCTIBound = m_iReadWriteRegion.ub ();
    }
    else {
	lowerCTIBound = 0;
	upperCTIBound = cteCount () - 1;
    }

/*****  ... and enumerate the modified containers:  *****/
    unsigned int roContainersProcessed = 0;
    unsigned int rwContainersProcessed = 0;

    for (bool okToProceed = true; okToProceed && lowerCTIBound <= upperCTIBound; lowerCTIBound++) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... skip inappropriate entries, ...  *****/
	VContainerHandle::Reference pCPCC;
	switch (cte.addressType ()) {
	case M_CTEAddressType_ROContainer:
	    if (m_pPASD->CompactingAll ()) {
	/*****  we will process this entry ....  *****/
	    } else if (notCompactingSpace ||
		PS_CTE_Segment (PS_ASD_CTEntry (m_pPASD, lowerCTIBound)) >=
		    m_pPASD->MinSavedSegment ()
	    ) continue;
	/*****  NO BREAK HERE  *****/

	case M_CTEAddressType_RWContainer:
	    pCPCC.setTo (cte.AttachHandle ());
	    break;

	case M_CTEAddressType_CPCC:
	    pCPCC = cte.addressAsContainerHandle ();
	    if (pCPCC->hasNoContainer ())
		continue;
            if (m_pPASD->CompactingAll () || pCPCC->hasAReadWriteContainer ())
		break;
	    if (notCompactingSpace ||
		PS_CTE_Segment (PS_ASD_CTEntry (m_pPASD, lowerCTIBound)) >= m_pPASD->MinSavedSegment ()
	    ) continue;
	    break;

	default:
	    continue;
	}

    /*****   ... and process the rest:  *****/
	if (pCPCC->hasAReadWriteContainer ())
	    rwContainersProcessed++;
	else
	    roContainersProcessed++;

	pCPCC->CheckConsistency ();

	okToProceed = (pCPCC.referent ()->*elementProcessor) (iArgList);
    }

    if (m_pPASD->TracingUpdate ()) IO_printf (
	"+++ ... %u read-only and %u read-write containers in space %u update pass.\n",
	roContainersProcessed,
	rwContainersProcessed,
	m_xSpace
    );
}


/**********************************
 *****  Segment Scan Handler  *****
 **********************************/

void M_ASD::ScanSegments () {
/*****  Access the container table bounds, ...  *****/
    unsigned int lowerCTIBound = m_iLiveReferenceRegion.lb ();
    unsigned int upperCTIBound = m_iLiveReferenceRegion.ub () + 1;

    ResetLiveRefBounds ();
/*****  ... and enumerate the containers:  *****/
    while (lowerCTIBound < upperCTIBound) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... 'continue' if this entry is inappropriate, ...  *****/
	switch (cte.addressType ()) {
	case M_CTEAddressType_ROContainer:
	    if (cte.mustStayMapped ())
		m_pPASD->RetainContainer (lowerCTIBound);
	    if (cte.mustBeRemapped () || !m_pPASD->ContainerHasBeenRetained (lowerCTIBound)) {
		cte.clearContainerAddress ();
		cte.clearMustBeRemapped ();
	    }
	    else if (IsntNil (cte.addressAsContainerAddress ()))
		AdjustLiveRefBounds (lowerCTIBound);
	    break;

	case M_CTEAddressType_CPCC:
	    if (cte.addressAsContainerHandle ()->hasAReadOnlyContainer ()) {
		m_pPASD->RetainContainer (lowerCTIBound);
		AdjustLiveRefBounds (lowerCTIBound);
	    }
	    break;

	default:
	    break;
	}
	lowerCTIBound++;
    }
}


/******************************************
 *****  Resource Reclamation Handlers *****
 ******************************************/

void VContainerHandle::flushCachedResources_() {
/*****
  Cached handles have a reference count of zero; however, this test used to
  check for a referenceCount () == 1.  Wonder what kind of dangling references
  that generated...
 *****/
    if (referenceCount () == 0 && m_pContainer && !m_bPrecious)
	DetachFromCTE ();
}

bool VContainerHandle::FlushCachedResources (VArgList const&) {
    flushCachedResources_();
    return true;
}

bool M_ASD::FlushCachedResources (VArgList const&) {
    EnumerateActiveContainers (&VContainerHandle::FlushCachedResources);
    return true;
}

void M_AND::FlushCachedResources () {
//  Don't do anything that could change the set of accessed containers if a GC is running, ...
    if (GarbageCollectionRunning ())
	return;

    bool InitialHandlePreservationFlag = VContainerHandle::g_bPreservingHandles;
    UNWIND_Try {
	VContainerHandle::g_bPreservingHandles = false;
	EnumerateAccessedSpaces (&M_ASD::FlushCachedResources);
    } UNWIND_Finally {
	VContainerHandle::g_bPreservingHandles = InitialHandlePreservationFlag;
    } UNWIND_EndTryFinally;
}

PublicFnDef void M_FlushCachedResources () {
    ENVIR_Session ()->FlushCachedResources ();
}

PublicFnDef unsigned int M_ReclaimSegments () {
    return ENVIR_Session ()->ReclaimSegments ();
}

PublicFnDef unsigned int M_ReclaimAllSegments () {
    return ENVIR_Session ()->ReclaimAllSegments ();
}

/*********************************
 *****  GRM Request Handler  *****
 *********************************/

PublicFnDef void M_ProcessGRMRequests () {
    if (PS_GRMRequestsArePending ())
	M_ReclaimSegments ();
}


/**************************
 *****  Space Update  *****
 **************************/

/**********************
 *  Transient Savers  *
 **********************/

/*---------------------------------------------------------------------------
 *****  Quasi-public routine to save the container associated with a POP if it
 *****  is transient.
 *
 *  Arguments:
 *	pPOP			- the address of the reference to make persistent.
 *
 *  Returns:
 *	true if the reference will persist, false otherwise.
 *
 *****/
bool M_ASD::Persist (M_POP *pOldPOP) {
    M_CTE iOldCTE (m_pAND, pOldPOP);
    M_CTE iNewCTE (iOldCTE);

    M_POP const *pNewPOP = iNewCTE.FollowForwardings ();

/*****  Determine if the reference can be made persistent in this update...  *****/
    if (!iNewCTE.persistentlyReferenceableFrom (this)) {
	m_pAND->SetUpdateStatusTo (PS_UpdateStatus_NotSeparable);
	return false;
    }

/*****  If the POP references the scratch pad, ...  *****/
    if (iNewCTE.namesAScratchPadObject ()) {
	/*****  ... check it's validity (why?), ...  *****/
	if (iNewCTE.isntReferenced ()) ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"Premature Zero Reference Count for Container [%u:%u]",
		M_POP_D_ObjectSpace	(iNewCTE.addressAsPOP ()),
		M_POP_D_ContainerIndex	(iNewCTE.addressAsPOP ())
	    )
	);

	/*****  ... forward the CTE, ...  *****/
	iNewCTE.ForwardToSpace (this);
	pNewPOP = iNewCTE.FollowForwardings ();
    }

    if (pNewPOP) {
	*pOldPOP = *pNewPOP;
	iNewCTE.retain ();
	iOldCTE.release ();
    }

    return true;
}


/*---------------------------------------------------------------------------
 *****  Quasi-public routine to save the containers associated with the
 *****  transient POPs in an array of POPs.
 *
 *  Arguments:
 *	pReferences		- the address of an array of POPs.
 *	cReferences		- the number of POPs in the POP array.
 *
 *  Returns:
 *	true if all references will persist, false otherwise;
 *
 *****/
bool M_ASD::Persist (M_POP *pReferences, unsigned int cReferences) {
    bool result = true;
    while (result && cReferences-- > 0)
	result = Persist (pReferences++);
    return result;
}


/***************************
 *  Transient Enumerators  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to save the transients referenced from a container.
 *
 *  Arguments:
 *
 *  Returns:
 *	true
 *
 *  Notes:
 *	This routine is intended for use as the action routine of one of the
 *	'M_Enumerate*ContainersInSpace' routines.
 *
 *****/
bool VContainerHandle::PersistReferences (VArgList const&) {
    return PersistReferences ();
}


/************************************
 *  Transient Reference Eliminator  *
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to eliminate a space's transient references.
 *
 *  Argument:
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void M_ASD::EliminateTransientReferences () {
//  Persist references from this space's modified containers, ...
    EnumerateModifiedContainers (&VContainerHandle::PersistReferences);

//  ... and do the same thing for all newcomers:
    while (UpdateIsWell () && hasTransientReferences ())
	EnumerateTRefRescanContainers (&VContainerHandle::PersistReferences);
}


/***********************************
 *  Incremental Compactor Support  *
 ***********************************/

void M_ASD::VisitUnaccessedContainers () {
    unsigned int unaccessedContainerCount = 0;
    for (unsigned int ctindex = 1; ctindex < cteCount (); ctindex++) {
	M_CTE cte (this, ctindex);
	if (cte.holdsAReadOnlyContainerAddress () && !cte.hasBeenAccessed ()) {
	    unaccessedContainerCount++;
	    VContainerHandle::Reference pCPCC (cte.AttachHandle ());
	}
    }
    if (m_pPASD->TracingCompaction ()) IO_printf (
	"     Unaccessed Container Count For Space %u: %u\n",
	m_xSpace, unaccessedContainerCount
    );
}


/*****************************
 *****  Space Remapping  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to remap the read-write containers in a space after an update.
 *
 *  Argument:
 *	this			- the address of an ASD for the space to be
 *				  remapped.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void M_ASD::RemapSpace () {
    unsigned int	lowerCTIBound,
			upperCTIBound;
    VContainerHandle::Reference cpcc;
    M_CPreamble*	oldContainerAddress;
    bool		oldContainerIsReadWrite;

/*****  Display the opening trace message, ...  *****/
    if (m_pPASD->TracingUpdate ()) IO_printf(
	"+++ Starting space %u remapping...\n", m_xSpace
    );

/*****  ... flush the container pointer for the writeable PCT, ...  *****/
    m_pPCTCPCC.clear ();

/*****  ... access and reset the container table bounds, ...  *****/
    if (!m_pPASD->DoingCompaction ()) {
	lowerCTIBound = m_iReadWriteRegion.lb ();
	upperCTIBound = m_iReadWriteRegion.ub ();
    }
    else {
	lowerCTIBound = 0;
	upperCTIBound = cteCount () - 1;
    }
    ResetRWBounds ();

/*****  ... enumerate the modified containers, ...  *****/
    unsigned int roContainersProcessed = 0;
    unsigned int rwContainersProcessed = 0;
    while (lowerCTIBound <= upperCTIBound) {
	M_CTE cte (this, lowerCTIBound);
	lowerCTIBound++;

	if (cte.mustBeRemapped ()) {
	    cte.clearHasBeenAccessed ();
	    switch (cte.addressType ()) {
	    case M_CTEAddressType_ROContainer:
		cte.clearContainerAddress ();
		roContainersProcessed++;
		break;

	    case M_CTEAddressType_RWContainer:
		DeallocateContainer (cte.addressAsContainerAddress ());
		cte.clearContainerAddress ();
		rwContainersProcessed++;
		break;

	    case M_CTEAddressType_CPCC:
		cpcc = cte.addressAsContainerHandle ();
		
		oldContainerAddress	= cpcc->containerAddress ();
		oldContainerIsReadWrite = cpcc->hasAReadWriteContainer ();

		cpcc->AdjustContainerPointers (
		    m_pPASD->AccessContainer (cte.containerIndex (), false),
		    false
		);
		cte.setHasBeenAccessed ();

		if (oldContainerIsReadWrite) {
		    rwContainersProcessed++;
		    if (oldContainerAddress) {
			DeallocateContainer (oldContainerAddress);
		    }
		}
		else roContainersProcessed++;

		cpcc.clear ();
		break;

	    default:
		break;
	    }

	    cte.clearIsNew ();
	    cte.clearMustBeRemapped ();
	}
    }

/*****  Display the trace messages, ...  *****/
    if (m_pPASD->TracingUpdate ()) {
	IO_printf (
	    "+++ ... %u read-only and %u read-write containers in space %u remap pass.\n",
	    roContainersProcessed,
	    rwContainersProcessed,
	    m_xSpace
	);

	IO_printf (
	    "+++ Done with space %u remapping...\n", m_xSpace
	);
    }
}


/************************************
 *****  Space Creation Routine  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new object space.
 *
 *  Arguments:
 *	this			- a CPCC for a SCRATCH PAD container which is
 *				  to become the root of the new space.
 *
 *  Returns:
 *	An 'PS_UpdateStatus' code describing the state of the operation.
 *
 *****/
PS_UpdateStatus VContainerHandle::CreateSpace () {
/*****
 *  Abort if an attempt is being made to create a space rooted in an already
 *  persistent object, ...
 *****/
    if (isntInTheScratchPad ()) ERR_SignalFault (
	EC__InternalInconsistency, "Attempt To Forward A Persistent Physical Object"
    );

/*****  Otherwise, create a new space, ...  *****/
    M_AND* pAttachedNetwork = Database ();
    PS_AND* pPhysicalAND = pAttachedNetwork->PhysicalAND ();
    PS_ASD* pPhysicalASD = pPhysicalAND->CreateSpace (pAttachedNetwork);

    if (pPhysicalAND->UpdateWasntSuccessful ())
        return pPhysicalAND->UpdateStatus ();

/*****  ... initialize its root, ...  *****/
    ForwardToSpace (pAttachedNetwork->CreateASD (pPhysicalASD->spaceIndex (), true));

/*****  ... and return.  *****/
    return PS_UpdateStatus_Successful;
}


/******************************************
 ******************************************
 *****  The Container Access Manager  *****
 ******************************************
 ******************************************/

/**************************************
 *****  Container Address Access  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the memory address of a container.
 *
 *  Argument:
 *	pReference		- a reference to a POP for the container whose
 *				  preamble address is desired.
 *
 *  Returns:
 *	The preamble address of the specified container.
 *
 *  Notes:
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 *	THIS ROUTINE IS EXPERIMENTAL.  ITS INTERFACE IS SUBJECT TO CHANGE.  IT
 *	SHOULD BE USED IN A HIGHLY CONTROLLED MANNER TO OPTIMIZE SUPER-CRITICAL
 *	CODE.  IT SHOULD NOT BE USED AS AN UNDISCIPLINED BYPASS OF THE MEMORY
 *	MANAGER'S NORMAL ACCESS CONTROL MECHANISMS.
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 *
 *	The preamble address returned by this routine is not guaranteed to be
 *	valid after the next memory manager call.
 *
 *****/
M_CPreamble* M_AND::GetContainerAddress (M_POP const *pReference) {
    M_CTE cte (this, pReference);
    cte.FollowForwardings ();
    cte.InitializeCTEContainerAddress ();

    return cte.containerAddress ();
}

VContainerHandle* M_AND::GetContainerHandle (
    M_POP const *pReference, RTYPE_Type xExpectedType
) {
//  Retrieve the CTE, ...
    M_CTE cte (this, pReference); //  FollowForwardings () called by AttachHandle

//  ... and return the container handle:
    return cte.AttachHandle (xExpectedType);
}


/*---------------------------------------------------------------------------
 *****  'Internal' routine to allocate a standard CPD given a space and
 *****  container index.
 *
 *  Arguments:
 *	xContainer		- the index of the container described above.
 *	xExpectedType		- the expected type of the container associated
 *				  with the POP defined above.  An error occurs
 *				  if the types do not match and xExpectedType
 *				  is not equal to RTYPE_C_Any.
 *
 *  Returns:
 *	The address of a standard CPD.
 *
 *  Notes:
 *	This routine is intended for use ONLY by the representation type
 *	handlers for the various container tables.  It does not check its
 *	arguments for validity.  All normal standard CPD requests should
 *	be handled by 'M_Get...CPD'.
 *
 *****/
VContainerHandle* M_ASD::GetContainerHandle (
    unsigned int xContainer, RTYPE_Type xExpectedType
) {
//  Retrieve the CTE, ...
    M_CTE cte (this, xContainer);

//  ... and return the container handle:
    return cte.AttachHandle (xExpectedType);
}

M_CPD* M_ASD::GetCPD (unsigned int xContainer, RTYPE_Type xExpectedType) {
    return GetContainerHandle (xContainer, xExpectedType)->GetCPD ();
}

VContainerHandle *M_AND::GetContainerHandle (
    unsigned int xSpace, unsigned int xContainer, RTYPE_Type xExpectedType
) {
    return AccessASD (xSpace)->GetContainerHandle (xContainer, xExpectedType);
}

M_CPD *M_AND::GetCPD (M_POP const *pReference, RTYPE_Type xExpectedType) {
    return GetContainerHandle (pReference, xExpectedType)->GetCPD ();
}

M_CPD *M_AND::GetCPD (
    unsigned int xSpace, unsigned int xContainer, RTYPE_Type xExpectedType
) {
    return GetContainerHandle (xSpace, xContainer, xExpectedType)->GetCPD ();
}


/*********************************************************************
 *****  Container Pointer Descriptor (CPD) Allocation Utilities  *****
 *********************************************************************/

/************************
 *****  Free Lists  *****
 ************************/

static V::VAllocator<
    12, sizeof(M_CPD::Pointer), sizeof(M_CPD) - sizeof(M_CPD::Pointer)
> CPDAllocator;


/************************
 *****  Allocation  *****
 ************************/

M_CPD* VContainerHandle::NewCPD () {
/*****  Allocate the CPD, ...  *****/
    unsigned int ptrCount = m_pRTD->cpdPointerCount ();
    M_CPD* pNewCPD = (M_CPD*)CPDAllocator.allocate (
	sizeof(M_CPD) + sizeof(pointer_t) * (ptrCount - 1)
    );

    M_RTD_CPDCreateCount (m_pRTD)++;

/*****  ... and initialize it.   *****/
    M_CPD_GCLock	(pNewCPD) = false;
    M_CPD_ReferenceCount(pNewCPD) = 1;

    if (ptrCount < 1)
	M_CPD_PointerToAny (pNewCPD, 0) = NilOf (void*);

    M_CPD_NLink		(pNewCPD) = m_pCPDChainHead;
    M_CPD_PLink		(pNewCPD) = NilOf (M_CPD*);
    M_CPD_CPCC		(pNewCPD) = this;
    M_CPD_PreamblePtr	(pNewCPD) = m_pContainer;

    if (m_pCPDChainHead)
	m_pCPDChainHead->m_pPrev = pNewCPD;
    else
	retain ();

    m_pCPDChainHead.setTo (pNewCPD);

    return pNewCPD;
}

M_CPD* VContainerHandle::GetCPD () {
//  Return a reuseable CPD if one exists, ...
    if (m_pRTD->cpdReuseable () && m_pCPDChainHead.isntNil ()) {
	m_pCPDChainHead->retain ();
	return m_pCPDChainHead;
    }

//  ... otherwise, allocate a new one:
    M_CPD *pNewCPD = NewCPD ();

    M_Type_CPDInitFn initFn = M_RTD_CPDInitFn (m_pRTD);
    if (initFn)
	initFn (pNewCPD);
    else {
	pointer_t* pointers = (pointer_t*)M_CPD_Pointers (pNewCPD);
	pointer_t* pl = pointers + cpdPointerCount ();
	pointer_t container = containerContent ();
	while (pointers < pl)
	    *pointers++ = container;
    }

    return pNewCPD;
}


/**************************
 *****  Deallocation  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to detach a CPD from its container.
 *
 *  Argument:
 *	cpd			- the address of the CPD to be detached.
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	The address of the CPD which has been freed should NOT be used after
 *	it has been passed to this routine.  The result is predictable - a
 *	segmentation fault.
 *
 *****/
void M_CPD::reclaim () {
/*****  Abort if an attempt is being made to free a GC-Locked CPD...  *****/
    if (m_fGCLocked) ERR_SignalFault (
	EC__MError, "M_FreeCPD: CPD locked"
    );

/*****  Cache the RTD, ...  *****/
    M_RTD* pRTD = RTD ();
    
/*****  Decrement the RTD CPD Count  ... *****/
    M_RTD_CPDDeleteCount (pRTD)++;

/*****  Unlink the CPD being freed  ...  *****/
    if (m_pNext)
	m_pNext->m_pPrev = m_pPrev;

    if (m_pPrev)
	m_pPrev->m_pNext = m_pNext;
    else if (IsNil (m_pContainerHandle->m_pCPDChainHead = m_pNext))
	m_pContainerHandle->release ();

/*****  Finally, free the detached CPD...  *****/
    CPDAllocator.deallocate (
	this, sizeof (M_CPD) + sizeof (pointer_t) * (pRTD->cpdPointerCount () - 1)
    );
}


/********************
 *  CPD Duplicator  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to duplicate a CPD.
 *
 *  Arguments:
 *	existingCPD	    - the address of an existing CPD which will be
 *			      duplicated.
 *  Returns:
 *	The address of the new CPD.
 *
 *****/
M_CPD* M_CPD::GetCPD () {
/*****  Allocate and thread a new CPD...  *****/
    M_CPD* pNewCPD = m_pContainerHandle->GetCPD ();

/*****  Initialize its pointer array...  *****/
    if (pNewCPD != this) memcpy (
	M_CPD_Pointers (pNewCPD), m_iPointerArray, sizeof (m_iPointerArray[0]) * PointerCount ()
    );

/*****  ... and return.  *****/
    return pNewCPD;
}


/*****************************************************
 *****  CPD Based Container Allocation Routines  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new Scratch Pad container.
 *
 *  Arguments:
 *	rType			- the index of the representation type of the
 *				  container being created.
 *	size			- the number of bytes to be allocated to the
 *				  new container.
 *
 *  Returns:
 *	The address of a CPD for the container created.
 *
 *****/
M_CPreamble *M_ASD::AllocateContainer (RTYPE_Type type, size_t size, unsigned int xContainer) {
    size = RoundContainerSize (size);

    M_CPreamble* result = (M_CPreamble*) UTIL_Malloc (
	sizeof (M_CPreamble) + sizeof(M_CEndMarker) + size
    );
    IncrementAllocation (size);

    M_CPreamble_NSize			(result) = size;
    M_CPreamble_RType			(result) = (unsigned int)(type);
    M_CPreamble_OSize			(result) = 0;
    M_CPreamble_POPObjectSpace		(result) = m_xSpace;
    M_CPreamble_POPContainerIndex	(result) = xContainer;

    M_CEndMarker_Size (M_CPreamble_EndMarker (result)) = size;

    AdjustRWBounds (xContainer);
    AllocateContainerCount++;

    return result;
}

VContainerHandle *M_ASD::CreateContainer (RTYPE_Type type, size_t size, VContainerHandle *pHandle) {
//  Allocate a CTE, ...
    M_CTE cte (0, this);

//  ... allocate the container, ...
    cte.setToContainerAddress (AllocateContainer (type, size, cte.containerIndex ()), true);

//  ... adjust the relevant bounds, ...
    cte.setTRefFlags ();

//  ... attach a CPCC, and allocate and return a CPD:
    return pHandle ? pHandle->attachTo (cte) : cte.MakeHandle ();
}

void M_ASD::CreateIdentity (M_POP &rResult, VContainerHandle *pHandle) {
    M_CTE cte (0, this);
    cte.setToContainerHandle (pHandle);
    pHandle->m_pDCTE = cte.dcte ();

    AdjustRWBounds(cte.containerIndex());
    cte.setTRefFlags ();
    cte.getPOP (rResult);
}


/**************************************************
 *****  CPD Based Container Swapping Routine  *****
 **************************************************/

/*---------------------------------------------------------------------------
 *****  External routine to swap container table slots of two containers given
 *****  their CPD's.
 *
 *  Argument:
 *	cpd1, cpd2		- the CPD's attached to the containers to be
 *				  swapped.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PrivateFnDef void M_SwapContainers (M_CPD* cpd1, M_CPD* cpd2) {
    VContainerHandle* cpcc1 = M_CPD_CPCC (cpd1);
    VContainerHandle* cpcc2 = M_CPD_CPCC (cpd2);

/*****  Obtain the container table entries for the two containers  *****/
    M_CTE cte1 (cpcc1->Space (), cpcc1->containerIndex ());
    M_CTE cte2 (cpcc2->Space (), cpcc2->containerIndex ());

/*****  Reassociate the cpcc's  *****/
    cpcc1->EnableModifications ();
    cpcc2->EnableModifications ();

    cpcc1->AttachToCTE (cte2);
    cpcc2->AttachToCTE (cte1);
}


/**********************************************
 *****  CPD Based Container Tail Shifter  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to adjust CPD pointers after a container tail shift.
 *
 *  Arguments:
 *	this			- the address of a CPD for the container whose
 *				  tail was moved.
 *	pOldTail		- the old tail origin address.
 *	pNewTail		- the new tail origin address.
 *	sa			- the shift amount (== pNewTail - pOldTail).
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	This routine takes an address of a CPD and not some other structure
 *	(like a CPCC) because that is the most useful thing to do given that
 *	container tail movement is precipitated by an action associated with
 *	a CPD.
 *
 *****/
void VContainerHandle::AdjustContainerTailPointers (
    pointer_t pOldTail, pointer_t pNewTail, pointer_diff_t sa
) {
    unsigned int const iPointerCount = cpdPointerCount ();

    for (M_CPD* pCPD = m_pCPDChainHead; pCPD; pCPD = M_CPD_NLink (pCPD)) {
	pointer_t *p, *pl;
	for (p = (pointer_t *)M_CPD_Pointers (pCPD), pl = p + iPointerCount; p < pl; p++) {
	    if (*p >= pOldTail)
		*p += sa;   /* adjust pointers in the tail */
	    else if (*p >= pNewTail)
		*p = pNewTail;   /* reset pointers in the deleted region */
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to move the tail of a container.
 *
 *  Arguments:
 *	this				- the address of a CPD for the
 *					  container whose tail is to be
 *					  shifted.
 *	xPointer/pTail			- the index of a pointer in the CPD /
 *					  pointer to the beginning of the
 *					  tail to be shifted.
 *	sTail				- the actual size of the tail.  This
 *					  size should reflect the number of
 *					  meaningful data bytes in the tail
 *					  and not simply the amount of space
 *					  allocated to the tail.  This routine
 *					  is smart enough to use any extra
 *					  space following the tail before
 *					  attempting to enlarge the container.
 *	sShift				- the number of bytes the tail is to be
 *					  shifted.  Positive values of this
 *					  argument produce right shifts of the
 *					  tail (i.e., insertions); negative
 *					  values left shifts (i.e., deletions).
 *					  Inserted regions will be zeroed.
 *	bSqueeze			- a boolean value which when non-zero
 *					  instructs this routine to reclaim
 *					  any extra space allocated after the
 *					  end of the tail.
 *	pShiftProcessor			- an optional (specified as Nil if not
 *					  required) of four arguments which
 *					  will be called to deal with the
 *					  region either inserted into or
 *					  deleted from the container.  The
 *					  arguments passed to this routine
 *					  are 'this', 'xPointer',
 *					  'sShift', and a <stdargs.h>
 *					  argument pointer pointing to
 *					  'regionProcessingFnArg1'.  The
 *					  specified CPD pointer will point
 *					  to the beginning of the container
 *					  region affected by this operation.
 *					  In the case of deletions (left
 *					  shifts), that pointer will point to
 *					  beginning of the region about to be
 *					  deleted; in the case of insertions
 *					  (right shifts), that pointer will
 *					  point to the beginning of the region
 *					  just inserted.
 *	pShiftProcessorArguments, ...	- a collection of optional arguments
 *					  passed by <stdargs.h> pointer to
 *					  the region processing function.
 *
 *  Returns:
 *	'this'
 *
 *  Behavior:
 *	In addition to inserting and deleting space from the middle of a
 *	container, this routine will adjust the pointers of the CPDs
 *	attached to the container.  Basically, the adjustment performed
 *	by this routine will attempt to leave existing CPD pointers pointing
 *	to the same data byte.  Pointers which point into the relocated tail
 *	will be adjusted by 'sShift'; pointers which point to a deleted
 *	byte will be adjusted to point to the new origin of the tail; pointers
 *	which point to an unmoved/deleted byte will be left unchanged.  All of
 *	this of course is in addition to the normal pointer adjustments which
 *	are driven by container movement.
 *
 *****/
void VContainerHandle::VShiftContainerTail (
    pointer_t			pTail,
    size_t			sTail,
    ptrdiff_t			sShift,
    bool			bSqueeze,
    ShiftProcessor		pShiftProcessor,
    va_list			pShiftProcessorArguments
) {
    ShiftContainerTailCount++;

/*****
 * WARNING:
 *	'pBase', 'pNewTail', and 'pOldTail' need to be recomputed if the container is
 *	grown.
 ******/
    pointer_t pBase	= containerContent ();
    pointer_t pOldTail	= pTail;
    pointer_t pNewTail	= pOldTail + sShift;
    ptrdiff_t sGrowth	= pNewTail - pBase + sTail - containerSize ();

    if (sShift < 0) {	/*  Delete/Shift-Left  */
        if (pNewTail < pBase) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "Attempt to shift a container's tail past its origin"
	);
	AdjustContainerTailPointers (pOldTail, pNewTail, sShift);

	if (pShiftProcessor)
	    pShiftProcessor (this, pNewTail, sShift, pShiftProcessorArguments);

	memcpy (pNewTail, pOldTail, sTail);
    }
    else if (sShift > 0) {	/*  Insertion/Shift-Right  */
        if (sGrowth > 0) {
	    GrowContainer (sGrowth);

	    /*  Recompute 'pOldTail' and 'pNewTail'  */
	    pOldTail += (pointer_size_t)containerContent () - (pointer_size_t)pBase;
	    pNewTail = pOldTail + sShift;
	}

	memcpyb (pNewTail, pOldTail, sTail);		/***  Shift right    ...  ***/
	memset	((void *)pOldTail, 0, (size_t)sShift);	/***  ... and zero fill.  ***/

	if (pShiftProcessor)
	    pShiftProcessor (this, pOldTail, sShift, pShiftProcessorArguments);

	AdjustContainerTailPointers (pOldTail, pNewTail, sShift);
    }
    if (bSqueeze && sGrowth < 0)
	GrowContainer (sGrowth);
}

void __cdecl VContainerHandle::ShiftContainerTail (
    pointer_t pTail, size_t sTail, ptrdiff_t sShift, bool bSqueeze, ShiftProcessor pShiftProcessor, ...
) {
    V_VARGLIST (pShiftProcessorArguments, pShiftProcessor);
    VShiftContainerTail (
	pTail, sTail, sShift, bSqueeze, pShiftProcessor, pShiftProcessorArguments
    );
}

void __cdecl M_CPD::ShiftContainerTail (
    unsigned int xPointer, size_t sTail, ptrdiff_t sShift, bool bSqueeze, ShiftProcessor pShiftProcessor, ...
) {
    V_VARGLIST (pShiftProcessorArguments, pShiftProcessor);
    VShiftContainerTail (
	xPointer, sTail, sShift, bSqueeze, pShiftProcessor, pShiftProcessorArguments
    );
}


/*****************************************
 *****************************************
 *****  The Reference (POP) Manager  *****
 *****************************************
 *****************************************/

/****************************************
 *****  Reference Creation (Direct) *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to store a physical object pointer.
 *
 *  Arguments:
 *	xReference		- the index of the pointer in the destination
 *				  CPD which points to the physical object
 *				  pointer to be replaced.
 *	pReference		- the address of the reference to be set.
 *	pThat			- the address of a container handle whose POP is
 *				  to be stored at 'this/xReference'.
 *
 *  Returns:
 *	'this'
 *
 *  Notes:
 *	'xReference' must be >= 0.  It MUST refer to an explicit pointer of
 *	the destination CPD and not to the POP associated with the CPD itself.
 *
 *****/
M_CPD *M_CPD::StoreReference (unsigned int xReference, VContainerHandle *pThat) {
    ASSERT_LIVENESS (this, POPCopyTarget);

    EnableModifications ();
    m_pContainerHandle->StoreReference (POP (xReference), pThat);

    return this;
}


/*******************************************
 *****  Reference Creation (Indirect)  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to copy a physical object pointer.
 *
 *  Arguments:
 *	xReference		- the index of the pointer in the destination
 *				  CPD which points to the physical object
 *				  pointer to be replaced.
 *	pThat			- the address of a CPD which points to the
 *				  source of the object pointer.
 *	xThatReference		- the index of the pointer in the source CPD
 *				  which points to the physical object pointer
 *				  to be copied.  If 'xThatReference < 0', the
 *				  physical object pointer of the container to
 *				  which 'pThat' is attached will be copied.
 *
 *  Returns:
 *	'this'
 *
 *  Notes:
 *	'xReference' must be >= 0.  It MUST refer to an explicit pointer of
 *	the destination CPD and not to the POP associated with the CPD itself.
 *
 *****/
M_CPD *M_CPD::StoreReference (
    unsigned int xReference, VContainerHandle const *pThat, M_POP const *pThatReference
) {
    ASSERT_LIVENESS (this, POPCopyTarget);

    EnableModifications ();
    m_pContainerHandle->StoreReference (POP (xReference), pThat, pThatReference);

    return this;
}

M_CPD *M_CPD::StoreReference (
    unsigned int xReference, M_CPD const *pThat, int xThatReference
) {
    ASSERT_LIVENESS (this , POPCopyTarget);
    ASSERT_LIVENESS (pThat, POPCopySource);

    EnableModifications ();
    if (xThatReference < 0) m_pContainerHandle->StoreReference (
	POP (xReference), pThat->containerHandle ()
    );
    else m_pContainerHandle->StoreReference (
	POP (xReference), pThat->containerHandle (), pThat->POP (xThatReference)
    );

    return this;
}

void M_AND::StoreReference (
    M_POP *pReference, M_ASD *pThatSpace, M_POP const *pThatReference
) {
    M_POP iOldReference = *pReference;

    M_TOP iReplacement;
    LocateOrAddNameOf (pThatSpace, pThatReference, iReplacement);
    iReplacement.getReferencedPOP (*pReference);
#if defined(INCLUDING_XDB_REFERENCE_TRACING)
    if (TracingCrossDBReferences && iReplacement.space ()->Database () != this) {
	display (
	    "\n+++ X-Reference: %s[%u]->%s[%u:%u]",
	    NDFPathName (),
	    iReplacement.spaceIndex (),
	    iReplacement.space()->Database ()->NDFPathName (),
	    iReplacement.space()->Index (),
	    iReplacement.containerIndex ()
	);
    }
#endif

    Release (&iOldReference);
}

void M_AND::StoreReference (M_POP *pReference, Vdd::Store *pThat) {
    VContainerHandle::Reference pThatHandle;
    pThat->getContainerHandle (pThatHandle);
    StoreReference (pReference, pThatHandle);
}


/*******************************
 *****  Reference Release  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Procedure to delete a POP reference.
 *
 *  Arguments:
 *	pReference		- the address of the POP whose reference is
 *				  to be deleted.
 *	pReferences		- the address of a POP array whose references
 *				  are to be deleted.
 *	cReferences		- the number of POPs in the array.
 *
 *****/
void M_AND::Release (M_POP const *pReference) {
    AccessASD (pReference)->releaseCTE (M_POP_ContainerIndex (pReference));
}

void M_AND::Release (M_POP const *pReferences, unsigned int cReferences) {
    while (cReferences-- > 0)
	Release (pReferences++);
}


/*---------------------------------------------------------------------------
 *****  Routine to clear a physical object pointer.
 *
 *  Arguments:
 *	xReference		- the index of the pointer in the destination
 *				  CPD which points to the physical object
 *				  pointer to be cleared.
 *
 *  Returns:
 *	Nothing
 *
 *  Notes:
 *	This routine copies the Nil POP into the object pointers
 *	it clears.  'xReference' must refer to an explicit pointer of the
 *	existing CPD - it CANNOT refer to the POP associated with the CPD
 *	itself.  The pointer selected by 'xReference' is left at the end
 *	of the range by this routine.
 *
 *****/
void M_CPD::ClearReference (unsigned int xReference) {
    ASSERT_LIVENESS (this, POPClearTarget);
    EnableModifications ();
    m_pContainerHandle->ClearReference (POP (xReference));
}


/*---------------------------------------------------------------------------
 *****  Routine to clear a range of physical object pointers.
 *
 *  Arguments:
 *	xReference		- the index of the pointer in the destination
 *				  CPD which points to the beginning of the
 *				  range of physical object pointers to be
 *				  cleared.
 *	cReferences		- the number of elements in the POP array.
 *
 *  Returns:
 *	Nothing
 *
 *  Notes:
 *	This routine effectively copies the Nil POP into the object pointers
 *	it clears.  'xReference' must refer to an explicit pointer of the
 *	existing CPD - it CANNOT refer to the POP associated with the CPD
 *	itself.  The pointer selected by 'xReference' is left at the end
 *	of the range by this routine.
 *
 *****/
void M_CPD::ClearReferences (unsigned int xReference, unsigned int cReferences) {
    ASSERT_LIVENESS (this, POPClearTarget);
    EnableModifications ();
    m_pContainerHandle->ClearReferences (POP (xReference), cReferences);
}

void M_AND::ClearReferences (M_POP *pReferences, unsigned int cReferences) {
    while (cReferences-- > 0)
	ClearReference (pReferences++);
}


/**************************************
 **************************************
 *****  Global Garbage Collector  *****
 **************************************
 **************************************/

/*************************
 *****   GCMetrics   *****
 *************************/

/*******************
 *****  State  *****
 *******************/
PrivateVarDef unsigned int MarkContainersInQueue_callCount = 0;
PrivateVarDef unsigned int MarkContainersInQueue_doCount = 0;
PrivateVarDef unsigned int GCQueueEntryCnt      = 0;
PrivateVarDef unsigned int GCMaxQueueEntryCnt   = 0;
PrivateVarDef unsigned int GCInvocationCount	= 0;
PrivateVarDef unsigned int GCRevisitCount	= 0;
PrivateVarDef unsigned int GCFirstVisitCount	= 0;


/**************************
 *****  Construction  *****
 **************************/

M_AND::GCMetrics::SpaceMetrics::SpaceMetrics (
    M_AND::GCMetrics *pDBMetrics, unsigned int xSpace
) : m_pDBMetrics (pDBMetrics), m_xSpace (xSpace), m_cPasses (0), m_pReferenceCounts (
    (ReferenceCounts*)allocate (pDBMetrics->spaceCount () * sizeof (ReferenceCounts))
) {
    unsigned int cSpaces = pDBMetrics->spaceCount ();
    for (unsigned int xReferencedSpace = 0; xReferencedSpace < cSpaces; xReferencedSpace++)
	m_pReferenceCounts[xReferencedSpace].initialize ();
}

M_AND::GCMetrics::GCMetrics (
    unsigned int cSpaces
) : m_cSpaceMetrics (cSpaces), m_pSpaceMetrics (
    static_cast <SpaceMetrics**>(allocate (cSpaces * sizeof (SpaceMetrics*)))
) {
    for (unsigned int xSpace = 0; xSpace < cSpaces; xSpace++)
	m_pSpaceMetrics[xSpace] = new SpaceMetrics (this, xSpace);
}

/*************************
 *****  Destruction  *****
 *************************/

M_AND::GCMetrics::SpaceMetrics::~SpaceMetrics () {
    deallocate (m_pReferenceCounts);
}

M_AND::GCMetrics::~GCMetrics () {
    for (unsigned int xSpace = 0; xSpace < m_cSpaceMetrics; xSpace++)
	delete m_pSpaceMetrics[xSpace];
}

/********************
 *****  Access  *****
 ********************/

void M_AND::GCMetrics::SpaceMetrics::accumulateCounts (ReferenceCounts &rCounts) const {
    if (m_cPasses > 0) {
	unsigned int cSpaces = m_pDBMetrics->spaceCount ();
	for (unsigned int xSpace = 0; xSpace < cSpaces; xSpace++)
	    rCounts += m_pReferenceCounts[xSpace];
    }
}

void M_AND::GCMetrics::accumulateCounts (ReferenceCounts &rCounts) const {
    for (unsigned int xSpace = 0; xSpace < m_cSpaceMetrics; xSpace++)
	m_pSpaceMetrics[xSpace]->accumulateCounts (rCounts);
}

unsigned int M_AND::GCMetrics::SpaceMetrics::firstVisitCount () const {
    unsigned int cVisits = 0;
    if (m_cPasses > 0) {
	unsigned int cSpaces = m_pDBMetrics->spaceCount ();
	for (unsigned int xSpace = 0; xSpace < cSpaces; xSpace++)
	    cVisits += m_pReferenceCounts[xSpace].firstVisitCount ();
    }
    return cVisits;
}

unsigned int M_AND::GCMetrics::firstVisitCount () const {
    unsigned int cVisits = 0;
    for (unsigned int xSpace = 0; xSpace < m_cSpaceMetrics; xSpace++)
	cVisits += m_pSpaceMetrics[xSpace]->firstVisitCount ();
    return cVisits;
}

unsigned int M_AND::GCMetrics::SpaceMetrics::returnVisitCount () const {
    unsigned int cVisits = 0;
    if (m_cPasses > 0) {
	unsigned int cSpaces = m_pDBMetrics->spaceCount ();
	for (unsigned int xSpace = 0; xSpace < cSpaces; xSpace++)
	    cVisits += m_pReferenceCounts[xSpace].returnVisitCount ();
    }
    return cVisits;
}

unsigned int M_AND::GCMetrics::returnVisitCount () const {
    unsigned int cVisits = 0;
    for (unsigned int xSpace = 0; xSpace < m_cSpaceMetrics; xSpace++)
	cVisits += m_pSpaceMetrics[xSpace]->returnVisitCount ();
    return cVisits;
}

/*********************
 *****  Display  *****
 *********************/

void M_AND::GCMetrics::SpaceMetrics::displayMetrics (M_AND *pAND) const {
    if (m_cPasses > 0) {
	IO_printf ("+++ Space %4u: %u\n", m_xSpace, m_cPasses);
	unsigned int cSpaces = m_pDBMetrics->spaceCount ();
	for (unsigned int xSpace = 0; xSpace < cSpaces; xSpace++)
	    if (m_pReferenceCounts[xSpace].isntZero () ||
		m_pReferenceCounts[xSpace].returnVisitCount () > 0) {
		IO_printf (
		    "      ... %4u: %9u %9u",xSpace,
		    m_pReferenceCounts[xSpace].firstVisitCount (),
		    m_pReferenceCounts[xSpace].returnVisitCount ()
		);
		M_ASD *pASD = pAND->AccessASD (xSpace);
		if (pASD->Database () != pAND) IO_printf (
		    " %s[%u]", pASD->Database ()->NDFPathName (), pASD->Index ()
		);
		IO_printf ("\n");
	    }
    }
}

void M_AND::GCMetrics::displayMetrics (M_AND *pAND) const {
    ReferenceCounts cVisits;
    cVisits.initialize ();
    accumulateCounts (cVisits);

    IO_printf ("\nGarbage Collection Metrics:\n");
    IO_printf (
	"\tNumber of Invocations . . . . . %u\n", GCInvocationCount
    );
    IO_printf (
	"\tNumber of Queue Checks  . . . . %u\n", MarkContainersInQueue_callCount
    );
    IO_printf (
	"\tNumber of Queue Traversals  . . %u\n", MarkContainersInQueue_doCount
    );
    IO_printf (
	"\tMax Number of Queue Entries . . %u\n", GCMaxQueueEntryCnt
    );
    IO_printf (
	"\tNumber of Containers Marked . . %6u/%6u\n",
	cVisits.firstVisitCount (), GCFirstVisitCount
    );
    IO_printf (
	"\tNumber of Containers Revisited. %6u/%6u\n",
	cVisits.returnVisitCount (), GCRevisitCount
    );
    IO_printf ("\n%s\n", pAND->NDFPathName ());
    for (unsigned int xSpace = 0; xSpace < m_cSpaceMetrics; xSpace++) {
	m_pSpaceMetrics[xSpace]->displayMetrics (pAND);
    }
}

void M_AND::DisplayGCMetrics () {
    if (m_pGCMetrics)
	m_pGCMetrics->displayMetrics (this);
}


/****************************
 *****  Initialization  *****
 ****************************/

void M_AND::InitializeGCMetrics () {
    if (m_pGCMetrics) {
	delete m_pGCMetrics;
	m_pGCMetrics = 0;
    }
    m_pGCMetrics = new GCMetrics (m_cActiveSpaces);
}


/******************************
 ****  Marking Utilities  *****
 ******************************/

M_ASD::GCQueue::GCQueue (unsigned int cEntries) : 
    m_cStorageCells ((cEntries +1024)/1024), m_xStorageCell (0), m_pHead (0), m_pTail (0), m_pFree (0) {
    m_pStorage = (Entry**)allocate (sizeof (Entry*) * m_cStorageCells);
}

M_ASD::GCQueue::~GCQueue () {
    if (m_pStorage) {
	while (m_xStorageCell > 0)
	    deallocate (m_pStorage [--m_xStorageCell]);
	deallocate (m_pStorage);
    }
}

void M_ASD::GCQueue::Insert (unsigned int xContainer) {
    if (++GCQueueEntryCnt > GCMaxQueueEntryCnt) 
	GCMaxQueueEntryCnt = GCQueueEntryCnt;
    Entry *qe = m_pFree;

    if (IsNil (m_pFree) && m_xStorageCell < m_cStorageCells) {
        unsigned int cEntries = 1024;
	qe = m_pStorage[m_xStorageCell++] = (Entry*)allocate (cEntries * sizeof (Entry));
	while (cEntries-- > 0) {
	  qe->next = m_pFree;
	  m_pFree	 = qe++;
	}
	qe = m_pFree;
    }
    else if (IsNil (m_pFree)) {
      	ERR_SignalFault (
	     EC__MError, UTIL_FormatMessage (
		"Insufficient Queue Entries for Garbage Collection %u",m_cStorageCells * 1024
	    )
	);
    }

    m_pFree = qe->next;

    qe->next	= NilOf (Entry*);
    qe->index	= xContainer;

    if (m_pTail)
	m_pTail->next = qe;
    else
	m_pHead	= qe;

    m_pTail = qe;
}

unsigned int M_ASD::GCQueue::Remove () {
    --GCQueueEntryCnt;
    Entry* qe = m_pHead;

    if (IsNil (m_pHead = qe->next))
	m_pTail = NilOf (Entry *);

    unsigned int xContainer = qe->index;

    qe->next	= m_pFree;
    m_pFree	= qe;

    return xContainer;
}

void M_ASD::GCQueueInsert (unsigned int xContainer) {
    if (IsNil (m_pGCQueue))
	m_pGCQueue = new GCQueue (cteCount ());
    m_pGCQueue->Insert (xContainer);
}


/*********************************
 *****  Initialization Pass  *****
 *********************************/

/*---------------------------------------------------------------------------
 * Routine to perform initialization steps for a single space during the
 * execution of global garbage collection. The space's root is accessed, the
 * reference counts of all containers are set to zero, and any container with
 * an attached cpcc is inserted into a queue to be marked as referenced. (The
 * root of the space will be one of those inserted in the queue.)
 *
 *  Arguments:
 *	this			- the ASD for the space being initialized.
 *	ap			- an unused 'stdargs' argument pointer.
 *
 *  Returns:
 *	true
 *
 *---------------------------------------------------------------------------
 */
bool M_ASD::InitializeSpaceForGC (VArgList const&) {
    unsigned cSelfReferences = 0;
    int ctIndex = -1;

    if (IsntTheScratchPad ()) {
/*****  Put the space root into the marking queue...  *****/
	M_CTE cte (this, (unsigned int)M_KnownCTI_SpaceRoot);
	switch (cte.addressType ()) {
	case M_CTEAddressType_ForwardingPOP:
	    cte.setReferenceCountToZero ();
	    cte.gcVisited(false); 
	    break;

	default:
	    cte.setReferenceCountToInfinity ();
	    GCQueueInsert (cte.containerIndex ());
	    cte.gcVisited(true); 
	    cSelfReferences++;
	    break;
	}
	ctIndex = M_KnownCTI_SpaceRoot;
    }

/*****  Put containers with an attached CPCC into the marking queue...  *****/
    int ctBound = cteCount ();
    while (++ctIndex < ctBound) {
	M_CTE cte (this, ctIndex);
	switch (cte.addressType ()) {
#if defined(DebuggingForwardings)
	case M_CTEAddressType_ForwardingPOP:
	    if (cte.isReferenced ()) {
		M_CTE targetCTE (this, cte.addressAsPOP ());
		IO_printf (
		    "+++ Forwarding Found At [%u:%u](RC:%u) Referencing [%u:%u](RC:%u)\n",
		    cte.spaceIndex (),
		    cte.containerIndex (),
		    cte.referenceCount (),
		    targetCTE.spaceIndex (),
		    targetCTE.containerIndex (),
		    targetCTE.referenceCount ()
		);
	    }
	    /* No Break */
#endif

	default:
/************************************************************************
 *  Now that CTE's of new objects start life with a reference count of	*
 *  zero, under construction testing must be done in the sweep phase:	*
 *	    if (cte.isntUnderConstruction ())				*
 ************************************************************************/
	    cte.setReferenceCountToZero ();
	    cte.gcVisited(false); 
	    break;
	}

        #if defined(DEBUG_SESSION_GC)
            fprintf(stderr, "initializing [%d: %d] new: %d type: %d refcount: %d\n", 
                Index(), ctIndex, cte.isNew(), cte.addressType(), 
                cte.referenceCount()
            );
        #endif
    }

/*****  ... and update the metrics:  *****/
    if (cSelfReferences > 0)
	m_pAND->GCMetricsForSpace (m_xSpace)->recordSelfReferences (cSelfReferences);
    GCFirstVisitCount += cSelfReferences;

    return true;
}


bool M_ASD::InitializeSpaceForTransientGC (VArgList const &rArgList) {
    if (IsTheScratchPad ())
	InitializeSpaceForGC (rArgList);
    else {
    /*****  Initialize the self reference count...  *****/
	unsigned int cSelfReferences = 0;

    /*****  ... mark changed and active new containers, ...  *****/
	unsigned int xUB = m_iReadWriteRegion.ub ();
	for (unsigned int xCTE = m_iReadWriteRegion.lb (); xCTE <= xUB; xCTE++) {
	    M_CTE cte (this, xCTE);
	    switch (cte.addressType ()) {
	    case M_CTEAddressType_RWContainer:
		if (cte.isNew ()) {
		    cte.setReferenceCountToZero ();
		    cte.gcVisited(false);
		}
		else {
		    GCQueueInsert (cte.containerIndex ());
		    cte.gcVisited(true); 
		    cSelfReferences++;
		}
		break;

	    case M_CTEAddressType_CPCC: {
		VContainerHandle *pHandle = cte.addressAsContainerHandle ();
		if (pHandle->isPrecious ()) {
		    GCQueueInsert (cte.containerIndex ());
		    cte.gcVisited(true); 
		    cSelfReferences++;
		}
		else if (cte.isNew()) {
		    cte.setReferenceCountToZero ();
		    cte.gcVisited(false);
		}
		else if (pHandle->hasAReadWriteContainer ()) {
		    GCQueueInsert (cte.containerIndex ());
		    cte.gcVisited(true);
		    cSelfReferences++;
		}
/*****
 *  The preceding 'else if' clauses replace and simplify the following...
 *****
 *		else {
 *		    if (cte.isNew()) {
 *			cte.setReferenceCountToZero ();
 *			cte.gcVisited(false);
 *		    }
 *
 *		    if (pHandle->hasAReadWriteContainer ()) {
 *			if (cte.isntNew ()) {
 *			    GCQueueInsert (cte.containerIndex ());
 *			    cte.gcVisited(true);
 *			    cSelfReferences++;
 *			}
 *		    }
 *		}
 *****
 *  ... and makes the flow of the container handle logic parallel the flow of the
 *      'M_CTEAddressType_RWContainer' logic it mirrors.
 *****/
	    }
		break;

	    default:
		break;
	    }

            #if defined(DEBUG_SESSION_GC)
                fprintf(stderr, "initializing [%d: %d] new: %d type: %d refcount: %d\n", 
                    Index(), xCTE, cte.isNew(), cte.addressType(), 
                    cte.referenceCount()
                );
            #endif
	}

    /*****  ... and update the metrics:  *****/
	if (cSelfReferences > 0)
	    m_pAND->GCMetricsForSpace (m_xSpace)->recordSelfReferences (cSelfReferences);
	GCFirstVisitCount += cSelfReferences;
    }

    return true;
}


/*********************************************************************************
The 'Cycle Detect' (now a bit of a misnomer) phase of the garbage collector
is responsible for estimating the minimal set of container handles that must
be added to the garbage collector's root set in order to protect database
structures currently in use in this process.

In pre-8.1 versions of Vision, container handles were simple structures that
always and only served as database access handles.  In that model, all handles
were always added to the garbage collector's root set.

In release 8.1, container handles became more general.  While still serving
as access handles, they can also act as data caches, 'future's for persistable
structures, and participants in potentially cyclic transient data structures
of their own.  Just because a container handle exists and is attached to a
container table entry (CTE) doesn't mean that either the associated CTE or the
handle must be retained.  Handles may be directly or indirectly referenced from
other handles or from elsewhere in the process. Only those handles that are (or
might be) referenced from elsewhere must be added to the garbage collector's
root set.

To identify which container handles must be in the root set, the garbage
collector first identifies the set of CTEs reachable from the persistent
roots of the database.  That closure is computed on the basis of both inter-
container (POP references) and inter-handle references.  Those CTEs NOT in
that persistent root-set closure become candidates for the cycle-detect
phase of the garbage collector.

Trivially, the cycle-detect phase could add all container handles it finds
in the set of CTEs not included in the persistent root set's closure.  While
doing so is safe, it is not minimal.  To identify the CTEs associated with
container handles referenced from elsewhere in the process, the cycle-detect
algorithm first identifies those container handles that are ONLY referenced
from other handles outside the persistent closure.  It does that by ONLY
following inter-handle references to compute partial reference counts for
each handle reached directly or indirectly.  At the end of that process,
those handles whose partial reference count is LESS THAN their actual reference
count MUST be considered to be referenced from elsewhere in the process. Adding
them to the garbage collector's root set, another marking pass is run, completing
the garbage collector's reachability phase.

Note that the partial reference count computation process is allowed to miss
references.  Doing so results in under-counting the number of intra-database
internal references.  Under-counting is always safe; however, the cost of under-
counting is the failure to detect and reclaim orphaned structures.
**********************************************************************************/
bool M_ASD::EnqueuePossibleCycles (VArgList const &rArgList) {
    unsigned int xUB = cteCount () - 1;
    for (unsigned int cti = 0; cti <= xUB; cti++) {
	M_CTE cte (this, cti);
        if (!cte.gcVisited()) {
            cte.cdVisited(false);
	    if (cte.holdsACPCC ()) {
		VContainerHandle *pHandle = cte.addressAsContainerHandle ();
		GCQueueInsert (cte.containerIndex ());
		cte.cdVisited(true);

		pHandle->generateLogRecord ("EnqueuePC");
	    }
        }
    } 

   return true;
}

bool M_ASD::EnqueueOmittingCycles (VArgList const &rArgList) {
    unsigned int xUB = cteCount () - 1;
    for (unsigned int cti = 0; cti <= xUB; cti++) {
	M_CTE cte (this, cti);
	if (cte.holdsACPCC ()) {
	    VContainerHandle *pHandle = cte.addressAsContainerHandle ();
	    // record the handle's status after cycle detection...
	    cte.foundAllReferences(pHandle->foundAllReferences());
	    if (!cte.gcVisited()) {
		if (cte.foundAllReferences()) {
		    pHandle->generateLogRecord ("Omit");
		} else {
		    GCQueueInsert (cte.containerIndex ());
		    cte.gcVisited(true);
		    pHandle->generateLogRecord ("Keep");
		}
	    }
	}
    } 

   return true;
}

bool M_ASD::TraverseAndDetectCycles (VArgList const &rArgList) {
    if (IsNil (m_pGCQueue) || m_pGCQueue->MarkingComplete ())
        return true;			/****  Nothing to do  ****/

    ConsiderContainersInQueue(&m_GCCycleDetect);

/****  Indicate that some marking was done...  *****/
    VArgList iArgList (rArgList);
    *iArgList.arg<bool*>() = true;

    return true;		/**** Processed some containers  ****/
}



/**************************
 *****  Marking Pass  *****
 **************************/

/*---------------------------------------------------------------------------
 * Routine to iteratively mark all the containers in a space which are in the
 * marking queue.
 *
 *  Argument:
 *	asd			- and ASD for the space whose marking queue
 * 				  is to be processed.
 *	ap			- a 'stdargs' argument pointer referencing
 *				  the address of a 'bool' that this routine
 *				  will set to true if it marks any containers.
 *				  If no containers are marked, the 'bool' at
 *				  this address will be left unchanged.
 *
 *  Returns:
 *	true.
 *
 *---------------------------------------------------------------------------
 */

PrivateVarDef unsigned int ContainerCounter = 0;

void M_ASD::ConsiderContainersInQueue(M_ASD::GCVisitBase* pGCV) {
    if (IsNil (m_pGCQueue)) return;

    while (m_pGCQueue->MarkingIncomplete ()) {
        unsigned int cIndex = GCQueueRemove();
	M_CTE cte (this, cIndex);

/****  Echo the progress report marker  ****/
	if (TracingContainerMarking ()) {
	    if (0 == ContainerCounter % 70000)
		::printf ("\n");
	    if (0 == ContainerCounter++ % 1000) {
		::printf ("-");
		fflush (stdout);
	    }
	}

/****  Next, access the container's address  ****/
	switch (cte.addressType ()) {
	case M_CTEAddressType_ForwardingPOP:
/****
 * This case should have been handled earlier. A forwarding pop should
 * never show up in the marking queue
 ****/
	    ERR_SignalFault (
		EC__MError, "MarkContainersInQueue: Forwarding POP in queue"
	    );
	    break;
	case M_CTEAddressType_CPCC:
	    pGCV->processContainerHandle (cte, cte.addressAsContainerHandle ());
	    break;
	case M_CTEAddressType_ROContainer:
	case M_CTEAddressType_RWContainer:
	    cte.InitializeCTEContainerAddress ();
	    pGCV->processContainerAddress (cte, cte.addressAsContainerAddress ());
	    break;
	default:
	    ERR_SignalFault (
		EC__MError, "MarkContainersInQueue: Unknown CTE type"
	    );
	    break;
	}
    }
}

bool M_ASD::MarkContainersInQueue (VArgList const &rArgList) {
    MarkContainersInQueue_callCount++;
    if (IsNil (m_pGCQueue) || m_pGCQueue->MarkingComplete ())
        return true;			/****  Nothing to do  ****/
    MarkContainersInQueue_doCount++;

/****  Update the pass counter...  *****/
    m_pAND->GCMetricsForSpace (m_xSpace)->recordPass ();

/****  Continue processing until nothing left in the marking queue  ****/
    ConsiderContainersInQueue(&m_GCMarker);


/****  Indicate that some marking was done...  *****/
    VArgList iArgList (rArgList);
    *iArgList.arg<bool*>() = true;

    return true;		/**** Processed some containers  ****/
}




/*---------------------------------------------------------------------------
 * Driving routine for the marking phase of the garbage collector.  Each space
 * will be iteratively processed until no more marking is required.
 *
 *  Arguments:
 *	None.
 *
 *  Returns:
 *	Nothing.
 *
 *---------------------------------------------------------------------------
 */
void M_AND::PerformMarkPhaseOfGC () {
    ContainerCounter = 0;

    bool moreToDo = true;
    while (moreToDo) {
	moreToDo = false;
	EnumerateAccessedSpaces (&M_ASD::MarkContainersInQueue, &moreToDo);
    }

    if (TracingContainerMarking ()) {
	::printf ("\n");
	fflush (stdout);
    }
}


/**************************
 *****  Cleanup Pass  *****
 **************************/

/*---------------------------------------------------------------------------
 * Routine to perform final cleanup steps for a single space during the
 * execution of global garbage collection.  The free list for the space's
 * transient container table is rebuilt from all containers whose reference
 * count remains zero.
 *
 *  Argument:
 *	this			- an ASD for the space being swept.
 *	ap			- a 'stdargs' pointer referencing the address
 *				  of a 'bool' that will be set to true if at
 *				  least one container is reclaimed.  If no
 *				  containers are reclaimed, the 'bool' at this
 *				  address will be unchanged.
 *
 *  Returns:
 *	See the discussion of 'ap'.
 *
 *---------------------------------------------------------------------------
 */
bool M_ASD::SweepUp (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    bool *pSweepResult= iArgList.arg<bool*> ();
    bool bReclaimedPersistentContainers = false;
    bool bReclaimedContainers = false;

/****  Cleanup marking structures...  ****/
    if (m_pGCQueue) {
	delete m_pGCQueue;
	m_pGCQueue = NilOf (GCQueue *);
    }

/****  Reinitialize the space's freelist  ****/
    m_iCT.resetFreeList ();

/****  Loop through the entries excluding the container table itself ...  ****/
/****  ... and rebuild free list so that lower indexed containers will    ****/
/****  ... be allocated first.                                            ****/
//    for (unsigned int ctIndex = cteCount () - 1; ctIndex > 0 ; ctIndex--) {
//	M_CTE cte (this, ctIndex);
//  The above was changed since cteCount () can be zero ...

    for (
        unsigned int ctIndex = cteCount (); 
        ctIndex > 1 || (ctIndex > 0 && m_xSpace == 0); 
    ) 
    {
	M_CTE cte (this, --ctIndex);
	m_iCT.setFreeListBaseTo (ctIndex);

        #if defined(DEBUG_SESSION_GC)
            fprintf(stderr, "sweep [%d: %d] new: %d type: %d refHandle: %d refcount: %d\n", 
                Index(), ctIndex, cte.isNew(), cte.addressType(), 
                ( cte.addressType() == M_CTEAddressType_CPCC &&
                  cte.addressAsContainerHandle()->isReferenced()
                ), 
                cte.referenceCount()
            );
        #endif

/*---------------------------------------------------------------------------*
 *  At this point, the garbage collector can reclaim resources consumed by
 *  unreferenced containers.  The logic here is similar to the logic of
 *  M_DCTE::reclaim (...); however, this code must NOT call any R-Type
 *  specific container reclamation handlers since the POP references
 *  those handlers 'release' will not have been counted during the garbage
 *  collection and may not even continue to refer to valid CTEs!
 *---------------------------------------------------------------------------*/
	if (cte.isASweepTarget ()) {
	    M_CPreamble *pAddress = 0;
	    bool bOkToReclaim = true;

	/****  Deallocate any read/write containers  ****/
	    switch (cte.addressType ()) {
	    case M_CTEAddressType_ROContainer:
		bReclaimedContainers = bReclaimedPersistentContainers = true;
		break;

	    case M_CTEAddressType_RWContainer:
		pAddress = cte.addressAsContainerAddress ();
		break;

	    case M_CTEAddressType_CPCC: {
		/*************************************************************
		 *  Handles that get here have been retained for performance
		 *  reasons or to hold transient extensions.  They do NOT
		 *  represent container references unless they themselves
		 *  are referenced.
		 *************************************************************/
		    VContainerHandle *pHandle = cte.addressAsContainerHandle ();
		    if (cte.gcVisited () || pHandle->isReferenced() && !cte.foundAllReferences()) {
			bOkToReclaim = false;
			pHandle->generateLogRecord ("Preserve");
		    } else {
			pHandle->generateLogRecord ("Reclaim");
			if (pHandle->hasAReadWriteContainer ())
			    pAddress = pHandle->containerAddress ();
			else {
			    bReclaimedContainers = true;
			    bReclaimedPersistentContainers = pHandle->hasAContainer ();
			}
			pHandle->DetachFromCTE ();
		    }
		}
		break;

	    default:
		break;
	    }

	    if (bOkToReclaim) {
#if defined(DEBUG_SESSION_GC)
		fprintf(stderr, "  reclaiming [%d: %d] new: %d type: %d "
			"refHandle: %d refcount: %d\n", 
			Index(), ctIndex, cte.isNew(), cte.addressType(), 
			cte.holdsACPCC() && cte.addressAsContainerHandle()->isReferenced(), 
			cte.referenceCount()
		);
#endif


		if (pAddress) {
		    DeallocateContainer (pAddress);
		    bReclaimedContainers = true;
		    if (cte.isntNew ())
			bReclaimedPersistentContainers = true;
		}
		cte.DeallocateCTE ();
	    }
	}
    }

    // cleanup CTE flags
    for (unsigned int ctIndex = cteCount (); ctIndex > 1 || (ctIndex > 0 && m_xSpace == 0); )  {
	M_CTE cte (this, --ctIndex);
	cte.gcVisited(true);
	cte.cdVisited(true);
	cte.foundAllReferences(false);
    }



/****  indicate that the container table has been modified  ****/
    if (bReclaimedPersistentContainers)
	CompelUpdate ();

    if (bReclaimedContainers)
	*pSweepResult= true;

    return true;
}


/********************
 *****  Driver  *****
 ********************/

/*---------------------------------------------------------------------------
 * Driving routine for the sweeping phase of the garbage collector.
 *
 *  Arguments:
 *	None.
 *
 *  Returns:
 *	True.
 *
 *---------------------------------------------------------------------------
 */
bool M_AND::PerformSweepPhaseOfGC () {
    bool sweptAway = false;
    EnumerateAccessedSpaces (&M_ASD::SweepUp, &sweptAway);
    return sweptAway;
}


/***************************
 *****  Marking Tools  *****
 ***************************/

bool M_DCTE::mark (M_ASD::GCVisitBase* pGCV, M_ASD *pASD, unsigned int xContainer) {
    bool isAReturnVisit = gcVisited();

    retain ();

    #if defined(DEBUG_SESSION_GC)
	fprintf(stderr, "  visiting [%d: %d]: new: %d type: %d refcount: %d\n", 
	    pASD->Index(), xContainer, isNew(), addressType(), referenceCount()
	);
    #endif


    if (isAReturnVisit)
	GCRevisitCount++;

/*****  Reference count was zero, so we haven't visited this container during
   this graph walk.  We are required to recursively mark the containers
   referenced by this previously unvisited one.  ****/
    else {
	GCFirstVisitCount++;
	switch (m_xAddressType) {
	case M_CTEAddressType_ForwardingPOP:
	    g_iForwardedContainerCount++;
	    pGCV->Mark (pASD, &addressAsPOP ());
	    break;
	default:
	    pASD->GCQueueInsert (xContainer);
	    gcVisited(true);
	    break;
	}
    }

    return !isAReturnVisit;
}


/*---------------------------------------------------------------------------
 * Quasi-public routine to mark an array of containers as referenced and for
 * each container that has not been visited, to initiate marking of the
 * containers it references.
 *
 *  Arguments:
 *	pReferences	-  A pointer to an array of POPs for the containers
 *			   to Mark.
 *	cReferences	-  The number of POPs in the array.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *---------------------------------------------------------------------------
 */
void M_ASD::GCVisitBase::Mark_(M_ASD* pASD, M_POP const *pReferences, unsigned int cReferences) {
    while (cReferences-- > 0)
	Mark (pASD, pReferences++);
}

/*---------------------------------------------------------------------------
 * Quasi-public routine to mark a container as referenced and if
 * the container has not been visited, to initiate marking of the
 * containers it references.
 *
 *  Arguments:
 *	pPOP		-  A pointer to a POP for the container to Mark.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *---------------------------------------------------------------------------
 */
void M_ASD::GCVisitBase::Mark_(M_ASD* pASD, M_POP const *pPOP) {
}

void M_ASD::GCVisitMark::Mark_(M_ASD* pASD, M_POP const *pPOP) {
  if (M_POP_ObjectSpace (pPOP) != 1) {
    M_CTE cte (pASD->Database(), pPOP);
    pASD->recordGCReference (
	M_POP_ObjectSpace (pPOP), cte.Mark (this)
    );
  }
}


/*--------------------------------*
 *  M_ASD::GCVisitBase
 *--------------------------------*/
void M_ASD::GCVisitBase::processContainerAddress (M_CTE &rCTE, M_CPreamble *pAddress) {
/****
 *  Call the rtype handler to insert the containers it references into the
 *  appropriate marking queue
 ****/
    if (pAddress) {	// ... nil for objects whose container creation has been deferred.
/****
 * Since we are pulling out an address to be used by the marking function
 * we need to protect it. Within the marking function, a new segment might be
 * accessed which could trigger a segment remapping. If that happens, then
 * pAddress would perhaps become invalid.
 ****/
	M_Type_MarkFn pMarkingFunction = M_RTD_MarkFn (
	    M_RTDPtr (M_CPreamble_RType (pAddress))
	);
	if (pMarkingFunction) {
	    rCTE.setMustStayMapped ();

#if defined(DEBUG_SESSION_GC)
	    fprintf(stderr, "marking [%d: %d] new: %d type: %d refHandle: %d refcount: %d\n", 
		    rCTE.spaceIndex(), cIndex, rCTE.isNew(), rCTE.addressType(), 
		    ( rCTE.holdsACPCC () && rCTE.addressAsContainerHandle()->isReferenced()), 
		    rCTE.referenceCount()
	    );
#endif

	    pMarkingFunction (this, rCTE.space (), pAddress);
/**** We are done with pAddress now, so we can remove the protection. ****/
	    rCTE.clearMustStayMapped ();
	}
    }
}

void M_ASD::GCVisitBase::processContainerHandle (M_CTE &rCTE, VContainerHandle *pHandle) {
    processContainerAddress (rCTE, pHandle->containerAddress ());
}

/*--------------------------------*
 *  M_ASD::GCVisitMark
 *--------------------------------*/
void M_ASD::GCVisitMark::processContainerHandle (M_CTE &rCTE, VContainerHandle *pHandle) {
    BaseClass::processContainerHandle (rCTE, pHandle);
    pHandle->visitReferencesUsing (this);
}
void M_ASD::GCVisitMark::visitHandle (VContainerHandle *pHandle) {
    pHandle->gcMarkFor (this);
}

/*--------------------------------*
 *  M_ASD::GCVisitCycleDetect
 *--------------------------------*/
void M_ASD::GCVisitCycleDetect::processContainerAddress (M_CTE &rCTE, M_CPreamble *pAddress) {
}

void M_ASD::GCVisitCycleDetect::processContainerHandle (M_CTE &rCTE, VContainerHandle *pHandle) {
    pHandle->visitReferencesUsing (this);
}

void M_ASD::GCVisitCycleDetect::visitHandle (VContainerHandle *pHandle) {
    pHandle->cdMarkFor (this);
}

/*---------------------------------------------------------------------------
 *---------------------------------------------------------------------------
 */

M_ASD::GCVisitMark 	  M_ASD::m_GCMarker;
M_ASD::GCVisitCycleDetect M_ASD::m_GCCycleDetect;



/*---------------------------------------------------------------------------
 * Top Level Routine for running the network's garbage collector.
 *
 *  Arguments:
 *	NONE.
 *
 *  Returns:
 *	true if any garbage was collected, false otherwise.
 *
 *---------------------------------------------------------------------------
 */
bool M_AND::DisposeOfNetworkGarbage () {
//  Disable garbage collection if multiple databases are open...
    if (ENVIR_Session ()->DatabaseActivationCount () > 1)
	return false;

//  ... otherwise, do the collection:
    FlushCachedResources ();

    bool result = false;
    UNWIND_Try {
	OnGCStart ();
	g_bNetworkGarbageCollectedInSession = true;

	GCInvocationCount++;
	GCRevisitCount = GCFirstVisitCount = 0;

	M_DCTE::g_bPotentialSessionGarbage = false;
	M_DCTE::g_iForwardedContainerCount = 0;

	InitializeGCMetrics ();
	EnumerateSpaces (&M_ASD::InitializeSpaceForGC);

	PerformMarkPhaseOfGC ();
	DoGCCycleElimination();
	PerformMarkPhaseOfGC ();
	result = PerformSweepPhaseOfGC ();

	M_DCTE::g_bPotentialSessionGarbage = false;
    } UNWIND_Finally {
	OnGCFinish ();
    } UNWIND_EndTryFinally;

    return result;
}


/*****************************************
 *****  Transient Garbage Collector  *****
 *****************************************/

bool __cdecl M_ASD::Apply (bool (M_ASD::*elementProcessor) (VArgList const&), ...) {
    V_VARGLIST (iArgList, elementProcessor);
    return (this->*elementProcessor) (iArgList);
}

void M_AND::DoTransientGCSetup () {
    InitializeGCMetrics ();
    EnumerateModifiedSpaces (&M_ASD::InitializeSpaceForTransientGC);
}

void VDatabaseFederatorForBatchvision::DoTransientGCSetup () const {
    VDatabaseActivation *pActivation = m_pActivationListHead;
    while (pActivation) {
	static_cast<M_AND*>(pActivation)->DoTransientGCSetup ();
	pActivation = pActivation->successor ();
    }
}

bool M_AND::DoTransientGCMarking () {
    ContainerCounter = 0;

    bool moreToDo = false;
    EnumerateModifiedSpaces (&M_ASD::MarkContainersInQueue, &moreToDo);


    if (TracingContainerMarking ()) {
	::printf ("\n");
	fflush (stdout);
    }

    return moreToDo;
}

void VDatabaseFederatorForBatchvision::DoTransientGCMarking () const {
    bool moreToDo;
    do {
	moreToDo = false;
	VDatabaseActivation *pActivation = m_pActivationListHead;
	while (pActivation) {
	    if (static_cast<M_AND*>(pActivation)->DoTransientGCMarking ())
		moreToDo = true;
	    pActivation = pActivation->successor ();
	}
    } while (moreToDo);
}

bool M_AND::EnqueuePossibleCycles () {
    EnumerateModifiedSpaces (&M_ASD::EnqueuePossibleCycles);
    return true;
}

bool VDatabaseFederatorForBatchvision::EnqueuePossibleCycles() const {
    for ( VDatabaseActivation *pActivation = m_pActivationListHead; 
          pActivation; 
          pActivation = pActivation->successor()
        ) 
    {
        static_cast<M_AND*>(pActivation)->EnqueuePossibleCycles ();
    }
    return true;
}

bool M_AND::TraverseAndDetectCycles() {
    bool moreToDo = true;
    bool workDone = false;
    while (moreToDo) {
	moreToDo = false;
	EnumerateModifiedSpaces (&M_ASD::TraverseAndDetectCycles, &moreToDo);
	if (moreToDo) 
	    workDone = true;
    }

    return workDone;
}

bool VDatabaseFederatorForBatchvision::TraverseAndDetectCycles() const {
    bool moreToDo = true;
    bool workDone = false;

    while (moreToDo) {
	moreToDo = false;
	VDatabaseActivation *pActivation = m_pActivationListHead;
	while (pActivation) {
	    if (static_cast<M_AND*>(pActivation)->TraverseAndDetectCycles ()) {
		moreToDo = true;
                workDone = true;
            }
	    pActivation = pActivation->successor ();
	}
    }

    return workDone;
}

bool M_AND::EnqueueOmittingCycles () {
    EnumerateModifiedSpaces (&M_ASD::EnqueueOmittingCycles);
    return false;
}

bool VDatabaseFederatorForBatchvision::EnqueueOmittingCycles () const {
    bool cyclesOmitted = false;
    for ( VDatabaseActivation *pActivation = m_pActivationListHead; 
          pActivation; 
          pActivation = pActivation->successor()
        ) 
    {
        if (static_cast<M_AND*>(pActivation)->EnqueueOmittingCycles ()) 
	    cyclesOmitted = true;
    }

    return cyclesOmitted;
}

bool M_AND::DoGCCycleElimination () { 
    EnqueuePossibleCycles();
    TraverseAndDetectCycles();

    return EnqueueOmittingCycles();
}
 
bool VDatabaseFederatorForBatchvision::DoGCCycleElimination() const { 
    EnqueuePossibleCycles();
    TraverseAndDetectCycles();

    return EnqueueOmittingCycles();
}

bool M_AND::DoTransientGCSweep () {
    bool sweptAway = false;
    EnumerateModifiedSpaces (&M_ASD::SweepUp, &sweptAway);
    return sweptAway;
}

bool VDatabaseFederatorForBatchvision::DoTransientGCSweep () const {
    VDatabaseActivation *pActivation = m_pActivationListHead;

    bool result = false;
    while (pActivation) {
	if (static_cast<M_AND*>(pActivation)->DoTransientGCSweep ())
	    result = true;
	pActivation = pActivation->successor ();
    }

    return result;
}

PrivateVarDef bool TracingSessionGC = false;

bool VDatabaseFederatorForBatchvision::DisposeOfSessionGarbage (bool bAggressive) const {
    bool result = false;

    // Don't run if the network garbage collector is running and only
    // bother if garbage could have potentially been created since
    // last disposal ....
    if (!M_AND::GarbageCollectionRunning () && M_DCTE::g_bPotentialSessionGarbage) {
	if (bAggressive)
	    FlushCachedResources ();

	UNWIND_Try {
	    M_AND::OnGCStart ();

	    GCInvocationCount++;
	    GCRevisitCount = GCFirstVisitCount = 0;
	    if (TracingSessionGC) {
		IO_printf ("Entering GC:\t%14.0f\n", ENVIR_SessionMemoryUse ());
		log ("Batchvision[%u]: Entering GC:\t%14.0f", getpid (), ENVIR_SessionMemoryUse ());
	    }
	    M_DCTE::g_bPotentialSessionGarbage = false;
	    M_DCTE::g_iForwardedContainerCount = 0;

	    DoTransientGCSetup ();
	    DoTransientGCMarking ();
	    DoGCCycleElimination();
	    DoTransientGCMarking (); // new second mark phase
	    result = DoTransientGCSweep ();
	    if (TracingSessionGC) {
	      IO_printf ("Leaving GC: \t%14.0f\n", ENVIR_SessionMemoryUse ());
	      log ("Batchvision[%u]: Leaving GC: \t%14.0f", getpid (), ENVIR_SessionMemoryUse ());
	    }

	} UNWIND_Finally {
	    M_AND::OnGCFinish ();
	} UNWIND_EndTryFinally;
    }
    return result;
}

PublicFnDef bool M_DisposeOfSessionGarbage (bool bAggressive) {
    return ENVIR_Session ()->DisposeOfSessionGarbage (bAggressive);
}


/*************************************
 *************************************
 *****  Miscellaneous Utilities  *****
 *************************************
 *************************************/

/*************************************
 *****  Error Logging Interface  *****
 *************************************/

/*---------------------------------------------------------------------------
 *****  Routine to make a 'vprintf' style error log entry.
 *
 *  Arguments:
 *	format			- a 'printf' style format string.
 *	ap			- a 'stdargs' argument pointer supplying the
 *				  parameters specified by 'format'.
 *
 *  Returns:
 *	The number of characters generated by 'format' and 'ap'.
 *
 *****/
void M_AND::VLogError (char const* format, VArgList const &rArgList) {
    m_pPhysicalAND->VLogError (format, rArgList);
}

/*---------------------------------------------------------------------------
 *****  Routine to make a 'printf' style error log entry.
 *
 *  Arguments:
 *	Like 'printf'.
 *
 *  Returns:
 *	The number of characters generated by 'format' and 'ap'.
 *
 *****/
PublicFnDef void __cdecl M_LogError (char const* format, ...) {
    V_VARGLIST (iArgList, format);
    M_AttachedNetwork->VLogError (format, iArgList);
}


/********************************
 *****  Statistics Queries  *****
 ********************************/

bool M_ASD::AddressSpaceQuery (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    unsigned __int64 *const allocTotal = iArgList.arg<unsigned __int64*>();
    unsigned __int64 *const mappingTotal = iArgList.arg<unsigned __int64*>();

    *allocTotal += m_sTransientAllocation;

    if (m_pPASD) {
	unsigned int segmentCount;
	*mappingTotal += m_pPASD->MappedSizeOfSpace (&segmentCount);
    }

    return true;
}

void M_AND::AccumulateAllocationStatistics (
    unsigned __int64 *pAllocationTotal, unsigned __int64 *pMappingTotal
) const {
    EnumerateAccessedSpaces (
	&M_ASD::AddressSpaceQuery, pAllocationTotal, pMappingTotal
    );
}

/*---------------------------------------------------------------------------
 *****  Routine to access the number of bytes in private containers and
 *****  memory mapped segments in this session.
 *
 *  Arguments:
 *	allocTotal		- address to hold private allocation result
 *	mappingTotal		- address to hold mapped segment size result
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void M_AccumulateAllocationStatistics (
    double *pAllocationTotal, double *pMappingTotal
) {
    unsigned __int64 sTotalAllocation = 0, sTotalMapping = 0;
    ENVIR_Session ()->AccumulateAllocationStatistics (&sTotalAllocation, &sTotalMapping);
    *pAllocationTotal = static_cast<double>(sTotalAllocation);
    *pMappingTotal = static_cast<double>(sTotalMapping);
}


/************************************
 *****  Default Object Printer  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to produce a representation type specific display of a
 *****  physical object.
 *
 *  Arguments:
 *	this			- the address of a standard CPD for the
 *				  container to be displayed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void M_CPD::print () const {
    IO_printf ("#%s[%u:%u]", RTypeName (), spaceIndex (), containerIndex ());
}


/*******************************
 *******************************
 *****  Facility Services  *****
 *******************************
 *******************************/

/******************************
 *****  Debugger Methods  *****
 ******************************/

/***************************
 *  Switch Access Methods  *
 ***************************/

IOBJ_DefineNilaryMethod (FreePoolsDisabledDM) {
    return IOBJ_SwitchIObject (&V::VAllocatorFreeList::g_bFreePoolDisabled);
}

IOBJ_DefineNilaryMethod (PreservingCPCCsDM) {
    return IOBJ_SwitchIObject (&VContainerHandle::g_bPreservingHandles);
}

IOBJ_DefineNilaryMethod (SegmentScanTraceDM) {
    return IOBJ_SwitchIObject (&PS_TracingSegmentScans);
}

IOBJ_DefineNilaryMethod (SessionGCTraceDM) {
    return IOBJ_SwitchIObject (&TracingSessionGC);
}

#if defined(INCLUDING_XDB_REFERENCE_TRACING)
IOBJ_DefineNilaryMethod (CrossDBReferenceTraceDM) {
    return IOBJ_SwitchIObject (&TracingCrossDBReferences);
}
#endif


/***************************
 *  State Display Methods  *
 ***************************/

IOBJ_DefineUnaryMethod (DisplayConstantsDM) {
    IO_printf ("Sizes of \'M\' Defined Types:\n");
    UTIL_DisplaySizeofType ( M_POP           );
    UTIL_DisplaySizeofType ( M_CPreamble     );
    UTIL_DisplaySizeofType ( M_CEndMarker    );
    UTIL_DisplaySizeofType ( M_CPD           );
    UTIL_DisplaySizeofType ( VContainerHandle);
    UTIL_DisplaySizeofType ( M_AND           );
    UTIL_DisplaySizeofType ( M_ASD           );
    UTIL_DisplaySizeofType ( M_CTE           );
    UTIL_DisplaySizeofType ( M_DCTE          );
    UTIL_DisplaySizeofType ( M_KOT           );
    UTIL_DisplaySizeofType ( M_KOTE          );
    UTIL_DisplaySizeofType ( M_RTD           );

    IO_printf ("\nSizes of \'PS\' Defined Types:\n");
    UTIL_DisplaySizeofType ( PS_AND          );
    UTIL_DisplaySizeofType ( PS_ASD          );
    UTIL_DisplaySizeofType ( PS_CT           );
    UTIL_DisplaySizeofType ( PS_SDC          );
    UTIL_DisplaySizeofType ( PS_SH           );
    UTIL_DisplaySizeofType ( PS_SMD          );
    UTIL_DisplaySizeofType ( PS_RID          );
    UTIL_DisplaySizeofType ( PS_TID          );
    UTIL_DisplaySizeofType ( PS_UID          );
    UTIL_DisplaySizeofType ( PS_CTE          );
    UTIL_DisplaySizeofType ( PS_NDH          );
    UTIL_DisplaySizeofType ( PS_NVD          );
    UTIL_DisplaySizeofType ( PS_SD           );
    UTIL_DisplaySizeofType ( PS_SRD          );
    UTIL_DisplaySizeofType ( PS_SVD          );

    IO_printf ("\nImplementation Constants:\n");
    UTIL_DisplayExpression (  %u, M_KnownSpace_ScratchPad  );
    UTIL_DisplayExpression (  %u, M_KnownSpace_Nil         );
    UTIL_DisplayExpression (  %u, M_KnownSpace_SystemRoot  );
    UTIL_DisplayExpression (  %u, M_KnownSpace_UserRoot    );
    UTIL_DisplayExpression (  %u, M_KnownCTI_ContainerTable);
    UTIL_DisplayExpression (  %u, M_KnownCTI_SpaceRoot     );
    UTIL_DisplayExpression (  %u, M_POP_MaxObjectSpace     );
    UTIL_DisplayExpression (  %u, M_POP_MaxContainerIndex  );
    UTIL_DisplayExpression (  %u, M_CTE_MaxReferenceCount  );

    return self;
}


IOBJ_DefineUnaryMethod (DisplayStateDM) {
    unsigned int CPCCAllocationCount, CPCCDeallocationCount;
    GetCPCCCounts (CPCCAllocationCount, CPCCDeallocationCount);
    unsigned int CPCCInUseCount = CPCCAllocationCount - CPCCDeallocationCount;

    IO_printf ("\nCPCC State:\n");
    UTIL_DisplayExpression (%u, CPCCAllocationCount  );
    UTIL_DisplayExpression (%u, CPCCDeallocationCount);
    UTIL_DisplayExpression (%u, CPCCInUseCount       );

    unsigned int CPDAllocationCount, CPDDeallocationCount;
    GetCPDCounts (CPDAllocationCount, CPDDeallocationCount);
    unsigned int CPDInUseCount = CPDAllocationCount - CPDDeallocationCount;

    IO_printf ("\nCPD State:\n");
    UTIL_DisplayExpression (  %u, CPDAllocationCount  );
    UTIL_DisplayExpression (  %u, CPDDeallocationCount);
    UTIL_DisplayExpression (  %u, CPDInUseCount       );

    return self;
}


IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    unsigned int CPDsCreated, CPDsDestroyed;
    GetCPDCounts (CPDsCreated, CPDsDestroyed);
    IO_printf ("Usage Counts:\n");
    IO_printf ("-------------\n");
    IO_printf ("CPDsCreated              = %u\n",CPDsCreated);
    IO_printf ("CPDsDestroyed            = %u\n",CPDsDestroyed);
    IO_printf ("CPDsInUse                = %u\n",CPDsCreated - CPDsDestroyed);
    IO_printf ("AllocateContainer        = %u\n",AllocateContainerCount);
    IO_printf ("DestroyContainer         = %u\n",DeallocateContainerCount);
    IO_printf ("ShiftContainerTailCount  = %u\n",ShiftContainerTailCount);
    IO_printf ("ReallocateContainerCount = %u\n",ReallocateContainerCount);
    IO_printf ("EnableModificationsCount = %u\n",EnableModificationsCount);
    IO_printf ("SegmentScanCount         = %u\n",PS_SegmentScanCount);
    IO_printf ("SegmentReclamationCount  = %u\n",PS_SegmentReclamationCount);
    IO_printf ("SegmentReclamationMinimum= %u\n",PS_SegmentReclamationMinimum);
    IO_printf ("SegmentReclamationMaximum= %u\n",PS_SegmentReclamationMaximum);
    IO_printf ("PCTPrivatizations        = %u\n",PS_PCTPrivatizations);
    IO_printf ("ForwardedContainerCount  = %u\n",M_DCTE::g_iForwardedContainerCount);
    return self;
}

IOBJ_DefineUnaryMethod (DisplayGCInfoDM) {
    IOBJ_ScratchPad->Database()->DisplayGCMetrics ();
    return self;
}


/***********************************
 *  Container Mapping Information  *
 ***********************************/

bool M_ASD::DisplaySpaceMappingInfo (VArgList const &rArgList) {
    unsigned int xContainer, mappedContainers;
    double segmentBytes = 0, mappedBytes = 0;
    unsigned int segmentCount = 0;

    for (xContainer = mappedContainers = 0; xContainer < cteCount (); xContainer++) {
	M_CTE cte (this, xContainer);

	M_CPreamble *cp = cte.containerAddress ();
	if (cp) {
	    mappedContainers++;
	    mappedBytes += M_CPreamble_Size (cp);
	}
    }

    if (m_pPASD)
	segmentBytes = static_cast<double>(m_pPASD->MappedSizeOfSpace (&segmentCount));

    IO_printf (
	"%8u   %8u   %14.0f   %8u    %14.0f\n",
	m_xSpace,
	mappedContainers,
	mappedBytes,
	segmentCount,
	segmentBytes
    );

    VArgList iArgList (rArgList);

    unsigned int*const	totalContainerCount = iArgList.arg<unsigned int*>();
    double	*const 	totalContainerBytes = iArgList.arg<double*>();
    unsigned int*const	totalSegmentCount   = iArgList.arg<unsigned int*>();
    double	*const 	totalSegmentBytes   = iArgList.arg<double*>();

    *totalContainerCount += mappedContainers;
    *totalContainerBytes += mappedBytes;
    *totalSegmentCount   += segmentCount;
    *totalSegmentBytes   += segmentBytes;

    return true;
}

IOBJ_DefineUnaryMethod (DisplayMappingInfoDM) {
    IO_printf ("   Space  Container        Container    Segment           Segment\n");
    IO_printf ("   Index      Count            Bytes      Count             Bytes\n");
    IO_printf ("   -----   --------       ----------   --------        ----------\n");

    unsigned int containerCount = 0;
    double containerBytes = 0;
    unsigned int segmentCount   = 0;
    double segmentBytes   = 0;

    IOBJ_ScratchPad->Database ()->EnumerateAccessedSpaces (
	&M_ASD::DisplaySpaceMappingInfo,
	&containerCount	, &containerBytes,
	&segmentCount	, &segmentBytes
    );

    IO_printf (
	"\n%8s   %8d   %14.0f   %8d    %14.0f\n",
	"total", containerCount, containerBytes, segmentCount, segmentBytes
    );

    return self;
}


/*************************************
 *  Container Retention Information  *
 *************************************/

PrivateFnDef void DisplayRetentionInfoHeading () {
    IO_printf ("                                       CPD   CPCC CTE\n");
    IO_printf ("R-Type               POP              # Refs Refs Refs Precious?\n");
    IO_printf ("-------------------- ------------- ---- ---- ---- ---- ---------\n");
}

bool M_ASD::DisplayRetentionInfoForSpace (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    bool const bDisplayingAllContainers = iArgList.arg<unsigned int> () ? true : false;

/*****  ... access the container table bounds, ...  *****/
    unsigned int lowerCTIBound = 0;
    unsigned int upperCTIBound = cteCount ();

/*****  ... and enumerate the modified containers:  *****/
    while (lowerCTIBound < upperCTIBound) {
    /*****  Access the CTE, ...  *****/
	M_CTE cte (this, lowerCTIBound);

    /*****  ... 'continue' if this entry is inappropriate, ...  *****/
	VContainerHandle const *pCPCC;
	switch (cte.addressType ()) {
	case M_CTEAddressType_CPCC:
	    pCPCC = cte.addressAsContainerHandle ();
	    if (bDisplayingAllContainers || pCPCC->isReferenced () || pCPCC->isPrecious ()) {
		unsigned int cpdCount = 0;
		unsigned int cpdReferenceCount = 0;
		for (
		    M_CPD const *cpd = pCPCC ->cpdChainHead ();

		    IsntNil (cpd);

		    cpdCount++,
		    cpdReferenceCount += M_CPD_ReferenceCount (cpd),
		    cpd = M_CPD_NLink (cpd)
		);
		IO_printf (
		    "#%-20s[%4u:%6u] %4u %4u %4u",
		    pCPCC->RTypeName (),
		    m_xSpace,
		    pCPCC->containerIndexNC (),
		    cpdCount,
		    cpdReferenceCount,
		    pCPCC->referenceCount ()
		);
		if (cte.referenceCountIsFinite ())
		    IO_printf (" %4u", cte.referenceCount ());
		else
		    IO_printf ("  Inf");

		if (pCPCC->isPrecious ())
		    IO_printf (" Precious");
		IO_printf ("\n");
	    }
	    break;

	default:
	    break;
	}
	lowerCTIBound++;
    }

    return true;
}

IOBJ_DefineUnaryMethod (DisplayRetentionInfoDM) {
    DisplayRetentionInfoHeading ();
    IOBJ_ScratchPad->Database ()->EnumerateAccessedSpaces (
	&M_ASD::DisplayRetentionInfoForSpace, true
    );

    return self;
}

IOBJ_DefineUnaryMethod (DisplayTXRetentionInfoDM) {
    DisplayRetentionInfoHeading ();
    IOBJ_ScratchPad->Database ()->EnumerateAccessedSpaces (
	&M_ASD::DisplayRetentionInfoForSpace, false
    );

    return self;
}


IOBJ_DefineUnaryMethod (DisplayCPDCountsByRType) {
    IO_printf
	("                                    CPD's     Total   Std Ptr\n");
    IO_printf
	(" RType                             In Use     CPD's     Count\n");
    IO_printf
	(" ------------------------------    ------    ------   -------\n");

    unsigned int lines = 0;
    for (unsigned int i = 0; i < (unsigned int)RTYPE_C_MaxType; i++) {
	M_RTD* rtd = M_RTDPtr (i);
	unsigned int cnt = M_RTD_CPDCreateCount (rtd);
	if (cnt > 0) {
	    lines++;
	    IO_printf (
		" %-30s    %6d  %8d    %6d%s\n",
		RTYPE_TypeIdAsString ((RTYPE_Type)i),
		cnt - M_RTD_CPDDeleteCount (rtd),
		cnt,
		rtd->cpdPointerCount (),
		rtd->cpdReuseable () ? " R" : ""
	    );
	    if (lines % 4 == 0)
		IO_printf ("\n");
	}
    }

    CPDAllocator.displayCounts ();

    return self;
}

IOBJ_DefineUnaryMethod (DisplayHandleCountsByRType) {
    IO_printf
	("                                  Handles     Total\n");
    IO_printf
	(" RType                             In Use   Handles\n");
    IO_printf
	(" ------------------------------   -------    ------\n");

    unsigned int lines = 0;
    for (unsigned int i = 0; i < (unsigned int)RTYPE_C_MaxType; i++) {
	M_RTD* rtd = M_RTDPtr (i);
	unsigned int cnt = M_RTD_HandleCreateCount (rtd);
	if (cnt > 0) {
	    lines++;
	    IO_printf (
		" %-30s    %6d  %8d\n",
		RTYPE_TypeIdAsString ((RTYPE_Type)i),
		cnt - M_RTD_HandleDeleteCount (rtd), cnt
	    );
	    if (lines % 4 == 0)
		IO_printf ("\n");
	}
    }

    IO_printf ("\n");

    return self;
}


IOBJ_DefineUnaryMethod (DisplayRTDTable) {
    unsigned int lines = 0;

    IO_printf ("RTD Table:\n");
    IO_printf (
	"RTYPE               CPDPtrcnt gcFn InitFn markFn reuse?\n"
    );

    for (unsigned int i = 0; i < RTYPE_C_MaxType; i++) {
	M_RTD *rtd = M_RTDPtr (i);
	lines++;
	char const *rtype = rtd->RTypeName ();
	if (strlen (rtype) > (size_t)21)
	    IO_printf ("%-s\n                      ", rtype);
	else
	    IO_printf ("%-21s", rtype);

        IO_printf (
	   "  %6d    %c      %c      %c      %c\n",
	    rtd->cpdPointerCount(),
	    M_RTD_ReclaimFn	(rtd)	? 'Y' : ' ',
	    M_RTD_CPDInitFn	(rtd)	? 'Y' : ' ',
	    M_RTD_MarkFn	(rtd)	? 'Y' : ' ',
	    rtd->cpdReuseable	()	? 'Y' : ' '
	);

        if ((lines % 4) == 0)
	    IO_printf ("\n");
    }

    IO_printf ("\n");

    return self;
}


bool M_ASD::DisplaySpaceMallocInfo (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    double *const total = iArgList.arg<double*>();
    double sAllocation = static_cast<double>(m_sTransientAllocation);

    IO_printf ("   Space %2u Containers =\t %14.0f\n", m_xSpace, sAllocation);

    *total += sAllocation;

    return true;
}

IOBJ_DefineUnaryMethod (DisplayMallocInfo) {
    if (UTIL_FreePoolAdaptations || UTIL_FreeListOversizes) {
	IO_printf ("\n Free List Manager Information:\n");
	if (UTIL_FreePoolAdaptations) IO_printf (
	    "   Free List Pool Allocator Adaptations = %u\n", UTIL_FreePoolAdaptations.value ()
	);
	if (UTIL_FreeListOversizes) IO_printf (
	    "   Free List Oversized Allocations      = %u\n", UTIL_FreeListOversizes.value ()
	);
    }

    IO_printf ("\n Malloc Information:\n");
    IO_printf ("   UTIL_TotalFreeList  =\t %14u\n"  , UTIL_FreeListTotal);

    double total = UTIL_FreeListTotal;
    if (UTIL_FreePoolTotal) {
	IO_printf ("   UTIL_FreePoolAvail  =\t %14u\n"  , UTIL_FreePoolTotal
							- UTIL_FreeListTotal
							- UTIL_FreePoolWaste);
	IO_printf ("   UTIL_FreePoolWaste  =\t %14u\n"  , UTIL_FreePoolWaste);
	IO_printf ("   UTIL_FreePoolTotal  =\t %14u\n\n", UTIL_FreePoolTotal);
	total = UTIL_FreePoolTotal;
    }
    else IO_printf ("\n");
    double nonContainerTotal = total;
    IOBJ_ScratchPad->Database ()->EnumerateAccessedSpaces (
	&M_ASD::DisplaySpaceMallocInfo, &total
    );

    IO_printf ("\n   Network Total       =\t %14.0f\n", total);
    IO_printf ("   Federation Total    =\t %14.0f\n", nonContainerTotal + ENVIR_Session ()->CurrentTransientAllocationLevel ());
    IO_printf ("   Session Maximum     =\t %14.0f\n", nonContainerTotal + ENVIR_Session ()->TransientAllocationHighWaterMark ());
    IO_printf ("   Configured Limit    =\t %14llu\n", ThisProcess.memoryAllocationLimit ());
    if (ThisProcess.totalMemoryAllocationHighWaterMark () > 0) {
	IO_printf ("   (Alt) Total         =\t %14llu\n", ThisProcess.totalMemoryAllocation ());
	IO_printf ("   (Alt) Maximum       =\t %14llu\n", ThisProcess.totalMemoryAllocationHighWaterMark ());
    }

    VTransient::DisplayAllocationStatistics ();

    return self;
}


/******************************
 *  Container Access Methods  *
 ******************************/

IOBJ_DefineNewaryMethod (AccessDM) {
    return RTYPE_QRegister (
	IOBJ_ScratchPad->Database ()->AccessASD (
	    IOBJ_NumericIObjectValue (parameterArray[0], int)
	)->GetCPD (
	    IOBJ_NumericIObjectValue (parameterArray[1], int)
	)
    );
}


/********************
 *  Network Update  *
 ********************/

IOBJ_DefineMethod (SwapDM) {
    M_SwapContainers (
	RTYPE_QRegisterCPD (parameterArray [0]),
	RTYPE_QRegisterCPD (parameterArray [1])
    );
    return self;
}

IOBJ_DefineNewaryMethod (CreateSpaceDM) {
    M_CPD *pSpaceRoot = RTYPE_QRegisterCPD (parameterArray[0]);
    IO_printf (
	"Space Creation Status: %s\n", M_UpdateStatusDescription (pSpaceRoot->CreateSpace (), NilOf (char *))
    );

    return IOBJ_IntIObject (RTYPE_QRegisterCPD (parameterArray[0])->spaceIndex ());
}

IOBJ_DefineUnaryMethod (UpdateNetworkDM) {
    IO_printf (
	"Network Update Status: %s\n", M_UpdateStatusDescription (
	    IOBJ_ScratchPad->UpdateNetwork (true), NilOf (char const*)
	)
    );

    return self;
}

IOBJ_DefineNewaryMethod (CmpContainersDM) {
/*****  Returns: true if the containers are equal, false otherwise *****/
    return IOBJ_IntIObject (
	RTYPE_QRegisterCPD (parameterArray[0])->Names (
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    static bool alreadyInitialized = false;
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("displayState"	, DisplayStateDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
	IOBJ_MDE ("displayGCInfo"	, DisplayGCInfoDM)
	IOBJ_MDE ("displayMappingInfo"	, DisplayMappingInfoDM)
	IOBJ_MDE ("displayRetentionInfo", DisplayRetentionInfoDM)
	IOBJ_MDE ("displayTXRetentionInfo"
					, DisplayTXRetentionInfoDM)
	IOBJ_MDE ("displayCPDCountsByRType"
					, DisplayCPDCountsByRType)
	IOBJ_MDE ("displayHandleCountsByRType"
					, DisplayHandleCountsByRType)

	IOBJ_MDE ("displayConstants"	, DisplayConstantsDM)
	IOBJ_MDE ("displayRTDTable"	, DisplayRTDTable)
	IOBJ_MDE ("displayMallocInfo"	, DisplayMallocInfo)
	IOBJ_MDE ("freePoolsDisabled"	, FreePoolsDisabledDM)
	IOBJ_MDE ("preservingCPCCs"	, PreservingCPCCsDM)
	IOBJ_MDE ("segmentScanTrace"	, SegmentScanTraceDM)
	IOBJ_MDE ("sessionGCTrace"	, SessionGCTraceDM)
#if defined(INCLUDING_XDB_REFERENCE_TRACING)
	IOBJ_MDE ("xdbReferenceTrace"	, CrossDBReferenceTraceDM)
#endif
	IOBJ_MDE ("access:cntnr:"	, AccessDM)
	IOBJ_MDE ("swap:with:"		, SwapDM)
	IOBJ_MDE ("createSpace:"	, CreateSpaceDM)
	IOBJ_MDE ("updateNetwork"	, UpdateNetworkDM)
	IOBJ_MDE ("cmp:to:"		, CmpContainersDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (M);
    FAC_FDFCase_FacilityDescription ("The Object Memory Manager");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    FAC_FDFCase_FacilityMD (methodDictionary);

    case FAC_DoStartupInitialization:
    /*****  Check for multiple initialization requests  *****/
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}

    /*****
     *  Initialize the Representation Type Description Data Base, ...
     *****/
	InitializeRTDDB ();

    /*****  Initialize the persistent storage manager, ...  *****/
	PS_Initialize ();

	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  m.c 1 replace /users/mjc/system
  860226 16:23:17 mjc create

 ************************************************************************/
/************************************************************************
  m.c 2 replace /users/mjc/system
  860321 17:53:28 mjc Release 1 of new memory architecture

 ************************************************************************/
/************************************************************************
  m.c 3 replace /users/mjc/system
  860323 17:22:13 mjc Release of additional work on new memory architecture

 ************************************************************************/
/************************************************************************
  m.c 4 replace /users/jad
  860326 19:51:11 jad fixed size bugs in realloc and grow routines

 ************************************************************************/
/************************************************************************
  m.c 5 replace /users/mjc/system
  860327 16:03:29 mjc Added container end markers and some framing tests

 ************************************************************************/
/************************************************************************
  m.c 6 replace /users/mjc/system
  860327 22:33:34 mjc Turned off traces at startup

 ************************************************************************/
/************************************************************************
  m.c 7 replace /users/mjc/system
  860403 11:23:24 mjc Gave framing error its own signal

 ************************************************************************/
/************************************************************************
  m.c 8 replace /users/mjc/system
  860407 11:05:04 mjc Converted 'Display{Expression, Sizeof...} to 'UTIL' versions

 ************************************************************************/
/************************************************************************
  m.c 9 replace /users/mjc/system
  860408 09:33:50 mjc Added CPCC/CPD allocation/deallocation counters

 ************************************************************************/
/************************************************************************
  m.c 10 replace /users/mjc/system
  860408 23:09:43 mjc Added standard CPD allocation capability

 ************************************************************************/
/************************************************************************
  m.c 11 replace /users/mjc/system
  860410 19:17:24 mjc Added standard CPDs, POP testing, and default printer

 ************************************************************************/
/************************************************************************
  m.c 12 replace /users/mjc/system
  860414 12:44:01 mjc Release 'M_ShiftContainerTail', delete 'M_Browser' calls

 ************************************************************************/
/************************************************************************
  m.c 13 replace /users/mjc/system
  860420 15:23:06 mjc Corrected 'M_ShiftContainerTail' bug, arranged for empty pointer arrays not to be created

 ************************************************************************/
/************************************************************************
  m.c 14 replace /users/hjb/system
  860423 00:41:42 hjb added M_AttachCPDPointers, and changed calls to 'StandardCPD' & 'NewSPad..'

 ************************************************************************/
/************************************************************************
  m.c 15 replace /users/mjc/system
  860423 11:04:10 mjc Added insertion zeroing to 'M_ShiftContainerTail' and a true undefined object

 ************************************************************************/
/************************************************************************
  m.c 16 replace /users/hjb/system
  860426 16:14:00 hjb done changing error message for 'M_AttachCPDPointers'

 ************************************************************************/
/************************************************************************
  m.c 17 replace /users/mjc/system
  860501 01:41:08 mjc Converted POP reference decoders to use new 'm.d' macros

 ************************************************************************/
/************************************************************************
  m.c 18 replace /users/jck/system
  860501 08:24:49 jck Modified M_FreeCPD so that a misuse of a CPD pointer
 will more likely result in a catastrophic error rather than a
subtle one

 ************************************************************************/
/************************************************************************
  m.c 19 replace /users/hjb/system
  860505 00:48:33 hjb completed fixing 'AttachCPDPointers'

 ************************************************************************/
/************************************************************************
  m.c 20 replace /users/hjb/system
  860517 19:05:17 hjb implemented the permanent user spaces

 ************************************************************************/
/************************************************************************
  m.c 21 replace /users/mjc/system
  860521 19:36:29 mjc Fixed non-return-of-value bug in 'M_EnableModifications'

 ************************************************************************/
/************************************************************************
  m.c 22 replace /users/mjc/system
  860607 12:14:36 mjc Added 'M_DuplicateCPDPtr' and CPD reference counts

 ************************************************************************/
/************************************************************************
  m.c 23 replace /users/spe/system
  860610 12:41:58 spe Modified M_DuplicateCPD so calls to 'M_ALLOCATECPD' and AttachCPDToContainer are no longer necessary.

 ************************************************************************/
/************************************************************************
  m.c 24 replace /users/mjc/system
  860615 18:59:52 mjc Fixed problems with map access mode and type, eliminated 'UNWIND_LongJumpTrap' uses, converted '_M_POPNil' to the root of OS#1

 ************************************************************************/
/************************************************************************
  m.c 25 replace /users/jck/system
  860619 14:37:31 jck Intermediate release of some bug fixes

 ************************************************************************/
/************************************************************************
  m.c 26 replace /users/jck/system
  860619 16:19:16 jck Object Space Segment Preamble Type Added

 ************************************************************************/
/************************************************************************
  m.c 27 replace /users/jck/system
  860711 16:55:23 jck Release Object Saver

 ************************************************************************/
/************************************************************************
  m.c 28 replace /users/jck/system
  860715 09:22:40 jck First Pass optimization of CPD allocation

 ************************************************************************/
/************************************************************************
  m.c 29 replace /users/jck/system
  860725 17:27:36 jck fixed some bugs

 ************************************************************************/
/************************************************************************
  m.c 30 replace /users/jad/system
  860731 17:47:06 jad added DBUPDATE utilities

 ************************************************************************/
/************************************************************************
  m.c 31 replace /users/mjc/system
  860802 12:03:48 mjc Fixed 'save' bug

 ************************************************************************/
/************************************************************************
  m.c 32 replace /users/mjc/system
  860803 00:16:56 mjc Reduced required permission for file open, reduced permission for new files and directories

 ************************************************************************/
/************************************************************************
  m.c 33 replace /users/jad/system
  860806 16:14:34 jad added Malloc Info Debug Method

 ************************************************************************/
/************************************************************************
  m.c 34 replace /users/mjc/rsystem
  860813 18:22:58 mjc Implemented statistics query/display, simple optimization of CPD allocator

 ************************************************************************/
/************************************************************************
  m.c 35 replace /users/mjc/system
  860817 20:17:57 mjc New, improved CPD allocators

 ************************************************************************/
/************************************************************************
  m.c 36 replace /users/mjc/system
  860819 12:31:35 mjc Added experimental fast preamble pointer access routine

 ************************************************************************/
/************************************************************************
  m.c 37 replace /users/mjc/system
  860820 21:37:18 mjc 'Streamlined' free, fixed CPD rewrite induced bug in mapping table error recovery

 ************************************************************************/
/************************************************************************
  m.c 38 replace /users/mjc/system
  860821 12:35:42 mjc Fixed network integrity check to restrict its test to saved containers only

 ************************************************************************/
/************************************************************************
  m.c 39 replace /users/jad/system
  860822 14:22:05 jad added Free Instance Counters

 ************************************************************************/
/************************************************************************
  m.c 40 replace /users/jad/system
  860822 19:56:29 jad added a displayRTDTable method and a std ptr cnt to
the displayCPDCountByRType method

 ************************************************************************/
/************************************************************************
  m.c 41 replace /users/jck/system
  860826 08:13:20 jck Bug fixed in GrowContainerTables

 ************************************************************************/
/************************************************************************
  m.c 42 replace /users/jad/system
  860826 14:41:47 jad changed CPD's and their pointer array to be in one
piece of memory

 ************************************************************************/
/************************************************************************
  m.c 43 replace /users/jad/system
  860828 11:59:29 jad fix the cpd allocation sizes

 ************************************************************************/
/************************************************************************
  m.c 44 replace /users/jad/system
  860903 12:54:51 jad made M_NewSpadContainer only invoke the initialization
routine if its not nil

 ************************************************************************/
/************************************************************************
  m.c 45 replace /users/jad/system
  860903 19:51:26 jad temporary fix for realloc bug

 ************************************************************************/
/************************************************************************
  m.c 46 replace /users/jad/system/test
  860904 13:48:52 jad fixed bug in cpd allocation

 ************************************************************************/
/************************************************************************
  m.c 47 replace /users/jad/system
  860914 11:15:11 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  m.c 48 replace /users/mjc/system
  861002 18:03:04 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  m.c 49 replace /users/mjc/mysystem
  861008 18:12:55 jad added cpd debugging information

 ************************************************************************/
/************************************************************************
  m.c 50 replace /users/jad/system
  861010 14:06:25 jad added a field to the RTD table.  The RTD Table now
contains total cpd allocation and deallocation counts.

 ************************************************************************/
/************************************************************************
  m.c 51 replace /users/mjc/system
  861015 23:54:12 lcn Eliminated hysteresis in CPD deallocator

 ************************************************************************/
/************************************************************************
  m.c 52 replace /users/mjc/system
  861112 09:34:36 mjc Implement garbage collector lock for CPDs

 ************************************************************************/
/************************************************************************
  m.c 53 replace /users/mjc/system
  861112 18:40:17 mjc Added usage counters to major operations

 ************************************************************************/
/************************************************************************
  m.c 54 replace /users/mjc/system
  861125 17:32:23 mjc Added metric to count '(cpd,-1)' CPD allocation calls

 ************************************************************************/
/************************************************************************
  m.c 55 replace /users/mjc/system
  870215 23:05:02 lcn Release of new memory manager

 ************************************************************************/
/************************************************************************
  m.c 56 replace /users/mjc/system
  870218 09:55:36 mjc Corrected omission of old container disposal code in 'RemapContainer'

 ************************************************************************/
/************************************************************************
  m.c 57 replace /users/mjc/system
  870326 19:27:48 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  m.c 58 replace /users/jck/system
  870415 09:54:40 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  m.c 59 replace /users/mjc/translation
  870524 09:39:37 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  m.c 60 replace /users/mjc/system
  870607 03:15:01 mjc Fixed VAX compilation errors

 ************************************************************************/
/************************************************************************
  m.c 61 replace /users/mjc/system
  870614 20:04:32 mjc Convert UNIX specific include files to 'stdoss.h'

 ************************************************************************/
/************************************************************************
  m.c 62 replace /users/jck/system
  870731 07:51:36 jck Added a container swapping debugger method

 ************************************************************************/
/************************************************************************
  m.c 63 replace /users/jck/system
  870810 14:14:55 jck Added counter to keep track of amount of space used by free lists

 ************************************************************************/
/************************************************************************
  m.c 64 replace /users/mjc/system
  870811 14:42:36 mjc Added 'displayMalloc{Regions,Blocks,BlockSummary}' debugger methods

 ************************************************************************/
/************************************************************************
  m.c 65 replace /users/jck/system
  871007 13:39:42 jck Release implementation of Global Persistent Garbage Collector

 ************************************************************************/
/************************************************************************
  m.c 66 replace /users/mjc/Software/system
  871123 20:10:40 mjc Added experimental CPCC allocation routine and '#M displayMappingInfo' debug method

 ************************************************************************/
/************************************************************************
  m.c 67 replace /users/jck/system
  871210 12:35:40 jck Shortened identifier 'EnumerateSpacesInAttachedNetwork' so that VAX-C would stop complaining

 ************************************************************************/
/************************************************************************
  m.c 68 replace /users/jad/system
  880203 12:06:52 jad added 'cmp:to:' debug method

 ************************************************************************/
/************************************************************************
  m.c 69 replace /users/jck/system
  880428 10:37:20 jck Initialized a variable for Apollo's and VAX's benefit

 ************************************************************************/
/************************************************************************
  m.c 70 replace /users/jck/system
  880902 09:00:19 jck Corrected an initialization omission in the creation of the transient portion of the container table

 ************************************************************************/
/************************************************************************
  m.c 71 replace /users/jck/system
  890222 14:49:40 jck Adding a consistency checker function to M's rtype descriptors

 ************************************************************************/
/************************************************************************
  m.c 72 replace /users/m2/backend
  890717 09:37:43 jck Initialized a CTE field used exclusively by the global garbage collector

 ************************************************************************/
/************************************************************************
  m.c 73 replace /users/jck/system
  891012 11:22:04 jck Updated RTD display to include latest items

 ************************************************************************/
