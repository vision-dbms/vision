#ifndef M_ASD_Interface
#define M_ASD_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "M_AND.h"

#include "PS_ASD.h"

class M_CTE;

class rtPCT_Handle;


/*************************
 *****  Definitions  *****
 *************************/

/****************************************
 *>>>>  Container Table Structures  <<<<*
 ****************************************/
/*---------------------------------------------------------------------------
 * The container table structure declarations which follow describe the
 * transient portions of container tables.  The persistent portions of
 * container tables are defined in "ps.d".
 *---------------------------------------------------------------------------
 */

/***********************************
 *****  Container Table Entry  *****
 ***********************************/

/*---------------------------------------------------------------------------
 * Transient container table entries contain a 'tag' and an 'address'.  The
 * 'tag' holds reference count, transient reference and address interpretation
 * information.  The 'address' points to the container, possibly through the
 * indirection of a CPCC or another CTE.  The following descriptions define
 * the components of the 'tag' and 'address' fields of a container table entry:
 *
 *  Container Table Entry Field Descriptions:
 *	referenceCount		- the reference count of the container.  When
 *				  0, this container table entry is unused.  In
 *				  that case, this container table entry has
 *				  address type 'M_CTEAddressType_ForwardingPOP'
 *				  and an address which is the index of the next
 *				  free entry in this container table.
 *	addressType		- the type of address contained in the address
 *				  component of this CTE chosen from one of the
 *				  following:
 *					'M_CTEAddressType_ROContainer'
 *					'M_CTEAddressType_RWContainer'
 *					'M_CTEAddressType_CPCC'
 *					'M_CTEAddressType_ForwardingPOP'
 *	attentionMask		- an experimental field introduced with the
 *				  the debugger used to trigger breakpoints.
 *				  This field is recorded, but not used, by
 *				  the container management sub-system.
 *	isAForwardingTarget	- a boolean that indicates if this container
 *				  has been forwarded from another space.  This
 *				  flag exists so that operations that build
 *				  structures that depend on POP comparisons
 *				  (e.g., vector u-segment indices) can adjust
 *				  their POP comparison algorithms to detect
 *				  forwarding aliases when appropriate.
 *	isATRefRescanTarget	- a boolean that indicates that this container
 *				  needs to be examined on passes 2 and above
 *				  of the transient reference elimination
 *				  algorithm.  This flag is set along with
 *				  'isAForwardingTarget' and cleared by the
 *				  transient reference elimination algorithm;
 *				  see 'M_ASD::EliminateTransientReferences'
 *				  for more information.
 *	isNew			- a boolean flag which, when true, indicates
 *				  that this entry is new (i.e., has never been
 *				  written to persistent storage).
 *	underConstruction	- a boolean flag which, when true, indicates
 *				  that this entry is been allocated, but that
 *				  the container it will point to is in the process
 *				  of being created.
 *	hasBeenUnmapped		- a boolean flag that indicates whether the
 *				  container associated with this entry has
 *				  been unmapped at least once by a segment
 *				  reclamation scan.  This flag supports the
 *				  incremental compactor implemented in version
 * 				  5.6.  The incremental compactor computes
 *				  segment utilization statistics similar to the
 *				  full compactor but based only on the current
 *				  network version.  To optimize performance,
 *				  these statistics are maintained by the access
 *				  manager (i.e., 'PS_ASD::AccessContainer').  This
 *				  flag is passed to 'PS_ASD::AccessContainer' to
 *				  designate remappings of the same container
 *				  which should not be double counted in the
 *				  segment access statistics.
 *	mustBeRemapped		- a boolean flag used to mark the containers
 *				  written during the segment creation pass of
 *				  an update.  These containers can be re-mapped
 *				  to their new persistent addresses at the end
 *				  of a successful update.
 *
 *  Container Table Entry Address Field Descriptions:
 *	asContainerAddress	- the address of the preamble of the container
 *				  identified by this container table entry.
 *				  Applicable to containers not currently in
 *				  the access set of this process.
 *	asCPCC			- the address of a CPCC pointing to the CPDs
 *				  currently attached to this container.
 *	asPOP			- a POP identifying the forwarding address of
 *				  this container.
 *	asCTI			- the index of the next free container in
 *				  the container table.  Negative indices
 *				  mark the end of the free list.
 *
 *---------------------------------------------------------------------------
 */


/*****
 *****  CTE Address Types
 *****/

#define M_CTEAddressType_ROContainer	0	/*  Must be 0  */
#define M_CTEAddressType_RWContainer	1	/*  Must be 1  */
#define M_CTEAddressType_CPCC		2
#define M_CTEAddressType_ForwardingPOP	3

/*****
 *****  CTE Reference Count Maximum
 *****/

#define M_CTE_MaxReferenceCount	USHRT_MAX


/*****************
 *>>>>  ASD  <<<<*
 *****************/

/*---------------------------------------------------------------------------
 * An Accessed Space Descriptor holds the state associated with M's access to
 * an object space.
 *
 *  Accessed Space Descriptor Field Descriptions:
 *	m_pAND			- the address of the access descriptor for the
 *				  database that contains this space.
 *	m_xSpace		- the index of this space in its database.
 *	m_pSuccessor		- the address of the next accessed space
 *				  descriptor in the chain of accessed space
 *				  descriptors maintained by the memory
 *				  manager.
 *	m_pPASD			- the address of the persistent ASD associated
 *				  with this object space.  ('Nil' for transient
 *				  object spaces (i.e., object space 0)).
 *	m_iReadWriteRegion	- the container table index bounds of this
 *				  space's read/write containers.
 *	m_iTRefRescanRegion	- the container table index bounds of this
 *				  space's transient reference rescan targets.
 *	m_iLiveReferenceRegion	- the container table index bounds of this
 *				  space's entries with live pointers.
 *	m_pGCQueue		- garbage collection management
 *	m_sTransientAllocation	- an approximate count of the number of
 *				  bytes currently allocated to private
 *				  containers. 
 *	m_pPCTCPCC		- the address of a CPCC for the persistent
 *				  portion of the container table associated
 *				  with this space.
 *	m_iCT			- the address of the transient portion of the
 *				  container table associated with this space.
 *
 *****/
