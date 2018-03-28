/*****  U-Vector Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName UV

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "vdsc.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

/*****  Shared  *****/
#include "uvector.h"


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

DEFINE_ABSTRACT_RTT (rtUVECTOR_Handle);


/*********************************************************
 *****  Standard U-Vector Consistency Check Routine  *****
 *********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to check a U-Vector for corruption.
 *****/
void rtUVECTOR_Handle::CheckConsistency () {
    if (isInconsistent ()) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "Corrupted uvector[%d:%d] detected", spaceIndex (), containerIndex ()
	)
    );
}


/********************************************************
 *****  Standard U-Vector CPD Management Utilities  *****
 ********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard U-Vector CPD.
 *
 *  Argument:
 *	cpd			- the address of a standard U-Vector CPD which
 *				  is to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *  Notes:
 *	A standard U-Vector CPD has four pointers.  The first three pointers
 *	should not be moved - they are intended to point to the U-Vector's
 *	P-Token, Reference P-Token, and Auxiliary POPs respectively.  The
 *	fourth pointer is intended as a moveable U-Vector element pointer.
 *
 *****/
PublicFnDef void UV_InitStdCPD (M_CPD *cpd) {
    UV_UVType *uvBase = UV_CPD_Base (cpd);

    UV_CPD_PToken	(cpd) = &UV_UV_PToken (uvBase);
    UV_CPD_RefPToken	(cpd) = &UV_UV_RefPToken (uvBase);
    UV_CPD_AuxiliaryPOP	(cpd) = &UV_UV_AuxiliaryPOP (uvBase);

    V::pointer_t a = UV_UV_Array (uvBase);
    V::pointer_t *p = (V::pointer_t *)M_CPD_Pointers (cpd);
    V::pointer_t *pl = p + cpd->PointerCount ();

    for (p += UV_CPx_Element; p < pl; *p++ = a);
}


/******************************************************
 *****  Standard U-Vector Instantiation Routines  *****
 ******************************************************/

/***********************************************
 *  Internal Container Initialization Routine  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a new U-Vector.
 *
 *  Arguments:
 *	cpd			- the address of an uninitialized standard
 *				  U-Vector CPD.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  in order: the number of elements in this
 *				  uvector, the granularity of an element,
 *				  the address of the U-Vector initialization
 *				  function passed to 'UV_New', and a
 *				  initFnArgs.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitUVector (
    M_CPD		*cpd,
    size_t		nelements,
    size_t		granularity, 
    Ref_UV_Initializer	initFn,
    va_list		initFnArgs
) {
    UV_InitStdCPD (cpd);
    UV_UVType *uvBase = UV_CPD_Base (cpd);

/*****  Initialize the preamble fields  *****/
    UV_UV_IsInconsistent	(uvBase) = true;
    UV_UV_ElementCount		(uvBase) = nelements;
    UV_UV_Granularity		(uvBase) = (unsigned short)granularity;
    UV_UV_IsASetUV		(uvBase) = false;

    cpd->constructReference (UV_CPx_PToken);
    cpd->constructReference (UV_CPx_RefPToken);
    cpd->constructReference (UV_CPx_AuxiliaryPOP);

/*****  Zero the U-Vector array area  *****/
    memset (UV_UV_Array (uvBase), 0, (nelements + 1) * granularity);

/*****  Invoke the initialization function if it was specified  *****/
    if (initFn)
	initFn (cpd, nelements, initFnArgs);
}


/************************************
 *  External Instantiation Routine  *
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create and initialize a new standard U-Vector.
 *
 *  Arguments:
 *	rType			- the representation type of the U-Vector
 *				  being created.
 *	pPPT			- a CPD for the positional p-token of this
 *				  U-Vector.
 *	refPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the referential state of the
 *				  elements of this U-Vector.
 *	granularity		- the granularity of an element in this
 *				  U-Vector.
 *	initFn			- the address of an optional function which
 *				  will be called to initialize the contents
 *				  of the U-Vector.  If not specified (i.e.,
 *				  'IsNil (initFn)', the elements of the
 *				  U-Vector will be set to zero.  If specified,
 *				  this function will be passed a initialized
 *				  standard CPD for the U-Vector, the number of
 *				  elements in the U-Vector (determined from
 *				  'posPTokenRef...' P-Token) and 'initFnArgs'.
 *				  This argument specification and order for
 *				  'initFn' is the same as 'UVECTOR_LCExtract's
 *				  'fillFn'.
 *	initFnArgs		- a <varargs.h> argument pointer referencing
 *				  a collection of optional arguments being
 *				  passed to 'initFn'.
 *
 *  Returns:
 *	A standard CPD for the U-Vector created.
 *
 *****/
