/*****  Undefined U-Vector Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtUNDEFUV

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
#include "RTrefuv.h"

#include "VCPDReference.h"

/*****  Self  *****/
#include "RTundefuv.h"


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

DEFINE_CONCRETE_RTT (rtUNDEFUV_Handle);


/***********************************
 *****  Standard Instantiator  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Standard UndefUV instantiation routine.
 *
 *  Arguments:
 *	posPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the initial positional state of
 *				  this U-Vector.
 *	refPTokenRefCPD/Index	- a reference to a POP for the P-Token which
 *				  will define the referential state of the
 *				  elements of this U-Vector.
 *
 *  Returns:
 *	A standard CPD for the UndefUV created.
 *
 *****/
PublicFnDef M_CPD *rtUNDEFUV_New (
    M_CPD*			posPTokenRefCPD,
    int				posPTokenRefIndex,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex
)
{
    VCPDReference pPPT (posPTokenRefCPD, posPTokenRefIndex, RTYPE_C_PToken);
    return UV_New (
	RTYPE_C_UndefUV,
	pPPT,
	refPTokenRefCPD,
	refPTokenRefIndex,
	0,
	NilOf (Ref_UV_Initializer),
	0
    );
}

PublicFnDef M_CPD *rtUNDEFUV_New (M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef) {
    return UV_New (
	RTYPE_C_UndefUV,
	pPPT,
	pRPTRef,
	xRPTRef,
	0,
	NilOf (Ref_UV_Initializer),
	0
    );
}


/*---------------------------------------------------------------------------
 *****  Standard UndefUV instantiation routine.
 *
 *  Arguments:
 *	posPToken		- the positional P-Token of the new uvector.
 *	refPToken		- the referential P-Token of the new uvector.
 *
 *  Returns:
 *	A standard CPD for the UndefUV created.
 *
 *****/
PublicFnDef M_CPD *rtUNDEFUV_New (M_CPD *posPToken, M_CPD *refPToken) {
    return UV_New (
	RTYPE_C_UndefUV,
	posPToken,
	refPToken,
	0,
	NilOf (Ref_UV_Initializer),
	0
    );
}


/*******************************
 *****  Alignment Routine  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally align a UndefUV.
 *
 *  Argument:
 *	cpd			- a CPD for the UndefUV to be aligned.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD* rtUNDEFUV_Align (M_CPD* cpd) {
    M_CPD* newPToken;

/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtUNDEFUV_Align", M_CPD_RType (cpd), RTYPE_C_UndefUV);

/*****  Align Positionally  *****/
    if (rtPTOKEN_IsCurrent (cpd, UV_CPx_PToken));
    else {
	cpd->EnableModifications ();
	newPToken = rtPTOKEN_BasePToken (cpd, UV_CPx_PToken);
	cpd->StoreReference (UV_CPx_PToken, newPToken);
	UV_CPD_ElementCount (cpd) = rtPTOKEN_CPD_BaseElementCount (newPToken);
	newPToken->release ();

/*****  Turn off the set bit if necessary ... *****/
	if (UV_CPD_ElementCount (cpd) > 1)
	    UV_CPD_IsASetUV (cpd) = false;
    }
/*****  Align Referentially  *****/
    if (rtPTOKEN_IsCurrent (cpd, UV_CPx_RefPToken))
	return cpd;

    cpd->EnableModifications ();
    newPToken = rtPTOKEN_BasePToken (cpd, UV_CPx_RefPToken);
    cpd->StoreReference (UV_CPx_RefPToken, newPToken);
    newPToken->release ();

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
 *  Notes:
 *	An undefined set uvector is defined to have either 0 or 1 element.
 *
 *****/
