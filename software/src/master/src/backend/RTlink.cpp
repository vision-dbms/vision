/*****  Link Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtLINK

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "vdsc.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTintuv.h"
#include "RTrefuv.h"

#include "V_VAllocator.h"

#include "VCPDReference.h"

/*****  Self  *****/
#include "RTlink.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtLINK_Handle);


/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool
    AnalyzingCompression	= false,
    TracingLCAllocator		= false,
    TracingLCAppending		= false,
    TracingLookup		= false,
    TracingRRDCAllocator	= false;


/****************************
 *****  Usage Counters  *****
 ****************************/

PrivateVarDef unsigned int
    RefConstructorCount		= 0,
    PosConstructorCount		= 0,
    AppendCount			= 0,
    CloseConstructorCount	= 0,
    DiscardConstructorCount	= 0,

    AllocateLCCount		= 0,
    MaxRRDCCount		= 0,

    AnalyzerLinkCount		= 0,
    AnalyzerRRDCCount		= 0,
    AnalyzerDomCount		= 0,
    AnalyzerCodCount		= 0,
    AnalyzerMaxRRDCs		= 0,
    AnalyzerMaxRRDCsHits	= 0,
    AnalyzerMaxRRDCsDomShrink	= 0,
    AnalyzerMaxRRDCsCodShrink	= 0,
    AnalyzerMaxDomShrink	= 0,
    AnalyzerMaxDomShrinkHits	= 0,
    AnalyzerMaxCodShrink	= 0,
    AnalyzerMaxCodShrinkHits	= 0,

    AlignLinkCount		= 0,
    AlignConstructorCount	= 0,

    BasicToLinkCount		= 0,
    ToConstructorCount		= 0,

    BasicNewEmptyLinkCount	= 0,
    NewRefLinkCount		= 0,
    CopyLinkCount		= 0,

    ConstructorToRefUVCount	= 0,
    RefUVToConstructorCount	= 0,
    LinkToRefUVCount		= 0,

    LCExtractFromLCCount	= 0,
    LCExtractCount		= 0,
    UVExtractFromLCCount	= 0,
    UVExtractCount		= 0,
    RefExtractCount		= 0,

    LCAddFromLCCount		= 0,
    LCAddCount			= 0,
    LocateOrAddFromLCCount	= 0,
    LocateOrAddFromRefCount	= 0,

    LookupUsingLCKeyCount	= 0,
    LookupUsingRefKeyCount	= 0,
    DeleteLCElementsCount	= 0,
    DeleteRefElementCount	= 0,

    LCCountReferencesCount	= 0,
    LCComplementCount		= 0,

    ColumnProjectionCount	= 0,
    RowProjectionCount		= 0,
    LinearizeLCrRcCount		= 0,
    LinearizeLCrLCicCount	= 0,
    LinearizeLCrUVicCount	= 0,

    TryAnotherRRDCount		= 0,
    LocateCount			= 0;


/*******************************
 *****  General Utilities  *****
 *******************************/

void rtLINK_Handle::CheckConsistency () {
    if (rtLINK_L_IsInconsistent ((rtLINK_Type*)ContainerContent ())
    ) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "Corrupted link[%d:%d] detected", SpaceIndex (), ContainerIndex ()
	)
    );
}


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard 'Link' CPD.
 *
 *  Argument:
 *	cpd			- the address of the CPD to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *cpd) {
    rtLINK_Type *base = rtLINK_CPD_Base (cpd);

    rtLINK_CPD_PosPToken (cpd) = &rtLINK_L_PosPToken (base);
    rtLINK_CPD_RefPToken (cpd) = &rtLINK_L_RefPToken (base);
}


/************************************************
 *****  Link Constructor Creation Routines  *****
 ************************************************/
/*---------------------------------------------------------------------------
 * Link constructors are characterized as 'open' or 'closed'.  Newly created
 * link constructors are 'open'.  An open link constructor has had either its
 * positional or referential state (but not both) specified.  'Open' link
 * constructors are initialized by the 'Append' routine.  'Open' link
 * constructors, because they are incomplete, cannot be used for subscripted
 * extraction or assignment operations.  'Open' link constructors are made
 * usable for these operations by 'CloseConstructor'.
 * 'Closed' link constructors cannot be modified by the 'Append' routine.
 *---------------------------------------------------------------------------
 */


/*******************
 *  Run Time Type  *
 *******************/

DEFINE_CONCRETE_RTT (rtLINK_CType);

/************************************************************
 *  Constructor Allocator Definitions and Utility Routines  *
 ************************************************************/

/*****  Constructor Free Lists  *****/
PrivateVarDef V::VAllocatorFreeList RRDConstructorFreeList (sizeof (rtLINK_RRDCType));


/*****  Constructor Allocators  *****/
/*---------------------------------------------------------------------------
 *****  Internal macro to allocate a RRD constructor.
 *
 *  Arguments:
 *	result		- a variable of type 'rtLINK_RRDCType*' which this
 *                        routine will set to the new rrd constructor
 * 			  allocated.
 *
 *****/
#define NewRRDC() ((rtLINK_RRDCType*)RRDConstructorFreeList.allocate ())


/*---------------------------------------------------------------------------
 *****  Internal macro to allocate a new RRD constructor and attach it to a
 *****  link constructor.
 *
 *  Arguments:
 *	repeatedRef		- the value of the 'repeatedRef' RRD slot.
 *	referenceOrigin		- the value of the 'referenceOrigin' RRD slot.
 *	count			- the number of positions contributed to the
 *				  link.
 *
 *****/
void rtLINK_CType::ThreadNewRRD (bool repeatedRef, int referenceOrigin, int count) {
/*****  ...convert one element repetitions into ranges...  *****/
    int repeat = repeatedRef;
    if (count == 1)
        repeat = false;

/*****  ... otherwise, initialize a new RRD constructor ...  *****/
    rtLINK_RRDCType *pChainTail = m_pChainTail;
    rtLINK_RRDCType *newRRDC = NewRRDC ();

    rtLINK_RRDC_NextRRDC	(newRRDC) = m_pFinalRRDC;
    rtLINK_RRDC_LinkCumulative	(newRRDC) = ElementCount ();
    rtLINK_RRDC_RepeatedRef	(newRRDC) = repeat;
    rtLINK_RRDC_ReferenceOrigin	(newRRDC) = referenceOrigin;

/*****  ... attach it to the link constructor ...  *****/
    if (pChainTail)
	rtLINK_RRDC_NextRRDC (pChainTail)= newRRDC;
    else
        m_pChainHead = newRRDC;

    m_pChainTail = newRRDC;

/*****  Adjust counts  *****/
    rtLINK_RRDC_LinkCumulative (m_pFinalRRDC) += count;
    m_iRRDCount++;

    /*** Set the Repeat and Range bits from the previous RRDC ***/
    /* NOTE: rtLINK_Append may modify the RRDC at the end of the chain so
     * don't count on it being permanent until another RRDC is added.  The
     * last RRDC value is counted in the close routine.
     */
    if (pChainTail) {
	if (rtLINK_RRDC_RepeatedRef (pChainTail))
	    m_fHasRepeats = true;
	else
	    m_fHasRanges = true;
    }

/*****  Print debug information if requested ... *****/
    if (TracingRRDCAllocator || TracingLCAppending)
	DisplayThreadNewRRDTrace ();
}


/**********************************
 *****  Compression Analysis  *****
 **********************************/

void rtLINK_CType::AnalyzeCompression () const {
    if (m_iRRDCount < 1)
	return;

    enum AnalyzerState {
	StartingRun, InRun
    } xDomState = StartingRun, xCodState = StartingRun;
    unsigned int iDomDelta, iCodDelta, iDelta;
    unsigned int iDomCount = m_iRRDCount + 1;
    unsigned int iCodCount = m_iRRDCount;

    rtLINK_RRDCType *pLast = m_pChainHead;
    rtLINK_RRDCType *pThis;
    while (m_pFinalRRDC != (pThis = rtLINK_RRDC_NextRRDC (pLast))) {
	iDelta = rtLINK_RRDC_LinkCumulative (pThis) - rtLINK_RRDC_LinkCumulative (pLast);
	switch (xDomState) {
	case StartingRun:
	    iDomDelta = iDelta;
	    xDomState = InRun;
	    break;
	case InRun:
	    if (iDomDelta != iDelta)
		xDomState = StartingRun;
	    else
		iDomCount--;
	    break;
	}

	iDelta = rtLINK_RRDC_ReferenceOrigin (pThis) - rtLINK_RRDC_ReferenceOrigin (pLast);
	switch (xCodState) {
	case StartingRun:
	    iCodDelta = iDelta;
	    xCodState = InRun;
	    break;
	case InRun:
	    if (iCodDelta != iDelta)
		xCodState = StartingRun;
	    else
		iCodCount--;
	    break;
	}

	pLast = pThis;
    }

    iDelta = rtLINK_RRDC_LinkCumulative (pThis) - rtLINK_RRDC_LinkCumulative (pLast);
    switch (xDomState) {
    case InRun:
	if (iDomDelta == iDelta)
	    iDomCount--;
	break;
    default:
	break;
    }

    AnalyzerLinkCount++;
    AnalyzerRRDCCount	+= m_iRRDCount + 1;
    AnalyzerDomCount	+= iDomCount;
    AnalyzerCodCount	+= iCodCount;

    if (AnalyzerMaxRRDCs == m_iRRDCount + 1)
	AnalyzerMaxRRDCsHits++;
    else if (
	AnalyzerMaxRRDCs < m_iRRDCount + 1
    ) {
	AnalyzerMaxRRDCs = m_iRRDCount + 1;
	AnalyzerMaxRRDCsDomShrink = m_iRRDCount + 1 - iDomCount;
	AnalyzerMaxRRDCsCodShrink = m_iRRDCount + 1 - iCodCount;
	AnalyzerMaxRRDCsHits = 1;
    }

    if (AnalyzerMaxDomShrink == m_iRRDCount + 1 - iDomCount)
	AnalyzerMaxDomShrinkHits++;
    else if (
	AnalyzerMaxDomShrink < m_iRRDCount + 1 - iDomCount
    ) {
	AnalyzerMaxDomShrink = m_iRRDCount + 1 - iDomCount;
	AnalyzerMaxDomShrinkHits = 1;
    }

    if (AnalyzerMaxCodShrink == m_iRRDCount + 1 - iCodCount)
	AnalyzerMaxCodShrinkHits++;
    else if (
	AnalyzerMaxCodShrink < m_iRRDCount + 1 - iCodCount
    ) {
	AnalyzerMaxCodShrink = m_iRRDCount + 1 - iCodCount;
	AnalyzerMaxCodShrinkHits = 1;
    }
}


/***************************
 *****  Debug Tracing  *****
 ***************************/

PublicFnDef char const *rtLINK_AppendStateName (rtLINK_AppendState xState) {
    switch (xState) {
    case _rtLINK_NothingSTATE:
	return "nothing";
    case _rtLINK_RangeSTATE:
	return "range";
    case _rtLINK_RepeatSTATE:
	return "repeat";
    case _rtLINK_SingleSTATE:
	return "single";
    default:
	return UTIL_FormatMessage ("state#%u", xState);
    }
}

void rtLINK_CType::DisplayThreadNewRRDTrace () const {
    IO_printf (
	"...ThreadNewRRDConstuctor: LC:%08X, CH:%08X, CT:%08X, F:%08X\n",
	this, m_pChainHead, m_pChainTail, m_pFinalRRDC
    );
    IO_printf (
	"                           RC:%08X, NX:%08X, %d:%d%c%d\n",
	m_pChainTail,
	rtLINK_RRDC_NextRRDC (m_pChainTail),
	rtLINK_RRDC_LinkCumulative (m_pChainTail),
	rtLINK_RRDC_ReferenceOrigin (m_pChainTail),
	rtLINK_RRDC_RepeatedRef (m_pChainTail) ? '*' : '+',
	rtLINK_RRDC_N (m_pChainTail)
    );
}

PrivateFnDef void DisplayModifyRRDCTrace (
    rtLINK_AppendState xState, unsigned int origin, unsigned int count
) {
    IO_printf (
	"--- Modifing Piece: %s, Origin=%d, Count=%d\n",
	rtLINK_AppendStateName (xState), origin, count
    );
}

void rtLINK_CType::DebugPrint () const {
    IO_printf ("\n>> Link Constructor Dump <<\n");
    IO_printf (
	"   LC:%08X, ChainHead:%08X, ChainTail:%08X, FinalRRDC:%08X\n",
	this, m_pChainHead, m_pChainTail, m_pFinalRRDC
    );

    IO_printf (
	"   RRDCount=%d, ReferenceLimit=%d, ElementCount=%d\n",
	m_iRRDCount, m_xReferenceLimit, ElementCount ()
    );

    IO_printf (
	"   ReferenceCount=%d, HasRepeats=%d HasRanges=%d Tracking=%d\n",
	m_iReferenceCount.operator V::unsigned_int32_count_t::value_t (),
	m_fHasRepeats, m_fHasRanges, m_fTracking
    );

    if (m_pChainHead) {
	for (rtLINK_RRDCType *rrdcp = m_pChainHead;
	     rrdcp != m_pFinalRRDC;
	     rrdcp = rtLINK_RRDC_NextRRDC (rrdcp)
	) IO_printf (
	    "   RRDC:%08X, NextRRDC:%08X, %d:%d%c%d\n",
	    rrdcp,
	    rtLINK_RRDC_NextRRDC (rrdcp),
	    rtLINK_RRDC_LinkCumulative (rrdcp),
	    rtLINK_RRDC_ReferenceOrigin (rrdcp),
	    rtLINK_RRDC_RepeatedRef (rrdcp) ? '*' : '+',
	    rtLINK_RRDC_N (rrdcp)
	);
    }
}


/*---------------------------------------------------------------------------
 *****  Internal routine to allocate a partially initialized link constructor.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	The address of the link constructor allocated.
 *
 *****/
rtLINK_CType::rtLINK_CType ()
: BaseClass		(1)
, m_pPPT		(0)
, m_pRPT		(0)
, m_pChainHead		(0)
, m_pChainTail		(0)
, m_pFinalRRDC		(NewRRDC ())
, m_iRRDCount		(0)
, m_fHasRanges		(false)
, m_fHasRepeats		(false)
, m_fTracking		(false)
, m_xReferenceLimit	(0)
, m_xAppendState	(_rtLINK_NothingSTATE)
{
    AllocateLCCount++;

    rtLINK_RRDC_LinkCumulative (m_pFinalRRDC) = 0;
    rtLINK_RRDC_NextRRDC (m_pFinalRRDC) = NilOf (rtLINK_RRDCType*);

    if (TracingLCAllocator) display (
	"...AllocateLinkConstructor: LC:%08X\n", this
    );
}


/*****************************************************
 *  Initialized Reference Link Constructor Creation  *
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a referentially initialized link constructor.
 *
 *  Arguments:
 *	refPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  specifies the referential state of the link
 *				  constructor being created.
 *
 *  Returns:
 *	The address of the newly created link constructor.
 *
 *  Notes:
 *	The link constructor returned by this routine must have its positional
 *	state specified by 'rtLINK_CloseConstructor' before it can be used by
 *	the subscripted extraction and assignment routines.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_RefConstructor (
    M_CPD *refPTokenRefCPD, int refPTokenRefIndex
) {
    rtLINK_CType *result = new rtLINK_CType ();

    RefConstructorCount++;
    if (refPTokenRefIndex >= 0)
	result->m_pRPT = refPTokenRefCPD->GetCPD (refPTokenRefIndex, RTYPE_C_PToken);
    else {
	RTYPE_MustBeA (
	    "rtLINK_RefConstructor", M_CPD_RType (refPTokenRefCPD), RTYPE_C_PToken
	);

	refPTokenRefCPD->retain ();
	result->m_pRPT = refPTokenRefCPD;
    }

    if (TracingLCAllocator) IO_printf (
	"...rtLINK_RefConstructor: LC:%08X created.\n", result
    );

    return result;
}


/****************************************************
 *  Initialized Position Link Constructor Creation  *
 ****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a positionally initialized link constructor.
 *
 *  Arguments:
 *	posPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  specifies the positional state of the link
 *				  constructor being created.
 *
 *  Returns:
 *	The address of the newly created link constructor.
 *
 *  Notes:
 *	The link constructor returned by this routine must have its referential
 *	state specified by 'rtLINK_CloseConstructor' before it can be used by
 *	the subscripted extraction and assignment routines.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_PosConstructor (
    M_CPD *posPTokenRefCPD, int posPTokenRefIndex
) {
    rtLINK_CType *result = new rtLINK_CType ();

    PosConstructorCount++;
    if (posPTokenRefIndex >= 0)
	result->m_pPPT = posPTokenRefCPD->GetCPD (posPTokenRefIndex, RTYPE_C_PToken);
    else {
	RTYPE_MustBeA (
	    "rtLINK_PosConstructor", M_CPD_RType (posPTokenRefCPD), RTYPE_C_PToken
	);

	posPTokenRefCPD->retain ();
	result->m_pPPT = posPTokenRefCPD;
    }

    if (TracingLCAllocator) IO_printf (
	"...rtLINK_PosConstructor: LC:%08X created.\n", result
    );

    return result;
}


/*************************************
 *  Link Constructor Append Routine  *
 *************************************/

/*---------------------------------------------------------------------------
 *****  Routine to append a reference to an open link constructor.
 *
 *  Arguments:
 *	origin			- the origin of the repitition or range in the
 *				  referential target of this link.
 *				  This value must be greater than the last
 *				  element referenced by this constructor.
 *	count			- the size of the repitition or range
 *				  referenced.
 *	fIsConstant		- if true the reference to append is a
 *				  repetition otherwise its a range.
 *
 *  Returns:
 *	'this'
 *
 *  Notes:
 *	For referentially initialized constructors, the range specified by
 *	'origin' and 'count' must lie totally within the range of defined
 *	positions in the target.  For positionally initialized constructors,
 *	the number of elements specified by 'count' must not cause the total
 *	number of elements specified by RRDC's to exceed the number of elements
 *	in the positional state of the constructor.
 *
 *****/
namespace {
    template <unsigned int State> class Classification_{
    public:
	static bool IsARepeatingReference () {
	    return false;
	}
    };

    template <> class Classification_<(unsigned int)_rtLINK_RepeatSTATE> {
    public:
	static bool IsARepeatingReference () {
	    return true;
	}
    };
}
#define IsARepeatingReference(xState) Classification_<(unsigned int)xState>::IsARepeatingReference ()

rtLINK_CType *rtLINK_CType::Append (int origin, int count, bool fIsConstant) {
    static char const *const pBadInputTypeMessage = "rtLINK_Append: state machine bad input type";

    enum stateCases {
	overlapSingle,
	contigSingle,
	discontigSingle,
	overlapRepeat,
	contigRepeat,
	discontigRepeat,
	overlapRange,
	contigRange,
	discontigRange
    } input;

#define newPiece(state,origin,count) {\
    m_xAppendState = (state);\
    /*****  Don't thread empty constructors...  *****/\
    if ((count) > 0)\
	ThreadNewRRD (IsARepeatingReference (state), (origin), (count));\
}

#define newPieceCC(state,origin,count) {\
    m_xAppendState = (state);\
    ThreadNewRRD (IsARepeatingReference (state), (origin), (count));\
}

#define modifyPiece(state,origin,count) {\
    m_xAppendState = state;\
    rtLINK_RRDC_LinkCumulative (m_pFinalRRDC) += (count) - rtLINK_RRDC_N(chaint);\
    rtLINK_RRDC_RepeatedRef	(chaint) = IsARepeatingReference (state);\
    rtLINK_RRDC_ReferenceOrigin (chaint) = (origin);\
    if (TracingLCAppending)\
	DisplayModifyRRDCTrace (state, origin, count);\
}
    AppendCount++;

/***** Don't do anything if the count is less than one *****/
    if (count <= 0)
	return this;

/***** Validate the boundaries ... *****/
    unsigned int newReferenceLimit = fIsConstant ? origin + 1 : origin + count;

/*****  Validate 'origin' relative to the current reference limit  *****/
    if (origin + 1 < m_xReferenceLimit) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtLINK_Append: Reference Out of Order: O=%d, RL=%d",
	    origin, m_xReferenceLimit - 1
	)
    );

/*****  Validate the input against what is known about the constructor:  *****/
    if (IsNil (m_pPPT)) {
/*****  ... validate the input referentially.  *****/
	if (newReferenceLimit > ReferenceNil () + 1) ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtLINK_Append: Reference Out of Bounds: NRL=%d, RN=%d",
		newReferenceLimit - 1, ReferenceNil ()
	    )
	);
    }
    else if (IsNil (m_pRPT)) {
/*****  ... validate the input positionally.  *****/
	if (ElementCount () + count > BaseElementCount ()) ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtLINK_Append: Count Out of Bounds: EC=%d, BEC=%d",
		ElementCount () + count, BaseElementCount ()
	    )
	);
    }
/*****  ... complain because this is a closed link constructor.  *****/
    else ERR_SignalFault (
	EC__InternalInconsistency, "rtLINK_Append: Closed Constructor"
    );

/*****  If we get this far, update the link constructor.  *****/
    m_xReferenceLimit = newReferenceLimit;

/***** Set up for the state machine ... *****/
    int oldOrigin=0, oldCount=0;

    rtLINK_RRDCType *chaint = m_pChainTail;
    rtLINK_AppendState state = m_xAppendState;
    if (state != _rtLINK_NothingSTATE) {
	oldOrigin = rtLINK_RRDC_ReferenceOrigin (chaint);
	oldCount  = rtLINK_RRDC_N (chaint);
    }

    enum inputType {
	singleton, range, repeat
    } type = count == 1 ? singleton : fIsConstant ? repeat : range;

    enum positioning {
	discontig, contig, overlap
    } position;

    switch (state) {
    case _rtLINK_NothingSTATE:
	position = discontig;  /** actually this is not used **/
	break;

    case _rtLINK_SingleSTATE:
    case _rtLINK_RepeatSTATE:
	position = oldOrigin == origin ? overlap : oldOrigin+1 == origin ? contig : discontig;
	break;

    default:
	position =
	    oldOrigin+oldCount == origin + 1	? overlap :
	    oldOrigin+oldCount == origin	? contig : discontig;
	break;
    }

    switch (type) {
    case singleton:
	switch(position) {
	case overlap:
	    input = overlapSingle;
	    break;
	case contig:
	    input = contigSingle;
	    break;
	case discontig:
	    input = discontigSingle;
	    break;
	}
	break;
    case repeat:
	switch(position) {
	case overlap:
	    input = overlapRepeat;
	    break;
	case contig:
	    input = contigRepeat;
	    break;
	case discontig:
	    input = discontigRepeat;
	    break;
	}
	break;
    default:
	switch(position) {
	case overlap:
	    input = overlapRange;
	    break;
	case contig:
	    input = contigRange;
	    break;
	case discontig:
	    input = discontigRange;
	    break;
	}
    }

    if (TracingLCAppending) {
	IO_printf (
	    "in rtLINK_Append: state=%s, type=%s, position=%s, input=%d\n",
	    (state == _rtLINK_SingleSTATE) ? "_rtLINK_SingleSTATE" :
	    (state == _rtLINK_RangeSTATE)  ? "_rtLINK_RangeSTATE"  :
	    "_rtLINK_RepeatSTATE",
	    (type == singleton) ? "single" :
	    (type == range) ? "range" : "repeat",
	    (position == overlap) ? "overlap" :
	    (position == contig) ? "contig" : "discontig",
	    input
	);
	IO_printf (
	    "        oldOrigin=%d, oldCount=%d, origin=%d, count=%d\n",
	    oldOrigin, oldCount, origin, count
	);
    }

/***** Begin the state machine ... *****/
    switch (state) {
    case _rtLINK_NothingSTATE:
	switch (type) {
	case singleton:
	    newPiece (_rtLINK_SingleSTATE, origin, count);
	    break;

	case repeat:
	    newPiece (_rtLINK_RepeatSTATE, origin, count);
	    break;

	case range:
	    newPiece (_rtLINK_RangeSTATE, origin, count);
	    break;

	default:
	    ERR_SignalFault (EC__InternalInconsistency, pBadInputTypeMessage);
	}
	break;

    case _rtLINK_SingleSTATE:
	switch (input) {
	case overlapSingle:
	case overlapRepeat:
	    /** goto repetition **/
	    modifyPiece (_rtLINK_RepeatSTATE, origin, oldCount+count);
	    break;

	case contigSingle:
	case contigRange:
	    /** goto range **/
	    modifyPiece (_rtLINK_RangeSTATE, oldOrigin, oldCount+count);
	    break;

	case discontigSingle:
	    /** output old, stay in single state **/
	    newPiece (_rtLINK_SingleSTATE, origin, count);
	    break;

	case contigRepeat:
	case discontigRepeat:
	    /** output old, goto repeat **/
	    newPiece (_rtLINK_RepeatSTATE, origin, count);
	    break;

	case overlapRange:
	    if (count == 2) {
		/** output a repeat of 2 and stay in singleton **/
		modifyPiece (_rtLINK_RepeatSTATE, oldOrigin, 2);
		newPieceCC (_rtLINK_SingleSTATE, origin+1, 1);
	    }
	    else {
		/** output a repeat of 2 and goto range with the rest **/
		modifyPiece (_rtLINK_RepeatSTATE, oldOrigin, 2);
		newPiece (_rtLINK_RangeSTATE, origin+1, count-1);
	    }
	    break;

	case discontigRange:
	    /** output old and goto range **/
	    newPiece (_rtLINK_RangeSTATE, origin, count);
	    break;

	default:
	    ERR_SignalFault (EC__InternalInconsistency, pBadInputTypeMessage);
	}
	break;

    case _rtLINK_RepeatSTATE:
	switch (input) {
	case overlapSingle:
	case overlapRepeat:
	    /** increment count and stay here **/
	    modifyPiece (_rtLINK_RepeatSTATE, oldOrigin, oldCount+count);
	    break;

	case contigSingle:
	case discontigSingle:
	    /** output repeat and goto single **/
	    newPiece (_rtLINK_SingleSTATE, origin, count);
	    break;

	case contigRepeat:
	case discontigRepeat:
	    /** output old repeat and start new **/
	    newPiece (_rtLINK_RepeatSTATE, origin, count);
	    break;

	case overlapRange:
	    if (count == 2) {
		/** output old+1 repeat goto single **/
		modifyPiece (_rtLINK_RepeatSTATE, oldOrigin, oldCount+1);
		newPieceCC (_rtLINK_SingleSTATE, origin+1, 1);
	    }
	    else {
		/** output old+1 repeat goto range-1 **/
		modifyPiece (_rtLINK_RepeatSTATE, oldOrigin, oldCount+1);
		newPiece (_rtLINK_RangeSTATE, origin+1, count-1);
	    }
	    break;

	case contigRange:
	case discontigRange:
	    /** output repeat and goto range **/
	    newPiece (_rtLINK_RangeSTATE, origin, count);
	    break;

	default:
	    ERR_SignalFault (EC__InternalInconsistency, pBadInputTypeMessage);
	}
	break;

    case _rtLINK_RangeSTATE:
	switch (input) {
	case overlapSingle:
	case overlapRepeat:
	    /** output range-1 goto repeat+1 **/
	    modifyPiece (_rtLINK_RangeSTATE, oldOrigin, oldCount-1);
	    newPiece (_rtLINK_RepeatSTATE, origin, count+1);
	    break;

	case contigSingle:
	case contigRange:
	    /** increment count and stay in range **/
	    modifyPiece (_rtLINK_RangeSTATE, oldOrigin, oldCount+count);
	    break;

	case discontigSingle:
	    /** output range and goto single **/
	    newPiece (_rtLINK_SingleSTATE, origin, count);
	    break;

	case contigRepeat:
	case discontigRepeat:
	    /** output range goto repeat **/
	    newPiece (_rtLINK_RepeatSTATE, origin, count);
	    break;

	case overlapRange:
	    if (count == 2) {
		/** output old-1 range, output size 2 repeat, goto single **/
		modifyPiece (_rtLINK_RangeSTATE, oldOrigin, oldCount-1);
		newPieceCC (_rtLINK_RepeatSTATE, origin, 2);
		newPieceCC (_rtLINK_SingleSTATE, origin+1, 1);
	    }
	    else {
		/** output old-1 range, output size 2 repeat, stay in range **/
		modifyPiece (_rtLINK_RangeSTATE, oldOrigin, oldCount-1);
		newPieceCC (_rtLINK_RepeatSTATE, origin, 2);
		newPiece (_rtLINK_RangeSTATE, origin+1, count-1);
	    }
	    break;

	case discontigRange:
	    /** output range stay in range **/
	    newPiece (_rtLINK_RangeSTATE, origin, count);
	    break;

	default:
	    ERR_SignalFault (EC__InternalInconsistency, pBadInputTypeMessage);
	}
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, "rtLINK_Append: state machine bad type"
	);
    }

/***** End of state machine *****/

    return this;
}


/*******************************
 *  Constructor Close Routine  *
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to close an open link constructor.
 *
 *  Arguments:
 *	pTokenRefCPD/Index	- a reference to a POP for a P-Token for the
 *				  missing (positional/referential) state of the
 *				  constructor.
 *
 *  Returns:
 *	'constructor'
 *
 *  Notes:
 *	This routine will confirm that the last reference / number of elements
 *	(as appropriate) in the open constructor does not exceed the base
 *	element count of the P-Token passed.  If that constraint is met, it
 *	will pad the RRDC chain with enough null references to insure equality
 *	between the base element count of the constructor's positional P-Token
 *	and the number of elements referenced by the RRDC chain.
 *
 *****/
