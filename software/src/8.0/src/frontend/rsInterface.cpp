/*******************************************************
 *****        rsInterface.c		    ************
 ******************************************************/

#include "Vk.h"

#include <sys/wait.h>

#include "rsInterface.h"

/***** local includes *****/
#include "stdcurses.h"
#include "misc.h"
#include "gopt.h"
#include "edit.h"
#include "buffers.h"
#include "keys.h"
#include "page.h"
#include "main.i"
#include "print.h"
#include "error.h"
#include "mainmenu.h"

#if !defined(solaris_1)
#include <term.h>
#endif

PublicVarDef int DEBUG;
PrivateVarDef int Administrator;
PrivateVarDef jmp_buf give_up;

PrivateFnDef void RS_callSignalHandler (
    int				sig
);


/*****************************************
 *****  Warning And Error Utilities  *****
 *****************************************/

PublicFnDef void STD_syswarn (char const *msg) {
    fprintf (stderr, "ERROR: %s (%d; %s)\n", msg, errno, strerror(errno));
}

PublicFnDef void STD_syserr (char const *msg) {
    STD_syswarn (msg);
    RS_callSignalHandler(0);
}


/*********************************
 *****  Execution Utilities  *****
 *********************************/


/*** NOTE: The stderr and stdout are mapped to the same streams ***/
PublicFnDef pid_t STD_execute (
    char const*			name,
    char*			args[],
    int *			fdin,
    int *			fdout,
    void			(*preChildProcessing)(void)
)
{
    int	Pfdin[2], Pfdout[2], tmpin, tmpout, tmperr, pid;

    if (pipe (Pfdin ) == -1 || pipe (Pfdout) == -1)
	STD_syswarn ("opening 2 pipes");

    tmpin  = dup(0);
    tmpout = dup(1);
    tmperr = dup(2);

    switch (pid = vfork ())
    {
    case -1:
	STD_syswarn ("vfork error");

    case 0:
	if (close (0) == -1)
	    STD_syswarn ("close std in");
	if ( dup (Pfdin[0]) != 0)
	    STD_syserr ("dup std in");
	if (close (1) == -1)
	    STD_syswarn ("close std out");
	if (dup (Pfdout[1]) != 1)
	    STD_syserr ("dup std out");
	if (close (2) == -1)
	    STD_syswarn ("close std err");
	if (dup (Pfdout[1]) != 2)
	    STD_syserr ("dup std err");

	if( preChildProcessing != NULL )
		(*preChildProcessing)();

	execvp (name, args);

	/* should never get here */
	STD_syswarn ("should never be here");
    }

    *fdin  = Pfdin[1];
    *fdout = Pfdout[0];

    if (close (0) == -1)
	    STD_syswarn ("close std in");
    if (dup (tmpin) != 0)
	    STD_syserr ("dup std in");
    if (close (1) == -1)
	    STD_syswarn ("close std out");
    if (dup (tmpout) != 1)
	    STD_syserr ("dup std out");
    if (close (2) == -1)
	    STD_syswarn ("close std err");
    if (dup (tmperr) != 2)
	    STD_syserr ("dup std err");
    close(tmpin);
    close(tmpout);
    close(tmperr);

    return( pid );
}


/*****************************
 *****  Print Utilities  *****
 *****************************/

PrivateVarDef FILE *RS_outfile;

PublicFnDef int RS_fprintf (char const*	fmt, ...) {
    va_list ap;
    int result;

    va_start (ap, fmt);

    result = vfprintf (RS_outfile, fmt, ap);
    fflush (RS_outfile);

    va_end (ap);

    return result;
}


/*******************************************
 **********	Public Variables    ********
 *******************************************/

PublicVarDef int RS_userSystem = FALSE;    /**** when TRUE cannot toggle
						  auto ?g *****/
PublicVarDef int RS_autog = TRUE;	    /**** always send ?g ****/
PublicVarDef jmp_buf RS_userjmpbuf;   
PublicVarDef int RS_jmpToUser = (-1);
PublicVarDef char RS_CompanyName[RS_NameLen+1];
PublicVarDef struct tm *RS_Date;
PublicVarDef int RS_AdministratorPrivate = FALSE; /* If TRUE then Data Entry allowed */
PublicVarDef int RS_DataWasUpdated = FALSE; /* If TRUE then Data Entry occurred */


/****************************************
 *****  Global Variables    *************
 ****************************************/

PrivateVarDef
    struct sigvec parentSignalValues[NSIG];

PrivateVarDef int   BackendIsLocal;
PrivateVarDef char  BackendHost[256];
PrivateVarDef char  BackendUser[128];
PrivateVarDef char  BackendPass[128];
PrivateVarDef char const*BackendPath;

PrivateVarDef int AllDone = FALSE;
PublicVarDef FILE *RSstdin;
PublicVarDef FILE *RSstdout;
PublicVarDef int RSfdout;
PublicVarDef int RSfdin;
PublicVarDef pid_t RSpid = 0;
PrivateVarDef int InitializedCurses = FALSE;
PrivateVarDef int WindowTooSmall = FALSE;
PrivateVarDef int ASubshellHasBeenSpawned = FALSE;
PrivateVarDef int PauseShouldBeRestarted = FALSE;

/***** globals used to pass the arguments to the research system *****/
PrivateVarDef char *RSargs[128];


