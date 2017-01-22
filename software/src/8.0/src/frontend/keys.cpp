/****************************************************************************
*****									*****
*****			keys.c		         			*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include <term.h>

#include "error.h"

/***** Key Defines *****/
#include "keys.h"

#define EMPTY 		0
#define	DelayOFF	0x02
#define	DelayON 	0x00
#define	PassthruON	0x04
#define	PassthruOFF	0x00

/*****  Forward Declarations  *****/
PrivateFnDef int getScript (int *c);

PrivateFnDef int putScript (int	c);

/*****  Globals  *****/
PrivateVarDef int Cbuf = EMPTY;
PublicVarDef  int DELAYbits = DelayOFF;
PublicVarDef  int PASSTHRUbits = PassthruON;
PublicVarDef int KEY_scriptRead = FALSE, KEY_scriptWrite = FALSE;
PrivateVarDef FILE *KEY_scriptReadFp = NULL;
PrivateVarDef FILE *KEY_scriptWriteFp = NULL;
PrivateVarDef int JustDidPutc = FALSE;
PrivateVarDef int DescribingOn = FALSE;
PublicVarDef int KEY_QuoteNextKey = FALSE;
PrivateVarDef char DescribeBuf[81];
PublicVarDef int RepetitionCount = 0,
		 RepetitionKey = 0,
		 RepetitionQuote = FALSE;

struct StringKey {
    char const 	*s;
    int		k;
    char const	*descrip;
};

#define NUMSTRINGS	82

