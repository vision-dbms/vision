/*****  Error Handling Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName ERR

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "verrdef.h"
#include "vfac.h"
#include "vfault.h"
#include "viobj.h"
#include "vstdio.h"
#include "vunwind.h"
#include "vutil.h"

/*****  Shared  *****/
#include "verr.h"
#include "V_VString.h"


/*************************************************
 *****  Global Definitions and Declarations  *****
 *************************************************/

/********************
 *  Error Handlers  *
 ********************/

struct ErrorHandlerDef {
    struct ErrorHandlerDef	*previousHandlerDef;
    ERR_ErrorHandler		handlerFunction;
};

/*****  Error Handler Definition Allocation Page  *****/
#define ErrorHandlerDefPageSize	    100
typedef ErrorHandlerDef ErrorHandlerDefPage[ErrorHandlerDefPageSize];

/*****  'Nil' Pointers  *****/
#define NilHandler  NilOf (ErrorHandlerDef *)

/*****  'ErrorHandlerDef' Allocator Free List  *****/
PrivateVarDef ErrorHandlerDef	*HandlerDefFreeList = NilHandler;

/******************
 *  Error Frames  *
 ******************/

#define HandlerSlot(errorCode) (int)(errorCode)
#define ErrorFrameHandlerSlots HandlerSlot (EC__MaxError)

struct ErrorFrame {
    struct ErrorFrame		*previousErrorFrame;
    ErrorHandlerDef		*handlers[ErrorFrameHandlerSlots];
};

/*****  'Nil' Pointers  *****/
#define NilFrame    NilOf (ErrorFrame *)

/*****  Global Error Frame and Error Frame Stack  *****/
PrivateVarDef ErrorFrame	GlobalErrorFrame,
				*ErrorFrameStack = NilFrame;


/***************************************************************
 *****  'ErrorHandlerDef' Allocator/Deallocator Utilities  *****
 ***************************************************************/

PrivateFnDef bool InvalidErrorCode (
    ERRDEF_ErrorCode		errorCode
)
{
    if (HandlerSlot (errorCode) >= 0 &&
	HandlerSlot (errorCode) < ErrorFrameHandlerSlots
    ) return false;

    ERR_SignalException (
	EC__InvalidErrorCode,
	"An invalid error code was specified in a handler definition"
    );
    return true;
}

PrivateFnDef ErrorHandlerDef *AllocateHandlerDef (
    void
)
{
    ErrorHandlerDef *ehp, *ehpage;
    int i;

    if (IsntNil (HandlerDefFreeList))
    {
        ehp = HandlerDefFreeList;
	HandlerDefFreeList = HandlerDefFreeList->previousHandlerDef;
	return ehp;
    }

    ehpage = (ErrorHandlerDef *)UTIL_Malloc (sizeof (ErrorHandlerDefPage));

    for (i = 0; i < ErrorHandlerDefPageSize; i++)
    {
        ehpage[i].previousHandlerDef = HandlerDefFreeList;
	HandlerDefFreeList = ehpage + i;
    }

    ehp = HandlerDefFreeList;
    HandlerDefFreeList = HandlerDefFreeList->previousHandlerDef;

    return ehp;
}

PrivateFnDef void DeallocateHandlerDefChain (
    ErrorHandlerDef		**chainPtr
)
{
    ErrorHandlerDef *chainTail, *nextDef;

    if (IsNil (*chainPtr))
        return;

    for (
	chainTail = *chainPtr;
        nextDef = chainTail->previousHandlerDef;
	chainTail = nextDef
    );

    chainTail->previousHandlerDef = HandlerDefFreeList;
    HandlerDefFreeList = *chainPtr;
    *chainPtr = NilHandler;
}

PrivateFnDef void DeallocateFrameHandlerDefs (
    ErrorFrame			*frame
)
{
    int i;

    for (i = 0; i < ErrorFrameHandlerSlots; i++)
	DeallocateHandlerDefChain (frame->handlers + i);
}


/********************************************************
 *****  Default Error Handler and Display Routines  *****
 ********************************************************/

/*********************
 *  Display Helpers  *
 *********************/

PrivateFnDef void __cdecl StderrWriter (char const* format, ...) {
    V_VARGLIST (ap, format);
    IO_vfprintf (stderr, format, ap);
}

