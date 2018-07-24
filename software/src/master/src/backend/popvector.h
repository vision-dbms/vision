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

#include "M_CPD.h"


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


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class ABSTRACT rtPOPVECTOR_Handle : public VContainerHandle {
    DECLARE_ABSTRACT_RTT (rtPOPVECTOR_Handle, VContainerHandle);

//  Construction
protected:
    rtPOPVECTOR_Handle (M_ASD *pSpace, RTYPE_Type xType, unsigned int cElements);
    rtPOPVECTOR_Handle (M_ASD *pSpace, RTYPE_Type xType);
    rtPOPVECTOR_Handle (M_CTE& rCTE) : VContainerHandle (rCTE) {
    }
protected:
    void CreateContainer (unsigned int cElements);
private:
    void constructContainer (unsigned int cElements) const;

//  Destruction
protected:
    ~rtPOPVECTOR_Handle () {
    }

//  Alignment
public:
    using BaseClass::align;
protected:
    bool align (unsigned int xAlignmentPToken, unsigned int xRegion, unsigned int sRegion);

//  Access
private:
    POPVECTOR_PVType *typecastContent () const {
	return reinterpret_cast<POPVECTOR_PVType*> (containerContent ());
    }
protected:
    M_POP *array () const {
	return reinterpret_cast<M_POP*>(typecastContent () + 1);
    }
public:
    M_POP *element (unsigned int xElement) const {
	return array() + xElement;
    }
    unsigned int elementCount () const {
	return POPVECTOR_PV_ElementCount (typecastContent ());
    }
    VContainerHandle *elementHandle (unsigned int xElement, RTYPE_Type xExpectedType = RTYPE_C_Any) const {
	return GetContainerHandle (element (xElement), xExpectedType);
    }

//  Query
public:
    bool isInconsistent () const {
	return hasNoContainer () || POPVECTOR_PV_IsInconsistent (typecastContent ());
    }

    bool ReferenceIsNil (unsigned int xElement) const {
	return BaseClass::ReferenceIsNil (element (xElement));
    }

    bool ReferenceNames (unsigned int xElement, M_CPD const *pThat) const {
	return ReferenceNames (xElement, pThat->containerHandle ());
    }
    bool ReferenceNames (unsigned int xElement, M_CPD const *pThat, unsigned int xThatReference) const {
	return ReferenceNames (xElement, pThat->containerHandle (), M_CPD_PointerToPOP (pThat, xThatReference));
    }
    bool ReferenceNames (unsigned int xElement, VContainerHandle const *pThat) const {
	return BaseClass::ReferenceNames (element (xElement), pThat);
    }
    bool ReferenceNames (unsigned int xElement, VContainerHandle const *pThat, M_POP const *pThatReference) const {
	return BaseClass::ReferenceNames (element (xElement), pThat, pThatReference);
    }

    bool ReferenceIsntNil (unsigned int xElement) const {
	return BaseClass::ReferenceIsntNil (element (xElement));
    }

    bool ReferenceDoesntName (unsigned int xElement, M_CPD const *pThat) const {
	return ReferenceDoesntName (xElement, pThat->containerHandle ());
    }
    bool ReferenceDoesntName (unsigned int xElement, M_CPD const *pThat, unsigned int xThatReference) const {
	return ReferenceDoesntName (xElement, pThat->containerHandle (), M_CPD_PointerToPOP (pThat, xThatReference));
    }
    bool ReferenceDoesntName (unsigned int xElement, VContainerHandle const *pThat) const {
	return BaseClass::ReferenceDoesntName (element (xElement), pThat);
    }
    bool ReferenceDoesntName (unsigned int xElement, VContainerHandle const *pThat, M_POP const *pThatReference) const {
	return BaseClass::ReferenceDoesntName (element (xElement), pThat, pThatReference);
    }

//  Update
private:
    void clearIsInconsistent () {
	setIsInconsistentTo (false);
    }
    void setIsInconsistent () {
	setIsInconsistentTo (true);
    }
    void setIsInconsistentTo (bool bValue) {
	POPVECTOR_PV_IsInconsistent (typecastContent ()) = bValue;
    }

    void adjustElementCountBy (int sChange) {
	POPVECTOR_PV_ElementCount (typecastContent ()) += sChange;
    }
    void decrementElementCountBy (unsigned int sChange) {
	POPVECTOR_PV_ElementCount (typecastContent ()) -= sChange;
    }
    void incrementElementCountBy (unsigned int sChange) {
	POPVECTOR_PV_ElementCount (typecastContent ()) += sChange;
    }
    void setElementCountTo (unsigned int cElements) {
	POPVECTOR_PV_ElementCount (typecastContent ()) = cElements;
    }

protected:
    void StoreReference (unsigned int xElement, M_CPD const *pThat) {
	StoreReference (xElement, pThat->containerHandle ());
    }
    void StoreReference (unsigned int xElement, M_CPD const *pThat, unsigned int xThatReference) {
	StoreReference (xElement, pThat->containerHandle (), M_CPD_PointerToPOP (pThat, xThatReference));
    }
    void StoreReference (unsigned int xElement, VContainerHandle *pThat) {
	BaseClass::StoreReference (element (xElement), pThat);
    }
    void StoreReference (unsigned int xElement, VContainerHandle const *pThat, M_POP const *pThatReference) {
	BaseClass::StoreReference (element (xElement), pThat, pThatReference);
    }
    void StoreReference (unsigned int xElement, Vdd::Store *pStore) {
	BaseClass::StoreReference (element (xElement), pStore);
    }

//  Callbacks
public:
    virtual void CheckConsistency () OVERRIDE;

protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual unsigned int getPOPCount () const OVERRIDE {
	return elementCount ();
    }
    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
};

typedef VStoreHandle_<rtPOPVECTOR_Handle> rtPOPVECTOR_StoreHandle;


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void POPVECTOR_InitStdCPD (M_CPD *cpd);

PublicFnDecl void POPVECTOR_ReclaimContainer (
    M_ASD			*ownerASD,
    M_CPreamble	const		*preambleAddress
);

PublicFnDecl void POPVECTOR_MarkContainers (
    M_ASD::GCVisitBase* pGCV,
    M_ASD* pSpace,
    M_CPreamble	const		*pContainer
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