PrivateVarDef
struct StringKey StringKeyTable[NUMSTRINGS] = {
	"esc",		KEY_ESC,	"ESCAPE prefix character",
	"bksp",		KEY_BKSP,	"Delete previous character",
	"help",		KEY_HELP,	"Help",
	"exec",		KEY_EXEC,	"Execute",
	"window",	KEY_WINDOW,	"Next window or Invoke Windows Menu",
	"edit",		KEY_EDIT,	"Invoke Editor",
	"files",	KEY_FILES,	"Invoke Files Menu",
	"regions",	KEY_REGIONS,	"Invoke Region Menu",
	"smenu",	KEY_SMENU,	"Invoke System Menu",
	"amenu",	KEY_AMENU,	"Invoke Application Menu",
	"prev",		KEY_PREV,	"To Previous Module",
	"quit",		KEY_QUIT,	"Quit VISION",
	"eol",		KEY_VEOL,	"Move to end of line",
	"bol",		KEY_BOL,	"Move to beginning of line",
	"del",		KEY_DEL,	"Delete current character",
	"deleol",	KEY_DELEOL,	"Delete to end of line",
	"fch",		KEY_FCH,	"Move forward one column",
	"bkch",		KEY_BKCH,	"Move backward one column",
	"btab",		KEY_BTAB,	"Back Tab",
	"tab",		KEY_TAB,	"Tab (converted to proper number of spaces)",
	"nl",		KEY_NL,		"Move down one row",
	"upl",		KEY_UPL,	"Move up one row",
	"repaint",	KEY_REPAINT,	"Redraw screen",
	"scr_l",	KEY_SCR_L,	"Scroll left one page",
	"scr_r",	KEY_SCR_R,	"Scroll right one page",
	"scr_u",	KEY_SCR_U,	"Scroll up one page",
	"scr_d",	KEY_SCR_D,	"Scroll down one page",
	"togglewin",	KEY_TOGGLEWIN,	"Toggle between windows",
	"incrementz",	KEY_INCREMENTZ,	"Increment Z axis",
	"decrementz",	KEY_DECREMENTZ,	"Decrement Z axis",
	"transpose",	KEY_TRANSPOSE,	"Transpose rows and columns",
	"top",		KEY_TOP,	"To top of buffer",
	"bottom",	KEY_BOTTOM,	"To bottom of buffer",
	"recall",	KEY_RECALL,	"Recall previous input buffer",
	"zerorow",	KEY_ZEROROW,	"Move current row to top of window",
	"search",	KEY_SEARCH,	"Search for a string",
	"snext",	KEY_SNEXT,	"Search forward for current search string",
	"sprev",	KEY_SPREV,	"Search backward for current search string",
	"begregion",	KEY_BEGREGION,	"Mark beginning of region",
	"endregion",	KEY_ENDREGION,	"Mark end of region (and copy region to temporary buffer)",
	"replace",	KEY_REPLACE,	"Replace all occurrences of one string with another string",
	"query",	KEY_QUERYR,	"Query to replace all occurrences of one string with another string",
	"readfile",	KEY_READFILE,	"Read a file",
	"insertreg",	KEY_INSERTREG,	"Insert region in temporary buffer at current cursor position",
	"meta",		KEY_META,	"Meta prefix character",
	"f1",		CUR_KEY_F1,	"F1 function key",
	"f2",		CUR_KEY_F2,	"F2 function key",
	"f3",		CUR_KEY_F3,	"F3 function key",
	"f4",		CUR_KEY_F4,	"F4 function key",
	"f5",		CUR_KEY_F5,	"F5 function key",
	"f6",		CUR_KEY_F6,	"F6 function key",
	"f7",		CUR_KEY_F7,	"F7 function key",
	"f8",		CUR_KEY_F8,	"F8 function key",
	"f9",		CUR_KEY_F9,	"F9 function key",
	"f10",		CUR_KEY_F10,	"F10 function key",
	"up",		CUR_KEY_UP,	"Move up one row",
	"down",		CUR_KEY_DOWN,	"Move down one row",
	"left",		CUR_KEY_LEFT,	"Move left one column",
	"right",	CUR_KEY_RIGHT,	"Move right one column",
	"home",		KEY_HOME,	"Move to top of buffer",
	"ll",		KEY_LL,		"Move to bottom of buffer",
	"wordback",	KEY_WORDBACK,	"Move to beginning of previous word",
	"wordforw",	KEY_WORDFORW,	"Move forward to end of next word",
	"worddelc",	KEY_WORDDELC,	"Delete from current position to end of next word",
	"worddelp",	KEY_WORDDELP,	"Delete from current position to beginning of previous word",
	"scrup1",	KEY_SCROLLUP1,	"Scroll up one row",
	"scrdn1",	KEY_SCROLLDN1,	"Scroll down one row",
	"onewin",	KEY_ONEWINDOW,	"Use one window",
	"twowin",	KEY_TWOWINDOW,	"Use two windows",
	"erasewin",	KEY_ERASEWIN,	"Erase current window",
	"savefile",	KEY_SAVEFILE,	"Save to the current file",
	"writefile",	KEY_WRITEFILE,	"Write to a file",
	"scriptr",	KEY_SCRIPTR,	"Read keystrokes from a script file",
	"scriptw",	KEY_SCRIPTW,	"Write keystrokes to a script file until another Write Script key received",
	"subshell",	KEY_SUBSHELL,	"Enter an operating system shell",
	"delregion",	KEY_DELREGION,	"Delete marked region",
	"describe",	KEY_DESCRIBE,	"Describe function of a keystroke",
	"quote",	KEY_QUOTE,	"Quote next character (i.e. insert character instead of performing its function)",
	"repeat",	KEY_REPEAT,	"Repeat key a given number of times",
	"copyPB",	KEY_COPYPB,	"Copy Last Output to Default Paste Buffer",
	"^",		'^',		"Control prefix in script files",
	"\\",		'\\',		"Command prefix in script files"
};

PrivateVarDef char	*KeypadSetString = NULL;

PublicFnDef void KEY_setKeypad() {
    char area[1024], *areap = area;

    char ks[] = "ks";
    if( KeypadSetString == NULL )
	    KeypadSetString = tgetstr(ks,&areap);

#if defined(__hp9000s700)
    CUR_keypad(CUR_stdscr,TRUE);
#endif

    if( KeypadSetString == NULL )
    	fputs("\033=",stdout);
    else
    	fputs(KeypadSetString,stdout);
}

PublicFnDef void KEY_initKeyboard() {
	KEY_setKeypad();
}

PrivateFnDef void KEY_nodelay(int on) {
    if( KEY_scriptRead || STD_doNotUseNodelay )
	return;

    CUR_nodelay(CUR_stdscr,on);
}

