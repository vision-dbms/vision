/*****  Int U-Vector Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtINTUV

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
#include "RTlink.h"

#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTrefuv.h"

#include "VCPDReference.h"

/*****  Self  *****/
#include "RTintuv.h"


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

DEFINE_CONCRETE_RTT (rtINTUV_Handle);


/*********************************
 *****  Basic Instantiator   *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Basic IntUV instantiation routine.
 *
 *  Arguments:
 *	pPPT			- a CPD for the PPT of the result.
 *	refPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the referential state of the
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
 *	A standard CPD for the IntUV created.
 *
 *****/
PublicFnDef M_CPD * __cdecl rtINTUV_New (
    M_CPD*			pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    Ref_UV_Initializer		initFn,
    ...
) {
/*****  Acquire the arguments passed to this function  *****/
    V_VARGLIST (initFnAP, initFn);

/*****  Create and initialize the IntUV  *****/
    return UV_New (
	RTYPE_C_IntUV,
	pPPT,
	refPTokenRefCPD,
	refPTokenRefIndex,
	sizeof (rtINTUV_ElementType),
	initFn,
	initFnAP
    );
}


/***********************************
 *****  Standard Instantiator  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Standard IntUV instantiation routine.
 *
 *  Arguments:
 *	posPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the initial positional state of
 *				  this U-Vector.
 *
 *  Returns:
 *	A standard CPD for the IntUV created.
 *
 *****/
PublicFnDef M_CPD *rtINTUV_New (
    M_CPD*			posPTokenRefCPD,
    int				posPTokenRefIndex,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex
)
{
    VCPDReference pPPT (posPTokenRefCPD, posPTokenRefIndex, RTYPE_C_PToken);
    return UV_New (
	RTYPE_C_IntUV,
	pPPT,
	refPTokenRefCPD,
	refPTokenRefIndex,
	sizeof (rtINTUV_ElementType),
	NilOf (Ref_UV_Initializer),
	0
    );
}

PublicFnDef M_CPD *rtINTUV_New (M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef) {
    return UV_New (
	RTYPE_C_IntUV,
	pPPT,
	pRPTRef,
	xRPTRef,
	sizeof (rtINTUV_ElementType),
	NilOf (Ref_UV_Initializer),
	0
    );
}


/*---------------------------------------------------------------------------
 *****  Standard IntUV instantiation routine.
 *
 *  Arguments:
 *	posPToken		- the positional P-Token of the new uvector.
 *	refPToken		- the referential P-Token of the new uvector.
 *
 *  Returns:
 *	A standard CPD for the IntUV created.
 *
 *****/
PublicFnDef M_CPD *rtINTUV_New (M_CPD *posPToken, M_CPD *refPToken) {
    return UV_New (
	RTYPE_C_IntUV,
	posPToken,
	refPToken,
	sizeof (rtINTUV_ElementType),
	NilOf (Ref_UV_Initializer),
	0
    );
}


/************************************************
 *****  Standard Set U-Vector Instantiator  *****
 ************************************************/

/******************************************
 *****  Positional Alignment Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally align a IntUV.
 *
 *  Argument:
 *	cpd			- a CPD for the IntUV to be aligned.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD *rtINTUV_Align (M_CPD *cpd) {
/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtINTUV_Align", M_CPD_RType (cpd), RTYPE_C_IntUV);

/*****  Align Positionally  *****/
/*---------------------------------------------------------------------------
 * This will probably change when P-Tokens are re-implemented !!!!!!!!!!!!!!!
 *---------------------------------------------------------------------------
 */
    UV_Align (cpd, NilOf (M_CPD::UVShiftProcessor));

