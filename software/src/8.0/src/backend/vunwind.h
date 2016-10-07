/*****  Execution Unwinding Facility Header File  *****/
#ifndef UNWIND_H
#define UNWIND_H

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

#include "Vk.h"

#if defined(__linux__) || defined(sun)

/*****
 *  The Solaris2 version of setjmp/longjmp does NOT save/restore 
 *  signal masks ... hence the following redefinitions ....
 *****/

#ifdef  longjmp
#undef  longjmp
#endif
#define longjmp         siglongjmp

#ifdef  setjmp
#undef  setjmp
#endif
#define setjmp(a)       sigsetjmp ((a), 1)

#ifdef  jmp_buf
#undef  jmp_buf
#endif
#define jmp_buf         sigjmp_buf

#endif


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void UNWIND_Exit (int exitValue = ErrorExitValue);

PublicFnDecl void UNWIND_ThrowException ();

PublicFnDecl void UNWIND_SetExit (unsigned int onOff);

PublicFnDecl void UNWIND_SetRestart (unsigned int onOff);

PublicFnDecl void UNWIND_SetMaximumNumberOfLongjumps (unsigned int max);

PublicFnDecl unsigned int UNWIND_GetMaximumNumberOfSeriousErrors ();


/********************************************
 ********************************************
 *****  Unwind Target Class Definition  *****
 ********************************************
 ********************************************/

class UNWIND_Target {
//  Construction
public:
    UNWIND_Target ();

//  Query
public:
    unsigned int restarting () const {
	return m_fRestarting;
    }

//  Stack Control
public:
    unsigned int set (int iSetjmpResult);
    void pop ();

    void hop ();

//  Panic
public:
    void panic () const;

//  State
protected:
    UNWIND_Target* const	m_pPredecessor;
    unsigned int		m_fRestarting;
public:
    jmp_buf			jumpBuffer;
};


/**************************************************
 **************************************************
 *****  Destination And Trap Creation Macros  *****
 **************************************************
 **************************************************/

/***********************************************************
 *****  Code Body Execution Reason Interrogator Macro  *****
 ***********************************************************/

#define UNWIND_IsRestarting UNWIND__Dest.restarting ()

/********************************
 *****  UNWIND_TryAndCatch  *****
 ********************************/

/*-----------------------------------------------------------------------------
 *  Use:
 *	UNWIND_TryAndCatch {
 *	    ... some code to run / re-run until it terminates normally
 *	} UNWIND_EndTryAndCatch;
 *
 *****/
#define UNWIND_TryAndCatch {\
    UNWIND_Target UNWIND__Dest;\
    UNWIND__Dest.set (setjmp (UNWIND__Dest.jumpBuffer));

#define UNWIND_EndTryAndCatch \
    UNWIND__Dest.pop ();\
}


/*************************
 *****  Try/Finally  *****
 *************************/

/*-----------------------------------------------------------------------------
 *  Use:
 *	UNWIND_Try {
 *	    ... some code to run
 *	} UNWIND_Finally {
 *	    ... code to run whenever 'some code to run' exits or raises an exception
 *	} UNWIND_EndTryFinally
 *
 *****/
#define UNWIND_Try {\
    UNWIND_Target UNWIND__Dest;\
    if (UNWIND__Dest.set (setjmp (UNWIND__Dest.jumpBuffer)))

#define UNWIND_Finally \
    UNWIND__Dest.pop ();

#define UNWIND_EndTryFinally \
    if (UNWIND__Dest.restarting ())\
        UNWIND__Dest.hop ();\
}


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  unwind.h 1 replace /users/mjc/system
  860226 16:25:52 mjc create

 ************************************************************************/
/************************************************************************
  unwind.h 2 replace /users/mjc/system
  860421 17:25:13 mjc Added 'UNWIND_LongJumpTrapAndCleanup' macro

 ************************************************************************/
/************************************************************************
  unwind.h 3 replace /users/mjc/translation
  870524 09:42:24 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  unwind.h 4 replace /users/jck/system
  880421 20:03:13 jck Corrected some enumeration type clashes that showed up during the Apollo's compile

 ************************************************************************/
