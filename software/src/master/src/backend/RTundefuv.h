/*****  Undefined Value U-Vector Representation Type Header File  *****/
#ifndef rtUNDEFUV_H
#define rtUNDEFUV_H

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

#include "RTundefuv.d"

/************************
 *****  Supporting  *****
 ************************/


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtUNDEFUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtUNDEFUV_Handle, rtUVECTOR_Handle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtUNDEFUV_Handle (rCTE);
    }
protected:
    rtUNDEFUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

//  Destruction
private:
    ~rtUNDEFUV_Handle () {
    }

//  Alignment
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
public:
    bool align ();

//  Access
public:

//  Query
public:

//  Callbacks
protected:

//  State
protected:
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtUNDEFUV_New (
    rtPTOKEN_Handle *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtUNDEFUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT
);

PublicFnDecl void rtUNDEFUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl M_CPD *rtUNDEFUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD* rtUNDEFUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD* rtUNDEFUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtUNDEFUV_RFExtract (
    pointer_t			resultAddr,
    M_CPD*			sourceCPD, 
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD* rtUNDEFUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtUNDEFUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtUNDEFUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    pointer_t			value
);

PublicFnDecl M_CPD* rtUNDEFUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    pointer_t			valuePtr
);

PublicFnDecl M_CPD* rtUNDEFUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    pointer_t			valuePtr
);

PublicFnDecl M_CPD* rtUNDEFUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl void rtUNDEFUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtUNDEFUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtUNDEFUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    int				*locationPtr
);

PublicFnDecl void rtUNDEFUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtUNDEFUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTundefuv.h 1 replace /users/mjc/system
  860516 18:10:33 mjc Create 'undefined value' u-vectors

 ************************************************************************/
/************************************************************************
  RTundefuv.h 2 replace /users/mjc/system
  860530 17:06:27 mjc Release 'RF{Extract,Assign}' 

 ************************************************************************/
/************************************************************************
  RTundefuv.h 3 replace /users/jad/system
  860718 17:16:56 jad added d file

 ************************************************************************/
/************************************************************************
  RTundefuv.h 4 replace /users/mjc/system
  861015 23:53:14 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTundefuv.h 5 replace /users/jad/system
  870429 16:50:30 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTundefuv.h 6 replace /users/mjc/translation
  870524 09:36:15 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
