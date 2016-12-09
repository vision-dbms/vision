/*****  String Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtSTRING

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "rslang.h"

#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTdictionary.h"

/*****  Self  *****/
#include "RTstring.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtSTRING_Handle);


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a standard CPD for a string.
 *
 *  Argument:
 *	cpd			- the address of the standard CPD to be
 *				  initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *cpd) {
    char *string = M_CPD_ContainerBase (cpd);

    rtSTRING_CPD_End (cpd) = (rtSTRING_CPD_Begin (cpd) = string) + strlen (string);
}


/************************************
 *****  Instantiation Routines  *****
 ************************************/

/******************
 *  Instantiator  *
 ******************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new String.
 *
 *  Argument:
 *	string			- the string value for the standard String CPD
 *				  to have.  This string MUST be null delimited.
 *
 *  Returns:
 *	A Standard String CPD for the String created.
 *
 *****/
PublicFnDef M_CPD *rtSTRING_New (M_ASD *pContainerSpace, char const *string) {
    M_CPD *cpd = pContainerSpace->CreateContainer (
	RTYPE_C_String, strlen (string) + 1
    )->NewCPD ();
    
    strcpy (rtSTRING_CPD_Begin (cpd) = M_CPD_ContainerBase (cpd), string);
    rtSTRING_CPD_End (cpd) = rtSTRING_CPD_Begin (cpd) + strlen (string);
    
    return cpd;
}

PublicFnDef M_CPD *rtSTRING_New (char const *pString) {
    return rtSTRING_New (IOBJ_ScratchPad, pString);
}


/*****************************************
 *****  String Manipulation Routines *****
 *****************************************/

/************
 *  Append  *
 ************/

/*---------------------------------------------------------------------------
 *****  Routine to append to a string.
 *
 *  Argument:
 *	orig_cpd		- the address of a standard String CPD for the
 *				  String to be appended to.
 *	new_str			- the address for the string to append to the
 *				  end of the orig_cpd.  This string MUST be
 *				  null delimited.
 *
 *  Returns:
 *	'orig_CPD'
 *
 *****/
PublicFnDef M_CPD *rtSTRING_Append (M_CPD *orig_cpd, char const *new_str) {
    size_t new_sz = strlen (new_str);
    if (new_sz == 0)
	return orig_cpd;

    size_t sz = rtSTRING_CPD_Size (orig_cpd) + new_sz;
    if (sz+1 > (int) M_CPD_Size (orig_cpd))
        orig_cpd->ReallocateContainer (sz+1024);
    memcpy (rtSTRING_CPD_End (orig_cpd), new_str, new_sz);
    rtSTRING_CPD_Begin (orig_cpd)[sz] = '\0';
    rtSTRING_CPD_End (orig_cpd) = rtSTRING_CPD_Begin (orig_cpd) + sz;

    return orig_cpd;
}


/**********
 *  Trim  *
 **********/

/*---------------------------------------------------------------------------
 *****  Routine to remove spaces from the beginning and end of a string.
 *
 *  Argument:
 *	orig_cpd		- the address of a standard String CPD to be
 *				  trimmed.
 *
 *  Returns:
 *	'orig_CPD'
 *
 *  Note:
 *	This routine does NOT give back unused space to the memory manager.
 *
 *****/
