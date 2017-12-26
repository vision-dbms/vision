/*****  VFragment Implementation  *****/

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

#include "VFragment.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"
#include "vstdio.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"
#include "RTundefuv.h"


/***********************************
 ***********************************
 *****  VFragment::Descriptor  *****
 ***********************************
 ***********************************/

/**************************
 *****  Construction  *****
 **************************/

/*************************
 *****  Destruction  *****
 *************************/

/****************************
 *****  Initialization  *****
 ****************************/

void VFragment::Descriptor::initializeFrom (VFragment* pFragment) {
    m_pFragment = pFragment;

    VDescriptor const& rContent = m_pFragment->datum ();
    if (rContent.isStandard ()) {
	m_pStore.setTo (rContent.contentAsMonotype ().store ());
    }
    else rContent.raiseTypeException ("VFragment::Descriptor::initializeFrom (VFragment*)");
}


/*******************
 *****  Query  *****
 *******************/

bool VFragment::Descriptor::getFastMergeData (unsigned int& rCumulativeRRDCount) const {
    rtLINK_CType* pSubset = m_pFragment->subset ();
    pSubset->Align ();

    if (rtLINK_LC_HasRepeats (pSubset) || pSubset->ContainsReferenceNils ())
	return false;

    rCumulativeRRDCount += rtLINK_LC_RRDCount (pSubset);
    return true;
}


/*********************
 *****  Merging  *****
 *********************/

/**********************
 *  'fastMergeBegin'  *
 **********************/

void VFragment::Descriptor::fastMergeBegin (
    Descriptor** &rppDescriptor, unsigned int* &rpReference, unsigned int* &rpSize
) {
    rtLINK_CType* pSubset = m_pFragment->subset ();

    unsigned int iRRDCount = rtLINK_LC_RRDCount (pSubset);
    rtLINK_RRDCType* pRRD = rtLINK_LC_ChainHead (pSubset);

    for (unsigned int xRRD = 0; xRRD < iRRDCount; xRRD++) {
	*rppDescriptor++ = this;
	*rpReference++	 = rtLINK_RRDC_ReferenceOrigin (pRRD);
	*rpSize++	 = rtLINK_RRDC_N (pRRD);
	pRRD = rtLINK_RRDC_NextRRDC (pRRD);
    }

    m_pAssignmentLinkc = rtLINK_PosConstructor (pSubset->PPT ());
}


/*******************
 *  'buildSubset'  *
 *******************/

void VFragment::Descriptor::buildSubset (rtLINK_CType *pNewSubset) {
    m_pAssignmentLinkc = pNewSubset->Add (m_pFragment->subset ());
}


/*********************
 *  'createContent'  *
 *********************/

M_CPD* VFragment::Descriptor::createContent (rtLINK_CType* pNewSubset) {
    DSC_Descriptor& rContent = m_pFragment->datum ().contentAsMonotype ();

    RTYPE_Type xContentRType = rContent.pointerRType ();

    rtPTOKEN_Handle::Reference pRPT (rContent.RPT ());

    M_CPD* pNewContent;
    switch (xContentRType) {
    case RTYPE_C_IntUV:
	pNewContent = rtINTUV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_DoubleUV:
	pNewContent = rtDOUBLEUV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_FloatUV:
	pNewContent = rtFLOATUV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_CharUV:
	pNewContent = rtCHARUV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_Link:
    case RTYPE_C_RefUV:
	pNewContent = rtREFUV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_Unsigned64UV:
	pNewContent = rtU64UV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_Unsigned96UV:
	pNewContent = rtU96UV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_Unsigned128UV:
	pNewContent = rtU128UV_New	(pNewSubset->PPT (), pRPT);
	break;
    case RTYPE_C_UndefUV:
	pNewContent = rtUNDEFUV_New	(pNewSubset->PPT (), pRPT);
	break;
    default:
	raiseException (
	    EC__UnknownUVectorRType,
	    "MergeFragments: Unsupported R-Type %s",
	    RTYPE_TypeIdAsString (xContentRType)
	);
	break;
    }

    return pNewContent;
}


/**********************
 *  'createFragment'  *
 **********************/

void VFragment::Descriptor::createFragment (
    rtLINK_CType* pNewSubset, M_CPD* pNewContent, VFragmentation* pParent
) {
    pParent->createFragment (pNewSubset)->datum ().setToMonotype (m_pStore, pNewContent);
}


/*******************
 *  'moveContent'  *
 *******************/

VFragment* VFragment::Descriptor::moveContent (M_CPD* pNewContent) {
    m_pFragment->datum ().normalize ();

    //  Move non-trivial content, ...
    if (RTYPE_C_UndefUV != pNewContent->RType ()) {
	m_pFragment->datum ().contentAsMonotype ().assignToUV (
	    m_pAssignmentLinkc, pNewContent
	);
    }

    //  ... delete the assignment link, ...
    m_pAssignmentLinkc->release ();
    m_pAssignmentLinkc = 0;

    //  ... and return the now unneeded fragment so it can be deleted...
    return m_pFragment;
}


/***************************
 *  Descriptor Comparison  *
 ***************************/

int __cdecl VFragment::CompareDescriptors (
    Descriptor const *pDescriptor1, Descriptor const *pDescriptor2
) {
    return *pDescriptor1 == *pDescriptor2 ? 0 : *pDescriptor1 > *pDescriptor2 ?  1 : -1;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VFragment::VFragment (VFragment* pNext, rtLINK_CType* pSubset)
: m_pNext (pNext), m_pPrevious (0), m_pSubset (pSubset)
{
    if (pNext)
	pNext->m_pPrevious = this;
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

/******************
 *  '~VFragment'  *
 ******************/

VFragment::~VFragment () {
    m_pSubset->release ();
}


/***************
 *  'discard'  *
 ***************/

void VFragment::Discard (VFragment* pThisFragment) {
    while (pThisFragment) {
        VFragment* pNextFragment = pThisFragment->next ();

/*****  If there is a fragmentation inside this fragment ... *****/
	if (pThisFragment->datum ().isAFragmentation ()) {
	    VFragmentation& rFragmentation = pThisFragment->datum().contentAsFragmentation();

/*****  add its fragments to the discard list ... *****/
	    rFragmentation.goToFirstFragment ();

	    VFragment *pInteriorFragment;
	    while (pInteriorFragment = rFragmentation.currentFragment ()) {
		rFragmentation.goToNextFragment ();
		pInteriorFragment->m_pNext = pNextFragment;
		pNextFragment = pInteriorFragment;
	    }

/*****  and remove them from the interior fragmentation.  *****/
	    rFragmentation.m_pFragmentList
		= rFragmentation.m_pCurrentFragment
		= NilOf (VFragment*);
	}

	delete pThisFragment;
	pThisFragment = pNextFragment;
    }
}


/******************
 *  'removeFrom'  *
 ******************/

void VFragment::removeFrom (VFragmentation* pParent) {
    if (m_pPrevious)
	m_pPrevious->m_pNext = m_pNext;
    else
	pParent->m_pFragmentList = m_pNext;

    if (m_pNext)
	m_pNext->m_pPrevious = m_pPrevious;

    pParent->m_iFragmentCount--;

    delete this;
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void VFragment::setSubsetTo (rtLINK_CType* pSubset) {
    m_pSubset->release ();
    m_pSubset = pSubset;
}


/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void VFragment::describe (unsigned int xLevel) {
    m_iDatum.describe (xLevel);
}