rtLINK_CType *rtLINK_CType::Close (M_CPD *pTokenCPD) {
    CloseConstructorCount++;

    RTYPE_MustBeA (
	"rtLINK_CloseConstructor", M_CPD_RType (pTokenCPD), RTYPE_C_PToken
    );

    M_CPD **pTokenCPDDest;
    int referenceNil, fillCount;

    if (IsNil (m_pPPT)) {
/*****  Validate the constructor's positional state ...  *****/
	pTokenCPDDest = &m_pPPT;
	fillCount = rtPTOKEN_CPD_BaseElementCount (pTokenCPD) - ElementCount ();
	referenceNil = ReferenceNil ();
	if (fillCount < 0) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLINK_CloseConstructor: Positional State Inconsistent"
	);
    }
    else if (IsNil (m_pRPT)) {
/*****  ... or validate the constructor's referential state ...  *****/
	pTokenCPDDest = &m_pRPT;
	fillCount = BaseElementCount () - ElementCount ();
	referenceNil = rtPTOKEN_CPD_BaseElementCount (pTokenCPD);
	if (m_xReferenceLimit > referenceNil + 1) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLINK_CloseConstructor: Referential State Inconsistent"
	);
    }
/*****  ... or signal an error.  *****/
    else ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_CloseConstructor: Constructor Already Closed"
    );

/*****  Complete the constructor  *****/
/***** Fill the rest of the constructor with nils *****/
    Append (referenceNil, fillCount, true);

/***** Set the two 'has' bits for the last RRDC ... *****/
    if (m_pChainTail) {
	if (rtLINK_RRDC_RepeatedRef (m_pChainTail))
	    m_fHasRepeats = true;
	else
	    m_fHasRanges = true;
    }

    pTokenCPD->retain ();
    *pTokenCPDDest = pTokenCPD;

    if (TracingLCAllocator) IO_printf (
	"...rtLINK_CloseConstructor: LC:%08X closed.\n", this
    );

    return this;
}


/***********************************************
 *****  Link Constructor Deletion Routine  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to destroy a link constructor.
 *
 *  Argument:
 *	constructor		- the address of the link constructor to be
 *				  discarded.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	This routine decrements the constructor's reference count.  Iff the
 *	new value is 0 then the constructor is freed.
 *
 *****/
rtLINK_CType::~rtLINK_CType () {
    DiscardConstructorCount++;

    if (AnalyzingCompression)
	AnalyzeCompression ();

    if (MaxRRDCCount < m_iRRDCount)
	MaxRRDCCount = m_iRRDCount;

/*****  Free the P-Token CPDs  *****/
    if (m_pPPT)
	m_pPPT->release ();
    if (m_pRPT)
	m_pRPT->release ();

/*****  Free the RRD constructor chain  *****/
    if (IsNil (m_pChainHead)) {
	if (TracingRRDCAllocator) display (
	    "...rtLINK_DiscardConstructor: RRDC:%08X freed.\n", m_pFinalRRDC
	);
	RRDConstructorFreeList.deallocate (m_pFinalRRDC);
    }
    else while (m_pChainHead) {
	rtLINK_RRDCType *nextrrdc = rtLINK_RRDC_NextRRDC (m_pChainHead);

	if (TracingRRDCAllocator) display (
	    "...rtLINK_DiscardConstructor: RRDC:%08X freed.\n", m_pChainHead
	);

	RRDConstructorFreeList.deallocate (m_pChainHead);
	m_pChainHead = nextrrdc;
    }

/*****  Set the chain pointers to Nil - force a segv on misuse *****/
    m_pChainTail = m_pFinalRRDC = NilOf (rtLINK_RRDCType*);

/*****  Free the link constructor  *****/
    if (TracingLCAllocator)
        display ("...rtLINK_DiscardConstructor: LC:%08X\n", this);
}


/*******************************************************************
 *****  Private Misc Link -> LinkC and LinkC -> Link Routines  *****
 *******************************************************************/

/*---------------------------------------------------------------------------
 *****  This routine takes a link constructor and a link and updates the
 *****  link from the link constructor's RRDs.
 *
 *  Arguments:
 *	linkCPD		    - the address of a standard CPD for the link to
 *			      updated.
 *	linkC		    - the address of a link constructor containing the
 *		              new RRDs.
 *
 *  Returns:
 *	nothing
 *
 *****/
PrivateFnDef void ReconstructLink (
    M_CPD*			linkCPD,
    rtLINK_CType*		linkC
)
{
/***** Shift the container tail to deal with this size difference *****/
    linkCPD->EnableModifications ();
    linkCPD->containerHandle ()->ShiftContainerTail (
	(pointer_t)(rtLINK_CPD_RRDArray (linkCPD) + rtLINK_CPD_RRDCount (linkCPD) + 1),
	0, sizeof (rtLINK_RRDType) * (
	    (ptrdiff_t)rtLINK_LC_RRDCount (linkC) - rtLINK_CPD_RRDCount(linkCPD)
	), true
    );

/*****  Set the counts  *****/
    rtLINK_CPD_RRDCount		(linkCPD) = rtLINK_LC_RRDCount (linkC);
    rtLINK_CPD_ElementCount	(linkCPD) = linkC->ElementCount ();
    rtLINK_CPD_HasRepeats	(linkCPD) =
    rtLINK_CPD_HasRanges	(linkCPD) = false;

/*****  Copy the new RRD's into the RRD array...  *****/
    rtLINK_RRDCType *srcp = rtLINK_LC_ChainHead (linkC);
    if (IsntNil (srcp)) {
	rtLINK_RRDType *dest = rtLINK_CPD_RRDArray (linkCPD);

	for (
	    rtLINK_RRDCType *srcl = rtLINK_LC_FinalRRDC (linkC);
	    srcp != srcl;
	    srcp = rtLINK_RRDC_NextRRDC (srcp)
	)
	{
	    *dest++ = rtLINK_RRDC_RRD (srcp);
	    if (rtLINK_RRDC_RepeatedRef (srcp))
		rtLINK_CPD_HasRepeats (linkCPD) = true;
	    else
		rtLINK_CPD_HasRanges (linkCPD) = true;
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to create a constructor given a link.
 *
 *  Arguments:
 *	linkCPD			- a CPD for the link to be converted.
 *
 *  Returns:
 *	The address of the link constructor.
 *
 *****/
PrivateFnDef rtLINK_CType *LinkToConstructor (M_CPD *linkCPD) {
/*****  ...allocate the link constructor and final RRDC...  *****/
    rtLINK_CType *linkc = new rtLINK_CType ();
    rtLINK_RRDCType *finalRRDC = rtLINK_LC_FinalRRDC (linkc);

/*****  ...initialize traversal pointers and RRD count...  *****/
    rtLINK_RRDType *rrdp, *rrdl;
    rtLINK_RRDC_RRD (finalRRDC) = *(
	rrdl = (rtLINK_LC_RRDCount (linkc) = rtLINK_CPD_RRDCount (linkCPD))
	     + (rrdp = rtLINK_CPD_RRDArray (linkCPD))
    );

/*****  ...allocate the RRD chain...  *****/
    if (rrdp < rrdl) {
	rtLINK_RRDCType *chainHead, *chainTail;
	rtLINK_RRDCType *newRRDC = NewRRDC ();
        rtLINK_RRDC_NextRRDC (chainHead = newRRDC)= finalRRDC;
	while (rtLINK_RRDC_RRD (chainTail = newRRDC) = *rrdp++, rrdp < rrdl) {
	    newRRDC = NewRRDC ();
	    rtLINK_RRDC_NextRRDC (newRRDC) = finalRRDC;
	    rtLINK_RRDC_NextRRDC (chainTail) = newRRDC;
	}
	rtLINK_LC_ChainHead (linkc) = chainHead;
	rtLINK_LC_ChainTail (linkc) = chainTail;
    }

/*****  ...set the repeat-range bits ... *****/
    rtLINK_LC_HasRepeats (linkc) = rtLINK_CPD_HasRepeats (linkCPD);
    rtLINK_LC_HasRanges  (linkc) = rtLINK_CPD_HasRanges  (linkCPD);

/*****  ...allocate the CPD's...  *****/
    rtLINK_LC_Tracking (linkc) = rtLINK_CPD_Tracking (linkCPD);
    linkc->m_pPPT = rtLINK_CPD_PosPTokenCPD (linkCPD);
    linkc->m_pRPT = rtLINK_CPD_RefPTokenCPD (linkCPD);

    return linkc;
}


/************************************************
 *****  Link Positional Alignment Routines  *****
 ************************************************/

/********************
 *  Link Alignment  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link positionally AND referentially.
 *
 *  Argument:
 *	linkCPD			- a standard CPD for the link to be aligned.
 *
 *  Returns:
 *	'linkCPD'
 *
 *****/
PublicFnDef M_CPD *rtLINK_AlignLink (M_CPD *linkCPD) {
    M_CPD*			pTokenCPD;
    int				currentOff,
				currentValue,
				currentCnt;
    bool			currentRepeat;



/*****************************
 * Referential Align Defines *
 *****************************/

#define outputSkip(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-SKIP: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    posPtokenC->AppendAdjustment (newOff + (count), -(count));\
}

#define outputNew(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-NEW: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    posPtokenC->AppendAdjustment (newOff, count);\
    linkc->Append ((origin) + adjustment, count, repeat);\
    newOff += count;\
}

#define outputOld(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-OLD: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    linkc->Append ((origin) + adjustment, count, repeat);\
    newOff += count;\
}

#define nextRRD {\
    rrdp++;\
    /*** set global place markers ***/\
    if (rrdp < rrdpl) {\
	currentOff	= rtLINK_RRD_LinkCumulative (rrdp);\
	currentCnt	= rtLINK_RRD_LinkCumulative (rrdp+1) -\
			  rtLINK_RRD_LinkCumulative (rrdp);\
	currentValue	= rtLINK_RRD_ReferenceOrigin (rrdp);\
	currentRepeat	= rtLINK_RRD_RepeatedRef (rrdp);\
	}\
    else currentOff = -999;\
}

#define advance {\
    if (TracingLCAppending) IO_printf ("alignLink: in advance...\n");\
    /*** output whats left ***/\
    if (currentCnt > 0)\
	outputOld (currentValue, currentCnt, currentRepeat);\
    /*** goto next chunk ***/\
    nextRRD;\
}

#define notInCurrentChunk(value) (\
    currentRepeat\
    ? (value) > currentValue\
    : (value) > currentValue + currentCnt - 1\
)

#define notNil (currentOff != -999)

#define locate(value) {\
    if (TracingLCAppending) IO_printf ("alignLink: in Locate %d...\n", value);\
    while (notInCurrentChunk(value) && notNil)\
	advance;\
    if (notNil && (currentValue < (value)) && !currentRepeat) {\
	outputOld (currentValue, ((value) - currentValue), currentRepeat);\
	currentOff = currentOff + ((value) - currentValue);\
	currentCnt = currentCnt - ((value) - currentValue);\
	currentValue = (value);\
    }\
}

#define handleInsert(ptOrigin,ptShift) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: in handleinsert: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin - adjustment;\
    locate (ptorig);\
    if (notNil && currentValue <= ptorig) {\
	if (!currentRepeat) {\
	    if (tracking)\
		outputNew (ptorig, ptShift, false);\
	}\
    }\
    adjustment += ptShift;\
    if (TracingLCAppending) IO_printf (\
	"alignLink: exit handleinsert: adjustment=%d\n", adjustment\
    );\
}

#define handleDelete(ptOrigin,ptShift) {\
/* note: ptShift is always negative */\
    if (TracingLCAppending) IO_printf (\
	"alignLink: in handledelete: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin + ptShift - adjustment;\
    enddel = ptOrigin - 1 - adjustment;\
    x = ptorig;\
\
    locate (x);\
    while (notNil && currentValue <= enddel && x <= enddel) {\
	if (currentValue > x)\
	    x = currentValue;\
	if (currentRepeat) {\
	    outputSkip (currentValue, currentCnt, true);\
	    x++;\
	    nextRRD;\
	}\
	else {  /* range */\
	    if (enddel >= (currentValue + currentCnt -1)) {\
		/* skip the whole range */\
		outputSkip (currentValue, currentCnt, false);\
		x += currentCnt;\
		nextRRD;\
	    }\
	    else {  /* skip part of the range */\
		skipAmt = (enddel - currentValue + 1);\
		outputSkip (currentValue, skipAmt, false);\
		x += skipAmt;\
		currentValue = currentValue + skipAmt;\
		currentCnt   = currentCnt   - skipAmt;\
	    }\
	}\
    }  /* end of while */\
    adjustment += ptShift;\
    if (TracingLCAppending) IO_printf (\
	"alignLink: exit handledelete: adjustment=%d\n", adjustment\
    );\
}


/**************************************
 * Defines to Positionally Align Link *
 **************************************/

#define posNotInCurrentChunk(off) ((off) > (currentOff + currentCnt - 1))

#define posLocate(off) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: in PosLocate %d, val=%d, cnt=%d, Off=%d, repeat=%d...\n",\
	off, currentValue, currentCnt, currentOff, currentRepeat\
    );\
    while (posNotInCurrentChunk(off) && notNil)\
	advance;\
    if (notNil && (currentOff < (off))) {\
	outputOld (currentValue, ((off) - currentOff), currentRepeat);\
	if (!currentRepeat)\
	    currentValue = currentValue + ((off) - currentOff);\
	currentCnt = currentCnt - ((off) - currentOff);\
	currentOff = (off);\
    }\
    if (TracingLCAppending) IO_printf (\
	"alignLink: x PosLocate: val=%d, cnt=%d, Off=%d, repeat=%d\n",\
	currentValue, currentCnt, currentOff, currentRepeat\
    );\
}

#define posHandleInsert(ptOrigin,ptShift) {\
    ERR_SignalFault (\
	EC__InternalInconsistency,\
	"rtLINK_AlignLink: The link's positional ptoken has insertions."\
    );\
}

#define posHandleDelete(ptOrigin,ptShift) {\
/* note: ptShift is always negative */\
    if (TracingLCAppending) IO_printf (\
	"alignLink: PosHandleDelete: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin + ptShift;\
    enddel = ptOrigin - 1;\
    x = ptorig;\
\
    posLocate (x);\
    while (notNil && currentOff <= enddel && x <= enddel) {\
	if (TracingLCAppending) IO_printf (\
"alignLink: PosHandleDelete: enddel=%d, x=%d, val=%d, cnt=%d, Off=%d, repeat=%d\n",\
	    enddel, x, currentValue, currentCnt, currentOff, currentRepeat\
	);\
	if (currentOff > x)\
	    x = currentOff;\
        if (enddel >= (currentOff + currentCnt -1)) {\
	    /* skip the whole rrd */\
	    x += currentCnt;\
	    nextRRD;\
	}\
	else {  /* skip part of the rrd */\
	    skipAmt = (enddel - currentOff + 1);\
	    x += skipAmt;\
	    currentOff = currentOff + skipAmt;\
	    if (!currentRepeat)\
		currentValue = currentValue + skipAmt;\
	    currentCnt   = currentCnt   - skipAmt;\
	}\
    }  /* end of while */\
}


/************************************
 * Main  rtLINK_AlignLink Cody Body *
 ************************************/

    AlignLinkCount++;
    RTYPE_MustBeA ("rtLINK_AlignLink", M_CPD_RType (linkCPD), RTYPE_C_Link);
    linkCPD->CheckConsistency ();

/*****  If Link's PosPToken isn't current, Align positionally  *****/

    rtPTOKEN_IsntCurrent (linkCPD, rtLINK_CPx_PosPToken, pTokenCPD);
    if (IsntNil (pTokenCPD)) {
/*****  Obtain the collapsed ptoken constructor  *****/
        rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
	pTokenCPD->release ();
	linkCPD->EnableModifications (); {
	    int ptorig, enddel, adjustment, newOff, x, skipAmt;

	    if (TracingLCAppending) IO_printf (
		"\n--- in AlignLink Positionally ---\n"
	    );

	/*****  Initialize the target traversal state variables...  *****/
	    newOff = adjustment = 0;
	    rtLINK_RRDType *rrdp = rtLINK_CPD_RRDArray (linkCPD);
	    rtLINK_RRDType *rrdpl = (rtLINK_CPD_RRDCount (linkCPD) + rrdp);

	    if (rrdp < rrdpl) {
		currentOff	= rtLINK_RRD_LinkCumulative (rrdp);
		currentCnt	= rtLINK_RRD_LinkCumulative (rrdp+1)
				- rtLINK_RRD_LinkCumulative (rrdp);
		currentValue	= rtLINK_RRD_ReferenceOrigin (rrdp);
		currentRepeat	= rtLINK_RRD_RepeatedRef (rrdp);
		}
	    else currentOff = -999;

	/*****  Create the temporary result link constructor...  *****/
	    rtLINK_CType *linkc = rtLINK_RefConstructor (linkCPD, rtLINK_CPx_RefPToken);

	/***** Traverse the Link... *****/
	    rtPTOKEN_FTraverseInstructions (
		ptConstructor, posHandleInsert, posHandleDelete
	    );
	    while (notNil)
		advance;

	/***** Reconstruct linkCPD from the new linkc...  *****/
	    linkc->Close (ptConstructor->NextGeneration ());

	    rtLINK_CPD_IsInconsistent (linkCPD) = true;
	    ReconstructLink (linkCPD, linkc);
	    linkc->release ();

	/***** Set the link's positional ptoken to be the new one... *****/
	    linkCPD->StoreReference (
		rtLINK_CPx_PosPToken, ptConstructor->NextGeneration ()
	    );
	    rtLINK_CPD_IsInconsistent (linkCPD) = false;
	}
	ptConstructor->discard ();
    }


/*****  If Link's RefPToken isn't current, Align referentially  *****/
    rtPTOKEN_IsntCurrent (linkCPD, rtLINK_CPx_RefPToken, pTokenCPD);
    if (IsntNil (pTokenCPD)) {
/*****  Obtain the collapsed ptoken constructor  *****/
        rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
	pTokenCPD->release ();
	linkCPD->EnableModifications ();

	/***** Special case empty links ... *****/
	if (rtPTOKEN_BaseElementCount (linkCPD, rtLINK_CPx_PosPToken) == 0 ||
	    rtPTOKEN_PTC_AltCount (ptConstructor) == 0
	) linkCPD->StoreReference (	/** Set the ptoken... **/
	    rtLINK_CPx_RefPToken, ptConstructor->NextGeneration ()
	);
	else {
	/***** If the RefPToken has only been changed at the end ... *****/
	    unsigned int baseCount = rtPTOKEN_BaseElementCount (
		linkCPD, rtLINK_CPx_RefPToken
	    );

	    if (baseCount == (unsigned int)rtPTOKEN_SDC_CurrentOrigin (
		    rtPTOKEN_PTC_ChainTail (ptConstructor)
		) && 0 <= rtPTOKEN_SDC_Shift (rtPTOKEN_PTC_ChainTail (ptConstructor))
	    ) {
	    /*****  Align End Only  *****/
	    /********************************************************************/
	    /** The link may have a referenceOrigin in its last RRD with the   **/
	    /** referenceNil value.  This referenceNil value has changed so    **/
	    /** it needs to be updated.                                        **/
	    /** The last rrd could be a repeat of the reference Nil value or   **/
	    /** it could be a range the last value being referenceNil.	   **/
	    /********************************************************************/
		if (TracingLCAppending) IO_printf (
		    "\n--- in AlignEndOnly Referentially ---\n"
		);

		unsigned int newBaseCount = rtPTOKEN_PTC_BaseCount (ptConstructor);
		rtLINK_RRDType* rrdp = rtLINK_CPD_RRDArray (
		    linkCPD
		) + rtLINK_CPD_RRDCount (linkCPD) - 1;
		unsigned int count = rtLINK_RRD_LinkCumulative (
		    rrdp + 1
		) - rtLINK_RRD_LinkCumulative (rrdp);

		/*** easy case first: either a repeat or a range of one ***/
		if (rtLINK_RRD_RepeatedRef (rrdp) || count == 1) {
		    rtLINK_CPD_IsInconsistent (linkCPD)   = true;
		    if (rtLINK_RRD_ReferenceOrigin (rrdp) == baseCount)
			rtLINK_RRD_ReferenceOrigin (rrdp) = newBaseCount;
		}

		/*** a referenceNil value is at the end of a range rrd ***/
		else if (rtLINK_RRD_ReferenceOrigin (rrdp) + count == baseCount + 1) {
		    /* decrement the range count, and make a new rrd */

		    /**  Create the temporary target link constructor...  **/
		    rtLINK_CType *linkc = LinkToConstructor (linkCPD);

		    /* Make it look open and set the new referenceNil value */
		    M_CPD *refPTokenCPD = linkc->RPT ();
		    linkc->m_pRPT = NilOf (M_CPD*);

		    /** decrement the count of the last rrd **/
		    rtLINK_RRDC_LinkCumulative (rtLINK_LC_FinalRRDC (linkc))--;

		    /** append a: range, count=1, value=newBaseCount **/
		    linkc->Append (newBaseCount, 1, false);

		    rtLINK_CPD_IsInconsistent (linkCPD) = true;
		    ReconstructLink (linkCPD, linkc);
		    linkc->m_pRPT = refPTokenCPD;
		    linkc->release ();
		}

		/** Set the ptoken... **/
		linkCPD->StoreReference (
		    rtLINK_CPx_RefPToken, ptConstructor->NextGeneration ()
		);

		rtLINK_CPD_IsInconsistent (linkCPD)   = false;
	    }
	    else {
	    /*****  Align Full  *****/
		int ptorig, enddel, adjustment, newOff, x, skipAmt;

		if (TracingLCAppending) IO_printf (
		    "\n--- in AlignFull Referentially ---\n"
		);

	    /*****  Initialize the target traversal state variables...  *****/
		newOff = adjustment = 0;
		bool tracking = rtLINK_CPD_Tracking (linkCPD);
		rtLINK_RRDType *rrdp = rtLINK_CPD_RRDArray (linkCPD);
		rtLINK_RRDType *rrdpl = (rtLINK_CPD_RRDCount (linkCPD) + rrdp);

		if (rrdp < rrdpl) {
		    currentOff	= rtLINK_RRD_LinkCumulative (rrdp);
		    currentCnt	= rtLINK_RRD_LinkCumulative (rrdp+1)
				    - rtLINK_RRD_LinkCumulative (rrdp);
		    currentValue	= rtLINK_RRD_ReferenceOrigin (rrdp);
		    currentRepeat	= rtLINK_RRD_RepeatedRef (rrdp);
		}
		else currentOff = -999;

	    /*****  Create a positional ptoken constructor for the link... *****/
		rtPTOKEN_CType *posPtokenC = rtPTOKEN_NewShiftPTConstructor (
		    linkCPD, rtLINK_CPx_PosPToken
		);

	    /*****  Create the temporary result positional link constructor...  *****/
	    /*****  (Make sure it has the new referential element count.) *****/
		rtLINK_CType *linkc = rtLINK_RefConstructor (
		    ptConstructor->NextGeneration (), -1
		);

	    /***** Traverse the Link... *****/
		rtPTOKEN_FTraverseAdjustments (ptConstructor, handleInsert, handleDelete);

		while (notNil)
		    advance;

	    /*****  Close the ptoken for the target... *****/
		M_CPD *newPosPtokenCPD = posPtokenC->ToPToken ();

	    /***** Reconstruct linkCPD from the new linkc...  *****/
		linkc->Close (newPosPtokenCPD);

		rtLINK_CPD_IsInconsistent (linkCPD) = true;
		ReconstructLink (linkCPD, linkc);
		linkc->release ();

		/* set the link's positional ptoken to the new one... */
		linkCPD->StoreReference (rtLINK_CPx_PosPToken, newPosPtokenCPD);
		newPosPtokenCPD->release ();

	    /***** Set the link's referential ptoken to be the new base ptoken... *****/
		linkCPD->StoreReference (
		    rtLINK_CPx_RefPToken, ptConstructor->NextGeneration ()
		);
		rtLINK_CPD_IsInconsistent (linkCPD) = false;
	    }
	}
	ptConstructor->discard ();
    }

    return linkCPD;

/***** Remove all the defines used by rtLINK_AlignLink *****/
#undef outputSkip
#undef outputNew
#undef outputOld
#undef nextRRD
#undef advance
#undef notInCurrentChunk
#undef notNil
#undef locate
#undef handleInsert
#undef handleDelete
#undef posNotInCurrentChunk
#undef posLocate
#undef posHandleInsert
#undef posHandleDelete
}


/****************************************************************
 *****  The following are the aligning routines where both  *****
 *****  arguments are Link Constructors.                    *****
 ****************************************************************/

/*---------------------------------------------------------------------------
 *****  This routine takes two link constructors and updates the linkc
 *****  destination from the linkc source.
 *
 *  Arguments:
 *	this		    - the address of a link constructor for the link to
 *			      be updated.
 *	linkcs		    - the address of a link constructor containing the
 *		              new RRDs.
 *
 *  Returns:
 *	nothing
 *
 *****/
void rtLINK_CType::ReconstructFrom (rtLINK_CType *pOther) {
    unsigned int	sourceRefCount	= pOther->m_iReferenceCount,
			destRefCount	= m_iReferenceCount;

/*****  Simply swap the structures (no copy constructors available or used)  *****/
    char buffer[sizeof (rtLINK_CType)];
    memcpy (buffer, pOther, sizeof (rtLINK_CType)); // rtLINK_CType buffer = *pOther;
    memcpy (pOther, this  , sizeof (rtLINK_CType)); // *pOther = *this;
    memcpy (this  , buffer, sizeof (rtLINK_CType)); // *this = buffer;

/*****  Retain original reference count  *****/
    pOther->m_iReferenceCount = sourceRefCount;
    m_iReferenceCount = destRefCount;
}


/********************************
 *  Link Constructor Alignment  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor positionally AND referentially.
 *
 *  Returns:
 *	'this'
 *
 *****/
rtLINK_CType *rtLINK_CType::Align () {
    int currentOff, currentValue, currentCnt;
    bool currentRepeat;

#define outputSkip(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-SKIP: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    posPtokenC->AppendAdjustment (newOff + (count), -(count));\
}

#define outputNew(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-NEW: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    posPtokenC->AppendAdjustment (newOff, count);\
    linkc->Append ((origin) + adjustment, count, repeat);\
    newOff += count;\
}

#define outputOld(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: OUTPUT-OLD: org=%d, cnt=%d, rep=%d, newoff=%d\n",\
	origin, count, repeat, newOff\
    );\
    linkc->Append ((origin) + adjustment, count, repeat);\
    newOff += count;\
}

#define nextRRD {\
    currentRRDC = rtLINK_RRDC_NextRRDC (currentRRDC);\
    /*** set global place markers ***/\
    if (IsntNil (rtLINK_RRDC_NextRRDC (currentRRDC))) {\
	currentOff = rtLINK_RRDC_LinkCumulative (currentRRDC);\
	currentCnt = rtLINK_RRDC_N (currentRRDC);\
	currentValue = rtLINK_RRDC_ReferenceOrigin (currentRRDC);\
	currentRepeat = rtLINK_RRDC_RepeatedRef (currentRRDC);\
    }\
    else currentOff = -999;\
}

#define advance {\
    if (TracingLCAppending)\
	IO_printf ("alignLink: in advance...\n");\
    /*** output whats left ***/\
    if (currentCnt > 0)\
	outputOld (currentValue, currentCnt, currentRepeat);\
    /*** goto next chunk ***/\
    nextRRD;\
}

#define notInCurrentChunk(value) (\
    currentRepeat\
    ? (value) > currentValue\
    : (value) > currentValue + currentCnt - 1\
)

#define notNil (currentOff != -999)

#define locate(value) {\
    if (TracingLCAppending) IO_printf ("alignLink: in Locate %d...\n", value);\
    while (notInCurrentChunk(value) && notNil)\
	advance;\
    if (notNil && (currentValue < (value)) && !currentRepeat) {\
	outputOld (currentValue, ((value) - currentValue), currentRepeat);\
	currentOff = currentOff + ((value) - currentValue);\
	currentCnt = currentCnt - ((value) - currentValue);\
	currentValue = (value);\
    }\
}

