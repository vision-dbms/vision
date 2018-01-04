#ifndef M_AND_Interface
#define M_AND_Interface

/************************
 *****  Components  *****
 ************************/

#include "VDatabaseActivation.h"
#include "VDatabaseFederatorForBatchvision.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "rtype.h"

#include "PS_AND.h"

#include "M_KnownObjectTable.h"

class DSC_Descriptor;

class M_ASD;
class M_CPD;
class M_TOP;

class rtDSC_Handle;
class rtPTOKEN_Handle;

class VContainerHandle;

class VDatabaseFederatorForBatchvision;

namespace Vdd {
    class Store;
}


/*************************
 *****  Definitions  *****
 *************************/

/*****************************************
 *****  Accessed Network Descriptor  *****
 *****************************************/

/*---------------------------------------------------------------------------
 * An 'Accessed Network Descriptor' holds the state needed to describe the
 * memory manager's access to an object network.
 *
 *  Accessed Network Descriptor Field Descriptions:
 *	m_pPhysicalAND		- the address of the physical network
 *				  associated with this logical network.
 *	m_pKnownObjectTable	- the address of the known object table
 *				  associated with this network.
 *	m_ASDRing		- the address of the first ASD in a ring of
 *				  logical ASDs associated with this network.
 *	m_iASDVector		- an array of ASD pointers used to translate
 *				  space indices into ASDs.
 *****/

class M_AND : public VDatabaseActivation {
//  Run Time Type
    DECLARE_CONCRETE_RTT (M_AND, VDatabaseActivation);

//  Friends
    friend class M_ASD;
    friend class M_TOP;

    friend class PS_AND;

    friend class VDatabaseFederatorForBatchvision;

//  Aliases
public:
    typedef V::VArgList VArgList;

//  GC Metrics
public:
    class GCMetrics : public VTransient {
	friend class M_AND;

    //  Per Space Metrics
    public:
	class SpaceMetrics : public VTransient {
	    friend class GCMetrics;

	public:
	    class ReferenceCounts {
		friend class SpaceMetrics;

	    //  Initialization
	    public:
		void initialize () {
		    m_cFirstVisits = m_cReturnVisits = 0;
		}

	    //  Access/Query
	    public:
		ReferenceCounts &operator += (ReferenceCounts const &rOther) {
		    m_cFirstVisits += rOther.m_cFirstVisits;
		    m_cReturnVisits += rOther.m_cReturnVisits;

		    return *this;
		}

		bool isntZero () const {
		    return m_cFirstVisits > 0;
		}
		unsigned int firstVisitCount () const {
		    return m_cFirstVisits;
		}
		unsigned int returnVisitCount () const {
		    return m_cReturnVisits;
		}

	    //  Update
	    public:
		void recordReference (bool bFirstVisit) {
		    if (bFirstVisit)
			m_cFirstVisits++;
		    else
			m_cReturnVisits++;
		}
		void recordSelfReferences (unsigned int cSelfReferences) {
		    m_cFirstVisits += cSelfReferences;
		}

	    //  State
	    private:
		unsigned int m_cFirstVisits;
		unsigned int m_cReturnVisits;
	    };

	//  Construction
	protected:
	    SpaceMetrics (GCMetrics *pDBMetrics, unsigned int xSpace);

	//  Destruction
	protected:
	    ~SpaceMetrics ();

	//  Access
	public:
	    void accumulateCounts (ReferenceCounts &rCounts) const;

	    unsigned int passCount () const {
		return m_cPasses;
	    }
	    unsigned int firstVisitCount () const;
	    unsigned int returnVisitCount () const;

	//  Update
	public:
	    void recordPass () {
		m_cPasses++;
	    }
	    void recordReference (unsigned int xSpace, bool bFirstVisit) {
		m_pReferenceCounts[xSpace].recordReference (bFirstVisit);
	    }
	    void recordSelfReferences (unsigned int cSelfReferences) {
		recordPass ();
		m_pReferenceCounts[m_xSpace].recordSelfReferences (cSelfReferences);
	    }