/*********************************
 *** TALKING THROUGH THE PIPES ***
 *********************************/

/*** make sure that the two delimiters ending the child line and the parent
     line are exactly what each expects, or the program will hang ***/

#define writeLine(l)\
{\
    if (fprintf(RSstdin, "%s\n", l) == EOF)\
        STD_syserr("writing to RSstdin");\
    fflush(RSstdin);\
    if (DEBUG) RS_fprintf ("WRITE: %s\n", l);\
}

PublicFnDef void RS_writeLine (char const *str) {
    writeLine(str);
}

PublicFnDef void RS_nodelayOutput (
    int				on
)
{
    PrivateVarDef int blockf, nonblockf;
    PrivateVarDef int first=TRUE;
    int flags;
    
    if (first)
    {
	first = FALSE;
	if ((flags = fcntl (RSfdout, F_GETFL, 0)) == -1)
	    STD_syswarn ("setting nodelay for RSstdout");
	blockf = flags & ~O_NDELAY;
	nonblockf = flags | O_NDELAY;
    }
    if (fcntl (RSfdout, F_SETFL, !on ? blockf : nonblockf) == -1)
	STD_syswarn ("setting nodelay for RSstdout");
}

PrivateVarDef char	fgetsBuf[BUF_maxlinesize+1];

/*---------------------------------------------------------------------------
 *****  Function to read a line from RSstdout
 *
 *  Arguments:
 *	str	    pointer to input buffer
 *	max	    len of input buffer
 *
 *  Returns:
 *	0	    if prompt is received from RS
 *      1, -1	    if input is other than the system prompt
 *
 *****/
PublicFnDef int RS_readLine (
    char *			str,
    int				max
)
{
    int len;

    if (fgets (fgetsBuf, max, RSstdout) == NULL)
	STD_syserr ("reading RSstdout");
    BUF_stripTabs(fgetsBuf, str, max);
    if (DEBUG)
        RS_fprintf ("READ: %s", str);
	
    /*** note: the research system sends \005\n to the user at the end
	       of each prompt string ***/

    len = strlen(str);
    if (len < 2)
        return(TRUE);
    else
	return(strcmp (&str[len - 2], "\005\n"));
}

/*---------------------------------------------------------------------------
 *****  Function to replace the unix "system" command.
 *****  Used For The HP9000s500 Only.
 *
 *   Arguments: 
 *        str		command to be executed
 *
 *   Returns:
 * 	 NOTHING
 *
 *****/
PublicFnDef void RS_system (char const *str) {
    char command[160];
    struct sigvec intvec, quitvec, ivec;
  
    ivec.sv_handler = (STD_SignalHandlerType)SIG_IGN;
    ivec.sv_mask    = ~0;
    ivec.sv_onstack = 0;

    /*****
     * The following code is identical to the unix 'system' command except it
     * uses a pause instead of a wait in order to tell when the subShell is 
     * done.  The wait does not work here because of interactions with the 
     * signal handlers.  The subShell generates a death of child signal when 
     * it is done which the parent signal handler catches.  The death of child 
     * signal handler issues a wait which competes with the wait that 
     * 'system' issued.  To solve this problem, we use pause instead.  
     * Pause will sit there until it gets a signal.  In the parent signal 
     * handler, we can restart a pause.  The parent signal handler will only 
     * exit the pause (not restart a pause) if the signal is a death of child 
     * other than the Research System.  Thus, this pause will only stop on the 
     * death of the subShell which is what we want.
     *****/
    strcpy (command, str);

    if (vfork() == 0)
    {
	execl ("/bin/sh", "sh", "-c", command, NULL);
    }
    STD_sigvector (SIGINT, &ivec, &intvec);
    STD_sigvector (SIGQUIT, &ivec, &quitvec);
    ASubshellHasBeenSpawned = TRUE;
    do
    {
	pause ();
    } 
    while (PauseShouldBeRestarted);
    ASubshellHasBeenSpawned = FALSE;
    STD_sigvector (SIGINT, &intvec, NilOf (struct sigvec *));
    STD_sigvector (SIGQUIT, &quitvec, NilOf (struct sigvec *));
}


/*---------------------------------------------------------------------------
 *****  Function to disgard RSstdout
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void RS_dumpOutput () {
    char str[RS_MaxLine + 1];
    int len;

    RS_nodelayOutput (TRUE);
    
    while (fgets (str, RS_MaxLine, RSstdout))
    {
	if (DEBUG)
	    RS_fprintf ("READ: %s", str);
	len = strlen(str);
	if (len >= 2 && 0 == strcmp (&str[len - 2], "\005\n")) break;
    }
    
    RS_nodelayOutput (FALSE);
}

/*---------------------------------------------------------------------------
 *****  Function to read all output from RSstdout
 *
 *  Arguments:
 *	outbuffer   LINEBUFFER pointer (for RSstdout)
 *	errbuffer   LINEBUFFER pointer (no longer used)
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void RS_readOutput (
    LINEBUFFER *		outbuffer,
    LINEBUFFER *		errbuffer
)
{   
    char str[RS_MaxLine];
    int	 memoryError = FALSE;

/***** read the output from the research system *****/
    while (RS_readLine (str, sizeof (str)))
	if( (outbuffer != NULL) && !memoryError )
	    if( BUF_appendLine(outbuffer, str) == NULL )
	    	memoryError = TRUE;
}