#define handleInsert(ptOrigin,ptShift) {\
    if (TracingLCAppending) IO_printf (\
	"alignLink: in handleinsert: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin - adjustment;\
    locate (ptorig);\
    if (notNil && currentValue <= ptorig) {\
	if (!currentRepeat) {\
	    if (tracking) outputNew (ptorig, ptShift, false);\
	}\
    }\
    adjustment += ptShift;\
    if (TracingLCAppending) IO_printf (\
	"alignLink: exit handleinsert: adjustment=%d\n", adjustment\
    );\
}

#define handleDelete(ptOrigin,ptShift) {\
/* note: ptShift is always negative */\
    if (TracingLCAppending) IO_printf (\
	"alignLink: in handledelete: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin + ptShift - adjustment;\
    enddel = ptOrigin - 1 - adjustment;\
    x = ptorig;\
\
    locate (x);\
    while (notNil && currentValue <= enddel && x <= enddel) {\
	if (currentValue > x)\
	    x = currentValue;\
	if (currentRepeat) {\
	    outputSkip (currentValue, currentCnt, true);\
	    x++;\
	    nextRRD;\
	}\
	else {  /* range */\
	    if (enddel >= (currentValue + currentCnt -1)) {\
		/* skip the whole range */\
		outputSkip (currentValue, currentCnt, false);\
		x += currentCnt;\
		nextRRD;\
	    }\
	    else {  /* skip part of the range */\
		skipAmt = (enddel - currentValue + 1);\
		outputSkip (currentValue, skipAmt, false);\
		x += skipAmt;\
		currentValue = currentValue + skipAmt;\
		currentCnt   = currentCnt   - skipAmt;\
	    }\
	}\
    }  /* end of while */\
    adjustment += ptShift;\
    if (TracingLCAppending) IO_printf (\
	"alignLink: exit handledelete: adjustment=%d\n", adjustment\
    );\
}

/**************************************
 * Defines to Positionally Align Link *
 **************************************/

#define posNotInCurrentChunk(off) ((off) > (currentOff + currentCnt - 1))

#define posLocate(off) {\
    if (TracingLCAppending) IO_printf("alignLink: in PosLocate %d...\n", off);\
    while (posNotInCurrentChunk(off) && notNil)\
	advance;\
    if (notNil && (currentOff < (off))) {\
	outputOld (currentValue, ((off) - currentOff), currentRepeat);\
	if (!currentRepeat)\
	    currentValue = currentValue + ((off) - currentOff);\
	currentCnt = currentCnt - ((off) - currentOff);\
	currentOff = (off);\
    }\
    if (TracingLCAppending) IO_printf (\
	"alignLink: x PosLocate: val=%d, cnt=%d, Off=%d, repeat=%d\n",\
	currentValue, currentCnt, currentOff, currentRepeat\
    );\
}

#define posHandleInsert(ptOrigin,ptShift) {\
    ERR_SignalFault (\
	EC__InternalInconsistency,\
	"rtLINK_AlignLink: The link's positional ptoken has insertions."\
    );\
}

#define posHandleDelete(ptOrigin,ptShift) {\
/* note: ptShift is always negative */\
    if (TracingLCAppending) IO_printf (\
	"alignLink: PosHandleDelete: ptorigin=%d, ptshift=%d\n",\
	ptOrigin, ptShift\
    );\
    ptorig = ptOrigin + ptShift;\
    enddel = ptOrigin - 1;\
    x = ptorig;\
\
    posLocate (x);\
    while (notNil && currentOff <= enddel && x <= enddel) {\
	if (currentOff > x)\
	    x = currentOff;\
        if (enddel >= (currentOff + currentCnt -1)) {\
	    /* skip the whole rrd */\
	    x += currentCnt;\
	    nextRRD;\
	}\
	else {  /* skip part of the rrd */\
	    skipAmt = (enddel - currentOff + 1);\
	    x += skipAmt;\
	    currentOff = currentOff + skipAmt;\
	    if (!currentRepeat)\
		currentValue = currentValue + skipAmt;\
	    currentCnt   = currentCnt   - skipAmt;\
	}\
    }  /* end of while */\
}


/*******************************************
 * Main  rtLINK_AlignConstructor Cody Body *
 *******************************************/

    AlignConstructorCount++;

/*****  If Link's PosPToken isn't current, Align positionally  *****/

    if (!rtPTOKEN_IsCurrent (m_pPPT, -1)) {
/*****  Obtain the collapsed ptoken constructor  *****/
        rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (m_pPPT); {
	    int ptorig, enddel, x, skipAmt;

	    if (TracingLCAppending) IO_printf (
		"\n--- in AlignLink Positionally ---\n"
	    );

	/*****  Initialize the target traversal state variables...  *****/
	    int newOff = 0;
	    int adjustment = 0;
	    rtLINK_RRDCType *currentRRDC = m_pChainHead;
	    if (IsntNil (currentRRDC) && IsntNil (rtLINK_RRDC_NextRRDC (currentRRDC))) {
		currentOff = rtLINK_RRDC_LinkCumulative (currentRRDC);
		currentCnt = rtLINK_RRDC_N (currentRRDC);
		currentValue = rtLINK_RRDC_ReferenceOrigin (currentRRDC);
		currentRepeat = rtLINK_RRDC_RepeatedRef (currentRRDC);
	    }
	    else currentOff = -999;

	/*****  Create the temporary result link constructor...  *****/
	    rtLINK_CType *linkc = rtLINK_RefConstructor (m_pRPT, -1);

	/***** Traverse the Link... *****/
	    rtPTOKEN_FTraverseInstructions (
		ptConstructor, posHandleInsert, posHandleDelete
	    );

	    while (notNil)
		advance;

	/***** Reconstruct constructor from the new linkc...  *****/
	    linkc->Close (ptConstructor->NextGeneration ());

	    ReconstructFrom (linkc);
	    linkc->release ();

	/*****  End of Positionally Align LC  *****/
	}
	ptConstructor->discard ();
    }

/*****  If Link's RefPToken isn't current, Align referentially  *****/

    if (!rtPTOKEN_IsCurrent (m_pRPT, -1)) {
/*****  Obtain the collapsed ptoken constructor  *****/
        rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (m_pRPT);

/***** Special case an empty link constructor ... *****/
	if (rtPTOKEN_CPD_BaseElementCount (m_pPPT) == 0 ||
	    rtPTOKEN_PTC_AltCount (ptConstructor) == 0
	) {
	    m_pRPT->release ();
	    m_pRPT = ptConstructor->NextGeneration ();
	    m_pRPT->retain ();
	    ptConstructor->discard ();
	    return this;
	}

    /*****	RefAlignLinkConstructorBody  *****/
    /***** If the RefPToken has only been changed at the end ... *****/
	unsigned int baseCount = rtPTOKEN_CPD_BaseElementCount (m_pRPT);

	if (baseCount == (unsigned int)rtPTOKEN_SDC_CurrentOrigin (
		rtPTOKEN_PTC_ChainTail (ptConstructor)
	    ) && 0 <= rtPTOKEN_SDC_Shift (rtPTOKEN_PTC_ChainTail (ptConstructor))
	) {
	/*****  Align End Only  *****/
	/********************************************************************/
	/** The link may have a referenceOrigin in its last RRD with the   **/
	/** referenceNil value.  This referenceNil value has changed so    **/
	/** it needs to be updated.                                        **/
	/** The last rrd could be a repeat of the reference Nil value or   **/
	/** it could be a range the last value being referenceNil.	   **/
	/********************************************************************/
	    if (TracingLCAppending) IO_printf (
		"\n--- in AlignEndOnly Referentially ---\n"
	    );

	    unsigned int newBaseCount = rtPTOKEN_PTC_BaseCount (ptConstructor);
	    rtLINK_RRDCType *currentRRDC = m_pChainTail;
	    unsigned int count = rtLINK_RRDC_N (currentRRDC);

	    /*** easy case first: either a repeat or a range of one ***/
	    if (rtLINK_RRDC_RepeatedRef (currentRRDC) || count == 1) {
		if (rtLINK_RRDC_ReferenceOrigin (currentRRDC) == baseCount)
		    rtLINK_RRDC_ReferenceOrigin (currentRRDC) = newBaseCount;
		/** Set the ptoken... **/
		m_pRPT->release ();
		m_pRPT = ptConstructor->NextGeneration ();
		m_pRPT->retain ();
	    }

	    /*** a referenceNil value is at the end of a range rrd ***/
	    else if (
		rtLINK_RRDC_ReferenceOrigin (currentRRDC) + count - 1 == baseCount
	    ) {
		/*****  ...initialize traversal pointer...  *****/
		rtLINK_CType *linkc = new rtLINK_CType ();
		rtLINK_RRDCType *rrdcs = m_pChainHead;
		rtLINK_RRDCType *finalRRDC = rtLINK_LC_FinalRRDC (linkc);
		rtLINK_RRDC_RRD (finalRRDC) = rtLINK_RRDC_RRD (m_pFinalRRDC);

		/*****  ...allocate the RRD chain...  *****/
		if (IsntNil (rrdcs)) {
		    rtLINK_RRDCType *chainHead, *chainTail;
		    rtLINK_RRDCType *newRRDC = NewRRDC ();
		    rtLINK_RRDC_NextRRDC (chainHead = newRRDC) = finalRRDC;
		    rtLINK_RRDC_RRD (chainTail = newRRDC) =
			rtLINK_RRDC_RRD (rrdcs);
		    rrdcs = rtLINK_RRDC_NextRRDC (rrdcs);
		    while (IsntNil (rtLINK_RRDC_NextRRDC (rrdcs))) {
			newRRDC = NewRRDC ();
			rtLINK_RRDC_NextRRDC (chainTail) = newRRDC;
			rtLINK_RRDC_RRD (chainTail = newRRDC) =
			    rtLINK_RRDC_RRD (rrdcs);
			rtLINK_RRDC_NextRRDC (newRRDC) = finalRRDC;
			rrdcs = rtLINK_RRDC_NextRRDC (rrdcs);
		    }
		    rtLINK_LC_ChainHead (linkc) = chainHead;
		    rtLINK_LC_ChainTail (linkc) = chainTail;
		}

		rtLINK_LC_Tracking (linkc) = m_fTracking;
		rtLINK_LC_RRDCount (linkc) = m_iRRDCount;
		/*****  ...allocate the CPD's...  *****/
		m_pPPT->retain ();
		m_pRPT->retain ();
		linkc->m_pPPT = m_pPPT;
		linkc->m_pRPT = m_pRPT;

		/* Make it look open and set the new referenceNil value */
		M_CPD *refPTokenCPD = linkc->RPT ();
		linkc->m_pRPT = NilOf (M_CPD*);

		/** decrement the count of the last rrd **/
		rtLINK_RRDC_LinkCumulative (rtLINK_LC_FinalRRDC (linkc))--;

		/** append a: range, count=1, value=newBaseCount **/
		linkc->Append (newBaseCount, 1, false);

		ReconstructFrom (linkc);
		linkc->m_pRPT = refPTokenCPD;
		linkc->release ();
	    }
	    else {
	/** no reference nil in this range, so just set the ptoken... **/
		m_pRPT->release ();
		m_pRPT = ptConstructor->NextGeneration ();
		m_pRPT->retain ();
	    }
	}
	else {
	/*****  Align Full  *****/
	    int ptorig, enddel, x, skipAmt;

	    if (TracingLCAppending) IO_printf (
		"\n--- in AlignFull Referentially ---\n"
	    );

	/*****  Initialize the target traversal state variables...  *****/
	    int newOff = 0;
	    int adjustment = 0;
	    bool tracking = m_fTracking;
	    rtLINK_RRDCType *currentRRDC = m_pChainHead;
	    if (IsntNil (currentRRDC) && IsntNil (rtLINK_RRDC_NextRRDC (currentRRDC))) {
		currentOff = rtLINK_RRDC_LinkCumulative (currentRRDC);
		currentCnt = rtLINK_RRDC_N (currentRRDC);
		currentValue = rtLINK_RRDC_ReferenceOrigin (currentRRDC);
		currentRepeat = rtLINK_RRDC_RepeatedRef (currentRRDC);
	    }
	    else currentOff = -999;

	/*****  Create a positional ptoken constructor for the link... *****/
	    rtPTOKEN_CType *posPtokenC = rtPTOKEN_NewShiftPTConstructor (m_pPPT, -1);

	/*****  Create the temporary result positional link constructor...  *****/
	/*****  (Make sure it has the new referential element count.) *****/
	    rtLINK_CType *linkc = rtLINK_RefConstructor (
		ptConstructor->NextGeneration (), -1
	    );

	/***** Traverse the Link... *****/
	    rtPTOKEN_FTraverseAdjustments (ptConstructor, handleInsert, handleDelete);
	    while (notNil)
		advance;

	/*****  Close the ptoken for the target... *****/
	    M_CPD *newPosPtokenCPD = posPtokenC->ToPToken ();

	/***** Reconstruct constructor from the new linkc...  *****/
	    linkc->Close (newPosPtokenCPD);
	    newPosPtokenCPD->release ();

	    ReconstructFrom (linkc);
	    linkc->release ();
	}

	ptConstructor->discard ();
    }

    return this;

/***** Remove all the defines used by rtLINK_AlignConstructor *****/
#undef outputSkip
#undef outputNew
#undef outputOld
#undef nextRRD
#undef advance
#undef notInCurrentChunk
#undef notNil
#undef locate
#undef handleInsert
#undef handleDelete
#undef posNotInCurrentChunk
#undef posLocate
#undef posHandleInsert
#undef posHandleDelete
}


/*******************************
 *****  Link New Routines  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new link with the specified reference ptoken, a new
 *****  positional ptoken of the same size, and initialized to 0 - n.
 *
 *  Argument:
 *	pPPT			- the optional (Nil if absent) PPT of the result.
 *				  If nil, RPT will be used.
 *	pRPT			- the RPT of the result.
 *
 *  Returns:
 *	The address of a standard CPD for the link created.
 *
 *****/
PublicFnDef M_CPD *rtLINK_NewRefLink (M_CPD *pPPT, M_CPD *pRPT) {
    unsigned int iCoCardinality = rtPTOKEN_CPD_BaseElementCount (pRPT);

    if (!pPPT)
	pPPT = pRPT;
    else if (rtPTOKEN_CPD_BaseElementCount (pPPT) != iCoCardinality) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "rtLINK_NewRefLink: Cardinality disagreement: (Dom:%u, Cod:%u).",
	    rtPTOKEN_CPD_BaseElementCount (pPPT), iCoCardinality
	)
    );

    M_CPD *result = pPPT->CreateContainer (
	RTYPE_C_Link,
	sizeof (rtLINK_Type) + sizeof (rtLINK_RRDType) * (iCoCardinality > 0 ? 1 : 0)
    );

    NewRefLinkCount++;

/***** Initialize the new cpd ... *****/
    InitStdCPD (result);
    rtLINK_CPD_IsInconsistent (result)   = true;

/*****  Initialize the POP's  *****/
    result->constructReference (rtLINK_CPx_PosPToken);
    result->constructReference (rtLINK_CPx_RefPToken);

/***** If cardinality is 0, then there is no extra RRD.
 *****    Set the last RRD's link cumulative to 0.
 ***** If cardinality is > 0 then set one RRD to a range from 0 to cardinality and
 *****    set the last RRD's link cumulative to cardinality.
 *****/
/***** set the RRD ... *****/
    rtLINK_RRDType *rrdp = rtLINK_CPD_RRDArray (result);
    if (iCoCardinality > 0) {
	rtLINK_RRD_LinkCumulative  (rrdp) = 0;
	rtLINK_RRD_ReferenceOrigin (rrdp) = 0;
	rtLINK_RRD_RepeatedRef     (rrdp) = false;

	rtLINK_CPD_RRDCount  (result) = 1;
	rtLINK_CPD_HasRanges (result) = true;
	rrdp++;
    }
    else {
	rtLINK_CPD_RRDCount  (result) = 0;
        rtLINK_CPD_HasRanges (result) = false;
    }

/***** Set the last RRD for the total link iCoCardinality (nil out the other fields)... *****/
    rtLINK_RRD_LinkCumulative	(rrdp) = iCoCardinality;
    rtLINK_RRD_ReferenceOrigin	(rrdp) = 0;
    rtLINK_RRD_RepeatedRef	(rrdp) = false;

/*****  Initialize the rest *****/
    rtLINK_CPD_HasRepeats	(result) = false;
    rtLINK_CPD_Tracking		(result) = false;

/***** Install the ptokens ... *****/
    result->StoreReference (
	rtLINK_CPx_PosPToken, pPPT
    )->StoreReference (
	rtLINK_CPx_RefPToken, pRPT
    );

    rtLINK_CPD_IsInconsistent (result)   = false;

    return result;
}


/*---------------------------------------------------------------------------
 *****  Routine to create an empty link with the ptokens specified.
 *
 *  Arguments:
 *	pPPT			- the positional ptoken for the new link.
 *	pRPT			- the referential ptoken for the new link.
 *
 *  Returns:
 *	The address of a standard CPD for the link created.
 *
 *****/
PublicFnDef M_CPD *rtLINK_NewEmptyLink (M_CPD *pPPT, M_CPD *pRPT) {
    BasicNewEmptyLinkCount++;

/***** Make sure the postional ptoken is empty *****/
    if (rtPTOKEN_CPD_BaseElementCount (pPPT) != 0) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_NewEmptyLink: Positional PToken Must Have Size 0"
    );

/***** Allocate the new link ... *****/
    M_CPD *result = pPPT->CreateContainer (RTYPE_C_Link, sizeof (rtLINK_Type));

/***** Initialize the new cpd ... *****/
    rtLINK_CPD_IsInconsistent (result)   = true;
    InitStdCPD (result);

/*****  Initialize the POP's  *****/
    result->constructReference (rtLINK_CPx_PosPToken);
    result->constructReference (rtLINK_CPx_RefPToken);

/***** Set the last RRD for the total link size (nil out the other fields)... *****/
    rtLINK_RRDType *rrdp = rtLINK_CPD_RRDArray (result);
    rtLINK_RRD_LinkCumulative (rrdp)	    = 0;
    rtLINK_RRD_ReferenceOrigin (rrdp)	    = 0;
    rtLINK_RRD_RepeatedRef (rrdp)	    = false;

/*****  Initialize the counts  *****/
    rtLINK_CPD_RRDCount		(result) = 0;
    rtLINK_CPD_ElementCount	(result) = 0;
    rtLINK_CPD_HasRepeats	(result) = false;
    rtLINK_CPD_HasRanges	(result) = false;
    rtLINK_CPD_Tracking		(result) = false;

/***** Install the ptokens ... *****/
    result->StoreReference (
	rtLINK_CPx_PosPToken, pPPT
    )->StoreReference (
	rtLINK_CPx_RefPToken, pRPT
    );

    rtLINK_CPD_IsInconsistent (result)   = false;

    return result;
}


/*******************************
 *****  Link Copy Routine  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new copy of a link.
 *
 *  Argument:
 *	pSource	- a standard CPD for the source link.
 *	pNewPPT	- an optional (nil if absent) p-token specifying the domain
 *		  of the copy.  If specified, must match the cardinality of
 *		  the current domain.
 *
 *  Returns:
 *	The address of a standard CPD for the link created.
 *
 *****/
PublicFnDef M_CPD *rtLINK_Copy (M_CPD *pSource, M_CPD *pNewPPT) {
    CopyLinkCount++;

/***** Align the source link ... *****/
    rtLINK_AlignLink (pSource);

    unsigned int iDomCardinality = rtLINK_CPD_ElementCount (pSource);

    bool bFreeNewPPT = false;
    if (!pNewPPT) {
	pNewPPT = rtLINK_CPD_PosPTokenCPD (pSource);
	bFreeNewPPT = true;
    }
    else if (iDomCardinality != rtPTOKEN_CPD_BaseElementCount (pNewPPT)) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtLINK_Copy: Cardinality disagreement: Current:%u, Proposed:%u.",
	    iDomCardinality, rtPTOKEN_CPD_BaseElementCount (pNewPPT)
	)
    );

/***** Allocate the new container ... *****/
    M_CPD *pResult = pNewPPT->CreateContainer (
	RTYPE_C_Link,
	sizeof (rtLINK_Type) + sizeof (rtLINK_RRDType) * rtLINK_CPD_RRDCount (pSource)
    );

/***** Initialize the new cpd ... *****/
    rtLINK_CPD_IsInconsistent (pResult)   = true;
    InitStdCPD (pResult);

/*****  Initialize the POP's  *****/
    pResult->constructReference (rtLINK_CPx_PosPToken);
    pResult->constructReference (rtLINK_CPx_RefPToken);

/*****  Initialize the header *****/
    rtLINK_CPD_RRDCount		(pResult) = rtLINK_CPD_RRDCount (pSource);
    rtLINK_CPD_HasRepeats	(pResult) = rtLINK_CPD_HasRepeats (pSource);
    rtLINK_CPD_HasRanges	(pResult) = rtLINK_CPD_HasRanges (pSource);
    rtLINK_CPD_Tracking		(pResult) = rtLINK_CPD_Tracking (pSource);

/*****  Initialize the RRD array  *****/
    rtLINK_RRDType *src  = rtLINK_CPD_RRDArray (pSource);
    rtLINK_RRDType *dest = rtLINK_CPD_RRDArray (pResult);
    for (unsigned int i = 0; i <= rtLINK_CPD_RRDCount (pSource); i++)
	*dest++ = *src++;

/***** Install the ptokens ... *****/
    pResult->StoreReference (
	rtLINK_CPx_PosPToken, pNewPPT
    )->StoreReference (
	rtLINK_CPx_RefPToken, pSource, rtLINK_CPx_RefPToken
    );

    rtLINK_CPD_IsInconsistent (pResult) = false;

    if (bFreeNewPPT)
	pNewPPT->release ();

    return pResult;
}


/************************************************************
 *****  Link-Constructor <-> Link Conversion Routines  ******
 ************************************************************/

PublicFnDef void rtLINK_DumpLink (M_CPD *linkCPD) {
    IO_printf ("\n>> Link Dump <<\n");
    IO_printf (
	"   Link:%08X, RRDCount=%d, ElementCount=%d, Tracking=%d\n",
	linkCPD,
	rtLINK_CPD_RRDCount (linkCPD),
	rtLINK_CPD_ElementCount (linkCPD),
	rtLINK_CPD_Tracking (linkCPD)
    );
    IO_printf (
	"   HasRepeats:%d, HasRanges=%d\n",
	rtLINK_CPD_HasRepeats (linkCPD),
	rtLINK_CPD_HasRanges (linkCPD)
    );

    rtLINK_RRDType *rrdp = rtLINK_CPD_RRDArray (linkCPD);
    rtLINK_RRDType *rrdpl= rtLINK_CPD_RRDCount (linkCPD) + rrdp;
    while (rrdp < rrdpl) {
	IO_printf (
	    "   RRD:%08X, NextRRD:%08X, %d:%d%c%d\n",
	    rrdp,
	    rrdp+1,
	    rtLINK_RRD_LinkCumulative (rrdp),
	    rtLINK_RRD_ReferenceOrigin (rrdp),
	    rtLINK_RRD_RepeatedRef (rrdp) ? '*' : '+',
	    rtLINK_RRD_LinkCumulative (rrdp+1) -
	    rtLINK_RRD_LinkCumulative (rrdp)
	);
	rrdp++;
    }
}


/*****************************************
 *  Link-Constructor To Link Conversion  *
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Link-Constructor To Link Conversion Routine  *****
 *
 *  Argument:
 *	this			- the address of the closed link constructor
 *				  to be converted.
 *	fDiscard		- if true, the constructor will be
 *				  released after conversion.
 *
 *  Returns:
 *	The address of a standard CPD for the link created.
 *
 *****/
M_CPD *rtLINK_CType::ToLink (M_CPD *pNewPPT, bool fDiscard) {
    if (m_iRRDCount >= (unsigned int)(1 << 28)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Requested RRD count %d exceeds maximum %d", m_iRRDCount, (1 << 28) - 1
	)
    );

    BasicToLinkCount++;

    unsigned int iDomCardinality = ElementCount ();

    if (!pNewPPT)
	pNewPPT = m_pPPT;
    else if (iDomCardinality != rtPTOKEN_CPD_BaseElementCount (pNewPPT)) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtLINK_CType::ToLink: Cardinality disagreement: Current:%u, Proposed:%u.",
	    iDomCardinality, rtPTOKEN_CPD_BaseElementCount (pNewPPT)
	)
    );

    M_CPD *result = pNewPPT->CreateContainer (
	RTYPE_C_Link, sizeof (rtLINK_Type) + sizeof (rtLINK_RRDType) * m_iRRDCount
    );

    rtLINK_CPD_IsInconsistent (result)   = true;
    InitStdCPD (result);

/*****  Initialize the POP's  *****/
    result->constructReference (rtLINK_CPx_PosPToken);
    result->constructReference (rtLINK_CPx_RefPToken);
    result->StoreReference (rtLINK_CPx_PosPToken, pNewPPT)
	  ->StoreReference (rtLINK_CPx_RefPToken, m_pRPT);


/*****  Initialize the counts  *****/
    rtLINK_CPD_RRDCount		(result) = m_iRRDCount;
    rtLINK_CPD_ElementCount	(result) = iDomCardinality;
    rtLINK_CPD_HasRepeats	(result) = m_fHasRepeats;
    rtLINK_CPD_HasRanges	(result) = m_fHasRanges;
    rtLINK_CPD_Tracking		(result) = m_fTracking;

/*****  Initialize the RRD array  *****/
    if (m_pChainHead) {
	rtLINK_RRDCType *srcp = m_pChainHead;
	rtLINK_RRDType  *dest = rtLINK_CPD_RRDArray (result);
	while (srcp != m_pFinalRRDC) {
	    *dest++ = rtLINK_RRDC_RRD (srcp);
	    srcp = rtLINK_RRDC_NextRRDC (srcp);
	}
    }

    rtLINK_CPD_IsInconsistent (result)   = false;

    if (fDiscard)
	release ();

    return result;
}


/*****************************************
 *  Link to Link-Constructor Conversion  *
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a link into a link constructor.
 *
 *  Argument:
 *	linkCPD			- the address of a standard CPD for the link
 *				  to be converted.
 *
 *  Returns:
 *	The address of a link constructor.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_ToConstructor (M_CPD *linkCPD) {
    ToConstructorCount++;

/*****  Align the link...  *****/
    rtLINK_AlignLink (linkCPD);

/***** Make the constructor *****/
    return LinkToConstructor (linkCPD);
}


/*****************************************************************
 *****  Positional Alignment And Access Validation Routines  *****
 *****************************************************************/

/***********************
 *  Coerce Validation  *
 ***********************/

void rtLINK_CType::AlignForCoerce () {
    Align ();

/*****  ... check that it is referentially related to the source.  *****/
    if (m_pRPT->DoesntNameTheScalarPToken ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_CType::AlignForCoerce: Non-Scalar Co-domain."
    );
}


/************************
 *  Extract Validation  *
 ************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor and validate it for use as an
 *****  extraction index.
 *
 *  Arguments:
 *	sPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object from which elements are to be
 *				  extracted.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the link constructor and source are not
 *	referentially related.
 *
 *****/
void rtLINK_CType::AlignForExtract (M_CPD *sPTokenRefCPD, int sPTokenRefIndex) {
/*****  Align the link constructor...  *****/
    Align ();

/*****  ... check that it is referentially related to the source.  *****/
    if (sPTokenRefCPD->ReferenceDoesntName (sPTokenRefIndex, m_pRPT)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_CType::AlignForExtract: Referential Inconsistency.\n\
The ref ptoken of the link is different from the pos ptoken of the source."
    );
}


/***************************
 *  Distribute Validation  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor and validate it for use as an
 *****  distribution index.
 *
 *  Arguments:
 *	this			- the address of the link constructor
 *				  supplying subscripts to the distribution
 *				  operation.
 *	sPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object from which elements are to be
 *				  distributed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the link constructor and source are not
 *	referentially related.
 *
 *****/
void rtLINK_CType::AlignForDistribute (M_CPD *sPTokenRefCPD, int sPTokenRefIndex) {
/*****  Align the link constructor...  *****/
    Align ();

/*****  ... check that it is referentially related to the source.  *****/
    if (sPTokenRefCPD->ReferenceDoesntName (sPTokenRefIndex, m_pPPT)) {
	M_CPD* pSourceCPD = sPTokenRefCPD->GetCPD (sPTokenRefIndex);
	unsigned int space = pSourceCPD->SpaceIndex ();
	unsigned int cntnr = pSourceCPD->ContainerIndex ();
	pSourceCPD->release ();
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtLINK_AlignConstructorForDistribute: Referential Inconsistency.\
\nThe pos ptoken of the link[%d:%d] is different from the pos ptoken of the source[%d:%d].",
		m_pPPT->SpaceIndex (), m_pPPT->ContainerIndex (), space, cntnr
		));
    }
}


/***********************
 *  Assign Validation  *
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor and validate it for use as an
 *****  assignment index.
 *
 *  Arguments:
 *	tPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object into which elements are to be
 *				  assigned.
 *	this			- the address of the link constructor
 *				  supplying subscripts to the assignment
 *				  operation.
 *	sPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object supplying the elements to be
 *				  assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the link constructor and source are not
 *	positionally related or if the link constructor and target are not
 *	referentially related.
 *
 *****/
void rtLINK_CType::AlignForAssign (
    M_CPD *tPTokenRefCPD, int tPTokenRefIndex, M_CPD *sPTokenRefCPD, int sPTokenRefIndex
) {
/*****  Align the link constructor...  *****/
    Align ();

/*****  ... check that it is referentially related to the target...  *****/
    if (tPTokenRefCPD->ReferenceDoesntName (tPTokenRefIndex, m_pRPT)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_AlignConstructorForAssign: Referential Inconsistency.\n\
The ref ptoken of the link is different from the pos ptoken of the target."
    );

/*****  ... and that it is positionally related to the source.  *****/
    if (sPTokenRefCPD->ReferenceDoesntName (sPTokenRefIndex, m_pPPT)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_AlignConstructorForAssign: Referential Inconsistency.\n\
The pos ptoken of the link is different from the pos ptoken of the source."
    );
}


