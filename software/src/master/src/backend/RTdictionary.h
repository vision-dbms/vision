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
#include "Vdd_Store.h"

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

#include "RTlink.h"
#include "RTptoken.h"
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


/********************************
 ********************************
 *****  rtDICTIONARY_Cache  *****
 ********************************
 ********************************/

PublicVarDecl bool rtDICTIONARY_UsingCache;

class rtDICTIONARY_Handle;

class rtDICTIONARY_Cache : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtDICTIONARY_Cache, VBenderenceable);

    friend class rtDICTIONARY_Handle;

//  Construction
private:
    rtDICTIONARY_Cache (rtDICTIONARY_Handle *pDictionary);

//  Destruction
private:
    ~rtDICTIONARY_Cache () {
    }

//  Access
public:
    rtVECTOR_Handle *bindings () const {
	return m_pBindings;
    }
    M_CPD *propertySubset () const {
	return m_pPropertySubset;
    }
    rtVECTOR_Handle *propertyPrototypes () const {
	return m_pPropertyPrototypes;
    }
    rtSELUV_Set &selectors () {
	return m_iSelectors;
    }

//  State
protected:
    rtSELUV_Set				m_iSelectors;
    rtVECTOR_Handle::Reference	const	m_pBindings;
    VCPDReference		const	m_pPropertySubset;
    rtVECTOR_Handle::Reference	const	m_pPropertyPrototypes;
};


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtDICTIONARY_Handle : public rtPOPVECTOR_StoreHandle {
    DECLARE_CONCRETE_RTT (rtDICTIONARY_Handle, rtPOPVECTOR_StoreHandle);

    friend class rtDICTIONARY_Cache;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtDICTIONARY_Handle (rCTE);
    }
    rtDICTIONARY_Handle (M_ASD *pContainerSpace);
private:
    rtDICTIONARY_Handle (M_CTE &rCTE);

//  Destruction
private:
    ~rtDICTIONARY_Handle () {
    }

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;

//  Cache
private:
    rtDICTIONARY_Cache *cache ();
    virtual void flushCachedResources_() OVERRIDE;

//  Access
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return static_cast<rtDICTIONARY_Handle*>(TheFixedPropertyClassDictionary().ObjectHandle ());
    }
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE {
	return getPToken ();
    }
public:
    rtPTOKEN_Handle *getPToken () const;

private:
    rtSELUV_Handle *selectors () const {
	return static_cast<rtSELUV_Handle*>(elementHandle (rtDICTIONARY_CPx_Selectors, RTYPE_C_SelUV));
    }
    rtVECTOR_Handle *values () const {
	return static_cast<rtVECTOR_Handle*>(elementHandle (rtDICTIONARY_CPx_Values, RTYPE_C_Vector));
    }
    rtVECTOR_Handle *propertyPrototypes () const {
	return static_cast<rtVECTOR_Handle*>(elementHandle (rtDICTIONARY_CPx_PropertyPrototypes, RTYPE_C_Vector));
    }
    rtLINK_Handle *propertySubset () const {
	return static_cast<rtLINK_Handle*>(elementHandle (rtDICTIONARY_CPx_PropertySubset, RTYPE_C_Link));
    }
public:
    void getSelectorVector (rtVECTOR_Handle::Reference &rpResult, M_ASD *pContainerSpace) const;

    void getSelectors (M_CPD *&rpResult) const {
	rpResult = selectors ()->GetCPD ();
    }
    void getSelectors (rtSELUV_Handle::Reference &rpResult) const {
	rpResult.setTo (selectors ());
    }

    void getPropertyPToken (rtPTOKEN_Handle::Reference &rpResult) const {
	rpResult.setTo (
	    static_cast<rtPTOKEN_Handle*>(elementHandle (rtDICTIONARY_CPx_PropertyPToken, RTYPE_C_PToken))
	);
    }

    void getPropertySubset (M_CPD *&rpResult) const {
	rtLINK_Handle::Reference pHandle;
	getPropertySubset (pHandle);
	rpResult = pHandle->GetCPD ();
    }
    void getPropertySubset (rtLINK_Handle::Reference &rpResult) const {
	rpResult.setTo (propertySubset ());
    }

    void getPropertyPrototypes (M_CPD *&rpResult) const {
	rtVECTOR_Handle::Reference pHandle;
	getPropertyPrototypes (pHandle);
	rpResult = pHandle->GetCPD ();
    }
    void getPropertyPrototypes (rtVECTOR_Handle::Reference &rpResult) const {
	rpResult.setTo (propertyPrototypes ());
    }

    void getValues (M_CPD *&rpResult) const {
	rtVECTOR_Handle::Reference pHandle;
	getValues (pHandle);
	rpResult = pHandle->GetCPD ();
    }
    void getValues (rtVECTOR_Handle::Reference &rpResult) const {
	rpResult.setTo (values ());
    }

