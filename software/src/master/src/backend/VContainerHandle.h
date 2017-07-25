#ifndef VContainerHandle_Interface
#define VContainerHandle_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "M_ASD.h"

#include "M_RTD.h"

#include "venvir.d"

class M_CPD;
class M_CTE;
class rtPTOKEN_CType;

namespace V {
    class VSimpleFile;
}

#include "Vdd_Store.h"


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
    typedef V::VAggregatePointer<M_CPD>		M_CPD_Pointer;
    typedef V::VAggregatePointer<M_DCTE>	M_DCTE_Pointer;
    typedef V::VAggregatePointer<M_POP const>	M_POP_Pointer;
    typedef V::VArgList VArgList;

    typedef Vdd::Object::Visitor Visitor;

//  class GCState
public:
    template <typename GenMaster> class GCState {
    //  Aliases
    public:
	typedef typename GenMaster::GenIndex_t GenIndex_t;

    //  Construction
    public:
	GCState () {
	    reset ();
	}

    //  Access
    public:
	GenIndex_t currentGeneration () const {
	    return m_xGeneration;
	}
	static GenIndex_t masterGeneration () {
	    return GenMaster::CurrentGeneration ();
	}

	unsigned int interhandleReferenceCount () const {
	    refresh ();
	    return m_cInterhandleReferences;
	}

	bool cdVisited () const {
	    refresh ();
	    return m_bCDVisited;
	}
	bool gcVisited () const {
	    refresh ();
	    return m_bGCVisited;
	}

    //  First Visit
    public:
	bool onFirstGCVisit () {
	    return onFirstVisit (m_bGCVisited);
	}
	bool onFirstCDVisit () {
	    return onFirstVisit (m_bCDVisited);
	}
    private:
	bool onFirstVisit (bool &rbVisited) {
	    refresh ();
	    bool const bFirstVisit = !rbVisited;
	    rbVisited = true;
	    return bFirstVisit;
	}

    // Update
    public:
	void incrementInterhandleReferenceCount () {
	    refresh ();
	    m_cInterhandleReferences++;
	}

    //  Refresh
    private:
	void refresh () const {
	    if (currentGeneration () != masterGeneration ())
		reset ();
	}
	void reset () const {
	    m_xGeneration = GenMaster::CurrentGeneration ();
	    m_bGCVisited = m_bCDVisited = false;
	    m_cInterhandleReferences = 0;
	}

    //  State
    private:
	GenIndex_t mutable	m_xGeneration;
	bool mutable		m_bGCVisited;
	bool mutable		m_bCDVisited;
	unsigned int mutable	m_cInterhandleReferences;
    };

//  Container Processing Callbacks
public:
    typedef void (*ShiftProcessor) (
	VContainerHandle* cpd, pointer_t pTail, ptrdiff_t sShift, va_list ap
    );

//  Parameters
public:
    static bool g_bPreservingHandles;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE);
protected:
    VContainerHandle (M_ASD *pSpace, RTYPE_Type xType, size_t sContainer);
    VContainerHandle (M_ASD *pSpace, RTYPE_Type xType);
    VContainerHandle (M_CTE &rCTE);
private:
    VContainerHandle *attachTo (M_CTE &rCTE);

//  Destruction
protected:
    ~VContainerHandle ();

//  Lifetime Management
public:
    bool isPrecious () const {
	return m_bPrecious;
    }
    bool isReferenced () const {
	return referenceCount () > 0;
    }
    bool isntReferenced () const {
	return referenceCount () == 0;
    }
protected:
    bool onDeleteThis ();

    void setPreciousTo (bool bPrecious) {
	m_bPrecious = bPrecious;
    }


//  Type Conversion
private:
    virtual Vdd::Store *getStore_();
public:
    Vdd::Store *getStore () {
	if (m_pStore.isNil ())
	    m_pStore.setTo (getStore_());
	return m_pStore;
    }
    bool getStore (VReference<Vdd::Store> &rpResult) {
	rpResult.setTo (getStore ());
	return rpResult.isntNil ();
    }

