/*****  Link Representation Type Header File  *****/
#ifndef rtLINK_H
#define rtLINK_H

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

#include "RTlink.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTptoken.h"
#include "RTrefuv.h"

#include "VCPDReference.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/**************************************
 *****  Standard CPD Definitions  *****
 **************************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtLINK_CPD_StdPtrCount	2

#define rtLINK_CPx_PosPToken	(unsigned int)0
#define rtLINK_CPx_RefPToken	(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtLINK_CPD_Base(cpd)\
    ((rtLINK_Type *)M_CPD_ContainerBase (cpd))

#define rtLINK_CPD_PosPToken(cpd)\
    M_CPD_PointerToPOP (cpd, rtLINK_CPx_PosPToken)

#define rtLINK_CPD_RefPToken(cpd)\
    M_CPD_PointerToPOP (cpd, rtLINK_CPx_RefPToken)

#define rtLINK_CPD_Tracking(cpd)\
    rtLINK_L_Tracking (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_HasRepeats(cpd)\
    rtLINK_L_HasRepeats (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_HasRanges(cpd)\
    rtLINK_L_HasRanges (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_IsInconsistent(cpd)\
    rtLINK_L_IsInconsistent (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_RRDCount(cpd)\
    rtLINK_L_RRDCount (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_RRDArray(cpd)\
    rtLINK_L_RRDArray (rtLINK_CPD_Base (cpd))

#define rtLINK_CPD_ElementCount(cpd)\
    rtLINK_RRD_LinkCumulative (rtLINK_CPD_RRDArray (cpd) + rtLINK_CPD_RRDCount (cpd))


/***********************************************
 ***********************************************
 *****  Link Constructor Type Definitions  *****
 ***********************************************
 ***********************************************/
/*---------------------------------------------------------------------------
 * As defined above, links are a permanent data structure built for storage by
 * the memory manager.  The persistent structure defined above matches the
 * memory manager's requirements for permanent storage but is not the most
 * appropriate form for creating and using links.  That form is a link
 * constructor.  All links start life as a link constructor; all 'subscripting'
 * uses of links use link constructors.  The following definitions describe
 * the structure of a link constructor.
 *---------------------------------------------------------------------------
 */


/**************************
 *  RRD Constructor Type  *
 **************************/
/*---------------------------------------------------------------------------
 * An RRD Constructor is a cell in a singly linked list of RRDs.
 *
 * RRD Constructor Field Descriptions:
 *	m_iRRD			- this constructor's RRD.
 *	m_pNextRRDC		- a pointer to this RRDC's successor.
 *---------------------------------------------------------------------------
 */
struct rtLINK_RRDCType {
    rtLINK_RRDType		m_iRRD;
    rtLINK_RRDCType *		m_pNextRRDC;
};

/*****  Access Macros  *****/
#define rtLINK_RRDC_RRD(rrdcp)		((rrdcp)->m_iRRD)
#define rtLINK_RRDC_NextRRDC(rrdcp)	((rrdcp)->m_pNextRRDC)

#define rtLINK_RRDC_LinkCumulative(rrdcp) rtLINK_RRD_D_LinkCumulative (\
    rtLINK_RRDC_RRD (rrdcp)\
)

#define rtLINK_RRDC_RepeatedRef(rrdcp) rtLINK_RRD_D_RepeatedRef (\
    rtLINK_RRDC_RRD (rrdcp)\
)

#define rtLINK_RRDC_ReferenceOrigin(rrdcp) rtLINK_RRD_D_RefOrigin (\
    rtLINK_RRDC_RRD (rrdcp)\
)

#define rtLINK_RRDC_N(rrdcp) (\
    rtLINK_RRDC_LinkCumulative (rtLINK_RRDC_NextRRDC (rrdcp)) - rtLINK_RRDC_LinkCumulative (rrdcp)\
)


/*******************************************
 * Constructs for the append state machine *
 *******************************************/

enum rtLINK_AppendState {
    _rtLINK_NothingSTATE, 
    _rtLINK_RangeSTATE, 
    _rtLINK_RepeatSTATE, 
    _rtLINK_SingleSTATE
};


/***************************
 *  Link Constructor Type  *
 ***************************/
/*---------------------------------------------------------------------------
 * A link constructor is a header which contains CPD's for the positional and
 * referential state of the 'link' and pointers to the head and tail of a
 * chain of RRDC's which contain the state of the 'link'.  Like a 'link',
 * there is always a final RRDC which contains the total number of elements in
 * this 'link'.
 *
 * Link Constructor Field Descriptions:
 *	m_pPPT			- a handle for the positional P-Token
 *				  associated with this link constructor.
 *	m_pRPT			- a handle for the referential P-Token
 *				  associated with this link constructor.
 *	m_fHasRepeats		- true if any of the 'rrdc's are repetitions.
 *	m_fHasRanges		- true if any of the 'rrdc's are ranges.
 *	m_fTracking		- used by align to determine whether to split 
 *		                  or enlarge ranges.
 *	m_iRRDCount		- the number of RRDC's in the RRDC chain.
 *				  This count does NOT include the final RRDC
 *				  used to hold the number of elements in the
 *				  link constructor.
 *	m_xReferenceLimit	- a least upper bound on the references of the
 *				  link constructor.  The highest real position
 *				  referenced by the link constructor will be
 *				  one less than this value.  This field is
 *				  valid for 'open' link constructors ONLY !!!
 *	m_iReferenceCount	- the number of references to this link 
 *			          constructor. 
 *	m_pChainHead		- a pointer to the first real RRDC in the RRDC
 *				  chain.  This field will never point to the
 *				  final element count RRDC.
 *	m_pChainTail		- a pointer to the last real RRDC in the RRDC
 *				  chain.  This field will never point to the
 *				  final element count RRDC.
 *	m_pFinalRRDC		- a pointer to the final, element count RRDC.
 *---------------------------------------------------------------------------
 */


/*******************
 *  Access Macros  *
 *******************/

#define rtLINK_LC_HasRepeats(lcp)	((lcp)->m_fHasRepeats)
#define rtLINK_LC_HasRanges(lcp)	((lcp)->m_fHasRanges)
#define rtLINK_LC_Tracking(lcp)		((lcp)->m_fTracking)
#define rtLINK_LC_RRDCount(lcp)		((lcp)->m_iRRDCount)
#define rtLINK_LC_ReferenceLimit(lcp)	((lcp)->m_xReferenceLimit)
#define rtLINK_LC_AppendState(lcp)	((lcp)->m_xAppendState)    
#define rtLINK_LC_ChainHead(lcp)	((lcp)->m_pChainHead)
#define rtLINK_LC_ChainTail(lcp)	((lcp)->m_pChainTail)
#define rtLINK_LC_FinalRRDC(lcp)	((lcp)->m_pFinalRRDC)


/**********************
 *  Class Definition  *
 **********************/

class rtLINK_CType : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtLINK_CType, VBenderenceable);

