/*****  Machine Specific Signal Processing Header File  *****/
#ifndef siSIG_H
#define siSIG_H

/***************************************************
 *****   SIGNAL HANDLING SERVICES              *****
 ***************************************************/

/****************
 *  Invariants  *
 ****************/

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/

/****  Define Apollo used signals (these will not occur on HP)  ****/
#define SIGAPOLLO  40   /* Apollo-specific fault */
#define SIGCHLD 41      /* child status has changed */
#define SIGTTIN 24      /* background read attempted from control terminal */
#define SIGTTOU 25      /* background write attempted to control terminal */
#define SIGXCPU 27      /* cpu time limit exceeded */
#define SIGXFSZ 28      /* file size limit exceeded */
#define SIGURG  31      /* urgent condition present on socket */

#define STD_sigvector		sigvector
PublicFnDecl unsigned long 
    alarm ();
PublicFnDecl long 
    sigsetmask ();
#define STD_maskType		long

#define STD_setRestartAfterSignal(contextStruct)\
{\
    if (contextStruct->sc_syscall != SYS_NOTSYSCALL && \
        contextStruct->sc_syscall != SYS_PAUSE)\
        contextStruct->sc_syscall_action = SIG_RESTART;\
}

#define STD_cleanupAfterSignal(handler,signo) /*** No op  ***/

#define STD_SigHandlerType	int

#endif /* hp */

#ifdef VMS			/*****  VAX/VMS  *****/
/*****  Define names for HP signals (these will not occur on VMS) *****/
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* death of a child */
#define	SIGPWR	19	/* power state indication */
#define	SIGVTALRM 20 	/* virtual timer alarm */
#define	SIGPROF 21	/* profiling timer alarm */
#define	SIGIO   22	/* asynchronous I/O */
#define	SIGWINDOW 23 	/* windowing signal */

/****  Define Apollo used signals (these will not occur on VMS)  ****/
#define SIGAPOLLO  40   /* Apollo-specific fault */
#define SIGCHLD 41      /* child status has changed */
#define SIGTTIN 24      /* background read attempted from control terminal */
#define SIGTTOU 25      /* background write attempted to control terminal */
#define SIGXCPU 27      /* cpu time limit exceeded */
#define SIGXFSZ 28      /* file size limit exceeded */
#define SIGURG  31      /* urgent condition present on socket */


#define STD_sigvector		sigvec
PublicFnDecl int
    alarm (unsigned int seconds), 
    sigsetmask (int mask);
#define STD_maskType		int
#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/

#define STD_cleanupAfterSignal(handler,signo)\
{\
    struct sigvec vec;\
    vec.sv_handler	= handler;\
    vec.sv_onstack	= 0;\
    vec.sv_mask		= ~(STD_maskType)0;\
    STD_sigvector (signo, &vec, (struct sigvec *)Nil);\
}

#define STD_SigHandlerType	int

#endif /* VMS */

#ifdef apollo			/*****  DOMAIN/IX  *****/
/*****  Define names for HP signals  *****/
#define	SIGPWR	  40	/* Apollo replaced SIGPWR with SIGAPOLLO */
#define	SIGWINDOW 41 	/* Apollo doesn't have SIGWINDOW */

#define STD_sigvector		sigvec
#define sigvector		sigvec
PublicFnDecl int
    alarm (), 
    sigsetmask ();
#define STD_maskType		int
#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/

#define STD_cleanupAfterSignal(handler,signo) /*** No op  ***/

#define STD_SigHandlerType	int

#endif /* apollo */

#ifdef sun		/*****  Sun  *****/
/*****  Define names for HP signals  *****/
#define	SIGPWR	  40	/* Apollo replaced SIGPWR with SIGAPOLLO */
#define	SIGWINDOW 41 	/* Apollo doesn't have SIGWINDOW */

/****  Define Apollo used signals (these will not occur on Sun)  ****/
#define SIGAPOLLO  42   /* Apollo-specific fault */

#define STD_sigvector		sigvec
#define sigvector		sigvec
PublicFnDecl unsigned long 
    alarm ();
PublicFnDecl int 
    sigsetmask ();
#define STD_maskType		int

#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/

#define STD_cleanupAfterSignal(handler,signo) /*** No op  ***/

#define STD_SigHandlerType	void

#endif /* sun */

#if defined(FRONTEND) || defined(BACKEND)
PublicFnDecl int	STD_SetupVAXSignals();
#endif /* FRONTEND || BACKEND */

#endif /* siSIG_h */


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SIsig.h 1 replace /users/jck/system
  880330 21:56:47 jck SIutil.h

 ************************************************************************/
/************************************************************************
  SIsig.h 2 replace /users/jck/system
  880407 10:05:14 jck Add some changes for the VAX 

 ************************************************************************/
/************************************************************************
  SIsig.h 3 replace /users/m2/dbup
  880421 17:18:23 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  SIsig.h 4 replace /users/jck/system
  880510 08:53:57 jck Completed Handling of Apollo signals

 ************************************************************************/
/************************************************************************
  SIsig.h 5 replace /users/m2/backend
  890503 14:58:24 m2 VAX fix

 ************************************************************************/
/************************************************************************
  SIsig.h 6 replace /users/m2/backend
  890927 14:35:54 m2 SUN port (preliminary)

 ************************************************************************/
