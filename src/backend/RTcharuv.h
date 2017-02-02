/*****  Char U-Vector Representation Type Header File  *****/
#ifndef rtCHARUV_H
#define rtCHARUV_H

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

#include "RTcharuv.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 * Char U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtCHARUV_CPD_Array(cpd) UV_CPD_Array (cpd, rtCHARUV_ElementType)

#define rtCHARUV_CPD_Element(cpd) M_CPD_PointerToCharacter (\
    cpd, UV_CPx_Element\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtCHARUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtCHARUV_Handle, rtUVECTOR_Handle);

//  Aliases
public:
    typedef rtCHARUV_ElementType element_t;
    typedef element_t *string_t;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtCHARUV_Handle (rCTE);
    }
protected:
    rtCHARUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

//  Destruction
private:
    ~rtCHARUV_Handle () {
    }

//  Access
public:
    element_t *array () const {
	return UV_UV_ArrayAsType (typecastContent (), element_t);
    }
    element_t element (unsigned int xElement) const {
	return array ()[xElement];
    }
    string_t string (unsigned int xString) const {
	return array () + xString;
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

PublicFnDecl M_CPD* __cdecl rtCHARUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT, Ref_UV_Initializer initFn, ...
);

PublicFnDecl M_CPD *rtCHARUV_New (
    rtPTOKEN_Handle *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtCHARUV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT
);

PublicFnDecl void rtCHARUV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD*			*resultCPD,
    M_CPD*			*refuvCPD
);

PublicFnDecl M_CPD *rtCHARUV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD* rtCHARUV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD* rtCHARUV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtCHARUV_RFExtract (
    rtCHARUV_ElementType	*resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD* rtCHARUV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtCHARUV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtCHARUV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtCHARUV_ElementType	*value
);

PublicFnDecl M_CPD* rtCHARUV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtCHARUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtCHARUV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtCHARUV_ElementType	*valuePtr
);

PublicFnDecl M_CPD* rtCHARUV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl void rtCHARUV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType*		*majorLC,
    rtLINK_CType*		*minorLC
);

PublicFnDecl void rtCHARUV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtCHARUV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtCHARUV_ElementType	*sourcePtr,
    int				*locationPtr
);

PublicFnDecl void rtCHARUV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType*		*locationsLinkC,
    rtLINK_CType*		*locatedLinkC
);

PublicFnDecl bool rtCHARUV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtCHARUV_ElementType	*keyPtr,
    rtLINK_LookupCase		lookupCase,
    int				*locationPtr
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTcharuv.h 1 replace /users/mjc/system
  860508 18:12:34 mjc Added 'character' u-vector

 ************************************************************************/
/************************************************************************
  RTcharuv.h 2 replace /users/mjc/system
  860530 22:51:04 mjc Added 'RF{Extract/Assign}' operations

 ************************************************************************/
/************************************************************************
  RTcharuv.h 3 replace /users/mjc/system
  861015 23:52:41 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  RTcharuv.h 4 replace /users/jad/system
  870429 16:52:37 jad implemented set operations

 ************************************************************************/
/************************************************************************
  RTcharuv.h 5 replace /users/mjc/translation
  870524 09:31:50 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