//  Free Lists and Counters
protected:

//  Construction
public:
    rtLINK_CType (rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT);

//  Destruction
private:
    ~rtLINK_CType ();

protected:
    inline void ThreadNewRRD (
	bool repeatedRef, int referenceOrigin, int count
    );

public:
    rtLINK_CType *Append (int origin, int count, bool repeating);
    rtLINK_CType *AppendRange (int origin, int count) {
	return Append (origin, count, false);
    }
    rtLINK_CType *AppendRepeat (int origin, int count) {
	return Append (origin, count, true);
    }
    rtLINK_CType *Close (M_CPD *pPTokenRef, int xPTokenRef);
    rtLINK_CType *Close (rtPTOKEN_Handle *pPToken);

//  Destruction
protected:

//  Access
public:
    unsigned int BaseElementCount () const {
	return m_pPPT->cardinality ();
    }
    unsigned int ElementCount () const {
	return rtLINK_RRDC_LinkCumulative (m_pFinalRRDC);
    }
    unsigned int LastReference () const {
	return IsNil (m_pChainTail)
	    ? 0
	    : rtLINK_RRDC_RepeatedRef (m_pChainTail)
	    ? rtLINK_RRDC_ReferenceOrigin (m_pChainTail)
	    : rtLINK_RRDC_ReferenceOrigin (m_pChainTail) + rtLINK_RRDC_N (m_pChainTail) - 1;
    }
    unsigned int ReferenceNil () const {
	return m_pRPT->cardinality ();
    }

    rtPTOKEN_Handle *PPT () const {
	return m_pPPT;
    }
    bool PPTNames (VContainerHandle *pThat, M_POP const *pThatPOP) const {
	return pThatPOP ? pThat->ReferenceNames (pThatPOP, PPT ()) : pThat->Names (PPT ());
    }
    bool PPTDoesntName (VContainerHandle *pThat, M_POP const *pThatPOP) const {
	return pThatPOP ? pThat->ReferenceDoesntName (pThatPOP, PPT ()) : pThat->DoesntName (PPT ());
    }

    rtPTOKEN_Handle *RPT () const {
	return m_pRPT;
    }
    bool RPTNames (VContainerHandle *pThat, M_POP const *pThatPOP) const {
	return pThatPOP ? pThat->ReferenceNames (pThatPOP, RPT ()) : pThat->Names (RPT ());
    }
    bool RPTDoesntName (VContainerHandle *pThat, M_POP const *pThatPOP) const {
	return pThatPOP ? pThat->ReferenceDoesntName (pThatPOP, RPT ()) : pThat->DoesntName (RPT ());
    }

