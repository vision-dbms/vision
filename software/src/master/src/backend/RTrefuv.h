/*****  Reference U-Vector Representation Type Header File  *****/
#ifndef rtREFUV_H
#define rtREFUV_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "uvector.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTrefuv.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Scalar.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 * Reference U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtREFUV_CPD_ReferencePToken(cpd) UV_CPD_RefPToken (cpd)

#define rtREFUV_CPD_Array(cpd) UV_CPD_Array (cpd, rtREFUV_ElementType)

#define rtREFUV_CPD_Element(cpd) M_CPD_PointerToInteger (\
    cpd, UV_CPx_Element\
)


/****************************
 ****************************
 *****  Reference Type  *****
 ****************************
 ****************************/

/*---------------------------------------------------------------------------
 * A 'Reference' represents a disembodied scalar form of a reference u-vector
 * element.  'Reference's are provided as an optimization used by the virtual
 * machine and some of the types it employs.  The availability of a reference
 * enables a number of high frequency scalar operations to be implemented
 * efficiently.  References are implemented as 'DSC_Scalar' objects.
 *---------------------------------------------------------------------------
 */
typedef class DSC_Scalar rtREFUV_Type_Reference, *rtREFUV_TypePTR_Reference;

/*****  Direct Access Macros  *****/
#define rtREFUV_Ref_D_Element(ref)	DSC_Scalar_Int (ref)

/*****  Indirect Access Macros  *****/
#define rtREFUV_Ref_Element(p)		rtREFUV_Ref_D_Element(*(p))


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtREFUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtREFUV_Handle, rtUVECTOR_Handle);

//  Aliases
public:
    typedef /*rtREFUV_ElementType*/ unsigned int element_t;

//  Alignment Fill Processor
public:
    class AlignmentProcessor;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtREFUV_Handle (rCTE);
    }
protected:
    rtREFUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

//  Destruction
private:
    ~rtREFUV_Handle () {
    }

//  Alignment
private:
    virtual /*override*/ bool align_() {
	return align ();
    }
public:
    bool align (bool bReferentialAlignmentRequired = true);

//  Access
public:
    element_t *array () const {
	return UV_UV_ArrayAsType (typecastContent (), element_t);
    }
    element_t element (unsigned int xElement) const {
	return xElement < elementCount () ? array ()[xElement] : referenceNil ();
    }
    element_t referenceNil () const {
	return rtPTOKEN_BaseElementCount (this, rptPOP ());
    }
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/********************
 *****  Macros  *****
 ********************/

/***** Macro to Create a New U-Vector by Distributing the Source into the
 ***** New U-Vector.
 *
 *  Arguments:
 *	newuvCPD	- a standard CPD for the new U-Vector to be created.
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
#define rtREFUV_LCDistribute(newuvCPD, referenceLC, sourceuvCPD) rtREFUV_LCAssign (\
    (newuvCPD = rtREFUV_New (referenceLC->RPT (), sourceuvCPD, UV_CPx_RefPToken)),\
    referenceLC, sourceuvCPD\
)


/**************************************
 *****  Functions and Procedures  *****
 **************************************/

PublicFnDecl M_CPD *__cdecl rtREFUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT, Ref_UV_Initializer initFn, ...
);

PublicFnDecl M_CPD *rtREFUV_New (
    rtPTOKEN_Handle *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD *rtREFUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT
);

PublicFnDecl M_CPD *rtREFUV_NewInitWithPrtitnBounds (
    rtLINK_CType*		partition
);

PublicFnDecl void rtREFUV_ComplainAboutElementVal (
    int				element,
    int				upperBound
);

PublicFnDecl rtREFUV_TypePTR_Reference rtREFUV_AlignReference (
    rtREFUV_TypePTR_Reference	refp
);

PublicFnDecl void rtREFUV_AlignAndValidateRef (
    DSC_Scalar *refp, M_CPD *pTokenRef, int xTokenRef
);
PublicFnDecl void rtREFUV_AlignAndValidateRef (
    DSC_Scalar *refp, VContainerHandle *pTokenRef, M_POP const *pTokenPOP
);

PublicFnDecl void rtREFUV_AlignForExtract (
    M_CPD*			sPTokenRefCPD,
    int				sPTokenRefIndex,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtREFUV_AlignForExtract (
    rtPTOKEN_Handle*		sPToken,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtREFUV_AlignForAssign (
    M_CPD*			tPTokenRefCPD,
    int				tPTokenRefIndex,
    M_CPD*			refuvCPD,
    M_CPD*			sPTokenRefCPD,
    int				sPTokenRefIndex
);

PublicFnDecl void rtREFUV_AlignForAssign (
    M_CPD*			tPTokenRefCPD,
    int				tPTokenRefIndex,
    M_CPD*			refuvCPD,
    rtPTOKEN_Handle*		sPToken
);

PublicFnDecl void rtREFUV_AlignForAssign (
    rtPTOKEN_Handle*		tPToken,
    M_CPD*			refuvCPD,
    rtPTOKEN_Handle*		sPToken
);

PublicFnDecl void rtREFUV_AlignForScalarAssign (
    M_CPD*			tPTokenRefCPD,
    int				tPTokenRefIndex,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtREFUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl int rtREFUV_IsASet (
    M_CPD*			refuvCPD,
    rtLINK_CType*		*refNilsLC
);

PublicFnDecl int rtREFUV_RestoreSetAttribute (
    M_CPD*			refuvCPD
);

PublicFnDecl M_CPD *rtREFUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD *rtREFUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD *rtREFUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtREFUV_RFExtract (
    rtREFUV_TypePTR_Reference	resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD *rtREFUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtREFUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtREFUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtREFUV_TypePTR_Reference	value
);

PublicFnDecl M_CPD *rtREFUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtREFUV_TypePTR_Reference	valuePtr
);

PublicFnDecl M_CPD *rtREFUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtREFUV_TypePTR_Reference	valuePtr
);

PublicFnDecl M_CPD *rtREFUV_AscendingSortIndices (
    M_CPD *refuvCPD, bool fResultRequired = true
);

PublicFnDecl M_CPD *rtREFUV_Flip (M_CPD *refuvCPD);

PublicFnDecl M_CPD *rtREFUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl M_CPD *rtREFUV_PartitndAssign (
    M_CPD*			target,
    M_CPD*			origins,
    rtLINK_CType*		partition,
    M_CPD*			source
);

PublicFnDecl void rtREFUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtREFUV_LinearizeRrRc (
    rtREFUV_TypePTR_Reference	resultRefp,
    rtPTOKEN_Handle*		cartesianPT,
    rtREFUV_TypePTR_Reference	rowRefp,
    rtREFUV_TypePTR_Reference	columnRefp
);

PublicFnDecl void rtREFUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtREFUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtREFUV_ElementType		*sourcePtr,
    int				*locationPtr
);

