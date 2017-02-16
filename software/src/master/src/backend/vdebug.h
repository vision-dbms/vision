/*****  Internal Debugger Exported Declarations  *****/
#ifndef DEBUG_H
#define DEBUG_H

class M_ASD;

/*****  Internal Debugger Main Program  *****/
PublicFnDecl void DEBUG_Main (M_ASD *pScratchPad);

/*****  Testdeck Tracing Declarations  *****/
PublicVarDecl bool DEBUG_TestdeckTracing;

#define DEBUG_TraceTestdeck(string) {\
    if (DEBUG_TestdeckTracing)\
	IO_printf ("///TESTDECK:%s///\n", string);\
}

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  debug.h 1 replace /users/mjc/system
  860226 16:17:51 mjc create

 ************************************************************************/
/************************************************************************
  debug.h 2 replace /users/mjc/translation
  870524 09:37:16 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  debug.h 3 replace /users/jad/system
  880321 16:26:23 jad implement testdeck tracing

 ************************************************************************/
