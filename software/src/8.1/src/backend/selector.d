/*****  Selector Shared Definitions  *****/
#ifndef SELECTOR_D
#define SELECTOR_D

/**********************************************
 *****  Known Selector Index Upper Bound  *****
 **********************************************/
/*---------------------------------------------------------------------------
 * The following constant defines a value 1 greater than the index of the
 * largest possible known selector.
 *---------------------------------------------------------------------------
 */

#define SELECTOR_C_KSIUpperBound	    256


/***********************************************
 *****  Known Selector Symbolic Constants  *****
 ***********************************************/
/*---------------------------------------------------------------------------
 * Known selectors are those selectors which must be sent to objects by either
 * the virtual machine or a primitive.  Every known selector must have an
 * associated symbolic constant.  The following definitions provide those
 * associations.  All known selector symbolic constants must be unique and
 * less than 'SELECTOR_C_KSIUpperBound'.  Because known selector indices are
 * saved in permanent object spaces, once assigned, a known selector index
 * cannot be changed.
 *---------------------------------------------------------------------------
 */

/*****  Binary Selectors...  *****/
#define KS__LAssign				0
#define KS__RAssign				1
#define KS__Comma				2
#define KS__LessThan				3
#define KS__LessThanOrEqual			4
#define KS__GreaterThanOrEqual			5
#define KS__GreaterThan				6
#define KS__Equal				7
#define KS__Equivalent				8
#define KS__NotEqual				9
#define KS__NotEquivalent			10
#define KS__Or					11
#define KS__And					12
#define KS__Plus				13
#define KS__Minus				14
#define KS__Times				15
#define KS__Divide				16

/*****  Object Support Selectors...  *****/
#define KS__Print				32
#define KS__Value				33
#define KS__Environment				34
#define KS__Send				35
#define KS__Extend				36
#define KS__AsMethod				37
#define KS__AsClosure				38
#define KS__AddToDictionaryOf			39
#define KS__BasicSend				41
#define KS__BasicExtend				42
#define KS__BasicSendWith			43
#define KS__BasicSendCanonicalized4Sort		44
#define KS__Super				45

/*****  Binary/Keyword Converse Selectors...  *****/
#define KS__BreakString				58
#define KS__CBreakString			59
#define KS__EvaluateFromTo			60
#define KS__PrefixSpannedIn			61
#define KS__PrefixNotSpannedIn			62
#define KS__MatchesString			63
#define KS__SubtractedFrom			64
#define KS__DividedInto				65
#define KS__ReferencedBy			66
#define KS__PointedToBy				67
#define KS__ClosureOf				68
#define KS__PrintColon				69
#define KS__Pow					70
#define KS__Wop					71
#define KS__PrintWithCommasColon		72
#define KS__LocateOrAddDictionaryEntry4		73
#define KS__LocateDictionaryEntryFor		74
#define KS__DBUpdateReplaceFile			75
#define KS__DBUpdateAppendFile			76
#define KS__DeleteDictionaryEntryFor		77
#define KS__SPrintColon				78
#define KS__SPrintWithCommasColon		79
#define KS__FillFromString			80
#define KS__TakeFromString			81
#define KS__DropFromString			82
#define KS__ReverseConcat			83
#define KS__StoreSelectRows			84
#define KS__LocateInAList			85
#define KS__DefineInAList			86
#define KS__DeleteFromAList			87
#define KS__Base0CellPositionOf 		88
#define KS__Base1CellPositionOf 		89
#define KS__DeleteColumnFromStore		90
#define KS__AppendTo				91
#define KS__ElementOf				92

/*****  Dialogue / VISION X-Widget Support Selectors...  *****/
#define KS__Pack8ByteWithMSW			95

#define KS__DLGSetStringIn			101
#define KS__DLGSetBooleanIn			102
#define KS__DLGSetDoubleIn			103
#define KS__DLGSetLongIn			104
#define KS__DLGSetObjectIn			105

#define KS__DLGAppendStringIn			106
#define KS__DLGAppendBooleanIn			107
#define KS__DLGAppendDoubleIn			108
#define KS__DLGAppendLongIn			109
#define KS__DLGAppendObjectIn			110

#define KS__VXWSetSlotIn			111
#define KS__VXWSendTo				112

/*****  Multi/Keyword Converse Selectors...  *****/
#define KS__ByToEvaluate			150
#define KS__ByFromEvaluate		    	151
#define KS__WithCompilationErrorHandlerEvaluate	152


/****************************************
 *****  Selector Descriptor Format  *****
 ****************************************/
/*---------------------------------------------------------------------------
 * A 'selector descriptor' is a transient data structure used by the virtual
 * machine as an identifier for the selector of a message.  Currently, a
 * selector is not implemented as a physical object; rather, either the text
 * of its name or its index in the known selector table is stored directly in
 * any block or environment that references it.  Selector descriptors serve
 * two purposes - they unify the dispatch treatment of known and user-defined
 * selectors and they provide a simple structure which the virtual machine
 * builds directly on the control stack to identify the name of the message
 * upon whose dispatch it is currently working.  Because selector descriptors
 * are built directly on the control stack, the interpreter can avoid the
 * overhead of CPD allocation.
 *
 * Because of the special way in which they are used, this facility provides
 * no direct services other than the macros defined below for creating and
 * manipulating selector descriptors.
 *
 * Selector Descriptor Field Definitions:
 *	type			- a type tag chosen from the set:
 *				    SELECTOR_C_UndefinedSelector
 *				    SELECTOR_C_KnownSelector
 *				    SELECTOR_C_DefinedSelector
 *	ksi			- the index of the selector if this is a
 *				  known selector descriptor (i.e., the 'type'
 *				  field contains SELECTOR_C_KnownSelector').
 *	name			- the actual name of the selector if this is a
 *				  user defined selector descriptor (i.e., 
 *				  'type == SELECTOR_C_DefinedSelector').
 *---------------------------------------------------------------------------
 */