PublicFnDef void RS_compile () {
    writeLine("?g");
}

PublicFnDef void RS_save () {
    int len, success;
    char buffer[RS_MaxLine + 1];
    static char const *output = "Global Environment Saved";

    ERR_displayStr("Saving Global Environment...",FALSE);
    writeLine("?w");

/**** check RSstdout ****/
    success = FALSE;
    len = strlen(output);
    while (RS_readLine (buffer, RS_MaxLine))
	if (0 == strncmp(buffer, output, len))
	    success = TRUE;
	    
    if (success)
        ERR_displayStr(output,FALSE);
    else
	ERR_displayPause("Error Saving Global Environment");

}

PublicFnDef int RS_sendAndCheck(
    char const*	input, char const *output
) {
    int result, len;
    char buffer[RS_MaxLine + 1], *buf;
    
    writeLine(input);
	
/**** read prompt ****/
    while (RS_readLine (buffer, RS_MaxLine))
      ;

    writeLine("?g");

    result = FALSE;

/**** check RSstdout ****/
    VString outp (eatLeadingAndTrailingSpaces(output));
    len = strlen(outp);
    while (RS_readLine (buffer, RS_MaxLine))
    {
	VString buf (eatLeadingAndTrailingSpaces(buffer));
	if (0 == strncmp(buf, outp, len))
	    result = TRUE;
    }
	    
    return(result);
}

PublicFnDef void RS_sendLine(char const* input) {
    char buffer[RS_MaxLine + 1];
    
    writeLine(input);

    while(RS_readLine(buffer, RS_MaxLine))
      ;

    writeLine("?g");
}

PublicFnDef void RS_sendOnly (char const *string) {
    char buffer[RS_MaxLine + 1];

    writeLine(string);
    while(RS_readLine(buffer, RS_MaxLine));
}


/************************************
 *** PIPE COMMUNICATION FUNCTIONS ***
 ************************************/

PrivateVarDef int Restart = FALSE;

PrivateFnDef void RS_restoreParentSignals ();

PrivateFnDef void childInitialization () {
	struct sigvec vec;

	/***** 
	 * On a restart, 
	 * make sure that the interrupts are as they were when we first
         * started the Research System.  They should be set to the same
	 * values as when the front end was started, before we setup the 
	 * parent interrupts.
	 *****/
	if (Restart)
	{
	    RS_restoreParentSignals ();
	    STD_sigsetmask (0);
	}

	/***** 
	 * Force the Research System's quit and Hangup signals to be ignored
	 *****/
	vec.sv_handler = (STD_SignalHandlerType)SIG_IGN;
	vec.sv_mask    = ~(STD_maskType)0;
	vec.sv_onstack = 0;
	STD_sigvector (SIGQUIT, &vec, NilOf (struct sigvec *));
	STD_sigvector (SIGHUP,  &vec, NilOf (struct sigvec *));
}


