/*********************************************************
 **********		profile.c		**********
 ********************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "edit.h"
#include "spsheet.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "month.d"

#include "profile.h"


/****** Forward Declarations *******/
PrivateFnDef void execCompany ();

PrivateFnDef void execItem ();

PrivateFnDef void itemList ();

PrivateFnDef void execExpr ();

/****** Globals *******/
PrivateVarDef SPRSHEET	*ProfileReport;
PrivateVarDef PAGE *ProfilePage, *ApplicPage;
PrivateVarDef CUR_WINDOW  *Win, *tWin, *StatWin;
PrivateVarDef char const *Company;
PrivateVarDef MENU *actionMenu;

PrivateFnDef void changeCompany(), displayItem(), sendExpression(), detailsMenu();

PrivateVarDef MENU_Choice menuChoices[] = {
 " Company ",	" New Company For Current Report",	'c', 
			    changeCompany, ON, 
 " Item ",	" Display Selected Item For Company",	'i', 
			    displayItem, ON, 
 " Details ",	" Show Detailed Report Area",	'd', 
			    detailsMenu, ON, 
 " Expression ", " Send Expression And Display Results",	'e', 
			    sendExpression, ON, 
 NULL, 
};

PrivateFnDef void profileFileMenu() {
	EDIT_reportFileMenu(ProfilePage,FALSE);
}

PrivateFnDef void applicFileMenu() {
	EDIT_reportFileMenu(ApplicPage,FALSE);
}


