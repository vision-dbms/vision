/****************************************************************************
*****									*****
*****			mainmenu.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "choices.h"
#include "keys.h"
#include "vars.h"

PrivateVarDef int NotDone;
PrivateVarDef CUR_WINDOW *MenuWin, *FormWin, *helpWin;
PrivateVarDef FORM *Form;
PrivateVarDef PAGE *MainPage;
PrivateVarDef MENU *quitMenu;
PrivateVarDef int  quitCols;

#if NOTRSATTACH
PrivateVarDef jmp_buf RS_userjmpbuf;   
PrivateVarDef int RS_userSystem, 
		  RS_jmpToUser;
#endif


/*************************************************
 **********	Menu Declarations	**********
 *************************************************/
PrivateVarDef int exitSystem(), 
		  noExitSystem();

PrivateVarDef MENU_Choice quitChoices[] = {
    " Yes ",    " Exit System ",	 'y',	    exitSystem,	  ON, 
    " No ",	" Do Not Exit System ",	 'n',	    noExitSystem, ON, 
    NULL, 
};

PublicFnDecl int statmenu(), queries(), financeSt(), dataEntryModule(), browser();

PublicFnDecl int MAIN_getCompany(), MAIN_getUniverse(), timeSeries();

#define DataEntryIndex 6

PrivateVarDef MENU_Choice menuChoices[] = {
    " Financial Statements ",
           " Financial Statement Analysis", 'f', financeSt, ON, 
    " Corporate Profile    ",
           " Summary Information",  'c',  MAIN_getCompany, ON, 
/*    " Models               ",
 	    " Stat Sheet and Other Modeling Applications", 'm',	statmenu, ON,*/
    " Screening            ",       
           " Ad Hoc Screening   ", 's', queries, ON, 
    " Report Writer        ", 
           " User Defined Reports", 'r',    MAIN_getUniverse, ON, 
    " Time Series Reports  ", 
           " User Defined Reports Over Time", 't',    timeSeries, ON, 
/*    " Data Entry           ",      
           " Data Entry module", 'd', dataEntryModule, ON, */
/*    " Other Applications   ",      
           " Other Applications and Reports", 'o', NULL, ON, */
    " Browser ",      
           " Browse Through Objects in the System", 'b', browser, ON,
    NULL, 
};

PrivateVarDef FORM_Field formFields[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ", 
	NULL, NULL, NULL, 
 3, 30, (CUR_A_UNDERLINE | CUR_A_BOLD), 19, 0, 'a', "Analyst Workstation",
	NULL, NULL, NULL, 
 4, 35, (CUR_A_UNDERLINE | CUR_A_BOLD), 9, 0, 'a', "Main Menu", 
	NULL, NULL, NULL, 
 -1, 
};
PrivateVarDef FORM_Field helpFields[] = {
 2, 1, CUR_A_NORMAL, 15, 0, 'a', " F2 -  Execute ", NULL, NULL, NULL, 
 3, 1, CUR_A_NORMAL, 15, 0, 'a', " F4 -  Editor  ", NULL, NULL, NULL, 
 4, 1, CUR_A_NORMAL, 15, 0, 'a', " F9 -  Quit    ", NULL, NULL, NULL, 
 -1, 
};

PublicFnDef int
QuitSystem()
{
    CUR_WINDOW *menuwin;

    if( PAGE_ExitSystem )
    	return(TRUE);	
    menuwin = CUR_newwin((MENU_choiceCount(quitMenu)+4), quitCols, 5, (CUR_COLS-25));
    MENU_handler(quitMenu, menuwin, PAGE_Input);
    CUR_delwin(menuwin);
    return(FALSE);
}

PrivateFnDef int	resetReentrantVars();

PrivateFnDef void displayMenu()
/*****  
 *****  Function to creat an page object (containing the main menu)
 *****	    and call PAGE_handler()
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
{
    MENU *menu = NULL;
    FORM *form, *helpform;
    PAGE_Action MENU_handler(), FORM_handler();
    int i, j, rows, cols, startrow, startcol, longest;
    static int alreadyCentered = FALSE;

/**** do curses initializations ****/    
    PAGE_initScreen();
    KEY_initKeyboard();

    VARS_initProfileVariables();
    RS_readOutput(NULL, NULL);
/*    menu = MENU_getMainMenu(); */

