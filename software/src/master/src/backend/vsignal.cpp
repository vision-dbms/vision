/*****  Signal Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName SIGNAL_

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "batchvision.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vunwind.h"
#include "vutil.h"

/*****  Self  *****/
#include "vsignal.h"


/***********************************
 ***********************************
 *****  Constants and Globals  *****
 ***********************************
 ***********************************/

/******************************
 *****  Internal Defines  *****
 ******************************/

/*****  
 * The number of seconds for the interrupt handler to set the alarm ...
 *****/
#define InterruptAlarmTime  60

/*********************************
 *****  Global Signal Flags  *****
 *********************************/

PublicVarDef  unsigned int SIGNAL_InterruptCount = 0;
PublicVarDef  bool SIGNAL_InInterruptState = false;

PrivateVarDef bool SIGNAL_InterruptEnabled = false;


/*************************************
 *************************************
 *****  Signal Handler Routines  *****
 *************************************
 *************************************/

/*---------------------------------------------------------------------------
 *****  Standard signal handling routine.
 *
 *  Arguments:
 *	See 'sigvector(2)' in the UNIX reference manuals.
 *	See 'sigvec' in the VAX reference manuals.
 *
 *  Returns:
 *	It matters not.
 *
 *  Notes:
 *	Currently, all signals go through the same signal handler by default.
 *	That handler effectively converts the signal to an error which is
 *	signalled using 'ERR_SignalError'.  If 'ERR_SignalError' returns to
 *	this signal handler, this signal handler will return control to the
 *	interrupted program in a manner appropriate to the signal generated.
 *
 *	The I-Object passed to ERR_SignalError will be an integer I-Object
 *	containing the number of the signal generated.  This may be changed
 *	to provide more information if the need arises.
 *
 *	We are using sigvector instead of signal for the following reasons:
 *	    - Signal resets the signal action everytime the signal occurs, 
 *	      so you need to reset it back inside the signal handler routine.
 *	      The problem is that there is a time gap when the signal is not
 *	      set.  Sigvector does not 'turn off' the interrupt when it occurs.
 *	    - Sigvector allows you to 'stack' signals whereas signal 
 *	      cannot 'stack' signals.  This means it is possible for a signal
 *	      to occur and while it is being processed another signal can occur
 *	      interrrupting the first signal handler.  Sigvector uses 'mask'
 *	      to specify which signals to stack.
 *	    - Sigvector allows you to restart an interrupted system call 
 *	      whereas signal does not.
 *
 *****/
PrivateFnDef void SIGNAL_StandardSignalHandler (
    int sig, int Unused(code), struct sigcontext *scp
)
{
    ERRDEF_ErrorCode	signalErrorCode;
    char const*		signalErrorMessage;

    switch (sig) {
    case SIGPIPE:
    case SIGHUP:
        signalErrorCode = EC__HangupSignal;
	signalErrorMessage = "Hangup Signal";
	break;
    case SIGINT:
        signalErrorCode = EC__InterruptSignal;
	signalErrorMessage = "Interrupt Signal";
	break;
    case SIGQUIT:
        signalErrorCode = EC__QuitSignal;
	signalErrorMessage = "The Quit Signal";
	break;
    case SIGILL:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Illegal Instruction Signal";
	break;
    case SIGTRAP:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Trace Trap Signal";
	break;
    case SIGIOT:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "IOT Instruction Signal";
	break;
    case SIGEMT:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "EMT Instruction Signal";
	break;
    case SIGFPE:
        signalErrorCode = EC__FPESignal;
	signalErrorMessage = "Floating Point Exception Signal";
	break;
    case SIGKILL:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Kill Signal";
	break;
    case SIGBUS:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Bus Error Signal";
	ThisProcess.dumpStack ();
	break;
    case SIGSEGV:
        signalErrorCode = EC__SegmentationFault;
	signalErrorMessage = "Segmentation Violation Signal";
	ThisProcess.dumpStack ();
	break;
    case SIGSYS:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Bad Argument To System Call Signal";
	break;
    case SIGALRM:
        signalErrorCode = EC__AlarmSignal;
	signalErrorMessage = "Interrupt Caused Alarm Clock Signal";
	SIGNAL_InInterruptState = false;
	break;
    case SIGTERM:
        signalErrorCode = EC__TerminateSignal;
	signalErrorMessage = "Software Termination Signal";
	break;

/*****  The following signals are not meaningful for VMS  *****/
    case SIGUSR1:
        signalErrorCode = EC__UserSignal1;
	signalErrorMessage = "User Defined Signal 1";
	IO_printf 
	    ("******* ERROR THE EDITOR DIED AND HAS RESTARTED ********\n");
	/* NOTICE THAT THIS GOES TO STDOUT */
	break;
    case SIGUSR2:
        signalErrorCode = EC__UserSignal2;
	signalErrorMessage = "User Defined Signal 2";
	break;
#ifndef __APPLE__
    case SIGWINDOW:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Windowing Signal Signal";
	break;
#endif
    case SIGTSTP:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Control Terminal Stop Signal";
	break;
    case SIGCONT:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Continue After Stop";
	break;
#ifndef __VMS
    case SIGCLD:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Death of a Child Signal";
	break;
#ifndef __APPLE__
    case SIGPWR:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Power State Indication Signal";
	break;
#endif
    case SIGVTALRM:
        signalErrorCode = EC__VTAlarmSignal;
	signalErrorMessage = "Virtual Timer Alarm Signal";
	break;
    case SIGPROF:
        signalErrorCode = EC__PTAlarmSignal;
	signalErrorMessage = "Profiling Timer Alarm Signal";
	break;
    case SIGIO:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = "Asynchronous I/O Signal";
	break;
#if SIGCLD != SIGCHLD
    case SIGCHLD:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Child Status Signal";
	break;
#endif
    case SIGTTIN:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Control Terminal Read Signal";
	break;
    case SIGTTOU:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Control Terminal Write Signal";
	break;
    case SIGXCPU:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "CPU Signal";
	break;
    case SIGXFSZ:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "File Size Signal";
	break;
    case SIGURG:
	signalErrorCode = EC__AnInterrupt;
        signalErrorMessage = "Socket Signal";
	break;
#endif
    default:
        signalErrorCode = EC__AnInterrupt;
	signalErrorMessage = UTIL_FormatMessage ("Unknown Signal # %d", sig);
        break;
    }

    ERR_SignalError (
	signalErrorCode, signalErrorMessage, IOBJ_IntIObject (sig)
    );

    STD_setRestartAfterSignal (scp);
}


