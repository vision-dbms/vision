#ifndef M_CPD_Interface
#define M_CPD_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "VkLongUnsigneds.h"

#include "VContainerHandle.h"

/*****************************************
 *****  Reference Monitoring Macros  *****
 *****************************************/

#if defined(_DEBUG)
#define ASSERT_LIVENESS(pCPD, xExceptionCase) pCPD->AssertLiveness (xExceptionCase)

#else
#define ASSERT_LIVENESS(pCPD, xExceptionCase)

#endif


/*************************
 *****  Definitions  *****
 *************************/
/*---------------------------------------------------------------------------
 * Access to all physical state in the system is made via a structure called a
 * container pointer descriptor (CPD).  A CPD is associated with a container
 * and identifies a set of physical memory pointers to the container.  CPD's
 * are allocated from an internal pool.  The holder of a CPD is free to modify
 * the pointers identified by the CPD at will.  The pointers held in a CPD are
 * adjusted as the container is moved.  To insure that this updates actually
 * take place, pointers contained in a CPD MUST be passed by REFERENCE.
 *
 * CPD Field Descriptions:
 *	m_pNext			- the address of the next CPD in the chain of
 *				  CPD's attached to this container.
 *	m_pPrev			- the address of the previous CPD in the chain
 *				  of CPD's attached to this container.
 *	m_pContainerHandle	- the address of the CPCC connecting this CPD
 *				  to a container table entry.
 *	m_fGCLocked		- a boolean which, when true, protects this
 *				  CPD from garbage collection.
 *	m_iReferenceCount	- the number of references to this CPD.
 *	m_pContainerPreamble	- the address of this container's preamble.
 *				  The address of the first byte of data in the
 *				  container is always (pointer_t)(m_pContainerPreamble+1).
 *	m_iPointerArray		- the address of an array of addresses pointing
 *				  to the container.
 *---------------------------------------------------------------------------
 */


/*******************
 *  Access Macros  *
 *******************/

/*****  CPD Components  *****/
#define M_CPD_NLink(cpd)		((cpd)->m_pNext)
#define M_CPD_PLink(cpd)		((cpd)->m_pPrev)
#define M_CPD_CPCC(cpd)			((cpd)->m_pContainerHandle)
#define M_CPD_GCLock(cpd)		((cpd)->m_fGCLocked)
#define M_CPD_ReferenceCount(cpd)	((cpd)->m_iReferenceCount)
#define M_CPD_PreamblePtr(cpd)		((cpd)->m_pContainerPreamble)
#define M_CPD_Pointers(cpd)		((cpd)->m_iPointerArray)

/*****  Container Components  *****/
#define M_CPD_ContainerBase(cpd) M_CPreamble_ContainerBase (\
    M_CPD_PreamblePtr (cpd)\
)

#define M_CPD_ContainerBaseAsType(cpd,type) M_CPreamble_ContainerBaseAsType(\
    M_CPD_PreamblePtr (cpd), type\
)

#define M_CPD_Size(cpd) M_CPreamble_Size (M_CPD_PreamblePtr (cpd))

#define M_CPD_RType(cpd) M_CPreamble_RType (M_CPD_PreamblePtr (cpd))

#define M_CPD_POP(cpd) M_CPreamble_POP (M_CPD_PreamblePtr (cpd))

#define M_CPD_PointerToType(cpd,xPointer,type) (\
    *(type*)&M_CPD_PointerToAny(cpd,xPointer)\
)
#define M_CPD_PointerToAny(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asAny\
)
#define M_CPD_PointerToByte(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asByte\
)
#define M_CPD_PointerToCharacter(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asCharacter\
)
#define M_CPD_PointerToCardinal(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asCardinal\
)
#define M_CPD_PointerToInteger(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asInteger\
)
#define M_CPD_PointerToUnsigned64(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asUnsigned64\
)
#define M_CPD_PointerToUnsigned96(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asUnsigned96\
)
#define M_CPD_PointerToUnsigned128(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asUnsigned128\
)
#define M_CPD_PointerToSmallReal(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asSmallReal\
)
#define M_CPD_PointerToReal(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asReal\
)
#define M_CPD_PointerToPOP(cpd,xPointer) (\
    M_CPD_Pointers(cpd)[xPointer].asPOP\
)


/**************************************
 *  CPD Garbage Collection Utilities  *
 **************************************/

/*---------------------------------------------------------------------------
 *****  Macro to copy a pointer to a CPD in a known (i.e., reference count
 *****	maintaining) way.
 *
 *  Arguments:
 *	cpd			- the address of the CPD whose reference
 *				  count is to be adjusted.
 *
 *  Syntactic Context:
 *	Expression
 *
 *  Returns:
 *	'cpd'	    ... 'M_DuplicateCPDPtr'
 *
 *  Notes:
 *	This macro should become obsolete given that it cannot be written to
 *	evaluate its parameter EXACTLY once and still return it.  The natural
 *	implementation of this macro as:
 *
 *		(M_CPD_IncrementReferenceCount (TEMP = cpd), TEMP)
 *
 *	is improperly optimized when this macro for more than one argument in
 *	the argument list of a function.
 *
 *****/