PublicFnDef M_CPD *UV_New (
    RTYPE_Type			rType,
    rtPTOKEN_Handle*		pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    size_t			granularity,
    Ref_UV_Initializer		initFn,
    va_list			initFnArgs
) {
/*****  Obtain the element count for the U-Vector...  *****/
    size_t nelements = pPPT->cardinality ();

/*****  ... and create the U-Vector  *****/
    M_CPD *result = pPPT->CreateContainer (
	rType, UV_SizeofUVector (nelements, granularity)
    )->NewCPD ();

    InitUVector (result, nelements, granularity, initFn, initFnArgs);

    result->StoreReference (
	UV_CPx_PToken, pPPT
    )->StoreReference (
	UV_CPx_RefPToken, refPTokenRefCPD, refPTokenRefIndex
    );

    UV_CPD_IsInconsistent (result) = false;

    return result;
}


/*---------------------------------------------------------------------------
 *****  Routine to create and initialize a new standard U-Vector.
 *
 *  Arguments:
 *	rType			- the representation type of the U-Vector
 *				  being created.
 *	pPPT			- the positional p-token of the new u-vector.
 *	pRPT			- the reference p-token of the new u-vector.
 *	granularity		- the granularity of an element in this
 *				  U-Vector.
 *	initFn			- the address of an optional function which
 *				  will be called to initialize the contents
 *				  of the U-Vector.  If not specified (i.e.,
 *				  'IsNil (initFn)', the elements of the
 *				  U-Vector will be set to zero.  If specified,
 *				  this function will be passed a initialized
 *				  standard CPD for the U-Vector, the number of
 *				  elements in the U-Vector (determined from
 *				  'posPTokenRef...' P-Token) and 'initFnArgs'.
 *				  This argument specification and order for
 *				  'initFn' is the same as 'UVECTOR_LCExtract's
 *				  'fillFn'.
 *	initFnArgs		- a <varargs.h> argument pointer referencing
 *				  a collection of optional arguments being
 *				  passed to 'initFn'.
 *
 *  Returns:
 *	A standard CPD for the U-Vector created.
 *
 *****/
PublicFnDef M_CPD *UV_New (
    RTYPE_Type			rType,
    rtPTOKEN_Handle*		pPPT,
    rtPTOKEN_Handle*		pRPT,
    size_t			granularity,
    Ref_UV_Initializer		initFn,
    va_list			initFnArgs
) {
/*****  Obtain the element count for the U-Vector...  *****/
    size_t nelements = pPPT->cardinality ();

/*****  ... and create the U-Vector  *****/
    M_CPD *result = pPPT->CreateContainer (
	rType, UV_SizeofUVector (nelements, granularity)
    )->NewCPD ();

    InitUVector (result, nelements, granularity, initFn, initFnArgs);

    result->StoreReference (
	UV_CPx_PToken, pPPT
    )->StoreReference (
	UV_CPx_RefPToken, pRPT
    );

    UV_CPD_IsInconsistent (result) = false;

    return result;
}


/**************************
 *  UVector Copy Routine  *
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to make a new copy of a uvector.
 *
 *  Arguments:
 *	pSource		- a standard CPD for the uvector to copy.
 *	pNewPPT		- a standard CPD for the new positional PToken.  If
 *			  null, this is taken to be the positional p-token of
 *			  the uvector; if specified, its size must match the
 *			  uvector's aligned size.
 *
 *  Returns:
 *	A standard CPD for the new U-Vector.
 *
 *****/
PublicFnDef M_CPD *UV_BasicCopy (M_CPD *pSource, rtPTOKEN_Handle *pNewPPT) {
    switch (pSource->RType ()) {
    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
	break;
    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, "AlignUVector: Unknown usegment type"
	);
	break;
    }

    pSource->align ();
    unsigned int iDomCardinality = UV_CPD_ElementCount (pSource);

    rtPTOKEN_Handle::Reference pLocalPPT;
    if (!pNewPPT) {
	pLocalPPT.setTo (static_cast<rtUVECTOR_Handle*>(pSource->containerHandle ())->pptHandle ());
	pNewPPT = pLocalPPT;
    }
    else if (iDomCardinality != pNewPPT->cardinality ()) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "UV_Copy: Cardinality disagreement: Current:%u, Proposed:%u.",
	    iDomCardinality, pNewPPT->cardinality ()
	)
    );

    size_t granularity = UV_CPD_Granularity (pSource);
    M_CPD *pResult = UV_New (
	pSource->RType (),
	pNewPPT,
	pSource,
	UV_CPx_RefPToken,
	granularity,
	NilOf (Ref_UV_Initializer),
	0
    );
    if (pSource->ReferenceIsntNil (UV_CPx_AuxiliaryPOP))
	pResult->StoreReference (UV_CPx_AuxiliaryPOP, pSource, UV_CPx_AuxiliaryPOP);

    UV_CPD_IsASetUV (pResult) = UV_CPD_IsASetUV (pSource);

    memcpy (
	UV_CPD_Array (pResult, void), UV_CPD_Array (pSource, void), (iDomCardinality + 1) * granularity
    );

    return pResult;
}


