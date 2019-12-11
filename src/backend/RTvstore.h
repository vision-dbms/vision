/*****  Value Store Representation Type Header File  *****/
#ifndef rtVSTORE_H
#define rtVSTORE_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "popvector.h"
#include "Vdd_Store.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTvstore.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdictionary.h"

#include "VGroundStore.h"

class rtLINK_CType;
class rtVECTOR_Handle;

class DSC_Descriptor;

class VBlockTask;
class VSelector;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtVSTORE_CPD_StdPtrCount	7

#define rtVSTORE_CPx_RowPToken		(unsigned int)0
#define rtVSTORE_CPx_InheritanceStore	(unsigned int)1
#define rtVSTORE_CPx_InheritancePtr	(unsigned int)2
#define rtVSTORE_CPx_ColumnPToken	(unsigned int)3
#define rtVSTORE_CPx_Dictionary		(unsigned int)4
#define rtVSTORE_CPx_AuxiliaryColumn	(unsigned int)5
#define rtVSTORE_CPx_Column		(unsigned int)6

/*****  Standard CPD Access Macros  *****/
#define rtVSTORE_CPD_Base(cpd)\
    ((rtVSTORE_Type *)(POPVECTOR_CPD_Array (cpd)))

#define rtVSTORE_CPD_ColumnArray(cpd)\
    rtVSTORE_VS_ColumnArray (rtVSTORE_CPD_Base (cpd))

#define rtVSTORE_CPD_Column(cpd)\
    M_CPD_PointerToPOP (cpd, rtVSTORE_CPx_Column)

#define rtVSTORE_CPD_DictionaryCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_Dictionary, RTYPE_C_Dictionary)\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtVSTORE_Handle : public rtPOPVECTOR_StoreHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtVSTORE_Handle, rtPOPVECTOR_StoreHandle);


    friend class rtVECTOR_Handle;
    friend class VBlockTask;
    friend class VGroundStore;

//  Aliases
public:
    typedef StoreBase::ExtensionType ExtensionType;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtVSTORE_Handle (rCTE);
    }
    rtVSTORE_Handle (
	rtPTOKEN_Handle*	pPPT,
	rtDICTIONARY_Handle*	pDictionary,
	rtPTOKEN_Handle*	pColumnPToken,
	Vdd::Store*		pSuperStore,
	M_CPD*			pSuperPointer
    );
private:
    rtVSTORE_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    }

//  Destruction
private:
    ~rtVSTORE_Handle () {
	if (m_pExtension)
	    m_pExtension->detachSurrogate (this);
    }

//  Alignment
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

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;
public:
    void canonicalize (DSC_Pointer &rPointer) const;

//  Cloning
private:
    virtual void clone_(Vdd::Store::Reference &rpResult, rtPTOKEN_Handle *pPPT) const OVERRIDE {
	Reference pHandle;
	clone (pHandle, pPPT);
	rpResult.setTo (pHandle);
    }
    virtual bool isACloneOfValueStore_(rtVSTORE_Handle const *pOther) const OVERRIDE {
	return isACloneOfValueStore (pOther);
    }
    virtual bool isACloneOf_(Vdd::Store const *pOther) const OVERRIDE {
	if (pOther->rtype () == RTYPE_C_ValueStore)
	    return isACloneOfValueStore (dynamic_cast<rtVSTORE_Handle const*>(pOther));
	return false;
    }

public:
    void clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const;
    bool isACloneOfValueStore (rtVSTORE_Handle const *pOther) const;

//  Forwarding
private:
    virtual bool forwardToSpace_(M_ASD *pSpace) OVERRIDE;

//  Dictionary Operations
public:
    using BaseClass::lookup;
    Vdd::Store::DictionaryLookup lookup (VSelector const &rSelector, unsigned int &rxPropertySlot, DSC_Descriptor *pValueReturn = 0);

//  Access
private:
    rtPTOKEN_Handle *columnPTokenHandle () const {
	return static_cast<rtPTOKEN_Handle*>(elementHandle (rtVSTORE_CPx_ColumnPToken, RTYPE_C_PToken));
    }
    rtDICTIONARY_Handle *dictionaryHandle () const {
	return static_cast<rtDICTIONARY_Handle*>(elementHandle (rtVSTORE_CPx_Dictionary, RTYPE_C_Dictionary));
    }
    M_POP *ptokenPOP () const {
	return element (rtVSTORE_CPx_RowPToken);
    }
    rtPTOKEN_Handle *ptokenHandle () const {
	return static_cast<rtPTOKEN_Handle*>(elementHandle (rtVSTORE_CPx_RowPToken, RTYPE_C_PToken));
    }
    VContainerHandle *inheritancePointerHandle () const {
	return elementHandle (rtVSTORE_CPx_InheritancePtr);
    }
    VContainerHandle *inheritanceStoreHandle () const {
	return elementHandle (rtVSTORE_CPx_InheritanceStore);
    }
public:
    unsigned int columnCount () const {
	return elementCount () - rtVSTORE_CPx_Column;
    }
    void getColumnPToken (rtPTOKEN_Handle::Reference &rpResult) const {
	rpResult.setTo (columnPTokenHandle ());
    }
    using BaseClass::getDictionary;
    void getDictionary (rtDICTIONARY_Handle::Reference &rpResult) const {
	rpResult.setTo (dictionaryHandle ());
    }
    rtDICTIONARY_Handle *getDictionary () const {
	return dictionaryHandle ();
    }
    void getInheritanceStore (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (inheritanceStoreHandle ());
    }
    bool getInheritanceStore (Vdd::Store::Reference &rpResult) const {
	return inheritanceStoreHandle ()->getStore (rpResult);
    }
    void getInheritancePointer (M_CPD *&rpResult) const {
	rpResult = inheritancePointerHandle ()->GetCPD ();
    }
    void getInheritancePointer (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (inheritancePointerHandle ());
    }