//  Query
public:
    bool ContainsReferenceNils () const {
	/***** Warning: 'this' must be aligned *****/
	return IsntNil (m_pChainTail) && (
	    rtLINK_RRDC_RepeatedRef (m_pChainTail)
	    ? rtLINK_RRDC_ReferenceOrigin (m_pChainTail)
	    : rtLINK_RRDC_ReferenceOrigin (m_pChainTail) + rtLINK_RRDC_N (m_pChainTail) - 1
	) == m_pRPT->cardinality ();
    }
    bool DoesntContainReferenceNils () const {
	return !ContainsReferenceNils ();
    }

    bool IsOpen () const {
	return m_pPPT.isNil () || m_pRPT.isNil ();
    }
    bool IsntOpen () const {
	return m_pPPT.isntNil () && m_pRPT.isntNil ();
    }

//  Maintenance
private:
    void ReconstructFrom (rtLINK_CType *pOther);

public:
    rtLINK_CType *Align ();

    void AlignForAdd (M_CPD *pTargetRef, int xTargetRef) {
	AlignForAdd (pTargetRef->containerHandle (), pTargetRef->OutboundPOP (xTargetRef));
    }
    void AlignForAdd (VContainerHandle *pTargetRef, M_POP const *pTargetPOP);

    void AlignForAssign (M_CPD *pTargetRef, int xTargetRef) {; // Scalar source
	AlignForAssign (pTargetRef->containerHandle (), pTargetRef->OutboundPOP (xTargetRef));
    }
    void AlignForAssign (VContainerHandle *pTargetRef, M_POP const *pTargetPOP);

    void AlignForAssign (
	M_CPD *pTargetRef, int xTargetRef, M_CPD *pSourceRef, int xSourceRef
    ) {
	AlignForAssign (
	    pTargetRef->containerHandle (), pTargetRef->OutboundPOP (xTargetRef),
	    pSourceRef->containerHandle (), pSourceRef->OutboundPOP (xSourceRef)
	);
    }
    void AlignForAssign (
	VContainerHandle *pTargetRef, M_POP const *pTargetPOP, VContainerHandle *pSourceRef, M_POP const *pSourcePOP = 0
    );

    void AlignForCoerce ();

    void AlignForDistribute (M_CPD *pSourceRef, int xSourceRef) {
	AlignForDistribute (pSourceRef->containerHandle (), pSourceRef->OutboundPOP (xSourceRef));
    }
    void AlignForDistribute (VContainerHandle *pSourceRef, M_POP const *pSourcePOP);

    void AlignForExtract (M_CPD *pSourceRef, int xSourceRef) {
	AlignForExtract (pSourceRef->containerHandle (), pSourceRef->OutboundPOP (xSourceRef));
    }
    void AlignForExtract (VContainerHandle *pSourceRef, M_POP const *pSourcePOP);