/******************************
 *  Scalar Assign Validation  *
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor and validate it for use as an
 *****  scalar assignment index.
 *
 *  Arguments:
 *	tPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object into which elements are to be
 *				  assigned.
 *	this			- the address of the link constructor
 *				  supplying subscripts to the assignment
 *				  operation.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the link constructor and target are not
 *	referentially related.
 *
 *****/
void rtLINK_CType::AlignForAssign (M_CPD *tPTokenRefCPD, int tPTokenRefIndex) {
/*****  Align the link constructor...  *****/
    Align ();

/*****  ... and check that it is referentially related to the target.  *****/
    if (tPTokenRefCPD->ReferenceDoesntName (tPTokenRefIndex, m_pRPT)) ERR_SignalFault (
	EC__InternalInconsistency,
        "rtLINK_AlignConstructorForScalarAssign: Referential Inconsistency.\n\
The ref ptoken of the link is different from the pos ptoken of the target."
    );
}


/********************
 *  Add Validation  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a link constructor and validate it for use as an
 *****  add source link.
 *
 *  Arguments:
 *	tPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the referential state of the
 *				  object into which elements are to be
 *				  added.
 *	this			- the address of the link constructor
 *				  supplying the slots to add.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the link constructor and target are not
 *	referentially related.
 *
 *****/
void rtLINK_CType::AlignForAdd (M_CPD *tPTokenRefCPD, int tPTokenRefIndex) {
/*****  Align the link constructor...  *****/
    Align ();

/*****  ... and check that it is referentially related to the target.  *****/
    if (tPTokenRefCPD->ReferenceDoesntName (tPTokenRefIndex, m_pRPT)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_AlignConstructorForAdd: Referential Inconsistency.\n\
The ref ptoken of the link is different from the ref ptoken of the target."
    );
}


/*************************************************************************
 *****  Link Constructor <-> Reference U-Vector Conversion Routines  *****
 *************************************************************************/

/********************************************
 *  Link Constructor -> Reference U-Vector  *
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Internal 'rtREFUV_New' routine to initialize a RefUV from a
 *****  link constructor.
 *
 *  Arguments:
 *	refuvCPD		- an initialized standard CPD for the new
 *				  RefUV.
 *	nelements		- the number of elements in the RefUV.
 *	linkcAP			- a <varargs.h> argument pointer referencing
 *				  the link constructor from which the RefUV
 *				  is to be initialized.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeRefUVFromConstructor (
    M_CPD*			refuvCPD,
    size_t			Unused(nelements),
    va_list			linkcAP
) {
    V::VArgList iArgList (linkcAP);
    rtLINK_CType*	linkc = iArgList.arg<rtLINK_CType*>();
    M_CPD*		orderingUV = iArgList.arg<M_CPD*>();
    int			*refuvp = rtREFUV_CPD_Array (refuvCPD),
			*orderinguvp;

/****  Define the RefUV repeated and range reference fill macros...  ****/

#define FillRefUVFromRepeatedRef(c, n, r)\
    while (n-- > 0) *refuvp++ = r

#define FillRefUVFromRangeRef(c, n, r)\
    while (n-- > 0) *refuvp++ = r++

#define FillRefUVFromOrderedRepeatedRef(c, n, r)\
    while (n-- > 0) *(refuvp + (*orderinguvp++)) = r

#define FillRefUVFromOrderedRangeRef(c, n, r)\
    while (n-- > 0) *(refuvp + (*orderinguvp++)) = r++

    if (IsNil (orderingUV))
    {
	/*****  ...loop through the link constructor...  *****/
	rtLINK_TraverseRRDCList (
	    linkc,
	    FillRefUVFromRepeatedRef,
	    FillRefUVFromRepeatedRef,
	    FillRefUVFromRangeRef
	);
    }
    else
    {
	orderinguvp = rtREFUV_CPD_Array (orderingUV);

	rtLINK_TraverseRRDCList (
	    linkc,
	    FillRefUVFromOrderedRepeatedRef,
	    FillRefUVFromOrderedRepeatedRef,
	    FillRefUVFromOrderedRangeRef
	);
    }

/*****  ...and dispose of the macros.  *****/
#undef FillRefUVFromRepeatedRef
#undef FillRefUVFromRangeRef
#undef FillRefUVFromOrderedRepeatedRef
#undef FillRefUVFromOrderedRangeRef
}


/*****  External Routine  *****/

/*---------------------------------------------------------------------------
 *****  Routine to convert a link constructor into a Reference U-Vector.
 *
 *  Argument:
 *	this			- the address of the link constructor to
 *				  be converted into a reference u-vector.
 *	orderingUV		- an optional (Nil if not specified) CPD
 *				  for a reference u-vector which specifies
 *				  the order in which the elements of the
 *				  link constructor are to be assigned to
 *				  the reference u-vector created.
 *
 *  Returns:
 *	A standard CPD for the reference u-vector created.
 *
 *****/
M_CPD *rtLINK_CType::ToRefUV (M_CPD *orderingUV) {
    ConstructorToRefUVCount++;

/*****  Align the link constructor...  *****/
    Align ();

/*****	Align the orderingUV, if it exists...	*****/
    if (orderingUV) {
	rtREFUV_Align (orderingUV);

	/*****	Check for referential inconsistency	*****/
	if (orderingUV->ReferenceDoesntName (UV_CPx_PToken, m_pPPT)) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLINK_ConstructorToRefUV: Referential Inconsistency"
	);
    }

/*****  ...convert it into a reference u-vector...  *****/
    M_CPD *ptoken = orderingUV ? UV_CPD_RefPTokenCPD (orderingUV) : m_pPPT;
    M_CPD *result = rtREFUV_New (
	ptoken, m_pRPT, -1, InitializeRefUVFromConstructor, this, orderingUV
    );

    if (orderingUV)
	ptoken->release ();

    return result;
}


/********************************************
 *  Reference U-Vector -> Link Constructor  *
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a reference U-Vector to a link constructor.
 *
 *  Arguments:
 *	refuvCPD		- a standard CPD for the reference u-vector
 *				  to be converted into a link constructor.
 *	ppSortReturn		- the address of an l-val which will be set
 *				  to a pointer to a standard CPD for the sort
 *				  indices reference u-vector produced during
 *				  the conversion.  This argument is optional.
 *				  If no value or null is supplied as the value
 *				  of this argument, the sort indices will be
 *				  discarded (but not necessarily ignored, see
 *				  below).  When created, DOM(result-link) ==
 *				  DOM(sort) and COD(sort) == DOM(refuv).
 *				  the sort indices produced will be discarded.
 *	sortMustBeExpressed	- true if the sort u-vector must always be
 *				  created, even if it is not returned, false if
 *				  trivial sorts (cardinality < 2 or in order
 *				  references) can be detected and suppressed.
 *				  When trivial sorts are suppressed, DOM(refuv)
 *				  == Dom(result-link). This argument is
 *				  optional with a default value of false.
 *
 *  Returns:
 *	The address of the link constructor created.  In all cases, COD(refuv)
 *	== COD(result-link)
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_RefUVToConstructor (
    M_CPD *refuvCPD, M_CPD **ppSortReturn, bool sortMustBeExpressed
) {
    RefUVToConstructorCount++;

/*****  Obtain sort indices for the reference u-vector  *****/
    M_CPD *pSort = rtREFUV_AscendingSortIndices (refuvCPD, sortMustBeExpressed);
    sortMustBeExpressed = IsntNil (pSort);

/*****  Create a link constructor  *****/
    rtLINK_CType *linkc = rtLINK_RefConstructor (refuvCPD, UV_CPx_RefPToken);

/*****  Traverse the reference u-vector in sorted order...  *****/
    enum {
        doingNothing, gotOne, buildingRange, buildingRepetition
    } state = doingNothing;

    rtREFUV_ElementType const *pRefUV = rtREFUV_CPD_Array (refuvCPD);
    rtREFUV_ElementType const *pIndex = sortMustBeExpressed 
					? rtREFUV_CPD_Array (pSort) : pRefUV;
    rtREFUV_ElementType const *pLimit = pIndex + UV_CPD_ElementCount (refuvCPD);

    rtREFUV_ElementType referenceNil = linkc->ReferenceNil ();

    rtREFUV_ElementType thisValue, origin;
    unsigned int count;
    for (
	rtREFUV_ElementType lastValue = 0; /* Initialized to suppress AIX/HP-UX warnings. */

	pIndex < pLimit && (
	    thisValue = sortMustBeExpressed ? pRefUV[*pIndex++] : *pIndex++
	) != referenceNil;

	lastValue = thisValue
    ) {
/*****  ... matching the new value to the last one encountered:  *****/
	switch (state) {
	case doingNothing:
	    origin = thisValue;
	    state = gotOne;
	    break;
	case gotOne:
	    if (thisValue == lastValue + 1) {
/*****  Start building a range...  *****/
		count = 2;
		state = buildingRange;
	    }
	    else if (thisValue == lastValue) {
/*****  Start building a repetition...  *****/
		count = 2;
		state = buildingRepetition;
	    }
	    else {
/*****  No pattern, output the last value and try again...  *****/
		linkc->Append (origin, 1, false);
		origin = thisValue;
	    }
	    break;
	case buildingRange:
	    if (thisValue == lastValue + 1)
/*****  Continue building a range...  *****/
		count++;
	    else {
/*****  Output the current range and start over...  *****/
		linkc->Append (origin, count, false);
		origin = thisValue;
		state = gotOne;
	    }
	    break;
	case buildingRepetition:
	    if (thisValue == lastValue)
/*****  Continue building a repetition...  *****/
		count++;
	    else {
/*****  Output the current repetition and start over...  *****/
		linkc->Append (origin, count, true);
		origin = thisValue;
		state = gotOne;
	    }
	    break;
	default:
	    ERR_SignalFault (
		EC__InternalInconsistency, UTIL_FormatMessage (
		    "rtLINK_RefUVToConstructor: Unknown state %d\n", state
		)
	    );
	    break;
	}  /*  End pattern building switch  */
    }  /*  End loop  */

/*****  Wrap up the last RRDC...  *****/
    switch (state) {
    case gotOne:
	linkc->Append (origin, 1, false);
	break;
    case buildingRange:
	linkc->Append (origin, count, false);
	break;
    case buildingRepetition:
	linkc->Append (origin, count, true);
	break;
    default:
	break;
    }  /*  End cleanup switch  */

/*****  ...and close the constructor  *****/
    if (sortMustBeExpressed) {
	linkc->Close (pSort, UV_CPx_PToken);
	if (ppSortReturn)
	    *ppSortReturn = pSort;
	else
	    pSort->release ();
    }
    else {
	linkc->Close (refuvCPD, UV_CPx_PToken);
	if (ppSortReturn)
	    *ppSortReturn = NilOf (M_CPD*);
    }

/*****  ...return the sort indices if a return address was specified...  *****/

/*****  ...and return the link constructor.  *****/
    return linkc;
}


/*************************************************************
 *****  Link <-> Reference U-Vector Conversion Routines  *****
 *************************************************************/

/********************************
 *  Link -> Reference U-Vector  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a link to a reference u-vector.
 *
 *  Argument:
 *	linkCPD			- a standard CPD for the link to be converted
 *				  to a reference u-vector.
 *	orderingUV		- an optional (Nil if not specified) CPD
 *				  for a reference u-vector which specifies
 *				  the order in which the elements of the
 *				  link constructor are to be assigned to
 *				  the reference u-vector created.
 *
 *  Returns:
 *	A standard CPD for the reference u-vector created.
 *
 *****/
PublicFnDef M_CPD *rtLINK_LinkToRefUV (M_CPD *linkCPD, M_CPD *orderingUV) {
    LinkToRefUVCount++;

    rtLINK_CType *linkc = rtLINK_ToConstructor (linkCPD);
    M_CPD *result = linkc->ToRefUV (orderingUV);
    linkc->release ();

    return result;
}


/**********************************************
 *****  Standard Element Access Routines  *****
 **********************************************/

/*****************************************************************************
 *  Link Constructor Source - Link Constructor Subscript Element Extraction  *
 *****************************************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	this			- the address of a link constructor from which
 *				  the elements are to be extracted.
 *	subscriptLinkC		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	The address of a link constructor containing the elements extracted.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = this[link[i]];
 *
 *****/
rtLINK_CType *rtLINK_CType::Extract (rtLINK_CType *subscriptLinkC) {
    int resultCount, resultOrigin, sourcePosition, nextSourcePosition;

/*****  Define the subscript link constructor traversal macros:  *****/
#define advanceSource {\
    source		= nextSource;\
    sourcePosition	= nextSourcePosition;\
    nextSourcePosition  = rtLINK_RRDC_LinkCumulative (\
	nextSource = rtLINK_RRDC_NextRRDC (source)\
    );\
}

#define locateSource(subscriptPosition, subscriptCount, subscriptOrigin)\
    while (subscriptOrigin >= nextSourcePosition)\
	advanceSource;\
    resultOrigin = rtLINK_RRDC_ReferenceOrigin (source);\
    if (!rtLINK_RRDC_RepeatedRef (source))\
        resultOrigin += subscriptOrigin - sourcePosition;\
    else;\
    if (TracingLCAppending) {\
	IO_printf ("in rtLINK_LCExtractFromLc locateSrc: ");\
	IO_printf (\
	    "srcPos=%d, nextSrcPos=%d, resultOrig=%d\n",\
	    sourcePosition, nextSourcePosition, resultOrigin\
	);\
    }\
    else

#define handleNil(subscriptPosition, subscriptCount, referenceNil)

#define handleRepetition(subscriptPosition, subscriptCount, subscriptOrigin)\
    locateSource (subscriptPosition, subscriptCount, subscriptOrigin);\
    resultLinkC->Append (resultOrigin, subscriptCount, true)

#define handleRange(subscriptPosition, subscriptCount, subscriptOrigin)\
    locateSource (subscriptPosition, subscriptCount, subscriptOrigin);\
    if ((resultCount = nextSourcePosition - subscriptOrigin) > subscriptCount)\
	resultCount = subscriptCount;\
    resultLinkC->Append (resultOrigin, resultCount, rtLINK_RRDC_RepeatedRef (source));\
    while (subscriptCount != resultCount) {\
	subscriptCount -= resultCount;\
	advanceSource;\
	resultOrigin	= rtLINK_RRDC_ReferenceOrigin (source);\
	if ((resultCount = nextSourcePosition - sourcePosition) >\
	    subscriptCount\
	) resultCount = subscriptCount;\
	resultLinkC->Append (resultOrigin, resultCount, rtLINK_RRDC_RepeatedRef (source));\
    }


    LCExtractFromLCCount++;

/*****
 * Align 'source' and 'subscriptLinkC' and validate the applicability of
 * 'subscriptLinkC' as an extraction index for 'source'...
 *****/
    Align ();
    subscriptLinkC->AlignForExtract (m_pPPT, -1);

/*****  ...initialize the source traversal state variables...  *****/
    rtLINK_RRDCType *source, *nextSource;
    if (IsntNil (source = m_pChainHead)) {
        sourcePosition	    = rtLINK_RRDC_LinkCumulative (source);
	nextSourcePosition  = rtLINK_RRDC_LinkCumulative (
	    nextSource = rtLINK_RRDC_NextRRDC (source)
	);
    }

/*****  ...create the result link constructor...  *****/
    rtLINK_CType *resultLinkC = rtLINK_RefConstructor (m_pRPT, -1);

/*****  ...traverse the subscript...  *****/
    rtLINK_TraverseRRDCList (subscriptLinkC, handleNil, handleRepetition, handleRange)

    if (TracingRRDCAllocator)
	IO_printf ("after traverse call\n");

/*****  ...close the completed constructor...  *****/
    resultLinkC->Close (subscriptLinkC->PPT ());

/*****  ...and return it.  *****/
    return resultLinkC;

/*****  Delete the subscript link constructor traversal macros:  *****/
#undef advanceSource
#undef locateSource
#undef handleNil
#undef handleRepetition
#undef handleRange
}


/*****************************************************************
 *  Link Source - Link Constructor Subscript Element Extraction  *
 *****************************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the link from which
 *				  the elements are to be extracted.
 *	subscriptLinkC		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	The address of a link constructor containing the elements extracted.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_LCExtract (M_CPD *sourceCPD, rtLINK_CType *subscriptLinkC) {
    rtLINK_RRDType		*source;
    int				resultCount, resultOrigin,
				sourcePosition, nextSourcePosition;

/*****  Define the subscript link constructor traversal macros:  *****/
#define advanceSource {\
    source++;\
    sourcePosition = nextSourcePosition;\
    nextSourcePosition  = rtLINK_RRD_LinkCumulative (source + 1);\
}

#define locateSource(subscriptPosition, subscriptCount, subscriptOrigin)\
    while (subscriptOrigin >= nextSourcePosition)\
	advanceSource;\
    resultOrigin = rtLINK_RRD_ReferenceOrigin (source);\
    if (!rtLINK_RRD_RepeatedRef (source))\
        resultOrigin += subscriptOrigin - sourcePosition;\
    if (TracingLCAppending) {\
	IO_printf ("in rtLINK_LCExtract locateSrc: ");\
	IO_printf (\
	    "srcPos=%d, nextSrcPos=%d, resultOrig=%d\n",\
	    sourcePosition, nextSourcePosition, resultOrigin\
	);\
    }\
    else


#define handleNil(subscriptPosition, subscriptCount, referenceNil)

#define handleRepetition(subscriptPosition, subscriptCount, subscriptOrigin)\
    locateSource (subscriptPosition, subscriptCount, subscriptOrigin);\
    resultLinkC->Append (resultOrigin, subscriptCount, true)

#define handleRange(subscriptPosition, subscriptCount, subscriptOrigin)\
    locateSource (subscriptPosition, subscriptCount, subscriptOrigin);\
    if ((resultCount = nextSourcePosition - subscriptOrigin) > subscriptCount)\
	resultCount = subscriptCount;\
    resultLinkC->Append (resultOrigin, resultCount, rtLINK_RRD_RepeatedRef (source));\
    while (subscriptCount != resultCount) {\
	subscriptCount -= resultCount;\
	advanceSource;\
	resultOrigin = rtLINK_RRD_ReferenceOrigin (source);\
	if ((resultCount = nextSourcePosition - sourcePosition) > subscriptCount)\
	    resultCount = subscriptCount;\
	resultLinkC->Append (resultOrigin, resultCount, rtLINK_RRD_RepeatedRef (source));\
    }


    LCExtractCount++;

/*****
 * Align 'source' and 'subscriptLinkC' and validate the applicability of
 * 'subscriptLinkC' as an extraction index for 'source'...
 *****/
    subscriptLinkC->AlignForExtract (rtLINK_AlignLink (sourceCPD), rtLINK_CPx_PosPToken);

/*****  ...initialize the source traversal state variables...  *****/
    if (IsntNil (source = rtLINK_CPD_RRDArray (sourceCPD))) {
        sourcePosition	    = rtLINK_RRD_LinkCumulative (source);
	nextSourcePosition  = rtLINK_RRD_LinkCumulative (source + 1);
    }

/*****  ...create the result link constructor...  *****/
    rtLINK_CType *resultLinkC = rtLINK_RefConstructor (sourceCPD, rtLINK_CPx_RefPToken);

/*****  ...traverse the subscript...  *****/
    rtLINK_TraverseRRDCList (
	subscriptLinkC, handleNil, handleRepetition, handleRange
    )

    if (TracingRRDCAllocator) IO_printf ("after traverse call\n");

/*****  ...close the completed constructor...  *****/
    resultLinkC->Close (subscriptLinkC->PPT ());

/*****  ...and return it.  *****/
    return resultLinkC;

/*****  Delete the subscript link constructor traversal macros:  *****/
#undef advanceSource
#undef locateSource
#undef handleNil
#undef handleRepetition
#undef handleRange
}


/***********************************************************************
 *  Link Constructor Source - Reference U-Vector Subscript Extraction  *
 ***********************************************************************/

/*---------------------------------------------------------------------------
 *****  Reference U-Vector Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	this			- a pointer to a link constructor from which
 *				  the elements are to be extracted.
 *	pSubscript		- a standard CPD for a reference U-Vector
 *				  specifying the elements to be extracted.
 *				  The reference U-Vector must be referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for a reference U-Vector containing the elements
 *	extracted.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (refuv); i++)
 *		result[i] = this[refuv[i]];
 *
 *****/
M_CPD *rtLINK_CType::Extract (M_CPD *pSubscript) {
    UVExtractFromLCCount++;
/*
    unsigned int sSource = ElementCount ();
    unsigned int sSubscript = UV_CPD_ElementCount (pSubscript);

    if (sSource > sSubscript) IO_printf (
	"+++  rtLINK_CType::Extract Reverse Candidate, |this| =%7u, |subs| =%7u\n",
	sSource, sSubscript
    );
*/
    M_CPD *sourceRefUV = ToRefUV ();
    M_CPD *result = rtREFUV_UVExtract (sourceRefUV, pSubscript);
    sourceRefUV->release ();

    return result;
}


/*******************************************************************
 *  Link Source - Reference U-Vector Subscript Element Extraction  *
 *******************************************************************/

/*---------------------------------------------------------------------------
 *****  Reference U-Vector Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the link from which
 *				  the elements are to be extracted.
 *	pSubscript		- a standard CPD for a reference U-Vector
 *				  specifying the elements to be extracted.
 *				  The reference U-Vector must be referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for a reference U-Vector containing the elements
 *	extracted.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *****/
PublicFnDef M_CPD *rtLINK_UVExtract (M_CPD *sourceCPD, M_CPD *pSubscript) {
    UVExtractCount++;
/*
    unsigned int sSource = UV_CPD_ElementCount (sourceCPD);
    unsigned int sSubscript = UV_CPD_ElementCount (pSubscript);

    if (sSource > sSubscript) IO_printf (
	"+++  rtLINK_UVExtract Reverse Candidate, |this| =%7u, |subs| =%7u\n",
	sSource, sSubscript
    );
*/
    M_CPD *sourceAsRefUV = rtLINK_LinkToRefUV (sourceCPD);
    M_CPD *result = rtREFUV_UVExtract (sourceAsRefUV, pSubscript);
    sourceAsRefUV->release ();

    return result;
}


/********************************************************
 * Link Source - Reference Subscript Element Extraction *
 ********************************************************/

/*---------------------------------------------------------------------------
 ***** Reference Subscripted Element Extraction Routine.
 *
 * Arguments:
 *	result			- the address of a 'reference' which will be
 *				  INITIALIZED to the extracted element.
 *	source			- a pointer to a link from which the element
 *				  is to be extracted.
 *	reference		- the address of a reference which specifies
 *				  the element to be extracted.  'reference'
 *			  	  must be referentially related to 'source'.
 *
 * Returns:
 *	A new reference containing the element extracted.
 *
 *****/
PublicFnDef void rtLINK_RefExtract (
    rtREFUV_TypePTR_Reference	result,
    M_CPD*			source,
    rtREFUV_TypePTR_Reference	reference
)
{
#define access(index) rtLINK_RRD_LinkCumulative (rrdp+index)

    RefExtractCount++;

/***** Align and check for consistency of PTokens *****/
    rtLINK_AlignLink (source);
    rtREFUV_AlignAndValidateRef (reference, source, rtLINK_CPx_PosPToken);

    unsigned int value = rtREFUV_Ref_Element (reference);
/*****  If the reference points to the 'nil' position of the link, ...  *****/
/*****  ... return the link's reference nil:  *****/
    if (value == rtLINK_CPD_ElementCount (source))
	value = rtPTOKEN_BaseElementCount (source, rtLINK_CPx_RefPToken);
    else {
/*****  ... otherwise, ...  *****/
    /*****  ...	binary Search for the value, ...  *****/
	int low, hi, index;
	int size = rtLINK_CPD_RRDCount (source);

	rtLINK_RRDType* rrdp = rtLINK_CPD_RRDArray (source);

	for (
	    low = 0, hi = size - 1, index = (hi + low) / 2;
	    access(index) != value && low <= hi;
	    index = (hi + low) / 2
	)
	{
	    if (value < access(index))
		hi = index - 1;
	    else low = index + 1;
	}

    /***** Determine the result value *****/
	value = rtLINK_RRD_RepeatedRef (rrdp + index)
	    ? rtLINK_RRD_ReferenceOrigin (rrdp + index)
	    : rtLINK_RRD_ReferenceOrigin (rrdp + index)
	    + (value - rtLINK_RRD_LinkCumulative (rrdp + index));
    }

/***** Set the result reference's values *****/
/* IO_printf ("result reference = %d\n", value); */
    DSC_InitReferenceScalar (
	*result, rtLINK_CPD_RefPTokenCPD (source), value
    );

#undef access
}


/**************************************
 *****  Link Complement Routines  *****
 **************************************/

rtLINK_CType *rtLINK_CType::Complement () {
    LCComplementCount++;

/*****  Align this ... *****/
    Align ();

/*****  Create the result ... *****/
    rtLINK_CType *result = rtLINK_RefConstructor (m_pRPT, -1);

/*****  Loop thru this appending anything not in this, ...  *****/
    int next = 0;
    rtLINK_RRDCType *srcp = m_pChainHead;
    if (srcp)
	for (
	    rtLINK_RRDCType *srcl = m_pFinalRRDC;
	    srcp != srcl;
	    srcp = rtLINK_RRDC_NextRRDC (srcp)
	) {
	    int origin = rtLINK_RRDC_ReferenceOrigin (srcp);
	    if (origin > next)
		rtLINK_AppendRange (result, next, origin - next);
	    next = (rtLINK_RRDC_RepeatedRef (srcp))
		 ? origin + 1
		 : origin + rtLINK_RRDC_N (srcp);
	}

/*****  Append the last piece if neccessary ... *****/
    int refNil = ReferenceNil ();
    if (refNil != next)
	rtLINK_AppendRange (result, next, refNil - next);

/*****  Close the result linkc ... *****/
    M_CPD *newPosPToken = rtPTOKEN_New (m_pPPT->ScratchPad (), result->ElementCount ());
    result->Close (newPosPToken);
    newPosPToken->release ();

    return result;
}


/**************************************
 ***** Link Modification Routines *****
 **************************************/

/*********
 *  Add  *
 *********/

/***********************************************************
 *  Link Constructor Target - Link Constructor Add Source  *
 ***********************************************************/

/*---------------------------------------------------------------------------
 ***** Routine to add elements to a link constructor.
 *
 *  Arguments:
 *	this			- the link constructor to update.
 *	addSourceLinkC		- the address of a link constructor containing
 *				  the elements to add.
 *	additionsMapWanted	- an optional (default=true) boolean that can be
 *				  used to suppress the creation of the addition
 *				  map when it isn't needed.
 *
 *  Returns:
 *	A link constructor indentifying the positions added to 'this' or NULL if
 *	additionsMapWanted is false.
 *
 *****/
rtLINK_CType *rtLINK_CType::Add (rtLINK_CType *addSourceLinkC, bool additionsMapWanted) {
    int val, newOff = 0;
    struct cursor_t {
	int	_currentOff,
		_currentValue,
		_currentCnt;
	bool	_currentRepeat;
    } positions[2];

/*** A is the Add Link Contructor
 *** B is the Target Link Constructor
 ***/
#define ofA 0
#define ofB 1

#define currentOff(which)	(positions[which]._currentOff)
#define currentValue(which)	(positions[which]._currentValue)
#define currentCnt(which)	(positions[which]._currentCnt)
#define currentRepeat(which)	(positions[which]._currentRepeat)

#define outputNew(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLCs: OUTPUT-NEW: newOff=%d, org=%d, cnt=%d, rep=%d\n",\
	newOff, origin, count, repeat\
    );\
    if (pAdditionsMap)\
	pAdditionsMap->Append (newOff, count, false);\
    targetPtokenC->AppendAdjustment (newOff, count);\
    temptargetLinkC->Append (origin, count, repeat);\
    newOff += count;\
}

#define outputOld(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLCs: OUTPUT-OLD: org=%d, cnt=%d, rep=%d\n",\
	origin, count, repeat\
    );\
    temptargetLinkC->Append (origin, count, repeat);\
    newOff += count;\
}

#define advanceOfA {\
    if (TracingLCAppending) IO_printf ("addTraverseLCs: in advanceOfA...\n");\
    /*** output whats left ***/\
    if (currentCnt(ofA) > 0) outputNew (\
	currentValue(ofA), currentCnt(ofA), currentRepeat(ofA)\
    );\
    /*** goto next chunk ***/\
    currentRRDCofA = rtLINK_RRDC_NextRRDC (currentRRDCofA);\
    /*** set global place markers ***/\
    if (IsntNil (rtLINK_RRDC_NextRRDC (currentRRDCofA))) {\
	currentOff(ofA) = rtLINK_RRDC_LinkCumulative (currentRRDCofA);\
	currentCnt(ofA) = rtLINK_RRDC_N (currentRRDCofA);\
	currentValue(ofA) = rtLINK_RRDC_ReferenceOrigin (currentRRDCofA);\
	currentRepeat(ofA) = rtLINK_RRDC_RepeatedRef (currentRRDCofA);\
    }\
    else currentOff(ofA) = -999;\
}

