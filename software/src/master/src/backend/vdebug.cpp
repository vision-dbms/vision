/*****  Internal Debugger Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName VDEBUG

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "viobj.h"
#include "vprimfns.h"
#include "vstdio.h"
#include "vunwind.h"

#include "RTstring.h"

#include "VkRunTimes.h"

#include "V_VArgList.h"

/*****  Self  *****/
#include "vdebug.h"


/**********************
 *****  Switches  *****
 **********************/

PublicVarDef bool DEBUG_TestdeckTracing = false;


/********************************************
 *****  External Facility Declarations  *****
 ********************************************/

/*---------------------------------------------------------------------------
 * The following list identifies the facilities known to the debugger.
 * The facility name is wrapped in a macro so that it can be expanded
 * into the different syntactic forms necessary for declaration and
 * incorporation into the debugger's internal literal table.  Note that
 * no commas are used to separate the entries in the list.
 *---------------------------------------------------------------------------
 */
#define DebuggerFacilityList\
    FacilityId (DBUPDATE)\
    FacilityId (VDEBUG)\
    FacilityId (ENVIR)\
    FacilityId (FAULT)\
    FacilityId (LIO)\
    FacilityId (M)\
    FacilityId (PS)\
    FacilityId (RSLANG)\
    FacilityId (RTYPE)\
    FacilityId (VMACHINE)

/*****  'FAC_DeclareFacility' Facility Declarations  *****/
#define FacilityId(facility) FAC_DeclareFacility (facility);

DebuggerFacilityList

#undef FacilityId
#define FacilityId(facility) facility,


/****************************************************
 *****  Literal Table Declarations and Manager  *****
 ****************************************************/

/******************
 *  Declarations  *
 ******************/

#define MaxDebuggerLiterals	1024

struct LTEntry {
    char const *	name;
    IOBJ_IObject	value;
};

/*****  Literal Table  *****/
PrivateVarDef LTEntry LT[MaxDebuggerLiterals];

/*****  Literals In Use Count  *****/
PrivateVarDef unsigned int LiteralsInUse = 0;


/************
 *  Search  *
 ************/

/*---------------------------------------------------------------------------
 *****  Internal routine to compare literal table entries.
 *
 *  Arguments:
 *	l1			- the address of literal table entry 1.
 *	l2			- the address of literal table entry 2.
 *
 *  Returns:
 *	strcmp (l1->name, l2->name)
 *****/
PrivateFnDef int __cdecl CompareLiterals (LTEntry const *l1, LTEntry  const *l2) {
    return strcmp (l1->name, l2->name);
}


/******************************
 *  Accessors and Generators  *
 ******************************/

#ifdef _M_X64
typedef unsigned long long IDValue;
typedef char const *(*Ref_IDNameAccessor) (IDValue id);
typedef IOBJ_IObject (*Ref_IDObjectAccessor) (IDValue id);
typedef IDValue (*Ref_IDListGenerator) (void *pGenerator);
#else
typedef unsigned long IDValue;
typedef char const *(*Ref_IDNameAccessor) (IDValue id);
typedef IOBJ_IObject (*Ref_IDObjectAccessor) (IDValue id);
typedef IDValue (*Ref_IDListGenerator) (void *pGenerator);
#endif



/******************
 *  Construction  *
 ******************/

/*---------------------------------------------------------------------------
 *****  Internal routine to add literals to the debugger literal table
 *****  using a generator function.
 *
 *  Arguments:
 *	idAsStringFn		- the address of a function which returns
 *				  the address of a string representation of
 *				  the id.
 *	idAsObjectFn		- the address of a function which returns
 *				  an I-Object representing the id.
 *	idListTerminator	- the value which will terminate the id list.
 *	idListParameter		- an address which will be passed to the
 *				  'idListGenerator'.
 *	idListGenerator		- the address of a function which returns
 *				  the id of the next literal to be added.
 *				  This routine will be passed one argument -
 *				  'idListParameter'; it should return the
 *				  index of the literal or the value
 *				  'idListTerminator' to indicate it is done.
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	As currently implemented, this routine will silently stop adding
 *	literals as soon as its literal table is full - it should signal
 *	an error.
 *
 *****/