//  Update
public:
    rtLINK_CType *Add (rtLINK_CType *addSourceLinkC, bool additionsMapWanted = true);

//  Use
public:
    rtLINK_CType *Complement ();

    rtLINK_CType *Extract (rtLINK_CType *pSubcript);
    M_CPD	 *Extract (M_CPD *pSubscript);

    M_CPD *ToLink (rtPTOKEN_Handle *pNewPPT, bool fDiscard);
    M_CPD *ToLink (bool fDiscard) {
	return ToLink (0, fDiscard);
    }
    M_CPD *ToLink () {
	return ToLink (0, true);
    }

    M_CPD *ToRefUV (M_CPD *orderingUV = NilOf (M_CPD*));

//  Analysis and Debugging
private:
    void DisplayThreadNewRRDTrace () const;

public:
    void AnalyzeCompression () const;

    void DebugPrint () const;

//  State
public:
    rtPTOKEN_Handle::Reference	m_pPPT;
    rtPTOKEN_Handle::Reference	m_pRPT;
    unsigned int		m_fTracking		: 1, 
				m_fHasRepeats		: 1, 
				m_fHasRanges		: 1, 
				m_iRRDCount		: 29;
    int				m_xReferenceLimit;
    rtLINK_AppendState		m_xAppendState;
    rtLINK_RRDCType*		m_pChainHead;
    rtLINK_RRDCType*		m_pChainTail;
    rtLINK_RRDCType*		m_pFinalRRDC;
};


/*******************************************
 * Covering Defines for the Append Member  *
 *******************************************/

#define rtLINK_Append(constructor,origin,count,repeating)\
    ((constructor)->Append ((origin), (count), (repeating)))

#define rtLINK_AppendRange(constructor,origin,count)\
    rtLINK_Append (constructor, (int)(origin), (int)(count), false)

#define rtLINK_AppendRepeat(constructor,origin,count)\
    rtLINK_Append (constructor, (int)(origin), (int)(count), true)


/**************************************************
 *  Covering Defines for the Add-Locate Routines  *
 **************************************************/

/*---------------------------------------------------------------------------
 * Macro to Add values to a link.  
 *
 *  Arguments:
 *	targetLinkCPD	- a standard CPD for the target link.
 *	addSourceLinkC  - a link constructor containing the source values to
 *                        add.
 *
 *  Returns:
 *	a link constructor containing the positions in the target where 
 *      values were added.
 *
 *****/
#define rtLINK_LCAdd(targetLinkCPD, addSourceLinkC) rtLINK_LCAddLocate (\
    targetLinkCPD, addSourceLinkC, false, NilOf (rtLINK_CType **)\
)

/*---------------------------------------------------------------------------
 * Macro to Locate or Add to a link.  This routine will only add the source
 * values which are not already in the target link.
 *
 *  Arguments:
 *	indexLink	- a standard CPD for the target link.
 *	keyLinkC	- the source link constructor.
 *	addedLinkC	- optional (if not requested should be Nil)
 *			  a pointer to an uninitialized link constructor which
 *			  will contain the positions from the source linkc
 * 		          whose values were added to the target link.  If all
 *                        of the values were added, then this link will be set
 *                        to Nil.
 *
 *  Returns:
 *	a link constructor containing the positions in the target where values
 *      were added.
 *
 *****/
#define rtLINK_LocateOrAddFromLC(indexLink, keyLinkC, addedLinkC) rtLINK_LCAddLocate (\
    indexLink, keyLinkC, true, addedLinkC\
)


/***************************************
 *  Link Constructor Traversal Macros  *
 ***************************************/
