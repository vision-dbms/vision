/*****  Method Representation Type Header File  *****/
#ifndef rtMETHOD_H
#define rtMETHOD_H

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

#include "RTmethod.d"

/************************
 *****  Supporting  *****
 ************************/

class rtBLOCK_Handle;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/
/**************************************
 *****  Standard CPD Definitions  *****
 **************************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtMETHOD_CPD_StdPtrCount (rtMETHOD_Method_POPCount + POPVECTOR_CPD_XtraPtrCount)

#define rtMETHOD_CPx_Block	(unsigned int)0
#define rtMETHOD_CPx_Origin	(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtMETHOD_CPD_Base(cpd)\
    ((rtMETHOD_Type *)M_ContainerBase (cpd))

/*****  Method Decoding Macros  *****/
#define rtMETHOD_CPD_BlockCPD(cpd) (\
    (cpd)->GetCPD (rtMETHOD_CPx_Block, RTYPE_C_Block)\
)

#define rtMETHOD_CPD_OriginCPD(cpd) (\
    (cpd)->GetCPD (rtMETHOD_CPx_Origin)\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtMETHOD_New (M_ASD *pContainerSpace, rtBLOCK_Handle *pBlock, M_CPD *pMy);

PublicFnDecl bool rtMETHOD_Align (M_CPD *method);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtMETHOD_Handle : public rtPOPVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtMETHOD_Handle, rtPOPVECTOR_Handle);

//  Construction
protected:
    rtMETHOD_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtMETHOD_Handle (rCTE);
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
  RTmethod.h 1 replace /users/mjc/system
  861002 17:55:31 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTmethod.h 2 replace /users/mjc/translation
  870524 09:33:58 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
