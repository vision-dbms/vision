/*****  Vector Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtVECTOR

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "RTvector.h"

/*****  Supporting  *****/
#include "m.h"
#include "ps.h"
#include "uvector.h"

#include "vdbupdate.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"
#include "RTrefuv.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

#include "RTdsc.h"
#include "RTlstore.h"
#include "RTvstore.h"

#include "VDescriptor.h"
#include "VFragment.h"

/*****  Forward Declarations  *****/
PrivateFnDef int LocateOrAddVectorSegment (
    M_CPD *pVector, M_CPD *pStore, RTYPE_Type xRType, M_CPD *pRPTRef, int xRPTRef, bool set
);

PrivateFnDef void ExpandUSegArray (M_CPD *vector, size_t growthIncrement);


/*******************************************
 *******************************************
 *****  USegment Index Sort Utilities  *****
 *******************************************
 *******************************************/

/*****  U-Segment Array Address (Used by Sort Predicate)  *****/
PrivateVarDef rtVECTOR_USDType const *USISortUSDArray;

/*****  Save Routine U-Segment Index Re-Sort Predicate  *****/
/*---------------------------------------------------------------------------
 *****  'qsort' predicate used to reorder the U-Segment Index of a vector.
 *
 *  Arguments:
 *	i1, i2	- pointers to the U-Segment Array indices being compared.
 *
 *  Returns:
 *	-1	if VStorePOP (i1) <  VStorePOP (i2),
 *	 0	if VStorePOP (i1) == VStorePOP (i2),
 *	 1	if VStorePOP (i1) >  VStorePOP (i2)
 *
 *****/
PrivateFnDef int __cdecl USISortPredicate (int const *i1, int const *i2) {
    return M_ComparePOPs (
	rtVECTOR_USD_VStore (USISortUSDArray + *i1),
	rtVECTOR_USD_VStore (USISortUSDArray + *i2)
    );
}


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

DEFINE_CONCRETE_RTT (rtVECTOR_Handle);


/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool
    TracingVCAllocator		= false,
    TracingUSDCAllocator	= false,
    TracingPMRDCAllocator	= false,
    TracingReconstructor	= false,
    TracingPMapBrief		= false,
    TracingPMapSearches		= false;


/****************************
 *****  Usage Counters  *****
 ****************************/

unsigned int rtVECTOR_PMRDC::g_cAllocations = 0;
unsigned int rtVECTOR_PMRDC::g_cDeallocations = 0;

unsigned int rtVECTOR_USDC::g_cAllocations = 0;
unsigned int rtVECTOR_USDC::g_cDeallocations = 0;

PrivateVarDef unsigned int
    NewCount			= 0,

    AllocateVCCount		= 0,
    AllocatePMRecCount		= 0,

    FreeVCCount			= 0,
    FreePMRecCount		= 0,

    AlignVectorCount		= 0,
    USegmentDeletionCount	= 0,
    USegmentCompactionCount	= 0,
    MaxUSegmentCountEncountered	= 0,
    AlignConstructorCount	= 0,

    ToSetVCCount		= 0,
    VCToSetVCCount		= 0,

    LCAssignFromValueDCount	= 0,
    LCAssignFromVCCount		= 0,
    RefAssignFromValueDCount	= 0,
    RefAssignFromVCCount	= 0,

    LCExtractCount		= 0,
    LCExtractHuntCount		= 0,
    SimpleLCExtractCount	= 0,
    RefExtractCount		= 0,
    RefExtractHuntCount		= 0,
    RefExtractFromVCCount	= 0,

    USegArrayExpansionCount	= 0,
    USegIndexHitCount		= 0,
    USegIndexMissCount		= 0,
    USegIndexLSearchCount	= 0,
    USegIndexLSearchComparisons	= 0,

    ToVectorCount		= 0,
    ToConstructorCount		= 0,

    ReOrderCount		= 0,
    ReOrderVCCount		= 0,
    DistributeCount		= 0,
    DistributeVCCount		= 0,

    PartitionStatsCount	        = 0,
    PartitionSortIndicesCount	= 0,
    PartitndPartitionCount	= 0,

    MakeConstructorLinksCount	= 0,
    VCSubsetInStoreCount	= 0,
    VCSubsetOfTypeCount		= 0,
    SubsetInStoreCount		= 0,
    SubsetOfTypeCount		= 0,
    SimplifyVCToDescriptorCount	= 0,
    SimplifyToDescriptorCount	= 0,

    LocateOrAddFromVCCount	= 0,
    LocateOrAddFromValueDCount	= 0,
    LookupFromVCCount		= 0,
    LookupFromValueDCount	= 0;


/********************************
 ********************************
 *****  U-Vector Utilities  *****
 ********************************
 ********************************/

/**********************
 *****  Creation  *****
 **********************/

PrivateFnDef M_CPD *NewUVector (
    RTYPE_Type xRType, M_CPD *pPPT, M_CPD *rptRefCPD, int rptRefIndex
) {
    switch (xRType) {
    case RTYPE_C_CharUV:
	return rtCHARUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_DoubleUV:
	return rtDOUBLEUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_FloatUV:
	return rtFLOATUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_IntUV:
	return rtINTUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_RefUV:
    case RTYPE_C_Link:
	return rtREFUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_UndefUV:
	return rtUNDEFUV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_Unsigned64UV:
	return rtU64UV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_Unsigned96UV:
	return rtU96UV_New	(pPPT, rptRefCPD, rptRefIndex);
    case RTYPE_C_Unsigned128UV:
	return rtU128UV_New	(pPPT, rptRefCPD, rptRefIndex);
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"NewUVector: Unsupported U-Vector Type '%s'",
		RTYPE_TypeIdAsString (xRType)
	    )
	);
    }  /* End of Type Switch */

    return NilOf (M_CPD*);
}


/***********************
 *****  Alignment  *****
 ***********************/

PrivateFnDef void AlignUVector (M_CPD *uvector) {
    switch (M_CPD_RType (uvector)) {
    case RTYPE_C_CharUV:
	rtCHARUV_Align (uvector);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Align (uvector);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Align (uvector);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_Align (uvector);
	break;
    case RTYPE_C_RefUV:
	rtREFUV_Align (uvector);
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_Align (uvector);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Align (uvector);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Align (uvector);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Align (uvector);
	break;
    default:
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "AlignUVector: Unknown usegment type"
	);
	break;
    }
}


/************************
 *****  Assignment  *****
 ************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to assign the values of a source Value Descriptor
 *****  into the appropriate locations of a target U-Segment.
 *
 *  Arguments:
 *	targetUVector		- a standard CPD for the target u-vector.  This
 *				  argument can be 'Nil' - if it is, the next
 *				  two arguments will be used to access the
 *				  u-vector to update.
 *	targetVector		- a standard CPD for the target vector.
 *	targetUSegment		- the index of the target U-Segment.
 *	subscriptPosition	- the position in the target U-Segment to be 
 *				  changed.
 *	sourceDescriptor	- a variable of type 'DSC_Descriptor *'
 *                                supplying the new values.
 *
 *  Returns:
 *	'targetUVector'
 *
 *****/
PrivateFnDef M_CPD *AtUVOffsetPutDescriptor (
    M_CPD		*targetUVector,
    M_CPD		*targetVector,
    int			 targetUSegment,
    int			 offset,
    DSC_Descriptor	*sourceDescriptor
) {
    static char const *const Routine = "AtUVOffsetPutDescriptor";

/*****  Obtain a CPD for the target u-vector...  *****/
    if (IsNil (targetUVector)) {
	rtVECTOR_CPD_USD (targetVector) =
	    rtVECTOR_CPD_USegArray (targetVector) + targetUSegment;
	rtVECTOR_CPD_SetUSDCursor (targetVector, rtVECTOR_USD_Values);
	targetUVector = targetVector->GetCPD (rtVECTOR_CPx_USDCursor);
    }

/*****  ... and update it:  *****/
    switch (sourceDescriptor->pointerType ()) {
    default:
	sourceDescriptor->complainAboutBadPointerType (Routine);
	break;
    case DSC_PointerType_Empty:
	DSC__ComplainAboutEmptyPtrType (Routine);
	break;
    case DSC_PointerType_Scalar:
	switch (DSC_Scalar_RType (DSC_Descriptor_Scalar (*sourceDescriptor))) {
	case RTYPE_C_CharUV:
	    rtCHARUV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtCHARUV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Char (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtDOUBLEUV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Double (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtFLOATUV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Float (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtINTUV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Int (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_RefUV:
	    rtREFUV_Align (targetUVector);
	    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (*sourceDescriptor));
	    targetUVector->EnableModifications ();
	    rtREFUV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Int (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_UndefUV:
	    rtUNDEFUV_Align (targetUVector);
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtU64UV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Unsigned64 (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtU96UV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Unsigned96 (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_Align (targetUVector);
	    targetUVector->EnableModifications ();
	    rtU128UV_CPD_Array (targetUVector)[offset] =
		DSC_Scalar_Unsigned128 (DSC_Descriptor_Scalar (*sourceDescriptor));
	    break;
	default:
	    targetUVector->release ();
	    ERR_SignalFault (
		EC__UnknownUVectorRType, UTIL_FormatMessage (
		    "%s: Unsupported Scalar R-Type %s",
		    Routine,
		    RTYPE_TypeIdAsString (
			DSC_Scalar_RType (DSC_Descriptor_Scalar (*sourceDescriptor))
		    )
		)
	    );
	    break;
	}
	break;
    case DSC_PointerType_Identity:
	rtREFUV_Align (targetUVector);
	targetUVector->EnableModifications ();
	rtREFUV_CPD_Array (targetUVector)[offset] = rtPTOKEN_CPD_BaseElementCount (
	    DSC_Descriptor_Identity (*sourceDescriptor).PToken ()
	) - 1;
	break;
    case DSC_PointerType_Value:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference: {
	    M_CPD *sourcePPTRefCPD; int sourcePPTRefIndex;
	    sourceDescriptor->getPPTReference (sourcePPTRefCPD, sourcePPTRefIndex);
	    rtLINK_CType *subscriptLC = rtLINK_AppendRange (
		rtLINK_RefConstructor (targetUVector, UV_CPx_PToken), offset, 1
	    )->Close (sourcePPTRefCPD, sourcePPTRefIndex);
	    sourcePPTRefCPD->release ();

	    sourceDescriptor->assignToUV (subscriptLC, targetUVector);
	    subscriptLC->release ();
	}
	break;
    }

    return targetUVector;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to assign the values of a source Value Descriptor
 *****  into the appropriate locations of a target U-Segment.
 *
 *  Arguments:
 *	targetUVector		- a standard CPD for the target u-vector.  This
 *				  argument can be 'Nil' - if it is, the next
 *				  two arguments will be used to access the
 *				  u-vector to update.
 *	targetVector		- a standard CPD for the target vector.
 *	targetUSegment		- the index of the target U-Segment.
 *	subscriptLC		- a link constructor specifying the elements of
 *				  the target U-Segment to be changed.
 *	sourceDescriptor	- a variable of type 'DSC_Descriptor *'
 *                                supplying the new values.
 *
 *  Returns:
 *	'targetUVector'
 *
 *****/
PrivateFnDef M_CPD *AtUVLinkCPositionsPutDescriptor (
    M_CPD		*targetUVector,
    M_CPD		*targetVector,
    int			 targetUSegment,
    rtLINK_CType	*subscriptLC,
    DSC_Descriptor	*sourceDescriptor
)
{
/*****  Obtain a CPD for the target u-vector...  *****/
    if (IsNil (targetUVector)) {
	rtVECTOR_CPD_USD (targetVector) =
	    rtVECTOR_CPD_USegArray (targetVector) + targetUSegment;
	rtVECTOR_CPD_SetUSDCursor (targetVector, rtVECTOR_USD_Values);
	targetUVector = targetVector->GetCPD (rtVECTOR_CPx_USDCursor);
    }

/***** Do the assignment ... *****/
    sourceDescriptor->assignToUV (subscriptLC, targetUVector);

/*****  ...and return.  *****/
    return targetUVector;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to assign the values of a source U-Vector into the
 *****  appropriate locations of a target U-Segment.
 *
 *  Arguments:
 *	targetUVector		- a standard CPD for the target u-vector.  This
 *				  argument can be 'Nil' - if it is, the next
 *				  two arguments will be used to access the
 *				  u-vector to update.
 *	targetVector		- a standard CPD for the target vector.
 *	targetUSegment		- the index of the target U-Segment.
 *	subscriptLC		- a link constructor specifying the elements of
 *				  the target U-Segment to be changed.
 *	sourceUVector		- a standard CPD for the source U-Vector.
 *
 *  Returns:
 *	'targetUVector'
 *
 *****/
PrivateFnDef M_CPD *AtUVLinkCPositionsPutUV (
    M_CPD		*targetUVector,
    M_CPD		*targetVector,
    int			 targetUSegment,
    rtLINK_CType	*subscriptLC,
    M_CPD		*sourceUVector
)
{
/*****  Obtain a CPD for the target u-vector...  *****/
    if (IsNil (targetUVector)) {
	rtVECTOR_CPD_USD (targetVector) =
	    rtVECTOR_CPD_USegArray (targetVector) + targetUSegment;
	rtVECTOR_CPD_SetUSDCursor (targetVector, rtVECTOR_USD_Values);
	targetUVector = targetVector->GetCPD (rtVECTOR_CPx_USDCursor);
    }

/*****  ...perform a case specific assignment...  *****/
    RTYPE_Type targetUVectorRType = (RTYPE_Type)M_CPD_RType (targetUVector);
    switch (targetUVectorRType) {
    case RTYPE_C_CharUV:
        rtCHARUV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_DoubleUV:
        rtDOUBLEUV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_FloatUV:
        rtFLOATUV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_IntUV:
        rtINTUV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_RefUV:
        rtREFUV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_Align (targetUVector);
	break;
    case RTYPE_C_Unsigned64UV:
        rtU64UV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_Unsigned96UV:
        rtU96UV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    case RTYPE_C_Unsigned128UV:
        rtU128UV_LCAssign (targetUVector, subscriptLC, sourceUVector);
	break;
    default:
	ERR_SignalFault (
	    EC__UnimplementedCase, UTIL_FormatMessage (
		"AtUVLinkCPositionsPutUV: Unsupported R-Type %s",
		RTYPE_TypeIdAsString (targetUVectorRType)
	    )
	);
	break;
    }

/*****  ...and return.  *****/
    return targetUVector;
}


/*******************************************
 *******************************************
 *****  Vector Constructor Management  *****
 *******************************************
 *******************************************/

/*******************************************
 *****  Constants, Types, and Globals  *****
 *******************************************/

/*****  Run Time Type  *****/
DEFINE_CONCRETE_RTT (rtVECTOR_CType);

PrivateVarDef bool g_bPOPOrderingEnabled = false;

PublicFnDef void rtVECTOR_EnablePOPOrdering (bool flag) {
    g_bPOPOrderingEnabled = flag;
}



/************************************
 *****  Construction Utilities  *****
 ************************************/

void rtVECTOR_CType::MakeUSDArray () {
    if (m_sUSDArray) {
	size_t sUSDArray = m_sUSDArray * sizeof (rtVECTOR_USDC*);
	m_pUSDArray = (rtVECTOR_USDC**)allocate (sUSDArray);
	memset (m_pUSDArray, 0, sUSDArray);
    }
}

void rtVECTOR_CType::CompactUSDArray () {
    unsigned int xAdjustedSegment = 0;
    for (unsigned int xSegment = 0; xSegment < m_sUSDArray; xSegment++) {
	rtVECTOR_USDC *pUSDC = m_pUSDArray[xSegment];
	if (pUSDC) {
	    pUSDC->m_xSegment = xAdjustedSegment;
	    m_pUSDArray[xAdjustedSegment++] = pUSDC;
	}
    }
    m_sUSDArray = xAdjustedSegment;
}


/*****************************************
 *****  rtVECTOR_CType Construction  *****
 *****************************************/

/*---------------------------------------------------------------------------
 *****  rtVECTOR_CType Constructor.
 *
 *  Arguments:
 *	pPPT	    		- the positional p-token of the constructor.
 *	sUSDArray		- an initial size for the USDCr Index array.
 *
 *****/
rtVECTOR_CType::rtVECTOR_CType (M_CPD *pPPT, unsigned int sUSDArray)
: BaseClass		(1)
, m_pPPT		(pPPT)
, m_sUSDArray		(sUSDArray)
, m_pUSDArray		(0)
, m_fIsASet		(false)
, m_iPMRDCount		(0)
, m_pPMRDChainHead	(0)
, m_pPMRDChainTail	(0)
{
    RTYPE_MustBeA ("rtVECTOR_CType::rtVECTOR_CType", M_CPD_RType (pPPT), RTYPE_C_PToken);

    AllocateVCCount++;

    pPPT->retain ();

    MakeUSDArray ();

    if (TracingVCAllocator) display (
	"...rtVECTOR_CType::rtVECTOR_CType: VC:%08X\n", this
    );
}


/*****************************************************
 *****  Vector To Vector-Constructor Conversion  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a vector into a constructor.
 *
 *  Arguments:
 *	pVector		- a standard CPD for the vector to be
 *				  converted to a constructor.
 *
 *  Returns:
 *	The address of a constructor for the vector.
 *
 *  Notes:
 *	- The constructor returned by this routine must be either consumed or
 *	  discarded.  Failure to do one of the above will prevent the garbage
 *	  collection of the memory it occupies and the physical objects it
 *	  references.
 *
 *****/
rtVECTOR_CType::rtVECTOR_CType (M_CPD *pVector)
: BaseClass		(1)
, m_pPPT		(rtVECTOR_CPD_RowPTokenCPD (rtVECTOR_Align (pVector)))
, m_sUSDArray		(cardinality () > 0 ? rtVECTOR_CPD_USegArraySize (pVector) : 0)
, m_pUSDArray		(0)
, m_fIsASet		(rtVECTOR_CPD_IsASet (pVector))
, m_iPMRDCount		(0)
, m_pPMRDChainHead	(0)
, m_pPMRDChainTail	(0)
{
    ToConstructorCount++;

    unsigned int *pRelocationArray;
    if (g_bPOPOrderingEnabled) {
	pRelocationArray = (unsigned int *)allocate (m_sUSDArray * sizeof (unsigned int));
	int *pUSegIndex = rtVECTOR_CPD_USegIndex (pVector);
	m_sUSDArray = rtVECTOR_CPD_USegIndexSize (pVector);
	for (unsigned int x = 0; x < m_sUSDArray; x++) {
	    pRelocationArray [*pUSegIndex++] = x;
	}
    }
    MakeUSDArray ();

    /*****  Copy the USDArray...  *****/
    for (unsigned int xUSDC = 0; xUSDC < m_sUSDArray; xUSDC++) {
	unsigned int xUSegment;
	xUSegment = g_bPOPOrderingEnabled ? rtVECTOR_CPD_USegIndex (pVector)[xUSDC] : xUSDC;
	/* position the vector to the current USD */
	rtVECTOR_CPD_USD (pVector) = rtVECTOR_CPD_USegArray (pVector) + xUSegment;

	/* copy the UVector into the new USDC */
	rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_Values);
	if (pVector->ReferenceIsntNil (rtVECTOR_CPx_USDCursor)) {
	    M_CPD *pPointer= pVector->GetCPD (rtVECTOR_CPx_USDCursor);

	    /* copy the VStore into the new USDC */
	    rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_VStore);
	    M_CPD *pStore= pVector->GetCPD (rtVECTOR_CPx_USDCursor);

	    /* copy the PToken into the new USDC */
	    rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_PToken);
	    M_CPD *pPPT = pVector->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);

	    NewUSDC (xUSDC, pStore, pPointer, pPPT);
	}
    }

    /***** Copy the PMap... *****/
    rtVECTOR_PMRDType const *pPMRD = rtVECTOR_CPD_PMap (pVector);
    rtVECTOR_PMRDType const *const pPMRDLimit = pPMRD + rtVECTOR_CPD_PMapSize (pVector);
    if (g_bPOPOrderingEnabled) {
	while (pPMRD < pPMRDLimit) {
	    AppendPMRD (
		pRelocationArray [rtVECTOR_PMRD_SegmentIndex (pPMRD)],
		rtVECTOR_PMRD_SegmentOrigin (pPMRD),
		rtVECTOR_PMRD_SegmentLength (pPMRD)
	    );
	    pPMRD++;
	}
	deallocate (pRelocationArray);
    } else {
	while (pPMRD < pPMRDLimit)
	    AppendPMRD (pPMRD++);

	CompactUSDArray ();
    }
    if (TracingVCAllocator) IO_printf (
	"...rtVECTOR_ToConstructor: VC:%08X created.\n", this
    );
}


/*************************
 *****  Destruction  *****
 *************************/

rtVECTOR_USDC::~rtVECTOR_USDC () {
    g_cDeallocations++;

    if (m_pAssociatedLink)
	m_pAssociatedLink->release ();

    if (m_pStore)
	m_pStore->release ();
    if (m_pPointer)
	m_pPointer->release ();
    if (m_pPPT)
	m_pPPT->release ();
}

rtVECTOR_CType::~rtVECTOR_CType () {
/***** Free the PMDRCr's ... *****/
    while (m_pPMRDChainHead) {
	rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead;
	m_pPMRDChainHead = pPMRDC->Successor ();
	delete pPMRDC;
    }

/***** Free the USDCr's ... *****/
    for (unsigned int xUSDC = 0; xUSDC < m_sUSDArray; xUSDC++) {
	rtVECTOR_USDC *pUSDC = m_pUSDArray[xUSDC];
	if (pUSDC)
	    delete pUSDC;
    }

/***** Free the Index array ... *****/
    if (m_pUSDArray)
	deallocate (m_pUSDArray);

/***** Free the ptoken ... *****/
    if (m_pPPT)
	m_pPPT->release ();

/***** And free the constructor ... *****/
    FreeVCCount++;
}


/***********************
 *****  Alignment  *****
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally align a vector constructor.
 *
 *  Argument:
 *	this			- the address of the vector constructor to be
 *				  aligned.
 *
 *  Returns:
 *	Nothing
 *
 *****/
void rtVECTOR_CType::Align () {
/*****  Do nothing if the vector is already current...  *****/
    if (rtPTOKEN_IsCurrent (m_pPPT, -1))
	return;

/*****  Otherwise complain about an unimplemented case for now...  *****/
    AlignConstructorCount++;

/*++++++++++++++++++++++++++++*
 *++++   NOT IMPLEMENTED  ++++*
 *++++++++++++++++++++++++++++*/
    ERR_SignalFault (
	EC__UnimplementedCase, "rtVECTOR_CType::Align: If you need me write me."
    );
}


/************************************************
 *****  Vector Constructor Misc. Routines  ******
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create and assign values to the Associated Link
 *****  Constructors for each U-Segment of the vector constructor.
 *****  These linkc's will the positions in the vector of the U-Segment
 *****  values.
 *
 *  Arguments:
 *	this 		- the address of the vector constructor whose
 *                        associated link constructors are to be created and
 *                        assigned values.
 *
 *  Returns:
 *	The address of the vector constructor.
 *
 ******/
void rtVECTOR_CType::MakeConstructorLinks () {
    MakeConstructorLinksCount++;

/***** Align the vector constructor ... *****/
    Align ();

/***** Create the Link Constructors for each U-Segment *****/
    unsigned int i;
    for (i = 0; i < m_sUSDArray; i++) {
	m_pUSDArray[i]->MakeDomAssociatedLink ();
    }

/***** Loop Thru the Pmap Appending Values to the Link Constructors ... *****/
    unsigned int vectorOrigin = 0;
    for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
	unsigned int count = pPMRDC->SegmentLength ();
	rtLINK_AppendRange (pPMRDC->AssociatedLink (), vectorOrigin, count);
	vectorOrigin += count;
    }

/***** Close the Linkc's *****/
    for (i = 0; i < m_sUSDArray; i++) {
	m_pUSDArray[i]->CloseAssociatedLink (m_pPPT);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to find the subset of vector elements referencing a particular
 *****  store.
 *
 *  Arguments:
 *	pStore 			- the store.
 *	pValueReturn		- the optional address ('Nil' if absent) of a
 *				  polytype descriptor that will be set to the
 *				  elements associated with the located subset.
 *
 *  Returns:
 *	A link constructor for the subset or Nil if no matching elements were 
 *	found.
 *
 ******/
rtLINK_CType *rtVECTOR_CType::subsetInStore (M_CPD *pStore, VDescriptor *pValueReturn) {
    VCSubsetInStoreCount++;

/***** Align the vector constructor ... *****/
    Align ();

/***** Find the requested u-segment, ... *****/
    bool notFound = true;
    rtVECTOR_USDC *pUSDC = 0;
    for (unsigned int xSegment=0; xSegment < m_sUSDArray; xSegment++) {
	pUSDC = m_pUSDArray [xSegment];
	if (pUSDC->Names (pStore)) {
	    notFound = false;
	    break;
	}
    }

    if (notFound || pUSDC->cardinality () == 0)
	return NilOf (rtLINK_CType*);

/***** Return the corresponding values if appropriate...  *****/
    if (pValueReturn)
	pValueReturn->setToMonotype (pStore, pUSDC->pointerCPD ());

/***** ... and compute the subset: *****/
    rtLINK_CType *pSubset = rtLINK_PosConstructor (pUSDC->PPT (), -1);

    unsigned int vectorOrigin = 0;
    for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
        unsigned int count = pPMRDC->SegmentLength ();
	if (pUSDC == pPMRDC->USDC ())
	    rtLINK_AppendRange (pSubset, vectorOrigin, count);
	vectorOrigin += count;
    }

    pSubset->Close (m_pPPT);

    return pSubset;
}


/*---------------------------------------------------------------------------
 *****  Routine to find the subset of vector elements referencing a particular
 *****  known object table member.
 *
 *  Arguments:
 *	pKOTM 			- the known object table member.
 *	pValueReturn		- the optional address ('Nil' if absent) of a
 *				  polytype descriptor that will be set to the
 *				  elements associated with the located subset.
 *
 *  Returns:
 *	A link constructor for the subset or Nil if no matching elements were
 *	found.
 *
 ******/
rtLINK_CType *rtVECTOR_CType::subsetOfType (
    M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
) {
    Align ();

/*****  Determine the number of matching elements and segments, ... *****/
    rtVECTOR_USDC *pMonotypeUSDC = 0; // Valid only if cSegments == 1.
    unsigned int cSegments = 0;
    unsigned int sSubset = 0;
    unsigned int xSegment;
    for (xSegment = 0; xSegment < m_sUSDArray; xSegment++) {
	rtVECTOR_USDC *pUSDC = m_pUSDArray [xSegment];
	if (pUSDC->Names (pKOTM)) {
	    unsigned int sSegment = pUSDC->cardinality ();
	    if (sSegment > 0) {
		pMonotypeUSDC = pUSDC;
		sSubset += sSegment;
		cSegments++;
	    }
	}
    }

    if (sSubset == 0)
	return NilOf (rtLINK_CType*);

/*****  Construct the required sets and subsets, ...  *****/
    rtLINK_CType *pSubset = rtLINK_RefConstructor (m_pPPT, -1);

    if (pValueReturn && cSegments > 1) {
	for (xSegment = 0; xSegment < m_sUSDArray; xSegment++) {
	    rtVECTOR_USDC *pUSDC = m_pUSDArray [xSegment];
	    if (pUSDC->Names (pKOTM))
		pUSDC->MakeDomAssociatedLink ();
	    else
		pUSDC->ClearAssociatedLink ();
	}

	unsigned int xVectorOrigin = 0;
	unsigned int xSubsetOrigin = 0;
	for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
	    unsigned int count = pPMRDC->SegmentLength ();
	    rtLINK_CType *pLC = pPMRDC->AssociatedLink ();
	    if (pLC) {
		pLC->AppendRange (xSubsetOrigin, count);
		xSubsetOrigin += count;
		pSubset->AppendRange (xVectorOrigin, count);
	    }
	    xVectorOrigin += count;
	}
    }
    else {
	unsigned int xVectorOrigin = 0;
	for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
	    unsigned int count = pPMRDC->SegmentLength ();
	    if (pPMRDC->USDC () ->Names (pKOTM)) {
		pSubset->AppendRange (xVectorOrigin, count);
	    }
	    xVectorOrigin += count;
	}
    }

    M_CPD *pSubsetPPT = rtPTOKEN_New (pSubsetSpace, sSubset);
    pSubset->Close (pSubsetPPT);

/*****  Construct the returned values, if appropriate: *****/
    if (pValueReturn) {
	if (cSegments == 1) {
	    rtLINK_CType *pIsolator = rtLINK_RefConstructor (
		pMonotypeUSDC->PPT (), -1
	    )->AppendRange (0, sSubset)->Close (pSubsetPPT);

	    VDescriptor iValueReturn;
	    iValueReturn.setToMonotype (
		pMonotypeUSDC->storeCPD (), pMonotypeUSDC->pointerCPD ()
	    );
	    pValueReturn->setToSubset (pIsolator, iValueReturn);
	    pIsolator->release ();
	}
	else {
	    VFragmentation &rResult = pValueReturn->setToFragmentation (pSubsetPPT);
	    for (xSegment = 0; xSegment < m_sUSDArray; xSegment++) {
		rtVECTOR_USDC *pUSDC = m_pUSDArray [xSegment];
		rtLINK_CType *pLC = pUSDC->ClaimedAssociatedLink ();
		if (pLC) {
		    pLC->Close (pSubsetPPT);
		    rResult.createFragment (pLC)->datum ().setToMonotype (
			pUSDC->storeCPD (), pUSDC->pointerCPD ()
		    );
		}
	    }
	}
    }

/*****  And clean-up and return:  *****/
    pSubsetPPT->release ();

    return pSubset;
}


/*************************************************
 *************************************************
 *****  General Vector Management Utilities  *****
 *************************************************
 *************************************************/

/**********************************
 *****  Consistency Checking  *****
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to verify an L-Store's consistency.
 *****/
void rtVECTOR_Handle::CheckConsistency () {
    if (rtVECTOR_V_IsInconsistent ((rtVECTOR_Type*)ContainerContent ())) ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "Corrupted vector[%d:%d] detected", SpaceIndex (), ContainerIndex ()
	)
    );
}


/*****************************************
 *****  Standard CPD Initialization  *****
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Interal routine to initialize a standard vector CPD.
 *
 *  Argument:
 *	cpd			- the address of the CPD to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *pVectorCPD) {
    rtVECTOR_Type *pVector = rtVECTOR_CPD_Base (pVectorCPD);

    rtVECTOR_CPD_PToken	    (pVectorCPD) = &rtVECTOR_V_PToken	(pVector);
    rtVECTOR_CPD_PMRD	    (pVectorCPD) = rtVECTOR_V_PMap	(pVector);
    rtVECTOR_CPD_USI	    (pVectorCPD) = rtVECTOR_V_USegIndex	(pVector);
    rtVECTOR_CPD_USD	    (pVectorCPD) = rtVECTOR_V_USegArray	(pVector);

    rtVECTOR_CPD_SetUSDCursor (pVectorCPD, rtVECTOR_USD_PToken);
}


/************************************
 ************************************
 *****  Instantiation Routines  *****
 ************************************
 ************************************/

/**********************************************
 *****  Internal Initialization Routines  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a U-Segment array entry.
 *
 *  Arguments:
 *	vector			- a standard CPD for the vector whose U-Segment
 *				  array is to be modified.
 *	uSegArrayLoc		- the index of the U-Segment Array entry to be
 *				  initialized.
 *	store			- a standard CPD for the value store to be
 *				  associated with this U-Segment Array entry.
 *	valuesCPD		- a standard CPD for the initial values for the
 *				  U-Segment.
 *	pTokenCPD		- a standard CPD for a P-Token defining the
 *				  positional state to be associated with the
 *				  U-Segment.
 *
 *****/
PrivateFnDef void InitUSegmentArrayEntry (
    M_CPD *vector, int uSegArrayLoc, M_CPD *store, M_CPD *valuesCPD, M_CPD *pTokenCPD
) {
    rtVECTOR_CPD_USD (vector) = rtVECTOR_CPD_USegArray (vector) + uSegArrayLoc;

/*****  ...initialize the slot...  *****/
    rtVECTOR_CPD_SetUSDCursor (vector, rtVECTOR_USD_VStore);
    vector->StoreReference (rtVECTOR_CPx_USDCursor, store);

    rtVECTOR_CPD_SetUSDCursor (vector, rtVECTOR_USD_Values);
    vector->StoreReference (rtVECTOR_CPx_USDCursor, valuesCPD);

    rtVECTOR_CPD_SetUSDCursor (vector, rtVECTOR_USD_PToken);
    vector->StoreReference (rtVECTOR_CPx_USDCursor, pTokenCPD);
}


/*---------------------------------------------------------------------------
 *****  Internal routine to create a new vector.
 *
 *  Arguments:
 *	pPPT			- the positional P-Token of the new vector.
 *	initialUSegmentStore	- the address of a standard CPD for the store
 *				  of the initial u-segment of this vector.
 *				  This argument may be Nil if 'nelements == 0'.
 *	initialUSegmentValues	- the address of a standard CPD for the values
 *				  of the initial u-segment of this vector.
 *				  This argument may be Nil if 'nelements == 0'.
 *	initialUSegmentPToken	- the address of a standard CPD for the ptoken
 *				  of the initial u-segment of this vector.
 *				  This argument may be Nil if 'nelements == 0'.
 *
 *  Returns:
 *	The address of a CPD for the vector created.
 *
 *****/
PrivateFnDef M_CPD *NewVector (
    M_CPD *pPPT,
    M_CPD *initialUSegmentStore	 = NilOf (M_CPD *),
    M_CPD *initialUSegmentValues = NilOf (M_CPD *),
    M_CPD *initialUSegmentPToken = NilOf (M_CPD *)
) {
/*****  Determine the p-map size of the new vector, ...  *****/
    unsigned int nelements = rtPTOKEN_CPD_BaseElementCount (pPPT);
    unsigned int pMapSize = nelements > 0 ? 1 : 0;

/*****  ...  allocate the vector, ...  *****/
    M_CPD *vector = pPPT->CreateContainer (
	RTYPE_C_Vector, rtVECTOR_V_SizeofVector (pMapSize, rtVECTOR_V_USegArrayInitialSize, 1)
    );

/*****  ... and initialize the vector's preamble, ...  *****/
    rtVECTOR_Type *base = rtVECTOR_CPD_Base (vector);
    rtVECTOR_V_IsInconsistent	(base) = true;
    rtVECTOR_V_USegArraySize	(base) = rtVECTOR_V_USegArrayInitialSize;
    rtVECTOR_V_USegFreeBound	(base) =
    rtVECTOR_V_USegIndexSize	(base) =
    rtVECTOR_V_PMapSize		(base) = pMapSize;
    rtVECTOR_V_PMapTransition	(base) = 0;
    rtVECTOR_V_ElementCount	(base) = nelements;
    rtVECTOR_V_IsASet		(base) = false;

/*****  ...  last P-Map entry, ...  *****/
    rtVECTOR_PMRD_SegmentIndex	(rtVECTOR_V_PMap (base) + pMapSize) = (-1);

/*****  ...  CPD, ...  *****/
    InitStdCPD (vector);

/*****  ...  PToken, ...  *****/
    vector->constructReference (rtVECTOR_CPx_PToken);
    vector->StoreReference (rtVECTOR_CPx_PToken, pPPT);

/*****  ...  USegment Array, ...  *****/
    vector->constructReferences (
	rtVECTOR_CPx_USD, rtVECTOR_V_USegArrayInitialSize * rtVECTOR_USD_POPsPerUSD
    );

/*****  ... and initial U-Segment:  *****/
    if (pMapSize > 0) {
	/*****  U-Segment Array...  *****/
	InitUSegmentArrayEntry (
	    vector, 0, initialUSegmentStore, initialUSegmentValues, initialUSegmentPToken
	);

	/*****  U-Segment Index...  *****/
	rtVECTOR_CPD_USegIndex (vector)[0] = 0;

	/*****  P-Map...  *****/
	rtVECTOR_PMRDType *pmrd0 = rtVECTOR_V_PMap (base);

	rtVECTOR_V_USegIndex (base)[0]		=
	rtVECTOR_PMRD_VectorOrigin	(pmrd0) =
	rtVECTOR_PMRD_SegmentIndex	(pmrd0) =
	rtVECTOR_PMRD_SegmentOrigin	(pmrd0) = 0;
    }

/*****  Finally, mark the vector as valid ...  *****/
    rtVECTOR_CPD_IsInconsistent	(vector) = false;

/*****  ... and return.  *****/
    return vector;
} 


/*********************************************
 *****  External Instantiation Routines  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a vector which will contain the specified uvector.
 *
 *  Arguments:
 *	pPPT		- a standard CPD for the positional p-token of the
 *			  new vector.  This p-token should be identical to
 *			  to the positional p-token of the u-vector argument
 *			  passed to this routine.
 *	uSegmentStore	- a standard CPD for the uvector's store.
 *	uSegmentValues	- a standard CPD for the uvector to install in the
 *                        new vector.  The positional ptoken of this uvector
 *			  will be changed to allow the vector to manage its
 *			  cardinality.
 *
 *  Returns:
 *	The address of a CPD for the vector created.
 *
 *****/
