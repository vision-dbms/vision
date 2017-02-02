/*****  Reference U-Vector Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtREFUV

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

#include "VkRadixListSorter.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "vdsc.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"

#include "VCPDReference.h"

#include "VfDistributionGenerator.h"
#include "VfLocateOrAddGenerator.h"
#include "VfLocateGenerator.h"

/*****  Self  *****/
#include "RTrefuv.h"


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

DEFINE_CONCRETE_RTT (rtREFUV_Handle);


/***********************************
 *****  Debugging Trace Flag   *****
 ***********************************/

PrivateVarDef bool fTracingRefAlign = false;


/****************************
 *****  Usage Counters  *****
 ****************************/

PrivateVarDef unsigned int
    NewCount			= 0,

    AlignReferenceCount		= 0,
    AlignCount			= 0,
    RefAlignCount               = 0,
    RefAlignEndOnlyCount        = 0,
    RefAlignRadixSortCount      = 0,
    RefAlignCodomainArrayCount  = 0,

    LCExtractCount		= 0,
    UVExtractCount		= 0,
    RFExtractCount		= 0,

    LCAssignCount		= 0,
    UVAssignCount		= 0,
    RFAssignCount		= 0,
    LCAssignScalarCount		= 0,
    UVAssignScalarCount		= 0,

    AscendingSortIndicesCount	= 0,
    FlipCount			= 0,
    LinearizeRrRcCount		= 0,

    LocateOrAddCount		= 0,
    ScalarLocateOrAddCount	= 0,
    LookupCount			= 0,
    ScalarLookupCount		= 0;


/************************
 *****  Parameters  *****
 ************************/

/*---------------------------------------------------------------------------
 *  CodomainDomainCriticalRatio   - a threshold ratio used to determine which
 *                                refUV alignment implementation to be used.
 *---------------------------------------------------------------------------
 */

PrivateVarDef double 
    CodomainDomainCriticalRatio   = 2.0;


PublicFnDef void rtREFUV_SetCodomainDomainRatio (double ratio) {
    if (ratio >= 0)
        CodomainDomainCriticalRatio = ratio;
}


/*********************************
 *****  Basic Instantiator   *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Basic RefUV instantiation routine.
 *
 *  Arguments:
 *	pPPT			- a CPD for the PPT of the result.
 *	rTokenRefCPD/Index	- a reference to a POP for a P-Token associated
 *				  with the positional state referenced by the
 *				  elements of this U-Vector.
 *	initFn			- the address of an initialization function for
 *				  the new U-Vector.  This function will be
 *				  passed an initialized standard CPD for the
 *				  new U-Vector, the number of elements in the
 *				  U-Vector, and a <varargs.h> argument pointer
 *				  positioned at 'initFnArg1'.  If this function
 *				  is not specified (i.e., 'IsNil (initFn)'),
 *				  the U-Vector array will be initialized to
 *				  zero.
 *	initFnArg1, ...		- a collection of optional arguments to the
 *				  'initFn'.
 *
 *  Returns:
 *	A standard CPD for the RefUV created.
 *
 *****/
PublicFnDef M_CPD * __cdecl rtREFUV_New (
    M_CPD*			pPPT,
    M_CPD*			rTokenRefCPD,
    int				rTokenRefIndex,
    Ref_UV_Initializer		initFn,
    ...
) {
    NewCount++;

/*****  Acquire the arguments passed to this function  *****/
    V_VARGLIST (initFnAP, initFn);

/*****  Create and initialize the RefUV  *****/
    return UV_New (
	RTYPE_C_RefUV,
	pPPT,
	rTokenRefCPD,
	rTokenRefIndex,
	sizeof (rtREFUV_ElementType),
	initFn,
	initFnAP
    );
}


PublicFnDef M_CPD * __cdecl rtREFUV_New (
    M_CPD *pPPT, M_CPD *pRPT, Ref_UV_Initializer initFn, ...
) {
    NewCount++;

/*****  Acquire the arguments passed to this function  *****/
    V_VARGLIST (initFnAP, initFn);

/*****  Create and initialize the RefUV  *****/
    return UV_New (
	RTYPE_C_RefUV, pPPT, pRPT, sizeof (rtREFUV_ElementType), initFn, initFnAP
    );
}


/***********************************
 *****  Standard Instantiator  *****
 ***********************************/

/*************************************
 *  Internal Initialization Routine  *
 *************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine invoked by 'rtREFUV_New' to initialize the
 *****  elements of a RefUV.
 *
 *  Arguments:
 *	cpd			- an initialized standard CPD for the RefUV
 *				  being initialized.  The element pointer of
 *				  the CPD will be positioned at the first
 *				  element to be initialized.  This will be
 *				  the beginning of the element array for new
 *				  u-vectors.
 *	nelements		- the number of elements in the RefUV to
 *				  initialize.  For new RefUV's, this will
 *				  be the number of elements in the U-Vector.
 *	ap			- a <varargs.h> argument pointer pointing to
 *				  a fill value appropriate for this U-Vector.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeRefUV (M_CPD *cpd, size_t nelements, va_list ap) {
    V::VArgList iArgList (ap);
    rtREFUV_ElementType
	referenceNil = iArgList.arg<rtREFUV_ElementType>(),
	*p = rtREFUV_CPD_Element (cpd),
	*pl = p + nelements;

    while (p < pl)
	*p++ = referenceNil;
}


/*********************************************
 *  Standard External Instantiation Routine  *
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Standard RefUV instantiation routine.
 *
 *  Arguments:
 *	pTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the initial positional state of
 *				  this U-Vector.
 *	rTokenRefCPD/Index	- a reference to a POP for a P-Token associated
 *				  with the positional state referenced by the
 *				  elements of this U-Vector.
 *
 *  Returns:
 *	A standard CPD for the RefUV created.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_New (
    M_CPD*			pTokenRefCPD,
    int				pTokenRefIndex,
    M_CPD*			rTokenRefCPD,
    int				rTokenRefIndex
)
{
    VCPDReference pPPT (pTokenRefCPD, pTokenRefIndex, RTYPE_C_PToken);
    return rtREFUV_New (
	pPPT,
	rTokenRefCPD,
	rTokenRefIndex,
	InitializeRefUV,
	rtPTOKEN_BaseElementCount (rTokenRefCPD, rTokenRefIndex)
    );
}

PublicFnDef M_CPD *rtREFUV_New (M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef) {
    return rtREFUV_New (
	pPPT,
	pRPTRef,
	xRPTRef,
	InitializeRefUV,
	rtPTOKEN_BaseElementCount (pRPTRef, xRPTRef)
    );
}


/*---------------------------------------------------------------------------
 *****  Standard RefUV instantiation routine.
 *
 *  Arguments:
 *	posPToken		- the positional P-Token of the new uvector.
 *	refPToken		- the referential P-Token of the new uvector.
 *
 *  Returns:
 *	A standard CPD for the RefUV created.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_New (M_CPD *pPPT, M_CPD *pRPT) {
    return rtREFUV_New (
	pPPT, pRPT, InitializeRefUV, rtPTOKEN_CPD_BaseElementCount (pRPT)
    );
}


/************************************************
 *****  Standard Set U-Vector Instantiator  *****
 ************************************************/

/************************************************
 *  Partition Boundary Initialized New Routine  *
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a new reference u-vector initialized with the
 *****  partition boundaries defined by a link constructor.
 *
 *  Arguments:
 *	partition		- a partition defining link constructor.  The
 *				  POSITIONAL and REFERENTIAL states of the
 *				  u-vector created by this routine will be the
 *				  REFERENTIAL and POSITIONAL states of this
 *				  constructor, respectively.  Each position of
 *				  the u-vector will contain the origin of the
 *				  corresponding partition in the constructor.
 *
 *  Returns:
 *	A standard CPD for the reference u-vector created.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_NewInitWithPrtitnBounds (rtLINK_CType *partition) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define fillToWith(to, with)\
    for (resultTarget = resultArray + to;\
	 resultPtr < resultTarget;\
	 *resultPtr++ = with)

#define handleRange(position, count, reference)\
    fillToWith (reference, position);\
    while (count-- > 0)\
	*resultPtr++ = position++

#define handleRepetition(position, count, reference)\
    fillToWith (reference, position);\
    *resultPtr++ = position


/*---------------------------------------------------------------------------
 *****  Local Variable Definitions
 *---------------------------------------------------------------------------*/
    rtREFUV_ElementType *resultArray, *resultPtr, *resultTarget;

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align the partition...  *****/
    partition->Align ();

/*****  Create the result u-vector...  *****/
    M_CPD *result = rtREFUV_New (
	partition->RPT (), partition->PPT (), NilOf (Ref_UV_Initializer)
    );

/*****  Initialize the traversal pointers...  *****/
    resultArray = resultPtr = rtREFUV_CPD_Array (result);

/*****  Traverse the partitioning link...  *****/
    rtLINK_TraverseRRDCList (
	partition, handleRepetition, handleRepetition, handleRange
    );

/*****  Return the result...  *****/
    return result;

/*---------------------------------------------------------------------------
 *****  Link Traversal Component Handler Macro Deletions
 *---------------------------------------------------------------------------*/
#undef fillToWith
#undef handleRange
#undef handleRepetition
}


/*********************************************
 *****  Element Bound Complaint Routine  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Quasi-Public routine to complain about the value of an element.
 *
 *  Arguments:
 *	element			- the objectionable element.
 *	upperBound		- the largest possible value allowed for this
 *				  reference U-Vector.
 *
 *  Returns:
 *	This routine never returns.
 *
 *****/
PublicFnDef void rtREFUV_ComplainAboutElementVal (
    int				element,
    int				upperBound
)
{
    ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "rtREFUV: Element %d Out Of Bounds 0:%d", element, upperBound
	)
    );
}


/******************************************
 *****  Reference Alignment Routines  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a reference to the current version of the positional
 *****  state it references.
 *
 *  Argument:
 *	refp			- the address of the reference to be aligned.
 *
 *  Returns:
 *	'refp'
 *
 *****/
PublicFnDef rtREFUV_TypePTR_Reference rtREFUV_AlignReference (
    rtREFUV_TypePTR_Reference	refp
)
{
    int value, refNil, theEnd, x, begindel, enddel, adjustment;

#define outputValue\
    rtREFUV_Ref_Element (refp) = rtREFUV_Ref_Element (refp) + adjustment

#define outputRefNil\
    rtREFUV_Ref_Element (refp) = refNil

#define handleInsertion(ptOrigin,ptShift) {\
    if (!theEnd) {\
	x = ptOrigin - adjustment;\
	if (x <= value)\
	    adjustment += ptShift;\
	else {\
	    outputValue;\
	    theEnd = true;\
	}\
    }\
}

#define handleDeletion(ptOrigin,ptShift) {\
    if (!theEnd) {\
	begindel = ptOrigin + ptShift - adjustment;\
	enddel = ptOrigin - 1 - adjustment;\
	if (begindel <= value && value <= enddel) {\
	    outputRefNil;\
	    theEnd = true;\
	}\
	else if (begindel < value)\
	    adjustment += ptShift;\
	else {\
	    outputValue;\
	    theEnd = true;\
	}\
    }\
}

    AlignReferenceCount++;

/*****  Return if no alignment is needed...  *****/
    if (rtREFUV_Ref_RefPTokenCPD (refp)->ReferenceIsNil (rtPTOKEN_CPx_NextGeneration))
	return refp;

/***** Otherwise - referentially align it... *****/

/***** Setup for the traversal *****/
    rtPTOKEN_CType *ptokenc = rtPTOKEN_CPDCumAdjustments (rtREFUV_Ref_RefPTokenCPD (refp));
    refNil = rtPTOKEN_PTC_BaseCount (ptokenc);

    theEnd = false;
    adjustment = 0;
    value = rtREFUV_Ref_Element (refp);

/***** Traverse the referential ptoken *****/
    rtPTOKEN_FTraverseAdjustments (ptokenc, handleInsertion, handleDeletion);
    if (!theEnd)  /** push out the last value **/
	outputValue;

/***** Set the refuv's referential ptoken to the new updated one *****/
    rtREFUV_Ref_RefPTokenCPD (refp)->release ();

    rtREFUV_Ref_RefPTokenCPD (refp) =
	M_DuplicateCPDPtr (ptokenc->NextGeneration ());

    ptokenc->discard ();


/*****  Return the Aligned Reference UVector. *****/
    return refp;

/***** Cleanup the traversal macros *****/
#undef outputValue
#undef outputRefNil
#undef handleInsertion
#undef handleDeletion
}


/*---------------------------------------------------------------------------
 *****  Routine to align a reference and validate that its P-Token matches
 *****  the current positional state of another object.
 *
 *  Arguments:
 *	refp			- the address of a reference.
 *	pTokenRefCPD/Index	- a CPD/Index pair referencing the P-Token
 *				  for the positional state of the source or
 *				  target.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void rtREFUV_AlignAndValidateRef (
    rtREFUV_TypePTR_Reference	refp, M_CPD *pTokenRefCPD, int pTokenRefIndex
)
{
/*****  Align the reference...  *****/
    rtREFUV_AlignReference (refp);

/*****  ...and check that it is referentially related to the source.  *****/
    if (pTokenRefCPD->ReferenceDoesntName (
	    pTokenRefIndex, rtREFUV_Ref_RefPTokenCPD (refp)
	)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_AlignAndValidateReference: Referential Inconsistency"
    );
}


/*******************************************
 *****  Positional Alignment Routines  *****
 *******************************************/

/*************************************
 *  Internal Alignment Fill Routine  *
 *************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to properly initialize inserted gaps during the
 *****  alignment process.
 *
 *  Arguments:
 *	cpd/index		- a CPD for the reference U-Vector and the
 *				  index of the pointer within that CPD
 *				  (== UV_CPx_Element) which points
 *				  to the region affected by the normalization
 *				  operation.
 *	shiftAmount		- the number of bytes in the affected region.
 *				  Values < 0 imply deletions, > 0 insertions.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  the 'shift' and 'regionProcessorArg' defined by UV_Align.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void FillPAlignmentGap (
    M_CPD *cpd, unsigned int Unused(index), ptrdiff_t Unused(shiftAmount), va_list ap
) {
/*****  Do nothing if this is a deletion...  *****/
    rtREFUV_ElementType	const shift = va_arg(ap, int);
    if (shift < 0)
	return;

/*****  Otherwise, fill the new region...  *****/
    rtREFUV_ElementType const referenceNil = va_arg(ap, int);
    rtREFUV_ElementType *p, *pl;
    for (pl = shift + (p = rtREFUV_CPD_Element (cpd));
         p < pl;
	 *p++ = referenceNil);
}


/********************************
 *  External Alignment Routine  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally AND referentially align a RefUV.
 *
 *  Argument:
 *	cpd			- a CPD for the RefUV to be aligned.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD *rtREFUV_Align (M_CPD *cpd, int referentialAlignmentRequired) {

#define locate(x) {\
    int locateX = (x);\
    while (notEnd && locateX > currentValue) {\
	currentValue += adjustment;\
	goToNext;\
    }\
}

#define handleInsertion(ptOrigin,ptShift) {\
    locate (ptOrigin - adjustment);\
    adjustment += ptShift;\
}

#define handleDeletion(ptOrigin,ptShift) {\
    int enddel = ptOrigin - 1 - adjustment;\
    locate (ptOrigin + ptShift - adjustment);\
    while (notEnd && currentValue <= enddel) {\
	currentValue = refNil;\
	UV_CPD_IsASetUV (cpd) = false;\
	goToNext;\
    }\
    adjustment += ptShift;\
}


/*****  Validate Argument R-Type  *****/
    AlignCount++;
    RTYPE_MustBeA ("rtREFUV_Align", M_CPD_RType (cpd), RTYPE_C_RefUV);

/*------------------------------*/
/*****  Align Positionally  *****/
/*------------------------------*/
    UV_Align (
	cpd,
	FillPAlignmentGap,
	rtPTOKEN_BaseElementCount (cpd, UV_CPx_RefPToken)
    );

/*-------------------------------*/
/*****  Align Referentially  *****/
/*-------------------------------*/

/***** if this is an unmodified uvector, and we aren't explictly asked ...  *****/
/***** ... let's not do it!!!  *****/
    if (cpd->IsReadOnly () && !referentialAlignmentRequired)
	return cpd;

/***** if the referencial ptoken is current, all done... *****/
    M_CPD *pTokenCPD;
    rtPTOKEN_IsntCurrent (cpd, UV_CPx_RefPToken, pTokenCPD);
    if (IsNil (pTokenCPD))
	return cpd;

    RefAlignCount++;
/***** if all alterations are beyond the uvector end, fix the ptoken and return... *****/
    unsigned int size = UV_CPD_ElementCount (cpd);
    if (0 == size || rtPTOKEN_AllAlterationsBeyondEnd (pTokenCPD, -1)) {
        RefAlignEndOnlyCount++;
	M_CPD *newPToken = rtPTOKEN_BasePToken (cpd, UV_CPx_RefPToken);
	/* Set the refuv's referential ptoken to the terminal p-token */
	cpd->StoreReference (UV_CPx_RefPToken, newPToken);
	/*  Update any reference nils that exist in the uvector */

	int oldRefNil = rtPTOKEN_CPD_BaseElementCount (pTokenCPD);
	int newRefNil = rtPTOKEN_CPD_BaseElementCount (newPToken);
	int *arrayp = rtREFUV_CPD_Array (cpd);
	unsigned int i = 0;
	while (i < size) {
	    if (*arrayp >= oldRefNil)
		*arrayp = newRefNil;
	     i++;
	     arrayp++;
	}
	newPToken->release ();
	pTokenCPD->release ();

	return cpd;
    }

