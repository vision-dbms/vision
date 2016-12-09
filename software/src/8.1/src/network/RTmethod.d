/*****  Method Representation Type Shared Declarations  *****/
#ifndef rtMETHOD_D
#define rtMETHOD_D

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

#include "popvector.d"


/***************************
 *****  Method Format  *****
 ***************************/
/*---------------------------------------------------------------------------
 * A 'Method' is an adjunct to a value store (similar to L-Store's, etc.)
 * which forms a primitive association between a BLOCK and the store which
 * defines that block's lexical origin (i.e., '^my').  Methods act as
 * extensions to existing objects and not true, independent instances.
 *
 *  Method Field Descriptions:
 *	block			- a POP for the block associated with this
 *				  method.
 *	origin			- the lexical origin (the '^my' value store)
 *				  of this method's block.
 *---------------------------------------------------------------------------
 */
struct rtMETHOD_Type_Method {
    M_POP
	block,
	origin;
};

/*****  Access Macros  *****/
#define rtMETHOD_Method_Block(method)\
    (method).block

#define rtMETHOD_Method_Origin(method)\
    (method).origin

#define rtMETHOD_Method_POPCount (sizeof (rtMETHOD_Type_Method) / sizeof (M_POP))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTmethod.d 1 replace /users/mjc/system
  861002 17:55:29 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTmethod.d 2 replace /users/mjc/translation
  870524 09:33:53 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
