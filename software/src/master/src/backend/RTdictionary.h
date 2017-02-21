/*****  Dictionary Representation Type Interface  *****/
#ifndef rtDICTIONARY_H
#define rtDICTIONARY_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VBenderenceable.h"

#include "popvector.h"

#include "RTseluv.h"

#include "VCPDReference.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTdictionary.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"

#include "RTvector.h"

class VSelector;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/**************************************
 *****  Implementation Constants  *****
 **************************************/

#define rtDICTIONARY_FixedElementCount		5

/*************************************
 ***** Standard CPD Definitions  *****
 *************************************/

/***** Standard CPD Pointer Assignments  *****/
#define rtDICTIONARY_CPD_StdPtrCount (\
    POPVECTOR_CPD_XtraPtrCount + rtDICTIONARY_FixedElementCount + 1\
)

#define rtDICTIONARY_CPx_Selectors		(unsigned int)0
#define rtDICTIONARY_CPx_Values			(unsigned int)1
#define rtDICTIONARY_CPx_PropertyPToken		(unsigned int)2
#define rtDICTIONARY_CPx_PropertySubset		(unsigned int)3
#define rtDICTIONARY_CPx_PropertyPrototypes	(unsigned int)4

/*****  Standard CPD Access Macros *****/
#define rtDICTIONARY_CPD_Base(cpd) ((M_POP*) M_CPD_ContainerBase (cpd))

#define rtDICTIONARY_CPD_SelectorsCPD(cpd) (\
    (cpd)->GetCPD (rtDICTIONARY_CPx_Selectors, RTYPE_C_SelUV)\
)

#define rtDICTIONARY_CPD_ValuesCPD(cpd) (\
    (cpd)->GetCPD (rtDICTIONARY_CPx_Values, RTYPE_C_Vector)\
)

#define rtDICTIONARY_CPD_PropertyPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtDICTIONARY_CPx_PropertyPToken, RTYPE_C_PToken)\
)

#define rtDICTIONARY_CPD_PropertySubsetCPD(cpd) (\
    (cpd)->GetCPD (rtDICTIONARY_CPx_PropertySubset, RTYPE_C_Link)\
)

#define rtDICTIONARY_CPD_PropertyPrototypesCPD(cpd) (\
    (cpd)->GetCPD (rtDICTIONARY_CPx_PropertyPrototypes, RTYPE_C_Vector)\
)


/*******************
 *******************
 *****  Types  *****
 *******************
 *******************/

enum rtDICTIONARY_LookupResult {
    rtDICTIONARY_LookupResult_FoundNothing,
    rtDICTIONARY_LookupResult_FoundProperty,
    rtDICTIONARY_LookupResult_FoundOther
};

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

PublicVarDecl bool rtDICTIONARY_UsingCache;


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtDICTIONARY_New (M_ASD *pContainerSpace);

PublicFnDecl M_CPD *rtDICTIONARY_Align (M_CPD *pDictionary);

PublicFnDecl bool rtDICTIONARY_AlignAll (
    M_CPD *pDictionary, bool deletingEmptyUSegments
);

PublicFnDecl void rtDICTIONARY_Assign (
    M_CPD*			pDictionary,
    rtLINK_CType*		pElementSelector,
    rtVECTOR_CType*		pValues
);

PublicFnDecl void rtDICTIONARY_Assign (
    M_CPD*			pDictionary,
    rtLINK_CType*		pElementSelector,
    DSC_Descriptor*		pValues
);

PublicFnDecl void rtDICTIONARY_Assign (
    M_CPD*			pDictionary,
    rtREFUV_TypePTR_Reference	pElementSelector,
    rtVECTOR_CType*		pValues
);

PublicFnDecl void rtDICTIONARY_Assign (
    M_CPD*			pDictionary,
    rtREFUV_TypePTR_Reference	pElementSelector,
    DSC_Descriptor*		pValues
);

PublicFnDecl M_CPD *rtDICTIONARY_SelectorPToken (M_CPD *pDictionary);

PublicFnDecl M_CPD *rtDICTIONARY_Contents (M_ASD *pContainerSpace, M_CPD *pDictionary);


/*---------------------------------------------------------------------------
 *****  Routines to define / locate a single selector.
 *
 *  Arguments:
 *	pDictionary		- a CPD for the dictionary.
 *	...
 *
 *****/
PublicFnDecl void rtDICTIONARY_Define (
    M_CPD *pDictionary, VSelector const* pSelector, DSC_Descriptor* pValues
);

PublicFnDecl rtDICTIONARY_LookupResult rtDICTIONARY_Lookup (
    M_CPD*		pDictionary,
    VSelector const*	pSelector,
    DSC_Descriptor*	pValueReturn,
    int*		pOffsetReturn
);

PublicFnDecl rtDICTIONARY_LookupResult rtDICTIONARY_Lookup (
    VContainerHandle*	pDictionary,
    VSelector const*	pSelector,
    DSC_Descriptor*	pValueReturn,
    int*		pOffsetReturn
);


/********************************
 ********************************
 *****  rtDICTIONARY_Cache  *****
 ********************************
 ********************************/

class rtDICTIONARY_Cache : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtDICTIONARY_Cache, VBenderenceable);

//  Construction
private:
    rtDICTIONARY_Cache (M_CPD *pDictionary);
    ~rtDICTIONARY_Cache () {
    }

