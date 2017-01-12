/*********************************************************
 **********		error.c			**********
 ********************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "keys.h"

#include "error.h"

PublicVarDef CUR_WINDOW *ERR_Window;
PublicVarDef int ERR_msgDisplayed = FALSE;
PublicVarDef int RS_TabSpacing = 8;

PrivateVarDef char const *Message[] = {
     "",			/**** do not use this message ****/
     " Top of buffer", 
     " Bottom of buffer", 
     " Line is full", 
     " Wrote file", 
     " File open error", 
     " No region set", 
     " Function undefined", 
     " Undefined key", 
     " Begin region", 
     " End region", 
     " Cleared region", 
     " Inserted region", 
     " Deleted region", 
     " Executing region...", 
     " Executing buffer...", 
     " *** hangup signal received ***", 
     " *** interrupt signal received ***",
     " *** quit signal received ***",
     " *** illegal instruction signal received ***",
     " *** trace trap signal received ***",
     " *** abort (IOT) signal received ***",
     " *** software generated signal received ***",
     " *** floating point exception signal received ***",
     " *** bus error signal received ***",
     " *** segmentation violation signal received ***",
     " *** bad arg. to system call signal received ***",
     " *** write on a pipe with no reader signal received ***",
     " *** alarm clock signal received ***",   
     " *** software termination signal received ***",
     " *** Error trying restart the RS nowhere in editor to go\n",
     " *** user defined signal 2 signal received ***",
     " *** power fail signal received ***",
     " *** virtual timer alarm signal received ***",
     " *** profiling timer alarm signal received ***",
     " *** SIGIO signal received ***",
     " *** window or mouse signal received ***",
     " *** unknown signal received ***",
     " Error setting signal vector", 
     " Type first letter of choice, or use arrow keys to highlight, <CR> to execute", 
     " *** user defined signal 1 signal received ***", 
    " Error allocating buffer space", 
    " Invalid row in buffer", 
    " Error appending to buffer", 
    " Read file", 
    " Bad region", 
    " Nothing to recall", 
    " Beginning of line", 
    " End of line", 
    " Invalid choice", 
    " Top of form", 
    " Bottom of form", 
    " Valid only in a menu field", 
    " Not valid in a menu field", 
    " Beginning of field", 
    " End of field", 
    " Numeric only", 
    " Only one window", 
    " Error in window handler function", 
    " Sorry, no help available", 
    " Sorry, application menu not available", 
    " Out of memory, please delete edit buffers", 
    " Cleared region markers. Region retained in temporary buffer", 
    " Asked for help",
    " Aborted prompt",
    " File was empty",
};

PublicFnDef void ERR_paintLineASCII(
    char const *str, int width, CUR_WINDOW *win, int useHilight, int hilight
) {
	int	i=0, j, k;
	char	c, *s;

	for( s=str ; ((*s != '\0') && (i<width)) ; s++ )
	{
		c = *s;
		if( c == '\t' )
		{
			j = (((i + RS_TabSpacing) / RS_TabSpacing) * RS_TabSpacing) - i;
			for( k=0 ; ((k<j) && (i<width)) ; k++ )
			{
				CUR_waddch(win,' ');
				i++;
			}
		}
		else if( iscntrl(c) )
		{
			if( c < 040 )	/* a space */
				c += '@';
			else
				c = '?';
			if( useHilight )
				CUR_wattron(win,hilight);
			CUR_waddch(win,c);
			if( useHilight )
				CUR_wattroff(win,hilight);
			i++;
		}
		else
		{
			CUR_waddch(win,c);
			i++;
		}
	}
	if( i < width )
		CUR_wprintw(win,"%-*.*s",(width-i),(width-i),"  ");
}

PublicFnDef void ERR_displayNoCurses(int i) {
    printf("\n%s%c\n\n",Message[i],7);
}

PublicFnDef void ERR_displayError(int i) {
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_mvwaddstr(ERR_Window, 0, 0, Message[i]);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, strlen(Message[i]));
    CUR_beep();
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displayMsg(int i) {
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_mvwaddstr(ERR_Window, 0, 0, Message[i]);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, strlen(Message[i]));
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_fatal(char const *msg) {
    int		i;
    char	buf[256];
    sprintf(buf, "%s, press any key to abort...", msg);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    CUR_beep();
    KEY_getkey(FALSE);
    KEY_QuoteNextKey = FALSE;
    exit(ErrorExitValue);
}