	//  Display
	public:
	    void displayMetrics (M_AND *pAND) const;

	//  State
	private:
	    GCMetrics*		const	m_pDBMetrics;
	    unsigned int	const	m_xSpace;
	    ReferenceCounts*	const	m_pReferenceCounts;
	    unsigned int		m_cPasses;
	};
	typedef SpaceMetrics::ReferenceCounts ReferenceCounts;

    //  Construction
    protected:
	GCMetrics (unsigned int cSpaces);

    //  Destruction
    protected:
	~GCMetrics ();

    //  Access
    public:
	void accumulateCounts (ReferenceCounts &rCounts) const;

	unsigned int firstVisitCount () const;
	unsigned int returnVisitCount () const;

	unsigned int spaceCount () const {
    	    return m_cSpaceMetrics;
	}
	SpaceMetrics *spaceMetrics (unsigned int xSpace) const {
	    return m_pSpaceMetrics[xSpace];
	}

    //  Display
    public:
	void displayMetrics (M_AND *pAND) const;

    //  State
    private:
	unsigned int	const	m_cSpaceMetrics;
	SpaceMetrics**	const	m_pSpaceMetrics;
    };

//  Update Output Handler
public:
    typedef void (__cdecl *UpdateOutputHandler) (void *pData, char const *pFormat, ...);

//  Construction
public:
    /*---------------------------------------------------------------------------
     * Arguments:
     *	pDatabaseNDF		- a pointer to the NDF being accessed.
     *	versionSpecification	- an optional character string (Nil if absent)
     *				  specifying the network version to open.
     *				  Valid forms are:
     *
     *					n	... (n > 0) open absolute
     *						    network version 'n'.
     *					-n	... open the 'n-th' version
     *						    before the current version.
     *					n-m	... (n > 0) open the 'm-th'
     *						    version before absolute
     *						    network version 'n'.
     *
     *****/
    M_AND (
	VDatabaseFederatorForBatchvision*	pDatabaseFederator,
	VDatabaseNDF*				pDatabaseNDF,
	char const*				pVersionSpec,
	bool					makingNewNDF
    );

    void AccessTheTLE (
	unsigned int xSpace, DSC_Descriptor &rTLE, VReference<rtDSC_Handle> *ppTLEDescriptor = 0
    );

    void CreateBootStrapObjects ();

    void RegisterObject (
	M_CPD *pObject, M_KOTE M_KOT::*pKOTE, M_POP ENVIR_BuiltInObjectPArray::*pKOTCE
    ) const {
	m_pKnownObjectTable->RegisterObject (pObject, pKOTE, pKOTCE);
    }

//  Destruction
protected:
    ~M_AND ();

//  Access
public:
    unsigned int ActiveSpaceCount () const {
	return m_cActiveSpaces;
    }

    VDatabase *database_() const;

    M_KOT *KOT () const {
	return m_pKnownObjectTable;
    }

    VkLicense &License () const {
	return m_pPhysicalAND->License ();
    }

    char const *NDFPathName () const {
	return m_pPhysicalAND->NDFPathName ();
    }
    char const *OSDPathName () const {
	return m_pPhysicalAND->OSDPathName ();
    }

    PS_AND *PhysicalAND () const {
	return m_pPhysicalAND;
    }

    unsigned int SpaceCount () const {
	return m_pPhysicalAND->SpaceCount ();
    }

    M_POP const &TheNilPOP () const {
	return m_iNilPOP;
    }

    bool TracingContainerMarking () const {
	return m_pPhysicalAND->TracingCompaction ();
    }

