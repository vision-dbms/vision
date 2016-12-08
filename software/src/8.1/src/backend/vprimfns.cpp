/*****  Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName PRIMFNS

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "PFalist.h"
#include "PFdate.h"
#include "PFdebug.h"
#include "PFdictionary.h"
#include "PFfault.h"
#include "PFidentity.h"
#include "PFlist.h"
#include "PFnumeric.h"
#include "PFprint.h"
#include "PFrun.h"
#include "PFstats.h"
#include "PFstring.h"
#include "PFts.h"
#include "PFutil.h"

/*****  Shared  *****/
#include "vprimfns.h"


/*********************************************************
 *********************************************************
 *****  The Primitive Dispatch Vector and Utilities  *****
 *********************************************************
 *********************************************************/

/*************************************
 *****  Known Primitive Indices  *****
 *************************************/

PublicVarDef unsigned int V_KnownPrimitive_Super;


/************************************
 *****  Primitive Installation  *****
 ************************************/

void VPrimitiveTaskBase::InstallPrimitives (
    VPrimitiveDescriptor* pDescriptors, unsigned int cDescriptors
)
{
/*****  Initialize the dispatch vector...  *****/
    static bool fDispatchVectorUninitialized = true;
    if (fDispatchVectorUninitialized) {
	for (unsigned int i = 0; i < V_PF_MaxPrimitiveIndex; i++)
	    g_iPDV [i] = NilOf (VPrimitiveDescriptor*);
	fDispatchVectorUninitialized = false;
    }

/*****  ...and process the descriptions.  *****/
    VPrimitiveDescriptor* pDescriptor = pDescriptors + cDescriptors;
    while (--pDescriptor >= pDescriptors) {
	unsigned int i = pDescriptor->index ();
	if (i >= V_PF_MaxPrimitiveIndex) ERR_SignalFault (
	    EC__InternalInconsistency, "Too Many Primitives"
	);

        if (IsntNil (g_iPDV [i])) {
	    ERR_SignalFault (
		EC__InternalInconsistency,
		UTIL_FormatMessage (
		    "Multiple Definitions of Primitive %u: <%s> <%s>",
		    i, pDescriptor->name (), g_iPDV [i]->name ()
		)
	    );
	}

	g_iPDV [i] = pDescriptor;
    }
}


/*****************************************************
 *****************************************************
 *****  Primitive Debugger and I-Object Support  *****
 *****************************************************
 *****************************************************/

/***************************************
 *****  Primitive Naming Routines  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the name of a primitive function given its index.
 *
 *  Argument:
 *	xPrimitive		- the index of the primitive to be named.
 *
 *  Returns:
 *	The address of a read only character string naming the primitive if
 *	the primitive is defined, 'Nil' otherwise.  This routine exists to
 *	provide names to the debugger.  Because the debugger will not add
 *	'Nil' names to its literal table, non-existent primitives will not
 *	added to the debugger's literal table.
 *
 *****/
PublicFnDef char const* PRIMFNS_BasicPrimitiveName (unsigned int xPrimitive) {
    VPrimitiveDescriptor* pDescriptor = VPrimitiveTask::PrimitiveDescriptor (xPrimitive);

    return pDescriptor ? pDescriptor->name () : NilOf (char const *);
}

/*---------------------------------------------------------------------------
 *****  Routine to return the name of a primitive function given its index.
 *
 *  Argument:
 *	xPrimitive		- the index of the primitive to be named.
 *
 *  Returns:
 *	The address of a read only character string naming the primitive.
 *
 *****/
PublicFnDef char const* PRIMFNS_PrimitiveName (unsigned int xPrimitive) {
    unsigned int flags = xPrimitive / 65536;
    xPrimitive %= 65536;
    char const* name = PRIMFNS_BasicPrimitiveName (xPrimitive);

    return name ? (
	flags ? UTIL_FormatMessage ("%s[%u]", name, flags) : name
    ) : flags ? UTIL_FormatMessage ("Primitive#%d[%u]", xPrimitive, flags) : UTIL_FormatMessage (
	"Primitive#%d", xPrimitive
    );
}


/*****************************************
 *****  Primitive I-Type Definition  *****
 *****************************************/

/************************
 *  Method Definitions  *
 ************************/

IOBJ_DefineUnaryMethod (PrintPrimitiveIObject)
{
    IO_printf ("%s", PRIMFNS_PrimitiveName (IOBJ_IObjectValueAsInt (self)));
    return self;
}

/***********************
 *  I-Type Definition  *
 ***********************/

IOBJ_BeginIType (PRIMFNS_PrimitiveIType)
    IOBJ_MDE ("qprint"	    , PrintPrimitiveIObject)
    IOBJ_MDE ("print"	    , PrintPrimitiveIObject)
IOBJ_EndIType


/***************************************************************
 *****  Primitive I-Object Packing and Unpacking Routines  *****
 ***************************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return an I-Object for a primitive.
 *
 *  Argument:
 *	xPrimitive		- the index of the primitive.
 *
 *  Returns:
 *	An I-Object for the primitive.
 *
 *****/
PublicFnDef IOBJ_IObject PRIMFNS_PackPrimitiveIObject (unsigned int xPrimitive) {
    return IOBJ_PackIntIObject (PRIMFNS_PrimitiveIType, xPrimitive);
}

/*---------------------------------------------------------------------------
 *****  Routine to return the index from a primitive function I-Object.
 *
 *  Argument:
 *	iobject			- a I-Object for the primitive function.
 *
 *  Returns:
 *	The index of the primitive.
 *
 *****/
