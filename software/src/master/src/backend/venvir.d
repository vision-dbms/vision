/*****  Run Time Environment Shared Definitions  *****/
#ifndef ENVIR_D
#define ENVIR_D

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

#include "m.d"

/*****************************************
 *****  Built In Object Definitions  *****
 *****************************************/
/*---------------------------------------------------------------------------
 * The built-in objects fall into four general categories:
 *	Behaviors	- value stores which define the protocol of the
 *			  primitively extant objects (i.e., numbers, dates,
 *			  etc.).
 *	Prototypes	- value stores which can be cloned to hold
 *			  non-primitive objects which the system instantiates
 *			  or structures specially (i.e., lists, time-series,
 *			  etc.).
 *	Dictionaries	- method dictionaries which define the protocol
 *			  directly associated with certain primitive classes
 *			  (i.e, 'Closure', 'Method', etc.).
 *	Miscellany	- all remaining primitive objects.
 *
 * The following definitions specify the field order of the P-Array used to
 * hold POPs for the built-in objects of the system.  New objects should be
 * be added only at the end of this list or by replacing '_available*_[0-9]+'
 * entries.  Inserting, deleting, or re-ordering these definitions requires
 * the reconstruction of all object spaces and versions of the system used
 * with those spaces.


 * Behaviors:
 *	TheNAClass		- the value store which defines the behavior
 *				  of all instances of class 'Undefined' (NA).
 *	TheNoValueClass		- the value store which defines the behavior
 *				  of all instances of class 'NoValue'.
 *	TheSelectorClass	- the value store which defines the behavior
 *				  of all instances of class 'Selector'.
 *	TheTrueClass		- the value store which defines the behavior
 *				  of all instances of class 'True'.
 *	TheFalseClass		- the value store which defines the behavior
 *				  of all instances of class 'False'.
 *	TheIntegerClass		- the value store which defines the behavior
 *				  of all instances of class 'Integer'.
 *	TheFloatClass		- the value store which defines the behavior
 *				  of all instances of class 'Float'.
 *	TheDoubleClass		- the value store which defines the behavior
 *				  of all instances of class 'Double'.
 *	TheDateClass		- the value store which defines the behavior
 *				  of all instances of class 'Date'.
 *	TheCharacterClass	- the value store which defines the behavior
 *				  of all instances of class 'Character'.
 *	ThePrimitiveClass	- the value store which defines the behavior
 *				  of all instances of class 'Primitive'.
 *	TheClosureClass		- the value store which defines the behavior
 *				  of all instances of class 'Closure'.
 *	TheMethodClass		- the value store which defines the behavior
 *				  of all instances of class 'Method'.
 *	TheFixedPropertyClass	- the value store which defines the behavior
 *				  of all instances of class 'FixedProperty'
 *				  ('Value').
 *	TheTimeSeriesClass	- the value store which defines the behavior
 *				  of all instances of class 'TimeSeries'.


 *  Prototypes:
 *	TheObjectPrototype	- a POP for a value store which can be cloned
 *				  to instantiate class 'Object'.
 *	TheListClass		- the value store which defines the behavior
 *				  of all instances of class 'List'.
 *	TheIndexedListClass	- the value store which defines the behavior
 *				  of all instances of class 'IndexedList'.
 *	TheStringClass		- the value store which defines the behavior
 *				  of all instances of class 'String'.
 *	TheFixedPropertySpecificationClass
 *				- the value store which designates a fixed
 *				  property in a legacy dictionary.
 *	TheTimeSeriesPropertySpecificationClass
 *				- a POP for a value store which can be cloned
 *				  to instantiate class 'TimeVaryingProperty'
 *	TheFixedPropertyPrototype
 *				- a POP for the vector that serves as the
 *				  property prototype for compatibility mode
 *				  fixed property definitions.
 *	TheTimeSeriesPropertyPrototype
 *				- a POP for the index that serves as the
 *				  property prototype for compatibility mode
 *				  time varying property definitions.


 *  Dictionaries:
 *	TheClosureClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'Closure'.
 *	TheMethodClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'Method'.
 *	TheFixedPropertyClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'FixedProperty'.
 *	TheTimeSeriesClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'TimeSeries'.
 *	TheListClassDictionary	- the dictionary which contains the definitions
 *				  directly associated with class 'List'.
 *	TheIndexedListClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'IndexedList'.
 *	TheStringClassDictionary- the dictionary which contains the definitions
 *				  directly associated with class 'String'.
 *	TheSelectorClassDictionary
 *				- the dictionary which contains the definitions
 *				  directly associated with class 'Selector'.


 * Miscellany:
 *	TheScalarPToken		- a P-Token for the set of all scalars in the
 *				  system.  Used to control the packing and
 *				  unpacking of value descriptors.
 *	TheDefaultProperty	- a P-Token for a value descriptor identifying
 *				  the instance of class 'Property' used to
 *				  create all non-time-varying properties in the
 *				  absence of other information.
 *	TheDefaultListProperty	- a P-Token for a value descriptor identifying
 *				  the instance of class 'ListProperty' used to
 *				  create all non-time-varying list valued
 *				  properties in the absence of other
 *				  information.
 *	TheDefaultAListProperty	- a P-Token for a value descriptor identifying
 *				  the instance of class 'AListProperty' used to
 *				  create all non-time-varying alist valued
 *				  properties in the absence of other
 *				  information.
 *	TheDefaultStringProperty
 *				- a P-Token for a value descriptor identifying
 *				  the instance of class 'StringProperty' used
 *				  to create all non-time-varying string valued
 *				  properties in the absence of other
 *				  information.
 *	TheDefaultTVProperty
 *				- a P-Token for a value descriptor identifying
 *				  the instance of class 'TimeVaryingProperty'
 *				  used to create all time varying properties in
 *				  absence of other information.
 *	TheDefaultTVListProp
 *				- a P-Token for a value descriptor identifying
 *				  the instance of 'TimeVaryingListProperty'
 *				  used to create all time varying list valued
 *				  properties in the absence of other
 *				  information.
 *	TheDefaultTVAListProp
 *				- a P-Token for a value descriptor identifying
 *				  the instance of 'TimeVaryingAListProperty'
 *				  used to create all time varying alist valued
 *				  properties in the absence of other
 *				  information.
 *	TheDefaultTVStringProp
 *				- a P-Token for a value descriptor identifying
 *				  the instance of 'TimeVaryingStringProperty'
 *				  used to create all time varying string valued
 *				  properties in the absence of other
 *				  information.
 *	TheSelectorPToken	- the P-Token for the set of all selectors.
 *				  Created internally by the boot strap process.
 *	TheRootEnvironment	- a descriptor for the root user environment.
 *---------------------------------------------------------------------------
 */