//  Query
public:
    bool hasAnInheritance () const {
	return ReferenceIsntNil (rtVSTORE_CPx_InheritanceStore);
    }
    bool hasNoInheritance () const {
	return ReferenceIsNil (rtVSTORE_CPx_InheritanceStore);
    }

//  Classification
public:
    bool Names (VContainerHandle const *pThat) const {
	return static_cast<HandleBase const*>(this)->Names (pThat);
    }
    virtual bool Names (M_KOTM pKOTM) const OVERRIDE {
	return static_cast<HandleBase const*>(this)->Names (pKOTM);
    }
    virtual bool NamesTheDateClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheDateClass ();
    }
    virtual bool NamesTheDoubleClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheDoubleClass ();
    }
    virtual bool NamesTheFloatClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheFloatClass ();
    }
    virtual bool NamesTheIntegerClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheIntegerClass ();
    }
    virtual bool NamesThePrimitiveClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesThePrimitiveClass ();
    }
    virtual bool NamesTheSelectorClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheSelectorClass ();
    }
    virtual bool NamesTheNAClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheNAClass ();
    }
    virtual bool NamesTheTrueClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheTrueClass ();
    }
    virtual bool NamesTheFalseClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesTheFalseClass ();
    }
    virtual bool NamesABuiltInNumericClass () const OVERRIDE {
	return static_cast<HandleBase const*>(this)->NamesABuiltInNumericClass ();
    }

//  Stock Access
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return getDictionary ();
    }
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE {
	return getPToken ();
    }
public:
    rtPTOKEN_Handle *getPToken () const {
	return ptokenHandle ();
    }

//  Instance Creation
private:
    virtual rtLINK_CType *addInstances_(rtPTOKEN_Handle *pAdditionPPT) OVERRIDE;
    rtLINK_CType *addExtensions_(
	ExtensionType	xExtensionType,
	Vdd::Store*	pInheritanceStore,
	M_CPD*		pInheritancePointer,
	rtLINK_CType*&	rpExtensionGuard,
	rtLINK_CType**	ppLocateOrAddAdditions
    );

//  Instance Deletion
private:
    virtual bool deleteInstances_(DSC_Scalar &rInstances) OVERRIDE;
    virtual bool deleteInstances_(
	rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses
    ) OVERRIDE;
protected:
    bool doVStoreInstanceDeletionSetup (VReference<rtPTOKEN_CType> &rpPTC);

//  Property Management
private:
    virtual bool getProperty_(
	Vdd::Store::Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
    ) OVERRIDE {
	return getProperty (rpResult, rPointer, xPropertySlot, pPropertyPrototype);
    }
public:
    bool getProperty (
	Vdd::Store::Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
    );

//  Transient Extension Support
private:
    void setTransientExtensionTo (VGroundStore *pExtension);
    virtual transientx_t *transientExtension_() const OVERRIDE {
	return m_pExtension;
    }
    virtual transientx_t *transientExtensionIfA_(VRunTimeType const &rRTT) const OVERRIDE;
    virtual bool transientExtensionIsA_(VRunTimeType const &rRTT) const OVERRIDE;

//  Display and Inspection
public:
    virtual void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference) OVERRIDE;
    virtual unsigned int getClusterReferenceMapSize () OVERRIDE;

    virtual unsigned __int64 getClusterSize () OVERRIDE;

//  State
private:
    VGroundStore::Reference m_pExtension;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl rtVSTORE_Handle *rtVSTORE_NewCluster (
    rtPTOKEN_Handle *pPPT, rtDICTIONARY_Handle *pDictionary, Vdd::Store *pSuperStore, M_CPD *pSuperPointer
);

PublicFnDecl void rtVSTORE_WriteDBUpdateInfo (
    M_CPD*			pVStoreCPD
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvstore.h 1 replace /users/mjc/system
  860404 20:15:20 mjc Create Value Store Representation Type

 ************************************************************************/
/************************************************************************
  RTvstore.h 2 replace /users/mjc/system
  860523 01:32:12 mjc Return to library for completion by 'jck'

 ************************************************************************/
/************************************************************************
  RTvstore.h 3 replace /users/jck/system
  860530 10:44:48 jck Release partial implementation for mjc

 ************************************************************************/
/************************************************************************
  RTvstore.h 4 replace /users/jck/system
  860605 16:22:29 jck Creation and Search operations functional

 ************************************************************************/
/************************************************************************
  RTvstore.h 5 replace /users/mjc/system
  860605 16:52:03 mjc Added declarations for function names

 ************************************************************************/
/************************************************************************
  RTvstore.h 6 replace /users/jck/system
  860611 11:12:09 jck AddRows and Align routines implemented

 ************************************************************************/
/************************************************************************
  RTvstore.h 7 replace /users/mjc/system
  860623 10:23:45 mjc Converted to use new value descriptor macros/R-Type

 ************************************************************************/
/************************************************************************
  RTvstore.h 8 replace /users/mjc/system
  860709 10:18:58 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTvstore.h 9 replace /users/jad/system
  860731 17:46:04 jad added DBUPDATE utilities

 ************************************************************************/
/************************************************************************
  RTvstore.h 10 replace /users/jad/system
  860820 13:35:38 jad added clone routine

 ************************************************************************/
/************************************************************************
  RTvstore.h 11 replace /users/mjc/system
  861002 18:02:43 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  RTvstore.h 12 replace /users/jad/system
  870120 15:00:22 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  RTvstore.h 13 replace /users/mjc/translation
  870524 09:36:42 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvstore.h 14 replace /users/jck/system
  870824 15:01:42 jck Added routine to align columns

 ************************************************************************/
