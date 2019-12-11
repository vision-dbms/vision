/*****  List Index Representation Type Header File  *****/
#ifndef rtINDEX_H
#define rtINDEX_H

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

#include "popvector.h"
#include "Vdd_Store.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTindex.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"
#include "RTdictionary.h"
#include "RTlink.h"
#include "RTlstore.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtINDEX_CPD_StdPtrCount	4

#define rtINDEX_CPx_Lists	(unsigned int)0
#define rtINDEX_CPx_KeyStore	(unsigned int)1
#define rtINDEX_CPx_KeyValues	(unsigned int)2
#define rtINDEX_CPx_Map		(unsigned int)3

/*****  Standard CPD Access Macros  *****/
#define rtINDEX_CPD_IsATimeSeries(cpd) (\
    (cpd)->ReferenceIsntNil (rtINDEX_CPx_KeyStore)\
)

/***************************
 *****  Lookup Macros  *****
 ***************************/

#define rtINDEX_TimeSeriesLookup(result, indexSelector, key) rtINDEX_Lookup (\
    result, indexSelector, key, rtLINK_LookupCase_LE\
)

#define rtINDEX_AListLookup(result, indexSelector, key) rtINDEX_Lookup (\
    result, indexSelector, key, rtLINK_LookupCase_EQ\
)


/*********************************************
 *********************************************
 *****  Default Key Map Type Management  *****
 *********************************************
 *********************************************/

PublicFnDecl void rtINDEX_GetDefaultKeyMapType (
    RTYPE_Type &xKeyMapType, bool &fAutoConversionEnabled
);

PublicFnDecl void rtINDEX_SetDefaultKeyMapType (
    RTYPE_Type xKeyMapType, bool fAutoConversionEnabled
);


/***********************************
 ***********************************
 *****  Key Manager Interface  *****
 ***********************************
 ***********************************/

/****************************
 *****  Key Definition  *****
 ****************************/
/*---------------------------------------------------------------------------
 * A 'Key' holds a sequenced collection of values to be located in an 'Index'.
 * Because sequenced collections are ill suited to associative access of an
 * 'Index', the general form of a key contains two components - a SET holding
 * the values to be used in searching the 'Index' and an ENUMERATOR which
 * connects positions in the sequenced collection to 'positions' in the set.
 * Because of the overhead associated with maintenance and use of the general
 * form of a key, the key manager attempts to recognize simpler key forms and
 * to defer the creation of sets and enumerators.  Currently, the simpler key
 * form recognized by the key manager are SCALAR keys.  Scalar keys contain
 * exactly one unique value.  A scalar key can be coerced implicitly to any
 * positional state and therefore does not require the creation or maintenance
 * of an 'enumerator'.  Because most keys are created by the virtual machine
 * as part of its task switching, most keys will be created by forming a
 * reordered (optional), subset (optional) of an existing key.  To exploit
 * this information, the key manager represents such derivative keys in
 * deferred form (after recognizing and exploiting any scalar key based
 * optimizations).  Two forms of deferral exist - subset/reordering deferral
 * (a FUTURE) and set creation deferral (a SEQUENCE key).
 *
 *  Key Field Descriptions:
 *	m_xType			- the structural type of this key.  The
 *				  possibilities are:
 *				    Scalar	... a scalar key.
 *				    Set		... a general key complete with
 *						    'm_iSet' and 'enumeration'.
 *				    Sequence	... a key whose values are
 *						    explicitly enumerated.
 *				    Future	... a key expressed as a
 *						    reordered subset of
 *						    another key.
 *	m_pPToken		- a handle for the positional p-token of the
 *				  sequence of elements found in this key.
 *	m_iSequence		- a standard descriptor containing the SEQUENCE
 *				  of values held in this key.  Valid for 'Set'
 *				  and 'Sequence' keys; possibly valid for
 *				  'Scalar' keys; undefined for 'Future'.
 *	m_iSet			- a standard descriptor containing the SET of
 *				  values held in this key.  Valid for 'Set' and
 *				  'Scalar' keys; undefined for 'Sequence' and
 *				  'Future'.
 *	enumeratorOrReordering	- a standard CPD identifying the reference
 *				  u-vector connecting 'm_iSequence' to 'm_iSet'
 *				  positions for 'Set' keys and the subset
 *				  reordering reference u-vector for 'Future';
 *				  undefined for 'Sequence'.
 *	m_pSubset		- a link constructor specifying the subset of
 *				  the 'parent' key to be used in defining the
 *				  values associated with this key.  Valid for
 *				  'Future' only.
 *	m_pParent			- the address of the key from which the values
 *				  associated with this key are to be extracted.
 *				  Valid for 'Future' only.
 *****/


