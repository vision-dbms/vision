/*****	V::OS::VMSMemoryRegion Implementation	 ****/

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

#include "Vos_VMSMemoryRegion.h"

/************************
 *****	Supporting  *****
 ************************/

#include "VkStatus.h"

namespace V {
//  Implemented in VkMemory.cpp
    size64_t BytesToPageBytes (size64_t);
}


/*>>>>>>>>>>>>>>><<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>><<<<<<<<<<<<<<<*
 *>>>>                      <<<<*
 *>>>>  VMS Implementation  <<<<*
 *>>>>                      <<<<*
 *>>>>>>>>>>>>>>><<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>><<<<<<<<<<<<<<<*/

#ifdef __VMS

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#include <secdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <vadef.h>


/********************************************
 ********************************************
 *****                                  *****
 *****  V::OS::VMSMemoryRegion::Handle  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****	Construction  *****
 **************************
 **************************/

V::OS::VMSMemoryRegion::Handle::Handle (VkStatus &rConstructionStatus, size64_t sRegion) : m_bOwner (false) {
    addr64_t pArea; size64_t sArea;
    m_bOwner = rConstructionStatus.MakeSuccessStatus (
	sys$create_region_64 (sRegion, VA$C_REGION_UCREATE_UOWN, 0, &m_hRegion, reinterpret_cast<__void_ptr64*>(&pArea), &sArea, 0)
    );
    if (m_bOwner) {
	m_iRegion.setTo (pArea, sArea);
    }
}

V::OS::VMSMemoryRegion::Handle::Handle (Handle &rOther)
    : m_hRegion (rOther.m_hRegion)
    , m_iRegion (rOther.m_iRegion)
    , m_bOwner (rOther.ownership ())
{
}

/*************************
 *************************
 *****	Destruction  *****
 *************************
 *************************/

V::OS::VMSMemoryRegion::Handle::~Handle () {
    if (m_bOwner) {
	m_bOwner = false;
//	void *pDeleted; size64_t sDeleted;
//	sys$delete_region_64 (&m_hRegion, 0, &pDeleted, &sDeleted);
    }
}


/******************************
 ******************************
 *****  Space Management  *****
 ******************************
 ******************************/

unsigned int V::OS::VMSMemoryRegion::adjustedSectionFlags_(unsigned int iFlags, addr64_t pSpace) const {
    return pSpace ? iFlags : iFlags | SEC$M_EXPREG;
}

bool V::OS::VMSMemoryRegion::getRegionID_(VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const {
    rRegionID = regionID ();
    return rStatus.MakeSuccessStatus ();
}

bool V::OS::VMSMemoryRegion::Handle::acquireSpace (VkStatus &rStatus, Area &rArea, Area const &rSpace) {
    Area const &rContainer = rArea.isntNil () ? rArea : m_iRegion;

    if (!rContainer.contains (rSpace))
	return rStatus.MakeErrorStatus (SS$_PAGNOTINREG);

    if (rArea.isNil ())
	rArea.setTo (rSpace);

    return rStatus.MakeSuccessStatus ();
}

bool V::OS::VMSMemoryRegion::Handle::provideSpace (VkStatus &rStatus, Area &rArea, size64_t sSpace) {
    rArea.clear ();	//  VMS => expand region

    return rStatus.MakeSuccessStatus ();
}

bool V::OS::VMSMemoryRegion::Handle::reclaimSpace (VkStatus &rStatus, Area &rArea) {
    rArea.clear ();
    return rStatus.MakeSuccessStatus ();
}

bool V::OS::VMSMemoryRegion::Handle::expandRegion (
    VkStatus &rStatus, addr64_t &rpAllocation, size64_t &rsAllocation, size64_t sAllocation
) {
    return rStatus.MakeSuccessStatus (
	sys$expreg_64 (&m_hRegion, BytesToPageBytes (sAllocation), 0, 0, reinterpret_cast<__void_ptr64*>(&rpAllocation), &rsAllocation)
    );
}


/************************************
 ************************************
 *****                          *****
 *****	V::OS::VMSMemoryRegion  *****
 *****                          *****
 ************************************
 ************************************/

/***************************
 ***************************
 *****	Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (V::OS::VMSMemoryRegion);

/**************************
 **************************
 *****	Construction  *****
 **************************
 **************************/

V::OS::VMSMemoryRegion *V::OS::VMSMemoryRegion::New (size64_t sRegion) {
    VkStatus iConstructionStatus;
    Handle iHandle (iConstructionStatus, sRegion);
    return iConstructionStatus.isCompleted () ? new ThisClass (iHandle) : 0;
}

V::OS::VMSMemoryRegion::VMSMemoryRegion (Handle &rHandle) : m_iHandle (rHandle) {
}

/*************************
 *************************
 *****	Destruction  *****
 *************************
 *************************/

V::OS::VMSMemoryRegion::~VMSMemoryRegion () {
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool V::OS::VMSMemoryRegion::acquireSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, Area const &rSpace) {
    return m_iHandle.acquireSpace (rStatus, rArea, rSpace);
}

bool V::OS::VMSMemoryRegion::provideSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, size64_t sSpace) {
    rpMM.setTo (m_iHandle.provideSpace (rStatus, rArea, sSpace) ? this : 0);
    return rpMM.isntNil ();
}

bool V::OS::VMSMemoryRegion::reclaimSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea) {
    MMR pMM;
    pMM.claim (rpMM);
    return pMM ? m_iHandle.reclaimSpace (rStatus, rArea) : rStatus.MakeSuccessStatus ();
}

#endif // __VMS
