/*****  Selector Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName SELECTOR

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "verr.h"
#include "vfac.h"
#include "vutil.h"

/*****  Shared  *****/
#include "selector.h"


/*****************************************
 *****  Known Selector Descriptions  *****
 *****************************************/

/************************************************
 *  Known Selector Description Type Definition  *
 ************************************************/

struct KSDescriptionType {
    char const *		name;
    unsigned short		index,
				valence;
};

/**************************************************
 *  Known Selector Description Definition Macros  *
 **************************************************/

#define BeginKnownSelectorDescriptions \
PrivateVarDef KSDescriptionType KSDescriptionArray [] = {

#define EndKnownSelectorDescriptions };

#define KS(name, index, valence)\
{name, index, valence},

#define KSDescriptionCount\
    (sizeof (KSDescriptionArray) / sizeof (KSDescriptionType))

/*********************************
 *  Known Selector Descriptions  *
 *********************************/
#include "selector.i"

/****************************************
 *  Known Selector Index Indexed Array  *
 ****************************************/
/*---------------------------------------------------------------------------
 * The known selector index array created in "selector.i" will be sorted by
 * selector name.  The following array, accessed by selector index will be
 * initialized to point to the sorted "selector.i" array.
 *---------------------------------------------------------------------------
 */

PrivateVarDef KSDescriptionType
    *KSIIndexedArray[SELECTOR_C_KSIUpperBound];


/*************************************************
 *  Known Selector Description Search Predicate  *
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine suitable for use with 'qsort(3C)' and 'bsearch(3C)'
 *****  to compare the names of two known selector descriptions.
 *
 *  Arguments:
 *	ksd1, ksd2		- pointers to the known selector descriptions
 *				  to be compared.
 *
 *  Returns:
 *	strcmp (ksd1->name, ksd2->name)
 *
 *****/
PrivateFnDef int __cdecl KSComparisonPredicate (
    KSDescriptionType const *ksd1, KSDescriptionType const *ksd2
) {
    return strcmp (ksd1->name, ksd2->name);
}


/*******************************************************
 *  Known Selector Description Initialization Routine  *
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine called at system startup to set up the known selector
 *****  description arrays.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void InitializeKSDescriptions (
    void
)
{
    int			ksi;
    KSDescriptionType	*p,
			*pl = KSDescriptionArray + KSDescriptionCount;

/*****  Sort the description array ...  *****/
    qsort (
	(char *)KSDescriptionArray, KSDescriptionCount, sizeof (KSDescriptionType), (VkComparator)KSComparisonPredicate
    );

/*****  ... check it for duplicated names ...  *****/
    for (
	p = KSDescriptionArray + 1; p < pl; p++
    ) if (KSComparisonPredicate (p, p - 1) == 0) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Duplicated Known Selector Name: %s %d %d", p->name, p->index, (p - 1)->index
	)
    );
    
/*****  ... and initialize 'KSIIndexedArray'.  *****/
    for (
	ksi = 0;
        ksi < SELECTOR_C_KSIUpperBound;
        KSIIndexedArray[ksi++] = NilOf (KSDescriptionType *)
    );

    for (
	p = KSDescriptionArray; p < pl; p++
    ) if (IsntNil (KSIIndexedArray[ksi = p->index])) ERR_SignalFault (
	EC__UsageError,
	UTIL_FormatMessage (
	    "Duplicated Known Selector Index: %d %s %s",
	     ksi, p->name, KSIIndexedArray[ksi]->name
	)
    );
    else KSIIndexedArray[ksi] = p;
}


/*****************************************************************
 *****  String <-> Known Selector Index Conversion Routines  *****
 *****************************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the known selector index for a string.
 *
 *  Argument:
 *	string			- the address of the first character of a
 *				  string naming the selector.
 *
 *  Returns:
 *	The index of the selector if it is known, -1 otherwise.
 *
 *****/
PublicFnDef int SELECTOR_StringToKSI (
    char const *		string
)
{
    KSDescriptionType
	key, *p;

    key.name = string;
    p = (KSDescriptionType *)bsearch (
	(char *)&key,
	(char *)KSDescriptionArray,
	KSDescriptionCount,
	sizeof (KSDescriptionType),
	(VkComparator)KSComparisonPredicate
    );

    return IsNil (p) ? -1 : (int)p->index;
}


/*---------------------------------------------------------------------------
 *****  Routine to return the address of a string naming a known selector.
 *
 *  Argument:
 *	ksi			- the index of the known selector.
 *
 *  Returns:
 *	An address of a READ ONLY character string naming the selector if
 *	the index is in use, ">>>Unknown Selector<<<" otherwise.
 *
 *****/
PublicFnDef char const *KS__ToString (unsigned int ksi) {
    KSDescriptionType *p;

    return ksi >= SELECTOR_C_KSIUpperBound || IsNil (p = KSIIndexedArray[ksi])
	? ">>>Unknown Selector<<<"
	: (char *)p->name;
}


/*******************************************
 *****  Known Selector Valence Access  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the valence of a known selector given its index.
 *
 *  Argument:
 *	ksi			- the index of the known selector.
 *
 *  Returns:
 *	The valence of the selector if 'ksi' identifies a valid known selector,
 *	-1 otherwise.
 *
 *****/
PublicFnDef int SELECTOR_KSValence (
    unsigned int		ksi
)
{
    KSDescriptionType *p;

    return
	ksi >= SELECTOR_C_KSIUpperBound || IsNil (p = KSIIndexedArray[ksi])
	? -1
	: (int)p->valence;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (SELECTOR);
    FAC_FDFCase_FacilityDescription ("Selector");
    case FAC_DoStartupInitialization:
        InitializeKSDescriptions ();
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  selector.c 1 replace /users/mjc/system
  860404 09:14:01 mjc Selector services (including known selectors)

 ************************************************************************/
/************************************************************************
  selector.c 2 replace /users/mjc/system
  860407 17:02:16 mjc Added KS valence access, altered selector descriptor type

 ************************************************************************/
/************************************************************************
  selector.c 3 replace /users/mjc/system
  861002 18:03:30 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  selector.c 4 replace /users/mjc/translation
  870524 09:41:33 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  selector.c 5 replace /users/mjc/system
  870614 20:10:18 mjc Added cast to correct improper VAX demotion of an integer to a character in an expression

 ************************************************************************/
