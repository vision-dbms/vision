/*****  M_ASD Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "M_ASD.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VContainerHandle.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

M_ASD::M_ASD (M_AND* pAND, PS_ASD* pPASD)
: m_pAND			(pAND)
, m_pPASD			(pPASD)
, m_xSpace			(pPASD ? pPASD->spaceIndex () : M_KnownSpace_ScratchPad)
, m_iTransientAllocation	(0)
, m_pPCTCPCC			(0)
, m_pGCQueue			(0)
, m_iCT				(this)
{
    if (pPASD)
	pPASD->m_pLogicalASD = this;

    M_ASD *pRingHead;
    if (IsntNil (pRingHead = pAND->m_pASDRing)) {
	m_pSuccessor = pRingHead->successor ();
	pRingHead->m_pSuccessor = this;
    }
    else
	m_pSuccessor = pAND->m_pASDRing = this;

    pAND->m_iASDVector[m_xSpace] = this;
}


/******************************************
 ******************************************
 *****  Container Table Entry Access  *****
 ******************************************
 ******************************************/

bool M_ASD::GetCTE (unsigned int xContainer, CTE &rResult) const {
    if (this && xContainer < cteCount ()) {
	rResult = *cte (xContainer);
	return true;
    }

    return false;
}