/***** We really need to do a referential alignment ... *****/
/***** Setup for the traversal *****/
    int sCodomain = rtPTOKEN_CPD_BaseElementCount(pTokenCPD);
    double dCodomainDomainRatio = (double) sCodomain/size;

    rtPTOKEN_CType *ptokenc = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
    pTokenCPD->release ();

    int refNil = rtPTOKEN_PTC_BaseCount (ptokenc);

    bool notEnd = true;
    int adjustment = 0;

    if (fTracingRefAlign) IO_printf ("rtREFUV_RefAlign:[%d:%d] CDRatio=%g(%d/%d) CDCriticalRatio= %g\n",
        cpd->SpaceIndex (), cpd->ContainerIndex (), dCodomainDomainRatio, sCodomain, size, CodomainDomainCriticalRatio);

    if (dCodomainDomainRatio > CodomainDomainCriticalRatio) {
	#define currentValue *arrayp 
	#define goToNext {\
	    if (notEnd = (++index < indexEnd))\
    		arrayp = rtREFUV_CPD_Array (cpd) + *index;\
	} 

        RefAlignRadixSortCount++;
	if (fTracingRefAlign) IO_printf ("*** Using the old referential alignment implementation (radix sort) ***\n");

	unsigned int* const indexStart = (unsigned int *)UTIL_Malloc (size * sizeof (unsigned int));
	unsigned int* const indexEnd = indexStart + size;
	unsigned int* index = indexStart;

        UTIL_RadixListSort ((unsigned int const *)rtREFUV_CPD_Array (cpd), index, size);

	/*****  Enable modifications of the U-Vector  *****/
	cpd->EnableModifications ();

        int *arrayp = rtREFUV_CPD_Array (cpd) + *index;

	/***** Traverse the referential ptoken *****/
	rtPTOKEN_FTraverseAdjustments (ptokenc, handleInsertion, handleDeletion);

	/*** process the remaining values ***/

	locate (refNil - adjustment + 1);

	UTIL_Free (indexStart);

	#undef currentValue
	#undef goToNext
    } else {
	#define currentValue *pCodomainArray 
	#define goToNext { notEnd = (++pCodomainArray < pCodomainArrayEnd); }

        RefAlignCodomainArrayCount++;
	if (fTracingRefAlign) IO_printf ("*** Using the new referential alignment implementation (codomain array) ***\n");

	int* const pCodomainArrayStart = (int *) UTIL_Malloc ((sCodomain+1) * sizeof (int));
	int* const pCodomainArrayEnd = pCodomainArrayStart + sCodomain +1;
	int* pCodomainArray  = pCodomainArrayStart;

	/*** Fill in the codomain array value including the reference Nil ***/
	int fillValue = 0;
	while (fillValue <= sCodomain) 	{
	    *pCodomainArray = fillValue;
	    pCodomainArray++;
	    fillValue++;
	}

	/*****  Enable modifications of the U-Vector  *****/
	cpd->EnableModifications ();

	pCodomainArray = pCodomainArrayStart;

	/***** Traverse the referential ptoken *****/
	rtPTOKEN_FTraverseAdjustments (ptokenc, handleInsertion, handleDeletion);

	/*** process the remaining values ***/

	locate (refNil - adjustment + 1);

	/*** Change the U-Vector according to the new values of codomain array ***/
	int *arrayp = rtREFUV_CPD_Array (cpd);
	unsigned int tmpSize = size;
	while (tmpSize--) {
	    *arrayp = *(pCodomainArrayStart+*arrayp);
	    arrayp++;
	}

	UTIL_Free (pCodomainArrayStart);

	#undef currentValue
	#undef goToNext
    }

    /***** Set the refuv's referential ptoken to the new updated one *****/
    cpd->StoreReference (UV_CPx_RefPToken, ptokenc->NextGeneration ());

    ptokenc->discard ();

/*****  Return the Aligned Reference UVector. *****/
    return cpd;

/***** Cleanup the traversal macros *****/

#undef locate
#undef handleInsertion
#undef handleDeletion
}


/*****************************************************************
 *****  Positional Alignment And Access Validation Routines  *****
 *****************************************************************/

/************************
 *  Extract Validation  *
 ************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a reference u-vector and validate it for use as an
 *****  extraction index.
 *
 *  Arguments:
 *	sPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object from which elements are to be
 *				  extracted.
 *	refuvCPD		- a standard CPD for the reference u-vector
 *				  supplying subscripts to the extraction
 *				  operation.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the reference u-vector and source are
 *	not referentially related.
 *
 *****/
