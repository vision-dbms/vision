/*****  Context Representation Type Shared Declarations  *****/
#ifndef rtCONTEXT_D
#define rtCONTEXT_D

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

#include "popvector.d"

/*************************************
 *****  Realized Context Format  *****
 *************************************/

/*---------------------------------------------------------------------------
 * Realized contexts consist of descriptors for 'current', 'self', and
 * 'origin'.
 *
 *  Context Field Descriptions:
 *	current			- the packed descriptor for 'current'.
 *	self			- the packed descriptor for 'self'.
 *	origin			- the packed descriptor for 'origin'.
 *
 *****/
struct rtCONTEXT_Context {
    M_POP			current;
    M_POP			self;
    M_POP			origin;
};

/*****  Access Macros  *****/
#define rtCONTEXT_Context_Current(contextp)	((contextp)->current)
#define rtCONTEXT_Context_Self(contextp)	((contextp)->self)
#define rtCONTEXT_Context_Origin(contextp)	((contextp)->origin)

/*****  POP Count  *****/
#define rtCONTEXT_Context_POPCount (\
    sizeof (rtCONTEXT_Context) / sizeof (M_POP)\
)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTcontext.d 1 replace /users/mjc/system
  861002 17:55:35 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTcontext.d 2 replace /users/mjc/translation
  870524 09:32:12 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