/******************************************
 *****  U-Vector Reclamation Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PublicFnDef void UV_ReclaimContainer (
    M_ASD *ownerASD, M_CPreamble const *preambleAddress
) {
    UV_UVType const *uv = (UV_UVType const *)(preambleAddress + 1);

    ownerASD->Release (&UV_UV_PToken		(uv));
    ownerASD->Release (&UV_UV_RefPToken		(uv));
    ownerASD->Release (&UV_UV_AuxiliaryPOP	(uv));
}


/********************************************
 *****  Standard U-Vector Save Routine  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'save' a U-Vector.
 *
 *  Arguments:
 *	pContainerHandle	- a handle for the container being saved.
 *
 *  Returns:
 *	true if all references will persist, false otherwise.
 *
 *****/
bool rtUVECTOR_Handle::PersistReferences () {
    UV_UVType *uv = typecastContent ();

    return Persist (&UV_UV_PToken	(uv))
	&& Persist (&UV_UV_RefPToken	(uv))
	&& Persist (&UV_UV_AuxiliaryPOP	(uv));
}


/********************************************
 *****  Standard U-Vector Mark Routine  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'mark' the containers referenced by a U-Vector.
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
PublicFnDef void UV_MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD* pSpace, M_CPreamble const *pContainer) {
    pGCV->Mark (pSpace, (M_POP const*) (pContainer + 1), 3);
}


/************************************************************
 *****  Standard U-Vector Positional Alignment Routine  *****
 ************************************************************/

/********************
 *  Utility Macros  *
 ********************/

/*---------------------------------------------------------------------------
 * The following macros serve to copy preserved regions of the uvector from the
 * source to the temporary result, and to process/initialize the
 * deleted/inserted regions.
 *---------------------------------------------------------------------------
 */

#define AlignmentDeleteMacro(origin, shift) {\
    unsigned int xByteOrigin = (origin) * sElement;\
    int sByteShift = (shift) * sElement;\
    AlignmentCopyMacro (xByteOrigin + sByteShift - xSourceOrigin);\
    xSourceOrigin = xByteOrigin;\
}

#define AlignmentInsertMacro(origin, shift) {\
    unsigned int xByteOrigin = (origin) * sElement;\
    unsigned int sByteShift  = (shift) * sElement;\
    bStillASet = false;\
    AlignmentCopyMacro (xByteOrigin - xSourceOrigin);\
    rFillProcessor.fill (pTargetArray + xTargetOrigin, sByteShift);\
    xSourceOrigin = xByteOrigin;\
    xTargetOrigin += sByteShift;\
}

#define AlignmentCopyMacro(numBytes) {\
    unsigned int sCopy = numBytes;\
    memcpy (pTargetArray + xTargetOrigin, pSourceArray + xSourceOrigin, sCopy);\
    xTargetOrigin += sCopy;\
}


/**********************************
 *  Positional Alignment Routine  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  U-Vector positional alignment.
 *
 *  Returns:
 *	'true' if a positional alignment was done, false otherwise.
 *
 *****/
bool rtUVECTOR_Handle::align_() {
    return align ();
}

bool rtUVECTOR_Handle::align () {
    AlignmentFillProcessor iFillProcessor;
    return alignUsing (iFillProcessor);
}

bool rtUVECTOR_Handle::alignUsing (AlignmentFillProcessor &rFillProcessor) {
/*****  If U-Vector's ptoken is a chain terminator, Nothing to do  *****/
    rtPTOKEN_CType *ptConstructor;
    if (isTerminal (pptPOP (), ptConstructor))
        return false;
/*****  Create a temporary result  *****/
    unsigned int const sElement = granularity ();
    unsigned int const sTargetArray = ptConstructor->NextGeneration ()->cardinality ();
    char *const pTargetArray = static_cast<char*>(UTIL_Malloc (sTargetArray * sElement));

/***** Set up for ptoken traversal  *****/
    bool bStillASet = isASet ();
    char const *const pSourceArray = arrayBase ();

    unsigned int xSourceOrigin = 0;
    unsigned int xTargetOrigin = 0;

/*****  Do the normalization  *****/
    rtPTOKEN_FTraverseInstructions (
	ptConstructor, AlignmentInsertMacro, AlignmentDeleteMacro
    );

/*****  Copy the final sequence from source to target  *****/
    AlignmentCopyMacro (elementCount () * sElement - xSourceOrigin);

/*****  Copy the results of the alignment back to the source  *****/
    CheckConsistency ();

    EnableModifications ();

    setIsInconsistent ();
    setIsASetTo (bStillASet);
    setElementCountTo (sTargetArray);
    StoreReference (pptPOP (), ptConstructor->NextGeneration ());
    ReallocateContainer (UV_SizeofUVector (sTargetArray, sElement));
    memcpy (arrayBase (), pTargetArray, sTargetArray * sElement);
    clearIsInconsistent ();

/*****  Clean up  the work areas  *****/
    UTIL_Free (pTargetArray);
    ptConstructor->discard ();

    return true;
}


