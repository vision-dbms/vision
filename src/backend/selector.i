/*---------------------------------------------------------------------------
 * This file contains the definitions for the known selectors.  A known
 * selector is defined by adding an entry of the form:
 *
 *	KnownSelector ("selectorName", selectorIndex, selectorValence)
 *
 * between the 'Begin...' and 'EndKnownSelectorDescriptions' statements.
 * The initialization routines will complain if duplicate selector names
 * or indices appear in this list.  The selector index must be defined in
 * "selector.d".
 *---------------------------------------------------------------------------
 */

BeginKnownSelectorDescriptions

/*****  Binary Selectors...  *****/
    KS ("<-"		, KS__LAssign			, 1)
    KS ("->"		, KS__RAssign			, 1)
    KS (","		, KS__Comma			, 1)
    KS ("<"		, KS__LessThan			, 1)
    KS ("<="		, KS__LessThanOrEqual		, 1)
    KS (">="		, KS__GreaterThanOrEqual	, 1)
    KS (">"		, KS__GreaterThan		, 1)
    KS ("="		, KS__Equal			, 1)
    KS ("=="		, KS__Equivalent		, 1)
    KS ("!="		, KS__NotEqual			, 1)
    KS ("!=="		, KS__NotEquivalent		, 1)
    KS ("||"		, KS__Or			, 1)
    KS ("&&"		, KS__And			, 1)
    KS ("+"		, KS__Plus			, 1)
    KS ("-"		, KS__Minus			, 1)
    KS ("*"		, KS__Times			, 1)
    KS ("/"		, KS__Divide			, 1)

/*****  Object Support Selectors...  *****/
    KS ("print"			, KS__Print		, 0)
    KS ("value"			, KS__Value		, 0)
    KS ("environment"		, KS__Environment	, 0)
    KS ("send:"			, KS__Send		, 1)
    KS ("extend:"		, KS__Extend		, 1)
    KS ("asMethod"		, KS__AsMethod		, 0)
    KS ("asClosure:"		, KS__AsClosure		, 1)
    KS ("add:toDictionaryOf:"	, KS__AddToDictionaryOf	, 2)
    KS ("basicSend:"		, KS__BasicSend		, 1)
    KS ("basicExtend:"		, KS__BasicExtend	, 1)
    KS ("basicSend:with:"	, KS__BasicSendWith	, 2)
    KS ("basicSendCanonicalizedForSort:",
	KS__BasicSendCanonicalized4Sort,
	1)
    KS ("super"			, KS__Super		, 0)

/*****  Binary/Keyword Inverse Selectors...  *****/
    KS ("breakString:"		, KS__BreakString	, 1)
    KS ("cbreakString:"		, KS__CBreakString	, 1)
    KS ("evaluate:from:to:"	, KS__EvaluateFromTo	, 3)
    KS ("prefixSpannedIn:"	, KS__PrefixSpannedIn	, 1)
    KS ("prefixNotSpannedIn:"	, KS__PrefixNotSpannedIn, 1)
    KS ("matchesString:"	, KS__MatchesString	, 1)
    KS ("subtractedFrom:"	, KS__SubtractedFrom	, 1)
    KS ("dividedInto:"		, KS__DividedInto	, 1)
    KS ("referencedBy:"		, KS__ReferencedBy	, 1)
    KS ("pointedToBy:"		, KS__PointedToBy	, 1)
    KS ("closureOf:"		, KS__ClosureOf	, 1)
    KS ("print:"		, KS__PrintColon	, 1)
    KS ("toThe:"		, KS__Pow		, 1)
    KS ("asAPowerOf:"		, KS__Wop		, 1)
    KS ("printWithCommasOf:",
	KS__PrintWithCommasColon,
	1)
    KS ("locateInAList:",
	KS__LocateInAList,
	1)
    KS ("defineInAList:",
	KS__DefineInAList,
	1)
    KS ("deleteFromAList:",
	KS__DeleteFromAList,
	1)
    KS ("deleteColumnFromStore:",
	KS__DeleteColumnFromStore,
	1)
    KS ("locateOrAddDictionaryEntryFor:",
	KS__LocateOrAddDictionaryEntry4,
	1)
    KS ("locateDictionaryEntryFor:",
	KS__LocateDictionaryEntryFor,
	1)
    KS ("DBUpdateReplaceDump:",
	KS__DBUpdateReplaceFile,
	1)
    KS ("DBUpdateAppendDump:",
	KS__DBUpdateAppendFile,
	1)
    KS ("deleteDictionaryEntryFor:",
	KS__DeleteDictionaryEntryFor,
	1)
    KS ("sprintOf:"		, KS__SPrintColon	, 1)
    KS ("sprintWithCommasOf:",
	KS__SPrintWithCommasColon,
	1)
    KS ("fillFromString:"	, KS__FillFromString	, 1)
    KS ("takeFromString:"	, KS__TakeFromString	, 1)
    KS ("dropFromString:"	, KS__DropFromString	, 1)
    KS ("prefixWith:"		, KS__ReverseConcat	, 1)
    KS ("adjacentInstancesFor:"	, KS__StoreSelectRows	, 1)
    KS ("base0CellPositionOf:"	, KS__Base0CellPositionOf , 1)
    KS ("base1CellPositionOf:"	, KS__Base1CellPositionOf , 1)
    KS ("__appendTo:"		, KS__AppendTo		, 1)
    KS ("__elementOf:"		, KS__ElementOf		, 1)