/*---------------------------------------------------------------------------
 *  This diagram shows which components each of the four types of Keys contain.
 *
 *  A '*' means that this component is required and an 'o' means it is 
 *  optional.  For Futures, a '*o' is used for the subset and reordering 
 *  components, it means that at least one of the two is required.  
 *  Any components not listed are invalid for that key type.
 *  
 *  TYPE	  COMPONENT	USE	
 *---------------------------------------------------------------------------
 *  Sequence	* m_iSequence	value descriptor containing the uvector.
 *---------------------------------------------------------------------------
 *  Set		* m_iSet	value descriptor containing the set uvector.
 *		* emunerator	reference uvector used to convert the set
 * 				uvector back to the original non-set uvector.
 *  		o m_iSequence	the original non-set uvector.
 *---------------------------------------------------------------------------
 *  Scalar	* m_iSet	a scalar value descriptor containing the scalar
 *				value.
 *		o m_iSequence	a coerced value descriptor containing a uvector
 *				of 'values' where 'values' are all the same
 * 				scalar value which is stored in the set 
 *                              component.  The number of values is determined 
 *                              by the positional ptoken of the key.
 *---------------------------------------------------------------------------
 *  Future	* m_pParent	a key from which the values for this key are
 *				to come.
 *		*o m_pSubset	a link constructor specifying the subset of the
 *				parent key to be used for this key.
 *		*o reordering	a reference uvector to be used to reorder the 
 *				values from the parent for this key.
 *---------------------------------------------------------------------------
 */


/*****  Key Type Definition  *****/
enum rtINDEX_KeyType {
    rtINDEX_KeyType_Scalar,
    rtINDEX_KeyType_Set,
    rtINDEX_KeyType_Sequence,
    rtINDEX_KeyType_Future
};

class rtINDEX_Handle;

class rtINDEX_Key : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtINDEX_Key, VBenderenceable);

//  Construction
private:
    rtINDEX_Key (rtINDEX_Key *parent, rtLINK_CType *subset, M_CPD *reordering);

public:
    rtINDEX_Key (rtPTOKEN_Handle *ptoken, DSC_Descriptor *source);

    rtINDEX_Key *NewFutureKey (rtLINK_CType *subset, M_CPD *reordering);

//  Destruction
protected:
    ~rtINDEX_Key ();

//  Key Conversion
protected:
    void AttemptSequenceToScalarKeyConv ();

//  Key Realization
protected:
    void Realize ();

public:
    void RealizeSequence ();
    void RealizeSet ();

//  Query
public:
    bool isScalar () const {
	return m_xType == rtINDEX_KeyType_Scalar;
    }
    bool IsSet () const {
	return m_xType == rtINDEX_KeyType_Set;
    }
    bool IsSequence () const {
	return m_xType == rtINDEX_KeyType_Sequence;
    }
    bool IsFuture () const {
	return m_xType == rtINDEX_KeyType_Future;
    }

    bool ConformsToIndex (rtINDEX_Handle *pIndex);
    bool ConformsToIndex (DSC_Descriptor const &rIndexDescriptor);

//  State
public:
    rtINDEX_KeyType		m_xType;
    rtPTOKEN_Handle::Reference	m_pPPT;
    DSC_Descriptor		m_iSequence,
				m_iSet;
    M_CPD*			enumeratorOrReordering;
    rtLINK_CType*		m_pSubset;
    rtINDEX_Key*		m_pParent;
};

/*****  Key Access Macros  *****/
#define rtINDEX_Key_Type(key)		((key)->m_xType)
#define rtINDEX_Key_Sequence(key)	((key)->m_iSequence)
#define rtINDEX_Key_Set(key)		((key)->m_iSet)
#define rtINDEX_Key_Enumerator(key)	((key)->enumeratorOrReordering)
#define rtINDEX_Key_Reordering(key)	((key)->enumeratorOrReordering)
#define rtINDEX_Key_Parent(key)		((key)->m_pParent)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtINDEX_Handle : public rtPOPVECTOR_StoreHandle {
    DECLARE_CONCRETE_RTT (rtINDEX_Handle, rtPOPVECTOR_StoreHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtINDEX_Handle (rCTE);
    }
    rtINDEX_Handle (
	M_ASD *pSpace, rtLSTORE_Handle *pLStore, M_CPD *pKeyMap, M_CPD *pKeySet, Vdd::Store *pKeyStore
    );
private:
    rtINDEX_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    }

//  Destruction
private:
    ~rtINDEX_Handle () {
    }

//  Alignment
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
    virtual bool alignAll_(bool bCleaning) OVERRIDE {
	return alignAll(bCleaning);
    }
public:
    bool align (bool bDeletingKeyMapNils = false);
    bool alignAll (bool bCleaning = true);

    void deleteUnusedKeys ();

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;