/*******************************************************
 *****  Standard U-Vector Element Access Routines  *****
 *******************************************************/

/*****************************************************
 *  Link Constructor Subscripted Element Extraction  *
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Link Constructor Extracted U-Vector Initialization Routine.
 *
 *  Arguments:
 *	resultCPD		- an initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vector being
 *				  created.
 *	ap			- a <varargs.h> argument pointer positioned at
 *				  the following sequence of arguments:
 *
 *  Indirect (<varargs.h>) Arguments:
 *	sourceCPD		- a standard CPD for an ALIGNED u-vector from
 *				  which elements are to be extracted.
 *	linkc			- the address of an ALIGNED link constructor
 *				  specifying which elements of source are to
 *				  be extracted.  This routine performs no
 *				  checks on the referential relationship
 *				  between 'linkc' and 'source'.
 *	fillFn			- the address of an optional function which
 *				  will be called to fill gaps created by 'nil'
 *				  references.  If this function is specified,
 *				  (i.e., 'IsntNil (fillFn)'), it will be called
 *				  with a standard u-vector CPD whose element
 *				  pointer is positioned at the beginning of the
 *				  region to be filled, the number of elements
 *				  to fill, and a <varargs.h> argument pointer
 *				  positioned at 'fillFnArg1'.  This is the SAME
 *				  argument format as this routine!  If this
 *				  function is not specified, 'nil' reference
 *				  gaps will be zero filled.
 *	fillFnArg1, ...		- the first of an optional series of arguments
 *				  which will be passed to 'fillFn' if it is
 *				  specified.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on 'result' only.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *	This routine is written for use as an initialization function for
 *	'UV_New'.
 *
 *****/
PublicFnDef void UV_InitLCExtractedUV (
    M_CPD*			resultCPD,
    size_t			Unused(nelements),
    va_list			ap
) {
    V::VArgList iArgList (ap);
    M_CPD*		sourceCPD	= iArgList.arg<M_CPD*>();
    rtLINK_CType	*linkc		= iArgList.arg<rtLINK_CType*>();
    Ref_UV_Initializer	fillFn		= iArgList.arg<Ref_UV_Initializer>();
    int			granularity	= UV_CPD_Granularity (sourceCPD);
    char		*sBaseAsChar,
			*sPtrAsChar,
			*rPtrAsChar;
    int			*sBaseAsInt,
			*rPtrAsInt,
			elementAsInt;
    double		*sBaseAsDouble,
			*rPtrAsDouble,
			elementAsDouble;

/*****  Define nil, repeated, and range reference handler macros:  *****/
#define nilReferenceHandler(c, n, referenceNil)\
    UV_CPD_Element (resultCPD, char) = sPtrAsChar =\
	UV_CPD_Array (resultCPD, char) + c * granularity;\
    if (IsNil (fillFn))\
        memset (sPtrAsChar, 0, n * granularity);\
    else\
	(*fillFn)(resultCPD, n, ap)

#define generalRepeatedRefHandler(c, n, r)\
    sPtrAsChar = sBaseAsChar + r * granularity;\
    for (; n-- > 0; rPtrAsChar += granularity)\
        memcpy (rPtrAsChar, sPtrAsChar, granularity)

#define generalRangeRefHandler(c, n, r)\
    memcpy (rPtrAsChar, sBaseAsChar + r * granularity, n *= granularity);\
    rPtrAsChar += n

#define intRepeatedRefHandler(c, n, r)\
    for (elementAsInt = sBaseAsInt[r];\
	 n-- > 0;\
	 *rPtrAsInt++ = elementAsInt)

#define intRangeRefHandler(c, n, r)\
    memcpy (rPtrAsInt, sBaseAsInt + r, n * sizeof (int));\
    rPtrAsInt += n

#define doubleRepeatedRefHandler(c, n, r)\
    for (elementAsDouble = sBaseAsDouble[r];\
	 n-- > 0;\
	 *rPtrAsDouble++ = elementAsDouble)

#define doubleRangeRefHandler(c, n, r)\
    memcpy (rPtrAsDouble, sBaseAsDouble + r, n * sizeof (double));\
    rPtrAsDouble += n

/*****
 * Traverse the link constructor, optimizing common granularities ...
 *****/
    if (4 == granularity)
/*****  Granularity 4  (int, float, reference, date)  *****/
    {
	sBaseAsInt = UV_CPD_Array (sourceCPD, int);
	rPtrAsInt  = UV_CPD_Array (resultCPD, int);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     intRepeatedRefHandler,
	     intRangeRefHandler);
    }
    else if (8 == granularity)