PublicFnDef M_CPD *rtVECTOR_NewFromUV (
    M_CPD *pPPT, M_CPD *uSegmentStore, M_CPD *uSegmentValues
) {
    M_CPD *uSegmentPToken = rtPTOKEN_New (
	pPPT->Space (), rtPTOKEN_CPD_BaseElementCount (pPPT)
    );
    uSegmentValues->StoreReference (UV_CPx_PToken, uSegmentPToken);

    M_CPD *pResult = NewVector (pPPT, uSegmentStore, uSegmentValues, uSegmentPToken);

    uSegmentPToken->release ();

    return pResult;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new vector initialized from a standard descriptor.
 *
 *  Arguments:
 *	initialValues		- the address of a descriptor supplying the
 *				  initial values of the vector.
 *
 *  Returns:
 *	The address of a standard CPD for the vector created.
 *
 *  NOTES:
 *	This routine is written for use by the VDescriptor form conversion
 *	routines only.
 *
 *****/
PublicFnDef M_CPD *rtVECTOR_NewFromDescriptor (DSC_Descriptor *initialValues) {
    M_CPD *pPPT = initialValues->PPT ();
    unsigned int nelements = rtPTOKEN_CPD_BaseElementCount (pPPT);
    M_CPD *vector;
    if (nelements < 1)
	vector = NewVector (pPPT);
    else {
	M_CPD *pUSegmentValues;
	if (initialValues->getUVector (pUSegmentValues)) {
	    //  The u-vector was just minted.
	}
//	else if (pUSegmentValues->ReferenceCount () > 1) // ... this might be enough, but..
	else if (pUSegmentValues->ContainerHasMultipleUses ())
	    pUSegmentValues = UV_Copy (pUSegmentValues);
	else
	    pUSegmentValues->retain ();

	M_CPD *pUSegmentPToken = rtPTOKEN_New (pPPT->Space (), nelements);
	pUSegmentValues->StoreReference (UV_CPx_PToken, pUSegmentPToken);

	vector = NewVector (
	    pPPT, initialValues->storeCPD (), pUSegmentValues, pUSegmentPToken
	);

	pUSegmentValues->release ();
	pUSegmentPToken->release ();
    }

    pPPT->release ();

    return vector;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new vector.
 *
 *  Arguments:
 *	pPPT	    		- a standard CPD for the positional P-Token
 *				  of the new vector.
 *
 *  Returns:
 *	The address of a CPD for the vector created.
 *
 *****/
PublicFnDef M_CPD *rtVECTOR_New (M_CPD *pPPT) {
    NewCount++;

    RTYPE_MustBeA ("rtVECTOR_New", M_CPD_RType (pPPT), RTYPE_C_PToken);

    unsigned int nelements = rtPTOKEN_CPD_BaseElementCount (pPPT);
    if (nelements < 1)
	return NewVector (pPPT);

    M_KOTE const &rNA = pPPT->TheNAClass ();

    M_CPD *initialUSegmentPToken = rtPTOKEN_New (pPPT->Space (), nelements);

    M_CPD *initialUSegmentValues = rtUNDEFUV_New (
	initialUSegmentPToken, rNA.PTokenCPD ()
    );

    M_CPD *vector = NewVector (
	pPPT, rNA, initialUSegmentValues, initialUSegmentPToken
    );

    initialUSegmentPToken->release ();
    initialUSegmentValues->release ();

    return vector;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new set vector.
 *
 *  Arguments:
 *	pTokenCPD	    	- a standard CPD for the positional p-token of
 *				  the new set vector.
 *
 *  Returns:
 *	The address of a CPD for the vector created.
 *
 *****/
PublicFnDef M_CPD *rtVECTOR_NewSetVector (M_CPD *pTokenCPD) {
    M_CPD *result = rtVECTOR_New (pTokenCPD);

    rtVECTOR_CPD_IsASet (result) = true;

    return result;
}


/*****************************************************
 *****  Vector-Constructor To Vector Conversion  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a constructor into a vector.
 *
 *  Arguments:
 *	this			- the address of the constructor to be
 *				  converted into a vector.
 *	discarding		- if true the constructor will be discarded
 *	                          by this routine and should NOT be
 *                                subsequently referenced.
 *
 *  Returns:
 *	The address of a standard CPD for the vector.
 *
 *****/
M_CPD *rtVECTOR_CType::ToVector (bool discarding) {
    ToVectorCount++;

/***** Align the constructor... *****/
    Align ();

/***** Create the new vector and set the IsASet bit ... *****/
    M_CPD *pResult = rtVECTOR_New (m_pPPT);
    rtVECTOR_CPD_IsASet (pResult) = m_fIsASet;

/***** Check for the trivial case *****/
    if (rtPTOKEN_CPD_BaseElementCount (m_pPPT) == 0) {
	if (discarding)
	    release ();
	return pResult;
    }

/***** Mark the result as inconsistent until modifications are complete  *****/
    rtVECTOR_CPD_IsInconsistent (pResult) = true;

/***** Expand the u-segment array and index, ... *****/
    if (m_sUSDArray > (unsigned int)rtVECTOR_CPD_USegArraySize (pResult)) {
	if (TracingUSDCAllocator) IO_printf ("ToVector: Call ExpandUSegArray\n");
	ExpandUSegArray (pResult, m_sUSDArray - rtVECTOR_CPD_USegArraySize (pResult));
    }

    rtVECTOR_CPD_USI (pResult) = rtVECTOR_CPD_USegIndex (pResult)
			       + rtVECTOR_CPD_USegIndexSize (pResult);
    pResult->ShiftContainerTail (
	rtVECTOR_CPx_USI, 0, (
	    (ptrdiff_t)m_sUSDArray - rtVECTOR_CPD_USegIndexSize (pResult)
	) * sizeof (int), false
    );
    rtVECTOR_CPD_USegIndexSize (pResult) = m_sUSDArray;
    int *pUSDIndex = rtVECTOR_CPD_USI (pResult) = rtVECTOR_CPD_USegIndex (pResult);

/***** Initialize the u-segment array and index, ... *****/
    for (unsigned int i = 0; i < m_sUSDArray; i++) {
	rtVECTOR_USDC *pUSDC = m_pUSDArray[i];

	M_CPD *ptoken = UV_CPD_PosPTokenCPD (pUSDC->pointerCPD ());
	/*** As a consistency check: ***/
	/*** compare the ptoken above with the rtVECTOR_VC_USDCr_PToken ***/
	InitUSegmentArrayEntry (
	    pResult, i, pUSDC->storeCPD (), pUSDC->pointerCPD (), ptoken
	);
	ptoken->release ();

	pUSDIndex[i] = i;
    }
    rtVECTOR_CPD_USegFreeBound (pResult) = m_sUSDArray;

/***** Sort the u-segment index, ... *****/
    USISortUSDArray = rtVECTOR_CPD_USegArray (pResult);
    qsort (
	(char *)rtVECTOR_CPD_USegIndex (pResult),
	m_sUSDArray, sizeof (int), (VkComparator)USISortPredicate
    );

/*****  Adjust the space allocated to the P-Map...  *****/
    unsigned int oldPMapSize = rtVECTOR_CPD_PMapSize (pResult);
    rtVECTOR_CPD_PMRD (pResult) = rtVECTOR_CPD_PMap (pResult) + oldPMapSize;

    pResult->ShiftContainerTail (
	rtVECTOR_CPx_PMRD, sizeof (rtVECTOR_PMRDType)
	    + sizeof (int) * rtVECTOR_CPD_USegIndexSize (pResult)
	    + sizeof (rtVECTOR_USDType) * rtVECTOR_CPD_USegArraySize (pResult),
	sizeof (rtVECTOR_PMRDType) * ((ptrdiff_t)m_iPMRDCount - oldPMapSize),
	true
    );
    rtVECTOR_CPD_PMapSize (pResult) = m_iPMRDCount;

/*****  ...Set the P-Map values...  *****/
    unsigned int xVectorOrigin = 0;
    rtVECTOR_PMRDType *pPMRD = rtVECTOR_CPD_PMap (pResult);
    for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
        rtVECTOR_PMRD_VectorOrigin (pPMRD) = xVectorOrigin;
	rtVECTOR_PMRD_SegmentIndex (pPMRD) = pPMRDC->SegmentIndex ();
	rtVECTOR_PMRD_SegmentOrigin(pPMRD) = pPMRDC->SegmentOrigin ();

	pPMRD++;

	xVectorOrigin += pPMRDC->SegmentLength ();
    }

    /** store the final size **/
    rtVECTOR_PMRD_VectorOrigin	(pPMRD) = xVectorOrigin;
    rtVECTOR_PMRD_SegmentIndex	(pPMRD) =
    rtVECTOR_PMRD_SegmentOrigin (pPMRD) = -1;

/***** Make sure that it has an 'undefined' u-segment... *****/
    M_KOTE const &rNA = pResult->TheNAClass ();
    LocateOrAddVectorSegment (
	pResult, rNA, RTYPE_C_UndefUV, rNA.PTokenCPD (), -1, false
    );

    rtVECTOR_CPD_IsInconsistent (pResult) = false;
    if (discarding)
	release ();

    return pResult;
}


/*****************************************
 *****************************************
 *****  Vector U-Segment Management  *****
 *****************************************
 *****************************************/

/*************************************************
 *****  U-Segment Array Expansion Utilities  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize new U-Segment Array space.
 *
 *  Arguments:
 *	vector			- a standard CPD for the vector just expanded.
 *	expansionIndex		- the index of a pointer in 'vector' which
 *				  points to the beginning of the new region.
 *	expansionBytes		- the number of bytes added to the region.
 *	ap			- a <varargs.h> argument pointer which points
 *				  to nothing.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitNewUSegmentArraySpace (
    VContainerHandle* vector, pointer_t pTail, ptrdiff_t expansionBytes, va_list Unused(ap)
) {
    vector->constructReferences ((M_POP*)pTail, (size_t)expansionBytes / sizeof (M_POP));
}


/*---------------------------------------------------------------------------
 *****  Internal procedure to expand the U-Segment array of a vector.
 *
 *  Arguments:
 *	vector			- a standard CPD for the vector to be updated.
 *	growthIncrement		- the amount to grow the u-segment array.
 *
 *****/
PrivateFnDef void ExpandUSegArray (M_CPD *vector, size_t growthIncrement) {
    USegArrayExpansionCount++;

    rtVECTOR_CPD_USD (vector) = rtVECTOR_CPD_USegArray (vector)
	+ rtVECTOR_CPD_USegArraySize (vector);

    growthIncrement = (growthIncrement + rtVECTOR_V_USegArrayGrowthInc - 1)
	/ rtVECTOR_V_USegArrayGrowthInc
	* rtVECTOR_V_USegArrayGrowthInc;

    vector->ShiftContainerTail (
	rtVECTOR_CPx_USD,
	rtVECTOR_CPD_USegIndexSize (vector) * sizeof (int),
	growthIncrement * sizeof (rtVECTOR_USDType),
	false,
	InitNewUSegmentArraySpace
    );

    rtVECTOR_CPD_USegArraySize (vector) += growthIncrement;
    if (TracingUSDCAllocator)
	IO_printf ("ExpandUSegArray by:%u\n",growthIncrement);
}


/*---------------------------------------------------------------------------
 *****  Internal routine to allocate a new vector U-Segment.
 *
 *  Arguments:
 *	vector			- a standard CPD for the vector in which the
 *				  new U-Segment is to be allocated.
 *
 *  Returns:
 *	The index of the U-Segment allocated.
 *
 *****/
PrivateFnDef int AllocateVectorUSegment (M_CPD *vector) {
    int				addLocation, uSegArraySize;
    rtVECTOR_USDType		*uSegArrayPtr;

/*****  Return the first free slot in the current U-Segment array...  *****/
    for (
	uSegArraySize	= rtVECTOR_CPD_USegArraySize (vector),
        uSegArrayPtr	= rtVECTOR_CPD_USegArray (vector) + (
	    addLocation = rtVECTOR_CPD_USegFreeBound (vector)
	);
	addLocation < uSegArraySize;
	addLocation++, uSegArrayPtr++
    )
    {
	if (rtVECTOR_USD_IsFree (vector, uSegArrayPtr)) {
	    vector->EnableModifications ();
	    rtVECTOR_CPD_USegFreeBound (vector) = addLocation;
	    return addLocation;
	}
    }

/*****  ...or, if no free locations were found, grow the array...  *****/
    if (TracingUSDCAllocator)
	IO_printf ("AllocateVectorUSegment: Call ExpandUSegArray\n");
    ExpandUSegArray (vector, 1);

/*****  ...and return the allocated location.  *****/
    return addLocation;
}


/******************************************************
 *****  Target U-Segment Locate/Update Utilities  *****
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to locate a U-Segment in a Vector.
 *
 *  Arguments:
 *	vector		    	- a standard CPD for the vector to search.
 *	vStoreCPD		- a standard CPD for the store of the segment
 *				  to locate.
 *	uSegment		- an address whose contents will be set to
 *				  either the position of the U-Segment in the
 *				  U-Segment Array if the U-Segment was found
 *				  or desired position of the U-Segment in the
 *				  U-Segment Index if the U-Segment was not
 *				  found.
 *	pStoreIdentity		- the optional address at which the 'vector'
 *				  relative local identity of 'vStoreCPD' will
 *				  be stored.  If NULL is supplied as the value
 *				  of this argument, '*uSegment' will be set
 *				  only if the store was found in vector.  If
 *				  a non-null address is supplied as the value
 *				  of this parameter, both '*uSegment' and
 *				  '*pStoreIdentity' will always be set.  These
 *				  conventions eliminate the need to ask for
 *				  the local identity if it isn't needed or
 *				  more than once if it is.
 *
 *  Returns:
 * 	true if the U-Segment was found, false otherwise.
 *
 *  Notes:
 *	In the interest of efficiency, this routine breaks the POP rule by
 *	copying into a private variable the POP for the value store for
 *	which it is searching.  The use of that copied POP is restricted in
 *	scope - no intervening actions invalid the POP and it is not used
 *	for reference - only identity.
 *
 *****/
PrivateFnDef bool LocateVectorSegment (
    M_CPD	*vector,
    M_CPD	*vStoreCPD,
    int		*uSegment,
    M_POP	*pStoreIdentity = NilOf (M_POP *)
) {
    //  Obtain the store's alias in the vector's database ...
    M_TOP iStoreTOP;
    M_POP iStorePOP;
    if (pStoreIdentity) {
	vector->LocateOrAddNameOf (vStoreCPD, iStoreTOP);
	iStoreTOP.getPOP (iStorePOP);
	*pStoreIdentity = iStorePOP;
    }
    else if (vector->LocateNameOf (vStoreCPD, iStoreTOP))
	iStoreTOP.getPOP (iStorePOP);
    else
	return false;

    rtVECTOR_Type	const * vectorBase	= rtVECTOR_CPD_Base (vector);
    rtVECTOR_USDType	const * uSegArray	= rtVECTOR_V_USegArray (vectorBase);
    int			const * uSegIndex	= rtVECTOR_V_USegIndex (vectorBase);
    unsigned int		uSegIndexSize	= rtVECTOR_V_USegIndexSize (vectorBase);

///////////////////////////////////////////////////////////////////////
// If the store was forwarded, we may have one of its aliases
// as a usegment store, so we have to do a linear search...
///////////////////////////////////////////////////////////////////////
    if (vStoreCPD->isAForwardingTarget ()) {
	USegIndexLSearchCount++;
	unsigned int xUSXInsert = 0;
	for (unsigned int xUSXElement = 0; xUSXElement < uSegIndexSize; xUSXElement++) {
	    unsigned int uSegArrayLoc = uSegIndex [xUSXElement];
	    M_POP iUSegStorePOP = rtVECTOR_USD_VStore (uSegArray + uSegArrayLoc);
	    if (vector->ReferencesAreIdentical (&iUSegStorePOP, &iStorePOP)) {
/*****  Found the store, return its U-Segment Array location ...  *****/
		USegIndexLSearchComparisons += xUSXElement + 1;
		USegIndexHitCount++;
		*uSegment = uSegArrayLoc;
		return true;
	    }
	    if (M_ComparePOPs (iUSegStorePOP, iStorePOP) < 0)
		xUSXInsert = xUSXElement + 1;
	}
/*****  Not found...  *****/
	USegIndexLSearchComparisons += uSegIndexSize;
	USegIndexMissCount++;
	*uSegment = xUSXInsert;
	return false;
    }

// Otherwise, perform a binary search ...
    int lowerSearchBound = (-1);
    int upperSearchBound = uSegIndexSize;
    while (upperSearchBound - lowerSearchBound > 1) {
	int uSegIndexLoc = (upperSearchBound + lowerSearchBound) / 2;
	int uSegArrayLoc = uSegIndex [uSegIndexLoc];
	int iComparisonResult = M_ComparePOPs (
	    rtVECTOR_USD_VStore (uSegArray + uSegArrayLoc), iStorePOP
	);
	if (iComparisonResult == 0) {
/*****
 * Found the value store, return its U-Segment Array location ...
 *****/
	    USegIndexHitCount++;
	    *uSegment = uSegArrayLoc;
	    return true;
	}
	if (iComparisonResult < 0)
	    lowerSearchBound = uSegIndexLoc;
	else
	    upperSearchBound = uSegIndexLoc;
    }
/*****  Not found...  *****/
    USegIndexMissCount++;
    *uSegment = upperSearchBound;
    return false;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to insure that a vector owns a U-Segment of a
 *****  specified type.
 *
 *  Arguments:
 *	targetVector		- a standard CPD for the vector.
 *	store			- a standard CPD for the u-segment's store.
 *	xRType			- the R-Type of any U-Vector created in the
 *				  'add' logic of this routine.
 *	rptRefCPD/Index		- the reference P-Token of any U-Vector created
 *				  in the 'add' logic of this routine.
 *	set			- the 'isASet' flag of any U-Vector created in
 *				  the 'add' logic of this routine.
 *
 *  Returns:
 *	The index of the U-Segment in the vector's USD array.
 *
 *****/
PrivateFnDef int LocateOrAddVectorSegment (
    M_CPD			*targetVector,
    M_CPD			*store,
    RTYPE_Type			xRType,
    M_CPD			*rptRefCPD,
    int				rptRefIndex,
    bool			set
)
{
/*
 *---------------------------------------------------------------------------
 * PART I: 'LocateOrAddVectorSegment'
 *	Determine if an appropriate U-Segment is already present.
 *---------------------------------------------------------------------------
 */
    int uSegIndexLoc; M_POP iStoreIdentity;
    if (LocateVectorSegment (targetVector, store, &uSegIndexLoc, &iStoreIdentity))
        return uSegIndexLoc;	/*****  U-Segment was Found ... *****/


/*
 *---------------------------------------------------------------------------
 * PART II: 'LocateOrAddVectorSegment'
 *	Add the U-Segment if it wasn't found.
 *---------------------------------------------------------------------------
 */
/*****  Create the new P-Token...  *****/
    M_CPD *newPToken = rtPTOKEN_New (targetVector->Space (), 0);

/*****  Create the new uvector... *****/
    M_CPD *newValues = NewUVector (xRType, newPToken, rptRefCPD, rptRefIndex);

    if (set)
	UV_CPD_IsASetUV (newValues) = true;

/*****  ...enable modification of the vector, ...  *****/
    targetVector->EnableModifications ();
    rtVECTOR_CPD_IsInconsistent (targetVector) = true;

/*****  ...obtain and initialize a new USD Array slot for them...  *****/
    int uSegArrayLoc = AllocateVectorUSegment (targetVector);
    InitUSegmentArrayEntry (
	targetVector, uSegArrayLoc, store, newValues, newPToken
    );

    newPToken->release ();
    newValues->release ();

/*****  ...add the new U-Segment to the U-Segment Index...  *****/
    rtVECTOR_CPD_USI (targetVector) = rtVECTOR_CPD_USegIndex (targetVector) + uSegIndexLoc;
    targetVector->ShiftContainerTail (
	rtVECTOR_CPx_USI,
	(rtVECTOR_CPD_USegIndexSize (targetVector) - uSegIndexLoc) * sizeof (int),
	sizeof (int),
	true
    );
    *(rtVECTOR_CPD_USI (targetVector) - 1) = uSegArrayLoc;
    rtVECTOR_CPD_USegIndexSize (targetVector)++;

/*****  ...return the USD's U-Segment array location field...  *****/
    return uSegArrayLoc;
}


/****************************************
 ****************************************
 *****  Vector Maintenance Utility  *****
 ****************************************
 ****************************************/

PrivateVarDef bool ImplicitPruningOfUSegmentsEnabled = true;
PrivateVarDef unsigned int USegmentCountThreshold = rtVECTOR_V_USegArrayInitialSize;

PublicFnDef void rtVECTOR_SetImplicitCleanupFlag (bool flag) {
    if (flag && !ImplicitPruningOfUSegmentsEnabled)
	PS_DisableCommits (flag);
    ImplicitPruningOfUSegmentsEnabled = flag;
}

PublicFnDef void rtVECTOR_SetUSegmentCountThreshold (unsigned int count) {
    if (count >= rtVECTOR_V_USegArrayInitialSize) USegmentCountThreshold = count;
}

///////
///  Private Function to remove any usegments not holding values for the vector.
///
///  \param	pVector - the vector to be modified
///  \param     pUSegmentRelocationMap - an array of integers. The size of the relocation map must equal the size of
///					 pVector's usegment array with each element in the relocation map identifying
///					 the new (possibly unchanged) location of its corresponding element in the
///					 usegment array. An element in the relocation equaling -1 indicates
///					 that the corresponding element in the usegment array is to be eliminated ...
///  \param	sUSegmentRelocationMap - the size of the relocation map. It must be less than or equal to the size of
///					 pVector's usegment array.
///
///////
PrivateFnDef void RemoveEmptyUSegments (M_CPD *pVector, int *pUSegmentRelocationMap, int sUSegmentRelocationMap) {
    int sUSegmentArray = rtVECTOR_CPD_USegArraySize (pVector);
/*****  Check Arguments for Consistency ....  *****/
    if (sUSegmentRelocationMap >  sUSegmentArray) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"RemoveEmptyUSegments: Map/Array size mismatch (%u:%u)",
		sUSegmentRelocationMap,
		sUSegmentArray
	    )
	);
    }

    pVector->EnableModifications ();
    rtVECTOR_CPD_IsInconsistent (pVector) = true;
    if ((unsigned int) sUSegmentArray > MaxUSegmentCountEncountered) MaxUSegmentCountEncountered = sUSegmentArray;
    bool fCompactingUSegments = USegmentCountThreshold < (unsigned int) sUSegmentArray;
    if (TracingUSDCAllocator)
	IO_printf ("RemoveEmptyUSegments: (%u:%u:%u)\n",
		   sUSegmentRelocationMap, USegmentCountThreshold, sUSegmentArray);

    if (fCompactingUSegments) {
	if (TracingUSDCAllocator)
	    IO_printf ("RemoveEmptyUSegments: rebuild pmap(%u:%u)\n",
		       sUSegmentRelocationMap, sUSegmentArray);
      /*****  ... rebuild the P-Map:  *****/
	rtVECTOR_PMRDType *pMapP, *pMapL;
	for (
	     pMapP = rtVECTOR_CPD_PMap (pVector),
	       pMapL = pMapP + rtVECTOR_CPD_PMapSize (pVector);
	     pMapP < pMapL;
	     pMapP++
	 ) rtVECTOR_PMRD_SegmentIndex (pMapP) =
	     pUSegmentRelocationMap[rtVECTOR_PMRD_SegmentIndex (pMapP)];
	USegmentCompactionCount++;
    }

/*****  ... rebuild the U-Segment Array:  *****/
    int uSegmentRIndex, uSegmentWIndex, newUSegmentCount;
    for (uSegmentRIndex = newUSegmentCount = 0;
	 uSegmentRIndex < sUSegmentRelocationMap;
	 uSegmentRIndex++ )
    {
	uSegmentWIndex = pUSegmentRelocationMap[uSegmentRIndex];
	if (uSegmentWIndex < 0) {
	    if (uSegmentRIndex < rtVECTOR_CPD_USegFreeBound (pVector))
		rtVECTOR_CPD_USegFreeBound (pVector) = uSegmentRIndex;
	    rtVECTOR_CPD_USD (pVector) =
		rtVECTOR_CPD_USegArray (pVector) + uSegmentRIndex;
	    if (pVector->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (pVector)))
		continue;  // already deleted ....
	    rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_PToken);

	    pVector->ClearReferences (rtVECTOR_CPx_USDCursor, rtVECTOR_USD_POPsPerUSD);
	    USegmentDeletionCount++;
	    continue;
	}

	newUSegmentCount++;

	if (uSegmentWIndex == uSegmentRIndex)
	    continue;
	if (fCompactingUSegments) {
	    rtVECTOR_USDType usd = rtVECTOR_CPD_USegArray (pVector)[uSegmentWIndex];
	    rtVECTOR_CPD_USegArray (pVector)[uSegmentWIndex] =
	      rtVECTOR_CPD_USegArray (pVector)[uSegmentRIndex];
	    rtVECTOR_CPD_USegArray (pVector)[uSegmentRIndex] = usd;
	}
    }
    if (fCompactingUSegments) {
	if (TracingUSDCAllocator)
	    IO_printf ("RemoveEmptyUSegments: compacting usegments(%u:%u:%u)\n",
			sUSegmentRelocationMap, sUSegmentArray,newUSegmentCount);
	rtVECTOR_CPD_USegFreeBound (pVector) = newUSegmentCount;
	if (newUSegmentCount < rtVECTOR_V_USegArrayInitialSize)
	    newUSegmentCount = rtVECTOR_V_USegArrayInitialSize;
	rtVECTOR_CPD_USI (pVector) = rtVECTOR_CPD_USegIndex (pVector);
	pVector->ShiftContainerTail (
	    rtVECTOR_CPx_USI,
	    rtVECTOR_CPD_USegIndexSize (pVector) * sizeof (int),
	    ((ptrdiff_t)newUSegmentCount - sUSegmentArray) * sizeof(rtVECTOR_USDType),
	    true
	);
	rtVECTOR_CPD_USegArraySize (pVector) = newUSegmentCount;
    }

/*****  ... rebuild the U-Segment Index:  *****/
    int uSegmentCount = rtVECTOR_CPD_USegIndexSize (pVector);
    for ( uSegmentRIndex = uSegmentWIndex = 0;
	 uSegmentRIndex < uSegmentCount;
	 uSegmentRIndex++ )
    {
	int index =
	    pUSegmentRelocationMap [rtVECTOR_CPD_USegIndex (pVector)[uSegmentRIndex]];
	if (index >= 0)
	    rtVECTOR_CPD_USegIndex (pVector)[uSegmentWIndex++] =
	      fCompactingUSegments ? index : rtVECTOR_CPD_USegIndex (pVector)[uSegmentRIndex];
    }

    if (uSegmentWIndex != uSegmentCount) {
	rtVECTOR_CPD_USI (pVector) = rtVECTOR_CPD_USegIndex (pVector) + uSegmentCount;
	pVector->ShiftContainerTail (
	    rtVECTOR_CPx_USI,
	    0,
	    ((ptrdiff_t)uSegmentWIndex - uSegmentCount) * sizeof(int),
	    true
	);
	rtVECTOR_CPD_USegIndexSize (pVector) = uSegmentWIndex;
    }
/*****  ... and mark the Vector as valid:  *****/
    rtVECTOR_CPD_IsInconsistent (pVector) = false;
}

/*********************************************
 *********************************************
 *****  Vector Reconstruction Utilities  *****
 *********************************************
 *********************************************/

/*******************************************
 *****  Constants, Types, and Globals  *****
 *******************************************/

/******************************
 *  P-Map Reconstructor Type  *
 ******************************/

/*----  Constants  ----*/
#define PMapReconstructorArraySize 40 // 1024

/*----  Type Definition  ----*/
class PMapReconstructorType : public VTransient {
//  State
public:
    PMapReconstructorType	*next;
    unsigned int		numberOfSlotsUsed;
    rtVECTOR_PMRDType		*currentPMRD,
				pmrdArray [PMapReconstructorArraySize];
};

/*----  Access Macros  ----*/
#define PMapReconstructorNext(p)		((p)->next)
#define PMapReconstructorNumberOfSlots(p)	((p)->numberOfSlotsUsed)
#define PMapReconstructorCurrentPMRD(p)		((p)->currentPMRD)
#define PMapReconstructorPMRDArray(p)		((p)->pmrdArray)


/**********************************
 *  U-Segment Reconstructor Type  *
 **********************************/

/*----  Type Definition  ----*/
struct USegReconstructorType {
    unsigned int		isNew		:  1,
				reverseIndex	: 31;
    M_POP			pop;
    int				oldPosition,
				newPosition;
    rtPTOKEN_CType		*ptc;
    union source_t {
	void			*source;
	DSC_Descriptor		*descriptor;
	rtVECTOR_USDC		*usdc;
    }				source;
    union subscript_t {
	int			offset;
	rtLINK_CType		*lc;
    }				subscript;
};

/*----  Access Macros  ----*/
#define USegReconstructorIsNew(p)		((p)->isNew)
#define USegReconstructorReverseIndex(p)	((p)->reverseIndex)
#define USegReconstructorPOP(p)			((p)->pop)
#define USegReconstructorOldPosition(p)		((p)->oldPosition)
#define USegReconstructorNewPosition(p)		((p)->newPosition)
#define USegReconstructorPTC(p)			((p)->ptc)
#define USegReconstructorSource(p)		((p)->source.source)
#define USegReconstructorDescriptor(p)		((p)->source.descriptor)
#define USegReconstructorUSDC(p)		((p)->source.usdc)
#define USegReconstructorOffset(p)		((p)->subscript.offset)
#define USegReconstructorLC(p)			((p)->subscript.lc)


/*******************************
 *  Vector Reconstructor Type  *
 *******************************/

/*----  Type Definition  ----*/
enum ReconstructorPurpose {
    Alignment,
    RfDsAssignment,
    LcDsAssignment,
    LcVcAssignment
};

class VectorReconstructor {
//  Construction
protected:
    void construct (ReconstructorPurpose xPurpose, M_CPD *pVector, int sUSegArray);

public:
    static VectorReconstructor *NewReconstructor (
	ReconstructorPurpose xPurpose, M_CPD *pVector, int maxUSegArrayExpansion
    );

//  Access
public:
    USegReconstructorType *USegArray () {
	return m_iUSegArray;
    }

//  Query
public:
    unsigned int ThisIsTheFirstAdjustment () const {
	return m_cPMRDs == 0;
    }
    unsigned int ThisIsntTheFirstAdjustment () const {
	return m_cPMRDs != 0;
    }

//  Segment Location
public:
    int LocateReconstructorSegment (
	M_CPD *store, int *uSegment, M_POP *pStoreIdentity = 0
    );
    int LocateOrAddReconstructorSegment (M_CPD *store);

    int LocateOrAddReconstructorSegmentForRfDsAssignment (DSC_Descriptor *sourceDescriptor);
    int LocateOrAddReconstructorSegmentForLcDsAssignment (
	DSC_Descriptor *sourceDescriptor, M_CPD *sourcePPT
    );
    int LocateOrAddReconstructorSegmentForLcVcAssignment (rtVECTOR_USDC *sourceUSDC);

//  PMap Maintenance
protected:
    void AppendPMapEntryToReconstructor (int segment, int count);

public:
/*---------------------------------------------------------------------------
 *****  Internal routine to append an entry to a reconstructor's P-Map chain
 *
 *  Arguments:
 *	segment			- the new P-Map entry's segment index.
 *	count			- the new P-Map entry's count.
 *	phase0			- flag which when true means no pmap entries
 *				  have been appended which were not already
 *				  present.
 *
 *  Returns:
 *	'reconstructor'
 *
 *****/
    void AppendPMapEntryToReconstructor (int segment, int count, bool phase0) {
	if (phase0) {
	    m_xCurrentVectorOrigin += count;
	    if (m_cPMRDs++)
		m_pPMRD++;
	}
	else {
	    AppendPMapEntryToReconstructor (segment, count);
	}
    }

//  P-Token Maintenance
public:
    M_CPD *NewPToken (unsigned int nElements) const {
	return rtPTOKEN_New (m_pVector->Space (), nElements);
    }

    void AdjustUSegReconstructorPToken (int segment, int count) {
	USegReconstructorType *uSegReconstructor = m_iUSegArray + segment;
	if (USegReconstructorIsNew (uSegReconstructor));
	else {
	    rtPTOKEN_CType *pTokenC = USegReconstructorPTC (uSegReconstructor);
	    if (IsNil (pTokenC)) {
		rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + segment;
		pTokenC = USegReconstructorPTC (uSegReconstructor) =
		    rtPTOKEN_NewShiftPTConstructor (m_pVector, rtVECTOR_CPx_USD);
	    }
	    pTokenC->AppendAdjustment (
		USegReconstructorOldPosition (uSegReconstructor), count
	    );
	}
	USegReconstructorOldPosition (uSegReconstructor) += count;
    }

//  Vector Reconstruction
public:
    void ReconstructVector ();

//  State
protected:
    ReconstructorPurpose	m_xPurpose;
    M_CPD			*m_pVector;
    M_CPD			*m_pCPD;
    int				m_sUSegArray,
				m_sUSegIndex,
				m_xUSegFreeBound,
				m_cUSegAdditions,
				m_xCurrentVectorOrigin,
				m_cPMRDs;
    rtVECTOR_PMRDType		*m_pPMRD;
    PMapReconstructorType	*m_pPMapChainHead,
				*m_pPMapChainTail;
    USegReconstructorType	m_iUSegArray[1];
};


/***************************
 *****  Instantiation  *****
 ***************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to create a new vector reconstructor.
 *
 *  Argument:
 *	purpose			- the purpose for the reconstructor.  One of:
 *					Alignment
 *					RfDsAssignment (RefAssignFromValueD)
 *					LcDsAssignment (LCAssignFromValueD)
 *					LcVcAssignment (LCAssignFromVC)
 *	vector			- a standard CPD for the vector which is to
 *				  be rebuilt using this constructor.  This
 *				  CPD must not be freed until AFTER the
 *				  reconstructor has completed its work.
 *	maxUSegArrayExpansion	- the maximum number of new u-segments that
 *				  may be added to this reconstructor.  The
 *				  reconstructor will be allocated to support
 *				  up to this number of additional segments.
 *
 *  Returns:
 *	The address of the reconstructor created.
 *
 *****/
void VectorReconstructor::construct (
    ReconstructorPurpose xPurpose, M_CPD *pVector, int sUSegArray
) {
    pVector->EnableModifications ();

    m_xPurpose			= xPurpose;
    m_pVector			= pVector;
    m_pCPD			= pVector->GetCPD ();
    m_pPMRD			= rtVECTOR_CPD_PMap (m_pCPD);
    m_sUSegArray		= rtVECTOR_CPD_USegArraySize (pVector);
    m_sUSegIndex		= rtVECTOR_CPD_USegIndexSize (pVector);
    m_xUSegFreeBound		= rtVECTOR_CPD_USegFreeBound (pVector);
    m_cUSegAdditions		=
    m_xCurrentVectorOrigin	=
    m_cPMRDs			= 0;
    m_pPMapChainHead		=
    m_pPMapChainTail		= NilOf (PMapReconstructorType *);

    for (
	USegReconstructorType *uSegPtr = m_iUSegArray + sUSegArray - 1;
        uSegPtr >= m_iUSegArray;
	uSegPtr--
    ) {
	USegReconstructorIsNew		(uSegPtr) = false;
	USegReconstructorOldPosition	(uSegPtr) =
	USegReconstructorNewPosition	(uSegPtr) = 0;
	USegReconstructorPTC		(uSegPtr) = NilOf (rtPTOKEN_CType*);
	USegReconstructorSource		(uSegPtr) = NilOf (void *);
	USegReconstructorLC		(uSegPtr) = NilOf (rtLINK_CType*);
    }
}

VectorReconstructor* VectorReconstructor::NewReconstructor (
    ReconstructorPurpose purpose, M_CPD *pVector, int maxUSegArrayExpansion
) {
    int sUSegArray = rtVECTOR_CPD_USegArraySize (pVector) + maxUSegArrayExpansion;
    VectorReconstructor *result = (VectorReconstructor *)UTIL_Malloc (
	sizeof (VectorReconstructor) + sizeof (USegReconstructorType) * (sUSegArray - 1)
    );
    result->construct (purpose, pVector, sUSegArray);
    return result;
}


/********************************
 *****  Segment Management  *****
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to locate a U-Segment in a Reconstructor.
 *
 *  Arguments:
 *	store			- a standard CPD for the store to locate.
 *	uSegment		- an address whose contents will be set to
 *				  either the position of the U-Segment in the
 *				  U-Segment Array if the U-Segment was found
 *				  or desired position of the U-Segment in the
 *				  U-Segment Index if the U-Segment was not
 *				  found.
 *
 *  Returns:
 * 	true if the U-Segment was found, and false otherwise.
 *
 *  Notes:
 *	In the interest of efficiency, this routine breaks the POP rule by
 *	copying into a private variable the POP for the value store for
 *	which it is searching.  The use of that copied POP is restricted in
 *	scope - no intervening actions invalid the POP and it is not used
 *	for reference - only identity.
 *
 *****/
