/*********************************************************
 **********		fsheet.c		**********
 ********************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "edit.h"
#include "misc.h"
#include "spsheet.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"

#include "fsheet.h"

/****** defines ******/
#include "financeSt.d"

/****** Forward Declarations *******/
PrivateFnDef void execReport ();

PrivateFnDef void execCompany ();

PrivateFnDef void execDates ();

PrivateFnDef int validDate (char const *string);

PrivateFnDef int validNumber (
    int				startDate,
    char const*			string,
    char			direction
);

/****** Globals *******/
PrivateVarDef FORM *Form1, *Form2, *Form3, *Form4;

PrivateVarDef int StartDate, EndDate;
PrivateVarDef char const *ReportType;
PrivateVarDef SPRSHEET	*FSheet;
PrivateVarDef CUR_WINDOW	*FWin, *StatWin;
PrivateVarDef PAGE *ReportPage, *FinanceStatementPage;

PrivateVarDef void changeReport ();
PrivateVarDef void changeCompany ();
PrivateVarDef void changeType ();
PrivateVarDef void changeDates ();
PrivateVarDef void printReport ();

PrivateVarDef MENU_Choice menuChoices[] = {
 " Report ",	" New Report For Current Company/Time Frame",	'r', 
			    changeReport, ON, 
 " Company ",	" New Company For Current Report/Time Frame",	'c', 
			    changeCompany, ON, 
/***
 " Type ",	" New Type For Current Report/Company",		't', 
			    changeType, ON, 
***/
 " Dates ",	" New Time Frame For Current Report/Company",	'd', 
			    changeDates, ON, 
 " Print ",     " Print Hard Copy Of Report",			 'p', 
			    printReport, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef int	didExec = FALSE;

PrivateFnDef void execFsheet() {
    char buffer[RS_MaxLine + 1];
    int	nobs;

    ERR_displayStr(" Validating Input, please wait...", FALSE);

    determineReport();
    
    /*****  clear buffer   *****/
#if RSATTACH
    RS_dumpOutput();
#endif

    strToUpper (CompanyField);
    
    /*****  check to see if company is a valid company  *****/
    if (isBlank(CompanyField))
    {
        ERR_displayPause ("   Please Enter a Company Ticker Symbol");
	return;
    }

    sprintf (buffer, "!__tmpCompany <- Named Company %s", CompanyField);

#if RSATTACH
    if (RS_sendAndCheck (buffer, ">>>"))
    {
	ERR_displayPause (" Invalid Company");
	return;
    }

    sprintf (buffer, "%s asDate", DateField);
    if (RS_sendAndCheck(buffer,"NA"))
    {
	ERR_displayPause (" Invalid Start Date");
	return;
    }
#endif
    
/**** create the stat sheet ****/
    ERR_displayStr(" Writing report, please wait...", FALSE);

    nobs = atoi(NumberField);
    if( nobs > 0 )
    	nobs--;
/*****  send message to research system and format answer  *****/
    sprintf(buffer, "__tmpCompany %s (%s) to: ((%s) asDate %c %d asInteger %s)",
			ReportType, 
			DateField, 
			DateField, 
			((Direction == 'F') ? '+' : '-'),
			nobs,
			((*FreqField == 'A') ? "yearEnds" : "quarterEnds"));
			
    SPR_erase(FSheet);
#if RSATTACH
    RS_sendLine (buffer);
    if( SPR_readSSheet (FSheet) )
    {
	 ERR_displayPause(" Error reading spread sheet");
	 return;
    }
#endif
    didExec = TRUE;
    PAGE_ExitF8PageParent = FALSE;
    SPR_handler(FSheet,FWin,PAGE_Init);
    PAGE_status(FinanceStatementPage) = PAGE_ExitOnExec;
}

PrivateFnDef void reportFileMenu() {
	EDIT_reportFileMenu(ReportPage,FALSE);
}

PrivateFnDef void fsheetFileMenu() {
	EDIT_reportFileMenu(FinanceStatementPage,FALSE);
}

PublicFnDef int fsheet (FORM *form1, PAGE *FSpage, int firstTime) {
    int i, j, longest;
    MENU *actionMenu;

    if( firstTime )
    {
	Form1 = form1;
	SPR_makeSheet(FSheet, 9, 14, 3, 4, 3);
	SPR_makeDummySheet(FSheet);

/*    MENU_makeMenu(actionMenu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);*/
    
    }

    FinanceStatementPage = FSpage;
    FWin = CUR_newwin(CUR_LINES - 2, CUR_COLS, 0, 0);
    PAGE_window(FinanceStatementPage,0) = FWin;
    PAGE_createPage(ReportPage, 2, NULL, FinanceStatementPage, NULL, PAGE_pageType, i);
    PAGE_fkey(ReportPage, 1) = NULL;
    PAGE_fkey(FSpage, 1) = execFsheet;
    PAGE_fkey(ReportPage, 4) = reportFileMenu;
    PAGE_fkey(FSpage, 4) = fsheetFileMenu;

/*** create report window ***/
    if( STD_hasInsertDeleteLine )
	CUR_idlok(FWin,TRUE);
    PAGE_createElement(ReportPage, 0, FSheet, FWin, PAGE_Scroll, SPR_handler, TRUE);
    StatWin = CUR_newwin(1, CUR_COLS-1, CUR_LINES-2, 0);
    CUR_wattron(StatWin,CUR_A_REVERSE);
    CUR_wmove(StatWin,0,0);
    CUR_wprintw(StatWin,"%-*.*s", CUR_WIN_cols(StatWin), CUR_WIN_cols(StatWin),
    	"  Interface Menu (F5)  Application Menu (F8)  Quit (F9)");
    CUR_wattroff(StatWin,CUR_A_REVERSE);
    PAGE_createElement(ReportPage, 1, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(FSpage, PAGE_windowCount(FSpage), NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_windowCount(FSpage)++;

    ERR_clearMsg();

    SPR_simulateApplic = TRUE;

    PAGE_status(ReportPage) = PAGE_Normal;

/**** call page handler ****/
    if( !didExec )
    	PAGE_ExitF8PageParent = TRUE;
    PAGE_handler(ReportPage);
    PAGE_ExitF8PageParent = FALSE;

#if 0
/*****  cleanup  *****/
    SPR_delete (FSheet);
    free(actionMenu);
#endif
    CUR_delwin(FWin);
    CUR_delwin(StatWin);
    PAGE_deletePage (ReportPage, i);

    return(FALSE);
}



/****************************************************
 *****		menu level functions		*****
 ***************************************************/
PrivateFnDef void writeReport() {
    char buffer[200];
    
    ERR_displayStr(" Writing report, please wait...", FALSE);
/*****  send message to research system and format answer  *****/
    sprintf(buffer, "__tmpCompany %s %d to: %d",
			ReportType, StartDate, EndDate);
    SPR_erase(FSheet);

#if RSATTACH
    RS_sendLine (buffer);
    SPR_readSSheet (FSheet);
    SPR_paintScreen(FSheet, FWin);
    CUR_touchwin(FWin);
    CUR_wrefresh(FWin);

#endif
}


/************************************************
 *********	printReport	*****************
 ************************************************/
PrivateFnDef void printReport() {
    SPR_print(FSheet, ReportPage);
}


/************************************************
 *********	changeReport	*****************
 ************************************************/
PrivateVarDef FORM_Field reportFields[] = {
 2, 5, CUR_A_NORMAL, 18, 0, 'a', "Select New Report:" , 
        static_cast<char const*>(NULL), NULL, NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'm', "                  ", 
        " Use Arrow Keys To Select Report Type, or F1 For Menu", NULL, NULL, 
 4, 16, CUR_A_NORMAL, 7, 0, 'a', "  Freq:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 4, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'm', "                  ", 
        " Use Arrow Keys To Select Format, or F1 For Menu", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        static_cast<char const*>(NULL), NULL, NULL, 
-1, 
};

#define ReportDlist    (ReportForm->field[1]->choiceArray)
#define REP_FORMAT 3

PrivateVarDef FORM *ReportForm;

PrivateFnDef void changeReport() {
    int i;
    CUR_WINDOW *win, *win2;
    PAGE *page;
    CUR_WINDOW *MenuWin;
    
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
    MenuWin = CUR_newwin(11, 21, 5, 55);
    FORM_makeForm(ReportForm, reportFields, i);
    ReportForm->field[1]->menu = ReportMenu;
    ReportForm->field[3]->menu = FreqMenu;

    PAGE_createPage(page, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(page, 1) = execReport;
    PAGE_status(page) = PAGE_ExitOnExec;
    
    PAGE_createElement(page, 0, NULL, FWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(page, 1, NULL, MenuWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(page, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(page, 3, ReportForm, win2,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(page, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(page);
    
    PAGE_deletePage(page, i);
    CUR_delwin(MenuWin);
    CUR_delwin(win2);
    CUR_delwin(win);
    free(ReportForm);
}

PrivateFnDef void execReport () {
    strcpy(ReportField, &ReportForm->field[1]->value[1]);
    strcpy(FreqField, &ReportForm->field[3]->value[1]);
    determineReport();
    writeReport();
}


/************************************************
 *********	changeCompany	*****************
 ************************************************/
PrivateVarDef FORM_Field companyFields[] = {
 2, 5, CUR_A_NORMAL, 18, 0, 'a', "Enter New Company:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'a', "                  ", 
        " Enter Company Ticker Symbol", NULL, NULL, 
/***
 4, 5, CUR_A_NORMAL, 14, 0, 'a', "     Company list:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 4, 24, (CUR_A_DIM | CUR_A_REVERSE), 14, 1, 'a', "              ", 
        " Enter Company List", NULL, NULL, 
***/
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        static_cast<char const*>(NULL), NULL, NULL, 
-1, 
};

PrivateVarDef FORM *CompanyForm;
PrivateVarDef PAGE *CompanyPage;

PrivateFnDef void changeCompany() {
    int i;
    CUR_WINDOW *win, *win2;
    CUR_WINDOW *MenuWin;
    
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
    MenuWin = CUR_newwin(11, 21, 5, 55);
    FORM_makeForm(CompanyForm, companyFields, i);

    PAGE_createPage(CompanyPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(CompanyPage, 1) = execCompany;
    
    PAGE_createElement(CompanyPage, 0, NULL, FWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 1, NULL, MenuWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 3, CompanyForm, win2,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(CompanyPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(CompanyPage);
    
    PAGE_deletePage(CompanyPage, i);
    CUR_delwin(MenuWin);
    CUR_delwin(win2);
    CUR_delwin(win);
    free(CompanyForm);
}

PrivateFnDef void execCompany () {
    char buffer[80];
    
    PAGE_status(CompanyPage) = PAGE_Normal;

    strToUpper(CompanyForm->field[1]->value);

    if (isBlank(CompanyForm->field[1]->value))
    {
        ERR_displayPause ("   Please Enter a Company Ticker Symbol");
	return;
    }

    ERR_displayStr(" Validating Company...",FALSE);
    sprintf (buffer,
         "!__tmpCompany <- Named Company %s", CompanyForm->field[1]->value);

    if (RS_sendAndCheck (buffer, ">>>"))
	ERR_displayPause(" Invalid Company");
    else
    {
	strcpy(CompanyField, CompanyForm->field[1]->value);
	writeReport();
	PAGE_status(CompanyPage) = PAGE_ExitOnExec;
    }
}


/************************************************
 *********	changeDates	*****************
 ************************************************/
PrivateVarDef FORM_Field dateFields[] = {
 1, 5, CUR_A_NORMAL, 14, 0, 'a', "   Start Year:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 1, 21, (CUR_A_DIM | CUR_A_REVERSE), 14, 1, 'a', "              ", 
        " Enter Starting Date of Report", NULL, NULL, 
 3, 5, CUR_A_NORMAL, 14, 0, 'a', "       Number:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 3, 21, (CUR_A_DIM | CUR_A_REVERSE), 14, 1, 'a', "              ", 
        " Enter the number of periods to display", NULL, NULL, 
 5, 5, CUR_A_NORMAL, 14, 0, 'a', "    Direction:", 
        static_cast<char const*>(NULL), NULL, NULL, 
 5, 21, (CUR_A_DIM | CUR_A_REVERSE), 14, 1, 'a', "              ", 
  " Use Arrow Keys To Select Direction From The Starting Date", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        static_cast<char const*>(NULL), NULL, NULL, 
-1, 
};

PrivateVarDef FORM *DatesForm;
PrivateVarDef PAGE *DatesPage;

PrivateFnDef void changeDates() {
    int i;
    CUR_WINDOW *win, *win2;
    CUR_WINDOW *MenuWin;
    
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
    MenuWin = CUR_newwin(11, 21, 5, 55);
    FORM_makeForm(DatesForm, dateFields, i);

    PAGE_createPage(DatesPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(DatesPage, 1) = execDates;
    
    PAGE_createElement(DatesPage, 0, NULL, FWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(DatesPage, 1, NULL, MenuWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(DatesPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(DatesPage, 3, DatesForm, win2,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(DatesPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(DatesPage);
    
    PAGE_deletePage(DatesPage, i);
    CUR_delwin(MenuWin);
    CUR_delwin(win2);
    CUR_delwin(win);
    free(DatesForm);
}

PrivateFnDef void execDates () {
    int startDate, endDate;
    
    PAGE_status(DatesPage) = PAGE_Normal;

    if ((startDate = validDate(DatesForm->field[1]->value)) == FALSE)
	ERR_displayPause (" Invalid Date");
    else
    if ((endDate = validNumber(startDate, 
	DatesForm->field[3]->value, DatesForm->field[5]->value[1])) == FALSE)
	ERR_displayPause (" Unable To Display Desired Number of Periods");
    else
    {
	StartDate = startDate;
	EndDate = endDate;
	strcpy(DateField, &DatesForm->field[1]->value[1]);
	strcpy(NumberField, &DatesForm->field[3]->value[1]);
	strcpy(DirectionField, &DatesForm->field[5]->value[1]);
	writeReport();
	PAGE_status(DatesPage) = PAGE_ExitOnExec;
    }
}



/***************************************************
 **********	Edit Routines	********************
 **************************************************/

PrivateFnDef int validDate (char const *string) {
    return ( atoi(string) );
}

PrivateFnDef int validNumber (
    int				startDate,
    char const*			string,
    char			direction
)
{
    int number = atoi(string);
    int endDate;
    
    if (number < 1)
       number = 1;
	
    if (direction == 'F' )
    {
       endDate = startDate + number - 1;
       return ((endDate > ENDDATE) ? ENDDATE : endDate);
    }
       

    if ( direction == 'R' )
    {
       endDate = startDate - number + 1;
       return ((endDate < STARTDATE) ? STARTDATE : endDate);
    }
       

   return FALSE;         /* should never reach here  */
        
}