class M_ASD : public VTransient {
    DECLARE_FAMILY_MEMBERS (M_ASD, VTransient);

//  Friends
    friend class M_CTE;

    friend class PS_ASD;
    friend class PS_AND;

    friend class VContainerHandle;

    class GCVisitBase;

//  Aliases
public:
    typedef V::VArgList VArgList;

//  Container Table
public:
    class CT : public VTransient {
	DECLARE_FAMILY_MEMBERS (CT, VTransient);

	friend class M_ASD;

    //  Region
    public:
	class Region : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Region, VTransient);

	//  Construction
	public:
	    Region () : m_xLB (UINT_MAX), m_xUB (0) {
	    }

	//  Destruction
	public:

	//  Access/Query
	public:
	    bool isEmpty () const {
		return m_xLB > m_xUB;
	    }
	    bool isntEmpty () const {
		return m_xLB <= m_xUB;
	    }

	    unsigned int lb () const {
		return m_xLB;
	    }
	    unsigned int ub () const {
		return m_xUB;
	    }

	//  Adjustment
	public:
	    void include (unsigned int xElement) {
		if (m_xLB > xElement)
		    m_xLB = xElement;
		if (m_xUB < xElement)
		    m_xUB = xElement;
	    }
	    void reset () {
		m_xLB = UINT_MAX;
		m_xUB = 0;
	    }