/*---------------------------------------------------------------------------
 *****  Function to fork child and execute the Vision backend.
 *
 *  Arguments:
 *	restart		TRUE = this is a restart
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void StartBackend (
    int				restart
)
{
    if (BackendIsLocal)
    {
	struct sigvec qvec, hvec;

	STD_sigvector (SIGQUIT, NilOf (struct sigvec *), &qvec);
	STD_sigvector (SIGHUP,  NilOf (struct sigvec *), &hvec);

	Restart = restart;

	RSpid = STD_execute (
	    BackendPath, RSargs, &RSfdin, &RSfdout, childInitialization
	);

	STD_sigvector (SIGQUIT, &qvec, NilOf (struct sigvec *));
	STD_sigvector (SIGHUP,  &hvec, NilOf (struct sigvec *));

    }
    else
    {
#ifdef HAS_REXEC
	char * pBackendHost = BackendHost;
	char** ppArg;
	size_t sCommand = 1;
	char * pCommand;

	for (ppArg = RSargs; IsntNil (*ppArg); ppArg++)
	    sCommand += strlen (*ppArg) + 1;
	pCommand = (char*)malloc (sCommand);
	pCommand[0] = '\0';
	for (ppArg = RSargs; IsntNil (*ppArg); ppArg++)
	{
	    strcat (pCommand, *ppArg);
	    strcat (pCommand, " ");
	}

	RSpid = getpid ();
	RSfdin = RSfdout = rexec (&pBackendHost, 512, BackendUser, BackendPass, pCommand, NULL);

	if (RSfdin < 0)
	    STD_syswarn ("rexec error");
#else
	ERR_fatal (" *** Rexec Not Supported");
#endif
    }

    if (restart) ERR_displayStr (
	" *** Research System has been restarted *** (please wait)", TRUE
    );
}

PrivateFnDef void setupParentSignals ();

/*---------------------------------------------------------------------------
 *****  Function to call the User Interface.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void execute_user () {
    STD_checkTerminalCapabilities ();
    STD_checkInputStream();
    CUR_initscr ();  
    CUR_savetty ();
#if defined(__hp9000s700)
    typeahead (-1);
#endif
    InitializedCurses = TRUE;

    setupParentSignals ();

    char it[] = "it";
    RS_TabSpacing = CUR_tgetnum(it);
    if (RS_TabSpacing <= 0)
	RS_TabSpacing = 8;

    if( (CUR_COLS < 80) || (CUR_LINES < 24) )
    {
    	CUR_werase(CUR_stdscr);
    	CUR_wmove(CUR_stdscr,0,0);
    	CUR_wprintw(CUR_stdscr,"The window size is only %d columns by %d rows.",CUR_COLS,CUR_LINES);
    	CUR_wmove(CUR_stdscr,1,0);
    	CUR_wprintw(CUR_stdscr,"VISION needs a window size of at least 80 by 24.");
    	CUR_wmove(CUR_stdscr,2,0);
    	CUR_wprintw(CUR_stdscr,"Resize the window and/or change the terminal type");
    	CUR_wmove(CUR_stdscr,3,0);
    	CUR_wprintw(CUR_stdscr,"to remedy the problem and then restart VISION.");
    	CUR_wmove(CUR_stdscr,4,(CUR_COLS-22)/2);
    	CUR_wprintw(CUR_stdscr,"Hit any key to quit...");
    	CUR_touchwin(CUR_stdscr);
    	CUR_wrefresh(CUR_stdscr);
    	CUR_wgetch(CUR_stdscr);
    	WindowTooSmall = TRUE;
    	return;
    }
    else if( (CUR_COLS > CUR_maxScrCols) || (CUR_LINES > CUR_maxScrLines) )
    {
    	CUR_werase(CUR_stdscr);
    	CUR_wmove(CUR_stdscr,0,0);
    	CUR_wprintw(CUR_stdscr,"The window size is %d columns by %d rows.",CUR_COLS,CUR_LINES);
    	CUR_wmove(CUR_stdscr,1,0);
    	CUR_wprintw(CUR_stdscr,"VISION needs a window size less than or equal to %d columns by %d rows.",CUR_maxScrCols,CUR_maxScrLines);
    	CUR_wmove(CUR_stdscr,2,0);
    	CUR_wprintw(CUR_stdscr,"Resize the window and/or change the terminal type");
    	CUR_wmove(CUR_stdscr,3,0);
    	CUR_wprintw(CUR_stdscr,"to remedy the problem and then restart VISION.");
    	CUR_wmove(CUR_stdscr,4,(CUR_COLS-22)/2);
    	CUR_wprintw(CUR_stdscr,"Hit any key to quit...");
    	CUR_touchwin(CUR_stdscr);
    	CUR_wrefresh(CUR_stdscr);
    	CUR_wgetch(CUR_stdscr);
    	WindowTooSmall = TRUE;
    	return;
    }
    else
    	WindowTooSmall = FALSE;

    mainmenu ();
}


/**************************************************************************
 *			 PARENT'S INTERRRUPTS                             *
 **************************************************************************/

PrivateFnDef void cleanupCurses () {
    if (InitializedCurses)
    {
        CUR_erase();
        CUR_touchwin(CUR_stdscr);
        CUR_refresh();
        CUR_resetty ();
        CUR_endwin ();
        STD_cleanupTerminal();
    }
}

PrivateFnDef void cleanupRs () {
/*****  
 * 'AllDone' is set so that the parent will ignore the "death of child"
 *  signal that the 'kill' signal will generate.
 *  See the SIGCLD case in the parentSignalHandler routine.
 *****/
    AllDone = TRUE;
    kill (RSpid, SIGTERM); 
}

PrivateFnDef void cleanup () {
/*****  Cleanup Printer ... *****/
    cleanupPrinters ();

/*****  Cleanup Curses ... *****/
    cleanupCurses ();

/*****  Kill the Research System child ... *****/
    cleanupRs ();
}

PrivateFnDef void handleQuickTermination () {
    cleanupRs ();
    exit (NormalExitValue);
}

PrivateFnDef void handleTermination (
    int				errcode
)
{
    cleanup ();
    if( errcode )
	ERR_displayNoCurses(errcode);
    exit (NormalExitValue);
}


#if defined(sun)
PrivateFnDef void seriousSegvHandler (int xSignal, int code, struct sigcontext *scp)
#else
PrivateFnDef void seriousSegvHandler (int xSignal)
#endif
{
/*****  
 *  We get here if we get a 2nd segmentation fault while handling the
 *  first.  
 *****/
    /*if (DEBUG) RS_fprintf (
	"seriousSegvHandler: signal = %d, code = %d\n", sig, code
    );*/
    longjmp (give_up, 1);
}

#if 0
#if defined(_AIX)
/* useful for debuggin on AIX. Not needed in everyday life ...*/
PublicFnDef void RS_DumpWindowStruct (CUR_WINDOW *win)
{
  if (DEBUG)
  {
    RS_fprintf ("Dump of %x:\n", win);
    RS_fprintf (
		"\t_cury:%d,_curx:%d,_maxy:%d,_maxx:%d,_begy:%d,_begx:%d\n",
      win->_cury,win->_curx,win->_maxy,win->_maxx,win->_begy,win->_begx);
    RS_fprintf (
		"\t_flags:%x,_yoffset:%d,_clear:%d,_leave:%d,_immed:%d\n",
      win->_flags,win->_yoffset, win->_clear, win->_leave,win->_immed);
    RS_fprintf (
		"\t_padwin:%x,_firstch:%x,_lastch:%x,_tmarg:%d,_bmarg:%d\n",
	win->_padwin,win->_firstch,win->_lastch,win->_tmarg,win->_bmarg);
    RS_fprintf (
	       "\t_scroll:%d,_use_idl:%d,_use_keypad:%d,_use_idc:%d,_attrs:%x\n",
     win->_scroll,win->_use_idl,win->_use_keypad,win->_use_idc,win->_attrs);
    RS_fprintf (
		"\t_bkgd:%d,_delay:%d,_ndescs:%d,_parx:%d,_pary:%d,_parent:%x\n",
       win->_bkgd,win->_delay,win->_ndescs,win->_parx,win->_pary,win->_parent);
  }
}
#endif
#endif