#define M_DuplicateCPDPtr(cpd) ((cpd)->retain (), (cpd))


/****************************
 *****  Testing Macros  *****
 ****************************/

/****************************************
 *  CPD POP Reference Decoding Macros   *
 ****************************************/
/*---------------------------------------------------------------------------
 * The following macros decode a POP reference (a la 'M_Get...CPD') into a
 * POP pointer (M_POP*) and a POP (M_POP) respectively.
 *---------------------------------------------------------------------------
 */
#define M_CPD_POPReferenceToPtr(cpd, index)\
    (((index) < 0) ? &M_CPD_POP (cpd) : M_CPD_PointerToPOP (cpd, index))

/***************************************************
 *  Direct Pointer Based Magnitude Testing Macros  *
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Macro to determine the relative magnitude of two POPs.
 *
 *  Arguments:
 *	pop1/pop2		- addresses of the POPs to be compared.
 *
 *  Result:
 *	comparisonResultVar	- negative if 'pop1 < pop2', 0 if 'pop1 == pop2',
 *				  and positive value if 'pop1 > pop2'.
 *
 *****/
#define M_ComparePOPs(pop1, pop2) (M_POP_D_AsInt (pop1) - M_POP_D_AsInt (pop2))


/**********************
 *  Class Definition  *
 **********************/

class M_CPD {
//  Container Processing Callbacks
public:
    typedef VContainerHandle::ShiftProcessor ShiftProcessor;

    typedef void (*UVShiftProcessor) (
	M_CPD *cpd, unsigned int xPointer, ptrdiff_t sShift, va_list ap
    );

//  Reference Count Exception Generation
protected:
    enum ReferenceCountExceptionCase {
	Attach, Reclaim, POPClearTarget, POPCopyTarget, POPCopySource
    };

    void RaiseReferenceCountException (ReferenceCountExceptionCase xExceptionCase) const;

    void AssertLiveness (ReferenceCountExceptionCase xExceptionCase) const;

//  Reference Management
private:
    void reclaim ();

public:
    void discard () {
	if (0 == m_iReferenceCount)
	    reclaim ();
    }
    void release () {
	if (0 == --m_iReferenceCount)
	    reclaim ();
    }
    void retain () {
	m_iReferenceCount++;
    }
    void retain (unsigned int iIncrement) {
	m_iReferenceCount += iIncrement;
    }
    void untain () {
	m_iReferenceCount--;
    }

//  Access
public:
    unsigned int attentionMask () const {
	return m_pContainerHandle->attentionMask ();
    }

    VContainerHandle *containerHandle () const {
	return m_pContainerHandle;
    }
    unsigned int ContainerIndex () const {
	return m_pContainerHandle->ContainerIndex ();
    }
    RTYPE_Type ContainerRType () const {
	return (RTYPE_Type)M_CPreamble_RType (m_pContainerPreamble);
    }

    M_AND *Database () const {
	return m_pContainerHandle->Database ();
    }

    M_KnownObjectTable *KnownObjectTable () const {
	return m_pContainerHandle->KnownObjectTable ();
    }
    M_KOT *KOT () const {
	return m_pContainerHandle->KOT ();
    }

    unsigned int PointerCount () const {
	return m_pContainerHandle->CPDPointerCount ();
    }

    unsigned int ReferenceCount () const {
	return m_iReferenceCount;
    }

    M_RTD *RTD () const {
	return m_pContainerHandle->RTD ();
    }
    RTYPE_Type RType () const {
	return (RTYPE_Type)M_CPreamble_RType (m_pContainerPreamble);
    }
    char const *RTypeName () const {
	return m_pContainerHandle->RTypeName ();
    }

    M_ASD *ScratchPad () const {
	return m_pContainerHandle->ScratchPad ();
    }

    M_ASD *Space () const {
	return m_pContainerHandle->Space ();
    }
    unsigned int SpaceIndex () const {
	return m_pContainerHandle->SpaceIndex ();
    }

    M_POP const &TheNilPOP () const {
	return m_pContainerHandle->TheNilPOP ();
    }

    transientx_t* TransientExtension () const {
	return m_pContainerHandle->TransientExtension ();
    }
    transientx_t* TransientExtensionIfA (VRunTimeType const& rRTT) const {
	return m_pContainerHandle->TransientExtensionIfA (rRTT);
    }

//  Query
public:
    bool ContainerHasMultipleUses () const {
	return m_iReferenceCount > 1
	    || IsntNil (m_pNext)
	    || IsntNil (m_pPrev)
	    || m_pContainerHandle->ContainerHasMultipleUses ();
    }

    bool HasATransientExtension () const {
	return m_pContainerHandle->HasATransientExtension ();
    }

    bool isAForwardingTarget () const {
	return m_pContainerHandle->isAForwardingTarget ();
    }
    bool isATRefRescanTarget () const {
	return m_pContainerHandle->isATRefRescanTarget ();
    }

    bool IsInTheScratchPad () const {
	return m_pContainerHandle->IsInTheScratchPad ();
    }
    bool IsntInTheScratchPad () const {
	return m_pContainerHandle->IsntInTheScratchPad ();
    }