PublicFnDecl void rtREFUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtREFUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtREFUV_ElementType		*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);

PublicFnDecl int rtREFUV_DeleteNils (
    M_CPD*			pTargetCPD
);

PublicFnDecl void rtREFUV_PartitionedLocateOrAdd (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    M_CPD*			pSource,
    rtLINK_CType*&		pTargetReference,
    rtLINK_CType*&		pTargetReferenceAdditions,
    M_CPD*&			pTargetReferenceDistribution
);

PublicFnDecl void rtREFUV_PartitionedLocateOrAdd (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    rtREFUV_Type_Reference&	pSource,
    rtLINK_CType*&		pTargetReference,
    rtLINK_CType*&		pTargetReferenceAdditions
);

PublicFnDecl void rtREFUV_PartitionedLocateOrAdd (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource,
    rtREFUV_Type_Reference&	pTargetReference,
    bool&			fTargetReferenceNotFound
);

PublicFnDecl void rtREFUV_PartitionedLocate (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    M_CPD*			pSource,
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType*&		pTargetReference,
    rtLINK_CType*&		pTargetReferenceGuard,
    M_CPD*&			pTargetReferenceDistribution
);

PublicFnDecl void rtREFUV_PartitionedLocate (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    rtREFUV_Type_Reference&	pSource,
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType*&		pTargetReference,
    rtLINK_CType*&		pTargetReferenceGuard
);

PublicFnDecl bool rtREFUV_PartitionedLocate (
    M_CPD*			pTarget,
    unsigned int*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource,
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	pTargetReference
);

PublicFnDecl void rtREFUV_PartitionedDelete (
    M_CPD*			pTarget,
    unsigned int const*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    M_CPD*			pSource
);

PublicFnDecl void rtREFUV_PartitionedDelete (
    M_CPD*			pTarget,
    unsigned int*		pTargetPartition,
    rtLINK_CType*		pSourcePartition,
    rtREFUV_Type_Reference&	pSource
);

PublicFnDecl void rtREFUV_PartitionedDelete (
    M_CPD*			pTarget,
    unsigned int*		pTargetPartition,
    rtREFUV_Type_Reference&	pSourcePartition,
    rtREFUV_Type_Reference&	pSource
);

PublicFnDecl void rtREFUV_SetCodomainDomainRatio (
    double			ratio
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTrefuv.h 1 replace /users/mjc/system
  860404 20:14:31 mjc Create Reference U-Vector Representation Type

 ************************************************************************/
/************************************************************************
  RTrefuv.h 2 replace /users/mjc/system
  860422 22:56:41 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTrefuv.h 3 replace /users/mjc/system
  860504 18:45:41 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTrefuv.h 4 replace /users/mjc/system
  860530 09:22:48 mjc Added 'Reference' type and alignment routines to deal with it

 ************************************************************************/
/************************************************************************
  RTrefuv.h 5 replace /users/mjc/system
  860530 23:40:40 mjc Release 'RF{Extract/Assign}' (untested)

 ************************************************************************/
/************************************************************************
  RTrefuv.h 6 replace /users/jad/system
  860617 12:21:17 jad fully implemented refuv and reference align

 ************************************************************************/
/************************************************************************
  RTrefuv.h 7 replace /users/mjc/system
  860623 10:55:21 mjc Added '{LC,UV}AssignScalar' routines (untested)

 ************************************************************************/
/************************************************************************
  RTrefuv.h 8 replace /users/mjc/system
  860709 10:14:52 mjc Release new format 'reference's for continued development by 'jad'

 ************************************************************************/
/************************************************************************
  RTrefuv.h 9 replace /users/jad/system
  860718 20:15:52 jad added rtREFUV_Flip 

 ************************************************************************/
/************************************************************************
  RTrefuv.h 10 replace /users/mjc/system
  861015 23:53:07 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTrefuv.h 11 replace /users/jad/system
  870303 17:34:10 jad added a reference linearize routine

 ************************************************************************/
/************************************************************************
  RTrefuv.h 12 replace /users/jad/system
  870429 16:51:21 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTrefuv.h 13 replace /users/mjc/translation
  870524 09:35:01 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