PrivateFnDef void __cdecl StdoutWriter (char const* format, ...) {
    V_VARGLIST (ap, format);
    IO_vfprintf (stdout, format, ap);
}


/**********************
 *  Display Routines  *
 **********************/

/*---------------------------------------------------------------------------
 *****  Default error display routines.
 *
 *  Argument:
 *	writer			- a 'printf' style routine used to format the
 *				  the message.
 *	banner			- the address of a string to be displayed on
 *				  the first line of the error display following
 *				  either the word 'Error ' or 'Warning '.
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	NOTHING
 *
 *  Behavior:
 *	This routine writes a description of the error to STDERR.
 *
 *****/
PublicFnDef void ERR_ReportError (
    void (__cdecl *writer)(char const*, ...), char const *banner, ERR_ErrorDescription *errorDescription, unsigned int counter
)
{
    // Don't report if we are over the limit. The act of reporting
    // could be causing a recursive error ...
    if (UNWIND_GetMaximumNumberOfSeriousErrors () < FAULT_SessionErrorCount) return;

    if (counter > 0) writer (
	">>>  %s %s[%u]  <<<\n*\t%s [%d]\n*\t%s\n*\t%s\n\n",
	ERR_SeverityCodeString (ERR_ED_Severity (errorDescription)),
	banner, counter,
	FAC_FacilityDescription (ERR_ED_Facility (errorDescription)),
	ERR_ED_LineNumber (errorDescription),
	ERRDEF_CodeDescription (ERR_ED_ErrorCode (errorDescription)),
	ERR_ED_Description (errorDescription)
    );
    else writer (
	">>>  %s %s  <<<\n*\t%s [%d]\n*\t%s\n*\t%s\n\n",
	ERR_SeverityCodeString (ERR_ED_Severity (errorDescription)),
	banner,
	FAC_FacilityDescription (ERR_ED_Facility (errorDescription)),
	ERR_ED_LineNumber (errorDescription),
	ERRDEF_CodeDescription (ERR_ED_ErrorCode (errorDescription)),
	ERR_ED_Description (errorDescription)
    );
}


/*---------------------------------------------------------------------------
 *****  Default error display routine.
 *
 *  Argument:
 *	banner			- the address of a string to be displayed on
 *				  the first line of the error display following
 *				  either the word 'Error ' or 'Warning '.
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	NOTHING
 *
 *  Behavior:
 *	This routine writes a description of the error to STDERR.
 *
 *****/
PublicFnDef void ERR_ReportErrorOnStderr (
    char const* banner, ERR_ErrorDescription* errorDescription
)
{
    ERR_ReportError (StderrWriter, banner, errorDescription);
}

/*---------------------------------------------------------------------------
 *****  Default error display routine.
 *
 *  Argument:
 *	banner			- the address of a string to be displayed on
 *				  the first line of the error display following
 *				  either the word 'Error ' or 'Warning '.
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	NOTHING
 *
 *  Behavior:
 *	This routine writes a description of the error to STDOUT.
 *
 *****/
PublicFnDef void ERR_ReportErrorOnStdout (
    char const* banner, ERR_ErrorDescription* errorDescription
)
{
    ERR_ReportError (StdoutWriter, banner, errorDescription);
}


/*---------------------------------------------------------------------------
 *****  Error dump routines.
 *
 *  Argument:
 *	strDump			- a string to which the formatted message is written.
 *	banner			- the address of a string to be displayed on
 *				  the first line of the error display following
 *				  either the word 'Error ' or 'Warning '.
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	NOTHING
 *
 *  Behavior:
 *	This routine writes a description of the error to a string.
 *
 *****/