    VString const &UpdateAnnotation () const {
	return m_pPhysicalAND->UpdateAnnotation ();
    }

//  Update
public:
    void IncrementAllocationLevel (size_t sIncrease) {
	m_pFederator->IncrementAllocationLevel (sIncrease);
    }
    void DecrementAllocationLevel (size_t sDecrease) {
	m_pFederator->DecrementAllocationLevel (sDecrease);
    }


//  Known Object Access
public:
    M_KOTE const &GetBlockEquivalentClassFromPToken (rtPTOKEN_Handle const *pHandle) const {
	return m_pKnownObjectTable->GetBlockEquivalentClassFromPToken (pHandle);
    }

    M_KOTE const &TheNAClass () const {
	return m_pKnownObjectTable->TheNAClass;
    }
    M_KOTE const &TheNoValueClass () const {
	return m_pKnownObjectTable->TheNoValueClass;
    }
    M_KOTE const &TheTrueClass () const {
	return m_pKnownObjectTable->TheTrueClass;
    }
    M_KOTE const &TheFalseClass () const {
	return m_pKnownObjectTable->TheFalseClass;
    }
    M_KOTE const &TheCharacterClass () const {
	return m_pKnownObjectTable->TheCharacterClass;
    }
    M_KOTE const &TheDateClass () const {
	return m_pKnownObjectTable->TheDateClass;
    }
    M_KOTE const &TheDoubleClass () const {
	return m_pKnownObjectTable->TheDoubleClass;
    }
    M_KOTE const &TheFloatClass () const {
	return m_pKnownObjectTable->TheFloatClass;
    }
    M_KOTE const &TheIntegerClass () const {
	return m_pKnownObjectTable->TheIntegerClass;
    }
    M_KOTE const &ThePrimitiveClass () const {
	return m_pKnownObjectTable->ThePrimitiveClass;
    }
    M_KOTE const &TheClosureClass () const {
	return m_pKnownObjectTable->TheClosureClass;
    }
    M_KOTE const &TheFixedPropertyClass () const {
	return m_pKnownObjectTable->TheFixedPropertyClass;
    }
    M_KOTE const &TheIndexedListClass () const {
	return m_pKnownObjectTable->TheIndexedListClass;
    }
    M_KOTE const &TheListClass () const {
	return m_pKnownObjectTable->TheListClass;
    }
    M_KOTE const &TheMethodClass () const {
	return m_pKnownObjectTable->TheMethodClass;
    }
    M_KOTE const &TheSelectorClass () const {
	return m_pKnownObjectTable->TheSelectorClass;
    }
    M_KOTE const &TheStringClass () const {
	return m_pKnownObjectTable->TheStringClass;
    }
    M_KOTE const &TheTimeSeriesClass () const {
	return m_pKnownObjectTable->TheTimeSeriesClass;
    }
    M_KOTE const &TheClosureClassDictionary () const {
	return m_pKnownObjectTable->TheClosureClassDictionary;
    }
    M_KOTE const &TheMethodClassDictionary () const {
	return m_pKnownObjectTable->TheMethodClassDictionary;
    }
    M_KOTE const &TheFixedPropertyClassDictionary () const {
	return m_pKnownObjectTable->TheFixedPropertyClassDictionary;
    }
    M_KOTE const &TheTimeSeriesClassDictionary () const {
	return m_pKnownObjectTable->TheTimeSeriesClassDictionary;
    }
    M_KOTE const &TheListClassDictionary () const {
	return m_pKnownObjectTable->TheListClassDictionary;
    }
    M_KOTE const &TheIndexedListClassDictionary () const {
	return m_pKnownObjectTable->TheIndexedListClassDictionary;
    }
    M_KOTE const &TheStringClassDictionary () const {
	return m_pKnownObjectTable->TheStringClassDictionary;
    }
    M_KOTE const &TheSelectorClassDictionary () const {
	return m_pKnownObjectTable->TheSelectorClassDictionary;
    }
    M_KOTE const &TheObjectPrototype () const {
	return m_pKnownObjectTable->TheObjectPrototype;
    }
    M_KOTE const &TheScalarPToken () const {
	return m_pKnownObjectTable->TheScalarPToken;
    }
    M_KOTE const &TheSelectorPToken () const {
	return m_pKnownObjectTable->TheSelectorPToken;
    }
    rtPTOKEN_Handle *TheScalarPTokenHandle () const {
	return m_pKnownObjectTable->TheScalarPTokenHandle ();
    }
    rtPTOKEN_Handle *TheSelectorPTokenHandle () const {
	return m_pKnownObjectTable->TheSelectorPTokenHandle ();
    }

//  Space Access
public:// protected:
    M_ASD *CreateASD (unsigned int xSpace, bool aMissingSpaceIsAnError);

public:
    M_ASD *AccessASD (unsigned int xSpace, bool aMissingSpaceIsAnError = true) {
	M_ASD *pASD = m_iASDVector[xSpace];
	return pASD ? pASD : CreateASD (xSpace, aMissingSpaceIsAnError);
    }
    M_ASD *AccessASD (M_POP const* pPOP, bool aMissingSpaceIsAnError = true) {
	return AccessASD (M_POP_ObjectSpace (pPOP), aMissingSpaceIsAnError);
    }