PublicFnDef int KEY_cready() {
    int	i;

    if (Cbuf != EMPTY)
	return(TRUE);

    if( KEY_scriptRead )
    {
	if( getScript(&i) )
	    return(FALSE);
	else
	    Cbuf = i;
    }
    else
    {
	if( STD_doNotUseNodelay )
            return(FALSE);

	KEY_nodelay(TRUE);

	if( (i = CUR_wgetch(CUR_stdscr)) == -1 )
	{
	    KEY_nodelay(FALSE);
	    return(FALSE);
	}
	Cbuf = i;
	KEY_nodelay(FALSE);
    }
    return(TRUE);
}

PublicFnDef FILE *LOGFILE () {
    static FILE *pLogFile = fopen ("logfile", "a");
    return pLogFile;
}

PrivateFnDef int cget() {
    FILE *pLogFile = LOGFILE ();
    static int PREV_LINES = CUR_LINES;
    static int PREV_COLS = CUR_COLS;

    int c;

    if (Cbuf != EMPTY)
    {
	c = Cbuf;
	Cbuf = EMPTY;
	return(c);
    }

    if( KEY_scriptRead )
    {
	if( !getScript(&c) )
	    return(c);
    }

    c = (CUR_wgetch(CUR_stdscr));
    if (pLogFile && (CUR_LINES != PREV_LINES || CUR_COLS != PREV_COLS)) {
	fprintf (pLogFile, "%04o: LINES: %u %u, COLS: %u %u\n", c, CUR_LINES, PREV_LINES, CUR_COLS, PREV_COLS);
	CUR_COLS = PREV_COLS;
	CUR_LINES = PREV_LINES;
    }
}

