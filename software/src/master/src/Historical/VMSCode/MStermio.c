/*****  Machine Specific Terminal IO *****/

#ifdef FRONTEND

#include <stdio.h>
#include "stdoss.h"
#include "MSsupport.h"
#include "SItermio.h"
#include <signal.h>
#include "SIsig.h"

/************************************************************
 *****  File (and input) reading with NODELAY           *****
 ************************************************************/
#ifdef VMS

#include file
#include ssdef

PublicVarDecl FILE	*RSstdout;
PublicVarDecl int	RSfdout, DEBUG;

PrivateVarDef int	NoDelay[2] = {0, 0};

PublicFnDef char *
VMS_fgets(str, maxl, fp)
char	*str;
int	maxl;
FILE	*fp;
{
	int	idx, error = FALSE;
	char	*s = NULL, *fgets();

	if( fp == RSstdout )		idx = 0;
	else				return( NULL );

#if 1
	if( NoDelay[idx] )
	{
	    /*** This method is highly unreliable but at least it is broken
	     *** on the safe side (i.e. it may say there are no characters
	     *** when there are.  If It said there were characters when there
	     *** were none, then the process would hang.)
	     *** The first attempt (after the #else) set up the pipe()'s using
	     *** NODELAY.  This would allow fgets to return immediately,
	     *** however, you could never turn off nodelay afterwards.  So you
	     *** were forced to loop until you read something.  Also, this old
	     *** method seemed to lose characters in the pipe.
	     ***/
	    if( (*fp)->_cnt > 0 )
	    {
		s = fgets(str, maxl, fp);
	    }
	    return( s );
	}
	while( (s = fgets(str, maxl, fp)) == NULL )
	    ;
	return( s );
#else
	if( NoDelay[idx] )
	{
		return( fgets(str, maxl, fp) );
	}

	while( s == NULL ) {
		/*
		if( ferror(fp) ) {
			error = TRUE;
			break;
		}
		if( feof(fp) ) {
			error = TRUE;
			break;
		}
		*/
		s = fgets(str, maxl, fp);
	}

	if( error )
		return( NULL );
	else
		return( s );
#endif
}

PublicFnDef int
VMS_fcntl(fd, cmd, arg)
int	fd, cmd, arg;
{
	if( cmd == F_GETFL )
		return( 0 );

	if( cmd != F_SETFL )
		return( 0 );

	if( arg & O_NDELAY ) {
		if( fd == RSfdout )		NoDelay[0] = TRUE;
	}
	else {
		if( fd == RSfdout )		NoDelay[0] = FALSE;
	}

	return( 0 );
}

/*****  Originally from the file ttyqio.c  *****/
/* #module  ttyqio */ 	/* HP compiler doesn't like this */

/****************************
 ****************************
 *****			*****
 *****  Possible  Bugs  *****
 *****			*****
 ****************************
 ****************************/
/*
  ttyqio_$enable_ast()  -  the ast is given a acces mode of 3 (user mode)
                           so don't assume it inherits the mode of the 
                           caller's process
  ttyqio_$disable_ast() -  the status returned is 0, which is not a standard
                           return status.  I can't find any mention of why
                           this status is returned in the documentation
*/


/***********************************
 ***********************************
 *****			       *****
 *****  Standard Declarations  *****
 *****			       *****
 ***********************************
 ***********************************/

#include ctype
#include iodef
#include ttdef
#include tt2def

/**************************************
 **************************************
 *****				  *****
 *****  System Module Referenced  *****
 *****				  *****
 **************************************
 **************************************/

/* #include systruct   still need to add this */


/*****************************************
 *****************************************
 *****				     *****
 *****  Local Structure Definitions  *****
 *****				     *****
 *****************************************
 *****************************************/

struct short_terminator
       {
         int            zero;
         unsigned int  char_mask;
       };


/*****************************
 *****************************
 *****			 *****
 *****  Local Constants  *****
 *****			 *****
 *****************************
 *****************************/

#define sys_$usrmode  3


/****************************
 ****************************
 *****			*****
 *****  Module Globals  *****
 *****			*****
 ****************************
 ****************************/

static int ttyqio_$terminal_chan = 0;


/******************************************
 ******************************************
 *****				      *****
 *****  Globally Accesable Variables  *****
 *****				      *****
 ******************************************
 ******************************************/

unsigned int ttyqio_$status;


/***************************
 ***************************
 *****		       *****
 *****  Common Macros  *****
 *****		       *****
 ***************************
 ***************************/

#define $init(dsc,len)\
    {dsc.dsc$w_length = 1; dsc.dsc$a_pointer = malloc (len);}

#define $address(dsc) dsc.dsc$a_pointer

#define $length(dsc) dsc.dsc$w_length

#define $exit return(-1)

#define STDIN_FD 0