PublicFnDef unsigned int PRIMFNS_UnpackPrimitiveIObject (IOBJ_IObject iobject) {
    IOBJ_IObjectMustBe (iobject, PRIMFNS_PrimitiveIType, "Not a Primitive");

    return (unsigned int)IOBJ_IObjectValueAsInt (iobject);
}


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (PRIMFNS);
    FAC_FDFCase_FacilityDescription ("Primitive Function Services");
    case FAC_DoStartupInitialization:
        pfALIST_InitDispatchVector	();
        pfDATE_InitDispatchVector	();
        pfDEBUG_InitDispatchVector	();
	pfDICTIONARY_InitDispatchVector	();
	pfFAULT_InitDispatchVector	();
	pfIDENTITY_InitDispatchVector	();
        pfLIST_InitDispatchVector	();
        pfNUMERIC_InitDispatchVector	();
	pfPRINT_InitDispatchVector	();
	pfRUN_InitDispatchVector	();
	pfSTATS_InitDispatchVector	();
        pfSTRING_InitDispatchVector	();
        pfTS_InitDispatchVector		();
        pfUTIL_InitDispatchVector	();

	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  primfns.c 1 replace /users/mjc/system
  860604 18:14:22 mjc Release primitive function facility

 ************************************************************************/
/************************************************************************
  primfns.c 2 replace /users/mjc/system
  860620 17:45:07 mjc Added primitive function I-Type/Objects

 ************************************************************************/
/************************************************************************
  primfns.c 3 replace /users/mjc/system
  860622 22:35:10 mjc Exported primitive function I-Type definition

 ************************************************************************/
/************************************************************************
  primfns.c 4 replace /users/mjc/system
  861002 18:03:14 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  primfns.c 4 replace /users/mjc/system
  861002 18:33:41 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  primfns.c 5 replace /users/mjc/mysystem
  861004 19:00:07 mjc Implemented 'identity' testing primitive

 ************************************************************************/
/************************************************************************
  primfns.c 6 replace /users/mjc/mysystem
  861005 11:56:34 mjc Fixed referential inconsistency in 'select:'

 ************************************************************************/
/************************************************************************
  primfns.c 7 replace /users/mjc/mysystem
  861006 09:54:21 mjc Adjusted messages sent by 'IIC'

 ************************************************************************/
/************************************************************************
  primfns.c 8 replace /users/mjc/system
  861008 20:55:32 mjc Fix garbage collection problem in initialized L-Store creation

 ************************************************************************/
/************************************************************************
  primfns.c 9 replace /users/jad/system
  861012 09:42:03 mjc Preliminary release of string and print primitives

 ************************************************************************/
/************************************************************************
  primfns.c 10 replace /users/mjc/system
  861015 23:53:24 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  primfns.c 11 replace /users/mjc/system
  861016 14:51:17 jad make 'print: +number' left justify
strings and 'print: -number right justify strings

 ************************************************************************/
/************************************************************************
  primfns.c 12 replace /users/mjc/system
  861016 18:32:30 jad changed the uvector type conversion routines to accept the new
ref ptoken as an argument

 ************************************************************************/
/************************************************************************
  primfns.c 13 replace /users/jad/system
  861021 19:10:49 jad added string sort

 ************************************************************************/
/************************************************************************
  primfns.c 14 replace /users/jad/system
  861024 15:36:03 jad made link descriptors
contain a linkc and/or a linkcpd to eliminate some conversions

 ************************************************************************/
/************************************************************************
  primfns.c 15 replace /users/mjc/system
  861024 18:30:02 mjc Fixed CPD enslavement in 'ListElementCount'

 ************************************************************************/
/************************************************************************
  primfns.c 16 replace /users/mjc/system
  861028 11:57:17 mjc Fixed reference p-token bug in rank values

 ************************************************************************/
/************************************************************************
  primfns.c 17 replace /users/jad/system
  861106 17:56:41 jad use new fragmentationSubsetInStore to make select faster

 ************************************************************************/
/************************************************************************
  primfns.c 18 replace /usr/rs/versions/5
  861108 01:06:55 mjc Eliminate enslaved CPDs

 ************************************************************************/
/************************************************************************
  primfns.c 19 replace /users/mjc/system
  861109 00:46:15 mjc Release correct implementation of 'asOf:' primitive for multiple dates

 ************************************************************************/
/************************************************************************
  primfns.c 20 replace /users/mjc/system
  861109 14:34:33 mjc Made 'ForDatesInRangeEvaluate' send an 'evaluate:from:to:' message to ^self in list mode

 ************************************************************************/
/************************************************************************
  primfns.c 21 replace /users/jad/system
  861112 10:53:19 jad add HomoginizeDescriptorIfPossible to 
speed things up

 ************************************************************************/
/************************************************************************
  primfns.c 22 replace /users/jad/system
  861116 15:59:28 jad split up the work done in primfns to 6 other files.
Primfns now is more of a covering module for the other six

 ************************************************************************/
/************************************************************************
  primfns.c 23 replace /users/jad/system
  870205 14:57:28 jad added code for the new PFprint module

 ************************************************************************/
/************************************************************************
  primfns.c 24 replace /users/mjc/translation
  870524 09:40:34 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  primfns.c 25 replace /users/jck/system
  870605 16:06:00 jck Incorporate PFalist modules

 ************************************************************************/
/************************************************************************
  primfns.c 26 replace /users/jad/system
  880711 16:15:24 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  primfns.c 27 replace /users/jck/system
  890912 16:05:57 jck Removing RTtstore from the system

 ************************************************************************/
