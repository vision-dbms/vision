/*****  Undefined U-Vector Representation Type Shared Declarations  *****/
#ifndef rtUNDEFUV_D
#define rtUNDEFUV_D

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

#include "uvector.d"


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define rtUNDEFUV_ContainerSize\
    (M_SizeOfPreambleType + M_SizeOfEndMarker + sizeof (UV_PreambleType))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTundefuv.d 1 replace /users/jad/system
  860718 11:24:37 jad added a Distribute Macro

 ************************************************************************/
/************************************************************************
  RTundefuv.d 2 replace /users/jck/system
  860723 12:33:31 jck Added DataBase Update Utility Macros

 ************************************************************************/
/************************************************************************
  RTundefuv.d 3 replace /users/jad/system
  860803 18:47:09 jad added another define for the data base update

 ************************************************************************/
/************************************************************************
  RTundefuv.d 4 replace /users/mjc/translation
  870524 09:36:10 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