	//  State
	private:
	    unsigned int m_xLB;
	    unsigned int m_xUB;
	};

    //  Page
    public:
	class Page : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Page, VTransient);

	//  Entry
	public:
	    class Entry {
	    //  Friends
		friend class CT;
		friend class Page;
		friend class M_ASD;
		friend class M_CTE;
		friend class VContainerHandle;

	    //  Globals
	    public:
		static unsigned int g_iForwardedContainerCount;
		static bool g_bPotentialSessionGarbage;

	    //  Construction/Initialization
	    private:
		void initializeForNewContainer (unsigned int iInitialReferenceCount) {
		    m_iReferenceCount		= (unsigned short)iInitialReferenceCount;
		    m_fIsNew			=
		    m_fUnderConstruction	=
		    m_bGcVisited                =
		    m_bCdVisited                =
		    m_fHasBeenAccessed		= true;
		    m_bFoundAllReferences	=
		    m_fIsAForwardingTarget	=
		    m_fIsATRefRescanTarget	=
		    m_fMustStayMapped		= 
		    m_fMustBeRemapped		= false;
		    m_iAttentionMask		= 0;
		}

		void initializeForOldContainer () {
		    setReferenceCountToInfinity ();
		    m_bGcVisited                =
		    m_bCdVisited                = true;
		    m_bFoundAllReferences       =
		    m_fIsNew			=
		    m_fUnderConstruction	=
		    m_fHasBeenAccessed		=
		    m_fIsAForwardingTarget	=
		    m_fIsATRefRescanTarget	=
		    m_fMustStayMapped		= 
		    m_fMustBeRemapped		= false;
		    m_iAttentionMask		= 0;
		    m_xAddressType = M_CTEAddressType_ROContainer;
		    m_iAddress.asContainerAddress = NilOf (M_CPreamble*);
		}

		void setToContainerAddress (M_CPreamble *pAddress, bool bReadWrite) {
		    m_xAddressType = bReadWrite;
		    m_iAddress.asContainerAddress = pAddress;
		}
		void clearContainerAddress () {
		    m_xAddressType = M_CTEAddressType_ROContainer;
		    m_iAddress.asContainerAddress = 0;
		}

		void setToContainerHandle (VContainerHandle *pHandle) {
		    m_iAddress.asCPCC = pHandle;
		    m_xAddressType = M_CTEAddressType_CPCC;
		    m_fUnderConstruction = false;
		}

		void setToForwardingPOP (M_POP const &iPOP) {
		    g_iForwardedContainerCount++;

		    m_iAddress.asPOP = iPOP;
		    m_xAddressType = M_CTEAddressType_ForwardingPOP;
		}

		void setToFreeListEntry (unsigned int xNextEntry) {
		    m_fHasBeenAccessed = false;
		    m_fUnderConstruction = false;
		    m_iReferenceCount = 0;
		    m_bFoundAllReferences = false;
		    m_iAddress.asCTI = xNextEntry;
		    m_xAddressType = M_CTEAddressType_ForwardingPOP;
		}

	    //  Access/Query
	    public:
		M_CPreamble *addressAsContainerAddress () const {
		    return m_iAddress.asContainerAddress;
		}
		VContainerHandle *addressAsContainerHandle () const {
		    return m_iAddress.asCPCC;
		}
		int addressAsCTI () const {
		    return m_iAddress.asCTI;
		}
		M_POP const &addressAsPOP () const {
		    return m_iAddress.asPOP;
		}

		unsigned int addressType () const {
		    return m_xAddressType;
		}

		unsigned int attentionMask () const {
		    return m_iAttentionMask;
		}

		bool isAForwardingTarget () /*const*/ {
		    if (m_fIsAForwardingTarget && 0 == g_iForwardedContainerCount)
			m_fIsAForwardingTarget = false;
		    return m_fIsAForwardingTarget;
		}
		bool isASweepTarget () const {
		    return isntReferenced () && isntUnderConstruction ();
		}
		bool isATRefRescanTarget () const {
		    return m_fIsATRefRescanTarget;
		}

		bool hasBeenAccessed () const {
		    return m_fHasBeenAccessed;
		}

		bool holdsAReadOnlyContainerAddress () const {
		    return m_xAddressType == M_CTEAddressType_ROContainer;
		}
		bool holdsAWriteableContainerAddress () const {
		    return m_xAddressType == M_CTEAddressType_RWContainer;
		}
		bool holdsAContainerAddress () const {
		    return m_xAddressType < M_CTEAddressType_CPCC;
		}
		bool holdsACPCC () const {
		    return m_xAddressType == M_CTEAddressType_CPCC;
		}
		bool holdsAForwardingPOP () const {
		    return m_xAddressType == M_CTEAddressType_ForwardingPOP;
		}

		bool isNew () const {
		    return m_fIsNew;
		}

		bool underConstruction () const {
		    return m_fUnderConstruction;
		}
		bool isntUnderConstruction () const {
		    return !m_fUnderConstruction;
		}

		bool isReferenced () const {
		    return m_iReferenceCount > 0;
		}
		bool isntReferenced () const {
		    return m_iReferenceCount == 0;
		}

		bool mustBeRemapped () const {
		    return m_fMustBeRemapped;
		}
		bool mustStayMapped () const {
		    return m_fMustStayMapped;
		}

		unsigned int referenceCount () const {
		    return m_iReferenceCount;
		}

		bool referenceCountIsFinite () const {
		    return m_iReferenceCount < M_CTE_MaxReferenceCount;
		}
		bool referenceCountIsInfinite () const {
		    return m_iReferenceCount == M_CTE_MaxReferenceCount;
		}

	    //  Content Maintenance
	    private:
		void setAttentionMaskTo (unsigned int iNewAttentionMask) {
		    m_iAttentionMask = (unsigned short)iNewAttentionMask;
		}

		void setHasBeenAccessed () {
		    m_fHasBeenAccessed = true;
		}
		void clearHasBeenAccessed () {
		    m_fHasBeenAccessed = false;
		}

		void setForwardingTargetFlags () {
		    m_fIsAForwardingTarget = m_fIsATRefRescanTarget = true;
		}

		void setTRefFlags () {
		    m_fIsATRefRescanTarget = true;
		}
		void clearTRefFlags () {
		    m_fIsATRefRescanTarget = false;
		}

		void clearIsNew () {
		    m_fIsNew = false;
		}

		void setMustBeRemapped () {
		    m_fMustBeRemapped = true;
		}
		void clearMustBeRemapped () {
		    m_fMustBeRemapped = false;
		}
		void setMustStayMapped () {
		    m_fMustStayMapped = true;
		}
		void clearMustStayMapped () {
		    m_fMustStayMapped = false;
		}

	    //  Reference Management
	    private:
		//  'mark' returns true for first visits, false for return visits
		bool mark (GCVisitBase*, M_ASD *pASD, unsigned int xContainer);

		void reclaim (M_ASD *pASD, unsigned int xContainer);

	    public:
		void retain () {
		    if (m_iReferenceCount < M_CTE_MaxReferenceCount)
			m_iReferenceCount++;
		}
		void untain () {
		    if (m_iReferenceCount < M_CTE_MaxReferenceCount)
			m_iReferenceCount--;
		}
		void release (M_ASD *pASD, unsigned int xContainer) {
		    if (m_iReferenceCount < M_CTE_MaxReferenceCount) {
			g_bPotentialSessionGarbage = true;
			if (0 == --m_iReferenceCount)
			    reclaim (pASD, xContainer);
		    }
		}
		void discard (M_ASD *pASD, unsigned int xContainer) {
		    if (0 == m_iReferenceCount) {
			g_bPotentialSessionGarbage = true;
			reclaim (pASD, xContainer);
		    }
		}
		void discard (M_ASD *pASD, unsigned int xContainer, M_CPreamble *pContainerAddress, bool bReadWrite) {
		    setToContainerAddress (pContainerAddress, bReadWrite);
		    discard (pASD, xContainer);
		}

		void setReferenceCountToZero () {
		    m_iReferenceCount = 0;
		}
		void setReferenceCountToOne () {
		    m_iReferenceCount = 1;
		}
		void setReferenceCountToInfinity () {
		    m_iReferenceCount = M_CTE_MaxReferenceCount;
		}

		bool gcVisited() {
		    return m_bGcVisited;
		}
		void gcVisited(bool visited) {
		    m_bGcVisited = visited;
		}

		bool cdVisited() {
		    return m_bCdVisited;
		}
		void cdVisited (bool visited) {
		    m_bCdVisited = visited;
		}
		bool foundAllReferences() {
		    return m_bFoundAllReferences;
		}
		void foundAllReferences (bool foundAll) {
		    m_bFoundAllReferences = foundAll;
		}

	    //  State
	    private:
		union address_t {
		    M_CPreamble*	asContainerAddress;
		    VContainerHandle*	asCPCC;
		    M_POP		asPOP;
		    int			asCTI;
		} m_iAddress;
		unsigned short		m_iReferenceCount;
		bool                    m_bGcVisited;
                bool                    m_bCdVisited;
                bool                    m_bFoundAllReferences;
		unsigned short		m_xAddressType		: 2,
					m_iAttentionMask	: 3,
					m_fIsAForwardingTarget	: 1,
					m_fIsATRefRescanTarget	: 1,
					m_fIsNew		: 1,
					m_fUnderConstruction	: 1,
					m_fHasBeenAccessed	: 1,
					m_fMustBeRemapped	: 1,
					m_fMustStayMapped	: 1;
	    };

	//  Size
	public:
	    enum { Size = 1024 };

	//  Construction
	public:
	    Page (CT const *pCT, unsigned int xPage);

	//  Destruction
	public:
	    ~Page ();

	//  Access
	public:
	    CT const *ct () const {
		return m_pCT;
	    }
	    Entry *entry (unsigned int xEntry) {
		return m_iDCTE + xEntry % Size;
	    }
	    Entry const *entry (unsigned int xEntry) const {
		return m_iDCTE + xEntry % Size;
	    }
	    unsigned int number () const {
		return m_xPage;
	    }
	    unsigned int origin () const {
		return Size * m_xPage;
	    }
	    unsigned int limit () const {
		return Size * (m_xPage + 1);
	    }

	//  State
	private:
	    CT const*		const	m_pCT;
	    unsigned int	const	m_xPage;
	    Entry			m_iDCTE[Size];
	};

    //  More Friends
	friend class Page;

    //  Entry
    public:
	typedef Page::Entry Entry;

    //  Free List Nil
	enum {FreeListNil = UINT_MAX};

    //  Page Vector Grain
    public:
	enum {PageVectorGrain = 4};

    //  Construction
    private:
	void sizePageVector ();
	void sizePageVector (unsigned int sPageVector);

    public:
	CT (M_ASD *pASD);

    //  Destruction
    public:
	~CT ();

    //  Access/Query
    public:
	unsigned int entryCount () const {
	    return m_cEntries;
	}

	inline char const *ndfPathName () const;

	inline unsigned int persistentEntryCount () const;

	inline unsigned int spaceIndex () const;

    //  Entry Access
    public:
	Entry *entry (unsigned int xEntry) const {
	    return page (xEntry)->entry (xEntry);
	}

    private:
	//  number of pages required to hold m_cEntries entries...
	unsigned int entryPageCount (unsigned int cEntries) const {
	    return (cEntries + Page::Size - 1) / Page::Size;
	}
	unsigned int entryPageCount () const {
	    return entryPageCount (m_cEntries);
	}

	Page *page_(unsigned int xPage) const;
	Page *page (unsigned int xEntry) const {
	    unsigned int xPage = xEntry / Page::Size;
	    Page *pPage = xPage < m_sPageVector ? m_pPageVector[xPage] : 0;
	    return pPage ? pPage : page_(xPage);
	}

    //  Entry Allocation
    public:
	unsigned int entry ();

    private:
	void growFreeList ();

	unsigned int freeListHead () const {
	    return m_xFreeListHead;
	}

    public:
	bool freeListIsEmpty () const {
	    return m_xFreeListHead == FreeListNil;
	}
	bool freeListIsntEmpty () const {
	    return m_xFreeListHead != FreeListNil;
	}

    private:
	void resetFreeList () {
	    m_xFreeListHead = FreeListNil;
	    m_xFreeListBase = m_cEntries;
	}
	void setFreeListHeadTo (unsigned int xCTE) {
	    if (m_xFreeListBase <= xCTE)
		m_xFreeListHead  = xCTE;
	}
	void setFreeListBaseTo (unsigned int xCTE) {
	    m_xFreeListBase = xCTE;
	}

    //  Allocation Accounting
    private:
	inline void incrementSpaceAllocation (size_t sAllocation) const;
	inline void decrementSpaceAllocation (size_t sAllocation) const;

    //  State
    private:
	M_ASD*		const	m_pASD;
	unsigned int		m_cEntries;
	unsigned int		m_xFreeListHead;
	unsigned int		m_xFreeListBase;
	unsigned int		m_sPageVector;
	Page**			m_pPageVector;
    };

