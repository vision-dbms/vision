/*****  Primitive Function Facility Header File  *****/
#ifndef PRIMFNS_H
#define PRIMFNS_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

#include "viobj.h"

#include "VPrimitiveTask.h"


/***********************************************
 ***********************************************
 *****  Primitive/Continuation Definition  *****
 ***********************************************
 ***********************************************/

/****************************************
 *****  Primitive Definition Macro  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Macro to define a primitive or continuation.
 *
 *  Arguments:
 *	functionName		- the name of the primitive being
 *				  defined.
 *
 *  Syntactic Context:
 *	Function Header
 *
 *  Notes:
 *	This macro supplies the header of a private function of type 'void'
 *	suitable for use as a primitive or continuation.
 *
 *****/
#define V_DefinePrimitive(functionName) static void functionName (VPrimitiveTask* pTask)

/**********************************************
 *****  Description Definition Utilities  *****
 **********************************************/

#define BeginDescriptions PrivateVarDef VPrimitiveDescriptor PrimitiveDescriptions[] = {

#define PD(index, name, codeOrIClass) VPrimitiveDescriptor (index, name, codeOrIClass),

#define EndDescriptions \
};

#define PrimitiveDescriptionCount (\
    sizeof (PrimitiveDescriptions) / sizeof (VPrimitiveDescriptor)\
)


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

/*****  Known Primitive Indices  *****/
PublicVarDecl unsigned int V_KnownPrimitive_Super;


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/*****  Primitive I-Type  *****/
IOBJ_DeclareIType (PRIMFNS_PrimitiveIType);

PublicFnDecl char const* PRIMFNS_BasicPrimitiveName (unsigned int xPrimitive);

PublicFnDecl char const* PRIMFNS_PrimitiveName (unsigned int xPrimitive);

PublicFnDecl IOBJ_IObject PRIMFNS_PackPrimitiveIObject (unsigned int xPrimitive);

PublicFnDecl unsigned int PRIMFNS_UnpackPrimitiveIObject (IOBJ_IObject iobject);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  primfns.h 1 replace /users/mjc/system
  860604 18:14:26 mjc Release primitive function facility

 ************************************************************************/
/************************************************************************
  primfns.h 2 replace /users/mjc/system
  860620 17:45:10 mjc Added primitive function I-Type/Objects

 ************************************************************************/
/************************************************************************
  primfns.h 3 replace /users/mjc/system
  860622 22:35:15 mjc Exported primitive function I-Type definition

 ************************************************************************/
/************************************************************************
  primfns.h 4 replace /users/mjc/system
  861002 18:03:21 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  primfns.h 5 replace /users/jad/system
  861116 15:59:36 jad split up the work done in primfns to 6 other files.
Primfns now is more of a covering module for the other six

 ************************************************************************/
/************************************************************************
  primfns.h 6 replace /users/mjc/translation
  870524 09:40:42 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