/*---------------------------------------------------------------------------
 * The following chart shows the signals which the frontend is interested
 * in catching, all of the others are set to SIG_IGN.
 *
 * SPECIAL
 * SIGNALS	VAX	HP	PURPOSE & ACTION
 * -------	---	--	------------------------------------------
 * SIGHUP	x	x	hangup 	- terminate
 * SIGQUIT	x	x	quit 	- terminate
 * SIGTERM	x	x	software terminate - terminate
 *
 * SIGSEGV	x	x	access violation - terminate (carefully)
 * SIGBUS	x		access violation - terminate (carefully)
 *
 * SIGCLD		x	death of child - backend may have died - try
 *				to restart it
 *---------------------------------------------------------------------------
 */

#if defined(sun)
PrivateFnDef void parentSignalHandler (int sig, int code = 0, struct sigcontext *scp = 0)
#else
PrivateFnDef void parentSignalHandler (int sig)
#endif
{
    int deceasedStatus;
    pid_t deceased;
    struct sigvec vec;

    /*if (DEBUG) RS_fprintf (
	"parentSignalHandler: signal = %d, code = %d\n", sig, code
    );*/
    
    switch (sig) 
    {
#if defined(__hp9000s700) || defined(_AIX)
    case SIGTSTP:
	if (InitializedCurses)
	{


	  /* the following refresh and flush are needed by AIX curses
	     to make sure the terminal is in the right state when Vision
	     is suspended: Not in Reverse video mode, and with the cursor
	     in the place we want it. HP doesn't require either action */
	    CUR_wrefresh (CUR_curscr);
	  /* move cursor to lower left corner of screen */
	    puts (tparm (cursor_address, CUR_LINES - 1, 0));
	    fflush (stdout);

	    CUR_saveterm ();
	    CUR_resetterm ();
	    kill (0, SIGSTOP);

	    CUR_fixterm ();
	    CUR_noecho ();
	    CUR_nonl ();
	    CUR_cbreak ();

	    CUR_clearok (CUR_curscr);
	    CUR_wrefresh (CUR_curscr);

	}
	else kill (0, SIGSTOP);
	break;
#endif

    case SIGCLD:
	/*****
	 * Note that the child that died may not be the research system. 
	 * It could be something from a subshell.
	 *****/

	/*****  If the dead child is NOT the research system, continue... ***/
	deceased = wait (&deceasedStatus);
	if (deceased != RSpid)
	{
	/*****
	 * When the editor starts a subShell it issues a pause and waits for 
	 * it to exit.  When a subshell exits it generates a SIGCLD interrupt.
	 * This handler for SIGCLD terminates the zombie subshell process by 
	 * doing a wait().  Then, to stop the pause we will not allow the
	 * pause to be restarted.
	 *****/

	    PauseShouldBeRestarted = FALSE;
	    break;
	}

	if (AllDone) 
	    exit (NormalExitValue);

	/** The rs died so the parent needs to restart it and the 
	    editor program needs to jmp into the top of its loop.
	    Otherwise the editor program hangs because it is waiting 
	    for the rs to reply to its last command.  **/

    	if (RS_jmpToUser == TRUE)
	{
	    ERR_displayStr(" *** Research System has died ***", TRUE);
	    close(RSfdin);
	    close(RSfdout);
	    StartBackend(TRUE);
	    RSstdin =  fdopen (RSfdin,  "w");
	    RSstdout = fdopen (RSfdout, "r");
	    RS_nodelayOutput (FALSE);
	    RS_readOutput(NULL, NULL);
            if (ASubshellHasBeenSpawned)
		PauseShouldBeRestarted = TRUE;
	    else
		longjmp(RS_userjmpbuf, 0);
	}
	else 	/**** just beginning can't recover ****/
	{  
	    /*** Don't try to display an error, the error window 
	     *** might not exist yet. 
	     ***/
	    handleTermination (0);
	}
	break;

    case SIGHUP:
	ERR_displayError (ERR_HangupSig);
	handleTermination (0);
	break;
	
    case SIGQUIT:
	ERR_displayError (ERR_QuitSig);
	handleTermination (0);
	break;
	
    case SIGSEGV:
	/*****  
	 *  Setup a new handler for SIGSEGV ...
	 *  'seriousSegvHandler' will be called if a second segmentation
	 *  fault occurs while processing this one.
	 *****/
	
	vec.sv_handler = (STD_SignalHandlerType)seriousSegvHandler;
        vec.sv_mask = (unsigned long)0;
	vec.sv_onstack = 0;

	STD_sigvector (SIGSEGV, &vec, (struct sigvec *)0);
	STD_sigsetmask (
	    STD_sigsetmask (~0L) &~ (1L << (SIGSEGV - 1))
	); /* unmask SIGSEGV */

	/*****  Try to do a full cleanup and then exit the program ... *****/
	if (setjmp (give_up) == 0)
	{
	    /*
	     * ERR_displayError (ERR_SegVSig);
	     * Don't push your luck, cleanup is more important.
	     */
	    handleTermination (ERR_SegVSig);
	}
    
	/*****  ... otherwise just terminate the child and exit ... *****/
	handleQuickTermination ();
	break;
	
    case SIGUSR2:
	ERR_displayError (ERR_User2Sig);
	/** this toggles the userSystem option **/
/*	RS_userSystem = !RS_userSystem;  */
	break;
	
    case SIGTERM:
	ERR_displayError (ERR_TermSig);
  	handleTermination (0);
	break;
	
    case SIGINT:
	ERR_displayError (ERR_IntrSig);
	break;
	
    case SIGILL:
	ERR_displayError (ERR_InstrSig);
	break;
	
    case SIGTRAP:
	ERR_displayError (ERR_TrapSig);
	break;
	
    case SIGFPE:
	ERR_displayError (ERR_FloatSig);
	break;
	
    case SIGBUS:
	ERR_displayError (ERR_BusSig);
	break;
	
    case SIGSYS:
	ERR_displayError (ERR_ArgSig);
	break;
	
    case SIGPIPE:
	ERR_displayError (ERR_PipeSig);
	break;
	
    case SIGALRM:
	ERR_displayError (ERR_AlarmSig);   
	break;
	
    case SIGUSR1:
	ERR_displayError (ERR_User1Sig);
	break;

#ifndef __APPLE__	
    case SIGPWR:
	ERR_displayError (ERR_PowerSig);
	break;
#endif
	
    case SIGVTALRM:
	ERR_displayError (ERR_VAlarmSig);
	break;
	
    case SIGPROF:
	ERR_displayError (ERR_PAlarmSig);
	break;
	
    case SIGIO:
	ERR_displayError (ERR_SIGIOSig);
	break;
	
#ifndef __APPLE__
    case SIGWINDOW:
	ERR_displayError (ERR_WinSig);
	break;
#endif
	
    default:
	ERR_displayError (ERR_UndefSig);
	break;
    }
}