int VectorReconstructor::LocateReconstructorSegment (
    M_CPD *store, int *uSegment, M_POP *pStoreIdentity
) {
    if (0 == m_cUSegAdditions)
	return LocateVectorSegment (m_pVector, store, uSegment, pStoreIdentity);

    //  Obtain the store's alias in the vector's database ...
    M_TOP iStoreTOP;
    M_POP iStorePOP;
    if (pStoreIdentity) {
	m_pVector->LocateOrAddNameOf (store, iStoreTOP);
	iStoreTOP.getPOP (iStorePOP);
	*pStoreIdentity = iStorePOP;
    }
    else if (m_pVector->LocateNameOf (store, iStoreTOP))
	iStoreTOP.getPOP (iStorePOP);
    else
	return false;

///////////////////////////////////////////////////////////////////////
// If the store was forwarded, we may have one of its aliases
// as a usegment store, so we have to do a linear search...
///////////////////////////////////////////////////////////////////////
    if (store->isAForwardingTarget ()) {
	USegIndexLSearchCount++;
	unsigned int xUSXInsert = 0;
	unsigned int sUSegIndex = m_sUSegIndex;
	for (unsigned int xUSXElement = 0; xUSXElement < sUSegIndex; xUSXElement++) {
	    unsigned int uSegArrayLoc = USegReconstructorReverseIndex (
		m_iUSegArray + xUSXElement
	    );
	    M_POP iUSegStorePOP = USegReconstructorPOP (m_iUSegArray + uSegArrayLoc);

	    if (m_pVector->ReferencesAreIdentical (&iUSegStorePOP, &iStorePOP)) {
/*****  Found the store, return its U-Segment Array location ...  *****/
		USegIndexLSearchComparisons += xUSXElement + 1;
		USegIndexHitCount++;
		*uSegment = uSegArrayLoc;
		return true;
	    }
	    if (M_ComparePOPs (iUSegStorePOP, iStorePOP) < 0)
		xUSXInsert = xUSXElement + 1;
	}
/*****  Not found...  *****/
	USegIndexLSearchComparisons += sUSegIndex;
	USegIndexMissCount++;
	*uSegment = xUSXInsert;
	return false;
    }

//  Otherwise, perform a binary search ...
    int lowerSearchBound = (-1);
    int upperSearchBound = m_sUSegIndex;

    while (upperSearchBound - lowerSearchBound > 1) {
	int uSegIndexLoc = (upperSearchBound + lowerSearchBound) / 2;
	int uSegArrayLoc = USegReconstructorReverseIndex(m_iUSegArray + uSegIndexLoc);
	int iComparisonResult = M_ComparePOPs (
	    USegReconstructorPOP (m_iUSegArray + uSegArrayLoc), iStorePOP
	);
	if (iComparisonResult == 0) {
/*****
 * Found the value store, return its U-Segment Array location ...
 *****/
	    USegIndexHitCount++;
	    *uSegment = uSegArrayLoc;
	    return true;
	}
	if (iComparisonResult < 0)
	    lowerSearchBound = uSegIndexLoc;
	else
	    upperSearchBound = uSegIndexLoc;
    }
/*****  Not found...  *****/
    USegIndexMissCount++;
    *uSegment = upperSearchBound;
    return false;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to insure that a reconstructor owns a U-Segment of a
 *****  specified type.
 *
 *  Arguments:
 *	store			- a standard CPD for the u-segment's store.
 *
 *  Returns:
 *	The index of the U-Segment in the reconstructor's USD array.
 *
 *
 *****/
int VectorReconstructor::LocateOrAddReconstructorSegment (M_CPD *store) {
/*
 *---------------------------------------------------------------------------
 * PART I: 'LocateOrAddReconstructorSegment'
 *	Determine if an appropriate U-Segment is already present.
 *---------------------------------------------------------------------------
 */
    int uSegIndexLoc; M_POP iStoreIdentity;
    if (LocateReconstructorSegment (store, &uSegIndexLoc, &iStoreIdentity))
	return uSegIndexLoc;	/*****  U-Segment was Found ... *****/


/*
 *---------------------------------------------------------------------------
 * PART II: 'LocateOrAddReconstructorSegment'
 *	Add the U-Segment if it wasn't found.
 *---------------------------------------------------------------------------
 */

/*****  Add the segment to the reconstructor's U-Segment array...  *****/
    rtVECTOR_USDType *vectorUSegArray = rtVECTOR_CPD_USegArray (m_pVector);
    int uSegArrayLoc;
    for (uSegArrayLoc = m_xUSegFreeBound; uSegArrayLoc < m_sUSegArray; uSegArrayLoc++) {
	if (rtVECTOR_USD_IsFree (m_pVector, vectorUSegArray + uSegArrayLoc))
	    break;
    }
    if (uSegArrayLoc >= m_sUSegArray)
	m_sUSegArray = uSegArrayLoc + 1;
    m_xUSegFreeBound = uSegArrayLoc + 1;

/*****  Add the segment to the reconstructor's U-Segment index...  *****/
    if (m_cUSegAdditions > 0) {
	int i;
	for (i = m_sUSegIndex; i > uSegIndexLoc; i--)
	    USegReconstructorReverseIndex (m_iUSegArray + i) =
	    USegReconstructorReverseIndex (m_iUSegArray + (i - 1));
	USegReconstructorReverseIndex (m_iUSegArray + i) = uSegArrayLoc;
    }
    else {
//	vectorUSegArray = rtVECTOR_CPD_USegArray (m_pVector);
	int i;
	for (i = 0; i < rtVECTOR_CPD_USegArraySize (m_pVector); i++)
	    USegReconstructorPOP (m_iUSegArray + i) = rtVECTOR_USD_VStore (vectorUSegArray + i);

	int const *vectorUSegIndex = rtVECTOR_CPD_USegIndex (m_pVector);
	m_sUSegIndex = rtVECTOR_CPD_USegIndexSize (m_pVector);
	for (i = m_sUSegIndex; i > uSegIndexLoc; i--)
	    USegReconstructorReverseIndex(m_iUSegArray + i) = vectorUSegIndex[i - 1];
	USegReconstructorReverseIndex (m_iUSegArray + i) = uSegArrayLoc;
	while (i-- > 0)
	    USegReconstructorReverseIndex (m_iUSegArray + i) = vectorUSegIndex[i];
    }
    m_sUSegIndex++;
    m_cUSegAdditions++;

/*****  ... and update the u-segment reconstructor.  *****/
    USegReconstructorType *uSegReconstructor = m_iUSegArray + uSegArrayLoc;
    USegReconstructorIsNew	(uSegReconstructor) = true;
    USegReconstructorPOP	(uSegReconstructor) = iStoreIdentity;

    return uSegArrayLoc;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to insure that a reconstructor owns a U-Segment of a
 *****  specified type.
 *
 *  Arguments:
 *	sourceDescriptor	- the address of a descriptor containing
 *				  the values to be assigned to the segment.
 *
 *  Returns:
 *	The index of the Segment in the reconstructor's USD array.
 *
 *
 *****/
int VectorReconstructor::LocateOrAddReconstructorSegmentForRfDsAssignment (
    DSC_Descriptor *sourceDescriptor
) {
    int uSegArrayLocation = LocateOrAddReconstructorSegment (sourceDescriptor->storeCPD ());
    USegReconstructorType *uSegReconstructor = m_iUSegArray + uSegArrayLocation;

    USegReconstructorDescriptor	(uSegReconstructor) = sourceDescriptor;

    return uSegArrayLocation;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to insure that a reconstructor owns a U-Segment of a
 *****  specified type.
 *
 *  Arguments:
 *	sourceDescriptor	- the address of a descriptor containing
 *				  the values which will be assigned to
 *				  the new segment once it is created.
 *	sourcePPT		- a standard CPD for the descriptor's
 *				  positional P-Token.  Although this p-token
 *				  can be obtained from the descriptor, it is
 *				  already available in the caller.
 *
 *  Returns:
 *	The index of the Segment in the reconstructor's USD array.
 *
 *
 *****/
int VectorReconstructor::LocateOrAddReconstructorSegmentForLcDsAssignment (
    DSC_Descriptor *sourceDescriptor, M_CPD *sourcePPT
) {
    int uSegArrayLocation = LocateOrAddReconstructorSegment (sourceDescriptor->storeCPD ());
    USegReconstructorType *uSegReconstructor = m_iUSegArray + uSegArrayLocation;

    USegReconstructorDescriptor (uSegReconstructor) = sourceDescriptor;
    USegReconstructorLC		(uSegReconstructor) = rtLINK_PosConstructor (
	sourcePPT, -1
    );

    return uSegArrayLocation;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to insure that a reconstructor owns a U-Segment of a
 *****  specified type.
 *
 *  Arguments:
 *	sourceUSDC		- the address of a u-segment constructor
 *				  containing the values to be assigned to
 *				  the segment.
 *
 *  Returns:
 *	The index of the Segment in the reconstructor's USD array.
 *
 *
 *****/
int VectorReconstructor::LocateOrAddReconstructorSegmentForLcVcAssignment (
    rtVECTOR_USDC *sourceUSDC
)
{
    int uSegArrayLocation = LocateOrAddReconstructorSegment (sourceUSDC->storeCPD ());
    USegReconstructorType *uSegReconstructor = m_iUSegArray + uSegArrayLocation;

    USegReconstructorUSDC	(uSegReconstructor) = sourceUSDC;
    USegReconstructorLC		(uSegReconstructor) = rtLINK_PosConstructor (
	sourceUSDC->PPT (), -1
    );

    return uSegArrayLocation;
}


/********************************
 *****  P-Token Management  *****
 ********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to append an entry to a reconstructor's P-Map chain
 *
 *  Arguments:
 *	segment			- the new P-Map entry's segment index.
 *	count			- the new P-Map entry's count.
 *
 *  Returns:
 *	'reconstructor'
 *
 *****/
void VectorReconstructor::AppendPMapEntryToReconstructor (int segment, int count) {

#define OperatingInPhase1 (IsNil (pmapR))

#define InitPMapReconstructorChain(pmapR) {\
    pmapR = m_pPMapChainHead = m_pPMapChainTail = new PMapReconstructorType ();\
}

#define AppendToPMapReconstructorChain(pmapR) {\
    pmapR = m_pPMapChainTail = PMapReconstructorNext (pmapR) = new PMapReconstructorType ();\
}

#define InitPMapReconstructor(pmapR, pMapEntry) {\
    PMapReconstructorNext (pmapR) = NilOf (PMapReconstructorType *);\
    AllocatePMRecCount++;\
    PMapReconstructorNumberOfSlots (pmapR) = 1;\
    pMapEntry = PMapReconstructorCurrentPMRD (pmapR)\
	      = PMapReconstructorPMRDArray (pmapR);\
}

#define BeginPhase2(pmapR, pMapEntry) {\
    InitPMapReconstructorChain	(pmapR);\
    InitPMapReconstructor	(pmapR, pMapEntry);\
}


    rtVECTOR_PMRDType		*vectorPMRD;
    int				oldSegment;

    PMapReconstructorType *pmapR = m_pPMapChainTail;

    if (TracingReconstructor) IO_printf (
	"...AppendPMapEntryToReconstructor: VRC:%08X, USeg:%d, Size:%d\n",
	this, segment, count
    );

/*****  Skip empty requests...  *****/
    if (count <= 0)
	return;

/*---------------------------------------------------------------------------
 * If the reconstructor is in phase 1, set the 'pMapEntry' pointer to the
 * vector reconstructor's cpd pointer.
 * If the reconstructor is in phase 2, set 'pMapEntry' to the current PMRD in
 * temporary storage.
 *---------------------------------------------------------------------------
 */
    rtVECTOR_PMRDType *pMapEntry = OperatingInPhase1
	? m_pPMRD : PMapReconstructorCurrentPMRD (pmapR);

    if (segment == rtVECTOR_PMRD_SegmentIndex (pMapEntry)) {
    /*****  Append to the last segment if the segments match...  *****/
    /*****  Doesn't matter which phase we're in.		 *****/
	m_xCurrentVectorOrigin += count;
	if (ThisIsTheFirstAdjustment ())
	    m_cPMRDs = 1;
	return;
    }
    if (OperatingInPhase1) {


/*---------------------------------------------------------------------------
 * Phase 1: Rebuilding pmap in place.
 *---------------------------------------------------------------------------
 */
        if (pMapEntry == (vectorPMRD = rtVECTOR_CPD_PMRD (m_pVector))) {
/*---------------------------------------------------------------------------
 * The segments don't match, so the pMapEntry we have to write on is still
 * needed by whatever operation is driving the reconstruction, so we must
 * enter phase two of the reconstruction process.
 *---------------------------------------------------------------------------
 */
	    if (ThisIsntTheFirstAdjustment ())
	    /*---------------------------------------------------------------
	     * This entry has been committed, so we must increment the pointer
	     * at which the phase 2 constructed pmap entries will be copied.
	     * Otherwise,  the last pmap entry will be overwritten. As the
	     * mnemonic conditional indicates,  this is almost always the
	     * case.
	     *---------------------------------------------------------------
	     */
	        ++m_pPMRD;
	    BeginPhase2 (pmapR, pMapEntry);
	}
	else if (pMapEntry < vectorPMRD) {
	    /*---------------------------------------------------------------
	     * If this pmapEntry has been committed, increment the
	     * reconstructor's pointer for reasons similar to those given just
	     * above.
	     *---------------------------------------------------------------
	     */
	    pMapEntry = ThisIsTheFirstAdjustment () ? m_pPMRD : ++m_pPMRD;
	    if ((oldSegment = rtVECTOR_PMRD_SegmentIndex (pMapEntry)) < 0 ||
	        ( (pMapEntry == vectorPMRD) && (segment != oldSegment) )
	    )
	    /*---------------------------------------------------------------
	     *  If the pmap entry now points beyond the end of the pmap or
	     * points to the entry being examined by the calling operation and
	     * the new segment doesn't match that entry's segment, we need to
	     * begin phase 2.
	     *----------------------------------------------------------------
	     */
	    {
		BeginPhase2 (pmapR, pMapEntry);
	    }
	    else
	    /*----------------------------------------------------------------
	     * We are modifying the vector itself, so mark it as corrupted.
	     *----------------------------------------------------------------
	     */
		rtVECTOR_CPD_IsInconsistent (m_pCPD) = true;
	}
	else ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
"AppendPMapEntryToReconstructor: PMRD pointers out of order: New:%d, Old:%d",
		m_pPMRD, rtVECTOR_CPD_PMRD (m_pVector)
	    )
	);
    }


/*---------------------------------------------------------------------------
 * Phase 2:  Now using transient pmap reconstructors. Will not revert to
 * 	     phase 1.
 *---------------------------------------------------------------------------
 */
    else if (PMapReconstructorArraySize > PMapReconstructorNumberOfSlots (pmapR)) {
    /** If there is room, simply increment the pointer. The case of segment
     ** matching the last entry has been taken care of above.
     **/
        pMapEntry = ++PMapReconstructorCurrentPMRD (pmapR);
	PMapReconstructorNumberOfSlots (pmapR)++;
    }
    else {    /**  Allocate a new pmap reconstructor  **/
	AppendToPMapReconstructorChain (pmapR);
	InitPMapReconstructor (pmapR, pMapEntry);
    }

/*****  Initialize any new pmap entries...  *****/
    rtVECTOR_PMRD_SegmentIndex  (pMapEntry) = segment;
    rtVECTOR_PMRD_SegmentOrigin (pMapEntry) = USegReconstructorNewPosition (
	m_iUSegArray + segment
    );
    rtVECTOR_PMRD_VectorOrigin   (pMapEntry) = m_xCurrentVectorOrigin;
    m_xCurrentVectorOrigin += count;
    m_cPMRDs++;

    if (TracingReconstructor) IO_printf (
	"\t\tNew PMRC:%08X->%d:#%d[%d]\n",
	pMapEntry,
	rtVECTOR_PMRD_VectorOrigin  (pMapEntry),
	rtVECTOR_PMRD_SegmentIndex  (pMapEntry),
	rtVECTOR_PMRD_SegmentOrigin (pMapEntry)
    );

    return;

#undef OperatingInPhase1
#undef InitPMapReconstructorChain
#undef AppendToPMapReconstructorChain
#undef InitPMapReconstructor
#undef BeginPhase2
}


/****************************
 *****  Reconstruction  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to reconstruct a vector given a reconstructor.
 *
 *  Arguments:
 *	this			- the address of a vector reconstructor.  The
 *				  reconstructor will be freed by this routine.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Notes:
 *	This routine performs whatever work is required by the reconstructor.
 *	This can include adding new segments, rebuilding the vector's P-Map,
 *	and assigning new segment values.
 *
 *****/
void VectorReconstructor::ReconstructVector () {
    M_CPD			*uSegPToken,
				*uSegValues,
				*uSegStore;
    RTYPE_Type			uSegRType;
    rtPTOKEN_CType		*ptc;
    PMapReconstructorType	*pMapPtr,
				*nextPMapPtr;
    USegReconstructorType	*uSegReconstructor;
    int				*uSegIndex,
				uSegOffset,
				uSegLimit;

/*---------------------------------------------------------------------------*
 * Part 0: 'ReconstructVector'
 *	Cache the required reconstructor state.
 *---------------------------------------------------------------------------*/

    unsigned int newPMapSize = m_cPMRDs;
    unsigned int oldPMapSize = rtVECTOR_CPD_PMapSize (m_pVector);

    rtVECTOR_CPD_PMRD (m_pCPD) = m_pPMRD;


/*---------------------------------------------------------------------------*
 * Part 1: 'ReconstructVector'
 *	 Update the Segment structures.
 *---------------------------------------------------------------------------*/

    rtVECTOR_CPD_IsInconsistent (m_pCPD) = true;

/*****  If segments were added, ...  *****/
    if (m_cUSegAdditions > 0) {
    /*****  ... expand the vector's U-Segment index ...  *****/
	rtVECTOR_CPD_USI (m_pVector) = rtVECTOR_CPD_USegIndex (m_pVector)
	    + rtVECTOR_CPD_USegIndexSize (m_pVector);
	m_pVector->ShiftContainerTail (
	    rtVECTOR_CPx_USI, 0, sizeof (int) * m_cUSegAdditions, true
	);
	uSegIndex = rtVECTOR_CPD_USegIndex (m_pVector);
	uSegLimit = m_sUSegIndex;
	for (uSegOffset = 0; uSegOffset < uSegLimit; uSegOffset++) uSegIndex[
	    uSegOffset
	] = USegReconstructorReverseIndex (m_iUSegArray + uSegOffset);
	rtVECTOR_CPD_USegIndexSize (m_pVector) = m_sUSegIndex;

    /*****  ... and the vector's u-segment array (if necessary).  *****/
	if (m_sUSegArray > rtVECTOR_CPD_USegArraySize (m_pVector)) {
	    if (TracingUSDCAllocator)
		IO_printf ("ReconstructVector: Call ExpandUSegArray(%u:%u)\n",
			   m_sUSegArray,rtVECTOR_CPD_USegArraySize (m_pVector));
	    ExpandUSegArray (
		m_pVector, m_sUSegArray - rtVECTOR_CPD_USegArraySize (m_pVector)
	    );
	}
	rtVECTOR_CPD_USegFreeBound (m_pVector) = m_xUSegFreeBound;
    }

/*****  Perform the specified segment adjustments and updates...  *****/
    uSegLimit = m_sUSegArray;
    int *pUSegmentRelocationMap;
    int uSegRetainedCnt = 0;
    bool fUSegmentDeleted = false;

    if (ImplicitPruningOfUSegmentsEnabled) {
	if (TracingUSDCAllocator)
	    IO_printf ("ReconstructVector: allocate pUSegmentRelocationMap (%u:%u)\n",
		       m_sUSegArray,m_xPurpose);
	pUSegmentRelocationMap = (int *)UTIL_Malloc(sizeof (int) * m_sUSegArray);
    }
    switch (m_xPurpose) {


    case Alignment:
	for (uSegOffset = 0; uSegOffset < uSegLimit; uSegOffset++) {
	    uSegReconstructor = m_iUSegArray + uSegOffset;
	    if (ImplicitPruningOfUSegmentsEnabled) pUSegmentRelocationMap[uSegOffset] = uSegRetainedCnt++;
	    if (USegReconstructorIsNew (uSegReconstructor) &&
		(0 < USegReconstructorNewPosition (uSegReconstructor) || !ImplicitPruningOfUSegmentsEnabled)) {
	      // if there are no elements in this new usegment, only do this part of the creation if
	      // implicit pruning is turned off. It has already been added to the index array, so if this work
	      // is not done, then the vector will be left in an inconsistent state. The implicit pruning, if 
	      // enabled will clear up the inconsistency by removing this empty usegment from the index array.
		M_KOTE const &rNA = m_pVector->TheNAClass ();

		uSegPToken = NewPToken (USegReconstructorNewPosition (uSegReconstructor));
		uSegValues = NewUVector (RTYPE_C_UndefUV, uSegPToken, rNA.PTokenCPD () , -1);
		UV_CPD_IsASetUV (uSegValues) = (unsigned short)rtVECTOR_CPD_IsASet (m_pVector);
		InitUSegmentArrayEntry (m_pVector, uSegOffset, rNA, uSegValues, uSegPToken);
		uSegPToken->release ();
		uSegValues->release ();
	    }
	    else if (IsntNil (ptc = USegReconstructorPTC (uSegReconstructor))) {
		if (ImplicitPruningOfUSegmentsEnabled && 0 == rtPTOKEN_PTC_BaseCount (ptc)) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		    ptc->discard ();
		} else {
		    uSegPToken = ptc->ToPToken ();
		    rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		    rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		    m_pVector->StoreReference (rtVECTOR_CPx_USDCursor, uSegPToken);
		    uSegPToken->release ();
		}
	    }
	    else if (ImplicitPruningOfUSegmentsEnabled) {
	        rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		if (m_pVector->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (m_pVector))) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		}
	    }
	}
	break;


    case RfDsAssignment:
	for (uSegOffset = 0; uSegOffset < uSegLimit; uSegOffset++) {
	    uSegReconstructor	= m_iUSegArray + uSegOffset;
	    uSegValues		= NilOf (M_CPD*);
	    if (ImplicitPruningOfUSegmentsEnabled) pUSegmentRelocationMap[uSegOffset] = uSegRetainedCnt++;
	    if (USegReconstructorIsNew (uSegReconstructor)) {
	/***** ... implies 'IsntNil (USegReconstructorDescriptor(...))'  *****/
		uSegStore = USegReconstructorDescriptor(uSegReconstructor)->storeCPD ();
		uSegRType = USegReconstructorDescriptor(uSegReconstructor)->pointerRType ();

		M_CPD *uSegRPTRefCPD; int uSegRPTRefIndex;
		USegReconstructorDescriptor(uSegReconstructor)->getRPTReference (
		    uSegRPTRefCPD, uSegRPTRefIndex
		);
		uSegPToken = NewPToken (USegReconstructorNewPosition (uSegReconstructor));
		uSegValues = NewUVector (
		    uSegRType ,uSegPToken, uSegRPTRefCPD, uSegRPTRefIndex
		);
		InitUSegmentArrayEntry (
		    m_pVector, uSegOffset, uSegStore, uSegValues, uSegPToken
		);
		uSegRPTRefCPD->release ();
		uSegPToken->release ();
	    }
	    else if (IsntNil (ptc = USegReconstructorPTC (uSegReconstructor))) {
		if (ImplicitPruningOfUSegmentsEnabled && 0 == rtPTOKEN_PTC_BaseCount (ptc)) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		    ptc->discard ();
		} else {
		    uSegPToken = ptc->ToPToken ();
		    rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		    rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		    m_pVector->StoreReference (rtVECTOR_CPx_USDCursor, uSegPToken);
		    uSegPToken->release ();
		}
	    }
	    else if (ImplicitPruningOfUSegmentsEnabled) {
	        rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		if (m_pVector->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (m_pVector))) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		}
	    }

	    if (IsntNil (USegReconstructorDescriptor (uSegReconstructor))) {
		uSegValues = AtUVOffsetPutDescriptor (
		    uSegValues,
		    m_pVector,
		    uSegOffset,
		    USegReconstructorOffset (uSegReconstructor),
		    USegReconstructorDescriptor (uSegReconstructor)
		);
		uSegValues->release ();
	    }
	}
	break;


    case LcDsAssignment:
	for (uSegOffset = 0; uSegOffset < uSegLimit; uSegOffset++) {
	    uSegReconstructor	= m_iUSegArray + uSegOffset;
	    uSegPToken		=
	    uSegValues		= NilOf (M_CPD*);
	    if (ImplicitPruningOfUSegmentsEnabled) pUSegmentRelocationMap[uSegOffset] = uSegRetainedCnt++;
	    if (USegReconstructorIsNew (uSegReconstructor))
	    {
	/***** ... implies 'IsntNil (USegReconstructorDescriptor(...))'  *****/
		uSegStore = USegReconstructorDescriptor(uSegReconstructor)->storeCPD ();
		uSegRType = USegReconstructorDescriptor(uSegReconstructor)->pointerRType ();

		M_CPD *uSegRPTRefCPD; int uSegRPTRefIndex;
		USegReconstructorDescriptor(uSegReconstructor)->getRPTReference (
		    uSegRPTRefCPD, uSegRPTRefIndex
		);
		uSegPToken = NewPToken (USegReconstructorNewPosition (uSegReconstructor));
		uSegValues = NewUVector (
		    uSegRType ,uSegPToken, uSegRPTRefCPD, uSegRPTRefIndex
		);
		InitUSegmentArrayEntry (
		    m_pVector, uSegOffset, uSegStore, uSegValues, uSegPToken
		);
		uSegRPTRefCPD->release ();
	    }
	    else if (IsntNil (ptc = USegReconstructorPTC (uSegReconstructor))) {
		if (ImplicitPruningOfUSegmentsEnabled && 0 == rtPTOKEN_PTC_BaseCount (ptc)) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		    ptc->discard ();
		    USegReconstructorSource(uSegReconstructor) = NilOf (void *);
		    if (IsntNil (USegReconstructorLC (uSegReconstructor)))
			USegReconstructorLC (uSegReconstructor)->release ();
		} else {
		    uSegPToken = ptc->ToPToken ();
		    rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		    rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		    m_pVector->StoreReference (rtVECTOR_CPx_USDCursor, uSegPToken);
		}
	    }
	    else if (IsntNil (USegReconstructorDescriptor (uSegReconstructor))) {
		rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		uSegPToken = m_pVector->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);
	    }
	    else if (ImplicitPruningOfUSegmentsEnabled) {
	        rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		if (m_pVector->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (m_pVector))) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		}
	    }
	    if (IsntNil (USegReconstructorDescriptor (uSegReconstructor))) {
		USegReconstructorLC (uSegReconstructor)->Close (uSegPToken);
		uSegValues = AtUVLinkCPositionsPutDescriptor (
		    uSegValues,
		    m_pVector,
		    uSegOffset,
		    USegReconstructorLC (uSegReconstructor),
		    USegReconstructorDescriptor (uSegReconstructor)
		);
		USegReconstructorLC (uSegReconstructor)->release ();
		uSegValues->release ();
	    }
	    if (uSegPToken)
		uSegPToken->release ();
	}
	break;


    case LcVcAssignment:
	for (uSegOffset = 0; uSegOffset < uSegLimit; uSegOffset++) {
	    uSegReconstructor	= m_iUSegArray + uSegOffset;
	    uSegPToken		=
	    uSegValues		= NilOf (M_CPD*);
	    if (ImplicitPruningOfUSegmentsEnabled) pUSegmentRelocationMap[uSegOffset] = uSegRetainedCnt++;
	    if (USegReconstructorIsNew (uSegReconstructor)) {
	/***** ... implies 'IsntNil (USegReconstructorUSDC(...))'  *****/
		M_CPD *sourceValues = USegReconstructorUSDC (uSegReconstructor)->pointerCPD ();
		uSegPToken = NewPToken (USegReconstructorNewPosition (uSegReconstructor));
		uSegValues = NewUVector (
		    (RTYPE_Type)M_CPD_RType (sourceValues),
		    uSegPToken,
		    sourceValues,
		    UV_CPx_RefPToken
		);
		InitUSegmentArrayEntry (
		    m_pVector,
		    uSegOffset,
		    USegReconstructorUSDC (uSegReconstructor)->storeCPD (),
		    uSegValues,
		    uSegPToken
		);
	    }
	    else if (IsntNil (ptc = USegReconstructorPTC (uSegReconstructor))) {
		if (ImplicitPruningOfUSegmentsEnabled && 0 == rtPTOKEN_PTC_BaseCount (ptc)) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		    ptc->discard ();
		    USegReconstructorSource(uSegReconstructor) = NilOf (void *);
		    if (IsntNil (USegReconstructorLC (uSegReconstructor)))
			USegReconstructorLC (uSegReconstructor)->release ();
		} else {
		    uSegPToken = ptc->ToPToken ();
		    rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		    rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		    m_pVector->StoreReference (rtVECTOR_CPx_USDCursor, uSegPToken);
		}
	    }
	    else if (IsntNil (USegReconstructorDescriptor (uSegReconstructor))) {
		rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		rtVECTOR_CPD_SetUSDCursor (m_pVector, rtVECTOR_USD_PToken);
		uSegPToken = m_pVector->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);
	    }
	    else if (ImplicitPruningOfUSegmentsEnabled) {
	        rtVECTOR_CPD_USD (m_pVector) = rtVECTOR_CPD_USegArray (m_pVector) + uSegOffset;
		if (m_pVector->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (m_pVector))) {
		    pUSegmentRelocationMap[uSegOffset] = -1;
		    uSegRetainedCnt--;
		    fUSegmentDeleted = true;
		}
	    }
	    if (IsntNil (USegReconstructorDescriptor (uSegReconstructor))) {
		USegReconstructorLC (uSegReconstructor)->Close (uSegPToken);
		uSegValues = AtUVLinkCPositionsPutUV (
		    uSegValues,
		    m_pVector,
		    uSegOffset,
		    USegReconstructorLC (uSegReconstructor),
		    USegReconstructorUSDC (uSegReconstructor)->pointerCPD ()
		);
		USegReconstructorLC (uSegReconstructor)->release ();
		uSegValues->release ();
	    }
	    if (uSegPToken)
		uSegPToken->release ();
	}
	break;


    default:
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"ReconstructVector: Unrecognized Reconstructor Purpose %d", m_xPurpose
	    )
	);
	break;
    }


/*
 *---------------------------------------------------------------------------
 * Part 2: 'ReconstructVector'
 *	Rebuild the P-Map.
 *---------------------------------------------------------------------------
 */

/*****  Adjust the space allocated to the P-Map...  *****/
    rtVECTOR_CPD_PMRD (m_pVector) = rtVECTOR_CPD_PMap (m_pVector) + oldPMapSize;
    m_pVector->ShiftContainerTail (
	rtVECTOR_CPx_PMRD,
	sizeof (rtVECTOR_PMRDType)
	    + sizeof(int) * rtVECTOR_CPD_USegIndexSize (m_pVector)
	    + sizeof(rtVECTOR_USDType) * rtVECTOR_CPD_USegArraySize(m_pVector),
	sizeof (rtVECTOR_PMRDType) * ((ptrdiff_t)newPMapSize - oldPMapSize),
	true
    );
    rtVECTOR_CPD_PMapSize (m_pVector) = newPMapSize;

/*****  ...re-create the P-Map...  *****/
    m_pPMRD = rtVECTOR_CPD_PMRD (m_pCPD);
    if (m_pPMRD > rtVECTOR_CPD_PMap (m_pVector) + oldPMapSize)
	m_pPMRD = rtVECTOR_CPD_PMap (m_pVector) + oldPMapSize;

    for (pMapPtr = m_pPMapChainHead; pMapPtr; pMapPtr = PMapReconstructorNext (pMapPtr)) {
	memcpy (
	    m_pPMRD,
	    PMapReconstructorPMRDArray (pMapPtr),
	    sizeof(rtVECTOR_PMRDType) * PMapReconstructorNumberOfSlots(pMapPtr)
	);
	m_pPMRD += PMapReconstructorNumberOfSlots (pMapPtr);
    }

    rtVECTOR_CPD_IsInconsistent (m_pCPD) = false;
    if (fUSegmentDeleted) {
	RemoveEmptyUSegments (m_pVector, pUSegmentRelocationMap, m_sUSegArray);
    }



/*
 *---------------------------------------------------------------------------
 * Part 3: 'ReconstructVector'
 *	Cleanup and exit.
 *---------------------------------------------------------------------------
 */

    for (pMapPtr = m_pPMapChainHead; pMapPtr; pMapPtr = nextPMapPtr) {
        nextPMapPtr = PMapReconstructorNext (pMapPtr);
	delete pMapPtr;
	FreePMRecCount++;
    }

    if (ImplicitPruningOfUSegmentsEnabled)
	UTIL_Free (pUSegmentRelocationMap);
    m_pCPD->release ();
    UTIL_Free (this);
}


/******************************
 ******************************
 *****  Vector Alignment  *****
 ******************************
 ******************************/


/****************************************
 *****  Standard Alignment Routine  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally align a vector.
 *
 *  Argument:
 *	vectorCPD		- a standard CPD for the vector to be aligned.
 *
 *  Returns:
 *	'vectorCPD'
 *
 *****/
PublicFnDef M_CPD *rtVECTOR_Align (M_CPD *vectorCPD) {


/*
 *---------------------------------------------------------------------------
 *  Local Definitions: 'rtVECTOR_Align'
 *---------------------------------------------------------------------------
 */

/*****  Variables  *****/
    USegReconstructorType	*uSegReconstructorArray,
				*targetUSegReconstructor,
				*undefinedValueUSegReconstructor;
    int				targetPosition,
				targetPMapPosition,
				nextTargetPMapPosition,
				targetUSegArrayLoc,
				undefinedValueUSegArrayLoc;
    bool			notTheFinalTargetPMapEntry,
				noChangesYet = true;
    M_CPD			*newPTokenCPD,
				*pTokenCPD,
				*undefuvCPD;


/*****  Vector P-Map Traversal Macros  *****/
#define targetRemaining (nextTargetPMapPosition - targetPosition)

#define advanceTarget() {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Align[advanceTarget]: OTP:%d, OTR:%d\n",\
	targetPosition, targetRemaining\
    );\
\
    advanceUSegmentScanPointers (targetRemaining);\
    setTargetScanPointers (++rtVECTOR_CPD_PMRD (vectorCPD));\
}

#define locateTarget(origin) {\
    while (origin >= nextTargetPMapPosition && notTheFinalTargetPMapEntry)\
	advanceTarget ();\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Align[locateTarget]: O:%d, TP:%d, TR:%d\n",\
	origin, targetPosition, targetRemaining\
    );\
    noChangesYet = false;\
    advanceUSegmentScanPointers (origin - targetPosition);\
    targetPosition = origin;\
}

#define setTargetScanPointers(targetPMapPtr) {\
    rtVECTOR_PMRDType *tPMapPtr;\
\
    targetPosition = targetPMapPosition =\
	rtVECTOR_PMRD_VectorOrigin (tPMapPtr = (targetPMapPtr));\
    if (notTheFinalTargetPMapEntry =\
	    (targetUSegArrayLoc = rtVECTOR_PMRD_SegmentIndex (tPMapPtr)) >= 0)\
    {\
	nextTargetPMapPosition	= rtVECTOR_PMRD_VectorOrigin (tPMapPtr + 1);\
	targetUSegReconstructor = uSegReconstructorArray + targetUSegArrayLoc;\
    }\
    else {\
	nextTargetPMapPosition = targetPMapPosition;\
        targetUSegReconstructor = NilOf (USegReconstructorType *);\
    }\
}

#define advanceUSegmentScanPointers(count) {\
    int	iCount;\
\
    if ((iCount = (count)) > 0) {\
	vectorReconstructor->AppendPMapEntryToReconstructor (\
	    targetUSegArrayLoc, iCount, noChangesYet\
	);\
	USegReconstructorOldPosition (targetUSegReconstructor) += iCount;\
	USegReconstructorNewPosition (targetUSegReconstructor) += iCount;\
    }\
}


/*****  P-Token Constructor Traversal Case Handlers  *****/
#define insertVectorPositions(origin, shift) {\
    locateTarget (origin);\
    vectorReconstructor->AppendPMapEntryToReconstructor (\
	undefinedValueUSegArrayLoc, shift, false\
    );\
    USegReconstructorNewPosition (undefinedValueUSegReconstructor)+= shift;\
    vectorReconstructor->AdjustUSegReconstructorPToken (\
	undefinedValueUSegArrayLoc, shift\
    );\
}

#define deleteVectorPositions(origin, shift) {\
    int targetCompleted,\
	effectiveOrigin = origin + shift,\
        count = (-shift);\
\
    locateTarget (effectiveOrigin);\
\
    if ((targetCompleted = targetRemaining) > count)\
	targetCompleted = count;\
    USegReconstructorOldPosition (targetUSegReconstructor) += targetCompleted;\
    vectorReconstructor->AdjustUSegReconstructorPToken (\
	targetUSegArrayLoc, -targetCompleted\
    );\
    targetPosition += targetCompleted;\
    while (targetCompleted != count) {\
	count -= targetCompleted;\
	advanceTarget ();\
	if ((targetCompleted = targetRemaining) > count)\
	    targetCompleted = count;\
	USegReconstructorOldPosition (targetUSegReconstructor)+= targetCompleted;\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -targetCompleted\
	);\
	targetPosition += targetCompleted;\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtVECTOR_Align'
 *---------------------------------------------------------------------------
 */
    RTYPE_MustBeA ("rtVECTOR_Align", M_CPD_RType (vectorCPD), RTYPE_C_Vector);

/*****  Do nothing if the vector is already current...  *****/
    rtPTOKEN_IsntCurrent (vectorCPD, rtVECTOR_CPx_PToken, pTokenCPD);
    if (IsNil (pTokenCPD))
	return vectorCPD;

/*****  ...otherwise:  *****/
    vectorCPD->CheckConsistency ();

/*****  ...create a vector reconstructor, ...  *****/
    VectorReconstructor	*vectorReconstructor = VectorReconstructor::NewReconstructor  (
	Alignment, vectorCPD, 1
    );

/*****  ...insure that the vector has an 'undefined' u-segment, ...  *****/
    undefinedValueUSegArrayLoc = vectorReconstructor->LocateOrAddReconstructorSegment (
	vectorCPD->TheNAClass ()
    );

/*****  ...cache the relevant reconstructor state, ...  *****/
    uSegReconstructorArray = vectorReconstructor->USegArray ();
    undefinedValueUSegReconstructor =
	uSegReconstructorArray + undefinedValueUSegArrayLoc;

/*****  ...initialize the alignment plan construction loop variables...  *****/
    setTargetScanPointers (
	rtVECTOR_CPD_PMRD (vectorCPD) = rtVECTOR_CPD_PMap (vectorCPD)
    );

    if (TracingPMapBrief) {
	M_CPD *basePToken = rtPTOKEN_BasePToken (pTokenCPD, -1);
	IO_printf (
	    "rtVECTOR_Align: OrigVectorSize=%d, NewVectorSize=%d, PMapSize=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (pTokenCPD),
	    rtPTOKEN_CPD_BaseElementCount (basePToken),
	    rtVECTOR_CPD_PMapSize (vectorCPD)
	);
	basePToken->release ();
    }

/*****  ...build the alignment plan, ...  *****/
    rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
    pTokenCPD->release ();

    rtPTOKEN_FTraverseInstructions (
	ptConstructor, insertVectorPositions, deleteVectorPositions
    );
    locateTarget (rtVECTOR_CPD_ElementCount (vectorCPD));

/*****  ...reset the vector's P-Token and element count, ...  *****/
    vectorCPD->StoreReference (
	rtVECTOR_CPx_PToken, newPTokenCPD = ptConstructor->NextGeneration ()
    );
    rtVECTOR_CPD_ElementCount (vectorCPD) = rtPTOKEN_CPD_BaseElementCount (newPTokenCPD);

/*****  ...reconstruct the vector, ...  *****/
    vectorReconstructor->ReconstructVector ();

/*****  ...cleanup, ...  *****/
    ptConstructor->discard ();
    AlignVectorCount++;


/*****  Align the vector's undefined uvector ...
 * Note: This code was added because the undefined uvector is not usually
 * accessed so it may never get aligned.  The approach that this routine takes
 * is to place any added elements into the undefined uvector.  Then, when
 * vector assign is called, the value is added to the proper usegment's
 * uvector and deleted from the undefined uvector.  The result can be
 * very long chains of shift ptokens attached to the undefined uvector.
 * The solution is to force an alignment of the undefined uvector (a very cheap
 * operation).  
 */

    if (LocateVectorSegment (vectorCPD, vectorCPD->TheNAClass (), &undefinedValueUSegArrayLoc)) {
	rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD)
	    + undefinedValueUSegArrayLoc;
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);

	undefuvCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_UndefUV);
	rtUNDEFUV_Align (undefuvCPD);
	undefuvCPD->release ();
    }