PublicFnDef void rtREFUV_AlignForExtract (
    M_CPD *sPTokenRefCPD, int sPTokenRefIndex, M_CPD *refuvCPD
) {
/*****  Align the reference u-vector...  *****/
    rtREFUV_Align (refuvCPD);

/*****  ... check that it is referentially related to the source.  *****/
    if (sPTokenRefCPD->ReferenceDoesntName (sPTokenRefIndex, refuvCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency, "rtREFUV_AlignForExtract: Referential Inconsistency"
    );
}


/***********************
 *  Assign Validation  *
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a reference u-vector and validate it for use as an
 *****  assignment index.
 *
 *  Arguments:
 *	tPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object into which elements are to be
 *				  assigned.
 *	refuvCPD		- a standard CPD for the reference u-vector
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
 *	'EC__InternalInconsistency' if the reference u-vector and source are
 *	not positionally equivalent or if the reference u-vector and target
 *	are not referentially equivalent.
 *
 *****/
PublicFnDef void rtREFUV_AlignForAssign (
    M_CPD*			tPTokenRefCPD,
    int				tPTokenRefIndex,
    M_CPD*			refuvCPD,
    M_CPD*			sPTokenRefCPD,
    int				sPTokenRefIndex
)
{
/*****  Align the reference u-vector...  *****/
    rtREFUV_Align (refuvCPD);

/*****  ... check that it is referentially related to the target...  *****/
    if (tPTokenRefCPD->ReferenceDoesntName (tPTokenRefIndex, refuvCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_AlignForAssign: Referential Inconsistency"
    );

/*****  ... and that it is positionally related to the source.  *****/
    if (sPTokenRefCPD->ReferenceDoesntName (sPTokenRefIndex, refuvCPD, UV_CPx_PToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_AlignForAssign: Referential Inconsistency"
    );
}


/******************************
 *  Scalar Assign Validation  *
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a reference u-vector and validate it for use as an
 *****  scalar assignment index.
 *
 *  Arguments:
 *	tPTokenRefCPD/Index	- a reference to a POP for the P-Token
 *				  describing the positional state of the
 *				  object into which elements are to be
 *				  assigned.
 *	refuvCPD		- a standard CPD for the reference u-vector
 *				  supplying subscripts to the assignment
 *				  operation.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if the reference u-vector and target are
 *	not referentially equivalent.
 *
 *****/
PublicFnDef void rtREFUV_AlignForScalarAssign (
    M_CPD *tPTokenRefCPD, int tPTokenRefIndex, M_CPD *refuvCPD
)
{
/*****  Align the reference u-vector...  *****/
    rtREFUV_Align (refuvCPD);

/*****  ... check that it is referentially related to the target...  *****/
    if (tPTokenRefCPD->ReferenceDoesntName (tPTokenRefIndex, refuvCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_AlignForAssign: Referential Inconsistency"
    );
}


/****************************
 *****  Set Conversion  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a uvector into a set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the uvector to be used as the
 *		          source for the set uvector.
 *	resultCPD 	- a pointer to a standard CPD for the set uvector to
 *                        be created.
 *	refuvCPD	- a pointer to a standard CPD for a reference uvector
 *                        to be created.
 *			  For each element in the original uvector, this
 *                        reference uvector will contain its position in the
 *                        new set uvector.
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtREFUV_ToSetUV (
    M_ASD *pContainerSpace, M_CPD *sourceCPD, M_CPD **resultCPD, M_CPD **refuvCPD
) {
/*****  Align the source uvector ... *****/
    rtREFUV_Align (sourceCPD);

/*****  ... and access the relevant source data:  *****/
    VCPDReference pSourcePPT (sourceCPD, UV_CPx_PToken, RTYPE_C_PToken);
    unsigned int sSource = UV_CPD_ElementCount (sourceCPD);

/*****  If the source is empty, ... *****/
    if (0 == sSource) {
    /*****  ... the result is trivial:  *****/
	M_CPD *pResultPPT = rtPTOKEN_New (pContainerSpace, 0);
	*refuvCPD = rtREFUV_New (pSourcePPT, pResultPPT);
	*resultCPD = rtREFUV_New (pResultPPT, sourceCPD, UV_CPx_RefPToken);
	pResultPPT->release ();
    }
/*****  ... otherwise, ...  *****/
    else {
    /*****  ... sort the source, ... *****/
	rtREFUV_ElementType const *const pSourceArray = rtREFUV_CPD_Array (sourceCPD);
        unsigned int *const pSortArray = (unsigned int *)UTIL_Malloc (
	    sSource * sizeof (unsigned int)
	);
	unsigned int const *const pSortLimit = pSortArray + sSource;

	UTIL_RadixListSort ((unsigned int const *)pSourceArray, pSortArray, sSource);

    /*****  ... create the source->set reference map (temporarily using the wrong codomain), ... *****/
	*refuvCPD = rtREFUV_New (pSourcePPT, pSourcePPT);
	rtREFUV_ElementType *const pRefuvArray = rtREFUV_CPD_Array (*refuvCPD);

	unsigned int const *pSortIndex = pSortArray;
	unsigned int xReferenceElement = *pSortIndex++;

	unsigned int xResultElement = 0;
	pRefuvArray[xReferenceElement] = (rtREFUV_ElementType)xResultElement;

	while (pSortIndex < pSortLimit) {
	    unsigned int xSourceElement = *pSortIndex++;
	    if (pSourceArray[xSourceElement] == pSourceArray[xReferenceElement])
		pRefuvArray[xSourceElement] = (rtREFUV_ElementType)xResultElement;
	    else {
		pRefuvArray[xSourceElement] = (rtREFUV_ElementType)++xResultElement;
		xReferenceElement = xSourceElement;
	    }
	}

    /*****  ... generate the set's domain,  ...  *****/
	M_CPD *pResultPPT = rtPTOKEN_New (pContainerSpace, xResultElement + 1);

    /*****  ... fix the source->set reference map's codomain, ...  *****/
	(*refuvCPD)->StoreReference (UV_CPx_RefPToken, pResultPPT);

    /*****  ... create the result set, ...  *****/
	*resultCPD = rtREFUV_New (pResultPPT, sourceCPD, UV_CPx_RefPToken);
	rtREFUV_ElementType *const pResultArray = rtREFUV_CPD_Array (*resultCPD);

    /*****  ... initialize its values, ...  *****/
	pSortIndex = pSortArray;
	xReferenceElement = *pSortIndex++;

	xResultElement = 0;
	pResultArray[xResultElement] = pSourceArray[xReferenceElement];

	while (pSortIndex < pSortLimit) {
	    unsigned int xSourceElement = *pSortIndex++;
	    if (pRefuvArray[xSourceElement] != pRefuvArray[xReferenceElement]) {
		pResultArray[++xResultElement] = pSourceArray[xSourceElement];
		xReferenceElement = xSourceElement;
	    }
	}

    /*****  ... and clean up:  *****/
	pResultPPT->release ();

	UTIL_Free (pSortArray);
    }

/*****  Finally set the IsASetUV bit:  *****/
    UV_CPD_IsASetUV (*resultCPD) = true;
}


/*---------------------------------------------------------------------------
 *****  Routine to determine if a RefUV is a set.
 *
 *  Arguments:
 *	refuvCPD	- a standard CPD for the uvector to check
 *	refNilsLC 	- a pointer to a link constructor this routine will
 *                        create to point out reference Nils which violate
 *                        the definition of a reference Set. Will be set to
 *                        Nil if the RefUV is a set, or if it cannot be fixed
 *                        by simply pointing out referenceNils.
 *
 *  Returns:
 *	True if a set, False if not.
 *
 *  Note: The Set Attribute of the RefUV is maintained by the RefUV's user
 *        by only modifying the RefUV by use of the LocateOrAdd function.
 *        The user is charged with the responsibility to avoid use of
 *        'Set violating' operations such as Assignment. However,
 *        the best efforts of the user to maintain the Set attribute can
 *        be subverted if an object being referenced is deleted. This
 *        routine is provided to discover that occurence and to provide
 *        a means (via refNilsLC) to recover from it.
 *****/
PublicFnDef int rtREFUV_IsASet (M_CPD *refuvCPD, rtLINK_CType **refNilsLC) {
    int uvSize, refNil, *refPtr, i, lastRef;

    *refNilsLC = NilOf (rtLINK_CType *);

    if (UV_CPD_IsASetUV (rtREFUV_Align (refuvCPD)))
	return true;

/***** Make the location link constructor ... *****/
    *refNilsLC = rtLINK_RefConstructor (refuvCPD, UV_CPx_PToken);
    
    refPtr = rtREFUV_CPD_Array (refuvCPD);
    uvSize = UV_CPD_ElementCount (refuvCPD);
    refNil = rtPTOKEN_BaseElementCount (refuvCPD, UV_CPx_RefPToken);

    for (i = 0, lastRef = -1;
	 i < uvSize;
	 i++, refPtr++)
    {
	if (refNil == *refPtr) /* record the positions of refNils */ {
	    if (i != uvSize -1) /* A refNil in the last position is allowed */
		rtLINK_AppendRange (*refNilsLC, i, 1);
	}
	else if (lastRef >= *refPtr) {
	    /*** There is more wrong than a referential alignment could
             *** have caused. If the caller wants to turn this refuv into
             *** a set, it should call rtREFUV_ToSetUV () instead.
	     ***/
	    (*refNilsLC)->release ();
	    *refNilsLC = NilOf (rtLINK_CType *);
	    return false;
	}
	else
	    lastRef = *refPtr;
    }
    if (0 == (*refNilsLC)->ElementCount ()) {
	/*** This RefUV meets the criteria for the set attribute ***/
	(*refNilsLC)->release ();
	*refNilsLC = NilOf (rtLINK_CType *);
	
	refuvCPD->EnableModifications ();
	UV_CPD_IsASetUV (refuvCPD) = true;
	
	return true;
    }
    M_CPD *ptoken = rtPTOKEN_New (refuvCPD->ScratchPad (), (*refNilsLC)->ElementCount ());
    (*refNilsLC)->Close (ptoken);
    ptoken->release ();
    return false;

}

/*---------------------------------------------------------------------------
 *****  Routine to determine if a RefUV is a set, and restore the set
 *      attribute if it is.
 *
 *  Arguments:
 *	refuvCPD	- a standard CPD for the uvector to check
 *
 *  Returns:
 *	True if a set, False if not.
 *
 *****/
PublicFnDef int rtREFUV_RestoreSetAttribute (M_CPD *refuvCPD) {
    if (UV_CPD_IsASetUV (rtREFUV_Align (refuvCPD)))
	return true;
    
    int *refPtr = rtREFUV_CPD_Array (refuvCPD);
    int uvSize = UV_CPD_ElementCount (refuvCPD);

    int lastRef = -1;
    for (int i = 0; i < uvSize; i++, refPtr++) {
	if (lastRef >= *refPtr)
	    return false;
	lastRef = *refPtr;
    }

    refuvCPD->EnableModifications ();
    UV_CPD_IsASetUV (refuvCPD) = true;
    return true;

}


/**************************
 *****  Distribution  *****
 **************************/

/*----------------------------------------------------------------------------
 ***** Routine to create a new u-vector by distribution.
 *
 *  Arguments:
 *	refuvCPD	- a standard CPD for the reference U-Vector to be used
 *                        to choose the location in the new U-Vector to place 
 *			  the source values.
 *	sourceuvCPD	- a standard CPD for the U-Vector which is supplying
 *			  the source values.
 *
 *  Returns:
 *	The New U-Vector.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_Distribute (M_CPD *refuvCPD, M_CPD *sourceuvCPD) {
    VCPDReference pPPT (refuvCPD, UV_CPx_RefPToken, RTYPE_C_PToken);
    return rtREFUV_UVAssign (
	rtREFUV_New (pPPT, sourceuvCPD, UV_CPx_RefPToken), refuvCPD, sourceuvCPD
    );
}


/**********************************************
 *****  Standard Element Access Routines  *****
 **********************************************/

/*****************************************************
 *  Link Constructor Subscripted Element Extraction  *
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the RefUV from which
 *				  the elements are to be extracted.
 *	linkConstructor		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the RefUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *****/
PublicFnDef M_CPD *rtREFUV_LCExtract (M_CPD *sourceCPD, rtLINK_CType* linkConstructor) {
    LCExtractCount++;
/*****
 *  Align the link constructor and validate its applicability as an extraction
 *  subscript for 'source'
 *****/
    linkConstructor->AlignForExtract (rtREFUV_Align (sourceCPD, false), UV_CPx_PToken);

/*****  Extract the requested values  *****/
    return rtREFUV_Align (
	rtREFUV_New (
	    linkConstructor->PPT (),
	    sourceCPD,
	    UV_CPx_RefPToken,
	    UV_InitLCExtractedUV,
	    sourceCPD,
	    linkConstructor,
	    InitializeRefUV,
	    rtPTOKEN_BaseElementCount (sourceCPD, UV_CPx_RefPToken)
	)
    );
}


/*******************************************************
 *  Reference U-Vector Subscripted Element Extraction  *
 *******************************************************/

/**********************************************************
 *  Internal 'Extract'ed U-Vector Initialization Routine  *
 **********************************************************/

/*---------------------------------------------------------------------------
 *****  Internal 'rtREFUV_New' initialization routine to extract a
 *****  collection of elements from a RefUV.
 *
 *  Arguments:
 *	extractedUVCPD		- an initialized standard CPD for the
 *				  RefUV being extracted.
 *	nelements		- the number of elements in the RefUV
 *				  being extracted.
 *	ap			- a <varargs.h> argument pointer positioned
 *				  at a CPD for the source u-vector followed
 *				  by a CPD for the subscript reference
 *				  u-vector.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeExtractedUV (
    M_CPD *extractedUVCPD, size_t Unused(nelements), va_list ap
) {
    V::VArgList iArgList (ap);

    M_CPD *sourceCPD = iArgList.arg<M_CPD*>();
    M_CPD *refuvCPD  = iArgList.arg<M_CPD*>();
    rtREFUV_ElementType
	*tp, *source;
    int
	*ip, *il, index,
	iRefNil = UV_CPD_ElementCount (sourceCPD),
	sRefNil = rtPTOKEN_BaseElementCount (sourceCPD, UV_CPx_RefPToken);

    for (source = rtREFUV_CPD_Array (sourceCPD),
	 tp = rtREFUV_CPD_Array (extractedUVCPD),
	 il =
	     UV_CPD_ElementCount (refuvCPD) +
	         (ip = rtREFUV_CPD_Array (refuvCPD));
	 ip < il;
	 *tp++ = (index = *ip++) >= iRefNil ? sRefNil : source[index]);
}


/**********************************
 *  External 'UVExtract' Routine  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  Reference U-Vector Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the RefUV from which
 *				  the elements are to be extracted.
 *	refuvCPD		- a standard CPD for a reference U-Vector
 *				  specifying the elements to be extracted.
 *				  The reference U-Vector must be referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the RefUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[refuv[i]];
 *
 *****/
PublicFnDef M_CPD *rtREFUV_UVExtract (M_CPD *sourceCPD, M_CPD *refuvCPD) {
    UVExtractCount++;

/*****
 * Align 'source' and 'refuv' and validate 'refuv's applicability as an
 * extraction subscript for 'source'.
 *****/
    rtREFUV_AlignForExtract (
	rtREFUV_Align (sourceCPD, false), UV_CPx_PToken, refuvCPD
    );

/*****  Extract and return the requested values  *****/
    VCPDReference pPPT (refuvCPD, UV_CPx_PToken, RTYPE_C_PToken);
    return rtREFUV_Align (
	rtREFUV_New (
	    pPPT,
	    sourceCPD,
	    UV_CPx_RefPToken,
	    InitializeExtractedUV,
	    sourceCPD,
	    refuvCPD
	)
    );
}


/**********************************************
 *  Reference Subscripted Element Extraction  *
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to extract an element of a RefUV given a reference.
 *
 *  Arguments:
 *	resultAddr		- the address of a reference to be initialized
 *				  to the element extracted.
 *	sourceCPD		- a standard CPD for the RefUV.
 *	referenceAddr		- the address of a reference.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on '*resultAddr'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    *resultAddr = source[reference]
 *
 *****/
PublicFnDef void rtREFUV_RFExtract (
    rtREFUV_TypePTR_Reference	resultAddr,
    M_CPD			*sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
)
{
    unsigned int element;
/*****
 * Align 'source' and 'reference' and validate 'reference's applicability as an
 * extraction subscript for 'source'.
 *****/
    RFExtractCount++;

    rtREFUV_AlignAndValidateRef (
	referenceAddr, rtREFUV_Align (sourceCPD, false), UV_CPx_PToken
    );

/*****  Extract and return the requested values  *****/
    DSC_InitReferenceScalar (
	*resultAddr,
 	UV_CPD_RefPTokenCPD (sourceCPD),
	rtREFUV_CPD_Array (sourceCPD) [
	    element = rtREFUV_Ref_Element (referenceAddr)
	]
    );
    rtREFUV_AlignReference (resultAddr);
    if (element >=
	rtPTOKEN_CPD_BaseElementCount(rtREFUV_Ref_RefPTokenCPD (referenceAddr))
    ) rtREFUV_Ref_Element (resultAddr) = rtPTOKEN_CPD_BaseElementCount (
	rtREFUV_Ref_RefPTokenCPD (resultAddr)
    );
}


/*****************************************************
 *  Link Constructor Subscripted Element Assignment  *
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted U-Vector Element Assignment.
 *
 *  Arguments:
 *	targetCPD		- a standard CPD for the u-vector to be
 *				  updated.
 *	linkConstructor		- the address of a link constructor specifying
 *				  the elements of 'target' to be updated.  The
 *				  link must be related positionally to 'source'
 *				  and referentially to 'target'.
 *	sourceCPD		- a standard CPD for the u-vector supplying the
 *				  values to be assigned to 'target'.
 *
 *  Returns:
 *	'targetCPD'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *****/
PublicFnDef M_CPD *rtREFUV_LCAssign (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, M_CPD *sourceCPD
) {
/*****
 * Align 'source' and 'target' and validate that they reference the same
 * P-Token.
 *****/
    LCAssignCount++;

    rtREFUV_Align (targetCPD);
    rtREFUV_Align (sourceCPD);

    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, targetCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_LCAssign: Source And Target Referentially Incompatible"
    );

/*****  Perform the assignment  *****/
    return UV_LCAssign (targetCPD, linkConstructor, sourceCPD);
}


/*******************************************************
 *  Reference U-Vector Subscripted Element Assignment  *
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  U-Vector Element Assignment.
 *
 *  Arguments:
 *	targetCPD		- a standard CPD for the u-vector to be
 *				  updated.
 *	refuvCPD		- a standard CPD for a reference U-Vector
 *				  specifying the elements of 'target' to be
 *				  updated.  The reference u-vector must be
 *				  related positionally to 'source' and
 *				  referentially to 'target'.
 *	sourceCPD		- a standard CPD for the u-vector supplying the
 *				  values to be assigned to 'target'.
 *
 *  Returns:
 *	'targetCPD'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[refuv[i]] = source[i];
 *
 *****/
PublicFnDef M_CPD *rtREFUV_UVAssign (M_CPD *targetCPD, M_CPD *refuvCPD, M_CPD *sourceCPD) {
    rtREFUV_ElementType		*target, *sp;
    int				*ip, *il;

/*****
 * Align 'source' and 'target' and validate that they reference the same
 * P-Token.
 *****/
    UVAssignCount++;

    rtREFUV_Align (targetCPD);
    rtREFUV_Align (sourceCPD);

    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, targetCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_UVAssign: Source And Target Referentially Incompatible"
    );

/*****
 * Align 'refuv' and validate its applicability as a subscript for assignment
 * of 'source' into 'target'.
 *****/
    rtREFUV_AlignForAssign (
	targetCPD, UV_CPx_PToken, refuvCPD, sourceCPD, UV_CPx_PToken
    );

/*****  Finally, do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

    for (target = rtREFUV_CPD_Array (targetCPD),
	 sp = rtREFUV_CPD_Array (sourceCPD),
	 il =
	     UV_CPD_ElementCount (refuvCPD) +
	         (ip = rtREFUV_CPD_Array (refuvCPD));
	 ip < il;
	 target[*ip++] = *sp++);

    return targetCPD;
}


/**********************************************
 *  Reference Subscripted Element Assignment  *
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Reference Element Assignment.
 *
 *  Arguments:
 *	targetCPD		- a standard CPD for the u-vector to be
 *				  updated.
 *	reference		- the address of a 'reference' specifying the
 *				  element of 'target' to be updated.  The
 *				  'reference' must be referentially related to
 *				  'target'.
 *	value			- the address of a reference supplying the
 *				  scalar value to be assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    target[reference] = value;
 *
 *****/
PublicFnDef void rtREFUV_RFAssign (
    M_CPD *targetCPD, rtREFUV_TypePTR_Reference reference, rtREFUV_TypePTR_Reference value
)
{
    RFAssignCount++;

/*****
 * Align 'target' and 'reference' and validate the applicability of
 * 'reference' as subscript for assignment into 'target'.
 *****/
    rtREFUV_AlignAndValidateRef (
	reference, rtREFUV_Align (targetCPD), UV_CPx_PToken
    );

/*****  Validate the source and target referentially  *****/
    if (targetCPD->ReferenceDoesntName (
	    UV_CPx_RefPToken, rtREFUV_Ref_RefPTokenCPD (value)
	)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_RFAssign: Source And Target Referentially Incompatible"
    );

/*****  Do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;
    rtREFUV_CPD_Array (targetCPD)
        [rtREFUV_Ref_Element (reference)] = rtREFUV_Ref_Element (value);
}


/****************************************************
 *  Link Constructor Subscripted Scalar Assignment  *
 ****************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Subscripted U-Vector Scalar Assignment.
 *
 *  Arguments:
 *	targetCPD		- a standard CPD for the u-vector to be
 *				  updated.
 *	linkConstructor		- the address of a link constructor specifying
 *				  the elements of 'target' to be updated.  The
 *				  link must be related referentially to
 *				  'target'.
 *	valuePtr		- the address of the reference containing the
 *				  value to be assigned.
 *
 *  Returns:
 *	'targetCPD'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = value;
 *
 *****/
PublicFnDef M_CPD *rtREFUV_LCAssignScalar (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, rtREFUV_TypePTR_Reference valuePtr
) {
    LCAssignScalarCount++;

/*****  Align the reference and 'target' and perform the assignment  *****/
    return UV_LCAssignScalar (
	rtREFUV_Align (targetCPD),
	linkConstructor,
	(pointer_t)&rtREFUV_Ref_Element (rtREFUV_AlignReference (valuePtr))
    );
}


/******************************************************
 *  Reference U-Vector Subscripted Scalar Assignment  *
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  U-Vector Element Assignment.
 *
 *  Arguments:
 *	targetCPD		- a standard CPD for the u-vector to be
 *				  updated.
 *	refuvCPD		- a standard CPD for a reference U-Vector
 *				  specifying the elements of 'target' to be
 *				  updated.  The reference u-vector must be
 *				  related referentially to 'target'.
 *	valuePtr		- the address of the reference containing the
 *				  value to be assigned.
 *
 *  Returns:
 *	'targetCPD'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[refuv[i]] = value;
 *
 *****/
PublicFnDef M_CPD *rtREFUV_UVAssignScalar (
    M_CPD *targetCPD, M_CPD *refuvCPD, rtREFUV_TypePTR_Reference valuePtr
)
{
    rtREFUV_ElementType		*target,
				value = rtREFUV_Ref_Element (rtREFUV_AlignReference (valuePtr));
    int				*ip, *il;

/*****
 * Align 'refuv' and validate the applicability of
 * 'refuv' as subscript for assignment into 'target'.
 *****/
    UVAssignScalarCount++;

    rtREFUV_AlignForScalarAssign (
	rtREFUV_Align (targetCPD), UV_CPx_PToken, refuvCPD
    );

/*****  Do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

    for (target = rtREFUV_CPD_Array (targetCPD),
	 il =
	     UV_CPD_ElementCount (refuvCPD) +
	         (ip = rtREFUV_CPD_Array (refuvCPD));
	 ip < il;
	 target[*ip++] = value);

    return targetCPD;
}


/**************************************************
 *****  Reference U-Vector Ordering Routines  *****
 **************************************************/

/***************************
 *  Internal Sort Routine  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Internal routine invoked by 'rtREFUV_New' to initialize a new
 *****  RefUV to the indices obtained from sorting another RefUV.
 *
 *  Arguments:
 *	pResult			- an initialized standard CPD for the RefUV
 *				  which is be initialized to the indices
 *				  produced by the sort.
 *	nelements		- the number of elements in the RefUV's.
 *				  Clearly, also the number of elements being
 *				  sorted.
 *	ap			- a <varargs.h> argument pointer pointing to
 *				  the address of a VkRadixListSorter for the
 *				  reference u-vector being sorted.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void SortRefUV (M_CPD *pResult, size_t nelements, va_list ap) {
    V::VArgList iArgList (ap);
    VkRadixListSorter *pSorter = iArgList.arg<VkRadixListSorter*>();
    pSorter->GetSort ((unsigned int *)rtREFUV_CPD_Array (pResult));
}


/***************************
 *  External Sort Routine  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to compute ascending sort indices for a reference u-vector.
 *
 *  Argument:
 *	refuvCPD		- a standard CPD for the reference u-vector
 *				  to be 'ordered'.
 *	fResultRequired		- true if a the sort u-vector must always be
 *				  returned, even for trivial and already sorted
 *				  arguments.  When false, this routine returns
 *				  a null result for trivial (cardinality < 2)
 *				  and already sorted reference u-vectors.  This
 *				  argument is optional and defaults to true.
 *
 *  Returns:
 *	The required sort indices.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_AscendingSortIndices (M_CPD *refuvCPD, bool fResultRequired) {
    AscendingSortIndicesCount++;

/*****  First align the u-vector to be sorted...  *****/
    rtREFUV_Align (refuvCPD);

/*****  Create a new P-Token for the positional state of the result  *****/
    unsigned int cElements = UV_CPD_ElementCount (refuvCPD);
    VkRadixListSorter iSorter;
    if (!iSorter.SetElements (
	    (unsigned int const *)rtREFUV_CPD_Array (refuvCPD), cElements
	) && !fResultRequired
    ) return NilOf (M_CPD*);

/*****  Create and initialize the sort indices  *****/
    M_CPD *pResultPPT = rtPTOKEN_New (refuvCPD->ScratchPad (), cElements);
    M_CPD *pResult = rtREFUV_New (
	pResultPPT, refuvCPD, UV_CPx_PToken, SortRefUV, &iSorter
    );
    pResultPPT->release ();

    return pResult;
}


/*---------------------------------------------------------------------------
 ***** Routine To Distribute from a refuv into a new refuv and
 ***** switch the new refuv's ptokens.
 *
 *  Arguments:
 *	refuvCPD	- a standard CPD for the reference U-Vector to be
 *                        used as the source.
 *
 *  Returns:
 * 	A standard CPD for the new reference U-Vector.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_Flip (M_CPD *refuvCPD) {
    FlipCount++;

    rtREFUV_Align (refuvCPD);	//  ... alignment call is new as of 6.3.0

    unsigned int size = rtPTOKEN_BaseElementCount (refuvCPD, UV_CPx_RefPToken);
    if (size != UV_CPD_ElementCount (refuvCPD)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_Flip: The size of the two ptokens is different"
    );

    M_CPD *newuvCPD = rtREFUV_New (refuvCPD, UV_CPx_RefPToken, refuvCPD, UV_CPx_PToken);

    rtREFUV_ElementType const *pSourceArray = rtREFUV_CPD_Array (refuvCPD);
    rtREFUV_ElementType *pResultArray = rtREFUV_CPD_Array (newuvCPD);

    for (unsigned int i=0; i<size; i++)
	pResultArray[pSourceArray[i]] = i;

    return newuvCPD;
}


/************************************
 *****  Partitioned Operations  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to generate partitioned sort indices
 *
 *  Arguments:
 *	partition		- a link constructor which partitions the
 *				  elements of 'values'.  Two elements of
 *				  'values' belong to the same partition
 *				  if corresponding positions in this
 *				  constructor contain the same reference.
 *	values			- a standard CPD for a u-vector supplying the
 *				  values to be sorted.
 *	descending		- a boolean which, when true, instructs this
 *				  routine to generate sort indices for a
 *				  descending sort.
 *
 *  Returns:
 *	A standard CPD for a reference U-Vector supplying sort indices for
 *	'values' suitable for use with 'LCExtract'.
 *
 *  Notes:
 *	The positional and referential p-tokens of the sort indices generated
 *	by this routine are identical.
 *
 *****/
PublicFnDef M_CPD *rtREFUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    while (count-- > 0)\
	*resultPtr++ = position++

#define handleRepetition(position, count, reference)\
    if (descending)\
	UTIL_IntQSort (valuesArray + position, (unsigned int *)resultPtr, count, true);\
    else UTIL_RadixListSort (\
	(unsigned int const*)valuesArray + position, (unsigned int *)resultPtr, count\
    );\
    for (back = count + (front = resultPtr);\
	 front < back;\
	 *front++ += position);\
    resultPtr += count

/*---------------------------------------------------------------------------
 *****  Local Variable Definitions
 *---------------------------------------------------------------------------*/
    rtREFUV_ElementType *front, *back;

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtREFUV_Align (values), UV_CPx_PToken);

/*****  Create the sort indices...  *****/
    M_CPD *result = rtREFUV_New (partition->PPT (), partition->PPT ());

/*****  Initialize the traversal pointers...  *****/
    rtREFUV_ElementType const *valuesArray = rtREFUV_CPD_Array (values);
    rtREFUV_ElementType *resultPtr = rtREFUV_CPD_Array (result);

/*****  Traverse the partitioning link...  *****/
    rtLINK_TraverseRRDCList (
	partition, handleRepetition, handleRepetition, handleRange
    );

/*****  Return the computed sort indices...  *****/
    return result;

/*---------------------------------------------------------------------------
 *****  Link Traversal Component Handler Macro Deletions
 *---------------------------------------------------------------------------*/
#undef handleRange
#undef handleRepetition
}


/*---------------------------------------------------------------------------
 *****  Routine to perform a partitioned range assignment.
 *
 *  Arguments:
 *	target			- a standard CPD for the u-vector to be updated
 *				  by this routine.
 *	origins			- a standard CPD for a reference u-vector
 *				  specifying, for each partition referenced
 *				  by 'partition', the origin in 'target' at
 *				  which the elements of 'source' are to be
 *				  copied.
 *	partition		- a link constructor which partitions the
 *				  elements of 'source'.  Two elements of
 *				  'source' belong to the same partition if
 *				  corresponding positions in this constructor
 *				  contain the same reference.
 *	source			- a standard CPD for a u-vector supplying the
 *				  values to be 'assigned'.
 *
 *  Returns:
 *	'target'
 *
 *****/
PublicFnDef M_CPD *rtREFUV_PartitndAssign (
    M_CPD *target, M_CPD *origins, rtLINK_CType *partition, M_CPD *source
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    while (count-- > 0)\
	targetArray [originArray [reference++]++] = *sourcePtr++

#define handleRepetition(position, count, reference)\
    memcpy\
	(targetArray + originArray [reference],\
	 sourcePtr,\
	 count * sizeof (rtREFUV_ElementType));\
    originArray [reference] += count;\
    sourcePtr += count

#define handleNil(position, count, reference)

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    rtREFUV_AlignForExtract (rtREFUV_Align (target), UV_CPx_PToken, origins);

    partition->AlignForAssign (
	origins, UV_CPx_PToken, rtREFUV_Align (source), UV_CPx_PToken
    );

/*****  Initialize the traversal pointers...  *****/
    rtREFUV_ElementType	*originArray = rtREFUV_CPD_Array (origins);
    rtREFUV_ElementType	*targetArray = rtREFUV_CPD_Array (target);
    rtREFUV_ElementType	const *sourcePtr = rtREFUV_CPD_Array (source);

/*****  Traverse the partitioning link...  *****/
    rtLINK_TraverseRRDCList (
	partition, handleNil, handleRepetition, handleRange
    );

/*****  And return...  *****/
    return target;

/*---------------------------------------------------------------------------
 *****  Link Traversal Component Handler Macro Deletions
 *---------------------------------------------------------------------------*/
#undef handleRange
#undef handleRepetition
#undef handleNil
}


/*---------------------------------------------------------------------------
 *****  Routine to partition the partitioned elements of a u-vector
 *****  by identity.
 *
 *  Arguments:
 *	partition		- a link constructor which partitions the
 *				  elements of 'values'.  Two elements of
 *				  'values' belong to the same partition
 *				  if corresponding positions in this
 *				  constructor contain the same reference.
 *	values			- a vector constructor supplying the values
 *				  to be partitioned.  Although not absolutely
 *				  required, the most meaningful results are
 *				  obtained when 'values' is sorted.
 *	majorLC			- an address which will be set to the address
 *				  of a link constructor defining the majorLC
 *				  partition computed by this routine.  This
 *				  constructor will share the same REFERENTIAL
 *				  state as 'partition'.
 *	minorLC			- an address which will be set to the address
 *				  of a link constructor defining the minorLC
 *				  partition computed by this routine.  This
 *				  constructor will share the same POSITIONAL
 *				  state as 'values' and 'partition' and will
 *				  have a REFERENTIAL state which matches the
 *				  POSITIONAL state of 'majorLC'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on '*majorLC' and '*minorLC' only.
 *
 *  Notes:
 *	This routine forms 'groups within groups'.  For each group defined
 *	by 'partition', this routine will group the elements of that group
 *	by matching identity.  The link constructor returned as 'minorLC'
 *	partitions the elements of 'values' (subject to the boundaries
 *	established by 'partition'),  the link constructor returned as
 *	'majorLC' partitions the groups defined by 'minorLC' to match the
 *	original collection of groups defined by 'partition'.
 *
 *****/
PublicFnDef void rtREFUV_PartitndPartition (
    rtLINK_CType *partition, M_CPD *values, rtLINK_CType **majorLC, rtLINK_CType **minorLC
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    rtLINK_AppendRange (majorPartition, reference, count);\
    rtLINK_AppendRange (minorPartition, minorCount, count);\
    minorCount += count;\
    valuesPtr += count

#define handleRepetition(position, count, reference)\
    initialMinorCount = minorCount;\
    minorRepetitionSize = 0;\
    previousValue = *valuesPtr;\
    while (count-- > 0) {\
	if (previousValue == (currentValue = *valuesPtr++))\
	    minorRepetitionSize++;\
	else {\
	    rtLINK_AppendRepeat (minorPartition, minorCount++, minorRepetitionSize);\
	    previousValue = currentValue;\
	    minorRepetitionSize = 1;\
	}\
    }\
    if (minorRepetitionSize > 0) {\
	rtLINK_AppendRepeat (minorPartition, minorCount++, minorRepetitionSize);\
    }\
    rtLINK_AppendRepeat (majorPartition, reference, minorCount - initialMinorCount)

/*---------------------------------------------------------------------------
 *****  Local Variable Definitions
 *---------------------------------------------------------------------------*/
    unsigned int initialMinorCount, minorRepetitionSize;
    rtREFUV_ElementType	previousValue, currentValue;

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtREFUV_Align (values), UV_CPx_PToken);

/*****  Create the major and minor partitions...  *****/
    rtLINK_CType *majorPartition = rtLINK_RefConstructor (partition->RPT (), -1);
    rtLINK_CType *minorPartition = rtLINK_PosConstructor (partition->PPT (), -1);

/*****  Initialize the traversal pointers...  *****/
    rtREFUV_ElementType	const *valuesPtr = rtREFUV_CPD_Array (values);
    unsigned int minorCount = 0;

/*****  Traverse the partitioning link...  *****/
    rtLINK_TraverseRRDCList (
	partition, handleRepetition, handleRepetition, handleRange
    );

/*****  Close and return the partitions created...  *****/
    M_CPD *groupPToken = rtPTOKEN_New (partition->PPT ()->Space (), minorCount);
    *majorLC = majorPartition->Close (groupPToken);
    *minorLC = minorPartition->Close (groupPToken);
    groupPToken->release ();

/*---------------------------------------------------------------------------
 *****  Link Traversal Component Handler Macro Deletions
 *---------------------------------------------------------------------------*/
#undef handleRange
#undef handleRepetition
}