//  Query
public:
    bool propertyPTokenDoesntName (VContainerHandle const *pThat, M_POP const *pThatPOP) const {
	return ReferenceDoesntName (rtDICTIONARY_CPx_PropertyPToken, pThat, pThatPOP);
    }
    bool propertyPTokenNames (VContainerHandle const *pThat, M_POP const *pThatPOP) const {
	return ReferenceNames (rtDICTIONARY_CPx_PropertyPToken, pThat, pThatPOP);
    }
    
//  Element Access
public:
    DictionaryLookup getElement (
	VSelector const &rSelector, DSC_Descriptor *pResult = 0, unsigned int *pElementOffsetReturn = 0
    );

//  Element Update
private:
    virtual bool setElements_(rtLINK_CType *pSubscript, DSC_Descriptor &rValues) OVERRIDE {
	assign (pSubscript, rValues);
	return true;
    }
    virtual bool setElements_(rtLINK_CType *pSubscript, rtVECTOR_CType *pValues) OVERRIDE {
	assign (pSubscript, pValues);
	return true;
    }
    virtual bool setElements_(DSC_Scalar &rSubscript, DSC_Descriptor &rValues) OVERRIDE {
	assign (rSubscript, rValues);
	return true;
    }
    virtual bool setElements_(DSC_Scalar &rSubscript, rtVECTOR_CType *pValues) OVERRIDE {
	assign (rSubscript, pValues);
	return true;
    }
public/*private*/:
    void CreatePropertySubset ();
private:
    void MaintainPropertySubset (rtVECTOR_Handle *pSelectorValues);
public:
    void assign (rtLINK_CType *pSubscript, DSC_Descriptor &rValues);
    void assign (rtLINK_CType *pSubscript, rtVECTOR_CType *pValues);
    void assign (DSC_Scalar &rSubscript, rtVECTOR_CType *pValues);
    void assign (DSC_Scalar &rSubscript, DSC_Descriptor &rValues);

    void define (VSelector const &rSelector, DSC_Descriptor &rValues);

//  Store Update
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
    virtual bool alignAll_(bool bCleaning) OVERRIDE {
	return alignAll (bCleaning);
    }
public:
    bool align ();
    bool alignAll (bool bCleaning = true);
	
	bool compact();

//  Associative Operations
private:
    virtual void associativeInsert_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeInsert_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeLocate_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeLocate_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeDelete_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeDelete_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;

    /*---------------------------------------------------------------------------
     *****  Templates to maintain a collection of selectors.
     *
     *  Arguments:
     *    pKeys			- a pointer to the collection of keys to be
     *				  processed.
     *    rpReordering		- an l-val which will be set to the address of
     *				  a reference u-vector CPD used to sort the keys
     *				  if they required sorting.  If no sort was
     *				  required, *rpReordering is set to Nil.  If
     *				  returned, cod(*rpReordering) = dom (*pKeys) and
     *				  dom(*rpReordering) = dom (rAssociation).
     *    rAssociation		- a reference to the associative result structure
     *				  that will receive the result of the operation.
     *
     *****/
    template <class KeyType> void Insert (
	KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) {
	if (rtDICTIONARY_UsingCache) {
	    rtDICTIONARY_Cache::Reference pCache (cache ());
	    pCache->selectors ().Insert (pKeys, rpReordering, rAssociation);
	}
	else {
	    rtSELUV_Set iSet (selectors ());
	    iSet.Insert (pKeys, rpReordering, rAssociation);
	}
	align ();
    }

    template <class KeyType> void Delete (
	KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) {
	if (rtDICTIONARY_UsingCache) {
	    rtDICTIONARY_Cache::Reference pCache (cache ());
	    pCache->selectors ().Delete (pKeys, rpReordering, rAssociation);
	}
	else {
	    rtSELUV_Set iSet (selectors ());
	    iSet.Delete (pKeys, rpReordering, rAssociation);
	}
	align ();
    }

    template <class KeyType> void Locate (
	KeyType *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) {
	if (rtDICTIONARY_UsingCache) {
	    rtDICTIONARY_Cache::Reference pCache (cache ());
	    pCache->selectors ().Locate (pKeys, rpReordering, rAssociation);
	}
	else {
	    rtSELUV_Set iSet (selectors ());
	    iSet.Locate (pKeys, rpReordering, rAssociation);
	}
    }

//  Instance Deletion
private:
    virtual bool deleteInstances_(DSC_Scalar &pInstances) OVERRIDE {
	return doInstanceDeletion (pInstances);
    }
    virtual bool deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) OVERRIDE {
	return doInstanceDeletion (pInstances, rpTrues, rpFalses);
    }

//  Display and Inspection
public:
    virtual void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference) OVERRIDE;
    virtual unsigned int getClusterReferenceMapSize () OVERRIDE;

    virtual unsigned __int64 getClusterSize () OVERRIDE;

//  State
private:
    rtDICTIONARY_Cache::Reference m_pCache;
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