#define advanceOfB {\
    if (TracingLCAppending) IO_printf ("addTraverseLCs: in advanceOfB...\n");\
    /*** output whats left ***/\
    if (currentCnt(ofB) > 0) outputOld (\
	currentValue(ofB), currentCnt(ofB), currentRepeat(ofB)\
    );\
    /*** goto next chunk ***/\
    currentRRDCofB = rtLINK_RRDC_NextRRDC (currentRRDCofB);\
    /*** set global place markers ***/\
    if (IsntNil (rtLINK_RRDC_NextRRDC (currentRRDCofB))) {\
	currentOff(ofB) = rtLINK_RRDC_LinkCumulative (currentRRDCofB);\
	currentCnt(ofB) = rtLINK_RRDC_N (currentRRDCofB);\
	currentValue(ofB) = rtLINK_RRDC_ReferenceOrigin (currentRRDCofB);\
	currentRepeat(ofB) = rtLINK_RRDC_RepeatedRef (currentRRDCofB);\
    }\
    else currentOff(ofB) = -999;\
}

#define notInCurrentChunk(which,value) (\
    currentRepeat (which)\
    ? (value) > currentValue (which)\
    : (value) > currentValue (which) + currentCnt (which) - 1\
)

#define notNil(which) (currentOff (which) != -999)

#define locate(which,advanceHandler,outputHandler,value) {\
    if (TracingLCAppending) IO_printf ("addTraverseLCs: in Locate...\n");\
    while (notInCurrentChunk(which, value) && notNil(which))\
	advanceHandler;\
    if (notNil(which) && currentValue(which)<(value) && !currentRepeat(which)) {\
	outputHandler(\
	    currentValue (which),\
	    ((value) - currentValue(which)),\
	    currentRepeat(which)\
	);\
	currentOff(which) = currentOff(which) + ((value) - currentValue(which));\
	currentCnt(which) = currentCnt(which) - ((value) - currentValue(which));\
	currentValue(which) = (value);\
    }\
}

#define adjustUsedPointers(which,advanceHandler) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLCs: in adjustUsedPointers...\n"\
    );\
    if (currentRepeat(which)) {\
	currentCnt(which)   = 0;\
	advanceHandler;\
    }\
    else {\
	currentCnt(which)   -= 1;\
	currentOff(which)   += 1;\
	currentValue(which) += 1;\
	if (currentCnt(which) == 0) advanceHandler;\
    }\
}

#define dumpit() {\
    IO_printf (\
"    OFF     VALUE   CNT     REPEAT\nA   %-6d  %-6d  %-6d  %-6d\nB   %-6d  %-6d  %-6d  %-6d\n",\
      currentOff(ofA), currentValue(ofA), currentCnt(ofA), currentRepeat(ofA),\
      currentOff(ofB), currentValue(ofB), currentCnt(ofB), currentRepeat(ofB)\
    );\
}

#define debugPrint(string) {\
    if (TracingLCAppending) {\
	IO_printf ("%s", string);\
        dumpit ();\
    }\
}


/****** Source code begins here *****/
    LCAddFromLCCount++;
    if (TracingLCAppending)
	IO_printf ("Entering rtLINK_LCAddtoLC...\n");

/*****
 * Align 'this' and 'addSourceLinkC' and validate the applicability of
 * 'addSourceLinkC' as an extraction index for 'temptargetLinkC'...
 *****/
    Align ();
    addSourceLinkC->AlignForAdd (m_pRPT, -1);

/*****  Initialize the add source traversal state variables... *****/
    rtLINK_RRDCType *currentRRDCofA = rtLINK_LC_ChainHead (addSourceLinkC);

    if (IsntNil (currentRRDCofA) && IsntNil (rtLINK_RRDC_NextRRDC (currentRRDCofA))) {
	currentOff(ofA) = rtLINK_RRDC_LinkCumulative (currentRRDCofA);
	currentCnt(ofA) = rtLINK_RRDC_N (currentRRDCofA);
	currentValue(ofA) = rtLINK_RRDC_ReferenceOrigin (currentRRDCofA);
	currentRepeat(ofA) = rtLINK_RRDC_RepeatedRef (currentRRDCofA);
    }
    else currentOff(ofA) = -999;


/*****  Initialize the target traversal state variables...  *****/
    rtLINK_RRDCType *currentRRDCofB = m_pChainHead;

    if (IsntNil (currentRRDCofB) && IsntNil (rtLINK_RRDC_NextRRDC (currentRRDCofB))) {
	currentOff(ofB) = rtLINK_RRDC_LinkCumulative (currentRRDCofB);
	currentCnt(ofB) = rtLINK_RRDC_N (currentRRDCofB);
	currentValue(ofB) = rtLINK_RRDC_ReferenceOrigin (currentRRDCofB);
	currentRepeat(ofB) = rtLINK_RRDC_RepeatedRef (currentRRDCofB);
    }
    else currentOff(ofB) = -999;

/*****  Create a ptoken constructor for this... *****/
    rtPTOKEN_CType *targetPtokenC = rtPTOKEN_NewShiftPTConstructor (m_pPPT, -1);

/*****  Create the result link constructor...  *****/
    rtLINK_CType *pAdditionsMap = additionsMapWanted ? rtLINK_PosConstructor (
	addSourceLinkC->PPT (), -1
    ) : NilOf (rtLINK_CType *);

/*****  Create the temporary target link constructor...  *****/
    rtLINK_CType *temptargetLinkC = rtLINK_RefConstructor (m_pRPT, -1);

/***** Traverse the two LCs...  *****/
    while (notNil(ofA) && notNil(ofB)) {
	if (currentValue(ofA) < (val = currentValue(ofB))) {
    	    debugPrint ("...addTraverseLCs: a < b\n");
	    locate (ofA, advanceOfA, outputNew, val);
	}
	else if ((val = currentValue(ofA)) > currentValue(ofB)) {
	    debugPrint ("...addTraverseLCs:  a > b\n");
	    locate (ofB, advanceOfB, outputOld, val);
	}
	else { /* values are equal */
	    debugPrint ("...addTraverseLCs:  a == b\n");
	    outputNew (
		currentValue(ofA),
		currentRepeat(ofA) ? currentCnt(ofA) : 1,
		currentRepeat(ofA)
	    );
	    outputOld (
		currentValue(ofB),
		currentRepeat(ofB) ? currentCnt(ofB) : 1,
		currentRepeat(ofB)
	    );
	    adjustUsedPointers(ofA,advanceOfA);
	    adjustUsedPointers(ofB,advanceOfB);
	}
    }

    debugPrint ("addTraverseLCs: end of looping one ran out\n");

    while (notNil(ofA))	advanceOfA;
    while (notNil(ofB)) advanceOfB;

/*****  All done creating the two new constructors.  Now complete them...*****/
/*****  Close the ptoken for the target... *****/
    M_CPD *newPosPtokenCPD = targetPtokenC->ToPToken ();

/***** Reconstruct this from the temptargetLinkC...  *****/
    temptargetLinkC->Close (newPosPtokenCPD);

    ReconstructFrom (temptargetLinkC);

/*****  ...close the completed result link constructor...  *****/
    /* the Ref ptoken of the result is the new Pos ptoken of the target */
    if (pAdditionsMap)
	pAdditionsMap->Close (newPosPtokenCPD);

    temptargetLinkC->release ();
    newPosPtokenCPD->release ();

/*****  ...and return it.  *****/
    if (TracingLCAppending)
	IO_printf ("exiting rtLINK_LCAddtoLC\n");

    return pAdditionsMap;

/*****  Delete the addSource link constructor traversal macros:  *****/
#undef ofA
#undef ofB
#undef currentOff
#undef currentValue
#undef currentCnt
#undef currentRepeat
#undef outputNew
#undef outputOld
#undef advanceOfA
#undef advanceOfB
#undef notInCurrentChunk
#undef notNil
#undef locate
#undef adjustUsedPointers
#undef dumpit
#undef debugPrint
}


/***********************************************
 *  Link Target - Link Constructor Add Source  *
 ***********************************************/

/*---------------------------------------------------------------------------
 ***** Routine to add and/or locate elements to a link.
 *
 *  Arguments:
 *	targetLinkCPD		- the address of a standard CPD for the link
 *				  to be added to.
 *	addSourceLinkC		- the address of a link constructor containing
 *				  the elements to add.
 *	locateOrAdd		- if false add all of the elements in
 *		                  'addSourceLinkC' to the target link.
 *                                if true only add the elements from
 *                                'addSourceLinkC' which are not already in the
 *                                target link.  If any of the source elements
 *                                are found in the target link, then the
 *                                'addedLinkC' must be constructed.
 *	addedLinkC		- optional and only valid if 'locateOrAdd' is
 *                                true (if not requested or valid set to Nil).
 *                                the pointer to the address of an
 *                                uninitialized link constructor
 *                                which will contain the positions from
 *                                'addSourceLinkC' whose values were added to
 *                                the target link.  If none of the values from
 *                                the source were added, then this link will be
 *                                unused and set to Nil.
 *
 *  Returns:
 *	A link constructor identifying the elements added to 'targetLinkCPD'.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_LCAddLocate (
    M_CPD*			targetLinkCPD,
    rtLINK_CType*		addSourceLinkC,
    int /*bool*/		locateOrAdd,
    rtLINK_CType*		*addedLinkC
)
{
    rtLINK_CType		*resultLinkC,
				*targetLinkC,
				*foundLinkC = NilOf (rtLINK_CType*);
    rtLINK_RRDType		*rrdp, *rrdpl;
    rtLINK_RRDCType		*currentRRDC;
    bool			wantAddedLinkC;
    int				val,
				newOff = 0,
				refNil;
    M_CPD*			newPosPtokenCPD;
    struct cursor_t {
	int	_currentOff,
		_currentValue,
		_currentCnt;
	bool	_currentRepeat;
    } sourceLC, targetLink;


#define currentOff(which)	((which)._currentOff)
#define currentValue(which)	((which)._currentValue)
#define currentCnt(which)	((which)._currentCnt)
#define currentRepeat(which)	((which)._currentRepeat)

#define	initFoundLinkC {\
    if (IsNil (foundLinkC))\
	foundLinkC = rtLINK_RefConstructor (addSourceLinkC->PPT (), -1);\
}

#define outputNew(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLinks: OUTPUT-NEW: newOff=%d, org=%d, cnt=%d, rep=%d\n",\
	newOff, origin, count, repeat\
    );\
    if (locateOrAdd && repeat) {\
	/*** only add ONE value ***/\
        rtLINK_AppendRepeat (resultLinkC, newOff, count);\
	targetPtokenC->AppendAdjustment (newOff, 1);\
	rtLINK_AppendRange (targetLinkC, origin, 1);\
	if (wantAddedLinkC) {\
	    initFoundLinkC;\
	    rtLINK_AppendRange (\
		foundLinkC, currentOff (sourceLC) + 1, count - 1\
	    );\
	}\
	newOff += 1;\
    }\
    else {\
        resultLinkC->Append (newOff, count, false);\
	targetPtokenC->AppendAdjustment (newOff, count);\
        targetLinkC->Append (origin, count, repeat);\
	newOff += count;\
    }\
}

#define outputOld(origin,count,repeat) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLinks: OUTPUT-OLD: org=%d, cnt=%d, rep=%d\n",\
	origin, count, repeat\
    );\
    targetLinkC->Append (origin, count, repeat);\
    newOff += count;\
}

#define outputFound(origin, count, repeat) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLinks: OUTPUT-FOUND: org=%d, cnt=%d, rep=%d\n",\
	origin, count, repeat\
    );\
    resultLinkC->Append (newOff, count, repeat);\
    if (wantAddedLinkC) {\
	initFoundLinkC;\
	rtLINK_AppendRange (foundLinkC, origin, count);\
    }\
}

#define advanceSource {\
    if (TracingLCAppending) IO_printf ("LCAddLocate: in advanceSource...\n");\
    /*** output whats left ***/\
    if (currentCnt (sourceLC) > 0) outputNew (\
	currentValue (sourceLC),\
	currentCnt (sourceLC),\
	currentRepeat (sourceLC)\
    );\
    /*** goto next chunk ***/\
    currentRRDC = rtLINK_RRDC_NextRRDC (currentRRDC);\
    /*** set global place markers ***/\
    if (IsntNil (rtLINK_RRDC_NextRRDC (currentRRDC))) {\
	currentOff (sourceLC) = rtLINK_RRDC_LinkCumulative (currentRRDC);\
	currentCnt (sourceLC) = rtLINK_RRDC_N (currentRRDC);\
	currentValue (sourceLC) = rtLINK_RRDC_ReferenceOrigin (currentRRDC);\
	currentRepeat (sourceLC) = rtLINK_RRDC_RepeatedRef (currentRRDC);\
    }\
    else currentOff (sourceLC) = -999;\
}

#define advanceTarget {\
    if (TracingLCAppending) IO_printf ("LCAddLocate: in advanceTarget...\n");\
    /*** output whats left ***/\
    if (currentCnt (targetLink) > 0) outputOld (\
	currentValue  (targetLink),\
	currentCnt (targetLink),\
	currentRepeat (targetLink)\
    );\
    /*** goto next chunk ***/\
    rrdp++;\
    /*** set global place markers ***/\
    if (rrdp < rrdpl) {\
	currentOff (targetLink) = rtLINK_RRD_LinkCumulative (rrdp);\
	currentCnt (targetLink) = rtLINK_RRD_LinkCumulative (rrdp+1) -\
			  rtLINK_RRD_LinkCumulative (rrdp);\
	currentValue (targetLink) = rtLINK_RRD_ReferenceOrigin (rrdp);\
	currentRepeat (targetLink) = rtLINK_RRD_RepeatedRef (rrdp);\
    }\
    else currentOff (targetLink) = -999;\
}

#define notInCurrentChunk(which,value) (\
    currentRepeat (which)\
    ? (value) > currentValue (which)\
    : (value) > currentValue (which) + currentCnt (which) - 1\
)

#define notNil(which) (currentOff (which) != -999)

#define locate(value, which, advance, output) {\
    if (TracingLCAppending) IO_printf ("addTraverseLinks: in Locate...\n");\
    while (notInCurrentChunk(which, value) && notNil(which))\
	advance;\
    if (notNil(which) && (currentValue(which) < (value)) && !currentRepeat(which)) {\
	output(\
	    currentValue (which), ((value) - currentValue(which)), currentRepeat(which)\
	);\
	currentOff(which) = currentOff(which) + ((value) - currentValue(which));\
	currentCnt(which) = currentCnt(which) - ((value) - currentValue(which));\
	currentValue(which) = (value);\
    }\
}

#define adjustUsedPointers(which, advance) {\
    if (TracingLCAppending) IO_printf (\
	"addTraverseLinks: in adjustUsedPointers...\n"\
    );\
    if (currentRepeat(which)) {\
	currentCnt(which)   = 0;\
	advance;\
    }\
    else {\
	currentCnt(which)   -= 1;\
	currentOff(which)   += 1;\
	currentValue(which) += 1;\
	if (currentCnt(which) == 0) advance;\
    }\
}

#define dumpit() {\
    IO_printf (\
"    OFF     VALUE   CNT     REPEAT\nA   %-6d  %-6d  %-6d  %-6d\nB   %-6d  %-6d  %-6d  %-6d\n",\
	currentOff (sourceLC),\
	currentValue (sourceLC),\
	currentCnt (sourceLC),\
	currentRepeat (sourceLC),\
	currentOff (targetLink),\
	currentValue (targetLink),\
	currentCnt (targetLink),\
	currentRepeat (targetLink)\
    );\
}

#define debugPrint(string) {\
    if (TracingLCAppending) {\
	IO_printf ("%s", string);\
        dumpit ();\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtLINK_LCAddLocate'
 *---------------------------------------------------------------------------
 */

    if (TracingLCAppending) IO_printf ("Entering rtLINK_LCAdd...\n");

/*****
 * Align 'targetLinkC' and 'addSourceLinkC' and validate the applicability of
 * 'addSourceLinkC' as an extraction index for 'targetLinkC'...
 *****/
    rtLINK_AlignLink (targetLinkCPD);
    addSourceLinkC->AlignForAdd (targetLinkCPD, rtLINK_CPx_RefPToken);

/***** Determine the source's referenceNil  *****/
    refNil = addSourceLinkC->ReferenceNil ();

/*****  Initialize *addedLinkC if neccessary ... *****/
    if (locateOrAdd) {
	LocateOrAddFromLCCount++;
	wantAddedLinkC = IsntNil (addedLinkC);
	if (wantAddedLinkC)
    	    *addedLinkC = NilOf (rtLINK_CType*);
    }
    else LCAddCount++;

/*****  Create the result link constructor...  *****/
    resultLinkC = rtLINK_PosConstructor (addSourceLinkC->PPT (), -1);

/*****  If the source link is empty, close resultLinkC and return ... *****/
    if (addSourceLinkC->ElementCount () == 0) {
	resultLinkC->Close (targetLinkCPD, rtLINK_CPx_PosPToken);
	return resultLinkC;
    }

/*****  Create a ptoken constructor for the targetLinkC... *****/
    rtPTOKEN_CType *targetPtokenC = rtPTOKEN_NewShiftPTConstructor (
	targetLinkCPD, rtLINK_CPx_PosPToken
    );

/*****  Create the temporary target link constructor...  *****/
    targetLinkC = rtLINK_RefConstructor (targetLinkCPD, rtLINK_CPx_RefPToken);

/*****  Initialize the add source traversal state variables... *****/
    currentRRDC = rtLINK_LC_ChainHead (addSourceLinkC);

    if (IsntNil (currentRRDC) && IsntNil (rtLINK_RRDC_NextRRDC (currentRRDC)))
    {
	currentOff (sourceLC) = rtLINK_RRDC_LinkCumulative (currentRRDC);
	currentCnt (sourceLC) = rtLINK_RRDC_N (currentRRDC);
	currentValue (sourceLC) = rtLINK_RRDC_ReferenceOrigin (currentRRDC);
	currentRepeat (sourceLC) = rtLINK_RRDC_RepeatedRef (currentRRDC);
    }
    else currentOff (sourceLC) = -999;


/*****  Initialize the target traversal state variables...  *****/
    rrdp = rtLINK_CPD_RRDArray (targetLinkCPD);
    rrdpl = (rtLINK_CPD_RRDCount (targetLinkCPD) + rrdp);

    if (rrdp < rrdpl) {
	currentOff (targetLink) = rtLINK_RRD_LinkCumulative (rrdp);
	currentCnt (targetLink) = rtLINK_RRD_LinkCumulative (rrdp+1) -
			  rtLINK_RRD_LinkCumulative (rrdp);
	currentValue (targetLink) = rtLINK_RRD_ReferenceOrigin (rrdp);
	currentRepeat (targetLink) = rtLINK_RRD_RepeatedRef (rrdp);
    }
    else currentOff (targetLink) = -999;

/*****  Traverse the links... *****/
    while (notNil (sourceLC) && notNil (targetLink)) {
	if (currentValue (sourceLC) < (val = currentValue (targetLink))) {
    	    debugPrint ("...addTraverseLinks: a < b\n");
	    locate (val, sourceLC, advanceSource, outputNew);
	}
	else if ((val = currentValue (sourceLC)) > currentValue (targetLink)) {
	    debugPrint ("...addTraverseLinks:  a > b\n");
	    locate (val, targetLink, advanceTarget, outputOld);
	}
	else { /* values are equal */
	    debugPrint ("...addTraverseLinks:  a == b\n");
	    /*****  Since the values are equal, only add the new one if
             *****  locate is false.  Otherwise, add to the found linkc
             *****/
	    if (!locateOrAdd) {
		outputNew (
		    currentValue (sourceLC),
		    (currentRepeat (sourceLC)) ? currentCnt (sourceLC) : 1,
		    currentRepeat (sourceLC)
		);
	    }
	    else {
		outputFound (
		    currentOff (sourceLC),
		    (currentRepeat (sourceLC)) ? currentCnt (sourceLC) : 1,
		    currentRepeat (sourceLC)
		);
	    }
	    outputOld (
		currentValue (targetLink),
		(currentRepeat (targetLink)) ? currentCnt (targetLink) : 1,
		currentRepeat (targetLink)
	    );
	    adjustUsedPointers (sourceLC, advanceSource);
	    adjustUsedPointers (targetLink, advanceTarget);
	}
    }

    debugPrint ("addTraverseLinks: end of looping one ran out\n");

    while (notNil (sourceLC)) {
        if (currentRepeat (sourceLC) && currentValue (sourceLC) == refNil) {
	    outputFound (currentOff (sourceLC), currentCnt (sourceLC), true);
	    currentCnt (sourceLC) = 0;
	    advanceSource;
	    if (notNil (sourceLC)) ERR_SignalFault (
		EC__InternalInconsistency, "RRD's beyond the reference nil"
	    );
	}
	else if (!currentRepeat (sourceLC) &&
		 currentValue (sourceLC) + currentCnt (sourceLC) - 1 == refNil
	)
	{
	    refNil = currentOff (sourceLC) + --currentCnt (sourceLC);
	    advanceSource;
	    outputFound (refNil, 1, false);
	    if (notNil (sourceLC)) ERR_SignalFault (
		EC__InternalInconsistency, "RRD's beyond the reference nil"
	    );
	}
	else advanceSource;
    }

    while (notNil (targetLink))
	advanceTarget;

/*****  All done creating the two new constructors.  Now complete them...*****/
/*****  Close the ptoken for the target... *****/
    newPosPtokenCPD = targetPtokenC->ToPToken ();

/***** Reconstruct targetLinkCPD from the new targetLinkC...  *****/
    targetLinkC->Close (newPosPtokenCPD);

    targetLinkCPD->CheckConsistency ();
    targetLinkCPD->EnableModifications ();
    rtLINK_CPD_IsInconsistent (targetLinkCPD) = true;
    ReconstructLink (targetLinkCPD, targetLinkC);

    /* set the link's positional ptoken to the new one */
    targetLinkCPD->StoreReference (rtLINK_CPx_PosPToken, newPosPtokenCPD);
    rtLINK_CPD_IsInconsistent (targetLinkCPD)   = false;

/*****  ...close the completed result link constructor...  *****/
    /* the Ref ptoken of the result is the new Pos ptoken of the target */
    resultLinkC->Close (newPosPtokenCPD);

    newPosPtokenCPD->release ();
    targetLinkC->release ();

/*****  Close foundLinkC and complement it into addedLinkC, if neccessary...
 *****/
    if (locateOrAdd && wantAddedLinkC) {
	if (IsntNil (foundLinkC)) {
	    /*** close the linkc ... ***/
	    newPosPtokenCPD = rtPTOKEN_New (
		addSourceLinkC->PPT ()->Space (), foundLinkC->ElementCount ()
	    );
	    foundLinkC->Close (newPosPtokenCPD);
	    newPosPtokenCPD->release ();

	    /*** if the found linkc has less elements than the source ... ***/
	    if (foundLinkC->ElementCount () != addSourceLinkC->ElementCount ())
	    /*** ... then,  complement it ... ***/
		*addedLinkC = foundLinkC->Complement ();
	    /*** ... else the complement is an empty linkc so don't make it. ***/
	    foundLinkC->release ();
	}
	else /*** found linkc is empty - so the complement is full...***/ {
	    *addedLinkC = rtLINK_RefConstructor (addSourceLinkC->PPT (), -1);
	    unsigned int size = addSourceLinkC->ElementCount ();
	    rtLINK_AppendRange (*addedLinkC, 0, size);
	    newPosPtokenCPD = rtPTOKEN_New (addSourceLinkC->PPT ()->Space (), size);
	    (*addedLinkC)->Close (newPosPtokenCPD);
	    newPosPtokenCPD->release ();
	}
    }

/*****  ...and return  *****/
    if (TracingLCAppending)
	IO_printf ("exiting rtLINK_LCAdd\n");

    return resultLinkC;


/*****  Delete the addSource link constructor traversal macros:  *****/
#undef currentOff
#undef currentValue
#undef currentCnt
#undef currentRepeat
#undef outputNew
#undef outputOld
#undef outputFound
#undef advanceSource
#undef advanceTarget
#undef notInCurrentChunk
#undef notNil
#undef locate
#undef adjustUsedPointers
#undef dumpit
#undef debugPrint
}


/*---------------------------------------------------------------------------
 ***** Routine to add or locate an element to a link.
 *
 *  Arguments:
 *	targetLink		- the address of a standard CPD for the link
 *				  to be added to.
 *	srcRefp			- the pointer to a reference containing
 *				  the element to add.
 *	locationRefp		- the pointer to a reference which this routine
 *                                will initialize to contain the position in
 *                                'targetLink' at which the value was found or
 *                                added.
 *
 *  Returns:
 *	true if the value was added, false otherwise.
 *
 *****/
PublicFnDef bool rtLINK_LocateOrAddFromRef (
    M_CPD*			targetLink,
    rtREFUV_TypePTR_Reference	srcRefp,
    rtREFUV_TypePTR_Reference	locationRefp
)
{
    unsigned int location;
    bool notNil, valueAdded;
    rtPTOKEN_CType *targetPtokenC;

#define currentValue	(rtLINK_RRD_ReferenceOrigin (rrdp))
#define currentOff	(rtLINK_RRD_LinkCumulative (rrdp))
#define currentRepeat	(rtLINK_RRD_RepeatedRef (rrdp))
#define currentCnt	(rtLINK_RRD_LinkCumulative (rrdp+1) - currentOff)

#define notInCurrentChunk(value) (\
    currentRepeat\
    ? (value) > currentValue\
    : (value) > currentValue + currentCnt - 1\
)

    LocateOrAddFromRefCount++;

/***** Align and validate the source and target ... *****/
    rtLINK_AlignLink (targetLink);
    rtREFUV_AlignAndValidateRef (srcRefp, targetLink, rtLINK_CPx_RefPToken);

    unsigned int keyValue = rtREFUV_Ref_Element (srcRefp);
    if (keyValue == rtPTOKEN_CPD_BaseElementCount (rtREFUV_Ref_RefPTokenCPD (srcRefp))) {
        location    = rtLINK_CPD_ElementCount (targetLink);
	valueAdded  = false;
    }
    else {
/***** Create the targetLinkC ... *****/
	rtLINK_CType *targetLinkC = rtLINK_RefConstructor (
	    targetLink, rtLINK_CPx_RefPToken
	);

/***** Begin the traversal *****/
/***** Traverse the target... *****/
	rtLINK_RRDType* rrdp  = rtLINK_CPD_RRDArray (targetLink);
	rtLINK_RRDType* rrdpl = rtLINK_CPD_RRDCount (targetLink) + rrdp;

/***** Scan thru the link looking for the value ... *****/
	while ((notNil = (rrdp < rrdpl)) && notInCurrentChunk (keyValue)) {
	    targetLinkC->Append (currentValue, currentCnt, currentRepeat);
	    rrdp++;
	}

/***** If the value isn't in the link ... *****/
	if (!notNil || keyValue < currentValue) {
    /*** Not Found ***/
	    valueAdded = true;
	    location = targetLinkC->ElementCount ();
	    rtLINK_AppendRange (targetLinkC, keyValue, 1);

        /* create a ptoken constructor for the targetLinkC... */
	    targetPtokenC = rtPTOKEN_NewShiftPTConstructor (
		targetLink, rtLINK_CPx_PosPToken
	    );
	    targetPtokenC->AppendAdjustment (location, 1);

	    if (notNil)
		targetLinkC->Append (currentValue, currentCnt, currentRepeat);
	}
/***** ... otherwise, its found ... *****/
	else {
    /*** Found ***/
	    valueAdded = false;
	    location = currentOff + (keyValue - currentValue);
	    targetLinkC->Append (currentValue, currentCnt, currentRepeat);
	}

/***** Append the rest of the link if the value was added ... *****/
	if (valueAdded) {
	    while (++rrdp < rrdpl)
		targetLinkC->Append (currentValue, currentCnt, currentRepeat);

/*****  Close the ptoken for the target... *****/
	    M_CPD *newPosPtokenCPD = targetPtokenC->ToPToken ();

/***** Reconstruct targetLink from the new targetLinkC...  *****/
	    targetLinkC->Close (newPosPtokenCPD);

	    targetLink->CheckConsistency ();
	    targetLink->EnableModifications ();
	    rtLINK_CPD_IsInconsistent (targetLink) = true;
	    ReconstructLink (targetLink, targetLinkC);

	/* set the link's positional ptoken to the new one */
	    targetLink->StoreReference (rtLINK_CPx_PosPToken, newPosPtokenCPD);

	    rtLINK_CPD_IsInconsistent (targetLink)   = false;
	    newPosPtokenCPD->release ();
	    targetLinkC->release ();
	}
/***** ... otherwise, no values were added, the target link stays the same
   *****/
	else targetLinkC->release ();
    }

/***** Initialize the location reference ... *****/
    DSC_InitReferenceScalar (
	*locationRefp, rtLINK_CPD_PosPTokenCPD (targetLink), location
    );

    return valueAdded;

/***** Undefine the macros used by 'rtLINK_LocateOrAddFromRef' *****/
#undef currentValue
#undef currentOff
#undef currentCnt
#undef currentRepeat
#undef notInCurrentChunk
}


/********************************
 *****  Link Lookup Macros  *****
 ********************************/

PrivateVarDef int InARut = 10;