/***********************************
 *****  Linearization Routine  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Calculate the cartesian linear location given the row and column.
 *
 *  Arguments:
 *	resultRefp	- a pointer to a reference which will be set
 *			  to contain the result linearization.
 *	cartesianPT	- a standard CPD for the cartesian PToken whose row
 *                        PToken must correspond referentially to 'rowRefp'
 *			  and whose column PToken must correspond referentially
 *                        to 'columnRefp'.
 *	rowRefp	    	- a pointer to a reference whose element is the row.
 *	columnRefp	- a pointer to a reference whose element is the column.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtREFUV_LinearizeRrRc (
    rtREFUV_TypePTR_Reference	resultRefp,
    M_CPD			*cartesianPT,
    rtREFUV_TypePTR_Reference	rowRefp,
    rtREFUV_TypePTR_Reference	columnRefp
)
{
    int	numberOfColumns, numberOfRows, result;

    LinearizeRrRcCount++;

/*****  Align and Validate 'rowRefp' and 'columnRefp'  *****/
    rtREFUV_AlignReference (rowRefp);
    rtREFUV_AlignReference (columnRefp);
    rtPTOKEN_CartesianVerification (
	cartesianPT,
	rtREFUV_Ref_RefPTokenCPD (rowRefp),
	rtREFUV_Ref_RefPTokenCPD (columnRefp),
	-1
    );

/*****  Calculate the linearization ... *****/
    numberOfColumns = rtPTOKEN_CPD_BaseElementCount (
	rtREFUV_Ref_RefPTokenCPD (columnRefp)
    );
    numberOfRows = rtPTOKEN_CPD_BaseElementCount (
	rtREFUV_Ref_RefPTokenCPD (rowRefp)
    );

    /*** If either the row or column are the reference nil value, the result
     *** is the cartesian reference nil.
     ***/
    result =
	(rtREFUV_Ref_Element (rowRefp) == numberOfRows ||
	 rtREFUV_Ref_Element (columnRefp) == numberOfColumns)
	    ? rtPTOKEN_CPD_BaseElementCount (cartesianPT)
	    :  (rtREFUV_Ref_Element (rowRefp) * numberOfColumns
		    + rtREFUV_Ref_Element (columnRefp));

    cartesianPT->retain ();
    DSC_InitReferenceScalar (*resultRefp, cartesianPT, result);

    return;
}


/*********************************
 *****  LocateOrAdd Routine  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to locate or add a set of values to a reference set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for a reference set uvector for the
 *                        uvector to be added to.
 *	sourceCPD	- a standard CPD for a reference set uvector containing
 *			  the elements to locate or add.
 *	locationsLinkC	- the address of a link constructor to be created.
 *			  For each value in the 'sourceCPD' it will contain
 *                        the position in the 'targetCPD' where that value
 *                        was found or added.
 *	addedLinkC	- optional (if not requested set to Nil).
 *                        the address of an uninitialized link constructor
 *		          which will be created iff requested and meaningful.
 *                        It will contain the positions from
 *                        'sourceCPD' whose values were added to
 *                        the target set uvector.  If none of the values from
 *                        the source were added, then this link will be
 *                        unused and set to Nil.
 *	pContainerSpace	- the space in which containers required to represent
 *			  the additions return will be created.  If omitted,
 *			  the key (query) space is used.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	If 'addedLinkC' is constructed, it's domain will reside in the key's
 *	scratch pad.
 *
 *****/
PublicFnDef void rtREFUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC
) {
    int
	wantAddedLinkC, largestSize, notNil, found,
	resultCount, srcPos, notYetInitialized = true,
	origTargetCount;
    rtREFUV_ElementType
	*resultStart, *resultp, *srcp, *srcpl, *trgp, *trgpl;
    rtPTOKEN_CType *targetPTokenC;

/***** Target handling Macros *****/
#define output(value) {\
    *resultp++ = value;\
    resultCount++;\
}

#define advance {\
    if (notNil) {\
	/*** output original value to result ***/\
	output (*trgp);\
    }\
    trgp++;\
    notNil = (trgp < trgpl);\
}

#define locate(value) {\
    while (notNil && (value) > *trgp)\
	advance;\
    found = (notNil && (value) == *trgp);\
}

#define initializeAdditionConstructors if (notYetInitialized) {\
    /*** Create the added link constructor ... ***/\
    if (wantAddedLinkC)\
    	*addedLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);\
\
    /*** Create a ptoken constructor for the target ... ***/\
    targetPTokenC = rtPTOKEN_NewShiftPTConstructor (targetCPD, UV_CPx_PToken);\
    notYetInitialized = false;\
}

    LocateOrAddCount++;

/***** Make sure both the source and target are reference set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_RefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_LocateOrAdd:  Source UVector Must Be A Reference Set"
    );

    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_RefUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_LocateOrAdd:  Target UVector Must Be A Reference Set"
    );

/***** Align and validate source and target uvectors ... *****/
    rtREFUV_Align (sourceCPD);
    rtREFUV_Align (targetCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, targetCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_LocateOrAdd:  Referential Inconsistency"
    );

/***** Make the link constructors ... *****/
    if (wantAddedLinkC = (int)(IsntNil (addedLinkC)))
	*addedLinkC = NilOf (rtLINK_CType *);

    *locationsLinkC = rtLINK_PosConstructor (sourceCPD, UV_CPx_PToken);

/***** If the source is empty, close the locationsLinkC and return ... *****/
    if (UV_CPD_ElementCount (sourceCPD) == 0) {
	(*locationsLinkC)->Close (targetCPD, UV_CPx_PToken);
	return;
    }

/***** Allocate space for the new target values ... *****/
    origTargetCount = UV_CPD_ElementCount (targetCPD);
    largestSize = UV_CPD_ElementCount (sourceCPD) + origTargetCount;

    resultStart = resultp = (rtREFUV_ElementType *) UTIL_Malloc (
	largestSize * sizeof (rtREFUV_ElementType)
    );

/***** Begin the traversal ... *****/
    resultCount = 0;
    srcp = rtREFUV_CPD_Array (sourceCPD);
    srcpl = srcp + UV_CPD_ElementCount (sourceCPD);
    srcPos = 0;
    trgp = rtREFUV_CPD_Array (targetCPD);
    trgpl = trgp + origTargetCount;
    notNil = (origTargetCount > 0);

/***** Loop through the source looking for the values in target ... *****/
    while (srcp < srcpl)
    {
	locate (*srcp);
	if (!found) {
	/*** Must Add this value ***/
	    initializeAdditionConstructors;

	    /*** output to addedLinkC ***/
	    if (wantAddedLinkC)
	    	rtLINK_AppendRange (*addedLinkC, srcPos, 1);

	    /*** modify target ptoken ***/
	    targetPTokenC->AppendAdjustment (resultCount, 1);

	    /*** Store where this value was found in 'locations' ... ***/
	    rtLINK_AppendRange (*locationsLinkC, resultCount, 1);

	    /*** add to the new target ***/
	    output (*srcp);
	}
    	else {
	    /*** Store where this value was found in 'locations' ... ***/
	    rtLINK_AppendRange (*locationsLinkC, resultCount, 1);
	}

	/*** Increment the source pointer for the next value ... ***/
	srcp++;
	srcPos++;
    }

/***** Done with the source, make sure that the target is finished ... *****/
    while (notNil)
	advance;

/***** Done with the traversal *****/

/***** If values were added ... *****/
    if (resultCount > origTargetCount) {
	/***** Deal with addedLinkC ... *****/
	if (wantAddedLinkC) {
	    M_CPD *ptoken = rtPTOKEN_New (
		sourceCPD->ScratchPad (), (*addedLinkC)->ElementCount ()
	    );
	    (*addedLinkC)->Close (ptoken);
	    ptoken->release ();
	}

	/***** Reconstruct the resulting target uvector ... *****/

	/*** Make the target writable ... ***/
	targetCPD->EnableModifications ();

	/*** Shift the container tail to add room ... ***/
	rtREFUV_CPD_Element (targetCPD) = rtREFUV_CPD_Array (targetCPD) + origTargetCount;

	targetCPD->ShiftContainerTail (
	    UV_CPx_Element,
	    0,
	    ((ptrdiff_t)resultCount - origTargetCount) * UV_CPD_Granularity (targetCPD),
	    false
	);

	UV_CPD_ElementCount (targetCPD) = resultCount;

	/*** Copy over the new values ... ***/
	resultp = resultStart;
	trgp = rtREFUV_CPD_Array (targetCPD);
	trgpl = trgp + resultCount;

	while (trgp < trgpl)
	    *trgp++ = *resultp++;

	/*** Deal with the new positional ptoken ... ***/
	M_CPD *ptoken = targetPTokenC->ToPToken ();

	targetCPD->StoreReference (UV_CPx_PToken, ptoken);

	ptoken->release ();
    }


/***** Close *locationsLinkC ... *****/
    (*locationsLinkC)->Close (targetCPD, UV_CPx_PToken);

/***** Free the scratch space and return ... *****/
    UTIL_Free (resultStart);
    return;

/***** Undefine the macros ... *****/
#undef output
#undef advance
#undef locate
#undef initializeAdditionConstructors
}


/*---------------------------------------------------------------------------
 *****  Routine to locate or add a value to a reference set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for a reference set uvector for the
 *                        uvector to be added to.
 *	sourcePtr	- the address of the scalar value to locate or add.
 *	locationPtr	- the address of a scalar value which this routine
 *                        will set to contain the position in 'targetCPD' at
 *                        which the value was found or added.
 *
 *  Returns:
 *	true if the value was added, false otherwise.
 *
 *****/
PublicFnDef bool rtREFUV_ScalarLocateOrAdd (
    M_CPD *targetCPD, rtREFUV_ElementType *sourcePtr, int *locationPtr
)
{
    int
	empty, size, low, hi, index, addAt;
    rtREFUV_ElementType
	value;

#define access(index)\
    *(rtREFUV_CPD_Array (targetCPD) + index)

    ScalarLocateOrAddCount++;

/***** Make sure the target is a reference set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_RefUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_ScalarLocateOrAdd:  Target UVector Must Be A Reference Set"
    );

/***** Align target uvector ... *****/
    rtREFUV_Align (targetCPD);

/*****  ...	binary Search for the value, ...  *****/
    size = UV_CPD_ElementCount (targetCPD);
    empty = (size == 0);
    value = *sourcePtr;

    for (low = 0, hi = size - 1, index = (hi + low) / 2;
	access(index) != value && low <= hi;
	index = (hi + low) / 2)
    {
	if (value < access(index))
	    hi = index - 1;
	else
	    low = index + 1;
    }


/***** Need to add the value ... *****/
    if (empty || access(index) != value)
    {
	if (empty)
	    addAt = 0;
	else
	{
/***** The binary search sets access(index) to be <= value.  The exception
 *     is when index is equal to 0.  Since index never gets set to -1, the
 *     0 value can mean several things.  This is why the 0 case is handled
 *     specially below.
 *****/
	    /*** fix index if neccessary ***/
	    if (index == 0 && access(index) > value)
		index = -1;
	    addAt = index + 1;
	}

	/*** Create a ptoken constructor for the target ... ***/
	rtPTOKEN_CType *targetPTokenC = rtPTOKEN_NewShiftPTConstructor (targetCPD, UV_CPx_PToken);

        /*** modify target ptoken ***/
	targetPTokenC->AppendAdjustment (addAt, 1);

	/***** Reconstruct the resulting target uvector ... *****/
	/*** Make the target writable ... ***/
	targetCPD->EnableModifications ();

	/*** Shift the container tail to add room ... ***/
	rtREFUV_CPD_Element (targetCPD) = rtREFUV_CPD_Array (targetCPD) + addAt;

	targetCPD->ShiftContainerTail (
	    UV_CPx_Element,
	    (size - addAt) * UV_CPD_Granularity (targetCPD),
	    UV_CPD_Granularity (targetCPD),
	    false
	);

	UV_CPD_ElementCount (targetCPD) = size + 1;

	/*** insert the value ***/
	*(rtREFUV_CPD_Array (targetCPD) + addAt) = value;

	/*** Deal with the new positional ptoken ... ***/
	M_CPD *ptoken = targetPTokenC->ToPToken ();

	targetCPD->StoreReference (UV_CPx_PToken, ptoken);

	ptoken->release ();

	*locationPtr = addAt;
	return true;
    }

/***** ... otherwise, the value was found *****/
    else {
	*locationPtr = index;
	return false;
    }

#undef access
}


/****************************
 *****  Lookup Routine  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup values in a reference set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the reference set uvector to be
 *			  be searched.
 *	keyCPD		- a standard CPD for the reference set uvector
 *			  containing the values to lookup in the 'sourceCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ, or
 *			          rtLINK_LookupCase_GE.
 *	locationsLinkC 	- a pointer to a link constructor which this routine
 *			  will create.  It will contain the locations in the
 *			  source where the looked up value was found.
 *	locatedLinkC	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found.  It will contain
 * 			  the positions in 'keyCPD' for which a value was
 *			  found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtREFUV_Lookup (
    M_CPD			*sourceCPD,
    M_CPD			*keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType		**locationsLinkC,
    rtLINK_CType		**locatedLinkC
)
{
    int
	backedupOk, found, srcPos, keyPos, sourceCount, notNil,
	someNotFound = false;
    rtREFUV_ElementType
	*srcp, *srcpl, *srcpf, *keyp, *keypl;

/***** Source Traversal Macros *****/
#define advance {\
    srcp++;\
    notNil = (srcp < srcpl);\
    srcPos++;\
}

#define backup {\
    if (srcp == srcpf)\
	backedupOk = false;\
    else {\
	srcp--;\
	notNil = (srcp < srcpl);\
	srcPos--;\
	backedupOk = true;\
    }\
}

#define locate(value) {\
    while (notNil && (value) > *srcp)\
	advance;\
\
    if (!notNil) {\
	if (lookupCase == rtLINK_LookupCase_LE) {\
	    backup;\
	    found = (backedupOk && notNil);\
	}\
	else found = false;\
    }\
    else {\
	if (lookupCase == rtLINK_LookupCase_GE)\
	    found = true;\
	else if (lookupCase == rtLINK_LookupCase_EQ)\
	    found = (*srcp == (value));\
	else  /* LE */ {\
	    if (!(found = (*srcp == (value)))) {\
		backup;\
		found = (backedupOk && notNil);\
	    }\
	}\
    }\
}

    LookupCount++;

/***** Make sure both the source and key are reference set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_RefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_Lookup:  Source UVector Must Be A Reference Set"
    );

    if (((RTYPE_Type)M_CPD_RType (keyCPD) != RTYPE_C_RefUV) ||
        !UV_CPD_IsASetUV (keyCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_Lookup:  Key UVector Must Be A Reference Set"
    );

/***** Align and validate source and key uvectors ... *****/
    rtREFUV_Align (sourceCPD);
    rtREFUV_Align (keyCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, keyCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_Lookup:  Referential Inconsistency"
    );

/***** Make the location link constructor ... *****/
    *locationsLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);

/***** If the key is empty, close the locationsLinkC and return ... *****/
    if (UV_CPD_ElementCount (keyCPD) == 0) {
	(*locationsLinkC)->Close (keyCPD, UV_CPx_PToken);
        *locatedLinkC = NilOf (rtLINK_CType *);
	return;
    }

/***** Make the located link constructor ... *****/
    *locatedLinkC = rtLINK_RefConstructor (keyCPD, UV_CPx_PToken);