/*---------------------------------------------------------------------------
 * The following macro implements a parameterized link constructor traversal
 * loop for use by the various routines which must process a link constructor
 * sequentially.
 *
 *  Arguments:
 *	linkc			- the NAME of a variable which holds a pointer
 *				  to the link constructor to be traversed.
 *	nilRefMacro		- the NAME of a macro which will be invoked
 *				  to perform a 'nil' reference operations.
 *				  This macro will be passed three arguments
 *				  guaranteed to be simple variables which
 *				  may be modified as convenient: the current
 *				  link cumulative, the number of references
 *				  described by the RRDC, and the reference
 *				  'nil' value.
 *	repeatedRefMacro	- the NAME of a macro which will be invoked
 *				  to perform repeated reference operations.
 *				  This macro will be passed three arguments
 *				  guaranteed to be simple variables which
 *				  may be modified as convenient: the current
 *				  link cumulative, the number of references
 *				  described by the RRDC, and the reference
 *				  origin.
 *	rangeRefMacro		- the NAME of a macro which will be invoked
 *				  to perform repeated reference operations.
 *				  This macro will be passed three arguments
 *				  guaranteed to be simple variables which
 *				  may be modified as convenient: the current
 *				  link cumulative, the number of references
 *				  described by the RRDC, and the reference
 *				  origin.
 *---------------------------------------------------------------------------
 */
#define rtLINK_TraverseRRDCList(linkc, nilRefMacro, repeatedRefMacro, rangeRefMacro) {\
    int	referenceNil = linkc->RPT ()->cardinality (),\
	sendLastNil = false,\
	c, n, r, t, cc;\
\
    rtLINK_RRDCType *rrdcp = rtLINK_LC_ChainHead (linkc);\
    for (rtLINK_RRDCType *nrrdcp = rrdcp ? rtLINK_RRDC_NextRRDC (rrdcp) : 0;\
         IsntNil (nrrdcp);\
	 nrrdcp = rtLINK_RRDC_NextRRDC (rrdcp = nrrdcp)\
    ) {\
	c = rtLINK_RRDC_LinkCumulative (rrdcp);\
	n = rtLINK_RRDC_N (rrdcp);\
\
	if ((r = rtLINK_RRDC_ReferenceOrigin (rrdcp)) >= referenceNil) {\
	    r = referenceNil;\
	    nilRefMacro (c, n, r);\
	}\
	else if (rtLINK_RRDC_RepeatedRef (rrdcp)) {\
	    repeatedRefMacro (c, n, r);\
	}\
	else if ((r+n-1) == referenceNil) {\
	    t = n - 1;\
	    cc = c;\
	    rangeRefMacro (c, t, r);\
	    sendLastNil = true;\
	}\
    	else {\
	    rangeRefMacro (c, n, r);\
	}\
    }\
\
    if (sendLastNil) {\
	t = cc + n - 1;\
	n = 1;\
	nilRefMacro (t, n, referenceNil);\
    }\
}


/*
 *---------------------------------------------------------------------------
 * This macro is the same as the above macro except it takes an extra 
 * argument 'extraArg' which it passes as the 4th argument to 'nilRefMacro',
 * 'repeatedRefMacro' and 'rangeRefMacro'.
 *---------------------------------------------------------------------------
 */
#define rtLINK_TraverseRRDCListExtraArg(linkc, nilRefMacro, repeatedRefMacro, rangeRefMacro, extraArg) {\
    int	referenceNil = linkc->RPT ()->cardinality (),\
	sendLastNil = false,\
	c, n, r, t, cc;\
\
    rtLINK_RRDCType *rrdcp = rtLINK_LC_ChainHead (linkc);\
    for (rtLINK_RRDCType *nrrdcp = rrdcp ? rtLINK_RRDC_NextRRDC (rrdcp) : 0;\
         IsntNil (nrrdcp);\
	 nrrdcp = rtLINK_RRDC_NextRRDC (rrdcp = nrrdcp)\
    ) {\
	c = rtLINK_RRDC_LinkCumulative (rrdcp);\
	n = rtLINK_RRDC_N (rrdcp);\
\
	if ((r = rtLINK_RRDC_ReferenceOrigin (rrdcp)) >= referenceNil) {\
	    r = referenceNil;\
	    nilRefMacro (c, n, r, extraArg);\
	}\
	else if (rtLINK_RRDC_RepeatedRef (rrdcp)) {\
	    repeatedRefMacro (c, n, r, extraArg);\
	}\
	else if ((r+n-1) == referenceNil) {\
	    t = n - 1;\
	    cc = c;\
	    rangeRefMacro (c, t, r, extraArg);\
	    sendLastNil = true;\
	}\
    	else {\
	    rangeRefMacro (c, n, r, extraArg);\
	}\
    }\
