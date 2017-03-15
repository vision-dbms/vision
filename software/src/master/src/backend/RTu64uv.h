/*****  Unsigned-64 U-Vector Representation Type Header File  *****/
#ifndef rtU64UV_H
#define rtU64UV_H

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

#include "RTu64uv.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/**************************************
 *****  Standard CPD Definitions  *****
 **************************************/
/*---------------------------------------------------------------------------
 * Integer U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Access Macros  *****/
#define rtU64UV_CPD_Array(cpd) UV_CPD_Array (cpd, rtU64UV_ElementType)

#define rtU64UV_CPD_Element(cpd) M_CPD_PointerToUnsigned64 (\
    cpd, UV_CPx_Element\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtU64UV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtU64UV_Handle, rtUVECTOR_Handle);

//  Aliases
public:
    typedef rtU64UV_ElementType element_t;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtU64UV_Handle (rCTE);
    }
protected:
    rtU64UV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

//  Destruction
private:
    ~rtU64UV_Handle () {
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

PublicFnDecl M_CPD* __cdecl rtU64UV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT, Ref_UV_Initializer initFn, ...
);

PublicFnDecl M_CPD *rtU64UV_New (
    rtPTOKEN_Handle *pPPT, M_CPD *pRPTRef, int xRPTRef
);

PublicFnDecl M_CPD* rtU64UV_New (
    rtPTOKEN_Handle *pPPT, rtPTOKEN_Handle *pRPT
);

PublicFnDecl void rtU64UV_ToSetUV (
    M_ASD*			pContainerSpace, 
    M_CPD*			sourceCPD,
    M_CPD**			resultCPD,
    M_CPD**			refuvCPD
);

PublicFnDecl M_CPD *rtU64UV_Distribute (
    M_CPD *refuvCPD, M_CPD *sourceuvCPD
);

PublicFnDecl M_CPD* rtU64UV_LCExtract (
    M_CPD*			sourceCPD,
    rtLINK_CType*		linkConstructor
);

PublicFnDecl M_CPD* rtU64UV_UVExtract (
    M_CPD*			sourceCPD,
    M_CPD*			refuvCPD
);

PublicFnDecl void rtU64UV_RFExtract (
    rtU64UV_ElementType*	resultAddr,
    M_CPD*			sourceCPD,
    rtREFUV_TypePTR_Reference	referenceAddr
);

PublicFnDecl M_CPD* rtU64UV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* rtU64UV_UVAssign (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    M_CPD*			sourceCPD
);

PublicFnDecl void rtU64UV_RFAssign (
    M_CPD*			targetCPD,
    rtREFUV_TypePTR_Reference	reference,
    rtU64UV_ElementType*	value
);

PublicFnDecl M_CPD* rtU64UV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkConstructor,
    rtU64UV_ElementType*	valuePtr
);

PublicFnDecl M_CPD* rtU64UV_UVAssignScalar (
    M_CPD*			targetCPD,
    M_CPD*			refuvCPD,
    rtU64UV_ElementType*	valuePtr
);

PublicFnDecl M_CPD* rtU64UV_PartitndSortIndices (
    rtLINK_CType *partition, M_CPD *values, bool descending
);

PublicFnDecl void rtU64UV_PartitndPartition (
    rtLINK_CType*		partition,
    M_CPD*			values,
    rtLINK_CType**		majorLC,
    rtLINK_CType**		minorLC
);

PublicFnDecl void rtU64UV_LocateOrAdd (
    M_CPD*			targetCPD,
    M_CPD*			sourceCPD,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		addedLinkC = 0
);

PublicFnDecl bool rtU64UV_ScalarLocateOrAdd (
    M_CPD*			targetCPD,
    rtU64UV_ElementType*	sourcePtr,
    int*			locationPtr
);

PublicFnDecl void rtU64UV_Lookup (
    M_CPD*			sourceCPD,
    M_CPD*			keyCPD,
    rtLINK_LookupCase		lookupCase,
    rtLINK_CType**		locationsLinkC,
    rtLINK_CType**		locatedLinkC
);

PublicFnDecl bool rtU64UV_ScalarLookup (
    M_CPD*			sourceCPD,
    rtU64UV_ElementType*	keyPtr,
    rtLINK_LookupCase		lookupCase,
    int*			locationPtr
);


#endif