//  More friends
    friend class CT::Page::Entry;
    friend class CT::Page;
    friend class CT;

//  CTE
public:
    typedef CT::Entry CTE;

//  Region
public:
    typedef CT::Region Region;

//  GCQueue
public:
    class GCQueue : public VTransient {
	DECLARE_FAMILY_MEMBERS (GCQueue, VTransient);

    // GCQueue::Entry
    public:
	struct Entry {
	    Entry	*next;
	    unsigned int index;
	};

    //  Construction
    public:
	GCQueue (unsigned int cEntries);

    //  Destruction
    public:
	~GCQueue ();

    //  Query
    public:
	bool MarkingComplete () const {
	    return IsNil (m_pHead);
	}
	bool MarkingIncomplete () const {
	    return IsntNil (m_pHead);
	}

    //  Use
    public:
	void		Insert (unsigned int xContainer);
	unsigned int	Remove ();

    //  State
    protected:
	unsigned int m_xStorageCell, m_cStorageCells;
	Entry **m_pStorage;
	Entry  *m_pHead, *m_pTail, *m_pFree;
    };


// GC Traversal Controllers: GCTraverseMarkBase
public:
    class GCVisitBase {
    public:
	void Mark (M_ASD* pASD, M_POP const *pPOP) {
	    Mark_(pASD, pPOP);
	}
	void Mark (M_ASD* pASD, M_POP const *pReferences, unsigned int cReferences) {
	    Mark_(pASD, pReferences, cReferences);
	}

    private: 
	virtual void Mark_ (M_ASD* pASD, M_POP const *pPOP) {
	}
	virtual void Mark_ (M_ASD* pASD, M_POP const *pReferences, unsigned int cReferences) {
	}

    };
    class GCVisitMark : public GCVisitBase {
    private:
	virtual void Mark_ (M_ASD* pASD, M_POP const *pPOP);
	virtual void Mark_ (M_ASD* pASD, M_POP const *pReferences, unsigned int cReferences);
    };
    class GCVisitCycleDetect : public GCVisitBase {
    private:
	virtual void Mark_ (M_ASD* pASD, M_POP const *pPOP);
	virtual void Mark_ (M_ASD* pASD, M_POP const *pReferences, unsigned int cReferences);
    };