    bool IsReadOnly () const {
	return m_pContainerHandle->IsReadOnly ();
    }
    bool IsReadWrite () const {
	return m_pContainerHandle->IsReadWrite ();
    }

    bool RTypeIs (RTYPE_Type xType) const {
	return RType () == xType;
    }
    bool RTypeIsnt (RTYPE_Type xType) const {
	return RType () != xType;
    }

    bool TransientExtensionIsA (VRunTimeType const& rRTT) const {
	return m_pContainerHandle->TransientExtensionIsA (rRTT);
    }

//  Consistency Checking
public:
    void CheckConsistency () const {
	m_pContainerHandle->CheckConsistency ();
    }

//  Display
public:
    void print () const;

//  Control
public:
    void setAttentionMaskTo (unsigned int iNewAttentionMask) {
	m_pContainerHandle->setAttentionMaskTo (iNewAttentionMask);
    }

    void SetGCLock () {
	m_fGCLocked = true;
    }
    void ClearGCLock () {
	m_fGCLocked = false;
    }

//  Migration
public:
    void ForwardToSpace (M_ASD *pSpace) const {
	m_pContainerHandle->ForwardToSpace (pSpace);
    }
    void ForwardToSpace (VContainerHandle *pHandle) const {
	m_pContainerHandle->ForwardToSpace (pHandle->Space ());
    }
    void ForwardToSpace (M_CPD *pCPD) const {
	m_pContainerHandle->ForwardToSpace (pCPD->Space ());
    }

//  Container Access
protected:
    M_POP *POP (unsigned int xPointer) const {
	return M_CPD_PointerToPOP (this, xPointer);
    }

public:
    M_CPreamble* GetContainerAddress (int xPointer) {
	return xPointer < 0
	    ? m_pContainerPreamble
	    : m_pContainerHandle->GetContainerAddress (POP (xPointer));
    }
    M_CPreamble* GetContainerAddress (unsigned int xPointer) {
	return m_pContainerHandle->GetContainerAddress (POP (xPointer));
    }

    VContainerHandle *GetContainerHandle (int xPointer, RTYPE_Type xExpectedType = RTYPE_C_Any) {
	return xPointer < 0 ? m_pContainerHandle : m_pContainerHandle->GetContainerHandle (
	    POP (xPointer), xExpectedType
	);
    }
    VContainerHandle *GetContainerHandle (unsigned int xPointer, RTYPE_Type xExpectedType = RTYPE_C_Any) {
	return m_pContainerHandle->GetContainerHandle (POP (xPointer), xExpectedType);
    }

    M_CPD *GetCPD (int xPointer, RTYPE_Type xExpectedType = RTYPE_C_Any) {
	ASSERT_LIVENESS (this, Attach);
	return xPointer < 0
	    ? m_pContainerHandle->GetCPD ()
	    : m_pContainerHandle->GetCPD (POP (xPointer), xExpectedType);
    }
    M_CPD *GetCPD (unsigned int xPointer, RTYPE_Type xExpectedType = RTYPE_C_Any) {
	ASSERT_LIVENESS (this, Attach);
	return m_pContainerHandle->GetCPD (POP (xPointer), xExpectedType);
    }
    M_CPD *GetCPD ();

//  Container Creation
public:
    M_CPD *CreateContainer (RTYPE_Type xRType, size_t sContainer) const {
	return m_pContainerHandle->CreateContainer (xRType, sContainer);
    }

//  Container Modification
public:
    void EnableModifications () {
	m_pContainerHandle->EnableModifications ();
    }
    void GrowContainer (ptrdiff_t sizeChange) {
	m_pContainerHandle->GrowContainer (sizeChange);
    }
    void ReallocateContainer (size_t newSize) {
	m_pContainerHandle->ReallocateContainer (newSize);
    }

    void VShiftContainerTail (
	unsigned int	xPointer,
	size_t		sTail,
	ptrdiff_t	sShift,
	bool		bSqueeze,
	ShiftProcessor	pRegionProcessor = 0,
	va_list		pRegionProcessorArguments = 0
    ) {
	EnableModifications ();
	m_pContainerHandle->VShiftContainerTail (
	    M_CPD_PointerToCharacter (this, xPointer),
	    sTail, sShift, bSqueeze, pRegionProcessor, pRegionProcessorArguments
	);
    }

    void __cdecl ShiftContainerTail (
	unsigned int	xPointer,
	size_t		sTail,
	ptrdiff_t	sShift,
	bool		bSqueeze,
	ShiftProcessor	pRegionProcessor,
	...
    );
    void ShiftContainerTail (unsigned int xPointer, size_t sTail, ptrdiff_t sShift, bool bSqueeze) {
	VShiftContainerTail (xPointer, sTail, sShift, bSqueeze);
    }

//  Reference Access
public:
    M_KOT *ReferencedKOT (M_POP const *pReference) const {
	return m_pContainerHandle->ReferencedKOT (pReference);
    }
    M_KOT *ReferencedKOT (unsigned int xReference) const {
	return m_pContainerHandle->ReferencedKOT (POP (xReference));
    }
    M_KOT *ReferencedKOT (int xReference) const {
	return xReference < 0
	    ? m_pContainerHandle->KOT ()
	    : m_pContainerHandle->ReferencedKOT (POP (xReference));
    }

