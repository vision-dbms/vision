/*******************************************
 **********	companyScr.c	************
 *******************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "buffers.h"
#include "page.h"
#include "form.h"
#include "report.h"
#include "rsInterface.h"


/*********************************************************
 **********	Forward Declarations		**********
 *********************************************************/

PrivateFnDef void execScreen ();

PrivateFnDef void execPrior();

PrivateFnDef void exec ();

PrivateFnDef int runScreen (
    char *			screen,
    int				expression	/*** boolean ***/
);

PrivateFnDef int initUniverse (
    char *			universe,
    int				count
);

PrivateFnDef void initHistory ();

PrivateFnDef void itemList ();

PrivateFnDef void exprScreen ();

PrivateFnDef void priorSubset ();

PrivateFnDef void clearSubsets ();

PrivateFnDef void displaySubset ();

PrivateFnDef void reportWriter ();


#define BUFFERSIZE  2048

typedef struct {
    int count;
    char item[17];
    char expr[81];
} SCREEN_LIST;

PrivateVarDef SCREEN_LIST **Screen;
PrivateVarDef int ScreenSize;
PrivateVarDef int Count;
PrivateVarDef LINEBUFFER *History;


#define ITEM		(Form1->field[2])
#define ITEM_INDX	2
#define OPERATOR	(Form1->field[4])
#define VALUE		(Form1->field[6])
#define UNIVERSE	(Form1->field[10])

PrivateVarDef PAGE *Page;
PrivateVarDef FORM *Form1;
PrivateVarDef CUR_WINDOW *Win1, *Win2, *Win3;

PrivateVarDef FORM_Field form1Fields[] = {
 1, 32, (CUR_A_BOLD | CUR_A_UNDERLINE), 15, 0, 'a', "COMPANY SCREENS", 
        NULL, NULL, NULL, 
 4, 11, CUR_A_NORMAL, 5, 0, 'a', "Item:", 
        NULL, NULL, NULL, 
 4, 17, (CUR_A_DIM | CUR_A_REVERSE), 16, (FORM_InputFlag|FORM_ScrollFlag), 'a', "                ", 
	" Enter Item, or Press F1 For Menu ", NULL, NULL, 
 6, 7, CUR_A_NORMAL, 9, 0, 'a', "Operator:", 
        NULL, NULL, NULL, 
 6, 17, (CUR_A_DIM | CUR_A_REVERSE), 4, 1, 'm', "    ", 
        " Enter Operator, or Press F1 for Menu", NULL, NULL, 
 8, 7, CUR_A_NORMAL, 9, 0, 'a', "   Value:", NULL, NULL, NULL, 
 8, 17, (CUR_A_DIM | CUR_A_REVERSE), 20, (FORM_InputFlag|FORM_ScrollFlag), 'a', "                    ", 
        " Enter Comparison Value ", NULL, NULL, 
 9, 0, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------", 
        NULL, NULL, NULL, 
 9, 40, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------", 
        NULL, NULL, NULL, 
 11, 31, CUR_A_NORMAL, 17, 0, 'a', "SCREENING HISTORY", NULL, NULL, NULL, 
 13, 5,  CUR_A_NORMAL, 60, 0, 'a', "                                                            ", 
	NULL, NULL, NULL, 
 -1, 
};


/***************************************************************
 **********	Menu Definitions 		****************
 ***************************************************************/

PrivateVarDef MENU_Choice actionChoices[] = {
 " Extended Query ", " Enter Extended Query Expression",      'e', 
     exprScreen, ON, 
 " Clear Subsets ",  " Reset to Starting Universe",	      'c', 
     clearSubsets, ON, 
 " Prior Subset ",   " Reset Current Subset to Prior Subset", 'p', 
     priorSubset, ON, 
 " Display Subset ", " Display Companies in Current Subset ", 'd', 
     displaySubset, ON, 
 " Report Writer ", " Go To Report Writer Using Current Subset ", 'r', 
     reportWriter, ON, 
 " Statistics ",     " Run Statistics For Specific Item ",    's', NULL, OFF, 
 " File ", " Save/Retrieve Screening Criteria ",	      'f', NULL, OFF, 
 NULL, 
};

PrivateVarDef MENU_Choice operatorChoices[] = {
 " >  ", " Greater Than ",		'>', FORM_menuToForm, ON, 
 " <  ", " Less Than ",			'<', FORM_menuToForm, ON, 
 " =  ", " Equal To ",			'=', FORM_menuToForm, ON, 
 " >= ", " Greater Than or Equal To ",  'g', FORM_menuToForm, ON, 
 " <= ", " Less Than or Equal To ",	'l', FORM_menuToForm, ON, 
 " != ", " Not Equal To ",		'n', FORM_menuToForm, ON, 
 NULL, 
};