/******************************************
 ******************************************
 *****  Interrupt Handling Utilities  *****
 ******************************************
 ******************************************/

/**********************************************
 *****  Private Interrupt Handling State  *****
 **********************************************/

PrivateVarDef struct sigvec	InterruptOffSetting,
				InterruptOnSetting;
PrivateVarDef bool		SIGNAL_InterruptAlwaysIgnored = false;


/***********************************************
 *****  Public Interrupt Control Handling  *****
 ***********************************************/

PublicFnDef void SIGNAL_TurnOffInterruptHandling () {
#if !defined(__VMS)
    if (SIGNAL_InterruptAlwaysIgnored)
	return;

    SIGNAL_InterruptEnabled = false;
    STD_sigvector (
	SIGINT, &InterruptOffSetting, NilOf (struct sigvec *)
    );
#endif
}

PublicFnDef void SIGNAL_TurnOnInterruptHandling () {
#if !defined(__VMS)
    if (SIGNAL_InterruptAlwaysIgnored)
	return;

    SIGNAL_InterruptEnabled = true;
    STD_sigvector (
	SIGINT, &InterruptOnSetting, NilOf (struct sigvec *)
    );
#endif
}


/*********************************************
 *****  Win32 Console Interrupt Handler  *****
 *********************************************/

#if defined(_WIN32)
PrivateFnDef BOOL WINAPI ConsoleCtrlHandler (DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
	if (SIGNAL_InterruptEnabled)
	    SIGNAL_HandleInterrupt (0);
	return true;

    default:
	return false;
    }
}
#endif


/**************************************
 *****  Interrupt Handling Setup  *****
 **************************************/

/*****  Private routine which sets up two sigvector structures to be used to
 *****  turn on and off interrupt handling.
 *****/
PrivateFnDef void InitializeInterruptHandling () {
    if (SIGNAL_InterruptAlwaysIgnored)
	return;

#if defined(__VMS)
#elif defined(_WIN32)
    SetConsoleCtrlHandler (ConsoleCtrlHandler, true);
#else
    struct sigvec ovec;

/*****  Get the original value of the interrupt signal handler ... *****/
    STD_sigvector (SIGINT, NilOf (struct sigvec *), &ovec);
    
/*****  Setup two settings ...  *****/
    InterruptOffSetting.sv_handler	= (STD_SignalHandlerType)SIG_IGN;
    InterruptOffSetting.sv_onstack	= 0;
    InterruptOffSetting.sv_mask		= ~(STD_maskType)0;

    /*****  If interrupts were set to ignore originally, never turn them on **/
    if ((STD_SignalHandlerType)SIG_IGN == ovec.sv_handler)
	SIGNAL_InterruptAlwaysIgnored = true;
    else {
	InterruptOnSetting.sv_handler	=
	    (STD_SignalHandlerType)SIGNAL_StandardSignalHandler;
	InterruptOnSetting.sv_onstack	= 0;
	InterruptOnSetting.sv_mask	= ~(STD_maskType)0;

    /*****  ... and turn them off initially:  *****/
	SIGNAL_TurnOffInterruptHandling ();
    }
#endif
}