/*****  ...and return.  *****/
    return vectorCPD;


/*
 *---------------------------------------------------------------------------
 *  Local Macro Deletions: 'rtVECTOR_Align'
 *---------------------------------------------------------------------------
 */
#undef targetRemaining
#undef advanceTarget
#undef locateTarget
#undef setTargetScanPointers
#undef advanceUSegmentScanPointers
#undef insertVectorPositions
#undef deleteVectorPositions
}


/*********************************************
 *****  Comprehensive Alignment Routine  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to fully align a vector.
 *
 *  Arguments:
 *	vectorCPD		- a standard CPD for the vector to align.
 *	deletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	True if any alignments were done, false otherwise.
 *
 *****/
PublicFnDef bool rtVECTOR_AlignAll (M_CPD *vectorCPD, bool deletingEmptyUSegments) {
    bool	result = false, emptyUSegmentsDeleted = false;
    int		uSegmentCount, uSegmentRIndex, uSegmentWIndex,
		*uSegmentRelocationMap;

/*****  Align the vector ... *****/
    if (!rtPTOKEN_IsCurrent (vectorCPD, rtVECTOR_CPx_PToken)) {
        rtVECTOR_Align (vectorCPD);
	result = true;
    }

/*****  ... and its USegments: *****/
    uSegmentCount = rtVECTOR_CPD_USegArraySize (vectorCPD);
    uSegmentRelocationMap = (int *)UTIL_Malloc(sizeof (int) * uSegmentCount);

    for (uSegmentRIndex = uSegmentWIndex = 0;
	 uSegmentRIndex < uSegmentCount;
	 uSegmentRIndex++
    )
    {
	uSegmentRelocationMap[uSegmentRIndex] = -1;

	rtVECTOR_CPD_USD (vectorCPD) =
	    rtVECTOR_CPD_USegArray (vectorCPD) + uSegmentRIndex;
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);

//	if (M_POPIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	if (vectorCPD->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	    continue;

	if (deletingEmptyUSegments &&
	    rtPTOKEN_BaseElementCount (vectorCPD, rtVECTOR_CPx_USDCursor) == 0) {
	    result = emptyUSegmentsDeleted = true;
	    continue;
	}

	uSegmentRelocationMap[uSegmentRIndex] = uSegmentWIndex++;

	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
	M_CPD *uvectorCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);
	if (rtPTOKEN_IsCurrent (uvectorCPD, UV_CPx_PToken) &&
	    rtPTOKEN_IsCurrent (uvectorCPD, UV_CPx_RefPToken)) {
	  uvectorCPD->release ();
	  continue;
	}
	result = true;
	AlignUVector (uvectorCPD);
	uvectorCPD->release ();
    }

/*****  If u-segments were deleted ... *****/
    if (emptyUSegmentsDeleted) {
	RemoveEmptyUSegments (vectorCPD, uSegmentRelocationMap, uSegmentCount);
    }

    UTIL_Free (uSegmentRelocationMap);

    return result;
}


/************************
 ************************
 *****  Assignment  *****
 ************************
 ************************/

/***********************
 *****  Utilities  *****
 ***********************/

/*******************************************************************
 *  Single-Source Segment P-Map Range Descriptor Traversal Macros  *
 *******************************************************************/


/*---------------------------------------------------------------------------
 *****  Link Constructor Range Assignment Traversal Macro:
 *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the range in the link.
 *	count			- the number of elements referenced by
 *				  the range.
 *	origin			- an alterable variable containing the
 *				  origin of the range in the target.
 *	targetRemaining		- the name of a parameterless macro or
 *				  variable which is expected to evaluate
 *				  to the number of elements yet to be
 *				  processed in the target block.
 *	advanceTarget		- the name of a macro or function of no
 *				  arguments which can be called to advance
 *				  target to the next block.
 *	locateTarget		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the target block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count' and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *---------------------------------------------------------------------------
 */
#define SingleSegmentRangeAssign(position,count,origin,targetRemaining,advanceTarget,locateTarget,output)\
{\
    int targetCompleted;\
\
    if (TracingPMapSearches) IO_printf (\
	"SingleSegmentRangeAssign: pos=%d, cnt=%d, org=%d, rem=%d\n",\
	position, count, origin, targetRemaining\
    );\
    locateTarget (position, count, origin);\
    if ((targetCompleted = targetRemaining) > count)\
	targetCompleted = count;\
    output (position, targetCompleted, origin);\
    position	+= targetCompleted;\
    origin	+= targetCompleted;\
    while (count != targetCompleted)\
    {\
	count	-= targetCompleted;\
	advanceTarget ();\
	if ((targetCompleted = targetRemaining) > count)\
	    targetCompleted = count;\
	output (position, targetCompleted, origin);\
	position+= targetCompleted;\
	origin	+= targetCompleted;\
    }\
    if (TracingPMapSearches) IO_printf ("exit SingleSegmentRangeAssign\n");\
}


/*---------------------------------------------------------------------------
 *****  Link Constructor Repetition Assignment Traversal Macro:
 *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the repetition in the link.
 *	count			- the repeat count.
 *	origin			- an alterable variable containing the
 *				  origin of the repetition in the target.
 *	locateTarget		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the target block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *---------------------------------------------------------------------------
 */
#define SingleSegmentRepetitionAssign(position,count,origin,locateTarget,output) {\
    if (TracingPMapSearches) IO_printf (\
	"SingleSegmentRepetitionAssign: pos=%d, cnt=%d, org=%d\n",\
	position, count, origin\
    );\
    locateTarget (position, count, origin);\
    output (position, count, origin);\
    if (TracingPMapSearches) IO_printf (\
	"exit SingleSegmentRepititionAssign\n"\
    );\
}


/******************************************************************
 *  Multi-Source Segment P-Map Range Descriptor Traversal Macros  *
 ******************************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Range Assignment Traversal Macro: *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the range in the link.
 *	count			- the number of elements referenced by
 *				  the range.
 *	origin			- an alterable variable containing the
 *				  origin of the range in the target.
 *	sourceRemaining		- the name of a parameterless macro or
 *				  variable which is expected to evaluate
 *				  to the number of elements yet to be
 *				  processed in the source block.
 *	targetRemaining		- the name of a parameterless macro or
 *				  variable which is expected to evaluate
 *				  to the number of elements yet to be
 *				  processed in the target block.
 *	advanceSource		- the name of a macro or function of one
 *				  argument - 'count' - which can be called
 *				  to advance source by 'count' positions.
 *	advanceTarget		- the name of a macro or function of no
 *				  arguments which can be called to advance
 *				  target to the next block.
 *	locateTarget		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the target block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count' and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *---------------------------------------------------------------------------
 */
#define MultiSegmentRangeAssign(position,count,origin,sourceRemaining,targetRemaining,advanceSource,advanceTarget,locateTarget,output)\
{\
    int sourceCompleted, targetCompleted;\
\
    locateTarget (position, count, origin);\
    while (count > 0)\
    {\
	if ((sourceCompleted = sourceRemaining) > count)\
	    sourceCompleted = count;\
	if ((targetCompleted = targetRemaining) > sourceCompleted)\
	    targetCompleted = sourceCompleted;\
	output (position, targetCompleted, origin);\
	position += targetCompleted;\
	origin	 += targetCompleted;\
	count	 -= targetCompleted;\
	advanceSource (targetCompleted);\
	while (sourceCompleted != targetCompleted)\
	{\
	    sourceCompleted -= targetCompleted;\
	    advanceTarget ();\
	    if ((targetCompleted = targetRemaining) > sourceCompleted)\
	        targetCompleted = sourceCompleted;\
	    output (position, targetCompleted, origin);\
	    position	+= targetCompleted;\
	    origin	+= targetCompleted;\
	    count	-= targetCompleted;\
	    advanceSource (targetCompleted);\
	}\
    }\
}


/*---------------------------------------------------------------------------
 *****  Link Constructor Repetition Assignment Traversal Macro:
 *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the repetition in the link.
 *	count			- the repeat count.
 *	origin			- an alterable variable containing the
 *				  origin of the repetition in the target.
 *	sourceRemaining		- the name of a parameterless macro or
 *				  variable which is expected to evaluate
 *				  to the number of elements yet to be
 *				  processed in the source block.
 *	advanceSource		- the name of a macro or function of one
 *				  argument - 'count' - which can be called
 *				  to advance source by 'count' positions.
 *	locateTarget		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the target block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *	Dealing correctly with repetitions turns out to be very complicated.
 *	The approach taken as well as the reasons why it is complicated are
 *	explained below.
 *
 *  A repetition really is assigning 'count' number of source values to the
 *  same spot in the vector.  The result in the final vector is that only
 *  the last assignment remains.  The most obvious approach would be to
 *  ignore all the assignments except for the last.  This will not work.
 *  For the assign algorithm to work there must be a spot for every source
 *  value to go.  It is a simple thing to do when all of the source values
 *  pointed to by a repetition are of the same class.  In this case we
 *  simply output the repetition as is.  However, if some of the values are
 *  integers and other values are floats, the assign routines will die if
 *  you try to put them all into the same integer USegment U-Vector.
 *  There needs to be a way to assign the unwanted values to some position
 *  where they will be overwritten or unseen.  The next possible solution is
 *  to put all of these unwanted values into the reference nil spot in their
 *  correspounding USegment U-Vectors.  This will not work either because the
 *  link constructors being built to do the final assign must be in assending
 *  order.  For example: if there was a repeat of 3 and then a range of 4.
 *  If you put the 3 unwanted values into the reference nil spot, say position
 *  500, and then tried to put the next set of values into the proper
 *  positions, say 10-14, the link constructor would be out of order.  The
 *  working approach is to hide the unwanted values behind a valid assignment
 *  to a position.  For example, if we need to hide one integer we wait until
 *  a wanted integer is assigned a position and FIRST assign the unwanted
 *  integer to the same spot.  This effectively allows us to lose the unwanted
 *  values.  The actual method by which this is implemented is fairly simple.
 *  Half of the method is implemented inside of 'MultiSegmentRepetitionAssign'.
 *  It knows that the only wanted value of a repeat is the last one.  It loops
 *  thru the values in the repeat skipping all of them except for the very last
 *  value and any directly before the last value which are in the same USegment
 *  as it (of the same class).  These get output as a repeat.  Now there is
 *  the problem of all the skipped unwanted values.  These get taken care of
 *  inside the output defines.  The two output defines check to see if
 *  USegment that they are to write to is of the expected size.  If not, they
 *  'hide' the misssing values behind the chunk that they are just about to
 *  output.  At the end, any values that are still missing are taken care of
 *  automatically by the link constructor close routine.  It knows how many
 *  values there should be and puts the missing ones in as reference nils.
 *
 *---------------------------------------------------------------------------
 */


#define MultiSegmentRepetitionAssign(position,count,origin,sourceRemaining,advanceSource,locateTarget,output)\
{\
    locateTarget (position, count, origin);\
    while (sourceRemaining < count)\
    {\
	count -= sourceRemaining;\
	advanceSource (sourceRemaining);\
    }\
    output (position, count, origin);\
    advanceSource (count);\
}


/*********************************
 *****  Assignment Routines  *****
 *********************************/

/*********************************************************************
 *  Link Constructor Subscript - Value Descriptor Source Assignment  *
 *********************************************************************/

/*---------------------------------------------------------------------------
 *****  Vector Element Assignment.
 *
 *  Arguments:
 *	targetVector		- a standard CPD for the vector to be updated.
 *	linkc			- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *				  'Linkc' must be related referentially to
 *				  'target' and positionally to 'source'.
 *	sourceDescriptor	- a pointer to a Value Descriptor supplying
 *				  the values to be assigned to 'target'.
 *				  The Value Descriptor allows this routine to
 *                                as a cover for scalar and u-vector values.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source;
 *
 *	  for scalar source values and:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *	  for u-vector sources values.
 *
 *****/
PublicFnDef void rtVECTOR_Assign (
    M_CPD *targetVector, rtLINK_CType *linkc, DSC_Descriptor *sourceDescriptor
) {


/*
 *---------------------------------------------------------------------------
 *  Local Definitions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */

/*****  Variables  *****/
    USegReconstructorType	*uSegReconstructorArray,
				*sourceUSegReconstructor,
				*targetUSegReconstructor;
    rtLINK_CType		*sourceUSegAssociatedLC;
    int				sourceUSegArrayLoc,
				targetUSegArrayLoc,
				targetPosition,
				targetPMapPosition,
				nextTargetPMapPosition;
    bool			notTheFinalTargetPMapEntry,
				noChangesYet = true;


/*****  Target P-Map Traversal Macros  *****/
#define targetRemaining (nextTargetPMapPosition - targetPosition)

#define advanceTarget() {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[advanceTarget]: OTP:%d, OTR:%d\n",\
	targetPosition, targetRemaining\
    );\
\
    advanceUSegmentScanPointers (targetRemaining);\
    setTargetScanPointers (++rtVECTOR_CPD_PMRD (targetVector));\
}

#define locateTarget(position, count, origin) {\
    while (origin >= nextTargetPMapPosition && notTheFinalTargetPMapEntry)\
	advanceTarget ();\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[locateTarget]: O:%d, TP:%d, TR:%d\n",\
	origin, targetPosition, targetRemaining\
    );\
    noChangesYet = false;\
    advanceUSegmentScanPointers (origin - targetPosition);\
    targetPosition = origin;\
}

#define setTargetScanPointers(targetPMapPtr) {\
    rtVECTOR_PMRDType	*tPMapPtr;\
\
    targetPosition = targetPMapPosition =\
	rtVECTOR_PMRD_VectorOrigin (tPMapPtr = (targetPMapPtr));\
    if (notTheFinalTargetPMapEntry =\
	    (targetUSegArrayLoc = rtVECTOR_PMRD_SegmentIndex (tPMapPtr)) >= 0)\
    {\
	nextTargetPMapPosition	= rtVECTOR_PMRD_VectorOrigin (tPMapPtr + 1);\
	targetUSegReconstructor = uSegReconstructorArray + targetUSegArrayLoc;\
    }\
    else {\
	nextTargetPMapPosition = targetPMapPosition;\
        targetUSegReconstructor = NilOf (USegReconstructorType *);\
    }\
\
    if (TracingPMapSearches) IO_printf (\
	"... rtVECTOR_Assign[setTrgScanPts]: TP:%d, TUSL:%d, NTP:%d\n",\
	targetPMapPosition, targetUSegArrayLoc, nextTargetPMapPosition\
    );\
}


/*****  U-Segment Output Macros  *****/
#define outputRange(position, count, origin) {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[outputRange]: pos=%d, cnt=%d, org=%d\n",\
	position, count, origin\
    );\
    rtLINK_AppendRange (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	count\
    );\
    vectorReconstructor->AppendPMapEntryToReconstructor (\
	sourceUSegArrayLoc, count, false\
    );\
    USegReconstructorOldPosition (targetUSegReconstructor)+= count;\
    USegReconstructorNewPosition (sourceUSegReconstructor)+= count;\
    if (sourceUSegArrayLoc != targetUSegArrayLoc) {\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -count\
	);\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    sourceUSegArrayLoc, count\
	);\
    }\
    targetPosition += count;\
}

#define outputRepetition(position, count, origin) {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[outputRep]: pos=%d, cnt=%d, org=%d\n",\
	position, count, origin\
    );\
    rtLINK_AppendRepeat (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	count\
    );\
    vectorReconstructor->AppendPMapEntryToReconstructor (sourceUSegArrayLoc, 1, false);\
    USegReconstructorOldPosition (targetUSegReconstructor)++;\
    USegReconstructorNewPosition (sourceUSegReconstructor)++;\
    if (sourceUSegArrayLoc != targetUSegArrayLoc) {\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -1\
	);\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    sourceUSegArrayLoc, 1\
	);\
    }\
    targetPosition++;\
}

#define advanceUSegmentScanPointers(count) {\
    int	iCount;\
\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[advUSegScanPts]: cnt:%d\n",\
	count\
    );\
\
    if ((iCount = (count)) > 0) {\
	vectorReconstructor->AppendPMapEntryToReconstructor (\
	    targetUSegArrayLoc, iCount, noChangesYet\
	);\
	USegReconstructorOldPosition (targetUSegReconstructor) += iCount;\
	USegReconstructorNewPosition (targetUSegReconstructor) += iCount;\
    }\
}


/*****  Subscript Link Constructor Traversal Case Handlers  *****/
#define nilAssignCase(position, count, origin)

#define repetitionAssignCase(position, count, origin)\
SingleSegmentRepetitionAssign (\
    position, count, origin, locateTarget, outputRepetition\
)

#define rangeAssignCase(position, count, origin) SingleSegmentRangeAssign (\
    position,\
    count,\
    origin,\
    targetRemaining,\
    advanceTarget,\
    locateTarget,\
    outputRange\
)


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */

    targetVector->CheckConsistency ();
/*****
 *  Align the target and link and validate the applicability of link as a
 *  subscript for assignment into target.
 *****/
    linkc->AlignForExtract (rtVECTOR_Align (targetVector), rtVECTOR_CPx_PToken);

/*****  ...increment the operation count...  *****/
    LCAssignFromValueDCount++;

    if (TracingPMapBrief) {
	IO_printf (
	    "rtVECTOR_Assign: assigning %d values, linkRRDCount=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (linkc->PPT ()),
	    rtLINK_LC_RRDCount (linkc)
	);
	IO_printf (
	    "                             vectorSize = %u,  pmapSize=%d\n",
	    rtPTOKEN_BaseElementCount (targetVector, rtVECTOR_CPx_PToken),
	    rtVECTOR_CPD_PMapSize (targetVector)
	);
    }

/*****  ...create a reconstructor for the vector...  *****/
    VectorReconstructor *vectorReconstructor = VectorReconstructor::NewReconstructor  (
	LcDsAssignment, targetVector, 1
    );

/*****  ...insure that a segment exists for the source type...  *****/
    sourceUSegArrayLoc = vectorReconstructor->LocateOrAddReconstructorSegmentForLcDsAssignment (
	sourceDescriptor, linkc->PPT ()
    );

/*****  ...cache the required reconstructor state...  *****/
    uSegReconstructorArray  = vectorReconstructor->USegArray ();
    sourceUSegReconstructor = uSegReconstructorArray + sourceUSegArrayLoc;
    sourceUSegAssociatedLC  = USegReconstructorLC (sourceUSegReconstructor);

/*****
 *  ...initialize the assignment plan construction loop variables...
 *****/
    setTargetScanPointers (
	rtVECTOR_CPD_PMRD(targetVector) = rtVECTOR_CPD_PMap(targetVector)
    );

/*****  ...build the assignment plan...  *****/
    rtLINK_TraverseRRDCList (
	linkc, nilAssignCase, repetitionAssignCase, rangeAssignCase
    );
    locateTarget (linkc->ElementCount (), 0, rtVECTOR_CPD_ElementCount (targetVector));

/*****  ...reconstruct the target vector...  *****/
    vectorReconstructor->ReconstructVector ();

/*****  ...and return.  *****/
    rtVECTOR_CPD_IsASet (targetVector) = false;
    return;


/*
 *---------------------------------------------------------------------------
 *  Local Macro Deletions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */
#undef targetRemaining
#undef advanceTarget
#undef locateTarget
#undef setTargetScanPointers
#undef outputRange
#undef outputRepetition
#undef advanceUSegmentScanPointers
#undef nilAssignCase
#undef repetitionAssignCase
#undef rangeAssignCase
}


/***********************************************************************
 *  Link Constructor Subscript - Vector Constructor Source Assignment  *
 ***********************************************************************/

/*---------------------------------------------------------------------------
 *****  Vector Element Assignment.
 *
 *  Arguments:
 *	targetVector		- a standard CPD for the vector to be updated.
 *	linkc			- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *				  'Linkc' must be related referentially to
 *				  'target' and positionally to 'source'.
 *	sourceVectorC		- a pointer to a vector constructor
 *				  supplying the values to be assigned to
 *				  'target'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *****/
PublicFnDef void rtVECTOR_Assign (
    M_CPD *targetVector, rtLINK_CType *linkc, rtVECTOR_CType *pSource
)
{


/*
 *---------------------------------------------------------------------------
 *  Local Definitions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */

/*****  Variables  *****/
    USegReconstructorType	*uSegReconstructorArray,
				*sourceUSegReconstructor,
				*targetUSegReconstructor;
    rtVECTOR_PMRDC		*sourcePMapPtr;
    rtLINK_CType		*sourceUSegAssociatedLC;
    int				sourceRemaining,
				sourceDifference,
				sourceUSegArrayLoc,
				targetUSegArrayLoc,
				targetPosition,
				targetPMapPosition,
				nextTargetPMapPosition;
    bool			notTheFinalTargetPMapEntry,
				noChangesYet = true;


/*****  Source P-Map Traversal Macros  *****/
#define advanceSource(count) if ((sourceRemaining -= (count)) <= 0) {\
    setSourceScanPointers (sourcePMapPtr = sourcePMapPtr->Successor ());\
}\
else

#define setSourceScanPointers(sourcePMapPtr) {\
    rtVECTOR_PMRDC *sPMapPtr = (sourcePMapPtr);\
\
    if (sPMapPtr) {\
	sourceRemaining		= sPMapPtr->SegmentLength ();\
	sourceUSegArrayLoc	= sPMapPtr->TargetSegmentIndex ();\
	sourceUSegReconstructor = uSegReconstructorArray + sourceUSegArrayLoc;\
	sourceUSegAssociatedLC	= USegReconstructorLC(sourceUSegReconstructor);\
    }\
    else {\
	sourceRemaining = sourceUSegArrayLoc = 0;\
        sourceUSegAssociatedLC  = NilOf (rtLINK_CType*);\
	sourceUSegReconstructor = NilOf (USegReconstructorType *);\
    }\
}


/*****  Target P-Map Traversal Macros  *****/
#define targetRemaining (nextTargetPMapPosition - targetPosition)

#define advanceTarget() {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[advanceTarget]: OTP:%d, OTR:%d\n",\
	targetPosition, targetRemaining\
    );\
\
    advanceUSegmentScanPointers (targetRemaining);\
    setTargetScanPointers (++rtVECTOR_CPD_PMRD (targetVector));\
}

#define locateTarget(position, count, origin) {\
    while (origin >= nextTargetPMapPosition && notTheFinalTargetPMapEntry)\
	advanceTarget ();\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[locateTarget]: O:%d, TP:%d, TR:%d\n",\
	origin, targetPosition, targetRemaining\
    );\
    noChangesYet = false;\
    advanceUSegmentScanPointers (origin - targetPosition);\
    targetPosition = origin;\
}

#define setTargetScanPointers(targetPMapPtr) {\
    rtVECTOR_PMRDType	*tPMapPtr;\
\
    targetPosition = targetPMapPosition =\
	rtVECTOR_PMRD_VectorOrigin (tPMapPtr = (targetPMapPtr));\
    if (notTheFinalTargetPMapEntry =\
	    (targetUSegArrayLoc = rtVECTOR_PMRD_SegmentIndex (tPMapPtr)) >= 0)\
    {\
	nextTargetPMapPosition	= rtVECTOR_PMRD_VectorOrigin (tPMapPtr + 1);\
	targetUSegReconstructor = uSegReconstructorArray + targetUSegArrayLoc;\
    }\
    else {\
	nextTargetPMapPosition = targetPMapPosition;\
        targetUSegReconstructor = NilOf (USegReconstructorType *);\
    }\
}


/*****  U-Segment Output Macros  *****/
#define outputRange(position, count, origin) {\
    /** output left over repetition **/\
    sourceDifference = sourcePMapPtr->SegmentLimit ()\
	- sourceRemaining - sourceUSegAssociatedLC->ElementCount ();\
\
    if (sourceDifference > 0) rtLINK_AppendRepeat (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	sourceDifference\
    );\
\
    /** now output the current range **/\
    rtLINK_AppendRange (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	count\
    );\
    vectorReconstructor->AppendPMapEntryToReconstructor (\
	sourceUSegArrayLoc, count, false\
    );\
    USegReconstructorOldPosition (targetUSegReconstructor)+= count;\
    USegReconstructorNewPosition (sourceUSegReconstructor)+= count;\
    if (sourceUSegArrayLoc != targetUSegArrayLoc)\
    {\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -count\
	);\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    sourceUSegArrayLoc, count\
	);\
    }\
    targetPosition += count;\
}

#define outputRepetition(position, count, origin) {\
    /** output left over repetition **/\
    sourceDifference = sourcePMapPtr->SegmentLimit ()\
	- sourceRemaining - sourceUSegAssociatedLC->ElementCount ();\
\
    if (sourceDifference > 0) rtLINK_AppendRepeat (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	sourceDifference\
    );\
\
    /** now output the current range **/\
    rtLINK_AppendRepeat (\
	sourceUSegAssociatedLC,\
	USegReconstructorNewPosition (sourceUSegReconstructor),\
	count\
    );\
    vectorReconstructor->AppendPMapEntryToReconstructor (\
	sourceUSegArrayLoc, 1, false\
    );\
    USegReconstructorOldPosition (targetUSegReconstructor)++;\
    USegReconstructorNewPosition (sourceUSegReconstructor)++;\
    if (sourceUSegArrayLoc != targetUSegArrayLoc)\
    {\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -1\
	);\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    sourceUSegArrayLoc, 1\
	);\
    }\
    targetPosition++;\
}

#define advanceUSegmentScanPointers(count) {\
    int	iCount;\
\
    if ((iCount = (count)) > 0) {\
	vectorReconstructor->AppendPMapEntryToReconstructor (\
	    targetUSegArrayLoc, iCount, noChangesYet\
	);\
	USegReconstructorOldPosition (targetUSegReconstructor) += iCount;\
	USegReconstructorNewPosition (targetUSegReconstructor) += iCount;\
    }\
}


/*****  Subscript Link Constructor Traversal Case Handlers  *****/
#define nilAssignCase(position, count, origin)

#define repetitionAssignCase(position, count, origin)\
MultiSegmentRepetitionAssign (\
    position,\
    count,\
    origin,\
    sourceRemaining,\
    advanceSource,\
    locateTarget,\
    outputRepetition\
)

#define rangeAssignCase(position, count, origin) MultiSegmentRangeAssign (\
    position,\
    count,\
    origin,\
    sourceRemaining,\
    targetRemaining,\
    advanceSource,\
    advanceTarget,\
    locateTarget,\
    outputRange\
)


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */
    targetVector->CheckConsistency ();
/*****
 *  Align the target and link and validate the applicability of 'link' as a
 *  subscript for assigning source into target...
 *****/
    pSource->Align ();
    linkc->AlignForAssign (
	rtVECTOR_Align (targetVector), rtVECTOR_CPx_PToken, pSource->PPT (), -1
    );

/*****  ...increment the operation count...  *****/
    LCAssignFromVCCount++;

    if (TracingPMapBrief) {
	IO_printf (
	    "rtVECTOR_Assign: assigning %d values, linkRRDCount=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (linkc->PPT ()),
	    rtLINK_LC_RRDCount (linkc)
	);
	IO_printf (
	    "                         vectorSize=%u, pmapSize=%d\n",
	    rtPTOKEN_BaseElementCount (targetVector, rtVECTOR_CPx_PToken),
	    rtVECTOR_CPD_PMapSize (targetVector)
	);
    }

/*****  ...create a vector reconstructor for the target...  *****/
    VectorReconstructor *vectorReconstructor = VectorReconstructor::NewReconstructor  (
	LcVcAssignment, targetVector, pSource->USDCCount ()
    );

/*****  ...insure that segments exist for the source types...  *****/
    rtVECTOR_USDC *const *	pSrcUSDCArray = pSource->USDCArray ();
    rtVECTOR_USDC *const *const	pSrcUSDCLimit = pSource->USDCLimit ();
    while (pSrcUSDCArray < pSrcUSDCLimit) {
	rtVECTOR_USDC *pUSDC = *pSrcUSDCArray++;
	pUSDC->SetTargetSegmentIndexTo (
	    vectorReconstructor->LocateOrAddReconstructorSegmentForLcVcAssignment (pUSDC)
	);
    }

/*****  ...cache the required reconstructor state...  *****/
    uSegReconstructorArray = vectorReconstructor->USegArray ();

/*****
 *  ...initialize the assignment plan construction loop variables...
 *****/
    setSourceScanPointers (sourcePMapPtr = pSource->PMRDChainHead ());
    setTargetScanPointers (
	rtVECTOR_CPD_PMRD(targetVector) = rtVECTOR_CPD_PMap(targetVector)
    );

/*****  ...build the assignment plan...  *****/
    rtLINK_TraverseRRDCList (
	linkc, nilAssignCase, repetitionAssignCase, rangeAssignCase
    );
    locateTarget (linkc->ElementCount (), 0, rtVECTOR_CPD_ElementCount (targetVector));

/*****  ...reconstruct the target vector...  *****/
    vectorReconstructor->ReconstructVector ();

/*****  ...and return the CPD for the target.  *****/
    rtVECTOR_CPD_IsASet (targetVector) = false;
    return;


/*
 *---------------------------------------------------------------------------
 *  Local Macro Deletions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */
#undef advanceSource
#undef setSourceScanPointers
#undef targetRemaining
#undef advanceTarget
#undef locateTarget
#undef setTargetScanPointers
#undef outputRange
#undef outputRepetition
#undef advanceUSegmentScanPointers
#undef nilAssignCase
#undef repetitionAssignCase
#undef rangeAssignCase
}


/*---------------------------------------------------------------------------
 *****  Vector Element Assignment.
 *
 *  Arguments:
 *	targetVector		- a standard CPD for the vector to be updated.
 *	refp			- a reference for the element to be changed.
 *				  The reference must be referentially related
 *				  to the target vector.
 *	sourceDescriptor		- a pointer to a Value Descriptor supplying
 *				  the values to be assigned to 'target'.
 *				  The Value Descriptor allows this routine to
 *                                act as a cover for scalar and u-vector
 *				  values.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *		target[reference] = source
 *
 *	  for scalar source values and:
 *
 *		target[reference] = source[elementCount (source) - 1];
 *
 *	  for u-vector sources values.
 *
 *****/
PublicFnDef void rtVECTOR_Assign (
    M_CPD			*targetVector,
    rtREFUV_TypePTR_Reference	refp,
    DSC_Descriptor		*sourceDescriptor
)
{


/*
 *---------------------------------------------------------------------------
 *  Local Definitions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */

/*****  Variables  *****/
    USegReconstructorType	*uSegReconstructorArray,
				*sourceUSegReconstructor,
				*targetUSegReconstructor;
    int				sourceUSegArrayLoc,
				targetUSegArrayLoc,
				targetPosition,
				targetPMapPosition,
				nextTargetPMapPosition;
    bool			notTheFinalTargetPMapEntry,
				noChangesYet = true;


/*****  Target P-Map Traversal Macros  *****/
#define targetRemaining (nextTargetPMapPosition - targetPosition)

#define advanceTarget() {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[advanceTarget]: OTP:%d, OTR:%d\n",\
	targetPosition, targetRemaining\
    );\
\
    advanceUSegmentScanPointers (targetRemaining);\
    setTargetScanPointers (++rtVECTOR_CPD_PMRD (targetVector));\
}

#define locate(position, count, origin)\
{\
    while (origin >= nextTargetPMapPosition && notTheFinalTargetPMapEntry)\
	advanceTarget ();\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[locate]: O:%d, TP:%d, TR:%d\n",\
	origin, targetPosition, targetRemaining\
    );\
    noChangesYet = false;\
    advanceUSegmentScanPointers (origin - targetPosition);\
    targetPosition = origin;\
}

#define setTargetScanPointers(targetPMapPtr)\
{\
    rtVECTOR_PMRDType	*tPMapPtr;\
\
    targetPosition = targetPMapPosition =\
	rtVECTOR_PMRD_VectorOrigin (tPMapPtr = (targetPMapPtr));\
    if (notTheFinalTargetPMapEntry =\
	    (targetUSegArrayLoc = rtVECTOR_PMRD_SegmentIndex (tPMapPtr)) >= 0)\
    {\
	nextTargetPMapPosition	= rtVECTOR_PMRD_VectorOrigin (tPMapPtr + 1);\
	targetUSegReconstructor = uSegReconstructorArray + targetUSegArrayLoc;\
    }\
    else\
    {\
	nextTargetPMapPosition = targetPMapPosition;\
        targetUSegReconstructor = NilOf (USegReconstructorType *);\
    }\
\
    if (TracingPMapSearches) IO_printf (\
	"... rtVECTOR_Assign[setTrgScanPts]: TP:%d, TUSL:%d, NTP:%d\n",\
	targetPMapPosition, targetUSegArrayLoc, nextTargetPMapPosition\
    );\
}


/*****  U-Segment Output Macros  *****/
#define output(position, count, origin) {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[output]: pos=%d, cnt=%d, org=%d\n",\
	position, count, origin\
    );\
    USegReconstructorOffset (sourceUSegReconstructor) =\
	USegReconstructorNewPosition (sourceUSegReconstructor);\
    vectorReconstructor->AppendPMapEntryToReconstructor (\
	sourceUSegArrayLoc, 1, false\
    );\
    USegReconstructorOldPosition (targetUSegReconstructor)++;\
    USegReconstructorNewPosition (sourceUSegReconstructor)++;\
    if (sourceUSegArrayLoc != targetUSegArrayLoc) {\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    targetUSegArrayLoc, -1\
	);\
	vectorReconstructor->AdjustUSegReconstructorPToken (\
	    sourceUSegArrayLoc, 1\
	);\
    }\
    targetPosition++;\
}

#define advanceUSegmentScanPointers(count)\
{\
    int	iCount;\
\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_Assign[advUSegScanPts]: cnt:%d\n",\
	count\
    );\
\
    if ((iCount = (count)) > 0)\
    {\
	vectorReconstructor->AppendPMapEntryToReconstructor (\
	    targetUSegArrayLoc, iCount, noChangesYet\
	);\
	USegReconstructorOldPosition (targetUSegReconstructor) += iCount;\
	USegReconstructorNewPosition (targetUSegReconstructor) += iCount;\
    }\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */

/*****  Increment the operation count...  *****/
    RefAssignFromValueDCount++;

/*****  Validate the vector...  *****/
    targetVector->CheckConsistency ();

/*****
 *  Align the target and subscript and validate the applicability of the
 *  subscript as an assignment index...
 *****/
    rtREFUV_AlignAndValidateRef (
	refp, rtVECTOR_Align (targetVector), rtVECTOR_CPx_PToken
    );

    if (TracingPMapBrief) IO_printf (
	"rtVECTOR_Assign: ref=%d, vectorSize = %d, pmapSize=%d\n",
	rtREFUV_Ref_Element (refp),
	rtVECTOR_CPD_ElementCount (targetVector),
	rtVECTOR_CPD_PMapSize (targetVector)
    );

/*****  ... if the reference is 'nil', do no more...  *****/
    if(rtREFUV_Ref_Element(refp) >= rtVECTOR_CPD_ElementCount(targetVector))
	return;

/*****  ... otherwise, create a vector reconstructor for the target...  *****/
    VectorReconstructor *vectorReconstructor = VectorReconstructor::NewReconstructor (
	RfDsAssignment, targetVector, 1
    );

/*****  ...insure that a segment exists for the source type...  *****/
    sourceUSegArrayLoc = vectorReconstructor->LocateOrAddReconstructorSegmentForRfDsAssignment (
	sourceDescriptor
    );

/*****  ...cache the required reconstructor state...  *****/
    uSegReconstructorArray  = vectorReconstructor->USegArray ();
    sourceUSegReconstructor = uSegReconstructorArray + sourceUSegArrayLoc;

/*****
 *  ...initialize the assignment plan construction loop variables...
 *****/
    setTargetScanPointers (
	rtVECTOR_CPD_PMRD(targetVector) = rtVECTOR_CPD_PMap(targetVector)
    );

/*****  ...build the assignment plan...  *****/
    SingleSegmentRepetitionAssign (
	0, 1, rtREFUV_Ref_Element (refp), locate, output
    );
    locate (1, 0, rtVECTOR_CPD_ElementCount (targetVector));

/*****  ...reconstruct the target vector...  *****/
    vectorReconstructor->ReconstructVector ();

/*****  ...and return.  *****/
    rtVECTOR_CPD_IsASet (targetVector) = false;
    return;


/*
 *---------------------------------------------------------------------------
 *  Local Macro Deletions: 'rtVECTOR_Assign'
 *---------------------------------------------------------------------------
 */
#undef targetRemaining
#undef setTargetScanPointers
#undef advanceUSegmentScanPointers
#undef advanceTarget
#undef output
#undef locate
}


/****************************************************************
 *  Reference Subscript - Vector Constructor Source Assignment  *
 ****************************************************************/

/*---------------------------------------------------------------------------
 *****  Vector Element Assignment.
 *
 *  Arguments:
 *	targetVector		- a standard CPD for the vector to be updated.
 *	refp			- a reference for the element to be changed.
 *				  The reference must be referentially related
 *				  to the target vector.
 *	sourceVectorC		- a pointer to a vector constructor supplying
 *				  the value to be assigned to 'target'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *		target[reference] = source[elementCount (source) - 1];
 *
 *****/
PublicFnDef void rtVECTOR_Assign (
    M_CPD *targetVector, rtREFUV_TypePTR_Reference refp, rtVECTOR_CType *sourceVectorC
) {
    RefAssignFromVCCount++;

/*****  Align the constructor, ...  *****/
    sourceVectorC->Align ();

/*****  ...turn the reference into a Link Constructor, ...  *****/
    rtLINK_CType *linkc = rtLINK_RefConstructor (rtREFUV_Ref_RefPTokenCPD (refp), -1);
    rtLINK_AppendRepeat (
	linkc,
	rtREFUV_Ref_Element (refp),
	rtPTOKEN_CPD_BaseElementCount (sourceVectorC->PPT ())
    );
    linkc->Close (sourceVectorC->PPT ());

/*****  ...perform the assignment, ...  *****/
    rtVECTOR_Assign (targetVector, linkc, sourceVectorC);

/*****  ...and clean up.  *****/
    rtVECTOR_CPD_IsASet (targetVector) = false;
    linkc->release ();
}