/**************************
 **************************
 *****		      *****
 *****  AST Routines  *****
 *****		      *****
 **************************
 **************************/

int ttyqio_$enable_ast(routine,bitmask)
/****** Routine to install a function as an AST for control chars
 *
 * Arguments :
 *        func    - a pointer to the function to be installed
 *        bitmask - an unsigned int specifing the control keys to
 *                  trap on. Where bit 0 is null, bit 1 is ctrl-a , ect..
 *                  Control keys that act on the driver should not be set.
 * Returns  :
 *      TRUE if the install was succesful or FALSE if there was an
 *      error.
 * 
 * Side effects :
 *      Sets ttyqio_$status.
 * Comments : 
 *           The AST will not be called if there is a C read pending;
 *       however if there is a qio read it will(strange).  Further 
 *       the AST remains active until process termination or it is disabled
 *       with ttyqio_$disable_ast or ttyqio_$enable_ast is called again -
 *       so it is not neccesary to disable ast's before enableing a new
 *       ast. 
 *****/
unsigned int bitmask;
int (*routine) ();
{ 
short iosb[4];
int chan = 0;
struct short_terminator the_char;
int func = IO$_SETMODE | IO$M_OUTBAND | IO$M_TT_ABORT /*| IO$M_INCLUDE*/;
   the_char.char_mask = bitmask;
   the_char.zero = 0;
   chan = ttyqio_$get_term_chan();
   if (chan == FALSE) 
       return(FALSE);
   /*  sys$qio - p1 = address of function to install
                 p2 = structure containing the bit mask
                 p3 = acess mode 
   */
   ttyqio_$status = sys$qio(0, chan, func, iosb, 0, 0,
                       routine,&the_char, sys_$usrmode, 
                       0, 0, 0);
  
   if (ttyqio_$status == SS$_NORMAL ) 
        return(TRUE);
   else 
        return(FALSE);      
} 


int ttyqio_$disable_ast()
/***** Routine to disable the AST for terminal IO
 *
 * Synopsis:
 *      int ttyqio_$disable_ast();
 *
 * Function:
 *      To disable the ast set by ttyqio$_enable_ast.
 * Returns:
 *      True if the qio service completed normally false otherwise.
 * Side effects:
 *      Sets ttyqio_$status.
 *****/
{
short iosb[4];
int func = IO$_SETMODE | IO$M_OUTBAND | IO$M_TT_ABORT /*| IO$M_INCLUDE*/;
   if (!ttyqio_$get_term_chan())
      return(FALSE);
   ttyqio_$status = sys$qio(0, ttyqio_$terminal_chan, func, iosb, 0, 0,
                             0, 0, 0, 0, 0, 0);
   if ((ttyqio_$status != SS$_NORMAL) && (ttyqio_$status != 0))
      return(FALSE); 
   else
      return(TRUE);
}


/**************************************************
 **************************************************
 *****					      *****
 *****  Terminal Channel Assignment Routines  *****
 *****					      *****
 **************************************************
 **************************************************/

int ttyqio_$assign_term_chan()
/***** Routine to assign a channel to the TTY
 *
 * Function:
 *      Assign to the TTY a channel number or return the assign 
 *      channel.
 * Returns:
 *      The channel number if the assign was succesful or 
 *      FALSE if there was any type of error
 * Side effects :
 *      Sets ttyqio_$status.
 *****/
{
$DESCRIPTOR (devname,"TT");
int chan;
   if (ttyqio_$terminal_chan)
      return(ttyqio_$terminal_chan);
   ttyqio_$status = sys$assign(&devname, &chan,0,0);
   if (ttyqio_$status != SS$_NORMAL) 
     return(FALSE);
   else
     return(ttyqio_$terminal_chan = chan);
}


int ttyqio_$get_term_chan()
/***** Routine to get the channel assigned to the terminal
 *
 * Function:
 *      Get the current contents of ttyqio_$terminal_chan or assign  a channel
 *      to the terminal if one does not currently exist.
 * Returns:
 *      The either a channel number or false if one can not be assigned
 * Side effects :
 *      Sets ttyqio_$status if a channel was not set.
 *****/
{
   if (ttyqio_$terminal_chan)
      return(ttyqio_$terminal_chan);
   else 
      return(ttyqio_$assign_term_chan());
}


/******************************
 ******************************
 *****			  *****
 *****  Character Reader  *****
 *****			  *****
 ******************************
 ******************************/

char ttyqio_$qio_c(control)
/*****  Routine to read a single character from the keyboard as it is typed:
 *
 * Argument:
 *     control - control bitmap :
 *                      bit 0 - set => echo character when typed.
 *                          1 - set => do not wait for character.
 *                          2 - set => pass all characters including ^Z & ^C.
 *
 * Returns :
 *     The value of the ASCII character which was typed as the value of this 
 *     function.
 * Side effects :
 *     Sets ttyqio_$status to the return status from the qio call.
 *****/
