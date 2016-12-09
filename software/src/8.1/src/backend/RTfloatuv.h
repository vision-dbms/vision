/*****  Float U-Vector Representation Type Header File  *****/
#ifndef rtFLOATUV_H
#define rtFLOATUV_H

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

#include "RTfloatuv.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

 /*---------------------------------------------------------------------------
 * Float U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtFLOATUV_CPD_Array(cpd) UV_CPD_Array (cpd, rtFLOATUV_ElementType)

#define rtFLOATUV_CPD_Element(cpd) M_CPD_PointerToSmallReal (\
    cpd, UV_CPx_Element\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtFLOATUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtFLOATUV_Handle, rtUVECTOR_Handle);

//  Aliases
public:
    typedef rtFLOATUV_ElementType element_t;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtFLOATUV_Handle (rCTE);
    }
protected:
    rtFLOATUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

//  Destruction
private:
    ~rtFLOATUV_Handle () {
    }

//  Access
public:
    element_t *array () const {
	return UV_UV_ArrayAsType (typecastContent (), element_t);
    }
    element_t element (unsigned int xElement) const {
	return array ()[xElement];
    }

//  Alignment
public:
    using BaseClass::align;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD* __cdecl rtFLOATUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT, Ref_UV_Initializer initFn, ...
);

PublicFnDecl M_CPD *rtFLOATUV_New (
    rtPTOKEN_Handle *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtFLOATUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT
);

PublicFnDecl void rtFLOATUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl M_CPD* rtFLOATUV_AsDoubleUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtFLOATUV_AsIntUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD,
    rtLINK_CType		**ppDDLC
);

PublicFnDecl M_CPD* rtFLOATUV_AsIntUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtFLOATUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD* rtFLOATUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD* rtFLOATUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtFLOATUV_RFExtract (
    rtFLOATUV_ElementType	*resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD* rtFLOATUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtFLOATUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtFLOATUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtFLOATUV_ElementType	*value
);

PublicFnDecl M_CPD* rtFLOATUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtFLOATUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtFLOATUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtFLOATUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtFLOATUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl void rtFLOATUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtFLOATUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtFLOATUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtFLOATUV_ElementType	*sourcePtr,
    int				*locationPtr
);

PublicFnDecl void rtFLOATUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtFLOATUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtFLOATUV_ElementType	*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTfloatuv.h 1 replace /users/mjc/system
  860323 17:19:25 mjc New U-Vector representation types

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 2 replace /users/mjc/system
  860422 22:55:35 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 3 replace /users/mjc/system
  860504 18:45:28 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 4 replace /users/mjc/system
  860530 17:50:39 mjc Added 'RF{Extract,Assign}' routines

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 5 replace /users/mjc/system
  861015 23:52:54 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 6 replace /users/jad/system
  870429 16:51:30 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTfloatuv.h 7 replace /users/mjc/translation
  870524 09:32:53 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