\
    if (sendLastNil) {\
	t = cc + n - 1;\
	n = 1;\
	nilRefMacro (t, n, referenceNil, extraArg);\
    }\
}


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/*********************
 *****  Dumping  *****
 *********************/

PublicFnDecl void rtLINK_DumpLink (
    M_CPD*			linkCPD
);

/***********************************************
 *****  Constructor Creation and Deletion  *****
 ***********************************************/

PublicFnDecl rtLINK_CType *rtLINK_PosConstructor (
    M_CPD *posPTokenRefCPD, int posPTokenRefIndex
);
PublicFnDecl rtLINK_CType *rtLINK_RefConstructor (
    M_CPD *refPTokenRefCPD, int refPTokenRefIndex
);

PublicFnDecl rtLINK_CType *rtLINK_PosConstructor (rtPTOKEN_Handle *pPPT);
PublicFnDecl rtLINK_CType *rtLINK_RefConstructor (rtPTOKEN_Handle *pRPT);

/**********************
 *****  Aligning  *****
 **********************/

PublicFnDecl M_CPD *rtLINK_Align (
    M_CPD*			linkCPD
);

/***************************
 *****  Link Creation  *****
 ***************************/

PublicFnDecl M_CPD *rtLINK_NewRefLink (rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT);

PublicFnDecl M_CPD *rtLINK_NewEmptyLink (rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT);

PublicFnDecl M_CPD *rtLINK_Copy (M_CPD *link, rtPTOKEN_Handle *pNewPPT = 0);

/*********************************************
 *****  Link <-> Constructor Conversion  *****
 *********************************************/

PublicFnDecl rtLINK_CType *rtLINK_ToConstructor (
    M_CPD*			linkCPD
);

/***************************************************
 *****  Link/Constructor <-> RefUV Conversion  *****
 ***************************************************/

PublicFnDecl rtLINK_CType *rtLINK_RefUVToConstructor (
    M_CPD*			refuvCPD,
    M_CPD**			ppSortReturn = NilOf (M_CPD**),
    bool			sortMustBeExpressed = false
);

PublicFnDecl M_CPD *rtLINK_LinkToRefUV (
    M_CPD*			linkCPD,
    M_CPD*			orderingUV = NilOf (M_CPD *)
);

/*********************
 *****  Extract  *****
 *********************/

PublicFnDecl rtLINK_CType *rtLINK_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		pSubcript
);

PublicFnDecl M_CPD *rtLINK_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refUVCPD
);

PublicFnDecl void rtLINK_RefExtract (
    rtREFUV_TypePTR_Reference	result,
    M_CPD*			source,
    rtREFUV_TypePTR_Reference	reference
);

/*******************************
 *****  Add/Locate/Delete  *****
 *******************************/

PublicFnDecl rtLINK_CType *rtLINK_LCAddLocate (
    M_CPD*			targetLinkCPD,
    rtLINK_CType*		addSourceLinkC,
    int				locateOnly,
    rtLINK_CType*		*addedLinkC
);

PublicFnDecl bool rtLINK_LocateOrAddFromRef (
    M_CPD*			targetLink,
    rtREFUV_TypePTR_Reference	srcRefp,
    rtREFUV_TypePTR_Reference	locationRefp
);