/************************************************
 ************************************************
 *****  Signal Handler Management Routines  *****
 ************************************************
 ************************************************/

/************************************
 *****  Win32 Exception Filter  *****
 ************************************/

#if defined(_WIN32)
static long __stdcall ExceptionFilter (EXCEPTION_POINTERS* pExceptionPointers) {
    EXCEPTION_RECORD * pExceptionRecord = pExceptionPointers->ExceptionRecord;
    DWORD xException = pExceptionRecord->ExceptionCode;

    ERRDEF_ErrorCode xErrorCode = EC__AnInterrupt;
    char const* pErrorDescription;

    switch (xException) {
    case EXCEPTION_ACCESS_VIOLATION:
	xErrorCode = EC__SegmentationFault;
	pErrorDescription = UTIL_FormatMessage (
	    "Access Violation %s Address %08X",
	    pExceptionRecord->ExceptionInformation[0] ? "Writing" : "Reading",
	    pExceptionRecord->ExceptionInformation[1]
	);
	break;

    case EXCEPTION_IN_PAGE_ERROR:
	pErrorDescription = "In Page Error";
	break;

    case EXCEPTION_STACK_OVERFLOW:
	pErrorDescription = "Stack Overflow";
	break;

    case EXCEPTION_PRIV_INSTRUCTION:
	pErrorDescription = "Privileged Instruction";
	break;

    case EXCEPTION_ILLEGAL_INSTRUCTION:
	pErrorDescription = "Illegal Instruction";
	break;

    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	pErrorDescription = "Noncontinuable Exception";
	break;

    default:
	pErrorDescription = UTIL_FormatMessage ("Windows/NT Exception %08X", xException);
	break;
    }

    ERR_SignalError (
	xErrorCode, UTIL_FormatMessage (
	    "%s [%08X]", pErrorDescription, pExceptionRecord->ExceptionAddress
	), IOBJ_IntIObject (xException)
    );

    return pExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE
	? EXCEPTION_CONTINUE_SEARCH
	: EXCEPTION_CONTINUE_EXECUTION;
}
#endif


/**************************
 *****  Signal Setup  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to set up handlers for the trapable signals.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 ******/
PublicFnDef void SIGNAL_SetUpSignalHandlers () {
/*****  Initialize special interrupt handling, ... *****/
    InitializeInterruptHandling ();

#if defined(__VMS)
#elif defined(_WIN32)
    SetUnhandledExceptionFilter (ExceptionFilter);

#else
    struct sigvec vec, lvec;

/*****  Ignore SIGPIPE, Child Status Change and Window Change  *****/
    vec.sv_handler		= (STD_SignalHandlerType)SIG_IGN;
    vec.sv_onstack		= 0;
    vec.sv_mask			= ~(STD_maskType)0;

    STD_sigvector (SIGPIPE	, &vec, NilOf (struct sigvec *));
    STD_sigvector (SIGCHLD	, &vec, NilOf (struct sigvec *));
    STD_sigvector (SIGWINCH	, &vec, NilOf (struct sigvec *));

/*----------------------------------------------------------------*
 ***  Death of child signals must be set to SIG_DFL if 'wait'	***
 ***  is to operate correctly.					***
 *----------------------------------------------------------------*/
/*****  'Ignore' Death of Child signal...  *****/
    vec.sv_handler = (STD_SignalHandlerType)SIG_DFL;
    STD_sigvector (SIGCLD, &vec, NilOf (struct sigvec *));

    vec.sv_handler = (STD_SignalHandlerType)SIGNAL_StandardSignalHandler;

/*****  
 * Attempt to catch all interesting, defined signals that have not been
 * previously set to 'ignore'.
 *****/
    for (int i = 1; i <= NSIG; i++) {
	switch (i) {
	case SIGTRAP:
	case SIGTSTP:
	case SIGCONT:
	case SIGTTIN:
	case SIGTTOU:
	case SIGWINCH:
	case SIGCLD:
	case SIGVTALRM:
	case SIGPROF:
	    break;
	case SIGSEGV:
	case SIGBUS:
	    if (IsntNil (getenv ("VisionEnableCoreDump")))
		break;
	default:
	    STD_sigvector (i, NilOf (struct sigvec *), &lvec);
	    if ((STD_SignalHandlerType)SIG_IGN != lvec.sv_handler)
		STD_sigvector (i, &vec, NilOf (struct sigvec *));
	    break;
	}
    }
#endif

}


