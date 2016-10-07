/*****  Selector U-Vector Representation Type Shared Declarations  *****/
#ifndef rtSELUV_D
#define rtSELUV_D

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

#include "uvector.d"

/*******************************
 *****  Base Element Type  *****
 *******************************/

struct rtSELUV_Type_Element {
    unsigned int
        type	    : 4,
	index	    :28;
};

/*****  Access Macros  *****/
#define rtSELUV_Element_Type(e)		((e)->type)
#define rtSELUV_Element_Index(e)	((e)->index)
#define rtSELUV_Element_MaxIndex	(~((~0) << 28))

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTseluv.d 1 replace /users/mjc/system
  860411 22:02:08 mjc Create 'selector' u-vectors

 ************************************************************************/
/************************************************************************
  RTseluv.d 2 replace /users/mjc/system
  860420 18:49:45 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTseluv.d 3 replace /users/jck/system
  860521 12:37:18 jck First Cut Implementation with enough functionallity for Method Dictionary

 ************************************************************************/
/************************************************************************
  RTseluv.d 4 replace /users/jck/system
  860828 09:45:51 jck Fixed bug in entryType definition

 ************************************************************************/
/************************************************************************
  RTseluv.d 5 replace /users/mjc/system
  870104 22:52:09 lcn Added parallel lookup and addition routines

 ************************************************************************/
/************************************************************************
  RTseluv.d 6 replace /users/mjc/translation
  870524 09:35:10 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