PrivateFnDef void RS_callSignalHandler (
    int				sig
)
{
    if( sig <= 0 )
    	handleTermination(sig);
    else
	parentSignalHandler (sig);
}

/*****  Internal routine to set up handlers for the trapable signals.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 ******/
PrivateFnDef void setupParentSignals () {
    int i;
    struct sigvec vec, ivec, lvec;

/*****  
 *  Special Signals are the signals that the parent is interested in.
 *  All of the others are set to SIG_IGN.  Each of the special signals
 *  should have a case in 'parentSignalHandler' which deals with them.
 *****/
#if defined(__hp9000s700) || defined(_AIX)
#define isaSpecialSignal(sig) (\
    sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM ||\
    sig == SIGILL || sig == SIGBUS  || sig == SIGSEGV ||\
    sig == SIGCLD || sig == SIGTSTP\
)

#define isanIgnoredSignal(sig) (\
    sig == SIGINT || sig == SIGUSR1 || sig == SIGUSR2\
)

#else
#define isaSpecialSignal(sig) (\
    sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM ||\
    sig == SIGILL || sig == SIGBUS  || sig == SIGSEGV ||\
    sig == SIGCLD\
)

#define isanIgnoredSignal(sig) (\
    sig == SIGINT || sig == SIGUSR1 || sig == SIGUSR2\
)

#endif

    vec.sv_handler	= (STD_SignalHandlerType)parentSignalHandler;
    vec.sv_mask		= ~(STD_maskType)0;
    vec.sv_onstack	= 0;

    ivec.sv_handler	= (STD_SignalHandlerType)SIG_IGN;
    ivec.sv_mask	= ~(STD_maskType)0;
    ivec.sv_onstack	= 0;

/*****  
 * Set all signals to 'ignore' except for a few special signals.
 * These special signals will only be 'caught' if they have not
 * previously been set to 'ignore'. (Before the start of the front end.)
 *****/
    for (i = 1; i < NSIG; i++)
    {
	/*****  Get the current value for this signal ... *****/
	STD_sigvector (i, NilOf (struct sigvec *), &lvec);

	/*****  If its not already set to ignore ... *****/
	if (lvec.sv_handler == (STD_SignalHandlerType)SIG_IGN);
		/***** Catch the special signals ... *****/
	else if (isaSpecialSignal (i)) STD_sigvector (
	    i, &vec, NilOf (struct sigvec *)
	);
		/***** Ignore the ignored signals ... *****/
	else if (isanIgnoredSignal (i)) STD_sigvector (
	    i, &ivec, NilOf (struct sigvec *)
	);
		/***** And do nothing with the rest.  *****/
    }
#undef isaSpecialSignal
#undef isanIgnoredSignal
}

/*---------------------------------------------------------------------------
 *****  Public routine to save the current values of the signal handlers
 *****  in the global array 'parentSignalValues'.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 ******/
PrivateFnDef void RS_saveParentSignals () {
    int i;

/*****  Get the current values for all of the posible signals ... *****/
    for (i = 1; i < NSIG; i++)
    {
	/*****  Get the current value for this signal ... *****/
	STD_sigvector (i, NilOf (struct sigvec *), &parentSignalValues[i - 1]);
    }
}

/*---------------------------------------------------------------------------
 *****  Public routine to restore the values of the signal handlers that
 *****  have been previously saved by the 'saveParentSignals' routine.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 ******/