#define tryAnotherRRD(huntedValue)\
{\
    int limitValue;\
\
    TryAnotherRRDCount++;\
    /*** goto next chunk ***/\
    if (targetOvershot)\
    {\
	if (currentRRD == beginningRRD)\
	    done = true;\
	else\
	{\
	    shortshots = 0;\
	    endingRRD = currentRRD;\
	    limitValue = rtLINK_RRD_ReferenceOrigin (beginningRRD);\
	    if (++longshots <= InARut)\
		adjustmentAmount = (int)((currentRRD - beginningRRD) *\
		      ((currentValue - (huntedValue)) /\
		       (double)(currentValue - limitValue)));\
	    else adjustmentAmount = (currentRRD - beginningRRD)/2;\
	    currentRRD -= adjustmentAmount ? adjustmentAmount : 1;\
	}\
    }\
    else\
    {\
	longshots = 0;\
        beginningRRD = currentRRD + 1;\
	if (beginningRRD >= endingRRD ||\
	    (int) rtLINK_RRD_ReferenceOrigin (beginningRRD) >= (huntedValue))\
	{\
            done = true;\
	    currentRRD = beginningRRD;\
	}\
	else\
	{\
	    limitValue = endingRRD == finalRRD\
		? sourceReferenceNil\
		: rtLINK_RRD_ReferenceOrigin (endingRRD);\
	    if (++shortshots <= InARut)\
		adjustmentAmount = (int)((endingRRD - currentRRD) *\
		      (((huntedValue) - currentValue) /\
		       (double)(limitValue - currentValue)));\
	    else adjustmentAmount = (endingRRD - currentRRD)/2;\
	    currentRRD += adjustmentAmount ? adjustmentAmount : 1;\
	}\
    }\
    /*** set global place markers ***/\
    if (currentRRD < finalRRD)\
    {\
	currentOff = rtLINK_RRD_LinkCumulative (currentRRD);\
	currentCnt = rtLINK_RRD_LinkCumulative (currentRRD+1) -\
			  currentOff;\
	currentValue = rtLINK_RRD_ReferenceOrigin (currentRRD);\
    }\
    else currentValue = sourceReferenceNil;\
    if (TracingLookup)\
    {\
	IO_printf ("lookup: leaving tryAnotherRRD...\n");\
	IO_printf (\
	"        ir:%d, ss:%d, ls:%d, hv:%d, cv:%d, lv:%d, ad:%d, done:%d\n",\
	    InARut, shortshots, longshots, huntedValue,\
	    currentValue, limitValue, adjustmentAmount, done\
	);\
	IO_printf (\
	    "        br:%x, cr:%x, er:%x, to:%d\n",\
	    beginningRRD, currentRRD, endingRRD, targetOvershot\
	);\
    }\
}

#define backup {\
    if (TracingLookup) IO_printf (\
	"lookup: in backup: cr:%x, ir:%x, fr:%x...\n",\
	currentRRD, initialRRD, finalRRD\
    );\
    /*** goto last chunk if there is one ***/\
    if (currentRRD <= initialRRD)\
	backedupOk = false;\
    else\
    {\
        currentRRD--;\
	backedupOk = true;\
	/*** set global place markers ***/\
        if (currentRRD < finalRRD)\
	{\
	    currentOff = rtLINK_RRD_LinkCumulative (currentRRD);\
	    currentCnt = rtLINK_RRD_LinkCumulative (currentRRD+1) -\
			     currentOff;\
	    currentValue = rtLINK_RRD_ReferenceOrigin (currentRRD);\
	    if (!currentRepeat)\
	    {\
		currentOff += (currentCnt - 1);\
		currentValue += (currentCnt - 1);\
		currentCnt = 1;\
	    }\
	}\
    }\
}

#define notInCurrentChunk(value) (\
    (targetOvershot = (value) < currentValue) ||\
    (\
	currentRepeat\
	? (value) > currentValue\
        : (value) > currentValue + currentCnt - 1\
    )\
)

#define notDone		(!done)

#define notNil		(currentRRD < finalRRD)

#define currentRepeat	(rtLINK_RRD_RepeatedRef (currentRRD))

#define locate(value)\
{\
    int longshots = 0,\
        shortshots = 0;\
    LocateCount++;\
    if (TracingLookup) IO_printf ("lookup: in Locate...\n");\
    while (notNil && notInCurrentChunk(value) && notDone)\
	tryAnotherRRD(value);\
    if (!notNil)\
    {\
	if (lookupCase == rtLINK_LookupCase_LE)\
	{\
	    backup;\
	    found = (backedupOk && notNil);\
	}\
	else found = false;\
    }\
    else\
    {\
	/*** For ranges, set current to inside the range ***/\
	if ((currentValue < (value)) && !currentRepeat)\
	{\
	    currentOff +=\
		((value) - currentValue);\
	    currentCnt -=\
		((value) - currentValue);\
	    currentValue = (value);\
	}\
\
	/*** Now set found ***/\
	if (lookupCase == rtLINK_LookupCase_GE  ||\
	    lookupCase == rtLINK_LookupCase_GT\
	) found = true;\
	else if (lookupCase == rtLINK_LookupCase_EQ)\
	    found = (currentValue == (value));\
	else /* LE */\
	{\
	    if (!(found = (currentValue == (value))))\
	    {\
		backup;\
		found = (backedupOk && notNil);\
	    }\
	}\
    }\
    beginningRRD = currentRRD;\
    endingRRD = finalRRD;\
}

#define checkPartitionSize(value1, value2, found) found = (\
    (!found)\
    ? false\
    : ((value1 / partitionSize) == (value2 / partitionSize))\
)


/**********************************
 *****  Link Lookup Routines  *****
 **********************************/

/*---------------------------------------------------------------------------
 ***** Routine to lookup the values in 'keyLinkC' in the source link.
 *
 *  Arguments:
 *	sourceLink	- the link to lookup the values in.
 *	keyLinkC	- a link constructor containing the values to lookup.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *				  rtLINK_LookupCase_EQ,
 *				  rtLINK_LookupCase_GE, or
 *				  rtLINK_LookupCase_GT.
 *	partitionSize	- an integer specifying the partition size of links.
 *	locationsLinkC	- a pointer to a link constructor which this routine
 *			  will create. It will contain the locations in the
 *			  source where the looked up value was found.
 *	locatedLinkC	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found.  It will contain
 *			  the positions in 'keyLinkC' for which a value was
 *                        found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtLINK_LookupUsingLCKey (
    M_CPD*			sourceLink,
    rtLINK_CType*		keyLinkC,
    rtLINK_LookupCase		lookupCase,
    int				partitionSize,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
)
{
    rtLINK_RRDType
	*currentRRD, *beginningRRD, *endingRRD, *finalRRD, *initialRRD;
    int
	currentValue, currentCnt, currentOff, targetOvershot,
	adjustmentAmount, found, backedupOk, lookupValue, done = false,
	sourceReferenceNil;
    rtLINK_CType*
	notFoundLinkC = NilOf (rtLINK_CType*);
    M_CPD*
	newPosPtokenCPD;


#define output(value, count, repeat) {\
    if (TracingLookup) IO_printf (\
	"lookup: OUTPUT: val=%d, cnt=%d, rep=%d\n", value, count, repeat\
    );\
    (*locationsLinkC)->Append (value, count, repeat);\
}

#define outputNotFound(value, count) {\
    if (TracingLookup) IO_printf (\
	"lookup: OUTPUT-NOTFOUND: val=%d, cnt=%d\n", value, count\
    );\
    if (IsNil (notFoundLinkC))\
	notFoundLinkC = rtLINK_RefConstructor (keyLinkC->PPT (), -1);\
    rtLINK_AppendRange (notFoundLinkC, value, count);\
}

#define handleRepeat(keyPosition, keyCount, keyValue) {\
    if (TracingLookup)IO_printf (\
	"lookup: HANDLE_REPEAT: keyPos=%d, keyCnt=%d, keyVal=%d\n",\
	keyPosition, keyCount, keyValue\
    );\
    lookupValue = lookupCase == rtLINK_LookupCase_GT\
		  ? keyValue + 1\
		  : keyValue;\
    if (notNil)\
    {\
	locate (lookupValue);\
	done = false;\
	checkPartitionSize (keyValue, currentValue, found);\
    }\
    else found = false;\
    if (found)\
    {\
	output (currentOff, keyCount, true);\
    }\
    else\
    {\
	outputNotFound(keyPosition, keyCount);\
    }\
}

#define handleRange(keyPosition, keyCount, keyValue)\
{\
    if (TracingLookup) IO_printf (\
	"lookup: HANDLE_RANGE: keyPos=%d, keyCnt=%d, keyVal=%d\n",\
	keyPosition, keyCount, keyValue\
    );\
    while (keyCount > 0)\
    {\
	lookupValue = lookupCase == rtLINK_LookupCase_GT\
		      ? keyValue + 1\
		      : keyValue;\
	if (notNil)\
	{\
	    locate (lookupValue);\
	    done = false;\
	    checkPartitionSize (keyValue, currentValue, found);\
	}\
	else\
	{\
	    outputNotFound (keyPosition, keyCount);\
	    break;\
	}\
	if (found)\
	{\
	    output (currentOff, 1, false);\
	}\
	else\
	{\
	    outputNotFound(keyPosition, 1);\
	}\
	keyCount--;\
	keyValue++;\
	keyPosition++;\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtLINK_LookupUsingLCKey'
 *---------------------------------------------------------------------------
 */

    LookupUsingLCKeyCount++;

    if (TracingLookup)
	IO_printf ("Entering rtLINK_LookupUsingLCKey...\n");

/*****
 * Align 'sourceLink' and 'keyLinkC' and validate the applicability of
 * 'keyLinkC' as an extraction index for 'sourceLink'...
 *****/
    rtLINK_AlignLink (sourceLink);
    keyLinkC->AlignForAdd (sourceLink, rtLINK_CPx_RefPToken);

/*****  Initialize the source traversal state variables... *****/
    sourceReferenceNil = rtPTOKEN_BaseElementCount (
	sourceLink, rtLINK_CPx_RefPToken
    );
    *locatedLinkC = NilOf (rtLINK_CType*);
    currentRRD = beginningRRD = initialRRD = rtLINK_CPD_RRDArray (sourceLink);
    endingRRD = finalRRD = (rtLINK_CPD_RRDCount (sourceLink) + initialRRD);

    if (currentRRD < finalRRD)
    {
	currentOff = rtLINK_RRD_LinkCumulative (currentRRD);
	currentCnt = rtLINK_RRD_LinkCumulative (currentRRD+1) - currentOff;
	currentValue = rtLINK_RRD_ReferenceOrigin (currentRRD);
    }
    else done = true;

/*****  Create the locations link constructor...  *****/
    *locationsLinkC = rtLINK_RefConstructor (sourceLink, rtLINK_CPx_PosPToken);

/***** Traverse the two links...  *****/
    rtLINK_TraverseRRDCList (keyLinkC,handleRepeat,handleRepeat,handleRange);

/*****  Done with the traversal  *****/
/*****  Close notFoundLinkC and complement it into locatedLinkC,
        if neccessary...  *****/
    if (IsntNil (notFoundLinkC))
    {
        /*** close the linkc ... ***/
	newPosPtokenCPD	= rtPTOKEN_New (
	    keyLinkC->PPT ()->Space (), notFoundLinkC->ElementCount ()
	);
	notFoundLinkC->Close (newPosPtokenCPD);
	newPosPtokenCPD->release ();

	/*** complement it ... ***/
	*locatedLinkC = notFoundLinkC->Complement ();
	notFoundLinkC->release ();
    }

/*****  Close locations ... *****/
    (*locationsLinkC)->Close ((*locatedLinkC ? *locatedLinkC : keyLinkC)->PPT ());

/*****  ...and return  *****/
    if (TracingLookup)
	IO_printf ("exiting rtLINK_LookupUsingLCKey\n");
    return;

/*****  Delete 'rtLINK_LookupUsingLCKey's macros ... *****/
#undef output
#undef outputNotFound
#undef handleRepeat
#undef handleRange
}


/*---------------------------------------------------------------------------
 ***** Routine to lookup the value in 'keyRefp' in the source link.
 *
 *  Arguments:
 *	sourceLink	- the link to lookup the value in.
 *	keyRefp 	- a pointer to a reference containing the value to
 *                        lookup.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *				  rtLINK_LookupCase_EQ,
 *				  rtLINK_LookupCase_GE, or
 *				  rtLINK_LookupCase_GT.
 *	partitionSize	- an integer containing the partition size of the link.
 *	locationRefp 	- a pointer to a reference which this routine
 *			  will create. It will contain the location in the
 *			  source where the looked up value was found.  If
 *                        the value was not found 'locationRefp' will be
 *                        untouched.
 *
 *  Returns:
 *	true if the value was found, false otherwise.
 *
 *****/
PublicFnDef bool rtLINK_LookupUsingRefKey (
    M_CPD*			sourceLink,
    rtREFUV_TypePTR_Reference	keyRefp,
    rtLINK_LookupCase		lookupCase,
    int				partitionSize,
    rtREFUV_TypePTR_Reference	locationRefp
)
{
    rtLINK_RRDType
	*currentRRD, *finalRRD, *endingRRD, *beginningRRD, *initialRRD;
    int
	currentValue, currentCnt, currentOff, done = false,
	adjustmentAmount, found, backedupOk, keyValue, targetOvershot,
	sourceReferenceNil;


/*
 *---------------------------------------------------------------------------
 *   Cody Body: 'rtLINK_LookupUsingRefKey'
 *---------------------------------------------------------------------------
 */
    LookupUsingRefKeyCount++;

/***** Align and validate the source and target ... *****/
    rtLINK_AlignLink (sourceLink);
    rtREFUV_AlignAndValidateRef (keyRefp, sourceLink, rtLINK_CPx_RefPToken);

/*****  Initialize the source traversal state variables... *****/
    sourceReferenceNil = rtPTOKEN_BaseElementCount (
	sourceLink, rtLINK_CPx_RefPToken
    );
    currentRRD = beginningRRD = initialRRD = rtLINK_CPD_RRDArray (sourceLink);
    endingRRD = finalRRD = (rtLINK_CPD_RRDCount (sourceLink) + initialRRD);

    if (currentRRD < finalRRD)
    {
	currentOff = rtLINK_RRD_LinkCumulative (currentRRD);
	currentCnt = rtLINK_RRD_LinkCumulative (currentRRD+1) - currentOff;
	currentValue = rtLINK_RRD_ReferenceOrigin (currentRRD);
    }
    else done = true;

/***** Lookup ... *****/
    keyValue = (int)rtREFUV_Ref_Element (keyRefp);
    if (lookupCase == rtLINK_LookupCase_GT)
        locate (keyValue + 1)
    else locate (keyValue);
    checkPartitionSize (keyValue, currentValue, found);
    if (found) {
	DSC_InitReferenceScalar (
	    *locationRefp, rtLINK_CPD_PosPTokenCPD (sourceLink), currentOff
	);
	return true;
    }
    else return false;	 /* not found */

}
/***** Undefine the macros used by 'rLINK_LookupUsing*Key' *****/
#undef tryAnotherRRD
#undef backup
#undef currentRepeat
#undef notInCurrentChunk
#undef notDone
#undef notNil
#undef locate
#undef checkPartitionSize


/************************************
 *****  Link Deletion Routines  *****
 ************************************/

/*---------------------------------------------------------------------------
 ***** Routine to delete the values in 'elementLinkC' from the source link.
 *
 *  Arguments:
 *	sourceLink	- the link to delete the values from.
 *	elementLinkC	- a link constructor containing the values to delete.
 *	deletedLinkC	- an optional address ('Nil' if absent) which,
 *			  when specified, will be set to a link contructor
 *                        identifying the positions in 'elementLinkC' of the
 *                        elements deleted from 'sourceLink'.  If no elements
 *	                  were deleted an empty link constructor is produced.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtLINK_DeleteLCSelectedElements (
    M_CPD *sourceLink, rtLINK_CType *elementLinkC, rtLINK_CType **deletedLinkC
) {
    rtPTOKEN_CType *sourcePTokenC = NilOf (rtPTOKEN_CType *);
    rtLINK_RRDType
	*rrdp, *rrdpl;
    int
	currentOff, currentCnt, currentValue, currentRepeat,
	adjustment;
    rtLINK_CType*
	deletions;
    M_CPD*
	ptoken;


/*****  Macros ... *****/

#define advance {\
    if (TracingLCAppending) IO_printf (\
	"rtLINK_DeleteLCSpecifiedElements: in advance...\n"\
    );\
    /*** goto next chunk ***/\
    rrdp++;\
    /*** set global place markers ***/\
    if (rrdp < rrdpl)\
    {\
	currentOff = rtLINK_RRD_LinkCumulative (rrdp);\
	currentCnt = rtLINK_RRD_LinkCumulative (rrdp+1) -\
			  rtLINK_RRD_LinkCumulative (rrdp);\
	currentValue = rtLINK_RRD_ReferenceOrigin (rrdp);\
	currentRepeat = rtLINK_RRD_RepeatedRef (rrdp);\
    }\
    else currentOff = -999;\
}

#define notInCurrentChunk(value) (\
    currentRepeat\
    ? (value) > currentValue\
    : (value) > currentValue + currentCnt - 1\
)

#define notNil	(currentOff != -999)

#define locate(value)\
{\
    if (TracingLCAppending) IO_printf (\
	"rtLINK_DeleteLCSpecifiedElements: in Locate(%d)...\n", value\
    );\
    while (notInCurrentChunk(value) && notNil)\
	advance;\
    if (notNil && (currentValue < (value)) && !currentRepeat)\
    {\
	currentOff = currentOff + ((value) - currentValue);\
	currentCnt = currentCnt - ((value) - currentValue);\
	currentValue = (value);\
    }\
}

#define min(a, b)	(((a) < (b)) ? (a) : (b))

#define expunge(pos, count) {\
    /*****  Create a ptoken constructor for the source link ... *****/\
    if (IsNil (sourcePTokenC)) sourcePTokenC = rtPTOKEN_NewShiftPTConstructor(\
	sourceLink, rtLINK_CPx_PosPToken\
    );\
\
    sourcePTokenC->AppendAdjustment (currentOff + (count) + adjustment, -(count));\
    if (wantsDeletions)\
	rtLINK_AppendRange (deletions, pos, count);\
\
    if (TracingLCAppending) {\
	IO_printf (\
	    "rtLINK_DeleteLCSpecifiedElements: In Delete Pos=%d, Cnt=%d\n",\
	    pos, count\
	);\
	IO_printf (\
	    "    AppendAdjustment: Pos=%d, Shift=%d\n",\
	    currentOff + (count) + adjustment, -(count)\
	);\
	IO_printf ("    AppendLCRange: Pos=%d, Cnt=%d\n", pos, count);\
    }\
\
    adjustment -= count;\
}

#define handleRepeat(pos, count, value) {\
    if (TracingLCAppending) IO_printf (\
"rtLINK_DeleteLCSpecifiedElements: In HandleRepeat Pos=%d, Cnt=%d,  Val=%d\n",\
	pos, count, value\
    );\
\
    locate (value);\
    if (value == currentValue) {\
	/***** delete as many of them as there are  *****/\
	count = min (count, (currentRepeat) ? currentCnt : 1);\
	expunge (pos, count);\
    }\
}

#define handleRange(pos, count, value) {\
    if (TracingLCAppending) IO_printf (\
"rtLINK_DeleteLCSpecifiedElements: In HandleRange Pos=%d, Cnt=%d,  Val=%d\n",\
	pos, count, value\
    );\
\
    while (count-- > 0) {\
	locate (value);\
	if (value == currentValue)\
	    expunge (pos, 1);\
	value++;\
	pos++;\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLINK_DeleteLCSelectedElements'
 *---------------------------------------------------------------------------
 */

    DeleteLCElementsCount++;

/*****
 * Align 'sourceLink' and 'elementLinkC' and validate 'elementLinkC'
 * as an extraction index for 'sourceLink' ...
 *****/
    rtLINK_AlignLink (sourceLink);
    elementLinkC->AlignForAdd (sourceLink, rtLINK_CPx_RefPToken);

/*****  Create deletedLinkC if neccessary ... *****/
    bool wantsDeletions = (int)(IsntNil (deletedLinkC));
    if (wantsDeletions)
	deletions = rtLINK_RefConstructor (elementLinkC->PPT (), -1);

/*****  Special case the empty cases ... *****/
    if (elementLinkC->BaseElementCount () == 0 ||
	rtPTOKEN_BaseElementCount (sourceLink, rtLINK_CPx_PosPToken) == 0
    ) {
	if (wantsDeletions) {
	    ptoken = rtPTOKEN_New (elementLinkC->PPT ()->Space (), 0);
	    deletions->Close (ptoken);
	    ptoken->release ();
	    *deletedLinkC = deletions;
	}

	return;
    }

/*****  Initialize the source traversal variables ... *****/
    adjustment = 0;

    rrdp = rtLINK_CPD_RRDArray (sourceLink);
    rrdpl = (rtLINK_CPD_RRDCount (sourceLink) + rrdp);

    if (rrdp < rrdpl) {
	currentOff = rtLINK_RRD_LinkCumulative (rrdp);
	currentCnt = rtLINK_RRD_LinkCumulative (rrdp+1) -
			  rtLINK_RRD_LinkCumulative (rrdp);
	currentValue = rtLINK_RRD_ReferenceOrigin (rrdp);
	currentRepeat = rtLINK_RRD_RepeatedRef (rrdp);
    }
    else currentOff = -999;

/*****  Do the traversal ... *****/
    rtLINK_TraverseRRDCList (
	elementLinkC,
	handleRepeat,
	handleRepeat,
	handleRange
    );

/*****  Close the source ptoken constructor ... *****/
    if (IsntNil (sourcePTokenC)) {
	ptoken = sourcePTokenC->ToPToken ();
	ptoken->release ();
    }

/*****  Close deletions if neccessary ... *****/
    if (wantsDeletions) {
	ptoken = rtPTOKEN_New (elementLinkC->PPT ()->Space (), deletions->ElementCount ());
	deletions->Close (ptoken);
	ptoken->release ();
	*deletedLinkC = deletions;
    }

    return;

/*****  undef the macros ... *****/
#undef advance
#undef notInCurrentChunk
#undef notNil
#undef locate
#undef min
#undef expunge
#undef handleRepeat
#undef handleRange
}


/*---------------------------------------------------------------------------
 ***** Routine to delete the value in 'elementRefp' from the source link.
 *
 *  Arguments:
 *	sourceLink	- the link to delete the value from.
 *	elementRefp 	- a pointer to a reference containing the value to
 *                        delete.
 *
 *  Returns:
 *	true if the value was deleted, false otherwise.
 *
 *****/
PublicFnDef bool rtLINK_DeleteRefSelectedElement (
    M_CPD *sourceLink, rtREFUV_TypePTR_Reference elementRefp
) {
    int currentOff, currentCnt, currentValue, currentRepeat, deleteValue;
    bool result = false;
    rtLINK_RRDType *rrdp, *rrdpl;
    M_CPD *ptoken;


/*****  Macros ... *****/
#define advance {\
    if (TracingLCAppending) IO_printf (\
	"rtLINK_DeleteRefSpecifiedElement: in advance...\n"\
    );\
    /*** goto next chunk ***/\
    rrdp++;\
    /*** set global place markers ***/\
    if (rrdp < rrdpl)\
    {\
	currentOff = rtLINK_RRD_LinkCumulative (rrdp);\
	currentCnt = rtLINK_RRD_LinkCumulative (rrdp+1) -\
			  rtLINK_RRD_LinkCumulative (rrdp);\
	currentValue = rtLINK_RRD_ReferenceOrigin (rrdp);\
	currentRepeat = rtLINK_RRD_RepeatedRef (rrdp);\
    }\
    else currentOff = -999;\
}

#define notInCurrentChunk(value) (\
    (currentRepeat)\
    ? ((value) > currentValue)\
    : ((value) > currentValue + currentCnt - 1)\
)

#define notNil	(currentOff != -999)

#define locate(value)\
{\
    if (TracingLCAppending) IO_printf (\
	"rtLINK_DeleteRefSpecifiedElement: in Locate(%d)...\n", value\
    );\
    while (notInCurrentChunk(value) && notNil)\
	advance;\
    if (notNil && (currentValue < (value)) && !currentRepeat)\
    {\
	currentOff = currentOff + ((value) - currentValue);\
	currentCnt = currentCnt - ((value) - currentValue);\
	currentValue = (value);\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLINK_DeleteRefSelectedElement'
 *---------------------------------------------------------------------------
 */

    DeleteRefElementCount++;

/***** Align and validate 'sourceLink' and 'elementRefp' ... *****/
    rtLINK_AlignLink (sourceLink);
    rtREFUV_AlignAndValidateRef(elementRefp, sourceLink, rtLINK_CPx_RefPToken);

/*****  Look for 'deleteValue' in the source link ...  *****/
    deleteValue = (int)rtREFUV_Ref_Element (elementRefp);

    rrdp = rtLINK_CPD_RRDArray (sourceLink);
    rrdpl = (rtLINK_CPD_RRDCount (sourceLink) + rrdp);

    if (rrdp < rrdpl) {
	currentOff = rtLINK_RRD_LinkCumulative (rrdp);
	currentCnt = rtLINK_RRD_LinkCumulative (rrdp+1) -
			  rtLINK_RRD_LinkCumulative (rrdp);
	currentValue = rtLINK_RRD_ReferenceOrigin (rrdp);
	currentRepeat = rtLINK_RRD_RepeatedRef (rrdp);
    }
    else return result;

    locate (deleteValue);

/*****  If found, delete it ... *****/
    if (currentValue == deleteValue) {
	rtPTOKEN_CType *sourcePTokenC = rtPTOKEN_NewShiftPTConstructor (
	    sourceLink, rtLINK_CPx_PosPToken
	);
	sourcePTokenC->AppendAdjustment (currentOff + 1, -1);
	ptoken = sourcePTokenC->ToPToken ();
	ptoken->release ();
	result = true;
    }

    return result;

/***** Undefine the macros ... *****/
#undef advance
#undef notInCurrentChunk
#undef notNil
#undef locate
}


/*********************************
 *****  Misc. Link Routines  *****
 *********************************/

/*---------------------------------------------------------------------------
 * Routine to distribute the values in a source link into a different
 * positional state.
 *
 * Arguments:
 *	referenceLC	- a link constructor whose reference PToken defines
 * 			  the positional state into which to distibute the
 * 			  source and whose values define the distribution.
 *	sourceLC	- a link constructor which is the source of the values
 * 			  to be distributed.
 *
 * Returns:
 *	A reference uvector which contains the distributed version of the
 * source.
 *---------------------------------------------------------------------------
 */
PublicFnDef M_CPD *rtLINK_Distribute (rtLINK_CType *referenceLC, rtLINK_CType *sourceLC) {
    rtREFUV_ElementType	sourceValue;
    bool rrdcAdvanced = true;
    int currentPosition = -1;

#define advanceSource(amount) {\
    currentPosition += amount;\
\
    while (\
	(unsigned int)currentPosition >= rtLINK_RRDC_LinkCumulative (\
	    nextRRDC = rtLINK_RRDC_NextRRDC (sourceRRDC)\
	)\
    ) {\
        sourceRRDC = nextRRDC;\
	rrdcAdvanced = true;\
    }\
    if (rrdcAdvanced && rtLINK_RRDC_RepeatedRef (sourceRRDC))\
        sourceValue = rtLINK_RRDC_ReferenceOrigin (sourceRRDC);\
    else if (rrdcAdvanced)\
	sourceValue = rtLINK_RRDC_ReferenceOrigin (sourceRRDC)\
		    + currentPosition\
		    - rtLINK_RRDC_LinkCumulative (sourceRRDC);\
    else if (!rtLINK_RRDC_RepeatedRef (sourceRRDC))\
	sourceValue++;\
    rrdcAdvanced = false;\
}

/*---------------------------------------------------------------------------
 * In the case of a repeat in referenceLC, we are supposedly putting count
 * values into one spot in the result. As only the final assignment remains
 * visible at the end, Handle Repeat only does the final assignment.
 *---------------------------------------------------------------------------
 */
#define HandleRepeat(pos, count, ref) {\
    advanceSource (count);\
    resultArray[ref] = sourceValue;\
}

#define HandleRange(pos, count, ref) {\
    while (count--) {\
	advanceSource (1);\
	resultArray[ref++] = sourceValue;\
    }\
}
/*---------------------------------------------------------------------------
 * 			Code Body For rtLINK_Distribute			    *
 *---------------------------------------------------------------------------
 */

/*****  Align and Validate the arguments  *****/
    sourceLC->Align ();
    referenceLC->AlignForDistribute (sourceLC->PPT (), -1);

/*****  Create the result  *****/
    M_CPD *resultuvCPD = rtREFUV_New (referenceLC->RPT (), sourceLC->RPT ());

/*****  Fill in the result  *****/
    rtREFUV_ElementType *resultArray = rtREFUV_CPD_Array (resultuvCPD);
    rtLINK_RRDCType *nextRRDC, *sourceRRDC = rtLINK_LC_ChainHead (sourceLC);
    rtLINK_TraverseRRDCList (
	referenceLC, HandleRepeat, HandleRepeat, HandleRange
    );

    return resultuvCPD;

#undef advanceSource
#undef HandleRepeat
#undef HandleRange
}


/*---------------------------------------------------------------------------
 *****  Routine to create an integer uvector containing the count of the
 *****  number of references to each position contained in 'linkc'.
 *
 *  Arguments:
 *	linkc			- the address of the link constructor whose
 *				  references are to be counted.
 *	refPToken		- a standard CPD for the new reference
 *				  ptoken for the result integer uvector.
 *	distributionCPD		- optional (Nil if ommitted).  A standard CPD
 *				  specifying the distribution reference
 *                                uvector.  If specified, instead of returning
 *                                the counts in regular order, they will be
 *                                distributed first.
 *
 *  Returns:
 *	An integer uvector containing the reference counts.
 *
 *****/
