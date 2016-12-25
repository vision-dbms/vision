/*******************************************
 **********	financeSt.c	************
 *******************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"

#include "fsheet.h"

/****** defines ******/
#include "financeSt.d"

PrivateVarDef FORM *Form1;

PrivateVarDef MENU *menu1, *menu2, *menu3;

PrivateVarDef FORM_Field form1Fields[] = {
 1, 1, CUR_A_NORMAL, 11, 0, 'a', "   Company:", 
        NULL, NULL, NULL, 
 1, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'a', "                    ", 
        " Enter Company Ticker Symbol", NULL, NULL, 
 3, 1, CUR_A_NORMAL, 11, 0, 'a', "      Type:", 
        NULL, NULL, NULL, 
 3, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'm', "                    ", 
        " Use Arrow Keys To Select Report Type, or F1 For Menu", NULL, NULL, 
 5, 1, CUR_A_NORMAL, 11, 0, 'a', "      Freq:", 
        NULL, NULL, NULL, 
 5, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'm', "                    ", 
        " Use Arrow Keys To Select Frequency, or F1 For Menu", NULL, NULL, 
 7, 1, CUR_A_NORMAL, 11, 0, 'a', "Start Year:", 
        NULL, NULL, NULL, 
 7, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'n', "87     ", 
        " Enter Starting Date of Report", NULL, NULL, 
 9, 1, CUR_A_NORMAL, 11, 0, 'a', "    Number:", 
        NULL, NULL, NULL, 
 9, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'n', "5      ", 
        " Enter The Number of Periods To Display", NULL, NULL, 
 11, 1, CUR_A_NORMAL, 11, 0, 'a', " Direction:", 
        NULL, NULL, NULL, 
 11, 13, (CUR_A_DIM | CUR_A_REVERSE), 20, 1, 'm', "                    ", 
        " Use Arrow Keys To Select Direction From The Starting Date",
        NULL, NULL,
 -1, 
};


/***************************************************************
 **********	Field Menu Definitions 		****************
 ***************************************************************/