//  Access
public:
    unsigned int attentionMask () const {
	return m_pDCTE ? m_pDCTE->attentionMask () : 0;
    }

    M_CPreamble *containerAddress () const {
	return m_pContainer;
    }
    pointer_t containerContent () const {
	return reinterpret_cast<pointer_t>(m_pContainer + 1);
    }

    M_POP const *containerIdentity () const {
	return m_pContainerIdentity;
    }
    M_POP const *containerIdentity () {
	if (m_pContainerIdentity.isNil ()) {
	    m_pASD->CreateIdentity (m_iContainerIdentity, this);
	    m_pContainerIdentity.setTo (&m_iContainerIdentity);
	}
	return m_pContainerIdentity;
    }

    unsigned int containerIndexNC () const {
	return containerIndex ();
    }
    unsigned int containerIndex () const {
	return hasAnIdentity () ? M_POP_ContainerIndex (containerIdentity ()) : UINT_MAX;
    }
    unsigned int containerIndex () {
	return M_POP_ContainerIndex (containerIdentity ());
    }

    unsigned int containerSize () const {
	return m_pContainer ? M_CPreamble_Size (m_pContainer) : 0;
    }

    M_CPD *cpdChainHead () const {
	return m_pCPDChainHead;
    }
    unsigned int cpdPointerCount () const {
	return m_pRTD->cpdPointerCount ();
    }

    M_AND* Database () const {
	return m_pASD->Database ();
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
    unsigned int spaceIndex () const {
	return m_pASD->Index ();
    }

    M_POP const &TheNilPOP () const {
	return m_pASD->TheNilPOP ();
    }

//  Query
public:
    bool hasMultipleUses () const {
	return isReferenced () || (m_pDCTE && m_pDCTE->isReferenced ());
    }

    bool hasAContainer () const {
	return IsntNil (m_pContainer);
    }
    bool hasAnIdentity () const {
	return m_pContainerIdentity.isntNil ();
    }

    bool hasAReadOnlyContainer () const {
	return m_pContainer && !m_bReadWrite;
    }
    bool hasAReadWriteContainer () const {
	return m_pContainer && m_bReadWrite;
    }

    bool isReadOnly () const {
	return !m_pContainer || !m_bReadWrite;
    }

    bool hasNoContainer () const {
	return IsNil (m_pContainer);
    }
    bool hasNoIdentity () const {
	return m_pContainerIdentity.isNil ();
    }

    bool isAForwardingTarget () const {
	return m_pDCTE && m_pDCTE->isAForwardingTarget ();
    }
    bool isATRefRescanTarget () const {
	return m_pDCTE && m_pDCTE->isATRefRescanTarget ();
    }

    bool isInTheScratchPad () const {
	return m_pASD->IsTheScratchPad ();
    }

    bool isntInTheScratchPad () const {
	return m_pASD->IsntTheScratchPad ();
    }

    bool RTypeIs (RTYPE_Type xType) const {
	return RType () == xType;
    }
    bool RTypeIsnt (RTYPE_Type xType) const {
	return RType () != xType;
    }

//  Callbacks
public:
    virtual void CheckConsistency ();

private:
    virtual bool PersistReferences ();

//  Container Access
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
    VContainerHandle *CreateContainer (RTYPE_Type xRType, size_t sContainer) const {
	return m_pASD->CreateContainer (xRType, sContainer);
    }
protected:
    void CreateContainer (size_t sContainer) {
	ReallocateContainer (sContainer);
    }

//  Container Update
protected:
    virtual void AdjustContainerPointers (M_CPreamble *newAddress, bool newAddressIsReadWrite);

    virtual void AdjustContainerTailPointers (pointer_t pOldTail, pointer_t pNewTail, pointer_diff_t sa);

public:
    void EnableModifications ();

    void ReallocateContainer (size_t newSize);

    void GrowContainer (ptrdiff_t sizeChange) {
	ReallocateContainer ((size_t)(containerSize () + sizeChange));
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

//  Aligment
private:
    virtual bool align_() {
	return false;
    }
    virtual bool alignAll_(bool bCleaning) {
	return false;
    }
public:
    bool align () {
	return align_();
    }
    bool alignAll (bool bCleaning = true) {
	return alignAll_(bCleaning);
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

    bool Names (M_AND *pThat, M_POP const *pThatReference) const {
	return hasAnIdentity () && m_pASD->ReferenceNames (containerIdentity (), pThat, pThatReference);
    }
    bool Names (M_ASD *pThat, M_POP const *pThatReference) const {
	return hasAnIdentity () && m_pASD->ReferenceNames (containerIdentity (), pThat, pThatReference);
    }

    bool ReferencesAreIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pASD->ReferencesAreIdentical (pReference1, pReference2);
    }

    bool ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceNames (pReference, pKOTM);
    }

    bool ReferenceNames (M_POP const *pReference, VContainerHandle const *pThat) const {
	return pThat->Names (m_pASD, pReference);
    }
    bool ReferenceNames (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pASD->ReferenceNames (pReference, pThat->Space (), pThatReference);
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

    bool DoesntName (M_AND *pThat, M_POP const *pThatReference) const {
	return hasNoIdentity () || m_pASD->ReferenceDoesntName (containerIdentity (), pThat, pThatReference);
    }
    bool DoesntName (M_ASD *pThat, M_POP const *pThatReference) const {
	return hasNoIdentity () || m_pASD->ReferenceDoesntName (containerIdentity (), pThat, pThatReference);
    }

    bool ReferencesArentIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pASD->ReferencesArentIdentical (pReference1, pReference2);
    }

    bool ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pASD->ReferenceDoesntName (pReference, pKOTM);
    }

    bool ReferenceDoesntName (M_POP const *pReference, VContainerHandle const *pThat) const {
	return pThat->DoesntName (m_pASD, pReference);
    }
    bool ReferenceDoesntName (
	M_POP const *pReference, VContainerHandle const	*pThat, M_POP const *pThatReference
    ) const {
	return m_pASD->ReferenceDoesntName (pReference, pThat->Space (), pThatReference);
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
	return hasAnIdentity () && pThat->LocateNameOf (m_pASD, containerIndex (), rName);
    }

    bool LocateNameIn (M_ASD *pThat, M_TOP &rName) const {
	return hasAnIdentity () && pThat->LocateNameOf (m_pASD, containerIndex (), rName);
    }

    bool LocateNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pASD->LocateNameOf (pThat, xThatName, rName);
    }

    bool LocateNameIn (VContainerHandle const *pThat, M_TOP &rName) const {
	return hasAnIdentity () && pThat->LocateNameOf (m_pASD, containerIndex (), rName);
    }
    bool LocateNameOf (VContainerHandle const *pThat, M_TOP &rName) const {
	return pThat->LocateNameIn (m_pASD, rName);
    }
    bool LocateNameOf (Vdd::Store *pStore, M_TOP &rName) const;

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
    bool LocateOrAddNameIn (M_AND *pThat, M_TOP &rName) {
	return pThat->LocateOrAddNameOf (m_pASD, containerIndex (), rName);
    }

    bool LocateOrAddNameIn (M_ASD *pThat, M_TOP &rName) {
	return pThat->LocateOrAddNameOf (m_pASD, containerIndex (), rName);
    }

    bool LocateOrAddNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pASD->LocateOrAddNameOf (pThat, xThatName, rName);
    }

    bool LocateOrAddNameIn (VContainerHandle const *pThat, M_TOP &rName) {
	return pThat->LocateOrAddNameOf (m_pASD, containerIndex (), rName);
    }
    bool LocateOrAddNameOf (VContainerHandle *pThat, M_TOP &rName) const {
	return pThat->LocateOrAddNameIn (m_pASD, rName);
    }
    bool LocateOrAddNameOf (Vdd::Store *pStore, M_TOP &rName) const;

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
    void StoreReference (M_POP *pReference, VContainerHandle *pThat) const {
	m_pASD->StoreReference (pReference, pThat);
    }
    void StoreReference (M_POP *pReference, Vdd::Store *pThat) const {
	m_pASD->StoreReference (pReference, pThat);
    }