/************************
 ************************
 *****  Extraction  *****
 ************************
 ************************/

/**********************************
 *****  Extraction Utilities  *****
 **********************************/

/*********************************************
 *  P-Map Range Descriptor Traversal Macros  *
 *********************************************/


/*---------------------------------------------------------------------------
 *****  Link Constructor Range Extraction Traversal Macro:
 *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the range in the link.
 *	count			- the number of elements referenced by
 *				  the range.
 *	origin			- an alterable variable containing the
 *				  origin of the range in the source.
 *	sourceRemaining		- the name of a parameterless macro or
 *				  variable which is expected to evaluate
 *				  to the number of elements yet to be
 *				  processed in the source block.
 *	advanceSource		- the name of a macro or function of no
 *				  arguments which can be called to advance
 *				  source to the next block.
 *	locateSource		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the source block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count' and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *---------------------------------------------------------------------------
 */
#define ExtractRange(position,count,origin,sourceRemaining,advanceSource,locateSource,output)\
{\
    locateSource (position, count, origin);\
    int sourceCompleted = sourceRemaining;\
    if (sourceCompleted > count)\
	sourceCompleted = count;\
    output (position, sourceCompleted, origin);\
    while (sourceCompleted != count) {\
	count -= sourceCompleted;\
	advanceSource ();\
	if ((sourceCompleted = sourceRemaining) > count)\
	    sourceCompleted = count;\
	output (position, sourceCompleted, origin);\
    }\
}


/*---------------------------------------------------------------------------
 *****  Link Constructor Repetition Extraction Traversal Macro:
 *
 *  Arguments:
 *	position		- an alterable variable containing the
 *				  position of the repetition in the link.
 *	count			- the repeat count.
 *	origin			- an alterable variable containing the
 *				  origin of the repetition in the source.
 *	locateSource		- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to locate
 *				  the source block containing 'origin'.
 *	output			- the name of a macro or function of three
 *				  arguments - 'position', 'count', and
 *				  'origin' - which can be called to output
 *				  a block of 'count' elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *---------------------------------------------------------------------------
 */
#define ExtractRepetition(position,count,origin,locateSource,output) {\
    locateSource (position, count, origin);\
    output (position, count, origin);\
}


/*********************************
 *****  Extraction Routines  *****
 *********************************/

PrivateFnDef void CheckForReferenceCompatibility (
    M_CPD *store, M_CPD *uvector, M_CPD *vector
) {
    M_CPD *storePToken	= rtPTOKEN_BasePToken (store, rtLSTORE_CPx_RowPToken);
    M_CPD *refPToken	= rtPTOKEN_BasePToken (uvector, UV_CPx_RefPToken);

    if (storePToken->DoesntName (refPToken)) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "Store [%u:%u] referenced by wrong PToken [%u:%u] in Vector [%u:%u]",
	    store->SpaceIndex (),
	    store->ContainerIndex (),
	    refPToken->SpaceIndex (),
	    refPToken->ContainerIndex (),
	    vector->SpaceIndex (),
	    vector->ContainerIndex ()
	)
    );

    storePToken->release ();
    refPToken->release ();
}


/***********************************************************
 *  Vector Source - Link Constructor Subscript Extraction  *
 ***********************************************************/

/**********************
 *  LC Extract Setup  *
 **********************/

/*---------------------------------------------------------------------------
 *  Arguments:
 *	pSource			- the source vector.
 *	pSubscript		- the subscript link constructor.
 *
 *  Returns:
 *	An instance of the ExtractResult enumeration.
 *
 *****/
enum ExtractResult {
    ExtractResult_IsEmpty, ExtractResult_IsAMonotype, ExtractResult_IsSomething
};

PrivateFnDef ExtractResult SetupExtract (M_CPD *pSource, rtLINK_CType *pSubscript) {
/*****  Align the source and link and validate link's use as a subscript *****/
    pSubscript->AlignForExtract (rtVECTOR_Align (pSource), rtVECTOR_CPx_PToken);

    if (TracingPMapBrief) {
	IO_printf (
	    "rtVECTOR_LCExtract: extracting %d elements, linkRRDCnt=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (pSubscript->PPT ()),
	    rtLINK_LC_RRDCount (pSubscript)
	);
	IO_printf (
	    "                    vectorSize=%u, pmapSize=%d\n",
	    rtPTOKEN_BaseElementCount (pSource, rtVECTOR_CPx_PToken),
	    rtVECTOR_CPD_PMapSize (pSource)
	);
    }

/******  Try to predict the form of the result...  *****/
    if (rtPTOKEN_CPD_BaseElementCount  (pSubscript->PPT ()) == 0)
	return ExtractResult_IsEmpty;

/*****
 *  Special case vectors that have only one pmap entry and
 *  that will not need an undefined usegment in the result ...
 *****/
    return rtVECTOR_CPD_PMapSize (pSource) == 1 && pSubscript->DoesntContainReferenceNils ()
	? ExtractResult_IsAMonotype : ExtractResult_IsSomething;
}


/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted Element Extraction Routine from a
 *****  Simple Vector (one with a single pmap entry).
 *
 *  Arguments:
 *	pSource			- the address of a standard CPD for the vector
 *				  from which elements are to be extracted.
 *	pSubscript			- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to the vector.
 *
 *  Returns:
 *	The address of a vector constructor containing the extracted elements.
 *
 *****/
PrivateFnDef void DoSimpleExtract (
    M_CPD *pSource, rtLINK_CType *pSubscript, M_CPD **ppResultStore, M_CPD **ppResultPointer
) {

    pSource->CheckConsistency ();
    SimpleLCExtractCount++;

/*****  Setup for the extract ... *****/
    rtVECTOR_CPD_USD (pSource) = rtVECTOR_CPD_USegArray (pSource) +
	rtVECTOR_PMRD_SegmentIndex (rtVECTOR_CPD_PMap (pSource));

/*****  Get the segment's store, ...  *****/
    rtVECTOR_CPD_SetUSDCursor (pSource, rtVECTOR_USD_VStore);
    M_CPD *pStore = pSource->GetCPD (rtVECTOR_CPx_USDCursor);

/*****  Get the segment's source pointer, ...  *****/
    rtVECTOR_CPD_SetUSDCursor (pSource, rtVECTOR_USD_Values);
    M_CPD *pSourcePointer = pSource->GetCPD (rtVECTOR_CPx_USDCursor);

/*****
 *  If the store is a string store, check for correct ref ptoken in uvector
 *  Potential exists for corruption by incorporator if the incorporator stores
 *  are incorrectly initialized ...
 *****/
    if (RTYPE_C_ListStore == pStore->RType () && rtLSTORE_CPD_StringStore (pStore))
	CheckForReferenceCompatibility (pStore, pSourcePointer, pSource);

/*****  (kludge):
 *  Temporarily change the subscript's referential ptoken to match that of the uvector ...
 *****/
    M_CPD *pSavedSubscriptRPT = pSubscript->RPT ();
    rtVECTOR_CPD_SetUSDCursor (pSource, rtVECTOR_USD_PToken);
    pSubscript->m_pRPT = pSource->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);

/*****  Extract the uvector ...  *****/
    RTYPE_PerformHandlerOperation (
	RTYPE_DoLCExtract,
	pSourcePointer->RType (),
	ppResultPointer,
	pSourcePointer,
	pSubscript
    );
    pSourcePointer->release ();

/*****  Restore the subscript's referential ptoken ... *****/
    pSubscript->RPT ()->release ();
    pSubscript->m_pRPT = pSavedSubscriptRPT;

/*****  And return:  *****/
    *ppResultStore = pStore;
}


/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	pSource			- the address of a standard CPD for the vector
 *				  from which elements are to be extracted.
 *	pSubscript		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to the vector.
 *
 *  Returns:
 *	The address of a vector constructor containing the extracted elements.
 *
 *****/
PrivateFnDef rtVECTOR_CType *DoComplexExtract (M_CPD *pSource, rtLINK_CType *pSubscript) {

/*****  Source P-Map Traversal Macros  *****/
#define extractSourceRemaining\
    (nextSourcePMapPosition - sourcePosition)

#define extractSourceUSegOrigin\
    (sourcePosition - sourcePMapPosition + srcPMapSegOrigin)

/*****  Source P-Map Traversal Macros  *****/
#define setPMRDPosition(pmrd, position)\
    ((pmrd) = rtVECTOR_CPD_PMap (pSource) + (position))

#define beginningValue(pmrd)\
    rtVECTOR_PMRD_VectorOrigin (pmrd)

#define endingValue(pmrd)\
    rtVECTOR_PMRD_VectorOrigin ((pmrd) + 1)

#define binSearchVSource(value) {\
    int beginPos, endPos, currentPos;\
    rtVECTOR_PMRDType *currentPMRD;\
    currentPos = beginPos = sourcePMapOffset;\
    endPos = sourcePMapSize - 1;\
    if (TracingPMapSearches) IO_printf (\
"...rtVECTOR_LCExtract[binSearchVSource]: Looking for:%d, SP:%d SR:%d BP=%d EP=%d\n",\
	value, sourcePosition, extractSourceRemaining, beginPos, endPos\
    );\
\
    setPMRDPosition (currentPMRD, currentPos);\
    while (beginPos <= endPos && (value < beginningValue (currentPMRD) ||\
				  value >= endingValue (currentPMRD)))\
    {\
	LCExtractHuntCount++;\
	currentPos = (beginPos + endPos) / 2;\
	setPMRDPosition (currentPMRD, currentPos);\
\
	if (value < beginningValue (currentPMRD))\
	    endPos = currentPos - 1;\
	else\
	    beginPos = currentPos + 1;\
    }\
\
    if (value >= beginningValue (currentPMRD) && value < endingValue (currentPMRD))\
	sourcePMapOffset = currentPos;	/*** Found it ***/\
    else sourcePMapOffset = sourcePMapSize;\
\
    rtVECTOR_CPD_PMRD (pSource) = rtVECTOR_CPD_PMap (pSource) + sourcePMapOffset;\
    setVSourceScanPointers (rtVECTOR_CPD_PMRD (pSource));\
}

#define advanceVSource() {\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_LCExtract[advanceVSource]: OTP:%d, OTR:%d\n",\
	sourcePosition, extractSourceRemaining\
    );\
\
    sourcePMapOffset++;\
    setVSourceScanPointers (++rtVECTOR_CPD_PMRD (pSource));\
}

#define locateVSource(position, count, origin) {\
    binSearchVSource (origin);\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_LCExtract[locateVSource]: O:%d, TP:%d, TR:%d\n",\
	origin, sourcePosition, extractSourceRemaining\
    );\
\
    sourcePosition = origin;\
}

#define setVSourceScanPointers(sourcePMapPtr) {\
    rtVECTOR_PMRDType *sPMapPtr = (sourcePMapPtr);\
\
    sourcePosition = sourcePMapPosition = rtVECTOR_PMRD_VectorOrigin (sPMapPtr);\
    xSourceUSegment = rtVECTOR_PMRD_SegmentIndex (sPMapPtr);\
    if (rtVECTOR_PMRD_SegmentIndex (sPMapPtr) >= 0){\
	nextSourcePMapPosition	= rtVECTOR_PMRD_VectorOrigin (sPMapPtr + 1);\
	srcPMapSegOrigin = rtVECTOR_PMRD_SegmentOrigin (sPMapPtr);\
    }\
    else nextSourcePMapPosition = sourcePMapPosition;\
}


/*****  Output Macros  *****/
/*---------------------------------------------------------------------------
 * The output macros do two things: they add a new PMRDC to the PMap and they
 * add to the linkc.  The PMap entry is for the new vector, its origin
 * and count pertain to the new uvector for this usegment which will be build
 * from the linkc.  Thus its origin starts at zero.  The linkc is used at the
 * end of this routine to extract the data for the new vector from the source
 * vector's uvectors so its origin must be relative to the source vector.
 ---------------------------------------------------------------------------*/
#define output(count, repeat) {\
    rtVECTOR_USDC *pUSDC = pResult->USDC (xSourceUSegment);\
    if (xSourceUSegment != xNilUSDC) {\
	if (!pUSDC) {\
	    rtVECTOR_CPD_USD (pSource) = rtVECTOR_CPD_USegArray(pSource) + xSourceUSegment;\
	    rtVECTOR_CPD_SetUSDCursor (pSource, rtVECTOR_USD_VStore);\
	    pUSDC = pResult->NewUSDC (\
		xSourceUSegment, pSource->GetCPD (rtVECTOR_CPx_USDCursor)\
	    );\
	    pUSDC->ClaimAssociatedLink (\
		rtLINK_RefConstructor (pSource, rtVECTOR_CPx_USD)\
	    );\
	}\
	rtLINK_CType *pAssociatedLink = pUSDC->AssociatedLink ();\
	pResult->AppendPMRD (pUSDC,pAssociatedLink->ElementCount (),count);\
	pAssociatedLink->Append (extractSourceUSegOrigin, count, repeat);\
    }\
    else {\
	if (!pUSDC)\
	    pUSDC = pResult->NewUSDC (xNilUSDC, rNA.RetainedObjectCPD ());\
	pResult->AppendPMRD (pUSDC, iResultNACount, count);\
	iResultNACount += count;\
    }\
}

#define outputRange(position, count, origin) {\
    output (count, false);\
}

#define outputRepeat(position, count, origin) {\
    output (count, true);\
}

/*****  Subscript Link Constructor Traversal Case Handlers  *****/
#define extractNil(position, count, origin) {\
    xSourceUSegment = xNilUSDC;\
    output (count, true);\
}

#define extractRepetition(position, count, origin) ExtractRepetition (\
    position, count, origin, locateVSource, outputRepeat\
)

#define extractRange(position, count, origin) ExtractRange (\
    position, count, origin, extractSourceRemaining, advanceVSource, locateVSource, outputRange\
)


/*
 *---------------------------------------------------------------------------
 *  Code Body: 'rtVECTOR_LCExtract'
 *---------------------------------------------------------------------------
 */

/*****  Adjust the operation counter...  *****/
    LCExtractCount++;

/*****  Check the input's consistency...  *****/
    pSource->CheckConsistency ();

/*****  Initialize the extraction plan variables  *****/
    unsigned int sResultUSDArray = rtVECTOR_CPD_USegArraySize (pSource);
    M_KOTE const &rNA = pSource->TheNAClass ();
    unsigned int xNilUSDC;
    if (!LocateVectorSegment(pSource, rNA, (int*)&xNilUSDC))
	xNilUSDC = sResultUSDArray++;

    rtVECTOR_CType *pResult = new rtVECTOR_CType (pSubscript->PPT (), sResultUSDArray);

    unsigned int xSourceUSegment, iResultNACount = 0;
    int nextSourcePMapPosition, sourcePosition, sourcePMapPosition, srcPMapSegOrigin;

    int sourcePMapSize = rtVECTOR_CPD_PMapSize (pSource);
    int sourcePMapOffset = 0;
    setVSourceScanPointers (
	rtVECTOR_CPD_PMRD (pSource) = rtVECTOR_CPD_PMap (pSource)
    );

/*****  Build the extraction Plan  *****/
/*****  This step builds the new PMaps and USegments for the result vector as
   well as the extraction plan.  The extraction plan is built in the
   associated linkcs.  There is a linkc for each used usegment which specifies
   where to get the 'extracted' data out of the source vector.
 *****/
    rtLINK_TraverseRRDCList (pSubscript, extractNil, extractRepetition, extractRange);

/*****  Do The Extractions  *****/
    unsigned int const cUSDCs = pResult->USDCCount ();
    for (unsigned x = 0; x < cUSDCs; x++) {
	rtVECTOR_USDC *pResultUSDC = pResult->USDC (x);
	if (pResultUSDC) {
	    M_CPD *pSegmentPPT, *pSegmentPointer;
	    if (x != xNilUSDC) {
		rtVECTOR_CPD_USD (pSource) = rtVECTOR_CPD_USegArray (pSource) + x;

		rtVECTOR_CPD_SetUSDCursor (pSource, rtVECTOR_USD_Values);
		M_CPD *pSourcePointer = pSource->GetCPD (rtVECTOR_CPx_USDCursor);
	/*****
	 *  If the store is a string store, check for correct ref ptoken in uvector
	 *  Potential exists for corruption by incorporator if the incorporator stores
	 *  are incorrectly initialized ...
	 *****/
		M_CPD *pStore = pResultUSDC->storeCPD ();
		if (pStore->RType () == RTYPE_C_ListStore && rtLSTORE_CPD_StringStore (pStore))
		    CheckForReferenceCompatibility (pStore, pSourcePointer, pSource);

		rtLINK_CType *pAssociatedLink = pResultUSDC->ClaimedAssociatedLink ();
		pSegmentPPT = pResult->NewPToken (pAssociatedLink->ElementCount ());
		pAssociatedLink->Close (pSegmentPPT);

		RTYPE_PerformHandlerOperation (
		    RTYPE_DoLCExtract,
		    pSourcePointer->RType (),
		    &pSegmentPointer,
		    pSourcePointer,
		    pAssociatedLink
		);
		pSourcePointer->release ();
		pAssociatedLink->release ();
	    }
	    else {
		pSegmentPPT = pResult->NewPToken (iResultNACount);
		pSegmentPointer = rtUNDEFUV_New (pSegmentPPT, rNA.PTokenCPD ());
	    }
	    pResultUSDC->SetPointerTo (pSegmentPointer, pSegmentPPT);
	}
    }

/*****  Compact and return the result...  *****/
    pResult->CompactUSDArray ();

    return pResult;

#undef extractSourceRemaining
#undef extractSourceUSegOrigin
#undef setPMRDPosition
#undef beginningValue
#undef endingValue
#undef binSearchVSource
#undef advanceVSource
#undef locateVSource
#undef setVSourceScanPointers
#undef output
#undef outputRange
#undef outputRepeat
#undef extractNil
#undef extractRepetition
#undef extractRange
}


/**********************************************
 *****  Public rtVECTOR_Extract Routines  *****
 **********************************************/

PublicFnDef void rtVECTOR_Extract (
    M_CPD *pSource, rtLINK_CType *pSubscript, VDescriptor &rResult
) {
    switch (SetupExtract (pSource, pSubscript)) {
    case ExtractResult_IsEmpty:
	rResult.setToVector (new rtVECTOR_CType (pSubscript->PPT (), 0));
	break;
    case ExtractResult_IsAMonotype: {
	    M_CPD *pStore, *pPointer;
	    DoSimpleExtract (pSource, pSubscript, &pStore, &pPointer);

	    rResult.setToMonotype (pStore, pPointer);

	    pPointer->release ();
	    pStore->release ();
	}
	break;
    default: {
	    rtVECTOR_CType *pResult = DoComplexExtract (pSource, pSubscript);

	    DSC_Descriptor iMonotype;
	    if (!pResult->SimplifiedToMonotype (&iMonotype, false))
		rResult.setToVector (pResult);
	    else {
		rResult.setToMoved (iMonotype);
		pResult->release ();
	    }
	}
	break;
    }
}


PublicFnDef void rtVECTOR_Extract (
    M_CPD *pSource, rtLINK_CType *pSubscript, M_CPD **ppResult
) {
    switch (SetupExtract (pSource, pSubscript)) {
    case ExtractResult_IsEmpty:
	*ppResult = NewVector (pSubscript->PPT ());
	break;
    case ExtractResult_IsAMonotype: {
	    M_CPD *pStore, *pPointer;
	    DoSimpleExtract (pSource, pSubscript, &pStore, &pPointer);

	    *ppResult = rtVECTOR_NewFromUV (pSubscript->PPT (), pStore, pPointer);

	    pPointer->release ();
	    pStore->release ();
	}
	break;
    default:
	*ppResult = DoComplexExtract (pSource, pSubscript)->ToVector ();
	break;
    }
}


/*****************************************
 *  rtVECTOR Extract - Container Result  *
 *****************************************/


/****************************************************
 *  Vector Source - Reference Subscript Extraction  *
 ****************************************************/

/*---------------------------------------------------------------------------
 *****  Reference Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	vectorCPD		- the address of a standard CPD for the vector
 *				  from which the element is to be extracted.
 *	refp			- the address of a reference specifying
 *				  the element to be extracted.  The
 *				  reference must be referentially
 *				  related to the vector.
 *	valuep			- a pointer to a value descriptor into which
 *				   the extracted element will be deposited.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtVECTOR_RefExtract (
    M_CPD *vectorCPD, rtREFUV_TypePTR_Reference refp, DSC_Descriptor *valuep
) {
#define setPMRDPosition(pmrd, position)\
    ((pmrd) = rtVECTOR_CPD_PMap (vectorCPD) + (position))

#define beginningValue(pmrd)\
    rtVECTOR_PMRD_VectorOrigin (pmrd)

#define endingValue(pmrd)\
    rtVECTOR_PMRD_VectorOrigin ((pmrd) + 1)


/*****
 *  Align the vector and reference and validate the applicability of the
 *  reference as a subscript for the vector...
 *****/
    rtREFUV_AlignAndValidateRef	(
	refp, rtVECTOR_Align (vectorCPD), rtVECTOR_CPx_PToken
    );
    vectorCPD->CheckConsistency ();
/*****  Increment operation count...  *****/
    RefExtractCount++;

/*****  Check for the nil extract case and empty vectors... *****/
    int position = rtREFUV_Ref_Element (refp);
    int referenceNil = rtPTOKEN_CPD_BaseElementCount (rtREFUV_Ref_RefPTokenCPD (refp));

    if (position == referenceNil) {
	/*** return an undefined value descriptor ***/
	valuep->constructNA (vectorCPD->KOT ());
	return;
    }

/*****  Find the PMRD which contains the referenced element  *****/
    int currentPos = 0, beginPos = 0;
    int endPos = rtVECTOR_CPD_PMapSize (vectorCPD) - 1;
    rtVECTOR_PMRDType *currentPMRD = rtVECTOR_CPD_PMap (vectorCPD);

    if (TracingPMapBrief) IO_printf (
	"rtVECTOR_RefExtract: Looking for:%d, vectorSize=%u, pmapSize=%d\n",
	position,
	rtPTOKEN_BaseElementCount (vectorCPD, rtVECTOR_CPx_PToken),
	rtVECTOR_CPD_PMapSize (vectorCPD)
    );

    /*****  Binary search for 'position' ... *****/
    while (beginPos <= endPos &&
	       (position < beginningValue (currentPMRD) ||
		position >= endingValue (currentPMRD))
    )
    {
	RefExtractHuntCount++;
	currentPos = (beginPos + endPos) / 2;
 	setPMRDPosition (currentPMRD, currentPos);

	if (position < beginningValue (currentPMRD))
	    endPos = currentPos - 1;
	else
	    beginPos = currentPos + 1;
    }

/*****
 *  Determine the offset in the U-Segment where the referenced element
 *  resides,  and make an appropriate reference to it...
 *****/
    int offset = position - beginningValue (currentPMRD)
	       + rtVECTOR_PMRD_SegmentOrigin (currentPMRD);

/*****  Set the U-Segment CPD pointer to the found U-Segment...  *****/
    rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD)
				 + rtVECTOR_PMRD_SegmentIndex (currentPMRD);

/*****  Obtain a CPD for the segment's U-Vector...  *****/
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
    M_CPD *uvectorCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

/*****  Obtain a CPD for the segment's Store...  *****/
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_VStore);
    M_CPD *vstoreCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

/*****
 *  If the store is a string store, check for correct ref ptoken in uvector
 *  Potential exists for corruption by incorporator if the incorporator stores
 *  are incorrectly initialized ...
 *****/
    if (RTYPE_C_ListStore == M_CPD_RType (vstoreCPD) &&
	rtLSTORE_CPD_StringStore (vstoreCPD))
	CheckForReferenceCompatibility (vstoreCPD, uvectorCPD, vectorCPD);

/*****  Extract the result and return in the result descriptor...  *****/
    valuep->constructScalarComposition (vstoreCPD, offset, uvectorCPD);

/*****  ... and clean up.  *****/
    uvectorCPD->release ();

#undef setPMRDPosition
#undef beginningValue
#undef endingValue
}


/************************
 ************************
 *****  Reordering  *****
 ************************
 ************************/

/**********************************************************
 * Vector Constructor Source  Reference U-Vector selector *
 **********************************************************/

/*---------------------------------------------------------------------------
 ***** Creates a new Vector Constructor which has the order specified by
 ***** the reference U-Vector.
 *
 *  Arguments:
 *	this		- the address of the source vector constructor.
 *	refuvCPD	- the address of a standard CPD for the reference
 *	                  U-vector specifying the new order.
 *
 *  Returns:
 *	The address of a new vector constructor in the specified order.
 *
 *****/
rtVECTOR_CType *rtVECTOR_CType::reorder (M_CPD *refuvCPD) {
    ReOrderVCCount++;

    Align ();
    rtREFUV_AlignForExtract (m_pPPT, -1, refuvCPD);

/*****  Create the result, ...  *****/
    M_CPD *pResultPPT = UV_CPD_PosPTokenCPD (refuvCPD);
    unsigned int nels = rtPTOKEN_CPD_BaseElementCount (pResultPPT);
    if (nels != rtPTOKEN_BaseElementCount (refuvCPD, UV_CPx_RefPToken)) ERR_SignalFault (
	EC__InternalInconsistency, "rtVECTOR_CType::reorder: Positional Inconsistency"
    );

    rtVECTOR_CType *pResult = new rtVECTOR_CType (pResultPPT, nels ? m_sUSDArray : 0);
    pResultPPT->release ();

/***** Check for empty results, ... *****/
    if (nels == 0)
	return pResult;

/***** Allocate and construct the uVectors Array *****/
    struct UVectorPair {
	union arraydsc_t {
	    void	    *asAnyPtr;
	    char	    *asCharacterPtr;
	    int		    *asIntegerPtr;
	    float	    *asSmallRealPtr;
	    double	    *asRealPtr;
	    VkUnsigned64    *asUnsigned64Ptr;
	    VkUnsigned96    *asUnsigned96Ptr;
	    VkUnsigned128   *asUnsigned128Ptr;
	} srcArray, dstArray;
	unsigned int	dstCount;
	RTYPE_Type	rtype;
    } *uVectorsArray = (UVectorPair *)UTIL_Malloc (sizeof (UVectorPair) * m_sUSDArray);

/***** copy the USDCr's *****/
    unsigned int i;
    for (i = 0; i < m_sUSDArray; i++) {
	rtVECTOR_USDC *pSrcUSDC = m_pUSDArray[i];

	/* ptoken */
	M_CPD *pPPT= pResult->NewPToken (pSrcUSDC->cardinality ());

	/* pointer */
	M_CPD *pSrcUVector = pSrcUSDC->pointerCPD ();
	AlignUVector (pSrcUVector);
	M_CPD *pDstUVector = NewUVector (
	    pSrcUVector->RType (), pPPT, pSrcUVector, UV_CPx_RefPToken
	);

	/* store */
	M_CPD *pStore = pSrcUSDC->storeCPD ();
	pStore->retain ();

	/* usdc */
	pResult->NewUSDC (i, pStore, pDstUVector, pPPT);

	/* set the uvectors array pointers */
	uVectorsArray[i].srcArray.asAnyPtr = UV_CPD_Array (pSrcUVector, void);
	uVectorsArray[i].dstArray.asAnyPtr = UV_CPD_Array (pDstUVector, void);
	uVectorsArray[i].dstCount = 0;
	uVectorsArray[i].rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);
    }

/***** Create the expanded source PMap *****/
    struct ExpandedPMap {
	rtVECTOR_USDC	*m_pSegment;
	unsigned int	 m_xOffset;
    } *const pExpandedPMap = (ExpandedPMap *)UTIL_Malloc (sizeof (ExpandedPMap) * nels);

    ExpandedPMap *pExpandedPMapEntry = pExpandedPMap;
    for (rtVECTOR_PMRDC *pPMRDC = m_pPMRDChainHead; pPMRDC; pPMRDC = pPMRDC->Successor ()) {
	rtVECTOR_USDC *pSegment = pResult->USDC (pPMRDC->SegmentIndex ());
	unsigned int xSegmentOrigin = pPMRDC->SegmentOrigin ();
	unsigned int xSegmentLimit  = pPMRDC->SegmentLimit ();

	while (xSegmentOrigin < xSegmentLimit) {
	    pExpandedPMapEntry->m_pSegment = pSegment;
	    pExpandedPMapEntry->m_xOffset  = xSegmentOrigin++;
	    pExpandedPMapEntry++;
	}
    }

/***** Initialize the result PMap chain *****/
    rtREFUV_ElementType const *pReorderIndex = UV_CPD_Array (refuvCPD, rtREFUV_ElementType);
    rtREFUV_ElementType const *const pReorderLimit = pReorderIndex + nels;

    rtVECTOR_USDC *pOpenSegment = pExpandedPMap[*pReorderIndex].m_pSegment;
    unsigned int xOpenOrigin = 0;
    unsigned int sOpenRun = 0;

/***** Loop thru the refuv for each value *****/
    while (pReorderIndex < pReorderLimit) {
	pExpandedPMapEntry = pExpandedPMap + *pReorderIndex++;

	rtVECTOR_USDC *pThisSegment = pExpandedPMapEntry->m_pSegment;
	UVectorPair *pUVectorPair = uVectorsArray + pThisSegment->SegmentIndex ();

	if (pThisSegment == pOpenSegment)
	    sOpenRun++;
	else {
	    pResult->AppendPMRD (pOpenSegment, xOpenOrigin, sOpenRun);
	    pOpenSegment = pThisSegment;
	    xOpenOrigin = pUVectorPair->dstCount;
	    sOpenRun = 1;
	}

	/*** Put the value into the new UVector ***/
	switch (pUVectorPair->rtype) {
	case RTYPE_C_CharUV:
	    *pUVectorPair->dstArray.asCharacterPtr++ =
		pUVectorPair->srcArray.asCharacterPtr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_DoubleUV:
	    *pUVectorPair->dstArray.asRealPtr++ =
		pUVectorPair->srcArray.asRealPtr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_FloatUV:
	    *pUVectorPair->dstArray.asSmallRealPtr++ =
		pUVectorPair->srcArray.asSmallRealPtr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_IntUV:
	case RTYPE_C_RefUV:
	    *pUVectorPair->dstArray.asIntegerPtr++ =
		pUVectorPair->srcArray.asIntegerPtr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_UndefUV:
	    break;
	case RTYPE_C_Unsigned64UV:
	    *pUVectorPair->dstArray.asUnsigned64Ptr++ =
		pUVectorPair->srcArray.asUnsigned64Ptr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_Unsigned96UV:
	    *pUVectorPair->dstArray.asUnsigned96Ptr++ =
		pUVectorPair->srcArray.asUnsigned96Ptr[pExpandedPMapEntry->m_xOffset];
	    break;
	case RTYPE_C_Unsigned128UV:
	    *pUVectorPair->dstArray.asUnsigned128Ptr++ =
		pUVectorPair->srcArray.asUnsigned128Ptr[pExpandedPMapEntry->m_xOffset];
	    break;
	default:
	    ERR_SignalFault (
		EC__InternalInconsistency,
		"rtVECTOR_ReOrder: Unimplemented UVector Type"
	    );
	}
	pUVectorPair->dstCount++;
    }

    /*** finish the last PMap entry ***/
    pResult->AppendPMRD (pOpenSegment, xOpenOrigin, sOpenRun);

/***** Cleanup and return *****/
    UTIL_Free (uVectorsArray);
    UTIL_Free (pExpandedPMap);

    pResult->CompactUSDArray ();

    return pResult;
}


/**********************************************
 * Vector Source  Reference U-Vector selector *
 **********************************************/

/*---------------------------------------------------------------------------
 ***** Creates a new Vector Constructor which has the order specified by
 ***** the reference U-Vector.
 *
 *  Arguments:
 *	vectorCPD	- the address of a standard CPD for the source vector.
 *	refuvCPD	- the address of a standard CPD for the reference
 *	                  U-vector specifying the new order.
 *
 *  Returns:
 *	The address of a new vector constructor in the specified order.
 *
 *****/
PublicFnDef rtVECTOR_CType *rtVECTOR_ReOrder (M_CPD *vectorCPD, M_CPD *refuvCPD) {
    ReOrderCount++;

    rtVECTOR_CType *vectorC = new rtVECTOR_CType (vectorCPD);
    rtVECTOR_CType *result = vectorC->reorder (refuvCPD);
    vectorC->release ();
    return result;
}


/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

/**********************************************************
 * Vector Constructor Source  Reference U-Vector selector *
 **********************************************************/

/*---------------------------------------------------------------------------
 ***** Takes a vector constructor created from ReOrder with the refuv and
 ***** produces the original vector constructor.
 *
 *  Arguments:
 *	this		- the address of the source vector constructor.
 *	refuvCPD	- the address of a standard CPD for the reference
 *	                  U-vector specifying the new order.
 *
 *  Returns:
 *	The address of a new vector constructor in the specified order.
 *
 *****/
rtVECTOR_CType *rtVECTOR_CType::distribute (M_CPD *refuvCPD) {
    DistributeVCCount++;

/***** Create a new flipped refuv ... *****/
    M_CPD *newrefuv = rtREFUV_Flip (refuvCPD);

/***** Now call ReOrder ... *****/
    rtVECTOR_CType *resultVectorC = reorder (newrefuv);

/***** Cleanup and return ... *****/
    newrefuv->release ();

    return resultVectorC;
}


/**********************************************************
 * Vector Constructor Source  Reference U-Vector selector *
 **********************************************************/

/*---------------------------------------------------------------------------
 ***** Takes a vector created from ReOrder with the refuv and
 ***** produces the original vector constructor.
 *
 *  Arguments:
 *	vectorCPD	- the address of a standard CPD for the source vector.
 *	refuvCPD	- the address of a standard CPD for the reference
 *	                  U-vector specifying the new order.
 *
 *  Returns:
 *	The address of a new vector constructor in the specified order.
 *
 *****/
PublicFnDef rtVECTOR_CType *rtVECTOR_Distribute (M_CPD *vectorCPD, M_CPD *refuvCPD) {
    DistributeCount++;

    rtVECTOR_CType *vectorC = new rtVECTOR_CType (vectorCPD);
    rtVECTOR_CType *result = vectorC->distribute (refuvCPD);
    vectorC->release ();
    return result;
}


/*******************************************
 *******************************************
 *****  Miscellaneous Vector Services  *****
 *******************************************
 *******************************************/

/****************************************
 *****  Type Subset Identification  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to find the subset of vector elements referencing a particular
 *****  store.
 *
 *  Arguments:
 *	pVector			- the vector.
 *	pStore 			- the store.
 *	pValueReturn		- the optional address ('Nil' if absent) of a
 *				  polytype descriptor that will be set to the
 *				  elements associated with the located subset.
 *
 *  Returns:
 *	A link constructor for the subset or Nil if no matching elements were 
 *	found.
 *
 ******/
PublicFnDef rtLINK_CType *rtVECTOR_SubsetInStore (
    M_CPD *pVector, M_CPD *pStore, VDescriptor *pValueReturn
) {
    SubsetInStoreCount++;

/***** Align the vector ... *****/
    rtVECTOR_Align (pVector);

/***** Find the correct U-Segment ... *****/
    int uSegment;
    if (!LocateVectorSegment (pVector, pStore, &uSegment))
	return NilOf (rtLINK_CType*);

/***** Verify that it has elements ...  *****/
    rtVECTOR_CPD_USD (pVector) = rtVECTOR_CPD_USegArray (pVector) + uSegment;

    rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_PToken);
    M_CPD *posPToken = pVector->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);

    if (0 == rtPTOKEN_CPD_BaseElementCount (posPToken)) {
	posPToken->release ();
	return NilOf (rtLINK_CType*);
    }

/***** Return the corresponding values if appropriate...  *****/
    if (pValueReturn) {
	rtVECTOR_CPD_SetUSDCursor (pVector, rtVECTOR_USD_Values);
	M_CPD *pValues = pVector->GetCPD (rtVECTOR_CPx_USDCursor);
	pValueReturn->setToMonotype (pStore, pValues);
	pValues->release ();
    }

/***** ... and compute the subset:  *****/
    rtLINK_CType *pSubset = rtLINK_PosConstructor (posPToken, -1);
    posPToken->release ();

    rtVECTOR_PMRDType const *pPMRD = rtVECTOR_CPD_PMap (pVector);
    rtVECTOR_PMRDType const *const pPMRDLimit = pPMRD + rtVECTOR_CPD_PMapSize (pVector);
    while (pPMRD < pPMRDLimit) {
	if (rtVECTOR_PMRD_SegmentIndex (pPMRD) == uSegment) {
	    unsigned int xOrigin = rtVECTOR_PMRD_VectorOrigin (pPMRD);
	    rtLINK_AppendRange (
		pSubset, xOrigin, rtVECTOR_PMRD_VectorOrigin (pPMRD + 1) - xOrigin
	    );
	}
	pPMRD++;
    }

    pSubset->Close (pVector, rtVECTOR_CPx_PToken);

    return pSubset;
}


/*---------------------------------------------------------------------------
 *****  Routine to find the subset of vector elements referencing a particular
 *****  known object table member.
 *
 *  Arguments:
 *	pVector			- the vector.
 *	pSubsetSpace		- the space in which to create the subset's
 *				  domain (and by extension, the subset).
 *	pKOTM 			- the known object table member.
 *	pValueReturn		- the optional address ('Nil' if absent) of a
 *				  polytype descriptor that will be set to the
 *				  elements associated with the located subset.
 *
 *  Returns:
 *	A link constructor for the subset or Nil if no matching elements were
 *	found.
 *
 ******/
PublicFnDef rtLINK_CType *rtVECTOR_SubsetOfType (
    M_CPD *pVector, M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
) {
    SubsetOfTypeCount++;

    rtVECTOR_CType *pVC = new rtVECTOR_CType (pVector);
    rtLINK_CType *pSubset = pVC->subsetOfType (pSubsetSpace, pKOTM, pValueReturn);
    pVC->release ();

    return pSubset;
}


/********************************************
 *  Simplify Vector To Descriptor Routines  *
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a vector constructor to a descriptor.  This can
 *****  only be done if the vector only has one used usegment.  Otherwise
 *****  the routine will do nothing and return false.
 *
 *  Arguments:
 *	this		- the address of the vector constructor to be
 *                        simplified.
 *	fNonDestructive	- a boolean that, when true, requires this routine to
 *			  copy the u-segment's u-vector instead of destructively
 *			  changing its positional p-token.
 *	dscp		- a pointer to an unused descriptor which this routine
 *                        will set to the simplified result if possible.
 *                        Otherwise it will be left alone.
 *
 *  Returns:
 *	true if a simplification took place,  false otherwise.
 *
 *****/