PrivateFnDef void AddToLTUsingGenerator (
    Ref_IDNameAccessor		idAsStringFn,
    Ref_IDObjectAccessor	idAsObjectFn,
    IDValue			idListTerminator,
    Ref_IDListGenerator		idListGenerator,
    void *			idListParameter
)
{
/*****  Add new entries to the literal table...  *****/
    IDValue id = (*idListGenerator)(idListParameter);
    while (id != idListTerminator) {
	char const *name = (*idAsStringFn)(id);
	if (name) {
	    if (LiteralsInUse >= MaxDebuggerLiterals) ERR_SignalFault (
		EC__UsageError, "Literal Table Full"
	    );
	    
	    LT[LiteralsInUse  ].name  = name;
	    LT[LiteralsInUse++].value = (*idAsObjectFn)(id);
	}
	id = (*idListGenerator)(idListParameter);
    }

/*****  ... and sort it when all is done.  *****/
    qsort ((char *)LT, (unsigned)LiteralsInUse, sizeof (LTEntry), (VkComparator)CompareLiterals);
}


/*************************
 *  Enumerated Addition  *
 *************************/

/*---------------------------------------------------------------------------
 *****  Internal generator routine for an enumerated literal list.
 *
 *  Argument:
 *	pArgList		- the address of the argument pointer
 *				  being used to step through the parameter
 *				  list of 'AddToLiteralTable'.
 *
 *  Returns:
 *	pArgList->arg<unsigned long>
 *
 *****/
PrivateFnDef IDValue AddEnumeratedLiteralsGenerator (V::VArgList *pArgList) {
    return pArgList->arg<IDValue>();
}

/*---------------------------------------------------------------------------
 *****  Internal routine to add an enumerated set of literals to the debugger
 *****  literal table.
 *
 *  Arguments:
 *	idAsStringFn		- the address of a function which returns
 *				  the address of a string representation of
 *				  the id.
 *	idAsObjectFn		- the address of a function which returns
 *				  an I-Object representing the id.
 *	idListTerminator	- the value which will terminate the id list.
 *	id1, id2, ...		- the identifiers to be added to the debugger
 *				  literal table.
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	As currently implemented, this routine will silently stop adding
 *	literals as soon as its literal table is full - it should signal
 *	an error.
 *
 *****/
PrivateFnDef void __cdecl AddEnumeratedLiteralsToLT (
    Ref_IDNameAccessor		idAsStringFn,
    Ref_IDObjectAccessor	idAsObjectFn,
    IDValue			idListTerminator,
    ...
) {
    V_VARGLIST (ap, idListTerminator);

    AddToLTUsingGenerator (
	idAsStringFn,
	idAsObjectFn,
	idListTerminator,
	(Ref_IDListGenerator)AddEnumeratedLiteralsGenerator,
	&ap
    );
}


/**********************
 *  Counted Addition  *
 **********************/

/*---------------------------------------------------------------------------
 *****  Internal generator routine for a counted literal list.
 *
 *  Argument:
 *	ip			- the address of the integer being used as
 *				  the counter.
 *
 *  Returns:
 *	(*ip)++
 *
 *****/
PrivateFnDef unsigned int AddCountedLiteralsGenerator (unsigned int *ip) {
    return (*ip)++;
}

/*---------------------------------------------------------------------------
 *****  Internal routine to add a counted set of literals to the debugger
 *****  literal table.
 *
 *  Arguments:
 *	idAsStringFn		- the address of a function which returns
 *				  the address of a string representation of
 *				  the id.
 *	idAsObjectFn		- the address of a function which returns
 *				  an I-Object representing the id.
 *	startingValue		- the starting value of the count.
 *	endingValue		- one more than the final value of the count.
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	As currently implemented, this routine will silently stop adding
 *	literals as soon as its literal table is full - it should signal
 *	an error.
 *
 *****/