//  Known Object Access
public:
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
    rtPTOKEN_Handle *TheScalarPTokenHandle () const {
	return m_pASD->TheScalarPTokenHandle ();
    }
    rtPTOKEN_Handle *TheSelectorPTokenHandle () const {
	return m_pASD->TheSelectorPTokenHandle ();
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

//  P-Token Terminality
public:
    bool isTerminal (M_POP const *pPOP);
    bool isTerminal (M_POP const *pPOP, rtPTOKEN_CType *&rpPTC) {
	return !isntTerminal (pPOP, rpPTC);
	}
    bool isTerminal (M_POP const *pPOP, VReference<rtPTOKEN_Handle> &rpPTH) {
	return !isntTerminal (pPOP, rpPTH);
    }

    bool isntTerminal (M_POP const *pPOP) {
	return !isTerminal (pPOP);
    }
    bool isntTerminal (M_POP const *pPOP, rtPTOKEN_CType *&rpPTC);
    bool isntTerminal (M_POP const *pPOP, VReference<rtPTOKEN_Handle> &rpPTH);

//  Update
public:
    void AttachToCTE (M_CTE &rCTE);
    void DetachFromCTE ();

    void setAttentionMaskTo (unsigned int iNewAttentionMask) const {
	if (m_pDCTE)
	    m_pDCTE->setAttentionMaskTo (iNewAttentionMask);
    }

//  Visiting
public:
    typedef void (ThisClass::*visitFunction)();

    void visitUsing (Visitor *visitor) {
	visitor->visitHandle (this);
    }

    virtual void visitReferencesUsing (Visitor *visitor);

// Garbage collection marking
public:
    void cdMarkFor(M_ASD::GCVisitCycleDetect *visitor);
    void gcMarkFor(M_ASD::GCVisitMark *visitor);

    unsigned int cdReferenceCount() const {
	return m_iGCState.interhandleReferenceCount ();
    }
    bool cdVisited () const {
	return m_iGCState.cdVisited ();
    }
    bool gcVisited () const {
	return m_iGCState.gcVisited ();
    }
    bool exceededReferenceCount () const {
	return cdReferenceCount () > referenceCount ();
    }
    bool foundAllReferences () const {
	return cdReferenceCount () == referenceCount ();
    }
    bool missedSomeReferences () const {
	return cdReferenceCount () < referenceCount ();
    }

//  Reference Forwarding
protected:
    virtual bool forwardToSpace_(M_ASD *pSpace);
public:
    bool forwardToSpace (M_ASD *pSpace) {
	return forwardToSpace_(pSpace);
    }
    bool ForwardToSpace (M_ASD *pSpace) {
	return forwardToSpace (pSpace);
    }

//  Reference Persistence
protected:
    bool Persist (M_POP *pReference) const {
	return m_pASD->Persist (pReference);
    }
    bool Persist (M_POP *pReferences, unsigned int cReferences) const {
	return m_pASD->Persist (pReferences, cReferences);
    }

//  Container Enumeration Callbacks
protected:
    virtual void flushCachedResources_();
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
    void WriteDBUpdateInfo (bool replace) {
	m_pASD->WriteDBUpdateInfo (containerIndex (), replace);
    }

//  Display and Inspection
private:
    virtual void describe_(bool bVerbose);
public:
    void describe (bool bVerbose = false) {
	describe_(bVerbose);
    }

    void generateLogRecord (char const *pWhere) const;

    virtual void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const;

public:
    class MapEntryData;
    virtual void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference) {
    }
    virtual unsigned int getClusterReferenceMapSize () {
	return 0;
    }
    virtual unsigned __int64 getClusterSize () {
	return containerSize ();
    }
    virtual unsigned int getPOPCount () const {
	return 0;
    }
    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const {
	return false;
    }