PublicFnDef void rtUNDEFUV_ToSetUV (
    M_ASD *pContainerSpace, M_CPD *sourceCPD, M_CPD **resultCPD, M_CPD **refuvCPD
) {
/***** Align the source uvector ... *****/
    rtUNDEFUV_Align (sourceCPD);
    unsigned int origSize = UV_CPD_ElementCount (sourceCPD);

/***** Create a new result uvector ... *****/
    M_CPD *newPosPToken = rtPTOKEN_New (pContainerSpace, origSize > 0 ? 1 : 0);
    *resultCPD = rtUNDEFUV_New (newPosPToken, sourceCPD, UV_CPx_RefPToken);
    UV_CPD_IsASetUV (*resultCPD) = true;

/***** Create the new reference uvector ... *****/
    *refuvCPD = rtREFUV_New (sourceCPD, UV_CPx_PToken, newPosPToken, -1);
    newPosPToken->release ();

/***** Fill in the reference uvector ... *****/
    rtREFUV_ElementType *refuvp = rtREFUV_CPD_Array (*refuvCPD);
    for (unsigned int i=0; i<origSize; i++)
	*refuvp++ = 0;

    return;
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
PublicFnDef M_CPD *rtUNDEFUV_Distribute (M_CPD *refuvCPD, M_CPD *sourceuvCPD) {
    VCPDReference pPPT (refuvCPD, UV_CPx_RefPToken, RTYPE_C_PToken);
    return rtUNDEFUV_UVAssign (
	rtUNDEFUV_New (pPPT, sourceuvCPD, UV_CPx_RefPToken), refuvCPD, sourceuvCPD
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
 *	sourceCPD		- a standard CPD for the UndefUV from which
 *				  the elements are to be extracted.
 *	linkConstructor		- the address of a link constructor specifying
 *				  the elements to be extracted.  The link
 *				  constructor must be closed and referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the UndefUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[link[i]];
 *
 *****/
PublicFnDef M_CPD *rtUNDEFUV_LCExtract (M_CPD *sourceCPD, rtLINK_CType *linkConstructor) {
/*****
 *  Align the link constructor and validate its applicability as an extraction
 *  subscript for 'source'
 *****/
    linkConstructor->AlignForExtract (rtUNDEFUV_Align (sourceCPD), UV_CPx_PToken);

/*****  Extract the requested values  *****/
    return rtUNDEFUV_New (linkConstructor->PPT (), sourceCPD, UV_CPx_RefPToken);
}


/*******************************************************
 *  Reference U-Vector Subscripted Element Extraction  *
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Reference U-Vector Subscripted Element Extraction Routine.
 *
 *  Arguments:
 *	sourceCPD		- a standard CPD for the UndefUV from which
 *				  the elements are to be extracted.
 *	refuvCPD		- a standard CPD for a reference U-Vector
 *				  specifying the elements to be extracted.
 *				  The reference U-Vector must be referentially
 *				  related to 'sourceCPD'.
 *
 *  Returns:
 *	A standard CPD for the UndefUV containing the extracted elements.
 *
 *  Notes:
 *	This routine performs the equivalent of:
 *	    for (i = 0; i < ElementCount (link); i++)
 *		result[i] = source[refuv[i]];
 *
 *****/
PublicFnDef M_CPD *rtUNDEFUV_UVExtract (M_CPD *sourceCPD, M_CPD *refuvCPD) {
/*****
 * Align 'source' and 'refuv' and validate 'refuv's applicability as an
 * extraction subscript for 'source'.
 *****/
    rtREFUV_AlignForExtract (
	rtUNDEFUV_Align (sourceCPD), UV_CPx_PToken, refuvCPD
    );

/*****  Extract and return the requested values  *****/
    return rtUNDEFUV_New (
	refuvCPD, UV_CPx_PToken, sourceCPD, UV_CPx_RefPToken
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
PublicFnDef void rtUNDEFUV_RFExtract (
    pointer_t Unused(resultAddr), M_CPD *sourceCPD, rtREFUV_TypePTR_Reference referenceAddr
)
{
/*****
 * Align 'source' and 'reference' and validate 'reference's applicability as an
 * extraction subscript for 'source'.
 *****/
    rtREFUV_AlignAndValidateRef (
	referenceAddr, rtUNDEFUV_Align (sourceCPD), UV_CPx_PToken
    );

/*****  Extract and return the requested value  *****/
/*----  That's It !!!  ----*/
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
PublicFnDef M_CPD *rtUNDEFUV_LCAssign (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, M_CPD *sourceCPD
) {
/*****  Align 'source' and 'target' and perform the assignment  *****/
    linkConstructor->AlignForAssign (
	rtUNDEFUV_Align (targetCPD), UV_CPx_PToken,
	rtUNDEFUV_Align (sourceCPD), UV_CPx_PToken
    );

    return targetCPD;
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
PublicFnDef M_CPD *rtUNDEFUV_UVAssign (
    M_CPD *targetCPD, M_CPD *refuvCPD, M_CPD *sourceCPD
)
{
/*****
 * Align 'source', 'target', and 'refuv' and validate the applicability of
 * 'refuv' as subscript for assignment of 'source' into 'target'.
 *****/
    rtREFUV_AlignForAssign (
	rtUNDEFUV_Align (targetCPD),
	UV_CPx_PToken,
	refuvCPD,
	rtUNDEFUV_Align (sourceCPD),
	UV_CPx_PToken
    );

/*****  Do the assignment and return  *****/

/*----  That's IT !!!  ----*/

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
PublicFnDef void rtUNDEFUV_RFAssign (
    M_CPD *targetCPD, rtREFUV_TypePTR_Reference reference, pointer_t Unused(value)
)
{
/*****
 * Align 'target' and 'reference' and validate the applicability of
 * 'reference' as subscript for assignment into 'target'.
 *****/
    rtREFUV_AlignAndValidateRef (
	reference, rtUNDEFUV_Align (targetCPD), UV_CPx_PToken
    );

/*****  Do the assignment and return  *****/
/*----  That's IT !!!  ----*/
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
PublicFnDef M_CPD *rtUNDEFUV_LCAssignScalar (
    M_CPD *targetCPD, rtLINK_CType *linkConstructor, pointer_t Unused(valuePtr)
) {
/*****  Align 'target' and perform the assignment  *****/
    linkConstructor->AlignForAssign (rtUNDEFUV_Align (targetCPD), UV_CPx_PToken);

    return targetCPD;
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
PublicFnDef M_CPD *rtUNDEFUV_UVAssignScalar (
    M_CPD *targetCPD, M_CPD *refuvCPD, pointer_t Unused(valuePtr)
)
{
/*****
 * Align 'target', and 'refuv' and validate the applicability of
 * 'refuv' as subscript for assignment into 'target'.
 *****/
    rtREFUV_AlignForScalarAssign (
	rtUNDEFUV_Align (targetCPD), UV_CPx_PToken, refuvCPD
    );

/*****  Do the assignment and return  *****/

/*----  That's IT !!!  ----*/

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
PublicFnDef M_CPD *rtUNDEFUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool Unused(descending)
) {
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtUNDEFUV_Align (values), UV_CPx_PToken);

/*****  Create the sort indices...  *****/
    M_CPD *result = rtREFUV_New (partition->PPT (), partition->PPT ());

/*****  Initialize the sort indices...  *****/
    rtREFUV_ElementType *resultPtr = rtREFUV_CPD_Array (result);
    unsigned int n = UV_CPD_ElementCount (result);
    for (unsigned int i = 0; i < n; *resultPtr++ = i++);

/*****  Return the computed sort indices...  *****/
    return result;
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
PublicFnDef void rtUNDEFUV_PartitndPartition (
    rtLINK_CType *partition, M_CPD *values, rtLINK_CType **majorLC, rtLINK_CType **minorLC
) {
/*---------------------------------------------------------------------------*
 *****  Link Traversal Component Handler Macro Definitions
 *---------------------------------------------------------------------------*/
#define handleRange(position, count, reference)\
    rtLINK_AppendRange (majorPartition, reference, count);\
    rtLINK_AppendRange (minorPartition, minorCount, count);\
    minorCount += count

#define handleRepetition(position, count, reference)\
    rtLINK_AppendRepeat (minorPartition, minorCount++, count);\
    rtLINK_AppendRepeat (majorPartition, reference, 1)

/*---------------------------------------------------------------------------
 *****  Algorithm
 *---------------------------------------------------------------------------*/
/*****  Align and validate parameters...  *****/
    partition->AlignForDistribute (rtUNDEFUV_Align (values), UV_CPx_PToken);

/*****  Create the major and minor partitions...  *****/
    rtLINK_CType *majorPartition = rtLINK_RefConstructor (partition->RPT (), -1);
    rtLINK_CType *minorPartition = rtLINK_PosConstructor (partition->PPT (), -1);

/*****  Initialize the traversal pointers...  *****/
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
 *****  Routine to locate or add a set of values to a undefined set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for a undefined set uvector for the
 *                        uvector to be added to.
 *	sourceCPD	- a standard CPD for a undefined set uvector containing
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
 *	By definition undefined set uvectors may contain either 0 or 1 element.
 *	Any 'undefined' element is equal to any other 'undefined' element.
 *
 *	If 'addedLinkC' is constructed, it's domain will reside in the key's
 *	scratch pad.
 *
 *****/
PublicFnDef void rtUNDEFUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC
) {
/***** Make sure both the source and target are undefined set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_UndefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_LocateOrAdd:  Source UVector Must Be An Undefined Set"
    );

    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_UndefUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_LocateOrAdd:  Target UVector Must Be An Undefined Set"
    );

/***** Align and validate source and target uvectors ... *****/
    rtUNDEFUV_Align (sourceCPD);
    rtUNDEFUV_Align (targetCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, targetCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_LocateOrAdd:  Referential Inconsistency"
    );

/***** Make the link constructors ... *****/
    bool wantAddedLinkC = IsntNil (addedLinkC);
    if (wantAddedLinkC)
	*addedLinkC = NilOf (rtLINK_CType *);

    *locationsLinkC = rtLINK_PosConstructor (sourceCPD, UV_CPx_PToken);

/***** If the source is empty, close the locationsLinkC and return ... *****/
    if (UV_CPD_ElementCount (sourceCPD) == 0) {
	(*locationsLinkC)->Close (targetCPD, UV_CPx_PToken);
	return;
    }

/*****  ... otherwise the source contains one element ... *****/

/***** ... if the target is empty ... *****/
/*****  Add one element to the target ... *****/
    if (UV_CPD_ElementCount (targetCPD) == 0) {
        rtPTOKEN_CType *targetPTokenC = rtPTOKEN_NewShiftPTConstructor (
	    targetCPD, UV_CPx_PToken
	);
	targetPTokenC->AppendAdjustment (0, 1);
	M_CPD *ptoken = targetPTokenC->ToPToken ();
	targetCPD->StoreReference (UV_CPx_PToken, ptoken);
	UV_CPD_ElementCount (targetCPD) = 1;
	ptoken->release ();

	/***  Show that the value was added ... ***/
	if (wantAddedLinkC) {
	    *addedLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);
	    rtLINK_AppendRange (*addedLinkC, 0, 1);
	    ptoken = rtPTOKEN_New (sourceCPD->ScratchPad (), 1);
	    (*addedLinkC)->Close (ptoken);
	    ptoken->release ();
	}
    }

/*****  Put the position into 'locationsLinkC' ... *****/
    rtLINK_AppendRange (*locationsLinkC, 0, 1);
    (*locationsLinkC)->Close (targetCPD, UV_CPx_PToken);
}


/*---------------------------------------------------------------------------
 *****  Routine to locate or add a value to a undefined set uvector.
 *
 *  Arguments:
 *	targetCPD	- a standard CPD for the undefined set uvector which
 *                        the value is being located in or added to.
 *	locationPtr	- the address of a scalar value which this routine
 *                        will set to contain the position in 'targetCPD' at
 *                        which the value was found or added.
 *
 *  Returns:
 *	true if the value was added, false otherwise.
 *
 *  Notes:
 *	The value actually being located or added is irrelevant since all
 *	undefined values are equal.
 *
 *****/
PublicFnDef bool rtUNDEFUV_ScalarLocateOrAdd (M_CPD *targetCPD, int *locationPtr) {
/***** Make sure the target is a undefined set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (targetCPD) != RTYPE_C_UndefUV) ||
	!UV_CPD_IsASetUV (targetCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_ScalarLocateOrAdd: Target UVector Must Be An Undefined Set"
    );

/***** Align target uvector ... *****/
    rtUNDEFUV_Align (targetCPD);

/*****  If the target is empty ... *****/
/*****	add one element to the target ... *****/
    if (UV_CPD_ElementCount (targetCPD) == 0) {
        rtPTOKEN_CType *targetPTokenC = rtPTOKEN_NewShiftPTConstructor (
	    targetCPD, UV_CPx_PToken
	);
	targetPTokenC->AppendAdjustment (0, 1);
	M_CPD *ptoken = targetPTokenC->ToPToken ();
	targetCPD->StoreReference (UV_CPx_PToken, ptoken);
	UV_CPD_ElementCount (targetCPD) = 1;
	ptoken->release ();

	*locationPtr = 0;
	return true;
    }
/*****  ... otherwise the value is found at position 0 ... *****/
    else {
	*locationPtr = 0;
	return false;
    }
}


/****************************
 *****  Lookup Routine  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup values in a undefined set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the undefined set uvector to be
 *			  be searched.
 *	keyCPD		- a standard CPD for the undefined set uvector
 *			  containing the values to lookup in the 'sourceCPD'.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ,
 *			          rtLINK_LookupCase_GE, or
 *				  rtLINK_LookupCase_GT.
 *			  Currently unused.
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
 *  Notes:
 *	By definition undefined set uvectors can have either 0 or 1 elements.
 *
 *	Since all undefined values are equal - all of the 'rtLINK_LookupCase's
 *	are equivalent and not necessary as an argument to this function.
 *
 *****/
PublicFnDef void rtUNDEFUV_Lookup (
    M_CPD			*sourceCPD,
    M_CPD			*keyCPD,
    rtLINK_LookupCase		Unused(lookupCase),
    rtLINK_CType		**locationsLinkC,
    rtLINK_CType		**locatedLinkC
)
{
/***** Make sure both the source and key are undefined set uvectors ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_UndefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_Lookup:  Source UVector Must Be An Undefined Set"
    );

    if (((RTYPE_Type)M_CPD_RType (keyCPD) != RTYPE_C_UndefUV) ||
        !UV_CPD_IsASetUV (keyCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_Lookup:  Key UVector Must Be An Undefined Set"
    );

/***** Align and validate source and key uvectors ... *****/
    rtUNDEFUV_Align (sourceCPD);
    rtUNDEFUV_Align (keyCPD);
    if (sourceCPD->ReferenceDoesntName (UV_CPx_RefPToken, keyCPD, UV_CPx_RefPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_Lookup:  Referential Inconsistency"
    );

/***** Make the location link constructor ... *****/
    *locationsLinkC = rtLINK_RefConstructor (sourceCPD, UV_CPx_PToken);

/***** If the key is empty ... *****/
    if (UV_CPD_ElementCount (keyCPD) == 0)
    {
	/*** locations = empty ***/
	(*locationsLinkC)->Close (keyCPD, UV_CPx_PToken);

	/*** located = Nil ***/
        *locatedLinkC = NilOf (rtLINK_CType *);
	return;
    }

/***** ... otherwise the key has one value ... *****/
/***** If the source is empty, the value is NOT found ... *****/
    if (UV_CPD_ElementCount (sourceCPD) == 0) {
	/*** locations = empty ***/
	M_CPD *ptoken = rtPTOKEN_New (keyCPD->Space (), 0);
	(*locationsLinkC)->Close (ptoken);

	/*** located = empty ***/
	*locatedLinkC = rtLINK_RefConstructor (keyCPD, UV_CPx_PToken);
	(*locatedLinkC)->Close (ptoken);

	ptoken->release ();
    }
/*****  ... else the value is found ... *****/
    else {
	/*** locations = position 0 ***/
	rtLINK_AppendRange (*locationsLinkC, 0, 1);
	(*locationsLinkC)->Close (keyCPD, UV_CPx_PToken);

	/*** located = Nil (all values were found) ***/
	*locatedLinkC = NilOf (rtLINK_CType *);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to lookup a value in a undefined set uvector.
 *
 *  Arguments:
 *	sourceCPD	- a standard CPD for the undefined set uvector to be
 *			  be searched.
 *	lookupCase	- either: rtLINK_LookupCase_LE,
 *			          rtLINK_LookupCase_EQ,
 *			          rtLINK_LookupCase_GE, or
 *				  rtLINK_LookupCase_GT.
 *			  Currently unused.
 *	locationPtr 	- the address of a scalar which this routine will set
 *			  to the location in the source where the looked up
 *                        value was found.  If the value was not found,
 *                        'locationPtr' will be untouched.
 *
 *  Returns:
 *	true if the value was found, false otherwise.
 *
 *  Notes:
 *	The value actually being looked up is irrelevant since all
 *	undefined values are equal.
 *
 *****/
PublicFnDef bool rtUNDEFUV_ScalarLookup (
    M_CPD			*sourceCPD,
    rtLINK_LookupCase		Unused(lookupCase),
    int				*locationPtr
)
{
/***** Make sure the source is a undefined set uvector ... *****/
    if (((RTYPE_Type)M_CPD_RType (sourceCPD) != RTYPE_C_UndefUV) ||
	!UV_CPD_IsASetUV (sourceCPD)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtUNDEFUV_Lookup:  Source UVector Must Be An Undefined Set"
    );

/***** Align the source uvector ... *****/
    rtUNDEFUV_Align (sourceCPD);

/*****  If source is empty ...  *****/
    if (UV_CPD_ElementCount (sourceCPD) == 0)
        return false;

/*****  Otherwise the value is found ... *****/
    *locationPtr = 0;
    return true;
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/********************
 *  Element Printer  *
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
PrivateFnDef int PrintElement (M_CPD *Unused(cpd)) {
    return IO_printf ("-");
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	rtUNDEFUV_New (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}

IOBJ_DefineNewaryMethod (NewDistributeDM) {
    return RTYPE_QRegister (
	rtUNDEFUV_Distribute (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}


/************************
 *  'Instance' Methods  *
 ************************/

UV_DefineEPrintDM (PrintElementDM, PrintElement)

UV_DefineAlignDM (AlignDM, rtUNDEFUV_Align)

UV_DefineAtDM (AtDM, rtUNDEFUV_LCExtract, rtUNDEFUV_UVExtract)

UV_DefineAtPutDM (AtPutDM, rtUNDEFUV_LCAssign, rtUNDEFUV_UVAssign)

/***  Since this method needs to return 2 uvectors,
 ***	the new set uvector is returned as self and
 ***	the refuv uvector can be accessed by: setFromLast
 ***/
IOBJ_DefineUnaryMethod (ToSetUVDM) {
    M_CPD *resultCPD, *refuvCPD;
    rtUNDEFUV_ToSetUV (IOBJ_ScratchPad, RTYPE_QRegisterCPD (self), &resultCPD, &refuvCPD);

    IOBJ_IObject saveresult = RTYPE_QRegister (resultCPD);
    RTYPE_QRegister (refuvCPD);

    return saveresult;
}

IOBJ_DefineUnaryMethod (DirectToSetUVDM)
{
    M_CPD* undefuv = RTYPE_QRegisterCPD (self);

    if (UV_CPD_ElementCount (undefuv) <= 1) {
	undefuv->EnableModifications ();
        UV_CPD_IsASetUV (undefuv) = true;
    }

    return self;
}

IOBJ_DefineMethod (LocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_CType *locationsLinkC, *addedLinkC;
    rtUNDEFUV_LocateOrAdd (targetCPD, sourceCPD, &locationsLinkC, &addedLinkC);
    if (addedLinkC) {
	IO_printf ("\nAdded LinkC:\n");
	addedLinkC->DebugPrint ();
	addedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineUnaryMethod (ScalarLocateOrAddDM) {
    M_CPD *targetCPD = RTYPE_QRegisterCPD (self);

    int location;
    if (rtUNDEFUV_ScalarLocateOrAdd (targetCPD, &location))
	IO_printf ("Scalar Value ADDED at position %d\n", location);
    else
	IO_printf ("Scalar Value found at position %d\n", location);

    return self;
}

IOBJ_DefineMethod (LookupDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);
    M_CPD *keyCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    rtLINK_CType *locationsLinkC, *locatedLinkC;
    rtUNDEFUV_Lookup (
	sourceCPD, keyCPD, rtLINK_LookupCase_EQ, &locationsLinkC, &locatedLinkC
    );
    if (locatedLinkC) {
	IO_printf ("\nLocated LinkC:\n");
	locatedLinkC->DebugPrint ();
	locatedLinkC->release ();
    }

    return RTYPE_QRegister (locationsLinkC);
}

IOBJ_DefineUnaryMethod (ScalarLookupDM) {
    M_CPD *sourceCPD = RTYPE_QRegisterCPD (self);

    int location;
    if (rtUNDEFUV_ScalarLookup (sourceCPD, rtLINK_LookupCase_EQ, &location))
	IO_printf ("Scalar Lookup Found Value at position %d\n", location);
    else
	IO_printf ("Scalar Lookup value not found\n");

    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtUNDEFUV_Handler) {
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
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("at:put:"		, AtPutDM)
	IOBJ_MDE ("toSetUV"		, ToSetUVDM)
	IOBJ_MDE ("markAsSet"		, DirectToSetUVDM)
	IOBJ_MDE ("locateOrAdd:"	, LocateOrAddDM)
	IOBJ_MDE ("scalarLocateOrAdd"	, ScalarLocateOrAddDM)
	IOBJ_MDE ("lookup:"		, LookupDM)
	IOBJ_MDE ("scalarLookup"	, ScalarLookupDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(UV_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = UV_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtUNDEFUV_Handle::Maker;
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
	UV_Print (rtUNDEFUV_Align(iArgList.arg<M_CPD*>()), false, PrintElement);
        break;
    case RTYPE_RPrintObject:
	UV_Print (rtUNDEFUV_Align(iArgList.arg<M_CPD*>()), true, PrintElement);
        break;
    case RTYPE_DoLCExtract:
        resultCPD   = iArgList.arg<M_CPD**>();
	sourceCPD   = iArgList.arg<M_CPD*>();
	linkc	    = iArgList.arg<rtLINK_CType*>();
	*resultCPD  = rtUNDEFUV_LCExtract (sourceCPD, linkc);
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
    FAC_FDFCase_FacilityIdAsString (rtUNDEFUV);
    FAC_FDFCase_FacilityDescription ("Undefined U-Vector Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTundefuv.c 1 replace /users/mjc/system
  860516 18:10:31 mjc Create 'undefined value' u-vectors

 ************************************************************************/
/************************************************************************
  RTundefuv.c 2 replace /users/mjc/system
  860521 11:10:20 mjc Release for coordination with jck

 ************************************************************************/
/************************************************************************
  RTundefuv.c 3 replace /users/mjc/system
  860530 14:37:00 mjc Added a referential p-token to the creation protocol for all u-vectors

 ************************************************************************/
/************************************************************************
  RTundefuv.c 4 replace /users/mjc/system
  860530 17:06:23 mjc Release 'RF{Extract,Assign}' 

 ************************************************************************/
/************************************************************************
  RTundefuv.c 5 replace /users/mjc/system
  860606 13:25:05 mjc Removed 'New' patch to ignore reference P-Token

 ************************************************************************/
/************************************************************************
  RTundefuv.c 6 replace /users/mjc/system
  860617 15:39:04 mjc Declared save routine

 ************************************************************************/
/************************************************************************
  RTundefuv.c 7 replace /users/jad/system
  860718 11:24:10 jad added a Distribute method

 ************************************************************************/
/************************************************************************
  RTundefuv.c 8 replace /users/jad/system
  860828 14:58:33 jad wrote its own align routine which is simpler
than the general uvector one

 ************************************************************************/
/************************************************************************
  RTundefuv.c 9 replace /users/mjc/system
  861015 23:53:11 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTundefuv.c 10 replace /users/mjc/system
  861020 10:38:26 mjc Fixed transposed reference arguments in range case of 'PartitionedPartition'

 ************************************************************************/
/************************************************************************
  RTundefuv.c 11 replace /users/jad/system
  870429 16:50:25 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTundefuv.c 12 replace /users/jad/system
  870501 18:55:28 jad turn off IsASet bit on some aligns &
make sure to enable modifications before turning off the bit

 ************************************************************************/
/************************************************************************
  RTundefuv.c 13 replace /users/mjc/translation
  870524 09:36:06 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTundefuv.c 14 replace /users/jck/system
  871007 13:34:50 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTundefuv.c 15 replace /users/jck/system
  880916 14:05:22 jck Added a debugger method to mark an undefuv as a set

 ************************************************************************/
/************************************************************************
  RTundefuv.c 16 replace /users/jck/system
  881104 08:00:30 jck Fixed bug in the markAsSet debugger method

 ************************************************************************/
/************************************************************************
  RTundefuv.c 17 replace /users/m2/backend
  890927 15:05:09 m2 Changed major and minor variables because of SUN macro confilct, dummyiobj added

 ************************************************************************/