//  Construction
public:
    M_ASD (M_AND* pAND, PS_ASD* pPhysicalASD);

//  Destruction
public:
    ~M_ASD ();

//  Access
public:
    unsigned int cteCount () const {
	return m_iCT.entryCount ();
    }
    unsigned int cteFreeListHead () const {
	return m_iCT.freeListHead ();
    }

    M_AND* Database () const {
	return m_pAND;
    }

    void recordGCReference (unsigned int xSpace, bool bFirstVisit) {
	m_pAND->GCMetricsForSpace (m_xSpace)->recordReference (xSpace, bFirstVisit);
    }

    unsigned int Index () const {
	return m_xSpace;
    }

    M_KOT *KOT () const {
	return m_pAND->KOT ();
    }

    PS_ASD *persistentASD () const {
	return m_pPASD;
    }
    PS_CT *persistentCT () const {
	return m_pPASD ? PS_ASD_CT (m_pPASD) : 0;
    }
    PS_ASD *PhysicalASD () const {
	return m_pPASD;
    }
    M_ASD *successor () const {
	return m_pSuccessor;
    }

    M_ASD *ScratchPad () const {
	return m_pAND->ScratchPad ();
    }

    M_POP const &TheNilPOP () const {
	return m_pAND->TheNilPOP ();
    }

    unsigned __int64 TransientAllocation () const {
	return m_sTransientAllocation;
    }

//  Query
public:
    bool IsTheScratchPad () const {
	return M_KnownSpace_ScratchPad == m_xSpace;
    }
    bool IsntTheScratchPad () const {
	return M_KnownSpace_ScratchPad != m_xSpace;
    }

    bool hasActiveContainerReferences () const {
	return m_iLiveReferenceRegion.isntEmpty ();
    }

    bool hasChanged () const {
	return m_iReadWriteRegion.isntEmpty ();
    }
    bool hasntChanged () const {
	return m_iReadWriteRegion.isEmpty ();
    }

    bool hasTransientReferences () const {
	return m_iTRefRescanRegion.isntEmpty ();
    }

    bool TracingContainerMarking () const {
	return m_pAND->TracingContainerMarking ();
    }

//  Container Access
public:
    M_CPreamble *GetContainerAddress (M_POP const *pReference) const {
	return m_pAND->GetContainerAddress (pReference);
    }
    VContainerHandle* GetContainerHandle (
	M_POP const *pReference, RTYPE_Type xExpectedType = RTYPE_C_Any
    ) const {
	return m_pAND->GetContainerHandle (pReference, xExpectedType);
    }

    VContainerHandle* GetContainerHandle (
	unsigned int xContainer, RTYPE_Type xExpectedType = RTYPE_C_Any
    );
    M_CPD* GetCPD (unsigned int xContainer, RTYPE_Type xExpectedType = RTYPE_C_Any);

//  Container Table Entry Access
public:
    CTE *cte (unsigned int xContainer) const {
	return m_iCT.entry (xContainer);
    }

    bool GetCTE (unsigned int xContainer, CTE &rResult) const;
    bool GetCTE (unsigned int xContainer, PS_CTE &rResult) const {
	return IsntNil (this) && m_pPASD->GetCTE (xContainer, rResult);
    }

    bool GetCTE (int xContainer, CTE &rResult) const {
	return xContainer >= 0 && GetCTE ((unsigned int)xContainer, rResult);
    }
    bool GetCTE (int xContainer, PS_CTE &rResult) const {
	return IsntNil (this) && m_pPASD->GetCTE (xContainer, rResult);
    }

    bool GetLiveCTE (unsigned int xContainer, CTE &rResult) const {
	return GetCTE (xContainer, rResult) && rResult.isReferenced ();
    }
    bool GetLiveCTE (unsigned int xContainer, PS_CTE &rResult) const {
	return GetCTE (xContainer, rResult) && PS_CTE_InUse (rResult);
    }
    bool GetLiveCTE (int xContainer, CTE &rResult) const {
	return GetCTE (xContainer, rResult) && rResult.isReferenced ();
    }
    bool GetLiveCTE (int xContainer, PS_CTE &rResult) const {
	return GetCTE (xContainer, rResult) && PS_CTE_InUse (rResult);
    }

//  Container Table Entry Management
public:
    unsigned int allocateCTE () {
	return m_iCT.entry ();
    }

    void retainCTE (unsigned int xContainer) const {
	cte (xContainer)->retain ();
    }
    void untainCTE (unsigned int xContainer) const {
	cte (xContainer)->untain ();
    }
    void releaseCTE (unsigned int xContainer) {
	cte (xContainer)->release (this, xContainer);
    }

private:
    void setFreeListHeadTo (unsigned int xContainer) {
	m_iCT.setFreeListHeadTo (xContainer);
    }

