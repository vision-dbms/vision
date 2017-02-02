/*****  List Store Representation Type Header File  *****/
#ifndef rtLSTORE_H
#define rtLSTORE_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VContainerHandle.h"

#include "VCPDReference.h"

#include "VSet.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTlstore.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"
#include "RTcharuv.h"
#include "RTlink.d"
#include "RTrefuv.d"

#include "VAssociativeCursor.h"
#include "VCollectionOf.h"
#include "VCollectionOfStrings.h"
#include "VOrdered.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtLSTORE_CPD_StdPtrCount	4

#define rtLSTORE_CPx_RowPToken		(unsigned int)0
#define rtLSTORE_CPx_Content		(unsigned int)1
#define rtLSTORE_CPx_ContentPToken	(unsigned int)2
#define rtLSTORE_CPx_Breakpoint		(unsigned int)3

/*****  Standard CPD Access Macros  *****/
#define rtLSTORE_CPD_Base(cpd) M_CPD_ContainerBaseAsType (\
    cpd, rtLSTORE_LStore\
)
#define rtLSTORE_CPD_StringStore(cpd) rtLSTORE_LStore_StringStore (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_BreakpointCount(cpd) rtLSTORE_LStore_BreakpointCount (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_IsInconsistent(cpd) rtLSTORE_LStore_IsInconsistent (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_BreakpointArray(cpd) rtLSTORE_LStore_BreakpointArray (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_RowPToken(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_RowPToken\
)
#define rtLSTORE_CPD_Content(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_Content\
)
#define rtLSTORE_CPD_ContentPToken(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_ContentPToken\
)
#define rtLSTORE_CPD_Breakpoint(cpd) M_CPD_PointerToCardinal (\
    cpd, rtLSTORE_CPx_Breakpoint\
)

#define rtLSTORE_CPD_RowPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtLSTORE_CPx_RowPToken, RTYPE_C_PToken)\
)
#define rtLSTORE_CPD_ContentCPD(cpd) (\
    (cpd)->GetCPD (rtLSTORE_CPx_Content)\
)
#define rtLSTORE_CPD_ContentStringsCPD(cpd) (\
    rtLSTORE_Align (cpd)->GetCPD (rtLSTORE_CPx_Content, RTYPE_C_CharUV)\
)
#define rtLSTORE_CPD_ContentPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtLSTORE_CPx_ContentPToken, RTYPE_C_PToken)\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

typedef char const *(*rtLSTORE_StringEnumerator) (
    bool restartEnumeration, va_list ap
);

PublicFnDef M_CPD *rtLSTORE_NewCluster (
    M_CPD *pInstancePPT, M_CPD *pContentPrototype, int
);

PublicFnDecl M_CPD *rtLSTORE_NewCluster (
    M_CPD *pInstancePPT, M_CPD *pPrototype
);

PublicFnDecl M_CPD *rtLSTORE_NewCluster (
    rtLINK_CType *pContentMap, M_CPD *pContentCluster, bool bDecoupled = false
);

PublicFnDecl void rtLSTORE_Copy (
    M_CPD *pSource, M_ASD *pResultSpace, DSC_Descriptor *pResult
);

PublicFnDecl M_CPD *__cdecl rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, rtLSTORE_StringEnumerator stringEnumerator, ...
);
PublicFnDecl M_CPD *rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, char const *pString
);

PublicFnDecl M_CPD *__cdecl rtLSTORE_NewStringStoreWithDelm (
    M_ASD			*pContainerSpace,
    char const			*ldelim,
    char const			*rdelim,
    rtLSTORE_StringEnumerator	 stringEnumerator,
    ...
);
PublicFnDecl M_CPD *rtLSTORE_NewStringStoreWithDelm (
    M_ASD *pContainerSpace, char const *ldelim, char const *rdelim, char const *pString
);

PublicFnDecl M_CPD *rtLSTORE_NewStringStoreFromUV (
    M_CPD*			charUV
);