bool rtVECTOR_CType::SimplifiedToMonotype (
    DSC_Descriptor *dscp, bool fNonDestructive
) {
/***** If cannot simplify, return nil ... *****/
    if (CantBeSimplified ())
	return false;

/***** Get the uvector and store of the valid usegment ... *****/
    rtVECTOR_USDC *pUSDC = m_pPMRDChainHead->USDC ();
    M_CPD *uvector = pUSDC->pointerCPD ();
    if (fNonDestructive)
	uvector = UV_CopyWithNewPToken (uvector, m_pPPT);
    else {
	uvector->retain ();
	uvector->StoreReference (UV_CPx_PToken, m_pPPT);
    }

/***** Initialize the new descriptor ... *****/
    dscp->constructMonotype (M_DuplicateCPDPtr (pUSDC->storeCPD ()), uvector);
    uvector->release ();

    SimplifyVCToDescriptorCount++;

    return true;
}


/***********************************
 *****  Descriptor Conversion  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a vector to a descriptor.  This can
 *****  only be done if the vector only has one used usegment.  Otherwise
 *****  the routine will do nothing and return false.
 *
 *  Arguments:
 *	vector		- a standard CPD for the vector to be simplified.
 *	fNonDestructive	- a boolean that, when true, requires this routine to
 *			  copy the u-segment's u-vector instead of destructively
 *			  changing its positional p-token.
 *	dscp		- a pointer to an unused descriptor which this routine
 *                        will set to the result of the simplification if one
 *                        occurred, otherwise it will be untouched.
 *
 *  Returns:
 *	true if a simplification occurred, otherwise false.
 *
 *****/
PublicFnDef bool rtVECTOR_SimplifiedToMonotype (
    M_CPD *vector, DSC_Descriptor *dscp, bool fNonDestructive
) {
/***** If cannot simplify, return nil ... *****/
    if (!rtVECTOR_CPD_CanBeSimplified (vector))
	return false;

/***** Get the uvector and store of the valid usegment ... *****/
    rtVECTOR_CPD_PMRD (vector)	= rtVECTOR_CPD_PMap (vector);
    rtVECTOR_CPD_USD (vector)	= rtVECTOR_CPD_USegArray (vector)
				+ rtVECTOR_CPD_PMRD_SegmentIndex (vector);

    /*** Copy the uvector with the pos ptoken of the vector ... ***/
    M_CPD *vectorPPT = rtVECTOR_CPD_RowPTokenCPD (vector);

    rtVECTOR_CPD_SetUSDCursor (vector, rtVECTOR_USD_Values);
    M_CPD *uvector = vector->GetCPD (rtVECTOR_CPx_USDCursor);
    if (fNonDestructive) {
	M_CPD *oldUVector = uvector;
	uvector = UV_CopyWithNewPToken (oldUVector, vectorPPT);
	oldUVector->release ();
    }
    else {
	uvector->StoreReference (UV_CPx_PToken, vectorPPT);
    }

/***** Initialize the new descriptor ... *****/
    rtVECTOR_CPD_SetUSDCursor (vector, rtVECTOR_USD_VStore);
    dscp->constructMonotype (vector->GetCPD (rtVECTOR_CPx_USDCursor), uvector);

    uvector->release ();
    vectorPPT->release ();

    SimplifyToDescriptorCount++;

    return true;
}


/*******************************************************
 *******************************************************
 *****  Partitioned Vector Constructor Operations  *****
 *******************************************************
 *******************************************************/

/************************************
 *****  Partitioned Statistics  *****
 ************************************/

/*---------------------------------------------------------------------------
 ***** Function to perform a statistic operation on a Partition.
 *
 *  Arguments:
 *	this		- the source vector constructor.
 *	pPartition	- a link constructor which be used to extract from
 *                        the vector and subscript into the result uvector.
 *	xStatistic	- the statistic to compute.
 *
 *  Returns:
 *	A standard CPD for the uvector created.
 *
 *****/
M_CPD *rtVECTOR_CType::PartitionedStatistics (
    rtLINK_CType *pPartition, rtVECTOR_PS_Type xStatistic, M_KOT *pKOT
) {
    int
	iCalcValue,
	calcNumberOfValues,
	nextSource,
	nextSourcePMapPosition,
	sourcePosition,
	sourcePMapPosition,
	srcPMapSegOrigin,
	vectorOrigin,
	*iptr;
    double
	dCalcValue,
	tempa,
	tempb,
	*dptr;
    float
	*fptr;


/*****  Error Macros  *****/
#define complainAboutType {\
    ERR_SignalFault (\
	EC__InternalInconsistency,\
	"rtVECTOR_PartitionStatistics: Unknown Statistic Type"\
    );\
}

#define complainAboutRType(rtype) {\
    ERR_SignalFault (\
	EC__UnknownUVectorRType,\
	UTIL_FormatMessage (\
	    "rtVECTOR_PartitionStatistics: Unsupported R-Type %s",\
	    RTYPE_TypeIdAsString (rtype)\
	)\
    );\
}


/*****  Source P-Map Traversal Macros  *****/
#define sourceRemaining	 (nextSourcePMapPosition - sourcePosition)

#define sourceUSegOrigin (\
    sourcePosition - sourcePMapPosition + srcPMapSegOrigin\
)

#define advanceVCSource() {\
    vectorOrigin += srcPMapPtr->SegmentLength ();\
    srcPMapPtr = srcPMapPtr->Successor ();\
    if (notTheFinalSourcePMapEntry =\
	    (srcPMapPtr != NilOf (rtVECTOR_PMRDC*))\
    ) setVCSourceScanPointers (srcPMapPtr);\
\
    if (TracingPMapSearches) IO_printf (\
	"...rtVECTOR_PartitionStatistics[advanceVCSource]: SP:%d, SR:%d\n",\
	sourcePosition, sourceRemaining\
    );\
}

#define locateVCSource() {\
    while (nextSource >= nextSourcePMapPosition && notTheFinalSourcePMapEntry)\
	advanceVCSource ();\
\
    sourcePosition = nextSource;\
\
    if (TracingPMapSearches) IO_printf (\
"...rtVECTOR_PartitionStatistics[locateVCSource]: X:%d, SP:%d, SR:%d\n",\
	nextSource, sourcePosition, sourceRemaining\
    );\
}

#define setVCSourceScanPointers(sPMapPtr) {\
    sourcePosition = sourcePMapPosition = vectorOrigin;\
    if (notTheFinalSourcePMapEntry) {\
	nextSourcePMapPosition	= vectorOrigin + sPMapPtr->SegmentLength ();\
	srcPMapSegOrigin = sPMapPtr->SegmentOrigin ();\
    }\
    else nextSourcePMapPosition = sourcePMapPosition;\
}


/*****  Subscript Link Constructor Traversal Case Handlers  *****/
#define handleNil(position, count, origin) {\
    nextSource += (count);\
}

#define handleRepeat(position,cnt,origin) {\
    int sourceCompleted, count = (cnt);\
\
    locateVCSource ();\
    if ((sourceCompleted = sourceRemaining) > count)\
	sourceCompleted = count;\
    calculate (sourceCompleted);\
    nextSource += sourceCompleted;\
    while (sourceCompleted != count) {\
	count -= sourceCompleted;\
	advanceVCSource ();\
	if ((sourceCompleted = sourceRemaining) > count)\
	    sourceCompleted = count;\
	calculate (sourceCompleted);\
	nextSource += sourceCompleted;\
    }\
    output (origin);\
}

#define handleRange(position,count,org) {\
    int ii, origin = (org);\
\
    for (ii=0; ii<(count); ii++) {\
	locateVCSource ();\
	calculate (1);\
	nextSource++;\
	output (origin);\
	origin++;\
    }\
}


/*---------------------------------------------------------------------------
 * 	'rtVECTOR_PartitionStatistics' Code Body:
 *---------------------------------------------------------------------------
 */

/*****  Align the source and link and validate link's use as a subscript *****/
/*****
 *  Align the constructors and validate that the positional ptokens of
 *  'pPartition' and 'this' are identical.
 *****/
    Align ();
    pPartition->Align ();
    if (m_pPPT->DoesntName (pPartition->PPT ())) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtVECTOR_CType::PartitionedStatistics: Positional Inconsistency."
    );

/*****  Increment operation count  *****/
    PartitionStatsCount++;

    if (TracingPMapBrief) {
	IO_printf (
	    "rtVECTOR_PartitionStats: linkSize=%d, linkRRDCount=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (pPartition->PPT ()),
	    rtLINK_LC_RRDCount (pPartition)
	);
	IO_printf (
	    "                          vectorSize=%d, pmapSize=%d\n",
	    rtPTOKEN_CPD_BaseElementCount (m_pPPT), m_iPMRDCount
	);
    }

/*****  Create the result uvector  *****/
    M_CPD *resultCPD;
    switch (xStatistic) {
    case rtVECTOR_PS_Sum:
    case rtVECTOR_PS_Product:
    case rtVECTOR_PS_Avg:
    case rtVECTOR_PS_Max:
    case rtVECTOR_PS_Min:
	resultCPD = rtDOUBLEUV_New (
	    pPartition->RPT (), pKOT->TheDoublePTokenCPD ()
	);
	break;
    case rtVECTOR_PS_NumericCount:
	resultCPD = rtINTUV_New (
	    pPartition->RPT (), pKOT->TheIntegerPTokenCPD ()
	);
	break;
    default:
	complainAboutType;
	break;
    }

/*****  Initialize the extraction plan variables  *****/
    rtVECTOR_PMRDC *srcPMapPtr = m_pPMRDChainHead;
    bool notTheFinalSourcePMapEntry = IsntNil (srcPMapPtr);
    vectorOrigin = 0;
    nextSource = 0;
    setVCSourceScanPointers (srcPMapPtr);

/***** Initialize the calculation variables ... *****/
    bool calcFirst = true;

/*****  Perform the calculation ...  *****/
    switch (xStatistic) {


    case rtVECTOR_PS_Sum:

#define calculate(count) {\
    int i;\
\
    if (calcFirst) {\
	dCalcValue = 0.0;\
	calcFirst = false;\
    }\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iptr = rtINTUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
	for (i=0; i<count; i++)\
	    dCalcValue += (double)(*iptr++);\
	break;\
    case RTYPE_C_DoubleUV:\
	dptr = rtDOUBLEUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue += *dptr++;\
	break;\
    case RTYPE_C_FloatUV:\
	fptr = rtFLOATUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue += (double)(*fptr++);\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    dptr = rtDOUBLEUV_CPD_Array (resultCPD) + (offset);\
    *dptr = dCalcValue;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef calculate
#undef output
	break;


    case rtVECTOR_PS_Product:

#define calculate(count) {\
    int i;\
\
    if (calcFirst) {\
	dCalcValue = 1.0;\
	calcFirst = false;\
    }\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iptr = rtINTUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
	for (i=0; i<count; i++)\
	    dCalcValue *= (double)(*iptr++);\
	break;\
    case RTYPE_C_DoubleUV:\
	dptr = rtDOUBLEUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue *= *dptr++;\
	break;\
    case RTYPE_C_FloatUV:\
	fptr = rtFLOATUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue *= (double)(*fptr++);\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    dptr = rtDOUBLEUV_CPD_Array (resultCPD) + (offset);\
    *dptr = dCalcValue;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef calculate
#undef output
	break;


    case rtVECTOR_PS_Avg:

#define calculate(count) {\
    int i;\
\
    if (calcFirst) {\
	dCalcValue = 0.0;\
	calcNumberOfValues = 0;\
	calcFirst = false;\
    }\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iptr = rtINTUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
	for (i=0; i<count; i++)\
	    dCalcValue += (double)(*iptr++);\
	calcNumberOfValues += count;\
	break;\
    case RTYPE_C_DoubleUV:\
	dptr = rtDOUBLEUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue += *dptr++;\
	calcNumberOfValues += count;\
	break;\
    case RTYPE_C_FloatUV:\
	fptr = rtFLOATUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    dCalcValue += (double)(*fptr++);\
	calcNumberOfValues += count;\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    dptr = rtDOUBLEUV_CPD_Array (resultCPD) + (offset);\
    if (calcNumberOfValues == 0)\
	*dptr = 0.0;\
    else\
	*dptr = dCalcValue / calcNumberOfValues;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef calculate
#undef output
	break;


    case rtVECTOR_PS_Max:

#define dMax(a, b) (((tempa = a) > (tempb = b)) ? (tempa) : (tempb))

#define calculate(count) {\
    int i;\
\
    if (calcFirst)\
	dCalcValue = 0.0;\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iptr = rtINTUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
	for (i=0; i<count; i++)\
	    if (calcFirst) {\
		dCalcValue = (double)(*iptr++);\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMax (dCalcValue, (double)(*iptr++));\
	break;\
    case RTYPE_C_DoubleUV:\
	dptr = rtDOUBLEUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    if (calcFirst) {\
		dCalcValue = *dptr++;\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMax (dCalcValue, *dptr++);\
	break;\
    case RTYPE_C_FloatUV:\
	fptr = rtFLOATUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++)\
	    if (calcFirst) {\
		dCalcValue = (double)(*fptr++);\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMax (dCalcValue, (double)(*fptr++));\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    dptr = rtDOUBLEUV_CPD_Array (resultCPD) + (offset);\
    *dptr = dCalcValue;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef dMax
#undef calculate
#undef output
	break;


    case rtVECTOR_PS_Min:

#define dMin(a, b) (((tempa = (a)) < (tempb = (b))) ? (tempa) : (tempb))

#define calculate(count) {\
    int i;\
\
    if (calcFirst)\
	dCalcValue = 0.0;\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iptr = rtINTUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
	for (i=0; i<count; i++) {\
	    if (calcFirst) {\
		dCalcValue = (double)(*iptr++);\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMin (dCalcValue, (double)(*iptr++));\
	}\
	break;\
    case RTYPE_C_DoubleUV:\
	dptr = rtDOUBLEUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++) {\
	    if (calcFirst) {\
		dCalcValue = *dptr++;\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMin (dCalcValue, *dptr++);\
	}\
	break;\
    case RTYPE_C_FloatUV:\
	fptr = rtFLOATUV_CPD_Array (pSrcUVector) + sourceUSegOrigin;\
        for (i=0; i<count; i++) {\
	    if (calcFirst) {\
		dCalcValue = (double)(*fptr++);\
		calcFirst = false;\
	    }\
	    else dCalcValue = dMin (dCalcValue, (double)(*fptr++));\
	}\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    dptr = rtDOUBLEUV_CPD_Array (resultCPD) + (offset);\
    *dptr = dCalcValue;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef dMin
#undef calculate
#undef output
	break;


    case rtVECTOR_PS_NumericCount:

#define calculate(count) {\
    if (calcFirst) {\
	iCalcValue = 0;\
	calcFirst = false;\
    }\
\
    M_CPD *pSrcUVector = srcPMapPtr->pointerCPD ();\
    RTYPE_Type rtype = (RTYPE_Type)M_CPD_RType (pSrcUVector);\
    switch (rtype) {\
    case RTYPE_C_IntUV:\
	iCalcValue += count;\
	break;\
    case RTYPE_C_DoubleUV:\
	iCalcValue += count;\
	break;\
    case RTYPE_C_FloatUV:\
	iCalcValue += count;\
	break;\
    case RTYPE_C_CharUV:\
    case RTYPE_C_RefUV:\
    case RTYPE_C_UndefUV:\
    case RTYPE_C_Unsigned64UV:\
    case RTYPE_C_Unsigned96UV:\
    case RTYPE_C_Unsigned128UV:\
	break;\
    default:\
	complainAboutRType (rtype);\
	break;\
    }\
}

#define output(offset) {\
    iptr = rtINTUV_CPD_Array (resultCPD) + (offset);\
    *iptr = iCalcValue;\
    calcFirst = true;\
}

	rtLINK_TraverseRRDCList (pPartition, handleNil, handleRepeat, handleRange);

#undef calculate
#undef output
	break;


    default:
	complainAboutType;
	break;
    }

/*****  Return the result vector constructor  *****/
    return resultCPD;

/***** Undefine the internal macros ... *****/
#undef complainAboutType
#undef complainAboutRType
#undef sourceRemaining
#undef sourceUSegOrigin
#undef advanceVCSource
#undef locateVCSource
#undef setVCSourceScanPointers
#undef handleNil
#undef handleRepeat
#undef handleRange
}


/**************************************
 *****  Partitioned Sort Routine  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to generate sort indices for the partitioned elements of a
 *****  vector constructor.
 *
 *  Arguments:
 *	this			- a vector constructor supplying the values
 *				  to be partitioned.
 *	pPartition		- a link constructor which partitions the
 *				  elements of 'this'.  Two elements of
 *				  'this' belong to the same partition
 *				  if corresponding positions in this
 *				  constructor contain the same reference.
 *	descending		- a boolean which, when true, instructs this
 *				  routine to generate sort indices for a
 *				  descending sort.
 *
 *  Returns:
 *	A standard reference U-Vector CPD suitable for use with 'this'
 *	and 'rtVECTOR_CType::reorder'.  Unlike the sort indices generated for
 *	non-partitioned u-vector operations, these sort indices have the
 *	same P-Token defining both their positional and referential state.
 *
 *****/
M_CPD *rtVECTOR_CType::PartitionedSortIndices (
    rtLINK_CType *pPartition, bool descending
) {
    PartitionSortIndicesCount++;

/*****  Align and validate the inputs to this routine...  *****/
    Align ();
    pPartition->AlignForDistribute (m_pPPT, -1);

/*****
 *  Create reference U-Vectors to hold the sort indices to be returned by
 *  this routine and to hold the segment origins used to coalesce elements
 *  of like segment within each partition...
 *****/
    M_CPD *sortIndices = rtREFUV_New (m_pPPT, m_pPPT);

    M_CPD *segmentOrigins = rtREFUV_NewInitWithPrtitnBounds (pPartition);

/*****  Create the constructor links needed by this routine...  *****/
    MakeConstructorLinks ();

/*****
 *  Obtain and merge sort indices for each U-Segments of the vector...
 *****/
    rtVECTOR_USDC **uSegmentPtr = m_pUSDArray;
    rtVECTOR_USDC **uSegmentLim = m_sUSDArray + uSegmentPtr;
    while (uSegmentPtr < uSegmentLim) {
	rtLINK_CType *pSegmentSubset = (*uSegmentPtr)->AssociatedLink ();
	if (pSegmentSubset) {
	    M_CPD *segmentValues = (*uSegmentPtr)->pointerCPD ();

	    rtLINK_CType *segmentPartition = pPartition->Extract (pSegmentSubset);

	    M_CPD *segmentSortIndices;
	    switch (M_CPD_RType (segmentValues)) {
	    case RTYPE_C_CharUV:
		segmentSortIndices = rtCHARUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_DoubleUV:
		segmentSortIndices = rtDOUBLEUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_FloatUV:
		segmentSortIndices = rtFLOATUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_IntUV:
		segmentSortIndices = rtINTUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_RefUV:
		segmentSortIndices = rtREFUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_UndefUV:
		segmentSortIndices = rtUNDEFUV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_Unsigned64UV:
		segmentSortIndices = rtU64UV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_Unsigned96UV:
		segmentSortIndices = rtU96UV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    case RTYPE_C_Unsigned128UV:
		segmentSortIndices = rtU128UV_PartitndSortIndices (
		    segmentPartition, segmentValues, descending
		);
		break;
	    default:
	        ERR_SignalFault (
		    EC__UnknownUVectorRType, UTIL_FormatMessage (
			"rtVECTOR_PartitionedSortIndices: %s",
			RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (segmentValues))
		    )
		);
		break;
	    }

	    M_CPD *sortIndicesContribution = pSegmentSubset->Extract (segmentSortIndices);
	    segmentSortIndices->release ();

	    rtREFUV_PartitndAssign (
		sortIndices, segmentOrigins, segmentPartition, sortIndicesContribution
	    );
	    sortIndicesContribution->release ();
	    segmentPartition->release ();
	}
	uSegmentPtr++;
    }

/*****  Clean up and return the vector constructor sort indices...  *****/
    segmentOrigins->release ();

    return sortIndices;
}


/*******************************************
 *****  Partitioned Partition Routine  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to partition the partitioned elements of a vector constructor
 *****  by identity.
 *
 *  Arguments:
 *	this			- a vector constructor supplying the values
 *				  to be partitioned.  Although not absolutely
 *				  required, the most meaningful results are
 *				  obtained when 'this' is sorted.
 *	pPartition		- a link constructor which partitions the
 *				  elements of 'this'.  Two elements of
 *				  'this' belong to the same partition
 *				  if corresponding positions in this
 *				  constructor contain the same reference.
 *	majorLC			- an address which will be set to the address
 *				  of a link constructor defining the majorLC
 *				  partition computed by this routine.  This
 *				  constructor will share the same REFERENTIAL
 *				  state as 'pPartition'.
 *	minorLC			- an address which will be set to the address
 *				  of a link constructor defining the minorLC
 *				  partition computed by this routine.  This
 *				  constructor will share the same POSITIONAL
 *				  state as 'distribution' and 'pPartition' and
 *				  will have a REFERENTIAL state which matches
 *				  the POSITIONAL state of 'majorLC'.
 *	reordering		- an address which will be set to the address
 *				  of a standard CPD for a reference u-vector
 *				  specifying the reordering which must be
 *				  applied to 'minorLC' to make it correspond
 *				  positionally to 'this'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on '*majorLC', '*minorLC, and
 *	'*reordering' only.
 *
 *  Notes:
 *	This routine forms 'groups within groups'.  For each group defined
 *	by 'pPartition', this routine will group the elements of that group
 *	by matching identity.  The link constructor returned as 'minorLC'
 *	partitions the elements of 'this' by matching identity (subject
 *	to the boundaries established by 'pPartition'),  the link constructor
 *	returned as 'majorLC' partitions the groups defined 'minorLC' to match
 *	the original collection of groups defined by 'pPartition'.
 *
 *****/
void rtVECTOR_CType::PartitionedPartition (
    rtLINK_CType		 *pPartition,
    rtLINK_CType		**majorLC,
    rtLINK_CType		**minorLC,
    M_CPD			**reordering
)
{
    PartitndPartitionCount++;

/*****  Align and validate the inputs to this routine...  *****/
    Align ();
    pPartition->AlignForDistribute (m_pPPT, -1);

/*****
 *  Create the major and minor partitions and reordering to be returned by
 *  this routine...
 *****/
    M_ASD *pContainerSpace = m_pPPT->Space();
    M_CPD *groupPToken = rtPTOKEN_New (pContainerSpace, 0);
    M_CPD *minorPToken = rtPTOKEN_New (pContainerSpace, 0);

    rtLINK_CType *majorPartition = *majorLC = rtLINK_RefConstructor (
	pPartition->RPT (), -1
    )->Close (groupPToken);

    rtLINK_CType *minorPartition = *minorLC = rtLINK_RefConstructor (
	groupPToken, -1
    )->Close (minorPToken);

    M_CPD *minorReordering = *reordering = rtREFUV_New (
	pPartition->PPT (), minorPToken
    );

    minorPToken->release ();
    groupPToken->release ();

/*****  Create the constructor links needed by this routine...  *****/
    MakeConstructorLinks ();

/*****
 *  Obtain and merge partitions for each U-Segment of the vector...
 *****/
    rtVECTOR_USDC *const *	uSegmentPtr = m_pUSDArray;
    rtVECTOR_USDC *const *const	uSegmentLim = m_sUSDArray + uSegmentPtr;
    while (uSegmentPtr < uSegmentLim) {
	rtLINK_CType *pSegmentSubset = (*uSegmentPtr)->AssociatedLink ();
	if (pSegmentSubset) {
	    M_CPD *segmentValues = (*uSegmentPtr)->pointerCPD ();

	    rtLINK_CType *segmentPartition = pPartition->Extract (pSegmentSubset);

	    rtLINK_CType *segmentMajorPartition, *segmentMinorPartition;
	    switch (M_CPD_RType (segmentValues)) {
	    case RTYPE_C_CharUV:
		rtCHARUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_DoubleUV:
		rtDOUBLEUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_FloatUV:
		rtFLOATUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_IntUV:
		rtINTUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_RefUV:
		rtREFUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_UndefUV:
		rtUNDEFUV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_Unsigned64UV:
		rtU64UV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_Unsigned96UV:
		rtU96UV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    case RTYPE_C_Unsigned128UV:
		rtU128UV_PartitndPartition (
		    segmentPartition,
		    segmentValues,
		    &segmentMajorPartition,
		    &segmentMinorPartition
		);
		break;
	    default:
	        ERR_SignalFault (
		    EC__UnknownUVectorRType, UTIL_FormatMessage (
			"rtVECTOR_PartitionedPartition: %s",
			RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (segmentValues))
		    )
		);
		break;
	    }

	    segmentPartition->release ();

	    rtLINK_CType *majorContribution = majorPartition->Add (segmentMajorPartition);
	    segmentMajorPartition->release ();

	    rtLINK_CType *adjustedSegmentMinorPartition = majorContribution->Extract (
		segmentMinorPartition
	    );
	    segmentMinorPartition->release ();
	    majorContribution->release ();

	    rtLINK_CType *minorContribution = minorPartition->Add (
		adjustedSegmentMinorPartition
	    );
	    adjustedSegmentMinorPartition->release ();

	    M_CPD *minorContributionUV = minorContribution->ToRefUV ();
	    minorContribution->release ();

	    rtREFUV_LCAssign (minorReordering, pSegmentSubset, minorContributionUV);
	    minorContributionUV->release ();
	}
	uSegmentPtr++;
    }
}


/**************************
 **************************
 *****  Set Routines  *****
 **************************
 **************************/

PrivateFnDef void AdjustPMapEntryVectorOrigins (
    M_CPD *targetCPD, unsigned int xInitialEntry, int adjustment
) {
    rtVECTOR_PMRDType *pMapEntry = rtVECTOR_CPD_PMap (targetCPD);
    rtVECTOR_PMRDType const *const pMapLimit = pMapEntry + rtVECTOR_CPD_PMapSize (targetCPD);

    pMapEntry += xInitialEntry;
    while (pMapEntry <= pMapLimit) {
	rtVECTOR_PMRD_VectorOrigin (pMapEntry++) += adjustment;
    }
}

PrivateFnDef M_CPD *GetUSegUVector (M_CPD *vectorCPD, int usegi) {
    rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + usegi;
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
    return vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);
}


/******************************
 *****  Lookup Utilities  *****
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup values in a set uvector.
 *
 *  Arguments:
 *	sourceUV	- a standard CPD for the set uvector to be
 *			  be searched.
 *	keyUV		- a standard CPD for the set uvector containing
 *			  the values to lookup in the 'sourceUV'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationsLinkC 	- a pointer to a link constructor which this routine
 *			  will create.  It will contain the locations in the
 *			  source where the looked up value was found.
 *	locatedLinkC	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found.  It will contain
 * 			  the positions in 'keyUV' for which a value was
 *			  found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void UVLookup (
    M_CPD			*sourceUV,
    M_CPD			*keyUV,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType		**locationsLinkC,
    rtLINK_CType		**locatedLinkC
)
{
    switch ((RTYPE_Type)M_CPD_RType (sourceUV)) {
    case RTYPE_C_CharUV:
	rtCHARUV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Lookup (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Lookup  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_RefUV:
	rtREFUV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_Lookup  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Lookup	  (sourceUV, keyUV, lookupCase, locationsLinkC, locatedLinkC);
	break;
    default:
        ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtVECTOR UVLookup: Unsupported R-Type %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (sourceUV))
	    )
	);
	break;
    }
}


/*---------------------------------------------------------------------------
 *****  Private routine to lookup a value in a set uvector.
 *
 *  Arguments:
 *	sourceUV	- a standard CPD for the set uvector to be
 *			  be searched.
 *	keyDsc		- the address of a scalar descriptor containing the
 *                        value to lookup in 'sourceUV'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationPtr 	- the address of a scalar which this routine will set
 *			  to the location in the source where the looked up
 *                        value was found.  If the value was not found,
 *                        'locationPtr' will be untouched.
 *
 *  Returns:
 *	true if the value was found, false otherwise.
 *
 *****/
PrivateFnDef bool ScalarLookup (
    M_CPD			*sourceUV,
    DSC_Descriptor		*keyDsc,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
)
{
    bool found;

    switch ((RTYPE_Type)M_CPD_RType (sourceUV)) {
    case RTYPE_C_CharUV:
	found = rtCHARUV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Char (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_DoubleUV:
	found = rtDOUBLEUV_ScalarLookup	(
	    sourceUV, &(DSC_Descriptor_Scalar_Double (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_FloatUV:
	found = rtFLOATUV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Float (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_IntUV:
	found = rtINTUV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Int (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_RefUV:
	found = rtREFUV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Int (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_UndefUV:
	found = rtUNDEFUV_ScalarLookup (sourceUV, lookupCase, locationPtr);
	break;
    case RTYPE_C_Unsigned64UV:
	found = rtU64UV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Unsigned64 (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_Unsigned96UV:
	found = rtU96UV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Unsigned96 (*keyDsc)), lookupCase, locationPtr
	);
	break;
    case RTYPE_C_Unsigned128UV:
	found = rtU128UV_ScalarLookup (
	    sourceUV, &(DSC_Descriptor_Scalar_Unsigned128 (*keyDsc)), lookupCase, locationPtr
	);
	break;
    default:
        ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtVECTOR ScalarLookup: Unsupported R-Type %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (sourceUV))
	    )
	);
	break;
    }

    return found;
}


#define LookupHandleNil(origin, count, value, trgLinkC) {\
    /* ???? */\
}

#define LookupHandleRepeat(origin, count, value, trgLinkC) {\
    rtLINK_AppendRepeat (trgLinkC, value + adjustment, count);\
}

#define LookupHandleRange(origin, count, value, trgLinkC) {\
    rtLINK_AppendRange (trgLinkC, value + adjustment, count);\
}

#define AppendToLocations(srcLC) {\
    rtLINK_TraverseRRDCListExtraArg (\
	srcLC, LookupHandleNil, LookupHandleRepeat, LookupHandleRange, *locationsLinkC\
    );\
    srcLC->release ();\
}


/*---------------------------------------------------------------------------
 *****  Private routine to lookup a value in a set vector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the set vector to be searched.
 *	keyDsc		- the address of a scalar descriptor containing
 *			  the value to lookup in 'sourceCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *			  'locationsDscPointer' will be set to a reference
 *			  scalar pointer containing the position in 'sourceCPD'
 *		          where the value was found.  If the value was not
 *                        found, then 'locationsDscPointer' will be set to an
 *                        empty descriptor pointer.
 *	locatedDscPointer
 *			- a pointer to a descriptor pointer which this routine
 *			  will create ONLY if needed.
 *			  if the value was found -
 *                             then 'locatedDscPointer' will
 *			       be set to a reference scalar descriptor pointer.
 *                        Otherwise 'locatedDscPointer' will be set to an
 *                             empty descriptor pointer.
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LookupFromScalarDsc (
    M_CPD			*sourceCPD,
    DSC_Descriptor		*keyDsc,
    rtLINK_LookupCase		lookupCase,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*locatedDscPointer
)
{
/****  Align and validate the source set vector and key scalar descriptor ****/
    rtVECTOR_Align (sourceCPD);

    if (keyDsc->isntScalar ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtVECTOR LookupFromScalarDescriptor: Key must be a scalar descriptor"
    );

    if (keyDsc->holdsAScalarReference ()) {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (*keyDsc));
    }

    if (!rtVECTOR_CPD_IsASet (sourceCPD)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtVECTOR LookupFromScalarDescriptor: Source must be a set vector"
    );

/*****  Find the key's usegment in the source ... *****/
/*****  If not in the source ... return empty descriptor pointers ... *****/
    int srcUSegi;
    if (!LocateVectorSegment (sourceCPD, keyDsc->storeCPDIfAvailable (), &srcUSegi)) {
	locationsDscPointer->construct ();
	locatedDscPointer->construct ();
	return;
    }

/*****  Get the src vector origin for 'srcUSegi' ... *****/
    int srcVectorOrig;
    for (unsigned int i=0; i < rtVECTOR_CPD_PMapSize (sourceCPD); i++) {
	rtVECTOR_CPD_PMRD (sourceCPD) = rtVECTOR_CPD_PMap (sourceCPD) + i;
	if (rtVECTOR_CPD_PMRD_SegmentIndex (sourceCPD) == srcUSegi) {
	    srcVectorOrig = rtVECTOR_CPD_PMRD_VectorOrigin (sourceCPD);
	    break;
	}
    }

/*****  Get the src usegment's uvector *****/
    M_CPD *srcUVector = GetUSegUVector (sourceCPD, srcUSegi);

/*****  Do the lookup *****/
    int location;
    bool found = ScalarLookup (srcUVector, keyDsc, lookupCase, &location);
    srcUVector->release ();

/*****  If found ... *****/
    if (found) {
	M_CPD *ptoken = rtVECTOR_CPD_RowPTokenCPD (sourceCPD);

	locationsDscPointer->constructReferenceScalar (ptoken, location + srcVectorOrig);

	ptoken = M_DuplicateCPDPtr (
	    DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (*keyDsc))
	);

	locatedDscPointer->constructReferenceScalar (ptoken, 0);
    }
/***** ... else not found ... *****/
    else {
	locationsDscPointer->construct ();
	locatedDscPointer->construct ();
    }
}


/*---------------------------------------------------------------------------
 *****  Private routine to lookup values in a set vector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the set vector to be searched.
 *	keyCPD		- a standard CPD for a set uvector containing
 *			  the values to lookup in 'sourceCPD'.
 *	keyVStore	- a standard CPD for the Value Store of 'keyCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *                        'locationsDscPointer' will be set to a link
 *                        descriptor pointer.  For each value in the
 *                        'keyCPD', the link will contain
 *                        the position in 'sourceCPD' where that value
 *                        was found.
 *	locatedDscPointer
 *			- a pointer to a descriptor pointer which this routine
 *			  will create ONLY if needed.
 *			  'locatedDscPointer' will be set to a link
 *                        descriptor pointer.  The link will contain the
 *                        positions in 'keyCPD' for which a value was
 *                        found in 'sourceCPD'.  If all of the
 *                        values from 'keyCPD' were found, then
 *                        'locatedDscPointer' will be set to an empty
 *                        descriptor pointer.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LookupFromUV (
    M_CPD			*sourceCPD,
    M_CPD			*keyCPD,
    M_CPD			*keyVStore,
    rtLINK_LookupCase		lookupCase,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*locatedDscPointer
)
{
/***** Align and validate source set vector and key set UV ... *****/
    rtVECTOR_Align (sourceCPD);
    AlignUVector (keyCPD);

    if (!rtVECTOR_CPD_IsASet (sourceCPD) || !UV_CPD_IsASetUV (keyCPD)) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtVECTOR LookupFromUV:  Source and Key must be sets"
	);
    }

/***** Make the location link constructor ... *****/
    rtLINK_CType *locationsLC = rtLINK_RefConstructor (sourceCPD, rtVECTOR_CPx_PToken);
    rtLINK_CType **locationsLinkC = &locationsLC; /* used in AppendToLocations macro */

/***** If the key is empty, close the locations linkc and return ... *****/
    if (UV_CPD_ElementCount (keyCPD) == 0) {
	locationsLC->Close (keyCPD, UV_CPx_PToken);
	locationsDscPointer->constructLink (locationsLC);
	locatedDscPointer->construct ();
	return;
    }

/*****  Find the key's usegment in the source... *****/
/*****  If not in the source ... return empty linkc's ... *****/
    int srcUSegi;
    if (!LocateVectorSegment (sourceCPD, keyVStore, &srcUSegi)) {
	rtLINK_CType *locatedLC = rtLINK_RefConstructor (keyCPD, UV_CPx_PToken);
	M_CPD *ptoken = rtPTOKEN_New (keyCPD->Space (), 0);
	locatedLC->Close (ptoken);
	locatedDscPointer->constructLink (locatedLC);

	locationsLC->Close (ptoken);
	locationsDscPointer->constructLink (locationsLC);
	ptoken->release ();

	return;
    }

/*****  Get the src vector origin for 'srcUSegi' ... *****/
    int srcVectorOrig;
    for (unsigned int i=0; i<rtVECTOR_CPD_PMapSize (sourceCPD); i++) {
	rtVECTOR_CPD_PMRD (sourceCPD) = rtVECTOR_CPD_PMap (sourceCPD) + i;
	if (rtVECTOR_CPD_PMRD_SegmentIndex (sourceCPD) == srcUSegi) {
	    srcVectorOrig = rtVECTOR_CPD_PMRD_VectorOrigin (sourceCPD);
	    break;
	}
    }

/*****  Get the src usegment's uvector *****/
    rtVECTOR_CPD_USD (sourceCPD) =rtVECTOR_CPD_USegArray(sourceCPD) + srcUSegi;
    rtVECTOR_CPD_SetUSDCursor (sourceCPD, rtVECTOR_USD_Values);
    M_CPD *srcUVector = sourceCPD->GetCPD (rtVECTOR_CPx_USDCursor);

/*****  Do the lookup *****/
    rtLINK_CType *locatedLC, *uvLocationsLC;
    UVLookup (srcUVector, keyCPD, lookupCase, &uvLocationsLC, &locatedLC);

    srcUVector->release ();

/*****  Append to 'locationsLinkC' *****/
    int adjustment = srcVectorOrig; /* used in AppendToLocations macro */
    AppendToLocations (uvLocationsLC);

/*****  Close the link constructors ... *****/

/*****  If some values were not found ... *****/
    if (locatedLC) {
	locatedDscPointer->constructLink (locatedLC);

	locationsLC->Close (locatedLC->PPT ());
	locationsDscPointer->constructLink (locationsLC);
    }