PublicFnDef void ERR_DumpErrorToString (
    V::VString &strDump, char const *banner, ERR_ErrorDescription *errorDescription, unsigned int counter
)
{
    // Don't report if we are over the limit. 
    if (UNWIND_GetMaximumNumberOfSeriousErrors () < FAULT_SessionErrorCount) return;

    if (counter > 0) strDump.printf (
	">>>  %s %s[%u]  <<<\n*\t%s [%d]\n*\t%s\n*\t%s\n\n",
	ERR_SeverityCodeString (ERR_ED_Severity (errorDescription)),
	banner, counter,
	FAC_FacilityDescription (ERR_ED_Facility (errorDescription)),
	ERR_ED_LineNumber (errorDescription),
	ERRDEF_CodeDescription (ERR_ED_ErrorCode (errorDescription)),
	ERR_ED_Description (errorDescription)
    );
    else strDump.printf (
	">>>  %s %s  <<<\n*\t%s [%d]\n*\t%s\n*\t%s\n\n",
	ERR_SeverityCodeString (ERR_ED_Severity (errorDescription)),
	banner,
	FAC_FacilityDescription (ERR_ED_Facility (errorDescription)),
	ERR_ED_LineNumber (errorDescription),
	ERRDEF_CodeDescription (ERR_ED_ErrorCode (errorDescription)),
	ERR_ED_Description (errorDescription)
    );
}

/***************************
 *  Default Error Handler  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Default error handling routine.
 *
 *  Argument:
 *	errorDescription	- the address of an error description for the
 *				  error being signalled.
 *
 *  Returns:
 *	This routine (by virtue of its behavior) never returns.
 *
 *  Behavior:
 *	This routine logs and displays a description of the error it is
 *	handling and then performs an 'UNWIND_ThrowException'.
 *
 *****/
PrivateFnDef int DefaultErrorHandler (ERR_ErrorDescription *errorDescription) {
    ERR_ReportErrorOnStderr ("Default Error Handler Exit", errorDescription);
    UNWIND_ThrowException ();

    return 0;
}


/**************************************
 *****  Error Signalling Routine  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Basic error signalling routine.
 *
 * Arguments:
 *	errorCode		- the error code being signalled.
 *	facility		- the facility in which the signal is being
 *				  raised.
 *	lineNumber		- the line number of the call.
 *	description		- a textual description of the error
 *				  encountered.
 *	noReturn		- a boolean which when true will inhibit any
 *				  direct return from this function to its
 *				  caller.  If the error handler ignores this
 *				  indicator and returns, a 'CantReturn' error
 *				  will be signalled.
 *	severity		- the severity code of the error.
 *
 *  Returns:
 *	If this routine returns at all, it returns whatever value was returned
 *	by the error handler.
 *
 *  Notes:
 *	See header for a number of helper macros intended for use with this
 *	routine.
 *
 *****/
PublicFnDef int ERR_BasicSignalError (
    ERRDEF_ErrorCode		errorCode,
    FAC_Facility		facility,
    int				lineNumber,
    char const*			description,
    int				noReturn, 
    int				severity
)
{
    ERR_ErrorDescription errorDescription;
    ERRDEF_ErrorCode searchCode, nextSearchCode;
    ErrorFrame *efp;
    ErrorHandlerDef *ehp;
    int handlerResult;

/*****  Construct Error Description  *****/
    errorDescription.severity		= severity;
    errorDescription.errorCode		= errorCode;
    errorDescription.handlerErrorCode	= EC__AnError;
    errorDescription.facility		= facility;
    errorDescription.fileName		= "Unrecorded";
    errorDescription.lineNumber		= lineNumber;
    errorDescription.description	= description;
    errorDescription.noReturn		= noReturn;

/*****  Attempt to find a handler  *****/
    ENTER_DEBUGGER ();
    for (efp = ErrorFrameStack; efp; efp = efp->previousErrorFrame) {
        nextSearchCode = errorCode;
	do {
	    searchCode = nextSearchCode;
	    if (IsntNil (ehp = efp->handlers[HandlerSlot (searchCode)])) {
	    /*****  A handler was found  *****/
		errorDescription.handlerErrorCode = searchCode;
	    /*****  Record the error  *****/
		FAULT_RecordError (&errorDescription);
	    /*****  and Run the handler found  *****/
		handlerResult = (*ehp->handlerFunction)(&errorDescription);
		if (!noReturn)
		    return handlerResult;
	    /*****  Don't allow return from a 'no return' signal  *****/
		ERR_SignalFault (
		    EC__CantReturn,
		    "Error Handler Ignored No Return Indication"
		);
	    /*****  Really punt if nobody's listening (bye, bye...) *****/
		DefaultErrorHandler (&errorDescription);
	    }
	    else nextSearchCode = ERRDEF_SuperCode (searchCode);
	} while (searchCode != EC__AnError);
    }

/*****  If no handler could be found, punt (bye, bye...)  *****/
    DefaultErrorHandler (&errorDescription);

    return 0;
}


