/*****  Integer U-Vector Representation Type Shared Declarations  *****/
#ifndef rtINTUV_D
#define rtINTUV_D

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

#include "uvector.d"

/*******************************
 *****  Base Element Type  *****
 *******************************/

typedef int rtINTUV_ElementType;

#if defined(__hp9000s700) && defined(__cplusplus)
#define rtINTUV_IsAValidInteger(value) (\
    ((double)-INT_MAX - 1) <= (value) && (value) <= (double)INT_MAX\
)

#else
#define rtINTUV_IsAValidInteger(value) (\
    (double)INT_MIN <= (value) && (value) <= (double)INT_MAX\
)

#endif


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTintuv.d 1 replace /users/mjc/system
  860504 18:42:39 mjc add decl files

 ************************************************************************/
/************************************************************************
  RTintuv.d 2 replace /users/jad/system
  860717 19:26:16 jad implemented rtINTUV_Distribute

 ************************************************************************/
/************************************************************************
  RTintuv.d 3 replace /users/mjc/translation
  870524 09:33:15 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
