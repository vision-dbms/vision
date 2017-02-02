/*****  Vector Representation Type Header File  *****/
#ifndef rtVECTOR_H
#define rtVECTOR_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VBenderenceable.h"
#include "VContainerHandle.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTvector.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"
#include "RTlink.h"
#include "RTrefuv.h"

class VDescriptor;

/**********************************************
 *****  PartitionedStatistics Statistics  *****
 **********************************************/

enum rtVECTOR_PS_Type {
    rtVECTOR_PS_Sum, 
    rtVECTOR_PS_Avg, 
    rtVECTOR_PS_Max, 
    rtVECTOR_PS_Min, 
    rtVECTOR_PS_NumericCount, 
    rtVECTOR_PS_Product
};


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtVECTOR_CPD_StdPtrCount	5

#define rtVECTOR_CPx_PToken		(unsigned int)0
#define rtVECTOR_CPx_PMRD		(unsigned int)1
#define rtVECTOR_CPx_USD		(unsigned int)2
#define rtVECTOR_CPx_USDCursor		(unsigned int)3
#define rtVECTOR_CPx_USI		(unsigned int)4


/*****  Standard CPD Access Macros  *****/
#define rtVECTOR_CPD_Base(cpd)\
    ((rtVECTOR_Type *)M_CPD_ContainerBase (cpd))

/*----  General Vector Access Macros  ----*/
#define rtVECTOR_CPD_PToken(cpd)\
    M_CPD_PointerToPOP (cpd, rtVECTOR_CPx_PToken)

#define rtVECTOR_CPD_RowPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtVECTOR_CPx_PToken, RTYPE_C_PToken)\
)