PrivateFnDef void RS_restoreParentSignals () {
    int i;

/*****  Get the current values for all of the posible signals ... *****/
    for (i = 1; i < NSIG; i++)
    {
	/*****  Restore the saved value for this signal ... *****/
	STD_sigvector (i, &parentSignalValues[i - 1], NilOf (struct sigvec *));
    }
}


PrivateFnDef void readCompanyName () {
    char buffer[81];
    int i, len, padding;
    FILE *fd;

    for (i = 0; i < RS_NameLen; i++)
	RS_CompanyName[i] = ' ';
    RS_CompanyName[RS_NameLen] = '\0';
    if( NULL == (fd = fopen(RS_NameFile, "r")) )
    	return;
    else if( NULL == fgets(buffer, 80, fd) )
    {
    	fclose(fd);
    	return;
    }
    else
    {
    	buffer[80] = '\0';
	len = strlen(buffer);
	if (buffer[len - 1] == '\n')
	{
	    buffer[len - 1] = '\0';
	    len--;	    
	}
	if (len < RS_NameLen)
	    padding = (RS_NameLen - len) / 2;
	else
	{
	    len = RS_NameLen;
	    padding = 0;
	}
	strncpy(&RS_CompanyName[padding], buffer, len);
	fclose(fd);
    }
}


PrivateFnDef void getDate () {
    long clock;
    
    time(&clock);
    RS_Date = localtime(&clock);
}

PrivateFnDef void displayLogo () {
    char logoname[80], *s;
    int c;
    FILE *fd;

    if( (s = getenv("VisionLibrary")) == NULL )
	return;
    strcpy(logoname,s);
    strcat(logoname,"/logo");
    if (NULL != (fd=fopen(logoname, "r")))
    {
        while (EOF != (c=fgetc(fd)))
    	    putchar(c);
	fclose(fd);
    }
}


/******************************
 ***********  MAIN  ***********
 ******************************/

#ifdef __APPLE__
PrivateFnDef char *cuserid (char *pResult) {
    char *pGetLoginResult = getlogin ();
    if (!pGetLoginResult)
	return NULL;

    strcpy (pResult, pGetLoginResult);
    return pResult;
}
#endif

PublicFnDef int main (
    int				argc,
    char *			argv[]
)
{
    int i;
    char buf[256], *home;

    displayLogo();

    /*** add a '-E' argument to argv to specify to the research system that it
         is being called from the user interface ***/

    for (i = 1; i < argc; i++)
	RSargs[i] = argv[i];
    RSargs[i++] = const_cast<char*>("-E");
    RSargs[i] = NULL;


/***** Acquire the options *****/

    GOPT_AcquireOptions (argc, argv);
    RS_userSystem = !GOPT_GetSwitchOption ("Developer");
    DEBUG = GOPT_GetSwitchOption("Debug");
    Administrator = GOPT_GetSwitchOption("Administrator");
    BackendPath = GOPT_GetValueOption ("WhichRS");

    if( NULL == getenv("VisionAdmPrivate") )
	RS_AdministratorPrivate = FALSE;
    else
	RS_AdministratorPrivate = TRUE;

    if (DEBUG)
    {
	if( (RS_outfile = fopen ("debug.log", "w")) == NULL )
	{
	    home = getenv("HOME");
	    if( home == NULL )
		DEBUG = FALSE;
	    else
	    {
		strncpy(buf,home,sizeof(buf)-1);
		strncat(buf,"/debug.log", sizeof(buf)-strlen(buf)-1);
		if( (RS_outfile = fopen (buf, "w")) == NULL )
		    DEBUG = FALSE;
	    }
	}
    }

/***** determine if 'rexec' should be used to run the backend, ...  *****/
    BackendPath += strspn (BackendPath, " \t");
    char const* const pHostBreak = strchr (BackendPath, ':');
    if (pHostBreak)
    {
	size_t sBackendHost = (size_t)(pHostBreak - BackendPath);

	if (0 == sBackendHost)
	    gethostname (BackendHost, sizeof (BackendHost));
	else
	{
	    strncpy (BackendHost, BackendPath, sBackendHost);
	    BackendHost[sBackendHost] = '\0';
	    BackendPath += sBackendHost;
	}
	if (BackendPath[0] == ':')
	    BackendPath++;

	if (0 == strlen (BackendPath))
	    BackendPath = "batchvision";

	cuserid (BackendUser);
	printf ("Username on %s [%s]: ", BackendHost, BackendUser);
	fflush (stdout);
	fgets (buf, sizeof(buf), stdin);
	if (strlen (buf + strspn (buf, " \t")) > (size_t)0)
	    strncpy (BackendUser, buf + strspn (buf, " \t"), sizeof(BackendUser));

	printf ("Password for %s on %s: ", BackendUser, BackendHost);
	fflush (stdout);
	CUR_initscr ();
	CUR_noecho ();
	fgets (BackendPass, sizeof(BackendPass), stdin);
	CUR_echo ();
	CUR_endwin ();

	BackendIsLocal = FALSE;
    }
    else BackendIsLocal = TRUE;

/***** try to access the BackendPath in execute mode *****/
/*****
 *
 *  >>>  ... but only if BackendPath appears to name a specific file, as evidenced
 *  >>>  by the presence of a '/' character in its value.  Without this test,
 *  >>>  all $PATH resolved backends (e.g., the frontend's default of 'batchvision'
 *  >>>  specified by "WhichRS" in this project's 'main.i' file) are rejected by the
 *  >>>  call to 'access' unless suitably named executable file exists in the current
 *  >>>  directory.  This change gives 'execvp' the shot it deserves for finding and
 *  >>>  successfully starting those backends
 *
 *****
 *****/
    RSargs[0] = const_cast<char*>(BackendPath);
    if (BackendIsLocal && strchr (BackendPath, '/') && access (BackendPath, 1) != 0)
    {
    	fprintf (stderr, "ERROR - cannot run the program: %s\n", BackendPath);
    	exit (ErrorExitValue);
    }

    readCompanyName();
    getDate();


/***** Begin ... *****/

    RS_saveParentSignals ();

    StartBackend (FALSE);

    /*** Don't set the parents signals until after the RS is forked ***/
    setupParentSignals (); 

    RSstdin =  fdopen (RSfdin,  "w");
    RSstdout = fdopen (RSfdout, "r");
    
    if (RSstdin == 0 || RSstdout == 0)	    
	STD_syswarn ("setting RSstdin and RSstdout");

    KEY_beginScriptReadF("vision.startup");

    execute_user ();
    
    if (DEBUG) fclose(RS_outfile);
    if( KEY_scriptWrite )
	KEY_beginScriptWrite();
    cleanup();
    exit(NormalExitValue);
}