//  State
protected:
    M_CPreamble*		m_pContainer;
    M_ASD*			m_pASD;
private:
    M_POP_Pointer		m_pContainerIdentity;
    M_POP			m_iContainerIdentity;
    M_DCTE_Pointer		m_pDCTE;
    M_CPD_Pointer		m_pCPDChainHead;
    Vdd::Store::Pointer		m_pStore;
    M_RTD *const		m_pRTD;  // ... must follow container address.
    GCState<M_AND>		m_iGCState;
    bool			m_bReadWrite;
    bool			m_bPrecious;
};


/***************************
 *----  VStoreHandle_  ----*
 ***************************/

/****************************************************************************
 *>>>>  For the benefit of MSVC 6, which has problems with such things  <<<<*
 ****************************************************************************
 *                                                                          *
 *                           >>>>>  WALK  <<<<<                             *
 *                                                                          *
 ****************************************************************************/
template<class Base> class VStoreHandleBase_ : public Base {
protected:
    VStoreHandleBase_(
	M_ASD *pSpace, RTYPE_Type xType, size_t sContainer
    ) : Base (pSpace, xType, sContainer) {
    }
    VStoreHandleBase_(M_ASD *pSpace, RTYPE_Type xType) : Base (pSpace, xType) {
    }
    VStoreHandleBase_(M_CTE &rCTE) : Base (rCTE) {
    }
    ~VStoreHandleBase_() {
    }
};