PublicFnDef void ERR_clearMsg()
{
    CUR_werase (ERR_Window);
    CUR_touchwin(ERR_Window);
    CUR_wnoutrefresh(ERR_Window);
    ERR_msgDisplayed = FALSE;
}

PublicFnDef int ERR_promptYesNo(char const *msg) {
    int c;
    int		i;
    char	buf[256];
    sprintf(buf, "%s (y/n) ?", msg);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    c = ' ';
    while (c != 'y' && c != 'Y' && 
	   c != 'n' && c != 'N')
    {
	CUR_beep();
        c = KEY_getkey(FALSE);
        KEY_QuoteNextKey = FALSE;
    }
    CUR_werase (ERR_Window);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    ERR_msgDisplayed = FALSE;

    if (c == 'y' || c == 'Y')
        return(TRUE);
    else
	return(FALSE);
}

PublicFnDef void ERR_displayStr(char const *str, int error) {
    int		i;
    char	buf[256];
    sprintf(buf, "%s", str);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    if (error) CUR_beep();
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displayStrNoRefresh(char const *str) {
    int		i;
    char	buf[256];
    sprintf(buf, "%s", str);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wnoutrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    CUR_beep();
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displaySearchStr(char const *str) {
    int		i;
    char	buf[256];
    sprintf(buf, "Searching for %s", str);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displayStrMsg(
    char const *str, char const *msg, int BEEP
) {
    int	i;
    char buf[256];

    sprintf(buf, "%s %s", str, msg);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    if ( BEEP ) CUR_beep();
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displayPause(char const *str) {
    int	i;
    char buf[256];

    sprintf(buf, "%s, press any key to continue...", str);
    if( (i = strlen(buf)) > CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window);
    CUR_werase (ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(buf,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, buf);*/
    CUR_wmove(ERR_Window, 0, i);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_beep();
    CUR_forceSetCursor(ERR_Window, 0, i);
    KEY_getkey(FALSE);
    KEY_QuoteNextKey = FALSE;
    ERR_msgDisplayed = TRUE;
}

PublicFnDef void ERR_displayMsgPause(int i) {
	ERR_displayPause(Message[i]);
}

PublicFnDef int ERR_promptForString(
    char const *prompt, char *buffer, int allowHelp
) {
    int i, c, col, len, idx;

    buffer[0] = '\0';

    if( (i = strlen(prompt)) >= CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window)-1;
    CUR_werase(ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(prompt,i,ERR_Window,TRUE,CUR_A_REVERSE);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    col = i;
    len = CUR_WIN_maxx(ERR_Window) - col;

    i = idx =0;
    c = -1;
    while (c != KEY_CR)
    {
    	idx = i - len;
    	if( idx < 0 )
    	    idx = 0;
	CUR_wmove(ERR_Window, 0, col);
	ERR_paintLineASCII(&buffer[idx],len,ERR_Window,TRUE,CUR_A_REVERSE);
	CUR_wmove(ERR_Window, 0, col+i-idx);
	CUR_wrefresh(ERR_Window);
	CUR_forceSetCursor(ERR_Window, 0, col+i-idx);

	c = KEY_getkey(FALSE);
	switch (c)
	{
	    case 0:
		break;

	    case KEY_PREV:
	    case KEY_QUIT:
	        ERR_clearMsg();
	        return(ERR_AbortedPrompt);

	    case KEY_HELP:
	    	if( allowHelp )
	    	    return(ERR_AskedForHelp);
	    	else
	    	    CUR_beep();
	    	break;

	    case KEY_CR:
	        break;

	    case KEY_BKSP:
	        if (i > 0)
		    buffer[--i] = '\0';
		else
		    CUR_beep();
	        break;

	    case KEY_QUOTE:
	    	c = KEY_getkey(FALSE);
	        if( (c > 0x00) && (c <= 0x7f) )
		{
		    buffer[i++] = c;
		    buffer[i] = '\0';
		}
		else
		    CUR_beep();
	        break;
	        
	    default:
	        if (isprint(c) && !(c & ~0x7f))
		{
		    buffer[i++] = c;
		    buffer[i] = '\0';
		}
		else
		    CUR_beep();
	        break;
	        
	}   /*** switch ***/

    }   /*** while ***/

    return(FALSE);
}

PublicFnDef int ERR_promptForRepetition(
    int *num, CUR_WINDOW *win, int oy, int ox
) {
    int i, c, col, len, atoi(), NotDone = TRUE;
    char buffer[80], buffer2[80];

    i = 0;
    buffer[0] = '\0';

    c = -1;
    sprintf(buffer2,"%s","Repetition (F9 to abort): ");
    CUR_werase(ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    CUR_wprintw(ERR_Window, buffer2);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    col = strlen(buffer2);
    len = CUR_WIN_maxx(ERR_Window) - col;
    while( NotDone )
    {
	CUR_wmove(ERR_Window, 0, col);
	CUR_wprintw(ERR_Window, "%-*.*s", len, len, buffer);
	CUR_wmove(ERR_Window, 0, col+i);
	CUR_wrefresh(ERR_Window);
#if 0
	CUR_forceSetCursor(ERR_Window, 0, col+i);
#endif
	CUR_wmove(win, oy, ox);
	CUR_wrefresh(win);
	CUR_forceSetCursor(win, oy, ox);

	c = KEY_getkey(FALSE);
	switch (c)
	{
	    case 0:
		break;

	    case KEY_PREV:
	    case KEY_QUIT:
	    	ERR_clearMsg();
	        return(TRUE);

	    case KEY_BKSP:
	        if (i > 0)
		    buffer[--i] = '\0';
		else
		    CUR_beep();
	        break;

	    default:
	        if( isdigit(c) && !(c & ~0x7f) )
		{
		    if( i >= 79 )
		    {
			CUR_beep();
			break;
		    }
		    buffer[i++] = c;
		    buffer[i] = '\0';
		}
		else
		    NotDone = FALSE;
	        break;
	        
	}   /*** switch ***/

    }   /*** while ***/

    *num = atoi(buffer);
    KEY_QuoteNextKey = FALSE;
    KEY_putc(c);
    ERR_clearMsg();

    return(FALSE);
}

PublicFnDef int ERR_promptForInt(char const *str, int *num) {
    int i, c, col, len, NotDone = TRUE;
    char buffer[80];

    buffer[0] = '\0';

    if( (i = strlen(str)) >= CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window)-1;
    CUR_werase(ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(str,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, str);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    col = i;
    len = CUR_WIN_maxx(ERR_Window) - col;
    i = 0;
    while( NotDone )
    {
	CUR_wmove(ERR_Window, 0, col);
	CUR_wprintw(ERR_Window, "%-*.*s", len, len, buffer);
	CUR_wmove(ERR_Window, 0, col+i);
	CUR_wrefresh(ERR_Window);
	CUR_forceSetCursor(ERR_Window, 0, col+i);

	c = KEY_getkey(FALSE);
	switch (c)
	{
	    case 0:
		break;

	    case KEY_PREV:
	    case KEY_QUIT:
	        ERR_clearMsg();
	        return(TRUE);

	    case KEY_CR:
		NotDone = FALSE;
		break;

	    case KEY_BKSP:
	        if (i > 0)
		    buffer[--i] = '\0';
		else
		    CUR_beep();
	        break;

	    default:
	        if( isdigit(c) && !(c & ~0x7f) )
		{
		    if( i >= 79 )
		    {
			CUR_beep();
			break;
		    }
		    buffer[i++] = c;
		    buffer[i] = '\0';
		}
		else
		    CUR_beep();
		KEY_QuoteNextKey = FALSE;
	        break;
	        
	}   /*** switch ***/

    }   /*** while ***/

    *num = atoi(buffer);

    return(FALSE);
}

PublicFnDef int ERR_promptForChar(char const *prompt, char const *validChars) {
    int c, i, done = FALSE;
    
    if( (i = strlen(prompt)) >= CUR_WIN_maxx(ERR_Window) )
    	i = CUR_WIN_maxx(ERR_Window)-1;
    CUR_werase(ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    ERR_paintLineASCII(prompt,i,ERR_Window,TRUE,CUR_A_REVERSE);
/*    CUR_wprintw(ERR_Window, "%-*.*s", i, i, prompt);*/
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, i);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    CUR_forceSetCursor(ERR_Window, 0, i);
    while (!done)
    {
	c = KEY_getkey(FALSE);
	KEY_QuoteNextKey = FALSE;
	if (c == KEY_PREV || c == KEY_QUIT)
	{
	    ERR_clearMsg();
	    return(c);
	}
	for (i = 0; validChars[i] != '\0'; i++)
	    if (c == validChars[i])
	        return(c);
	CUR_beep();
    }
    return(FALSE);
}