/***** Begin the traversal ... *****/
    srcp = srcpf = rtREFUV_CPD_Array (sourceCPD);
    srcpl = srcp + (sourceCount = UV_CPD_ElementCount (sourceCPD));
    srcPos = 0;
    notNil = (sourceCount > 0);

    keyp = rtREFUV_CPD_Array (keyCPD);
    keypl = keyp + UV_CPD_ElementCount (keyCPD);
    keyPos = 0;

/***** Loop through the key looking for the values in the source ... *****/
    while (keyp < keypl)
    {
	locate (*keyp);

	if (!found)
	    someNotFound = true;
    	else  /* found */
    	{
	    /*** Store where this value was found ... ***/
	    rtLINK_AppendRange (*locationsLinkC, srcPos, 1);

	    /*** Store into the located linkc ... ***/
	    rtLINK_AppendRange (*locatedLinkC, keyPos, 1);
	}

	/*** Increment the key pointer for the next value ... ***/
	keyp++;
	keyPos++;
    }


/***** Done with the traversal *****/
/***** Close the link constructors ... *****/
    if (someNotFound) {
	M_CPD *ptoken = rtPTOKEN_New (keyCPD->Space (), (*locatedLinkC)->ElementCount ());
	(*locatedLinkC)->Close (ptoken);
	(*locationsLinkC)->Close (ptoken);
	ptoken->release ();
    }
    else {
	/*** all found, so the locatedLinkC is not needed ***/
	(*locatedLinkC)->release ();
	*locatedLinkC = NilOf (rtLINK_CType *);
	(*locationsLinkC)->Close (keyCPD, UV_CPx_PToken);
    }

    return;

/***** Undefine the macros ... *****/
#undef advance
#undef backup
#undef locate
}


/*---------------------------------------------------------------------------
 *****  Routine to lookup a value in a reference set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the reference set uvector to be
 *			  be searched.
 *	keyPtr		- the address of the scalar value to lookup in
 *			  the 'sourceCPD'.
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
PublicFnDef bool rtREFUV_ScalarLookup (
    M_CPD			*sourceCPD,
    rtREFUV_ElementType		*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
)
{
    int				size, low, hi, index;
    rtREFUV_ElementType		value;

#define access(index) *(rtREFUV_CPD_Array (sourceCPD) + index)

    ScalarLookupCount++;

/***** Make sure the source is a reference set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_RefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtREFUV_Lookup:  Source UVector Must Be A Reference Set"
    );

/***** Align the source uvector ... *****/
    rtREFUV_Align (sourceCPD);

/*****  ...	binary Search for the value, ...  *****/
    size = UV_CPD_ElementCount (sourceCPD);
    value = *keyPtr;

    /*** If the source is empty... return *****/
    if (size == 0)
	return false;

    for (low = 0, hi = size - 1, index = (hi + low) / 2;
	access(index) != value && low <= hi;
	index = (hi + low) / 2)
    {
	if (value < access(index))
	    hi = index - 1;
	else
	    low = index + 1;
    }

/***** The binary search sets access(index) to be <= value.  The exception
 *     is when index is equal to 0.  Since index never gets set to -1, the
 *     0 value can mean several things.  This is why the 0 case is handled
 *     specially below.
 *****/

    if (access(index) == value)
	*locationPtr = index;
    else if (lookupCase == rtLINK_LookupCase_GE)
    {
	if (index == 0 && (access(index) >= value))
	    *locationPtr = index;
	else
	{
	    index += 1;
	    if (index >= size)
		return false;
	    else *locationPtr = index;
	}
    }
    else if (lookupCase == rtLINK_LookupCase_LE)
    {
	if (index == 0)
	{
	    if (access(index) <= value)
		*locationPtr = index;
	    else
		return false;
	}
	else
	    *locationPtr = index;
    }
    else
	return false;

    return true;

#undef access
}


/************************************
 ************************************
 *****  Reference Nil Deletion  *****
 ************************************
 ************************************/

PublicFnDef int rtREFUV_DeleteNils (M_CPD *pTargetCPD) {
    rtREFUV_Align (pTargetCPD);
    unsigned int const* pTargetArray = (unsigned int *)rtREFUV_CPD_Array (pTargetCPD);

    rtPTOKEN_CType *pTargetAdjustment = NilOf (rtPTOKEN_CType *);

    unsigned int const xNil = rtPTOKEN_BaseElementCount (pTargetCPD, UV_CPx_RefPToken);
    unsigned int xRunOrigin = 0;
    unsigned int sRunExtent = 0;
    unsigned int xElement   = UV_CPD_ElementCount (pTargetCPD);
    while (xElement-- > 0) {
	if (sRunExtent > 0) {
	    if (pTargetArray[xElement] == xNil)
		sRunExtent++;
	    else {
		pTargetAdjustment->AppendAdjustment ((int)xRunOrigin, -(int)sRunExtent);
		sRunExtent = 0;
	    }
	}
	else if (pTargetArray[xElement] == xNil) {
	    if (IsNil (pTargetAdjustment)) pTargetAdjustment = rtPTOKEN_NewShiftPTConstructor (
		pTargetCPD, UV_CPx_PToken
	    );
	    xRunOrigin = xElement + 1;
	    sRunExtent = 1;
	}
    }
    if (sRunExtent > 0)
	pTargetAdjustment->AppendAdjustment ((int)xRunOrigin, -(int)sRunExtent);

    if (pTargetAdjustment) {
	pTargetAdjustment->ToPToken ()->release ();
	rtREFUV_Align (pTargetCPD);

	return true;
    }

    return false;
}


/************************************
 ************************************
 *****  Partitioned Operations  *****
 ************************************
 ************************************/

/*********************************************************
 *****  Partitioned Operation Driver Abstract Class  *****
 *********************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_PartitionedOperationDriver {
/*****  Construction  *****/
protected:
    rtREFUV_PartitionedOperationDriver (
	M_CPD *pTarget, unsigned int const* pTargetPartition
    );

/*****  Target Region Selection  *****/
protected:
    inline void setTargetRegionToPartition (unsigned int xPartition);

/*****  Target Region Search  *****/
protected:
    inline void setTargetRegionLowerBoundToComainElement (
	rtLINK_LookupCase	xLookupCase,
	unsigned int		xComainElement,
	bool			&fComainElementNotFound
    );

    inline void setTargetRegionLowerBoundToComainElement (
	unsigned int xComainElement, bool &fComainElementNotFound
    );

    unsigned int xTargetRegionLowerBound () const {
	return m_pTargetRegionCursor - m_pTargetArray;
    }

/*****  Error Generation  *****/
protected:
    void reportUnimplementedMember (char const *pMemberName);

/*****  State  *****/
protected:
    M_CPD *const		m_pTarget;
    unsigned int const*		m_pTargetArray;
    unsigned int const*		m_pTargetRegionOrigin;
    unsigned int const*		m_pTargetRegionCursor;
    unsigned int const*		m_pTargetRegionLimit;

    unsigned int const* const	m_pTargetPartition;

    unsigned int		m_xTargetComainNil;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_PartitionedOperationDriver::rtREFUV_PartitionedOperationDriver (
    M_CPD *pTarget, unsigned int const* pTargetPartition
) : m_pTarget(pTarget) , m_pTargetPartition(pTargetPartition)
{
    rtREFUV_Align (m_pTarget);
    m_pTargetArray = (unsigned int const*)rtREFUV_CPD_Array (m_pTarget);

    m_xTargetComainNil = rtPTOKEN_BaseElementCount (m_pTarget, UV_CPx_RefPToken);
}


/*****************************
 *  Target Region Selection  *
 *****************************/

inline void rtREFUV_PartitionedOperationDriver::setTargetRegionToPartition (
    unsigned int xPartition
)
{
    unsigned int const* pPartition = m_pTargetPartition + xPartition;
    m_pTargetRegionCursor =
    m_pTargetRegionOrigin = m_pTargetArray + pPartition[0];
    m_pTargetRegionLimit  = m_pTargetRegionOrigin + (pPartition[1] - pPartition[0]);
}


/**************************
 *  Target Region Search  *
 **************************/

inline void rtREFUV_PartitionedOperationDriver::setTargetRegionLowerBoundToComainElement (
    unsigned int xComainElement, bool &fComainElementNotFound
)
{
    unsigned int xDomainLowerBound = 0;
    unsigned int xDomainUpperBound = m_pTargetRegionLimit - m_pTargetRegionCursor;

    if (xComainElement < m_xTargetComainNil) while (xDomainUpperBound > xDomainLowerBound)
    {
	unsigned int xCurrentDomainElement = (xDomainUpperBound + xDomainLowerBound) / 2;
	unsigned int xCurrentComainElement = m_pTargetRegionCursor[xCurrentDomainElement];
	if (xCurrentComainElement == xComainElement)
	{
	    //  Comain element found...
	    m_pTargetRegionCursor += xCurrentDomainElement;
	    fComainElementNotFound = false;
	    return;
	}
	else if (xCurrentComainElement < xComainElement)
	    xDomainLowerBound = xCurrentDomainElement + 1;
	else
	    xDomainUpperBound = xCurrentDomainElement;
    }

    // Comain element not found...
    m_pTargetRegionCursor += xDomainUpperBound;
    fComainElementNotFound = true;
}


inline void rtREFUV_PartitionedOperationDriver::setTargetRegionLowerBoundToComainElement (
    rtLINK_LookupCase		xLookupCase,
    unsigned int		xComainElement,
    bool			&fComainElementNotFound
)
{
    setTargetRegionLowerBoundToComainElement (xComainElement, fComainElementNotFound);

/*---------------------------------------------------------------------------
 * The target region lower bound now points to the first element >=
 * 'xComainElement'.  If there is no such element, the target region lower
 * bound points to the region limit which is one element beyond the region.
 *---------------------------------------------------------------------------
 */
    if (xComainElement < m_xTargetComainNil) switch (xLookupCase)
    {
    case rtLINK_LookupCase_LT:
	// We're >=, only report a find if back-up is possible...
	if (m_pTargetRegionOrigin < m_pTargetRegionCursor)
	{
	    m_pTargetRegionCursor += -1;
	    fComainElementNotFound = false;
	}
	else fComainElementNotFound = true;
	break;
    case rtLINK_LookupCase_LE:
	// If not found and back-up is possible, do so and report a find...
	if (fComainElementNotFound && m_pTargetRegionOrigin < m_pTargetRegionCursor)
	{
	    m_pTargetRegionCursor += -1;
	    fComainElementNotFound = false;
	}
	break;
    case rtLINK_LookupCase_EQ:
	break;
    case rtLINK_LookupCase_GE:
	// If not found and in the region, we matched the > case...
	if (fComainElementNotFound && m_pTargetRegionLimit > m_pTargetRegionCursor)
	    fComainElementNotFound = false;
	break;
    case rtLINK_LookupCase_GT:
	// If found, advance the bound and report a find if still in the region...
	if (!fComainElementNotFound)
	    fComainElementNotFound = ++m_pTargetRegionCursor >= m_pTargetRegionLimit;
	// else if in the region, we matched the > case...
	else if (m_pTargetRegionCursor < m_pTargetRegionLimit)
	    fComainElementNotFound = false;
	break;
    default:
	reportUnimplementedMember (
	    UTIL_FormatMessage (
		"rtREFUV_PartitionedOperationDriver::setTargetRegionLowerBoundToComainElement[%u]",
		xLookupCase
	    )
	);
	break;
    }
}


/**********************
 *  Error Generation  *
 **********************/

void rtREFUV_PartitionedOperationDriver::reportUnimplementedMember (char const *pMemberName) {
    ERR_SignalFault (
	EC__UnimplementedCase,
	UTIL_FormatMessage ("%s: Not Implemented", pMemberName)
    );
}


/***********************************************************************
 *****  Partitioned Locate Or Add Operation Driver Abstract Class  *****
 ***********************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_PartitionedLocateOrAddDriver : public rtREFUV_PartitionedOperationDriver
{
/*****  Construction  *****/
public:
    rtREFUV_PartitionedLocateOrAddDriver (
	M_CPD *pTarget, unsigned int const *pTargetPartition
    );
};

/******************
 *  Construction  *
 ******************/

rtREFUV_PartitionedLocateOrAddDriver::rtREFUV_PartitionedLocateOrAddDriver (
    M_CPD *pTarget, unsigned int const *pTargetPartition
): rtREFUV_PartitionedOperationDriver (
    pTarget, pTargetPartition
)
{
}


/**********************************************************************************
 *****  Non-Scalar Partitioned Locate Or Add Operation Driver Abstract Class  *****
 **********************************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_NonScalarPartitionedLocateOrAddDriver : public rtREFUV_PartitionedLocateOrAddDriver {
/*****  Construction  *****/
protected:
    rtREFUV_NonScalarPartitionedLocateOrAddDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	VAssociativeResult	&rAssociativeResult
    );

/*****  State  *****/
protected:
    rtLINK_CType * const	m_pSourcePartition;
    VfLocateOrAddGenerator	m_iLocateOrAddGenerator;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_NonScalarPartitionedLocateOrAddDriver::rtREFUV_NonScalarPartitionedLocateOrAddDriver (
    M_CPD		*pTarget,
    unsigned int const	*pTargetPartition,
    rtLINK_CType	*pSourcePartition,
    VAssociativeResult	&rAssociativeResult
): rtREFUV_PartitionedLocateOrAddDriver (
    pTarget, pTargetPartition
),m_pSourcePartition (
    pSourcePartition
),m_iLocateOrAddGenerator (
    rAssociativeResult
)
{
    pSourcePartition->Align ();
}


/************************************************************************
 *****  Multi-Key Partitioned Locate Or Add Operation Driver Class  *****
 ************************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_MultiKeyPartitionedLocateOrAddDriver
: public rtREFUV_NonScalarPartitionedLocateOrAddDriver
{
/*****  Construction  *****/
public:
    rtREFUV_MultiKeyPartitionedLocateOrAddDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	M_CPD			*pSource,
	VAssociativeResult	&rAssociativeResult,
	M_CPD			*&pTargetReferenceDistribution
    );

/*****  Operation  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    M_CPD *const		m_pSource;
    unsigned int const*		m_pSourceArray;

    VfDistributionGenerator	m_iDistributionGenerator;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_MultiKeyPartitionedLocateOrAddDriver::rtREFUV_MultiKeyPartitionedLocateOrAddDriver (
    M_CPD		*pTarget,
    unsigned int const	*pTargetPartition,
    rtLINK_CType	*pSourcePartition,
    M_CPD		*pSource,
    VAssociativeResult	&rAssociativeResult,
    M_CPD		*&pTargetReferenceDistribution
): rtREFUV_NonScalarPartitionedLocateOrAddDriver (
    pTarget, pTargetPartition, pSourcePartition, rAssociativeResult
),m_pSource (
    pSource
),m_iDistributionGenerator (
    pTargetReferenceDistribution
)
{
    rtREFUV_Align (m_pSource);
    m_pSourceArray = (unsigned int const*)rtREFUV_CPD_Array (m_pSource);
}


/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_MultiKeyPartitionedLocateOrAddDriver::handleTraversalNil (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int Unused(xComainRegion)
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );
    m_iLocateOrAddGenerator.emitReference (xDomainRegion, sDomainRegion);
}

inline void rtREFUV_MultiKeyPartitionedLocateOrAddDriver::handleTraversalRange (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitMonotone (xDomainRegion, sDomainRegion);

//  Since monotonic distributions do not reorder the source elements to which they
//  apply, the distribution can be ignored for the remainder of this routine...
    while (sDomainRegion-- > 0) {
	setTargetRegionToPartition (xComainRegion++);

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    m_pSourceArray[xDomainRegion], fTargetComainElementNotFound
	);
	m_iLocateOrAddGenerator.emitReference (
	    xDomainRegion, 1, xTargetRegionLowerBound (), fTargetComainElementNotFound
	);

	xDomainRegion++;
    }
}


inline void rtREFUV_MultiKeyPartitionedLocateOrAddDriver::handleTraversalRepetition (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );

    setTargetRegionToPartition (xComainRegion);

    while (sDomainRegion-- > 0)
    {
	unsigned int xSourceComainElement = m_pSourceArray[
	    m_iDistributionGenerator.at (xDomainRegion)
	];
	unsigned int sSourceComainElementRun = 1;
	while (
	    sDomainRegion > 0 && xSourceComainElement == m_pSourceArray[
		m_iDistributionGenerator.at (xDomainRegion + sSourceComainElementRun)
	    ]
	)
	{
	    sDomainRegion--;
	    sSourceComainElementRun++;
	}

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    xSourceComainElement, fTargetComainElementNotFound
	);
	m_iLocateOrAddGenerator.emitReference (
	    xDomainRegion,
	    sSourceComainElementRun,
	    xTargetRegionLowerBound (),
	    fTargetComainElementNotFound
	);

	xDomainRegion += sSourceComainElementRun;
    }
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_MultiKeyPartitionedLocateOrAddDriver::operate ()
{
    m_iDistributionGenerator.initializeUsingComainPToken (
	m_pSourcePartition->PPT ()
    );
    m_iLocateOrAddGenerator.initialize (
	m_pTarget, UV_CPx_PToken, m_iDistributionGenerator.distributionDomainPTokenCPD ()
    );

    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );

    m_iLocateOrAddGenerator.commit ();
    if (m_iLocateOrAddGenerator.additionsGenerated ()) {
	rtLINK_CType *pTargetDomainAdditions = m_iLocateOrAddGenerator.targetReference (
	)->Extract (m_iLocateOrAddGenerator.sourceReference ());
	M_CPD *pSourceComainAdditions; {
	    M_CPD *pSourceDomainAdditions = rtREFUV_LCExtract (
		m_iDistributionGenerator.distributionCPD (),
		m_iLocateOrAddGenerator.sourceReference ()
	    );
	    pSourceComainAdditions = rtREFUV_UVExtract (m_pSource, pSourceDomainAdditions);
	    pSourceDomainAdditions->release ();
	}
	rtREFUV_LCAssign (m_pTarget, pTargetDomainAdditions, pSourceComainAdditions);
	pTargetDomainAdditions->release ();
	pSourceComainAdditions->release ();
    }
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned LocateOrAdd
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  source elements to their partitions.
 *	pSource			- a pointer to a CPD for the source.
 *	pTargetReference	- the return address for the address of a link
 *				  constructor identifying the target elements
 *				  found or added by this routine.
 *	pTargetReferenceAdditions
 *				- the return address for the address of a link
 *				  constructor identifying the source elements
 *				  added to the target by this routine.  Returned
 *				  as Nil if no elements were added.
 *	pTargetReferenceDistribution
 *				- the return address for the address of a CPD for
 *				  distribution that maps source elements in
 *				  'locations' elements.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedLocateOrAdd (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    M_CPD			*pSource,
    rtLINK_CType		*&pTargetReference,
    rtLINK_CType		*&pTargetReferenceAdditions,
    M_CPD			*&pTargetReferenceDistribution
)
{
    VAssociativeResult iAssociativeResult;
    rtREFUV_MultiKeyPartitionedLocateOrAddDriver operation (
	pTarget,
	pTargetPartition,
	pSourcePartition,
	pSource,
	iAssociativeResult,
	pTargetReferenceDistribution
    );
    operation.operate ();
    pTargetReference		= iAssociativeResult.claimedTargetReference ();
    pTargetReferenceAdditions	= iAssociativeResult.claimedSourceReference ();
}


/**************************************************************************
 *****  Coerced-Key Partitioned Locate Or Add Operation Driver Class  *****
 **************************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_CoercedKeyPartitionedLocateOrAddDriver
: public rtREFUV_NonScalarPartitionedLocateOrAddDriver
{
/*****  Construction  *****/
public:
    rtREFUV_CoercedKeyPartitionedLocateOrAddDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	rtREFUV_Type_Reference	&pSource,
	VAssociativeResult	&rAssociativeResult
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    rtREFUV_Type_Reference&	m_pSource;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_CoercedKeyPartitionedLocateOrAddDriver::rtREFUV_CoercedKeyPartitionedLocateOrAddDriver (
    M_CPD			*pTarget,
    unsigned int const		*	pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    VAssociativeResult		&rAssociativeResult
): rtREFUV_NonScalarPartitionedLocateOrAddDriver (
    pTarget, pTargetPartition, pSourcePartition, rAssociativeResult
),m_pSource (
    pSource
)
{
    rtREFUV_AlignReference (&m_pSource);
}


