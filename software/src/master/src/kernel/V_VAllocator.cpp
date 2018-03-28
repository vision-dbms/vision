/*****  V_VAllocator Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

typedef unsigned __int64 size64_t;

/******************
 *****  Self  *****
 ******************/

#include "V_VAllocator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VSpinlock.h"
#include "VkStatus.h"

#include "Vos_VMSMemoryRegion.h"

/* GG - required for prototype of placement new on vms with rw30 and up */
#ifdef __VMS
#include <new.hxx>
#endif


/***********************************
 ***********************************
 *****                         *****
 *****  V::VAllocatorFreeList  *****
 *****                         *****
 ***********************************
 ***********************************/

/*********************
 *****  Globals  *****
 *********************/

typedef V::VAllocatorGranule::counter_t aflcounter_t;

#ifdef __VMS
bool V::VAllocatorGranule::g_bFreePoolDisabled	= getenv ("VDisableAllocPool");
#else
bool V::VAllocatorGranule::g_bFreePoolDisabled	= !getenv ("VUseAllocPool");
#endif

unsigned int	V::VAllocatorGranule::g_sFreeListTotal		= 0;
unsigned int	V::VAllocatorGranule::g_sFreePoolTotal		= 0;
unsigned int	V::VAllocatorGranule::g_sFreePoolWaste		= 0;
aflcounter_t	V::VAllocatorGranule::g_cFreePoolAdaptations	= 0;
aflcounter_t	V::VAllocatorGranule::g_cFreeListOverflows	= 0;

/*********************
 *****  Display  *****
 *********************/

void V::VAllocatorGranule::displayCounts (unsigned int sCell, count_t nAllocated, count_t nDeallocated) {
    if (nAllocated > 0)
	display ("  %-3u: %8u %8u %8d\n", sCell, nAllocated, nDeallocated, nAllocated - nDeallocated);
}


/************************
 *****  Allocation  *****
 ************************/

#ifdef __VMS
typedef V::OS::VMSMemoryRegion::Handle VMSMemoryRegionHandle;

static VMSMemoryRegionHandle *FreePoolRegion () {
    static bool bNotInitialized = true;
    static VMSMemoryRegionHandle *pFreePoolRegion = 0;
    static char iHandleStorage[sizeof(VMSMemoryRegionHandle)];

    if (bNotInitialized) {
	bNotInitialized = false;
	VkStatus iConstructionStatus;
	pFreePoolRegion = new (iHandleStorage) VMSMemoryRegionHandle (iConstructionStatus, 0x100000000); // == 4GB == max size_t
	if (iConstructionStatus.isFailed ()) {
	    pFreePoolRegion = 0;
	}
    }
    return pFreePoolRegion;
}

static size_t const	sInitialScaleFactor = 8;
#else
static size_t const	sInitialScaleFactor = 1;

#endif

static size_t const	sInitialAllocation = 2 * 1024 * 1024 / sInitialScaleFactor;
static size_t const	sMaximumAllocation = 64 * sInitialAllocation * sInitialScaleFactor;
static size_t		sNextAllocation = sInitialAllocation;
static size_t		sThisAdaptiveInterval = 4;
static size_t		sNextAdaptiveInterval = sThisAdaptiveInterval;
static size_t		sPool = 0;
static V::pointer_t	pPool = 0;

