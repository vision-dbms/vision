#ifndef VContainerHandle_Interface
#define VContainerHandle_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

#include "VMutex.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "M_ASD.h"

#include "M_RTD.h"

#include "venvir.d"

class M_CPD;
class M_CTE;


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * One of the responsibilities of an entry in a transient container table is
 * maintenance of the connection between CPDs and containers.  To simplify
 * that function, a transient container table entry which must keep track of
 * CPDs is extended by a Container Pointer Connector Cell (CPCC).  A CPCC is
 * used to store a pointer to the list of CPDs attached to a container and
 * all container locative information which would normally be stored in the
 * container table entry.  Because CPCCs live in uncompacted memory (unlike the
 * rest of the container table entry), they can be directly addressed by CPDs
 * using simple 'C' pointers.   Because all container locative information is
 * stored in the CPCC, the bookkeeping associated with CPD initiated container
 * movement can be performed without reference to the container table.
 *
 *  CPCC Field Descriptions:
 *	spaceASD		- the address of the ASD associated with the
 *				  space of the container to which this CPCC
 *				  is attached.
 *	isReadWrite		- a boolean flag which, when true, indicates
 *				  that the container associated with this CPCC
 *				  can be modified.  When this flag is false,
 *				  an attempt to modify the container will
 *				  cause a segmentation fault.
 *	containerAddress	- the address of the container accessed via
 *				  this CPCC.  This address will ALWAYS be
 *				  valid.
 *	cpdChainHead		- a pointer to the first CPD in the chain of
 *				  CPD's attached to this CPCC.
 *
 *---------------------------------------------------------------------------
 */
class VContainerHandle : public VBenderenceable {
    DECLARE_CONCRETE_RTT (VContainerHandle, VBenderenceable);

//  Friends
    friend class M_ASD;
    friend class M_ASD::CT::Page::Entry;
    friend class M_CPD;
    friend class M_CTE;

//  Aliases
public:
    typedef V::VArgList VArgList;

//  Container Processing Callbacks
public:
    typedef char *pointer_t;
    typedef void (*ShiftProcessor) (
	VContainerHandle* cpd, pointer_t pTail, ptrdiff_t sShift, va_list ap
    );

//  Parameters
public:
    static bool g_fPreservingHandles;

//  Construction
protected:
    VContainerHandle (M_CTE &rCTE);

public:
    static VContainerHandle *Maker (M_CTE &rCTE);

//  Destruction
protected:
    ~VContainerHandle ();

//  Reference Management
private:
    void createReference () {
	BaseClass::retain ();
    }
    void deleteReference () {
	BaseClass::release ();
    }
public:
    void retain () {
	AcquireAccessLock ();
    }
    void release () {
	ReleaseAccessLock ();
    }

//  Access
public:
    unsigned int AccessCount () const {
	return m_iAccessCount;
    }

    unsigned int attentionMask () const {
	return m_pDCTE->attentionMask ();
    }

    M_CPreamble *containerAddress () const {
	return m_pContainerAddress;
    }
    pointer_t containerContent () const {
	return reinterpret_cast<pointer_t>(m_pContainerAddress + 1);
    }
    pointer_t ContainerContent () const {
	return containerContent ();
    }

    M_POP const &containerIdentity () const {
	return M_CPreamble_POP (m_pContainerAddress);
    }

    RTYPE_Type ContainerRType () const {
	return (RTYPE_Type)M_CPreamble_RType (m_pContainerAddress);
    }

    unsigned int ContainerSize () const {
	return M_CPreamble_Size (m_pContainerAddress);
    }

    unsigned int ContainerIndex () const {
	return M_CPreamble_POPContainerIndex (m_pContainerAddress);
    }

    M_CPD* CPDChainHead () const {
	return m_pCPDChainHead;
    }
    unsigned int CPDPointerCount () const {
	return m_pRTD->CPDPointerCount ();
    }

    M_AND* Database () const {
	return m_pASD->Database ();
    }

    M_KnownObjectTable *KnownObjectTable () const {
	return m_pASD->KnownObjectTable ();
    }
    M_KOT *KOT () const {
	return m_pASD->KOT ();
    }

    M_RTD *RTD () const {
	return m_pRTD;
    }

    RTYPE_Type RType () const {
	return m_pRTD->RType ();
    }
    char const *RTypeName () const {
	return m_pRTD->RTypeName ();
    }