//  Update
public:
    void AdjustRWBounds (unsigned int xCTE);
    void AdjustTRBounds (unsigned int xCTE);
    void AdjustLiveRefBounds (unsigned int xCTE);

    void ResetRWBounds ();
    void ResetTRBounds ();
    void ResetLiveRefBounds ();

    void IncrementAllocation (size_t sIncrease) {
	m_sTransientAllocation += sIncrease;
	m_pAND->IncrementAllocationLevel (sIncrease);
    }
    void DecrementAllocation (size_t sDecrease) {
	m_sTransientAllocation -= sDecrease;
	m_pAND->DecrementAllocationLevel (sDecrease);
    }

private:
    void AlignCT			();

    void EliminateTransientReferences	();
    void EnablePCTModification		();
    void RemapSpace			();
    void ScanSegments			();

    void VisitUnaccessedContainers	();

public:
    void CompelUpdate			();
    void Distinguish			();

//  Reference Access
public:
    M_KOT *ReferencedKOT (M_POP const *pReference) const {
	return m_pAND->ReferencedKOT (pReference);
    }

    M_AND *ReferencedNetwork (M_POP const *pReference) const {
	return m_pAND->ReferencedNetwork (pReference);
    }

//  Reference Testing
public:
    bool ReferencesAreIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pAND->ReferencesAreIdentical (pReference1, pReference2);
    }
    bool ReferencesArentIdentical (	// use for local references only.
	M_POP const *pReference1, M_POP const *pReference2
    ) const {
	return m_pAND->ReferencesArentIdentical (pReference1, pReference2);
    }

    bool ReferenceNames (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pAND->ReferenceNames (pReference, pKOTM);
    }

    bool ReferenceNames (
	M_POP const *pReference, M_AND *pThatSpace, M_POP const *pThatName
    ) const {
	return m_pAND->ReferenceNames (pReference, pThatSpace, pThatName);
    }
    bool ReferenceNames (
	M_POP const *pReference, M_ASD *pThatSpace, M_POP const *pThatName
    ) const {
	return pThatSpace->ReferenceNames (pThatName, m_pAND, pReference);
    }

    bool ReferenceDoesntName (M_POP const *pReference, M_KOTM pKOTM) const {
	return m_pAND->ReferenceDoesntName (pReference, pKOTM);
    }

    bool ReferenceDoesntName (
	M_POP const *pReference, M_AND *pThatSpace, M_POP const *pThatName
    ) const {
	return m_pAND->ReferenceDoesntName (pReference, pThatSpace, pThatName);
    }
    bool ReferenceDoesntName (
	M_POP const *pReference, M_ASD *pThatSpace, M_POP const* pThatName
    ) const {
	return pThatSpace->ReferenceDoesntName (pThatName, m_pAND, pReference);
    }

    bool ReferenceIsNil (M_POP const *pReference) const {
	return m_pAND->ReferenceIsNil (pReference);
    }
    bool ReferenceIsntNil (M_POP const *pReference) const {
	return m_pAND->ReferenceIsntNil (pReference);
    }

//  Reference Management
public:
    void constructReference (M_POP *pReference) const {
	m_pAND->constructReference (pReference);
    }
    void constructReferences (M_POP *pReferences, unsigned int cReferences) const {
	m_pAND->constructReferences (pReferences, cReferences);
    }

    void ClearReference (M_POP *pReference) const {
	m_pAND->ClearReference (pReference);
    }
    void ClearReferences (M_POP *pReferences, unsigned int cReferences) const {
	m_pAND->ClearReferences (pReferences, cReferences);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pAND->LocateNameOf (pThat, xThatName, rName);
    }
    bool LocateNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) const {
	return m_pAND->LocateNameOf (pThat, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool LocateOrAddNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) const {
	return m_pAND->LocateOrAddNameOf (pThat, xThatName, rName);
    }
    bool LocateOrAddNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) const {
	return m_pAND->LocateOrAddNameOf (pThat, pThatName, rName);
    }

    /*-----------------------------------------------------------------------*/
    bool Persist (M_POP *pReference);
    bool Persist (M_POP *pReferences, unsigned int cReferences);

    void Release (M_POP const *pReference) const {
	m_pAND->Release (pReference);
    }
    void Release (M_POP const *pReferences, unsigned int cReferences) const {
	m_pAND->Release (pReferences, cReferences);
    }

    void StoreReference (M_POP *pReference, M_ASD *pThatSpace, M_POP const *pThatName) const {
	m_pAND->StoreReference (pReference, pThatSpace, pThatName);
    }
    void StoreReference (M_POP *pReference, VContainerHandle *pThat) const {
	m_pAND->StoreReference (pReference, pThat);
    }
    void StoreReference (M_POP *pReference, Vdd::Store *pThat) const {
	m_pAND->StoreReference (pReference, pThat);
    }

