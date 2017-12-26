/*****  PS_ASD Implementation  *****/

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

#include "PS_ASD.h"

/************************
 *****  Supporting  *****
 ************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool PS_ASD::GetCTE (unsigned int xContainer, PS_CTE &rResult) const {
    if (xContainer < CTEntryCount ()) {
	rResult = PS_ASD_CTEntry (this, xContainer);
	return true;
    }

    return false;
}


/******************************
 ******************************
 *****  Settings Control  *****
 ******************************
 ******************************/

void PS_ASD::SetLargeContainerSizeTo (double iValue) {
    if (iValue >= 100)
	m_iLargeContainerSize = iValue < UINT_MAX ? (unsigned int)iValue : UINT_MAX;
}

void PS_ASD::SetLargeContainerSizeTo (int iValue) {
    if (iValue >= 100)
	m_iLargeContainerSize = iValue;
}

void PS_ASD::SetMaxCompactionSegmentsTo (double iValue) {
    if (iValue > 0)
	m_iMaxCompactionSegs = iValue < UINT_MAX ? (unsigned int)iValue : UINT_MAX;
}

void PS_ASD::SetMaxCompactionSegmentsTo (int iValue) {
    if (iValue > 0)
	m_iMaxCompactionSegs = iValue;
}

void PS_ASD::SetMaxSegmentSizeTo (double iValue) {
    if (iValue > 0)
	m_iMaxSegmentSize = iValue < UINT_MAX ? (unsigned int)iValue : UINT_MAX;
}

void PS_ASD::SetMaxSegmentSizeTo (int iValue) {
    if (iValue > 0)
	m_iMaxSegmentSize = iValue;
}

void PS_ASD::SetMSSOverrideTo (double iValue) {
    if (iValue < PS_ASD_MinSegment (this) || iValue > PS_ASD_RootSegment (this) + 1)
	m_bMinSMDSet = false;
    else {
	m_xMinSMD = (unsigned int)iValue - m_xBaseSegment;
	m_bMinSMDSet = true;
    }
}

void PS_ASD::SetMSSOverrideTo (int iValue) {
    if (iValue < PS_ASD_MinSegment (this) || iValue > PS_ASD_RootSegment (this) + 1)
	m_bMinSMDSet = false;
    else {
	m_xMinSMD = iValue - m_xBaseSegment;
	m_bMinSMDSet = true;
    }
}