    M_ASD *ScratchPad () const {
	return m_pASD->ScratchPad ();
    }

    M_ASD *Space () const {
	return m_pASD;
    }
    unsigned int SpaceIndex () const {
	return m_pASD->Index ();
    }

    M_POP const &TheNilPOP () const {
	return m_pASD->TheNilPOP ();
    }

    transientx_t *TransientExtension () const {
	return m_pTransientExtension;
    }

    transientx_t *TransientExtensionIfA (VRunTimeType const& rRTT) const;

//  Query
public:
    bool ContainerHasMultipleUses () const {
	return m_iReferenceCount > 1
	    || m_iAccessCount > 1
	    || m_pDCTE->referenceCount () > 1;
    }

    bool isAForwardingTarget () const {
	return m_pDCTE->isAForwardingTarget ();
    }
    bool isATRefRescanTarget () const {
	return m_pDCTE->isATRefRescanTarget ();
    }

    bool IsReadOnly () const {
	return !m_fIsReadWrite;
    }
    bool IsReadWrite () const {
	return m_fIsReadWrite;
    }

    bool IsInTheScratchPad () const {
	return m_pASD->IsTheScratchPad ();
    }

    bool IsntInTheScratchPad () const {
	return m_pASD->IsntTheScratchPad ();
    }

    bool HasATransientExtension	() const {
	return m_pTransientExtension.isntEmpty ();
    }

    bool RTypeIs (RTYPE_Type xType) const {
	return RType () == xType;
    }
    bool RTypeIsnt (RTYPE_Type xType) const {
	return RType () != xType;
    }

    bool TransientExtensionIsA (VRunTimeType const& rRTT) const;

//  Callbacks
public:
    virtual void CheckConsistency ();

private:
    virtual bool PersistReferences ();

//  Container Access
private:
    void ReleaseCTE ();
    void AcquireAccessLock ();
    void ReleaseAccessLock ();

public:
    M_CPreamble *GetContainerAddress (M_POP const *pReference) const {
	return m_pASD->GetContainerAddress (pReference);
    }
    VContainerHandle* GetContainerHandle (
	M_POP const *pReference, RTYPE_Type xExpectedType = RTYPE_C_Any
    ) const {
	return m_pASD->GetContainerHandle (pReference, xExpectedType);
    }

    M_CPD* GetCPD ();
    M_CPD* GetCPD (M_POP const *pReference, RTYPE_Type xExpectedType = RTYPE_C_Any) {
	return GetContainerHandle (pReference, xExpectedType)->GetCPD ();
    }

    M_CPD* NewCPD ();

//  Container Creation
public:
    M_CPD *CreateContainer (RTYPE_Type xRType, size_t sContainer) const {
	return m_pASD->CreateContainer (xRType, sContainer);
    }

//  Container Update
protected:
    virtual void AdjustContainerPointers (M_CPreamble *newAddress, bool newAddressIsReadWrite);

    virtual void AdjustContainerTailPointers (pointer_t pOldTail, pointer_t pNewTail, pointer_diff_t sa);

public:
    void EnableModifications ();

    void ReallocateContainer (size_t newSize);

    void GrowContainer (ptrdiff_t sizeChange) {
	ReallocateContainer ((size_t)(M_CPreamble_Size (m_pContainerAddress) + sizeChange));
    }

    //  The following methods work with writeable containers only...
    void VShiftContainerTail (
	pointer_t	pTail,
	size_t		sTail,
	ptrdiff_t	sShift,
	bool		bSqueeze,
	ShiftProcessor	pShiftProcessor = 0,
	va_list		pShiftProcessorArgs = 0
    );
    void __cdecl ShiftContainerTail (
	pointer_t	pTail,
	size_t		sTail,
	ptrdiff_t	sShift,
	bool		bSqueeze,
	ShiftProcessor	pShiftProcessor,
	...
    );
    void ShiftContainerTail (pointer_t pTail, size_t sTail, ptrdiff_t sShift, bool bSqueeze) {
	VShiftContainerTail (pTail, sTail, sShift, bSqueeze);
    }

//  Reference Access
public:
    M_KOT *ReferencedKOT (M_POP const *pReference) const {
	return m_pASD->ReferencedKOT (pReference);
    }

    M_AND *ReferencedNetwork (M_POP const *pReference) const {
	return m_pASD->ReferencedNetwork (pReference);
    }

//  Reference Testing
public:
    /****************
     *----  ==  ----*
     ****************/