/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_CoercedKeyPartitionedLocateOrAddDriver::handleTraversalNil (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int Unused(xComainRegion)
) {
    m_iLocateOrAddGenerator.emitReference (xDomainRegion, sDomainRegion);
}

inline void rtREFUV_CoercedKeyPartitionedLocateOrAddDriver::handleTraversalRange (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
) {
    while (sDomainRegion-- > 0) {
	setTargetRegionToPartition (xComainRegion++);

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    (unsigned int)DSC_Scalar_Int (m_pSource), fTargetComainElementNotFound
	);
	m_iLocateOrAddGenerator.emitReference (
	    xDomainRegion, 1, xTargetRegionLowerBound (), fTargetComainElementNotFound
	);

	xDomainRegion++;
    }
}

inline void rtREFUV_CoercedKeyPartitionedLocateOrAddDriver::handleTraversalRepetition (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    setTargetRegionToPartition (xComainRegion);

    bool fTargetComainElementNotFound;
    setTargetRegionLowerBoundToComainElement (
	(unsigned int)DSC_Scalar_Int (m_pSource), fTargetComainElementNotFound
    );
    m_iLocateOrAddGenerator.emitReference (
	xDomainRegion,
	sDomainRegion,
	xTargetRegionLowerBound (),
	fTargetComainElementNotFound
    );
}


/***************
 *  Execution  *
 ***************/

void rtREFUV_CoercedKeyPartitionedLocateOrAddDriver::operate () {
    m_iLocateOrAddGenerator.initialize (
	m_pTarget, UV_CPx_PToken, m_pSourcePartition->PPT ()
    );

    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );

    m_iLocateOrAddGenerator.commit ();
    if (m_iLocateOrAddGenerator.additionsGenerated ()) rtREFUV_LCAssignScalar (
	m_pTarget, m_iLocateOrAddGenerator.targetReference (), &m_pSource
    );
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned LocateOrAdd
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  the source element to its partitions.
 *	pSource			- a pointer to a reference to the source element.
 *	pTargetReference	- the return address for the address of a link
 *				  constructor identifying the target elements
 *				  found or added by this routine.
 *	pTargetReferenceAdditions
 *				- the return address for the address of the link
 *				  constructor identifying the source elements
 *				  added to the target by this routine.  Returned
 *				  as Nil if no elements were added.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedLocateOrAdd (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtLINK_CType		*&pTargetReference,
    rtLINK_CType		*&pTargetReferenceAdditions
)
{
    VAssociativeResult iAssociativeResult;
    rtREFUV_CoercedKeyPartitionedLocateOrAddDriver operation (
	pTarget,
	pTargetPartition,
	pSourcePartition,
	pSource,
	iAssociativeResult
    );

    operation.operate ();

    pTargetReference		= iAssociativeResult.claimedTargetReference ();
    pTargetReferenceAdditions	= iAssociativeResult.claimedSourceReference ();
}


/*********************************************************************
 *****  Scalar Partitioned Locate Or Add Operation Driver Class  *****
 *********************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_ScalarPartitionedLocateOrAddDriver : public rtREFUV_PartitionedLocateOrAddDriver
{
/*****  Construction  *****/
public:
    rtREFUV_ScalarPartitionedLocateOrAddDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtREFUV_Type_Reference	&pSourcePartition,
	rtREFUV_Type_Reference	&pSource,
	rtREFUV_Type_Reference	&pTargetReference,
	bool		&fTargetReferenceNotFound
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  State  *****/
    rtREFUV_Type_Reference	&m_pSourcePartition;
    rtREFUV_Type_Reference	&m_pSource;
    rtREFUV_Type_Reference	&m_pTargetReference;
    bool			&m_fTargetReferenceNotFound;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_ScalarPartitionedLocateOrAddDriver::rtREFUV_ScalarPartitionedLocateOrAddDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtREFUV_Type_Reference	&pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtREFUV_Type_Reference	&pTargetReference,
    bool			&fTargetReferenceNotFound
): rtREFUV_PartitionedLocateOrAddDriver (
    pTarget, pTargetPartition
),m_pSourcePartition (
    pSourcePartition
),m_pSource (
    pSource
),m_pTargetReference (
    pTargetReference
),m_fTargetReferenceNotFound (
    fTargetReferenceNotFound
)
{
    rtREFUV_AlignReference (&m_pSourcePartition);
    rtREFUV_AlignReference (&m_pSource);
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_ScalarPartitionedLocateOrAddDriver::operate () {
    M_CPD *pTargetDomainPToken = UV_CPD_PosPTokenCPD (m_pTarget);

    if (m_pSourcePartition.holdsANil ()) {
	DSC_InitReferenceScalar (
	    m_pTargetReference,
	    pTargetDomainPToken,
	    rtPTOKEN_CPD_BaseElementCount (pTargetDomainPToken)
	);
	m_fTargetReferenceNotFound = false;
	return;
    }

    setTargetRegionToPartition ((unsigned int)DSC_Scalar_Int (m_pSourcePartition));

    setTargetRegionLowerBoundToComainElement (
	(unsigned int)DSC_Scalar_Int (m_pSource), m_fTargetReferenceNotFound
    );

    DSC_InitReferenceScalar (
	m_pTargetReference,
	m_fTargetReferenceNotFound ? rtPTOKEN_NewShiftPTConstructor (
	    pTargetDomainPToken, -1
	)->AppendAdjustment (
	    xTargetRegionLowerBound (), 1
	)->ToPToken () : M_DuplicateCPDPtr (pTargetDomainPToken),
	xTargetRegionLowerBound ()
    );

    if (m_fTargetReferenceNotFound) rtREFUV_RFAssign (
	m_pTarget, &m_pTargetReference, &m_pSource
    );

    pTargetDomainPToken->release ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned LocateOrAdd
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a reference that assigns the source element to
 *				  its partition.
 *	pSource			- a pointer to a reference to the source element.
 *	pTargetReference	- the address of a reference that will be initialized
 *				  to identify the target element found or added by
 *				  this routine.
 *	fTargetReferenceNotFound
 *				- the address of a integer that will be set to
 *				  true if the source was added to the target by
 *				  this routine.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedLocateOrAdd (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtREFUV_Type_Reference	&pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtREFUV_Type_Reference	&pTargetReference,
    bool			&fTargetReferenceNotFound
)
{
    rtREFUV_ScalarPartitionedLocateOrAddDriver operation (
	pTarget,
	pTargetPartition,
	pSourcePartition,
	pSource,
	pTargetReference,
	fTargetReferenceNotFound
    );
    operation.operate ();
}


/****************************************************************
 *****  Partitioned Locate Operation Driver Abstract Class  *****
 ****************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_PartitionedLocateDriver : public rtREFUV_PartitionedOperationDriver {
/*****  Construction  *****/
protected:
    rtREFUV_PartitionedLocateDriver (
	M_CPD *pTarget, unsigned int const *pTargetPartition, rtLINK_LookupCase xLookupCase
    );

/*****  State  *****/
protected:
    rtLINK_LookupCase const m_xLookupCase;
};

/******************
 *  Construction  *
 ******************/

rtREFUV_PartitionedLocateDriver::rtREFUV_PartitionedLocateDriver (
    M_CPD *pTarget, unsigned int const *pTargetPartition, rtLINK_LookupCase xLookupCase
)
: rtREFUV_PartitionedOperationDriver (pTarget, pTargetPartition)
, m_xLookupCase (xLookupCase)
{
}


/***************************************************************************
 *****  Non-Scalar Partitioned Locate Operation Driver Abstract Class  *****
 ***************************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_NonScalarPartitionedLocateDriver : public rtREFUV_PartitionedLocateDriver {
/*****  Construction  *****/
protected:
    rtREFUV_NonScalarPartitionedLocateDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	rtLINK_LookupCase	xLookupCase,
	VAssociativeResult	&rAssociativeResult
    );

/*****  State  *****/
protected:
    rtLINK_CType * const	m_pSourcePartition;

    VfLocateGenerator		m_iLocateGenerator;
};

/******************
 *  Construction  *
 ******************/

rtREFUV_NonScalarPartitionedLocateDriver::rtREFUV_NonScalarPartitionedLocateDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtLINK_LookupCase		xLookupCase,
    VAssociativeResult		&rAssociativeResult
): rtREFUV_PartitionedLocateDriver (
    pTarget, pTargetPartition, xLookupCase
),m_pSourcePartition (
    pSourcePartition
),m_iLocateGenerator (
    rAssociativeResult
)
{
    pSourcePartition->Align ();
}


/*****************************************************************
 *****  Multi-Key Partitioned Locate Operation Driver Class  *****
 *****************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_MultiKeyPartitionedLocateDriver
: public rtREFUV_NonScalarPartitionedLocateDriver
{
/*****  Construction  *****/
public:
    rtREFUV_MultiKeyPartitionedLocateDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	M_CPD			*pSource,
	rtLINK_LookupCase	xLookupCase,
	VAssociativeResult	&rAssociativeResult,
	M_CPD			*&pTargetReferenceDistribution
    );

/*****  Operation  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    M_CPD *const		m_pSource;
    unsigned int const*		m_pSourceArray;

    VfDistributionGenerator	m_iDistributionGenerator;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_MultiKeyPartitionedLocateDriver::rtREFUV_MultiKeyPartitionedLocateDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    M_CPD			*pSource,
    rtLINK_LookupCase		xLookupCase,
    VAssociativeResult		&rAssociativeResult,
    M_CPD			*&pTargetReferenceDistribution
): rtREFUV_NonScalarPartitionedLocateDriver (
    pTarget,
    pTargetPartition,
    pSourcePartition,
    xLookupCase,
    rAssociativeResult
),m_pSource (
    pSource
),m_iDistributionGenerator (
    pTargetReferenceDistribution
)
{
    rtREFUV_Align (m_pSource);
    m_pSourceArray = (unsigned int const*)rtREFUV_CPD_Array (m_pSource);
}


/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_MultiKeyPartitionedLocateDriver::handleTraversalNil (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int Unused(xComainRegion)
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );
}

inline void rtREFUV_MultiKeyPartitionedLocateDriver::handleTraversalRange (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitMonotone (xDomainRegion, sDomainRegion);

//  Since monotonic distributions do not reorder the source elements to which they
//  apply, the distribution can be ignored for the remainder of this routine...
    while (sDomainRegion-- > 0)
    {
	setTargetRegionToPartition (xComainRegion++);

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    m_xLookupCase, m_pSourceArray[xDomainRegion], fTargetComainElementNotFound
	);
	m_iLocateGenerator.emitReference (
	    xDomainRegion, 1, xTargetRegionLowerBound (), fTargetComainElementNotFound
	);

	xDomainRegion++;
    }
}


inline void rtREFUV_MultiKeyPartitionedLocateDriver::handleTraversalRepetition (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );

    setTargetRegionToPartition (xComainRegion);

    while (sDomainRegion-- > 0)
    {
	unsigned int xSourceComainElement = m_pSourceArray[
	    m_iDistributionGenerator.at (xDomainRegion)
	];
	unsigned int sSourceComainElementRun = 1;
	while (
	    sDomainRegion > 0 && xSourceComainElement == m_pSourceArray[
		m_iDistributionGenerator.at (xDomainRegion + sSourceComainElementRun)
	    ]
	)
	{
	    sDomainRegion--;
	    sSourceComainElementRun++;
	}

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    m_xLookupCase, xSourceComainElement, fTargetComainElementNotFound
	);
	m_iLocateGenerator.emitReference (
	    xDomainRegion,
	    sSourceComainElementRun,
	    xTargetRegionLowerBound (),
	    fTargetComainElementNotFound
	);

	xDomainRegion += sSourceComainElementRun;
    }
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_MultiKeyPartitionedLocateDriver::operate () {
    m_iDistributionGenerator.initializeUsingComainPToken (
	m_pSourcePartition->PPT ()
    );
    m_iLocateGenerator.initialize (
	m_pTarget, UV_CPx_PToken, m_iDistributionGenerator.distributionDomainPTokenCPD ()
    );

    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );

    m_iLocateGenerator.commit ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Locate
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  source elements to their partitions.
 *	pSource			- a pointer to a CPD for the source.
 *	xLookupCase		- the comparison lookup case.
 *	pTargetReference	- the return address for a pointer to a link
 *				  constructor identifying the target elements
 *				  found by this routine.
 *	pTargetReferenceGuard	- the return address for the address of the link
 *				  constructor identifying the source elements
 *				  found by this routine.  Returned as Nil if all
 *				  elements were found.
 *	pTargetReferenceDistribution
 *				- the return address for the address of a CPD for
 *				  distribution that maps source elements into
 *				  'locations' elements.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedLocate (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    M_CPD			*pSource,
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*&pTargetReference,
    rtLINK_CType		*&pTargetReferenceGuard,
    M_CPD			*&pTargetReferenceDistribution
)
{
    VAssociativeResult iAssociativeResult;
    rtREFUV_MultiKeyPartitionedLocateDriver operation (
	pTarget,
	pTargetPartition,
	pSourcePartition,
	pSource,
	xLookupCase,
	iAssociativeResult,
	pTargetReferenceDistribution
    );

    operation.operate ();

    pTargetReference		= iAssociativeResult.claimedTargetReference ();
    pTargetReferenceGuard	= iAssociativeResult.claimedSourceReference ();
}


/*******************************************************************
 *****  Coerced-Key Partitioned Locate Operation Driver Class  *****
 *******************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_CoercedKeyPartitionedLocateDriver
: public rtREFUV_NonScalarPartitionedLocateDriver
{
/*****  Construction  *****/
public:
    rtREFUV_CoercedKeyPartitionedLocateDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	rtREFUV_Type_Reference	&pSource,
	rtLINK_LookupCase	xLookupCase,
	VAssociativeResult	&rAssociativeResult
    );