//  Known Object Access
public:
    M_KOTE const &GetBlockEquivalentClassFromPToken (rtPTOKEN_Handle const *pHandle) const {
	return m_pAND->GetBlockEquivalentClassFromPToken (pHandle);
    }

    M_KOTE const &TheNAClass () const {
	return m_pAND->TheNAClass ();
    }
    M_KOTE const &TheNoValueClass () const {
	return m_pAND->TheNoValueClass ();
    }
    M_KOTE const &TheTrueClass () const {
	return m_pAND->TheTrueClass ();
    }
    M_KOTE const &TheFalseClass () const {
	return m_pAND->TheFalseClass ();
    }
    M_KOTE const &TheCharacterClass () const {
	return m_pAND->TheCharacterClass ();
    }
    M_KOTE const &TheDateClass () const {
	return m_pAND->TheDateClass ();
    }
    M_KOTE const &TheDoubleClass () const {
	return m_pAND->TheDoubleClass ();
    }
    M_KOTE const &TheFloatClass () const {
	return m_pAND->TheFloatClass ();
    }
    M_KOTE const &TheIntegerClass () const {
	return m_pAND->TheIntegerClass ();
    }
    M_KOTE const &ThePrimitiveClass () const {
	return m_pAND->ThePrimitiveClass ();
    }
    M_KOTE const &TheClosureClass () const {
	return m_pAND->TheClosureClass ();
    }
    M_KOTE const &TheFixedPropertyClass () const {
	return m_pAND->TheFixedPropertyClass ();
    }
    M_KOTE const &TheIndexedListClass () const {
	return m_pAND->TheIndexedListClass ();
    }
    M_KOTE const &TheListClass () const {
	return m_pAND->TheListClass ();
    }
    M_KOTE const &TheMethodClass () const {
	return m_pAND->TheMethodClass ();
    }
    M_KOTE const &TheSelectorClass () const {
	return m_pAND->TheSelectorClass ();
    }
    M_KOTE const &TheStringClass () const {
	return m_pAND->TheStringClass ();
    }
    M_KOTE const &TheTimeSeriesClass () const {
	return m_pAND->TheTimeSeriesClass ();
    }
    M_KOTE const &TheClosureClassDictionary () const {
	return m_pAND->TheClosureClassDictionary ();
    }
    M_KOTE const &TheMethodClassDictionary () const {
	return m_pAND->TheMethodClassDictionary ();
    }
    M_KOTE const &TheFixedPropertyClassDictionary () const {
	return m_pAND->TheFixedPropertyClassDictionary ();
    }
    M_KOTE const &TheTimeSeriesClassDictionary () const {
	return m_pAND->TheTimeSeriesClassDictionary ();
    }
    M_KOTE const &TheListClassDictionary () const {
	return m_pAND->TheListClassDictionary ();
    }
    M_KOTE const &TheIndexedListClassDictionary () const {
	return m_pAND->TheIndexedListClassDictionary ();
    }
    M_KOTE const &TheStringClassDictionary () const {
	return m_pAND->TheStringClassDictionary ();
    }
    M_KOTE const &TheSelectorClassDictionary () const {
	return m_pAND->TheSelectorClassDictionary ();
    }
    M_KOTE const &TheObjectPrototype () const {
	return m_pAND->TheObjectPrototype ();
    }
    M_KOTE const &TheScalarPToken () const {
	return m_pAND->TheScalarPToken ();
    }
    M_KOTE const &TheSelectorPToken () const {
	return m_pAND->TheSelectorPToken ();
    }
    rtPTOKEN_Handle *TheScalarPTokenHandle () const {
	return m_pAND->TheScalarPTokenHandle ();
    }
    rtPTOKEN_Handle *TheSelectorPTokenHandle () const {
	return m_pAND->TheSelectorPTokenHandle ();
    }

//  Container Management
public:
    void GCQueueInsert (unsigned int xContainer);
private:
    unsigned int GCQueueRemove () {
	return m_pGCQueue->Remove ();
    }

private:
    void CreateIdentity (M_POP &rResult, VContainerHandle *pHandle);
public:
    static size_t RoundContainerSize (size_t sContainer) {
	return static_cast<size_t>(sContainer + 3 & ~3);
    }

    M_CPreamble *AllocateContainer (RTYPE_Type type, size_t size, unsigned int xContainer);
    M_CPreamble *ReallocateContainer (M_CPreamble *containerAddress, size_t newSize);

    VContainerHandle *CreateContainer (RTYPE_Type xType, size_t sContainer) {
	return CreateContainer (xType, sContainer, 0);
    }
    VContainerHandle *CreateContainer (RTYPE_Type xType, size_t sContainer, VContainerHandle *pHandle);

    void DestroyContainer (M_CPreamble *containerAddress);
private:
    void DeallocateContainer (M_CPreamble *containerAddress);

//  Container Enumeration
public:
    void __cdecl EnumerateContainers (
	bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
    );

    void __cdecl EnumerateActiveContainers (
	bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
    );

    void __cdecl EnumerateModifiedContainers (
	bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
    );

    void __cdecl EnumerateTRefRescanContainers (
	bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
    );

    void __cdecl EnumerateUpdatingContainers (
	bool (VContainerHandle::*elementProcessor) (VArgList const&), ...
    );

//  Space Enumeration Handlers
public:
    bool __cdecl Apply (bool (M_ASD::*elementProcessor) (VArgList const&), ...);

    bool AddressSpaceQuery		(VArgList const &rArgList);
    bool DisplaySpaceMallocInfo		(VArgList const &rArgList);
    bool DisplayRetentionInfoForSpace	(VArgList const &rArgList);
    bool DisplaySpaceMappingInfo	(VArgList const &rArgList);
    bool FlushCachedResources		(VArgList const &rArgList);
    bool HasTransientReferences		(VArgList const &rArgList); //  sets va_arg (pArguments,bool*), returns false when true;
    bool InitializeSpaceForGC		(VArgList const &rArgList);
    bool InitializeSpaceForTransientGC	(VArgList const &rArgList);
    bool EnqueuePossibleCycles	        (VArgList const &rArgList);
    bool EnqueueOmittingCycles	        (VArgList const &rArgList);
    bool TraverseAndDetectCycles	(VArgList const &rArgList);
    bool LockSpace			(VArgList const &rArgList);
    bool MarkContainersInQueue		(VArgList const &rArgList);
    bool SweepUp			(VArgList const &rArgList);
    bool PersistReferences		(VArgList const &rArgList);
    bool PersistStructures		(VArgList const &rArgList);
    bool ValidateSpaceConsistency	(VArgList const &rArgList);

    void ConsiderContainersInQueue(M_ASD::GCVisitBase* pGCV);

