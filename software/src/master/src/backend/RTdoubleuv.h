/*****  Double U-Vector Representation Type Header File  *****/
#ifndef rtDOUBLEUV_H
#define rtDOUBLEUV_H

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

#include "RTdoubleuv.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 * Double U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtDOUBLEUV_CPD_Array(cpd) UV_CPD_Array (cpd, double)

#define rtDOUBLEUV_CPD_Element(cpd) M_CPD_PointerToReal (\
    cpd, UV_CPx_Element\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD* __cdecl rtDOUBLEUV_New (
    M_CPD*			pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    Ref_UV_Initializer		initFn,
    ...
);

PublicFnDecl M_CPD *rtDOUBLEUV_New (
    M_CPD *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtDOUBLEUV_New (
    M_CPD*			posPToken,
    M_CPD*			refPToken
);

PublicFnDecl M_CPD* rtDOUBLEUV_Align (
    M_CPD*			cpd
);

PublicFnDecl void rtDOUBLEUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl M_CPD* rtDOUBLEUV_AsFloatUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtDOUBLEUV_AsIntUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD,
    rtLINK_CType*		*ppDDLC
);

PublicFnDecl M_CPD* rtDOUBLEUV_AsIntUV (
    M_KOT*			pKOT,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD *rtDOUBLEUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD* rtDOUBLEUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD* rtDOUBLEUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtDOUBLEUV_RFExtract (
    rtDOUBLEUV_ElementType	*resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD* rtDOUBLEUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtDOUBLEUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtDOUBLEUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtDOUBLEUV_ElementType	*value
);

PublicFnDecl M_CPD* rtDOUBLEUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtDOUBLEUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtDOUBLEUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtDOUBLEUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtDOUBLEUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl M_CPD* rtDOUBLEUV_PartitndCumulative (
    rtLINK_CType*		partition,
    M_CPD*			values,
    M_CPD*			refPToken
);

PublicFnDecl void rtDOUBLEUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtDOUBLEUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtDOUBLEUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtDOUBLEUV_ElementType	*sourcePtr,
    int				*locationPtr
);

PublicFnDecl void rtDOUBLEUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtDOUBLEUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtDOUBLEUV_ElementType	*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtDOUBLEUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtDOUBLEUV_Handle, rtUVECTOR_Handle);

//  Construction
protected:
    rtDOUBLEUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtDOUBLEUV_Handle (rCTE);
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
  RTdoubleuv.h 1 replace /users/mjc/system
  860323 17:19:21 mjc New U-Vector representation types

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 2 replace /users/mjc/system
  860422 22:55:28 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 3 replace /users/mjc/system
  860504 18:45:34 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 4 replace /users/mjc/system
  860530 21:39:59 mjc Added 'RT{Extract/Assign}'

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 5 replace /users/mjc/system
  861015 23:52:48 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 6 replace /users/jck/system
  870325 09:44:24 jck Added 'RunningTotal' primitive (rtDOUBLEUV_PartitndCumulative)

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 7 replace /users/jad/system
  870429 16:52:45 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTdoubleuv.h 8 replace /users/mjc/translation
  870524 09:32:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
