/*****	V::VMM Implementation   ****/

/************************
 ************************
 *****	Interfaces  *****
 ************************
 ************************/

/********************
 *****	System	*****
 ********************/

#include "Vk.h"

/******************
 *****	Self  *****
 ******************/

#include "V_VMM.h"

/************************
 *****	Supporting  *****
 ************************/

#include "VkStatus.h"


/**************************
 **************************
 *****                *****
 *****  V::VMM::Area  *****
 *****                *****
 **************************
 **************************/

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool V::VMM::Area::cover (Area const &rOther) {
    bool bAdjusted = false;
    if (areaOrigin () > rOther.areaOrigin ()) {
	m_sArea += reinterpret_cast<size64_t>(areaOrigin ()) - reinterpret_cast<size64_t>(rOther.areaOrigin ());
	m_pArea = rOther.areaOrigin ();
	bAdjusted = true;
    }
    if (rOther.areaExtent () > areaExtent ()) {
	m_sArea += reinterpret_cast<size64_t>(rOther.areaExtent ()) - reinterpret_cast<size64_t>(areaExtent ());
	bAdjusted = true;
    }
    return bAdjusted;
}

bool V::VMM::Area::prune (Area const &rOther) {
    bool bAdjusted = false;
    if (areaOrigin () == rOther.areaOrigin ()) {
	size64_t sAdjustment = rOther.areaSize ();
	if (sAdjustment > m_sArea)
	    sAdjustment = m_sArea;
	m_pArea += sAdjustment;
	m_sArea -= sAdjustment;
	bAdjusted = true;
    }
    else if (areaExtent () == rOther.areaExtent ()) {
	size64_t sAdjustment = rOther.areaSize ();
	if (sAdjustment > m_sArea)
	    sAdjustment = m_sArea;
	m_sArea -= sAdjustment;
	bAdjusted = true;
    }
    return bAdjusted;
}

bool V::VMM::Area::pruneOrigin (Area const &rOther) {
    bool bAdjusted = false;
    if (areaOrigin () == rOther.areaOrigin ()) {
	size64_t sAdjustment = rOther.areaSize ();
	if (sAdjustment > m_sArea)
	    sAdjustment = m_sArea;
	m_pArea += sAdjustment;
	m_sArea -= sAdjustment;
	bAdjusted = true;
    }
    return bAdjusted;
}

bool V::VMM::Area::pruneExtent (Area const &rOther) {
    bool bAdjusted = false;
    if (areaExtent () == rOther.areaExtent ()) {
	size64_t sAdjustment = rOther.areaSize ();
	if (sAdjustment > m_sArea)
	    sAdjustment = m_sArea;
	m_sArea -= sAdjustment;
	bAdjusted = true;
    }
    return bAdjusted;
}


/***************************
 ***************************
 *****                 *****
 *****  V::VMM::Space  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VMM::Space::Space () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VMM::Space::~Space () {
    releaseSpace ();
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

#ifdef __VMS
unsigned int V::VMM::Space::adjustedSectionFlags (unsigned int iFlags) const {
    return m_pManager ? m_pManager->adjustedSectionFlags_(iFlags, areaOrigin ()) : iFlags;
}

bool V::VMM::Space::getRegionID (VkStatus &rStatus, _generic_64 &rRegionID) const {
    return m_pManager ? m_pManager->getRegionID_(rStatus, rRegionID, areaOrigin ()) : rStatus.MakeFailureStatus ();
}
#endif


/*********************************
 *********************************
 *****	Acquisition/Release  *****
 *********************************
 *********************************/

bool V::VMM::Space::acquireSpace (VkStatus &rStatus, VMM *pManager, size64_t sSpace) {
    return releaseSpace (rStatus) && pManager->provideSpace_(rStatus, m_pManager, m_iArea, sSpace);
}

bool V::VMM::Space::acquireSpace (VkStatus &rStatus, Area const &rSpace) {
    return m_pManager ? m_pManager->acquireSpace_(rStatus, m_pManager, m_iArea, rSpace) : rStatus.MakeFailureStatus ();
}

bool V::VMM::Space::acquireSpace (VkStatus &rStatus, Space &rSpace) {
    if (!releaseSpace (rStatus))
	return false;

    m_iArea.setTo (rSpace.area ());
    m_pManager.claim (rSpace.m_pManager);

    return rStatus.MakeSuccessStatus ();
}

bool V::VMM::Space::releaseSpace (VkStatus &rStatus) {
    return m_pManager ? m_pManager->reclaimSpace_(rStatus, m_pManager, m_iArea) : rStatus.MakeSuccessStatus ();
}

bool V::VMM::Space::releaseSpace () {
    VkStatus iStatus;
    return releaseSpace (iStatus);
}


/********************
 ********************
 *****          *****
 *****  V::VMM  *****
 *****          *****
 ********************
 ********************/

/***************************
 ***************************
 *****	Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (V::VMM);

/**************************
 **************************
 *****	Construction  *****
 **************************
 **************************/

V::VMM::VMM () {
}

/*************************
 *************************
 *****	Destruction  *****
 *************************
 *************************/

V::VMM::~VMM () {
}