/****************************************************************************
 *                                                                          *
 *                           >>>>>  CHEW  <<<<<                             *
 *                                                                          *
 ****************************************************************************/
template<class Base> class VStoreHandle_ : public VStoreHandleBase_<Base>, public Vdd::Store {
    DECLARE_ABSTRACT_RTT (VStoreHandle_<Base>, VStoreHandleBase_<Base>);

//  Aliases
public:
    typedef VStoreHandleBase_<Base> HandleBase;
    typedef Vdd::Store StoreBase;

    typedef typename StoreBase::Visitor Visitor;

//  Construction
protected:
    VStoreHandle_(
	M_ASD *pSpace, RTYPE_Type xType, size_t sContainer
    ) : HandleBase (pSpace, xType, sContainer), StoreBase (this) {
    }
    VStoreHandle_(M_ASD *pSpace, RTYPE_Type xType) : HandleBase (pSpace, xType), StoreBase (this) {
    }
    VStoreHandle_(M_CTE &rCTE) : HandleBase (rCTE), StoreBase (this) {
    }

//  Destruction
protected:
    ~VStoreHandle_() {
    }

//  Aligment
public:
    bool align () {
	return static_cast<HandleBase*>(this)->align ();
    }
    bool alignAll (bool bCleaning = true) {
	return static_cast<HandleBase*>(this)->alignAll (bCleaning);
    }

//  Attention Mask
private:
    unsigned int attentionMask_() const;
    virtual /*override*/ void setAttentionMaskTo_(unsigned int iValue);
public:
    unsigned int attentionMask () const {
	return static_cast<HandleBase const*>(this)->attentionMask ();
    }
    void setAttentionMaskTo (unsigned int iValue) {
	static_cast<HandleBase*>(this)->setAttentionMaskTo (iValue);
    }

//  Database Access
private:
    virtual /*override*/ M_AND *database_() const;
    virtual /*override*/ M_KOT *kot_() const;
    virtual /*override*/ M_ASD *objectSpace_() const;
    virtual /*override*/ RTYPE_Type rtype_() const;
    virtual /*override*/ M_ASD *scratchPad_() const;

//  Database Container Access
private:
    void getContainerHandle_(VReference<VContainerHandle>&rpResult);

//  Database Container Management
private:
    bool isAForwardingTarget_() const;
public:
    bool forwardToSpace (M_ASD *pSpace) {
	return static_cast<HandleBase*>(this)->forwardToSpace (pSpace);
    }

//  Store Access
private:
    virtual /*override*/ StoreBase *getStore_();
public:
    bool getStore (VReference<StoreBase> &rpResult) {
	rpResult.setTo (getStore ());
	return rpResult.isntNil ();
    }
    StoreBase *getStore () {
	return static_cast<StoreBase*>(this);
    }

//  Reference Reporting and Visitor Support
public:
    virtual /*override*/ void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const {
	BaseClass::generateReferenceReport (rOutputFile, xLevel);
    }
    virtual /*override*/ void visitReferencesUsing (Visitor *visitor) {
	BaseClass::visitReferencesUsing (visitor);
    }

//  Visitor Support
public:
    virtual /*override*/ void visitUsing (Visitor *visitor) {
	BaseClass::visitUsing (visitor);
    }
};