PublicFnDef void profile (char const *company) {
    int i, j, longest;
    char buffer[RS_MaxLine + 1];

    if( company != NULL )	/* Must be the first time */
    {
	ERR_displayStr(" Writing report, please wait...", FALSE);

	Company = company;
/**** create the report ****/
	SPR_makeSheet(ProfileReport, 20, 4, 0, 6, 0);

	RS_dumpOutput();

/*****  send message to research system and read report *****/
	sprintf(buffer, "Named Company %s profile", company);
	RS_sendLine (buffer);
	SPR_readSSheet (ProfileReport);
    }
    else
	SPR_simulateApplic = TRUE;
    
/**** create menu object ****/
	MENU_makeMenu(actionMenu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(actionMenu) = " Corporate Profile:";
	longest += 4;
	if( (i = strlen(MENU_title(actionMenu))) >= longest )
		longest = i + 1;
	i = MENU_choiceCount(actionMenu) + 4;
	if( (longest + 55 + 4) > CUR_COLS )
		j = CUR_COLS - longest - 4;
	else
		j = 55;
	Win = CUR_newwin(CUR_LINES - 2, CUR_COLS, 0, 0);
	if( STD_hasInsertDeleteLine )
		CUR_idlok(Win,TRUE);
	tWin = CUR_newwin(i, longest, 5, j);

	PAGE_createPage(ApplicPage, 3, NULL, NULL, NULL, PAGE_noType, i);

	PAGE_createElement(ApplicPage, 0, NULL, Win, PAGE_Init, NULL, FALSE)
	PAGE_createElement(ApplicPage, 1, actionMenu, tWin, PAGE_Input, 
					    MENU_handler, TRUE);

	PAGE_createPage(ProfilePage, 2, NULL, ApplicPage, NULL, PAGE_pageType, i);
	PAGE_createElement(ProfilePage, 0, ProfileReport, Win, PAGE_Scroll, 
					    SPR_handler, TRUE);
    StatWin = CUR_newwin(1, CUR_COLS-1, CUR_LINES-2, 0);
    CUR_wattron(StatWin,CUR_A_REVERSE);
    CUR_wmove(StatWin,0,0);
    CUR_wprintw(StatWin,"%-*.*s", CUR_WIN_cols(StatWin), CUR_WIN_cols(StatWin),
    	"  Interface Menu (F5)  Application Menu (F8)  Quit (F9)");
    CUR_wattroff(StatWin,CUR_A_REVERSE);
    PAGE_createElement(ProfilePage, 1, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ApplicPage, 2, NULL, StatWin, PAGE_Init, NULL, FALSE);

	PAGE_fkey(ProfilePage, 1) = NULL;
	PAGE_fkey(ApplicPage, 1) = NULL;
	PAGE_fkey(ProfilePage, 4) = profileFileMenu;
	PAGE_fkey(ApplicPage, 4) = applicFileMenu;

    ERR_clearMsg();

/**** call page handler ****/
    PAGE_handler(ProfilePage);

#if 0
/*****  cleanup  *****/
    SPR_delete (ProfileReport);
#endif
    CUR_delwin(Win);
    CUR_delwin(tWin);
    CUR_delwin(StatWin);
    MENU_deleteMenu(actionMenu, i);
    PAGE_deletePage (ApplicPage, i);
    PAGE_deletePage (ProfilePage, i);
}



/****************************************************
 *****		menu level functions		*****
 ***************************************************/
PrivateFnDef void writeReport() {
    char buffer[200];
    
    ERR_displayStr(" Writing report, please wait...", FALSE);

    SPR_erase(ProfileReport);

    sprintf(buffer, "Named Company %s profile", Company);
    RS_sendLine (buffer);
    if( SPR_readSSheet (ProfileReport) )
    {
	 ERR_displayPause(" Error reading spread sheet");
	 return;
    }
    SPR_handler(ProfileReport, Win, PAGE_Init);
    MENU_status(actionMenu) = MENU_ExitOnExec;
#if 0
    CUR_touchwin(Win);
    CUR_wrefresh(Win);
#endif
}


/************************************************
 *********	printReport	*****************
 ************************************************/
#if 0
PrivateFnDef void printReport() {
    SPR_print(ProfileReport, ProfilePage);
}
#endif

/************************************************
 *********	changeCompany	*****************
 ************************************************/
#define COMPANY (CompanyForm->field[1]->value)

PrivateVarDef FORM_Field companyFields[] = {
 2, 5, CUR_A_NORMAL, 18, 0, 'a', "Enter New Company:", 
        NULL, NULL, NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'a', "                  ", 
        " Enter Company Ticker Symbol", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PrivateVarDef FORM *CompanyForm;
PrivateVarDef PAGE *CompanyPage;

PrivateFnDef void changeCompany() {
    int i;
    CUR_WINDOW *win, *win2;
/*    CUR_WINDOW *MenuWin;*/
    
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
/*    MenuWin = CUR_newwin(11, 21, 5, 55);*/
    FORM_makeForm(CompanyForm, companyFields, i);
    COMPANY[0] = '\0';
    
    PAGE_createPage(CompanyPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(CompanyPage, 1) = execCompany;
    
    PAGE_createElement(CompanyPage, 0, NULL, Win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 1, NULL, tWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(CompanyPage, 3, CompanyForm, win2,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(CompanyPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(CompanyPage);
    
    PAGE_deletePage(CompanyPage, i);
/*    CUR_delwin(MenuWin);*/
    CUR_delwin(win2);
    CUR_delwin(win);
    free(CompanyForm);
}

PrivateFnDef void execCompany () {
    char buffer[80];
    
    PAGE_status(CompanyPage) = PAGE_Normal;

    if (isBlank(COMPANY))
    {
        ERR_displayPause (" Please Enter A Company Ticker Symbol");
	return;
    }

    ERR_displayStr(" Validating Company...", FALSE);
    strToUpper(COMPANY);
    sprintf (buffer, "Named Company %s", COMPANY);

    if (RS_sendAndCheck (buffer, ">>>"))
	ERR_displayPause(" Invalid Company");
    else
    {
	Company = COMPANY;
	writeReport();
	PAGE_status(CompanyPage) = PAGE_ExitOnExec;
    }
}


/************************************************
 *********	displayItem	*****************
 ************************************************/
#define ITEM	(ItemForm->field[1])
#ifdef YEAR
#undef YEAR
#endif
#define YEAR	(ItemForm->field[3])
#define MONTH	(ItemForm->field[5])
#define DISPLAY	(ItemForm->field[6])

PrivateVarDef FORM_Field itemFields[] = {
 1, 2, CUR_A_NORMAL, 16, 0, 'a', "Item To Display:", 
        NULL, NULL, NULL, 
 1, 19, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', "                ", 
        " Enter Item To Display, or Press F1 For Menu", NULL, NULL, 
 3, 13, CUR_A_NORMAL, 5, 0, 'a', "Year:", 
        NULL, NULL, NULL, 
 3, 19, (CUR_A_DIM | CUR_A_REVERSE), 4, 1, 'n', "    ", 
        " Enter Year, or Press F1 For Menu", NULL, NULL, 
 3, 27, CUR_A_NORMAL, 6, 0, 'a', "Month:", 
        NULL, NULL, NULL, 
 3, 34, (CUR_A_DIM | CUR_A_REVERSE), 4, 1, 'n', "    ", 
        " Enter Month, or Press F1 For Menu", NULL, NULL, 
 5, 1, CUR_A_NORMAL, 45, 0, 'a', "                                             ", 
        NULL, NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PrivateVarDef MENU_Choice yearChoices[] = {
 "1985", "Year To Display", '5', FORM_menuToForm, ON, 
 "1984", "Year To Display", '4', FORM_menuToForm, ON, 
 "1983", "Year To Display", '3', FORM_menuToForm, ON, 
 "1982", "Year To Display", '2', FORM_menuToForm, ON, 
 "1981", "Year To Display", '1', FORM_menuToForm, ON, 
 "1980", "Year To Display", '0', FORM_menuToForm, ON, 
 "1979", "Year To Display", '9', FORM_menuToForm, ON, 
 "1978", "Year To Display", '8', FORM_menuToForm, ON, 
 "1977", "Year To Display", '7', FORM_menuToForm, ON, 
 "1976", "Year To Display", '6', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef FORM *ItemForm;
PrivateVarDef PAGE *ItemPage;
PrivateVarDef CUR_WINDOW *ItemWin;

PrivateFnDef void displayItem() {
    MENU *menu1, *menu2;
    int i, j, longest;
    CUR_WINDOW *win;
/*    CUR_WINDOW *MenuWin;*/
    
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    ItemWin = CUR_subwin(win, 8, 48, 11, 1);
/*    MenuWin = CUR_newwin(11, 21, 5, 55);*/

    FORM_makeForm(ItemForm, itemFields, i);
    FORM_fieldValue(ITEM)[0] = '\0';
    FORM_fieldValue(YEAR)[0] = '\0';
    FORM_fieldValue(MONTH)[0] = '\0';
    FORM_fieldValue(DISPLAY)[0] = '\0';
    
    menu1 = MENU_getMenu("companyItemCategoryListList");
    if (menu1 != NULL)
    {
	for (i = 0; i < MENU_choiceCount(menu1); i++)
	    MENU_choiceHandler(menu1, i) = itemList;
	MENU_title(menu1) = " Item Category: ";
    }
    FORM_fieldMenu(ITEM) = menu1;
    MENU_makeMenu(menu2, yearChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(menu2) = " Years: ";
    FORM_fieldMenu(YEAR) = menu2;

    PAGE_createPage(ItemPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(ItemPage, 1) = execItem;
    
    PAGE_createElement(ItemPage, 0, NULL, Win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ItemPage, 1, NULL, tWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(ItemPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ItemPage, 3, ItemForm, ItemWin,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(ItemPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(ItemPage);

    MENU_deleteMenu(menu1, i);
    MENU_deleteMenu(menu2, i);
    PAGE_deletePage(ItemPage, i);
/*    CUR_delwin(MenuWin);*/
    CUR_delwin(ItemWin);
    CUR_delwin(win);
    free(ItemForm);
}

PrivateFnDef void execItem () {
    char inbuffer[RS_MaxLine + 1], outbuffer[RS_MaxLine + 1];
    int year, month, dateGiven;
    
    PAGE_status(ItemPage) = PAGE_Normal;

    if (isBlank(FORM_fieldValue(ITEM)))
    {
        ERR_displayPause (" Please Enter An Item To Display");
	return;
    }

    ERR_displayStr(" Executing...",FALSE);
    if (isBlank(FORM_fieldValue(YEAR)) && isBlank(FORM_fieldValue(MONTH)))
	dateGiven = FALSE;
    else if (isBlank(FORM_fieldValue(MONTH)))
    {
	strcpy(FORM_fieldValue(MONTH), "12");
	CUR_wattron(ItemWin, FORM_fieldAttr(MONTH));
	CUR_wmove(ItemWin, FORM_fieldY(MONTH), FORM_fieldX(MONTH));
	CUR_wprintw(ItemWin,
	    "%-*.*s", FORM_fieldLen(MONTH), FORM_fieldLen(MONTH),
					     FORM_fieldValue(MONTH));
	CUR_wattroff(ItemWin, FORM_fieldAttr(MONTH));
	dateGiven = TRUE;
    }
    else if (isBlank(FORM_fieldValue(YEAR)))
    {
        ERR_displayPause (" Please Enter Year");
	return;
    }
    else
	dateGiven = TRUE;
    
    if (dateGiven)
    {
	month = atoi(FORM_fieldValue(MONTH));
	if (month < 1 || month > 12)
	{
	    ERR_displayPause (" Please Enter Month (1 - 12)");
	    return;
	}
	year = atoi(FORM_fieldValue(YEAR));
	if (year < 100) year += 1900;
    }

    if (dateGiven)
	sprintf (inbuffer, "Named Company %s :%s asOf: %d%02d%d asDate", 
		Company, FORM_fieldValue(ITEM),
			     year, month, MONTH_lastDay(month));
    else
	sprintf (inbuffer, "Named Company %s %s", 
			    Company, FORM_fieldValue(ITEM));
    
    RS_sendLine(inbuffer);

    if (dateGiven)
	sprintf(inbuffer, "%s%d %s = ", MONTH_text(month), year, 
				    FORM_fieldValue(ITEM));
    else
	sprintf(inbuffer, " %s = ", FORM_fieldValue(ITEM));

    RS_getValue(outbuffer);
    strcat(inbuffer, outbuffer);
    strncpy(FORM_fieldValue(DISPLAY), inbuffer, FORM_fieldLen(DISPLAY));
    FORM_fieldValue(DISPLAY)[FORM_fieldLen(DISPLAY)] = '\0';

    CUR_wattron(ItemWin, FORM_fieldAttr(DISPLAY));
    CUR_wmove(ItemWin, FORM_fieldY(DISPLAY), FORM_fieldX(DISPLAY));
    CUR_wprintw(ItemWin,
	"%-*.*s", FORM_fieldLen(DISPLAY), FORM_fieldLen(DISPLAY), 
					FORM_fieldValue(DISPLAY));
    CUR_wattroff(ItemWin, FORM_fieldAttr(DISPLAY));
}

PrivateFnDef void itemList () {
    MENU *menu1, *menu2;
    int choice, i;
    char string[80];
    CUR_WINDOW *SysWin;
    
    menu1 = FORM_fieldMenu(ITEM);
    choice = MENU_currChoice(menu1);
    
    sprintf(string,
         "companyItemCategory%sList", MENU_choiceLabel(menu1, choice));
    menu2 = MENU_getMenu(string);
    
    if (menu2 == NULL)
    {
        ERR_displayPause(" No Items For Category Selected");
	return;
    }
    
    for (i = 0; i < MENU_choiceCount(menu2); i++)
	MENU_choiceHandler(menu2, i) = FORM_menuToForm;

    SysWin = CUR_newwin(10, 20, 5, 5);
    
    FORM_fieldMenu(ITEM) = menu2;
    MENU_handler(menu2, SysWin, PAGE_Input);
    FORM_fieldMenu(ITEM) = menu1;
    CUR_delwin(SysWin);
    MENU_deleteMenu(menu2, i);
}


/************************************************
 *********	sendExpression	*****************
 ************************************************/
#define EXPR1	    (ExprForm->field[1])
#define EXPR2	    (ExprForm->field[2])
#define RESULT	    (ExprForm->field[4])

PrivateVarDef FORM_Field exprFields[] = {
 1, 1, CUR_A_NORMAL, 11, 0, 'a', "Expression:", 
        NULL, NULL, NULL, 
 1, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ", " Enter Expression", NULL, NULL, 
 2, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ", " Enter Expression", NULL, NULL, 
 5, 5, CUR_A_NORMAL, 7, 0, 'a', "Result:", 
        NULL, NULL, NULL, 
 5, 13, CUR_A_NORMAL, 40, 0, 'a', "                                        ", 
        NULL, NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PrivateVarDef FORM *ExprForm;
PrivateVarDef PAGE *ExprPage;
PrivateVarDef CUR_WINDOW *ExprWin;

PrivateFnDef void sendExpression() {
    int i;
    CUR_WINDOW *win;
/*    CUR_WINDOW *MenuWin;*/
    
    win = CUR_newwin(10, 60, 10, 0);
    WIN_ReverseBox(win,NULL);
    ExprWin = CUR_subwin(win, 8, 58, 11, 1);
/*    MenuWin = CUR_newwin(11, 21, 5, 55);*/

    FORM_makeForm(ExprForm, exprFields, i);
    FORM_fieldValue(EXPR1)[0] = '\0';
    FORM_fieldValue(EXPR2)[0] = '\0';
    FORM_fieldValue(RESULT)[0] = '\0';
    
    PAGE_createPage(ExprPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(ExprPage, 1) = execExpr;
    
    PAGE_createElement(ExprPage, 0, NULL, Win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ExprPage, 1, NULL, tWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(ExprPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ExprPage, 3, ExprForm, ExprWin,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(ExprPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_handler(ExprPage);

    PAGE_deletePage(ExprPage, i);
/*    CUR_delwin(MenuWin);*/
    CUR_delwin(ExprWin);
    CUR_delwin(win);
    free(ExprForm);
}

PrivateFnDef void execExpr () {
    char inbuffer[RS_MaxLine + 1], outbuffer[RS_MaxLine + 1];

    if (isBlank(FORM_fieldValue(EXPR1)) &&
	isBlank(FORM_fieldValue(EXPR2)))
    {
        ERR_displayPause (" Please Enter An Expression");
	return;
    }

    ERR_displayStr(" Executing...",FALSE);
    sprintf (inbuffer, "Named Company %s send: [ %s %s ]", 
		Company, FORM_fieldValue(EXPR1), FORM_fieldValue(EXPR2));
    
    RS_sendLine(inbuffer);

    RS_getValue(outbuffer);
    strncpy(FORM_fieldValue(RESULT), outbuffer, FORM_fieldLen(RESULT));
    FORM_fieldValue(RESULT)[FORM_fieldLen(RESULT)] = '\0';

    CUR_wattron(ExprWin, FORM_fieldAttr(RESULT));
    CUR_wmove(ExprWin, FORM_fieldY(RESULT), FORM_fieldX(RESULT));
    CUR_wprintw(ExprWin,
	"%-*.*s", FORM_fieldLen(RESULT), FORM_fieldLen(RESULT), 
					FORM_fieldValue(RESULT));
    CUR_wattroff(ExprWin, FORM_fieldAttr(RESULT));
}

/***********************************************
 **********	detailsMenu	****************
 **********************************************/
PrivateVarDef SPRSHEET *DetailReport;
PrivateVarDef PAGE *DetailPage;
PrivateVarDef MENU *DetailMenu;
PrivateVarDef void reportDetails(), printDetails();

PrivateVarDef MENU_Choice detailsChoices[] = {
 " Fundamentals ",  " Fundamentals Report",	'f', NULL, ON, 
 " Holdings ",	    " Holdings Report",		'h',  NULL, ON, 
 " Price/Div/Earn ", " Price, Dividend, Earnings Report",   'p',  NULL, ON, 
 " Estimates ",	    " Estimates Report",	'e',  reportDetails, ON, 
 " Models ",	    " Models Report",		'm', NULL, ON,
 " Ratings ",	    " Ratings Report",		'r', NULL, ON, 
 " Per Share ",	    " Per Share Report",	's', NULL, ON, 
 " Ratios ",	    " Ratios Report",		'a', NULL, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice detailActions[] = {
 " Print ", " Print Hard Copy Of Report",   'p', printDetails, ON, 
 NULL, 
};

PrivateFnDef void detailsMenu() {
    int i, j, longest;
    CUR_WINDOW *SysWin;
    
    SysWin = CUR_newwin(10, 20, 5, 5);
    
    MENU_makeMenu(DetailMenu,
	detailsChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(DetailMenu) = " Detailed Reports: ";

    MENU_handler(DetailMenu, SysWin, PAGE_Input);

    CUR_delwin(SysWin);
    MENU_deleteMenu(DetailMenu, i);
}

PrivateFnDef void detailFileMenu() {
	EDIT_reportFileMenu(DetailPage,FALSE);
}

PrivateFnDef void reportDetails() {
    int i, j, longest;
    MENU *menu;
    CUR_WINDOW *win;
    char buffer[RS_MaxLine + 1];
    char const *title;

/**** determine which report and format message ****/

    switch (MENU_choiceLetter(DetailMenu, MENU_currChoice(DetailMenu)))
    {
    case 'e':
        sprintf(buffer, "Named Company %s detailEstimates", Company);
	title = " Estimate Details: ";
        break;
    default:
        ERR_displayPause(" Undefined Report");
	return;
    }

/**** send message and read report *****/    
    ERR_displayStr(" Writing report, please wait...", FALSE);
    RS_dumpOutput();
    SPR_makeSheet(DetailReport, 10, 8, 3, 8, 0);
    RS_sendLine (buffer);
    SPR_readSSheet (DetailReport);
    
    MENU_makeMenu(menu, detailActions, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(menu) = title;

    PAGE_createPage(DetailPage, 2, NULL, menu, NULL, PAGE_menuType, i);
    win = CUR_newwin(CUR_LINES - 2, CUR_COLS, 0, 0);
    if( STD_hasInsertDeleteLine )
	CUR_idlok(win,TRUE);
    PAGE_createElement(DetailPage, 0, DetailReport, win, PAGE_Scroll, 
					    SPR_handler, TRUE);
    PAGE_createElement(DetailPage, 1, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_fkey(DetailPage, 4) = detailFileMenu;
    ERR_clearMsg();

/***** call page handler *****/
    PAGE_handler(DetailPage);

/*****  cleanup  *****/
    SPR_delete (DetailReport);
    SPR_CurrSPR = ProfileReport;
    MENU_deleteMenu(menu, i);
    CUR_delwin(win);
    PAGE_deletePage (DetailPage, i);
}


/************************************************
 *********	printDetails	*****************
 ************************************************/
PrivateFnDef void printDetails() {
    SPR_print(DetailReport, DetailPage);
}