PublicFnDecl void rtLINK_LookupUsingLCKey (
    M_CPD*			sourceLink,
    rtLINK_CType*		keyLinkC,
    rtLINK_LookupCase		lookupCase,
    int				partitionSize,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtLINK_LookupUsingRefKey (
    M_CPD*			sourceLink,
    rtREFUV_TypePTR_Reference	keyRefp,
    rtLINK_LookupCase		lookupCase,
    int				partitionSize,
    rtREFUV_TypePTR_Reference	locationRefp
);

PublicFnDecl void rtLINK_DeleteLCSelectedElements (
    M_CPD*			sourceLink,
    rtLINK_CType*		elementLinkC,
    rtLINK_CType*		*deletedLinkC
);

PublicFnDecl bool rtLINK_DeleteRefSelectedElement (
    M_CPD*			sourceLink,
    rtREFUV_TypePTR_Reference	elementRefp
);

/**************************
 *****  Distribution  *****
 **************************/

PublicFnDecl M_CPD *rtLINK_Distribute (
    rtLINK_CType*		referenceLC,
    rtLINK_CType*		sourceLC
);

/************************
 *****  Projection  *****
 ************************/

PublicFnDecl M_CPD *rtLINK_LCCountReferences (
    rtLINK_CType*		linkc,
    rtPTOKEN_Handle*		refPToken,
    M_CPD*			distributionCPD
);

PublicFnDecl M_CPD *rtLINK_ColumnProjection (
    rtLINK_CType*		cartesianLinkC
);

PublicFnDecl rtLINK_CType *rtLINK_RowProjection (
    rtLINK_CType*		cartesianLinkC
);

PublicFnDecl rtLINK_CType *rtLINK_LinearizeLCrRc (
    rtPTOKEN_Handle*		cartesianPT,
    rtLINK_CType*		rowLinkC,
    rtREFUV_TypePTR_Reference	columnRefp
);

PublicFnDecl M_CPD *rtLINK_LinearizeLCrUVic (
    rtPTOKEN_Handle*		cartesianPT,
    rtLINK_CType*		rowLinkC,
    M_CPD*			columnUV,
    M_CPD*			columnIndirection
);

PublicFnDecl M_CPD *rtLINK_LinearizeLCrLCic (
    rtPTOKEN_Handle*		cartesianPT,
    rtLINK_CType*		rowLinkC,
    rtLINK_CType*		columnLinkC,
    M_CPD*			columnIndirection
);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtLINK_Handle : public VContainerHandle {
    DECLARE_CONCRETE_RTT (rtLINK_Handle, VContainerHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtLINK_Handle (rCTE);
    }
protected:
    rtLINK_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

//  Destruction
private:
    ~rtLINK_Handle () {
    }

//  Alignment
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
public:
    bool align ();

//  Access
private:
    rtLINK_Type *typecastContent () const {
	return reinterpret_cast<rtLINK_Type*>(containerContent ());
    }
    M_POP *pptPOP () const {
	return &rtLINK_L_PosPToken (typecastContent ());
    }
    M_POP *rptPOP () const {
	return &rtLINK_L_RefPToken (typecastContent ());
    }
public:
    rtPTOKEN_Handle *pptHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (pptPOP (), RTYPE_C_PToken));
    }
    rtPTOKEN_Handle *rptHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (rptPOP (), RTYPE_C_PToken));
    }
    unsigned int elementCount () const {
	rtLINK_Type *pContent = typecastContent ();
	return rtLINK_RRD_LinkCumulative (rtLINK_L_RRDArray (pContent) + rtLINK_L_RRDCount (pContent));
    }

    rtPTOKEN_Handle *getPPT () const {
	return pptHandle ();
    }
    rtPTOKEN_Handle *getRPT () const {
	return rptHandle ();
    }

    rtLINK_RRDType *rrdArray () const {
	return rtLINK_L_RRDArray (typecastContent ());
    }
    unsigned int rrdCount () const {
	return rtLINK_L_RRDCount (typecastContent ());
    }

//  Forwarding
private:
    virtual bool forwardToSpace_(M_ASD *pSpace) OVERRIDE;

//  Callbacks
public:
    virtual void CheckConsistency () OVERRIDE;

protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 2;
    }
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlink.h 1 replace /users/mjc/system
  860421 12:29:53 mjc Create link representation type

 ************************************************************************/
/************************************************************************
  RTlink.h 2 replace /users/mjc/system
  860422 22:55:58 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTlink.h 3 replace /users/mjc/system
  860504 18:45:54 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTlink.h 4 replace /users/mjc/system
  860513 09:29:23 mjc Added logic for 'LCExtract' - not yet working

 ************************************************************************/