/***** ... else all values were found ... *****/
    else {
	locatedDscPointer->construct ();

	locationsLC->Close (keyCPD, UV_CPx_PToken);
	locationsDscPointer->constructLink (locationsLC);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to lookup values in a set vector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the set vector to be searched.
 *	keyDsc		- the address of a descriptor containing
 *			  the values to lookup in 'sourceCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *
 *	                  If 'keyDsc' is a scalar descriptor, then
 *			  'locationsDscPointer' will be set to a reference
 *			  scalar pointer containing the position in 'sourceCPD'
 *		          where the value was found.  If the value was not
 *                        found, then 'locationsDscPointer' will be set to an
 *                        empty descriptor pointer.
 *
 *	                  If 'keyDsc' is not a scalar descriptor, then
 *                        'locationsDscPointer' will be set to a link
 *                        descriptor pointer.  For each value in the
 *                        'keyDsc', the link will contain
 *                        the position in 'sourceCPD' where that value
 *                        was found.
 *	locatedDscPointer
 *			- a pointer to a descriptor pointer which this routine
 *			  will create ONLY if needed.
 *
 *                        If 'keyDsc' is a scalar descriptor -
 *			    if the value was found -
 *                             then 'locatedDscPointer' will
 *			       be set to a reference scalar descriptor pointer.
 *                          Otherwise 'locatedDscPointer' will be set to an
 *                             empty descriptor pointer.
 *
 *                        If 'keyDsc' is not a scalar descriptor -
 *			     then 'locatedDscPointer' will be set to a link
 *                           descriptor pointer.  The link will contain the
 *                           positions in 'keyDsc' for which a value was
 *                           found in 'sourceCPD'.  If all of the
 *                           values from 'keyDsc' were found, then
 *                           'locatedDscPointer' will be set to an empty
 *                           descriptor pointer.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtVECTOR_LookupFromValueD (
    M_CPD			*sourceCPD,
    DSC_Descriptor		*keyDsc,
    rtLINK_LookupCase		lookupCase,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*locatedDscPointer
) {
    LookupFromValueDCount++;

/*****  Decode the key descriptor ... *****/
    if (keyDsc->isScalar ()) {
	LookupFromScalarDsc (
	    sourceCPD, keyDsc, lookupCase, locationsDscPointer, locatedDscPointer
	);
    }
    else if (keyDsc->holdsNonScalarValues ()) {
	LookupFromUV (
	    sourceCPD,
	    DSC_Descriptor_Value (*keyDsc),
	    keyDsc->storeCPDIfAvailable (),
	    lookupCase,
	    locationsDscPointer,
	    locatedDscPointer
	);
    }
    else if (keyDsc->holdsNonScalarReferences ()) {
	LookupFromUV (
	    sourceCPD,
	    DSC_Descriptor_Reference (*keyDsc),
	    keyDsc->storeCPDIfAvailable (),
	    lookupCase,
	    locationsDscPointer,
	    locatedDscPointer
	);
    }
    else ERR_SignalFault (
	EC__InternalInconsistency,
	"rtVECTOR_LookupFromValueD: Unimplemented key descriptor type"
    );
}


/***********************************
 *****  LocateOrAdd Utilities  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to locate or add a set of values to a set uvector.
 *
 *  Arguments:
 *	targetUV	- a standard CPD for the set uvector in which the
 *			  values will be located or added to.
 *	keyUV		- a standard CPD for the set uvector containing
 *			  the values to locate or add.
 *	locationsLinkC 	- a pointer to a link constructor which this routine
 *			  will create.  It will contain the locations in the
 *			  target where the value was found or added.
 *	addedLinkC	- optional (if not requested set to Nil).
 *                        the address of an uninitialized link constructor
 *		          which will be created iff requested and meaningful.
 *                        It will contain the positions from
 *                        'keyUV' whose values were added to
 *                        the target set uvector.  If none of the values from
 *                        the key were added, then this link will be
 *                        unused and set to Nil.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	If 'addedLinkC' is constructed, it's domain will reside in the key's
 *	scratch pad.
 *
 *****/
PrivateFnDef void UVLocateOrAdd (
    M_CPD*		targetUV,
    M_CPD*		keyUV,
    rtLINK_CType**	locationsLinkC,
    rtLINK_CType**	addedLinkC
) {
    switch ((RTYPE_Type)M_CPD_RType (targetUV)) {
    case RTYPE_C_CharUV:
	rtCHARUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_RefUV:
	rtREFUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_LocateOrAdd (targetUV, keyUV, locationsLinkC, addedLinkC);
	break;
    default:
        ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtVECTOR UVLocateOrAdd: Unsupported R-Type %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (targetUV))
	    )
	);
	break;
    }
}


/*---------------------------------------------------------------------------
 *****  Private routine to locate or add a value to a set uvector.
 *
 *  Arguments:
 *	targetUV	- a standard CPD for the set uvector for the value
 *			  to be located or added to.
 *	keyDsc		- the address of a scalar descriptor containing the
 *                        value to locate or add in 'targetUV'.
 *	locationPtr 	- the address of a scalar which this routine will set
 *			  to the location in the target where the looked up
 *                        value was found or added.  If the value was not
 *			  found, 'locationPtr' will be untouched.
 *
 *  Returns:
 *	true if the value was found, false otherwise.
 *
 *****/
PrivateFnDef bool ScalarLocateOrAdd (
    M_CPD *targetUV, DSC_Descriptor *keyDsc, int *locationPtr
) {
    bool added;

    switch ((RTYPE_Type)M_CPD_RType (targetUV)) {
    case RTYPE_C_CharUV:
	added = rtCHARUV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Char (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_DoubleUV:
	added = rtDOUBLEUV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Double (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_FloatUV:
	added = rtFLOATUV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Float (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_IntUV:
	added = rtINTUV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Int (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_RefUV:
	added = rtREFUV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Int (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_UndefUV:
	added = rtUNDEFUV_ScalarLocateOrAdd (targetUV, locationPtr);
	break;
    case RTYPE_C_Unsigned64UV:
	added = rtU64UV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Unsigned64 (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_Unsigned96UV:
	added = rtU96UV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Unsigned96 (*keyDsc)), locationPtr
	);
	break;
    case RTYPE_C_Unsigned128UV:
	added = rtU128UV_ScalarLocateOrAdd (
	    targetUV, &(DSC_Descriptor_Scalar_Unsigned128 (*keyDsc)), locationPtr
	);
	break;
    default:
        ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtVECTOR ScalarLocateOrAdd: Unsupported R-Type %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (targetUV))
	    )
	);
	break;
    }

    return added;
}


/*---------------------------------------------------------------------------
 *****  Private Routine to add a pmap entry to correspond to a newly added
 *****  usegment in a set vector.
 *
 *  Arguments:
 *	targetCPD		- A cpd for the vector being modified.
 *	trgUSegi		- the index of the usegment for which the pmap
 *				  entry is being created.
 *
 *  Returns:
 *	the offset into 'targetCPD's pmap of the added entry.
 *
 *  Note:
 *	This routine leaves targetCPD's PMRD pointer pointing to the added
 *  entry.
 *
 *****/
PrivateFnDef unsigned int AddNewPMapEntryToSetVector (M_CPD *targetCPD, int trgUSegi) {

/*****  (1) Determine where to put the new entry *****/

    rtVECTOR_CPD_USI (targetCPD) = rtVECTOR_CPD_USegIndex (targetCPD);
    rtVECTOR_CPD_PMRD (targetCPD) = rtVECTOR_CPD_PMap (targetCPD);

    unsigned int trgPMapi = 0;
    for (int i=0; i < rtVECTOR_CPD_USegIndexSize (targetCPD); i++) {
	if (*rtVECTOR_CPD_USI (targetCPD) == trgUSegi)
	    break;
	if (*rtVECTOR_CPD_USI (targetCPD) == rtVECTOR_CPD_PMRD_SegmentIndex (targetCPD)) {
	    trgPMapi++;
	    rtVECTOR_CPD_PMRD (targetCPD)++;
	}
	rtVECTOR_CPD_USI (targetCPD)++;
    }

    unsigned int trgVectorOrig = rtVECTOR_CPD_PMRD_VectorOrigin (targetCPD);

/*****  (2) move the container tail to make room for the new pmap entry *****/

    targetCPD->ShiftContainerTail (
	rtVECTOR_CPx_PMRD,
	sizeof (rtVECTOR_PMRDType) *
	    (rtVECTOR_CPD_PMapSize (targetCPD) + 1 - trgPMapi) +
	    sizeof (int) * rtVECTOR_CPD_USegIndexSize (targetCPD) +
	    sizeof (rtVECTOR_USDType) * rtVECTOR_CPD_USegArraySize (targetCPD),
	sizeof (rtVECTOR_PMRDType),
	true
    );

    rtVECTOR_CPD_PMapSize (targetCPD)++;

/*****  (3) fill in the new pmap entry  *****/

    rtVECTOR_CPD_PMRD (targetCPD) = rtVECTOR_CPD_PMap (targetCPD) + trgPMapi;
    rtVECTOR_CPD_PMRD_VectorOrigin (targetCPD) = trgVectorOrig;
    rtVECTOR_CPD_PMRD_SegmentOrigin (targetCPD) = 0;
    rtVECTOR_CPD_PMRD_SegmentIndex (targetCPD) = trgUSegi;

    return trgPMapi;
}


/*---------------------------------------------------------------------------
 *****  Private Routine to 'align' the set vector and the affected usegment
 *
 *  Arguments:
 *	vectorCPD		- The set vector being modified
 *	usegi			- The index of the affected usegment.
 *	vectorOrig		- The vector offset for the affected usegment's
 *				  first position.
 *	uvectorCPD		- The container for the affected usegment's
 *				  values.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PrivateFnDef void UpdateVectorSetPositionalState (
    M_CPD *vectorCPD, int usegi, int vectorOrig, M_CPD *uvectorCPD
) {
#define LocateOrAddHandleInsertion(ptOrigin,ptShift) {\
    /***** modify target ptoken *****/\
    targetPTokenC->AppendAdjustment (ptOrigin + vectorOrig, ptShift);\
}

#define LocateOrAddHandleDeletion(ptOrigin,ptShift) {\
    ERR_SignalFault (\
	EC__InternalInconsistency,\
	"LocateOrAddHandleDelete: Should never be deletions"\
    );\
}

/***** Create a ptoken constructor for the target ... *****/
    rtPTOKEN_CType *targetPTokenC = rtPTOKEN_NewShiftPTConstructor (
	vectorCPD, rtVECTOR_CPx_PToken
    );

/*****  Get the modified ptoken for the usegment uvector ... *****/
    rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + usegi;
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);

    M_CPD *modifiedPToken;
    rtPTOKEN_IsntCurrent (vectorCPD, rtVECTOR_CPx_USDCursor, modifiedPToken);
    if (IsNil (modifiedPToken)) ERR_SignalFault (
	EC__InternalInconsistency,
	"LocateOrAddFromUV: Modified PToken isnt modified"
    );

/***** Setup for the traversal *****/
    rtPTOKEN_CType *modifiedPTokenC = rtPTOKEN_CPDCumAdjustments (modifiedPToken);
    modifiedPToken->release ();

/***** Traverse the modified ptoken *****/
    rtPTOKEN_FTraverseAdjustments (
	modifiedPTokenC, LocateOrAddHandleInsertion, LocateOrAddHandleDeletion
    );

    modifiedPTokenC->discard ();

/*****  Attach the vector's new positional ptoken ... *****/
    M_CPD *ptoken = targetPTokenC->ToPToken ();
    vectorCPD->StoreReference (rtVECTOR_CPx_PToken, ptoken);
    ptoken->release ();

/*****
 *  Update the USegment's PToken ...
 *****/

    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);
    vectorCPD->StoreReference (rtVECTOR_CPx_USDCursor, uvectorCPD, UV_CPx_PToken);

#undef LocateOrAddHandleInsertion
#undef LocateOrAddHandleDeletion
}


/*---------------------------------------------------------------------------
 *****  Private routine to locate or add a value to a set vector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for the set vector in which the values
 *                        will be located or added.
 *	keyDsc		- the address of a scalar descriptor containing
 *			  the element to locate or add.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *			  'locationsDscPointer' will be set to a reference
 *			  scalar pointer containing the position in 'targetCPD'
 *		          where the value was found or added.
 *	addedDscPointer - optional (if not requested set to Nil).
 *                        the address of an uninitialized descriptor pointer
 *		          which will be created iff requested and meaningful.
 *			  If the value was added -
 *                             then 'addedDscPointer' will
 *			       be set to a reference scalar descriptor pointer.
 *                        Otherwise 'addedDscPointer' will be set to an
 *                             empty descriptor pointer.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LocateOrAddFromScalarDsc (
    M_CPD			*targetCPD,
    DSC_Descriptor		*keyDsc,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*addedDscPointer
)
{
    unsigned int trgPMapi;

/***** Align and validate target set vector and keydsc ... *****/
    rtVECTOR_Align (targetCPD);

    if (keyDsc->holdsAScalarReference ()) {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (*keyDsc));
    }

    if (!rtVECTOR_CPD_IsASet (targetCPD)) ERR_SignalFault (
	EC__InternalInconsistency,
	"LocateOrAddFromScalarDsc:  Target must be a set"
    );
    targetCPD->CheckConsistency ();
    bool wantAdded = IsntNil (addedDscPointer);

/*****  Find the key's usegment in the target *****/
    int origUSegSize = rtVECTOR_CPD_USegIndexSize (targetCPD);
    M_CPD *store = keyDsc->storeCPD ();
    int trgUSegi = LocateOrAddVectorSegment (
	targetCPD,
	store,
	DSC_Scalar_RType (DSC_Descriptor_Scalar (*keyDsc)),
	DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (*keyDsc)),
	-1,
	true
    );
    bool newPMapEntryNeeded = origUSegSize < rtVECTOR_CPD_USegIndexSize (targetCPD);

/*****  Get the trg usegment's uvector *****/
    M_CPD *trgUVector = GetUSegUVector (targetCPD, trgUSegi);

/*****  Do the lookup *****/
    int location;
    bool added = ScalarLocateOrAdd (trgUVector, keyDsc, &location);

/***** Make the target writable ... *****/
    targetCPD->EnableModifications ();

/*****  Get the PMap index for 'trgUSegi' ... *****/
    if (!newPMapEntryNeeded) {
	rtVECTOR_CPD_PMRD (targetCPD) = rtVECTOR_CPD_PMap (targetCPD);
	for (
	    trgPMapi = 0;
	    trgPMapi < rtVECTOR_CPD_PMapSize (targetCPD);
	    trgPMapi++, rtVECTOR_CPD_PMRD (targetCPD)++
	) {
	    if (rtVECTOR_CPD_PMRD_SegmentIndex (targetCPD) == trgUSegi)
		break;
	}
	newPMapEntryNeeded = trgPMapi == rtVECTOR_CPD_PMapSize (targetCPD);
    }
    if (newPMapEntryNeeded)
	trgPMapi = AddNewPMapEntryToSetVector (targetCPD, trgUSegi);

    int adjustment = rtVECTOR_CPD_PMRD_VectorOrigin (targetCPD);

/*****  If nothing was added ... close locations and return ... *****/
    if (!added) {
	if (wantAdded)
	    addedDscPointer->construct ();
    }
    else {
    /*****  adjust the vector origin of each subsequent pmap entry  *****/
	AdjustPMapEntryVectorOrigins (targetCPD, trgPMapi + 1, 1);

    /*****
     *  Fix the target vector's positional ptoken ...
     *****/

	UpdateVectorSetPositionalState (
	    targetCPD, trgUSegi, adjustment, trgUVector
	);

	if (wantAdded) {
	    DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (*keyDsc))->retain ();
	    addedDscPointer->constructReferenceScalar (
		DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (*keyDsc)), 0
	    );
	}
    }
    rtVECTOR_CPD_IsInconsistent (targetCPD) = false;

/*** Prepare the locations pointer ... ***/
    M_CPD *ptoken = rtPTOKEN_BasePToken (targetCPD, rtVECTOR_CPx_PToken);
    locationsDscPointer->constructReferenceScalar (ptoken, location + adjustment);

    trgUVector->release ();
}


/*---------------------------------------------------------------------------
 *****  Private routine to locate or add a set of values to a set vector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for the set vector in which the values
 *                        will be located or added.
 *	keyCPD		- a standard CPD for a set uvector containing
 *			  the elements to locate or add.
 *	keyVStore	- a standard CPD for the Value Store of 'keyCPD'.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *                        'locationsDscPointer' will be set to a link
 *                        descriptor pointer.  For each value in the
 *                        'keyDsc', the link will contain
 *                        the position in the 'targetCPD' where that value
 *                        was found or added.
 *	addedDscPointer - optional (if not requested set to Nil).
 *                        the address of an uninitialized descriptor pointer
 *		          which will be created iff requested and meaningful.
 *
 *			  'addedDscPointer' will be set to a link
 *                        descriptor pointer.  The link will contain the
 *                        positions from 'keyDsc' whose values were
 *                        added to the target set vector.  If none of the
 *                        values from the key were added, then
 *                        'addedDscPointer' will be set to an empty
 *                        descriptor pointer.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	If additions are returned, they will reside in the key's scratch pad.
 *
 *****/
PrivateFnDef void LocateOrAddFromUV (
    M_CPD			*targetCPD,
    M_CPD			*keyCPD,
    M_CPD			*keyVStore,
    DSC_Pointer			*locationsDscPointer,
    DSC_Pointer			*addedDscPointer
)
{
    rtLINK_CType *locationsLC, **locationsLinkC, *uvLocationsLC, *addedLC;
    int origUSegSize, trgUSegi, adjustment;
    unsigned int trgPMapi;

/***** Align and validate target set vector and key set UV ... *****/
    rtVECTOR_Align (targetCPD);
    AlignUVector (keyCPD);

    if (!rtVECTOR_CPD_IsASet (targetCPD) || !UV_CPD_IsASetUV (keyCPD)) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtVECTOR LocateOrAddFromUV:  Target and Key must be sets"
	);
    }

    bool wantAdded = IsntNil (addedDscPointer);

/***** If the key is empty, close the locations linkc and return ... *****/
    if (UV_CPD_ElementCount (keyCPD) == 0) {
    /***** Make the location link constructor ... *****/
	locationsLC = rtLINK_RefConstructor (targetCPD, rtVECTOR_CPx_PToken);

	locationsLC->Close (keyCPD, UV_CPx_PToken);
	locationsDscPointer->constructLink (locationsLC);

	if (wantAdded)
	    addedDscPointer->construct ();

	return;
    }
    targetCPD->CheckConsistency ();

/*****  Find the key's usegment in the target *****/
    origUSegSize = rtVECTOR_CPD_USegIndexSize (targetCPD);
    trgUSegi = LocateOrAddVectorSegment (
	targetCPD,
	keyVStore,
	(RTYPE_Type)M_CPD_RType (keyCPD),
	keyCPD,
	UV_CPx_RefPToken,
	true
    );
    bool newPMapEntryNeeded = origUSegSize < rtVECTOR_CPD_USegIndexSize (targetCPD);

/*****  Get the trg usegment's uvector *****/
    M_CPD *trgUVector = GetUSegUVector (targetCPD, trgUSegi);

/*****  Do the lookup *****/
    UVLocateOrAdd (trgUVector, keyCPD, &uvLocationsLC, &addedLC);

/***** Make the target writable ... *****/
    targetCPD->EnableModifications ();

/*****  Get the PMap index for 'trgUSegi' ... *****/
    if (!newPMapEntryNeeded) {
	rtVECTOR_CPD_PMRD (targetCPD) = rtVECTOR_CPD_PMap (targetCPD);
	for (
	    trgPMapi = 0;
	    trgPMapi < rtVECTOR_CPD_PMapSize (targetCPD);
	    trgPMapi++, rtVECTOR_CPD_PMRD (targetCPD)++
	) {
	    if (rtVECTOR_CPD_PMRD_SegmentIndex (targetCPD) == trgUSegi)
		break;
	}
	newPMapEntryNeeded = trgPMapi == rtVECTOR_CPD_PMapSize (targetCPD);
    }
    if (newPMapEntryNeeded)
	trgPMapi = AddNewPMapEntryToSetVector (targetCPD, trgUSegi);

    adjustment = rtVECTOR_CPD_PMRD_VectorOrigin (targetCPD);

/*****  If nothing was added ...  return ... *****/
    if (IsNil (addedLC)) {
	if (wantAdded)
	    addedDscPointer->construct ();
    }
    else {
    /*****  adjust the vector origin of each subsequent pmap entry  *****/
	AdjustPMapEntryVectorOrigins (targetCPD, trgPMapi + 1, addedLC->ElementCount ());

    /*****
     *  Fix the target vector's positional ptoken ...
     *****/

	UpdateVectorSetPositionalState (
	    targetCPD, trgUSegi, adjustment, trgUVector
	);

	if (wantAdded)
	    addedDscPointer->constructLink (addedLC);
	else
	    addedLC->release ();
    }
    rtVECTOR_CPD_IsInconsistent (targetCPD) = false;

/***** Make the location link constructor ... *****/
    locationsLC = rtLINK_RefConstructor (targetCPD, rtVECTOR_CPx_PToken);
    locationsLinkC = &locationsLC; /* used in AppendToLocations macro */

    AppendToLocations (uvLocationsLC);

    locationsLC->Close (keyCPD, UV_CPx_PToken);
    locationsDscPointer->constructLink (locationsLC);

    trgUVector->release ();
}


/*---------------------------------------------------------------------------
 *****  Routine to locate or add a set of values to a set vector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for the set vector in which the values
 *                        will be located or added.
 *	keyDsc		- the address of a descriptor containing
 *			  the elements to locate or add.
 *	locationsDscPointer
 *                      - the address of a descriptor pointer to be created.
 *	                  If 'keyDsc' is a scalar descriptor, then
 *			  'locationsDscPointer' will be set to a reference
 *			  scalar pointer containing the position in 'targetCPD'
 *		          where the value was found or added.
 *	                  If 'keyDsc' is not a scalar descriptor, then
 *                        'locationsDscPointer' will be set to a link
 *                        descriptor pointer.  For each value in the
 *                        'keyDsc', the link will contain
 *                        the position in the 'targetCPD' where that value
 *                        was found or added.
 *	addedDscPointer - optional (if not requested set to Nil).
 *                        the address of an uninitialized descriptor pointer
 *		          which will be created iff requested and meaningful.
 *
 *                        If 'keyDsc' is a scalar descriptor -
 *			    if the value was added -
 *                             then 'addedDscPointer' will
 *			       be set to a reference scalar descriptor pointer.
 *                          Otherwise 'addedDscPointer' will be set to an
 *                             empty descriptor pointer.
 *
 *                        If 'keyDsc' is not a scalar descriptor -
 *			     then 'addedDscPointer' will be set to a link
 *                           descriptor pointer.  The link will contain the
 *                           positions from 'keyDsc' whose values were
 *                           added to the target set vector.  If none of the
 *                           values from the key were added, then
 *                           'addedDscPointer' will be set to an empty
 *                           descriptor pointer.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	If additions are returned, they will reside in the key's scratch pad.
 *
 *****/
PublicFnDef void rtVECTOR_LocateOrAdd (
    M_CPD*		targetCPD,
    DSC_Descriptor*	keyDsc,
    DSC_Pointer*	locationsDscPointer,
    DSC_Pointer*	addedDscPointer
) {
    LocateOrAddFromValueDCount++;

/*****  Decode the key descriptor ... *****/
    if (keyDsc->isScalar ()) {
	LocateOrAddFromScalarDsc (
	    targetCPD, keyDsc, locationsDscPointer, addedDscPointer
	);
    }
    else if (keyDsc->holdsNonScalarValues ()) {
	LocateOrAddFromUV (
	    targetCPD,
	    DSC_Descriptor_Value (*keyDsc),
	    keyDsc->storeCPDIfAvailable (),
	    locationsDscPointer,
	    addedDscPointer
	);
    }
    else if (keyDsc->holdsNonScalarReferences ()) {
	LocateOrAddFromUV (
	    targetCPD,
	    DSC_Descriptor_Reference (*keyDsc),
	    keyDsc->storeCPDIfAvailable (),
	    locationsDscPointer,
	    addedDscPointer
	);
    }
    else ERR_SignalFault (
	EC__InternalInconsistency,
	"rtVECTOR_LocateOrAdd: Unimplemented key descriptor type"
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to determine if a vector is a set.
 *
 *  Arguments:
 *      vectorCPD	- a standard CPD for the vector to check
 *	refNilsLC 	- a pointer to a link constructor this routine will
 *                        create to point out reference Nils which violate
 *                        the definition of a reference Set. Will be set to
 *                        Nil if the vector is a set, or if it cannot be fixed
 *                        by simply pointing out referenceNils.
 *
 *  Returns:
 *	True if a set, False if not.
 *
 *  Note: The Set Attribute of the vector is maintained by the vector's user
 *        by only modifying the vector by use of the LocateOrAdd function.
 *        The user is charged with the responsibility to avoid use of
 *        'Set violating' operations such as Assignment. However,
 *        the best efforts of the user to maintain the Set attribute can
 *        be subverted if an object being referenced is deleted. This
 *        routine is provided to discover that occurence and to provide
 *        a means (via refNilsLC) to recover from it.
 *****/
PublicFnDef int rtVECTOR_IsASet (M_CPD *vectorCPD, rtLINK_CType **refNilsLC) {
/*-------------------------*
 *  Link Traversal Macros  *
 *-------------------------*/
#define handleNil(position, count, origin); /** should not happen **/

#define handleRange(position, count, origin) {\
     rtLINK_AppendRange (*refNilsLC, (origin) + adjustment, count);\
}

#define handleRepeat(position, count, origin) {\
     /*** Should not happen. rtREFUV_IsASet() should only append ranges ***/\
     rtLINK_AppendRange (*refNilsLC, (origin) + adjustment, 1);\
}

/*-------------*
 *  Code Body  *
 *-------------*/
    *refNilsLC = NilOf (rtLINK_CType*);

    /*****
     * If vector is not marked as being a set, then the problem
     * is greater than what would be caused by a referential alignment
     * of a reference usegment
     *****/
    if (!rtVECTOR_CPD_IsASet (rtVECTOR_Align (vectorCPD)))
	return false;

    /***** Unless we learn otherwise, ... *****/
    bool result = true;

    *refNilsLC = rtLINK_RefConstructor (vectorCPD, rtVECTOR_CPx_PToken);

    /***** Traverse the PMap, checking each usegment for "setness" *****/
    rtVECTOR_CPD_PMRD (vectorCPD) = rtVECTOR_CPD_PMap (vectorCPD);
    for (unsigned int xPMRD = 0; xPMRD < rtVECTOR_CPD_PMapSize (vectorCPD); xPMRD++) {
	rtVECTOR_CPD_USD (vectorCPD) =
	    rtVECTOR_CPD_USegArray (vectorCPD) +
	    rtVECTOR_CPD_PMRD_SegmentIndex (vectorCPD);
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
	M_CPD *uvector = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

	if (RTYPE_C_RefUV == (RTYPE_Type)M_CPD_RType (uvector)) {
	    rtLINK_CType *usegNilsLC = NilOf (rtLINK_CType*);
	    if (!rtREFUV_IsASet (uvector, &usegNilsLC)) {
		result = false;
		if (IsNil (usegNilsLC)) {
		    (*refNilsLC)->release ();
		    *refNilsLC = NilOf (rtLINK_CType*);
		    uvector->release ();
		    vectorCPD->EnableModifications ();
		    rtVECTOR_CPD_IsASet (vectorCPD) = false;
		    break;
		}
		int adjustment = rtVECTOR_CPD_PMRD_VectorOrigin (vectorCPD);
		rtLINK_TraverseRRDCList (
		    usegNilsLC, handleNil, handleRepeat, handleRange
		);
		usegNilsLC->release ();
	    }
	}
	else if (!UV_CPD_IsASetUV (uvector)) {
	    result = false;
	    (*refNilsLC)->release ();
	    *refNilsLC = NilOf (rtLINK_CType*);
	    uvector->release ();
	    vectorCPD->EnableModifications ();
	    rtVECTOR_CPD_IsASet (vectorCPD) = false;
	    break;
	}
	uvector->release ();

	rtVECTOR_CPD_PMRD (vectorCPD)++;
    }
    if (result || 0 == (*refNilsLC)->ElementCount ()) {
	result = true;
	(*refNilsLC)->release ();
	*refNilsLC = NilOf (rtLINK_CType*);
    }
    if (*refNilsLC != NilOf (rtLINK_CType*)) {
	M_CPD *ptoken = rtPTOKEN_New (
	    vectorCPD->ScratchPad (), (*refNilsLC)->ElementCount ()
	);
	(*refNilsLC)->Close (ptoken);
	ptoken->release ();
    }

    return result;

#undef handleNil
#undef handleRange
#undef handleRepeat
}

/*---------------------------------------------------------------------------
 *****  Routine to determine if a vector is a set, and to restore the
 *      set attribute to it and all its usegments if the criteria for set,
 *      unique and sorted, are met.
 *
 *  Arguments:
 *      vectorCPD	- a standard CPD for the vector to check
 *
 *  Returns:
 *	True if a set, False if not.
 *
 *****/
PublicFnDef int rtVECTOR_RestoreSetAttribute (M_CPD *vectorCPD) {
    bool result = true;
    M_POP lastVStorePOP;

    // initialize to something (anything) so that compiler doesn't complain ...
    vectorCPD->containerHandle ()->constructReference (&lastVStorePOP);

/*****
 * Ensure that any positional adjustments are propogated to the usegments
 *****/
    rtVECTOR_Align (vectorCPD);

/***** Traverse the PMap, checking each usegment for "setness" *****/
    rtVECTOR_CPD_PMRD (vectorCPD) = rtVECTOR_CPD_PMap (vectorCPD);
    for (
	unsigned int xPMRD = 0;
	xPMRD < rtVECTOR_CPD_PMapSize (vectorCPD) && result;
	xPMRD++
    ) {
	rtVECTOR_CPD_USD (vectorCPD) =
	    rtVECTOR_CPD_USegArray (vectorCPD) +
	    rtVECTOR_CPD_PMRD_SegmentIndex (vectorCPD);

	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
	M_CPD *uvector = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

	if (RTYPE_C_RefUV == (RTYPE_Type)M_CPD_RType (uvector)) {
	    if (!rtREFUV_RestoreSetAttribute (uvector))
		result = false;
	}
	else if (!UV_CPD_IsASetUV (uvector))
	    result = false;

	uvector->release ();

/*****  Make sure that usegments are in POP order  *****/
	M_POP thisVStorePOP = rtVECTOR_CPD_USD_VStore (vectorCPD);
	if (xPMRD > 0 && M_ComparePOPs (thisVStorePOP, lastVStorePOP) <= 0)
	    result = false;
	lastVStorePOP = thisVStorePOP;

	rtVECTOR_CPD_PMRD (vectorCPD)++;
    }
    vectorCPD->EnableModifications ();
    rtVECTOR_CPD_IsASet (vectorCPD) = result;
    return result;
}

/************************************************************
 *****  Standard Representation Type Services Routines  *****
 ************************************************************/

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
)
{
    rtVECTOR_Type const *vector = (rtVECTOR_Type const *)(preambleAddress+1);

    ownerASD->Release (&rtVECTOR_V_PToken (vector));
    ownerASD->Release (
	(M_POP*)(rtVECTOR_V_USegArray (vector)),
	rtVECTOR_V_USegArraySize (vector) * rtVECTOR_USD_POPsPerUSD
    );
}


/***********
 *  Saver  *
 ***********/

/*****  Save Routine  *****/
/*---------------------------------------------------------------------------
 *****  Routine to 'save' a Vector.
 *
 *  Note:
 *	The U-Segment Index is ordered by internal POP content.  When a vector
 *	is saved, the POPs it references may change.  The save function must
 *	take this into account and reorder the U-Segment Index to reflect the
 *	the new V-Store POPs present in the U-Segment Array.  If the vector is
 *	a set,  this strategy of reordering would be innappropriate. Until the
 *	ordering mechanism is changed to use a UID that is not
 *	subject to change as POP's are,  saving of vector sets that reference
 *	transient stores must be disallowed.
 *****/
bool rtVECTOR_Handle::PersistReferences () {
    rtVECTOR_Type	*vector = (rtVECTOR_Type *)ContainerContent ();
    rtVECTOR_USDType	*uSegmentArray = rtVECTOR_V_USegArray (vector);

/*****  Check for validity of vector sets  *****/
    if (rtVECTOR_V_IsASet (vector))
	for (int i = 0; i < rtVECTOR_V_USegArraySize (vector); i++) {
	    M_TOP iTOP;
	    LocateNameOf (this, &rtVECTOR_USD_VStore (&uSegmentArray[i]), iTOP);
	    if (iTOP.isAScratchPadName ()) ERR_SignalFault (
		EC__InternalInconsistency,
		"rtVECTOR_Save: Transient store in vector set"
	    );
	}

/*****  Save the P-Token and U-Segment Components...  *****/
    bool result = Persist (&rtVECTOR_V_PToken (vector)) && Persist (
	(M_POP*)(uSegmentArray),
	rtVECTOR_V_USegArraySize (vector) * rtVECTOR_USD_POPsPerUSD
    );

/*****  ...and re-sort the U-Segment Index.  *****/
    USISortUSDArray = uSegmentArray;
    qsort (
	(char *)rtVECTOR_V_USegIndex (vector),
	rtVECTOR_V_USegIndexSize (vector),
	sizeof (int),
	(VkComparator)USISortPredicate
    );

    return result;
}


/*********************************************
 *****  Standard Vector Marking Routine  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'mark' containers referenced by a Vector.
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
    rtVECTOR_Type const		*vector = (rtVECTOR_Type const *)(pContainer + 1);
    rtVECTOR_USDType const	*uSegmentArray = rtVECTOR_V_USegArray (vector);

/*****  Mark the P-Token and U-Segment Components...  *****/
    pSpace->Mark (&rtVECTOR_V_PToken (vector));
    pSpace->Mark (
	(M_POP const*)(uSegmentArray),
	rtVECTOR_V_USegArraySize (vector) * rtVECTOR_USD_POPsPerUSD
    );
}


/****************************************
 *****  Data Base Update Utilities  *****
 ****************************************/

PublicFnDef void rtVECTOR_WriteDBUpdateInfo (M_CPD *vectorCPD, char const *itemName) {
/*****  Do NOT Align Vector ... *****/
/*****  Determine the number of usegments  *****/
    int i;
    for (i=0; i<rtVECTOR_CPD_USegIndexSize (vectorCPD); i++) {
	rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + i;

//	if (M_POPIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	if (vectorCPD->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	    break;
    }
/***** Output the vector DB File Record ... *****/
    unsigned int vectorIndex = vectorCPD->ContainerIndex ();
    DBUPDATE_OutputVectorRecord	(
	itemName, vectorIndex, M_POP_ContainerIndex (rtVECTOR_CPD_PToken (vectorCPD)), i
    );

/***** Now dump the USegments ... *****/
    for (i=0; i<rtVECTOR_CPD_USegIndexSize (vectorCPD); i++) {
	rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + i;

//	if (M_POPIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	if (vectorCPD->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (vectorCPD)))
	    break;
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
	M_CPD *uvectorCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

	DBUPDATE_OutputUSegmentRecord (
	    M_CPD_RType (uvectorCPD),
	    M_POP_D_ContainerIndex (rtVECTOR_CPD_USD_Values (vectorCPD)),
	    M_POP_D_ContainerIndex (rtVECTOR_CPD_USD_PToken (vectorCPD)),
	    vectorIndex
	);

	uvectorCPD->release ();
    }
}


/*********************
 *  Print Utilities  *
 *********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a vector's P-Map
 *
 *  Argument:
 *	vectorCPD		- a standard CPD for the vector whose P-Map
 *				  is to be printed.
 *
 *  Returns:
 *	NOTHING	- Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintPMap (M_CPD *vectorCPD) {
    rtVECTOR_PMRDType
	*pMapPtr, *pMapLimit;
    char
	leadingChar;
    int
	pMapSize;

    IO_printf ("P-Map[%d]", pMapSize = rtVECTOR_CPD_PMapSize (vectorCPD));

    for (
	pMapLimit = pMapSize + (pMapPtr = rtVECTOR_CPD_PMap (vectorCPD)),
	leadingChar = '{';
        pMapPtr < pMapLimit;
	pMapPtr++, leadingChar = ' '
    ) IO_printf (
	"%c%d:#%d[%d+%d]",
	leadingChar,
	rtVECTOR_PMRD_VectorOrigin (pMapPtr),
	rtVECTOR_PMRD_SegmentIndex (pMapPtr),
	rtVECTOR_PMRD_SegmentOrigin (pMapPtr),
	rtVECTOR_PMRD_SegmentLength (pMapPtr)
    );

    IO_printf ("}");
}


/*---------------------------------------------------------------------------
 *****  Internal Routine to display a vector's U-Segment Index
 *
 *  Arguments:
 *	vectorCPD		- a standard CPD for the vector whose
 *				  U-Segment Index is to be displayed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintUSegIndex (M_CPD *vectorCPD) {
    int
	uSegIndexSize, *uSegIndexPtr, *uSegIndexLimit;
    char
	leadingChar;

    IO_printf (
	"U-Segment Index[%d]",
	uSegIndexSize = rtVECTOR_CPD_USegIndexSize (vectorCPD)
    );

    for (
	uSegIndexLimit =
	    uSegIndexSize +
	        (uSegIndexPtr = rtVECTOR_CPD_USegIndex (vectorCPD)),
	leadingChar = '{';
        uSegIndexPtr < uSegIndexLimit;
	uSegIndexPtr++, leadingChar = ' '
    ) IO_printf ("%c%d", leadingChar, *uSegIndexPtr);

    IO_printf ("}");
}


/*---------------------------------------------------------------------------
 *****  Internal Routine to display a vector's U-Segment Array
 *
 *  Arguments:
 *	vectorCPD		- a standard CPD for the vector whose
 *				  U-Segment Array is to be displayed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintUSegArray (M_CPD *vectorCPD) {
    int				i, uSegArraySize;

    IO_printf (
	"U-Segment Array[%d](FB:%d):",
	uSegArraySize = rtVECTOR_CPD_USegArraySize (vectorCPD),
	rtVECTOR_CPD_USegFreeBound (vectorCPD)
    );

    for (
	rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD),
        i = 0;
        i < uSegArraySize;
	rtVECTOR_CPD_USD (vectorCPD)++, i++
    )
    {
	IO_printf ("\n%d\tP-Token: ", i);
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);
	RTYPE_QPrint (vectorCPD, rtVECTOR_CPx_USDCursor);

	IO_printf ("\n\tV-Store: ");
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_VStore);
	RTYPE_QPrint (vectorCPD, rtVECTOR_CPx_USDCursor);

	IO_printf ("\n\tValues : ");
	rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
	RTYPE_QPrint (vectorCPD, rtVECTOR_CPx_USDCursor);
    }
}


/*---------------------------------------------------------------------------
 *****  Internal routine to print the elements associated with a P-Map entry.
 *
 *  Argument:
 *	vectorCPD		- a standard CPD for a vector whose P-Map
 *				  pointer is positioned at the entry to be
 *				  printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintPMRDUSegmentValues (M_CPD *vectorCPD) {
    rtVECTOR_PMRDType *pmrd = rtVECTOR_CPD_PMRD (vectorCPD);
    int
	segmentIndex	= rtVECTOR_PMRD_SegmentIndex (pmrd),
	segmentOrigin	= rtVECTOR_PMRD_SegmentOrigin (pmrd),
	segmentSize	= rtVECTOR_PMRD_SegmentLength (pmrd);
    M_CPD
	*pTokenCPD,
	*valuesCPD,
	*xValuesCPD;

    rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + segmentIndex;

    IO_printf (
	"\n%d:#%d[%d+%d]\t: ",
	rtVECTOR_PMRD_VectorOrigin (pmrd),
	segmentIndex,
	segmentOrigin,
	segmentSize
    );

/*****  Create a link for the values to be displayed, ...  *****/
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);
    rtLINK_CType *linkc = rtLINK_AppendRange (
	rtLINK_PosConstructor (
	    pTokenCPD = rtPTOKEN_New (vectorCPD->ScratchPad (), segmentSize), -1
	), segmentOrigin, segmentSize
    )->Close (vectorCPD, rtVECTOR_CPx_USDCursor);