/*****  Granularity 8  (double)  *****/
    {
	sBaseAsDouble = UV_CPD_Array (sourceCPD, double);
	rPtrAsDouble  = UV_CPD_Array (resultCPD, double);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     doubleRepeatedRefHandler,
	     doubleRangeRefHandler);
    }
    else
/*****  General Granularity  *****/
    {
	sBaseAsChar = UV_CPD_Array (sourceCPD, char);
	rPtrAsChar  = UV_CPD_Array (resultCPD, char);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     generalRepeatedRefHandler,
	     generalRangeRefHandler);
    }

/*****
 * Dispose of the nil, repeated, and range reference handler macros:
 *****/
#undef nilReferenceHandler
#undef generalRepeatedRefHandler
#undef generalRangeRefHandler
#undef intRepeatedRefHandler
#undef intRangeRefHandler
#undef doubleRepeatedRefHandler
#undef doubleRangeRefHandler
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
 *	linkc			- the address of a link constructor specifying
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
PublicFnDef M_CPD *UV_LCAssign (M_CPD *targetCPD, rtLINK_CType *linkc, M_CPD *sourceCPD) {
    int granularity = UV_CPD_Granularity (sourceCPD);
    char
	*sPtrAsChar, *tBaseAsChar;
    int
	*sPtrAsInt, *tBaseAsInt;
    double
	*sPtrAsDouble, *tBaseAsDouble;

/*****  Define nil, repeated, and range reference handler macros:  *****/
#define nilReferenceHandler(c, n, referenceNil)

#define generalRepeatedRefHandler(c, n, r)\
    memcpy (\
	tBaseAsChar + r * granularity,\
	(sPtrAsChar += n * granularity) - granularity,\
	granularity\
    )

#define generalRangeRefHandler(c, n, r)\
    memcpy (tBaseAsChar + r * granularity, sPtrAsChar, n *= granularity);\
    sPtrAsChar += n

#define intRepeatedRefHandler(c, n, r)\
    tBaseAsInt[r] = sPtrAsInt[n - 1];\
    sPtrAsInt += n

#define intRangeRefHandler(c, n, r)\
    memcpy (tBaseAsInt + r, sPtrAsInt, n * sizeof (int));\
    sPtrAsInt += n

#define doubleRepeatedRefHandler(c, n, r)\
    tBaseAsDouble[r] = sPtrAsDouble[n - 1];\
    sPtrAsDouble += n

#define doubleRangeRefHandler(c, n, r)\
    memcpy (tBaseAsDouble + r, sPtrAsDouble, n * sizeof (double));\
    sPtrAsDouble += n

/*****
 * Align the link constructor and validate its applicability as a subscript
 * for assignment of 'source' into 'target'.
 *****/
    linkc->AlignForAssign (targetCPD, UV_CPx_PToken, sourceCPD, UV_CPx_PToken);

/*****  Enable modification of the target  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

/*****
 * Traverse the link constructor, optimizing common granularities ...
 *****/
    if (4 == granularity)
/*****  Granularity 4  (int, float, reference, date)  *****/
    {
	tBaseAsInt = UV_CPD_Array (targetCPD, int);
	sPtrAsInt  = UV_CPD_Array (sourceCPD, int);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     intRepeatedRefHandler,
	     intRangeRefHandler);
    }
    else if (8 == granularity)
/*****  Granularity 8  (double)  *****/
    {
	tBaseAsDouble = UV_CPD_Array (targetCPD, double);
	sPtrAsDouble  = UV_CPD_Array (sourceCPD, double);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     doubleRepeatedRefHandler,
	     doubleRangeRefHandler);
    }
    else
/*****  General Granularity  *****/
    {
	tBaseAsChar = UV_CPD_Array (targetCPD, char);
	sPtrAsChar  = UV_CPD_Array (sourceCPD, char);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     generalRepeatedRefHandler,
	     generalRangeRefHandler);
    }

/*****
 * Dispose of the nil, repeated, and range reference handler macros:
 *****/
#undef nilReferenceHandler
#undef generalRepeatedRefHandler
#undef generalRangeRefHandler
#undef intRepeatedRefHandler
#undef intRangeRefHandler
#undef doubleRepeatedRefHandler
#undef doubleRangeRefHandler