PublicFnDef M_CPD *rtSTRING_Trim (M_CPD *orig_cpd) {
    char *str, *sptr, *eptr;

    RTYPE_MustBeA ("rtSTRING_Trim", orig_cpd->RType (), RTYPE_C_String);
    str = rtSTRING_CPD_Begin (orig_cpd);

    for (sptr = str; isspace (*sptr); sptr++);

    for (eptr = sptr + strlen (sptr);
	 eptr > sptr && isspace (*(eptr - 1));
	 eptr--);

    *eptr = '\0';
    strcpy (str, sptr);

    return orig_cpd;
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/********************
 *  Print Function  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to display a String.
 *
 *  Arguments:
 *	cpd			- the address of a standard CPD for the string
 *				  to be displayed.
 *	full			- a boolean,  if true prints string size
 *				  information in addition to the string value.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void PrintString (M_CPD *cpd, int full) {
    if (full) {
	IO_printf ("String[%u]{", rtSTRING_CPD_Size (cpd));
	IO_puts (rtSTRING_CPD_Begin (cpd));
	IO_puts ("}");
    }
    else IO_puts (rtSTRING_CPD_Begin (cpd));
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (Size_DM) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);

    return IOBJ_IntIObject (rtSTRING_CPD_Size (cpd));
}

IOBJ_DefineMethod (Append_DM) {
    M_CPD *newTailCPD = RTYPE_QRegisterCPD (parameterArray[0]);

    RTYPE_MustBeA (
	"'append:' Debug Method", newTailCPD->RType (), RTYPE_C_String
    );

    rtSTRING_Append (
	RTYPE_QRegisterCPD (self), rtSTRING_CPD_Begin (newTailCPD)
    );

    return self;
}

IOBJ_DefineUnaryMethod (Compile_DM) {
    rtDICTIONARY_Handle::Reference pDictionary (new rtDICTIONARY_Handle (IOBJ_ScratchPad));
    M_CPD *pProgram = RSLANG_Compile (
	IOBJ_ScratchPad, rtSTRING_CPD_Begin (RTYPE_QRegisterCPD (self)), pDictionary
    );
    return pProgram ? RTYPE_QRegister (pProgram) : IOBJ_TheNilIObject;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtSTRING_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"	, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	, RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("size"	, Size_DM)
	IOBJ_MDE ("append:"	, Append_DM)
	IOBJ_MDE ("compile"	, Compile_DM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtSTRING_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtSTRING_Handle::Maker;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_QPrintObject:
	PrintString (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	PrintString (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtSTRING);
    FAC_FDFCase_FacilityDescription ("String Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTstring.c 1 replace /users/jad
  860326 19:54:28 jad creation

 ************************************************************************/
/************************************************************************
  RTstring.c 2 replace /users/jad
  860327 19:11:13 jad made append check the size before asking for more

 ************************************************************************/
/************************************************************************
  RTstring.c 3 replace /users/mjc/system
  860412 13:14:06 mjc Added standard CPD declarations, fixed string stuff

 ************************************************************************/
/************************************************************************
  RTstring.c 4 replace /users/mjc/system
  860412 18:10:57 mjc Deleted terminating CR on debugger display

 ************************************************************************/
/************************************************************************
  RTstring.c 5 replace /users/mjc/system
  860414 12:29:56 mjc Delete 'Browser' initialization from M_InitializeMData case

 ************************************************************************/
/************************************************************************
  RTstring.c 6 replace /users/hjb/system
  860423 00:40:07 hjb done updating calls

 ************************************************************************/
/************************************************************************
  RTstring.c 7 replace /users/mjc/system
  860504 00:00:13 mjc Added 'compile' and 'append:' messages

 ************************************************************************/
/************************************************************************
  RTstring.c 8 replace /users/jad/system
  860513 15:15:20 jad added Trim routine to remove spaces from
both end of a RTstring.

 ************************************************************************/
/************************************************************************
  RTstring.c 9 replace /users/mjc/system
  860526 20:32:55 mjc Return vmachine invocation changes

 ************************************************************************/
/************************************************************************
  RTstring.c 10 replace /users/mjc/system
  860531 10:43:00 mjc Changed 'RSLANG_Compile' to pick up its method dictionary from 'envir'

 ************************************************************************/
/************************************************************************
  RTstring.c 11 replace /users/mjc/system
  860625 17:07:40 mjc Converted 'print' message to produce a more natural representation

 ************************************************************************/
/************************************************************************
  RTstring.c 12 replace /users/mjc/system
  860817 20:15:49 mjc Converted CPD initializer/instantiator/gc routines to expect nothing of new CPDs

 ************************************************************************/
/************************************************************************
  RTstring.c 13 replace /users/jck/system
  870415 09:53:39 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTstring.c 14 replace /users/mjc/translation
  870524 09:35:19 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTstring.c 15 replace /users/jck/system
  880114 16:11:12 jck Optimized rtSTRING_Append and added short-term fix to PrintString

 ************************************************************************/
/************************************************************************
  RTstring.c 16 replace /users/jck/system
  880419 07:42:25 jck Removed a temporary fix that had circumvented a VAX printf Bug (since fixed by DEC)

 ************************************************************************/
/************************************************************************
  RTstring.c 17 replace /users/m2/backend
  890503 14:52:26 m2 IO_puts() changes

 ************************************************************************/