PublicFnDecl M_CPD *rtLSTORE_Align (
    M_CPD*			lstore
);

PublicFnDecl bool rtLSTORE_AlignAll (
    M_CPD*			cpd,
    bool			deletingEmptyUSegments
);

PublicFnDecl M_CPD *rtLSTORE_AlignUsingLCSelctLists (
    M_CPD*			lstore,
    rtLINK_CType*		listSelectorLC,
    M_CPD*			adjustmentUV
);

PublicFnDecl M_CPD *rtLSTORE_AlignUsingRefSelctList (
    M_CPD*			lstore,
    rtREFUV_TypePTR_Reference	listSelectorRef,
    int				adjustment
);

PublicFnDecl rtLINK_CType* rtLSTORE_AddLists (
    M_CPD*			lstore,
    M_CPD*			newListsPToken
);

PublicFnDecl int rtLSTORE_RefSelListElementCount (
    M_CPD*			lstore,
    rtREFUV_TypePTR_Reference	selectionRef
);

PublicFnDecl M_CPD *rtLSTORE_LCSelListElementCount (
    M_CPD*			lstore,
    rtLINK_CType*		selectionLC
);

PublicFnDecl void rtLSTORE_Extract (
    DSC_Descriptor&	rListDescriptor,
    M_CPD*&			pElementCluster,
    rtLINK_CType*&		pElementSelector,
    rtLINK_CType*&		expansionLinkc,
    M_CPD*&			reOrderingUVector
);

PublicFnDecl void rtLSTORE_Subscript (
    DSC_Descriptor		*sourceDsc,
    DSC_Descriptor		*keyDsc,
    int				keyModifier,
    DSC_Descriptor		*resultDsc,
    rtLINK_CType*		*locatedLinkc
);

PublicFnDecl void rtLSTORE_AppendToLCSelectedList (
    M_CPD*			lstore,
    rtLINK_CType*		selectionLC,
    rtLINK_CType*		*elementLC
);

PublicFnDecl void rtLSTORE_AppendToRefSelectdList (
    M_CPD*			lstore,
    rtREFUV_TypePTR_Reference	selectionRef,
    rtREFUV_TypePTR_Reference	elementRef
);

PublicFnDecl void rtLSTORE_WriteDBUpdateInfo (
    M_CPD*			lstoreCPD
);


/********************************
 ********************************
 *****  rtLSTORE_StringSet  *****
 ********************************
 ********************************/

class rtLSTORE_StringSet : public VSet {
//  Friends
    friend class VAssociativeCursor<rtLSTORE_StringSet, VCollectionOfStrings, char const*>;
    friend class VAssociativeCursor<rtLSTORE_StringSet, VCollectionOfUnsigned32, unsigned int>;

//  Construction Precondition Predicate
public:
    static bool IsAStringSet (M_CPD *pLStore);

//  Cache Maintenance
protected:
    void refreshCachedPointers () {
	m_pStringOrigins = rtLSTORE_CPD_BreakpointArray (m_pStringSet);
	m_pStringStorage = rtCHARUV_CPD_Array (m_pStringSpace);
    }

//  Alignment
public:
    void align () {
	rtLSTORE_Align (m_pStringSet);
	refreshCachedPointers ();
    }

//  Construction
public:
    rtLSTORE_StringSet (M_CPD *pSet);

//  Destruction
public:
    ~rtLSTORE_StringSet ();

//  Deferred Initialization
protected:
    virtual void initializeDPT ();

//  Access
protected:
    unsigned int origin (unsigned int xElement) const {
	return m_pStringOrigins[xElement];
    }
    char* storage (unsigned int xElement) const {
	return (char*)m_pStringStorage + origin (xElement);
    }

public:
    unsigned int cardinality () const {
	return rtLSTORE_CPD_BreakpointCount (m_pStringSet);
    }

    char const* element (unsigned int xElement) const {
	return m_pStringStorage + origin (xElement);
    }

    int compare (unsigned int xElement, char const* pKey) const {
	return strcmp (element (xElement), pKey);
    }