/*****  ...obtain them, ...  *****/
    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
    valuesCPD = vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor);

/*****  ...display them.  *****/
    if (RTYPE_PerformHandlerOperation (
	    RTYPE_DoLCExtract,
	    (RTYPE_Type)M_CPD_RType (valuesCPD),
	    &xValuesCPD,
	    valuesCPD,
	    linkc
	) == 0
    ) RTYPE_Print (xValuesCPD, -1);

/*****  ...and clean-up.  *****/
    pTokenCPD->release ();
    valuesCPD->release ();
    xValuesCPD->release ();
    linkc->release ();
}


/**********************
 *  Standard Printer  *
 **********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to produce a 'standard' display of a vector.
 *
 *  Argument:
 *	vectorCPD		- a standard CPD for the vector to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintVector (M_CPD *vectorCPD) {
    int i, n;

    rtVECTOR_Align (vectorCPD);

    IO_printf ("%s{", RTYPE_TypeIdAsString (RTYPE_C_Vector));
    IO_printf ("\nIsASet=%d", rtVECTOR_CPD_IsASet (vectorCPD));
    IO_printf ("\nIsInconsistent=%d", rtVECTOR_CPD_IsInconsistent (vectorCPD));

    for (
	i = 0,
        n = rtVECTOR_CPD_PMapSize (vectorCPD),
	rtVECTOR_CPD_PMRD (vectorCPD) = rtVECTOR_CPD_PMap (vectorCPD);
	i < n;
	i++, rtVECTOR_CPD_PMRD (vectorCPD)++
    ) PrintPMRDUSegmentValues (vectorCPD);

    IO_printf ("}");
}


/***********************
 *  Recursive Printer  *
 ***********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to produce a recursive display of a vector.
 *
 *  Argument:
 *	vectorCPD		- a standard CPD for the vector to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void RPrintVector (M_CPD *vectorCPD) {
    rtVECTOR_Align (vectorCPD);

    IO_printf ("\n********************\n");
    vectorCPD->print ();
    IO_printf (":\n\t");
    PrintPMap (vectorCPD);
    IO_printf ("\n\t");
    PrintUSegIndex (vectorCPD);
    IO_printf ("\n\t");
    PrintUSegArray (vectorCPD);
    IO_printf ("\n********************\n");
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	rtVECTOR_New (RTYPE_QRegisterCPD (parameterArray[0]))
    );
}

IOBJ_DefineNilaryMethod (NewSetDM) {
    M_CPD *pPPT = rtPTOKEN_New (IOBJ_ScratchPad, 0);
    M_CPD *pSet = rtVECTOR_NewSetVector (pPPT);
    pPPT->release ();
    
    return RTYPE_QRegister (pSet);
}

IOBJ_DefineUnaryMethod (TracesOnDM) {
    TracingVCAllocator		=
    TracingUSDCAllocator	=
    TracingPMRDCAllocator	=
    TracingReconstructor	=
    TracingPMapBrief		=
    TracingPMapSearches		= true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM) {
    TracingVCAllocator		=
    TracingUSDCAllocator	=
    TracingPMRDCAllocator	=
    TracingReconstructor	=
    TracingPMapBrief		=
    TracingPMapSearches		= false;

    return self;
}

IOBJ_DefineNilaryMethod (VCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingVCAllocator);
}

IOBJ_DefineNilaryMethod (USDCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingUSDCAllocator);
}

IOBJ_DefineNilaryMethod (PMRDCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingPMRDCAllocator);
}

IOBJ_DefineNilaryMethod (ReconstructorTraceDM) {
    return IOBJ_SwitchIObject (&TracingReconstructor);
}

IOBJ_DefineNilaryMethod (PMapSearchesTraceDM) {
    return IOBJ_SwitchIObject (&TracingPMapSearches);
}

IOBJ_DefineNilaryMethod (PMapBriefTraceDM) {
    return IOBJ_SwitchIObject (&TracingPMapBrief);
}


IOBJ_DefineUnaryMethod (DisplayParametersDM) {
    IO_printf ("\nVector Tuning Parameter Settings\n");
    IO_printf (  "--------------------------------\n");
    IO_printf ("ImplicitPropertyCleanup	is %s\n", ImplicitPruningOfUSegmentsEnabled ? "Enabled" : "Disabled");
    IO_printf ("VisionPre7Ordering	is %s\n", g_bPOPOrderingEnabled ? "Enabled" : "Disabled");
    IO_printf ("USegmentCountThreshold	=  %d\n", USegmentCountThreshold);
    IO_printf ("\n");
    return self;
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nVector Routines Usage Counts\n");
    IO_printf (  "-----------------------------------------\n");

    IO_printf ("NewCount			= %u\n", NewCount);
    IO_printf ("\n");

    IO_printf ("AllocateVCCount			= %u\n", AllocateVCCount);
    IO_printf ("AllocateUSDCCount		= %u\n", rtVECTOR_USDC::AllocationCount ());
    IO_printf ("AllocatePMRDCCount		= %u\n", rtVECTOR_PMRDC::AllocationCount ());
    IO_printf ("AllocatePMRecCount		= %u\n", AllocatePMRecCount);
    IO_printf ("\n");

    IO_printf ("FreeVCCount			= %u\n", FreeVCCount);
    IO_printf ("FreeUSDCCount			= %u\n", rtVECTOR_USDC::DeallocationCount ());
    IO_printf ("FreePMRDCCount			= %u\n", rtVECTOR_PMRDC::DeallocationCount ());
    IO_printf ("FreePMRecCount			= %u\n", FreePMRecCount);
    IO_printf ("\n");

    IO_printf ("AlignVectorCount		= %u\n", AlignVectorCount);
    IO_printf ("USegmentDeletionCount		= %u\n", USegmentDeletionCount);
    IO_printf ("USegmentCompactionCount		= %u\n", USegmentCompactionCount);
    IO_printf ("MaxUSegmentCountEncountered	= %u\n", MaxUSegmentCountEncountered);
    IO_printf ("AlignConstructorCount		= %u\n", AlignConstructorCount);
    IO_printf ("\n");

    IO_printf ("ToSetVCCount			= %u\n", ToSetVCCount);
    IO_printf ("VCToSetVCCount			= %u\n", VCToSetVCCount);
    IO_printf ("\n");

    IO_printf ("LCAssignFromValueDCount		= %u\n", LCAssignFromValueDCount);
    IO_printf ("LCAssignFromVCCount		= %u\n", LCAssignFromVCCount);
    IO_printf ("RefAssignFromValueDCount	= %u\n", RefAssignFromValueDCount);
    IO_printf ("RefAssignFromVCCount		= %u\n", RefAssignFromVCCount);
    IO_printf ("\n");

    IO_printf ("LCExtractCount			= %u\n", LCExtractCount);
    IO_printf ("LCExtractHuntCount		= %u\n", LCExtractHuntCount);
    IO_printf ("SimpleLCExtractCount		= %u\n", SimpleLCExtractCount);
    IO_printf ("RefExtractCount			= %u\n", RefExtractCount);
    IO_printf ("RefExtractHuntCount		= %u\n", RefExtractHuntCount);
    IO_printf ("RefExtractFromVCCount		= %u\n", RefExtractFromVCCount);
    IO_printf ("\n");

    IO_printf ("USegArrayExpansionCount		= %u\n", USegArrayExpansionCount);
    IO_printf ("USegIndexHitCount		= %u\n", USegIndexHitCount);
    IO_printf ("USegIndexMissCount		= %u\n", USegIndexMissCount);
    IO_printf ("USegIndexLSearchCount		= %u\n", USegIndexLSearchCount);
    IO_printf ("USegIndexLSearchComparisons	= %u\n", USegIndexLSearchComparisons);
    IO_printf ("\n");

    IO_printf ("ToVectorCount			= %u\n", ToVectorCount);
    IO_printf ("ToConstructorCount		= %u\n", ToConstructorCount);
    IO_printf ("\n");

    IO_printf ("ReOrderCount			= %u\n", ReOrderCount);
    IO_printf ("ReOrderVCCount			= %u\n", ReOrderVCCount);
    IO_printf ("DistributeCount			= %u\n", DistributeCount);
    IO_printf ("DistributeVCCount		= %u\n", DistributeVCCount);
    IO_printf ("\n");

    IO_printf ("PartitionStatisticsCount	= %u\n", PartitionStatsCount);
    IO_printf ("PartitionSortIndicesCount	= %u\n", PartitionSortIndicesCount);
    IO_printf ("PartitionedPartitionCount	= %u\n", PartitndPartitionCount);
    IO_printf ("\n");

    IO_printf ("MakeConstructorLinksCount	= %u\n", MakeConstructorLinksCount);
    IO_printf ("VCSubsetInStoreCount		= %u\n", VCSubsetInStoreCount);
    IO_printf ("VCSubsetOfTypeCount             = %u\n", VCSubsetOfTypeCount);
    IO_printf ("SubsetInStoreCount		= %u\n", SubsetInStoreCount);
    IO_printf ("SubsetOfTypeCount               = %u\n", SubsetOfTypeCount);
    IO_printf ("SimplifyVCToDescriptorCount	= %u\n", SimplifyVCToDescriptorCount);
    IO_printf ("SimplifyToDescriptorCount	= %u\n", SimplifyToDescriptorCount);
    IO_printf ("\n");

    IO_printf ("LocateOrAddFromVCCount		= %u\n", LocateOrAddFromVCCount);
    IO_printf ("LocateOrAddFromValueDCount	= %u\n", LocateOrAddFromValueDCount);
    IO_printf ("LookupFromVCCount		= %u\n", LookupFromVCCount);
    IO_printf ("LookupFromValueDCount		= %u\n", LookupFromValueDCount);
    IO_printf ("\n");

    return self;
}

IOBJ_DefineUnaryMethod (ResetCountsDM) {
    NewCount = 0;
    
    rtVECTOR_USDC::ResetCounts ();
    rtVECTOR_PMRDC::ResetCounts ();

    AllocateVCCount = 0;
    AllocatePMRecCount = 0;

    FreeVCCount = 0;
    FreePMRecCount = 0;

    AlignVectorCount = 0;
    USegmentDeletionCount = 0;
    USegmentCompactionCount = 0;
    AlignConstructorCount = 0;
    
    ToSetVCCount = 0;
    VCToSetVCCount = 0;
    
    LCAssignFromValueDCount = 0;
    LCAssignFromVCCount = 0;
    RefAssignFromValueDCount = 0;
    RefAssignFromVCCount = 0;

    LCExtractCount = 0;
    LCExtractHuntCount = 0;
    SimpleLCExtractCount = 0;
    RefExtractCount = 0;
    RefExtractHuntCount = 0;
    RefExtractFromVCCount = 0;
    

    USegArrayExpansionCount = 0;
    USegIndexHitCount = 0;
    USegIndexMissCount = 0;
    USegIndexLSearchCount = 0;
    USegIndexLSearchComparisons = 0;
    

    ToVectorCount = 0;
    ToConstructorCount = 0;
    

    ReOrderCount = 0;
    ReOrderVCCount = 0;
    DistributeCount = 0;
    DistributeVCCount = 0;
    

    PartitionStatsCount = 0;
    PartitionSortIndicesCount = 0;
    PartitndPartitionCount = 0;
    

    MakeConstructorLinksCount = 0;
    VCSubsetInStoreCount = 0;
    VCSubsetOfTypeCount = 0;
    SubsetInStoreCount = 0;
    SubsetOfTypeCount = 0;
    SimplifyVCToDescriptorCount = 0;
    SimplifyToDescriptorCount = 0;
    

    LocateOrAddFromVCCount = 0;
    LocateOrAddFromValueDCount = 0;
    LookupFromVCCount = 0;
    LookupFromValueDCount = 0;
    
    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (ElementCountDM)
{
    return IOBJ_IntIObject (
	rtVECTOR_CPD_ElementCount (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (PTokenDM)
{
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVECTOR_CPx_PToken);
}

IOBJ_DefineUnaryMethod (PMapSizeDM)
{
    return IOBJ_IntIObject (rtVECTOR_CPD_PMapSize (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (USegArraySizeDM)
{
    return IOBJ_IntIObject (
	rtVECTOR_CPD_USegArraySize (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (USegFreeBoundDM)
{
    return IOBJ_IntIObject (
	rtVECTOR_CPD_USegFreeBound (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (USegIndexSizeDM)
{
    return IOBJ_IntIObject (
	rtVECTOR_CPD_USegIndexSize (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (AlignDM) {
    rtVECTOR_Align (RTYPE_QRegisterCPD (self));

    return self;
}


IOBJ_DefineMethod (AtDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self), *result;

    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
	rtREFUV_Type_Reference ref;
        DSC_InitReferenceScalar (
	    ref,
	    rtVECTOR_CPD_RowPTokenCPD (sourceCPD),
	    IOBJ_IObjectValueAsInt (parameterArray[0])
	);

	DSC_Descriptor resultValueD;
	rtVECTOR_RefExtract (sourceCPD, &ref, &resultValueD);
	result = rtDSC_Pack (IOBJ_ScratchPad, &resultValueD);
	resultValueD.clear ();
	DSC_ClearScalar (ref);
    }
    else {
	rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));
	rtVECTOR_Extract (sourceCPD, linkc, &result);
	linkc->release ();
    }

    return RTYPE_QRegister (result);
}


IOBJ_DefineMethod (AtPutDM) {
    rtREFUV_Type_Reference	ref;
    rtLINK_CType		*linkc = NilOf (rtLINK_CType*);
    rtVECTOR_CType		*vectorc;
    M_CPD			*targetCPD = RTYPE_QRegisterCPD (self),
				*sourceCPD;
    DSC_Descriptor		valueD;

/*****  Decode the 'at:' parameter...  *****/
    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
        DSC_InitReferenceScalar (
	    ref,
	    rtVECTOR_CPD_RowPTokenCPD (targetCPD),
	    IOBJ_IObjectValueAsInt (parameterArray[0])
	)
    }
    else linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));

/*****  Decode the 'put:' parameter...  *****/
    if (rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[1], &valueD))
	vectorc = NilOf (rtVECTOR_CType *);
    else switch (M_CPD_RType(sourceCPD=RTYPE_QRegisterCPD(parameterArray[1]))) {
    case RTYPE_C_Vector:
	vectorc = new rtVECTOR_CType (sourceCPD);
	break;
    default:
	if (linkc)
	    linkc->release ();
	else
	    DSC_ClearScalar (ref)

        ERR_SignalFault (
	    EC__UsageError, UTIL_FormatMessage (
		"'at:put:' Don't know how to handle %s source",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (sourceCPD))
	    )
	);

	break;
    }

    if (vectorc) {
	if (linkc)
	    rtVECTOR_Assign (targetCPD, linkc, vectorc);
	else 
	    rtVECTOR_Assign (targetCPD, &ref, vectorc);
    }
    else if (linkc)
	rtVECTOR_Assign (targetCPD, linkc, &valueD);
    else 
	rtVECTOR_Assign (targetCPD, &ref, &valueD);

    if (vectorc)
	vectorc->release ();
    else
	valueD.clear ();

    if (linkc)
	linkc->release ();
    else {
	DSC_ClearScalar (ref);
    }

    return self;
}


IOBJ_DefineMethod (GoToUSegmentDM) {
    M_CPD *vectorCPD = RTYPE_QRegisterCPD (self);
    int useg = IOBJ_IObjectValueAsInt (parameterArray[0]);


    useg = (useg >= rtVECTOR_CPD_USegArraySize (vectorCPD))
    ? (rtVECTOR_CPD_USegArraySize (vectorCPD) - 1)
    : (useg < 0)
    ? 0
    : useg;

    rtVECTOR_CPD_USD (vectorCPD) = rtVECTOR_CPD_USegArray (vectorCPD) + useg;

    return self;
}

IOBJ_DefineUnaryMethod (GetUSegVStoreDM) {
    M_CPD *vectorCPD = RTYPE_QRegisterCPD (self);

    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_VStore);
    return RTYPE_QRegister (vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor));
}

IOBJ_DefineUnaryMethod (GetUSegUVectorDM) {
    M_CPD *vectorCPD = RTYPE_QRegisterCPD (self);

    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_Values);
    return RTYPE_QRegister (vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor));
}

IOBJ_DefineUnaryMethod (GetUSegPTokenDM) {
    M_CPD *vectorCPD = RTYPE_QRegisterCPD (self);

    rtVECTOR_CPD_SetUSDCursor (vectorCPD, rtVECTOR_USD_PToken);
    return RTYPE_QRegister (vectorCPD->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken));
}


IOBJ_DefineMethod (ReorderDM) {
    return RTYPE_QRegister (
	rtVECTOR_ReOrder (
	    RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0])
	)->ToVector ()
    );
}

IOBJ_DefineMethod (DistributeVCDM) {
    return RTYPE_QRegister (
	rtVECTOR_Distribute (
	    RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0])
	)->ToVector ()
    );
}


IOBJ_DefineMethod (PartitionStatsDM) {
    rtVECTOR_CType *vectorc = new rtVECTOR_CType (RTYPE_QRegisterCPD (self));
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[1]));
    rtVECTOR_PS_Type type = (rtVECTOR_PS_Type)IOBJ_IObjectValueAsInt (parameterArray[0]);

    M_CPD *resultCPD = vectorc->PartitionedStatistics (
	linkc, type, IOBJ_ScratchPad->KOT ()
    );
    linkc->release ();
    vectorc->release ();

    return RTYPE_QRegister (resultCPD);
}

IOBJ_DefineMethod (SubsetInStoreDM) {
    rtLINK_CType *linkc = rtVECTOR_SubsetInStore (
	RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0])
    );

    if (linkc)
	return RTYPE_QRegister (linkc);

    IO_printf ("empty link\n");
    return self;
}

IOBJ_DefineUnaryMethod (SimplifyToDescriptorDM) {
    DSC_Descriptor dsc;
    if (rtVECTOR_SimplifiedToMonotype (RTYPE_QRegisterCPD (self), &dsc)) {
	M_CPD *dscCPD = rtDSC_Pack (IOBJ_ScratchPad, &dsc);
	dsc.clear ();

	return RTYPE_QRegister (dscCPD);
    }

    return self;
}

IOBJ_DefineMethod (LookupFromUVDM) {
    M_CPD *store = RTYPE_QRegisterCPD (parameterArray[1]);
    store->retain ();

    DSC_Descriptor keyDsc;
    keyDsc.constructMonotype (store, RTYPE_QRegisterCPD (parameterArray[0]));

    rtLINK_LookupCase lookupCase = (rtLINK_LookupCase)IOBJ_IObjectValueAsInt (
	parameterArray[2]
    );

    DSC_Pointer locatedDscPtr, locationsDscPtr;
    rtVECTOR_LookupFromValueD (
	RTYPE_QRegisterCPD (self), &keyDsc, lookupCase, &locationsDscPtr, &locatedDscPtr
    );
    keyDsc.clear ();

    if (locatedDscPtr.isntEmpty ()) {
	IO_printf ("\nLocated LinkC:\n");
	DSC_Pointer_Link (locatedDscPtr)->DebugPrint ();
	locatedDscPtr.clear ();
    }

    M_CPD *location = DSC_Pointer_Link (locationsDscPtr).realizedContainer ();
    location->retain ();
    locationsDscPtr.clear ();
    return RTYPE_QRegister (location);
}

IOBJ_DefineMethod (LookupFromScalarDM) {
    DSC_Descriptor keyDsc;
    if (!rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[0], &keyDsc)) {
	ERR_SignalFault (
	    EC__UsageError,
	    "LookupFromScalarDebugMethod: Unknown scalar type"
	);
    }

    rtLINK_LookupCase lookupCase = (rtLINK_LookupCase)IOBJ_IObjectValueAsInt (
	parameterArray[1]
    );

    DSC_Pointer locatedDscPtr, locationsDscPtr;
    rtVECTOR_LookupFromValueD (
	RTYPE_QRegisterCPD (self), &keyDsc, lookupCase, &locationsDscPtr, &locatedDscPtr
    );
    keyDsc.clear ();

    if (locatedDscPtr.isntEmpty ()) {
	IO_printf ("\nLocated: 0\n");
	locatedDscPtr.clear ();
    }

    if (locationsDscPtr.isEmpty ())
	IO_printf ("\n NOT Found\n");
    else {
	IO_printf ("\nFound at: %d\n", DSC_Pointer_Scalar_Int (locationsDscPtr));
	locationsDscPtr.clear ();
    }

    return self;
}

IOBJ_DefineMethod (LocateOrAddFromUVDM) {
    M_CPD *store = RTYPE_QRegisterCPD (parameterArray[1]);
    store->retain ();

    DSC_Descriptor keyDsc;
    keyDsc.constructMonotype (store, RTYPE_QRegisterCPD (parameterArray[0]));

    DSC_Pointer addedDscPtr, locationsDscPtr;
    rtVECTOR_LocateOrAdd (
	RTYPE_QRegisterCPD (self), &keyDsc, &locationsDscPtr, &addedDscPtr
    );
    keyDsc.clear ();

    if (addedDscPtr.isntEmpty ()) {
	IO_printf ("\nAdded LinkC:\n");
	DSC_Pointer_Link (addedDscPtr)->DebugPrint ();
	addedDscPtr.clear ();
    }

    M_CPD *location = DSC_Pointer_Link (locationsDscPtr).realizedContainer ();
    location->retain ();
    locationsDscPtr.clear ();
    return RTYPE_QRegister (location);
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtVECTOR_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"		, NewDM)
	IOBJ_MDE ("newSet"		, NewSetDM)
	IOBJ_MDE ("tracesOn"		, TracesOnDM)
	IOBJ_MDE ("tracesOff"		, TracesOffDM)
	IOBJ_MDE ("vcAllocatorTrace"	, VCAllocatorTraceDM)
	IOBJ_MDE ("usdcAllocatorTrace"	, USDCAllocatorTraceDM)
	IOBJ_MDE ("pmrdcAllocatorTrace"	, PMRDCAllocatorTraceDM)
	IOBJ_MDE ("reconstructorTrace"	, ReconstructorTraceDM)
	IOBJ_MDE ("pMapSearchesTrace"	, PMapSearchesTraceDM)
	IOBJ_MDE ("pMapBriefTrace"	, PMapBriefTraceDM)
	IOBJ_MDE ("displayParameters"	, DisplayParametersDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
	IOBJ_MDE ("resetCounts"		, ResetCountsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("size"		, ElementCountDM)
	IOBJ_MDE ("ptoken"		, PTokenDM)
	IOBJ_MDE ("pMapSize"		, PMapSizeDM)
	IOBJ_MDE ("uSegArraySize"	, USegArraySizeDM)
	IOBJ_MDE ("uSegFreeBound"	, USegFreeBoundDM)
	IOBJ_MDE ("uSegIndexSize"	, USegIndexSizeDM)
	IOBJ_MDE ("goToUSegment:"	, GoToUSegmentDM)
	IOBJ_MDE ("getUSegVStore"	, GetUSegVStoreDM)
	IOBJ_MDE ("getUSegUVector"	, GetUSegUVectorDM)
	IOBJ_MDE ("getUSegPToken"	, GetUSegPTokenDM)
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("at:put:"		, AtPutDM)
	IOBJ_MDE ("reOrderVC:"		, ReorderDM)
	IOBJ_MDE ("distributeVC:"	, DistributeVCDM)
	IOBJ_MDE ("stats:lc:"		, PartitionStatsDM)
	IOBJ_MDE ("subsetInStore:"	, SubsetInStoreDM)
	IOBJ_MDE ("simplifyToDescriptor", SimplifyToDescriptorDM)
	IOBJ_MDE ("lookupFromUV:store:type:"
					, LookupFromUVDM)
	IOBJ_MDE ("lookupFromScalar:type:"
					, LookupFromScalarDM)
	IOBJ_MDE ("locateOrAddFromUV:store:"
					, LocateOrAddFromUVDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtVECTOR_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtVECTOR_Handle::Maker;
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
        PrintVector (iArgList.arg<M_CPD*>());
	break;
    case RTYPE_RPrintObject:
        RPrintVector (iArgList.arg<M_CPD*>());
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
    static bool alreadyInitialized = false;
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtVECTOR);
    FAC_FDFCase_FacilityDescription ("Vector Representation Type Handler");
    case FAC_DoStartupInitialization:
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}
	char *			estring;
	unsigned int		evalue;

	if (IsntNil (estring = getenv ("VisionImplicitClean"))) {
	    evalue = (unsigned int)strtoul (estring, (char **)NULL, 0);
	    rtVECTOR_SetImplicitCleanupFlag (evalue ? true : false);
	}
	if (IsntNil (estring = getenv ("VisionPre7Ordering"))) {
	    evalue = (unsigned int)strtoul (estring, (char **)NULL, 0);
	    rtVECTOR_EnablePOPOrdering (evalue ? true : false);
	}
	if (IsntNil (estring = getenv ("VisionUSegCountLmt"))) {
	    evalue = (unsigned int)strtoul (estring, (char **)NULL, 0);
	    rtVECTOR_SetUSegmentCountThreshold (evalue);
	}
	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
	break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvector.c 1 replace /users/mjc/system
  860404 20:14:47 mjc Create Vector Representation Type

 ************************************************************************/
/************************************************************************
  RTvector.c 2 replace /users/mjc/system
  860422 22:56:46 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTvector.c 3 replace /users/mjc/system
  860504 18:45:57 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTvector.c 4 replace /users/mjc/system
  860515 13:29:24 mjc Intermediate release of value descriptors for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.c 5 replace /users/mjc/system
  860521 11:11:57 mjc Release for coordination with 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.c 6 replace /users/mjc/system
  860523 00:59:54 mjc Implemented vector align

 ************************************************************************/
/************************************************************************
  RTvector.c 7 replace /users/mjc/system
  860527 15:38:17 mjc Return vector to library for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.c 8 replace /users/jck/system
  860605 16:16:41 jck Implemented rtVECTOR_RefExtract

 ************************************************************************/
/************************************************************************
  RTvector.c 9 replace /users/mjc/system
  860606 13:25:58 mjc Added 'primitive function' USVT, eliminated 'RTypeVStorePArray...'

 ************************************************************************/
/************************************************************************
  RTvector.c 10 replace /users/jck/system
  860611 11:12:46 jck Replace bug fix

 ************************************************************************/
/************************************************************************
  RTvector.c 11 replace /users/jck/system
  860617 11:55:31 jck _LCExtract in place, but untested. Return for jad

 ************************************************************************/
/************************************************************************
  RTvector.c 12 replace /users/mjc/system
  860617 19:33:55 mjc Added 'save' routine

 ************************************************************************/
/************************************************************************
  RTvector.c 14 replace /users/mjc/system
  860623 10:24:18 mjc Converted to use new value descriptor macros, restructured assign operations

 ************************************************************************/
/************************************************************************
  RTvector.c 15 replace /users/mjc/system
  860623 10:55:47 mjc Enabled call to 'rtREFUV_LCAssignScalar'

 ************************************************************************/
/************************************************************************
  RTvector.c 16 replace /users/jad/system
  860627 19:33:43 jad release assign, still working on extract

 ************************************************************************/
/************************************************************************
  RTvector.c 17 replace /users/jad/system
  860702 13:00:32 jad fixed extract

 ************************************************************************/
/************************************************************************
  RTvector.c 18 replace /users/jad/system
  860702 15:21:54 jad fix nil case of RefExtract

 ************************************************************************/
/************************************************************************
  RTvector.c 19 replace /users/jad/system
  860702 18:29:23 jad fixed a bug in extract with nils

 ************************************************************************/
/************************************************************************
  RTvector.c 20 replace /users/mjc/system
  860702 22:33:18 mjc Removed spurious debugging print statements

 ************************************************************************/
/************************************************************************
  RTvector.c 21 replace /users/jad/system
  860703 22:16:53 jad implemented the ReOrder routine

 ************************************************************************/
/************************************************************************
  RTvector.c 22 replace /users/mjc/system
  860709 10:17:43 mjc Release new format value descriptors for continued development by 'jad'

 ************************************************************************/
/************************************************************************
  RTvector.c 23 replace /users/jad/system
  860710 20:51:29 jad value descriptor changes complete

 ************************************************************************/
/************************************************************************
  RTvector.c 24 replace /users/mjc/system
  860713 11:01:20 mjc Converted 'at:put:' to unpack 'put:' parameter with 'rtVALUEDSC_UnpackIObject'

 ************************************************************************/
/************************************************************************
  RTvector.c 25 replace /users/jad/system
  860717 17:48:31 jad implemented rtVECTOR_MakeConstructorLinks

 ************************************************************************/
/************************************************************************
  RTvector.c 26 replace /users/jad/system
  860718 20:15:21 jad added rtVECTOR_ReverseReOrderVC

 ************************************************************************/
/************************************************************************
  RTvector.c 27 replace /users/jck/system
  860721 15:40:43 jck Modified 'SaveContainer' to make use of M_SaveFlags

 ************************************************************************/
/************************************************************************
  RTvector.c 28 replace /users/jad/system
  860722 17:43:26 jad fixed multi segment assignments with repetitions

 ************************************************************************/
/************************************************************************
  RTvector.c 29 replace /users/jck/system
  860723 12:35:25 jck Added Utilities for DataBase Building

 ************************************************************************/
/************************************************************************
  RTvector.c 30 replace /users/jad/system
  860724 16:07:12 jad fixed rtVECTOR_DiscardConstructor to free the
vectorc's pos ptoken as well as the USDCr ptokens

 ************************************************************************/
/************************************************************************
  RTvector.c 31 replace /users/jck/system
  860725 17:26:56 jck Freed some CPD's

 ************************************************************************/
/************************************************************************
  RTvector.c 32 replace /users/mjc/system
  860728 13:54:31 mjc Release of changed preamble

 ************************************************************************/
/************************************************************************
  RTvector.c 33 replace /users/jad/system
  860731 17:45:20 jad added DBUPDATE utilties

 ************************************************************************/
/************************************************************************
  RTvector.c 34 replace /users/mjc/system
  860803 00:18:26 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  RTvector.c 35 replace /users/mjc/system
  860805 18:46:55 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  RTvector.c 36 replace /users/jad/system
  860807 20:09:28 jad freed some more enslaved cpd's

 ************************************************************************/
/************************************************************************
  RTvector.c 37 replace /users/jad/system
  860814 14:16:59 jad use new ptoken IsntCurrent routines in align

 ************************************************************************/
/************************************************************************
  RTvector.c 38 replace /users/jad/system
  860814 17:54:03 jad added a displayCounts debug method

 ************************************************************************/
/************************************************************************
  RTvector.c 39 replace /users/jck/system
  860820 17:58:41 jck Modified DataBase Dumpers

 ************************************************************************/
/************************************************************************
  RTvector.c 40 replace /users/jad/system
  860821 16:24:23 jad added a missing counter

 ************************************************************************/
/************************************************************************
  RTvector.c 41 replace /users/jad/system
  860828 14:59:12 jad align the undefined uvector segment as part of
vector align

 ************************************************************************/
/************************************************************************
  RTvector.c 42 replace /users/jad/system
  860914 11:14:51 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  RTvector.c 43 replace /users/mjc/system
  861002 18:02:30 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  RTvector.c 44 replace /users/jck/system
  861003 20:05:33 jad fixed vector routines to deal with
empty vectors and linkcs

 ************************************************************************/
/************************************************************************
  RTvector.c 45 replace /users/mjc/mysystem
  861005 16:26:41 mjc Fixed ANCIENT bug converting constructors with more than 4 U-Segments to vectors

 ************************************************************************/
/************************************************************************
  RTvector.c 46 replace /users/mjc/mysystem
  861005 20:18:16 mjc Corrected Ancient And Venerable Bug in 'ReOrderVC'

 ************************************************************************/
/************************************************************************
  RTvector.c 47 replace /users/mjc/system
  861015 23:53:17 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTvector.c 48 replace /users/mjc/system
  861021 14:39:47 mjc Added usage counters to form conversion and reordering routines

 ************************************************************************/
/************************************************************************
  RTvector.c 49 replace /users/jad/system
  861106 17:55:57 jad added some functions, fixed discard constructor, renamed reversereorder to distribute

 ************************************************************************/
/************************************************************************
  RTvector.c 50 replace /users/jad/system
  861112 10:50:54 jad add routines to convert a vector into a simple descriptor

 ************************************************************************/
/************************************************************************
  RTvector.c 51 replace /users/jad/system
  870120 14:59:24 jad modified DB update procedure:

 ************************************************************************/
/************************************************************************
  RTvector.c 52 replace /users/jad/system
  870317 15:16:17 jad implemented product primitive

 ************************************************************************/
/************************************************************************
  RTvector.c 53 replace /users/jck/system
  870415 09:53:53 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTvector.c 54 replace /users/jad/system
  870519 19:28:59 jad partial implementation of set vectors

 ************************************************************************/

/************************************************************************
  RTvector.c 55 replace /users/jad/system
  870523 12:40:46 jad Return vector for name shortening

 ************************************************************************/
/************************************************************************
  RTvector.c 56 replace /users/mjc/translation
  870524 09:36:21 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvector.c 57 replace /users/jck/system
  870605 16:09:02 jck Completed first phase in the implementation of set functionality

 ************************************************************************/
/************************************************************************
  RTvector.c 58 replace /users/mjc/system
  870614 19:59:42 mjc Fix VAX compilation errors

 ************************************************************************/
/************************************************************************
  RTvector.c 59 replace /users/jck/system
  870616 12:36:30 jck Disallowed saves of sets with transient usegment stores

 ************************************************************************/
/************************************************************************
  RTvector.c 60 replace /users/jck/system
  870626 11:17:19 jck Corrected improper handling of empty vector in rtVECTOR_LCExtract

 ************************************************************************/
/************************************************************************
  RTvector.c 61 replace /users/jck/system
  870730 09:32:30 jck Added some constructor usage counts and freed some enslaved constructors

 ************************************************************************/
/************************************************************************
  RTvector.c 62 replace /users/jck/system
  870824 15:02:56 jck Added a routine to do a comprehensive alignment of all usegments

 ************************************************************************/
/************************************************************************
  RTvector.c 63 replace /users/jck/system
  871007 13:33:28 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTvector.c 64 replace /users/jad/system
  871014 12:03:56 jad changes to improve performance:
1) made LCExtract and RefExtract do a binary search for the pmap entry
2) added a TracingPMapBrief debug trace
3) made M_ShiftContainerTail calls pass true for compression.

 ************************************************************************/
/************************************************************************
  RTvector.c 65 replace /users/jad/system
  871020 13:57:57 jad added 'TracingUnusesUSegs' trace

 ************************************************************************/
/************************************************************************
  RTvector.c 66 replace /users/jad/system
  871023 11:45:03 jad special cased the simple case of LCExtract

 ************************************************************************/
/************************************************************************
  RTvector.c 67 replace /users/jad/system/real
  871103 14:58:48 jad fixed bug caused by
the simple case optimization of rtVECTOR_LCExtract when selecting a
 'referenceNil' value.

 ************************************************************************/
/************************************************************************
  RTvector.c 68 replace /users/jck/system
  871110 09:37:36 jck Added a minor improvement to the pmap binary search algorithm in the extract routines

 ************************************************************************/
/************************************************************************
  RTvector.c 69 replace /users/mjc/Software/system
  871228 23:42:52 mjc Re-implement 'rtVECTOR_PartitndStats'

 ************************************************************************/
/************************************************************************
  RTvector.c 70 replace /users/jck/system
  880229 08:14:33 jck Fixed bug in LocateOrAddFrom...

 ************************************************************************/
/************************************************************************
  RTvector.c 71 replace /users/jad
  880322 14:07:05 jad fix bug in rtVECTOR_Align with set vectors

 ************************************************************************/
/************************************************************************
  RTvector.c 72 replace /users/jad/system
  880413 10:06:39 jad Release the SequenceLinks Project

 ************************************************************************/
/************************************************************************
  RTvector.c 73 replace /users/jad/system
  880711 16:40:44 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTvector.c 74 replace /users/jck/system
  890223 08:39:44 jck Fortified against saving corrupted vectors. Reimplemented Vector reconstruction

 ************************************************************************/
/************************************************************************
  RTvector.c 75 replace /users/jck/system
  890413 11:14:06 jck Effected removal of empty usegments from vectors which are not 'fixed properties'

 ************************************************************************/
/************************************************************************
  RTvector.c 76 replace /users/jck/system
  890421 07:56:11 jck Disabled USegment deletion (temporarily)

 ************************************************************************/
/************************************************************************
  RTvector.c 77 replace /users/jck/system
  890508 12:04:54 jck Corrected an editting error in '...LCExtract'

 ************************************************************************/
/************************************************************************
  RTvector.c 78 replace /users/m2/backend
  890927 15:19:08 m2 Changed major and minor variables because of SUN macro conflict, dummyiobj added

 ************************************************************************/