    bool AccessSpace (M_ASD *&rpResult, unsigned int xSpace);
    bool AccessSpace (M_ASD *&rpResult, M_POP const *pPOP) {
	return AccessSpace (rpResult, M_POP_ObjectSpace (pPOP));
    }

    M_ASD *AccessSpace (unsigned int xSpace, bool *pfSpaceValid);
    M_ASD *AccessSpace (M_POP const *pPOP, bool *pfSpaceValid) {
	return AccessSpace (M_POP_ObjectSpace (pPOP), pfSpaceValid);
    }

    M_ASD *ScratchPad () const {
	return m_iASDVector [M_KnownSpace_ScratchPad];
    }

//  Space Enumeration
public:
    void __cdecl EnumerateSpaces (bool (M_ASD::*elementProcessor) (VArgList const&), ...);

    void __cdecl EnumerateAccessedSpaces (
	bool (M_ASD::*elementProcessor) (VArgList const&), ...
    ) const;
    void __cdecl EnumerateModifiedSpaces (
	bool (M_ASD::*elementProcessor) (VArgList const&), ...
    ) const;

//  Container Access
public:
    M_CPreamble* GetContainerAddress (M_POP const *pReference);

    VContainerHandle* GetContainerHandle (
	M_POP const *pReference, RTYPE_Type xExpectedType = RTYPE_C_Any
    );

    VContainerHandle *GetContainerHandle (
	unsigned int xSpace, unsigned int xContainer, RTYPE_Type xExpectedType = RTYPE_C_Any
    );

    M_CPD *GetCPD (M_POP const *pReference, RTYPE_Type xExpectedType = RTYPE_C_Any);
    M_CPD *GetCPD (
	unsigned int xSpace, unsigned int xContainer, RTYPE_Type xExpectedType = RTYPE_C_Any
    );

    bool ContainerExists (M_POP const *pReference);
    bool ContainerPersists (M_POP const *pReference);

    VContainerHandle *SafeGetContainerHandle (M_POP const *pReference);
    VContainerHandle *SafeGetContainerHandle (M_POP const *pReference, RTYPE_Type xType);

//  Container Maintenance
public:
    bool align (M_POP const *pReference);
    bool alignAll (M_POP const *pReference, bool bCleaning = true);

//  Reference Access
public:
    M_AND *ReferencedNetwork (M_POP const *pReference);

    M_KOT *ReferencedKOT (M_POP const *pReference) {
	return ReferencedNetwork (pReference)->KOT ();
    }

    M_KOTE const &ReferencedKOTE (M_POP const *pReference, M_KOTM pKOTM) {
	return ReferencedKOT (pReference)->*pKOTM;
    }