PublicFnDef int KEY_getkey(int allowRepetition) {
    int c, i;

    c = cget();

    if( KEY_QuoteNextKey )
    {
    	    KEY_QuoteNextKey = FALSE;
	    if( !JustDidPutc && KEY_scriptWrite )
		putScript(c);
	    if( JustDidPutc )
		JustDidPutc = FALSE;
	    return(c);
    }
    
    switch (c)	
    {			    /*** main switch	      ***/
    case KEY_ESC:

	c = cget();

	switch (c) 
	{		    /*** switch after ESC key ***/
	case '[':
	    c = cget();
	    switch (c)
	    {		    /*** switch after [ key ***/
	    case 'A':		c = CUR_KEY_UP; break;
	    case 'B':		c = CUR_KEY_DOWN; break;
	    case 'C':	        c = CUR_KEY_RIGHT; break;
	    case 'D':	        c = CUR_KEY_LEFT; break;

	    /* The following are for X windows on the Apollo */
	    case '1':
		c = cget();
		switch (c)
		{
		case '1':	c = CUR_KEY_F1; cget(); break;
		case '2':	c = CUR_KEY_F2; cget(); break;
		case '3':	c = CUR_KEY_F3; cget(); break;
		case '4':	c = CUR_KEY_F4; cget(); break;
		case '5':	c = CUR_KEY_F5; cget(); break;
		case '7':	c = CUR_KEY_F6; cget(); break;
		case '8':	c = CUR_KEY_F7; cget(); break;
		case '9':	c = CUR_KEY_F8; cget(); break;
		default:	c = -1; break;
		}
		break;
	    case '2':
		c = cget();
		switch (c)
		{
		case '0':	c = CUR_KEY_F9; cget(); break;
#if defined(sun)
                case '2':
		   c = cget();
		   switch (c)
		     {
		     case '4':	c = CUR_KEY_F1; cget(); break;
		     case '5':	c = CUR_KEY_F2; cget(); break;
		     case '6':	c = CUR_KEY_F3; cget(); break;
		     case '7':	c = CUR_KEY_F4; cget(); break;
		     case '8':	c = CUR_KEY_F5; cget(); break;
		     case '9':	c = CUR_KEY_F6; cget(); break;
		     default:	c = -1; break;
		     }
		   break;
                case '3':
		   c = cget();
		   switch (c)
		     {
		     case '0':	c = CUR_KEY_F7; cget(); break;
		     case '1':	c = CUR_KEY_F8; cget(); break;
		     case '2':	c = CUR_KEY_F9; cget(); break;
		     default:	c = -1; break;
		     }
		   break;
#endif
		default:	c = -1; break;
		}
		break;
	    case STD_CTRL('E'):	c = CUR_KEY_UP; break;
	    case STD_CTRL('D'):	c = CUR_KEY_UP; break;
	    case '0':		c = CUR_KEY_DOWN; break;
	    case 'F':	        c = CUR_KEY_RIGHT; break;
	    case STD_CTRL('@'):       c = CUR_KEY_LEFT; break;

	    default:	    	c = -1; break;
	    }			/*** switch after [ key ***/
	    break;

	case 'O':
	    c = cget();
	    switch (c) 
	    {		    /*** switch after O key ***/
	    case 'A':	        c = CUR_KEY_UP; break;
	    case 'B':	        c = CUR_KEY_DOWN; break;
	    case 'C':	        c = CUR_KEY_RIGHT; break;
	    case 'D':	        c = CUR_KEY_LEFT; break;
	    case 'P':		c = CUR_KEY_F1; break;
	    case 'Q':		c = CUR_KEY_F2; break;
	    case 'w':
	    case 'R':		c = CUR_KEY_F3; break;
	    case 'x':
	    case 'S':		c = CUR_KEY_F4;	break;
	    case 't':		c = CUR_KEY_F5;	break;
	    case 'u':		c = CUR_KEY_F6;	break;
	    case 'q':		c = CUR_KEY_F7;	break;
	    case 'r':		c = CUR_KEY_F8;	break;
	    case 'p':		c = CUR_KEY_F9;	break;
	    case 'n':		c = CUR_KEY_F10;break;
	    case 'm':		c = KEY_HOME;	break;
	    case 'l':		c = KEY_LL;	break;

	    /* The following are for X windows on the Apollo */
	    case STD_CTRL('E'):	c = CUR_KEY_UP; break;
	    case STD_CTRL('D'):	c = CUR_KEY_UP; break;
	    case '0':		c = CUR_KEY_DOWN; break;
	    case 'F':	        c = CUR_KEY_RIGHT; break;
	    case STD_CTRL('@'):     c = CUR_KEY_LEFT; break;

	    default:	    	c = -1;		break;
	    }		    /*** switch after O key ***/
	    break;

	case '<':	c = KEY_HOME; break;
	case '>':	c = KEY_LL; break;
	case 'R':
	case 'r':	c = KEY_RECALL; break;
	case 'H':
	case 'h':	c = KEY_ZEROROW; break;
	case 'S':
	case 's':	c = KEY_SEARCH; break;
	case 'N':
  	case 'n':	c = KEY_SNEXT; break;
	case 'P':
	case 'p':	c = KEY_SPREV; break;
	case 'B':
	case 'b':	c = KEY_BEGREGION; break;
	case 'E':
	case 'e':	c = KEY_ENDREGION; break;
	case 'D':
	case 'd':	c = KEY_DELREGION; break;
	case 'I':
	case 'i':	c = KEY_INSERTREG; break;
	case 'Q':
	case 'q':	c = KEY_QUERYR; break;
	case 'C':
	case 'c':	c = KEY_REPLACE; break;
	case 'F':
	case 'f':	   /* alias for function keys */
	    c = cget();
	    c = c - '0';
	    switch(c) {
		case 1:		c = CUR_KEY_F1; break;
		case 2:		c = CUR_KEY_F2; break;
		case 3:		c = CUR_KEY_F3; break;
		case 4:		c = CUR_KEY_F4; break;
		case 5:		c = CUR_KEY_F5; break;
		case 6:		c = CUR_KEY_F6; break;
		case 7:		c = CUR_KEY_F7; break;
		case 8:		c = CUR_KEY_F8; break;
		case 9:		c = CUR_KEY_F9; break;
		case 0: 	c = CUR_KEY_F10; break;
		default: 	c = -1; break;
	    }
	    break;           

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    if( !allowRepetition )
	    {
	    	c = -1;
	    	break;
	    }
	    KEY_putc(c);
	    c = KEY_REPEAT;
	    break;
	default:	c = -1; break;
	}		    /*** switch after ESC key ***/
	break;

    case KEY_META:    
	c = cget();
	switch (c) 
	{		    /*** switch after META key ***/
	case STD_CTRL('B'):	c = KEY_WORDBACK; break;
	case STD_CTRL('F'):	c = KEY_WORDFORW; break;
	case STD_CTRL('G'):	c = KEY_WORDDELC; break;
	case STD_CTRL('H'):
	case KEY_BKSP:	c = KEY_WORDDELP; break;
	case STD_CTRL('P'):	c = KEY_SCROLLUP1; break;
	case STD_CTRL('N'):	c = KEY_SCROLLDN1; break;
	case '1':	c = KEY_ONEWINDOW; break;
	case '2':	c = KEY_TWOWINDOW; break;
	case 'E':
	case 'e':	c = KEY_ERASEWIN; break;
	case 'R':
	case 'r':	c = KEY_READFILE; break;
	case 'S':
	case 's':	c = KEY_SAVEFILE; break;
	case 'W':
	case 'w':	c = KEY_WRITEFILE; break;
	case '<':	c = KEY_SCRIPTR; break;
	case '>':	c = KEY_SCRIPTW; break;
	case STD_CTRL('X'):	c = KEY_SUBSHELL; break;
	case '?':
		if( DescribingOn )
		{
			c = KEY_DESCRIBE;
			break;
		}
		DescribingOn = TRUE;
		ERR_displayStr("Describe key: ",FALSE);
		c = KEY_getkey(FALSE);
		KEY_QuoteNextKey = FALSE;
		for( i=0 ; i<(NUMSTRINGS-2) ; i++ )
			if( c == StringKeyTable[i].k )
					break;
		if( i >= (NUMSTRINGS-2) )
		{
			if( c & ~0x7f )
				ERR_displayStr("Undefined key",TRUE);
			else
			{
				if( (c == STD_CTRL('J')) || (c == STD_CTRL('M')) )
					sprintf(DescribeBuf,"Newline");
				else if( isalpha(c) )
					sprintf(DescribeBuf,"The letter: '%c'",c);
				else if( isdigit(c) )
					sprintf(DescribeBuf,"The digit: '%c'",c);
				else if( ispunct(c) )
					sprintf(DescribeBuf,"The punctuation character: '%c'",c);
				else
					sprintf(DescribeBuf,"Undefined key");
				ERR_displayStr(DescribeBuf,FALSE);
			}
		}
		else
			ERR_displayStr(StringKeyTable[i].descrip,FALSE);
		ERR_msgDisplayed = FALSE;
		DescribingOn = FALSE;
		c = 0;
		break;
	default:	c = -1; break;
	}
	break;
    case KEY_NPAGE:	c = KEY_SCR_D; break;
    case KEY_PPAGE:	c = KEY_SCR_U;break;
    case KEY_DC:	c = KEY_DEL; break;
    case KEY_SF:	c = KEY_SNEXT;break;
    case KEY_QUOTE:    	
    	KEY_QuoteNextKey = TRUE;
    	break;

    case STD_CTRL('H'):    	c = KEY_BKSP; break;
    default:		break;
    }			    /*** main switch  ***/

    if( !JustDidPutc && KEY_scriptWrite )
	putScript(c);
    if( JustDidPutc )
	JustDidPutc = FALSE;

    return(c);
    
}