//  Database Component Access
public:
    M_ASD* AccessASD (
	M_POP const *pReference, bool aMissingSpaceIsAnError = true
    ) const {
	return m_pAND->AccessASD (pReference, aMissingSpaceIsAnError);
    }

//  Database Update Support
public:
    PS_UpdateStatus SaveTheTLE (
	void *pOutputHandlerData, M_AND::UpdateOutputHandler pOutputHandler
    ) {
	return m_pAND->SaveTheTLE (pOutputHandlerData, pOutputHandler);
    }

    PS_UpdateStatus UpdateNetwork (bool bAdministrativeUpdate) {
	return m_pAND->UpdateNetwork (bAdministrativeUpdate);
    }

    bool UpdateIsWell () const {
	return m_pAND->UpdateIsWell ();
    }
    bool UpdateIsntWell () const {
	return m_pAND->UpdateIsntWell ();
    }

//  Incorporator Support
public:
    void WriteDBUpdateInfo (unsigned int xContainer, bool replace) const {
	m_pPASD->WriteDBUpdateInfo (xContainer, replace);
    }

//  State
private:
    M_AND* const		m_pAND;
    PS_ASD* const		m_pPASD;
    unsigned int const		m_xSpace;
    M_ASD*			m_pSuccessor;
    unsigned __int64		m_sTransientAllocation;
    Region			m_iReadWriteRegion,
				m_iTRefRescanRegion,
				m_iLiveReferenceRegion;
    VReference<rtPCT_Handle>	m_pPCTCPCC;
    GCQueue::Pointer		m_pGCQueue;
    CT				m_iCT;

    static GCVisitMark		m_GCMarker;
    static GCVisitCycleDetect   m_GCCycleDetect;
};


/********************
 *****  M_DCTE  *****
 ********************/

typedef M_ASD::CTE M_DCTE;


/*******************
 *****  M_TOP  *****
 *******************/

/*--------------------------------------------------------------------------*
 *****  M_TOP's (Transient-Persistent Object Pointers) are the fully
 *****	qualified in-session analogs to M_POP's.  Their member variables -
 *****	m_pDatabase, m_xSpace, and m_xContainer - form the full path to a
 *****	container.  Because they remember both the database specific space
 *****	index for their referent as well as an access descriptor for that
 *****  database, they can access container table entries and construct
 *****  POPs with equal ease.  In contrast, M_CTE instances only provide
 *****  direct container table entry access but cannot construct a POP.
 *--------------------------------------------------------------------------*/

class M_TOP {
//  Initialization
public:
    void setTo (M_AND *pAND, unsigned int xSpace, unsigned int xContainer) {
	m_pDatabase = pAND;
	m_xSpace = xSpace;
	m_xContainer = xContainer;
    }

//  Access/Query
public:
    M_DCTE *cte () const {
	return space ()->cte (m_xContainer);
    }

    unsigned int containerIndex () const {
	return m_xContainer;
    }

    M_AND *database () const {
	return m_pDatabase;
    }

    void getPOP (M_POP &rPOP) const {
	M_POP_D_ObjectSpace	(rPOP) = m_xSpace;
	M_POP_D_ContainerIndex	(rPOP) = m_xContainer;
    }
    void getReferencedPOP (M_POP &rPOP) const {
	space ()->retainCTE (m_xContainer);
	getPOP (rPOP);
    }

    bool isAScratchPadName () const {
	return space ()->IsTheScratchPad ();
    }
    bool isntAScratchPadName () const {
	return space ()->IsntTheScratchPad ();
    }

    M_ASD *space () const {
	return m_pDatabase->m_iASDVector[m_xSpace];
    }
    unsigned int spaceIndex () const {
	return m_xSpace;
    }

//  State
private:
    M_AND*		m_pDatabase;
    unsigned int	m_xSpace;
    unsigned int	m_xContainer;
};


/********************************
 *****  Inline Definitions  *****
 ********************************/

/*******************
 *----  M_AND  ----*
 *******************/

bool M_AND::LocateNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) {
    if (this == pThat->Database ()) {
	rName.setTo (this, pThat->Index (), xThatName);
	return true;
    }
    return LocateName (pThat, xThatName, rName);
}

bool M_AND::LocateNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) {
    return LocateNameOf (pThat->AccessASD (pThatName), M_POP_ContainerIndex (pThatName), rName);
}

bool M_AND::LocateOrAddNameOf (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) {
    return LocateNameOf (pThat, xThatName, rName)
	|| CreateName   (pThat, xThatName, rName);
}

bool M_AND::LocateOrAddNameOf (M_ASD *pThat, M_POP const *pThatName, M_TOP &rName) {
    return LocateOrAddNameOf (
	pThat->AccessASD (pThatName), M_POP_ContainerIndex (pThatName), rName
    );
}

/***********************
 *----  M_ASD::CT  ----*
 ***********************/

void M_ASD::CT::incrementSpaceAllocation (size_t sAllocation) const {
    m_pASD->IncrementAllocation (sAllocation);
}

void M_ASD::CT::decrementSpaceAllocation (size_t sAllocation) const {
    m_pASD->DecrementAllocation (sAllocation);
}

unsigned int M_ASD::CT::persistentEntryCount () const {
    PS_ASD *pPASD = m_pASD->persistentASD ();
    return pPASD ? pPASD->CTEntryCount () : 0;
}

char const *M_ASD::CT::ndfPathName () const {
    return m_pASD->Database ()->NDFPathName ();
}

unsigned int M_ASD::CT::spaceIndex () const {
    return m_pASD->Index ();
}


#endif
