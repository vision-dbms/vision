/*****  Keys Header File  *****/
#ifndef KEYS_H
#define KEYS_H

/*****  Shared Definitions  *****/
#include "keys.d"

/*****  Keyboard input routines  *****/
PublicFnDecl void KEY_onResizeSignal ();

PublicFnDecl void KEY_setKeypad();
PublicFnDecl void KEY_initKeyboard();
PublicFnDecl int KEY_cready();
PublicFnDecl int KEY_getkey(int allowRepetition);
PublicFnDecl int KEY_putc(int c);
PublicFnDecl int KEY_beginScriptWrite();
PublicFnDecl int KEY_beginScriptRead();
PublicFnDecl int KEY_beginScriptReadF(char const *scriptFile);

PublicVarDecl int KEY_scriptWrite, KEY_scriptRead, KEY_QuoteNextKey,
		  DELAYbits, PASSTHRUbits;

PublicVarDecl int RepetitionCount, RepetitionKey, RepetitionQuote;

#endif


/************************************************************************
  keys.h 2 replace /users/ees/interface
  860730 15:27:25 ees  

 ************************************************************************/
/************************************************************************
  keys.h 3 replace /users/lis/frontend/M2/sys
  870811 17:04:54 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  keys.h 4 replace /users/lis/frontend/M2/sys
  871022 17:56:33 m2 Added script files

 ************************************************************************/
/************************************************************************
  keys.h 5 replace /users/lis/frontend/M2/sys
  871023 17:50:52 m2 Added help  screen for META commands

 ************************************************************************/
/************************************************************************
  keys.h 6 replace /users/lis/frontend/M2/sys
  880505 11:12:19 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  keys.h 7 replace /users/lis/frontend/M2/sys
  880525 18:16:45 m2 Interface form, Paste Buffers, editor enhacncements, etc.

 ************************************************************************/
/************************************************************************
  keys.h 8 replace /users/m2/frontend/sys
  891011 14:25:31 m2 Keys and variable fixes

 ************************************************************************/