/*****  Dialogue / VISION X-Widget Support Selectors...  *****/
    KS ("pack8ByteWithMSW:"	, KS__Pack8ByteWithMSW	, 1)

    KS ("dlgSetStringIn:"	, KS__DLGSetStringIn	, 1)
    KS ("dlgSetBooleanIn:"	, KS__DLGSetBooleanIn	, 1)
    KS ("dlgSetDoubleIn:"	, KS__DLGSetDoubleIn	, 1)
    KS ("dlgSetLongIn:"		, KS__DLGSetLongIn	, 1)
    KS ("dlgSetObjectIn:"	, KS__DLGSetObjectIn	, 1)

    KS ("dlgAppendStringIn:"	, KS__DLGAppendStringIn	, 1)
    KS ("dlgAppendBooleanIn:"	, KS__DLGAppendBooleanIn, 1)
    KS ("dlgAppendDoubleIn:"	, KS__DLGAppendDoubleIn	, 1)
    KS ("dlgAppendLongIn:"	, KS__DLGAppendLongIn	, 1)
    KS ("dlgAppendObjectIn:"	, KS__DLGAppendObjectIn	, 1)

    KS ("vxwSetSlotIn:"		, KS__VXWSetSlotIn	, 1)
    KS ("vxwSendTo:"		, KS__VXWSendTo		, 1)

/*****  Multi/Keyword Converse Selectors...  *****/
    KS ("by:to:evaluate:"	, KS__ByToEvaluate	, 3)
    KS ("by:from:evaluate:"	, KS__ByFromEvaluate	, 3)
    KS ("withCompilationErrorHandler:evaluate:",
	KS__WithCompilationErrorHandlerEvaluate,
	2)

EndKnownSelectorDescriptions


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  selector.i 1 replace /users/mjc/system
  860404 09:14:06 mjc Selector services (including known selectors)

 ************************************************************************/
/************************************************************************
  selector.i 2 replace /users/mjc/system
  861002 18:03:37 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  selector.i 3 replace /users/jad/system
  861012 09:42:34 mjc Preliminary release of string and print primitives

 ************************************************************************/
/************************************************************************
  selector.i 4 replace /users/mjc/system
  861015 23:53:34 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  selector.i 5 replace /users/mjc/system
  861109 00:40:58 mjc Define 'evaluate:from:to:' selector for use by date range iterator

 ************************************************************************/
/************************************************************************
  selector.i 6 replace /users/mjc/system
  861207 17:55:51 mjc Fix non-declaration of known selectors

 ************************************************************************/
/************************************************************************
  selector.i 7 replace /users/mjc/system
  861226 12:25:44 mjc Implemented 'super'

 ************************************************************************/
/************************************************************************
  selector.i 8 replace /users/mjc/system
  870104 22:51:30 lcn Added dictionary search known selector converses

 ************************************************************************/
/************************************************************************
  selector.i 9 replace /users/jad/system
  870120 15:00:40 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  selector.i 10 replace /users/jad/system
  870128 19:05:57 jad added DeleteDictionaryEntry... defines

 ************************************************************************/
/************************************************************************
  selector.i 11 replace /users/jad/system
  870204 15:27:52 jad added sprint selectors

 ************************************************************************/
/************************************************************************
  selector.i 12 replace /users/jad/system
  870210 14:21:22 jad add string fill, take, & drop selectors

 ************************************************************************/
/************************************************************************
  selector.i 13 replace /users/mjc/system
  870211 13:19:49 mjc Inserted missing colons, deleted 'delete:fromDictionaryOf:'

 ************************************************************************/
/************************************************************************
  selector.i 14 replace /users/jad/system
  870211 16:31:11 jad added string concat selector

 ************************************************************************/
/************************************************************************
  selector.i 15 replace /users/jad/system
  870220 15:50:53 jad implemented SelectedRowsInsStore primitive

 ************************************************************************/
/************************************************************************
  selector.i 16 replace /users/mjc/translation
  870524 09:41:46 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  selector.i 17 replace /users/jck/system
  870605 16:04:02 jck Added binary converse selectors for indexed list primitives

 ************************************************************************/
/************************************************************************
  selector.i 18 replace /users/jck/system
  870810 14:16:13 jck Added binary converse selectors for associative list deletion

 ************************************************************************/
/************************************************************************
  selector.i 19 replace /users/jad/system
  871215 13:58:58 jad add converses for DateRangeEvaluate

 ************************************************************************/
/************************************************************************
  selector.i 20 replace /users/jad/system
  880711 17:05:44 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  selector.i 21 replace /users/jck/system
  890117 13:37:40 jck Implemented Indexed List entire column deletion

 ************************************************************************/
