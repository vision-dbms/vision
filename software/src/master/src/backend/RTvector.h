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
#include "Vdd_Store.h"

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

class rtVECTOR_CType;

class VBlockTask;
class VDescriptor;
class VectorReconstructor;

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

#define rtVECTOR_CPD_IsASet(cpd)\
    rtVECTOR_V_IsASet (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_PMapSize(cpd)\
    rtVECTOR_V_PMapSize (rtVECTOR_CPD_Base (cpd))

#define rtVECTOR_CPD_IsInconsistent(cpd)\
    rtVECTOR_V_IsInconsistent (rtVECTOR_CPD_Base (cpd))

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
    
/*****
 * The following macro does not expand into efficient code.  Use a different
 * mechanism in speed critical applications.
 *****/
#define rtVECTOR_CPD_ElementCount(cpd)\
    rtVECTOR_V_ElementCount (rtVECTOR_CPD_Base (cpd))

/*----  P-Map Entry Access Macros  ----*/
#define rtVECTOR_CPD_PMRD(cpd)\
    M_CPD_PointerToType (cpd, rtVECTOR_CPx_PMRD, rtVECTOR_PMRDType *)


/*----  U-Segment Descriptor Access Macros  ----*/
#define rtVECTOR_CPD_USD(cpd)\
    M_CPD_PointerToType (cpd, rtVECTOR_CPx_USD, rtVECTOR_USDType *)

#define rtVECTOR_CPD_USDCursor(cpd)\
    M_CPD_PointerToPOP (cpd, rtVECTOR_CPx_USDCursor)

#define rtVECTOR_CPD_SetUSDCursor(cpd, usdAccessMacro)\
    (rtVECTOR_CPD_USDCursor (cpd) = &usdAccessMacro (rtVECTOR_CPD_USD (cpd)))

#define rtVECTOR_CPD_USD_PToken(cpd)\
    rtVECTOR_USD_PToken (rtVECTOR_CPD_USD (cpd))

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
    rtVECTOR_USDC (unsigned int xSegment, Vdd::Store *pStore, M_CPD *pPointer, rtPTOKEN_Handle *pPPT)
	: m_xSegment		(xSegment)
	, m_pPPT		(pPPT)
	, m_pStore		(pStore)
	, m_pPointer		(pPointer)
	, m_pAssociatedLink	(0)
    {
	g_cAllocations++;
    }

    void SetPointerTo (M_CPD *pPointer, rtPTOKEN_Handle *pPPT) {
	if (m_pPointer)
	    m_pPointer->release ();
	m_pPointer = pPointer;
	m_pPPT.setTo (pPPT);
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
	return m_pPPT->cardinality ();
    }

    rtPTOKEN_Handle *PPT () const {
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

    Vdd::Store *store () const {
	return m_pStore;
    }
    RTYPE_Type storeRType () const {
	return m_pStore->rtype ();
    }

    unsigned int TargetSegmentIndex () const {
	return m_xTargetSegment;
    }

//  Query
public:
    bool Names (Vdd::Store *pStore) const {
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
	ClaimAssociatedLink (cardinality () ? rtLINK_RefConstructor (m_pPPT) : 0);
    }
    void MakeDomAssociatedLink () {
	ClaimAssociatedLink (cardinality () ? rtLINK_PosConstructor (m_pPPT) : 0);
    }

    void CloseAssociatedLink (rtPTOKEN_Handle *pPPT) const {
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
    unsigned int		m_xSegment;
    Vdd::Store::Reference	m_pStore;
    rtPTOKEN_Handle::Reference	m_pPPT;
    M_CPD*			m_pPointer;
    rtLINK_CType*		m_pAssociatedLink;
    unsigned int		m_xTargetSegment;
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

private:
    static bool NewTail (rtVECTOR_PMRDC *&rpChainTail, rtVECTOR_USDC *const pUSDC, unsigned int xOrigin, unsigned int sRun) {
    //  Return true if a new PMRD was created, false otherwise...
	if (sRun > 0) {
	    rpChainTail = new rtVECTOR_PMRDC (pUSDC, xOrigin, sRun);
	    return true;
	}
	return false;
    }

    bool UpdateTail (rtVECTOR_PMRDC *&rpChainTail, rtVECTOR_USDC *const pUSDC, unsigned int xOrigin, unsigned int sRun) {
    //  Return true if a new PMRD was created, false otherwise...
	bool bNewTail = false;
	if (pUSDC == m_pUSDC)
	    m_iSegmentLength += sRun;
	else if (NewTail (m_pSuccessor, pUSDC, xOrigin, sRun)) {
	    rpChainTail = m_pSuccessor;
	    bNewTail = true;
	}
	return bNewTail;
    }

public:
    static bool Append (rtVECTOR_PMRDC *&rpChainTail, rtVECTOR_USDC *const pUSDC, unsigned int xOrigin, unsigned int sRun) {
    //  Return true if a new PMRD was created, false otherwise...
	return rpChainTail ? rpChainTail->UpdateTail (rpChainTail, pUSDC, xOrigin, sRun) : NewTail (rpChainTail, pUSDC, xOrigin, sRun);
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
    Vdd::Store *store () const {
	return m_pUSDC->store ();
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
 *	m_pPPT			- a handle for the P-Token defining the
 *				  positional state of this vector constructor.
 *	m_sUSDArray		- the number of USDCr's in the USDCr list.
 *	m_pUSDArray		- a pointer to an array of pointers to USDCr's
 *				  contained in the above list.  This array is
 *				  ordered for binary search by the homogeneity
 *				  defining routines.
 *	m_bASet			- true if the vector constructor is a set, F
 *                                false otherwise.
 *	m_iPMRDCount		- the number of PMRDCr's in the PMRDCr list.
 *	m_pPMRDChainHead	- a pointer to the first real PMRDCr in the
 *				  list of PMRDCr's associated with this
 *				  constructor.
 *	m_pPMRDChainTail	- a pointer to the last real PMRDCr in the list
 *				  of PMRDCr's associated with this constructor.
 *---------------------------------------------------------------------------
 */
class rtVECTOR_Handle;
class rtVECTOR_CType : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtVECTOR_CType, VBenderenceable);

//  Construction
private:
    void MakeUSDArray ();

public:
    rtVECTOR_CType (rtPTOKEN_Handle *pPPT, unsigned int sUSDArray);
    rtVECTOR_CType (rtVECTOR_Handle *pVector);

    void AppendPMRD (rtVECTOR_USDC *pUSDC, unsigned int xOrigin, unsigned int sRun) {
    //  Append returns true if the PMRD list was extended, false otherwise...
	if (rtVECTOR_PMRDC::Append (m_pPMRDChainTail, pUSDC, xOrigin, sRun)) {
	    m_iPMRDCount++;
	    if (!m_pPMRDChainHead)
		m_pPMRDChainHead = m_pPMRDChainTail;
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
	unsigned int xSegment, Vdd::Store *pStore, M_CPD *pPointer, rtPTOKEN_Handle *pPPT
    ) {
	return m_pUSDArray[xSegment] = new rtVECTOR_USDC (xSegment, pStore, pPointer, pPPT);
    }
    rtVECTOR_USDC *NewUSDC (unsigned int xSegment, Vdd::Store *pStore) {
	return NewUSDC (xSegment, pStore, 0, 0);
    }

    void CompactUSDArray ();

//  Destruction
protected:
    ~rtVECTOR_CType ();

//  Access
public:
    rtPTOKEN_Handle *alignedPToken () {
	Align ();
	return m_pPPT;
    }

    unsigned int cardinality () const {
	return m_pPPT->cardinality ();
    }

    rtVECTOR_PMRDC *PMRDChainHead () const {
	return m_pPMRDChainHead;
    }

    rtPTOKEN_Handle *PPT () const {
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
	return m_bASet;
    }

//  Use
public:
    void Align ();

    rtVECTOR_CType *distribute (M_CPD *pDistribution);
    rtVECTOR_CType *reorder (M_CPD *pReordering);

    void MakeConstructorLinks ();

    rtPTOKEN_Handle *NewPToken (unsigned int nElements) const {
	return new rtPTOKEN_Handle (m_pPPT->Space (), nElements);
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

    rtLINK_CType *subsetInStore (Vdd::Store *pStore, VDescriptor *pValueReturn);
    rtLINK_CType *subsetInStore (Vdd::Store *pStore) {
	return subsetInStore (pStore, 0);
    }

    rtLINK_CType *subsetOfType (
	M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
    );
    rtLINK_CType *subsetOfType (M_ASD *pSubsetSpace, M_KOTM pKOTM) {
	return subsetOfType (pSubsetSpace, pKOTM, 0);
    }

    void getVector (VReference<rtVECTOR_Handle> &rpResult);

//  State
protected:
    rtPTOKEN_Handle::Reference	m_pPPT;
    unsigned int		m_sUSDArray;
    rtVECTOR_USDC**		m_pUSDArray;
    rtVECTOR_PMRDC*		m_pPMRDChainHead;
    rtVECTOR_PMRDC*		m_pPMRDChainTail;
    unsigned int		m_iPMRDCount;
    bool			m_bASet;
};


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtVECTOR_Handle : public VStoreContainerHandle {
    DECLARE_CONCRETE_RTT (rtVECTOR_Handle, VStoreContainerHandle);

    friend class rtVECTOR_CType;
    friend class VBlockTask;
    friend class VDescriptor;
    friend class VectorReconstructor;

//  Constants
    enum ExtractResult {
	ExtractResult_IsEmpty, ExtractResult_IsAMonotype, ExtractResult_IsSomething
    };

//  Segment Data
public:
    class SegmentData {
    public:
	SegmentData () {
	}
	~SegmentData () {
	}
    public:
	Vdd::Store *store () const {
	    return m_pStore;
	}
	M_CPD *pointer () const {
	    return m_pPointer;
	}
	rtPTOKEN_Handle *ptoken () const {
	    return m_pPToken;
	}
    public:
	void setStoreTo (Vdd::Store *pStore) {
	    m_pStore.setTo (pStore);
	}

	void setPointerTo (M_CPD *pPointer) {
	    m_pPointer.setTo (pPointer);
	}
	void claimPointer (M_CPD *pPointer) {
	    m_pPointer.claim (pPointer);
	}

	void setPTokenTo (rtPTOKEN_Handle *pPToken) {
	    m_pPToken.setTo (pPToken);
	}
	void claimPToken (rtPTOKEN_Handle::Reference &rpPToken) {
	    m_pPToken.claim (rpPToken);
	}

    private:
	Vdd::Store::Reference		m_pStore;
	VCPDReference			m_pPointer;
	rtPTOKEN_Handle::Reference	m_pPToken;
    };

//  Construction Utilities
private:
    static size_t initialSizeFrom (rtPTOKEN_Handle *pPPT);

    void initializeSegment (
	unsigned int xSegment, Vdd::Store *pStore, M_CPD *pPointer, rtPTOKEN_Handle *pPToken
    ) {
	initializeSegment (xSegment, pStore, pPointer->containerHandle (), pPToken);
    }
    void initializeSegment (
	unsigned int xSegment, Vdd::Store *pStore, VContainerHandle *pPointer, rtPTOKEN_Handle *pPToken
    );
    void initializeSegment (unsigned int xSegment, rtVECTOR_USDC *pUSDC) {
	initializeSegment (xSegment, pUSDC->store (), pUSDC->pointerCPD (), pUSDC->PPT ());
    }
    void initializeSegment (unsigned int xSegment, SegmentData const *pInitializationData = 0);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtVECTOR_Handle (rCTE);
    }

public:
    static rtVECTOR_Handle *NewFrom (
	rtPTOKEN_Handle *pPPT, Vdd::Store *pSegmentStore, M_CPD *pSegmentValues
    );
private:
    static rtVECTOR_Handle *NewFrom (DSC_Descriptor &rInitialValues);

public:
    rtVECTOR_Handle (rtVECTOR_CType *pConstructor);
    rtVECTOR_Handle (rtPTOKEN_Handle *pPPT, bool bMarkingAsSet = false, SegmentData const *pInitialSegmentData = 0);
private:
    rtVECTOR_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    }

//  Destruction
private:
    ~rtVECTOR_Handle () {
    }

//  Alignment
private:
    virtual /*override*/ bool align_() {
	return align ();
    }
    virtual /*override*/ bool alignAll_(bool bCleaning) {
	return alignAll (bCleaning);
    }
public:
    bool align ();
    bool alignAll (bool bCleaning = true);

//  Canonicalization
private:
    virtual /*override*/ bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer);

//  Cloning
private:
    virtual /*override*/ void clone_(StoreBase::Reference &rpResult, rtPTOKEN_Handle *pPPT) const {
	Reference pHandle;
	clone (pHandle, pPPT);
	rpResult.setTo (pHandle);
    }
public:
    void clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const;

//  Attribute Access
public:
    bool isASet () const {
	return rtVECTOR_V_IsASet (typecastContent ());
    }
    bool isInconsistent () const {
	return rtVECTOR_V_IsInconsistent (typecastContent ());
    }

//  Attribute Update
private:
    void clearIsASet () {
	setIsASetTo (false);
    }
    void setIsASet () {
	setIsASetTo (true);
    }
    void setIsASetTo (bool bValue) {
	rtVECTOR_V_IsASet (typecastContent ()) = bValue;
    }
    void clearIsInconsistent () {
	setIsInconsistentTo (false);
    }
    void setIsInconsistent () {
	setIsInconsistentTo (true);
    }
    void setIsInconsistentTo (bool bValue) {
	rtVECTOR_V_IsInconsistent (typecastContent ()) = bValue;
    }

//  Component Access
private:
    virtual /*overrride*/ rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const;
    virtual /*overrride*/ rtPTOKEN_Handle *getPToken_() const {
	return getPToken ();
    }
public:
    rtPTOKEN_Handle *alignedPToken () {
	align ();
	return ptokenHandle ();
    }
    rtPTOKEN_Handle *getPToken () const {
	return ptokenHandle ();
    }

//  Component Access
private:
    rtVECTOR_Type *typecastContent () const {
	return reinterpret_cast<rtVECTOR_Type *>(containerContent ());
    }
    M_POP *ptokenPOP () const {
	return &rtVECTOR_V_PToken (typecastContent ());
    }
    rtPTOKEN_Handle *ptokenHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (ptokenPOP (), RTYPE_C_PToken));
    }
    rtVECTOR_PMRDType *pmap () const {
	return rtVECTOR_V_PMap (typecastContent ());
    }
    unsigned int pmapSize () const {
	return rtVECTOR_V_PMapSize (typecastContent ());
    }
    pointer_t pmapLimit () const {
	rtVECTOR_Type *pContent = typecastContent ();
	return reinterpret_cast<pointer_t>(rtVECTOR_V_PMap (pContent) + rtVECTOR_V_PMapSize (pContent));
    }
    rtVECTOR_USDType *segmentArray () const {
	return rtVECTOR_V_USegArray (typecastContent ());
    }
public:
    unsigned int segmentArraySize () const {
	return rtVECTOR_V_USegArraySize (typecastContent ());
    }
private:
    pointer_t segmentArrayLimit () const {
	rtVECTOR_Type *pContent = typecastContent ();
	return reinterpret_cast<pointer_t>(rtVECTOR_V_USegArray (pContent) + rtVECTOR_V_USegArraySize (pContent));
    }
    unsigned int segmentFreeBound () const {
	return rtVECTOR_V_USegFreeBound (typecastContent ());
    }
    int *segmentIndex () const {
	return rtVECTOR_V_USegIndex (typecastContent ());
    }
public:
    unsigned int segmentIndexElement (unsigned int xElement) const {
	return segmentIndex ()[xElement];
    }
    unsigned int segmentIndexSize () const {
	return rtVECTOR_V_USegIndexSize (typecastContent ());
    }
private:
    pointer_t segmentIndexLimit () const {
	rtVECTOR_Type *pContent = typecastContent ();
	return (pointer_t)(rtVECTOR_V_USegIndex (pContent) + rtVECTOR_V_USegIndexSize (pContent));
    }
    rtVECTOR_USDType *segment (unsigned int xSegment) const {
	return segmentArray() + xSegment;
    }
    rtPTOKEN_Handle *segmentPTokenHandle (unsigned int xSegment) const {
	return static_cast<rtPTOKEN_Handle*>(
	    GetContainerHandle (segmentPTokenPOP (xSegment), RTYPE_C_PToken)
	);
    }
    M_CPD *segmentPointerCPD (unsigned int xSegment) const {
	return segmentPointerHandle (xSegment)->GetCPD ();
    }
    VContainerHandle *segmentPointerHandle (unsigned int xSegment) const {
	return GetContainerHandle (segmentPointerPOP (xSegment));
    }
    VContainerHandle *segmentStoreHandle (unsigned int xSegment) const {
	return GetContainerHandle (segmentStorePOP (xSegment));
    }
    M_POP *segmentPTokenPOP (unsigned int xSegment) const {
	return &rtVECTOR_USD_PToken (segment (xSegment));
    }
    M_POP *segmentPointerPOP (unsigned int xSegment) const {
	return &rtVECTOR_USD_Values (segment (xSegment));
    }
    M_POP *segmentStorePOP (unsigned int xSegment) const {
	return &rtVECTOR_USD_VStore (segment (xSegment));
    }

public:
    void getSegmentPToken (rtPTOKEN_Handle::Reference &rpResult, unsigned int xSegment) const {
	rpResult.setTo (segmentPTokenHandle (xSegment));
    }

    void getSegmentPointer (M_CPD *&rpResult, unsigned int xSegment) const {
	rpResult = segmentPointerHandle (xSegment)->GetCPD ();
    }
    void getSegmentPointer (VContainerHandle::Reference &rpResult, unsigned int xSegment) const {
	rpResult.setTo (segmentPointerHandle (xSegment));
    }

    void getSegmentStore (VContainerHandle::Reference &rpResult, unsigned int xSegment) const {
	rpResult.setTo (segmentStoreHandle (xSegment));
    }
    bool getSegmentStore (Vdd::Store::Reference &rpResult, unsigned int xSegment) const {
	return segmentStoreHandle (xSegment)->getStore (rpResult);
    }

    unsigned int segmentCardinality (unsigned int xSegment) const {
	return rtPTOKEN_BaseElementCount (this, segmentPTokenPOP (xSegment));
    }

//  Component Query
public:
    bool segmentInUse (unsigned int xSegment) const {
	return ReferenceIsntNil (segmentPTokenPOP (xSegment));
    }
    bool segmentUnused (unsigned int xSegment) const {
	return ReferenceIsNil (segmentPTokenPOP (xSegment));
    }

//  Element Access Support
private:
    void CheckForReferenceCompatibility (Vdd::Store *pStore, M_CPD *pPointer) {
	CheckForReferenceCompatibility (pStore, pPointer->containerHandle ());
    }
    void CheckForReferenceCompatibility (Vdd::Store *pStore, VContainerHandle *pPointer);

    unsigned int LocateOrAddSegment (
	Vdd::Store *pStore, RTYPE_Type xRType, rtPTOKEN_Handle *pSegmentRPT, bool bASet
    );
    bool LocateSegment (Vdd::Store *pStore, unsigned int &rxSegment, M_POP *pIdentityReturn = 0);

    ExtractResult SetupExtract (rtLINK_CType *pSubscript);
    void DoSimpleExtract (
	rtLINK_CType *pSubscript, Vdd::Store::Reference &rpResultStore, M_CPD *&rpResultPointer
    );
    void DoComplexExtract (rtLINK_CType *pSubscript, rtVECTOR_CType::Reference &rpResult);

//  Element Access
private:
    virtual bool getElements_(VReference<rtVECTOR_Handle> &rpResult, rtLINK_CType *pSubscript) {
	return getElements (rpResult, pSubscript);
    }
    virtual bool getElements_(VDescriptor &rResult, rtLINK_CType *pSubscript) {
	return getElements (rResult, pSubscript);
    }
    virtual bool getElements_(DSC_Descriptor &rResult, DSC_Scalar &rSubscript) {
	return getElements (rResult, rSubscript);
    }
public:
    unsigned int elementCount () const {
	return rtVECTOR_V_ElementCount (typecastContent ());
    }

    void getElements (M_CPD *&rpResult, rtLINK_CType *pSubscript) {
	rtVECTOR_Handle::Reference pResult;
	getElements (pResult, pSubscript);
	rpResult = pResult->GetCPD ();
    }
    bool getElements (rtVECTOR_Handle::Reference &rpResult, rtLINK_CType *pSubscript);
    bool getElements (VDescriptor &rResult, rtLINK_CType *pSubscript);
    bool getElements (DSC_Descriptor &rResult, DSC_Scalar &rSubscript);

    rtVECTOR_CType *reorder (M_CPD *pReordering);
    rtVECTOR_CType *distribute (M_CPD *pDistribution);

    bool canBeSimplifiedToMonotype () const {
	return pmapSize () == 1;
    }
    bool cantBeSimplifiedToMonotype () const {
	return pmapSize () != 1;
    }
    bool simplifiedToMonotype (DSC_Descriptor &rpResult, bool bNonDestructive = true); // return = true if successful, else false

//  Element Update Support
private:
    void setSegmentPTokenTo (unsigned int xSegment, rtPTOKEN_Handle *pSegmentPPT) {
	StoreReference (segmentPTokenPOP (xSegment), pSegmentPPT);
    }
    void setSegmentValuesTo (
	M_CPD *&rpTarget, unsigned int xTarget, unsigned int xSubscript, DSC_Descriptor &rSource
    );
    void setSegmentValuesTo (
	M_CPD *&rpTarget, unsigned int xTarget, rtLINK_CType*pSubscript, DSC_Descriptor &rSource
    );
    void setSegmentValuesTo (
	M_CPD *&rpTarget, unsigned int xTarget, rtLINK_CType*pSubscript, M_CPD *pSource
    );

    void ExpandSegmentArray (unsigned int sExpansion);
    void RemoveEmptySegments (int *pRelocationMap, unsigned int sRelocationMap);

    void decrementSegmentArraySizeBy (unsigned int sChange) {
	rtVECTOR_V_USegArraySize (typecastContent ()) -= sChange;
    }
    void decrementSegmentIndexSizeBy (unsigned int sChange) {
	rtVECTOR_V_USegIndexSize (typecastContent ()) -= sChange;
    }
    void decrementPMapSizeBy (unsigned int sChange) {
	rtVECTOR_V_PMapSize (typecastContent ()) -= sChange;
    }
    
    void incrementSegmentArraySizeBy (unsigned int sChange) {
	rtVECTOR_V_USegArraySize (typecastContent ()) += sChange;
    }
    void incrementSegmentIndexSizeBy (unsigned int sChange) {
	rtVECTOR_V_USegIndexSize (typecastContent ()) += sChange;
    }
    void incrementPMapSizeBy (unsigned int sChange) {
	rtVECTOR_V_PMapSize (typecastContent ()) += sChange;
    }

    void setSegmentArraySizeTo (unsigned int sArray) {
	rtVECTOR_V_USegArraySize (typecastContent ()) = sArray;
    }
    void setSegmentIndexSizeTo (unsigned int sIndex) {
	rtVECTOR_V_USegIndexSize (typecastContent ()) = sIndex;
    }
    void setPMapSizeTo (unsigned int sPMap) {
	rtVECTOR_V_PMapSize (typecastContent ()) = sPMap;
    }

    void setSegmentFreeBoundTo (unsigned int xBound) {
	rtVECTOR_V_USegFreeBound (typecastContent ()) = xBound;
    }

//  Element Update
private:
    virtual /*override*/ bool setElements_(rtLINK_CType *pSubscript, DSC_Descriptor &rValues) {
	return setElements (pSubscript, rValues);
    }
    virtual /*override*/ bool setElements_(rtLINK_CType *pSubscript, rtVECTOR_CType *pValues) {
	return setElements (pSubscript, pValues);
    }
    virtual /*override*/ bool setElements_(DSC_Scalar &rSubscript, DSC_Descriptor &rValues) {
	return setElements (rSubscript, rValues);
    }
    virtual /*override*/ bool setElements_(DSC_Scalar &rSubscript, rtVECTOR_CType *pValues) {
	return setElements (rSubscript, pValues);
    }
public:
    bool setElements (rtLINK_CType *pSubscript, DSC_Descriptor &rValues);
    bool setElements (rtLINK_CType *pSubscript, rtVECTOR_CType *pValues);
    bool setElements (DSC_Scalar &rSubscript, DSC_Descriptor &rValues);
    bool setElements (DSC_Scalar &rSubscript, rtVECTOR_CType *pValues);

//  Instance Deletion
private:
    virtual bool deleteInstances_(DSC_Scalar &pInstances) {
	return doInstanceDeletion (pInstances);
    }
    virtual bool deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) {
	return doInstanceDeletion (pInstances, rpTrues, rpFalses);
    }

