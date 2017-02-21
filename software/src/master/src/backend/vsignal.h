/***** Signal Handler *****/
#ifndef SIGNAL_Interface
#define SIGNAL_Interface

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

struct ERR_ErrorDescription;

/*******************************
 ***** Global Signal Flags *****
 *******************************/

PublicVarDecl unsigned int	SIGNAL_InterruptCount;
PublicVarDecl bool		SIGNAL_InInterruptState;

/**********************************
 ***** Public Signal Routines *****
 **********************************/

PublicFnDecl void SIGNAL_TurnOffInterruptHandling ();

PublicFnDecl void SIGNAL_TurnOnInterruptHandling ();

PublicFnDecl void SIGNAL_SetUpSignalHandlers ();

PublicFnDecl void SIGNAL_SetUpSignalIgnorance ();

PublicFnDecl int SIGNAL_HandleDisconnect (ERR_ErrorDescription *errorDescription);

PublicFnDecl int SIGNAL_HandleInterrupt (ERR_ErrorDescription *errorDescription);

PublicFnDecl void SIGNAL_ResetInterrupt ();

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  signal.h 1 replace /users/jad/system
  870318 12:21:48 jad split tout the signal stuff from main and mainbody

 ************************************************************************/
/************************************************************************
  signal.h 2 replace /users/jad/system
  870324 13:11:38 jad modified interrupt handling to only be on during execution

 ************************************************************************/
/************************************************************************
  signal.h 3 replace /users/mjc/translation
  870524 09:41:54 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