PublicFnDef M_CPD *rtLINK_LCCountReferences (
    rtLINK_CType *linkc, M_CPD *refPToken, M_CPD *distributionCPD
) {
/*****  Traverse the linkc counting references  *****/
#define offset	((distribute) ? distribp [resultPos++] : resultPos++)

#define nullFillTo(position) {\
    while (resultPos < position) resultp [offset] = 0;\
}

#define handleRepeat(position, count, value) {\
    nullFillTo ((unsigned int)value);\
    resultp [offset] = count;\
}

#define handleRange(position, count, value) {\
    nullFillTo ((unsigned int)value);\
    while (count > 0) {\
	resultp [offset] = 1;\
	count--;\
    }\
}

#define handleNil(position, count, value) {\
    nullFillTo ((unsigned int)value);\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLINK_LCCountReferences'
 *---------------------------------------------------------------------------
 */

    LCCountReferencesCount++;

/***** Align the arguments and set distribute ... *****/
    bool distribute = IsntNil (distributionCPD);
    rtREFUV_ElementType	*distribp;
    if (distribute) {
	rtREFUV_Align (distributionCPD);
	linkc->AlignForExtract (distributionCPD, UV_CPx_PToken);
	distribp = rtREFUV_CPD_Array (distributionCPD);
    }
    else linkc->Align ();

/*****  Create the result integer uvector ... *****/
    M_CPD *resultCPD = distribute
	? rtINTUV_New (distributionCPD, UV_CPx_RefPToken, refPToken, -1)
	: rtINTUV_New (linkc->RPT (), refPToken);

/***** Setup for the traversal ... *****/
    rtINTUV_ElementType	*resultp = rtINTUV_CPD_Array (resultCPD);
    unsigned int resultPos = 0;

/***** Do the traversal ... *****/
    rtLINK_TraverseRRDCList (linkc, handleNil, handleRepeat, handleRange);

/***** Fill in any unused slots with nulls ... *****/
    nullFillTo (linkc->ReferenceNil ());

/***** Return the result ... *****/
    return resultCPD;

/***** undef the macros ... *****/
#undef offset
#undef nullFillTo
#undef handleRepeat
#undef handleRange
#undef handleNil
}


/**************************************
 *****  Link Projection Routines  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to determine the columns of the cartesian elements in
 *****  'cartesianLinkC'.
 *
 *  Arguments:
 *	cartesianLinkC	- a link constructor containing cartesian elements.
 *
 *  Returns:
 *	A standard CPD for a reference U-Vector which will contain the
 *	column numbers for each of the elements in 'cartesianLinkC'.
 *
 *****/
PublicFnDef M_CPD *rtLINK_ColumnProjection (rtLINK_CType *cartesianLinkC) {
#define projectionRefNil numberOfColumns

#define handleNil(pos, count, value) {\
    while (count-- > 0)\
	*resultp++ = projectionRefNil;\
}

#define handleRepeat(pos, count, value) {\
    while (count-- > 0)\
	*resultp++ = value % numberOfColumns;\
}

#define handleRange(pos, count, value) {\
    while (count-- > 0)\
	*resultp++ = value++ % numberOfColumns;\
}

/*****  Bump the routine count ... *****/
    ColumnProjectionCount++;

/*****  Align and validate the cartesian linkc ... *****/
    cartesianLinkC->Align ();
    if (!rtPTOKEN_CPD_IsCartesian (cartesianLinkC->RPT ())) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_ColumnProjection: Not a cartesian link constructor"
    );

/*****  Create the new reference uvector ... *****/
    M_CPD *result = rtREFUV_New (
	cartesianLinkC->PPT (), cartesianLinkC->RPT (), rtPTOKEN_CPx_ColPToken
    );

/*****  Fill the reference uvector with the column projections ... *****/
    int const numberOfColumns = rtPTOKEN_BaseElementCount (result, UV_CPx_RefPToken);
    rtREFUV_ElementType *resultp = rtREFUV_CPD_Array (result);

    rtLINK_TraverseRRDCList(cartesianLinkC,handleNil,handleRepeat,handleRange);

    return result;

/***** undef the macros ... *****/
#undef handleNil
#undef handleRepeat
#undef handleRange
#undef projectionRefNil
}


/*---------------------------------------------------------------------------
 *****  Routine to determine the rows of the cartesian elements in
 *****  'cartesianLinkC'.
 *
 *  Arguments:
 *	cartesianLinkC	- a link constructor containing cartesian elements.
 *
 *  Returns:
 *	A link constructor which will contain the row numbers for each of
 *      the elements in 'cartesianLinkC'.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_RowProjection (rtLINK_CType *cartesianLinkC) {
#define handleNil(pos, count, value) {\
    rtLINK_AppendRepeat (result, projectionRefNil, count);\
}

#define handleRepeat(pos, count, value) {\
    rtLINK_AppendRepeat (result, (int)(value / numberOfColumns), count);\
}

#define handleRange(pos, count, value) {\
    while (count-- > 0)\
	rtLINK_AppendRange (result, (int)(value++ / numberOfColumns), 1);\
}

/*****  Bump the routine count ... *****/
    RowProjectionCount++;

/*****  Align and validate the cartesian linkc ... *****/
    cartesianLinkC->Align ();
    if (!rtPTOKEN_CPD_IsCartesian (cartesianLinkC->RPT ())) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLINK_ColumnProjection: Not a cartesian link constructor"
    );

/*****  Create the new link constructor ... *****/
    rtLINK_CType *result = rtLINK_RefConstructor (
	cartesianLinkC->RPT (), rtPTOKEN_CPx_RowPToken
    );

/*****  Append the row projections to the linkc ... *****/
    int const numberOfColumns = rtPTOKEN_BaseElementCount (
	cartesianLinkC->RPT (), rtPTOKEN_CPx_ColPToken
    );
    int const projectionRefNil = rtPTOKEN_CPD_BaseElementCount (result->RPT ());

    rtLINK_TraverseRRDCList(cartesianLinkC,handleNil,handleRepeat,handleRange);

/*****  Close the result link constructor ... *****/
    result->Close (cartesianLinkC->PPT ());

    return result;

/***** undef the macros ... *****/
#undef handleNil
#undef handleRepeat
#undef handleRange
}


/*****************************************
 *****  Link Linearization Routines  *****
 *****************************************/

/***********************************************
 *  Link Constructor Row  -  Reference Column  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Calculate the cartesian linear locations given the rows and columns.
 *
 *  Arguments:
 *	cartesianPT	- a standard CPD for the cartesian PToken whose row
 *                        PToken must correspond referentially to 'rowLinkC'
 *			  and whose column PToken must correspond referentially
 *                        to 'columnRefp'.
 *	rowLinkC	- a link constructor whose elements provide the rows.
 *	columnRefp	- a pointer to a reference whose element is the column.
 *
 *  Returns:
 *	A link constructor containing the linearized locations.
 *
 *****/
PublicFnDef rtLINK_CType *rtLINK_LinearizeLCrRc (
    M_CPD *cartesianPT, rtLINK_CType *rowLinkC, rtREFUV_TypePTR_Reference columnRefp
) {
#define handleNil(pos, count, value) {\
    rtLINK_AppendRepeat (result, refNil, count);\
}

#define handleRepeat(pos, count, value) {\
    rtLINK_AppendRepeat (result, value * numberOfColumns + column, count);\
}

#define handleRange(pos, count, value) {\
    while (count-- > 0)\
	rtLINK_AppendRange (result, value++ * numberOfColumns + column, 1);\
}

    LinearizeLCrRcCount++;

/*****  Align and Validate 'rowLinkC' and 'columnRefp'  *****/
    rowLinkC->Align ();
    rtREFUV_AlignReference (columnRefp);
    rtPTOKEN_CartesianVerification (
	cartesianPT, rowLinkC->RPT (), rtREFUV_Ref_RefPTokenCPD (columnRefp), -1
    );

/*****  Make the result link constructor ... *****/
    rtLINK_CType *result = rtLINK_PosConstructor (rowLinkC->PPT (), -1);

/*****  Traverse the 'rowLinkC' calculating the linearizations ... *****/
    int const resultSize = rtPTOKEN_CPD_BaseElementCount (rowLinkC->PPT ());
    int const refNil = rtPTOKEN_CPD_BaseElementCount (cartesianPT);
    int const numberOfColumns = rtPTOKEN_CPD_BaseElementCount (
	rtREFUV_Ref_RefPTokenCPD (columnRefp)
    );
    int column = rtREFUV_Ref_Element (columnRefp);

    if (column == numberOfColumns)
	rtLINK_AppendRepeat (result, refNil, resultSize);
    else {
	rtLINK_TraverseRRDCList (rowLinkC,handleNil,handleRepeat,handleRange);
    }

/*****  Close the result linkc ... *****/
    result->Close (cartesianPT);

    return result;

/*****  undef the macros ... *****/
#undef handleNil
#undef handleRepeat
#undef handleRange
}


/********************************************************************
 *  Link Constructor Row  -  Reference U-Vector Indirection Column  *
 ********************************************************************/

/*---------------------------------------------------------------------------
 *****  Calculate the cartesian linear locations given the rows and columns.
 *
 *  Arguments:
 *	cartesianPT	    - a standard CPD for the cartesian PToken whose row
 *                            PToken must correspond referentially to
 *                            'rowLinkC' and whose column PToken must
 *                            correspond referentially to 'columnUV'.
 *	rowLinkC	    - a link constructor whose elements provide the
 *                            rows.
 *	columnUV	    - a standard CPD for a reference UVector whose
 *                            elements provide the columns.
 *	columnIndirection   - a standard CPD for a reference UVector used to
 *			      index into 'columnUV' for the specified
 *                            columns.
 *
 *  Returns:
 *	A standard CPD for a reference UVector containing the linearized
 *      locations.
 *
 *****/
PublicFnDef M_CPD *rtLINK_LinearizeLCrUVic (
    M_CPD*			cartesianPT,
    rtLINK_CType*		rowLinkC,
    M_CPD*			columnUV,
    M_CPD*			columnIndirection
)
{
#define handleNil(pos, count, value) {\
    while (count-- > 0)	*resultp++ = refNil;\
}

#define handleRepeat(pos, count, value) {\
    int column, columni;\
\
    while (count-- > 0) {\
	column = (int)rtREFUV_CPD_Array (columnUV)[columni = *colp++];\
	if (column == numberOfColumns || columni == colpRefNil)\
	    *resultp++ = refNil;\
	else *resultp++ = value * numberOfColumns + column;\
    }\
}

#define handleRange(pos, count, value) {\
    int column, columni;\
\
    for (;count-- > 0; value++) {\
	column = (int)rtREFUV_CPD_Array (columnUV)[columni = *colp++];\
	if (column == numberOfColumns || columni == colpRefNil)\
	    *resultp++ = refNil;\
	else *resultp++ = value * numberOfColumns + column;\
    }\
}

    LinearizeLCrUVicCount++;

/*****  Align and Validate 'rowLinkC' and 'columnLinkC'  *****/
    rowLinkC->Align ();
    rtREFUV_Align (columnUV);
    rtPTOKEN_CartesianVerification (
	cartesianPT, rowLinkC->RPT (), columnUV, UV_CPx_RefPToken
    );

/******  Align and Validate 'columnIndirection'  *****/
    rtREFUV_AlignForAssign (
	columnUV, UV_CPx_PToken, columnIndirection, rowLinkC->PPT (), -1
    );

/*****  Make the result reference UVector ... *****/
    M_CPD *result = rtREFUV_New (rowLinkC->PPT (), cartesianPT);

/*****  Calculate the linearizations ... *****/
    int const refNil = rtPTOKEN_CPD_BaseElementCount (cartesianPT);
    int const numberOfColumns = rtPTOKEN_BaseElementCount (columnUV, UV_CPx_RefPToken);
    int const colpRefNil = rtPTOKEN_BaseElementCount (columnIndirection, UV_CPx_RefPToken);
    rtREFUV_ElementType *colp = rtREFUV_CPD_Array (columnIndirection);
    rtREFUV_ElementType *resultp = rtREFUV_CPD_Array (result);

    rtLINK_TraverseRRDCList (rowLinkC, handleNil, handleRepeat, handleRange);

    return result;

/*****  undef the macros ... *****/
#undef handleNil
#undef handleRepeat
#undef handleRange
}


/******************************************************************
 *  Link Constructor Row  -  Link Constructor Indirection Column  *
 ******************************************************************/

/*---------------------------------------------------------------------------
 *****  Calculate the cartesian linear locations given the rows and columns.
 *
 *  Arguments:
 *	cartesianPT	    - a standard CPD for the cartesian PToken whose row
 *                            PToken must correspond referentially to
 *                            'rowLinkC' and whose column PToken must
 *                            correspond referentially to 'columnLinkC'.
 *	rowLinkC	    - a link constructor whose elements provide the
 *                            rows.
 *	columnLinkC	    - a link constructor whose elements provide the
 *	                      columns.
 *	columnIndirection   - a standard CPD for a reference UVector used to
 *			      index into 'columnLinkC' for the specified
 *                            columns.
 *
 *  Returns:
 *	A standard CPD for a reference UVector containing the linearized
 *      locations.
 *
 *****/
PublicFnDef M_CPD *rtLINK_LinearizeLCrLCic (
    M_CPD*			cartesianPT,
    rtLINK_CType*		rowLinkC,
    rtLINK_CType*		columnLinkC,
    M_CPD*			columnIndirection
)
{
    LinearizeLCrLCicCount++;

    M_CPD *columnUV = columnLinkC->ToRefUV ();
    M_CPD *result = rtLINK_LinearizeLCrUVic (
	cartesianPT, rowLinkC, columnUV, columnIndirection
    );
    columnUV->release ();

    return result;
}


/************************************************************
 *****  Standard Representation Type Services Routines  *****
 ************************************************************/

/*************
 *  Printer  *
 *************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a link.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the link to be printed.
 *	recursive		- a boolean which, when true, instructs this
 *				  routine to display the link's associated
 *				  P-Tokens.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintLink (M_CPD *cpd, int recursive) {
    rtLINK_RRDType *rrdp, *rrdl;

    rtLINK_AlignLink (cpd);

    IO_printf ("%s{", RTYPE_TypeIdAsString (RTYPE_C_Link));
    if (recursive) {
        RTYPE_Print (cpd, rtLINK_CPx_PosPToken);
	IO_printf (" ");
	RTYPE_Print (cpd, rtLINK_CPx_RefPToken);
    }

    for (rrdl = rtLINK_CPD_RRDCount (cpd) + (rrdp = rtLINK_CPD_RRDArray (cpd));
	 rrdp < rrdl;
	 rrdp++
    ) IO_printf (
	" %d:%d%c%d",
	rtLINK_RRD_LinkCumulative (rrdp),
	rtLINK_RRD_ReferenceOrigin (rrdp),
	rtLINK_RRD_RepeatedRef (rrdp) ? '*' : '+',
	rtLINK_L_N (rrdp)
    );
    IO_printf ("}");
}


/***************************
 *  Container Reclamation  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PrivateFnDef void ReclaimContainer (
    M_ASD *ownerASD, M_CPreamble const *preambleAddress
) {
    rtLINK_Type	const *link = (rtLINK_Type const *)(preambleAddress + 1);

    ownerASD->Release (&rtLINK_L_PosPToken (link));
    ownerASD->Release (&rtLINK_L_RefPToken (link));
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'save' a P-Token
 *****/
bool rtLINK_Handle::PersistReferences () {
    rtLINK_Type	*link = (rtLINK_Type *)ContainerContent ();

    return Persist (&rtLINK_L_PosPToken (link))
	&& Persist (&rtLINK_L_RefPToken (link));
}


/*******************************************
 *****  Standard Link Marking Routine  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'mark' containers referenced by a Link
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void MarkContainers (M_ASD *pSpace, M_CPreamble const *pContainer) {
    pSpace->Mark ((M_POP const*)(pContainer + 1), 2);
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewReferencingDM) {
    rtLINK_CType *linkc = rtLINK_RefConstructor (RTYPE_QRegisterCPD (parameterArray[1]), -1);

    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
	M_CPD *posPToken = rtPTOKEN_New (IOBJ_ScratchPad, 1);
        rtLINK_AppendRange (linkc, IOBJ_IObjectValueAsInt (parameterArray[0]), 1)->Close (
	    posPToken
	);
	posPToken->release ();
    }
    else
	linkc->Close (RTYPE_QRegisterCPD (parameterArray[0]));

    return RTYPE_QRegister (linkc);
}

IOBJ_DefineNewaryMethod (NewPosRefInitDM) {
/***** Method to make a new link with the ptokens specified and initialize the
   values to a range starting at the number specified by 'init:'. *****/
    int			refNil, size, cnt, start;
    M_CPD*		posPToken = NilOf (M_CPD*);
    rtLINK_CType*	linkc = rtLINK_RefConstructor (
	RTYPE_QRegisterCPD (parameterArray[1]), -1
    );

    start = IOBJ_IObjectValueAsInt (parameterArray[2]);
    posPToken = RTYPE_QRegisterCPD (parameterArray[0]);
    refNil = rtPTOKEN_CPD_BaseElementCount (RTYPE_QRegisterCPD (parameterArray[1]));
    size = rtPTOKEN_CPD_BaseElementCount (posPToken);

    if (start > refNil)
	cnt = 0;
    else {
	cnt = (refNil > (size+start)) ? size : (refNil-start);
	rtLINK_AppendRange (linkc, start, cnt);
    }
    if (cnt < size)
	rtLINK_AppendRepeat (linkc, refNil, (size-cnt));

    linkc->Close (posPToken);

    return RTYPE_QRegister (linkc);
}

IOBJ_DefineNewaryMethod (NewRefLinkDM) {
    M_CPD *pRPT = RTYPE_QRegisterCPD (parameterArray[0]);
    RTYPE_MustBeA ("'newRefLink:'", pRPT->RType (), RTYPE_C_PToken);
    return RTYPE_QRegister (rtLINK_NewRefLink (NilOf (M_CPD*), pRPT));
}

IOBJ_DefineNewaryMethod (NewEmptyLinkDM) {
    return RTYPE_QRegister (
	rtLINK_NewEmptyLink (
	    RTYPE_QRegisterCPD (parameterArray[0]), RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}

IOBJ_DefineUnaryMethod (TracesOnDM) {
    TracingLCAllocator	    =
    TracingLookup	    =
    TracingLCAppending	    =
    TracingRRDCAllocator    = true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM) {
    TracingLCAllocator	    =
    TracingLookup	    =
    TracingLCAppending	    =
    TracingRRDCAllocator    = false;

    return self;
}

IOBJ_DefineUnaryMethod (DisplayTracesDM) {
    IO_printf ("TracingLCAllocator=%s\n", (TracingLCAllocator) ? "ON" : "OFF");
    IO_printf ("TracingLookup=%s\n", (TracingLookup) ? "ON" : "OFF");
    IO_printf ("TracingLCAppending=%s\n", (TracingLCAppending) ? "ON" : "OFF");
    IO_printf ("TracingRRDCAllocator=%s\n", (TracingRRDCAllocator) ? "ON" : "OFF");

    return self;
}

IOBJ_DefineNilaryMethod (AnalyzingCompressionDM) {
    return IOBJ_SwitchIObject (&AnalyzingCompression);
}

IOBJ_DefineNilaryMethod (LCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingLCAllocator);
}

IOBJ_DefineNilaryMethod (LCAppendingTraceDM) {
    return IOBJ_SwitchIObject (&TracingLCAppending);
}

IOBJ_DefineNilaryMethod (LookupTraceDM) {
    return IOBJ_SwitchIObject (&TracingLookup);
}

IOBJ_DefineNilaryMethod (RRDCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingRRDCAllocator);
}

IOBJ_DefineUnaryMethod (DisplayCompressionAnalyzerInfoDM) {
    IO_printf ("\nCompression Analyzer Info\n");
    IO_printf (  "-------------------------\n");

    IO_printf ("LinkCount         = %6u\n", AnalyzerLinkCount);
    IO_printf ("RRDCCount	  = %6u\n", AnalyzerRRDCCount);
    IO_printf ("DomCount	  = %6u\n", AnalyzerDomCount);
    IO_printf ("CodCount	  = %6u\n", AnalyzerCodCount);
    IO_printf ("DomShrink	  = %6u\n", AnalyzerRRDCCount - AnalyzerDomCount);
    IO_printf ("CodShrink	  = %6u\n", AnalyzerRRDCCount - AnalyzerCodCount);
    IO_printf ("\n");

    IO_printf ("MaxRRDCs          = %6u\n", AnalyzerMaxRRDCs);
    IO_printf ("MaxRRDCsHits      = %6u\n", AnalyzerMaxRRDCsHits);
    IO_printf ("MaxRRDCsDomShrink = %6u\n", AnalyzerMaxRRDCsDomShrink);
    IO_printf ("MaxRRDCsCodShrink = %6u\n", AnalyzerMaxRRDCsCodShrink);
    IO_printf ("\n");

    IO_printf ("MaxDomShrink	  = %6u\n", AnalyzerMaxDomShrink);
    IO_printf ("MaxDomShrinkHits  = %6u\n", AnalyzerMaxDomShrinkHits);
    IO_printf ("MaxCodShrink	  = %6u\n", AnalyzerMaxCodShrink);
    IO_printf ("MaxCodShrinkHits  = %6u\n", AnalyzerMaxCodShrinkHits);
    IO_printf ("\n");

    IO_printf ("Analyzer Is Currently %s\n", AnalyzingCompression ? "On" : "Off");

    return self;
}

IOBJ_DefineUnaryMethod (ResetCompressionAnalyzerInfoDM) {
    AnalyzerLinkCount		=
    AnalyzerRRDCCount		=
    AnalyzerDomCount		=
    AnalyzerCodCount		=
    AnalyzerMaxRRDCs		=
    AnalyzerMaxRRDCsHits	=
    AnalyzerMaxRRDCsDomShrink	=
    AnalyzerMaxRRDCsCodShrink	=
    AnalyzerMaxDomShrink	=
    AnalyzerMaxDomShrinkHits	=
    AnalyzerMaxCodShrink	=
    AnalyzerMaxCodShrinkHits	= 0;

    return self;
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nLink Routines Usage Counts\n");
    IO_printf (  "--------------------------\n");

    IO_printf ("RefConstructorCount     = %6u\n", RefConstructorCount);
    IO_printf ("PosConstructorCount     = %6u\n", PosConstructorCount);
    IO_printf ("AppendCount             = %6u\n", AppendCount);
    IO_printf ("CloseConstructorCount   = %6u\n", CloseConstructorCount);
    IO_printf ("DiscardConstructorCount = %6u\n", DiscardConstructorCount);
    IO_printf ("\n");

    IO_printf ("AllocateLCCount		= %6u\n", AllocateLCCount);
    IO_printf ("AllocateRRDCCount	= %6u\n", RRDConstructorFreeList.allocationCount ());
    IO_printf ("DiscardRRDCCount	= %6u\n", RRDConstructorFreeList.deallocationCount ());
    IO_printf ("MaxRRDCCount            = %6u\n", MaxRRDCCount);
    IO_printf ("\n");

    IO_printf ("AlignLinkCount          = %6u\n", AlignLinkCount);
    IO_printf ("AlignConstructorCount   = %6u\n", AlignConstructorCount);
    IO_printf ("\n");

    IO_printf ("BasicToLinkCount        = %6u\n", BasicToLinkCount);
    IO_printf ("ToConstructorCount      = %6u\n", ToConstructorCount);
    IO_printf ("\n");

    IO_printf ("BasicNewEmptyLinkCount  = %6u\n", BasicNewEmptyLinkCount);
    IO_printf ("NewRefLinkCount		= %6u\n", NewRefLinkCount);
    IO_printf ("CopyLinkCount		= %6u\n", CopyLinkCount);
    IO_printf ("\n");

    IO_printf ("ConstructorToRefUVCount	= %6u\n", ConstructorToRefUVCount);
    IO_printf ("LinkToRefUVCount	= %6u\n", LinkToRefUVCount);
    IO_printf ("RefUVToConstructorCount	= %6u\n", RefUVToConstructorCount);
    IO_printf ("\n");

    IO_printf ("LCExtractFromLCCount	= %6u\n", LCExtractFromLCCount);
    IO_printf ("LCExtractCount		= %6u\n", LCExtractCount);
    IO_printf ("UVExtractFromLCCount	= %6u\n", UVExtractFromLCCount);
    IO_printf ("UVExtractCount		= %6u\n", UVExtractCount);
    IO_printf ("RefExtractCount		= %6u\n", RefExtractCount);
    IO_printf ("\n");

    IO_printf ("LCAddFromLCCount	= %6u\n", LCAddFromLCCount);
    IO_printf ("LCAddCount		= %6u\n", LCAddCount);
    IO_printf ("LocateOrAddFromLCCount	= %6u\n", LocateOrAddFromLCCount);
    IO_printf ("LocateOrAddFromRefCount	= %6u\n", LocateOrAddFromRefCount);
    IO_printf ("\n");

    IO_printf ("LookupUsingLCKeyCount	= %6u\n", LookupUsingLCKeyCount);
    IO_printf ("LookupUsingRefKeyCount	= %6u\n", LookupUsingRefKeyCount);
    IO_printf ("TryAnotherRRDCount	= %6u\n", TryAnotherRRDCount);
    IO_printf ("LocateCount		= %6u\n", LocateCount);
    if (LocateCount) IO_printf (
	       "HuntsPerLocate		= %6u\n", TryAnotherRRDCount/ LocateCount
    );
    IO_printf ("DeleteLCElementsCount	= %6u\n", DeleteLCElementsCount);
    IO_printf ("DeleteRefElementCount	= %6u\n", DeleteRefElementCount);
    IO_printf ("\n");

    IO_printf ("LCCountReferencesCount  = %6u\n", LCCountReferencesCount);
    IO_printf ("LCComplementCount	= %6u\n", LCComplementCount);
    IO_printf ("\n");

    IO_printf ("ColumnProjectionCount	= %6u\n", ColumnProjectionCount);
    IO_printf ("RowProjectionCount	= %6u\n", RowProjectionCount);
    IO_printf ("LinearizeLCrRcCount	= %6u\n", LinearizeLCrRcCount);
    IO_printf ("LinearizeLCrLCicCount	= %6u\n", LinearizeLCrLCicCount);
    IO_printf ("LinearizeLCrUVicCount	= %6u\n", LinearizeLCrUVicCount);
    IO_printf ("\n");

    return self;
}

IOBJ_DefineUnaryMethod (InitializeCountsDM) {
    RRDConstructorFreeList.resetCounts ();

    RefConstructorCount		= 0;
    PosConstructorCount		= 0;
    AppendCount			= 0;
    CloseConstructorCount	= 0;
    DiscardConstructorCount	= 0;

    AllocateLCCount		= 0;
    MaxRRDCCount		= 0;

    AlignLinkCount		= 0;
    AlignConstructorCount	= 0;

    BasicToLinkCount		= 0;
    ToConstructorCount		= 0;

    BasicNewEmptyLinkCount	= 0;
    NewRefLinkCount		= 0;
    CopyLinkCount		= 0;

    ConstructorToRefUVCount	= 0;
    LinkToRefUVCount		= 0;
    RefUVToConstructorCount	= 0;

    LCExtractFromLCCount	= 0;
    LCExtractCount		= 0;
    UVExtractFromLCCount	= 0;
    UVExtractCount		= 0;
    RefExtractCount		= 0;

    LCAddFromLCCount		= 0;
    LCAddCount			= 0;
    LocateOrAddFromLCCount	= 0;
    LocateOrAddFromRefCount	= 0;

    LookupUsingLCKeyCount	= 0;
    LookupUsingRefKeyCount	= 0;
    TryAnotherRRDCount		= 0;
    LocateCount			= 0;
    DeleteLCElementsCount	= 0;
    DeleteRefElementCount	= 0;

    LCCountReferencesCount	= 0;
    LCComplementCount		= 0;

    ColumnProjectionCount	= 0;
    RowProjectionCount		= 0;
    LinearizeLCrRcCount		= 0;
    LinearizeLCrLCicCount	= 0;
    LinearizeLCrUVicCount	= 0;

    return self;
}