PrivateFnDef void AddCountedLiteralsToLT (
    Ref_IDNameAccessor		idAsStringFn,
    Ref_IDObjectAccessor	idAsObjectFn,
    unsigned int		startingValue,
    unsigned int		endingValue
) {
    unsigned int count = startingValue;

    AddToLTUsingGenerator (
	idAsStringFn,
	idAsObjectFn,
	endingValue,
	(Ref_IDListGenerator)AddCountedLiteralsGenerator,
	&count
    );
}


/*---------------------------------------------------------------------------
 *****  Internal routine to initialize the debugger literal table.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void InitializeLT () {
    AddCountedLiteralsToLT (
	(Ref_IDNameAccessor)RTYPE_TypeIdAsString,
	(Ref_IDObjectAccessor)RTYPE_TypeIObject,
	0,
	RTYPE_C_MaxType
    );
    AddCountedLiteralsToLT (
	(Ref_IDNameAccessor)PRIMFNS_BasicPrimitiveName,
	(Ref_IDObjectAccessor)PRIMFNS_PackPrimitiveIObject,
	0,
	V_PF_MaxPrimitiveIndex
    );
    AddCountedLiteralsToLT (
	(Ref_IDNameAccessor)RTYPE_QRegisterName,
	(Ref_IDObjectAccessor)RTYPE_QRegisterIObject,
	0,
	RTYPE_TotalQRegisterCount
    );
    AddEnumeratedLiteralsToLT (
	(Ref_IDNameAccessor)FAC_FacilityIdAsString,
	(Ref_IDObjectAccessor)FAC_FacilityIObject,
	0,
	DebuggerFacilityList	    /*  Note: No Comma Here !  */
	NilOf (FAC_Facility)
    );
}


/*******************************************************
 *****  Identifier Table Declarations and Manager  *****
 *******************************************************/

/******************
 *  Declarations  *
 ******************/

#define MaxDebuggerIds	256

struct IdTableEntry {
    char		name [48];
    IOBJ_IObject	value;
};

/*****  Id Table  *****/
PrivateVarDef IdTableEntry IdTable[MaxDebuggerIds];

/*****  Ids In Use Count  *****/
PrivateVarDef int IdsInUse = 0;


/************
 *  Search  *
 ************/

/*---------------------------------------------------------------------------
 *****  Internal routine to compare literal table entries.
 *
 *  Arguments:
 *	l1			- the address of literal table entry 1.
 *	l2			- the address of literal table entry 2.
 *
 *  Returns:
 *	strcmp (l1->name, l2->name)
 *****/
PrivateFnDef int __cdecl CompareIds (IdTableEntry const *l1, IdTableEntry const *l2) {
    return strcmp (l1->name, l2->name);
}


/******************
 *  Construction  *
 ******************/

/*---------------------------------------------------------------------------
 *****  Internal routine to add literals to the debugger literal table.
 *
 *
 *  Arguments:
 *	id			- the name of the id to be added to the
 *				  table.
 *	value			- the value of the id to be added to the table.
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	As currently implemented, this routine will silently stop adding
 *	literals as soon as its literal table is full - it should signal
 *	an error.
 *
 *****/
PrivateFnDef void AddToIdTable (
    char			*id,
    IOBJ_IObject		value
)
{

/*****  Add new entry to the id table...  *****/
    if (IdsInUse < MaxDebuggerIds)
    {
	strcpy (IdTable[IdsInUse].name, id);
	IdTable[IdsInUse++].value = value;
    }


/*****  ... and sort it when all is done.  *****/
    qsort ((char *)IdTable, (unsigned)IdsInUse, sizeof (IdTableEntry), (VkComparator)CompareIds);
}


/*******************************************************
 *****  I-Object Stack Declarations and Utilities  *****
 *******************************************************/

#define MaxStackDepth 256

PrivateVarDef IOBJ_IObject Stack[MaxStackDepth];

PrivateVarDef int StackPointer = 0;

PrivateFnDef void Push (
    IOBJ_IObject		iobject
)
{
    if (StackPointer >= MaxStackDepth)
        IO_printf (">>>  Stack Full  <<<\n");
    else
	Stack[StackPointer++] = iobject;
}

PrivateFnDef IOBJ_IObject Pop (
    void
)
{
    if (StackPointer <= 0)
    {
        IO_printf (">>>  Stack Empty  <<<\n");
	return IOBJ_TheNilIObject;
    }
    else
	return Stack[--StackPointer];
}

