#ifndef V_VAllocator_Interface
#define V_VAllocator_Interface

#ifndef V_VAllocator
#define V_VAllocator extern
#endif

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

    /******************
     *----  Node  ----*
     ******************/
    public:
	class Node {
	    DECLARE_NUCLEAR_FAMILY (Node);
	public:
	    Pointer m_pNext;
	};

    /**************************
     *----  ThreadScoped  ----*
     **************************/
    public:
	class V_API ThreadScoped : public VTransient {
	    DECLARE_FAMILY_MEMBERS (ThreadScoped, VTransient);

	//  Construction
	public:
	    ThreadScoped ();

	//  Destruction
	public:
	    ~ThreadScoped ();

	//  Update
	private:
	    void push (Node *pNew) {
		m_pFreeList.push (pNew, &Node::m_pNext);
	    }
	    bool pop (Node::Pointer &rpFirst) {
		return m_pFreeList.pop (rpFirst, &Node::m_pNext);
	    }
	    template <class hazard_pointer_t> void *grow (size_t sCell, hazard_pointer_t &rpHazard);
	public:
	    template <class hazard_pointer_t> void *allocate (size_t sCell, hazard_pointer_t &rpHazard) {
		Node::Pointer pFirst;
		return pop (pFirst) ? pFirst.referent () : grow (sCell, rpHazard);
	    }
	    void deallocate (void *pCell);
	    void flush (size_t sCell);

	//  State
	private:
	    Node::Pointer m_pFreeList;
	};
	friend class ThreadScoped;


    /***************************
     *----  ProcessScoped  ----*
     ***************************/
    public:
	class V_API ProcessScoped : public VTransient {
	    DECLARE_FAMILY_MEMBERS (ProcessScoped, VTransient);

	//  Construction
	public:
	    ProcessScoped ();

	//  Destruction
	public:
	    ~ProcessScoped ();

	//  Update
	private:
	    void push (Node *pNew) {
		m_pFreeList.interlockedPush (pNew, &Node::m_pNext);
	    }
	    template <class hazard_pointer_t> bool pop (Node::Pointer &rpFirst, hazard_pointer_t &rpHazard) {
		return m_pFreeList.interlockedPop (rpFirst, &Node::m_pNext, rpHazard);
	    }
	    void *grow (size_t sCell);
	public:
	    template <class hazard_pointer_t> void *allocate (size_t sCell, hazard_pointer_t &rpHazard) {
		Node::Pointer pFirst;
		return pop (pFirst, rpHazard) ? pFirst.referent () : grow (sCell);
	    }
	    void deallocate (void *pCell);
	    void flush (size_t sCell);

	//  State
	private:
	    VAtomicallyLinkablePointer<Node> m_pFreeList;
	};
	friend class ProcessScoped;


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
	typedef Model model_t;

    //  Construction
    public:
	VAllocatorGranule_();

    //  Destruction
    public:
	~VAllocatorGranule_();

    //  Allocation
    public:
	template <class hazard_pointer_t> void *allocate (size_t sObject, hazard_pointer_t &rpHazard) {
	    m_iACount.increment ();
	    return m_iModel.allocate (sObject, rpHazard);
	}

    //  Deallocation
    public:
	void deallocate (void *pObject) {
	    m_iDCount.increment ();
	    m_iModel.deallocate (pObject);
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
	void flush (size_t sCell);

    //  Display
    public:
	void displayCounts (unsigned int sGrain) const {
	    BaseClass::displayCounts (sGrain, m_iACount, m_iDCount);
	}

    //  State
    private:
	Model		m_iModel;
	counter_t	m_iACount;
	counter_t	m_iDCount;
    };


     /********************************
      *----  VAllocatorFreeList  ----*
      ********************************/

    class VAllocatorFreeList : public VAllocatorGranule_<VAllocatorGranule::ThreadScoped> {
	DECLARE_FAMILY_MEMBERS (VAllocatorFreeList, VAllocatorGranule_<VAllocatorGranule::ThreadScoped>);

    //  Construction
    public:
	VAllocatorFreeList (size_t sObject) : m_sAllocation (sObject) {
	}

    //  Use
    public:
	void *allocate () {
	    NoHazardPointer pHazard;
	    return BaseClass::allocate (m_sAllocation, pHazard);
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
	VAllocatorBase ();

    //  Destruction
    protected:
	~VAllocatorBase ();

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

    template <unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model = VAllocatorGranule::ThreadScoped>
	class VAllocator : public VAllocatorBase
    {
    public:
	typedef VAllocatorGranule_<Model> Cell;

    //  Construction
    public:
	VAllocator ();

    //  Destruction
    public:
	~VAllocator ();

    //  Grain and Counter Indices
    protected:
	static unsigned int cellIndex (size_t sObject) {
	    return sObject <= CellFloor ? 0 : (sObject - CellFloor + CellGrain - 1) / CellGrain;
	}
	static size_t cellSize (unsigned int xGrain) {
	    return CellGrain * (xGrain + 1) + CellFloor;
	}

    //  Allocation
    public:
	template <class hazard_pointer_t> void *allocate (size_t sObject, hazard_pointer_t &rpHazard) {
	    unsigned int const xGrain = cellIndex (sObject);
	    return xGrain < CellCount
		? m_iCell[xGrain].allocate (cellSize (xGrain), rpHazard)
		: allocateOversizedObject (sObject);
	}
	void *allocate (size_t sObject) {
	    NoHazardPointer pHazard;
	    return allocate (sObject, pHazard);
	}

    //  Deallocation
    public:
	void deallocate (void *pObject, size_t sObject) {
	    unsigned int const xGrain = cellIndex (sObject);
	    if (xGrain < CellCount)
		m_iCell[xGrain].deallocate (pObject);
	    else
		deallocateOversizedObject (pObject);
	}

    //  Cleanup
    public:
	void flush ();

    //  Display
    public:
	void displayCounts () const;

    //  State
    private:
	Cell m_iCell[CellCount];
    };


    /**********************************************************
     *----  VAllocatorGranule_<Model> Member Definitions  ----*
     **********************************************************/

    template <class Model> VAllocatorGranule_<Model>::VAllocatorGranule_() {
    }
    template <class Model> VAllocatorGranule_<Model>::~VAllocatorGranule_() {
    }
    template <class Model> void VAllocatorGranule_<Model>::flush (size_t sCell) {
	m_iModel.flush (sCell);
    }

    /**************************************************
     *----  VAllocator<Model> Member Definitions  ----*
     **************************************************/

    template <unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model>
	VAllocator<CellCount,CellGrain,CellFloor,Model>::VAllocator ()
    {
    }

    template <unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model>
	VAllocator<CellCount,CellGrain,CellFloor,Model>::~VAllocator ()
    {
	flush ();
    }

    template <unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model>
	void VAllocator<CellCount,CellGrain,CellFloor,Model>::flush ()
    {
	for (unsigned int xGrain = 0; xGrain < CellCount; xGrain++)
	    m_iCell[xGrain].flush (cellSize (xGrain));
    }

    template <unsigned int CellCount, size_t CellGrain, size_t CellFloor, class Model>
	void VAllocator<CellCount,CellGrain,CellFloor,Model>::displayCounts () const
    {
	displayCountHeadings ();
	for (unsigned int xGrain = 0; xGrain < CellCount; xGrain++)
	    m_iCell[xGrain].displayCounts (cellSize (xGrain));
	displayOversizedObjectCounts (CellCount * CellGrain + CellFloor);
    }

    /****************************************
     *----  VAllocatorInstance_<Model>  ----*
     ****************************************/

    template <class Model> class VAllocatorInstance_ : public VAllocator<(unsigned int)64, (size_t)8, (size_t)0, Model> {
    };

    /*********************
     *----  Aliases  ----*
     *********************/

    typedef VAllocatorInstance_<VAllocatorGranule::ThreadScoped>	VThreadDumbAllocator;
    typedef VAllocatorInstance_<VAllocatorGranule::ProcessScoped>	VThreadSafeAllocator;

    /****************************
     *----  Instantiations  ----*
     ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(V_VAllocator_Instantiations)
    V_VAllocator template V_API void *VAllocatorGranule::ThreadScoped::grow (size_t,HazardPointer&);
    V_VAllocator template V_API void *VAllocatorGranule::ThreadScoped::grow (size_t,NoHazardPointer&);

    V_VAllocator template class V_API VAllocatorGranule_<VAllocatorGranule::ThreadScoped>;
    V_VAllocator template class V_API VAllocatorGranule_<VAllocatorGranule::ProcessScoped>;

    V_VAllocator template class V_API VAllocator<(unsigned int)64, (size_t)8, (size_t)0, VAllocatorGranule::ThreadScoped>;
    V_VAllocator template class V_API VAllocator<(unsigned int)64, (size_t)8, (size_t)0, VAllocatorGranule::ProcessScoped>;
#endif
} // namespace V


#endif