PublicFnDef int KEY_putc(int c) {
    Cbuf = c;
    JustDidPutc = TRUE;
    return FALSE;
}

PublicFnDef int KEY_beginScriptWrite() {
    char scriptFile[128];

    if( KEY_scriptWrite )
    {
	fclose(KEY_scriptWriteFp);
	KEY_scriptWrite = FALSE;
	ERR_displayStr("Script terminated",FALSE);
	return(FALSE);
    }
    if( ERR_promptForString("Enter script file in which to save: ", scriptFile, FALSE) )
	return(FALSE);
    if( (KEY_scriptWriteFp = fopen(scriptFile,"w")) == NULL )
    {
	ERR_displayError(ERR_OpenError);
	return(FALSE);
    }
    KEY_scriptWrite = TRUE;
    return(FALSE);
}

PublicFnDef int KEY_beginScriptRead() {
    char scriptFile[128];

    if( KEY_scriptRead )
    {
	CUR_beep();
	return(FALSE);
    }
    if( ERR_promptForString("Enter script file from which to read: ", scriptFile, FALSE) )
	return(FALSE);
    if( KEY_beginScriptReadF(scriptFile) )
	ERR_displayError(ERR_OpenError);
    return(FALSE);
}

PublicFnDef int KEY_beginScriptReadF(char const *scriptFile) {
    if( KEY_scriptRead )
    {
	CUR_beep();
	return(FALSE);
    }
    if( (KEY_scriptReadFp = fopen(scriptFile,"r")) == NULL )
	return(TRUE);
    KEY_scriptRead = TRUE;
    return(FALSE);
}