//  Cloning
private:
    virtual void clone_(Vdd::Store::Reference &rpResult, rtPTOKEN_Handle *pPPT) const OVERRIDE {
	Reference pHandle;
	clone (pHandle, pPPT);
	rpResult.setTo (pHandle);
    }
    virtual bool isACloneOfIndex_(rtINDEX_Handle const *pOther) const OVERRIDE {
	return isACloneOfIndex (pOther);
    }
public:
    void clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const;
    bool isACloneOfIndex (rtINDEX_Handle const *pOther) const;

//  Component Access
private:
    VContainerHandle *keyMapHandle () const {
	return elementHandle (rtINDEX_CPx_Map);
    }
    VContainerHandle *keySetHandle () const {
	return elementHandle (rtINDEX_CPx_KeyValues);
    }
    VContainerHandle *keyStoreHandle () const {
	return elementHandle (rtINDEX_CPx_KeyStore);
    }
    rtLSTORE_Handle *listStoreHandle () const {
	return static_cast <rtLSTORE_Handle*>(elementHandle (rtINDEX_CPx_Lists, RTYPE_C_ListStore));
    }
public:
    void getKeyMap (M_CPD *&rpResult) const {
	rpResult = keyMapHandle ()->GetCPD ();
    }
    void getKeyMap (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (keyMapHandle ());
    }

    void getKeySet (M_CPD *&rpResult) const {
	rpResult = keySetHandle ()->GetCPD ();
    }
    void getKeySet (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (keySetHandle ());
    }
    bool getKeySet (Vdd::Store::Reference &rpResult) const {
	return keySetHandle ()->getStore (rpResult);
    }

    void getKeyStore (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (keyStoreHandle ());
    }
    bool getKeyStore (Vdd::Store::Reference &rpResult) const {
	return keyStoreHandle ()->getStore (rpResult);
    }

    void getListStore (M_CPD *&rpResult) const {
	rpResult = listStoreHandle ()->GetCPD ();
    }
    void getListStore (rtLSTORE_Handle::Reference &rpResult) const {
	rpResult.setTo (listStoreHandle ());
    }
    void getListStore (Vdd::Store::Reference &rpResult) const {
	rpResult.setTo (listStoreHandle ());
    }

//  Key Map Type
public:
    void getKeyMapType (RTYPE_Type&rxKeyMapType) const;
    void setKeyMapType (RTYPE_Type  xKeyMapType);

//  Dictionary Access
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return static_cast<rtDICTIONARY_Handle*> (
	    (isATimeSeries () ? TheTimeSeriesClassDictionary () : TheIndexedListClassDictionary()).ObjectHandle ()
	);
    }

//  Cluster Access
private:
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE {
	return getPToken ();
    }
public:
    rtPTOKEN_Handle *getPToken () const {
	rtLSTORE_Handle::Reference pListStore (listStoreHandle ());
	return pListStore->getPToken ();
    }

//  Cluster Forwarding
private:
    virtual bool forwardToSpace_(M_ASD *pSpace) OVERRIDE;

//  Cluster Update
private:
    virtual rtLINK_CType *addInstances_(rtPTOKEN_Handle *pAdditionPPT) OVERRIDE;

//  Instance Deletion
private:
    virtual bool deleteInstances_(DSC_Scalar &pInstances) OVERRIDE {
	return doInstanceDeletion (pInstances);
    }
    virtual bool deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) OVERRIDE {
	return doInstanceDeletion (pInstances, rpTrues, rpFalses);
    }

//  List Cardinality Access
private:
    virtual bool getCardinality_(M_CPD *&rpResult, rtLINK_CType *pSubscript) OVERRIDE {
	return getCardinality (rpResult, pSubscript);
    }
    virtual bool getCardinality_(unsigned int &rpResult, DSC_Scalar &rSubscript) OVERRIDE {
	return getCardinality (rpResult, rSubscript);
    }
public:
    bool getCardinality (M_CPD *&rpResult, rtLINK_CType *pSubscript);
    bool getCardinality (unsigned int &rpResult, DSC_Scalar &rSubscript);


//  List Element Access
private:
    virtual bool getListElements_(
	DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int keyModifier, rtLINK_CType **ppGuard
    ) OVERRIDE {
	return getListElements (rResult, pInstances, pSubscript, keyModifier, ppGuard);
    }
    virtual bool getListElements_(
	DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
    ) OVERRIDE {
	return getListElements (rResult, rInstance, xSubscript, iModifier);
    }
    virtual bool getListElements_(
	DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
    ) OVERRIDE {
	return getListElements (rResult, rInstances, rSubscript, iModifier, ppGuard);
    }
    virtual bool getListElements_(
	DSC_Pointer&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution,
	rtINDEX_Key*&		rpKey
    ) OVERRIDE {
	return getListElements (
	    rInstances, rpElementStore, rpElementPointer, rpExpansion, rpDistribution, rpKey
	);
    }