//  Access
public:
    M_CPD *bindings () const {
	return m_pBindings;
    }
    M_CPD *propertySubset () const {
	return m_pPropertySubset;
    }
    M_CPD *propertyPrototypes () const {
	return m_pPropertyPrototypes;
    }
    rtSELUV_Set &selectors () {
	return m_iSelectors;
    }

    static rtDICTIONARY_Cache* GetCacheOf (M_CPD *pDictionary) {
	return M_CPD_CPCC(pDictionary)->TransientExtensionIsA (rtDICTIONARY_Cache::RTT)
	    ? (rtDICTIONARY_Cache*)M_CPD_CPCC(pDictionary)->TransientExtension ()
	    : new rtDICTIONARY_Cache (pDictionary);
    }
    static rtDICTIONARY_Cache* GetCacheOf (VContainerHandle* pDictionary) {
	rtDICTIONARY_Cache* pHandle;
	if (pDictionary->TransientExtensionIsA (rtDICTIONARY_Cache::RTT))
	    pHandle = (rtDICTIONARY_Cache*)pDictionary->TransientExtension ();
	else {
	    M_CPD *pCPD = pDictionary->GetCPD ();
	    pHandle = new rtDICTIONARY_Cache (pCPD);
	    pCPD->release ();
	}
	return pHandle;
    }

//  Transient Extension Support
protected:
    void FlushCacheAsTransientExtensionOf (VContainerHandle* pContainerHandle);

//  State
protected:
    rtSELUV_Set		m_iSelectors;
    VCPDReference	m_pBindings;
    VCPDReference	m_pPropertySubset;
    VCPDReference	m_pPropertyPrototypes;
};


/*---------------------------------------------------------------------------
 *****  Templates to maintain a collection of selectors.
 *
 *  Arguments:
 *	pDictionary		- a CPD for the dictionary.
 *	pKeys			- a pointer to the collection of keys to be
 *				  processed.
 *	rpReordering		- an l-val which will be set to the address of
 *				  a reference u-vector CPD used to sort the keys
 *				  if they required sorting.  If no sort was
 *				  required, *rpReordering is set to Nil.  If
 *				  returned, cod(*rpReordering) = dom (*pKeys) and
 *				  dom(*rpReordering) = dom (rAssociation).
 *	rAssociation		- a reference to the associative result structure
 *				  that will receive the result of the operation.
 *
 *****/
template <class KeyType> void rtDICTIONARY_Insert (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);

template <class KeyType> void rtDICTIONARY_Delete (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);

template <class KeyType> void rtDICTIONARY_Locate (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);


template <class KeyType> void rtDICTIONARY_Insert (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    if (rtDICTIONARY_UsingCache) {
	VReference<rtDICTIONARY_Cache> pCache (
	    rtDICTIONARY_Cache::GetCacheOf (pDictionary)
	);
	pCache->selectors ().Insert (pKeys, rpReordering, rAssociation);
	rtDICTIONARY_Align (pDictionary);
    }
    else {
	rtSELUV_Set iSet (pDictionary, rtDICTIONARY_CPx_Selectors);
	iSet.Insert (pKeys, rpReordering, rAssociation);
	rtDICTIONARY_Align (pDictionary);
    }
}

template <class KeyType> void rtDICTIONARY_Delete (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    if (rtDICTIONARY_UsingCache) {
	VReference<rtDICTIONARY_Cache> pCache (
	    rtDICTIONARY_Cache::GetCacheOf (pDictionary)
	);
	pCache->selectors ().Delete (pKeys, rpReordering, rAssociation);
	rtDICTIONARY_Align (pDictionary);
    }
    else {
	rtSELUV_Set iSet (pDictionary, rtDICTIONARY_CPx_Selectors);
	iSet.Delete (pKeys, rpReordering, rAssociation);
	rtDICTIONARY_Align (pDictionary);
    }
}

template <class KeyType> void rtDICTIONARY_Locate (
    M_CPD *pDictionary, KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    if (rtDICTIONARY_UsingCache) {
	VReference<rtDICTIONARY_Cache> pCache (
	    rtDICTIONARY_Cache::GetCacheOf (pDictionary)
	);
	pCache->selectors ().Locate (pKeys, rpReordering, rAssociation);
    }
    else {
	rtSELUV_Set iSet (pDictionary, rtDICTIONARY_CPx_Selectors);
	iSet.Locate (pKeys, rpReordering, rAssociation);
    }
}


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtDICTIONARY_Handle : public rtPOPVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtDICTIONARY_Handle, rtPOPVECTOR_Handle);

//  Construction
private:
    rtDICTIONARY_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
    }
    ~rtDICTIONARY_Handle () {
    }
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtDICTIONARY_Handle (rCTE);
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
  RTdictionary.h 1 replace /users/jck/system
  860519 07:38:44 jck Initial release of Method Dictionary representation type

 ************************************************************************/
/************************************************************************
  RTdictionary.h 2 replace /users/jck/system
  860521 12:36:05 jck First Complete Implementation

 ************************************************************************/
/************************************************************************
  RTdictionary.h 3 replace /users/jck/system
  860522 12:09:01 jck Added New Entry addition routines

 ************************************************************************/
/************************************************************************
  RTdictionary.h 4 replace /users/jck/system
  860605 16:16:00 jck Updated release to support RTvstore

 ************************************************************************/
/************************************************************************
  RTdictionary.h 5 replace /users/mjc/system
  870104 22:52:22 lcn Added parallel lookup and addition routines

 ************************************************************************/
/************************************************************************
  RTdictionary.h 6 replace /users/jad/system
  870128 19:04:45 jad implemented delete and contents routines

 ************************************************************************/
/************************************************************************
  RTdictionary.h 7 replace /users/mjc/translation
  870524 09:34:10 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