/*****  Selector Descriptor Type Constants  *****/
#define SELECTOR_C_UndefinedSelector	0
#define SELECTOR_C_KnownSelector	1
#define SELECTOR_C_DefinedSelector	2

/*****  Selector Descriptor Type Definition  *****/
struct SELECTOR_DescriptorType {
    unsigned char
	type;
    union content_t {
	unsigned char
	    ksi;
	char
	    name [1];
    } content;
};


/*****  Sizing Macros  *****/
/*---------------------------------------------------------------------------
 * Given a string, the following macro determines how much ADDITIONAL space to
 * add to the size of a selector descriptor to accomodate the string.
 *---------------------------------------------------------------------------
 */

#define SELECTOR_D_AdditionlStringSpace(string)\
    strlen (string)

/*---------------------------------------------------------------------------
 * Given a string, the following macro determines how much TOTAL space is
 * required to store a user-defined-selector descriptor for the string.
 *---------------------------------------------------------------------------
 */
#define SELECTOR_D_Size(string)\
    (sizeof (SELECTOR_DescriptorType) +\
        SELECTOR_D_AdditionlStringSpace (string))

/*****  Access Macros  *****/
#define SELECTOR_D_Type(p)\
    (p)->type

#define SELECTOR_D_Ksi(p)\
    (p)->content.ksi

#define SELECTOR_D_Name(p)\
    (p)->content.name

#define SELECTOR_D_IsUndefined(p)\
    (SELECTOR_D_Type (p) == SELECTOR_C_UndefinedSelector)

#define SELECTOR_D_IsKnown(p)\
    (SELECTOR_D_Type (p) == SELECTOR_C_KnownSelector)

#define SELECTOR_D_IsDefined(p)\
    (SELECTOR_D_Type (p) == SELECTOR_C_DefinedSelector)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  selector.d 1 replace /users/mjc/system
  860404 09:14:03 mjc Selector services (including known selectors)

 ************************************************************************/
/************************************************************************
  selector.d 2 replace /users/mjc/system
  860407 17:02:19 mjc Added KS valence access, altered selector descriptor type

 ************************************************************************/
/************************************************************************
  selector.d 3 replace /users/mjc/system
  860412 16:36:45 mjc Added 'SELECTOR_D_Size' macro

 ************************************************************************/
/************************************************************************
  selector.d 4 replace /users/mjc/system
  861002 18:03:33 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  selector.d 5 replace /users/jad/system
  861012 09:42:11 mjc Preliminary release of string and print primitives

 ************************************************************************/
/************************************************************************
  selector.d 6 replace /users/mjc/system
  861015 23:53:31 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  selector.d 7 replace /users/mjc/system
  861109 00:40:54 mjc Define 'evaluate:from:to:' selector for use by date range iterator

 ************************************************************************/
/************************************************************************
  selector.d 8 replace /users/jad/system
  861120 15:51:57 jad add keywords for power and its converse

 ************************************************************************/
/************************************************************************
  selector.d 9 replace /users/jad/system
  861201 18:27:57 jad added PrintWithCommasOf primitives

 ************************************************************************/
/************************************************************************
  selector.d 10 replace /users/mjc/system
  861226 12:25:41 mjc Implemented 'super'

 ************************************************************************/
/************************************************************************
  selector.d 11 replace /users/mjc/system
  870104 22:51:27 lcn Added dictionary search known selector converses

 ************************************************************************/
/************************************************************************
  selector.d 12 replace /users/jad/system
  870120 15:00:37 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  selector.d 13 replace /users/jad/system
  870128 19:05:54 jad added DeleteDictionaryEntry... defines

 ************************************************************************/
/************************************************************************
  selector.d 14 replace /users/jad/system
  870204 15:27:49 jad added sprint selectors

 ************************************************************************/
/************************************************************************
  selector.d 15 replace /users/jad/system
  870210 14:21:13 jad add string fill, take, & drop selectors

 ************************************************************************/
/************************************************************************
  selector.d 16 replace /users/mjc/system
  870211 13:19:45 mjc Inserted missing colons, deleted 'delete:fromDictionaryOf:'

 ************************************************************************/
/************************************************************************
  selector.d 17 replace /users/jad/system
  870211 16:31:07 jad added string concat selector

 ************************************************************************/
/************************************************************************
  selector.d 18 replace /users/jad/system
  870220 15:50:49 jad implemented SelectedRowsInsStore primitive

 ************************************************************************/
/************************************************************************
  selector.d 19 replace /users/mjc/translation
  870524 09:41:37 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  selector.d 20 replace /users/jck/system
  870605 16:03:58 jck Added binary converse selectors for indexed list primitives

 ************************************************************************/
/************************************************************************
  selector.d 21 replace /users/jck/system
  870810 14:15:39 jck Added binary converse selectors for associative list deletion

 ************************************************************************/
/************************************************************************
  selector.d 22 replace /users/jad/system
  871215 13:58:53 jad add converses for DateRangeEvaluate

 ************************************************************************/
/************************************************************************
  selector.d 23 replace /users/jad/system
  880711 17:04:31 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  selector.d 24 replace /users/jck/system
  890117 13:35:30 jck Implemented Indexed List entire column deletion

 ************************************************************************/