/*****  Return 'targetCPD'  *****/
    return targetCPD;
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
 *	linkc			- the address of a link constructor specifying
 *				  the elements of 'target' to be updated.  The
 *				  link must be related positionally to 'source'
 *				  and referentially to 'target'.
 *	sourceValueAddress	- the address at which the scalar value is
 *				  stored.
 *
 *  Returns:
 *	'targetCPD'.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = *sourceValueAddress;
 *
 *****/
PublicFnDef M_CPD *UV_LCAssignScalar (
    M_CPD *targetCPD, rtLINK_CType *linkc, V::pointer_t sourceValueAddress
) {
    int			granularity	= UV_CPD_Granularity (targetCPD);
    char		*tBaseAsChar,
			*tPtrAsChar;
    int			*tBaseAsInt,
			*tPtrAsInt,
			sourceValueAsInt;
    double		*tBaseAsDouble,
			*tPtrAsDouble,
			sourceValueAsDouble;

/*****  Define nil, repeated, and range reference handler macros:  *****/
#define nilReferenceHandler(c, n, referenceNil)

#define generalRepeatedRefHandler(c, n, r)\
    memcpy (tBaseAsChar + r * granularity, sourceValueAddress, granularity)

#define generalRangeRefHandler(c, n, r)\
    for (tPtrAsChar = tBaseAsChar + r * granularity;\
         n-- > 0;\
	 tPtrAsChar += granularity)\
	memcpy (tPtrAsChar, sourceValueAddress, granularity)

#define intRepeatedRefHandler(c, n, r)\
    tBaseAsInt[r] = sourceValueAsInt

#define intRangeRefHandler(c, n, r)\
    for (tPtrAsInt = tBaseAsInt + r;\
         n-- > 0;\
	 *tPtrAsInt++ = sourceValueAsInt)

#define doubleRepeatedRefHandler(c, n, r)\
    tBaseAsDouble[r] = sourceValueAsDouble

#define doubleRangeRefHandler(c, n, r)\
    for (tPtrAsDouble = tBaseAsDouble + r;\
         n-- > 0;\
	 *tPtrAsDouble++ = sourceValueAsDouble)

/*****
 * Align the link constructor and validate its applicability as a subscript
 * for assignment of 'source' into 'target'.
 *****/
    linkc->AlignForAssign (targetCPD, UV_CPx_PToken);

/*****  Enable modification of the target  *****/
    targetCPD->EnableModifications ();
    UV_CPD_IsASetUV (targetCPD) = false;

/*****
 * Traverse the link constructor, optimizing common granularities ...
 *****/
    if (4 == granularity)
/*****  Granularity 4  (int, float, reference, date)  *****/
    {
	tBaseAsInt = UV_CPD_Array (targetCPD, int);
	sourceValueAsInt = *(int *)sourceValueAddress;
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     intRepeatedRefHandler,
	     intRangeRefHandler);
    }
    else if (8 == granularity)
/*****  Granularity 8  (double)  *****/
    {
	tBaseAsDouble = UV_CPD_Array (targetCPD, double);
	sourceValueAsDouble = *(double *)sourceValueAddress;
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     doubleRepeatedRefHandler,
	     doubleRangeRefHandler);
    }
    else
/*****  General Granularity  *****/
    {
	tBaseAsChar = UV_CPD_Array (targetCPD, char);
	rtLINK_TraverseRRDCList
	    (linkc,
	     nilReferenceHandler,
	     generalRepeatedRefHandler,
	     generalRangeRefHandler);
    }

/*****
 * Dispose of the nil, repeated, and range reference handler macros:
 *****/
#undef nilReferenceHandler
#undef generalRepeatedRefHandler
#undef generalRangeRefHandler
#undef intRepeatedRefHandler
#undef intRangeRefHandler
#undef doubleRepeatedRefHandler
#undef doubleRangeRefHandler

/*****  Return 'targetCPD'  *****/
    return targetCPD;
}


/*********************************************
 *****  Standard U-Vector Print Routine  *****
 *********************************************/

/*****  U-Vector Display Parameters  *****/
#define PrintLimitColumn 72