//  Instance Locate
public:
    void locate (
	Vdd::Store *pKeyStore, DSC_Scalar &rKeyPointer, rtLINK_LookupCase xLookupCase, DSC_Pointer &rLocationsReturn, DSC_Pointer &rLocatedReturn
    );
    void locate (
	Vdd::Store *pKeyStore, M_CPD *pKeyPointer, rtLINK_LookupCase xLookupCase, DSC_Pointer &rLocationsReturn, DSC_Pointer &rLocatedReturn
    );
    void locate (
	DSC_Descriptor &rKey, rtLINK_LookupCase xLookupCase, DSC_Pointer &rLocationsReturn, DSC_Pointer &rLocatedReturn
    );

//  Instance LocateOrAdd
private:
    unsigned int AddNewPMapEntryToSetVector (int xSegmentIndex);
    void AdjustPMapVectorOrigins (unsigned int xInitialEntry, int sAdjustment);
    void UpdateVectorSetPositionalState (unsigned int xSegment, unsigned int xVectorOrigin, M_CPD *pNewSegmentPointer);
public:
    void locateOrAdd (
	Vdd::Store *pKeyStore, DSC_Scalar &rKeyPointer, DSC_Pointer &rLocationsReturn, DSC_Pointer *pAdditionsReturn = 0
    );
    void locateOrAdd (
	Vdd::Store *pKeyStore, M_CPD *pKeyPointer, DSC_Pointer &rLocationsReturn, DSC_Pointer *pAdditionsReturn = 0
    );
    void locateOrAdd (DSC_Descriptor &rKey, DSC_Pointer &rLocationsReturn, DSC_Pointer *pAdditionsReturn = 0);

//  Instance Set Maintenance
 public:
    bool isASet (rtLINK_CType **refNilsLC);
    bool restoreSetAttribute ();

//  Instance Subsets
public:
    rtLINK_CType *subsetInStore (Vdd::Store *pStore, VDescriptor *pValueReturn = 0);
    rtLINK_CType *subsetOfType (M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn = 0);

//  Kludges
private:
    void setPTokenTo (rtVSTORE_Handle *pStore);

//  Callbacks
public:
    void CheckConsistency ();
protected:
    bool PersistReferences ();

//  Display and Inspection
public:
    virtual /*override*/ void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference);
    virtual /*override*/ unsigned int getClusterReferenceMapSize ();

    virtual /*override*/ unsigned __int64 getClusterSize ();

    virtual /*override*/ unsigned int getPOPCount () const {
	return 1/*ptoken*/ + rtVECTOR_USD_POPsPerUSD * segmentArraySize ();
    }
    virtual /*override*/ bool getPOP (M_POP *pResult, unsigned int xPOP) const;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

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