    bool Names (VContainerHandle const *pThat) const {
	return this == pThat;
    }
    bool Names (M_KOTM pKOTM) const {
	return this == (KOT ()->*pKOTM).ObjectHandle ();
    }
    bool NamesPToken (M_KOTM pKOTM) const {
	return this == (KOT ()->*pKOTM).PTokenHandle ();
    }

    bool Names (M_AND *pThat, M_POP const *pThatReference) const {
	return m_pASD->ReferenceNames (&containerIdentity (), pThat, pThatReference);
    }
    bool Names (M_ASD *pThat, M_POP const *pThatReference) const {
	return m_pASD->ReferenceNames (&containerIdentity (), pThat, pThatReference);
    }

    bool ReferencesAreIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pASD->ReferencesAreIdentical (pReference1, pReference2);
    }

    bool ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceNames (pReference, pKOTM);
    }
    bool ReferenceNamesPToken (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceNamesPToken (pReference, pKOTM);
    }

    bool ReferenceNames (
	M_POP const *pReference, VContainerHandle const *pThat
    ) const {
	return m_pASD->ReferenceNames (
	    pReference, pThat->Space (), &pThat->containerIdentity ()
	);
    }
    bool ReferenceNames (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pASD->ReferenceNames (
	    pReference, pThat->Space (), pThatReference
	);
    }

    bool ReferenceIsNil (M_POP const *pReference) const {
	return m_pASD->ReferenceIsNil (pReference);
    }

    /*****************
     *----  !==  ----*
     *****************/

    bool DoesntName (VContainerHandle const *pThat) const {
	return this != pThat;
    }

    bool DoesntName (M_KOTM pKOTM) const {
	return this != (KOT ()->*pKOTM).ObjectHandle ();
    }
    bool DoesntNamePToken (M_KOTM pKOTM) const {
	return this != (KOT ()->*pKOTM).PTokenHandle ();
    }

    bool DoesntName (M_AND *pThat, M_POP const *pThatReference) const {
	return m_pASD->ReferenceDoesntName (&containerIdentity (), pThat, pThatReference);
    }
    bool DoesntName (M_ASD *pThat, M_POP const *pThatReference) const {
	return m_pASD->ReferenceDoesntName (&containerIdentity (), pThat, pThatReference);
    }

    bool ReferencesArentIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pASD->ReferencesArentIdentical (pReference1, pReference2);
    }

    bool ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceDoesntName (pReference, pKOTM);
    }
    bool ReferenceDoesntNamePToken (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceDoesntNamePToken (pReference, pKOTM);
    }

    bool ReferenceDoesntName (
	M_POP const *pReference, VContainerHandle const *pThat
    ) const {
	return m_pASD->ReferenceDoesntName (
	    pReference, pThat->Space (), &pThat->containerIdentity ()
	);
    }
    bool ReferenceDoesntName (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pASD->ReferenceDoesntName (
	    pReference, pThat->Space (), pThatReference
	);
    }

    bool ReferenceIsntNil (M_POP const *pReference) const {
	return m_pASD->ReferenceIsntNil (pReference);
    }

