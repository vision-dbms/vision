/**********************************************************************
 *****        edit.c			                    ***********
 *****  VISION editor's main module.                        ***********
 *********************************************************************/

#include "Vk.h"

#include "edit.h"

/***** local includes *****/
#include "stdcurses.h"
#include "misc.h"
#include "page.h"
#include "form.h"
#include "buffers.h"
#include "mainmenu.h"
#include "menu.h"
#include "keys.h"
#include "gopt.h"
#include "error.h"
#include "rsInterface.h"
#include "choices.h"
#include "print.h"
#include "spsheet.h"
#include "vars.h"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PublicFnDef void browser ();

PublicFnDef void ED_subShell ();

PrivateFnDef void readFile (
    void
);

PrivateFnDef void readFileInt (
    void
);

PrivateFnDef void saveFileInt (
    void
);

PrivateFnDef void saveFile (
    void
);

PrivateFnDef void listFile (
    void    
);

PrivateFnDef void runSysMenu (
    void
);

PrivateFnDef void runEditorInterface (
    void
);

PrivateFnDef int execInterface (
    void
);

PrivateFnDef void copyPasteBufInt (
    void
);

PrivateFnDef int recall (
    void
);

PrivateFnDef void printBuffer (
    void
);

PrivateFnDef void saveRegionInt (
    void
);

PrivateFnDef void printRegion (
    void
);

#if 0
PrivateFnDef int uploadRegion (
    void
);
#endif

#if 0
PrivateFnDef int downloadRegion (
    void
);
#endif

PrivateFnDef void beginRegion (
    void
);

PrivateFnDef void endRegion (
    void
);

PrivateFnDef void clearRegion (
    void
);

PrivateFnDef void deleteRegion (
    void
);

PrivateFnDef void insertRegion (
    void
);

PrivateFnDef void runRegion (
    void
);

PrivateFnDef void switchWindow (
    void
);

PrivateFnDef void oneWindow (
    void
);

PrivateFnDef void eraseWindow (
    void
);

PrivateFnDef void twoWindows (
    void
);

PrivateFnDef void saveReportInt (
    void
);

PrivateFnDef void printReportInt (
    void
);

PrivateFnDef void copyPasteBufReport (
    void
);

PrivateFnDef void runReportInterface (
    void
);

PrivateFnDef void listReport ();

PrivateFnDef void PrintScreen (
    int				c
);

PrivateFnDef int copyFile (
    char *			fname,
    char *			dname,
    int				mode,
    int				filter,
    char *			filterOpts
);


#define SourceDirEnv  "VisionLibrary" 		/* Env. var needed to locate */
#define EditHelpName  "edit.help"		/* help files.               */
#define OutHelpName   "output.help"
PrivateVarDef char UnnamedPasteBuf[] = "default.txt";

#define OBUFSIZE		400000
#define IBUFSIZE		150000
#define RBUFSIZE		150000
#define NUMRECALL		3
#define MAX_OUTPUT_LINE_SIZE	(BUF_maxlinesize-1)	/* Max # of chars in one line*/
						/* of the output buffer.     */

#define READ 0		/* these constants are for pipe usage */
#define WRITE 1
#define STDERR 2


/*******************************************************************
 **********	Menu Declarations	****************************
 *******************************************************************/

#define COPYPASTEBUFchoice	3

PrivateVarDef MENU_Choice fileMenu[] = {
" Read ", " Read file and insert into buffer",		 'r', readFileInt, ON, 
" Save ",  " Save buffer contents to file",		 's', saveFileInt, ON, 
" Print ",  " Print buffers contents",			 'p', printBuffer, ON, 
" Copy ",  " Copy output to paste buffer",               'c', copyPasteBufInt, OFF, 
" Last ",  " Use Last Interface Form",			 'l', runEditorInterface, ON, 
" Directory ",  " List Files in a given Directory",      'd', listFile, ON,
" sHell",  " Enter an Operating System Sub-Shell (use 'exit' to return)",      'h', ED_subShell, ON,
NULL, 
};

PrivateVarDef MENU_Choice regionMenu[] = {
" Begin ", " Mark the beginning of region at cursor",	    'b', beginRegion, ON,
" End ", " Mark the end of region, save to region buffer",  'e', endRegion, ON, 
" Clear ", " Clear region markers",			    'c', clearRegion, ON,
" Delete ", " Delete region from the edit buffer",	    'd', deleteRegion, ON, 
" Insert ", " Insert region into buffer at cursor",	    'i', insertRegion, ON, 
" Run ", " Execute the region buffer",			    'r', runRegion, ON, 
" Save", " Save the region buffer",			    's', saveRegionInt, ON,
" Print ",  " Print regions contents",			 'p', printRegion, ON, 
#if 0
" UpLoad ",  " Upload a file from the PC into the Region buffer",
	'\0', uploadRegion, OFF, 
" Lotus ",  " Download a PRN file to the PC from the Region buffer",
	'l', downloadRegion, ON, 
#endif
NULL, 
};

PrivateVarDef MENU_Choice applicMenu[] = {
" Browser ", " Browse Through Objects In System",	    'b', browser, ON,
NULL, 
};

PrivateVarDef MENU_Choice windowMenu[] = {
" Switch ", " Move cursor to other window",		    's', switchWindow, ON, 
" One ", " Enlarge current window to fill entire screen",   'o', oneWindow, ON, 
" Two ", " Split the screen between Editor and Output",	    't', twoWindows, ON, 
" Erase ", " Erase buffer associated with current window",  'e', eraseWindow, ON, 
NULL, 
};


/*******************************************************************
 **********     Interface Form          ****************************
 *******************************************************************/

PrivateVarDef PAGE	*interfacePage = NULL, *reportPage = NULL;
PrivateVarDef FORM	*interfaceForm = NULL;
PrivateVarDef MENU	*pbufSMenu = NULL, *pbufDMenu = NULL;
PrivateVarDef CUR_WINDOW *intWin1, *intWin2;
PrivateVarDef int	doInterfaceClear = FALSE;

#define FILEfield	0
#define BUFFERfield	1
#define REGIONfield	2
#define PRINTERfield	3
#define PCfield		4
#define PASTEBUFfield	5

#define SourceType	1
#define SourceValue	2
#define DestType	10
#define DestValue	11
#define	FilterType	19
#define	FilterOpts	20

PrivateVarDef FORM_Field interfaceFields[] = {
 1, 1, CUR_A_NORMAL, 7, 0, 'a', "Source:",
	NULL, NULL, NULL,
 1, 14, CUR_A_REVERSE, 12, 1, 'S', "            ",
        " Use Arrow Keys to Select Source Type, or F1 For Menu", NULL, NULL, 
 1, 27, CUR_A_REVERSE, 48, 1, 'X', "                                                ",
 	NULL, NULL, NULL,
 1, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
 	" Enter Source File Name", NULL, NULL,
 1, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Source Buffer, or F1 For Menu", NULL, NULL, 
 1, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Source Region, or F1 For Menu", NULL, NULL, 
 1, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Source Printer, or F1 For Menu", NULL, NULL, 
 1, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
 	" Enter PC Source File Name", NULL, NULL,
 1, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
        " Enter Source Paste Buffer Name, or F1 For Menu of existing names", NULL, NULL, 
 3, 1, CUR_A_NORMAL, 12, 0, 'a', "Destination:",
	NULL, NULL, NULL,
 3, 14, CUR_A_REVERSE, 12, 1, 'S', "            ",
        " Use Arrow Keys to Select Destination Type, or F1 For Menu", NULL, NULL, 
 3, 27, CUR_A_REVERSE, 48, 1, 'X', "                                                ",
 	NULL, NULL, NULL,
 3, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
 	" Enter Destination File Name", NULL, NULL,
 3, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Destination Buffer, or F1 For Menu", NULL, NULL, 
 3, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Destination Region, or F1 For Menu", NULL, NULL, 
 3, 27, CUR_A_REVERSE, 48, 0, 'M', "                                                ",
        " Use Arrow Keys to Select Destination Printer, or F1 For Menu", NULL, NULL, 
 3, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
 	" Enter PC Destination File Name", NULL, NULL,
 3, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag), 'A', "                                                ",
        " Enter Destination Paste Buffer Name, or F1 For Menu of existing names", NULL, NULL, 
 5, 1, CUR_A_NORMAL, 7, 0, 'a', "Filter:",
	NULL, NULL, NULL,
 5, 14, CUR_A_REVERSE, 12, 1, 'm', "            ",
        " Use Arrow Keys to Select Filter Type, or F1 For Menu", NULL, NULL, 
 5, 27, CUR_A_REVERSE, 48, (FORM_ScrollFlag|FORM_InputFlag), 'a', "                                                ",
        " Enter Filter Options", NULL, NULL, 
#if 0
 7, 7, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" ,
	NULL, NULL, NULL,
#endif
-1,
};

#define	FILTERnone	0
#define FILTERformatter	1
#define FILTERuser	2

PrivateVarDef MENU_Choice filterChoices[] = {
"none        ", " Use no intermediate filter for transfer", 'n', FORM_menuToForm, ON,
"Formatter   ", " Use the VISION report formatter on Source before sending to Destination", 'f', FORM_menuToForm, ON,
"User Defined", " Use the Command in the Filter Options Field", 'u', FORM_menuToForm, ON,
NULL,
};

PrivateVarDef MENU_Choice srcChoices[] = {
"File        ", " Read from a system file",	    'f', FORM_menuToForm, ON, 
"Buffer      ", " Read from an editor buffer",	    'b', FORM_menuToForm, ON, 
"Region      ", " Read from an editor region",	    'r', FORM_menuToForm, ON,
"Printer     ", " Should never get this message",   'p', FORM_menuToForm, OFF, 
"PC          ", " Read from a text file on the PC", 'c', FORM_menuToForm, ON, 
"Paste Buffer", " Read from an Apollo Paste Buffer",'\0', FORM_menuToForm, OFF, 
NULL, 
};

PrivateVarDef MENU_Choice dstChoices[] = {
"File        ", " Write to a text file",	   'f', FORM_menuToForm, ON, 
"Buffer      ", " Write to an editor buffer",	   'b', FORM_menuToForm, ON, 
"Region      ", " Should never get this message",  'r', FORM_menuToForm, OFF,
"Printer     ", " Write to a printer",		   'p', FORM_menuToForm, ON, 
"PC          ", " Write to a text file on the PC", 'c', FORM_menuToForm, ON, 
"Paste Buffer", " Write to an Apollo Paste Buffer",'\0', FORM_menuToForm, OFF, 
NULL, 
};

#define BUFFERedit	0
#define BUFFERoutput	1
#define BUFFERtemp	2
#define BUFFERreport	3
#define BUFFERbrowse	4

PrivateVarDef MENU_Choice intBufferChoices[] = {
"Edit           ", " Use editor input buffer",	'e', FORM_menuToForm, ON,
"Output         ", " Use editor output buffer",	'o', FORM_menuToForm, ON,
"Temporary      ", " Use temporary (Region) buffer",	't', FORM_menuToForm, ON,
"Current Report ", " Use Current Report/Profile/Statement", 'c', FORM_menuToForm, ON,
"Last Browse    ", " Use Last Output from Browse Buffer", 'l', FORM_menuToForm, ON,
NULL,
};

#define REGIONlastoutput	0
#define REGIONmarked		1

PrivateVarDef MENU_Choice intRegionChoices[] = {
"Last Output ", " Use last output generated from executing the edit buffer",	'l', FORM_menuToForm, ON,
"Marked      ", " Use the region between the current region markers (actually Region buffer)",	'm', FORM_menuToForm, ON,
NULL,
};

#define PRINTERsystem	0
#define PRINTERlaser	1
#define PRINTERpc	2

#ifdef OLD_PRINTER
PrivateVarDef MENU_Choice intPrinterChoices[] = {
"System      ", " Use the system line printer",	's', FORM_menuToForm, ON,
"Laser       ", " Use the laser printer",	'l', FORM_menuToForm, ON,
"PC          ", " Use the printer attached to the PC",	'p', FORM_menuToForm, ON,
NULL,
};
#endif


#define REPORTcopyPB	2

PrivateVarDef MENU_Choice fileReportMenu[] = {
" Save ",  " Save Current Report to file",		 's', saveReportInt, ON, 
" Print ",  " Print Current Report",			 'p', printReportInt, ON, 
" Copy ",  " Copy output to paste buffer",               'c', copyPasteBufReport, OFF, 
" Last ",  " Use Last Interface Form",			 'l', runReportInterface, ON, 
" Directory ",  " List Files in a given Directory",      'd', listReport, ON,
" sHell",  " Enter an Operating System Sub-Shell (use 'exit' to return)",      'h', ED_subShell, ON,
NULL, 
};

/****************************************
 *****  System Option Descriptions  *****
 ****************************************/

#if RSATTACH
PublicVarDecl int RS_userSystem;
PublicVarDecl int RS_autog;
#endif

#if NOTRSATTACH
PrivateVarDef int RS_userSystem = TRUE;
PrivateVarDef int RS_autog = TRUE;
#endif


/**************************************
 *** GLOBAL WINDOW DECLARATIONS ETC ***
 **************************************/
PrivateVarDef int EDIT_Init = FALSE;
PrivateVarDef int NotDone, ExecutedBuffer;
PrivateVarDef void editorIO();

#define EdKeys \
" Edit: Help(F1) Exec(F2) Window(F3) Interface(F5) Regions(F6) Quit(F9) "
#define OutKeys \
    " Output: Help(F1) Window(F3) Interface(F5) Regions(F6) Quit(F9) "

PrivateVarDef CUR_WINDOW  *StWinOne, 
		      *StWinTwo, 
		      *FileMenuWin, 
		      *RegionMenuWin, 
		      *WindowMenuWin, 
		      *ApplicMenuWin, 
		      *TopWin, *BotWin, *FullWin, 
		      *CurrWin, 
		      *CurrStat,
		      *HelpWin;
		      
PrivateVarDef int	FMWsr, FMWsc, FMWnr, FMWnc,
			RMWsr, RMWsc, RMWnr, RMWnc,
			AMWsr, AMWsc, AMWnr, AMWnc,
			WMWsr, WMWsc, WMWnr, WMWnc;