    void Locate (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    bool Locate (char const* pKey, unsigned int& rxElement);

//  Update
public:
    void Insert (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Delete (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    unsigned int Insert (char const*  pKey, unsigned int& rxElement);
    unsigned int Delete (char const*  pKey);

protected:
    void install (unsigned int xElement, char const* pKey);

    void install (
	rtLINK_CType* pAdditions, VOrdered<VCollectionOfStrings,char const*> &rAdditions
    );

//  State
protected:
    VCPDReference const m_pStringSet;
    VCPDReference const m_pStringSpace;
    unsigned int*	m_pStringOrigins;
    char const*		m_pStringStorage;
};


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtLSTORE_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtLSTORE_Handle, VContainerHandle);

//  Construction
protected:
    rtLSTORE_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtLSTORE_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
public:
    void CheckConsistency ();

protected:
    bool PersistReferences ();

//  State
protected:
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlstore.h 1 replace /users/mjc/system
  860404 20:13:20 mjc Create List Store Representation Type

 ************************************************************************/
/************************************************************************
  RTlstore.h 2 replace /users/mjc/system
  860530 14:38:22 mjc No further work for now

 ************************************************************************/
/************************************************************************
  RTlstore.h 3 replace /users/mjc/system
  860616 10:03:28 mjc Changed name from 'RTpartitn' to 'RTlstore'

 ************************************************************************/
/************************************************************************
  RTlstore.h 4 replace /users/mjc/system
  860709 10:18:38 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTlstore.h 5 replace /users/jck/system
  860910 14:14:52 jck Added update dump utility

 ************************************************************************/
/************************************************************************
  RTlstore.h 6 replace /users/mjc/system
  861002 18:37:15 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  RTlstore.h 7 replace /users/jad/system
  861222 17:48:36 jad implemented align for the content ptoken

 ************************************************************************/
/************************************************************************
  RTlstore.h 8 replace /users/jad/system
  870120 14:59:20 jad modified DB update procedure:

 ************************************************************************/
/************************************************************************
  RTlstore.h 9 replace /users/jad/system
  870123 16:14:38 jad implemented NewStringStore routine

 ************************************************************************/
/************************************************************************
  RTlstore.h 10 replace /users/jad/system
  870204 15:27:30 jad implemented another routine to make new string stores

 ************************************************************************/
/************************************************************************
  RTlstore.h 11 replace /users/jad/system
  870421 12:40:33 jad change interface to the Extract routines

 ************************************************************************/
/************************************************************************
  RTlstore.h 12 replace /users/jck/system
  870511 15:31:10 jck Added a copy routine and an initialized new routine

 ************************************************************************/
/************************************************************************
  RTlstore.h 13 replace /users/mjc/translation
  870524 09:33:45 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlstore.h 14 replace /users/jck/system
  870612 13:31:20 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  RTlstore.h 15 replace /users/jck/system
  870615 06:15:00 jck Added conditional inclusion of RTlink.d

 ************************************************************************/
/************************************************************************
  RTlstore.h 16 replace /users/mjc/Software/system
  871214 10:52:57 mjc Added dummy 'rtLSTORE_AlignAll'

 ************************************************************************/
/************************************************************************
  RTlstore.h 17 replace /users/mjc/Software/system
  871214 16:25:55 mjc Added comprehensive alignment routines for all property types

 ************************************************************************/
/************************************************************************
  RTlstore.h 18 replace /users/jad/system
  880509 12:16:04 jad Implement 'sprint' for Blocks, Methods, and Closures

 ************************************************************************/
/************************************************************************
  RTlstore.h 19 replace /users/jad/system
  880711 17:03:54 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTlstore.h 20 replace /users/jck/system
  890223 12:37:52 jck Fortified against saving corrupted lstores. Implemented Comprehensive Align

 ************************************************************************/
/************************************************************************
  RTlstore.h 21 replace /users/jck/system
  890413 13:22:33 jck Added rtLSTORE_DetermineExpansion

 ************************************************************************/