PublicFnDef int RS_getValue (
    char *			value
)
{
    char buffer[RS_MaxLine + 1];
    int empty, error;
    
    empty = TRUE;    
    error = FALSE;
    while (RS_readLine(buffer, RS_MaxLine))
    {
	if (empty && (!isBlank(buffer)))
	{
	    empty = FALSE;
	    strcpy(value, buffer);
	}
	if (0 == strncmp(buffer, ">>>", 3) || 
	   (0 == strncmp(buffer, "Type:", 5)))
	    error = TRUE;
    }
    if (empty || error) strcpy(value, "NA");

    return (FALSE);
}

PrivateVarDef int	doPC = FALSE;

#if 0
PrivateFnDef void systemPrint(
    void
)
{
    char buffer[RS_MaxLine+1], filename[80];
    FILE *fd, *fopen();

    ERR_displayStr("Generating report, please wait...",FALSE);
    sprintf(filename, "/tmp/vision%d", getpid());

    fd = fopen(filename, "w");

    while (RS_readLine(buffer, RS_MaxLine))
	fputs(buffer, fd);

    fclose(fd);

/***    sprintf(buffer, "lp -c -s -oc < %s", filename);	***/
/*    sprintf(buffer, "%s < %s", PRINT_Command, filename);*/
    sprintf(buffer, "/bin/csh -if -c \"%s %c %s\"", 
	PRINT_Command, 
	(doPC ? ' ' : '<'),
	filename);

    RS_system(buffer);
    remove(filename);
    CUR_werase(ERR_Window);
    CUR_clearok(CUR_curscr);	/*** need to repaint the screen ***/
    CUR_wrefresh(CUR_curscr);
}
#endif

#if 0
PrivateFnDef void pcPrint(
    void
)
{
    if( isBlank(PRINT_Command) )
    {
	ERR_clearMsg();
	return;
    }
    ERR_displayPause("Please Turn On PC Printer");
    doPC = TRUE;
    systemPrint();
    doPC = FALSE;

 #if 0
    char buffer[RS_MaxLine];
    
    ERR_displayPause("Please Turn On PC Printer");
    ERR_displayStr("Printing report, please wait...",FALSE);

    putchar('\020');	/*** toggle printer ***/

    while (RS_readLine(buffer, RS_MaxLine))
	puts(buffer);

    putchar('\020');	/*** toggle printer ***/

    CUR_werase(ERR_Window);
    CUR_clearok(CUR_curscr);	/*** need to repaint the screen ***/
    CUR_wrefresh(CUR_curscr);
 #endif
}
#endif


PublicFnDef int RS_printOutput (
    PAGE *			page
)
{
    char buffer[RS_MaxLine+1],
         filename[80];
    FILE *fd;
#if 0
    int c;
    
    c = ERR_promptForChar("Print to System or PC (s/p) ? ", "sSpP");
	
    if (c == 'p' || c == 'P')    
	pcPrint();
    else
    if (c == 's' || c == 'S')    
	systemPrint();
#endif
    print (page,-1);

    if (isBlank (PRINT_Command.command))
        {
	while (RS_readLine (buffer, RS_MaxLine))
            ;
	return (FALSE);
        }

    sprintf (filename, "/tmp/vision%d", getpid());

    if ((fd = fopen (filename, "w")) == NULL)
        {
	while (RS_readLine (buffer, RS_MaxLine))
            ;
	return (FALSE);
        }

    while (RS_readLine (buffer, RS_MaxLine))
	fputs (buffer, fd);

    fclose (fd);
    
#if 0
    if (0 == strncmp(PRINT_Command, "pc", 2))
	pcPrint();
    else
    if (PRINT_Lines)
	systemPrint();
#endif

    printFile (filename);
    remove (filename);
    ERR_clearMsg ();
    EDIT_displayErrorMessage ();
    CUR_clearok (CUR_curscr);	/*** need to repaint the screen ***/

    return (FALSE);
}
