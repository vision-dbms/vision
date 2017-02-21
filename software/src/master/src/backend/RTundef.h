/*****  The Undefined Object Representation Type Header File  *****/
#ifndef rtUNDEF_H
#define rtUNDEF_H

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

/************************
 *****  Supporting  *****
 ************************/

/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtUNDEF_New (M_ASD *);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtUNDEF_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtUNDEF_Handle, VContainerHandle);

//  Construction
protected:
    rtUNDEF_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtUNDEF_Handle (rCTE);
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
  RTundef.h 1 replace /users/mjc/system
  860423 11:01:25 mjc Create 'RTundef' header file

 ************************************************************************/
/************************************************************************
  RTundef.h 2 replace /users/mjc/translation
  870524 09:36:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