/*****  U-Vector Print Routine  *****/
/*---------------------------------------------------------------------------
 *****  Standard routine to print a u-vector.
 *
 *  Arguments:
 *	uvectorCPD		- a standard CPD for the u-vector to be
 *				  printed.
 *	recursive		- a boolean which, when true, requests the
 *				  display of the P-Token associated with this
 *				  U-Vector.
 *	elementPrinter		- a function which will is expected to print
 *				  the element at the current position of its
 *				  single standard CPD argument.  This routine
 *				  should return the number of characters it
 *				  actually displayed.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void UV_Print (
    M_CPD*			uvectorCPD,
    int				recursive,
    int				(*elementPrinter)(M_CPD*)
)
{
    int
	granularity	= UV_CPD_Granularity (uvectorCPD),
	elementCount	= UV_CPD_ElementCount (uvectorCPD),
	i, cc;

    IO_printf ("%s{", uvectorCPD->RTypeName ());

    if (recursive)
    {
	IO_printf ("\nPosPToken: ");
	RTYPE_RPrint (uvectorCPD, UV_CPx_PToken);
	IO_printf ("\nRefPToken: ");
	RTYPE_RPrint (uvectorCPD, UV_CPx_RefPToken);
	IO_printf ("\nAuxObject: ");
	RTYPE_RPrint (uvectorCPD, UV_CPx_AuxiliaryPOP);
    }

    IO_printf ("\n    ");

    for (i = cc = 0,
         UV_CPD_Element (uvectorCPD, char) =
	     UV_CPD_Array (uvectorCPD, char);
	 i < elementCount;
	 i++, UV_CPD_Element (uvectorCPD, char) += granularity)
	if ((cc += (*elementPrinter)(uvectorCPD)) > PrintLimitColumn)
	{
    	    cc = 0;
	    IO_printf ("\n    ");
	}
	else
	{
	    cc++;
	    IO_printf (" ");
	}
    if (cc > 0)
	IO_printf ("\n");
    IO_printf ("}");
}


/***************************************************
 ***** Standard U-Vector Manipulation Routines *****
 ***************************************************/

PublicFnDef void UV_NextElement (
    M_CPD*			uvectorCPD
)
{
    int
	granularity	    = UV_CPD_Granularity (uvectorCPD),
	elementCount	    = UV_CPD_ElementCount (uvectorCPD);
    V::pointer_t
	*elementPtrAddr	    = &UV_CPD_Element (uvectorCPD, char);


    if ((*elementPtrAddr += granularity) >=
        UV_CPD_Array (uvectorCPD, char) + granularity * elementCount)
    {
        *elementPtrAddr -= granularity;
	IO_printf (">>>  End of U-Vector  <<<\n");
    }
}

PublicFnDef void UV_GoToElement (
    M_CPD*			uvectorCPD,
    unsigned int		i
)
{
    if (i < UV_CPD_ElementCount (uvectorCPD))
	UV_CPD_Element (uvectorCPD, char) = 
	    UV_CPD_Array (uvectorCPD, char) + i * UV_CPD_Granularity (uvectorCPD);
}


/************************************************
 *****  Standard U-Vector Debugger Methods  *****
 ************************************************/

IOBJ_DefinePublicUnaryMethod (UV_DM_Align) {
    RTYPE_QRegisterHandle (self)->align ();
    return self;
}

IOBJ_DefinePublicUnaryMethod (UV_DM_PToken) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), UV_CPx_PToken);
}

IOBJ_DefinePublicUnaryMethod (UV_DM_RefPToken) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), UV_CPx_RefPToken);
}

IOBJ_DefinePublicUnaryMethod (UV_DM_AuxiliaryPOP) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), UV_CPx_AuxiliaryPOP);
}