int control;
{
char input_char;
int func;
short iosb[4];
static int
    isnt_interactive,
    undetermined_if_interactive = TRUE;

    if (undetermined_if_interactive) {
	isnt_interactive = !isatty (STDIN_FD);
	undetermined_if_interactive = FALSE;
    }

    if (isnt_interactive) {
	if (read (STDIN_FD, &input_char, 1) != 1)
	    $exit;
	switch (input_char) {
	    case '\n':
		input_char = '\r';
		break;
	}
	return input_char;
    }

   if ((!ttyqio_$terminal_chan) & (ttyqio_$assign_term_chan()))
      $exit;
   func = IO$M_NOFILTR |
             (control & 1 ? 0 : (IO$M_TRMNOECHO | IO$M_NOECHO)) |
             (control & 2 ? IO$M_TIMED : 0) |
             (control & 4 ? IO$_TTYREADPALL : IO$_READVBLK);
   for(;;)
   {
	input_char = -1;
	ttyqio_$status = sys$qiow(0, ttyqio_$terminal_chan, func, iosb, 0, 0,
                             &input_char, 1, 0, 0, 0, 0);
        if (ttyqio_$status != SS$_NORMAL)
	    $exit;			/* fatal exit */

	if( (input_char !=  3) &&	/* Control-C */
	    (input_char != 25) &&	/* Control-Y */
	    (input_char != 28) )	/* Control-\ */
		return input_char;
	VAXints(input_char);
   }
}

#define BufEmpty	-999
static int CBuf = BufEmpty;

char ttyqio_$get_vtxxxc (control)
/*****  Routine to get and decode a single VTxxx key stroke:
 *
 * Argument:
 *	control	- see the description of 'io_$qio_c'.
 *
 * Returns:
 *	The character/keystroke that was typed as the result of this function
 *	and the io status as the value of the extern variable 'io_$status'.
 *
 *****/
int control;
{
    static char arrtbl[5] = "ABCD";		      /* Which of these	*/
    static char funtbl[19] = "pqrstuvwxylmnPQRSM";    /* are caps???	*/
    static int vt2tbl[] = {	0,		/* Keycode NULL # 0	*/
				-21,		/* Keycode FIND		*/
				-22,		/* Keycode INSERT HERE	*/
				-23,		/* Keycode REMOVE	*/
				-24,		/* Keycode SELECT	*/
				-25,		/* Keycode PREV SCREEN	*/
				-26,		/* Keycode NEXT SCREEN	*/
				0,0,0,0,0,0,0,0,0,0, /* Nulls 7-16	*/
				-6,		/* F6			*/
				-7,		/* F7			*/
				-8,		/* F8			*/
				-9,		/* F9			*/
				-10,		/* F10			*/
				0,		/* Keycode NULL # 22	*/
				-11,		/* F11			*/
				-12,		/* F12 			*/
				-13,		/* F13 			*/
				-14,		/* F14			*/
				0,		/* Keycode NULL # 27	*/
				-15,		/* F15 (HELP)		*/
				-16,		/* F16 (DO)		*/
				0,		/* Keycode NULL # 30	*/
				-17,		/* F17			*/
				-18,		/* F18			*/
				-19,		/* F19			*/
				-20 };		/* F20			*/
    static char c, vt2xx[4];
    static int i,p;

    if (CBuf != BufEmpty) {
	c = (char)(CBuf & 0x000000ff);
	CBuf = BufEmpty;
	return(c);
    }
    c = ttyqio_$qio_c (control);		/* Get initial char	*/
    if (c != 27 && c != -101 && c != -113)	/* Return invalid char	*/
	return (c);				/* for esc seq start	*/
    if (c == -101) goto csi_sequence;		/* Received <ESC>[	*/
    if (c == -113) goto ss3_sequence;		/* Received <ESC>O	*/
    c = ttyqio_$qio_c (control & 0xfffffffd);
    if (c != 'O' && c != '[') {
	CBuf = (int)(c & 0x000000ff);
	return (27);
    }
    if (c == 'O')  goto ss3_sequence;

csi_sequence:
    c = ttyqio_$qio_c (control & 0xfffffffd);
    if ((p = strcspn (arrtbl, &c)) >= 4) {
	vt2xx[0] = c;
	for (p = 1; p < 4; p++) {
	    if (!isdigit (c = vt2xx[p] = ttyqio_$qio_c(control))) {
		if (c != '~')
		    return (c);
		vt2xx[p] = NULL;
		break;
	    }
	}
	return ((p = atoi(vt2xx)) > 34 ? p : vt2tbl[p]);
    }
    else
	return ((p + 27) * -1);

ss3_sequence:
    c = ttyqio_$qio_c (control & 0xfffffffd);
    if ((p = strcspn (arrtbl, &c)) >= 4)
	return ((p = strcspn (funtbl, &c)) >= 18 ? c : (p + 31) * -1);
    else
	return ((p + 27) * -1);
}

