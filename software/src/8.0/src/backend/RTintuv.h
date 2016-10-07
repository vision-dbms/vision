/*****  Integer U-Vector Representation Type Header File  *****/
#ifndef rtINTUV_H
#define rtINTUV_H

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

#include "RTintuv.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 * Integer U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtINTUV_CPD_Array(cpd) UV_CPD_Array (cpd, rtINTUV_ElementType)

#define rtINTUV_CPD_Element(cpd) M_CPD_PointerToInteger (\
    cpd, UV_CPx_Element\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *__cdecl rtINTUV_New (
    M_CPD*			pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    Ref_UV_Initializer		initFn,
    ...
);

PublicFnDecl M_CPD *rtINTUV_New (
    M_CPD*			posPTokenRefCPD,
    int				posPTokenRefIndex,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex
);

PublicFnDecl M_CPD *rtINTUV_New (
    M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD *rtINTUV_New (
    M_CPD*			posPToken,
    M_CPD*			refPToken
);

PublicFnDecl M_CPD *rtINTUV_Align (
    M_CPD*			cpd
);

PublicFnDecl void rtINTUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl M_CPD *rtINTUV_AsDoubleUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtINTUV_AsFloatUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtINTUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD *rtINTUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD *rtINTUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtINTUV_RFExtract (
    rtINTUV_ElementType		*resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD *rtINTUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtINTUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtINTUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtINTUV_ElementType		*value
);

PublicFnDecl M_CPD *rtINTUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtINTUV_ElementType		*valuePtr
);

PublicFnDecl M_CPD *rtINTUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtINTUV_ElementType		*valuePtr
);

PublicFnDecl M_CPD *rtINTUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl M_CPD *rtINTUV_PartitndRanks (
    rtLINK_CType*		partition,
    M_CPD*			sortIndices,
    M_CPD*			pRefPTokenCPD
);

PublicFnDecl void rtINTUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtINTUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtINTUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtINTUV_ElementType		*sourcePtr,
    int				*locationPtr
);

PublicFnDecl void rtINTUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtINTUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtINTUV_ElementType		*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtINTUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtINTUV_Handle, rtUVECTOR_Handle);

//  Construction
protected:
    rtINTUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtINTUV_Handle (rCTE);
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
  RTintuv.h 1 replace /users/mjc/system
  860323 17:19:28 mjc New U-Vector representation types

 ************************************************************************/
/************************************************************************
  RTintuv.h 2 replace /users/mjc/system
  860422 22:55:43 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTintuv.h 3 replace /users/mjc/system
  860504 18:45:21 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTintuv.h 4 replace /users/mjc/system
  860530 21:39:53 mjc Added 'RT{Extract/Assign}'

 ************************************************************************/
/************************************************************************
  RTintuv.h 5 replace /users/mjc/system
  861015 23:53:01 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTintuv.h 6 replace /users/jad/system
  861216 17:41:06 jad added locateOrAdd and lookup routines for set uvectors

 ************************************************************************/
/************************************************************************
  RTintuv.h 7 replace /users/jad/system
  861217 16:52:35 jad added scalar versions of locateOrAdd and lookup routines

 ************************************************************************/
/************************************************************************
  RTintuv.h 8 replace /users/mjc/translation
  870524 09:33:20 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