#define rtVECTOR_CPD_IsASet(cpd)\
    rtVECTOR_V_IsASet (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_PMapSize(cpd)\
    rtVECTOR_V_PMapSize (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_IsInconsistent(cpd)\
    rtVECTOR_V_IsInconsistent (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_PMapTransition(cpd)\
    rtVECTOR_V_PMapTransition (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_USegArraySize(cpd)\
    rtVECTOR_V_USegArraySize (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_USegFreeBound(cpd)\
    rtVECTOR_V_USegFreeBound (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_USegIndexSize(cpd)\
    rtVECTOR_V_USegIndexSize (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_PMap(cpd)\
    rtVECTOR_V_PMap (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_USegArray(cpd)\
    rtVECTOR_V_USegArray (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_USegIndex(cpd)\
    rtVECTOR_V_USegIndex (rtVECTOR_CPD_Base (cpd))

/*****  Can be Simplified Query  *****/
#define rtVECTOR_CPD_CanBeSimplified(cpd)\
    (rtVECTOR_CPD_PMapSize (cpd) == 1)
    
/*****
 * The following macro does not expand into efficient code.  Use a different
 * mechanism in speed critical applications.
 *****/
#define rtVECTOR_CPD_ElementCount(cpd)\
    rtVECTOR_V_ElementCount (rtVECTOR_CPD_Base (cpd))

/*----  P-Map Entry Access Macros  ----*/
#define rtVECTOR_CPD_PMRD(cpd)\
    M_CPD_PointerToType (cpd, rtVECTOR_CPx_PMRD, rtVECTOR_PMRDType *)

#define rtVECTOR_CPD_PMRD_VectorOrigin(cpd)\
    rtVECTOR_PMRD_VectorOrigin (rtVECTOR_CPD_PMRD (cpd))

#define rtVECTOR_CPD_PMRD_SegmentIndex(cpd)\
    rtVECTOR_PMRD_SegmentIndex (rtVECTOR_CPD_PMRD (cpd))

#define rtVECTOR_CPD_PMRD_SegmentOrigin(cpd)\
    rtVECTOR_PMRD_SegmentOrigin (rtVECTOR_CPD_PMRD (cpd))


/*----  U-Segment Descriptor Access Macros  ----*/
#define rtVECTOR_CPD_USD(cpd)\
    M_CPD_PointerToType (cpd, rtVECTOR_CPx_USD, rtVECTOR_USDType *)

#define rtVECTOR_CPD_USDCursor(cpd)\
    M_CPD_PointerToPOP (cpd, rtVECTOR_CPx_USDCursor)

#define rtVECTOR_CPD_SetUSDCursor(cpd, usdAccessMacro)\
    (rtVECTOR_CPD_USDCursor (cpd) = &usdAccessMacro (rtVECTOR_CPD_USD (cpd)))

#define rtVECTOR_CPD_USD_PToken(cpd)\
    rtVECTOR_USD_PToken (rtVECTOR_CPD_USD (cpd))

#define rtVECTOR_CPD_USD_VStore(cpd)\
    rtVECTOR_USD_VStore (rtVECTOR_CPD_USD (cpd))

#define rtVECTOR_CPD_USD_Values(cpd)\
    rtVECTOR_USD_Values (rtVECTOR_CPD_USD (cpd))

/*----  U-Segment Index Access Macros  ----*/
#define rtVECTOR_CPD_USI(cpd)\
    M_CPD_PointerToType (cpd, rtVECTOR_CPx_USI, int *)


/******************************************
 ******************************************
 *****  Constructor Type Definitions  *****
 ******************************************
 ******************************************/

/**********************************************
 *****  U-Segment Descriptor Constructor  *****
 **********************************************/
/*---------------------------------------------------------------------------
 * A USD (U-Segment Descriptor) Constructor is a cell in a singly linked list
 * of USDCr's.
 *
 * USD Constructor Field Descriptions:
 *	m_pSuccessor		- a pointer to the successor of this USDCr in
 *				  the chain.
 *	m_pAssociatedLink	- a pointer to a link constructor associated
 *				  with this USDCr.  Used to hold intermediate
 *				  results of vector algorithms.
 *	as_pUSDC		- a pointer to a USD Constructor associated
 *				  with this USDCr.  Used to hold intermediate
 *				  results of vector algorithms.  Mutually
 *				  exclusive with the next field.
 *	as_xUSD			- a position in a USD Array associated with
 *				  this USDCr.  Used to hold intermediate
 *				  results of vector algorithms.  Mutually
 *				  exclusive with the previous field.
 *---------------------------------------------------------------------------
 */
class rtVECTOR_USDC : public VTransient {
//  Friends
    friend class rtVECTOR_CType;

//  Globals
private:
    static unsigned int g_cAllocations;
    static unsigned int g_cDeallocations;

public:
    static unsigned int AllocationCount () {
	return g_cAllocations;
    }
    static unsigned int DeallocationCount () {
	return g_cDeallocations;
    }
    static void ResetCounts () {
	g_cAllocations = 0;
	g_cDeallocations = 0;
    }

//  Construction
public:
    rtVECTOR_USDC (unsigned int xSegment, M_CPD *pStore, M_CPD *pPointer, M_CPD *pPPT)
	: m_xSegment		(xSegment)
	, m_pPPT		(pPPT)
	, m_pStore		(pStore)
	, m_pPointer		(pPointer)
	, m_pAssociatedLink	(0)
    {
	g_cAllocations++;
    }

    void SetPointerTo (M_CPD *pPointer, M_CPD *pPPT) {
	if (m_pPointer)
	    m_pPointer->release ();
	m_pPointer = pPointer;
	if (m_pPPT)
	    m_pPPT->release ();
	m_pPPT = pPPT;
    }

//  Destruction
public:
    ~rtVECTOR_USDC ();

//  Access
public:
    rtLINK_CType *AssociatedLink () const {
	return m_pAssociatedLink;
    }

    unsigned int cardinality () const {
	return rtPTOKEN_CPD_BaseElementCount (m_pPPT);
    }

    M_CPD *PPT () const {
	return m_pPPT;
    }

    M_CPD *pointerCPD () const {
	return m_pPointer;
    }
    RTYPE_Type pointerRType () const {
	return m_pPointer->RType ();
    }

    unsigned int SegmentIndex () const {
	return m_xSegment;
    }

    M_CPD *storeCPD () const {
	return m_pStore;
    }
    RTYPE_Type storeRType () const {
	return m_pStore->RType ();
    }

    unsigned int TargetSegmentIndex () const {
	return m_xTargetSegment;
    }

//  Query
public:
    bool Names (M_CPD *pStore) const {
	return m_pStore->Names (pStore);
    }
    bool Names (M_KOTM pKOTM) const {
	return m_pStore->Names (pKOTM);
    }

//  Use
public:
    void ClearAssociatedLink () {
	if (m_pAssociatedLink) {
	    m_pAssociatedLink->release ();
	    m_pAssociatedLink = 0;
	}
    }
    void ClaimAssociatedLink (rtLINK_CType *pLink) {
	ClearAssociatedLink ();
	m_pAssociatedLink = pLink;
    }

    void MakeCodAssociatedLink () {
	ClaimAssociatedLink (cardinality () ? rtLINK_RefConstructor (m_pPPT, -1) : 0);
    }
    void MakeDomAssociatedLink () {
	ClaimAssociatedLink (cardinality () ? rtLINK_PosConstructor (m_pPPT, -1) : 0);
    }

    void CloseAssociatedLink (M_CPD *pPPT) const {
	if (m_pAssociatedLink)
	    m_pAssociatedLink->Close (pPPT);
    }

    rtLINK_CType *ClaimedAssociatedLink () {
	rtLINK_CType *pAssociatedLink = m_pAssociatedLink;
	m_pAssociatedLink = 0;
	return pAssociatedLink;
    }

    void SetTargetSegmentIndexTo (unsigned int xTargetSegment) {
	m_xTargetSegment = xTargetSegment;
    }

//  State
protected:
    unsigned int	m_xSegment;
    M_CPD		*m_pPPT, 
			*m_pStore, 
			*m_pPointer;
    rtLINK_CType	*m_pAssociatedLink;
    unsigned int	m_xTargetSegment;
};


/*---------------------------------------------------------------------------
 * A P-Map Range Descriptor Constructor (PMRDCr) is a cell in a singly linked
 * list of PMRDCr's.  Each PMRDCr holds a pointer to its associated USDCr, a
 * segment origin, and a count.
 *
 * P-Map Range Descriptor Constructor Field Descriptions:
 *	m_pSuccessor		- a pointer to the next PMRDCr in the list.
 *	m_pUSDC			- a pointer to a USDCr for the U-Segment
 *				  associated with this P-Map range.
 *	m_xSegmentOrigin	- the origin of the range described by this
 *				  descriptor relative to the positional state
 *				  of the U-Segment.
 *	m_iSegmentLength	- the number of elements referenced by this
 *				  PMRDCr.
 *---------------------------------------------------------------------------
 */
class rtVECTOR_PMRDC : public VTransient {
//  Globals
private:
    static unsigned int g_cAllocations;
    static unsigned int g_cDeallocations;

public:
    static unsigned int AllocationCount () {
	return g_cAllocations;
    }
    static unsigned int DeallocationCount () {
	return g_cDeallocations;
    }
    static void ResetCounts () {
	g_cAllocations = 0;
	g_cDeallocations = 0;
    }

//  Construction
private:
    rtVECTOR_PMRDC (rtVECTOR_USDC *pUSDC, unsigned int xOrigin, unsigned int sRun)
	: m_pSuccessor		(0)
	, m_pUSDC		(pUSDC)
	, m_xSegmentOrigin	(xOrigin)
	, m_iSegmentLength	(sRun)
    {
	g_cAllocations++;
    }

public:
    rtVECTOR_PMRDC *Append (rtVECTOR_USDC *pUSDC, unsigned int xOrigin, unsigned int sRun) {
	rtVECTOR_PMRDC *pResult = this;
	if (this && pUSDC == m_pUSDC)
	    m_iSegmentLength += sRun;
	else if (sRun) {
	    pResult = new rtVECTOR_PMRDC (pUSDC, xOrigin, sRun);
	    if (this)
		m_pSuccessor = pResult;
	}
	return pResult;
    }

//  Destruction
public:
    ~rtVECTOR_PMRDC () {
	g_cDeallocations++;
    }

//  Access
public:
    rtLINK_CType *AssociatedLink () const {
	return m_pUSDC->AssociatedLink ();
    }

    M_CPD *pointerCPD () const {
	return m_pUSDC->pointerCPD ();
    }
    M_CPD *storeCPD () const {
	return m_pUSDC->storeCPD ();
    }

    rtVECTOR_PMRDC *Successor () const {
	return m_pSuccessor;
    }

    unsigned int SegmentIndex () const {
	return m_pUSDC->SegmentIndex ();
    }
    unsigned int SegmentOrigin () const {
	return m_xSegmentOrigin;
    }
    unsigned int SegmentLength () const {
	return m_iSegmentLength;
    }
    unsigned int SegmentLimit () const {
	return m_xSegmentOrigin + m_iSegmentLength;
    }

    unsigned int TargetSegmentIndex () const {
	return m_pUSDC->TargetSegmentIndex ();
    }

    rtVECTOR_USDC *USDC () const {
	return m_pUSDC;
    }

//  State
protected:
    rtVECTOR_PMRDC*		m_pSuccessor;
    rtVECTOR_USDC* const	m_pUSDC;
    unsigned int const		m_xSegmentOrigin;
    unsigned int 		m_iSegmentLength;
};


/********************************
 *****  Vector Constructor  *****
 ********************************/

/*---------------------------------------------------------------------------
 * Vector Constructor Field Descriptions:
 *	m_pPPT			- a standard CPD for the P-Token defining the
 *				  positional state of this vector constructor.
 *	m_sUSDArray		- the number of USDCr's in the USDCr list.
 *	m_pUSDArray		- a pointer to an array of pointers to USDCr's
 *				  contained in the above list.  This array is
 *				  ordered for binary search by the homogeneity
 *				  defining routines.
 *	m_fIsASet		- true if the vector constructor is a set, F
 *                                false otherwise.
 *	m_iPMRDCount		- the number of PMRDCr's in the PMRDCr list.
 *	m_pPMRDChainHead	- a pointer to the first real PMRDCr in the
 *				  list of PMRDCr's associated with this
 *				  constructor.
 *	m_pPMRDChainTail	- a pointer to the last real PMRDCr in the list
 *				  of PMRDCr's associated with this constructor.
 *---------------------------------------------------------------------------
 */
class rtVECTOR_CType : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtVECTOR_CType, VBenderenceable);

//  Construction
private:
    void MakeUSDArray ();

public:
    rtVECTOR_CType (M_CPD *pPPT, unsigned int sUSDArray);
    rtVECTOR_CType (M_CPD *pVector);

    void AppendPMRD (rtVECTOR_USDC *pUSDC, unsigned int xOrigin, unsigned int sRun) {
	rtVECTOR_PMRDC *pNewPMRD = m_pPMRDChainTail->Append (pUSDC, xOrigin, sRun);
	if (m_pPMRDChainTail != pNewPMRD) {
	    m_pPMRDChainTail = pNewPMRD;
	    m_iPMRDCount++;
	    if (!m_pPMRDChainHead)
		m_pPMRDChainHead = pNewPMRD;
	}
    }
    void AppendPMRD (unsigned int xSegment, unsigned int xOrigin, unsigned int sRun) {
	AppendPMRD (m_pUSDArray[xSegment], xOrigin, sRun);
    }
    void AppendPMRD (rtVECTOR_PMRDType const *pPMRD) {
	AppendPMRD (
	    rtVECTOR_PMRD_SegmentIndex (pPMRD),
	    rtVECTOR_PMRD_SegmentOrigin (pPMRD),
	    rtVECTOR_PMRD_SegmentLength (pPMRD)
	);
    }

    rtVECTOR_USDC *NewUSDC (
	unsigned int xSegment, M_CPD *pStore, M_CPD *pPointer, M_CPD *pPPT
    ) {
	return m_pUSDArray[xSegment] = new rtVECTOR_USDC (xSegment, pStore, pPointer, pPPT);
    }
    rtVECTOR_USDC *NewUSDC (unsigned int xSegment, M_CPD *pStore) {
	return NewUSDC (xSegment, pStore, 0, 0);
    }

    void CompactUSDArray ();

//  Destruction
protected:
    ~rtVECTOR_CType ();

//  Access
public:
    unsigned int cardinality () const {
	return rtPTOKEN_CPD_BaseElementCount (m_pPPT);
    }

    rtVECTOR_PMRDC *PMRDChainHead () const {
	return m_pPMRDChainHead;
    }

    M_CPD *PPT () const {
	return m_pPPT;
    }

    M_ASD *Space () const {
	return m_pPPT->Space ();
    }

    rtVECTOR_USDC *USDC (unsigned int xSegment) const {
	return m_pUSDArray[xSegment];
    }
    rtVECTOR_USDC *const *USDCArray () const {
	return m_pUSDArray;
    }
    rtVECTOR_USDC *const *USDCLimit () const {
	return m_pUSDArray + m_sUSDArray;
    }
    unsigned int USDCCount () const {
	return m_sUSDArray;
    }

//  Query
public:
    bool CanBeSimplified () const {
	return m_iPMRDCount == 1;
    }
    bool CantBeSimplified () const {
	return m_iPMRDCount != 1;
    }
    bool IsASet () const {
	return m_fIsASet;
    }

//  Use
public:
    void Align ();

    rtVECTOR_CType *distribute (M_CPD *pDistribution);
    rtVECTOR_CType *reorder (M_CPD *pReordering);

    void MakeConstructorLinks ();

    M_CPD *NewPToken (unsigned int nElements) const {
	return rtPTOKEN_New (m_pPPT->Space (), nElements);
    }

    M_CPD *PartitionedStatistics (
	rtLINK_CType *pPartition, rtVECTOR_PS_Type xStatistic, M_KOT *pKOT
    );

    M_CPD *PartitionedSortIndices (rtLINK_CType *pPartition, bool descending);

    void PartitionedPartition (
	rtLINK_CType	 *pPartition,
	rtLINK_CType	**majorLC,
	rtLINK_CType	**minorLC,
	M_CPD		**reordering
    );

    bool SimplifiedToMonotype (DSC_Descriptor *dscp, bool fNonDestructive);
    bool SimplifiedToMonotype (DSC_Descriptor *dscp) {
	return SimplifiedToMonotype (dscp, true);
    }

    rtLINK_CType *subsetInStore (M_CPD *pStore, VDescriptor *pValueReturn);
    rtLINK_CType *subsetInStore (M_CPD *pStore) {
	return subsetInStore (pStore, 0);
    }

    rtLINK_CType *subsetOfType (
	M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
    );
    rtLINK_CType *subsetOfType (M_ASD *pSubsetSpace, M_KOTM pKOTM) {
	return subsetOfType (pSubsetSpace, pKOTM, 0);
    }

    M_CPD *ToVector (bool fRelease);
    M_CPD *ToVector () {
	return ToVector (true);
    }

//  State
protected:
    M_CPD		 *m_pPPT;
    unsigned int	  m_sUSDArray;
    rtVECTOR_USDC	**m_pUSDArray;
    unsigned int	  m_fIsASet	: 1, 
			  m_iPMRDCount	: 31;
    rtVECTOR_PMRDC	 *m_pPMRDChainHead,
			 *m_pPMRDChainTail;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/**********************************
 *****  Vector Instantiation  *****
 **********************************/

PublicFnDecl M_CPD *rtVECTOR_NewFromUV (
    M_CPD*			resultPToken,
    M_CPD*			uSegmentStore,
    M_CPD*			uSegmentValues
);

PublicFnDecl M_CPD *rtVECTOR_NewFromDescriptor (
    DSC_Descriptor		*initialValues
);

PublicFnDecl M_CPD *rtVECTOR_New (
    M_CPD*			pTokenCPD
);

PublicFnDecl M_CPD *rtVECTOR_NewSetVector (
    M_CPD*			pTokenCPD
);

/*******************
 *****  Align  *****
 *******************/

PublicFnDecl M_CPD *rtVECTOR_Align (
    M_CPD*			vectorCPD
);

PublicFnDecl bool rtVECTOR_AlignAll (
    M_CPD*			vectorCPD,
    bool			deletingEmptyUSegments
);

/********************
 *****  Assign  *****
 ********************/

PublicFnDecl void rtVECTOR_Assign (
    M_CPD*			targetVector,
    rtLINK_CType*		linkc,
    DSC_Descriptor		*sourceDescriptor
);

PublicFnDecl void rtVECTOR_Assign (
    M_CPD*			targetVector,
    rtLINK_CType*		linkc,
    rtVECTOR_CType*		sourceVectorC
);

PublicFnDecl void rtVECTOR_Assign (
    M_CPD*			targetVector,
    rtREFUV_TypePTR_Reference	refp,
    DSC_Descriptor		*sourceDescriptor
);

PublicFnDecl void rtVECTOR_Assign (
    M_CPD*			targetVector,
    rtREFUV_TypePTR_Reference	refp,
    rtVECTOR_CType*		sourceVectorC
);

/*********************
 *****  Extract  *****
 *********************/

PublicFnDecl void rtVECTOR_Extract (
    M_CPD *pVector, rtLINK_CType *pSubscript, VDescriptor &rResult
);

PublicFnDecl void rtVECTOR_Extract (
    M_CPD *pVector, rtLINK_CType *pSubscript, M_CPD **ppResult
);
    
PublicFnDecl void rtVECTOR_RefExtract (
    M_CPD*			vectorCPD,
    rtREFUV_TypePTR_Reference	refp,
    DSC_Descriptor		*valuep
);

/********************************
 *****  Reorder/Distribute  *****
 ********************************/

PublicFnDecl rtVECTOR_CType *rtVECTOR_ReOrder (
    M_CPD*			vectorCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl rtVECTOR_CType *rtVECTOR_Distribute (
    M_CPD*			vectorCPD,
    M_CPD*			refuvCPD
);

/*****************************
 *****  Type Operations  *****
 *****************************/

PublicFnDecl rtLINK_CType *rtVECTOR_SubsetInStore (
    M_CPD *pThis, M_CPD *pStore, VDescriptor *pValueReturn = 0
);

PublicFnDecl rtLINK_CType *rtVECTOR_SubsetOfType (
    M_CPD *pThis, M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn = 0
);

PublicFnDecl bool rtVECTOR_SimplifiedToMonotype (
    M_CPD *vector, DSC_Descriptor *dscp, bool fNonDestructive = true
);

/****************************
 *****  Set Operations  *****
 ****************************/

PublicFnDecl void rtVECTOR_LookupFromValueD (
    M_CPD*			sourceCPD,
    DSC_Descriptor		*keyDsc,
    rtLINK_LookupCase		lookupCase,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*locatedDscPointer
);

PublicFnDecl void rtVECTOR_LocateOrAdd (
    M_CPD*			targetCPD,
    DSC_Descriptor*		keyDsc,
    DSC_Pointer*		locationsDscPointer,
    DSC_Pointer*		addedDscPointer = 0
);

PublicFnDecl int rtVECTOR_IsASet (
    M_CPD*			vectorCPD,
    rtLINK_CType*		*refNilsLC
);

PublicFnDecl int rtVECTOR_RestoreSetAttribute (
    M_CPD*			vectorCPD
);

/************************************
 *****  Incorporator Interface  *****
 ************************************/

PublicFnDecl void rtVECTOR_WriteDBUpdateInfo (
    M_CPD			*vectorCPD,
    char const			*itemName
);

/*********************************
 *****  Maintenance Control  *****
 *********************************/
PublicFnDecl void rtVECTOR_EnablePOPOrdering (bool flag);
PublicFnDecl void rtVECTOR_SetImplicitCleanupFlag (bool flag);
PublicFnDecl void rtVECTOR_SetUSegmentCountThreshold (unsigned int count);



/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtVECTOR_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtVECTOR_Handle, VContainerHandle);

//  Construction
protected:
    rtVECTOR_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtVECTOR_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
public:
    void CheckConsistency ();

protected:
    bool PersistReferences ();

//  State
protected:
};

    
#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvector.h 1 replace /users/mjc/system
  860404 20:14:51 mjc Create Vector Representation Type

 ************************************************************************/
/************************************************************************
  RTvector.h 2 replace /users/mjc/system
  860422 22:56:55 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTvector.h 3 replace /users/mjc/system
  860504 18:46:04 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTvector.h 4 replace /users/mjc/system
  860515 13:29:31 mjc Intermediate release of value descriptors for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.h 5 replace /users/mjc/system
  860521 11:12:05 mjc Release for coordination with 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.h 6 replace /users/mjc/system
  860527 15:38:24 mjc Return vector to library for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.h 7 replace /users/jck/system
  860605 16:21:47 jck Added Decl for _RefExtract

 ************************************************************************/
/************************************************************************
  RTvector.h 8 replace /users/mjc/system
  860623 10:24:26 mjc Converted to use new value descriptor macros, restructured assign operations

 ************************************************************************/
/************************************************************************
  RTvector.h 9 replace /users/jad/system
  860627 19:33:50 jad release assign, still working on extract

 ************************************************************************/
/************************************************************************
  RTvector.h 10 replace /users/jad/system
  860703 22:16:56 jad implemented the ReOrder routine

 ************************************************************************/
/************************************************************************
  RTvector.h 11 replace /users/jad/system
  860717 17:48:36 jad implemented rtVECTOR_MakeConstructorLinks

 ************************************************************************/
/************************************************************************
  RTvector.h 12 replace /users/jad/system
  860718 20:15:24 jad added rtVECTOR_ReverseReOrderVC 

 ************************************************************************/
/************************************************************************
  RTvector.h 13 replace /users/jck/system
  860723 12:35:29 jck Added Utilities for DataBase Building

 ************************************************************************/
/************************************************************************
  RTvector.h 14 replace /users/jad/system
  860731 17:45:27 jad added DBUPDATE utilties

 ************************************************************************/
/************************************************************************
  RTvector.h 15 replace /users/mjc/system
  861002 18:02:34 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  RTvector.h 16 replace /users/mjc/mysystem
  861004 11:10:09 mjc Declared 'rtVECTOR_PartitionStatistics'

 ************************************************************************/
/************************************************************************
  RTvector.h 17 replace /users/mjc/system
  861015 23:53:21 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTvector.h 18 replace /users/jad/system
  861106 17:56:00 jad added some functions, fixed discard constructor, renamed reversereorder to distribute

 ************************************************************************/
/************************************************************************
  RTvector.h 19 replace /users/jad/system
  861112 10:51:01 jad add routines to convert a vector into a simple descriptor

 ************************************************************************/
/************************************************************************
  RTvector.h 20 replace /users/jad/system
  870120 14:59:28 jad modified DB update procedure:

 ************************************************************************/
/************************************************************************
  RTvector.h 21 replace /users/jad/system
  870519 19:29:09 jad partial implementation of set vectors

 ************************************************************************/
/************************************************************************
  RTvector.h 22 replace /users/mjc/translation
  870524 09:36:29 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvector.h 23 replace /users/jck/system
  870824 15:03:02 jck Added a routine to do a comprehensive alignment of all usegments

 ************************************************************************/
/************************************************************************
  RTvector.h 24 replace /users/jad/system
  880711 16:42:08 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