/*****  Opeation  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    rtREFUV_Type_Reference&	 m_pSource;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_CoercedKeyPartitionedLocateDriver::rtREFUV_CoercedKeyPartitionedLocateDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtLINK_LookupCase		xLookupCase,
    VAssociativeResult		&rAssociativeResult
): rtREFUV_NonScalarPartitionedLocateDriver (
    pTarget, pTargetPartition, pSourcePartition, xLookupCase, rAssociativeResult
),m_pSource (
    pSource
)
{
    rtREFUV_AlignReference (&m_pSource);
}


/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_CoercedKeyPartitionedLocateDriver::handleTraversalNil (
    unsigned int Unused(xDomainRegion), unsigned int Unused(sDomainRegion), unsigned int Unused(xComainRegion)
)
{
}

inline void rtREFUV_CoercedKeyPartitionedLocateDriver::handleTraversalRange (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    while (sDomainRegion-- > 0)
    {
	setTargetRegionToPartition (xComainRegion++);

	bool fTargetComainElementNotFound;
	setTargetRegionLowerBoundToComainElement (
	    m_xLookupCase,
	    (unsigned int)DSC_Scalar_Int (m_pSource),
	    fTargetComainElementNotFound
	);
	m_iLocateGenerator.emitReference (
	    xDomainRegion, 1, xTargetRegionLowerBound (), fTargetComainElementNotFound
	);

	xDomainRegion++;
    }
}

inline void rtREFUV_CoercedKeyPartitionedLocateDriver::handleTraversalRepetition (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    setTargetRegionToPartition (xComainRegion);
    bool fTargetComainElementNotFound;
    setTargetRegionLowerBoundToComainElement (
	m_xLookupCase, (unsigned int)DSC_Scalar_Int (m_pSource), fTargetComainElementNotFound
    );
    m_iLocateGenerator.emitReference (
	xDomainRegion, sDomainRegion, xTargetRegionLowerBound (), fTargetComainElementNotFound
    );
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_CoercedKeyPartitionedLocateDriver::operate () {
    m_iLocateGenerator.initialize (
	m_pTarget, UV_CPx_PToken, m_pSourcePartition->PPT ()
    );

    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );

    m_iLocateGenerator.commit ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Locate
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  the source element to its partitions.
 *	pSource			- a pointer to a reference to the source element.
 *	xLookupCase		- the comparison lookup case.
 *	pTargetReference	- the return address for a pointer to a link
 *				  constructor identifying the target elements
 *				  found by this routine.
 *	pTargetReferenceGuard	- the return address for the address of the link
 *				  constructor identifying the source elements
 *				  found by this routine.  Returned as Nil if all
 *				  elements were found.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedLocate (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*&pTargetReference,
    rtLINK_CType		*&pTargetReferenceGuard
)
{
    VAssociativeResult iAssociativeResult;
    rtREFUV_CoercedKeyPartitionedLocateDriver operation (
	pTarget, pTargetPartition, pSourcePartition, pSource, xLookupCase, iAssociativeResult
    );
    operation.operate ();
    pTargetReference		= iAssociativeResult.claimedTargetReference ();
    pTargetReferenceGuard	= iAssociativeResult.claimedSourceReference ();
}


/**************************************************************
 *****  Scalar Partitioned Locate Operation Driver Class  *****
 **************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_ScalarPartitionedLocateDriver : public rtREFUV_PartitionedLocateDriver
{
/*****  Construction  *****/
public:
    rtREFUV_ScalarPartitionedLocateDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtREFUV_Type_Reference	&pSourcePartition,
	rtREFUV_Type_Reference	&pSource,
	rtLINK_LookupCase	 xLookupCase,
	rtREFUV_Type_Reference	&pTargetReference,
	bool		&fTargetReferenceIsValid
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  State  *****/
protected:
    rtREFUV_Type_Reference	&m_pSourcePartition;
    rtREFUV_Type_Reference	&m_pSource;
    rtREFUV_Type_Reference	&m_pTargetReference;
    bool			&m_fTargetReferenceIsValid;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_ScalarPartitionedLocateDriver::rtREFUV_ScalarPartitionedLocateDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtREFUV_Type_Reference	&pSourcePartition,
    rtREFUV_Type_Reference	&pSource,
    rtLINK_LookupCase		 xLookupCase,
    rtREFUV_Type_Reference	&pTargetReference,
    bool			&fTargetReferenceIsValid
): rtREFUV_PartitionedLocateDriver (
    pTarget, pTargetPartition, xLookupCase
),m_pSourcePartition (
    pSourcePartition
),m_pSource (
    pSource
),m_pTargetReference (
    pTargetReference
),m_fTargetReferenceIsValid (
    fTargetReferenceIsValid
)
{
    rtREFUV_AlignReference (&m_pSourcePartition);
    rtREFUV_AlignReference (&m_pSource);
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_ScalarPartitionedLocateDriver::operate () {
    if (m_pSourcePartition.holdsANil ()) {
	m_fTargetReferenceIsValid = false;
	return;
    }

    setTargetRegionToPartition ((unsigned int)DSC_Scalar_Int (m_pSourcePartition));

    bool fTargetReferenceNotFound;
    setTargetRegionLowerBoundToComainElement (
	m_xLookupCase, (unsigned int)DSC_Scalar_Int (m_pSource), fTargetReferenceNotFound
    );
    
    if (fTargetReferenceNotFound)
	m_fTargetReferenceIsValid = false;
    else {
	DSC_InitReferenceScalar (
	    m_pTargetReference,
	    UV_CPD_PosPTokenCPD (m_pTarget),
	    xTargetRegionLowerBound ()
	);
	m_fTargetReferenceIsValid = true;
    }
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Locate
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a reference that assigns the source element to
 *				  its partition.
 *	pSource			- a pointer to a reference to the source element.
 *	xLookupCase		- the comparison lookup case.
 *	pTargetReference	- the address of a reference that will be initialized
 *				  to identify the target element found by this routine.
 *
 *  Returns:
 *	true if the source element was found and pTargetReference identifies it,
 *	false otherwise.
 *
 *****/
PublicFnDef bool rtREFUV_PartitionedLocate (
    M_CPD *			pTarget,
    unsigned int*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource,
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	pTargetReference
)
{
    bool fTargetReferenceIsValid;
    rtREFUV_ScalarPartitionedLocateDriver operation (
	pTarget,
	pTargetPartition,
	pSourcePartition,
	pSource,
	xLookupCase,
	pTargetReference,
	fTargetReferenceIsValid
    );

    operation.operate ();

    return fTargetReferenceIsValid;
}


/****************************************************************
 *****  Partitioned Delete Operation Driver Abstract Class  *****
 ****************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_PartitionedDeleteDriver : public rtREFUV_PartitionedOperationDriver
{
/*****  Construction  *****/
public:
    rtREFUV_PartitionedDeleteDriver (
	M_CPD *pTarget, unsigned int const *pTargetPartition
    );

/*****  Destruction  *****/
public:
    ~rtREFUV_PartitionedDeleteDriver ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int Unused(xDomainRegion), unsigned int Unused(sDomainRegion), unsigned int Unused(xComainRegion)
    )
    {
    }

/*****  Update  *****/
protected:
    inline void deleteComainElement (unsigned int xComainElement);
    void commit ();

/*****  State  *****/
    rtPTOKEN_CType *		m_pTargetTrajectory;
    unsigned int		m_sTargetAdjustment;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_PartitionedDeleteDriver::rtREFUV_PartitionedDeleteDriver (
    M_CPD *pTarget, unsigned int const *pTargetPartition
): rtREFUV_PartitionedOperationDriver (
    pTarget, pTargetPartition
)
{
    m_pTargetTrajectory = NilOf (rtPTOKEN_CType *);
    m_sTargetAdjustment = 0;
}

/*****************
 *  Destruction  *
 *****************/

rtREFUV_PartitionedDeleteDriver::~rtREFUV_PartitionedDeleteDriver () {
    if (m_pTargetTrajectory)
	m_pTargetTrajectory->discard ();
}


/************
 *  Update  *
 ************/

inline void rtREFUV_PartitionedDeleteDriver::deleteComainElement (
    unsigned int xComainElement
) {
    bool fComainElementNotFound = true;
    setTargetRegionLowerBoundToComainElement (xComainElement, fComainElementNotFound);

    if (fComainElementNotFound)
	return;

    if (IsNil (m_pTargetTrajectory)) m_pTargetTrajectory = rtPTOKEN_NewShiftPTConstructor (
	m_pTarget, UV_CPx_PToken
    );

    m_pTargetTrajectory->AppendAdjustment (
	(int)(1 + xTargetRegionLowerBound () - m_sTargetAdjustment), -1
    );
    m_sTargetAdjustment++;
}

void rtREFUV_PartitionedDeleteDriver::commit () {
    if (m_pTargetTrajectory) {
	m_pTargetTrajectory->ToPToken ()->release ();
	m_pTargetTrajectory = NilOf (rtPTOKEN_CType *);

	rtREFUV_Align (m_pTarget);
    }
}


/***************************************************************************
 *****  Non-Scalar Partitioned Delete Operation Driver Abstract Class  *****
 ***************************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_NonScalarPartitionedDeleteDriver : public rtREFUV_PartitionedDeleteDriver
{
/*****  Construction  *****/
public:
    rtREFUV_NonScalarPartitionedDeleteDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition
    );

/*****  State  *****/
protected:
    rtLINK_CType * const	m_pSourcePartition;
};

/******************
 *  Construction  *
 ******************/

rtREFUV_NonScalarPartitionedDeleteDriver::rtREFUV_NonScalarPartitionedDeleteDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition
): rtREFUV_PartitionedDeleteDriver (
    pTarget, pTargetPartition
),m_pSourcePartition (
    pSourcePartition
)
{
    m_pSourcePartition->Align ();
}


/*****************************************************************
 *****  Multi-Key Partitioned Delete Operation Driver Class  *****
 *****************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_MultiKeyPartitionedDeleteDriver : public rtREFUV_NonScalarPartitionedDeleteDriver
{
/*****  Construction  *****/
public:
    rtREFUV_MultiKeyPartitionedDeleteDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	M_CPD			*pSource,
	M_CPD			*&pTargetReferenceDistribution
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalNil (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    M_CPD *const		m_pSource;
    unsigned int const*		m_pSourceArray;

    VfDistributionGenerator	m_iDistributionGenerator;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_MultiKeyPartitionedDeleteDriver::rtREFUV_MultiKeyPartitionedDeleteDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    M_CPD			*pSource,
    M_CPD			*&pTargetReferenceDistribution
): rtREFUV_NonScalarPartitionedDeleteDriver (
    pTarget, pTargetPartition, pSourcePartition
),m_pSource (
    pSource
),m_iDistributionGenerator (
    pTargetReferenceDistribution
)
{
    rtREFUV_Align (m_pSource);
    m_pSourceArray = (unsigned int const*)rtREFUV_CPD_Array (m_pSource);
}


/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_MultiKeyPartitionedDeleteDriver::handleTraversalNil (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int Unused(xComainRegion)
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );
}

inline void rtREFUV_MultiKeyPartitionedDeleteDriver::handleTraversalRange (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitMonotone (xDomainRegion, sDomainRegion);

//  Since monotonic distributions do not reorder the source elements to which they
//  apply, the distribution can be ignored for the remainder of this routine...
    while (sDomainRegion-- > 0)
    {
	setTargetRegionToPartition (xComainRegion++);
	deleteComainElement (m_pSourceArray[xDomainRegion++]);
    }
}

