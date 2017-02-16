/*****  List Index Representation Type Header File  *****/
#ifndef rtINDEX_H
#define rtINDEX_H

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VBenderenceable.h"

#include "popvector.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTindex.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"
#include "RTlink.h"


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
#define rtINDEX_CPD_Base(cpd)\
    ((rtINDEX_Type *)(POPVECTOR_CPD_Array (cpd)))

#define rtINDEX_CPD_IsATimeSeries(cpd) (\
    (cpd)->ReferenceIsntNil (rtINDEX_CPx_KeyStore)\
)

#define rtINDEX_CPD_ListStoreCPD(cpd) (\
    (cpd)->GetCPD (rtINDEX_CPx_Lists, RTYPE_C_ListStore)\
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
 *	m_pPToken		- a CPD for the positional p-token of the
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

class rtINDEX_Key : public VBenderenceable {
    DECLARE_CONCRETE_RTT (rtINDEX_Key, VBenderenceable);

//  Construction
private:
    rtINDEX_Key (rtINDEX_Key *parent, rtLINK_CType *subset, M_CPD *reordering);

public:
    rtINDEX_Key (M_CPD *ptoken, DSC_Descriptor *source);

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

    bool HoldsKeysInStore (M_CPD *pStoreRefCPD, int xStoreRef);

    bool ConformsToIndex (M_CPD *pIndexCPD);

    bool ConformsToIndex (DSC_Descriptor *pIndexDescriptor);

//  State
public:
    rtINDEX_KeyType	 m_xType;
    M_CPD		*m_pPPT;
    DSC_Descriptor	 m_iSequence,
			 m_iSet;
    M_CPD		*enumeratorOrReordering;
    rtLINK_CType	*m_pSubset;
    rtINDEX_Key		*m_pParent;
};

/*****  Key Access Macros  *****/
#define rtINDEX_Key_Type(key)		((key)->m_xType)
#define rtINDEX_Key_Sequence(key)	((key)->m_iSequence)
#define rtINDEX_Key_Set(key)		((key)->m_iSet)
#define rtINDEX_Key_Enumerator(key)	((key)->enumeratorOrReordering)
#define rtINDEX_Key_Reordering(key)	((key)->enumeratorOrReordering)
#define rtINDEX_Key_Parent(key)		((key)->m_pParent)


/*************************************
 *************************************
 *****  Index Manager Interface  *****
 *************************************
 *************************************/

PublicFnDecl M_CPD* rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    RTYPE_Type			xKeySetRType,
    M_CPD*			pKeySetRefPTokenCPD,
    M_CPD*			pKeyStoreCPD,
    M_CPD*			pContentPrototypeCPD
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    M_CPD*			pContentPrototypeCPD,
    int
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    DSC_Descriptor*		pKeySetPrototype,
    M_CPD*			pContentPrototypeCPD
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    M_CPD*			pPrototypeIndexCPD
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_NewCluster (
    rtLINK_CType*		pContentPartition,
    M_CPD*			pContent,
    rtINDEX_Key*		pKey
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_NewTimeSeries (
    M_CPD*			pInstancePTokenCPD
);
/*---------------------*/

PublicFnDecl M_CPD* rtINDEX_Align (
    M_CPD*			pIndexCPD,
    bool			fDeletingKeyMapNils
);

PublicFnDecl M_CPD* rtINDEX_Align (
    M_CPD*			pIndexCPD
);
/*---------------------*/

PublicFnDecl bool rtINDEX_AlignAll (
    M_CPD*			pIndexCPD,
    bool			deletingEmptyUSegments
);
/*---------------------*/

PublicFnDecl void rtINDEX_GetKeyMapType (
    M_CPD*			pIndexCPD,
    RTYPE_Type&			xKeyMapType
);

PublicFnDecl void rtINDEX_SetKeyMapType (
    M_CPD*			pIndexCPD,
    RTYPE_Type			xKeyMapType
);
/*---------------------*/

PublicFnDecl rtLINK_CType* rtINDEX_AddLists (
    M_CPD*			pIndexCPD,
    M_CPD*			newListsPToken
);
/*---------------------*/

PublicFnDecl void rtINDEX_Subscript (
    DSC_Descriptor*		sourceDsc,
    DSC_Descriptor*		keyDsc,
    int				keyModifier,
    DSC_Descriptor*		resultDsc,
    rtLINK_CType*		*locatedLinkc
);
/*---------------------*/

PublicFnDecl void rtINDEX_TimeZero (
    DSC_Descriptor*		pResultDsc,
    DSC_Descriptor*		pIndexSelectorDsc
);
/*---------------------*/

PublicFnDecl void rtINDEX_DeleteUnusedKeys (
    M_CPD*			pIndexCPD
);
/*---------------------*/

PublicFnDecl void rtINDEX_Extract (
    DSC_Descriptor&		rIndexDescriptor,
    M_CPD*&			pElementCluster,
    rtLINK_CType*&		elementSelector,
    rtLINK_CType*&		expansionLinkc,
    M_CPD*&			elementDistribution,
    rtINDEX_Key*&		pKey
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


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtINDEX_Handle : public rtPOPVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtINDEX_Handle, rtPOPVECTOR_Handle);

//  Construction
protected:
    rtINDEX_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtINDEX_Handle (rCTE);
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