//  Reference Management
public:
    //  The following methods work with writeable containers only...
    void constructReference (M_POP *pReference) const {
	m_pASD->constructReference (pReference);
    }
    void constructReferences (M_POP *pReferences, unsigned int cReferences) const {
	m_pASD->constructReferences (pReferences, cReferences);
    }

    void ClearReference (M_POP *pReference) const {
	m_pASD->ClearReference (pReference);
    }
    void ClearReferences (M_POP *pReferences, unsigned int cReferences) const {
	m_pASD->ClearReferences (pReferences, cReferences);
    }

    /*----------------------------------------------------------------------------
     *  The 'Locate*Name*' routines defined below return true when the local
     *  name already exists, and false when it does not.  As their names suggest,
     *  the '*OrAdd*' variants will update the appropriate federation and return 
     *  the requested local name if it does not already exist.
     *----------------------------------------------------------------------------
     */
    bool LocateNameIn (M_AND *pThat, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pASD, ContainerIndex (), rName);
    }

    bool LocateNameIn (M_ASD *pThat, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pASD, ContainerIndex (), rName);
    }

    bool LocateNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pASD->LocateNameOf (pThat, xThatName, rName);
    }

    bool LocateNameIn (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pASD, ContainerIndex (), rName);
    }
    bool LocateNameOf (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateNameIn (m_pASD, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateNameIn (M_AND *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pASD, pThisName, rName);
    }

    bool LocateNameIn (M_ASD *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return pThat->LocateNameOf (m_pASD, pThisName, rName);
    }

    bool LocateNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) const {
	return m_pASD->LocateNameOf (pThat, pThatName, rName);
    }

    bool LocateNameIn (
	VContainerHandle const *pThat, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThat->LocateNameOf (m_pASD, pThisName, rName);
    }
    bool LocateNameOf (
	VContainerHandle const *pThat, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThat->LocateNameIn (m_pASD, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (M_AND *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pASD, ContainerIndex (), rName);
    }

    bool LocateOrAddNameIn (M_ASD *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pASD, ContainerIndex (), rName);
    }

    bool LocateOrAddNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pASD->LocateOrAddNameOf (pThat, xThatName, rName);
    }

    bool LocateOrAddNameIn (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pASD, ContainerIndex (), rName);
    }
    bool LocateOrAddNameOf (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameIn (m_pASD, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameIn (M_AND *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pASD, pThisName, rName);
    }

    bool LocateOrAddNameIn (M_ASD *pThat, M_POP const *pThisName, M_TOP &rName) const {
	return pThat->LocateOrAddNameOf (m_pASD, pThisName, rName);
    }

    bool LocateOrAddNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) const {
	return m_pASD->LocateOrAddNameOf (pThat, pThatName, rName);
    }

    bool LocateOrAddNameIn (
	VContainerHandle *pThat, M_POP const *pThisName, M_TOP &rName
    ) const {
	return pThat->LocateOrAddNameOf (m_pASD, pThisName, rName);
    }
    bool LocateOrAddNameOf (
	VContainerHandle *pThat, M_POP const *pThatName, M_TOP &rName
    ) const {
	return pThat->LocateOrAddNameIn (m_pASD, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    void StoreReference (
	M_POP *pReference, VContainerHandle const *pThat, M_POP const *pThatReference
    ) const {
	m_pASD->StoreReference (pReference, pThat->Space (), pThatReference);
    }
    void StoreReference (M_POP *pReference, VContainerHandle const *pThat) const {
	StoreReference (pReference, pThat, &pThat->containerIdentity ());
    }

//  Known Object Access
public:
    M_KOTE const &GetBlockEquivalentClassFromPToken () const {
	return m_pASD->GetBlockEquivalentClassFromPToken (this);
    }
    M_KOTE const &GetBlockEquivalentClassFromPToken (M_POP const *pReference) const {
	M_KOT *pKOT = ReferencedKOT (pReference);
	return ReferenceNames (pReference, pKOT->TheStringPTokenHandle ())
	    ? pKOT->TheStringClass : pKOT->TheSelectorClass;
    }

    M_KOTE const &TheNAClass () const {
	return m_pASD->TheNAClass ();
    }
    M_KOTE const &TheNoValueClass () const {
	return m_pASD->TheNoValueClass ();
    }
    M_KOTE const &TheTrueClass () const {
	return m_pASD->TheTrueClass ();
    }
    M_KOTE const &TheFalseClass () const {
	return m_pASD->TheFalseClass ();
    }
    M_KOTE const &TheCharacterClass () const {
	return m_pASD->TheCharacterClass ();
    }
    M_KOTE const &TheDateClass () const {
	return m_pASD->TheDateClass ();
    }
    M_KOTE const &TheDoubleClass () const {
	return m_pASD->TheDoubleClass ();
    }
    M_KOTE const &TheFloatClass () const {
	return m_pASD->TheFloatClass ();
    }
    M_KOTE const &TheIntegerClass () const {
	return m_pASD->TheIntegerClass ();
    }
    M_KOTE const &ThePrimitiveClass () const {
	return m_pASD->ThePrimitiveClass ();
    }
    M_KOTE const &TheClosureClass () const {
	return m_pASD->TheClosureClass ();
    }
    M_KOTE const &TheFixedPropertyClass () const {
	return m_pASD->TheFixedPropertyClass ();
    }
    M_KOTE const &TheIndexedListClass () const {
	return m_pASD->TheIndexedListClass ();
    }
    M_KOTE const &TheListClass () const {
	return m_pASD->TheListClass ();
    }
    M_KOTE const &TheMethodClass () const {
	return m_pASD->TheMethodClass ();
    }
    M_KOTE const &TheSelectorClass () const {
	return m_pASD->TheSelectorClass ();
    }
    M_KOTE const &TheStringClass () const {
	return m_pASD->TheStringClass ();
    }
    M_KOTE const &TheTimeSeriesClass () const {
	return m_pASD->TheTimeSeriesClass ();
    }
    M_KOTE const &TheClosureClassDictionary () const {
	return m_pASD->TheClosureClassDictionary ();
    }
    M_KOTE const &TheMethodClassDictionary () const {
	return m_pASD->TheMethodClassDictionary ();
    }
    M_KOTE const &TheFixedPropertyClassDictionary () const {
	return m_pASD->TheFixedPropertyClassDictionary ();
    }
    M_KOTE const &TheTimeSeriesClassDictionary () const {
	return m_pASD->TheTimeSeriesClassDictionary ();
    }
    M_KOTE const &TheListClassDictionary () const {
	return m_pASD->TheListClassDictionary ();
    }
    M_KOTE const &TheIndexedListClassDictionary () const {
	return m_pASD->TheIndexedListClassDictionary ();
    }
    M_KOTE const &TheStringClassDictionary () const {
	return m_pASD->TheStringClassDictionary ();
    }
    M_KOTE const &TheSelectorClassDictionary () const {
	return m_pASD->TheSelectorClassDictionary ();
    }
    M_KOTE const &TheObjectPrototype () const {
	return m_pASD->TheObjectPrototype ();
    }
    M_KOTE const &TheScalarPToken () const {
	return m_pASD->TheScalarPToken ();
    }
    M_KOTE const &TheSelectorPToken () const {
	return m_pASD->TheSelectorPToken ();
    }

//  Known Object Query
public:
    bool NamesTheDateClass () const {
	return this == TheDateClass ().ObjectHandle ();
    }
    bool NamesTheDoubleClass () const {
	return this == TheDoubleClass ().ObjectHandle ();
    }
    bool NamesTheFloatClass () const {
	return this == TheFloatClass ().ObjectHandle ();
    }
    bool NamesTheIntegerClass () const {
	return this == TheIntegerClass ().ObjectHandle ();
    }
    bool NamesThePrimitiveClass () const {
	return this == ThePrimitiveClass ().ObjectHandle ();
    }
    bool NamesTheSelectorClass () const {
	return this == TheSelectorClass ().ObjectHandle ();
    }

    bool NamesTheNAClass () const {
	return this == TheNAClass ().ObjectHandle ();
    }
    bool NamesTheTrueClass () const {
	return this == TheTrueClass ().ObjectHandle ();
    }
    bool NamesTheFalseClass () const {
	return this == TheFalseClass ().ObjectHandle ();
    }

    bool NamesTheScalarPToken () const {
	return this == TheScalarPToken ().ObjectHandle ();
    }

    bool NamesABuiltInNumericClass () const {
	return NamesTheDoubleClass	()
	    || NamesTheFloatClass	()
	    || NamesTheIntegerClass	();
    }

    bool DoesntNameTheDateClass () const {
	return this != TheDateClass ().ObjectHandle ();
    }
    bool DoesntNameTheDoubleClass () const {
	return this != TheDoubleClass ().ObjectHandle ();
    }
    bool DoesntNameTheFloatClass () const {
	return this != TheFloatClass ().ObjectHandle ();
    }
    bool DoesntNameTheIntegerClass () const {
	return this != TheIntegerClass ().ObjectHandle ();
    }
    bool DoesntNameThePrimitiveClass () const {
	return this != ThePrimitiveClass ().ObjectHandle ();
    }
    bool DoesntNameTheSelectorClass () const {
	return this != TheSelectorClass ().ObjectHandle ();
    }

    bool DoesntNameTheNAClass () const {
	return this != TheNAClass ().ObjectHandle ();
    }
    bool DoesntNameTheTrueClass () const {
	return this != TheTrueClass ().ObjectHandle ();
    }
    bool DoesntNameTheFalseClass () const {
	return this != TheFalseClass ().ObjectHandle ();
    }

    bool DoesntNameTheScalarPToken () const {
	return this != TheScalarPToken ().ObjectHandle ();
    }

    bool DoesntNameABuiltInNumericClass () const {
	return DoesntNameTheDoubleClass	()
	    && DoesntNameTheFloatClass	()
	    && DoesntNameTheIntegerClass();
    }

//  Mutex Management
public:
    bool AcquireMutex (VMutexClaim& rClaim, VComputationUnit *pSupplicant) {
	return m_iMutex.acquireClaim (rClaim, pSupplicant, this);
    }

//  Transient Extension Management
public:
    void AcquireTransientExtensionAccessLock (transientx_t const *pTransientExtension) {
	if (m_pTransientExtension == pTransientExtension && !m_fTransientExtensionHasAnAccessLock) {
	    AcquireAccessLock ();
	    m_fTransientExtensionHasAnAccessLock = true;
	}
    }
    void ReleaseTransientExtensionAccessLock (transientx_t const *pTransientExtension) {
	if (m_pTransientExtension == pTransientExtension && m_fTransientExtensionHasAnAccessLock) {
	    m_fTransientExtensionHasAnAccessLock = false;
	    ReleaseAccessLock ();
	}
    }

    void ClearTransientExtension () {
	if (m_pTransientExtension) {
	    m_pTransientExtension->EndRoleAsTransientExtensionOf (this);
	    m_pTransientExtension.clear ();
	}
	if (m_fTransientExtensionHasAnAccessLock) {
	    m_fTransientExtensionHasAnAccessLock = false;
	    ReleaseAccessLock ();
	}
    }
    void SetTransientExtensionTo (transientx_t *pTransientExtension) {
	if (pTransientExtension != m_pTransientExtension) {
	    ClearTransientExtension ();
	    m_pTransientExtension.setTo (pTransientExtension);
	}
    }

//  Update
public:
    void AttachToCTE (M_CTE &rCTE);

    void setAttentionMaskTo (unsigned int iNewAttentionMask) const {
	m_pDCTE->setAttentionMaskTo (iNewAttentionMask);
    }

//  Reference Forwarding
public:
    void ForwardToSpace (M_ASD *pTargetSpace);

//  Reference Persistence
protected:
    bool Persist (M_POP *pReference) const {
	return m_pASD->Persist (pReference);
    }
    bool Persist (M_POP *pReferences, unsigned int cReferences) const {
	return m_pASD->Persist (pReferences, cReferences);
    }

//  Container Enumeration Callbacks
public:
    bool CallFunction			(VArgList const &rArgList);
    bool CallProcedure			(VArgList const &rArgList);
    bool DoNothing			(VArgList const &rArgList);
    bool FlushCachedResources		(VArgList const &rArgList);
    bool PersistReferences		(VArgList const &rArgList);
    bool StoreContainer			(VArgList const &rArgList);
    bool StoreLargeContainer		(VArgList const &rArgList);
    bool StoreSmallContainer		(VArgList const &rArgList);

//  Database Update Support
public:
    PS_UpdateStatus CreateSpace ();

    PS_UpdateStatus SaveTheTLE (
	void *pOutputHandlerData, M_AND::UpdateOutputHandler pOutputHandler
    ) {
	return m_pASD->SaveTheTLE (pOutputHandlerData, pOutputHandler);
    }

    PS_UpdateStatus UpdateNetwork (bool bAdministrativeUpdate) {
	return m_pASD->UpdateNetwork (bAdministrativeUpdate);
    }

//  Incorporator Support
public:
    void WriteDBUpdateInfo (bool replace) const {
	m_pASD->WriteDBUpdateInfo (ContainerIndex (), replace);
    }

//  State
protected:
    M_CPreamble*		m_pContainerAddress;
    M_RTD *const		m_pRTD;
    M_ASD*			m_pASD;
    M_DCTE*			m_pDCTE;
    M_CPD*			m_pCPDChainHead;
    VMutex			m_iMutex;
    transientx_t::Reference	m_pTransientExtension;
    unsigned short		m_iAccessCount;
    unsigned int		m_fIsReadWrite				: 1;
    unsigned int		m_fOwnsAnUnusedAccessLock		: 1;
    unsigned int		m_fTransientExtensionHasAnAccessLock	: 1;
};


/*****************************************
 *****  Deferred Inline Definitions  *****
 *****************************************/

bool M_AND::ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEObjectHandle (pReference, pKOTM)->Names (this, pReference);
}

bool M_AND::ReferenceNamesPToken (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEPTokenHandle (pReference, pKOTM)->Names (this, pReference);
}
 
bool M_AND::ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEObjectHandle (pReference, pKOTM)->DoesntName (this, pReference);
}

bool M_AND::ReferenceDoesntNamePToken (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEPTokenHandle (pReference, pKOTM)->DoesntName (this, pReference);
}


#endif
