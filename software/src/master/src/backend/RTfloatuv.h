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


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD* __cdecl rtFLOATUV_New (
    M_CPD*			pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    Ref_UV_Initializer		initFn,
    ...
);

PublicFnDecl M_CPD *rtFLOATUV_New (
    M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtFLOATUV_New (
    M_CPD*			posPToken,
    M_CPD*			refPToken
);

PublicFnDecl M_CPD* rtFLOATUV_Align (
    M_CPD*			cpd
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


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtFLOATUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtFLOATUV_Handle, rtUVECTOR_Handle);

//  Construction
protected:
    rtFLOATUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtFLOATUV_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
protected:

//  State
protected:
};


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