/******************************************
 *****  Error Frame Creation Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to establish an error frame for the duration of the execution
 *****  of '*actionFn'.
 *
 *  Arguments:
 *	actionFn		- the address of a function to be executed.
 *				  The function should expect its argument in
 *				  the form of a 'varargs' argument pointer.
 *	actionFnArg1, ... actionFnArgN
 *				- an arbitrary collection of arguments to be
 *				  passed to '*actionFn'.
 *
 *  Returns:
 *	Nothing.
 *
 *  Behavior:
 *	An error frame is a repository for a collection of error handler
 *	defaults.  Error frames are established to allow local overrides
 *	of the current processing of certain errors.  There is always a
 *	global error frame containing a default handler for 'AnError'
 *	that aborts the system.  The definitions in that global frame may
 *	be overridden or new error frames may be pushed at will.
 *
 *	The error frame established by this function exists as long as
 *	'*actionFn' is executing.  When '*actionFn' returns, the error
 *	frame is popped.  The error frame is also popped whenever the
 *	stack is unwound to a point above the call to this routine.
 *
 *****/
PublicFnDef void __cdecl ERR_EstablishErrorFrame (void (*actionFn) (va_list), ...) {
/*****  Initialize the new frame  *****/
    ErrorFrame newFrame;
    newFrame.previousErrorFrame = ErrorFrameStack;
    for (int i = 0; i < ErrorFrameHandlerSlots; i++)
        newFrame.handlers[i] = NilHandler;

/*****  Push the new frame and call the user function  *****/
    UNWIND_Try {
	ErrorFrameStack = &newFrame;
	V_VARGLIST (ap, actionFn);
	actionFn (ap);
    } UNWIND_Finally {
	ErrorFrameStack = ErrorFrameStack->previousErrorFrame;
	DeallocateFrameHandlerDefs (&newFrame);
    } UNWIND_EndTryFinally;
}


/***********************************************
 *****  Error Handler Definition Routines  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to set a handler for a specified error code in the current
 *****  error frame.
 *
 *  Arguments:
 *	errorCode		- the error code for which the handler is
 *				  being established.
 *	handlerFn		- the address of a handler for the error code.
 *				  When invoked, the handler will be passed the
 *				  address of an 'ERR_ErrorDescription' (see
 *				  header for more information).
 *
 *  Returns:
 *	NOTHING.
 *
 *  Errors Signalled:
 *	InvalidErrorCode
 *	MallocError
 *
 *  Behavior:
 *	If no error frame is active (possible only before this facility is
 *	initialized), this routine will do nothing.
 *
 *****/
PublicFnDef void ERR_SetErrorHandler (
    ERRDEF_ErrorCode		errorCode,
    ERR_ErrorHandler		handlerFn
)
{
    ErrorHandlerDef *ehp;

    if (IsNil (ErrorFrameStack) || InvalidErrorCode (errorCode))
        return;

    if (IsNil (ehp = ErrorFrameStack->handlers[HandlerSlot (errorCode)]))
    {
        ehp = AllocateHandlerDef ();
	ehp->previousHandlerDef	    = NilHandler;
    }

    ehp->handlerFunction	    = handlerFn;

    ErrorFrameStack->handlers[HandlerSlot (errorCode)] = ehp;
}

/*---------------------------------------------------------------------------
 *****  Routine to push a handler for a specified error code in the current
 *****  error frame.
 *
 *  Arguments:
 *	errorCode		- the error code for which the handler is
 *				  being established.
 *	handlerFn		- the address of a handler for the error code.
 *				  When invoked, the handler will be passed the
 *				  address of an 'ERR_ErrorDescription' (see
 *				  header for more information).
 *
 *  Returns:
 *	NOTHING.
 *
 *  Errors Signalled:
 *	InvalidErrorCode
 *	MallocError
 *
 *  Behavior:
 *	If no error frame is active (possible only before this facility is
 *	initialized), this routine will do nothing.
 *
 *****/