ttyqio_$cancel()
{
   if (!ttyqio_$get_term_chan())
      return(FALSE);
   sys$cancel(ttyqio_$terminal_chan);
}

PublicFnDef int
STD_cleanupTerminal()
{
   ttyqio_$cancel();
   ttyqio_$disable_ast();
}
#endif /* VMS */

/*******************************************
 *****  CURSES compatibility routines  *****
 *******************************************/

PublicVarDef int	STD_hasInsertDeleteLine = FALSE;
PublicVarDef int	STD_doNotUseNodelay = FALSE;

#if defined(hp9000s500) || defined(apollo) || defined(sun)
PublicVarDef int	STD_delwinDoesNotRefresh = TRUE;

PublicFnDef int STD_checkTerminalCapabilities()
{
    static char entry[1024];
    static int  entryIsntValid = TRUE;
    extern char *tgetstr(), *getenv ();
    char *term, *areap, area[1024];

    if (entryIsntValid)
    {
	term = getenv ("TERM");
	if (IsNil (term))
	    term = "vt100";

	entryIsntValid = tgetent (entry, term) != 1;
    }

    areap = area;
    if( ((NULL != tgetstr("dl",&areap)) || (NULL != tgetstr("DL",&areap))) &&
	((NULL != tgetstr("al",&areap)) || (NULL != tgetstr("AL",&areap))) )
    	STD_hasInsertDeleteLine = TRUE;
    else
    	STD_hasInsertDeleteLine = FALSE;
}

PublicFnDef int
STD_cleanupTerminal()
{
}
#endif /* hp || apollo */

#if defined(apollo)
#include <sgtty.h>

PrivateVarDef struct sgttyb	ShellModeTerminalState,
				ProgramModeTerminalState;

PublicFnDef int
    APO_def_shell_mode		()
{
    gtty (0, &ShellModeTerminalState);
}

PublicFnDef int
    APO_reset_shell_mode	()
{
    stty (0, &ShellModeTerminalState);
}

PublicFnDef int
    APO_def_prog_mode		()
{
    gtty (0, &ProgramModeTerminalState);
}

PublicFnDef int
    APO_reset_prog_mode		()
{
    stty (0, &ProgramModeTerminalState);
}

PublicFnDef int
    APO_initscr			()
{
    APO_def_shell_mode	();
    initscr		();
}
#endif

#ifdef VMS
PublicVarDef int	STD_delwinDoesNotRefresh = FALSE;

PublicFnDecl void	RS_callSignalHandler();

PrivateFnDef int
VAXints(c)
int	c;
{
	switch( c )
	{
		case 3:		/* Control-C */
			runPseudoSignalHandler(SIGINT);
			break;
		case 25:	/* Control-Y */
		case 28:	/* Control-\ */
			RS_callSignalHandler(SIGQUIT);
			break;
	}
}

int
ttyqio_$enable_all_asts()
{
	int	status;

    	status = ttyqio_$enable_ast( VAXints,
    				     ((1<<3)  |
    				      (1<<25) |
    				      (1<<28)) );
    	if( status == FALSE )
		printf("\r\nfailed to set outband\r\n");
}

PublicFnDef int STD_checkTerminalCapabilities()
{
	/* VAX does it automatically so don't bother setting to TRUE */
    	STD_hasInsertDeleteLine = FALSE;
	ttyqio_$enable_all_asts();
}

PublicFnDef int
beep()
{
	putchar(7);
}

PublicFnDef int
VMS_tgetnum(id)
char	*id;
{
	/* Assume tabs on VMS are always 8 */
	if( strcmp(id,"it") == 0 )
		return( 8 );
	
	/* Don't know about other id's */
	return( 0 );
}
#endif /* VMS */

#if defined (hp9000s500) || defined(VMS) || defined(sun)
PublicFnDef int STD_checkInputStream()
{
	return(FALSE);
}
#endif /* hp || VMS */

#ifdef apollo
PublicFnDef int STD_checkInputStream()
{
    STD_doNotUseNodelay = TRUE;

    return(FALSE);
}
#endif /* apollo */

#endif /* FRONTEND */

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MStermio.c 1 replace /users/jck/interface
  880330 22:05:25 jck Installing New Standard Interface

 ************************************************************************/
/************************************************************************
  MStermio.c 2 replace /users/lis/frontend/M2/sys
  880505 11:36:29 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  MStermio.c 3 replace /users/m2/frontend/sys
  891011 14:12:30 m2 VAX signals and pipes fixes, APOLLO DN10000 fixes

 ************************************************************************/