PrivateVarDef MENU *FileMenu, *RegMenu, *WinMenu, *AppMenu;

PrivateVarDef int TwoWin = FALSE;

PrivateVarDef char CurrentFile[BUF_MaxPathNameChars], *CurrKeys;

/**** buffers *****/

PrivateVarDef LINEBUFFER *Edit, *Region, *Output, *CurrBuffer, 
			 *RecallBuf[NUMRECALL];
PrivateVarDef int	useBrowser = FALSE;

PrivateVarDef int doInterface = FALSE;
PrivateVarDef int RecallIndex = 0;
PublicVarDef LINEBUFFER	*BrowserBuf = NULL, *BrowserInput = NULL;

#define	UploadDir	0
#define DownloadDir	1

#define OVERWRITEmode	0
#define APPENDmode	1
#define ABORTmode	2
#define NOmode		3

PrivateVarDef int	EDIT_fileMode = NOmode;

/***********************************
 *** Macro Defines for Editor ******
 ***********************************/

#if defined(_AIX)
/** AIX curses will leave the terminal in ReverseVideo mode if the
*** last character printed for the screen is in ReverseVideo at the
*** time of a ^z suspend. This trailer character has normal attributes,
*** and if a refresh is done in the signal handler for SIG_TSTP,
*** will ensure that the terminal is in normal mode when a ^Z is typed...
**/
#define AppendStatusTrailer(win)	CUR_waddch(win, ':')
#else
#define AppendStatusTrailer(win)
#endif

#define current_settings(win, stat, keys, buffer)\
{\
    CurrWin = win;\
    CurrStat = stat;\
    CurrKeys = keys;\
    CurrBuffer = buffer;\
}\

#define write_stat(update)\
{ \
    CUR_werase (CurrStat);\
    CUR_wattron(CurrStat, CUR_A_REVERSE);\
    CUR_wmove(CurrStat, 0, 0);\
    CUR_wprintw(CurrStat, CurrKeys);\
    CUR_wattroff(CurrStat, CUR_A_REVERSE);\
    if (!RS_autog)\
    {\
	CUR_wattron(CurrStat, CUR_A_BOLD);\
	CUR_wmove(CurrStat, 0, CUR_WIN_cols(CurrStat)-6);\
	CUR_wprintw(CurrStat, "DEBUG");\
	CUR_wattroff(CurrStat, CUR_A_BOLD);\
    }\
    AppendStatusTrailer(CurrStat);\
    CUR_wnoutrefresh(CurrStat);\
    ((update) ? CUR_doupdate() : 1 );\
}

#define init_help()\
{\
    HelpWin = CUR_newwin(CUR_LINES,CUR_COLS,0,0);\
    CUR_werase(HelpWin);\
    if (NULL != (helpfd=fopen(helpfile, "r")))\
    {\
	CUR_wmove(HelpWin, 0, 0);\
        while (EOF != (c=fgetc(helpfd)))\
    	    CUR_waddch(HelpWin, c);\
	fclose(helpfd);\
    }\
}

#define clear_region(do_start,do_end)\
{ \
    if (do_start && BUF_startcol(CurrBuffer) != -1 && BUF_startrow(CurrBuffer) != NULL){\
	BUF_startcol(CurrBuffer) = -1;\
	BUF_startrow(CurrBuffer) = NULL;\
	}\
    if (do_end && BUF_endcol(CurrBuffer) != -1 && BUF_endrow(CurrBuffer) != NULL){\
	BUF_endcol(CurrBuffer) = -1;\
	BUF_endrow(CurrBuffer) = NULL;\
	}\
}

#define unset_markers(buffer)\
/* clear region markers */\
{\
    BUF_startcol(buffer) = -1;\
    BUF_startrow(buffer) = NULL;\
    BUF_endrow(buffer) = NULL;\
    BUF_endcol(buffer) = -1;\
}

#define one_window()\
{\
    CurrWin = FullWin;\
    BUF_paintWindow(CurrBuffer, CurrWin);\
    CurrStat = StWinOne;\
    write_stat(FALSE);\
    TwoWin = FALSE;\
}

#define two_windows()\
{\
    BUF_paintWindow(Edit, TopWin);\
    CUR_werase (StWinTwo);\
    CUR_wattron(StWinTwo, CUR_A_REVERSE);\
    CUR_wmove(StWinTwo, 0, 0);\
    CUR_wprintw(StWinTwo, EdKeys);\
    CUR_wattroff(StWinTwo, CUR_A_REVERSE);\
    if (!RS_autog)\
    {\
	CUR_wattron(StWinTwo, CUR_A_BOLD);\
	CUR_wmove(StWinTwo, 0, CUR_WIN_cols(StWinTwo)-6);\
	CUR_wprintw(StWinTwo, "DEBUG");\
	CUR_wattroff(StWinTwo, CUR_A_BOLD);\
    }\
    AppendStatusTrailer (StWinTwo);\
    BUF_paintWindow(Output, BotWin);\
    CUR_werase (StWinOne);\
    CUR_wattron(StWinOne, CUR_A_REVERSE);\
    CUR_wmove(StWinOne, 0, 0);\
    CUR_wprintw(StWinOne, OutKeys);\
    CUR_wattroff(StWinOne, CUR_A_REVERSE);\
    AppendStatusTrailer (StWinOne);\
    TwoWin = TRUE;\
    if (CurrBuffer == Edit)\
    {\
        CurrWin = TopWin;\
	CurrStat = StWinTwo;\
    }\
    else\
    {\
        CurrWin = BotWin;\
	CurrStat = StWinOne;\
    }\
}

#define refresh_two()\
{\
    CUR_touchwin(TopWin);\
    CUR_wnoutrefresh(TopWin);\
    CUR_touchwin(StWinTwo);\
    CUR_wnoutrefresh(StWinTwo);\
    CUR_touchwin(BotWin);\
    CUR_wnoutrefresh(BotWin);\
    CUR_touchwin(StWinOne);\
    CUR_wnoutrefresh(StWinOne);\
    CUR_doupdate();\
}

#define refresh_one()\
{\
    CUR_touchwin(FullWin);\
    CUR_wnoutrefresh(FullWin);\
    CUR_touchwin(StWinOne);\
    CUR_wnoutrefresh(StWinOne);\
    CUR_doupdate();\
}

#define hold_screen()\
{\
    CUR_waddstr(HelpWin, "\n         Hit any key to continue.....");\
    CUR_touchwin(HelpWin);\
    CUR_wrefresh(HelpWin);\
    KEY_getkey(FALSE);\
    KEY_QuoteNextKey = FALSE;\
}\

PrivateFnDef int
copyToPasteBuf()
{
    return(FALSE);
}
	
/************************************
 *** EDITOR                       ***
 *** When in top window of editor ***
 ************************************/