PublicFnDef void ERR_PushErrorHandler (
    ERRDEF_ErrorCode		errorCode,
    ERR_ErrorHandler		handlerFn
)
{
    ErrorHandlerDef *ehp;

    if (IsNil (ErrorFrameStack) || InvalidErrorCode (errorCode))
        return;

    ehp = AllocateHandlerDef ();

    ehp->previousHandlerDef	    =
        ErrorFrameStack->handlers[HandlerSlot (errorCode)];
    ehp->handlerFunction	    = handlerFn;

    ErrorFrameStack->handlers[HandlerSlot (errorCode)] = ehp;
}

/*---------------------------------------------------------------------------
 *****  Routine to pop the current handler for the specified error in the
 *****  current error frame.
 *
 *  Argument:
 *	errorCode		- the error code whose handler stack is to be
 *				  popped.
 *
 *  Returns:
 *	NOTHING
 *
 *  Errors Signalled:
 *	InvalidErrorCode
 *	HandlerStackEmpty
 *
 *  Behavior:
 *	If no error frame is active (possible only before this facility is
 *	initialized), this routine will do nothing.
 *
 *****/
PublicFnDef void ERR_PopErrorHandler (
    ERRDEF_ErrorCode		errorCode
)
{
    ErrorHandlerDef *ehp;

    if (IsNil (ErrorFrameStack) || InvalidErrorCode (errorCode))
        return;

    if (IsNil (ehp = ErrorFrameStack->handlers[HandlerSlot (errorCode)]))
    {
	ERR_SignalException (
	    EC__HandlerStackEmpty,
	    "Attempt to pop an empty error handler stack"
	);
	return;
    }

    ErrorFrameStack->handlers[HandlerSlot (errorCode)] =
        ehp->previousHandlerDef;

    ehp->previousHandlerDef = HandlerDefFreeList;
    HandlerDefFreeList = ehp;
}

/*---------------------------------------------------------------------------
 *****  Routine to clear all handlers for the specified error in the
 *****  current error frame.
 *
 *  Argument:
 *	errorCode		- the error code whose handler stack is to be
 *				  cleared.  This routine affects the current
 *				  error frame only; currently, it is impossible
 *				  to alter a prior error frame.
 *
 *  Returns:
 *	NOTHING
 *
 *  Errors Signalled:
 *	InvalidErrorCode
 *
 *  Behavior:
 *	If no error frame is active (possible only before this facility is
 *	initialized), this routine will do nothing.
 *
 *****/
PublicFnDef void ERR_ClearErrorHandler (
    ERRDEF_ErrorCode		errorCode
)
{
    if (IsNil (ErrorFrameStack) || InvalidErrorCode (errorCode))
        return;

    DeallocateHandlerDefChain (
	ErrorFrameStack->handlers + HandlerSlot (errorCode)
    );
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    int i;
    char *estring;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (ERR);
    FAC_FDFCase_FacilityDescription ("Error Handler");
    case FAC_DoStartupInitialization:
    /*****  Initialize the global error frame...  *****/
	GlobalErrorFrame.previousErrorFrame = NilFrame;
        for (i = 0; i < ErrorFrameHandlerSlots; i++)
	    GlobalErrorFrame.handlers[i] = NilHandler;
    /*****  ... and install it.  *****/
	ErrorFrameStack = &GlobalErrorFrame;

	if (IsntNil (estring = getenv ("VisionExitOnError")))
	    UNWIND_SetExit (strtoul (estring, (char **)NULL, 0));

	if (IsntNil (estring = getenv ("VisionRestartOnError")))
	    UNWIND_SetRestart (strtoul (estring, (char **)NULL, 0));

	if (IsntNil (estring = getenv ("VisionMaxSeriousErrors")))
	    UNWIND_SetMaximumNumberOfLongjumps (strtoul (estring, (char **)NULL, 0));

	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  err.c 1 replace /users/mjc/system
  860226 16:18:11 mjc create

 ************************************************************************/
/************************************************************************
  err.c 2 replace /users/jad
  860421 17:38:58 jad now uses the io module

 ************************************************************************/
/************************************************************************
  err.c 3 replace /users/mjc/translation
  870524 09:37:51 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  err.c 4 replace /users/jad/system
  880711 16:03:34 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