    M_AND *ReferencedNetwork (M_POP const *pReference) const {
	return m_pContainerHandle->ReferencedNetwork (pReference);
    }
    M_AND *ReferencedNetwork (unsigned int xReference) const {
	return m_pContainerHandle->ReferencedNetwork (POP (xReference));
    }
    M_AND *ReferencedNetwork (int xReference) const {
	return xReference < 0
	    ? m_pContainerHandle->Database ()
	    : m_pContainerHandle->ReferencedNetwork (POP (xReference));
    }

//  Reference Testing
public:
    /****************
     *----  ==  ----*
     ****************/

    bool Names (M_KOTM pKOTM) const {
	return m_pContainerHandle->Names (pKOTM);
    }
    bool NamesPToken (M_KOTM pKOTM) const {
	return m_pContainerHandle->NamesPToken (pKOTM);
    }

    bool Names (VContainerHandle const *pThat) const {
	return pThat->Names (m_pContainerHandle);
    }
    bool Names (M_CPD const *pThat) const {
	return this == pThat || pThat->Names (m_pContainerHandle);
    }
    bool Names (
	VContainerHandle const *pThat, M_POP const* pThatReference
    ) const {
	return pThat->ReferenceNames (pThatReference, m_pContainerHandle);
    }

    bool ReferencesAreIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pContainerHandle->ReferencesAreIdentical (pReference1, pReference2);
    }

    bool ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceNames (pReference, pKOTM);
    }
    bool ReferenceNames (unsigned int xReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceNames (POP (xReference), pKOTM);
    }
    bool ReferenceNames (int xReference, M_KOTM pKOTM) const {
	return xReference < 0 ? m_pContainerHandle->Names (
	    pKOTM
	) : m_pContainerHandle->ReferenceNames (POP (xReference), pKOTM);
    }

    bool ReferenceNamesPToken (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceNamesPToken (pReference, pKOTM);
    }
    bool ReferenceNamesPToken (unsigned int xReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceNamesPToken (POP (xReference), pKOTM);
    }
    bool ReferenceNamesPToken (int xReference, M_KOTM pKOTM) const {
	return xReference < 0 ? m_pContainerHandle->Names (
	    pKOTM
	) : m_pContainerHandle->ReferenceNamesPToken (POP (xReference), pKOTM);
    }

    bool ReferenceNames (
	M_POP const *pReference, VContainerHandle const *pThat
    ) const {
	return m_pContainerHandle->ReferenceNames (pReference, pThat);
    }
    bool ReferenceNames (M_POP const *pReference, M_CPD const *pThat) const {
	return pThat->Names (m_pContainerHandle, pReference);
    }
    bool ReferenceNames (unsigned int xReference, VContainerHandle const *pThat) const {
	return m_pContainerHandle->ReferenceNames (POP (xReference), pThat);
    }
    bool ReferenceNames (int xReference, VContainerHandle const *pThat) const {
	return xReference < 0  ? m_pContainerHandle->Names (
	    pThat
	) : m_pContainerHandle->ReferenceNames (POP (xReference), pThat);
    }
    bool ReferenceNames (unsigned int xReference, M_CPD const *pThat) const {
	return pThat->Names (m_pContainerHandle, POP (xReference));
    }
    bool ReferenceNames (int xReference, M_CPD const *pThat) const {
	return xReference < 0 ? pThat->Names (m_pContainerHandle) : pThat->Names (
	    m_pContainerHandle, POP (xReference)
	);
    }

    bool ReferenceNames (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pContainerHandle->ReferenceNames (pReference, pThat, pThatReference);
    }
    bool ReferenceNames (
	unsigned int xReference, VContainerHandle const *pThat, M_POP const *pThatReference
    ) const {
	return pThat->ReferenceNames (pThatReference, m_pContainerHandle, POP (xReference));
    }
    bool ReferenceNames (
	int xReference, VContainerHandle const *pThat, M_POP const *pThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceNames (
	    pThatReference, m_pContainerHandle
	) : pThat->ReferenceNames (
	    pThatReference, m_pContainerHandle, POP (xReference)
	);
    }
    bool ReferenceNames (
	M_POP const *pReference, M_CPD const *pThat, int xThatReference
    ) const {
	return pThat->ReferenceNames (xThatReference, m_pContainerHandle, pReference);
    }
    bool ReferenceNames (
	unsigned int xReference, M_CPD const *pThat, int xThatReference
    ) const {
	return pThat->ReferenceNames (xThatReference, m_pContainerHandle, POP (xReference));
    }
    bool ReferenceNames (
	unsigned int xReference, M_CPD const *pThat, unsigned int xThatReference
    ) const {
	return pThat->ReferenceNames (xThatReference, m_pContainerHandle, POP (xReference));
    }
    bool ReferenceNames (
	int xReference, M_CPD const *pThat, unsigned int xThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceNames (
	    xThatReference, m_pContainerHandle
	) : pThat->ReferenceNames (
	    xThatReference, m_pContainerHandle, POP (xReference)
	);
    }
    bool ReferenceNames (
	int xReference, M_CPD const *pThat, int xThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceNames (
	    xThatReference, m_pContainerHandle
	) : pThat->ReferenceNames (
	    xThatReference, m_pContainerHandle, POP (xReference)
	);
    }

    bool ReferenceIsNil (M_POP const* pReference) const {
	return m_pContainerHandle->ReferenceIsNil (pReference);
    }
    bool ReferenceIsNil (unsigned int xReference) const {
	return ReferenceIsNil (POP (xReference));
    }

    /*****************
     *----  !==  ----*
     *****************/

    bool DoesntName (M_KOTM pKOTM) const {
	return m_pContainerHandle->DoesntName (pKOTM);
    }
    bool DoesntNamePToken (M_KOTM pKOTM) const {
	return m_pContainerHandle->DoesntNamePToken (pKOTM);
    }

    bool DoesntName (VContainerHandle const *pThat) const {
	return pThat->DoesntName (m_pContainerHandle);
    }
    bool DoesntName (M_CPD const *pThat) const {
	return this != pThat && pThat->DoesntName (m_pContainerHandle);
    }
    bool DoesntName (
	VContainerHandle const *pThat, M_POP const* pThatReference
    ) const {
	return pThat->ReferenceDoesntName (pThatReference, m_pContainerHandle);
    }

    bool ReferencesArentIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pContainerHandle->ReferencesArentIdentical (pReference1, pReference2);
    }

    bool ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceDoesntName (pReference, pKOTM);
    }
    bool ReferenceDoesntName (unsigned int xReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceDoesntName (POP (xReference), pKOTM);
    }
    bool ReferenceDoesntName (int xReference, M_KOTM pKOTM) const {
	return xReference < 0 ? m_pContainerHandle->Names (
	    pKOTM
	) : m_pContainerHandle->ReferenceDoesntName (POP (xReference), pKOTM);
    }

    bool ReferenceDoesntNamePToken (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceDoesntNamePToken (pReference, pKOTM);
    }
    bool ReferenceDoesntNamePToken (unsigned int xReference, M_KOTM pKOTM) const {
	return m_pContainerHandle->ReferenceDoesntNamePToken (POP (xReference), pKOTM);
    }
    bool ReferenceDoesntNamePToken (int xReference, M_KOTM pKOTM) const {
	return xReference < 0 ? m_pContainerHandle->Names (
	    pKOTM
	) : m_pContainerHandle->ReferenceDoesntNamePToken (POP (xReference), pKOTM);
    }

    bool ReferenceDoesntName (
	M_POP const *pReference, VContainerHandle const *pThat
    ) const {
	return m_pContainerHandle->ReferenceDoesntName (pReference, pThat);
    }
    bool ReferenceDoesntName (M_POP const *pReference, M_CPD const *pThat) const {
	return pThat->DoesntName (m_pContainerHandle, pReference);
    }
    bool ReferenceDoesntName (unsigned int xReference, VContainerHandle const *pThat) const {
	return m_pContainerHandle->ReferenceDoesntName (POP (xReference), pThat);
    }
    bool ReferenceDoesntName (int xReference, VContainerHandle const *pThat) const {
	return xReference < 0  ? m_pContainerHandle->DoesntName (
	    pThat
	) : m_pContainerHandle->ReferenceDoesntName (POP (xReference), pThat);
    }
    bool ReferenceDoesntName (unsigned int xReference, M_CPD const *pThat) const {
	return pThat->DoesntName (m_pContainerHandle, POP (xReference));
    }
    bool ReferenceDoesntName (int xReference, M_CPD const *pThat) const {
	return xReference < 0 ? pThat->DoesntName (m_pContainerHandle) : pThat->DoesntName (
	    m_pContainerHandle, POP (xReference)
	);
    }

    bool ReferenceDoesntName (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pContainerHandle->ReferenceDoesntName (pReference, pThat, pThatReference);
    }
    bool ReferenceDoesntName (
	unsigned int xReference, VContainerHandle const *pThat, M_POP const *pThatReference
    ) const {
	return pThat->ReferenceDoesntName (pThatReference, m_pContainerHandle, POP(xReference));
    }
    bool ReferenceDoesntName (
	int xReference, VContainerHandle const *pThat, M_POP const *pThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceDoesntName (
	    pThatReference, m_pContainerHandle
	) : pThat->ReferenceDoesntName (
	    pThatReference, m_pContainerHandle, POP (xReference)
	);
    }
    bool ReferenceDoesntName (
	M_POP const *pReference, M_CPD const *pThat, int xThatReference
    ) const {
	return pThat->ReferenceDoesntName (xThatReference, m_pContainerHandle, pReference);
    }
    bool ReferenceDoesntName (
	unsigned int xReference, M_CPD const *pThat, int xThatReference
    ) const {
	return pThat->ReferenceDoesntName (xThatReference, m_pContainerHandle, POP (xReference));
    }
    bool ReferenceDoesntName (
	unsigned int xReference, M_CPD const *pThat, unsigned int xThatReference
    ) const {
	return pThat->ReferenceDoesntName (xThatReference, m_pContainerHandle, POP (xReference));
    }
    bool ReferenceDoesntName (
	int xReference, M_CPD const *pThat, unsigned int xThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceDoesntName (
	    xThatReference, m_pContainerHandle
	) : pThat->ReferenceDoesntName (
	    xThatReference, m_pContainerHandle, POP (xReference)
	);
    }
    bool ReferenceDoesntName (
	int xReference, M_CPD const *pThat, int xThatReference
    ) const {
	return xReference < 0 ? pThat->ReferenceDoesntName (
	    xThatReference, m_pContainerHandle
	) : pThat->ReferenceDoesntName (
	    xThatReference, m_pContainerHandle, POP (xReference)
	);
    }

    bool ReferenceIsntNil (M_POP const* pReference) const {
	return m_pContainerHandle->ReferenceIsntNil (pReference);
    }
    bool ReferenceIsntNil (unsigned int xReference) const {
	return ReferenceIsntNil (POP (xReference));
    }