PrivateFnDef int editor()
{
    int c, i, j, length, error, cursrow, curscol;
    char *ptr, helpfile[BUF_MaxPathNameChars], *SrcDirName;
    FILE *helpfd;
    
    SrcDirName = getenv(SourceDirEnv);
    if( SrcDirName != NULL )
    {
	strcpy(helpfile, SrcDirName);
	strcat(helpfile,"/");
	strcat(helpfile, EditHelpName);
    }
    else
    	helpfile[0] = '\0';

/*** erase buffer ***/
    if( ExecutedBuffer )
    {
	BUF_eraseBuffer(Edit);
	ExecutedBuffer = FALSE;
    }

/*** initialize region markers ***/
/*    unset_markers(Edit);*/

/*** display initial screen from the top down ***/
    current_settings(TopWin, StWinTwo, EdKeys, Edit);
    twoWindows();
    ERR_clearMsg();

    NotDone = TRUE;
    while (NotDone)
    {

/*** only refresh screen when there are no characters ***/
	if( RepetitionCount > 0 )
	{
	    RepetitionCount--;
	    c = RepetitionKey;
	    if( RepetitionQuote )
	    {
		    if( (c > 0x00) && (c <= 0x7f) )
		    {
			if ( error = BUF_insertChar (Edit, c, CurrWin))
			    ERR_displayError(error);		    
		    }
		    else
		    {
			ERR_displayError(ERR_UndefKey);
			RepetitionCount = 0;
		    }
		    continue;
	    }
	}
	else
	{
	    if (!KEY_cready())
	    {
		if (BUF_repaintScreen(Edit))
		    BUF_paintWindow(Edit, CurrWin);
		BUF_moveCursor(Edit, CurrWin, cursrow, curscol, i);
	/*	CUR_touchwin (CurrWin);*/
		CUR_wnoutrefresh(CurrWin);
		CUR_doupdate();
		CUR_forceSetCursor(CurrWin, cursrow, curscol);
 	    }

	    c = KEY_getkey(TRUE);
	}

	if (BUF_row(Edit) == NULL)
	    length = 0;
	else		
	    length = strlen(BUF_editRow(Edit));
        
	if (ERR_msgDisplayed)
	    ERR_clearMsg();
     
	switch(c) {

	case -1:
	case 0:
	    break;

	case KEY_COPYPB:
	    copyToPasteBuf();
	    break;
	    
	case KEY_REPEAT:
	    if( ERR_promptForRepetition(&i,CurrWin,CUR_WIN_cury(CurrWin),CUR_WIN_curx(CurrWin)) ||
	        (i <= 1) )
	    	break;
	    if( (RepetitionKey = KEY_getkey(FALSE)) == KEY_QUOTE )
	    {
	    	RepetitionQuote = TRUE;
	    	RepetitionKey = KEY_getkey(FALSE);
	    }
	    else
	    	RepetitionQuote = FALSE;
	    RepetitionCount = i;
	    break;

	case KEY_WORDFORW:
	    BUF_forwardWord(Edit,CurrWin);
	    break;

	case KEY_WORDBACK:
	    BUF_backwardWord(Edit,CurrWin);
	    break;

	case KEY_WORDDELC:
	    BUF_deleteCurrWord(Edit,CurrWin);
	    break;

	case KEY_WORDDELP:
	    BUF_deletePrevWord(Edit,CurrWin);
	    break;

	case KEY_ONEWINDOW:
	    oneWindow();
	    refresh_one();
	    RepetitionCount = 0;
	    break;

	case KEY_TWOWINDOW:
	    twoWindows();
	    refresh_two();
	    RepetitionCount = 0;
	    break;

	case KEY_ERASEWIN:
	    eraseWindow();
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_EXEC:
	    ERR_displayMsg(ERR_ExecBuffer);
#if RSATTACH
	    editorIO (Edit, Output, Output);
#endif
	    ExecutedBuffer = TRUE;
	    RepetitionCount = 0;
	    BUF_screenRow(Output) = 0;
	    return(FALSE);

	case KEY_RECALL:
	    recall();
	    RepetitionCount = 0;
	    BUF_resetScreen(Edit, CurrWin);
	    break;

	case KEY_ZEROROW:
	    BUF_screenRow(Edit) = 0;
	    BUF_resetScreen(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_SCROLLUP1:
	    BUF_scrollUp1(Edit, CurrWin);
	    break;

	case KEY_SCROLLDN1:
	    BUF_scrollDown1(Edit, CurrWin);
	    break;

	case KEY_HELP:
	    init_help();
	    CUR_touchwin(HelpWin);
	    CUR_wrefresh(HelpWin);
	    KEY_getkey(FALSE);
	    KEY_QuoteNextKey = FALSE;
	    CUR_delwin(HelpWin);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else 
	    {
		refresh_one();
		write_stat (TRUE);
	    }
	    ERR_clearMsg();
	    RepetitionCount = 0;
	    break;
	    
	case KEY_WINDOW:
	    MENU_handler(WinMenu, WindowMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;
	    
	case CUR_KEY_F4:
	    if( DefaultModule != NULL )
	    {
	    	(*DefaultModule)();
		if (TwoWin) 
		{
		    refresh_two();
		}
		else
		{
		    refresh_one();
		}
	    }
	    else
		ERR_displayError(ERR_UndefKey);
	    RepetitionCount = 0;
	    break;

	case KEY_FILES:
	    MENU_handler(FileMenu, FileMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_REGIONS:
	    MENU_handler(RegMenu, RegionMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_SMENU:
/*	    ERR_displayError(ERR_UndefKey);*/
	    runSysMenu();
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_AMENU:
	    MENU_handler(AppMenu, ApplicMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_QUIT:
	    RepetitionCount = 0;
	    QuitSystem();
	    if( PAGE_ExitSystem )
		return TRUE;
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    break;

	case KEY_PREV:
	    RepetitionCount = 0;
	    return TRUE;

	case KEY_SCRIPTR:
	    RepetitionCount = 0;
	    KEY_beginScriptRead();
	    break;

	case KEY_SCRIPTW:
	    RepetitionCount = 0;
	    KEY_beginScriptWrite();
	    break;

	case KEY_SUBSHELL:
	    RepetitionCount = 0;
	    ED_subShell();
	    if (TwoWin)
	    {
	        refresh_two();
	    }
	    else
		refresh_one();    
	    break;

	case KEY_BKSP:
	    if (error = BUF_backSpace (Edit, CurrWin))
		ERR_displayError(error);
	    break;

	case STD_CTRL('Z'):
	    /*** toggles on and off the autog setting ***/
	    /* autog should only be off to use the debugger so no users can
		set it */

	    if (!RS_userSystem)
	    {
		RS_autog = !RS_autog;
		write_stat (TRUE);
	    }
	    else ERR_displayError(ERR_UndefKey);
	    RepetitionCount = 0;
	    break;
    
	case KEY_VEOL:
	    BUF_goToEOL(Edit, length, CurrWin);
	    RepetitionCount = 0;
	    break;
   
	case KEY_BOL:
	    BUF_goToBOL(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;
   
	case KEY_BKCH:
	case CUR_KEY_LEFT:
	    BUF_keyLeft(Edit, length, CurrWin);
	    break;

	case KEY_NL:
	case CUR_KEY_DOWN:
	    BUF_keyDown(Edit, length, CurrWin);
	    break;
	    
	case KEY_UPL:
	case CUR_KEY_UP:
	    BUF_keyUp(Edit, length, CurrWin);
	    break;

	case KEY_FCH:
	case CUR_KEY_RIGHT:
	    BUF_keyRight(Edit, length, CurrWin);
	    break;

	case KEY_DEL:
	    if (error = BUF_deleteChar (Edit, CurrWin))
		ERR_displayError(error);
	    break;
	    
	    
	case KEY_CR:
	case STD_CTRL('J'):
	    if (error = BUF_carrReturn (Edit, CurrWin))
		ERR_displayError(error);
	    break;

	case KEY_DELEOL:
	    BUF_deleteEOL(Edit, length, CurrWin, ptr);
	    break;

	case KEY_REPAINT:
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    RepetitionCount = 0;
	    break;

	case KEY_SCR_R:
	    BUF_scrollRight(Edit, CurrWin, i);
	    break;

	case KEY_SCR_L:
	    BUF_scrollLeft(Edit, CurrWin);
	    break;
	
	case KEY_SCR_U:
	    BUF_scrollUp(Edit, CurrWin);
	    break;

	case KEY_SCR_D:
	    BUF_scrollDown(Edit, CurrWin, i);
	    break;

	case KEY_TOP:
	    BUF_goToTop(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_BOTTOM:
	    BUF_goToBottom(Edit, CurrWin, i);
	    RepetitionCount = 0;
	    break;

	case KEY_SEARCH:
	    BUF_searchString(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_SNEXT:
	    BUF_searchNext(Edit, CurrWin);
	    break;

	case KEY_SPREV:
	    BUF_searchPrev(Edit, CurrWin);
	    break;

	case KEY_TAB:
#if 0
	    i = (((BUF_col(Edit) + RS_TabSpacing) / RS_TabSpacing) *
			RS_TabSpacing) - BUF_col(Edit);
	    for( j=0 ; j<i ; j++ )
		    if (error = BUF_insertChar(Edit, ' ', CurrWin))
			ERR_displayError(error);		    
#endif
	    if (error = BUF_insertChar(Edit, '\t', CurrWin))
		ERR_displayError(error);		    
	    break;

	case KEY_BEGREGION:
	    beginRegion();
	    RepetitionCount = 0;
	    break;

	case KEY_ENDREGION:
	    endRegion();
	    RepetitionCount = 0;
	    break;

	case KEY_QUERYR:
	    BUF_queryReplaceString(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_REPLACE:
	    BUF_replaceString(Edit, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_READFILE:
	    readFile();
	    doInterface = FALSE;
	    if (TwoWin)
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    break;

	case KEY_SAVEFILE:
	    if( !strlen(CurrentFile) )
	    	ERR_displayStr("No current file",TRUE);
	    else if( (error = BUF_writeFile(CurrBuffer, CurrentFile)) )
		ERR_displayError(error);
	    else
    		ERR_displayMsg(ERR_WroteFile);
	    RepetitionCount = 0;
	    break;

	case KEY_WRITEFILE:
	    saveFile();
	    doInterface = FALSE;
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

        case KEY_TOGGLEWIN:     /* switch to bottom window */
	    NotDone = FALSE;	/* done with this EDIT */
	    break;

        case KEY_INSERTREG:
	    insertRegion();
	    break;

        case KEY_DELREGION:
	    deleteRegion();
	    RepetitionCount = 0;
	    break;

	case KEY_QUOTE:
	    c = KEY_getkey(FALSE);
	    if( (c > 0x00) && (c <= 0x7f) )
	    {
		if ( error = BUF_insertChar (Edit, c, CurrWin))
		    ERR_displayError(error);		    
	    }
	    else
	    { 
		ERR_displayError(ERR_UndefKey);
		RepetitionCount = 0;
	    }
	    break;

	default:
	    if( !(c & ~0x7f) && isprint(c) )
	    {
		if ( error = BUF_insertChar (Edit, c, CurrWin))
		    ERR_displayError(error);		    
	    }
	    else 
	    {
		ERR_displayError(ERR_UndefKey);
		RepetitionCount = 0;
	    }
	    break;

	}   /*** switch ***/

    }	/*** while NotDone ***/

    return (FALSE);
}



/*************************************************
 **********	Window Functions	**********
 *************************************************/

PrivateFnDef void switchWindow () {
    NotDone = FALSE;
}

PrivateFnDef void eraseWindow () {
    int error;
    
    if (error = BUF_eraseBuffer(CurrBuffer))
	ERR_displayError(error);
    else
	CUR_werase(CurrWin);
}

PrivateFnDef void oneWindow () {
    one_window();
}

PrivateFnDef void twoWindows () {
    two_windows();
    refresh_two();
}


/*************************************************
 **********	File Functions		**********
 *************************************************/

PrivateFnDef int readFilePrime (
    void
)
{
	MENU *mptr;

	mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	MENU_currChoice(mptr) = BUFFERfield;
	mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
	MENU_currChoice(mptr) = BUFFERedit;
	mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
	MENU_currChoice(mptr) = FILEfield;
	if (strlen(CurrentFile) > (size_t)0) strncpy(
	    FORM_fieldValue(FORM_field(interfaceForm,SourceType+2+FILEfield)),
	    CurrentFile,
	    FORM_fieldLen(FORM_field(interfaceForm,SourceType+2+FILEfield))
	);
	FORM_currField(interfaceForm) = SourceValue;
	runEditorInterface();
	BUF_resetScreen(CurrBuffer, CurrWin);
}

PrivateFnDef void readFile () {
    int error;

/**** always read into the Edit buffer ****/
    if (CurrBuffer != Edit)
    {
	KEY_putc(KEY_READFILE);
	NotDone = FALSE;
    }
    else
    {
	if( !doInterface && (error = BUF_readFile(CurrBuffer, CurrentFile)) != ERR_AskedForHelp )
	{
		if( error == 0 )
			ERR_displayMsg(ERR_ReadFile);
		else if( error != ERR_AbortedPrompt )
	        	ERR_displayError(error);
		BUF_resetScreen(CurrBuffer, CurrWin);
        	return;
	}
	readFilePrime();
	doInterface = FALSE;
    }
}

PrivateFnDef void readFileInt () {
	doInterface = TRUE;
	readFile();
}
/*---------------------------------------------------------------------*/
PrivateFnDef int checkFileExistence(char *dname) {
	int	response;
	int	mode = OVERWRITEmode;

	if( access(dname,0) == 0 )
	{
		if( access(dname,02) == -1 )
		{
			ERR_displayPause("You do not have permission to write to file");
			return(ABORTmode);
		}
		if( EDIT_fileMode != NOmode )
			return(EDIT_fileMode);
		response = ERR_promptForChar("Destination file exists, Overwrite/Append/Abort (o/a/<F9>)? ","OoAa");
		if( response == KEY_PREV || response == KEY_QUIT )
			return(ABORTmode);
		if( response == 'A' || response == 'a' )
			mode = APPENDmode;
	}
	return(mode);			
}

PrivateFnDef int 
getFileName(current_file, pstr)
char *current_file, *pstr;
{
    char string[BUF_MaxPathNameChars], prompt[BUF_MaxPathNameChars + 80];
    int	error;

    if (!strlen(current_file))
    {
	sprintf(prompt, "No current file, enter file to %s to: ",pstr);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	else
	{
	    if (strlen(string))
	        strcpy(current_file, string);
	    else
		return(ERR_AbortedPrompt);
	}
    }
    else
    {
	sprintf(prompt,
	 "Enter filename or <CR> to %s to %s: ", pstr, current_file);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	if (strlen(string))
	        strcpy(current_file, string);
    }
    return(FALSE);
}

PrivateFnDef void saveFilePrime () {
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (CurrBuffer == Edit ? BUFFERedit : BUFFERoutput);
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = FILEfield;
    if (strlen(CurrentFile) > (size_t)0) strncpy (
	FORM_fieldValue(FORM_field(interfaceForm,DestType+2+FILEfield)),
	CurrentFile,
	FORM_fieldLen(FORM_field(interfaceForm,DestType+2+FILEfield))
    );
    FORM_currField(interfaceForm) = DestValue;
    runEditorInterface();
}

PrivateFnDef void saveFile () {
    int error, mode;
    
    if( !doInterface && (error = getFileName(CurrentFile,"write")) != ERR_AskedForHelp )
    {
    	if( error == 0 )
    	{
    		if( (mode = checkFileExistence(CurrentFile)) == ABORTmode )
    			return;
    		if( (mode == OVERWRITEmode ) &&
    		    (error = BUF_writeFile(CurrBuffer, CurrentFile)) )
		        ERR_displayError(error);
		else if( (mode == APPENDmode) &&
    		    (error = BUF_appendToFile(CurrBuffer, CurrentFile)) )
		        ERR_displayError(error);
		else
    			ERR_displayMsg(ERR_WroteFile);
    	}
    	return;
    }
    saveFilePrime();
}

PrivateFnDef void saveFileInt () {
	doInterface = TRUE;
	saveFile();
	doInterface = FALSE;
}

PrivateFnDef void saveReportInt () {
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (useBrowser ? BUFFERbrowse : BUFFERreport);
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = FILEfield;
    if (strlen(CurrentFile) > (size_t)0) strncpy (
	FORM_fieldValue(FORM_field(interfaceForm,DestType+2+FILEfield)),
	CurrentFile,
	FORM_fieldLen(FORM_field(interfaceForm,DestType+2+FILEfield))
    );
    FORM_currField(interfaceForm) = DestValue;
    runReportInterface();
}

PrivateFnDef void copyPasteBufInt () {
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = REGIONfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+REGIONfield));
    MENU_currChoice(mptr) = REGIONlastoutput;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = PASTEBUFfield;
    FORM_currField(interfaceForm) = DestValue;
    runEditorInterface();
}

PrivateFnDef void copyPasteBufReport () {
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (useBrowser ? BUFFERbrowse : BUFFERreport);
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = PASTEBUFfield;
    FORM_currField(interfaceForm) = DestValue;
    runReportInterface();
}

/*---------------------------------------------------------------------*/

PrivateVarDef int	RefreshAfterList = TRUE;

/* 1) asks the user for a directory name    */
/* 2) displays an "ll" of that directory    */
/* 3) waits for the user to hit any key	    */
/* 4) returns to editing session	    */
PrivateFnDef void listFile () {
   char Directory[BUF_MaxPathNameChars], buf[BUF_MaxPathNameChars + 80];
   int pipe1[2];
   int pipe2[2];
   FILE *fp;
   int onechar;
    
    
    if (BUF_listFile(CurrBuffer, Directory))
       ERR_clearMsg();
    else
    {
       ERR_clearMsg();
       CUR_wrefresh(ERR_Window);
       if (isBlank(Directory))
       	   strcpy(Directory,".");
       HelpWin = CUR_newwin(CUR_LINES,CUR_COLS,0,0);
       CUR_werase(HelpWin);
       CUR_wmove(HelpWin, 0, 0);       /* cursor at top left */

       pipe(pipe1);
       pipe(pipe2);
       if (vfork()==0) {
	    close(pipe1[READ]);	/* child won't be reading */
	    close(WRITE);
	    dup(pipe1[WRITE]);	/* child's stdout is the write end of pipe1 */
	    close(pipe1[WRITE]);

	    close(pipe2[READ]);	/* child won't be reading */
            close(STDERR);
	    dup(pipe2[WRITE]);	/* child's stderr is the write end of pipe2 */
	    close(pipe2[WRITE]);

	    sprintf(buf,"/bin/csh -if -c \"/bin/ls -p -l %s\"",Directory);
	    execl ("/bin/sh", "ll", "-c", buf, NULL);
	    _exit(1);
       } /* if */
       else {
	    close(pipe1[WRITE]);	/* parent won't be writing */
	    close(pipe2[WRITE]);

 	    fp = fdopen(pipe1[READ],"r");		/* Normal exec of ls */
	    while ( (onechar = getc(fp)) != EOF )
	        PrintScreen(onechar);
	    fclose(fp);

	    fp = fdopen(pipe2[READ],"r");		/* Error with ls */
	    while ( (onechar = getc(fp)) != EOF )
	        PrintScreen(onechar);
	    fclose(fp);

	    hold_screen();	/* macro asking user to hit key to cont. */
	    CUR_delwin(HelpWin);
	    if( RefreshAfterList )
	    {
   	    	if (TwoWin)
            	{
		    refresh_two();
            	}
   	    	else
	    	{
		    refresh_one();
		    write_stat(TRUE);
		}
	    }
       } /* else */
    } /* else */
}

PrivateFnDef void listReport () {
	RefreshAfterList = FALSE;
	listFile();
	RefreshAfterList = TRUE;
}
/*---------------------------------------------------------------------*/
PrivateFnDef void PrintScreen (int c) {
    int y;

     y = CUR_WIN_cury(HelpWin);
     if (y==(CUR_WIN_rows(HelpWin)-3))       		/* screen is full */
     {
	 hold_screen();		/* macro asking user to hit key to cont. */
	 CUR_werase(HelpWin);
     }  
     CUR_waddch(HelpWin, c);
}
/*---------------------------------------------------------------------*/

#if 0
PrivateFnDef int appendFilePrime()
{
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (CurrBuffer == Edit ? BUFFERedit : BUFFERoutput);
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = FILEfield;
    if( strlen(CurrentFile) > 0 )
	strncpy(FORM_fieldValue(FORM_field(interfaceForm,DestType+2+FILEfield)),
		CurrentFile,
		FORM_fieldLen(FORM_field(interfaceForm,DestType+2+FILEfield)));
    FORM_currField(interfaceForm) = DestValue;
    EDIT_fileMode = APPENDmode;
    runEditorInterface();
    EDIT_fileMode = NOmode;
}

PrivateFnDef int appendFile (
    void
)
{
    int error;
    
    if( !doInterface && (error = getFileName(CurrentFile,"append")) != ERR_AskedForHelp )
    {
    	if( error == 0 )
    	{
    		EDIT_fileMode = APPENDmode;
    		if( checkFileExistence(CurrentFile) == ABORTmode )
    		{
    			EDIT_fileMode = NOmode;
    			return;
    		}
		EDIT_fileMode = NOmode;
    		if( error = BUF_appendToFile(CurrBuffer, CurrentFile) )
		        ERR_displayError(error);
		else
    			ERR_displayMsg(ERR_WroteFile);
    	}
        return;
    }
    appendFilePrime();
}

PrivateFnDef int appendFileInt (
    void
)
{
	doInterface = TRUE;
	appendFile();
	doInterface = FALSE;
}
#endif


/*---------------------------------------------------------------------*/
#if 0
PrivateFnDef int currentFile (
    void
)
{
    char buffer[BUF_MaxPathNameChars + 80];
    
    sprintf(buffer, " Current file is: %s ", CurrentFile);
    ERR_displayPause(buffer);
    return(FALSE);
}
#endif
/*---------------------------------------------------------------------*/

/*************************************************
 **********	Region Functions	**********
 *************************************************/

PrivateFnDef void beginRegion () {
    unset_markers(CurrBuffer);
    if BUF_editted(CurrBuffer)
      	BUF_adjustRow(CurrBuffer);
    BUF_startrow(CurrBuffer) = BUF_row(CurrBuffer);
    BUF_startcol(CurrBuffer) = BUF_col(CurrBuffer);
    ERR_displayMsg(ERR_BegRegion);
}
/*---------------------------------------------------------------------*/

PrivateFnDef int test_region()
{
 /* Checks:                                                            */
 /*   1) for valid values in the region vars (Ex. no negative numbers) */
 /*   2) that start_region != end_region                               */
 /*   3) that start_region precedes end_region                         */

    int col, foundit;
    char *row;

    if (BUF_startcol(CurrBuffer) == -1 || BUF_endcol(CurrBuffer) == -1 ||
        BUF_startrow(CurrBuffer) == NULL || BUF_endrow(CurrBuffer) == NULL)
    {
	ERR_displayError (ERR_NoRegion);
	unset_markers(CurrBuffer);
	return(TRUE);
    }
    if (BUF_startrow(CurrBuffer) == BUF_endrow(CurrBuffer) && BUF_startcol(CurrBuffer) == BUF_endcol(CurrBuffer))
    {
	ERR_displayError (ERR_BadRegion);
	return(TRUE);
    }
    if (BUF_startrow(CurrBuffer) == BUF_endrow(CurrBuffer)) /* If start and end regions are on */
    {                                  		/* the same row, make sure the     */
	if (BUF_startcol(CurrBuffer) > BUF_endcol(CurrBuffer)) /* start column precedes the end   */
	{                              		/* column. If they are not in the  */
	    col = BUF_startcol(CurrBuffer);	/* correct order, they are switched*/
	    BUF_startcol(CurrBuffer) = BUF_endcol(CurrBuffer);
	    BUF_endcol(CurrBuffer) = col;
	}
    }
    else
    {                                  /* Make sure StartRow precedes     */
	row = BUF_startrow(CurrBuffer);/* EndRow.  If this is not true,   */
	foundit = FALSE;               /* swap end region with start      */
	while (row != NULL)            /* region.                         */
	{
	    if (row == BUF_endrow(CurrBuffer))
	    {
		foundit = TRUE;
		break;
	    }
	    row = BUF_nextLine(row);
	}
	if (!foundit)
	{
	    row = BUF_startrow(CurrBuffer);
	    BUF_startrow(CurrBuffer) = BUF_endrow(CurrBuffer);
	    BUF_endrow(CurrBuffer) = row;
	    col = BUF_startcol(CurrBuffer);
	    BUF_startcol(CurrBuffer) = BUF_endcol(CurrBuffer);
	    BUF_endcol(CurrBuffer) = col;
	}
    }
    return(FALSE);
}

/*---------------------------------------------------------------------*/
/* Used by the buffers module to check validity of regions             */

PublicFnDef void ED_unsetMarkers(LINEBUFFER *buffer) {
	unset_markers(buffer);
}

PrivateFnDef void endRegion () {
    int error;

    if BUF_editted(CurrBuffer)
    {
    	if( BUF_startrow(CurrBuffer) == BUF_row(CurrBuffer) )
    	{
    	    ERR_displayStr(" Begin mark has been invalidated.", TRUE);
    	    ED_unsetMarkers(CurrBuffer);
    	    return;
    	}
      	BUF_adjustRow(CurrBuffer);
    }
    BUF_endrow(CurrBuffer) = BUF_row(CurrBuffer);
    BUF_endcol(CurrBuffer) = BUF_col(CurrBuffer);
    if (test_region ())
	return;				  /* error with region */
    if (error = BUF_eraseBuffer(Region))  /* erases previous region */
    {
	ERR_displayError(error);
    }
    else
    {
	if (error = BUF_appendRegion(Region, CurrBuffer,
			 BUF_startrow(CurrBuffer), BUF_startcol(CurrBuffer), BUF_endrow(CurrBuffer), BUF_endcol(CurrBuffer)))
	{
	    ERR_displayError(error);
	}
	else
	    ERR_displayMsg(ERR_EndRegion);
    }
}
/*---------------------------------------------------------------------*/

PrivateFnDef void clearRegion () {
    unset_markers(CurrBuffer);
    BUF_eraseBuffer(Region);
    ERR_displayMsg(ERR_ClearRegion);
}
/*---------------------------------------------------------------------*/

PrivateFnDef void deleteRegion () {
    int error;
    
    if (test_region ())         /* needed because beginRegion doesn't do it*/
	return;			/* error with region */
    if (error = BUF_deleteRegion(CurrBuffer, BUF_startrow(CurrBuffer), BUF_startcol(CurrBuffer), 
					  BUF_endrow(CurrBuffer), BUF_endcol(CurrBuffer)))
        ERR_displayError(error);
    else
    {
	BUF_resetScreen(CurrBuffer, CurrWin);
	ERR_displayMsg(ERR_DeleteRegion);
    }
    unset_markers(CurrBuffer);
}
/*---------------------------------------------------------------------*/

PrivateFnDef void insertRegion () {
    int error;
    
    if (BUF_firstLine(Region) == NULL && BUF_lastLine(Region) == NULL)
    {
	ERR_displayError(ERR_NoRegion);
	return;
    }
    if (error = BUF_insertRegion(CurrBuffer, CurrWin, Region))
	ERR_displayError(error);
    else
	ERR_displayMsg(ERR_InsertRegion);
    BUF_resetScreen(CurrBuffer, CurrWin);
}
/*---------------------------------------------------------------------*/

PrivateFnDef void runRegion () {

/*
    if (CurrBuffer == Output)
    {
        ERR_displayError(ERR_UndefFn);
	return(FALSE);
    }
    if (test_region())
	return(TRUE);
*/
    if (BUF_firstLine(Region) == NULL && BUF_lastLine(Region) == NULL)
    {
	ERR_displayError(ERR_NoRegion);
	return;
    }
    ERR_displayMsg(ERR_ExecRegion);
#if RSATTACH
    editorIO(Region, Output, Output);
#endif
    if (CurrBuffer != Output)
	    NotDone = FALSE;
}

/*---------------------------------------------------------------------*/

static char uplTemplate[] = "/tmp/uplXXXXXX" ;
static char dnlTemplate[] = "/tmp/dnlXXXXXX" ;

#if 0
PrivateFnDef int uploadRegion (
    void
)
{
    int error;
    char	*s, tname[128], pcname[128], *mktemp(), cmd[128];
    
    strcpy(tname,uplTemplate);
    if( !strlen((s = mktemp(tname))) )
    {
	ERR_displayError(ERR_OpenError);
	return(TRUE);
    }

    if (error = ERR_promptForString("Enter name of file on PC to upload (F9 to abort): ", pcname, TRUE))
    {
    	if( error == ERR_AbortedPrompt )
	    return(FALSE);
	else
	    return(error);
    }
    else if (!strlen(pcname))
	return(FALSE);
    ERR_clearMsg();

    sprintf(cmd,"/bin/csh -if -c \"/usr/local/bin/pcupload %s %s\"",s,pcname);
    CUR_saveterm();
    CUR_resetterm();
    RS_system(cmd);
    CUR_fixterm();
    CUR_noecho();
    CUR_nonl();
    CUR_cbreak();
    CUR_clearok(CUR_curscr);
    CUR_wrefresh(CUR_curscr);
    if (TwoWin)
    {
        refresh_two();
    }
    else
	refresh_one();    
    ERR_clearMsg();
    BUF_eraseBuffer(Region);
    if( (error = BUF_getFile( Region, s )) )
    {
	ERR_displayStr(" Unable to access temporary file",TRUE);
	return(TRUE);
    }
    remove(s);
    return(FALSE);
}
#endif


#if 0
PrivateFnDef int downloadRegion (
    void
)
{
    int error;
    char	*s, tname[128], pcname[128], *mktemp(), cmd[128];
    
    if (BUF_firstLine(Region) == NULL && BUF_lastLine(Region) == NULL)
    {
	ERR_displayError(ERR_NoRegion);
	return(TRUE);
    }

    strcpy(tname,dnlTemplate);
    if( !strlen((s = mktemp(tname))) )
    {
	ERR_displayError(ERR_OpenError);
	return(TRUE);
    }
    if( (error = BUF_writeFile( Region, s )) )
    {
	ERR_displayStr(" Unable to access temporary file",TRUE);
	return(TRUE);
    }
    if (error = ERR_promptForString("Enter name of file (.PRN added automatically) (F9 to abort): ", pcname, TRUE))
    {
    	if( error == ERR_AbortedPrompt )
	    return(FALSE);
	else
	    return(error);
    }
    else if (!strlen(pcname))
	return(FALSE);
    ERR_clearMsg();
    strcat(pcname,".PRN");

    sprintf(cmd,"/bin/csh -if -c \"/usr/local/bin/pcdownload %s %s\"",s,pcname);
    CUR_saveterm();
    CUR_resetterm();
    RS_system(cmd);
    CUR_fixterm();
    CUR_noecho();
    CUR_nonl();
    CUR_cbreak();
    CUR_clearok(CUR_curscr);
    CUR_wrefresh(CUR_curscr);
    if (TwoWin)
    {
        refresh_two();
    }
    else
	refresh_one();    
    ERR_clearMsg();
    remove(s);
    return(FALSE);
}
#endif


PrivateFnDef int saveRegionPrime (
    void
)
{
    MENU *mptr;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = REGIONfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+REGIONfield));
    MENU_currChoice(mptr) = REGIONmarked;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = FILEfield;
    if (strlen(CurrentFile) > (size_t)0)
	strncpy(FORM_fieldValue(FORM_field(interfaceForm,DestType+2+FILEfield)),
		CurrentFile,
		FORM_fieldLen(FORM_field(interfaceForm,DestType+2+FILEfield)));
    FORM_currField(interfaceForm) = DestValue;
    runEditorInterface();
}

PrivateFnDef int saveRegion (
    void    
)
{
    int error, mode;
    
    if (BUF_firstLine(Region) == NULL && BUF_lastLine(Region) == NULL)
    {
	ERR_displayError(ERR_NoRegion);
	return(TRUE);
    }
    if( (error = getFileName(CurrentFile,"write")) != ERR_AskedForHelp )
    {
    	if( error == 0 )
    	{
    		if( (mode = checkFileExistence(CurrentFile)) == ABORTmode )
    			return(FALSE);
    		if( (mode == OVERWRITEmode) &&
    		    (error = BUF_writeFile(Region, CurrentFile)) )
		        ERR_displayError(error);
		else if( (mode == APPENDmode) &&
    		    (error = BUF_appendToFile(Region, CurrentFile)) )
		        ERR_displayError(error);
		else
    			ERR_displayMsg(ERR_WroteFile);
    	}
        return(TRUE);
    }
    saveRegionPrime();
    return(FALSE);
}

PrivateFnDef void saveRegionInt () {
	doInterface = TRUE;
	saveRegion();
	doInterface = FALSE;
}

PrivateFnDef void printRegion () {
#if 0
    PAGE *page;
    int i;

/*** print needs to know the current screen contents,  so make page ***/    
    if (TwoWin)
    {
	PAGE_createPage(page, 4, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(page, 0, NULL, TopWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 1, NULL, StWinTwo, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 2, NULL, BotWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 3, NULL, StWinOne, PAGE_Init, NULL, FALSE);
    }
    else
    {
	PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(page, 0, NULL, FullWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 1, NULL, StWinOne, PAGE_Init, NULL, FALSE);
    }
    
    BUF_printBuffer(Region, page, -1);

    PAGE_deletePage(page, i);
#endif

    MENU *mptr;
    
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = REGIONfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+REGIONfield));
    MENU_currChoice(mptr) = REGIONlastoutput;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = PRINTERfield;
    FORM_currField(interfaceForm) = DestValue;
    runEditorInterface();
}

/**************************************************************
 ***************	Misc. Functions		***************
 *************************************************************/
PrivateFnDef void editorIO(inbuffer, outbuffer, errbuffer)
LINEBUFFER *inbuffer, *outbuffer, *errbuffer;
{
    char *row, *ptr, *ptr2;
    int  OMemoryError = FALSE, RMemoryError = FALSE;

    if (RS_autog || useBrowser)
    {
        if( (BUF_LastOutput(outbuffer) = BUF_appendLine(outbuffer, "Input:")) == NULL )
            OMemoryError = TRUE;
    }
    else
    	BUF_LastOutput(outbuffer) = NULL;

    row = BUF_firstLine(inbuffer);
    if( !useBrowser )
	BUF_eraseBuffer(RecallBuf[RecallIndex]);
    while (row != NULL)
    {
	if (row == BUF_row(inbuffer))
	    ptr2 = BUF_editRow(inbuffer);
	else
	    ptr2 = BUF_line(row);
	if( !useBrowser && !RMemoryError )
	    if( BUF_appendLine(RecallBuf[RecallIndex], ptr2) == NULL )
	        RMemoryError = TRUE;
	if( !OMemoryError )
	{
	    if( (ptr = BUF_appendLine(outbuffer, ptr2)) == NULL )
	    	OMemoryError = TRUE;
	    else
	    	BUF_LastOutput(outbuffer) = ptr;
	}
	row = BUF_nextLine(row);
    }
    if( !useBrowser )
	RecallIndex = (RecallIndex + 1) % NUMRECALL;

    if( (RS_autog || useBrowser) && !OMemoryError )
	BUF_LastOutput(outbuffer) = BUF_appendLine(outbuffer, "Output:");

    row = BUF_firstLine(inbuffer);
    while (row != NULL)
    {
	if (row == BUF_row(inbuffer))
	    ptr2 = BUF_editRow(inbuffer);
	else
	    ptr2 = BUF_line(row);
	RS_writeLine(ptr2);
	RS_readOutput(outbuffer, errbuffer);
	row = BUF_nextLine(row);
    }

    if (RS_autog || useBrowser)
    {
    	if( BUF_LastOutput(outbuffer) != NULL )
    	{
	    BUF_changeRow(outbuffer, BUF_LastOutput(outbuffer));
	    BUF_setCol(outbuffer, (CUR_WINDOW *)NULL, 0);
	}
	RS_compile();
	RS_readOutput(outbuffer, errbuffer);
	if( (BUF_LastOutput(outbuffer) != NULL) &&
	    (BUF_nextLine(BUF_LastOutput(outbuffer)) != NULL) )
	    BUF_LastOutput(outbuffer) = BUF_nextLine(BUF_LastOutput(outbuffer));
    }
    else
    {
	if( BUF_LastOutput(outbuffer) != NULL )
	    BUF_changeRow(outbuffer, BUF_LastOutput(outbuffer));
	if( (BUF_row(outbuffer) != NULL) &&
	    (BUF_nextLine(BUF_row(outbuffer)) != NULL) )
	    BUF_changeRow(outbuffer, BUF_nextLine(BUF_row(outbuffer)));
	BUF_LastOutput(outbuffer) = BUF_row(outbuffer);
	BUF_setCol(outbuffer, (CUR_WINDOW *)NULL, 0);
    }
}

PublicFnDef void EDIT_browserIO() {
	useBrowser = TRUE;
	editorIO(BrowserInput, BrowserBuf, BrowserBuf);
	useBrowser = FALSE;
}

/*---------------------------------------------------------------------*/

PublicFnDef void ED_subShell () {
    ERR_displayStr(" Entering subshell...",FALSE);
    CUR_saveterm();
    CUR_resetterm();

    RS_system("/bin/csh -i");	/* vision's version of a system call */
    
    /***** Exited from shell, setup the screen ... *****/
    CUR_fixterm();
    CUR_noecho();
    CUR_nonl();
    CUR_cbreak();
    CUR_clearok(CUR_curscr);
    ERR_clearMsg();
/*    CUR_wrefresh(CUR_curscr);*/

    /***** 
     * This code (I think) is unneccessay but it may be needed to keep the
     * Research System in sync.
     *****/
    RS_dumpOutput ();
    KEY_setKeypad();
}
/*---------------------------------------------------------------------*/

PrivateFnDef int recall (
    void
)
{
    int i, error;
    
    if( RepetitionCount >= NUMRECALL )
    {
	ERR_displayStr(" Not that many recall buffers",TRUE);
	return FALSE;
    }
    i = (RecallIndex - RepetitionCount - 1) % NUMRECALL;
    while( i<0 )
	i += NUMRECALL;
    if( BUF_firstLine(RecallBuf[i]) == NULL)
    {
	ERR_displayError(ERR_RecallError);
	return FALSE;
    }

    if (error = BUF_insertRegion(CurrBuffer, CurrWin, RecallBuf[i]))
	ERR_displayError(error);
    else
	ERR_displayMsg(ERR_InsertRegion);
    BUF_resetScreen(CurrBuffer, CurrWin);
    return FALSE;
}
/*---------------------------------------------------------------------*/

PrivateFnDef void printBuffer () {
#if 0
    PAGE *page;
    int i;

/*** print needs to know the current screen contents,  so make page ***/    
    if (TwoWin)
    {
	PAGE_createPage(page, 4, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(page, 0, NULL, TopWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 1, NULL, StWinTwo, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 2, NULL, BotWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 3, NULL, StWinOne, PAGE_Init, NULL, FALSE);
    }
    else
    {
	PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(page, 0, NULL, FullWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 1, NULL, StWinOne, PAGE_Init, NULL, FALSE);
    }
    
    BUF_printBuffer(CurrBuffer, page, -1);

    PAGE_deletePage(page, i);
#endif

    MENU *mptr;
    
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (CurrBuffer == Edit ? BUFFERedit : BUFFERoutput);

    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = PRINTERfield;
    FORM_currField(interfaceForm) = DestValue;

    runEditorInterface();
}

PrivateFnDef void printReportInt () {
    MENU *mptr;
    
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
    MENU_currChoice(mptr) = BUFFERfield;
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
    MENU_currChoice(mptr) = (useBrowser ? BUFFERbrowse : BUFFERreport);
    mptr = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
    MENU_currChoice(mptr) = PRINTERfield;
    FORM_currField(interfaceForm) = DestValue;
    runReportInterface();
}

/***************************************
 *** SESSION                         ***
 *** When in output window of editor ***
 **************************************/

PrivateFnDef int session () {
    int c, i, length, cursrow, curscol, error;
    char helpfile[BUF_MaxPathNameChars], *SrcDirName;
    FILE *helpfd;

    SrcDirName = getenv(SourceDirEnv);
    if( SrcDirName != NULL )
    {
	strcpy(helpfile, SrcDirName);
	strcat(helpfile,"/");
	strcat(helpfile, OutHelpName);
    }
    else
    	helpfile[0] = '\0';

/***** initialize, display the windows from top down *****/
/*    BUF_screenRow(Output) = 0;*/
    current_settings(BotWin, StWinOne, OutKeys, Output);
    twoWindows();
    ERR_clearMsg();

/*    unset_markers(Output);*/
    
/***** the input loop *****/
    NotDone = TRUE;
    while (NotDone)
    {
	if( RepetitionCount > 0 )
	{
	    if( RepetitionQuote )
		return(FALSE);
	    RepetitionCount--;
	    c = RepetitionKey;
	}
	else
	{
	    if (!KEY_cready())
	    {
		if (BUF_repaintScreen(Output))
		    BUF_paintWindow(Output, CurrWin);
		BUF_moveCursor(Output, CurrWin, cursrow, curscol, i);
    /*	        CUR_touchwin(CurrWin);*/
		CUR_wnoutrefresh(CurrWin);
		CUR_doupdate();
		CUR_forceSetCursor(CurrWin, cursrow, curscol);
	    }

	    c = KEY_getkey(TRUE);
	}

	if (BUF_row(Output) == NULL)
	    length = 0;
	else		
	    length = strlen(BUF_editRow(Output));
        
	if (ERR_msgDisplayed)
	    ERR_clearMsg();
     
	switch(c) {

	case 0:
	case -1:
	    break;

	case KEY_COPYPB:
	    copyToPasteBuf();
	    break;
	    
	case KEY_REPEAT:
	    if( ERR_promptForRepetition(&i,CurrWin,CUR_WIN_cury(CurrWin),CUR_WIN_curx(CurrWin)) ||
	        (i <= 1) )
	    	break;
	    if( (RepetitionKey = KEY_getkey(FALSE)) == KEY_QUOTE )
	    {
	    	RepetitionQuote = TRUE;
	    	RepetitionKey = KEY_getkey(FALSE);
	    }
	    else
	    	RepetitionQuote = FALSE;
	    RepetitionCount = i;
	    break;

	case KEY_WORDFORW:
	    BUF_forwardWord(Output,CurrWin);
	    break;

	case KEY_WORDBACK:
	    BUF_backwardWord(Output,CurrWin);
	    break;

	case KEY_READFILE:
	    KEY_putc(KEY_READFILE);
	    NotDone = FALSE;
	    break;

	case KEY_SAVEFILE:
	    if( !strlen(CurrentFile) )
	    	ERR_displayStr("No current file",TRUE);
	    else if( (error = BUF_writeFile(CurrBuffer, CurrentFile)) )
		ERR_displayError(error);
	    else
    		ERR_displayMsg(ERR_WroteFile);
	    RepetitionCount = 0;
	    break;

	case KEY_WRITEFILE:
	    saveFile();
	    doInterface = FALSE;
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_ONEWINDOW:
	    oneWindow();
	    refresh_one();
	    RepetitionCount = 0;
	    break;

	case KEY_TWOWINDOW:
	    twoWindows();
	    refresh_two();
	    RepetitionCount = 0;
	    break;

	case KEY_ERASEWIN:
	    eraseWindow();
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_EXEC:
	    ERR_displayError(ERR_UndefKey);
	    RepetitionCount = 0;
	    break;
	    
	case KEY_ZEROROW:
	    BUF_screenRow(Output) = 0;
	    BUF_resetScreen(Output, CurrWin);
	    break;

	case KEY_SCROLLUP1:
	    BUF_scrollUp1(Output, CurrWin);
	    break;

	case KEY_SCROLLDN1:
	    BUF_scrollDown1(Output, CurrWin);
	    break;

	case KEY_HELP:
	    init_help();
	    CUR_touchwin(HelpWin);
	    CUR_wrefresh(HelpWin);
	    KEY_getkey(FALSE);
	    KEY_QuoteNextKey = FALSE;
	    CUR_delwin(HelpWin);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    ERR_clearMsg();
	    RepetitionCount = 0;
	    break;

	case KEY_WINDOW: 
	    MENU_handler(WinMenu, WindowMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_FILES:
	    MENU_handler(FileMenu, FileMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;
	
	case KEY_REGIONS:
	    MENU_handler(RegMenu, RegionMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case CUR_KEY_F4:
	    if( DefaultModule != NULL )
	    {
	    	(*DefaultModule)();
		if (TwoWin) 
		{
		    refresh_two();
		}
		else
		{
		    refresh_one();
		}
	    }
	    else
		ERR_displayError(ERR_UndefKey);
	    RepetitionCount = 0;
	    break;

	case KEY_SMENU:
/*	    ERR_displayError(ERR_UndefKey);*/
	    runSysMenu();
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_AMENU:
	    MENU_handler(AppMenu, ApplicMenuWin, PAGE_Input);
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    RepetitionCount = 0;
	    break;

	case KEY_QUIT:
	    RepetitionCount = 0;
	    QuitSystem();
	    if( PAGE_ExitSystem )
		return TRUE;
	    if (TwoWin) 
	    {
		refresh_two();
	    }
	    else
	    {
		refresh_one();
	    }
	    break;

	case KEY_PREV:
	    RepetitionCount = 0;
	    return (TRUE);

	case KEY_SCRIPTR:
	    RepetitionCount = 0;
	    KEY_beginScriptRead();
	    break;

	case KEY_SCRIPTW:
	    RepetitionCount = 0;
	    KEY_beginScriptWrite();
	    break;

	case KEY_SUBSHELL:
	    RepetitionCount = 0;
	    ED_subShell();
	    if (TwoWin)
	    {
	        refresh_two();
	    }
	    else
		refresh_one();    
	    break;

	case KEY_REPAINT:
	    RepetitionCount = 0;
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    break;

	case KEY_VEOL:
	    BUF_goToEOL(Output, length, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_BOL:
	    BUF_goToBOL(Output, CurrWin);
	    RepetitionCount = 0;
	    break;

	case CUR_KEY_DOWN:
	case KEY_NL:
	    BUF_keyDown(Output, length, CurrWin);
	    break;
	    
	case KEY_UPL:
	case CUR_KEY_UP:
	    BUF_keyUp(Output, length, CurrWin);
	    break;

	case KEY_BKCH:
	case CUR_KEY_LEFT:
	    BUF_keyLeft(Output, length, CurrWin);
	    break;
   
	case KEY_FCH:
	case CUR_KEY_RIGHT:
	    BUF_keyRight(Output, length, CurrWin);
	    break;

	case KEY_SCR_R:
	    BUF_scrollRight(Output, CurrWin, i);
	    break;

	case KEY_SCR_L:
	    BUF_scrollLeft(Output, CurrWin);
	    break;
	
	case KEY_SCR_U:
	    BUF_scrollUp(Output, CurrWin);
	    break;

	case KEY_SCR_D:
	    BUF_scrollDown(Output, CurrWin, i);
	    break;

	case KEY_TOP:
	    BUF_goToTop(Output, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_BOTTOM:
	    BUF_goToBottom(Output, CurrWin, i);
	    RepetitionCount = 0;
	    break;

	case KEY_SEARCH:
	    BUF_searchString(Output, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_SNEXT:
	    BUF_searchNext(Output, CurrWin);
	    break;

	case KEY_SPREV:
	    BUF_searchPrev(Output, CurrWin);
	    break;

        case KEY_SPACE:         /* switch to top window */
	    NotDone = FALSE;	/* done with this SESSION */
	    break;

        case KEY_TOGGLEWIN:     /* switch to top window */
	    NotDone = FALSE;	/* done with this SESSION */
	    break;

	case KEY_BEGREGION:
	    beginRegion();
	    RepetitionCount = 0;
	    break;

	case KEY_ENDREGION:
	    endRegion();
	    RepetitionCount = 0;
	    break;

        case KEY_DELREGION:
	    deleteRegion();
	    RepetitionCount = 0;
	    break;

	case KEY_RECALL:
	    if( RepetitionCount > 0 )
		RepetitionCount++;
	    else
		KEY_putc(c);
	    NotDone = FALSE;	/* done with this SESSION */
	    break;

	case KEY_QUERYR:
	    BUF_queryReplaceString(Output, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_REPLACE:
	    BUF_replaceString(Output, CurrWin);
	    RepetitionCount = 0;
	    break;

	case KEY_QUOTE:
	    KEY_QuoteNextKey = FALSE;
	    KEY_putc(c);
	    NotDone = FALSE;
	    break;

	default:
	    if( !(c & ~0x7f) && (isprint(c) || (c == '\t')) )
	    {
	        KEY_putc(c);	/* stores 1 char. which will be printed */
		return FALSE;   /* in the top (edit) window             */
	    }
	    ERR_displayError(ERR_UndefKey);
	    RepetitionCount = 0;
	    break;
	}   /**** switch ****/

    }   /**** while ****/

    return(FALSE);
}

PublicFnDef int EDIT_reportFileMenu (
    PAGE *			page,
    int				doBrowse
)
{
    int		i, j, longest, sr, sc;
    MENU	*menu;
    CUR_WINDOW	*menuWin;

    MENU_makeMenu(menu, fileReportMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(menu) =" Interface Menu: ";
    i = MENU_choiceCount(menu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(menu)) )
	j = strlen(MENU_title(menu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    menuWin = CUR_newwin(i, j, sr, sc);
    reportPage = page;
    useBrowser = doBrowse;
    MENU_handler(menu,menuWin,PAGE_Input);
    useBrowser = FALSE;
    MENU_deleteMenu(menu, i);
    CUR_delwin(menuWin);
}

PrivateFnDef void initEditor() {
    int longest, i, j, sr, sc;

/*    ERR_displayStr(" Initializing editor...");*/

/***** Initialize buffers *****/
    Edit = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(Edit) = MAX_OUTPUT_LINE_SIZE;
    BUF_initBuffer(Edit, IBUFSIZE);

    Region = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(Region) = MAX_OUTPUT_LINE_SIZE;
    BUF_initBuffer(Region, RBUFSIZE);

    BrowserInput = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(BrowserInput) = MAX_OUTPUT_LINE_SIZE;
    BUF_initBuffer(BrowserInput, 4096);

    for( i=0 ; i<NUMRECALL ; i++ )
    {
	RecallBuf[i] = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
	BUF_maxLineSize(RecallBuf[i]) = MAX_OUTPUT_LINE_SIZE;
	BUF_initBuffer(RecallBuf[i], RBUFSIZE);
	BUF_eraseBuffer(RecallBuf[i]);
    }
    RecallIndex = 0;    

    BrowserBuf = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(BrowserBuf) = MAX_OUTPUT_LINE_SIZE;
    BUF_initBuffer(BrowserBuf, RBUFSIZE);

    Output = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(Output) = MAX_OUTPUT_LINE_SIZE;
    BUF_initBuffer(Output, OBUFSIZE);

    /* initialize the printers,  so the menu choices can be turned off */
    if (checkPrinters () == 0)
        {
        /* turn off the printer choice for each menu */
        fileMenu[2].active = OFF;
        regionMenu[7].active = OFF;
        dstChoices[3].active = OFF;
        fileReportMenu[1].active = OFF;
        }

/**** initialize menus ****/
    MENU_makeMenu (
	FileMenu, fileMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j
    );
    MENU_title(FileMenu) =" Interface Menu: ";
    i = MENU_choiceCount(FileMenu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(FileMenu)) )
	j = strlen(MENU_title(FileMenu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    FMWnr = i; FMWnc = j; FMWsr = sr; FMWsc = sc;
    MENU_makeMenu (
	WinMenu, windowMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j
    );
    MENU_title(WinMenu) =" Window Menu: ";
    i = MENU_choiceCount(WinMenu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(WinMenu)) )
	j = strlen(MENU_title(WinMenu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    WMWnr = i; WMWnc = j; WMWsr = sr; WMWsc = sc;
    MENU_makeMenu (
	RegMenu, regionMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j
    );
    MENU_title(RegMenu) =" Region Menu: ";
    i = MENU_choiceCount(RegMenu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(RegMenu)) )
	j = strlen(MENU_title(RegMenu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    RMWnr = i; RMWnc = j; RMWsr = sr; RMWsc = sc;

    MENU_makeMenu (
	AppMenu, applicMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j
    );
    MENU_title(AppMenu) =" Applications Menu: ";
    i = MENU_choiceCount(AppMenu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(AppMenu)) )
	j = strlen(MENU_title(AppMenu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    AMWnr = i; AMWnc = j; AMWsr = sr; AMWsc = sc;

    CurrBuffer = Edit;
    
    EDIT_Init = TRUE;    
}

PrivateFnDef void createEditorWindows() {
    TopWin        = CUR_newwin((CUR_LINES - 3) / 2, CUR_COLS-1, 0, 0);
    BotWin        = CUR_newwin((CUR_LINES - 3) - ((CUR_LINES - 3) / 2),
				CUR_COLS-1, ((CUR_LINES - 3) / 2) + 1, 0);
    FullWin       = CUR_newwin(CUR_LINES - 2, CUR_COLS-1, 0, 0);
    StWinOne      = CUR_newwin(1, CUR_COLS, CUR_LINES - 2, 0);
    StWinTwo      = CUR_newwin(1, CUR_COLS, ((CUR_LINES - 3) / 2), 0);
    FileMenuWin   = CUR_newwin(FMWnr, FMWnc, FMWsr, FMWsc);
    RegionMenuWin = CUR_newwin(RMWnr, RMWnc, RMWsr, RMWsc);
    WindowMenuWin = CUR_newwin(WMWnr, WMWnc, WMWsr, WMWsc);
    ApplicMenuWin = CUR_newwin(AMWnr, AMWnc, AMWsr, AMWsc);

    if( STD_hasInsertDeleteLine )
    {
	CUR_idlok(TopWin,TRUE);
	CUR_idlok(BotWin,TRUE);
	CUR_idlok(FullWin,TRUE);
    }
    CUR_wclear(TopWin);
    CUR_wclear(BotWin);
    CUR_wclear(FullWin);
}

PrivateFnDef int deleteEditorWindows()
{
    CUR_delwin(FileMenuWin);
    CUR_delwin(WindowMenuWin);
    CUR_delwin(RegionMenuWin);
    CUR_delwin(ApplicMenuWin);
    if( CurrWin == FullWin )
    {
	CUR_delwin(TopWin);
	CUR_delwin(BotWin);
	CUR_delwin(StWinTwo);
	CUR_delwin(FullWin);
	CUR_delwin(StWinOne);
    }
    else
    {
	CUR_delwin(FullWin);
	CUR_delwin(TopWin);
	CUR_delwin(StWinTwo);
	CUR_delwin(BotWin);
	CUR_delwin(StWinOne);
    }
}

#if 0
PrivateFnDef void cleanupEditor()
{
    int i;
    BUF_deleteBuffer(Edit);
    BUF_deleteBuffer(Region);
    BUF_deleteBuffer(Output);
    for( i=0 ; i<NUMRECALL ; i++ )
	BUF_deleteBuffer(RecallBuf[i]);
    CUR_delwin(TopWin);
    CUR_delwin(BotWin);
    CUR_delwin(FullWin);
    CUR_delwin(FileMenuWin);
    CUR_delwin(WindowMenuWin);
    CUR_delwin(RegionMenuWin);
    CUR_delwin(StWinOne);
    CUR_delwin(StWinTwo);
    MENU_deleteMenu((char *)FileMenu, i);
    MENU_deleteMenu((char *)RegMenu, i);
    MENU_deleteMenu((char *)WinMenu, i);
}

#endif

/******************
 *** MAINEDITOR ***
 ******************/

PublicVarDef int	inEditor = FALSE;

PublicFnDef void EDIT_main () {
    if( interfacePage != NULL )
    {
    	ERR_displayPause(" Not allowed to enter editor from Interface Form");
    	return;
    }

    if( inEditor )
    {
    	ERR_displayPause(" Editor already running");
    	return;
    }
#if RSATTACH	
/***** dump any RS output that is left over *****/
    RS_dumpOutput();
#endif
    
    CUR_werase(CUR_stdscr);
    CUR_touchwin(CUR_stdscr);
    CUR_wnoutrefresh(CUR_stdscr);

    createEditorWindows();

/***** run the program *****/
    inEditor = TRUE;
    while (TRUE)    
    {
	if (editor()) break;
	if (session()) break;
    }
    inEditor = FALSE;

    deleteEditorWindows();
}

/************************************************************************
 *********         Interface Form         *******************************
 ************************************************************************/
 
PrivateFnDef int
initInterface()
{
	int	i, j, longest;
	MENU	*menu;

	FORM_makeForm(interfaceForm, interfaceFields, i);
	
	MENU_makeMenu(menu, srcChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Source ";
	FORM_fieldMenu(FORM_field(interfaceForm,SourceType)) = menu;

	MENU_makeMenu(menu, dstChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Destination ";
	FORM_fieldMenu(FORM_field(interfaceForm,DestType)) = menu;
	
	MENU_makeMenu(menu, intBufferChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Buffer ";
	FORM_fieldMenu(FORM_field(interfaceForm,SourceValue+1+BUFFERfield)) = menu;	

	MENU_makeMenu(menu, intBufferChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Buffer ";
	FORM_fieldMenu(FORM_field(interfaceForm,DestValue+1+BUFFERfield)) = menu;	

	MENU_makeMenu(menu, intRegionChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Region ";
	FORM_fieldMenu(FORM_field(interfaceForm,SourceValue+1+REGIONfield)) = menu;	

	MENU_makeMenu(menu, intRegionChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Region ";
	FORM_fieldMenu(FORM_field(interfaceForm,DestValue+1+REGIONfield)) = menu;	

	MENU_makeMenu(menu, printerChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Printer ";
	FORM_fieldMenu(FORM_field(interfaceForm,SourceValue+1+PRINTERfield)) = menu;	

	MENU_makeMenu(menu, printerChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Printer ";
	FORM_fieldMenu(FORM_field(interfaceForm,DestValue+1+PRINTERfield)) = menu;	

	MENU_makeMenu(menu, filterChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Filter ";
	FORM_fieldMenu(FORM_field(interfaceForm,FilterType)) = menu;	

}

PrivateFnDef void runSysMenu () {
	PAGE	*editPage;
	int	i;

	if (TwoWin)
	{
		PAGE_createPage(editPage, 4, NULL, NULL, NULL, PAGE_noType, i);
		PAGE_createElement(editPage, 0, NULL, TopWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 1, NULL, StWinTwo, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 2, NULL, BotWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 3, NULL, StWinOne, PAGE_Init, NULL, FALSE);
	}
	else
	{
		PAGE_createPage(editPage, 2, NULL, NULL, NULL, PAGE_noType, i);
		PAGE_createElement(editPage, 0, NULL, FullWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 1, NULL, StWinOne, PAGE_Init, NULL, FALSE);
	}
	PAGE_runSysMenu(editPage);
	PAGE_deletePage(editPage, i);
}
	
PrivateFnDef void runEditorInterface () {
	PAGE	*editPage;
	int	i;

	if (TwoWin)
	{
		PAGE_createPage(editPage, 4, NULL, NULL, NULL, PAGE_noType, i);
		PAGE_createElement(editPage, 0, NULL, TopWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 1, NULL, StWinTwo, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 2, NULL, BotWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 3, NULL, StWinOne, PAGE_Init, NULL, FALSE);
	}
	else
	{
		PAGE_createPage(editPage, 2, NULL, NULL, NULL, PAGE_noType, i);
		PAGE_createElement(editPage, 0, NULL, FullWin, PAGE_Init, NULL, FALSE);
		PAGE_createElement(editPage, 1, NULL, StWinOne, PAGE_Init, NULL, FALSE);
	}
	EDIT_runInterface(editPage);
	PAGE_deletePage(editPage, i);
}
	
PrivateFnDef void runReportInterface () {
	EDIT_runInterface(reportPage);
}
	
PublicVarDef int	displayErr = FALSE;
PublicVarDef char	errorMessage[81],
			errorName[81];

PublicFnDef void EDIT_runInterface(PAGE *iPage) {
	int	i;

	if( interfacePage != NULL )
	{
		ERR_displayPause("Already in Interface Form");
		ERR_msgDisplayed = FALSE;
		return;
	}

	doInterfaceClear = FALSE;	
	intWin1 = CUR_newwin(9, 78, ((CUR_LINES - 1) - 9)/2, (CUR_COLS - 78)/2);
	WIN_ReverseBox(intWin1,NULL);
	CUR_wattron(intWin1,CUR_A_REVERSE);
	CUR_wmove(intWin1,0,32);
	CUR_wprintw(intWin1,"INTERFACE FORM");
	CUR_wmove(intWin1,CUR_WIN_maxy(intWin1),2);
	CUR_wprintw(intWin1,"Execute(F2)  Quit(F9)");
	CUR_wattroff(intWin1,CUR_A_REVERSE);
	intWin2 = CUR_subwin(intWin1, 7, 76, (((CUR_LINES - 1) - 9)/2)+1, ((CUR_COLS - 78)/2)+1);

	PAGE_createPage(interfacePage,(PAGE_windowCount(iPage)+2),NULL,NULL,NULL,PAGE_noType,i);
	for( i=0 ; i<PAGE_windowCount(iPage) ; i++ )
	{
		PAGE_createElement(interfacePage, i, NULL, PAGE_window(iPage,i), PAGE_Init, NULL, FALSE);
	}
	PAGE_createElement(interfacePage, i, NULL, intWin1, PAGE_Init, NULL, FALSE);
	PAGE_createElement(interfacePage, (i+1), interfaceForm, intWin2, PAGE_Input, FORM_handler, TRUE);
    
	PAGE_fkey(interfacePage, 1) = execInterface;

	PAGE_handler(interfacePage);

	PAGE_deletePage(interfacePage, i);
	interfacePage = NULL;
	CUR_delwin(intWin2);
	CUR_delwin(intWin1);
	EDIT_displayErrorMessage();
	if( doInterfaceClear )
		CUR_clearok(CUR_curscr);
	doInterfaceClear = FALSE;	
}

PublicFnDef void EDIT_displayErrorMessage() {
	if( displayErr )
		ERR_displayStrNoRefresh(errorMessage);
	displayErr = FALSE;
	errorMessage[0] = '\0';
}

PrivateFnDef int
getErrorMessage()
{
	int	len;
	FILE	*fp, *fopen();

	displayErr = FALSE;	
	errorMessage[0] = '\0';
	if( (fp = fopen(errorName,"r")) == NULL )
		return(FALSE);
	if( fgets(errorMessage, 81, fp) == NULL )
	{
		remove(errorName);
		return(FALSE);
	}
	len = strlen(errorMessage);
	if( (len > 0) && (errorMessage[len-1] == '\n') )
		errorMessage[len-1] = '\0';
	displayErr = TRUE;
	remove(errorName);
	return(FALSE);
}

PrivateFnDef int
doFilter (sname, dname, filter, filterOpts)
char	*sname,
	*dname;
int	filter;
char	*filterOpts;
{
	char	buffer[2 * BUF_MaxPathNameChars + 80];
	
	if (filter == FILTERnone)
		{
		copyFile (sname, dname, OVERWRITEmode, FILTERnone, NULL);
		return (FALSE);
		}
		
	switch (filter)
		{
		case FILTERformatter:
			sprintf (errorName, "/tmp/viserr%d", getpid());
			sprintf (buffer,"/bin/csh -if -c \"(%s %s < %s > %s) >& %s\"",
				"/vision/bin/format",
				((filterOpts == NULL) ? " " : filterOpts),
				sname,
				dname,
				errorName);
			break;

		case FILTERuser:
			if ((filterOpts == NULL) || isBlank (filterOpts))
				{
				copyFile (sname, dname, OVERWRITEmode, FILTERnone, NULL);
				return (FALSE);
				}
			sprintf (errorName, "/tmp/viserr%d", getpid());
			sprintf (buffer,"/bin/csh -if -c \"(%s < %s > %s) >& %s\"",
				filterOpts,
				sname,
				dname,
				errorName);
			break;

		default:
			ERR_displayPause ("Unknown filter choice. Ignoring filter");
			copyFile (sname,dname,OVERWRITEmode,FILTERnone,NULL);
			return (TRUE);
		}
	CUR_saveterm ();
	CUR_resetterm ();
	RS_system (buffer);
	CUR_fixterm();
	CUR_noecho();
	CUR_nonl();
	CUR_cbreak();
	getErrorMessage();
	return(FALSE);
}	

			
PublicFnDef int printFile (char const *fname) {
    FILE *fd;
    int  filtered = FALSE;
    char tmpName[BUF_MaxPathNameChars],
         buffer[120];

    if (isBlank (PRINT_Command.command))
        {
        ERR_clearMsg ();
        return (FALSE);
        }

    if (!isBlank (PRINT_Command.initString))
        {
        sprintf (tmpName, "/tmp/visprn%d", getpid ());
        if ((fd = fopen (tmpName, "w")) == NULL)
            return (FALSE);
        fprintf (fd, "%s",  PRINT_Command.initString);
        fclose (fd);

        if (copyFile (fname, tmpName, APPENDmode, FILTERnone, NULL))
            {
            remove (tmpName);
            return (FALSE);
            }
        filtered = TRUE;
        }
    else
        strcpy (tmpName, fname);

    ERR_displayStr ("Printing, please wait...", FALSE);

    sprintf (errorName, "/tmp/viserr%d", getpid());
    sprintf (buffer, "/bin/csh -if -c \"(%s %s) >& %s\"", 
             PRINT_Command.command, tmpName, errorName);

    RS_system (buffer);
    CUR_werase (ERR_Window);
#if 0
	CUR_clearok(CUR_curscr);	/*** need to repaint the screen ***/
	CUR_wrefresh(CUR_curscr);
#endif
    doInterfaceClear = TRUE;
    if (filtered)
        remove (tmpName);

    return (FALSE);
}

PrivateFnDef int
pcTransfer(fname, pname, DirFlag, mode, filter, filterOpts)
char	*fname, *pname;
int	DirFlag, mode, filter;
char	*filterOpts;
{
	char	buffer[256], buffer2[256], tmpName[256], filterName[256];

	ERR_displayStr("Transfer in process, please wait...",FALSE);
	if( DirFlag == UploadDir )
	{
		sprintf(tmpName, "/tmp/pctr%d", getpid());
		sprintf(buffer, "/bin/csh -if -c \"%s %s %s\"",
			"pcupload",
			tmpName,
			pname);
		CUR_saveterm();
		CUR_resetterm();
		RS_system(buffer);
		CUR_fixterm();
		CUR_noecho();
		CUR_nonl();
		CUR_cbreak();
		copyFile(tmpName,fname,mode,filter,filterOpts);
		remove(tmpName);
	}
	else
	{
		if( filter != FILTERnone )
		{
			sprintf(filterName, "/tmp/visfil%d", getpid());
			doFilter(fname, filterName, filter, filterOpts);
		}
		sprintf(buffer, "/bin/csh -if -c \"%s %s %s\"",
			"pcdownload",
			((filter == FILTERnone) ? fname : filterName),
			pname);
		CUR_saveterm();
		CUR_resetterm();
		RS_system(buffer);
		CUR_fixterm();
		CUR_noecho();
		CUR_nonl();
		CUR_cbreak();
		if( filter != FILTERnone )
			remove(filterName);
	}
	CUR_werase(ERR_Window);
	doInterfaceClear = TRUE;
	return(FALSE);
}

PrivateFnDef int copyFile(
    char *			fname,
    char *			dname,
    int				mode,
    int				filter,
    char *			filterOpts
)
{
	char	buffer[1024], filterName[256], *sname;
	int	ifd, ofd, imode = O_RDONLY, omode = (O_WRONLY|O_CREAT), nread;

	if( mode == ABORTmode )
		return(TRUE);

	ERR_displayStr("Executing, please wait...",FALSE);
	sname = fname;
	if( filter != FILTERnone )
	{
		sprintf(filterName, "/tmp/visfil%d", getpid());
		doFilter(fname, filterName, filter, filterOpts);
		sname = filterName;
	}
		
	if( mode == APPENDmode )
		omode |= O_APPEND;
	else
		omode |= O_TRUNC;
	if( (ifd = open(sname, imode, 0)) == -1 )
	{
		ERR_displayPause(" Unable to Open Source File");
		return(TRUE);
	}
	if( (ofd = open(dname, omode, 0640)) == -1 )
	{
		ERR_displayPause(" Unable to Open Destination File");
		close(ifd);
		return(TRUE);
	}
	while( (nread = read(ifd, buffer, 1024)) != 0 )
	{
		if( nread == -1 )
		{
			ERR_displayPause(" Error Reading Source File");
			close(ifd);
			close(ofd);
			return(TRUE);
		}
		if( write(ofd, buffer, nread) == -1 )
		{
			ERR_displayPause(" Error Writing Destination File");
			close(ifd);
			close(ofd);
			return(TRUE);
		}
	}
	close(ifd);
	close(ofd);
	if( filter != FILTERnone )
		remove(filterName);
	return(FALSE);
}

PrivateFnDef int
eatTrailingSpaces(tmpName)
char	*tmpName;
{
	int	i;

	i = strlen(tmpName);
	while( (i > 0) && isspace(tmpName[i-1]) )
		i--;
	tmpName[i] = '\0';
	return(FALSE);
}

#define checkFileDestination\
	dname = FORM_fieldValue(FORM_field(interfaceForm,DestType+2+FILEfield));\
	while( *dname != '\0' && isspace(*dname) )\
		dname++;\
	eatTrailingSpaces(dname);\
	if( *dname == '\0' )\
	{\
		ERR_displayPause(" You must supply a Destination File Name");\
		return(TRUE);\
	}\
	if( (mode = checkFileExistence(dname)) == ABORTmode )\
		return(TRUE);

#define checkPCDestination\
	dname = FORM_fieldValue(FORM_field(interfaceForm,DestType+2+PCfield));\
	while( *dname != '\0' && isspace(*dname) )\
		dname++;\
	eatTrailingSpaces(dname);\
	if( *dname == '\0' )\
	{\
		ERR_displayPause(" You must supply a PC Destination File Name");\
		return(TRUE);\
	}

#define checkPasteBufDestination\
	dname = FORM_fieldValue(FORM_field(interfaceForm,DestType+2+PASTEBUFfield));\
	while( *dname != '\0' && isspace(*dname) )\
		dname++;\
	eatTrailingSpaces(dname);\
	if( *dname == '\0' )\
	{\
		dname = UnnamedPasteBuf;\
		EDIT_fileMode = OVERWRITEmode;\
	}\
	if( strchr(dname,'/') == NULL )\
	{\
		realPasteBuf = TRUE;\
		strcpy(tmpName2,"/sys/node_data/paste_buffers/");\
		strcat(tmpName2,dname);\
	}\
	else\
		strcpy(tmpName2,dname);\
	mode = checkFileExistence(tmpName2);\
	EDIT_fileMode = NOmode;\
	if( realPasteBuf &&\
	    (mode != ABORTmode) && \
	    ((error = access(tmpName2,0)) == -1) )\
		PBUF_createEmptyPbuf(dname);\
	else if( realPasteBuf &&\
		(mode != ABORTmode) && \
		(error == 0) &&\
		PBUF_checkPbufType(dname) )\
		return(TRUE);

PrivateFnDef int illegalDestination (
    void
)
{
	MENU	*menu;
	
	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	switch( MENU_currChoice(menu) )
	{
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERoutput:
				ERR_displayPause(" Not permitted to modify output buffer");
				return(TRUE);
			    case BUFFERbrowse:
				ERR_displayPause(" Not permitted to modify browse buffer");
				return(TRUE);
			    case BUFFERreport:
				ERR_displayPause(" Operation Not Permitted");
				return(TRUE);			        
			    default:
			    	break;
			}
			break;
		case REGIONfield:
			ERR_displayPause(" Not allowed to use a Region as a destination");
			return(TRUE);
		default:
			break;
	}
	return(FALSE);
}

PrivateFnDef int	doingSPR = FALSE;

PrivateFnDef int execInterfaceFile (
    char *			fname
)
{
	MENU	*menu;
	char	*dname, *sname, tmpName[80], tmpName2[80], *filterOpts;
	int	error, mode, filterType, realPasteBuf = FALSE;

	if( access(fname,04) == -1 )
	{
		ERR_displayPause("Unable to access Source File");
		return(TRUE);
	}
	sname = fname;
	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	filterType = MENU_currChoice(FORM_fieldMenu(FORM_field(interfaceForm,FilterType)));
	filterOpts = FORM_fieldValue(FORM_field(interfaceForm,FilterOpts));
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			checkFileDestination;
			if( copyFile(fname,dname,mode,filterType,filterOpts) )
				return(TRUE);
			break;
		case BUFFERfield:
			if( filterType != FILTERnone )
			{
				sprintf(tmpName, "/tmp/visfilf%d", getpid());
				if( copyFile(fname,tmpName,OVERWRITEmode,filterType,filterOpts) )
					return(TRUE);
				sname = tmpName;
			}
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERedit:
				if( ExecutedBuffer )
				{
					BUF_eraseBuffer(Edit);
					ExecutedBuffer = FALSE;
				}
			    	if( error = BUF_getFile(Edit,sname) )
			    	{
					if( filterType != FILTERnone )
						remove(tmpName);
			    		ERR_displayError(error);
			    		if( error == ERR_FileEmpty )
			    			return(FALSE);
			    		return(TRUE);
			    	}
			    	else
			    		ERR_displayMsg(ERR_ReadFile);
				BUF_repaintScreen(Edit) = TRUE;
			    	break;
			    case BUFFERtemp:
			    	if( error = BUF_getFile(Region,sname) )
			    	{
					if( filterType != FILTERnone )
						remove(tmpName);
			    		ERR_displayError(error);
			    		if( error == ERR_FileEmpty )
			    			return(FALSE);
			    		return(TRUE);
			    	}
			    	else
			    		ERR_displayMsg(ERR_ReadFile);
			    	break;
			    default:
			    	break;
			}
			if( filterType != FILTERnone )
				remove(tmpName);
			break;

		case PRINTERfield:
			if (filterType != FILTERnone)
				{
				sprintf (tmpName, "/tmp/visfilf%d", getpid ());
				if (copyFile (fname,tmpName,OVERWRITEmode,filterType,filterOpts))
					return (TRUE);
				sname = tmpName;
				}
			menu = FORM_fieldMenu (FORM_field (interfaceForm,DestType+2+PRINTERfield));
			print (interfacePage, MENU_currChoice (menu));
			if (printFile (sname))
				{
				if (filterType != FILTERnone)
					remove (tmpName);
				return (TRUE);
				}
			if (filterType != FILTERnone)
				remove (tmpName);
			break;

		case PCfield:
			checkPCDestination;
			if( pcTransfer(fname,dname,DownloadDir,OVERWRITEmode,filterType,filterOpts) )
				return(TRUE);
			break;
		default:
			ERR_displayPause("What destination??????");
			return(TRUE);
	}
	if( !doingSPR )
	    	strcpy(CurrentFile,fname);
    	return(FALSE);
}
	
PrivateFnDef int execInterfaceBuffer (
    int				whichBuf
)
{
	MENU		*menu;
	char		*dname, tmpName[80], tmpName2[80];
	int		error, mode, filterType, realPasteBuf = FALSE;
	LINEBUFFER	*tmp;

	switch( whichBuf )
	{
		case BUFFERedit:	tmp = Edit; break;
		case BUFFERoutput:	tmp = Output; break;
		case BUFFERtemp:	tmp = Region; break;
		case BUFFERreport:
			if( SPR_CurrSPR == NULL )
			{
				ERR_displayPause("No Current Report Defined");
				return(TRUE);
			}
			sprintf(tmpName, "/tmp/visspr%d", getpid());
			if( SPR_writeToFile(SPR_CurrSPR,tmpName) )
			{
				ERR_displayPause("Error saving to temporary file");
				return(TRUE);
			}
			doingSPR = TRUE;
			error = execInterfaceFile(tmpName);
			doingSPR = FALSE;
			remove(tmpName);
			return(error);
		case BUFFERbrowse:
			if( BUF_LastOutput(BrowserBuf) == NULL )
			{
				ERR_displayPause("No Last Output in Browse Buffer");
				return(TRUE);
			}
			sprintf(tmpName, "/tmp/visbrow%d", getpid());
			if( BUF_writeLastOutput(BrowserBuf, tmpName, "w") )
			{
				ERR_displayPause("Error saving to temporary file");
				return(TRUE);
			}
			doingSPR = TRUE;
			error = execInterfaceFile(tmpName);
			doingSPR = FALSE;
			remove(tmpName);
			return(error);
	}
	if (BUF_firstLine(tmp) == NULL && BUF_lastLine(tmp) == NULL)
	{
		ERR_displayPause("Buffer is empty");
		return(TRUE);
	}
	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	filterType = MENU_currChoice(FORM_fieldMenu(FORM_field(interfaceForm,FilterType)));
	if( filterType != FILTERnone )
	{
		sprintf(tmpName, "/tmp/visfilb%d", getpid());
		if( BUF_writeFile( tmp, tmpName ) )
		{
			ERR_displayPause("Unable to create temporary file");
			return(TRUE);
		}
		doingSPR = TRUE;
		error = execInterfaceFile(tmpName);
		doingSPR = FALSE;
		remove(tmpName);
		return(error);
	}
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			checkFileDestination;
			if( (mode == OVERWRITEmode) &&
			    (error = BUF_writeFile( tmp, dname )) )
			{
				ERR_displayMsgPause(error);
				return(TRUE);
			}
			else if( (mode == APPENDmode) &&
				 (error = BUF_appendToFile( tmp, dname )) )
			{
				ERR_displayMsgPause(error);
				return(TRUE);
			}
		    	strcpy(CurrentFile,dname);
			break;
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERedit:
			    	if( tmp == Edit )
			    	{
			    		ERR_displayPause(" Not allowed to copy buffer onto itself");
			    		return(TRUE);
			    	}
				if( ExecutedBuffer )
				{
					BUF_eraseBuffer(Edit);
					ExecutedBuffer = FALSE;
				}
				if( (error = BUF_insertRegion(Edit, NULL, tmp)) )
				{
					BUF_repaintScreen(Edit) = TRUE;
					ERR_displayMsgPause(error);
					return(TRUE);
				}
				BUF_repaintScreen(Edit) = TRUE;
			    	break;
			    case BUFFERtemp:
			    	if( tmp == Region )
			    	{
			    		ERR_displayPause(" Not allowed to copy buffer onto itself");
			    		return(TRUE);
			    	}
				if( (error = BUF_insertRegion(Region, NULL, tmp)) )
				{
					ERR_displayMsgPause(error);
					return(TRUE);
				}
			    	break;
			    default:
			    	break;
			}
			break;

		case PRINTERfield:
			menu = FORM_fieldMenu (FORM_field(interfaceForm,DestType+2+PRINTERfield));
			sprintf (tmpName, "/tmp/vision%d", getpid());
			if ((error = BUF_writeFile (tmp, tmpName)))
				{
				ERR_displayMsgPause (error);
				return (TRUE);
				}
			print (interfacePage, MENU_currChoice (menu));
			if (printFile (tmpName))
				return (TRUE);
			/* remove (tmpName); */
#if 0
			if( BUF_printBuffer(tmp, interfacePage, MENU_currChoice(menu)) )
				return(TRUE);
#endif
			break;

		case PCfield:
			checkPCDestination;
			sprintf(tmpName, "/tmp/vision%d", getpid());
			if( (error = BUF_writeFile( tmp, tmpName )) )
			{
				ERR_displayPause(" Unable to access temporary file");
				return(TRUE);
			}
			if( pcTransfer(tmpName,dname,DownloadDir,OVERWRITEmode,FILTERnone,NULL) )
				return(TRUE);
			remove(tmpName);
			break;
		default:
			ERR_displayPause("What destination??????");
			return(TRUE);
	}
    	return(FALSE);
}
			
PrivateFnDef int execInterfaceRegion (
    int				whichReg
)
{
	MENU		*menu;
	char		*dname, tmpName[80], tmpName2[80];
	int		error, mode, filterType, realPasteBuf = FALSE;

	if( whichReg == REGIONmarked )
		return( execInterfaceBuffer(BUFFERtemp) );
	if( BUF_LastOutput(Output) == NULL )
	{
		ERR_displayPause(" Last output region is empty or invalid");
		return(TRUE);
	}
	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	filterType = MENU_currChoice(FORM_fieldMenu(FORM_field(interfaceForm,FilterType)));
	if( filterType != FILTERnone )
	{
		sprintf(tmpName, "/tmp/visfilr%d", getpid());
		if( BUF_writeLastOutput( Output, tmpName, "w" ) )
		{
			ERR_displayPause("Unable to create temporary file");
			return(TRUE);
		}
		doingSPR = TRUE;
		error = execInterfaceFile(tmpName);
		doingSPR = FALSE;
		remove(tmpName);
		return(error);
	}
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			checkFileDestination;
			if( (error = BUF_writeLastOutput( Output, dname, (mode == OVERWRITEmode ? "w" : "a"))) )
			{
				ERR_displayMsgPause(error);
				return(TRUE);
			}
		    	strcpy(CurrentFile,dname);
			break;
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERedit:
				if( ExecutedBuffer )
				{
					BUF_eraseBuffer(Edit);
					ExecutedBuffer = FALSE;
				}
				/* Note the fall through here */
			    case BUFFERtemp:
				sprintf(tmpName, "/tmp/vision%d", getpid());
				if( (error = BUF_writeLastOutput( Output, tmpName, "w")) )
				{
					ERR_displayMsgPause(error);
					return(TRUE);
				}
				if( MENU_currChoice(menu) == BUFFERedit )
				{
				    	if( error = BUF_getFile(Edit,tmpName) )
				    	{
				    		ERR_displayError(error);
				    		return(TRUE);
				    	}
					BUF_repaintScreen(Edit) = TRUE;
				}
				else
				{
			    		if( error = BUF_getFile(Region,tmpName) )
				    	{
				    		ERR_displayError(error);
			    			return(TRUE);
			    		}
			    	}
			    	remove(tmpName);
			    	break;
			    default:
			    	break;
			}
			break;

		case PRINTERfield:
			menu = FORM_fieldMenu (FORM_field (interfaceForm,DestType+2+PRINTERfield));
			sprintf (tmpName, "/tmp/vision%d", getpid());
			if ((error = BUF_writeLastOutput (Output, tmpName, "w")))
				{
				ERR_displayMsgPause (error);
				return (TRUE);
				}
			print (interfacePage, MENU_currChoice (menu));
			if (printFile (tmpName))
				return (TRUE);

/*			remove (tmpName); */
			break;

		case PCfield:
			checkPCDestination;
			sprintf(tmpName, "/tmp/vision%d", getpid());
			if( (error = BUF_writeLastOutput( Output, tmpName, "w")) )
			{
				ERR_displayMsgPause(error);
				return(TRUE);
			}
			if( pcTransfer(tmpName,dname,DownloadDir,OVERWRITEmode,FILTERnone,NULL) )
				return(TRUE);
			remove(tmpName);
			break;
		default:
			ERR_displayPause("What destination??????");
			return(TRUE);
	}
    	return(FALSE);
}
			
PrivateFnDef int execInterfacePC (
    char *			fname
)
{
	MENU	*menu;
	char	*dname, *filterOpts;
	char	tmpName[80], tmpName2[80];
	int	error, mode, filterType, realPasteBuf = FALSE;

	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	filterType = MENU_currChoice(FORM_fieldMenu(FORM_field(interfaceForm,FilterType)));
	filterOpts = FORM_fieldValue(FORM_field(interfaceForm,FilterOpts));
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			checkFileDestination;
			if( pcTransfer(dname,fname,UploadDir,mode,filterType,filterOpts) )
				return(TRUE);
		    	strcpy(CurrentFile,dname);
			break;
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERedit:
				if( ExecutedBuffer )
				{
					BUF_eraseBuffer(Edit);
					ExecutedBuffer = FALSE;
				}
				/* Note the fall through here */
			    case BUFFERtemp:
				sprintf(tmpName, "/tmp/vision%d", getpid());
				if( pcTransfer(tmpName,fname,UploadDir,OVERWRITEmode,filterType,filterOpts) )
					return(TRUE);
				if( MENU_currChoice(menu) == BUFFERedit )
				{
				    	if( error = BUF_getFile(Edit,tmpName) )
				    	{
				    		ERR_displayError(error);
				    		if( error == ERR_FileEmpty )
				    			return(FALSE);
				    		return(TRUE);
				    	}
				    	else
				    		ERR_displayMsg(ERR_ReadFile);
					BUF_repaintScreen(Edit) = TRUE;
				}
				else
				{
			    		if( error = BUF_getFile(Region,tmpName) )
				    	{
				    		ERR_displayError(error);
				    		if( error == ERR_FileEmpty )
				    			return(FALSE);
			    			return(TRUE);
				    	}
				    	else
				    		ERR_displayMsg(ERR_ReadFile);
			    	}
			    	remove(tmpName);
			    	break;
			    default:
			    	break;
			}
			break;
		case PRINTERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+PRINTERfield));
			print(interfacePage,MENU_currChoice(menu));
			sprintf(tmpName, "/tmp/vision%d", getpid());
			if( pcTransfer(tmpName,fname,UploadDir,OVERWRITEmode,filterType,filterOpts) )
				return(TRUE);
			if( printFile(tmpName) )
				return(TRUE);
/*			remove(tmpName); */
			break;
		case PCfield:
			ERR_displayPause(" Not permitted.  Use the DOS COPY command.");
			return(TRUE);
		default:
			ERR_displayPause("What destination??????");
			return(TRUE);
	}
    	return(FALSE);
}
	
PrivateFnDef int execInterfacePasteBuf (
    char *			fname
)
{
	MENU	*menu;
	char	*dname, tmpName[80], tmpName2[80];
	int	error, mode, filterType, realPasteBuf = FALSE;

	if( strchr(fname,'/') == NULL )
	{
		strcpy(tmpName,"/sys/node_data/paste_buffers/");
		strcat(tmpName,fname);
	}
	else
		strcpy(tmpName,fname);
	if( access(tmpName,04) == -1 )
	{
		ERR_displayPause("Unable to access Source Paste Buffer");
		return(TRUE);
	}
	menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType));
	filterType = MENU_currChoice(FORM_fieldMenu(FORM_field(interfaceForm,FilterType)));
	if( filterType != FILTERnone )
	{
		doingSPR = TRUE;
		error = execInterfaceFile(tmpName);
		doingSPR = FALSE;
		return(error);
	}
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			checkFileDestination;
			if( copyFile(tmpName,dname,mode,FILTERnone,NULL) )
				return(TRUE);
		    	strcpy(CurrentFile,dname);
			break;
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+BUFFERfield));
			switch( MENU_currChoice(menu) )
			{
			    case BUFFERedit:
				if( ExecutedBuffer )
				{
					BUF_eraseBuffer(Edit);
					ExecutedBuffer = FALSE;
				}
			    	if( error = BUF_getFile(Edit,tmpName) )
			    	{
			    		ERR_displayError(error);
			    		if( error == ERR_FileEmpty )
			    			return(FALSE);
			    		return(TRUE);
			    	}
			    	else
			    		ERR_displayMsg(ERR_ReadFile);
				BUF_repaintScreen(Edit) = TRUE;
			    	break;
			    case BUFFERtemp:
			    	if( error = BUF_getFile(Region,tmpName) )
			    	{
			    		ERR_displayError(error);
			    		if( error == ERR_FileEmpty )
			    			return(FALSE);
			    		return(TRUE);
			    	}
			    	else
			    		ERR_displayMsg(ERR_ReadFile);
			    	break;
			    default:
			    	break;
			}
			break;
		case PRINTERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,DestType+2+PRINTERfield));
			print(interfacePage,MENU_currChoice(menu));
			if( printFile(tmpName) )
				return(TRUE);
			break;
		case PCfield:
			checkPCDestination;
			if( pcTransfer(tmpName,dname,DownloadDir,OVERWRITEmode,FILTERnone,NULL) )
				return(TRUE);
			break;
		default:
			ERR_displayPause("What destination??????");
			return(TRUE);
	}
    	return(FALSE);
}

PrivateFnDef int execInterface (
    void
)
{
	MENU	*menu;
	char	*fname;
	
	if( illegalDestination() )
		return(TRUE);
	menu = FORM_fieldMenu(FORM_field(interfaceForm,SourceType));
	switch( MENU_currChoice(menu) )
	{
		case FILEfield:
			fname = FORM_fieldValue(FORM_field(interfaceForm,SourceType+2+FILEfield));
			while( *fname != '\0' && isspace(*fname) )
				fname++;
			eatTrailingSpaces(fname);
			if( *fname == '\0' )
			{
				ERR_displayPause(" You must supply a Source File Name");
				return TRUE;
			}
			if( !execInterfaceFile(fname) )
				PAGE_status(interfacePage) = PAGE_ExitOnExec;
			break;
		case BUFFERfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+BUFFERfield));
			if( !execInterfaceBuffer(MENU_currChoice(menu)) )
				PAGE_status(interfacePage) = PAGE_ExitOnExec;
			break;
		case REGIONfield:
			menu = FORM_fieldMenu(FORM_field(interfaceForm,SourceType+2+REGIONfield));
			if( !execInterfaceRegion(MENU_currChoice(menu)) )
				PAGE_status(interfacePage) = PAGE_ExitOnExec;
			break;
		case PRINTERfield:
			ERR_displayPause("Not allowed to use a Printer as A Source");
			return TRUE;
		case PCfield:
			fname = FORM_fieldValue(FORM_field(interfaceForm,SourceType+2+PCfield));
			while( *fname != '\0' && isspace(*fname) )
				fname++;
			eatTrailingSpaces(fname);
			if( *fname == '\0' )
			{
				ERR_displayPause(" You must supply a PC Source File Name");
				return TRUE;
			}
			if( !execInterfacePC(fname) )
				PAGE_status(interfacePage) = PAGE_ExitOnExec;
			break;
		default:
			ERR_displayPause("What source??????");
			PAGE_status(interfacePage) = PAGE_ExitOnExec;
			break;
	}
	ERR_clearMsg();
	return FALSE;
}
			

PublicFnDef void EDIT_initEditor() {
    if (!EDIT_Init)
    {
        initEditor();
        initInterface();
	BUF_eraseBuffer(Region);
    }
}
