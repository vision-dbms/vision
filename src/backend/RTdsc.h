/*****  Descriptor Representation Type Header File  *****/
#ifndef rtDSC_H
#define rtDSC_H

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

#include "RTdsc.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtDSC_CPD_StdPtrCount	2

#define rtDSC_CPx_Store		(unsigned int)0
#define rtDSC_CPx_Pointer	(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtDSC_CPD_Base(cpd) reinterpret_cast<rtDSC_Descriptor*>(M_CPD_ContainerBase (cpd))

#define rtDSC_CPD_PointerType(cpd) rtDSC_Descriptor_PointerType (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_RType(cpd) rtDSC_Descriptor_RType (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_ScalarValue(cpd) rtDSC_Descriptor_ScalarValue (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_Store(cpd) M_CPD_PointerToPOP (cpd, rtDSC_CPx_Store)

#define rtDSC_CPD_Pointer(cpd) M_CPD_PointerToPOP (cpd, rtDSC_CPx_Pointer)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl bool rtDSC_UnpackIObject (
    M_ASD *pContainerSpace, IOBJ_IObject iObject, DSC_Descriptor *dscPtr
);

PublicFnDecl M_CPD* rtDSC_Pack (M_ASD *pContainerSpace, DSC_Descriptor *dscPtr);

PublicFnDecl void rtDSC_Unpack (M_CPD *dscCPD, DSC_Descriptor *dscPtr);

PublicFnDecl bool rtDSC_Align (M_CPD *dscCPD);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtDSC_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtDSC_Handle, VContainerHandle);

//  Construction
protected:
    rtDSC_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtDSC_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
protected:
    bool PersistReferences ();

//  State
protected:
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTdsc.h 1 replace /users/mjc/system
  861002 17:59:39 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  RTdsc.h 2 replace /users/mjc/translation
  870524 09:32:41 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
