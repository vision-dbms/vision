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

/***********************
 *****  Container  *****
 ***********************/

#include "RTvstore.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdictionary.h"

class rtLINK_CType;

class DSC_Descriptor;

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
#define rtVSTORE_CPx_AuxillaryColumn	(unsigned int)5
#define rtVSTORE_CPx_Column		(unsigned int)6

/*****  Standard CPD Access Macros  *****/
#define rtVSTORE_CPD_Base(cpd)\
    ((rtVSTORE_Type *)(POPVECTOR_CPD_Array (cpd)))

#define rtVSTORE_CPD_ColumnCount(cpd)\
    (POPVECTOR_CPD_ElementCount (cpd) -\
	(sizeof (rtVSTORE_Type) - sizeof (M_POP)) / sizeof (M_POP))

#define rtVSTORE_CPD_ColumnArray(cpd)\
    rtVSTORE_VS_ColumnArray (rtVSTORE_CPD_Base (cpd))

#define rtVSTORE_CPD_Column(cpd)\
    M_CPD_PointerToPOP (cpd, rtVSTORE_CPx_Column)

#define rtVSTORE_CPD_RowPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_RowPToken, RTYPE_C_PToken)\
)

#define rtVSTORE_CPD_InheritanceStoreCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_InheritanceStore)\
)

#define rtVSTORE_CPD_InheritancePointerCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_InheritancePtr)\
)

#define rtVSTORE_CPD_ColumnPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_ColumnPToken, RTYPE_C_PToken)\
)

#define rtVSTORE_CPD_DictionaryCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_Dictionary, RTYPE_C_Dictionary)\
)

#define rtVSTORE_CPD_AuxillaryColumnCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_AuxillaryColumn)\
)

#define rtVSTORE_CPD_ColumnCPD(cpd) (\
    (cpd)->GetCPD (rtVSTORE_CPx_Column)\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD* rtVSTORE_NewCluster (
    M_CPD *pPPT, M_CPD *pDictionary, M_CPD *pSuperStore, M_CPD *pSuperPointer
);

PublicFnDecl M_CPD* rtVSTORE_NewCluster (M_CPD *pPPT, M_CPD *pPrototype);

PublicFnDecl bool rtVSTORE_ForwardCluster (
    M_CPD*			pTransientStoreCPD,
    M_CPD*			pPersistentStoreCPD
);

PublicFnDecl M_CPD* rtVSTORE_Align (
    M_CPD*			pVStoreCPD
);

PublicFnDecl bool rtVSTORE_AlignAll (
    M_CPD*			pVStoreCPD,
    bool			deletingEmptyUSegments
);

PublicFnDecl rtLINK_CType* rtVSTORE_AddRows (
    M_CPD*			pVStoreCPD,
    M_CPD*			newRowsPToken
);

enum rtVSTORE_Extension {
    rtVSTORE_Extension_Add,
    rtVSTORE_Extension_LocateOrAdd,
    rtVSTORE_Extension_Locate
};

PublicFnDecl rtLINK_CType* rtVSTORE_ExtendRowsTo (
    rtVSTORE_Extension		xExtensionOperation,
    M_CPD*			pInheritanceStore,
    M_CPD*			pInheritancePointer,
    M_CPD*			pDescendantStore,
    rtLINK_CType*&		rpExtensionGuard,
    rtLINK_CType**		ppLocateOrAddAdditions	// optional locate or add result, nil if not wanted
);

PublicFnDecl rtDICTIONARY_LookupResult rtVSTORE_Lookup (
    M_CPD*			pVStoreCPD,
    VSelector const*		pSelector,
    DSC_Descriptor*		pValueReturn
);

PublicFnDecl int rtVSTORE_AreBehavioriallyEquivalent (
    M_CPD*			pStore1CPD,
    M_CPD*			pStore2CPD
);

PublicFnDecl void rtVSTORE_WriteDBUpdateInfo (
    M_CPD*			pVStoreCPD
);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtVSTORE_Handle : public rtPOPVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtVSTORE_Handle, rtPOPVECTOR_Handle);

//  Construction
protected:
    rtVSTORE_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtVSTORE_Handle (rCTE);
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