/************************************************
 *----  VStoreHandle_<Base>::Run Time Type  ----*
 ************************************************/

template<class Base> typename DEFINE_CONCRETE_RTT (VStoreHandle_<Base>);


/*************************************************
 *----  VStoreHandle_<Base>::Attention Mask  ----*
 *************************************************/

template<class Base> unsigned int VStoreHandle_<Base>::attentionMask_() const {
    return static_cast<HandleBase const*>(this)->attentionMask ();
}
template<class Base> void VStoreHandle_<Base>::setAttentionMaskTo_(unsigned int iValue) {
    static_cast<HandleBase*>(this)->setAttentionMaskTo (iValue);
}

/**************************************************
 *----  VStoreHandle_<Base>::Database Access  ----*
 **************************************************/

template<class Base> M_AND *VStoreHandle_<Base>::database_() const {
    return static_cast<HandleBase const*>(this)->Database ();
}
template<class Base> M_KOT *VStoreHandle_<Base>::kot_() const {
    return static_cast<HandleBase const*>(this)->KOT ();
}
template<class Base> M_ASD *VStoreHandle_<Base>::objectSpace_() const {
    return static_cast<HandleBase const*>(this)->Space ();
}
template<class Base> RTYPE_Type VStoreHandle_<Base>::rtype_() const {
    return static_cast<HandleBase const*>(this)->RType ();
}
template<class Base> M_ASD *VStoreHandle_<Base>::scratchPad_() const {
    return static_cast<HandleBase const*>(this)->ScratchPad ();
}

/************************************************************
 *----  VStoreHandle_<Base>::Database Container Access  ----*
 ************************************************************/

template<class Base> void VStoreHandle_<Base>::getContainerHandle_(
    VReference<VContainerHandle>&rpResult
) {
    rpResult.setTo (static_cast<BaseClass*>(this));
}

/****************************************************************
 *----  VStoreHandle_<Base>::Database Container Management  ----*
 ****************************************************************/

template<class Base> bool VStoreHandle_<Base>::isAForwardingTarget_() const {
    return static_cast<HandleBase const*>(this)->isAForwardingTarget ();
}

/***********************************************
 *----  VStoreHandle_<Base>::Store Access  ----*
 ***********************************************/

template<class Base> typename VStoreHandle_<Base>::StoreBase *VStoreHandle_<Base>::getStore_() {
    return getStore ();
}

/***********************************
 *----  VStoreContainerHandle  ----*
 ***********************************/

typedef VStoreHandle_<VContainerHandle> VStoreContainerHandle;


/*****************************************
 *****  Deferred Inline Definitions  *****
 *****************************************/

bool M_AND::ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEObjectHandle (pReference, pKOTM)->Names (this, pReference);
}

bool M_AND::ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) {
    return ReferencedKOTEObjectHandle (pReference, pKOTM)->DoesntName (this, pReference);
}

void M_AND::StoreReference (M_POP *pReference, VContainerHandle *pThat) {
    StoreReference (pReference, pThat->Space (), pThat->containerIdentity ());
}


#endif