    M_CPD *ReferencedKOTEObjectCPD (M_POP const *pReference, M_KOTM pKOTM) {
	return ReferencedKOTE (pReference, pKOTM).ObjectCPD ();
    }

    VContainerHandle *ReferencedKOTEObjectHandle (M_POP const *pReference, M_KOTM pKOTM) {
	return ReferencedKOTE (pReference, pKOTM).ObjectHandle ();
    }
    rtPTOKEN_Handle *ReferencedKOTEPTokenHandle (M_POP const *pReference, M_KOTM pKOTM) {
	return ReferencedKOTE (pReference, pKOTM).PTokenHandle ();
    }

//  Reference Testing
protected:
    bool ReferencesAreIdentical_(
	M_POP const *pReference, M_AND *pThat, M_POP const *pThatReference
    );

public:
    bool ReferencesAreIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) {
	return M_POP_AsInt (pReference1) == M_POP_AsInt (pReference2)
	    || ReferencesAreIdentical_(pReference1, this, pReference2);
    }
    bool ReferencesArentIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) {
	return M_POP_AsInt (pReference1) != M_POP_AsInt (pReference2)
	    && !ReferencesAreIdentical_(pReference1, this, pReference2);
    }

    inline bool ReferenceNames (M_POP const *pReference, M_KOTM pKOTM);

    bool ReferenceNames (
	M_POP const *pReference, M_AND *pThat, M_POP const *pThatReference
    ) {
	return (
	    this == pThat && M_POP_AsInt (pReference) == M_POP_AsInt (pThatReference)
	) || ReferencesAreIdentical_(pReference, pThat, pThatReference);
    }

    inline bool ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM);

    bool ReferenceDoesntName (
	M_POP const *pReference, M_AND *pThat, M_POP const* pThatReference
    ) {
	return (
	    this != pThat || M_POP_AsInt (pReference) != M_POP_AsInt (pThatReference)
	) && !ReferencesAreIdentical_(pReference, pThat, pThatReference);
    }

    bool ReferenceIsNil (M_POP const *pReference) {
	return M_POP_AsInt (pReference) == M_POP_D_AsInt (m_iNilPOP);
    }
    bool ReferenceIsntNil (M_POP const *pReference) {
	return M_POP_AsInt (pReference) != M_POP_D_AsInt (m_iNilPOP);
    }

//  Reference Management
public:
    void constructReference (M_POP *pReference) const {
	*pReference = m_iNilPOP;
    }
    void constructReferences (M_POP *pReferences, unsigned int cReferences) const {
	while (cReferences--)
	    *pReferences++ = m_iNilPOP;
    }

    void Release (M_POP const *pReference);
    void Release (M_POP const *pReferences, unsigned int cReferences);

    void ClearReference (M_POP *pReference) {
	Release (pReference);
	constructReference (pReference);
    }
    void ClearReferences (M_POP *pReferences, unsigned int cReferences);

private:
    bool LocateName (M_ASD *pThat, unsigned int xThatName, M_TOP &rName);
    bool CreateName (M_ASD *pThat, unsigned int xThatName, M_TOP &rName);

public:
    inline bool LocateNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName);
    inline bool LocateNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName);

    inline bool LocateOrAddNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName);
    inline bool LocateOrAddNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName);

    void StoreReference (M_POP *pReference, M_ASD *pThatSpace, M_POP const *pThatName);
    inline void StoreReference (M_POP *pReference, VContainerHandle *pHandle);
    void StoreReference (M_POP *pReference, Vdd::Store *pThat);

//  Database Update
private:
    void PersistReferences ();
    void PersistStructures ();

public:
    bool HasTransientReferences () const;

    PS_UpdateStatus IncorporateExternalUpdate (char const *xuSpecPathName);

    PS_UpdateStatus SaveTheTLE (
	void *pOutputHandlerData, UpdateOutputHandler pOutputHandler
    );

    PS_UpdateStatus UpdateNetwork (bool globalUpdate);

