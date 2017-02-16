/*****  POP-Vector Facility Header File  *****/
#ifndef POPVECTOR_H
#define POPVECTOR_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VContainerHandle.h"

/***********************
 *****  Container  *****
 ***********************/

#include "popvector.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  CPD Extra Pointer Count     *****/
#define POPVECTOR_CPD_XtraPtrCount	(unsigned int)1

#define POPVECTOR_CPx_Cursor(cpd)\
    (cpd->PointerCount () - POPVECTOR_CPD_XtraPtrCount)

/*****  Standard CPD Access Macros  *****/
#define POPVECTOR_CPD_Base(cpd)\
    ((POPVECTOR_PVType *)M_CPD_ContainerBase (cpd))

#define POPVECTOR_CPD_Array(cpd)\
    (M_POP*)(POPVECTOR_CPD_Base (cpd) + 1)

#define POPVECTOR_CPD_Cursor(cpd)\
    M_CPD_PointerToPOP (cpd, cpd->PointerCount () - POPVECTOR_CPD_XtraPtrCount)

#define POPVECTOR_CPD_ElementCount(cpd)\
    POPVECTOR_PV_ElementCount (POPVECTOR_CPD_Base (cpd))

#define POPVECTOR_CPD_IsInconsistent(cpd)\
    POPVECTOR_PV_IsInconsistent (POPVECTOR_CPD_Base (cpd))


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void POPVECTOR_InitStdCPD (M_CPD *cpd);

PublicFnDecl M_CPD *POPVECTOR_New (M_ASD *pSpace, RTYPE_Type xType, unsigned int cElements);

PublicFnDecl void POPVECTOR_ReclaimContainer (
    M_ASD			*ownerASD,
    M_CPreamble	const		*preambleAddress
);

PublicFnDecl void POPVECTOR_MarkContainers (
    M_ASD			*pSpace,
    M_CPreamble	const		*pContainer
);

PublicFnDecl M_CPD* POPVECTOR_Align (
    M_CPD*			cpd,
    unsigned int		xPToken,
    unsigned int		alignmentIndex,
    unsigned int		elementCount
);

PublicFnDecl void POPVECTOR_Print (
    M_CPD*			popvectorCPD,
    int				recursive
);

PublicFnDecl void POPVECTOR_NextElement (
    M_CPD*			popvectorCPD
);

PublicFnDecl void POPVECTOR_PreviousElement (
    M_CPD*			popvectorCPD
);

PublicFnDecl void POPVECTOR_GoToElement (
    M_CPD*			popvectorCPD,
    int				i
);


/**********************************
 *****  Debugger Definitions  *****
 **********************************/

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_Size (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_Eprint (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_Browser (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_NextElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_PreviousElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl POPVECTOR_DM_GoToElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

#define POPVECTOR_StdDMDEPackage\
    RTYPE_StandardDMDEPackage\
    IOBJ_MDE ("eprint"		, POPVECTOR_DM_Eprint)\
    IOBJ_MDE ("browser"	    	, POPVECTOR_DM_Browser)\
    IOBJ_MDE ("next"		, POPVECTOR_DM_NextElement)\
    IOBJ_MDE ("previous"	, POPVECTOR_DM_PreviousElement)\
    IOBJ_MDE ("goTo:"		, POPVECTOR_DM_GoToElement)\
    IOBJ_MDE ("size"		, POPVECTOR_DM_Size)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class ABSTRACT rtPOPVECTOR_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (rtPOPVECTOR_Handle, VContainerHandle);

//  Construction
protected:
    rtPOPVECTOR_Handle (M_CTE& rCTE) : VContainerHandle (rCTE) {
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
  popvector.h 1 replace /users/jck/system
  860526 23:25:56 jck Introducing popvectors as a support type 
for the subset of rtypes that is represented as a sequence of POP's

 ************************************************************************/
/************************************************************************
  popvector.h 2 replace /users/jck/system
  860605 16:25:45 jck Release in support of RTvstore

 ************************************************************************/
/************************************************************************
  popvector.h 3 replace /users/jck/system
  860611 11:08:49 jck Added Alignment Routine

 ************************************************************************/
/************************************************************************
  popvector.h 4 replace /users/mjc/system
  860617 18:44:07 mjc Declared 'save' routine

 ************************************************************************/
/************************************************************************
  popvector.h 5 replace /users/jck/system
  870415 09:54:13 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  popvector.h 6 replace /users/mjc/translation
  870524 09:40:29 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  popvector.h 7 replace /users/jck/system
  871007 13:33:08 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  popvector.h 8 replace /users/jck/system
  890222 15:29:24 jck Implemented corruption detection mechanism

 ************************************************************************/