#define ENVIR_KOTEs(KOTE,AKOE)\
    KOTE (TheNAClass),\
    KOTE (TheNoValueClass),\
    KOTE (TheSelectorClass),\
    KOTE (TheTrueClass),\
    KOTE (TheFalseClass),\
    KOTE (TheIntegerClass),\
    KOTE (TheFloatClass),\
    KOTE (TheDoubleClass),\
    KOTE (TheDateClass),\
    KOTE (TheCharacterClass),\
    KOTE (ThePrimitiveClass),\
    KOTE (TheClosureClass),\
    KOTE (TheMethodClass),\
    KOTE (TheFixedPropertyClass),\
    KOTE (TheTimeSeriesClass),\
    AKOE (16)\
    AKOE (17)\
    AKOE (18)\
    AKOE (19)\
    AKOE (20)\
    AKOE (21)\
    AKOE (22)\
    AKOE (23)\
    AKOE (24)\
    AKOE (25)\
    AKOE (26)\
    AKOE (27)\
    AKOE (28)\
    AKOE (29)\
    AKOE (30)\
    AKOE (31)\
    AKOE (32)\
    KOTE (TheObjectPrototype),\
    AKOE (34)\
    AKOE (35)\
    KOTE (TheListClass),\
    KOTE (TheStringClass),\
    KOTE (TheFixedPropertySpecificationClass),\
    KOTE (TheIndexedListClass),\
    AKOE (40)\
    KOTE (TheTimeSeriesPropertySpecificationClass),\
    AKOE (42)\
    AKOE (43)\
    AKOE (44)\
    AKOE (45)\
    AKOE (46)\
    KOTE (TheFixedPropertyPrototype),\
    KOTE (TheTimeSeriesPropertyPrototype),\
    KOTE (TheClosureClassDictionary),\
    KOTE (TheMethodClassDictionary),\
    KOTE (TheFixedPropertyClassDictionary),\
    KOTE (TheTimeSeriesClassDictionary),\
    KOTE (TheListClassDictionary),\
    KOTE (TheStringClassDictionary),\
    KOTE (TheSelectorClassDictionary),\
    KOTE (TheIndexedListClassDictionary),\
    AKOE (57)\
    AKOE (58)\
    AKOE (59)\
    AKOE (60)\
    AKOE (61)\
    AKOE (62)\
    AKOE (63)\
    AKOE (64)\
    KOTE (TheScalarPToken),\
    KOTE (TheDefaultProperty),\
    KOTE (TheDefaultListProperty),\
    KOTE (TheDefaultStringProperty),\
    KOTE (TheDefaultTVProperty),\
    KOTE (TheDefaultTVListProp),\
    KOTE (TheDefaultTVStringProp),\
    KOTE (TheSelectorPToken),\
    KOTE (TheDefaultAListProperty),\
    KOTE (TheDefaultTVAListProp),\
    AKOE (75)\
    AKOE (76)\
    AKOE (77)\
    AKOE (78)\
    AKOE (79)\
    KOTE (TheRootEnvironment)