PrivateFnDef IOBJ_IObject *PopN (
    int				n
)
{
    int i;

    if (StackPointer < n)
    {
        IO_printf
	    (">>>  Stack Too Short (%d Requested, %d Present)  <<<\n",
	     n, StackPointer);
	for (i = StackPointer; i < n; i++)
	    Stack[i] = IOBJ_TheNilIObject;
	StackPointer = n;
    }
    return Stack + (StackPointer -= n);
}


/*****************************************************
 *****  String Stack Declarations and Utilities  *****
 *****************************************************/

#define StringStackDepth 32
#define StringFrameSize 64

PrivateVarDef char StringStack[StringStackDepth][StringFrameSize];

PrivateVarDef int StringStackPointer = 0;

PrivateFnDef void PushString (
    char			*string
)
{
    if (StringStackPointer >= StringStackDepth)
        IO_printf (">>> String Stack Full  <<<\n");
    else
	strcpy (StringStack[StringStackPointer++], string);
	
}

PrivateFnDef char const *PopString () {
    if (StringStackPointer <= 0) {
        IO_printf (">>> String Stack Empty  <<<\n");
	return "";
    }

    return StringStack[--StringStackPointer];
}


/********************************
 *****  Selector Utilities  *****
 ********************************/

/*---------------------------------------------------------------------------
 *****  Internal temporary routine which determines the valence of a selector.
 *
 *  Argument:
 *	selector		- the address of a character string containing
 *				  the selector.
 *
 *  Returns:
 *	The valence of the selector (>=1) or zero if the selector appears to
 *	be invalid.
 *
 *****/
PrivateFnDef int SelectorValence (
    char			*selector
)
{
    char *p;
    int valence, length;

    for (valence = length = 0, p = selector; *p; p++, length++)
	valence += ':' == *p;

    if (0 == valence || valence * 2 > length)
	return 0;
    if (1 == valence)
        return
	    ':' == *selector
	    ? 1
	    : ':' == selector[length - 1]
	    ? 2
	    : 0;
    if (':' == *selector || ':' != selector[length - 1])
        return 0;

    return valence;
}


/******************************************************
 *****  Error Handler Declarations and Utilities  *****
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal debugger error handler routine
 *
 *  Argument:
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	NOTHING.
 *
 *  Behavior:
 *	After displaying its error message, this routine will return to the
 *	top level loop of the debugger.
 *
 *****/
PrivateFnDef int DebuggerDefaultErrorHandler (ERR_ErrorDescription *errorDescription) {
    ERR_ReportErrorOnStderr ("Trapped By Debugger", errorDescription);
    UNWIND_ThrowException ();

    return 0;
}


/*********************
 *****  Parsing  *****
 *********************/

PrivateFnDef char NextCharacter () {
    static char InputBuffer[1024] = {'\0'};
    static char *InputCursor = InputBuffer;

    if (*InputCursor)
	return *InputCursor++;

    InputCursor = InputBuffer;

    if (IO_pfgets ("D> ", InputBuffer, sizeof (InputBuffer)) && *InputCursor)
	return *InputCursor++;

    InputBuffer[0] = '\0';

    return '\0';
}

#if defined(_AIX) && !defined(_CPP_IOSTREAMS)
extern "C"

void debugerror (char *s) {
    IO_printf ("%s\n", s);
}

#else

void debugerror (char const *s) {
    IO_printf ("%s\n", s);
}

void debugerror (char *s) {
    debugerror ((char const*)s);
}

#endif

extern "C" {
    int debugwrap () {
	return 1;
    }

    extern int debuglex ();
}

#include "vdebug.yo"
#include "vdebug.lo"


/************************
 *****  Debug Eval  *****
 ************************/

/*---------------------------------------------------------------------------
 *****  Debug Eval
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *  Note:
 *	The purpose of this routine is to execute in context of the error frame
 *	set up by 'DEBUG_Main'.  It establishes a default error handler for the
 *	the debugger and invokes the debugger's read-eval-print loop.
 *
 *****/