PublicFnDef int companyScr(char *universe, int count) {
    MENU *actionMenu, *menu;
    int i, longest, j;
    static int alreadyCentered = FALSE;



/**** create form objects ****/
    FORM_makeForm(Form1, form1Fields, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
   	FORM_centerFormElts(Form1, CUR_COLS);
    }

    menu = MENU_getMenu("companyItemCategoryListList");
    if (menu != NULL)
    {
	for (i = 0; i < MENU_choiceCount(menu); i++)
	    MENU_choiceHandler(menu, i) = itemList;
	MENU_title(menu) = " Item Category: ";
    }
    FORM_fieldMenu(ITEM) = menu;
	
    MENU_makeMenu(menu, operatorChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(menu) = " Operator: ";
    FORM_fieldMenu(OPERATOR) = menu;

    MENU_makeMenu(actionMenu, actionChoices,
			     CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);

/*** create form windows ****/
    Win1 = CUR_newwin(14, CUR_COLS, 0, 0);
    Win2 = CUR_newwin((CUR_LINES-14-1-1), CUR_COLS-1, 14, 0);
    Win3 = CUR_newwin(1, CUR_COLS, CUR_LINES-2, 0);

    initUniverse(universe, count);
    initHistory();

/**** create Page object ****/
    PAGE_createPage(Page, 3, NULL, actionMenu, NULL, PAGE_menuType, i);
    PAGE_fkey(Page, 1) = exec;

    PAGE_createElement(Page, 0, Form1, Win1,
					 PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(Page, 1, History, Win2,
					 PAGE_Scroll, BUF_handler, FALSE);
    PAGE_createElement(Page, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

/**** call Page handler ****/

    PAGE_handler(Page);
           
/*****  cleanup  *****/

    MENU_deleteMenu(FORM_fieldMenu(ITEM), i);
    MENU_deleteMenu(FORM_fieldMenu(OPERATOR), i);
    free (Form1);
    CUR_delwin(Win1);
    CUR_delwin(Win2);
    CUR_delwin(Win3);

    MENU_deleteMenu(actionMenu, i);
    PAGE_deletePage (Page, i);

    BUF_deleteBuffer(History);

    for (i = 0; i <= Count; i++)
	free(Screen[i]);
    free(Screen);

    return (FALSE);
}


/***************************************************
 ************	Menu Functions	********************
 **************************************************/
PrivateVarDef PAGE *MenuPage;

PrivateFnDef void clearSubsets () {
    int i;
    
    if (Count < 1)
    {
        ERR_displayPause(" No Subsets To Clear");
	return;
    }
    
    BUF_eraseBuffer(History);
    CUR_werase(Win2);

    for (i = 1; i <= Count; i++)
        free(Screen[i]);
    Count = 0;
}


/****************************************************
 *********	exprScreen	    *****************
 ****************************************************/
#define EXPR1	    (ExprForm->field[1])
#define EXPR2	    (ExprForm->field[2])

PrivateVarDef FORM_Field exprFields[] = {
 1, 1, CUR_A_NORMAL, 11, 0, 'a', "Expression:", 
        NULL, NULL, NULL, 
 1, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ",
 " Enter Screening Expression", NULL, NULL, 
 2, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ",
 " Enter Screening Expression", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PrivateVarDef FORM *ExprForm;
PrivateVarDef CUR_WINDOW *ExprWin;

PrivateFnDef void exprScreen () {
    int i;
    CUR_WINDOW *win;
    CUR_WINDOW *MenuWin;
    
    win = CUR_newwin(10, 60, 10, 0);
    WIN_ReverseBox(win,NULL);
    ExprWin = CUR_subwin(win, 8, 58, 11, 1);
    MenuWin = CUR_newwin(11, 21, 5, 55);

    FORM_makeForm(ExprForm, exprFields, i);
    FORM_fieldValue(EXPR1)[0] = '\0';
    FORM_fieldValue(EXPR2)[0] = '\0';
    
    PAGE_createPage(MenuPage, 4, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execScreen;
    
    PAGE_createElement(MenuPage, 0, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, NULL, MenuWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 3, ExprForm, ExprWin,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_handler(MenuPage);

    PAGE_deletePage(MenuPage, i);
    CUR_delwin(MenuWin);
    CUR_delwin(ExprWin);
    CUR_delwin(win);
    free(ExprForm);
}

PrivateFnDef void execScreen () {
    char buffer[RS_MaxLine + 1];

    PAGE_status(MenuPage) = PAGE_Normal;
    if (isBlank(FORM_fieldValue(EXPR1)) &&
	isBlank(FORM_fieldValue(EXPR2)))
    {
        ERR_displayPause (" Please Enter An Expression");
	return;
    }

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    sprintf(buffer, "%s %s", FORM_fieldValue(EXPR1), FORM_fieldValue(EXPR2));
    runScreen(buffer, TRUE);
}


/**************************************************
 **********	priorSubset	*******************
 *************************************************/

PrivateFnDef void priorSubset () {
    int i;

    if (Count <= 1)    
    {
        ERR_displayPause(" No Prior Subsets");
	return;
    }

    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
    " Use Arrow Keys To Select Prior Subset, Execute(F2), Quit(F9)");

    PAGE_createPage(MenuPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execPrior;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, History, Win2,
				    PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    BUF_status(History) = BUF_Hilight;

    PAGE_handler(MenuPage);

    BUF_status(History) = BUF_Normal;
    PAGE_deletePage(MenuPage, i);
    CUR_werase(Win3);
}

PrivateFnDef void execPrior() {
    char *ptr, *row;
    int i, count;

/*** get count of prior screen selected by user ****/
    ptr = BUF_line(BUF_row(History));
    count = atoi(ptr);

/**** delete remaining rows in buffer ****/
    row = BUF_nextLine(BUF_row(History));
    while (row != NULL)
    {
	BUF_deleteLine(History, row);
	row = BUF_nextLine(row);
    }

    BUF_changeRow(History, BUF_lastLine(History));
    if (count > CUR_WIN_rows(Win2))
	BUF_screenRow(History) = CUR_WIN_maxy(Win2);
    else
	BUF_screenRow(History) = count - 1;
/*    BUF_resetScreen(History, Win2);*/
    BUF_paintWindow(History, Win2);

/*** delete screen's and set Count to count ***/
    for (i = (count + 1); i <= Count; i++)
        free(Screen[i]);
    Count = count;
}


/***************************************************
 **********	displaySubset		************
 ***************************************************/

PrivateFnDef void displaySubset () {
    int i;
    char string[80];
    LINEBUFFER *displayBuf;
    CUR_WINDOW *win, *swin;

    if (Screen[Count]->count < 1)
    {
        ERR_displayPause(" No Companies In Subset");
	return;
    }

    ERR_displayStr(" Getting Companies, please wait...",FALSE);
    
    win = CUR_newwin(CUR_LINES - 1, 40, 0, CUR_COLS-41);
    WIN_ReverseBox(win,NULL);
    CUR_wattron(win, CUR_A_REVERSE);
    CUR_wmove(win, CUR_WIN_maxy(win), 1);
    CUR_wprintw(win, " PgUp, PgDn, Home, End, Quit(F9) ");
    CUR_wattroff(win, CUR_A_REVERSE);
    swin = CUR_subwin(win, CUR_LINES - 3, 38, 1, CUR_COLS-40);

    displayBuf = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(displayBuf) = 40;
    BUF_initBuffer(displayBuf, (100 * BUFFERSIZE));

    sprintf(string, "__subset%d displayBasicData", Count);
    RS_sendOnly(string);
    RS_compile();
    RS_readOutput(displayBuf, displayBuf);
    
    PAGE_createPage(MenuPage, 4, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, NULL, Win2, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 2, NULL, win,  PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 3, displayBuf, swin, 
				    PAGE_Scroll, BUF_handler, TRUE);
    ERR_displayStr(
    "Use Arrow Keys, PgUp, PgDn, Home, End, Quit(F9)",FALSE);

    PAGE_handler(MenuPage);

    PAGE_deletePage(MenuPage, i);
    CUR_delwin(swin);
    CUR_delwin(win);

    BUF_deleteBuffer(displayBuf);
}


/***************************************************
 **********	reportWriter		************
 ***************************************************/

PrivateFnDef void reportWriter () {
    char buffer[80], **reportItem;
    int i, j;
    
    sprintf(buffer, "!companyUniverseScreeningList <- __subset%d", Count);
    if (RS_sendAndCheck(buffer, ">>>"))
    {
        ERR_displayPause(" Error creating screening list");
	return;
    }

    reportItem = (char **) malloc((Count + 2) * sizeof(char *));
    if (reportItem == NULL)
    {
        ERR_displayPause(" Error Allocating Space");
	return;
    }
    
    j = 0;
    for (i = 0; i <= Count; i++)
	if (Screen[i]->item[0] != '\0')
	    reportItem[j++] = Screen[i]->item;
	    
    reportItem[j] = NULL;
    
    report("Screening", reportItem);

    free(reportItem);
    return;
}


/***************************************************
 **********	Exec Functions		************
 ***************************************************/
#define displayField(win, fptr)\
{\
    CUR_wattron(win, FORM_fieldAttr(fptr));\
    CUR_wmove(win, FORM_fieldY(fptr), FORM_fieldX(fptr));\
    CUR_wprintw(win,\
    "%-*.*s", FORM_fieldLen(fptr), FORM_fieldLen(fptr), \
					FORM_fieldValue(fptr));\
    CUR_wattroff(win, FORM_fieldAttr(fptr));\
}

PrivateFnDef void exec () {
    char buffer[RS_MaxLine + 1];
    
    if (isBlank(FORM_fieldValue(ITEM)))
    {
        ERR_displayPause(" Please Enter Item to Screen On");
	return;
    }
    
    if (isBlank(FORM_fieldValue(VALUE)))
    {
        ERR_displayPause(" Please Enter Value For Comparison");
	return;
    }

    sprintf(buffer, "%s%s%s", 
    FORM_fieldValue(ITEM), FORM_fieldValue(OPERATOR), FORM_fieldValue(VALUE));

    runScreen(buffer, FALSE);
}

PrivateFnDef int runScreen (
    char *			screen,
    int				expression	/*** boolean ***/
)
{
    char *ptr, buffer[RS_MaxLine + 1];

    ERR_displayStr(" Executing...",FALSE);

    Count++;
    if (ScreenSize <= Count)
    {
      ptr = (char*)realloc(Screen, (ScreenSize + 10) * sizeof(SCREEN_LIST *));
	if (ptr == NULL) ERR_fatal(" Error allocating space");
        ScreenSize += 10;
	Screen = (SCREEN_LIST **)ptr;
    }
    
    if (NULL == (Screen[Count] =
       (SCREEN_LIST *)calloc(1, sizeof(SCREEN_LIST))))
	ERR_fatal(" Error allocating space");
    
    strcpy(Screen[Count]->expr, screen);
    if (expression)
        Screen[Count]->item[0] = '\0';
    else
	strcpy(Screen[Count]->item, FORM_fieldValue(ITEM));
			
    sprintf(buffer, "!__subset%d <- __subset%d select: [ %s ] ", 
	    Count, (Count - 1), Screen[Count]->expr);
    
    if (RS_sendAndCheck(buffer, ">>>"))
    {
        free(Screen[Count]);
	Count--;
	ERR_displayPause(" Error Executing Screen");
	return(FALSE); 
    }
    
    sprintf(buffer, "__subset%d count", Count);
    RS_sendLine(buffer);
    RS_getValue(buffer);
    Screen[Count]->count = atoi(buffer);
    if (Screen[Count]->count <= 0)
    {
        free(Screen[Count]);
	Count--;
	ERR_displayPause(" No Companies Found - Screen Ignored");
	return(FALSE);
    }
    
    sprintf(buffer, "     %d.  %-40.40s %d Companies", Count, 
		    Screen[Count]->expr, Screen[Count]->count);
    ptr = BUF_appendLine(History, buffer);
    BUF_changeRow(History, ptr);

    if (Count > CUR_WIN_rows(Win2))
	BUF_screenRow(History) = CUR_WIN_maxy(Win2);
    else
	BUF_screenRow(History) = Count - 1;

/*    BUF_resetScreen(History, Win2);*/
    BUF_paintWindow(History, Win2);

/**** reset fields ****/
    FORM_fieldValue(ITEM)[0] = '\0';
    displayField(Win1, ITEM);
    FORM_fieldValue(VALUE)[0] = '\0';
    displayField(Win1, VALUE);
    FORM_currField(Form1) = ITEM_INDX;

    ERR_clearMsg();
    return(FALSE);
}



/*************************************************
 **********	Misc. Functions		**********
 *************************************************/

PrivateFnDef int initUniverse (
    char *			universe,
    int				count
)
{
    Screen = (SCREEN_LIST **) calloc(10, sizeof(SCREEN_LIST *));
    ScreenSize = 10;
    Screen[0] = (SCREEN_LIST *) calloc(1, sizeof(SCREEN_LIST));
    Count = 0;
    
    strcpy(Screen[0]->expr, universe);
    Screen[0]->count = count;

    sprintf(FORM_fieldValue(UNIVERSE), 
	    "%s Universe Contains %d Companies", universe, count);

    return FALSE;
}

PrivateFnDef void initHistory () {
    History = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(History) = BUF_maxlinesize-1;
    BUF_initBuffer(History, (10 * BUFFERSIZE));    
}

PrivateFnDef void itemList () {
    MENU *menu1, *menu2;
    int choice, i;
    char string[80];
    CUR_WINDOW *SysWin;
    
    menu1 = FORM_fieldMenu(ITEM);
    choice = MENU_currChoice(menu1);
    
    sprintf(string, "companyItemCategory%sList", MENU_choiceLabel(menu1, choice).content ());
    menu2 = MENU_getMenu(string);
    
    if (menu2 == NULL)
    {
        ERR_displayPause(" No items for category selected");
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