//  Database Update Status
public:
    void SetUpdateStatusTo (PS_UpdateStatus xUpdateStatus) const {
	m_pPhysicalAND->SetUpdateStatusTo (xUpdateStatus);
    }

    bool UpdateIsWell () const {
	return m_pPhysicalAND->UpdateIsWell ();
    }
    bool UpdateIsntWell () const {
	return m_pPhysicalAND->UpdateIsntWell ();
    }

//  Garbage Collection
private:
    GCMetrics::SpaceMetrics *GCMetricsForSpace (unsigned int xSpace) const {
	return m_pGCMetrics->spaceMetrics (xSpace);
    }

    void InitializeGCMetrics	();

    void PerformMarkPhaseOfGC	();
    bool PerformSweepPhaseOfGC	();

    bool DoGCCycleElimination   ();
    bool EnqueuePossibleCycles  ();
    bool TraverseAndDetectCycles();
    bool EnqueueOmittingCycles  ();

    void DoTransientGCSetup	();
    bool DoTransientGCMarking	();
    bool DoTransientGCSweep	();

public:
    void DisplayGCMetrics	();

    bool DisposeOfNetworkGarbage ();

    typedef unsigned int GenIndex_t;

    static GenIndex_t CurrentGeneration () {
	return g_xGCGeneration;
    }

    static bool GarbageCollectionRunning () {
	return g_bGarbageCollectionRunning;
    }
    static bool NetworkGarbageCollectedInSession () {
	return g_bNetworkGarbageCollectedInSession;
    }

private:
    static void OnGCStart () {
	g_xGCGeneration++;
	g_bGarbageCollectionRunning = true;
    }
    static void OnGCFinish () {
	g_bGarbageCollectionRunning = false;
    }

    static GenIndex_t g_xGCGeneration;
    static bool g_bGarbageCollectionRunning;
    static bool g_bNetworkGarbageCollectedInSession;

//  Settings Control
public:
    void SetDoingCompactionTo (bool iValue) const {
	m_pPhysicalAND->SetDoingCompactionTo (iValue);
    }
    void SetMakingBaseVersionTo (bool iValue) const {
	m_pPhysicalAND->SetMakingBaseVersionTo (iValue);
    }

    void SetTracingCompactionTo (bool iValue) const {
	m_pPhysicalAND->SetTracingCompactionTo (iValue);
    }
    void SetTracingUpdateTo (bool iValue) const {
	m_pPhysicalAND->SetTracingUpdateTo (iValue);
    }

    void SetUpdateAnnotationTo (char const* pValue) const {
	m_pPhysicalAND->SetUpdateAnnotationTo (pValue);
    };

    void SetValidatingChecksumsTo (bool iValue) const {
	m_pPhysicalAND->SetValidatingChecksumsTo (iValue);
    }

//  Resource Utilization Query
public:
    void AccumulateAllocationStatistics (
	unsigned __int64 *pAllocationTotal, unsigned __int64 *pMappingTotal
    ) const;

//  Resource Utilization Management
public:
    void FlushCachedResources ();

    unsigned int ReclaimSegments () {
	return m_pPhysicalAND->ReclaimSegments ();
    }
    unsigned int ReclaimAllSegments () {
	return m_pPhysicalAND->ReclaimAllSegments ();
    }

//  Logging
public:
    void VLogError (char const* format, VArgList const &rArgList);

//  State
protected:
    PS_AND *const	m_pPhysicalAND;
    unsigned int	m_cActiveSpaces;
    VReference<M_KOT>	m_pKnownObjectTable;
    M_POP		m_iNilPOP;
    GCMetrics*		m_pGCMetrics;
    M_ASD*		m_pASDRing;
    M_ASD*		m_iASDVector [M_POP_MaxObjectSpace];
};

PublicVarDecl VReference<M_AND> M_AttachedNetwork;


#endif