PrivateFnDef void DEBUG_Eval (va_list pArgList) {
    V::VArgList iArgList (pArgList);
    IOBJ_ScratchPad = iArgList.arg<M_ASD*>();

    ERR_SetErrorHandler (EC__AnError, DebuggerDefaultErrorHandler);
    UNWIND_TryAndCatch {
	debugparse ();
    } UNWIND_EndTryAndCatch;
}


/************************
 *****  Debug Main  *****
 ************************/

/*---------------------------------------------------------------------------
 *****  Debug Main
 *
 *  Arguments:
 *	input			- the input string to process.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void DEBUG_Main (M_ASD *pScratchPad) {
    ERR_EstablishErrorFrame (DEBUG_Eval, pScratchPad);
}


/***************************************
 *****  Facility I-Object Methods  *****
 ***************************************/

IOBJ_DefineUnaryMethod (DumpLT) {
/*****  Determine maximum symbol length  *****/
    size_t maxSymbolLength = 0;
    unsigned int i;
    for (i = 0; i < LiteralsInUse; i++) {
	size_t symbolLength = strlen (LT[i].name);
	if (maxSymbolLength < symbolLength)
	    maxSymbolLength = symbolLength;
    }

/*****  Display Symbols  *****/
    unsigned int symbolsPerLine = maxSymbolLength > 79 ? 1 : 79 / (maxSymbolLength + 1);

    IO_printf ("Literal Table:");
    for (i = 0; i < LiteralsInUse; i++) IO_printf (
	"%c%-*s",
	i % symbolsPerLine ? ' ' : '\n',
	maxSymbolLength,
	LT[i].name
    );

    IO_printf ("\n");

    return self;
}

IOBJ_DefineUnaryMethod (DumpIdTable) {
/*****  Determine maximum symbol length  *****/
    size_t maxSymbolLength = 0;
    int i;
    for (i = 0; i < IdsInUse; i++) {
	size_t symbolLength = strlen (IdTable[i].name);
	if (maxSymbolLength < symbolLength)
	    maxSymbolLength = symbolLength;
    }

/*****  Display Symbols  *****/
    unsigned int symbolsPerLine = maxSymbolLength > 79 ? 1 : 79 / (maxSymbolLength + 1);

    IO_printf ("Id Table:");
    for (i = 0; i < IdsInUse; i++) IO_printf (
	"%c%-*s",
	i % symbolsPerLine ? ' ' : '\n',
	maxSymbolLength,
	IdTable[i].name
    );

    IO_printf ("\n");

    return self;
}


/*****
 *  Timer Methods
 *****/

/*****  Execution Time Recording Variables  *****/
PrivateVarDef VkRunTimes	StartTime (0),
				StopTime  (0);
PrivateVarDef bool		StartTimeSet = false, 
				StopTimeSet  = false;

IOBJ_DefineUnaryMethod (StartTimer) {
    StartTime.refresh ();
    StartTimeSet = true;
    StopTimeSet = false;

    return self;
}

IOBJ_DefineUnaryMethod (StopTimer) {
    StopTime.refresh ();
    StopTimeSet = true;

    return self;
}

IOBJ_DefineUnaryMethod (DisplayTimer) {
    if (StartTimeSet && StopTimeSet) {
	IO_printf ("Execute Times:\n");
        UTIL_PrintTimeDelta
            (StartTime, StopTime, elapsed);
        UTIL_PrintTimeDelta
            (StartTime, StopTime, user   );
        UTIL_PrintTimeDelta
            (StartTime, StopTime, system );
    }
    else
	IO_printf ("The timers are not set\n");

    return self;
}

IOBJ_DefineNilaryMethod (TestdeckTrace) {
    return IOBJ_SwitchIObject (&DEBUG_TestdeckTracing);
}

