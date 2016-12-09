/*****  Magic Word Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName MAGIC

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "vmagic.h"

/*****  Supporting  *****/
#include "verr.h"
#include "vutil.h"

#include "VMagicWordDescriptor.h"


/*****************************************************************
 *****  String <->  Magic Word Index Conversion Routines  *****
 *****************************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the known magic word index for a string.
 *
 *  Argument:
 *	pString			- the address of the first character of a
 *				  string naming the magic word.
 *
 *  Returns:
 *	The index of the magic word if it is known, -1 otherwise.
 *
 *****/
PublicFnDef int MAGIC_StringToMWI (char const* pString) {
    VMagicWordDescriptor const *pDescription = VMagicWordDescriptor::Description (pString, 0);
    return pDescription ? (int)pDescription->code () : -1;
}


/*---------------------------------------------------------------------------
 *****  Routine to return the address of a string naming a known magic word.
 *
 *  Argument:
 *	xMagicWord		- the index of the known magic word.
 *
 *  Returns:
 *	An address of a READ ONLY character string naming the magic word.
 *
 *****/
PublicFnDef char const* MAGIC_MWIToString (unsigned int xMagicWord) {
    VMagicWordDescriptor const *pDescription = VMagicWordDescriptor::Description (xMagicWord, 0);
    return pDescription
	? pDescription->name ()
	: VMagicWord_Datum == xMagicWord
	? "expression"
	: UTIL_FormatMessage ("Magic#%d", xMagicWord);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (MAGIC);
    FAC_FDFCase_FacilityDescription ("Magic Word");
    case FAC_DoStartupInitialization:
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  magic.c 1 replace /users/jck/system
  860411 14:39:52 jck Translate Magic Words from ascii to code
and vice versa.

 ************************************************************************/
/************************************************************************
  magic.c 2 replace /users/mjc/system
  860601 18:26:39 mjc Release virtual machine through byte code 4

 ************************************************************************/
/************************************************************************
  magic.c 3 replace /users/mjc/translation
  870524 09:39:50 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  magic.c 4 replace /users/mjc/system
  870614 20:10:13 mjc Added cast to correct improper VAX demotion of an integer to a character in an expression

 ************************************************************************/