inline void rtREFUV_MultiKeyPartitionedDeleteDriver::handleTraversalRepetition (
    unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    m_iDistributionGenerator.emitSort (
	xDomainRegion, sDomainRegion, m_pSourceArray + xDomainRegion
    );

    setTargetRegionToPartition (xComainRegion);

    while (sDomainRegion-- > 0)
    {
	unsigned int xSourceComainElement = m_pSourceArray[
	    m_iDistributionGenerator.at (xDomainRegion)
	];
	unsigned int sSourceComainElementRun = 1;
	while (
	    sDomainRegion > 0 && xSourceComainElement == m_pSourceArray[
		m_iDistributionGenerator.at (xDomainRegion + sSourceComainElementRun)
	    ]
	)
	{
	    sDomainRegion--;
	    sSourceComainElementRun++;
	}
	deleteComainElement (xSourceComainElement);
	xDomainRegion += sSourceComainElementRun;
    }
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_MultiKeyPartitionedDeleteDriver::operate () {
    m_iDistributionGenerator.initializeUsingComainPToken (m_pSourcePartition->PPT ());
    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );
    commit ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Delete
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  source elements to their partitions.
 *	pSource			- a pointer to a CPD for the source.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedDelete (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    M_CPD			*pSource
)
{
    M_CPD *pTargetReferenceDistribution;
    rtREFUV_MultiKeyPartitionedDeleteDriver operation (
	pTarget, pTargetPartition, pSourcePartition, pSource, pTargetReferenceDistribution
    );
    operation.operate ();
    pTargetReferenceDistribution->release ();
}


/*******************************************************************
 *****  Coerced-Key Partitioned Delete Operation Driver Class  *****
 *******************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_CoercedKeyPartitionedDeleteDriver
: public rtREFUV_NonScalarPartitionedDeleteDriver
{
/*****  Construction  *****/
public:
    rtREFUV_CoercedKeyPartitionedDeleteDriver (
	M_CPD			*pTarget,
	unsigned int const	*pTargetPartition,
	rtLINK_CType		*pSourcePartition,
	rtREFUV_Type_Reference	&pSource
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  Traversal  *****/
protected:
    inline void handleTraversalRange (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );
    
    inline void handleTraversalRepetition (
	unsigned int xDomainRegion, unsigned int sDomainRegion, unsigned int xComainRegion
    );

/*****  State  *****/
protected:
    rtREFUV_Type_Reference&	m_pSource;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_CoercedKeyPartitionedDeleteDriver::rtREFUV_CoercedKeyPartitionedDeleteDriver (
    M_CPD			*pTarget,
    unsigned int const		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource
): rtREFUV_NonScalarPartitionedDeleteDriver (
    pTarget, pTargetPartition, pSourcePartition
),m_pSource (
    pSource
)
{
    rtREFUV_AlignReference (&m_pSource);
}

/***************
 *  Traversal  *
 ***************/

inline void rtREFUV_CoercedKeyPartitionedDeleteDriver::handleTraversalRange (
    unsigned int Unused(xDomainRegion), unsigned int sDomainRegion, unsigned int xComainRegion
)
{
    while (sDomainRegion-- > 0)
    {
	setTargetRegionToPartition (xComainRegion++);
	deleteComainElement ((unsigned int)DSC_Scalar_Int (m_pSource));
    }
}

inline void rtREFUV_CoercedKeyPartitionedDeleteDriver::handleTraversalRepetition (
    unsigned int Unused(xDomainRegion), unsigned int Unused(sDomainRegion), unsigned int xComainRegion
)
{
    setTargetRegionToPartition (xComainRegion);
    deleteComainElement ((unsigned int)DSC_Scalar_Int (m_pSource));
}

/***************
 *  Operation  *
 ***************/

void rtREFUV_CoercedKeyPartitionedDeleteDriver::operate () {
    rtLINK_TraverseRRDCList (
	m_pSourcePartition, handleTraversalNil, handleTraversalRepetition, handleTraversalRange
    );
    commit ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Delete
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a pointer to a link constructor that assigns
 *				  the source element to its partitions.
 *	pSource			- a reference to the source element.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedDelete (
    M_CPD			*pTarget,
    unsigned int		*pTargetPartition,
    rtLINK_CType		*pSourcePartition,
    rtREFUV_Type_Reference	&pSource
)
{
    rtREFUV_CoercedKeyPartitionedDeleteDriver operation (
	pTarget, pTargetPartition, pSourcePartition, pSource
    );
    operation.operate ();
}


/**************************************************************
 *****  Scalar Partitioned Delete Operation Driver Class  *****
 **************************************************************/

/**********************
 *  Class Definition  *
 **********************/

class rtREFUV_ScalarPartitionedDeleteDriver : public rtREFUV_PartitionedDeleteDriver
{
/*****  Construction  *****/
public:
    rtREFUV_ScalarPartitionedDeleteDriver (
	M_CPD *			pTarget,
	unsigned int const*	pTargetPartition,
	rtREFUV_Type_Reference&	pSourcePartition,
	rtREFUV_Type_Reference&	pSource
    );

/*****  Execution  *****/
public:
    void operate ();

/*****  State  *****/
protected:
    rtREFUV_Type_Reference&	m_pSourcePartition;
    rtREFUV_Type_Reference&	m_pSource;
};


/******************
 *  Construction  *
 ******************/

rtREFUV_ScalarPartitionedDeleteDriver::rtREFUV_ScalarPartitionedDeleteDriver (
    M_CPD *			pTarget,
    unsigned int const*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource
): rtREFUV_PartitionedDeleteDriver (
    pTarget, pTargetPartition
),m_pSourcePartition (
    pSourcePartition
),m_pSource (
    pSource
)
{
    rtREFUV_AlignReference (&m_pSourcePartition);
    rtREFUV_AlignReference (&m_pSource);
}


/***************
 *  Operation  *
 ***************/

void rtREFUV_ScalarPartitionedDeleteDriver::operate () {
    if (m_pSourcePartition.holdsANil ())
	return;

    setTargetRegionToPartition ((unsigned int)DSC_Scalar_Int (m_pSourcePartition));
    deleteComainElement ((unsigned int)DSC_Scalar_Int (m_pSource));
    commit ();
}


/*****************************
 *  External Implementation  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Partitioned Delete
 *
 *  Arguments:
 *	pTarget			- a pointer to a CPD for the target.
 *	pTargetPartition	- a pointer to a breakpoint array that assigns
 *				  target elements to their partitions.
 *	pSourcePartition	- a reference that assigns the source element to
 *				  its partition.
 *	pSource			- a reference to the source element.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtREFUV_PartitionedDelete (
    M_CPD *			pTarget,
    unsigned int*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource
)
{
    rtREFUV_ScalarPartitionedDeleteDriver operation (
	pTarget, pTargetPartition, pSourcePartition, pSource
    );
    operation.operate ();
}


/*******************************************************************
 *******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************
 *******************************************************************/

/********************
 *  Element Printer *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to print a representation of an 'RTrefuv' element.
 *
 *  Arguments:
 *	cpd			- a standard CPD positioned at the element
 *				  to be displayed.
 *
 *  Returns:
 *	The number of characters displayed.
 *
 *****/
PrivateFnDef int PrintElement (M_CPD *cpd) {
    return IO_printf ("%d", *rtREFUV_CPD_Element (cpd));
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	rtREFUV_New (
	    RTYPE_QRegisterCPD (parameterArray[0]), RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}

IOBJ_DefineNewaryMethod (NewDistributeDM) {
    return RTYPE_QRegister (
	rtREFUV_Distribute (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}

/*****  Hook to the last 'asLink' debug method sort indices  *****/
PrivateVarDef M_CPD *LastAsLinkSortIndicesCPD = NilOf (M_CPD*);

IOBJ_DefineNilaryMethod (LastAsLinkSortIndicesDM) {
    return IsNil (LastAsLinkSortIndicesCPD) ? IOBJ_TheNilIObject : RTYPE_Browser (
	LastAsLinkSortIndicesCPD, -1
    );
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nReference U-Vector Routines Usage Counts\n");
    IO_printf (  "-----------------------------------------\n");

    IO_printf ("NewCount		    = %d\n",  NewCount);
    IO_printf ("\n");

    IO_printf ("AlignReferenceCount	    = %d\n",  AlignReferenceCount);
    IO_printf ("AlignCount		    = %d\n",  AlignCount);
    IO_printf ("RefAlignCount		    = %d\n",  RefAlignCount);
    IO_printf ("RefAlignEndOnlyCount	    = %d\n",  RefAlignEndOnlyCount);
    IO_printf ("RefAlignRadixSortCount	    = %d\n",  RefAlignRadixSortCount);
    IO_printf ("RefAlignCodomainArrayCount  = %d\n",  RefAlignCodomainArrayCount);
    IO_printf ("\n");

    IO_printf ("LCExtractCount		    = %d\n",  LCExtractCount);
    IO_printf ("UVExtractCount		    = %d\n",  UVExtractCount);
    IO_printf ("RFExtractCount		    = %d\n",  RFExtractCount);
    IO_printf ("\n");

    IO_printf ("LCAssignCount		    = %d\n",  LCAssignCount);
    IO_printf ("UVAssignCount		    = %d\n",  UVAssignCount);
    IO_printf ("RFAssignCount		    = %d\n",  RFAssignCount);
    IO_printf ("LCAssignScalarCount	    = %d\n",  LCAssignScalarCount);
    IO_printf ("UVAssignScalarCount	    = %d\n",  UVAssignScalarCount);
    IO_printf ("\n");

    IO_printf ("AscendingSortIndicesCount   = %d\n",  AscendingSortIndicesCount);
    IO_printf ("FlipCount		    = %d\n",  FlipCount);
    IO_printf ("LinearizeRrRcCount	    = %d\n",  LinearizeRrRcCount);
    IO_printf ("\n");

    IO_printf ("LocateOrAddCount	    = %d\n",  LocateOrAddCount);
    IO_printf ("ScalarLocateOrAddCount	    = %d\n",  ScalarLocateOrAddCount);
    IO_printf ("LookupCount		    = %d\n",  LookupCount);
    IO_printf ("ScalarLookupCount	    = %d\n",  ScalarLookupCount);
    IO_printf ("\n");

    IO_printf ("CodomainDomainCriticalRatio   = %g\n",  CodomainDomainCriticalRatio);
    IO_printf ("\n");

    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

UV_DefineEPrintDM (PrintElementDM, PrintElement)

UV_DefineAlignDM (AlignDM, rtREFUV_Align)

UV_DefineAtDM (AtDM, rtREFUV_LCExtract, rtREFUV_UVExtract)

UV_DefineAtPutDM (AtPutDM, rtREFUV_LCAssign, rtREFUV_UVAssign)


IOBJ_DefineUnaryMethod (AtCurrentDM) {
    return IOBJ_IntIObject (*rtREFUV_CPD_Element (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (AtCurrentPutDM) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);
    int referenceNil = rtPTOKEN_BaseElementCount (cpd, UV_CPx_RefPToken),
	newValue = IOBJ_NumericIObjectValue (parameterArray[0], int);

    cpd->EnableModifications ();
    *rtREFUV_CPD_Element (cpd) = newValue < 0 || newValue > referenceNil
	? referenceNil : newValue;

    return self;
}

IOBJ_DefineUnaryMethod (StepValuesDM) {
    M_CPD * cpd = RTYPE_QRegisterCPD (self);
    cpd->EnableModifications ();

    rtREFUV_ElementType *ptr = rtREFUV_CPD_Array (cpd);

    unsigned int size	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_PToken);
    unsigned int refNil	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_RefPToken);
    unsigned int cnt = size < refNil ? size : refNil;

    for (unsigned int i=0; i<cnt; *ptr++ = i++);

    while (cnt++ < size)
	*ptr++ = refNil;

    return self;
}

IOBJ_DefineMethod (FillDM) {
    M_CPD *cpd	= RTYPE_QRegisterCPD (self);
    int	refNil	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_RefPToken),
	size	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_PToken),
	value	= IOBJ_NumericIObjectValue (parameterArray[0], int);
    rtREFUV_ElementType	i;

    cpd->EnableModifications ();
    rtREFUV_ElementType *ptr = rtREFUV_CPD_Array (cpd);

    value = (refNil > value) ? value : refNil;

    for (i=0; i<size; *ptr++ = value, i++);

    return self;
}

IOBJ_DefineMethod (SetRangeAtValCntDM) {
    M_CPD *cpd	= RTYPE_QRegisterCPD (self);
    int	refNil	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_RefPToken),
	at	= IOBJ_NumericIObjectValue (parameterArray[0], int),
	value	= IOBJ_NumericIObjectValue (parameterArray[1], int),
	count	= IOBJ_NumericIObjectValue (parameterArray[2], int),
	size	= rtPTOKEN_BaseElementCount (cpd, UV_CPx_PToken);
    rtREFUV_ElementType i;

    if (at >= size)
	return self;

    cpd->EnableModifications ();
    rtREFUV_ElementType  *ptr = rtREFUV_CPD_Array (cpd);

    if ((at + count - 1) >= size)
	count = size - at;

    ptr += at;

    for (i=0; i<count; i++){
	value = (value > refNil) ? refNil : value;
	*ptr++ = (rtREFUV_ElementType)value;
	value++;
	}

    return self;
}

IOBJ_DefineUnaryMethod (SortDM) {
    return RTYPE_QRegister (rtREFUV_AscendingSortIndices (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (TestRefAlignDM) {
    rtREFUV_ElementType val = IOBJ_NumericIObjectValue (parameterArray[0], int);
    rtREFUV_Type_Reference ref;
    DSC_InitReferenceScalar (
	ref, UV_CPD_RefPTokenCPD (RTYPE_QRegisterCPD (self)), val
    );
    rtREFUV_AlignReference (&ref);
    IO_printf ("Result Reference Value = %d\n", rtREFUV_Ref_Element (&ref));

    DSC_ClearScalar (ref);
    return (self);
}

IOBJ_DefineUnaryMethod (AsLinkDM) {
    if (LastAsLinkSortIndicesCPD)
        LastAsLinkSortIndicesCPD->release ();

    return RTYPE_QRegister (
	rtLINK_RefUVToConstructor (RTYPE_QRegisterCPD (self), &LastAsLinkSortIndicesCPD)
    );
}

IOBJ_DefineUnaryMethod (CopyDM) {
    return RTYPE_QRegister (UV_Copy (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (CopyWithPTokenDM) {
    M_CPD *ptokenCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    RTYPE_MustBeA ("rtREFUV CopyWithPToken", M_CPD_RType (ptokenCPD), RTYPE_C_PToken);

    return RTYPE_QRegister (UV_CopyWithNewPToken (RTYPE_QRegisterCPD (self), ptokenCPD));
}

/*****
 *
 *  Arguments:
 *	self	- any reference uvector - not used
 *	arg0	- the cartesian ptoken
 *	arg1	- the integer row number
 *	arg2	- the integer column number
 *
 *  Returns:
 *	self
 *
 *****/
IOBJ_DefineMethod (TestLinearizeDM) {
    rtREFUV_Type_Reference	resultRef, rowRef, columnRef;

    M_CPD *cartesianPT = RTYPE_QRegisterCPD (parameterArray [0]);

    DSC_InitReferenceScalar (
	rowRef,
	rtPTOKEN_CPD_RowPTokenCPD (cartesianPT),
	IOBJ_NumericIObjectValue (parameterArray[1], unsigned int)
    );

    DSC_InitReferenceScalar (
	columnRef,
	rtPTOKEN_CPD_ColPTokenCPD (cartesianPT),
	IOBJ_NumericIObjectValue (parameterArray[2], unsigned int)
    );

    rtREFUV_LinearizeRrRc (&resultRef, cartesianPT, &rowRef, &columnRef);
    IO_printf
	("\nResult Linearization: %d\n", rtREFUV_Ref_Element (&resultRef));

    DSC_ClearScalar (resultRef);
    DSC_ClearScalar (rowRef);
    DSC_ClearScalar (columnRef);

    return self;
}

/***  Since this method needs to return 2 uvectors,
 ***	the new set uvector is returned as self and
 ***	the refuv uvector can be accessed by: setFromLast
 ***/
IOBJ_DefineUnaryMethod (ToSetUVDM) {
    M_CPD *resultCPD, *refuvCPD;

    rtREFUV_ToSetUV (IOBJ_ScratchPad, RTYPE_QRegisterCPD (self), &resultCPD, &refuvCPD);

    IOBJ_IObject saveresult = RTYPE_QRegister (resultCPD);
    RTYPE_QRegister (refuvCPD);

    return saveresult;
}

IOBJ_DefineUnaryMethod (MarkAsSetDM) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);
    unsigned int 
	size = rtPTOKEN_BaseElementCount (cpd, UV_CPx_PToken),
	i;
    rtREFUV_ElementType
	*ptr = rtREFUV_CPD_Array (cpd),
	lastRef;

    /* Make sure this RefUV meets the definition of 'set' */
    rtREFUV_Align (cpd);
    if (1 < size) /* we have to check for uniqueness and sortedness */ {
	for (i = 1, lastRef = *ptr++; i < size; i++, ptr++) {
	    if (lastRef >= *ptr)
		return self;
	    lastRef = *ptr;
	}
    }
    /* It meets the definition, so mark it as requested */
    cpd->EnableModifications ();
    UV_CPD_IsASetUV (cpd) = true;

    return self;
}

IOBJ_DefineMethod (LocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_CType *locationsLinkC, *addedLinkC;
    rtREFUV_LocateOrAdd (targetCPD, sourceCPD, &locationsLinkC, &addedLinkC);
    if (addedLinkC) {
	IO_printf ("\nAdded LinkC:\n");
	addedLinkC->DebugPrint ();
	addedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineMethod (ScalarLocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);
    rtREFUV_ElementType key = (rtREFUV_ElementType)IOBJ_IObjectValueAsInt (
	parameterArray[0]
    );

    int location;
    if (rtREFUV_ScalarLocateOrAdd (targetCPD, &key, &location))
	IO_printf ("Scalar Value ADDED at position %d\n", location);
    else
	IO_printf ("Scalar Value found at position %d\n", location);

    return self;
}

IOBJ_DefineMethod (LookupDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);
    M_CPD *keyCPD = RTYPE_QRegisterCPD (parameterArray[0]);
    rtLINK_LookupCase lookupCase =
	(rtLINK_LookupCase)IOBJ_IObjectValueAsInt (parameterArray[1]);

    rtLINK_CType *locationsLinkC, *locatedLinkC;
    rtREFUV_Lookup (sourceCPD, keyCPD, lookupCase, &locationsLinkC, &locatedLinkC);
    if (locatedLinkC) {
	IO_printf ("\nLocated LinkC:\n");
	locatedLinkC->DebugPrint ();
	locatedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineMethod (ScalarLookupDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);

    rtLINK_LookupCase lookupCase =
	(rtLINK_LookupCase)IOBJ_IObjectValueAsInt (parameterArray[1]);
    rtREFUV_ElementType key =
	(rtREFUV_ElementType)IOBJ_IObjectValueAsInt (parameterArray[0]);

    int location;
    if (rtREFUV_ScalarLookup (sourceCPD, &key, lookupCase, &location))
	IO_printf ("Scalar Lookup Found Value at position %d\n", location);
    else
	IO_printf ("Scalar Lookup value not found\n");

    return self;
}


IOBJ_DefineUnaryMethod (TracesRefAlignOnDM) {
    fTracingRefAlign = true;
    return self;
}

IOBJ_DefineUnaryMethod (TracesRefAlignOffDM) {
    fTracingRefAlign = false;
    return self;
}

IOBJ_DefineNilaryMethod (TracesRefAlignDM) {
    return IOBJ_SwitchIObject (&fTracingRefAlign);
}

/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtREFUV_Handler) {
    M_CPD **resultCPD, *sourceCPD;
    rtLINK_CType *linkc;

    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:referencing:"	, NewDM)
	IOBJ_MDE ("newDistribute:from:" , NewDistributeDM)
	IOBJ_MDE ("lastAsLinkIndices"	, LastAsLinkSortIndicesDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
        IOBJ_MDE ("tracesRefAlignOn"    , TracesRefAlignOnDM)
        IOBJ_MDE ("tracesRefAlignOff"   , TracesRefAlignOffDM)
        IOBJ_MDE ("tracesRefAlign"      , TracesRefAlignDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	UV_StandardDMDEPackage
	IOBJ_MDE ("eprint"		, PrintElementDM)
	IOBJ_MDE ("at"			, AtCurrentDM)
	IOBJ_MDE ("put:"		, AtCurrentPutDM)
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("at:put:"		, AtPutDM)
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("sort"		, SortDM)
	IOBJ_MDE ("asLink"		, AsLinkDM)
	IOBJ_MDE ("testRefAlign:"	, TestRefAlignDM)
	IOBJ_MDE ("stepValues"		, StepValuesDM)
	IOBJ_MDE ("fill:"		, FillDM)
	IOBJ_MDE ("setRangeAt:val:cnt:"	, SetRangeAtValCntDM)
	IOBJ_MDE ("copy"		, CopyDM)
	IOBJ_MDE ("copyWithPToken:"	, CopyWithPTokenDM)
	IOBJ_MDE ("testLinearizePT:row:col:", TestLinearizeDM)
	IOBJ_MDE ("toSetUV"		, ToSetUVDM)
	IOBJ_MDE ("markAsSet"		, MarkAsSetDM)
	IOBJ_MDE ("locateOrAdd:"	, LocateOrAddDM)
	IOBJ_MDE ("scalarLocateOrAdd:"	, ScalarLocateOrAddDM)
	IOBJ_MDE ("lookup:type:"	, LookupDM)
	IOBJ_MDE ("scalarLookup:type:"	, ScalarLookupDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(UV_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = UV_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtREFUV_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = UV_ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = UV_MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	UV_Print (rtREFUV_Align (iArgList.arg<M_CPD*>()), false, PrintElement);
        break;
    case RTYPE_RPrintObject:
	UV_Print (rtREFUV_Align (iArgList.arg<M_CPD*>()), true, PrintElement);
        break;
    case RTYPE_DoLCExtract:
        resultCPD   = iArgList.arg<M_CPD **>();
	sourceCPD   = iArgList.arg<M_CPD*>();
	linkc	    = iArgList.arg<rtLINK_CType *>();
	*resultCPD  = rtREFUV_LCExtract (sourceCPD, linkc);
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
    FAC_FDFCase_FacilityIdAsString (rtREFUV);
    FAC_FDFCase_FacilityDescription ("Reference U-Vector Representation Type Handler");
    case FAC_DoStartupInitialization:
        char *                  estring;
        double			evalue;
        if (IsntNil (estring = getenv ("VisionRefAlignCDRatio")) &&
            (evalue = strtod (estring, (char **)NULL)) >= 0
        ) rtREFUV_SetCodomainDomainRatio (evalue);

        FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;

    default:
        break;
    }
}



/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTrefuv.c 1 replace /users/mjc/system
  860404 20:14:28 mjc Create Reference U-Vector Representation Type

 ************************************************************************/
/************************************************************************
  RTrefuv.c 2 replace /users/mjc/system
  860414 12:29:52 mjc Delete 'Browser' initialization from M_InitializeMData case

 ************************************************************************/
/************************************************************************
  RTrefuv.c 3 replace /users/mjc/system
  860422 22:56:26 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTrefuv.c 4 replace /users/mjc/system
  860504 18:45:37 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTrefuv.c 5 replace /users/mjc/system
  860521 11:10:16 mjc Release for coordination with jck

 ************************************************************************/
/************************************************************************
  RTrefuv.c 6 replace /users/mjc/system
  860523 00:58:36 mjc Change 'RefPToken' to use new u-vector 'RefPToken'

 ************************************************************************/
/************************************************************************
  RTrefuv.c 7 replace /users/mjc/system
  860530 09:22:40 mjc Added 'Reference' type and alignment routines to deal with it

 ************************************************************************/
/************************************************************************
  RTrefuv.c 8 replace /users/mjc/system
  860530 14:36:54 mjc Added a referential p-token to the creation protocol for all u-vectors

 ************************************************************************/
/************************************************************************
  RTrefuv.c 9 replace /users/mjc/system
  860530 23:40:37 mjc Release 'RF{Extract/Assign}' (untested)

 ************************************************************************/
/************************************************************************
  RTrefuv.c 10 replace /users/jad/system
  860617 12:21:14 jad fully implemented refuv and reference align

 ************************************************************************/
/************************************************************************
  RTrefuv.c 11 replace /users/mjc/system
  860617 15:38:58 mjc Declared save routine

 ************************************************************************/
/************************************************************************
  RTrefuv.c 12 replace /users/mjc/system
  860623 10:55:16 mjc Added '{LC,UV}AssignScalar' routines (untested)

 ************************************************************************/
/************************************************************************
  RTrefuv.c 13 replace /users/mjc/system
  860709 10:14:44 mjc Release new format 'reference's for continued development by 'jad'

 ************************************************************************/
/************************************************************************
  RTrefuv.c 14 replace /users/jad/system
  860715 11:39:44 jad added 2 debug methods: stepValues and setRangeAt:val:cnt:

 ************************************************************************/
/************************************************************************
  RTrefuv.c 15 replace /users/jad/system
  860717 19:25:30 jad implemented rtREFUV_Distribute

 ************************************************************************/
/************************************************************************
  RTrefuv.c 16 replace /users/jad/system
  860718 20:15:49 jad added rtREFUV_Flip

 ************************************************************************/
/************************************************************************
  RTrefuv.c 17 replace /users/jad/system
  860723 16:43:03 jad added copy debug methods

 ************************************************************************/
/************************************************************************
  RTrefuv.c 18 replace /users/jck/system
  860725 18:12:15 jck freed old refPToken in Reference Align

 ************************************************************************/
/************************************************************************
  RTrefuv.c 19 replace /users/jad/system
  860805 16:57:22 jad fixed bug in ref-aligning a refuv of size 0

 ************************************************************************/
/************************************************************************
  RTrefuv.c 20 replace /users/mjc/system
  860805 18:46:52 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  RTrefuv.c 21 replace /users/jad/system
  860806 13:21:01 jad changed align to use UTIL_Malloc and UTIL_Free

 ************************************************************************/
/************************************************************************
  RTrefuv.c 22 replace /users/jad/system
  860814 14:16:26 jad use new ptoken IsntCurrent routines in align

 ************************************************************************/
/************************************************************************
  RTrefuv.c 23 replace /users/jad/system
  860814 19:40:33 jad added counters and a displayCounts
debug method

 ************************************************************************/
/************************************************************************
  RTrefuv.c 24 replace /users/mjc/system
  861015 23:53:04 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTrefuv.c 25 replace /users/jck/system
  861017 21:18:12 jck Added a debug method to fill the refuv with a particular value

 ************************************************************************/
/************************************************************************
  RTrefuv.c 26 replace /users/mjc/system
  861020 10:38:23 mjc Fixed transposed reference arguments in range case of 'PartitionedPartition'

 ************************************************************************/
/************************************************************************
  RTrefuv.c 27 replace /users/mjc/system
  861226 12:26:27 mjc Removed superfluous unwind trap

 ************************************************************************/
/************************************************************************
  RTrefuv.c 28 replace /users/mjc/system
  870219 12:13:39 mjc Fixed ancient bug in referential alignment cleanup case

 ************************************************************************/
/************************************************************************
  RTrefuv.c 29 replace /users/jad/system
  870303 17:34:06 jad added a reference linearize routine

 ************************************************************************/
/************************************************************************
  RTrefuv.c 30 replace /users/jad/system
  870429 16:51:17 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTrefuv.c 31 replace /users/jad/system
  870501 18:54:13 jad turn off IsASet bit on some ref aligns &
 make sure to enable modifications before turning off the bit

 ************************************************************************/
/************************************************************************
  RTrefuv.c 32 replace /users/mjc/translation
  870524 09:34:53 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTrefuv.c 33 replace /users/mjc/system
  870526 16:21:18 mjc Corrected 'intializeAdditionConstructors' (note missing 'i') mis-spelling

 ************************************************************************/
/************************************************************************
  RTrefuv.c 34 replace /users/jck/system
  871007 13:34:38 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTrefuv.c 35 replace /users/jck/system
  890222 15:19:18 jck Implemented corruption detection mechanism

 ************************************************************************/
/************************************************************************
  RTrefuv.c 36 replace /users/m2/backend
  890503 14:50:57 m2 Changed PublicFnDecl to PublicFnDef

 ************************************************************************/
/************************************************************************
  RTrefuv.c 37 replace /users/m2/backend
  890927 15:03:50 m2 Changed major and minor variables because of SUN macro confilct, dummyiobj added

 ************************************************************************/