PrivateVarDef char Rbuf[256];
PrivateVarDef int  Rcount = 0, Rindex = 0;

PrivateFnDef int getScript (int *c) {
    char *b, temp;
    int i;

    if( Rcount > Rindex )
    {
	*c = (int)Rbuf[Rindex++];
	return(FALSE);
    }
    if( fgets(Rbuf,256,KEY_scriptReadFp) == NULL )
    {
	fclose(KEY_scriptReadFp);
	KEY_scriptRead = FALSE;
	return(TRUE);
    }

    Rcount = Rindex = 0;
    b = &Rbuf[1];
    switch( Rbuf[0] )
    {
	case '\0':
	    *c = 0;
	    break;
	case '\n':
	case '\r':
	    *c = KEY_CR;
	    break;
	case '^':
	    temp = (islower(Rbuf[1]) ? Rbuf[1]-'a'+'A' : Rbuf[1]);
	    if( temp == '\n' )
		*c = (int)'^';
	    else if( temp < 'A' )
		*c = 0;
	    else
		*c = (int)(temp - 'A' + 1);
	    break;
	case '\\':
	    for( i=0 ; i<NUMSTRINGS ; i++ )
	    {
		if( strncmp(b,StringKeyTable[i].s,strlen(StringKeyTable[i].s)) == 0 )
		{
		    *c = StringKeyTable[i].k;
		    break;
		}
	    }
	    if( i >= NUMSTRINGS )
		*c = (int)'\\';
	    break;
	default:
	    *c = (int)Rbuf[0];
	    Rcount = strlen(Rbuf);
	    if( Rbuf[Rcount-1] == '\n' )
		Rcount--;
	    Rindex = 1;
	    break;
    }
    return(FALSE);
}

#define dumpline if( bufidx > 0 )\
{\
	buf[bufidx++] = '\n';\
	buf[bufidx] = '\0';\
	bufidx = 0;\
	if( fputs(buf,KEY_scriptWriteFp) == EOF )\
	{\
		fclose(KEY_scriptWriteFp);\
		KEY_scriptWrite = FALSE;\
		return(TRUE);\
	}\
}

PrivateFnDef int putScript (int c) {
    static char buf[256];
    static int	bufidx = 0;
    int i;

    if( c == KEY_SCRIPTW )
    {
    	dumpline;
	return(FALSE);
    }

    if( (c > 0) && (c < ' ') )
    {
    	dumpline;
	buf[bufidx++] = '^';
	buf[bufidx++] = c + 'A' - 1;
	dumpline;
    }
    else if( (c == '\\') || (c == '^') )
    {
    	dumpline;
	buf[bufidx++] = '\\';
	buf[bufidx++] = c;
	dumpline;
    }
    else if( isprint(c) && !(c & 0xff00) )
    {
	buf[bufidx++] = c;
	if( bufidx > 78 )
	{
		dumpline;
	}
    }
    else
    {
    	dumpline;
	for( i=0 ; i<NUMSTRINGS ; i++ )
	{
	    buf[0] = '\\';
	    if( c == StringKeyTable[i].k )
	    {
		strcpy(&buf[1],StringKeyTable[i].s);
		break;
	    }
	}
	if( i >= NUMSTRINGS )
	    sprintf(buf,"0x%08x",c);
	bufidx = strlen(buf);
	dumpline;
    }
    return(FALSE);
}