IOBJ_DefineUnaryMethod (SegFault) {
    pointer_size_t p=0;
    int *pp = (int *)p;
    int xx = *pp;

    IO_printf ("%x:%d\n", pp, *pp, xx);
    return self;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("dumpLiteralTable"	, DumpLT)
	IOBJ_MDE ("dumpIdTable"		, DumpIdTable)
	IOBJ_MDE ("startTimer"		, StartTimer)
	IOBJ_MDE ("stopTimer"		, StopTimer)
	IOBJ_MDE ("displayTimer"	, DisplayTimer)
	IOBJ_MDE ("testdeckTrace"       , TestdeckTrace)
	IOBJ_MDE ("segFault"		, SegFault)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (VDEBUG);
    FAC_FDFCase_FacilityDescription ("Internal Debugger");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    case FAC_ReturnFacilityMD:
        FAC_MDResultField = methodDictionary;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    case FAC_DoStartupInitialization:
        InitializeLT ();
	debugout    = stderr;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  debug.c 1 replace /users/mjc/system
  860226 16:17:49 mjc create

 ************************************************************************/
/************************************************************************
  debug.c 2 replace /users/jck/system
  860318 12:55:33 jck The new command language is now implemented

 ************************************************************************/
/************************************************************************
  debug.c 3 replace /users/mjc/system
  860321 18:06:02 mjc Upgraded to new memory manager/R-Type literals

 ************************************************************************/
/************************************************************************
  debug.c 4 replace /users/mjc/system
  860322 10:35:58 mjc Changed :quickPrint to :qprint

 ************************************************************************/
/************************************************************************
  debug.c 5 replace /users/mjc/system
  860323 17:17:34 mjc Changed Q-Register enumeration to list all registers

 ************************************************************************/
/************************************************************************
  debug.c 6 replace /users/mjc/system
  860403 13:31:34 mjc Eliminated : from unary messages

 ************************************************************************/
/************************************************************************
  debug.c 7 replace /users/mjc/system
  860412 12:32:55 mjc Added string syntax and objects

 ************************************************************************/
/************************************************************************
  debug.c 8 replace /users/jck/source
  860418 00:02:00 jck Expression separator replaced with comma
Continue prompt removed, YACC bug fixed (ExpressionList type),
hooks added for editor

 ************************************************************************/
/************************************************************************
  debug.c 9 replace /users/jad/system
  860425 12:08:02 jad now uses prompting input in its read-eval-print lop

 ************************************************************************/
/************************************************************************
  debug.c 10 replace /users/hjb/system
  860428 19:09:36 hjb added facilities for LTYPE

 ************************************************************************/
/************************************************************************
  debug.c 11 replace /users/mjc/system
  860615 18:53:52 mjc Delete 'ltype' declarations

 ************************************************************************/
/************************************************************************
  debug.c 12 replace /users/mjc/system
  860620 17:45:30 mjc Added literals for the primitive functions

 ************************************************************************/
/************************************************************************
  debug.c 13 replace /users/mjc/system
  860711 10:44:26 mjc Changed limit on selector size from 32 to 64 characters

 ************************************************************************/
/************************************************************************
  debug.c 14 replace /users/mjc/system
  860811 13:49:52 mjc Completed initial implementation of date offsets

 ************************************************************************/
/************************************************************************
  debug.c 15 replace /users/mjc/system
  861002 18:02:46 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  debug.c 16 replace /users/jad/system
  861116 15:57:51 jad moved the primitive array to vmachine - so needed to change the name

 ************************************************************************/
/************************************************************************
  debug.c 17 replace /users/mjc/translation
  870524 09:37:12 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  debug.c 18 replace /users/mjc/system
  870527 12:51:14 mjc Moved initialization of lex 'output' file to module initialization (VAX portability)

 ************************************************************************/
/************************************************************************
  debug.c 19 replace /users/jad/system
  871020 12:28:42 jad added timers to the debugger in #DEBUG

 ************************************************************************/
/************************************************************************
  debug.c 20 replace /users/jad/system
  880321 16:25:14 jad implement testdeck tracing

 ************************************************************************/
/************************************************************************
  debug.c 21 replace /users/jad/system
  880711 16:00:30 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  debug.c 22 replace /users/jck/system
  890808 16:04:00 jck Fixed bug in fault recording

 ************************************************************************/
/************************************************************************
  debug.c 23 replace /users/jck/system
  890809 12:39:38 jck adjusted fault recording management

 ************************************************************************/
