#ifndef V_VAllocator_Interface
#define V_VAllocator_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VAggregatePointer.h"
#include "V_VCount.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {

/*******************************
 *----  VAllocatorGranule  ----*
 *******************************/

    class V_API VAllocatorGranule : public VTransient {
	DECLARE_FAMILY_MEMBERS (VAllocatorGranule, VTransient);

    //  Aliases
    public:
	typedef counter32nil_t		counter_t;
	typedef counter_t::value_t	count_t;

    //  Globals
    public:
	static bool		g_bFreePoolDisabled;
	static unsigned int	g_sFreeListTotal;
	static unsigned int	g_sFreePoolTotal;
	static unsigned int	g_sFreePoolWaste;
	static counter_t	g_cFreePoolAdaptations;
	static counter_t	g_cFreeListOverflows;

    /****************************
     *----  SingleThreaded  ----*
     ****************************/
    public:
	class V_API SingleThreaded : public VTransient {
	    DECLARE_FAMILY_MEMBERS (SingleThreaded, VTransient);

	//  Aliases
	public:
	    typedef ThisClass Link;

	//  Construction
	public:
	    SingleThreaded () {
	    }

	//  Destruction
	public:
	    ~SingleThreaded () {
	    }

	//  Update
	private:
	    void push (ThisClass *pNew) {
		m_pNextFree.push (pNew, &ThisClass::m_pNextFree);
	    }
	    bool pop (Pointer &rpFirst) {
		return m_pNextFree.pop (rpFirst, &ThisClass::m_pNextFree);
	    }
	    void *grow (size_t sCell);
	public:
	    void *allocate (size_t sCell) {
		Pointer pFirst;
		return pop (pFirst) ? pFirst.referent () : grow (sCell);
	    }
	    void deallocate (void *pCell) {
		push (reinterpret_cast<ThisClass*>(pCell));
	    }

	//  State
	private:
	    Pointer m_pNextFree;
	};
	friend class SingleThreaded;


    /****************************
     *----  MultiThreaded  ----*
     ****************************/
    public:
	class V_API MultiThreaded : public VTransient {
	    DECLARE_FAMILY_MEMBERS (MultiThreaded, VTransient);

	//  Aliases
	public:
	    typedef ThisClass Link;

	//  Construction
	public:
	    MultiThreaded () {
	    }

	//  Destruction
	public:
	    ~MultiThreaded () {
	    }

	//  Update
	private:
	    void push (ThisClass *pNew) {
		m_pNextFree.interlockedPush (pNew, &ThisClass::m_pNextFree);
	    }
	    bool pop (Pointer &rpFirst) {
		return m_pNextFree.interlockedPop (rpFirst, &ThisClass::m_pNextFree);
	    }
	    void *grow (size_t sCell);
	public:
	    void *allocate (size_t sCell) {
		Pointer pFirst;
		return pop (pFirst) ? pFirst.referent () : grow (sCell);
	    }
	    void deallocate (void *pCell) {
		push (reinterpret_cast<ThisClass*>(pCell));
	    }

	//  State
	private:
	    Pointer m_pNextFree;
	};
	friend class MultiThreaded;


    /***************************************
     *----  VAllocatorGranule (con't)  ----*
     ***************************************/

    //  Construction
    public:
	VAllocatorGranule () {
	}

    //  Destruction
    public:
	~VAllocatorGranule () {
	}

    //  Allocation
    private:
	static size_t allocationSize (size_t sCell) {
	    unsigned int nCells = static_cast<unsigned int>(1024 / sCell);
	    return nCells > 0 ? nCells * sCell : sCell;
	}
	static pointer_t getSpace (size_t sCell);

    //  Display
    protected:
	static void displayCounts (unsigned int sCell, count_t nAllocated, count_t nDeallocated);
    };


    /********************************
     *----  VAllocatorGranule_  ----*
     ********************************/

    template <class Model> class VAllocatorGranule_ : public VAllocatorGranule {
	DECLARE_FAMILY_MEMBERS (VAllocatorGranule_<Model>, VAllocatorGranule);

    //  Aliases
    public:
	typedef typename Model::Link Link;

    //  Construction
    public:
	VAllocatorGranule_() {
	}

    //  Allocation
    public:
	void *allocate (size_t sObject) {
	    m_iACount.increment ();
	    return m_pNextFree.allocate (sObject);
	}

    //  Deallocation
    public:
	void deallocate (void *pObject) {
	    m_iDCount.increment ();
	    m_pNextFree.deallocate (pObject);
	}

    //  Access
    public:
	count_t activeCount () const {
	    return m_iACount - m_iDCount;
	}
	count_t allocationCount () const {
	    return m_iACount;
	}
	count_t deallocationCount () const {
	    return m_iDCount;
	}
	bool inUse () const {
	    return m_iACount > 0 || m_iDCount > 0;
	}

    //  Update
    public:
	void resetCounts () {
	    m_iACount = m_iACount - m_iDCount;
	    m_iDCount = 0;
	}

    //  Display
    public:
	void displayCounts (unsigned int sGrain) const {
	    BaseClass::displayCounts (sGrain, m_iACount, m_iDCount);
	}

    //  State
    private:
	Link		m_pNextFree;
	counter_t	m_iACount;
	counter_t	m_iDCount;
    };

     /********************************
      *----  VAllocatorFreeList  ----*
      ********************************/

    class VAllocatorFreeList : public VAllocatorGranule_<VAllocatorGranule::SingleThreaded> {
	DECLARE_FAMILY_MEMBERS (VAllocatorFreeList, VAllocatorGranule_<VAllocatorGranule::SingleThreaded>);

    //  Construction
    public:
	VAllocatorFreeList (size_t sObject) : m_sAllocation (sObject) {
	}

    //  Use
    public:
	void *allocate () {
	    return BaseClass::allocate (m_sAllocation);
	}

    //  State
    private:
	size_t const m_sAllocation;
    };


    /****************************
     *----  VAllocatorBase  ----*
     ****************************/

    class V_API VAllocatorBase : public VTransient {
	DECLARE_FAMILY_MEMBERS (VAllocatorBase, VTransient);

    //  Counts and Counters
    public:
	typedef counter32nil_t		counter_t;
	typedef counter_t::value_t	count_t;

    //  Construction
    protected:
	VAllocatorBase () {
	}

    //  Destruction
    protected:
	~VAllocatorBase () {
	}

    //  Allocation
    protected:
	void *allocateOversizedObject (size_t sObject) {
	    m_iOversizedACount.increment ();
	    return allocate (sObject);
	}

    //  Deallocation
    protected:
	void deallocateOversizedObject (void* pObject) {
	    m_iOversizedDCount.increment ();
	    deallocate (pObject);
	}

    //  Display
    protected:
	static void displayCountHeadings ();

	void displayOversizedObjectCounts (size_t sSmallestOversizedObject) const;

    //  State
    protected:
	counter_t m_iOversizedACount;
	counter_t m_iOversizedDCount;
    };


    /************************
     *----  VAllocator  ----*
     ************************/

    template<
	unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model = VAllocatorGranule::SingleThreaded
    > class VAllocator : public VAllocatorBase {
    //  Aliases
    public:
	typedef VAllocatorGranule_<Model> Cell;

    //  Construction
    public:
	VAllocator () {
	}

    //  Destruction
    public:
	~VAllocator () {
	}

    //  Grain and Counter Indices
    protected:
	static unsigned int cellIndex (size_t sObject) {
	    return sObject <= CellFloor ? 0 : static_cast<unsigned int>((sObject - CellFloor + CellGrain - 1) / CellGrain);
	}
	static size_t cellSize (unsigned int xGrain) {
	    return CellGrain * (xGrain + 1) + CellFloor;
	}

    //  Allocation
    public:
	void *allocate (size_t sObject) {
	    unsigned int xGrain = cellIndex (sObject);
	    return xGrain < CellCount
		? m_iCell[xGrain].allocate (cellSize (xGrain))
		: allocateOversizedObject (sObject);
	}

    //  Deallocation
    public:
	void deallocate (void *pObject, size_t sObject) {
	    unsigned int xGrain = cellIndex (sObject);
	    if (xGrain < CellCount)
		m_iCell[xGrain].deallocate (pObject);
	    else
		deallocateOversizedObject (pObject);
	}

    //  Display
    public:
	void displayCounts () const {
	    displayCountHeadings ();
	    for (unsigned int i = 0; i < CellCount; i++)
		m_iCell[i].displayCounts (cellSize (i));
	    displayOversizedObjectCounts (CellCount * CellGrain + CellFloor);
	}

    //  State
    private:
	Cell m_iCell[CellCount];
    };


    /*****************************************
     *----  VAllocatorInstance__<Model>  ----*
     *****************************************/

    template <class Model> class VAllocatorInstance_ {
    public:
	static VAllocator<(unsigned int)64, (size_t)8, (size_t)0, Model> Data;
    };
    template <class Model> VAllocator<(unsigned int)64, (size_t)8, (size_t)0, Model> VAllocatorInstance_<Model>::Data;

    typedef VAllocatorInstance_<VAllocatorGranule::SingleThreaded>	VThreadDumbAllocator;
    typedef VAllocatorInstance_<VAllocatorGranule::MultiThreaded>	VThreadSafeAllocator;
}


#endif
