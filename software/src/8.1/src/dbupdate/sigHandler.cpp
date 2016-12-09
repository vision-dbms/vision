/************************************************
 **********	sigHandler.c	*****************
 ***********************************************/

#include "Vk.h"

#include "sigHandler.h"

PublicFnDef void display_error (
    char const*			msg
)
{
    char buffer[256];
    sprintf(buffer, "%s [errinfo = 0,  errno = %d]", msg, errno);
    perror(buffer);
}

PublicFnDef void handle_signal (
    int				sig,
    int				code,
    struct sigcontext *		scp
)
{
    char fpebuf[128];

    STD_setRestartAfterSignal(scp);

    switch (sig) {

    case SIGHUP:
	display_error ("*** hangup signal received ***");
	break;
	
    case SIGINT:
	display_error ("*** interrupt signal received ***");
	break;
	
    case SIGQUIT:
	display_error ("*** quit signal received ***");
	break;
	
    case SIGILL:
	display_error ("*** illegal instruction signal received ***");
	break;
	
    case SIGTRAP:
	display_error ("*** trace trap signal received ***");
	break;
	
    case SIGIOT:
	display_error ("*** abort (IOT) signal received ***");
	break;
	
    case SIGEMT:
	display_error ("*** software generated signal received ***");
	break;
	
    case SIGFPE:
	sprintf(fpebuf,"*** floating point exception signal received (%d) ***",code);
	display_error (fpebuf);
	break;
	
    case SIGBUS:
	display_error ("*** bus error signal received ***");
	break;
	
    case SIGSEGV:
	display_error ("*** segmentation violation signal received ***");
	break;
	
    case SIGSYS:
	display_error ("*** bad arg. to system call signal received ***");
	break;
	
    case SIGPIPE:
	display_error ("*** write on a pipe with no reader signal received ***");
	break;
	
    case SIGALRM:
	display_error ("*** alarm clock signal received ***");
	break;
	
    case SIGTERM:
	display_error ("*** software termination signal received ***");
	break;
	
    case SIGUSR1:
	display_error ("*** user signal 1 signal received ***");
	break;
	
    case SIGUSR2:
	display_error ("*** user defined signal 2 signal received ***");
	break;
	
    case SIGPWR:
	display_error ("*** power fail signal received ***");
	break;
	
    case SIGVTALRM:
	display_error ("*** virtual timer alarm signal received ***");
	break;
	
    case SIGPROF:
	display_error ("*** profiling timer alarm signal received ***");
	break;
	
    case SIGIO:
	display_error ("*** SIGIO signal received ***");
	break;
	
    case SIGWINDOW:
	display_error ("*** window or mouse signal received ***");
	break;
	
    case SIGCHLD:
        display_error ("*** child status has changed ***");
	break;
	
    case SIGTTIN:
        display_error ("*** background read attempted from control terminal ***");
	break;

    case SIGTTOU:
        display_error ("*** background write attempted to control terminal ***");
	break;

    case SIGXCPU:
        display_error ("*** cpu time limit exceeded ***");
	break;
	
    case SIGXFSZ:
        display_error ("*** file size limit exceeded ***");
	break;
	
    case SIGURG:
        display_error ("*** urgent condition present on socket ***");
	break;
	
    default:
	display_error ("*** unknown signal received ***");
	break;
    }

    exit(ErrorExitValue);
}

PublicFnDef int set_signals (
    void (*routine)(
	int				sig,
	int				code,
	struct sigcontext		*scp
    )
)
{
    struct sigvec vec;
    
    vec.sv_handler = (STD_SignalHandlerType)routine;
    vec.sv_mask    = 037777777777;
     
    if (
/****
	STD_sigvector (SIGHUP, &vec, 0) == -1 ||  
	STD_sigvector (SIGINT, &vec, 0) == -1 ||  
	STD_sigvector (SIGQUIT, &vec, 0) == -1 || 
****/
	STD_sigvector (SIGILL, &vec, 0) == -1 ||
	STD_sigvector (SIGTRAP, &vec, 0) == -1 ||
	STD_sigvector (SIGIOT, &vec, 0) == -1 ||
	STD_sigvector (SIGEMT, &vec, 0) == -1 ||
	STD_sigvector (SIGFPE, &vec, 0) == -1 ||
	STD_sigvector (SIGBUS, &vec, 0) == -1 ||
	STD_sigvector (SIGSEGV, &vec, 0) == -1 ||
	STD_sigvector (SIGSYS, &vec, 0) == -1 ||
	STD_sigvector (SIGPIPE,  &vec, 0) == -1 ||
	STD_sigvector (SIGALRM,  &vec, 0) == -1 ||
	STD_sigvector (SIGTERM,  &vec, 0) == -1 ||
	STD_sigvector (SIGUSR1,  &vec, 0) == -1 ||
	STD_sigvector (SIGUSR2,  &vec, 0) == -1 ||
	STD_sigvector (SIGPWR,  &vec, 0) == -1 ||
	STD_sigvector (SIGVTALRM,  &vec, 0) == -1 ||
	STD_sigvector (SIGPROF,  &vec, 0) == -1 ||
	STD_sigvector (SIGIO,  &vec, 0) == -1 ||
	STD_sigvector (SIGWINDOW, &vec, 0) == -1 ) {
	    return(0);
	    }
/****  Block Hangup, Interrupt and Quit ****/
    STD_sigblock (7);
    
    return(1);
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  sigHandler.c 1 replace /users/ees/dbutil
  861017 12:21:18 ees set_signal() and display_error() 

 ************************************************************************/
/************************************************************************
  sigHandler.c 2 replace /users/jck/updates
  861103 09:04:24 jck Modified signal handling organization

 ************************************************************************/
/************************************************************************
  sigHandler.c 3 replace /users/jck/updates
  861219 11:16:15 jck Hangup and interrupt signals now blocked

 ************************************************************************/
/************************************************************************
  sigHandler.c 4 replace /users/m2/dbup
  880426 12:24:27 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  sigHandler.c 5 replace /users/jck/updates
  880510 08:58:55 jck Added Error Messages for the Apollo signals

 ************************************************************************/
/************************************************************************
  sigHandler.c 6 replace /users/jck/dbupdate
  891006 14:54:43 jck Added STD exit codes. Arranged to display more info for floating point exceptions

 ************************************************************************/