V::pointer_t V::VAllocatorGranule::getSpace (size_t sSpace) {
    pointer_t pSpace;
    if (g_bFreePoolDisabled)
	pSpace = static_cast<pointer_t>(BaseClass::allocate (sSpace));
    else {
	static VSpinlock iCriticalSectionMutex;
	VSpinlock::Claim iCriticalSectionClaim (iCriticalSectionMutex);
	//  Cache the original pool settings...
	size_t    const sPoolAtEntry = sPool;
	pointer_t const pPoolAtEntry = pPool;
	try {
	    if (sPool < sSpace) {
		size_t sThisAllocation = sNextAllocation;
		if (sThisAllocation < sSpace)
		    sThisAllocation = sSpace;
#ifdef __VMS
		pointer_t pThisAllocation = 0;
		VMSMemoryRegionHandle *pFreePoolRegion = FreePoolRegion ();
		if (pFreePoolRegion) {
		    VkStatus iStatus; addr64_t pAllocation; size64_t sAllocation;
		    if (pFreePoolRegion->expandRegion (iStatus, pAllocation, sAllocation, sThisAllocation)) {
			pThisAllocation = static_cast<pointer_t>(pAllocation);
			sThisAllocation = static_cast<size_t>(sThisAllocation);
		    }
		}
		if (!pThisAllocation)
		    pThisAllocation = static_cast<pointer_t>(BaseClass::allocate (sThisAllocation));
#else
		pointer_t pThisAllocation = static_cast<pointer_t>(BaseClass::allocate (sThisAllocation));
#endif
		if (pPool && pPool + sPool == pThisAllocation)
		    sPool += sThisAllocation;
		else {
		    g_sFreePoolWaste += sPool;
		    pPool = pThisAllocation;
		    sPool = sThisAllocation;
		}
		g_sFreePoolTotal += sThisAllocation;

		if (sNextAllocation < sMaximumAllocation && 0 == sThisAdaptiveInterval--) {
		    g_cFreePoolAdaptations++;
		    sNextAllocation *= 2;
		    if (sNextAllocation > sMaximumAllocation)
			sNextAllocation = sMaximumAllocation;
		    sThisAdaptiveInterval = sNextAdaptiveInterval;
//		sNextAdaptiveInterval /= 2;
//		if (sNextAdaptiveInterval < 1)
//		    sNextAdaptiveInterval = 1;
		}
	    }
	    pSpace = pPool;
	    pPool += sSpace;
	    sPool -= sSpace;
	} catch (...) {
	    //  Restore the original pool settings, ...
	    sPool = sPoolAtEntry;
	    pPool = pPoolAtEntry;

	    //  ... and fail:
	    pSpace = 0;
	}
    }

    VAtomicOperations_<unsigned int>::fetchAndAdd (g_sFreeListTotal, sSpace);

    return pSpace;
}

void *V::VAllocatorGranule::SingleThreaded::grow (size_t sCell) {
    size_t sSpace = allocationSize (sCell);
    pointer_t const pSpace = getSpace (sSpace);
    for (pointer_t pCell = pSpace + sSpace - sCell; pCell > pSpace; pCell -= sCell)
	deallocate (pCell);
    return pSpace;
}

void *V::VAllocatorGranule::MultiThreaded::grow (size_t sCell) {
    size_t sSpace = allocationSize (sCell);
    pointer_t const pSpace = getSpace (sSpace);
    for (pointer_t pCell = pSpace + sSpace - sCell; pCell > pSpace; pCell -= sCell)
	deallocate (pCell);
    return pSpace;
}


/****************************
 ****************************
 *****                  *****
 *****  VAllocatorBase  *****
 *****                  *****
 ****************************
 ****************************/

/*********************
 *****  Display  *****
 *********************/

void V::VAllocatorBase::displayCountHeadings () {
    display ("\n'new/delete' Statistics:\n");
}

void V::VAllocatorBase::displayOversizedObjectCounts (size_t sSmallestOversizeObject) const {
    if (m_iOversizedACount || m_iOversizedDCount) display (
	">=%-3u: %8u %8u %8d\n",
	sSmallestOversizeObject,
	m_iOversizedACount.operator count_t () , m_iOversizedDCount.operator count_t (),
	m_iOversizedACount.operator count_t () - m_iOversizedDCount.operator count_t ()
    );
}


/***************************
 ***************************
 *****                 *****
 *****  V::VAllocator  *****
 *****                 *****
 ***************************
 ***************************/

/****************************
 ****************************
 *****  Instantiations  *****
 ****************************
 ****************************/

template class V_API V::VAllocatorInstance_<V::VAllocatorGranule::SingleThreaded>;
template class V_API V::VAllocatorInstance_<V::VAllocatorGranule::MultiThreaded>;