IOBJ_DefineMethod (SetInARutDM) {
    InARut = IOBJ_NumericIObjectValue (parameterArray[0], int);
    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

UV_DefineAlignDM (AlignDM, rtLINK_AlignLink)

IOBJ_DefineUnaryMethod (AlignLCDM) {
    return RTYPE_QRegister (LinkToConstructor (RTYPE_QRegisterCPD (self))->Align ());
}

IOBJ_DefineUnaryMethod (ElementCountDM) {
    return IOBJ_IntIObject (rtLINK_CPD_ElementCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (TrackingOnDM) {
    rtLINK_CPD_Tracking (RTYPE_QRegisterCPD (self)) = 1;
    return (self);
}

IOBJ_DefineUnaryMethod (TrackingOffDM) {
    rtLINK_CPD_Tracking (RTYPE_QRegisterCPD (self)) = 0;
    return (self);
}

IOBJ_DefineUnaryMethod (DumpLinkDM) {
    rtLINK_DumpLink (RTYPE_QRegisterCPD (self));
    return (self);
}

IOBJ_DefineUnaryMethod (RRDCountDM) {
    return IOBJ_IntIObject (rtLINK_CPD_RRDCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (PosPTokenDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtLINK_CPx_PosPToken);
}

IOBJ_DefineUnaryMethod (RefPTokenDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtLINK_CPx_RefPToken);
}

IOBJ_DefineUnaryMethod (AsRefUVDM) {
    return RTYPE_QRegister (rtLINK_LinkToRefUV (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (AsReorderedRefUVDM) {
    return RTYPE_QRegister (
	rtLINK_LinkToRefUV (
	    RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0])
	)
    );
}


IOBJ_DefineMethod (AtDM) {
    IOBJ_IObject result;
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self), *subscriptCPD;

    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
	rtREFUV_Type_Reference sref, rref;
        DSC_InitReferenceScalar (
	    sref,
	    rtLINK_CPD_PosPTokenCPD (sourceCPD),
	    IOBJ_IObjectValueAsInt (parameterArray[0])
	);
	rtLINK_RefExtract (&rref, sourceCPD, &sref);
	result = IOBJ_IntIObject (rtREFUV_Ref_D_Element (rref));
	DSC_ClearScalar (sref);
	DSC_ClearScalar (rref);
    }
    else if (RTYPE_C_RefUV ==
		(RTYPE_Type)M_CPD_RType
		    (subscriptCPD = RTYPE_QRegisterCPD (parameterArray[0])))
/*****  Handle the 'RefUV' case:  *****/
	result = RTYPE_QRegister (rtLINK_UVExtract (sourceCPD, subscriptCPD));
    else {
/*****  Handle the 'Link' case:  *****/
	rtLINK_CType *linkc = rtLINK_ToConstructor (subscriptCPD);
	result = RTYPE_QRegister (rtLINK_LCExtract (sourceCPD, linkc));
	linkc->release ();
    }

    return result;
}

IOBJ_DefineMethod (AddlcDM) {
/* NOTE: Unlike AddDM, this routine is not able to modify the Q register
 * pointed to by "self".  Therefore, the modified target (originally from self)
 * is placed in a new temporary Q register.  This can be accessed in the
 * debugger by using the "setFromLast" command.
 */
    M_CPD *targetCPD	= RTYPE_QRegisterCPD (self);
    M_CPD *addSourceCPD	= RTYPE_QRegisterCPD (parameterArray[0]);

    if (RTYPE_C_Link == (RTYPE_Type)M_CPD_RType (addSourceCPD)) {
	rtLINK_CType *linkcd = rtLINK_ToConstructor (targetCPD);
	rtLINK_CType *linkcs = rtLINK_ToConstructor (addSourceCPD);
	IOBJ_IObject newself = RTYPE_QRegister (linkcd->Add (linkcs));
	RTYPE_QRegister (linkcd);
	linkcs->release ();

	return newself;
    }

    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (AddDM) {
    M_CPD
	*targetCPD	= RTYPE_QRegisterCPD (self),
	*addSourceCPD	= RTYPE_QRegisterCPD (parameterArray[0]);

    if (RTYPE_C_Link == (RTYPE_Type)M_CPD_RType (addSourceCPD)) {
	rtLINK_CType *pSource = rtLINK_ToConstructor (addSourceCPD);
	rtLINK_CType *pResult = rtLINK_LCAdd (targetCPD, pSource);
	pSource->release ();

	return RTYPE_QRegister (pResult);
    }

    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (LocateOrAddDM) {
    M_CPD
	*targetCPD	= RTYPE_QRegisterCPD (self),
	*addSourceCPD	= RTYPE_QRegisterCPD (parameterArray[0]);

    if (RTYPE_C_Link == (RTYPE_Type)M_CPD_RType (addSourceCPD)) {
	rtLINK_CType *linkc = rtLINK_ToConstructor (addSourceCPD);

	rtLINK_CType *addedLinkc = NilOf (rtLINK_CType*);
	rtLINK_CType *pResult = rtLINK_LocateOrAddFromLC (targetCPD, linkc, &addedLinkc);
	linkc->release ();
	if (addedLinkc) {
	    IO_printf ("\nAdded Link Constructor:\n");
	    addedLinkc->DebugPrint ();
	    addedLinkc->release ();
	}
	return RTYPE_QRegister (pResult);
    }

    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (LocateOrAddNoDM) {
    M_CPD
	*targetCPD	= RTYPE_QRegisterCPD (self),
	*addSourceCPD	= RTYPE_QRegisterCPD (parameterArray[0]);

    if (RTYPE_C_Link == (RTYPE_Type)M_CPD_RType (addSourceCPD)) {
	rtLINK_CType *linkc = rtLINK_ToConstructor (addSourceCPD);
	rtLINK_CType *pResult = rtLINK_LocateOrAddFromLC (
	    targetCPD, linkc, NilOf (rtLINK_CType **)
	);
	linkc->release ();
	return RTYPE_QRegister (pResult);
    }

    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (LocateOrAddRefDM) {
    M_CPD*			targetCPD = RTYPE_QRegisterCPD (self);
    rtREFUV_Type_Reference	srcRef, locationRef;

    DSC_InitReferenceScalar (
	srcRef,
	rtLINK_CPD_RefPTokenCPD (targetCPD),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    bool added = rtLINK_LocateOrAddFromRef (targetCPD, &srcRef, &locationRef);

    IO_printf ("\nValue Added = %s,  position = %d\n",
	(added) ? "true" : "false", rtREFUV_Ref_Element (&locationRef));
    DSC_ClearScalar (locationRef);
    DSC_ClearScalar (srcRef);

    return (self);
}

IOBJ_DefineMethod (LookupTypeDM) {
    M_CPD *sourceCPD	= RTYPE_QRegisterCPD (self);
    M_CPD *keyCPD	= RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_LookupCase lookupCase = (rtLINK_LookupCase)IOBJ_IObjectValueAsInt (
	parameterArray[1]
    );

    if (RTYPE_C_Link == (RTYPE_Type)M_CPD_RType (keyCPD)) {
	rtLINK_CType *linkc = rtLINK_ToConstructor (keyCPD);
	rtLINK_CType *pResult, *locatedLinkC = NilOf (rtLINK_CType*);
        rtLINK_LookupUsingLCKey (
	    sourceCPD, linkc, lookupCase, 100000, &pResult, &locatedLinkC
	);
	linkc->release ();
	if (locatedLinkC) {
	    IO_printf ("\nLocated Link Constructor:\n");
	    locatedLinkC->DebugPrint ();
	    locatedLinkC->release ();
	}

	return RTYPE_QRegister (pResult);
    }

    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (LookupRefTypeDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);

    rtLINK_LookupCase lookupCase = (rtLINK_LookupCase)IOBJ_IObjectValueAsInt (
	parameterArray[1]
    );

    rtREFUV_Type_Reference keyRef;
    DSC_InitReferenceScalar (
	keyRef,
	rtLINK_CPD_RefPTokenCPD (sourceCPD),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    rtREFUV_Type_Reference locatedRef;
    if (rtLINK_LookupUsingRefKey (sourceCPD, &keyRef, lookupCase, 100000, &locatedRef)) {
	/* found */
	IO_printf (
	    "Lookedup Value Found at position = %d\n", rtREFUV_Ref_Element (&locatedRef)
	);
	DSC_ClearScalar (locatedRef);
    }
    else
	IO_printf ("Lookedup Value not found\n");

    DSC_ClearScalar (keyRef);

    return self;
}

IOBJ_DefineMethod (DeleteDM) {
    rtLINK_CType *elementLinkc = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray [0])
    );

    rtLINK_CType *deletedLinkc;
    rtLINK_DeleteLCSelectedElements (RTYPE_QRegisterCPD (self), elementLinkc, &deletedLinkc);
    elementLinkc->release ();

    if (IsntNil (deletedLinkc)) {
	IO_printf ("\nDeleted Link Constructor:\n");
	deletedLinkc->DebugPrint ();
	deletedLinkc->release ();
    }

    return self;
}

IOBJ_DefineMethod (DeleteNoDM) {
    rtLINK_CType *elementLinkc = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray [0])
    );

    rtLINK_DeleteLCSelectedElements (
	RTYPE_QRegisterCPD (self), elementLinkc, NilOf (rtLINK_CType **)
    );
    elementLinkc->release ();

    return self;
}

IOBJ_DefineMethod (DeleteRefDM) {
    M_CPD *sourceLink = RTYPE_QRegisterCPD (self);

    rtREFUV_Type_Reference deleteRef;
    DSC_InitReferenceScalar (
	deleteRef,
	rtLINK_CPD_RefPTokenCPD (sourceLink),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    bool result = rtLINK_DeleteRefSelectedElement (sourceLink, &deleteRef);
    DSC_ClearScalar (deleteRef);

    if (result)
	IO_printf ("\nElement Deleted\n");
    else
	IO_printf ("\nElement Not Found\n");

    return self;
}

IOBJ_DefineMethod (AtLCRefUVDM) {
    return RTYPE_QRegister (
	rtLINK_ToConstructor (RTYPE_QRegisterCPD (self))->Extract (
	    RTYPE_QRegisterCPD (parameterArray[0])
	)
    );
}

IOBJ_DefineMethod (TestToLinkNewDM) {
//  ... and return a copy of the link with its new PPT:
    return RTYPE_QRegister (
	rtLINK_Copy (RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0]))
    );
}

IOBJ_DefineUnaryMethod (CopyLinkDM) {
    return RTYPE_QRegister (rtLINK_Copy (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (LCComplementDM) {
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    rtLINK_CType *result = linkc->Complement ();
    linkc->release ();
    return RTYPE_QRegister (result);
}

IOBJ_DefineMethod (LCCountReferencesDM) {
    M_CPD *pRPT = RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *resultCPD = rtLINK_LCCountReferences (linkc, pRPT, NilOf (M_CPD*));
    linkc->release ();

    return RTYPE_QRegister (resultCPD);
}

IOBJ_DefineMethod (LCCountReferencesDistribDM) {
    M_CPD *pRPT = RTYPE_QRegisterCPD (parameterArray[0]);
    M_CPD *distribCPD = RTYPE_QRegisterCPD (parameterArray[1]);

    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *resultCPD = rtLINK_LCCountReferences (linkc, pRPT, distribCPD);
    linkc->release ();

    return RTYPE_QRegister (resultCPD);
}

IOBJ_DefineUnaryMethod (ColumnProjectionDM) {
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *result = rtLINK_ColumnProjection (linkc);
    linkc->release ();

    return RTYPE_QRegister (result);
}

IOBJ_DefineUnaryMethod (RowProjectionDM) {
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    rtLINK_CType *resultLinkc = rtLINK_RowProjection (linkc);

    linkc->release ();
    return RTYPE_QRegister (resultLinkc);
}

IOBJ_DefineMethod (LinearizeLCrRcDM) {
/*****
 *
 *  Arguments:
 *	self		- the row link
 *	arg0		- the cartesian ptoken
 *	arg1		- the integer column number
 *
 *****/
    rtLINK_CType *rowLinkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *cartesianPT = RTYPE_QRegisterCPD (parameterArray [0]);

    rtREFUV_Type_Reference columnRef;
    DSC_InitReferenceScalar (
	columnRef,
	rtPTOKEN_CPD_ColPTokenCPD (cartesianPT),
	IOBJ_IObjectValueAsInt (parameterArray[1])
    );

    rtLINK_CType *resultLinkc = rtLINK_LinearizeLCrRc (cartesianPT, rowLinkc, &columnRef);
    rowLinkc->release ();
    DSC_ClearScalar (columnRef);

    return RTYPE_QRegister (resultLinkc);
}

IOBJ_DefineMethod (LinearizeLCrUVicDM) {
/*****
 *
 *  Arguments:
 *	self		- the row link
 *	arg0		- the cartesian ptoken
 *	arg1		- the column reference UVector
 *	arg2		- the column indirection reference UVector
 *
 *****/
    rtLINK_CType *rowLinkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *cartesianPT = RTYPE_QRegisterCPD (parameterArray [0]);
    M_CPD *columnUV = RTYPE_QRegisterCPD (parameterArray[1]);
    M_CPD *columnIndirection = RTYPE_QRegisterCPD (parameterArray[2]);

    M_CPD *result = rtLINK_LinearizeLCrUVic (
	cartesianPT, rowLinkc, columnUV, columnIndirection
    );
    rowLinkc->release ();

    return RTYPE_QRegister (result);
}

IOBJ_DefineMethod (LinearizeLCrLCicDM) {
/*****
 *
 *  Arguments:
 *	self		- the row link
 *	arg0		- the cartesian ptoken
 *	arg1		- the column link
 *	arg2		- the column indirection reference UVector
 *
 *****/
    rtLINK_CType *rowLinkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (self));
    M_CPD *cartesianPT = RTYPE_QRegisterCPD (parameterArray [0]);
    rtLINK_CType *columnLinkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[1]));
    M_CPD *columnIndirection = RTYPE_QRegisterCPD (parameterArray[2]);

    M_CPD *result = rtLINK_LinearizeLCrLCic (
	cartesianPT, rowLinkc, columnLinkc, columnIndirection
    );
    rowLinkc->release ();
    columnLinkc->release ();

    return RTYPE_QRegister (result);
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtLINK_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:referencing:"	, NewReferencingDM)
	IOBJ_MDE ("newPos:ref:init:"	, NewPosRefInitDM)
	IOBJ_MDE ("newRefLink:"		, NewRefLinkDM)
	IOBJ_MDE ("newEmptyPos:ref:"	, NewEmptyLinkDM)
	IOBJ_MDE ("tracesOn"		, TracesOnDM)
	IOBJ_MDE ("tracesOff"		, TracesOffDM)
	IOBJ_MDE ("traces"		, DisplayTracesDM)
	IOBJ_MDE ("lcAllocatorTrace"	, LCAllocatorTraceDM)
	IOBJ_MDE ("lookupTrace"		, LookupTraceDM)
	IOBJ_MDE ("lcAppendingTrace"	, LCAppendingTraceDM)
	IOBJ_MDE ("rrdcAllocatorTrace"	, RRDCAllocatorTraceDM)
	IOBJ_MDE ("analyzingCompression", AnalyzingCompressionDM)
	IOBJ_MDE (
	    "displayCompressionInfo"	, DisplayCompressionAnalyzerInfoDM
	)
	IOBJ_MDE ("resetCompressionInfo", ResetCompressionAnalyzerInfoDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
	IOBJ_MDE ("initCounts"		, InitializeCountsDM)
	IOBJ_MDE ("tuneLookup:"		, SetInARutDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("size"		, ElementCountDM)
	IOBJ_MDE ("rrdCount"		, RRDCountDM)
	IOBJ_MDE ("posPToken"		, PosPTokenDM)
	IOBJ_MDE ("refPToken"		, RefPTokenDM)
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("alignLC"		, AlignLCDM)
	IOBJ_MDE ("addlc:"		, AddlcDM)
	IOBJ_MDE ("asRefUV"		, AsRefUVDM)
	IOBJ_MDE ("asRefUV:"		, AsReorderedRefUVDM)
	IOBJ_MDE ("add:"		, AddDM)
	IOBJ_MDE ("locateOrAdd:"	, LocateOrAddDM)
	IOBJ_MDE ("locateOrAddNo:"	, LocateOrAddNoDM)
	IOBJ_MDE ("locateOrAddRef:"	, LocateOrAddRefDM)
	IOBJ_MDE ("lookup:type:"	, LookupTypeDM)
	IOBJ_MDE ("lookupRef:type:"	, LookupRefTypeDM)
	IOBJ_MDE ("delete:"		, DeleteDM)
	IOBJ_MDE ("deleteNo:"		, DeleteNoDM)
	IOBJ_MDE ("deleteRef:"		, DeleteRefDM)
	IOBJ_MDE ("trackingOn"		, TrackingOnDM)
	IOBJ_MDE ("trackingOff"		, TrackingOffDM)
	IOBJ_MDE ("atLCRefUV:"		, AtLCRefUVDM)
	IOBJ_MDE ("testToLinkNew:"	, TestToLinkNewDM)
	IOBJ_MDE ("dump"		, DumpLinkDM)
	IOBJ_MDE ("copyLink"		, CopyLinkDM)
	IOBJ_MDE ("lcComplement"	, LCComplementDM)
	IOBJ_MDE ("lcCountReferences:distrib:"
				        , LCCountReferencesDistribDM)
	IOBJ_MDE ("lcCountReferences:"	, LCCountReferencesDM)
 	IOBJ_MDE ("columnProjection"	, ColumnProjectionDM)
	IOBJ_MDE ("rowProjection"	, RowProjectionDM)
	IOBJ_MDE ("linearizePT:col:"	, LinearizeLCrRcDM)
	IOBJ_MDE ("linearizePT:cLC:ciUV:", LinearizeLCrLCicDM)
	IOBJ_MDE ("linearizePT:cUV:ciUV:", LinearizeLCrUVicDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtLINK_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtLINK_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
        PrintLink (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
        PrintLink (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtLINK);
    FAC_FDFCase_FacilityDescription
        ("Link Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlink.c 1 replace /users/mjc/system
  860421 12:29:48 mjc Create link representation type

 ************************************************************************/
/************************************************************************
  RTlink.c 2 replace /users/mjc/system
  860422 22:55:50 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTlink.c 3 replace /users/mjc/system
  860504 18:45:47 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTlink.c 4 replace /users/mjc/system
  860513 09:29:19 mjc Added logic for 'LCExtract' - not yet working

 ************************************************************************/
/************************************************************************
  RTlink.c 5 replace /users/jad/system
  860523 15:33:45 jad release link to link extractions

 ************************************************************************/
/************************************************************************
  RTlink.c 6 replace /users/jad/system
  860523 16:20:15 jad release empty shells for add routines

 ************************************************************************/
/************************************************************************
  RTlink.c 7 replace /users/jad/system
  860603 20:25:19 jad implemented add and the extract routine for CHharlie

 ************************************************************************/
/************************************************************************
  RTlink.c 8 replace /users/jad/system
  860605 18:18:37 jad implement 1st version of Align, add ref cnt
to linkc's, and remove references to valuedsc refuv

 ************************************************************************/
/************************************************************************
  RTlink.c 9 replace /users/jck/system
  860609 12:53:01 jck Added positional alignment to rtLINK_Align

 ************************************************************************/
/************************************************************************
  RTlink.c 10 replace /users/jad/system
  860612 15:02:52 jad fully implemented align, added ToLinkNoDiscard,
discard constructor now zeros out chain head and tail

 ************************************************************************/
/************************************************************************
  RTlink.c 11 replace /users/jad/system
  860612 18:43:59 jad fix align end conditions

 ************************************************************************/
/************************************************************************
  RTlink.c 12 replace /users/mjc/system
  860617 15:55:03 mjc Added saver

 ************************************************************************/
/************************************************************************
  RTlink.c 13 replace /users/mjc/system
  860623 10:21:54 mjc Converted to use new 'valuedsc' macros

 ************************************************************************/
/************************************************************************
  RTlink.c 14 replace /users/mjc/system
  860627 10:36:54 mjc Integer 'new:' parameter to 'new:referencing:' now creates a 1 element link referencing the position specified by the integer

 ************************************************************************/
/************************************************************************
  RTlink.c 15 replace /users/m2/system
  860701 17:18:13 m2 Added align and add routines with LinkConstructors as args

 ************************************************************************/
/************************************************************************
  RTlink.c 16 replace /users/m2/system
  860702 16:45:04 m2 Fixed bug in AlignConstructor which allocated too many RRDCs

 ************************************************************************/
/************************************************************************
  RTlink.c 17 replace /users/mjc/system
  860702 22:33:14 mjc Removed spurious debugging print statements

 ************************************************************************/
/************************************************************************
  RTlink.c 18 replace /users/mjc/system
  860709 10:15:37 mjc Release new format 'reference's for continued development by 'jad'

 ************************************************************************/
/************************************************************************
  RTlink.c 19 replace /users/mjc/system
  860711 14:26:49 mjc Eliminated automatic deletion of the link constructor in 'rtLINK_CToRefUV'

 ************************************************************************/
/************************************************************************
  RTlink.c 20 replace /users/spe/system
  860712 16:07:19 spe Added orderingUV operations in rtLINK_CToRefUV.

 ************************************************************************/
/************************************************************************
  RTlink.c 21 replace /users/jad/system
  860715 16:25:53 jad added NewPos:ref method, expanded the align error messages

 ************************************************************************/
/************************************************************************
  RTlink.c 22 replace /users/jad/system
  860717 14:50:20 jad fixed ptoken problem in rtVECTOR_ConstructorToRefUV

 ************************************************************************/
/************************************************************************
  RTlink.c 23 replace /users/jad/system
  860717 17:47:20 jad made Dump link constructor public

 ************************************************************************/
/************************************************************************
  RTlink.c 24 replace /users/jad/system
  860721 12:46:08 jad implemented rtLINK_UVExtractFromLC routine

 ************************************************************************/
/************************************************************************
  RTlink.c 25 replace /users/jad/system
  860723 16:42:02 jad added ToLink routine with a new pos ptoken

 ************************************************************************/
/************************************************************************
  RTlink.c 26 replace /users/jck/system
  860725 16:43:56 jck Emancipated some CPD's

 ************************************************************************/
/************************************************************************
  RTlink.c 27 replace /users/mjc/system
  860803 21:22:57 mjc Fixed close constructor reference limit test

 ************************************************************************/
/************************************************************************
  RTlink.c 28 replace /users/mjc/system
  860805 18:46:44 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  RTlink.c 29 replace /users/jad/system
  860807 20:09:56 jad freed some more enslaved cpd's

 ************************************************************************/
/************************************************************************
  RTlink.c 30 replace /users/jad/system
  860812 19:25:35 jad freed newPosPtokenCPD's in the two link add routines

 ************************************************************************/
/************************************************************************
  RTlink.c 31 replace /users/jad/system
  860814 14:17:29 jad use new ptoken IsntCurrent routines in aligns

 ************************************************************************/
/************************************************************************
  RTlink.c 32 replace /users/jad/system
  860814 19:40:29 jad added counters and a displayCounts
debug method

 ************************************************************************/
/************************************************************************
  RTlink.c 33 replace /users/jad/system
  860903 12:55:58 jad 1) added hasRepeats and hasRanges fields to both links and linkcs 2) added 2 simple link creation routines

 ************************************************************************/
/************************************************************************
  RTlink.c 34 replace /users/jad/system
  860904 17:17:47 jad fixed bug which occurred on bootstrap

 ************************************************************************/
/************************************************************************
  RTlink.c 35 replace /users/jad/system
  860914 11:14:29 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  RTlink.c 36 replace /users/jad/system
  860929 12:01:00 jad made DumpLink public

 ************************************************************************/
/************************************************************************
  RTlink.c 37 replace /users/mjc/system
  861002 19:07:19 mjc Fixed long lurking bug in reference alignment that corrupted P-Tokens

 ************************************************************************/
/************************************************************************
  RTlink.c 38 replace /users/mjc/nsystem
  861003 14:04:55 mjc Placed appropriate calls to M_EnableModifications in
the align routine.

 ************************************************************************/
/************************************************************************
  RTlink.c 39 replace /users/mjc/system
  861014 22:53:25 mjc Added 'AlignForDistribute', fixed empty link alignment bug in 'AlignConstructor'

 ************************************************************************/
/************************************************************************
  RTlink.c 40 replace /users/jad/system
  861023 17:08:59 jad freed a ptoken, and implemented linkcopy

 ************************************************************************/
/************************************************************************
  RTlink.c 41 replace /users/jad/system
  861024 14:07:08 jad removed some temp code left in

 ************************************************************************/
/************************************************************************
  RTlink.c 42 replace /users/jad/system
  861113 17:07:22 jad make threadConstructor and AllocateRRDConstructor macros for speed

 ************************************************************************/
/************************************************************************
  RTlink.c 43 replace /users/jad/system
  861119 12:43:05 jad added rtLINK_LCComplement routine, needed by vmachine

 ************************************************************************/
/************************************************************************
  RTlink.c 44 replace /users/mjc/system
  861121 15:43:07 mjc Fixed bug in 'nil' reference 'RefExtract'

 ************************************************************************/
/************************************************************************
  RTlink.c 45 replace /users/mjc/system
  861125 17:33:10 mjc Added metric to count number of '(cpd,-1)' calls to {Ref,Pos,Close}Constructor calls

 ************************************************************************/
/************************************************************************
  RTlink.c 46 replace /users/jad/system
  861205 17:32:01 jad added LocateOrAdd, and Lookup routines

 ************************************************************************/
/************************************************************************
  RTlink.c 47 replace /users/jad/system
  861210 18:47:31 jad added ref versions of locateOrAdd and lookup

 ************************************************************************/
/************************************************************************
  RTlink.c 48 replace /users/jad/system
  861216 17:52:30 jad fixed the case of an empty source in AddLocate routine

 ************************************************************************/
/************************************************************************
  RTlink.c 49 replace /users/jad/system
  861219 15:43:31 jad added LCCountReferences routine, fixed
LocateOrAdd to not add duplicates

 ************************************************************************/
/************************************************************************
  RTlink.c 50 replace /users/jad/system
  861219 18:19:16 jad added a check to the countReferences routine

 ************************************************************************/
/************************************************************************
  RTlink.c 51 replace /users/jad/system
  861224 17:54:14 jad change some calls to M_ShiftContainerTail to reclaim released space

 ************************************************************************/
/************************************************************************
  RTlink.c 52 replace /users/jad/system
  870303 16:12:59 jad added linearization, projection, & deletion routines

 ************************************************************************/
/************************************************************************
  RTlink.c 53 replace /users/jad/system
  870413 15:36:44 jad 1) fixed bug in reference LocateOrAdd routine,
2) added a distribute routine, 3) added an argument to lookup routines: panSize

 ************************************************************************/
/************************************************************************
  RTlink.c 54 replace /users/jck/system
  870415 09:53:17 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTlink.c 55 replace /users/jck/system
  870511 15:33:12 jck fixed a number of bugs

 ************************************************************************/
/************************************************************************
  RTlink.c 56 replace /users/jck/system
  870513 11:36:55 jck Added rtLINK_LookupCase_GT

 ************************************************************************/
/************************************************************************
  RTlink.c 57 replace /users/mjc/translation
  870524 09:33:24 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlink.c 58 replace /users/mjc/system
  870607 03:11:16 mjc Eliminate VAX Compilation Errors

 ************************************************************************/
/************************************************************************
  RTlink.c 59 replace /users/jck/system
  870730 09:32:34 jck Added some constructor usage counts and freed some enslaved constructors

 ************************************************************************/
/************************************************************************
  RTlink.c 60 replace /users/jck/system
  870807 16:05:41 jck Changed 'LookupUsingRefKey' to use a binary search of the rrd's rather than a linear search

 ************************************************************************/
/************************************************************************
  RTlink.c 61 replace /users/jck/system
  870810 14:13:19 jck Optimized rtLINK_LookupUsingLCKey

 ************************************************************************/
/************************************************************************
  RTlink.c 62 replace /users/jck/system
  870812 15:42:11 jck Fixed assorted edge condition problems with the new optimized lookup routines

 ************************************************************************/
/************************************************************************
  RTlink.c 63 replace /users/jck/system
  870831 14:03:51 jck found bug in an optimization to the binary search algorithm
of rtLINK_LookupUsingLCKey. As the optimization was of no observable worth, I removed it
thereby fixing the bug

 ************************************************************************/
/************************************************************************
  RTlink.c 64 replace /users/jck/system
  871007 13:35:10 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTlink.c 65 replace /users/jad/system
  871230 11:25:53 jad added omitted 'optional' argument to 'rtLINK_CToRefUV'

 ************************************************************************/
/************************************************************************
  RTlink.c 67 replace /users/jck/system
  880826 09:40:01 jck Removing sequence links from the main line of descent

 ************************************************************************/
/************************************************************************
  RTlink.c 68 replace /users/jck/system
  880915 12:33:06 jck Added an omitted case to ...AlignConstructor

 ************************************************************************/
/************************************************************************
  RTlink.c 69 replace /users/jck/system
  881104 09:45:24 jck rtLINK_LCExtract, rtLINK_LookupUsing*Key optimizations

 ************************************************************************/
/************************************************************************
  RTlink.c 70 replace /users/jck/system
  881128 10:01:55 jck Fixed bug in lookup routines

 ************************************************************************/
/************************************************************************
  RTlink.c 71 replace /users/jck/system
  881212 10:47:04 jck Introduced memory saving efficiencies into some Distribution functions

 ************************************************************************/
/************************************************************************
  RTlink.c 72 replace /users/jck/system
  890223 08:20:09 jck Fortified against saving links which have been corrupted

 ************************************************************************/
/************************************************************************
  RTlink.c 73 replace /users/jck/system
  890413 12:45:13 jck Fixed a bug with the way the Add routines dealt with the reference nil as a value to add

 ************************************************************************/
/************************************************************************
  RTlink.c 74 replace /users/m2/backend
  890503 11:49:28 m2 Fix for uninitialized var

 ************************************************************************/
/************************************************************************
  RTlink.c 75 replace /users/jck/system
  890511 07:49:36 jck Fix bug in the Align routines

 ************************************************************************/
/************************************************************************
  RTlink.c 76 replace /users/jck/system
  890705 14:45:57 jck Disabled unnecessary and potentially hazardous pointer increment in ...LookupUsingLCKey

 ************************************************************************/
/************************************************************************
  RTlink.c 77 replace /users/m2/backend
  890927 15:20:55 m2 dummyiobj added

 ************************************************************************/
