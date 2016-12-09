/*****  String Representation Type Header File  *****/
#ifndef rtSTRING_H
#define rtSTRING_H

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

#include "RTstring.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

#define rtSTRING_CPD_StdPtrCount	    2

#define rtSTRING_CPD_Begin(cpd)\
    M_CPD_PointerToType (cpd, 0, char *)

#define rtSTRING_CPD_End(cpd)\
    M_CPD_PointerToType (cpd, 1, char *)
    /* this points to the null character terminating the string */ 

#define rtSTRING_CPD_Size(cpd)\
    ((size_t)(rtSTRING_CPD_End(cpd) - rtSTRING_CPD_Begin(cpd)))


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtSTRING_New (M_ASD *pContainerSpace, char const *string);
PublicFnDecl M_CPD *rtSTRING_New (char const *pString);

PublicFnDecl M_CPD *rtSTRING_Append (M_CPD *orig_cpd, char const *new_str);

PublicFnDecl M_CPD *rtSTRING_Trim (M_CPD *orig_cpd);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtSTRING_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtSTRING_Handle, VContainerHandle);

//  Construction
protected:
    rtSTRING_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtSTRING_Handle (rCTE);
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
  RTstring.h 1 replace /users/jad
  860326 19:54:32 jad creation

 ************************************************************************/
/************************************************************************
  RTstring.h 2 replace /users/jad/system
  860513 15:15:24 jad added Trim routine to remove spaces from
both end of a RTstring.

 ************************************************************************/
/************************************************************************
  RTstring.h 3 replace /users/mjc/translation
  870524 09:35:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