/************************************
 *****  Signal Ignorance Setup  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to set up ignorance of quit, interrupt and hangup
 *****  signals.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 ******/
PublicFnDef void SIGNAL_SetUpSignalIgnorance () {
    SIGNAL_InterruptAlwaysIgnored = true;

#if defined(__VMS)
#elif defined(_WIN32)
    SetConsoleCtrlHandler (0, true);
#else
    struct sigvec vec;

    vec.sv_handler	= (STD_SignalHandlerType)SIG_IGN;
    vec.sv_onstack	= 0;
    vec.sv_mask		= ~(STD_maskType)0;

    STD_sigvector (SIGHUP, &vec, NilOf (struct sigvec *));
    STD_sigvector (SIGINT, &vec, NilOf (struct sigvec *));
    STD_sigvector (SIGQUIT, &vec, NilOf (struct sigvec *));
#endif
}


/***********************************
 *****  Signal Error Handlers  *****
 ***********************************/

/************************
 *  Disconnect Handler  *
 ************************/

/*---------------------------------------------------------------------------
 *****  The default read-eval-print loop disconnect handler.
 *
 *  Arguments:
 *	errorDescription	- the address of the error description
 *				  associated with the hang-up.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef int SIGNAL_HandleDisconnect (ERR_ErrorDescription* Unused(errorDescription)) {
/*****  Gracefully exit the program  *****/
    UNWIND_Exit ();

    return 0;
}


/***********************
 *  Interrupt Handler  *
 ***********************/

/*---------------------------------------------------------------------------
 *****  The interrupt handler.
 *
 *  Arguments:
 *	errorDescription	- the address of the error description
 *				  associated with the interrupt.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef int SIGNAL_HandleInterrupt (ERR_ErrorDescription* Unused(errorDescription)) {
/*****  Acknowledge the interrupt ... *****/
    IO_printf ("\n*** INTERRUPT ***\n");

/*****  If this is a new interrupt, process it ... *****/
    if (!SIGNAL_InInterruptState) {
	alarm (InterruptAlarmTime); 
	SIGNAL_InInterruptState = true;
    }

    return 0;
}

PublicFnDef void SIGNAL_ResetInterrupt () {
#if !defined(__VMS)
    SIGNAL_TurnOffInterruptHandling ();
    if (SIGNAL_InInterruptState) {
    /*****  Make sure that this does not get interrupted ... *****/
	STD_maskType mask = STD_sigsetmask (~(STD_maskType)0);

    /*****  Reset the interrupt ... *****/
	alarm (0);
	SIGNAL_InInterruptState = false;

    /*****  Restore the original mask ... *****/
	STD_sigsetmask (mask);
    }
#endif
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (SIGNAL);
    FAC_FDFCase_FacilityDescription ("The Signal Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  signal.c 1 replace /users/jad/system
  870318 12:21:44 jad split tout the signal stuff from main and mainbody

 ************************************************************************/
/************************************************************************
  signal.c 2 replace /users/jad/system
  870324 13:11:34 jad modified interrupt handling to only be on during execution

 ************************************************************************/
/************************************************************************
  signal.c 3 replace /users/mjc/translation
  870524 09:41:50 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  signal.c 4 replace /users/mjc/system
  870525 13:50:43 mjc Changed 'FacilityName' to correct global id case insensitivity problem on VAX

 ************************************************************************/
/************************************************************************
  signal.c 5 replace /users/jad/system
  880411 13:16:01 jad vax signal implementation

 ************************************************************************/
/************************************************************************
  signal.c 6 replace /users/jck/system
  880510 08:56:55 jck Completed Handling of Apollo signals

 ************************************************************************/
/************************************************************************
  signal.c 7 replace /users/m2/backend
  890503 15:31:25 m2 VAX fixes

 ************************************************************************/
/************************************************************************
  signal.c 8 replace /users/jck/system
  890714 15:13:59 jck Arrange to always ignore SIGCLD

 ************************************************************************/
/************************************************************************
  signal.c 9 replace /users/m2/backend
  890927 15:39:04 m2 Ignore SIGWINCH signal, and make sure SIGCLD different from SIGCHLD

 ************************************************************************/