PrivateVarDef MENU_Choice typeChoices[] = {
 " Individual",	    " Individual Analysis",	'i', FORM_menuToForm, ON, 
 " Consolidated",   " Consolidated Analysis",	'c', FORM_menuToForm, ON, 
 " Share of",	    " Share of Analysis",	's', FORM_menuToForm, ON, 
 " Peer group",	    " Peer Group Analysis",	'p', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice reportChoices[] = {
 " Balance Sheet",	" Balance Sheet",	'b', FORM_menuToForm, ON, 
 " Income Statement",	" Income Statement",	'i', FORM_menuToForm, ON, 
 " Sources and Uses",	" Sources and Uses",	's', FORM_menuToForm, ON, 
 " Ratio Analysis",	" Ratio Analysis",	'r', FORM_menuToForm, ON, 
 " Credit Analysis",    " Credit Analysis",     'c', FORM_menuToForm, ON, 
 " Bal Sht (Pct)",	" Balance Sheet (Percentage Analysis)",	
                                           '1', FORM_menuToForm, ON, 
 " Bal Sht (Trn)",	" Balance Sheet (Trend Analysis)",
                                           '2', FORM_menuToForm, ON, 
 " Incm Stmt (Pct) ",	" Income Statement (Percentage Analysis)",	
                                           '3', FORM_menuToForm, ON, 
 " Incm Stmt (Trn)",	" Income Statement (Trend Analysis)",
                                           '4', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice frequencyChoices[] = {
 " Annual",	" Annual",	'a', FORM_menuToForm, ON, 
 " Quarterly",	" Quarterly",	'q', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice directionChoices[] = {
 " Reverse",	" Reverse Order From Start Date",   'r', FORM_menuToForm, ON, 
 " Forward",	" Forward Order From Start Date",   'f', FORM_menuToForm, ON, 
 NULL, 
};


/*****
PrivateVarDef MENU_Choice formatChoices[] = {
 " Standard",	    " Standard Format",	    's', FORM_menuToForm, ON, 
 " Percentage",    " Percentage Analysis Format",  'p', FORM_menuToForm, ON, 
 " Trend",   " Trend Analysis Format", 't', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice formChoices[] = {
 " Long",	" Long Report Form", 	    'l', FORM_menuToForm, ON, 
 " Short",	" Short Report Form",	    's', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice yearChoices[] = {
 " 1985", "Starting Year", '5', FORM_menuToForm, ON, 
 " 1984", "Starting Year", '4', FORM_menuToForm, ON, 
 " 1983", "Starting Year", '3', FORM_menuToForm, ON, 
 " 1982", "Starting Year", '2', FORM_menuToForm, ON, 
 " 1981", "Starting Year", '1', FORM_menuToForm, ON, 
 " 1980", "Starting Year", '0', FORM_menuToForm, ON, 
 " 1979", "Starting Year", '9', FORM_menuToForm, ON, 
 " 1978", "Starting Year", '8', FORM_menuToForm, ON, 
 " 1977", "Starting Year", '7', FORM_menuToForm, ON, 
 " 1976", "Starting Year", '6', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice numberChoices[] = {
  " 5", "5 Years", '5', FORM_menuToForm, ON, 
  " 6", "6 Years", '6', FORM_menuToForm, ON, 
  " 7", "7 Years", '7', FORM_menuToForm, ON, 
  " 8", "8 Years", '8', FORM_menuToForm, ON, 
  " 9", "9 Years", '9', FORM_menuToForm, ON, 
  " 10", "10 Years", '0', FORM_menuToForm, ON, 
  " 1", "1 Year", '1', FORM_menuToForm, ON, 
  " 2", "2 Years", '2', FORM_menuToForm, ON, 
  " 3", "3 Years", '3', FORM_menuToForm, ON, 
  " 4", "4 Years", '4', FORM_menuToForm, ON, 
 NULL, 
};
*****/


/**************************************************************
 ************	Dependancy lists for Report	***************
 **************************************************************/

#if 0
#define ReportDlist    (Form3->field[1]->choiceArray)

/****** field affected *****/
#define REP_FREQ 	3
#define REP_FORMAT 	5
#define REP_FORM	7

PrivateVarDef CHOICE_MenuChoice dependencyBS[] = {
    REP_FREQ,   "111",
    REP_FORMAT, "111", 
    REP_FORM,   "11", 
    -1, 
};

PrivateVarDef CHOICE_MenuChoice dependencyIS[] = {
    REP_FREQ,   "111",
    REP_FORMAT, "111", 
    REP_FORM,   "11", 
    -1, 
};

PrivateVarDef CHOICE_MenuChoice dependencySU[] = {
    REP_FREQ,   "100",
    REP_FORMAT, "100", 
    REP_FORM,   "11", 
    -1, 
};

PrivateVarDef CHOICE_MenuChoice dependencyR[] = {
    REP_FREQ,   "111",
    REP_FORMAT, "100", 
    REP_FORM,   "01", 
    -1, 
};

PrivateVarDef CHOICE_MenuChoice dependencyCA[] = {
    REP_FREQ,   "111",
    REP_FORMAT, "100", 
    REP_FORM,   "01", 
    -1, 
};

PrivateVarDef CHOICE_MenuChoice *dependencyList[] = {
    dependencyBS, dependencyIS, dependencySU, dependencyR, dependencyCA, 
};

#endif

PrivateVarDef int	firstTime = TRUE;
PublicVarDef int	inFS = FALSE;

PublicFnDef PAGE_Action financeSt()
{
    PAGE *page;
    CUR_WINDOW *form1Win, *form2Win, *form3Win, *form4Win, *tmpWin;
    int i, longest, j;

    if( inFS )
    {
    	ERR_displayPause("Finance Statement already running");
    	return(PAGE_Input);
    }
    inFS = TRUE;
        
    if( firstTime )
    {
/**** create form objects ****/
	FORM_makeForm(Form1, form1Fields, i);

	MENU_makeMenu(menu1, reportChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_makeMenu(menu2, frequencyChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_makeMenu(menu3, directionChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	ReportMenu = menu1;
	MENU_title(ReportMenu) = " Report Type:";
	FreqMenu = menu2;
	MENU_title(FreqMenu) = " Frequency:";
	DirMenu = menu3;
	MENU_title(DirMenu) = " Direction:";
    }

	i = CUR_LINES - 15 - 3;
	tmpWin = CUR_newwin(15, 36, i, 2);
	WIN_ReverseBox(tmpWin,NULL);
	CUR_wmove(tmpWin,0,4);
	CUR_wattron(tmpWin,CUR_A_REVERSE);
	CUR_wprintw(tmpWin,"FINANCIAL STATEMENT ANALYSIS");
	CUR_wmove(tmpWin,CUR_WIN_maxy(tmpWin),2);
	CUR_wprintw(tmpWin,"Execute(F2)  Quit(F9)");
	CUR_wattroff(tmpWin,CUR_A_REVERSE);
	form1Win = CUR_subwin(tmpWin, 13, 34, i+1, 3);

/**** create page object ****/
	PAGE_createPage (page, 3, NULL, NULL, NULL, PAGE_noType, i);

	/* NOTE: the window parameter for the first element will be filled in */
	/* by the call to fsheet() */
	PAGE_createElement(page, 0, NULL, NULL, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 1, NULL, tmpWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(page, 2, Form1, form1Win,
					 PAGE_Input, FORM_handler, TRUE);

    if( firstTime )
    {
    	firstTime = FALSE;
	fsheet(Form1,page,TRUE);
    }
    else
    	fsheet(Form1,page,FALSE);

#if 0
/**** call page handler ****/

    PAGE_handler(page);
           
/*****  cleanup  *****/
    free (Form1);
    MENU_deleteMenu (menu1, i);
    MENU_deleteMenu (menu2, i);
    MENU_deleteMenu (menu3, i);
#endif
    CUR_delwin (form1Win);
    CUR_delwin (tmpWin);
    PAGE_deletePage (page, i)

    inFS = FALSE;
    return (PAGE_Input);

}