IOBJ_DefinePublicUnaryMethod (UV_DM_Size) {
    return IOBJ_IntIObject (UV_CPD_ElementCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefinePublicUnaryMethod (UV_DM_Granularity) {
    return IOBJ_IntIObject (UV_CPD_Granularity (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefinePublicUnaryMethod (UV_DM_IsInconsistent) {
    return IOBJ_IntIObject (UV_CPD_IsInconsistent (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefinePublicUnaryMethod (UV_DM_IsASetUV) {
    return IOBJ_IntIObject (UV_CPD_IsASetUV (RTYPE_QRegisterCPD (self)));
}


IOBJ_DefinePublicUnaryMethod (UV_DM_NextElement) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);

    UV_NextElement (cpd);

    return self;
}

IOBJ_DefinePublicUnaryMethod (UV_DM_PreviousElement)
{
    M_CPD*	cpd = RTYPE_QRegisterCPD (self);
    int		granularity = UV_CPD_Granularity (cpd);
    V::pointer_t
		*elementPtrAddr = &UV_CPD_Element (cpd, char),
		arrayOrigin = UV_CPD_Array (cpd, char);

    if (*elementPtrAddr <= arrayOrigin)
    {
	IO_printf (">>>  Beginning of U-Vector  <<<\n");
	*elementPtrAddr = arrayOrigin;
    }
    else
	*elementPtrAddr -= granularity;

    return self;
}


IOBJ_DefinePublicMethod (UV_DM_GoToElement)
{
    M_CPD*		cpd = RTYPE_QRegisterCPD (self);
    unsigned int	i = IOBJ_NumericIObjectValue (parameterArray[0], unsigned int);

    UV_GoToElement (cpd, i);

    return self;
}

IOBJ_DefinePublicUnaryMethod (UV_DM_Position)
{
    M_CPD*	cpd = RTYPE_QRegisterCPD (self);

    return
	IOBJ_IntIObject
	    ((UV_CPD_Element(cpd, char) - UV_CPD_Array(cpd, char)) /
	     (unsigned int)UV_CPD_Granularity (cpd));
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (UV);
    FAC_FDFCase_FacilityDescription ("U-Vector");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  uvector.c 1 replace /users/mjc/system
  860323 17:20:57 mjc New U-Vector facility

 ************************************************************************/
/************************************************************************
  uvector.c 2 replace /users/mjc/system
  860410 19:20:06 mjc Added P-Token support

 ************************************************************************/
/************************************************************************
  uvector.c 3 replace /users/mjc/system
  860414 12:43:11 mjc Release version of 'uvector' consistent with RT{int,float,double}uv.c

 ************************************************************************/
/************************************************************************
  uvector.c 4 replace /users/mjc/system
  860420 18:49:04 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  uvector.c 5 replace /users/mjc/system
  860422 22:56:58 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  uvector.c 6 replace /users/hjb/system
  860423 00:39:57 hjb done updating calls

 ************************************************************************/
/************************************************************************
  uvector.c 7 replace /users/mjc/system
  860504 18:46:07 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  uvector.c 8 replace /users/jck/system
  860514 09:59:26 jck Align routine updated to use new P-Token interface

 ************************************************************************/
/************************************************************************
  uvector.c 9 replace /users/jad/system
  860514 16:15:54 jad added routines to implement the
stepValues method

 ************************************************************************/
/************************************************************************
  uvector.c 10 replace /users/mjc/system
  860521 11:11:33 mjc Fixed 'LCAssign' granularity 8 bug

 ************************************************************************/
/************************************************************************
  uvector.c 11 replace /users/mjc/system
  860523 00:57:38 mjc Added 'reference P-Token' field, fixed print CPD initialization bug

 ************************************************************************/
/************************************************************************
  uvector.c 12 replace /users/mjc/system
  860530 14:36:37 mjc Added a referential p-token to the creation protocol for all u-vectors

 ************************************************************************/
/************************************************************************
  uvector.c 13 replace /users/mjc/system
  860607 12:15:02 mjc Eliminated CPD copy on align

 ************************************************************************/
/************************************************************************
  uvector.c 14 replace /users/mjc/system
  860617 15:09:57 mjc Added and declared container save function

 ************************************************************************/
/************************************************************************
  uvector.c 15 replace /users/jck/system
  860625 20:56:05 jck Added M_EnableModifications call to Align routine

 ************************************************************************/
/************************************************************************
  uvector.c 16 replace /users/mjc/system
  860626 17:28:55 mjc Deleted 'UNWIND' traps

 ************************************************************************/
/************************************************************************
  uvector.c 17 replace /users/jad/system
  860723 16:13:12 jad added Copy routines

 ************************************************************************/
/************************************************************************
  uvector.c 18 replace /users/jad/system
  860723 16:42:38 jad added a size check on the new ptoken in copy

 ************************************************************************/
/************************************************************************
  uvector.c 19 replace /users/jad/system
  860805 16:58:28 jad fixed a bug in align where it didn't allocate
space for the uvector extra element

 ************************************************************************/
/************************************************************************
  uvector.c 20 replace /users/jad/system
  860814 14:17:55 jad use new ptoken IsntCurrent routines in align

 ************************************************************************/
/************************************************************************
  uvector.c 21 replace /users/jad/system
  860914 11:15:35 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  uvector.c 22 replace /users/mjc/system
  861002 18:37:37 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  uvector.c 23 replace /users/mjc/system
  861014 22:52:37 mjc Fixed default initializer to zero extra element

 ************************************************************************/
/************************************************************************
  uvector.c 24 replace /users/jad/system
  861216 17:38:53 jad added set uvectors

 ************************************************************************/
/************************************************************************
  uvector.c 25 replace /users/jad/system
  861222 17:45:42 jad turn off IsASetUV bit on most assigns and aligns

 ************************************************************************/
/************************************************************************
  uvector.c 26 replace /users/jck/system
  870415 09:53:58 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  uvector.c 27 replace /users/mjc/translation
  870524 09:42:40 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  uvector.c 28 replace /users/jck/system
  870730 09:30:48 jck Optimized UV_Align operation

 ************************************************************************/
/************************************************************************
  uvector.c 29 replace /users/jck/system
  871007 13:33:16 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  uvector.c 32 replace /users/jck/system
  880422 06:28:27 jck Inserting bug fix in pre sequence-link version

 ************************************************************************/
/************************************************************************
  uvector.c 33 replace /users/jck/system
  890222 15:01:29 jck Implemented corruption detection mechanism

 ************************************************************************/
/************************************************************************
  uvector.c 34 replace /users/m2/backend
  890503 15:35:37 m2 lint fix for undeclared parameter

 ************************************************************************/