/************************************************************************
  RTlink.h 5 replace /users/jad/system
  860523 15:33:52 jad release link to link extractions

 ************************************************************************/
/************************************************************************
  RTlink.h 6 replace /users/jad/system
  860523 16:20:18 jad release empty shells for add routines

 ************************************************************************/
/************************************************************************
  RTlink.h 7 replace /users/jad/system
  860603 20:25:22 jad implemented add and the extract routine for CHharlie

 ************************************************************************/
/************************************************************************
  RTlink.h 8 replace /users/jad/system
  860605 18:18:44 jad implement 1st version of Align, add ref cnt 
to linkc's, and remove references to valuedsc refuv

 ************************************************************************/
/************************************************************************
  RTlink.h 9 replace /users/jad/system
  860612 15:03:00 jad fully implemented align, added ToLinkNoDiscard,
discard constructor now zeros out chain head and tail

 ************************************************************************/
/************************************************************************
  RTlink.h 10 replace /users/mjc/system
  860623 10:22:00 mjc Converted to use new 'valuedsc' macros

 ************************************************************************/
/************************************************************************
  RTlink.h 11 replace /users/mjc/system
  860701 09:11:17 mjc Correctly incorrect declarations of extract routines

 ************************************************************************/
/************************************************************************
  RTlink.h 12 replace /users/mjc/system
  860709 10:15:41 mjc Release new format 'reference's for continued development by 'jad'

 ************************************************************************/
/************************************************************************
  RTlink.h 13 replace /users/jad/system
  860717 17:47:56 jad made dump link constructor public

 ************************************************************************/
/************************************************************************
  RTlink.h 14 replace /users/jad/system
  860723 16:42:09 jad added ToLink routine with a new pos ptoken

 ************************************************************************/
/************************************************************************
  RTlink.h 15 replace /users/mjc/system
  860805 18:46:48 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  RTlink.h 16 replace /users/jad/system
  860903 12:56:08 jad 1) added hasRepeats and hasRanges fields to both links and linkcs 2) added 2 simple link creation routines

 ************************************************************************/
/************************************************************************
  RTlink.h 17 replace /users/jad/system
  860929 12:01:03 jad made DumpLink public 

 ************************************************************************/
/************************************************************************
  RTlink.h 18 replace /users/mjc/system
  861014 22:53:29 mjc Added 'AlignForDistribute', fixed empty link alignment bug in 'AlignConstructor'

 ************************************************************************/
/************************************************************************
  RTlink.h 19 replace /users/jad/system
  861023 17:09:17 jad freed a ptoken, and implemented linkcopy

 ************************************************************************/
/************************************************************************
  RTlink.h 20 replace /users/jad/system
  861024 14:07:13 jad removed some temp code left in

 ************************************************************************/
/************************************************************************
  RTlink.h 21 replace /users/jad/system
  861119 12:43:08 jad added rtLINK_LCComplement routine, needed by vmachine

 ************************************************************************/
/************************************************************************
  RTlink.h 22 replace /users/jad/system
  861205 17:32:08 jad added LocateOrAdd, and Lookup routines

 ************************************************************************/
/************************************************************************
  RTlink.h 23 replace /users/jad/system
  861210 18:47:37 jad added ref versions of locateOrAdd and lookup

 ************************************************************************/
/************************************************************************
  RTlink.h 24 replace /users/jad/system
  861219 15:43:36 jad added LCCountReferences routine, fixed
LocateOrAdd to not add duplicates

 ************************************************************************/
/************************************************************************
  RTlink.h 25 replace /users/jad/system
  870303 16:13:04 jad added linearization, projection, & deletion routines

 ************************************************************************/
/************************************************************************
  RTlink.h 26 replace /users/jad/system
  870413 15:36:59 jad 1) fixed bug in reference LocateOrAdd routine,
2) added a distribute routine, 3) added an argument to lookup routines: panSize

 ************************************************************************/
/************************************************************************
  RTlink.h 27 replace /users/mjc/translation
  870524 09:33:32 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlink.h 29 replace /users/jck/system
  880826 09:42:37 jck Removing sequence links from the main line of descent

 ************************************************************************/