public:
    bool getListElements (
	DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int keyModifier, rtLINK_CType **ppGuard
    );
    bool getListElements (
	DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
    );
    bool getListElements (
	DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
    );
    bool getListElements (
	DSC_Pointer&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution,
	rtINDEX_Key*&		rpKey
    );

//  Time Series Specifics
public:
    bool isATimeSeries () const {
	return ReferenceIsntNil (rtINDEX_CPx_KeyStore);
    }
    bool getTimeZero (DSC_Descriptor &rResult, rtPTOKEN_Handle *pResultPPT);

//  Store Verification
public:
    void EnsureIndexKeyMeetsSetDefinition ();

//  Display and Inspection
public:
    virtual void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference) OVERRIDE;
    virtual unsigned int getClusterReferenceMapSize () OVERRIDE;

    virtual unsigned __int64 getClusterSize () OVERRIDE;
};


/*************************************
 *************************************
 *****  Index Manager Interface  *****
 *************************************
 *************************************/

PublicFnDecl rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    RTYPE_Type			xKeySetRType,
    rtPTOKEN_Handle*		pKeySetRefPToken,
    Vdd::Store*			pKeyStore,
    Vdd::Store*			pContentPrototype
);
/*---------------------*/

PublicFnDecl rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    Vdd::Store*			pContentPrototype = 0 // -> vector
);
/*---------------------*/

PublicFnDecl rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    DSC_Descriptor*		pKeySetPrototype,
    Vdd::Store*			pContentPrototype
);
/*---------------------*/

PublicFnDecl rtINDEX_Handle *rtINDEX_NewCluster (
    rtLINK_CType*		pContentPartition,
    Vdd::Store*			pContent,
    rtINDEX_Key*		pKey
);
/*---------------------*/

PublicFnDecl rtINDEX_Handle *rtINDEX_NewTimeSeries (
    rtPTOKEN_Handle*		pInstancePToken
);
/*---------------------*/

PublicFnDecl void rtINDEX_Define (
    DSC_Descriptor*		pResultDsc,
    DSC_Descriptor*		pIndexSelectorDsc,
    rtINDEX_Key*		pKey
);
/*---------------------*/

PublicFnDecl void rtINDEX_Lookup (
    DSC_Descriptor*		pResultDsc,
    DSC_Descriptor*		pIndexSelectorDsc,
    rtINDEX_Key*		pKey,
    rtLINK_LookupCase		comparisonType
);
/*---------------------*/

PublicFnDecl void rtINDEX_IntervalLimits (
    DSC_Descriptor*		pResultDsc,
    DSC_Descriptor*		pIndexSelectorDsc,
    rtINDEX_Key*		pKey,
    rtLINK_LookupCase		comparisonType
);
/*---------------------*/

PublicFnDecl void rtINDEX_Delete (
    DSC_Descriptor*		pIndexSelectorDsc,
    rtINDEX_Key*		pKey
);
/*---------------------*/

PublicFnDecl void rtINDEX_KeyDelete (
    DSC_Descriptor*		pIndexSelectorDsc,
    rtINDEX_Key*		pKey
);
/*---------------------*/


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTindex.h 1 replace /users/mjc/system
  870109 10:26:44 mjc Release 'index' representation type shell

 ************************************************************************/
/************************************************************************
  RTindex.h 2 replace /users/mjc/system
  870330 19:07:15 mjc Release for ongoing work by 'jad'

 ************************************************************************/
/************************************************************************
  RTindex.h 3 replace /users/jad/system
  870414 16:01:33 jad implemented basics, key routines, & Define and
Lookup routines.

 ************************************************************************/
/************************************************************************
  RTindex.h 4 replace /users/jad/system
  870421 12:38:34 jad implemented Extract routine

 ************************************************************************/
/************************************************************************
  RTindex.h 5 replace /users/jck/system
  870511 15:30:05 jck Added 'interval lookup' and 'delete' capability

 ************************************************************************/
/************************************************************************
  RTindex.h 6 replace /users/mjc/translation
  870524 09:33:07 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTindex.h 7 replace /users/jck/system
  870612 13:31:01 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  RTindex.h 8 replace /users/jck/system
  871214 14:32:13 jck (1) Forced alignment of underlying lstores in the various lookup routines.
(2) Added rtINDEX_Align and rtINDEX_AlignAll

 ************************************************************************/
/************************************************************************
  RTindex.h 9 replace /users/jad/system
  880711 16:59:58 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTindex.h 10 replace /users/jck/system
  890117 13:29:58 jck Implemented Indexed List entire column deletion

 ************************************************************************/
/************************************************************************
  RTindex.h 11 replace /users/jck/system
  890413 13:42:02 jck Added rtINDEX_DetermineExpansion

 ************************************************************************/