//  Reference Management
public:
    //  The following methods work with writeable containers only.
    void constructReference (int xPointer) {
	m_pContainerHandle->constructReference (POP (xPointer));
    }
    void constructReferences (int xPointer, unsigned int sPOPArray) {
	m_pContainerHandle->constructReferences (POP (xPointer), sPOPArray);
    }

    //  The following methods work with both read-only and writeable containers.
    void ClearReference	 (unsigned int xPointer);
    void ClearReferences (unsigned int xPointer, unsigned int sPOPArray);

    /*----------------------------------------------------------------------------
     *  The 'Locate*Name*' routines defined below return true when the local
     *  name already exists, and false when it does not.  As their names suggest,
     *  the '*OrAdd*' variants will update the appropriate federation and return 
     *  the requested local name if it does not already exist.
     *----------------------------------------------------------------------------
     */
    bool LocateNameIn (M_AND *pThat, M_TOP &rName) const {
	return m_pContainerHandle->LocateNameIn (pThat, rName);
    }

    bool LocateNameIn (M_ASD *pThat, M_TOP &rName) const {
	return m_pContainerHandle->LocateNameIn (pThat, rName);
    }

    bool LocateNameIn (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pContainerHandle, rName);
    }
    bool LocateNameOf (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateNameIn (m_pContainerHandle, rName);
    }

    bool LocateNameIn (M_CPD const *pThat, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pContainerHandle, rName);
    }
    bool LocateNameOf (M_CPD const *pThat, M_TOP &rName) const {
	return pThat->LocateNameIn (m_pContainerHandle, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateNameIn (M_AND *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return m_pContainerHandle->LocateNameIn (pThat, pThisName, rName);
    }

    bool LocateNameIn (M_ASD *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return m_pContainerHandle->LocateNameIn (pThat, pThisName, rName);
    }

    bool LocateNameIn (
	VContainerHandle const *pThatSpace, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThatSpace->LocateNameOf (m_pContainerHandle, pThisName, rName);
    }
    bool LocateNameOf (
	VContainerHandle const *pThatSpace, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThatSpace->LocateNameIn (m_pContainerHandle, pThatName, rName);
    }

    bool LocateNameIn (
	M_CPD const *pThatSpace, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThatSpace->LocateNameOf (m_pContainerHandle, pThisName, rName);
    }
    bool LocateNameOf (
	M_CPD const *pThatSpace, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThatSpace->LocateNameIn (m_pContainerHandle, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (M_AND *pThat, M_TOP &rName) const {
	return m_pContainerHandle->LocateOrAddNameIn (pThat, rName);
    }
    bool LocateOrAddNameIn (M_ASD *pThat, M_TOP &rName) const {
	return m_pContainerHandle->LocateOrAddNameIn (pThat, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (M_AND *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return m_pContainerHandle->LocateOrAddNameIn (pThat, pThisName, rName);
    }
    bool LocateOrAddNameIn (M_ASD *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return m_pContainerHandle->LocateOrAddNameIn (pThat, pThisName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pContainerHandle, rName);
    }
    bool LocateOrAddNameOf (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameIn (m_pContainerHandle, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (M_CPD const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pContainerHandle, rName);
    }
    bool LocateOrAddNameOf (M_CPD const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameIn (m_pContainerHandle, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameOf (
	VContainerHandle const *pThatSpace, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThatSpace->LocateOrAddNameIn (m_pContainerHandle, pThatName, rName);
    }
    bool LocateOrAddNameIn (
	VContainerHandle const *pThatSpace, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThatSpace->LocateOrAddNameOf (m_pContainerHandle, pThisName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (
	M_CPD const *pThatSpace, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThatSpace->LocateOrAddNameOf (m_pContainerHandle, pThisName, rName);
    }
    bool LocateOrAddNameOf (
	M_CPD const *pThatSpace, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThatSpace->LocateOrAddNameIn (m_pContainerHandle, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    M_CPD *StoreReference (unsigned int xDst, VContainerHandle const *pThat);
    M_CPD *StoreReference (unsigned int xDst, M_CPD const *sourceCPD) {
	return StoreReference (xDst, sourceCPD->containerHandle ());
    }
    M_CPD *StoreReference (unsigned int xDst, M_CPD const *sourceCPD, int xSrc);
    M_CPD *StoreReference (
	unsigned int xDst, VContainerHandle const *pThat, M_POP const *pThatReference
    );

//  Known Object Access
public:
    M_KOTE const &GetBlockEquivalentClassFromPToken () const {
	return m_pContainerHandle->GetBlockEquivalentClassFromPToken ();
    }
    M_KOTE const &GetBlockEquivalentClassFromPToken (int xReference) const {
	return xReference < 0
	    ? m_pContainerHandle->GetBlockEquivalentClassFromPToken ()
	    : m_pContainerHandle->GetBlockEquivalentClassFromPToken (POP (xReference));
    }

    M_KOTE const &TheNAClass () const {
	return m_pContainerHandle->TheNAClass ();
    }
    M_KOTE const &TheNoValueClass () const {
	return m_pContainerHandle->TheNoValueClass ();
    }
    M_KOTE const &TheTrueClass () const {
	return m_pContainerHandle->TheTrueClass ();
    }
    M_KOTE const &TheFalseClass () const {
	return m_pContainerHandle->TheFalseClass ();
    }
    M_KOTE const &TheCharacterClass () const {
	return m_pContainerHandle->TheCharacterClass ();
    }
    M_KOTE const &TheDateClass () const {
	return m_pContainerHandle->TheDateClass ();
    }
    M_KOTE const &TheDoubleClass () const {
	return m_pContainerHandle->TheDoubleClass ();
    }
    M_KOTE const &TheFloatClass () const {
	return m_pContainerHandle->TheFloatClass ();
    }
    M_KOTE const &TheIntegerClass () const {
	return m_pContainerHandle->TheIntegerClass ();
    }
    M_KOTE const &ThePrimitiveClass () const {
	return m_pContainerHandle->ThePrimitiveClass ();
    }
    M_KOTE const &TheClosureClass () const {
	return m_pContainerHandle->TheClosureClass ();
    }
    M_KOTE const &TheFixedPropertyClass () const {
	return m_pContainerHandle->TheFixedPropertyClass ();
    }
    M_KOTE const &TheIndexedListClass () const {
	return m_pContainerHandle->TheIndexedListClass ();
    }
    M_KOTE const &TheListClass () const {
	return m_pContainerHandle->TheListClass ();
    }
    M_KOTE const &TheMethodClass () const {
	return m_pContainerHandle->TheMethodClass ();
    }
    M_KOTE const &TheSelectorClass () const {
	return m_pContainerHandle->TheSelectorClass ();
    }
    M_KOTE const &TheStringClass () const {
	return m_pContainerHandle->TheStringClass ();
    }
    M_KOTE const &TheTimeSeriesClass () const {
	return m_pContainerHandle->TheTimeSeriesClass ();
    }
    M_KOTE const &TheClosureClassDictionary () const {
	return m_pContainerHandle->TheClosureClassDictionary ();
    }
    M_KOTE const &TheMethodClassDictionary () const {
	return m_pContainerHandle->TheMethodClassDictionary ();
    }
    M_KOTE const &TheFixedPropertyClassDictionary () const {
	return m_pContainerHandle->TheFixedPropertyClassDictionary ();
    }
    M_KOTE const &TheTimeSeriesClassDictionary () const {
	return m_pContainerHandle->TheTimeSeriesClassDictionary ();
    }
    M_KOTE const &TheListClassDictionary () const {
	return m_pContainerHandle->TheListClassDictionary ();
    }
    M_KOTE const &TheIndexedListClassDictionary () const {
	return m_pContainerHandle->TheIndexedListClassDictionary ();
    }
    M_KOTE const &TheStringClassDictionary () const {
	return m_pContainerHandle->TheStringClassDictionary ();
    }
    M_KOTE const &TheSelectorClassDictionary () const {
	return m_pContainerHandle->TheSelectorClassDictionary ();
    }
    M_KOTE const &TheObjectPrototype () const {
	return m_pContainerHandle->TheObjectPrototype ();
    }
    M_KOTE const &TheScalarPToken () const {
	return m_pContainerHandle->TheScalarPToken ();
    }
    M_KOTE const &TheSelectorPToken () const {
	return m_pContainerHandle->TheSelectorPToken ();
    }

//  Known Object Query
public:
    bool NamesTheDateClass () const {
	return m_pContainerHandle->NamesTheDateClass ();
    }
    bool NamesTheDoubleClass () const {
	return m_pContainerHandle->NamesTheDoubleClass ();
    }
    bool NamesTheFloatClass () const {
	return m_pContainerHandle->NamesTheFloatClass ();
    }
    bool NamesTheIntegerClass () const {
	return m_pContainerHandle->NamesTheIntegerClass ();
    }
    bool NamesThePrimitiveClass () const {
	return m_pContainerHandle->NamesThePrimitiveClass ();
    }
    bool NamesTheSelectorClass () const {
	return m_pContainerHandle->NamesTheSelectorClass ();
    }

    bool NamesTheNAClass () const {
	return m_pContainerHandle->NamesTheNAClass ();
    }
    bool NamesTheTrueClass () const {
	return m_pContainerHandle->NamesTheTrueClass ();
    }
    bool NamesTheFalseClass () const {
	return m_pContainerHandle->NamesTheFalseClass ();
    }

    bool NamesTheScalarPToken () const {
	return m_pContainerHandle->NamesTheScalarPToken ();
    }

    bool NamesABuiltInNumericClass () const {
	return m_pContainerHandle->NamesABuiltInNumericClass ();
    }

    bool DoesntNameTheDateClass () const {
	return m_pContainerHandle->DoesntNameTheDateClass ();
    }
    bool DoesntNameTheDoubleClass () const {
	return m_pContainerHandle->DoesntNameTheDoubleClass ();
    }
    bool DoesntNameTheFloatClass () const {
	return m_pContainerHandle->DoesntNameTheFloatClass ();
    }
    bool DoesntNameTheIntegerClass () const {
	return m_pContainerHandle->DoesntNameTheIntegerClass ();
    }
    bool DoesntNameThePrimitiveClass () const {
	return m_pContainerHandle->DoesntNameThePrimitiveClass ();
    }
    bool DoesntNameTheSelectorClass () const {
	return m_pContainerHandle->DoesntNameTheSelectorClass ();
    }

    bool DoesntNameTheNAClass () const {
	return m_pContainerHandle->DoesntNameTheNAClass ();
    }
    bool DoesntNameTheTrueClass () const {
	return m_pContainerHandle->DoesntNameTheTrueClass ();
    }
    bool DoesntNameTheFalseClass () const {
	return m_pContainerHandle->DoesntNameTheFalseClass ();
    }

    bool DoesntNameTheScalarPToken () const {
	return m_pContainerHandle->DoesntNameTheScalarPToken ();
    }

    bool DoesntNameABuiltInNumericClass () const {
	return m_pContainerHandle->DoesntNameABuiltInNumericClass ();
    }

//  Mutex Management
public:
    bool AcquireMutex (VMutexClaim& rClaim, VComputationUnit* pSupplicant) const {
	return m_pContainerHandle->AcquireMutex (rClaim, pSupplicant);
    }

//  Transient Extension Management
public:
    void AcquireTransientExtensionAccessLock (transientx_t const* pTransientExtension) const {
	m_pContainerHandle->AcquireTransientExtensionAccessLock (pTransientExtension);
    }
    void ReleaseTransientExtensionAccessLock (transientx_t const* pTransientExtension) const {
	m_pContainerHandle->ReleaseTransientExtensionAccessLock (pTransientExtension);
    }

    void ClearTransientExtension () const {
	m_pContainerHandle->ClearTransientExtension ();
    }
    void SetTransientExtensionTo (transientx_t* pTransientExtension) const {
	m_pContainerHandle->SetTransientExtensionTo (pTransientExtension);
    }

//  Database Update Support
public:
    PS_UpdateStatus CreateSpace () {
	return m_pContainerHandle->CreateSpace ();
    }
    PS_UpdateStatus SaveTheTLE (
	void *pOutputHandlerData, M_AND::UpdateOutputHandler pOutputHandler
    ) {
	return m_pContainerHandle->SaveTheTLE (pOutputHandlerData, pOutputHandler);
    }

//  Incorporator Support
public:
    void WriteDBUpdateInfo (bool replace) const {
	m_pContainerHandle->WriteDBUpdateInfo (replace);
    }

//  State
public:
    M_CPreamble			*m_pContainerPreamble;
    VContainerHandle		*m_pContainerHandle;
    unsigned int		m_fGCLocked		: 1,
				m_iReferenceCount	: 31;
    M_CPD			*m_pNext;
    M_CPD			*m_pPrev;
    union Pointer {
	void		*asAny;
	unsigned char	*asByte;
	char		*asCharacter;
	unsigned int	*asCardinal;
	int		*asInteger;
	VkUnsigned64	*asUnsigned64;
	VkUnsigned96	*asUnsigned96;
	VkUnsigned128	*asUnsigned128;
	float		*asSmallReal;
	double		*asReal;
	M_POP		*asPOP;
    }				m_iPointerArray[1];
};


/*****************************************
 *****  Deferred Inline Definitions  *****
 *****************************************/

M_ASD *M_KnownObjectTable::Space () const {
    return m_pKOTC->Space ();
}

M_CPD *M_KnownObjectTable::Entry::RetainedObjectCPD () const {
    m_pObjectCPD->retain ();
    return m_pObjectCPD;
}

M_CPD *M_KnownObjectTable::Entry::RetainedPTokenCPD () const {
    m_pPTokenCPD->retain ();
    return m_pPTokenCPD;
}


#endif