/*****  Return the Argument  *****/
    return cpd;
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
PublicFnDef void rtINTUV_ToSetUV (
    M_ASD *pContainerSpace, M_CPD *sourceCPD, M_CPD **resultCPD, M_CPD **refuvCPD
) {
/*****  Align the source uvector ... *****/
    rtINTUV_Align (sourceCPD);

/*****  ... and access the relevant source data:  *****/
    VCPDReference pSourcePPT (sourceCPD, UV_CPx_PToken, RTYPE_C_PToken);
    unsigned int sSource = UV_CPD_ElementCount (sourceCPD);

/*****  If the source is empty, ... *****/
    if (0 == sSource) {
    /*****  ... the result is trivial:  *****/
	M_CPD *pResultPPT = rtPTOKEN_New (pContainerSpace, 0);
	*refuvCPD = rtREFUV_New (pSourcePPT, pResultPPT);
	*resultCPD = rtINTUV_New (pResultPPT, sourceCPD, UV_CPx_RefPToken);
	pResultPPT->release ();
    }
/*****  ... otherwise, ...  *****/
    else {
    /*****  ... sort the source, ... *****/
	rtINTUV_ElementType const *const pSourceArray = rtINTUV_CPD_Array (sourceCPD);
        unsigned int *const pSortArray = (unsigned int *)UTIL_Malloc (
	    sSource * sizeof (unsigned int)
	);
	unsigned int const *const pSortLimit = pSortArray + sSource;

        UTIL_IntQSort (pSourceArray, pSortArray, sSource, false);

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
	*resultCPD = rtINTUV_New (pResultPPT, sourceCPD, UV_CPx_RefPToken);
	rtINTUV_ElementType *const pResultArray = rtINTUV_CPD_Array (*resultCPD);

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


/**********************************************
 *****  Numeric Type Conversion Routines  *****
 **********************************************/

/*****************
 *  As DoubleUV  *
 *****************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a 'DoubleUV' routine from an 'IntUV'.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  a standard CPD for the source u-vector.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeDoubleUVFromIntUV (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);

    rtDOUBLEUV_ElementType
	*tp = rtDOUBLEUV_CPD_Array (resultCPD);
    rtINTUV_ElementType
	*sp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>()),
	*sl = sp + nelements;

    while (sp < sl)
	*tp++ = *sp++;
}


/*---------------------------------------------------------------------------
 ******  Routine to convert an IntUV to a DoubleUV
 *
 *  Arguments:
 *	pKOT			- the known object table of the result.
 *	sourceCPD		- a standard CPD for the u-vector to be
 *				  converted.
 *
 *  Returns:
 *	A standard CPD for the DoubleUV created.
 *
 *****/
PublicFnDef M_CPD *rtINTUV_AsDoubleUV (M_KOT *pKOT, M_CPD *sourceCPD) {
    rtINTUV_Align (sourceCPD);

    VCPDReference pPPT (sourceCPD, UV_CPx_PToken, RTYPE_C_PToken);
    return rtDOUBLEUV_New (
	pPPT,
	pKOT->TheDoublePTokenCPD (),
	-1,
	InitializeDoubleUVFromIntUV,
	sourceCPD
    );
}


/*****************
 *  As FloatUV  *
 *****************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a 'FloatUV' routine from an 'IntUV'.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  a standard CPD for the source u-vector.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeFloatUVFromIntUV (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);

    rtFLOATUV_ElementType
	*tp = rtFLOATUV_CPD_Array (resultCPD);
    rtINTUV_ElementType
	*sp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>()),
	*sl = sp + nelements;

    while (sp < sl)
	*tp++ = (rtFLOATUV_ElementType)*sp++;
}


/*---------------------------------------------------------------------------
 *****  Routine to convert an IntUV to a FloatUV
 *
 *  Arguments:
 *	pKOT			- the known object table of the result.
 *	sourceCPD		- a standard CPD for the u-vector to be
 *				  converted.
 *
 *  Returns:
 *	A standard CPD for the FloatUV created.
 *
 *****/
PublicFnDef M_CPD *rtINTUV_AsFloatUV (M_KOT *pKOT, M_CPD *sourceCPD) {
    rtINTUV_Align (sourceCPD);

    VCPDReference pPPT (sourceCPD, UV_CPx_PToken, RTYPE_C_PToken);
    return rtFLOATUV_New (
	pPPT,
	pKOT->TheFloatPTokenCPD (),
	-1,
	InitializeFloatUVFromIntUV,
	sourceCPD
    );
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
PublicFnDef M_CPD *rtINTUV_Distribute (M_CPD *refuvCPD, M_CPD *sourceuvCPD) {
    VCPDReference pPPT (refuvCPD, UV_CPx_RefPToken, RTYPE_C_PToken);
    return rtINTUV_UVAssign (
	rtINTUV_New (pPPT, sourceuvCPD, UV_CPx_RefPToken), refuvCPD, sourceuvCPD
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
 *	sourceCPD		- a standard CPD for the IntUV from which
 *				  the elements are to be extracted.
 *	linkConstructor		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the IntUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *****/
PublicFnDef M_CPD *rtINTUV_LCExtract (M_CPD *sourceCPD, rtLINK_CType *linkConstructor) {
/*****
 *  Align the link constructor and validate its applicability as an extraction
 *  subscript for 'source'
 *****/
    linkConstructor->AlignForExtract (rtINTUV_Align (sourceCPD), UV_CPx_PToken);

/*****  Extract the requested values  *****/
    return rtINTUV_New (
	linkConstructor->PPT (),
	sourceCPD,
	UV_CPx_RefPToken,
	UV_InitLCExtractedUV,
	sourceCPD,
	linkConstructor,
	NilOf (Ref_UV_Initializer)
    );
}


/*******************************************************
 *  Reference U-Vector Subscripted Element Extraction  *
 *******************************************************/

/**********************************************************
 *  Internal 'Extract'ed U-Vector Initialization Routine  *
 **********************************************************/

/*---------------------------------------------------------------------------
 *****  Internal 'rtINTUV_New' initialization routine to extract a
 *****  collection of elements from a IntUV.
 *
 *  Arguments:
 *	extractedUVCPD		- an initialized standard CPD for the
 *				  IntUV being extracted.
 *	nelements		- the number of elements in the IntUV
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
    rtINTUV_ElementType
	*tp, *source;
    int
	*ip, *il, index,
	iRefNil = UV_CPD_ElementCount (sourceCPD);

    for (source = rtINTUV_CPD_Array (sourceCPD),
	 tp = rtINTUV_CPD_Array (extractedUVCPD),
	 il =
	     UV_CPD_ElementCount (refuvCPD) +
	         (ip = rtREFUV_CPD_Array (refuvCPD));
	 ip < il;
	 *tp++ = (index = *ip++) >= iRefNil ? 0 : source[index]);
}


/**********************************
 *  External 'UVExtract' Routine  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  Reference U-Vector Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the IntUV from which
 *				  the elements are to be extracted.
 *	refuvCPD		- a standard CPD for a reference U-Vector
 *				  specifying the elements to be extracted.
 *				  The reference U-Vector must be referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the IntUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[refuv[i]];
 *
 *****/
PublicFnDef M_CPD *rtINTUV_UVExtract (M_CPD *sourceCPD, M_CPD *refuvCPD) {
/*****
 * Align 'source' and 'refuv' and validate 'refuv's applicability as an
 * extraction subscript for 'source'.
 *****/
    rtREFUV_AlignForExtract (
	rtINTUV_Align (sourceCPD), UV_CPx_PToken, refuvCPD
    );

/*****  Extract and return the requested values  *****/
    VCPDReference pPPT (refuvCPD, UV_CPx_PToken, RTYPE_C_PToken);
    return rtINTUV_New (
	pPPT,
	sourceCPD,
	UV_CPx_RefPToken,
	InitializeExtractedUV,
	sourceCPD,
	refuvCPD
    );
}


/**********************************************
 *  Reference Subscripted Element Extraction  *
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to extract an element of a FloatUV given a reference.
 *
 *  Arguments:
 *	resultAddr		- the address at which to deposit the element.
 *	sourceCPD		- a standard CPD for the FloatUV.
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
PublicFnDef void rtINTUV_RFExtract (
    rtINTUV_ElementType		*resultAddr,
    M_CPD			*sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
)
{
    unsigned int element;
/*****
 * Align 'source' and 'reference' and validate 'reference's applicability as an
 * extraction subscript for 'source'.
 *****/
    rtREFUV_AlignAndValidateRef (
	referenceAddr, rtINTUV_Align (sourceCPD), UV_CPx_PToken
    );

/*****  Extract and return the requested values  *****/
    *resultAddr =
	(element = rtREFUV_Ref_Element (referenceAddr)) >=
	    rtPTOKEN_CPD_BaseElementCount
	        (rtREFUV_Ref_RefPTokenCPD (referenceAddr))
	? 0
	: rtINTUV_CPD_Array (sourceCPD)[element];
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
PublicFnDef M_CPD *rtINTUV_LCAssign (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, M_CPD *sourceCPD
) {
/*****  Align 'source' and 'target' and perform the assignment  *****/
    return UV_LCAssign (
	rtINTUV_Align (targetCPD), linkConstructor, rtINTUV_Align (sourceCPD)
    );
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
PublicFnDef M_CPD *rtINTUV_UVAssign (
    M_CPD *targetCPD, M_CPD *refuvCPD, M_CPD *sourceCPD
)
{
    rtINTUV_ElementType		*target, *sp;
    int				*ip, *il;

/*****
 * Align 'source', 'target', and 'refuv' and validate the applicability of
 * 'refuv' as subscript for assignment of 'source' into 'target'.
 *****/
    rtREFUV_AlignForAssign (
	rtINTUV_Align (targetCPD),
	UV_CPx_PToken,
	refuvCPD,
	rtINTUV_Align (sourceCPD),
	UV_CPx_PToken
    );

/*****  Do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

    for (target = rtINTUV_CPD_Array (targetCPD),
	 sp = rtINTUV_CPD_Array (sourceCPD),
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
 *	value			- the address of the scalar value to be
 *				  assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    target[reference] = value;
 *
 *****/
PublicFnDef void rtINTUV_RFAssign (
    M_CPD			*targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtINTUV_ElementType		*value
)
{
/*****
 * Align 'target' and 'reference' and validate the applicability of
 * 'reference' as subscript for assignment into 'target'.
 *****/
    rtREFUV_AlignAndValidateRef (
	reference, rtINTUV_Align (targetCPD), UV_CPx_PToken
    );

/*****  Do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;
    rtINTUV_CPD_Array (targetCPD)
        [rtREFUV_Ref_Element (reference)] = *value;
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
 *	valuePtr		- the address of the scalar value to be
 *				  assigned.
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
PublicFnDef M_CPD *rtINTUV_LCAssignScalar (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, rtINTUV_ElementType *valuePtr
) {
/*****  Align 'target' and perform the assignment  *****/
    return UV_LCAssignScalar (
	rtINTUV_Align (targetCPD), linkConstructor, (pointer_t)valuePtr
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
 *	valuePtr		- the address of the scalar value to be
 *				  assigned.
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
PublicFnDef M_CPD *rtINTUV_UVAssignScalar (
    M_CPD *targetCPD, M_CPD *refuvCPD, rtINTUV_ElementType *valuePtr
)
{
    rtINTUV_ElementType		*target, value = *valuePtr;
    int				*ip, *il;

/*****
 * Align 'source' and 'refuv' and validate the applicability of
 * 'refuv' as subscript for assignment into 'target'.
 *****/
    rtREFUV_AlignForScalarAssign (
	rtINTUV_Align (targetCPD), UV_CPx_PToken, refuvCPD
    );

/*****  Do the assignment and return  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

    for (
	target = rtINTUV_CPD_Array (targetCPD),
	il = UV_CPD_ElementCount (refuvCPD)
	   + (ip = rtREFUV_CPD_Array (refuvCPD));
	ip < il;
	target[*ip++] = value
    );

    return targetCPD;
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
PublicFnDef M_CPD *rtINTUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    while (count-- > 0)\
	*resultPtr++ = position++

#define handleRepetition(position, count, reference)\
    UTIL_IntQSort (\
	valuesArray + position, (unsigned int *)resultPtr, count, descending\
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
    partition->AlignForDistribute (rtINTUV_Align (values), UV_CPx_PToken);

/*****  Create the sort indices...  *****/
    M_CPD *result = rtREFUV_New (partition->PPT (), partition->PPT ());

/*****  Initialize the traversal pointers...  *****/
    rtINTUV_ElementType const *valuesArray = rtINTUV_CPD_Array (values);
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
 *****  Routine to convert partitioned sort indices into partitioned ranks.
 *
 *  Arguments:
 *	partition		- a link constructor which partitions the
 *				  elements of 'sortIndices'.  Two elements of
 *				  'sortIndices' belong to the same partition
 *				  if corresponding positions in this
 *				  constructor contain the same reference.
 *	sortIndices		- a standard CPD for a reference u-vector
 *				  returned from a 'PartitionedSortIndices'
 *				  routine.
 *	pRefPTokenCPD		- a pointer to a CPD for the result's
 *				  reference p-token.
 *
 *  Returns:
 *	A standard CPD for an integer u-vector containing ranks computed from
 *	'sortIndices'.
 *
 *****/
PublicFnDef M_CPD *rtINTUV_PartitndRanks (
    rtLINK_CType *partition, M_CPD *sortIndices, M_CPD *pRefPTokenCPD
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    sourcePtr += count;\
    while (count-- > 0)\
	*resultPtr++ = 1

#define handleRepetition(position, count, reference)\
    while (count-- > 0)\
	*resultPtr++ = *sourcePtr++ + 1 - position\

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtREFUV_Align (sortIndices), UV_CPx_RefPToken);

/*****  Flip the sort indices and create the result 'rank' u-vector...  *****/
    M_CPD *source = rtREFUV_Flip (sortIndices);
    M_CPD *result = rtINTUV_New (partition->PPT (), pRefPTokenCPD);

/*****  Initialize the traversal pointers...  *****/
    rtREFUV_ElementType const *sourcePtr = rtREFUV_CPD_Array (source);
    rtINTUV_ElementType	*resultPtr = rtINTUV_CPD_Array (result);

/*****  Traverse the partitioning link...  *****/
    rtLINK_TraverseRRDCList (
	partition, handleRepetition, handleRepetition, handleRange
    );

/*****  Clean up and return the computed sort indices...  *****/
    source->release ();

    return result;

/*---------------------------------------------------------------------------
 *****  Link Traversal Component Handler Macro Deletions
 *---------------------------------------------------------------------------*/
#undef handleRange
#undef handleRepetition
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
PublicFnDef void rtINTUV_PartitndPartition (
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
    rtINTUV_ElementType	previousValue, currentValue;

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtINTUV_Align (values), UV_CPx_PToken);

/*****  Create the major and minor partitions...  *****/
    rtLINK_CType *majorPartition = rtLINK_RefConstructor (partition->RPT (), -1);
    rtLINK_CType *minorPartition = rtLINK_PosConstructor (partition->PPT (), -1);

/*****  Initialize the traversal pointers...  *****/
    rtINTUV_ElementType	const *valuesPtr = rtINTUV_CPD_Array (values);
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


/*********************************
 *****  LocateOrAdd Routine  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to locate or add a set of values to an integer set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for an integer set uvector for the
 *                        uvector to be added to.
 *	sourceCPD	- a standard CPD for an integer set uvector containing
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
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	If 'addedLinkC' is constructed, it's domain will reside in the key's
 *	scratch pad.
 *
 *****/
PublicFnDef void rtINTUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC
) {
    int
	wantAddedLinkC, largestSize, notNil, found,
	resultCount, srcPos, notYetInitialized = true,
	origTargetCount;
    rtINTUV_ElementType
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

#define initializeAdditionConstructors \
if (notYetInitialized) {\
    /*** Create the added link constructor ... ***/\
    if (wantAddedLinkC)\
    	*addedLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);\
\
    /*** Create a ptoken constructor for the target ... ***/\
    targetPTokenC = rtPTOKEN_NewShiftPTConstructor (targetCPD, UV_CPx_PToken);\
    notYetInitialized = false;\
}


/***** Make sure both the source and target are integer set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_IntUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_LocateOrAdd:  Source UVector Must Be An Integer Set"
    );

    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_IntUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_LocateOrAdd:  Target UVector Must Be An Integer Set"
    );

/***** Align and validate source and target uvectors ... *****/
    rtINTUV_Align (sourceCPD);
    rtINTUV_Align (targetCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, targetCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_LocateOrAdd:  Referential Inconsistency"
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

    resultStart = resultp = (rtINTUV_ElementType *) UTIL_Malloc (
	largestSize * sizeof (rtINTUV_ElementType)
    );

/***** Begin the traversal ... *****/
    resultCount = 0;
    srcp = rtINTUV_CPD_Array (sourceCPD);
    srcpl = srcp + UV_CPD_ElementCount (sourceCPD);
    srcPos = 0;
    trgp = rtINTUV_CPD_Array (targetCPD);
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
	rtINTUV_CPD_Element (targetCPD) =
	    rtINTUV_CPD_Array (targetCPD) + origTargetCount;

	targetCPD->ShiftContainerTail (
	    UV_CPx_Element,
	    0,
	    ((ptrdiff_t)resultCount - origTargetCount) * UV_CPD_Granularity (targetCPD),
	    false
	);

	UV_CPD_ElementCount (targetCPD) = resultCount;

	/*** Copy over the new values ... ***/
	resultp = resultStart;
	trgp = rtINTUV_CPD_Array (targetCPD);
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
 *****  Routine to locate or add a value to an integer set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for an integer set uvector for the
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
PublicFnDef bool rtINTUV_ScalarLocateOrAdd (
    M_CPD *targetCPD, rtINTUV_ElementType *sourcePtr, int *locationPtr
)
{
    int
	empty, size, low, hi, index, addAt;
    rtINTUV_ElementType
	value;

#define access(index)\
    *(rtINTUV_CPD_Array (targetCPD) + index)


/***** Make sure the target is a integer set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_IntUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_ScalarLocateOrAdd:  Target UVector Must Be An Integer Set"
    );

/***** Align target uvector ... *****/
    rtINTUV_Align (targetCPD);

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
	rtPTOKEN_CType *targetPTokenC = rtPTOKEN_NewShiftPTConstructor (
	    targetCPD, UV_CPx_PToken
	);

        /*** modify target ptoken ***/
	targetPTokenC->AppendAdjustment (addAt, 1);

	/***** Reconstruct the resulting target uvector ... *****/
	/*** Make the target writable ... ***/
	targetCPD->EnableModifications ();

	/*** Shift the container tail to add room ... ***/
	rtINTUV_CPD_Element (targetCPD)=rtINTUV_CPD_Array (targetCPD) + addAt;

	targetCPD->ShiftContainerTail (
	    UV_CPx_Element,
	    (size - addAt) * UV_CPD_Granularity (targetCPD),
	    UV_CPD_Granularity (targetCPD),
	    false
	);

	UV_CPD_ElementCount (targetCPD) = size + 1;

	/*** insert the value ***/
	*(rtINTUV_CPD_Array (targetCPD) + addAt) = value;

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
 *****  Routine to lookup values in an integer set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the integer set uvector to be
 *			  be searched.
 *	keyCPD		- a standard CPD for the integer set uvector containing
 *			  the values to lookup in the 'sourceCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ,
 *			          rtLINK_LookupCase_GE, or
 *				  rtLINK_LookupCase_GT.
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
PublicFnDef void rtINTUV_Lookup (
    M_CPD		*sourceCPD,
    M_CPD		*keyCPD,
    rtLINK_LookupCase	lookupCase,
    rtLINK_CType	**locationsLinkC,
    rtLINK_CType	**locatedLinkC
)
{
    int
	backedupOk, found, srcPos, keyPos, sourceCount, notNil,
	someNotFound = false;
    rtINTUV_ElementType
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
	switch (lookupCase)\
	{\
	case rtLINK_LookupCase_GT:\
	    if (*srcp == (value)) advance;\
	case rtLINK_LookupCase_GE:\
	    found = true;\
	    break;\
	case rtLINK_LookupCase_EQ:\
	    found = (*srcp == (value));\
	    break;\
	case rtLINK_LookupCase_LE:\
	    if (!(found = (*srcp == (value))))\
	    {\
		backup;\
		found = (backedupOk && notNil);\
	    }\
	    break;\
	default:\
	    ERR_SignalFault (\
		EC__InternalInconsistency,\
		"rtINTUV_Lookup: Unknown Comparison Operation"\
	    );\
	    break;\
	}\
    }\
}


/***** Make sure both the source and key are integer set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_IntUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_Lookup:  Source UVector Must Be An Integer Set"
    );

    if (((RTYPE_Type)M_CPD_RType (keyCPD) != RTYPE_C_IntUV) ||
        !UV_CPD_IsASetUV (keyCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_Lookup:  Key UVector Must Be An Integer Set"
    );

/***** Align and validate source and key uvectors ... *****/
    rtINTUV_Align (sourceCPD);
    rtINTUV_Align (keyCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, keyCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_Lookup:  Referential Inconsistency"
    );

/***** Make the location link constructor ... *****/
    *locationsLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);/*JCK*/

/***** If the key is empty, close the locationsLinkC and return ... *****/
    if (UV_CPD_ElementCount (keyCPD) == 0) {
	(*locationsLinkC)->Close (keyCPD, UV_CPx_PToken);
        *locatedLinkC = NilOf (rtLINK_CType *);
	return;
    }

/***** Make the located link constructor ... *****/
    *locatedLinkC = rtLINK_RefConstructor (keyCPD, UV_CPx_PToken);

/***** Begin the traversal ... *****/
    srcp = srcpf = rtINTUV_CPD_Array (sourceCPD);
    srcpl = srcp + (sourceCount = UV_CPD_ElementCount (sourceCPD));
    srcPos = 0;
    notNil = (sourceCount > 0);

    keyp = rtINTUV_CPD_Array (keyCPD);
    keypl = keyp + UV_CPD_ElementCount (keyCPD);
    keyPos = 0;

/***** Loop through the key looking for the values in the source ... *****/
    while (keyp < keypl) {
	locate (*keyp);

	if (!found)
	    someNotFound = true;
    	else  /* found */ {
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
 *****  Routine to lookup a value in an integer set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the integer set uvector to be
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
PublicFnDef bool rtINTUV_ScalarLookup (
    M_CPD			*sourceCPD,
    rtINTUV_ElementType		*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
)
{
    int				size, low, hi, index;
    rtINTUV_ElementType		value;

#define access(index)\
    *(rtINTUV_CPD_Array (sourceCPD) + index)

/***** Make sure the source is an integer set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_IntUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtINTUV_Lookup:  Source UVector Must Be An Integer Set"
    );

/***** Align the source uvector ... *****/
    rtINTUV_Align (sourceCPD);

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

    switch (lookupCase)
    {
    case rtLINK_LookupCase_EQ:
	if (access (index) != value)
	    return false;

 	*locationPtr = index;
        break;
    case rtLINK_LookupCase_GE:
	if (index == 0 && (access(index) >= value))
	    *locationPtr = index;
	else
	{
	    if (access (index) < value)
		index++;
	    if (index >= size)
		return false;
	    *locationPtr = index;
	}
        break;
    case rtLINK_LookupCase_GT:
	if (index == 0 && (access(index) > value))
	    *locationPtr = index;
	else
	{
	    while (access (index) <= value && index < size)
		index++;
	    if (index >= size)
		return false;
	    *locationPtr = index;
	}
        break;
    case rtLINK_LookupCase_LE:
	if (access(index) <= value) /*****  Check because of 0 case *****/
	    *locationPtr = index;
	else
	    return false;

        break;
    default:
	ERR_SignalFault (
	    EC__UsageError,
	    "rtINTUV_Lookup:  Unknown comparison operation"
	);
	break;
    }

    return true;

#undef access
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/********************
 *  Element Printer  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to print a representation of an 'RTintuv' element.
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
    return IO_printf ("%d", *rtINTUV_CPD_Element (cpd));
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	rtINTUV_New (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}

IOBJ_DefineNewaryMethod (NewDistributeDM) {
    return RTYPE_QRegister (
	rtINTUV_Distribute (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}


/************************
 *  'Instance' Methods  *
 ************************/

UV_DefineEPrintDM (PrintElementDM, PrintElement)

UV_DefineAlignDM (AlignDM, rtINTUV_Align)

UV_DefineTypeConversionDM (AsDoubleUVDM, rtINTUV_AsDoubleUV)

UV_DefineTypeConversionDM (AsFloatUVDM, rtINTUV_AsFloatUV)

UV_DefineAtDM (AtDM, rtINTUV_LCExtract, rtINTUV_UVExtract)

UV_DefineAtPutDM (AtPutDM, rtINTUV_LCAssign, rtINTUV_UVAssign)

UV_DefineAtRefDM (
    AtReferenceDM, rtINTUV_RFExtract, rtINTUV_ElementType, IOBJ_IntIObject
)

UV_DefineAtRefPutDM (
    AtReferencePutDM,
    rtINTUV_RFAssign,
    rtINTUV_ElementType,
    IOBJ_NumericIObjectValue (parameterArray[1], rtINTUV_ElementType)
)

UV_DefineAtPutScalarDM (
    AtPutScalarDM,
    rtINTUV_LCAssignScalar,
    rtINTUV_UVAssignScalar,
    rtINTUV_ElementType,
    IOBJ_NumericIObjectValue (parameterArray[1], rtINTUV_ElementType)
)


IOBJ_DefineUnaryMethod (AtCurrentDM)
{
    return IOBJ_IntIObject (*rtINTUV_CPD_Element (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (AtCurrentPutDM) {
    M_CPD* pCPD = RTYPE_QRegisterCPD (self);

    pCPD->EnableModifications ();
    *rtINTUV_CPD_Element (pCPD) = IOBJ_NumericIObjectValue (parameterArray[0], int);

    return self;
}

IOBJ_DefineUnaryMethod (StepValuesDM) {
/***** Sets values for all the elements in the uvector - from 0 to n *****/
    M_CPD *cpd = RTYPE_QRegisterCPD (self);
    rtINTUV_ElementType *ptr = rtINTUV_CPD_Array (cpd);
    unsigned int size = UV_CPD_ElementCount (cpd);

    for (unsigned int i=0; i<size; *ptr++ = i++);

    return self;
}

IOBJ_DefineUnaryMethod (CopyDM) {
    return RTYPE_QRegister (UV_Copy (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineMethod (CopyWithPTokenDM) {
    M_CPD *ptokenCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    RTYPE_MustBeA ("rtREFUV CopyWithPToken", M_CPD_RType (ptokenCPD), RTYPE_C_PToken);

    return RTYPE_QRegister (UV_CopyWithNewPToken (RTYPE_QRegisterCPD (self), ptokenCPD));
}

/***  Since this method needs to return 2 uvectors,
 ***	the new set uvector is returned as self and
 ***	the refuv uvector can be accessed by: setFromLast
 ***/
IOBJ_DefineUnaryMethod (ToSetUVDM) {
    M_CPD *resultCPD, *refuvCPD;
    rtINTUV_ToSetUV (IOBJ_ScratchPad, RTYPE_QRegisterCPD (self), &resultCPD, &refuvCPD);

    IOBJ_IObject saveresult = RTYPE_QRegister (resultCPD);
    RTYPE_QRegister (refuvCPD);

    return saveresult;
}

IOBJ_DefineMethod (LocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_CType *locationsLinkC, *addedLinkC;
    rtINTUV_LocateOrAdd (targetCPD, sourceCPD, &locationsLinkC, &addedLinkC);
    if (addedLinkC) {
	IO_printf ("\nAdded LinkC:\n");
	addedLinkC->DebugPrint ();
	addedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineMethod (ScalarLocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);
    rtINTUV_ElementType key = (rtINTUV_ElementType)IOBJ_IObjectValueAsInt (
	parameterArray[0]
    );

    int location;
    if (rtINTUV_ScalarLocateOrAdd (targetCPD, &key, &location))
	IO_printf ("Scalar Value ADDED at position %d\n", location);
    else
	IO_printf ("Scalar Value found at position %d\n", location);

    return self;
}

IOBJ_DefineMethod (LookupDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);
    M_CPD *keyCPD = RTYPE_QRegisterCPD (parameterArray[0]);
    rtLINK_LookupCase lookupCase = (rtLINK_LookupCase)IOBJ_IObjectValueAsInt (
	parameterArray[1]
    );

    rtLINK_CType *locationsLinkC, *locatedLinkC;
    rtINTUV_Lookup (sourceCPD, keyCPD, lookupCase, &locationsLinkC, &locatedLinkC);
    if (locatedLinkC) {
	IO_printf ("\nLocated LinkC:\n");
	locatedLinkC->DebugPrint ();
	locatedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineMethod (ScalarLookupDM) {
    M_CPD* sourceCPD = RTYPE_QRegisterCPD (self);
    int location;

    rtLINK_LookupCase lookupCase =
	(rtLINK_LookupCase)IOBJ_IObjectValueAsInt (parameterArray[1]);
    rtINTUV_ElementType key =
	(rtINTUV_ElementType)IOBJ_IObjectValueAsInt (parameterArray[0]);

    if (rtINTUV_ScalarLookup (sourceCPD, &key, lookupCase, &location))
	IO_printf ("Scalar Lookup Found Value at position %d\n", location);
    else
	IO_printf ("Scalar Lookup value not found\n");

    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtINTUV_Handler) {
    M_CPD **resultCPD, *sourceCPD;
    rtLINK_CType *linkc;

    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:referencing:"	, NewDM)
	IOBJ_MDE ("newDistribute:from:" , NewDistributeDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	UV_StandardDMDEPackage
	IOBJ_MDE ("eprint"		, PrintElementDM)
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("asDoubleUV:"		, AsDoubleUVDM)
	IOBJ_MDE ("asFloatUV:"		, AsFloatUVDM)
	IOBJ_MDE ("at"			, AtCurrentDM)
	IOBJ_MDE ("atref:"		, AtReferenceDM)
	IOBJ_MDE ("put:"		, AtCurrentPutDM)
	IOBJ_MDE ("atref:put:"		, AtReferencePutDM)
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("at:put:"		, AtPutDM)
	IOBJ_MDE ("at:putScalar:"	, AtPutScalarDM)
	IOBJ_MDE ("stepValues"		, StepValuesDM)
	IOBJ_MDE ("copy"		, CopyDM)
	IOBJ_MDE ("copyWithPToken:"	, CopyWithPTokenDM)
	IOBJ_MDE ("toSetUV"		, ToSetUVDM)
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
	    M_RTD_HandleMaker		(rtd) = &rtINTUV_Handle::Maker;
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
	UV_Print (rtINTUV_Align (iArgList.arg<M_CPD*>()), false, PrintElement);
        break;
    case RTYPE_RPrintObject:
	UV_Print (rtINTUV_Align (iArgList.arg<M_CPD*>()), true, PrintElement);
        break;
    case RTYPE_DoLCExtract:
        resultCPD   = iArgList.arg<M_CPD **>();
	sourceCPD   = iArgList.arg<M_CPD*>();
	linkc	    = iArgList.arg<rtLINK_CType *>();
	*resultCPD  = rtINTUV_LCExtract (sourceCPD, linkc);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtINTUV);
    FAC_FDFCase_FacilityDescription
        ("Int U-Vector Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTintuv.c 1 replace /users/mjc/system
  860323 17:19:26 mjc New U-Vector representation types

 ************************************************************************/
/************************************************************************
  RTintuv.c 2 replace /users/mjc/system
  860410 19:19:16 mjc Added P-Token and standard printer

 ************************************************************************/
/************************************************************************
  RTintuv.c 3 replace /users/mjc/system
  860414 12:29:43 mjc Delete 'Browser' initialization from M_InitializeMData case

 ************************************************************************/
/************************************************************************
  RTintuv.c 4 replace /users/mjc/system
  860420 18:49:20 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTintuv.c 5 replace /users/mjc/system
  860422 22:55:39 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTintuv.c 6 replace /users/hjb/system
  860423 00:35:00 hjb done updating calls to 'M_Get...CPD' & 'M_NewSPadContainer'

 ************************************************************************/
/************************************************************************
  RTintuv.c 7 replace /users/mjc/system
  860504 18:45:18 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTintuv.c 8 replace /users/jad/system
  860514 18:13:01 jad added stepValues method

 ************************************************************************/
/************************************************************************
  RTintuv.c 9 replace /users/mjc/system
  860521 11:10:12 mjc Release for coordination with jck

 ************************************************************************/
/************************************************************************
  RTintuv.c 10 replace /users/mjc/system
  860530 14:36:51 mjc Added a referential p-token to the creation protocol for all u-vectors

 ************************************************************************/
/************************************************************************
  RTintuv.c 11 replace /users/mjc/system
  860530 21:39:50 mjc Added 'RT{Extract/Assign}'

 ************************************************************************/
/************************************************************************
  RTintuv.c 12 replace /users/mjc/system
  860615 19:01:06 mjc Eliminated 'ElementVStore' handler service case

 ************************************************************************/
/************************************************************************
  RTintuv.c 13 replace /users/mjc/system
  860617 15:38:55 mjc Declared save routine

 ************************************************************************/
/************************************************************************
  RTintuv.c 14 replace /users/jad/system
  860717 19:26:12 jad implemented rtINTUV_Distribute

 ************************************************************************/
/************************************************************************
  RTintuv.c 15 replace /users/jad/system
  860723 16:43:20 jad added copy debug methods

 ************************************************************************/
/************************************************************************
  RTintuv.c 16 replace /users/mjc/system
  861015 23:52:57 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTintuv.c 17 replace /users/mjc/system
  861016 18:32:16 jad changed the uvector type conversion routines to accept the new
ref ptoken as an argument

 ************************************************************************/
/************************************************************************
  RTintuv.c 18 replace /users/mjc/system
  861020 10:38:20 mjc Fixed transposed reference arguments in range case of 'PartitionedPartition'

 ************************************************************************/
/************************************************************************
  RTintuv.c 19 replace /users/jad/system
  861216 17:41:02 jad added locateOrAdd and lookup routines for set uvectors

 ************************************************************************/
/************************************************************************
  RTintuv.c 20 replace /users/jad/system
  861217 16:52:23 jad added scalar versions of locateOrAdd and lookup routines

 ************************************************************************/
/************************************************************************
  RTintuv.c 21 replace /users/jad/system
  861222 17:45:39 jad turn off IsASetUV bit on most assigns and aligns

 ************************************************************************/
/************************************************************************
  RTintuv.c 22 replace /users/jad/system
  861224 17:54:18 jad change some calls to M_ShiftContainerTail to reclaim released space

 ************************************************************************/
/************************************************************************
  RTintuv.c 23 replace /users/jck/system
  870415 14:35:01 jck Fixed numerous minor bugs in the integer set routines

 ************************************************************************/
/************************************************************************
  RTintuv.c 24 replace /users/jad/system
  870429 16:51:52 jad modified the set operations slightly

 ************************************************************************/
/************************************************************************
  RTintuv.c 25 replace /users/jad/system
  870501 18:53:03 jad make sure to enable modifications
before turning off the IsASet bit

 ************************************************************************/
/************************************************************************
  RTintuv.c 26 replace /users/jck/system
  870513 11:36:50 jck Added rtLINK_LookupCase_GT

 ************************************************************************/
/************************************************************************
  RTintuv.c 27 replace /users/mjc/translation
  870524 09:33:11 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTintuv.c 28 replace /users/mjc/system
  870526 16:21:13 mjc Corrected 'intializeAdditionConstructors' (note missing 'i') mis-spelling

 ************************************************************************/
/************************************************************************
  RTintuv.c 29 replace /users/jck/system
  870605 16:08:25 jck Fixed bug in scalar locate routine

 ************************************************************************/
/************************************************************************
  RTintuv.c 30 replace /users/jck/system
  871007 13:34:31 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTintuv.c 31 replace /users/jck/system
  890222 15:18:16 jck Implemented corruption detection mechanism

 ************************************************************************/
/************************************************************************
  RTintuv.c 32 replace /users/m2/backend
  890927 15:02:58 m2 Changed major and minor variables because of SUN macro confilct, dummyiobj added

 ************************************************************************/