/*** create quit menu ***/
    MENU_makeMenu(quitMenu, quitChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(quitMenu) = " Exit System? ";
    quitCols = longest + 4;
    if( quitCols <= (i = strlen(MENU_title(quitMenu))) )
    	quitCols = i + 1;
    
    if( menu == NULL )
    {
/*    	ERR_displayStr("No Kits Defined.  Using Original Menu", TRUE);*/
/*	MENU_makeMenu(menu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);*/
	EDIT_initEditor();

/***** set the restart position for interrups *****/
	setjmp (RS_userjmpbuf);
	RS_jmpToUser = TRUE;
	resetReentrantVars();

/*****  Must do the following because if the first character printed is in 
	reverse video, curses displays it as normal.  Therefore, we make sure
	the first character displayed is not in reverse video (then erase it) */
	CUR_wprintw(CUR_stdscr,".");
	CUR_wrefresh(CUR_stdscr);
	CUR_werase(CUR_stdscr);
	CUR_wrefresh(CUR_stdscr);

	NotDone = TRUE;
	while (NotDone && !PAGE_ExitSystem)
	{
	    if( StartupModule != NULL )
	    	(*StartupModule)();
	    else
	    	EDIT_main();
	    QuitSystem();
/*	    CUR_clearok(CUR_curscr);*/
	}

	PAGE_endScreen();
	MENU_deleteMenu(quitMenu, i);
	return;
    }
    else
    {
	longest = 0;
	for( i=0 ; i<MENU_choiceCount(menu) ; i++ )
    	    if( (j = strlen(MENU_choiceLabel(menu,i))) > longest )
    		longest = j;
    }
    rows = MENU_choiceCount(menu) + 4;
    cols = longest + 4;
    startrow = 4 + ((20 - rows) / 2);
    startcol = (CUR_COLS - cols) / 2;
    MenuWin = CUR_newwin(rows, cols, startrow, startcol);

    helpWin = CUR_newwin(7, 17, 16, (CUR_COLS-18));
    WIN_ReverseBox(helpWin,NULL);
    FORM_makeForm(helpform, helpFields, i);

/**** create form object ****/
    FORM_makeForm(form, formFields, i);
    strcpy(form->field[0]->value, RS_CompanyName);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form,CUR_COLS);
    }

/*** create menu window ***/
    FormWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);
    
/**** create page object ****/
    PAGE_createPage(MainPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_createElement(MainPage, 0, form, FormWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(MainPage, 1, menu, MenuWin, PAGE_Input, 
						 MENU_handler, TRUE);
    PAGE_createElement(MainPage, 2, helpform, helpWin, PAGE_Input, 
						 FORM_handler, FALSE);
    
/***** read the initial prompt, it will wait here for the RS to respond ****/
/***** 
 * if the RS dies on startup, before it puts out the initial prompt,
 * we will still be here waiting when we get the RS death of child signal. 
 * This is good since RS_jmpToUser will still be false.  The death of child
 * signal handler looks at RS_jmpToUser and if its FALSE it realizes that
 * nothing really started yet and will simply cause everyone to exit.  This
 * is much better than trying to restart the RS because the RS will die again
 * and you would then be in an infinite loop.
 *****/
/*    RS_readOutput(NULL, NULL);*/
#ifdef VMS
    ERR_displayStr(" ",TRUE);
#endif

    EDIT_initEditor();

/***** set the restart position for interrups *****/
    setjmp (RS_userjmpbuf);
    RS_jmpToUser = TRUE;
    resetReentrantVars();

/*****  Must do the following because if the first character printed is in 
	reverse video, curses displays it as normal.  Therefore, we make sure
	the first character displayed is not in reverse video (then erase it) */
    CUR_wprintw(CUR_stdscr,".");
    CUR_wrefresh(CUR_stdscr);
    CUR_werase(CUR_stdscr);
    CUR_wrefresh(CUR_stdscr);

/**** call page handler ****/
    NotDone = TRUE;
    while (NotDone && !PAGE_ExitSystem)
    {
	PAGE_handler(MainPage);
	QuitSystem();
/*	CUR_clearok(CUR_curscr);*/
    }

    PAGE_endScreen();

/**** cleanup page ****/
    CUR_delwin(MenuWin);
    CUR_delwin(FormWin);
    CUR_delwin(helpWin);
    MENU_deleteMenu(quitMenu, i);
    MENU_deleteMenu(menu, i);
    free(form);
    PAGE_deletePage(MainPage, i);
}


PrivateFnDef int exitSystem()
{
    PAGE_ExitSystem = TRUE;
    NotDone = FALSE;
}

PrivateFnDef int noExitSystem()
{
    PAGE_ExitSystem = FALSE;
    NotDone = TRUE;
}


PublicFnDef int 
    mainmenu()
/*****  
 *****  This is the User Interface Main Program.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING	
 *
 *****/
{
#if NOTRSATTACH
    CUR_initscr();
    CUR_savetty();
#endif
    
    displayMenu();

#if NOTRSATTACH
    CUR_erase();
    CUR_touchwin(CUR_stdscr);
    CUR_refresh();
    CUR_resetty();
    CUR_endwin();
#endif      
}


#if NOTRSATTACH
main()
{
    CUR_initscr ();
    CUR_savetty ();
    mainmenu();
}
#endif


PublicVarDecl int	inQueries;
PublicVarDecl int	inProfile;
PublicVarDecl int	inReport;
PublicVarDecl int	inSM;
PublicVarDecl int	inTS;
PublicVarDecl int	inVars;
PublicVarDecl int	inBrowser;
PublicVarDecl int	inDE;
PublicVarDecl int	inEditor;
PublicVarDecl int	inFS;

PrivateFnDef int
resetReentrantVars()
{
	inQueries = FALSE;
	inProfile = FALSE;
	inReport = FALSE;
	inSM = FALSE;
	inTS = FALSE;
	inVars = FALSE;
	inBrowser = FALSE;
	inDE = FALSE;
	inEditor = FALSE;
	inFS = FALSE;
}