/******************************************
 *****  ENVIR_KOTEs Expansion Macros  *****
 ******************************************/

#define ENVIR_KOTE_Include(n) n
#define ENVIR_KOTE_IncludeP2TKOTEM(n) &M_KnownObjectTable::n

#define ENVIR_AKOE_Exclude(n)
#define ENVIR_AKOE_Include(n) _available_##n,
#define ENVIR_AKOE_IncludeZero(n) 0,


/*****************************************************
 *****  Built-In Object P/CPD Array Definitions  *****
 *****************************************************/

/*****  'RTYPE_C_PArray' Overlay Type Definition  *****/
struct ENVIR_BuiltInObjectPArray {
    M_POP ENVIR_KOTEs (ENVIR_KOTE_Include, ENVIR_AKOE_Include);
};

/*****  Built In Object P/CPD Array Element Count  *****/
#define ENVIR_BuiltInObjectPArraySize\
    (sizeof (ENVIR_BuiltInObjectPArray) / sizeof (M_POP))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  envir.d 1 replace /users/mjc/system
  860531 10:41:41 mjc Added run time 'envir' facility

 ************************************************************************/
/************************************************************************
  envir.d 2 replace /users/mjc/system
  860531 17:57:08 mjc Release new known CPD names

 ************************************************************************/
/************************************************************************
  envir.d 3 replace /users/mjc/system
  860606 13:28:10 mjc Release numeric/primitive function method dictionary initialization

 ************************************************************************/
/************************************************************************
  envir.d 4 replace /users/mjc/system
  860610 11:19:10 mjc Added code to create 'NoValueBehavior'

 ************************************************************************/
/************************************************************************
  envir.d 5 replace /users/mjc/system
  860709 10:19:29 mjc Release new format value descriptors and boot strap environment

 ************************************************************************/
/************************************************************************
  envir.d 6 replace /users/mjc/system
  860713 17:13:31 mjc Release new format environment structure

 ************************************************************************/
/************************************************************************
  envir.d 7 replace /users/mjc/system
  860728 13:55:46 mjc Added new property types

 ************************************************************************/
/************************************************************************
  envir.d 8 replace /users/mjc/system
  860803 00:18:38 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  envir.d 9 replace /users/mjc/system
  860826 14:21:55 mjc Added default list and string properties and character behavior, deleted class, metaclass, and date offset

 ************************************************************************/
/************************************************************************
  envir.d 10 replace /users/mjc/system
  861002 18:02:52 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  envir.d 11 replace /users/mjc/system
  870215 23:07:35 lcn Added selector p-token as a special p-token created during bootstrap

 ************************************************************************/
/************************************************************************
  envir.d 12 replace /users/mjc/translation
  870524 09:37:42 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  envir.d 13 replace /users/jck/system
  870605 16:06:40 jck Added known objects to support associative lists

 ************************************************************************/
/************************************************************************
  envir.d 14 replace /users/mjc/system
  870607 03:08:12 mjc Shorten 'ENVIR_KCPD_TheDefaultTVAListProp' to <= 31 characters

 ************************************************************************/
