/*******************************************
 **********	report.c	************
 *******************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "page.h"
#include "edit.h"
#include "form.h"
#include "rsInterface.h"
#include "spsheet.h"
#include "keys.h"
#include "buffers.h"

#include "report.h"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef void execInsert ();

PrivateFnDef void execMove ();

PrivateFnDef void moveIt ();

PrivateFnDef void execDelete ();

PrivateFnDef void execReplace ();

PrivateFnDef void execOptions ();

PrivateFnDef void initOptions ();

PrivateFnDef void execExpr ();

PrivateFnDef void initBuffer ();

PrivateFnDef void itemList ();

PrivateFnDef void initSprPage();
PrivateFnDef void initUniverse ();
PrivateFnDef void execUniverse ();


/*************************************************
 **********	Constants and Globals	**********
 *************************************************/

#define COLWIDTH 12
#define TITLEROWS 5
#define FIXEDROWS 1
#define FIXEDCOLS 2
#define BUFFERSIZE  2048

typedef struct {
    char const *type;
    char const *format;
} TOTTABLE;

PrivateVarDef TOTTABLE Table[] = {
 " Total ",	":totalBlock <- [ aList total: [%s] ];", 
 " Average ",   ":totalBlock <- [ aList average: [%s] ];", 
 " Per Share ",
 ":totalBlock <- [ (aList total: [ %s * sharesOut]) / (aList total: [sharesOut]) ];",
 " Mkt Cap ",
 ":totalBlock <- [ (aList total: [ %s * marketCap] ) / (aList total: [marketCap]) ];", 
 " None ",	":totalBlock <- NA ;", 
 NULL, 
};

PrivateVarDef char const *Universe;
PrivateVarDef char **ScreenItem;
PrivateVarDef LINEBUFFER *DisplayBuf;
PrivateVarDef SPRSHEET *Sprsheet = NULL;

PrivateVarDef CUR_WINDOW *Win1, *Win2, *Win3, *SprWin, *AppWin, *StatWin;
PrivateVarDef FORM *Form, *Form1, *Form2, *Form3, *ParmForm;
PrivateVarDef PAGE *ItemsPage, *ReportPage, *ApplicPage, *PPage;
PrivateVarDef MENU::Reference itemsActionMenu, AppActionMenu;

#define UNIVERSE	(ParmForm->field[1])

#define ITEM	(Form->field[2])
#define ITEM_INDX 2
#define HEADING	(Form->field[4])
#define WIDTH	(Form->field[6])
#define DECIMAL	(Form->field[8])
#define TOTAL	(Form->field[10])

PrivateVarDef FORM_Field formFields[] = {
 1, 33, (CUR_A_BOLD | CUR_A_UNDERLINE), 13, 0, 'a', "REPORT WRITER", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 4, 11, CUR_A_NORMAL, 5, 0, 'a', "Item:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 4, 17, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', 
 "                ", " Use Arrow Keys To Select Item, or F1 For Menu", MENU::Reference(), NULL, 
 4, 36, CUR_A_NORMAL, 8, 0, 'a', "Heading:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 4, 45, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', "                ", 
        " Enter Column(Row) Label For Item", MENU::Reference(), NULL, 
 6, 10, CUR_A_NORMAL, 6, 0, 'a', "Width:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 6, 17, (CUR_A_DIM | CUR_A_REVERSE), 5, 0, 'm', "     ", 
    " Use Arrow Keys To Select Width, or F1 For Menu", MENU::Reference(), NULL, 
 6, 35, CUR_A_NORMAL, 9, 0, 'a', "Decimals:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 6, 45, (CUR_A_DIM | CUR_A_REVERSE), 5, 0, 'm', "     ", 
    " Use Arrow Keys To Select Decimals Places, or F1 For Menu", MENU::Reference(), NULL, 
 6, 54, CUR_A_NORMAL, 6, 0, 'a', "Total:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 6, 61, (CUR_A_DIM | CUR_A_REVERSE), 12, 1, 'm', "            ", 
    " Use Arrow Keys To Select Total Type, or F1 For Menu", MENU::Reference(), NULL, 
 8, 0, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 8, 40, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 5, CUR_A_NORMAL, 3, 0, 'a', "Col", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 12, CUR_A_NORMAL, 4, 0, 'a', "Item", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 29, CUR_A_NORMAL, 7, 0, 'a', "Heading", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 46, CUR_A_NORMAL, 5, 0, 'a', "Width", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 56, CUR_A_NORMAL, 8, 0, 'a', "Decimals", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 67, CUR_A_NORMAL, 5, 0, 'a', "Total", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 -1, 
};


/***************************************************************
 **********	Menu Definitions 		****************
 ***************************************************************/
PrivateFnDef void addField(), deleteField(), insertField(), replaceField(), 
    moveField(), itemsReportOptions(), clearFields(), specialItem(), 
        screeningItems(), reportItems(), getItemsUniverse(), 
        outerReportOptions(), getOuterUniverse();

PrivateVarDef MENU_Choice actionChoices[] = {
 " Add ",     " Add Field To Report Format ",	   'a', addField, ON, 
 " Delete ",  " Delete Field From Report Format ", 'd', deleteField, ON, 
 " Insert ",  " Insert Field Into Report Format ", 'i', insertField, ON, 
 " Move ",    " Move Field Within Report Format ", 'm', moveField, ON, 
 " Replace ", " Replace Field In Report Format ",  'r', replaceField, ON, 
 " Options ", " Select Report Options ",	   'o', itemsReportOptions, ON, 
 " Parameters "   , " Select Report Parameters"	       , 'p', getItemsUniverse,ON,
 " Clear ",   " Delete All Fields In Report Format ", 
					           'c', clearFields, ON, 
 " Expression ", " Define Item Expression ",	   'e', specialItem, ON, 
 " Screen Items ", " Add Screening Items to Report Format", 
						   's', screeningItems, ON, 
 " File ", " Save/Retrieve Report Format ",	   'f', NULL, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice reportChoices[] = {
 " Parameters ",   " Change Report Parameters",	'p', getOuterUniverse, ON, 
 " Items ",    " Change Report Items to Display", 'i', reportItems, ON, 
 " Options ",    " Change Report Options", 'o', outerReportOptions, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice widthChoices[] = {
 " 12 ", " 12 Character Column Width ",	'1', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice decimalChoices[] = {
 " 2 ",	" 2 Decimals Places ",	'2', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice totalChoices[] = {
 " None ",	" No Total For This Item",	'n', FORM_menuToForm, ON, 
 " Total ",	" Total For All Companies",	't', FORM_menuToForm, ON, 
 " Average ",   " Average Of All Companies",	'a', FORM_menuToForm, ON, 
 " Per Share ", " Per Share Total ",		'p', FORM_menuToForm, ON, 
 " Mkt Cap ",   " Market Cap. Total",		'm', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef int	firstTime = TRUE, gotInitialInput = FALSE, didExec = FALSE;
PublicVarDef  int	inReport = FALSE;
PrivateFnDef void	exec();

PublicFnDef PAGE_Action report(char const *universe, char **itemArray) {
    MENU::Reference menu;
    int i, longest, j;

    if( inReport )
    {
    	ERR_displayPause("Report Writer already running");
    	return(PAGE_Input);
    }
    inReport = TRUE;
        
    Universe = universe;
    ScreenItem = itemArray;

    if( firstTime ) {
    	ERR_displayStr(" Getting Universes...",FALSE);
    	firstTime = FALSE;    
	SPR_makeSheet(Sprsheet, COLWIDTH, 1, FIXEDCOLS, TITLEROWS, FIXEDROWS);
	SPR_makeDummySheet(Sprsheet);

/**** create form objects ****/
	FORM_makeForm(Form, formFields, i);

	itemsActionMenu.setTo (new MENU (actionChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	MENU_title(itemsActionMenu) = " Report Writer:";

	menu.setTo (new MENU (widthChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	MENU_title(menu) = " Width:";
	FORM_fieldMenu(WIDTH).setTo (menu);
	menu.setTo (new MENU (decimalChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	MENU_title(menu) = " Decimals:";
	FORM_fieldMenu(DECIMAL).setTo (menu);
	menu.setTo (new MENU (totalChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	MENU_title(menu) = " Total Type:";
	FORM_fieldMenu(TOTAL).setTo (menu);

/*** create form windows ****/
	initBuffer();

/**** initialize option forms and display buffer ****/
	menu = MENU_getMenu("companyItemCategoryListList");
	if (menu.isntNil ())
	{
            MENU_title(menu) = " Item Categories:";
	    for (i = 0; i < MENU_choiceCount(menu); i++)
		MENU_choiceHandler(menu, i) = itemList;
	}
	FORM_fieldMenu(ITEM).setTo (menu);
    
	initOptions();
	initUniverse();
	ERR_clearMsg();
    }

    SprWin = CUR_newwin(CUR_LINES - 2, CUR_COLS, 0, 0);
    if( STD_hasInsertDeleteLine )
	CUR_idlok(SprWin,TRUE);
    initSprPage();
    if( Universe != NULL )
	strncpy(FORM_fieldValue(UNIVERSE), Universe, FORM_fieldLen(UNIVERSE));

    if( !gotInitialInput )
    {
    	getOuterUniverse();
	if( !gotInitialInput )
	{
	    CUR_delwin(SprWin);
	    CUR_delwin(AppWin);
	    CUR_delwin(StatWin);
	    AppActionMenu.clear ();
	    PAGE_deletePage(ReportPage, i);
	    PAGE_deletePage(ApplicPage, i);
	    inReport = FALSE;
	    return(PAGE_Input);
	}
	else
	    KEY_putc('i');
    }

    if( !didExec )
	PAGE_ExitF8PageParent = TRUE;
    SPR_simulateApplic = TRUE;
    PAGE_handler(ReportPage);
    PAGE_ExitF8PageParent = FALSE;
    CUR_delwin(SprWin);
    CUR_delwin(AppWin);
    CUR_delwin(StatWin);
    AppActionMenu.clear ();
    PAGE_deletePage(ReportPage, i);
    PAGE_deletePage(ApplicPage, i);

    inReport = FALSE;
    return (PAGE_Input);
}

PrivateVarDef int	inItemsPage = FALSE;

PrivateFnDef void reportItems() {
    int	i;
    Win1 = CUR_newwin(11, CUR_COLS, 0, 0);
    Win2 = CUR_newwin((CUR_LINES-11-1-1), CUR_COLS, 11, 0);
    Win3 = CUR_newwin(1,  CUR_COLS, CUR_LINES-2, 0);
    if( STD_hasInsertDeleteLine )
	CUR_idlok(Win2,TRUE);

    PAGE_createPage(ItemsPage, 3, NULL, itemsActionMenu, NULL, PAGE_menuType, i);
    PAGE_fkey(ItemsPage, 1) = exec;

    PAGE_createElement(ItemsPage, 0, Form, Win1, PAGE_Input, FORM_handler, TRUE);
    PAGE_createElement(ItemsPage, 1, DisplayBuf,
    		 Win2, PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(ItemsPage, 2, NULL, Win3, PAGE_Input, NULL, FALSE);

    inItemsPage = TRUE;
    PAGE_handler(ItemsPage);
    inItemsPage = FALSE;

    CUR_delwin (Win1);
    CUR_delwin (Win2);
    CUR_delwin (Win3);
    PAGE_deletePage(ItemsPage, i);
}

/************************************************
 **********	Menu Functions		*********
 ***********************************************/

PrivateVarDef PAGE *MenuPage;

PrivateFnDef void newField(SPR_Field *fptr) {
    
    strcpy(SPR_item(fptr), FORM_fieldValue(ITEM));
    if (isBlank(FORM_fieldValue(HEADING)))
	strcpy(SPR_heading(fptr), FORM_fieldValue(ITEM));
    else
	strcpy(SPR_heading(fptr), FORM_fieldValue(HEADING));
    strcpy(SPR_width(fptr), FORM_fieldValue(WIDTH));
    strcpy(SPR_decimals(fptr), FORM_fieldValue(DECIMAL));
    strcpy(SPR_totalType(fptr), FORM_fieldValue(TOTAL));
    FORM_fieldValue(ITEM)[0] = '\0';
    CUR_wattron(Win1, FORM_fieldAttr(ITEM));
    CUR_wmove(Win1, FORM_fieldY(ITEM), FORM_fieldX(ITEM));
    CUR_wprintw(Win1,
    "%-*.*s", FORM_fieldLen(ITEM), FORM_fieldLen(ITEM), 
					FORM_fieldValue(ITEM));
    CUR_wattroff(Win1, FORM_fieldAttr(ITEM));
    FORM_fieldValue(HEADING)[0] = '\0';
    CUR_wattron(Win1, FORM_fieldAttr(HEADING));
    CUR_wmove(Win1, FORM_fieldY(HEADING), FORM_fieldX(HEADING));
    CUR_wprintw(Win1,
    "%-*.*s", FORM_fieldLen(HEADING), FORM_fieldLen(HEADING), 
					FORM_fieldValue(HEADING));
    CUR_wattroff(Win1, FORM_fieldAttr(HEADING));
    strcpy(FORM_fieldValue(TOTAL), MENU_choiceLabel(FORM_fieldMenu(TOTAL), 0));
    CUR_wattron(Win1, FORM_fieldAttr(TOTAL));
    CUR_wmove(Win1, FORM_fieldY(TOTAL), FORM_fieldX(TOTAL));
    CUR_wprintw(Win1,
    "%-*.*s", FORM_fieldLen(TOTAL), FORM_fieldLen(TOTAL), 
					FORM_fieldValue(TOTAL));
    CUR_wattroff(Win1, FORM_fieldAttr(TOTAL));
    FORM_currField(Form) = ITEM_INDX;
}

PrivateFnDef void printField(SPR_Field *fptr, int col) {
    char string[100];
    
/**** NOTE: col must printed as it is used in insert, delete, etc. ****/

    sprintf(string, " %6d     %-16.16s %-16.16s %-9.9s %-9.9s %-12.12s", 
					 col, 
					 SPR_item(fptr),
					 SPR_heading(fptr),
					 SPR_width(fptr),
					 SPR_decimals(fptr),
					 SPR_totalType(fptr));
    BUF_appendLine(DisplayBuf, string);
}

PrivateFnDef void displayFields()
{
    if( BUF_lastLine(DisplayBuf) != NULL )
	BUF_changeRow(DisplayBuf, BUF_lastLine(DisplayBuf));
    if (SPR_fieldCount(Sprsheet) > CUR_WIN_rows(Win2))
	BUF_screenRow(DisplayBuf) = CUR_WIN_maxy(Win2);
    else
	BUF_screenRow(DisplayBuf) = SPR_fieldCount(Sprsheet) - 1;
    BUF_resetScreen(DisplayBuf, Win2);
    BUF_paintWindow(DisplayBuf, Win2);
}

/************************************************
 **********	clearFields	*****************
 ***********************************************/
PrivateFnDef void clearFields() {
    int i;
    
    if (SPR_fieldCount(Sprsheet) <= 0)
        return;
	
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
        free(SPR_field(Sprsheet, i));
    SPR_fieldCount(Sprsheet) = 0;

    BUF_eraseBuffer(DisplayBuf);
    displayFields();
}


/************************************************
 **********	addField	*****************
 ***********************************************/
PrivateFnDef void addField() {
    SPR_Field *fptr;
    char *ptr;

    if (isBlank(FORM_fieldValue(ITEM)))
    {
        ERR_displayPause(" Please Enter An Item To Print");
	return;
    }
    
    SPR_addField(Sprsheet, ptr);

    fptr =  SPR_field(Sprsheet, SPR_fieldCount(Sprsheet) - 1);   

    newField(fptr);

    printField(fptr, SPR_fieldCount(Sprsheet));

    displayFields();
}


/************************************************
 **********	insertField	*****************
 ***********************************************/
PrivateFnDef void insertField() {
    int i;

    if (SPR_fieldCount(Sprsheet) <= 0)    
    {
        ERR_displayPause(" No Fields To Insert Before");
	return;
    }
    
    if (isBlank(FORM_fieldValue(ITEM)))
    {
        ERR_displayPause(" Please Enter An Item To Print");
	return;
    }

    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
	" Use Arrow Keys To Select INSERT Position, Execute(F2), Quit(F9)");

    PAGE_createPage(MenuPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execInsert;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, DisplayBuf, Win2,
				    PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    BUF_status(DisplayBuf) = BUF_Hilight;

    PAGE_handler(MenuPage);

    BUF_status(DisplayBuf) = BUF_Normal;
    CUR_werase(Win3);
    PAGE_deletePage(MenuPage, i);
}

PrivateFnDef void execInsert () {
    char *ptr;
    int i, field;
    SPR_Field *fptr, *holdfptr;

    ptr = BUF_line(BUF_row(DisplayBuf));
    field = atoi(ptr) - 1;

    SPR_addField(Sprsheet, ptr);
    holdfptr = SPR_field(Sprsheet, (SPR_fieldCount(Sprsheet) - 1));
    
    for (i = (SPR_fieldCount(Sprsheet) - 1); i > field; i--)
        SPR_field(Sprsheet, i) = SPR_field(Sprsheet, i - 1);
	

    SPR_field(Sprsheet, field) = holdfptr;
    fptr =  SPR_field(Sprsheet, field);   
    newField(fptr);

    BUF_eraseBuffer(DisplayBuf);
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
    {
        fptr = SPR_field(Sprsheet, i);
	printField(fptr, (i + 1));
    }

    displayFields();
}



/************************************************
 **********	moveField	*****************
 ***********************************************/
PrivateFnDef void moveField() {
    int i;

    if (SPR_fieldCount(Sprsheet) <= 0)    
    {
        ERR_displayPause(" No Fields To Move");
	return;
    }
    
    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
	" Use Arrow Keys To Select Field To MOVE, Execute(F2), Quit(F9)");

    PAGE_createPage(MenuPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execMove;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, DisplayBuf, Win2,
				    PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    BUF_status(DisplayBuf) = BUF_Hilight;

    PAGE_handler(MenuPage);

    BUF_status(DisplayBuf) = BUF_Normal;
    CUR_werase(Win3);
    PAGE_deletePage(MenuPage, i);
}

PrivateVarDef int FieldToMove;

PrivateFnDef void execMove () {
    char *ptr;

    ptr = BUF_line(BUF_row(DisplayBuf));
    FieldToMove = atoi(ptr) - 1;

    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
	" Use Arrow Keys To Select MOVE Position, Execute(F2), Quit(F9)");
    PAGE_fkey(MenuPage, 1) = moveIt;
    PAGE_handler(MenuPage);
}

PrivateFnDef void moveIt () {
    char *ptr;
    int i, insField;
    SPR_Field *fptr, *holdfptr;

    ptr = BUF_line(BUF_row(DisplayBuf));
    insField = atoi(ptr) - 1;

    holdfptr = SPR_field(Sprsheet, FieldToMove);
    
    if (FieldToMove > insField)
    {
	for (i = FieldToMove; i > insField; i--)
	    SPR_field(Sprsheet, i) = SPR_field(Sprsheet, i - 1);
	SPR_field(Sprsheet, insField) = holdfptr;
    }
    else if (FieldToMove < insField)
    {
	for (i = FieldToMove; i < (insField - 1); i++)
	    SPR_field(Sprsheet, i) = SPR_field(Sprsheet, i + 1);
	SPR_field(Sprsheet, (insField - 1)) = holdfptr;
    }
    else
	return;
	
    BUF_eraseBuffer(DisplayBuf);
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
    {
        fptr = SPR_field(Sprsheet, i);
	printField(fptr, (i + 1));
    }
    displayFields();
}



/**************************************************
 **********	deleteField	*******************
 *************************************************/
PrivateFnDef void deleteField() {
    int i;

    if (SPR_fieldCount(Sprsheet) <= 0)    
    {
        ERR_displayPause(" No Fields To Delete");
	return;
    }
    
    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
    " Use Arrow Keys To Select Field To DELETE, Execute(F2), Quit(F9)");

    PAGE_createPage(MenuPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execDelete;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, DisplayBuf, Win2,
				    PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    BUF_status(DisplayBuf) = BUF_Hilight;

    PAGE_handler(MenuPage);

    BUF_status(DisplayBuf) = BUF_Normal;
    CUR_werase(Win3);
    PAGE_deletePage(MenuPage, i);
}

PrivateFnDef void execDelete () {
    char *ptr;
    int i, field;
    SPR_Field *fptr;

    ptr = BUF_line(BUF_row(DisplayBuf));
    field = atoi(ptr) - 1;

    free(SPR_field(Sprsheet, field));
    
    if ((field + 1) != SPR_fieldCount(Sprsheet))
    {
	SPR_fieldCount(Sprsheet)--;
        for (i = field; i < SPR_fieldCount(Sprsheet); i++)
	    SPR_field(Sprsheet, i) = SPR_field(Sprsheet, i + 1);
    }
    else
	SPR_fieldCount(Sprsheet)--;
    

    BUF_eraseBuffer(DisplayBuf);
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
    {
        fptr = SPR_field(Sprsheet, i);
	printField(fptr, (i + 1));
    }

    displayFields();
}


/************************************************
 **********	replaceField	*****************
 ***********************************************/
PrivateFnDef void replaceField() {
    int i;

    if (SPR_fieldCount(Sprsheet) <= 0)    
    {
        ERR_displayPause(" No Fields To Replace");
	return;
    }
    
    if (isBlank(FORM_fieldValue(ITEM)))
    {
        ERR_displayPause(" Please Enter An Item To Print");
	return;
    }
    
    ERR_clearMsg();
    CUR_wmove(Win3, 0, 0);
    CUR_wprintw(Win3, 
    " Use Arrow Keys To Select Field To REPLACE, Execute(F2), Quit(F9)");

    PAGE_createPage(MenuPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execReplace;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, DisplayBuf, Win2,
				    PAGE_Scroll, BUF_handler, TRUE);
    PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);

    PAGE_status(MenuPage) = PAGE_ExitOnExec;
    BUF_status(DisplayBuf) = BUF_Hilight;

    PAGE_handler(MenuPage);

    BUF_status(DisplayBuf) = BUF_Normal;
    CUR_werase(Win3);
    PAGE_deletePage(MenuPage, i);
}

PrivateFnDef void execReplace () {
    char *ptr;
    int i, field;
    SPR_Field *fptr;

    ptr = BUF_line(BUF_row(DisplayBuf));
    field = atoi(ptr) - 1;
    fptr =  SPR_field(Sprsheet, field);   
    newField(fptr);

    BUF_eraseBuffer(DisplayBuf);
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
    {
        fptr = SPR_field(Sprsheet, i);
	printField(fptr, (i + 1));
    }
    displayFields();
}



/****************************************************
 **********	reportOptions		*************
 ***************************************************/
#define TITLE1		(Form1->field[1])
#define TITLE2		(Form1->field[2])
#define SORT		(Form1->field[4])
#define ORDER		(Form1->field[6])
#define GROUP1		(Form1->field[8])
#define GROUP2		(Form1->field[10])
#define SUBTOTAL	(Form1->field[12])
#define GRAND		(Form1->field[14])
#define FORMAT		(Form1->field[16])

PrivateVarDef FORM_Field form1Fields[] = {
 1, 1, CUR_A_NORMAL, 6, 0, 'a', "Title:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 1, 8, (CUR_A_DIM | CUR_A_REVERSE), 50, (FORM_InputFlag|FORM_ScrollFlag), 'a', 
"                                        ", " Enter Report Title", MENU::Reference(), NULL, 
 2, 8, (CUR_A_DIM | CUR_A_REVERSE), 50, (FORM_InputFlag|FORM_ScrollFlag), 'a', 
"                                        ", " Enter Report Title", MENU::Reference(), NULL, 
 4, 6, CUR_A_NORMAL, 8, 0, 'a', "Sort By:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 4, 15, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', "                ", 
        " Use Arrow Keys To Select Item, or F1 For Menu", MENU::Reference(), NULL, 
 4, 35, CUR_A_NORMAL, 6, 0, 'a', "Order:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 4, 42, (CUR_A_DIM | CUR_A_REVERSE), 12, 1, 'm', "            ", 
        " Use Arrow Keys To Select Sort Order, or F1 For Menu", MENU::Reference(), NULL, 
 6, 6, CUR_A_NORMAL, 8, 0, 'a', "Group 1:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 6, 15, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', "                ", 
        " Use Arrow Keys To Select Item, or F1 For Menu", MENU::Reference(), NULL, 
 6, 33, CUR_A_NORMAL, 8, 0, 'a', "Group 2:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 6, 42, (CUR_A_DIM | CUR_A_REVERSE), 16, 1, 'a', "                ", 
        " Use Arrow Keys To Select Item, or F1 For Menu", MENU::Reference(), NULL, 
 8, 1, CUR_A_NORMAL, 13, 0, 'a', "Group Totals:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 8, 15, (CUR_A_DIM | CUR_A_REVERSE), 5, 1, 'm', "     ", 
        " Use Arrow Keys To Select Yes/No", MENU::Reference(), NULL, 
 8, 28, CUR_A_NORMAL, 13, 0, 'a', "Grand Totals:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 8, 42, (CUR_A_DIM | CUR_A_REVERSE), 5, 1, 'm', "     ", 
        " Use Arrow Keys To Select Yes/No", MENU::Reference(), NULL, 
 10, 7, CUR_A_NORMAL, 7, 0, 'a', "Format:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 10, 15, (CUR_A_DIM | CUR_A_REVERSE), 10, 1, 'm', "          ", 
        " Use Arrow Keys To Select Report Format, or F1 For Menu", MENU::Reference(), NULL, 
-1, 
};

PrivateVarDef MENU_Choice orderChoices[] = {
 " Ascending ",	 " Sort In Ascending Order",    'a', FORM_menuToForm, ON, 
 " Descending ", " Sort In Descending Order",   'd', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice formChoices[] = {
 " Standard ",  " Print Companies As Rows, Items As Columns", 's',
      FORM_menuToForm, ON, 
 " Flipped ", " Print Companies As Columns, Items As Rows", 'f',
      FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef MENU_Choice booleanChoices[] = {
  " No ",   static_cast<char const*>(NULL), 'n', FORM_menuToForm, ON, 
  " Yes ",  static_cast<char const*>(NULL), 'y', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};


PrivateFnDef int reportOptions(int useOuterPage) {
    CUR_WINDOW *form1Win, *form2Win;
    int i;

/*** create form windows ****/
    form1Win = CUR_newwin(14, 61, CUR_LINES-14-3, 2);
    WIN_ReverseBox(form1Win,NULL);
    CUR_wmove(form1Win,0,20);
    CUR_wattron(form1Win,CUR_A_REVERSE);
    CUR_wprintw(form1Win,"REPORT WRITER OPTIONS");
    CUR_wmove(form1Win,CUR_WIN_maxy(form1Win),2);
    CUR_wprintw(form1Win,"Save Options(F2)  Quit(F9)");
    CUR_wattroff(form1Win,CUR_A_REVERSE);
    form2Win = CUR_subwin(form1Win, 12, 59, CUR_LINES-14-2, 3);

/**** create page object ****/
    if( useOuterPage )
    {
	PAGE_createPage(MenuPage, 5, NULL, NULL, NULL, PAGE_noType, i);    
	PAGE_createElement(MenuPage, 0, NULL, SprWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(MenuPage, 1, NULL, AppWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(MenuPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
	i = 2;
    }
    else
    {
	PAGE_createPage(MenuPage, 5, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
	PAGE_createElement(MenuPage, 1, NULL, Win2, PAGE_Init, NULL, FALSE);
	PAGE_createElement(MenuPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);
	i = 3;
    }
    PAGE_fkey(MenuPage, 1) = execOptions;
    
    PAGE_createElement(MenuPage, i, NULL, form1Win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, (i+1), Form1, form2Win,
					 PAGE_Input, FORM_handler, TRUE);

/**** call page handler ****/

    PAGE_handler(MenuPage);
           
/*****  cleanup  *****/

    CUR_delwin (form2Win);
    CUR_delwin (form1Win);

    PAGE_deletePage (MenuPage, i);

    return (FALSE);
}

PrivateVarDef int	optionsExec = FALSE;

PrivateFnDef void outerReportOptions() {
	optionsExec = TRUE;
	reportOptions(TRUE);
	optionsExec = FALSE;
}

PrivateFnDef void itemsReportOptions() {
	reportOptions(FALSE);
}

PrivateFnDef void execOptions () {
    if( optionsExec )
    {
	FORM_fieldValue(ITEM)[0] = '\0';
    	exec();
    }
    PAGE_status(MenuPage) = PAGE_ExitOnExec;
}

PrivateFnDef void initOptions () {
    int i, longest, j;
    
/**** create form objects ****/
    FORM_makeForm(Form1, form1Fields, i);

    FORM_fieldMenu(SORT) = FORM_fieldMenu(ITEM);
	
    MENU::Reference menu;
    menu.setTo (new MENU (orderChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(menu) = " Sort Order: ";
    FORM_fieldMenu(ORDER).setTo (menu);

    menu.setTo (new MENU (formChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(menu) = " Format: ";
    FORM_fieldMenu(FORMAT).setTo (menu);

    menu.setTo (MENU_getMenu("companyItemGroupedByList"));
    if (menu.isntNil ()) {
	for (i = 0; i < MENU_choiceCount(menu); i++)
	    MENU_choiceHandler(menu, i) = FORM_menuToForm;
	MENU_title(menu) = " Grouping Criteria: ";
    }
    FORM_fieldMenu(GROUP1).setTo (menu);
    FORM_fieldMenu(GROUP2).setTo (menu);

    menu.setTo (new MENU (booleanChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(menu) = " Group Total? ";
    FORM_fieldMenu(SUBTOTAL).setTo (menu);

    menu.setTo (new MENU (booleanChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(menu) = " Grand Total? ";
    FORM_fieldMenu(GRAND).setTo (menu);

}

/************************************************
 *********	specialItem	*****************
 ************************************************/
#define EXPR1	    (ExprForm->field[1])
#define EXPR2	    (ExprForm->field[2])

PrivateVarDef FORM_Field exprFields[] = {
 2, 1, CUR_A_NORMAL, 11, 0, 'a', "Expression:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 2, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ", " Enter Expression", MENU::Reference(), NULL, 
 3, 13, (CUR_A_DIM | CUR_A_REVERSE), 40, 1, 'a',
 "                                        ", " Enter Expression", MENU::Reference(), NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
-1, 
};

PrivateVarDef FORM *ExprForm;

PrivateFnDef void specialItem() {
    int i;
    CUR_WINDOW *win, *swin;
    CUR_WINDOW *MenuWin;
    
    win = CUR_newwin(10, 60, 10, 0);
    WIN_ReverseBox(win,NULL);
    swin = CUR_subwin(win, 8, 58, 11, 1);
    MenuWin = CUR_newwin(11, 21, 5, 55);

    FORM_makeForm(ExprForm, exprFields, i);
    FORM_fieldValue(EXPR1)[0] = '\0';
    FORM_fieldValue(EXPR2)[0] = '\0';
    
    PAGE_createPage(MenuPage, 5, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(MenuPage, 1) = execExpr;
    
    PAGE_createElement(MenuPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 1, NULL, Win2, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 2, NULL, MenuWin,
				 PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 3, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(MenuPage, 4, ExprForm, swin,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_handler(MenuPage);

    PAGE_deletePage(MenuPage, i);
    CUR_delwin(MenuWin);
    CUR_delwin(swin);
    CUR_delwin(win);
    free(ExprForm);
}

PrivateFnDef void execExpr () {
    char buffer[100];

    if (isBlank(FORM_fieldValue(EXPR1)) &&
	isBlank(FORM_fieldValue(EXPR2)))
    {
        ERR_displayPause (" Please Enter An Expression");
	return;
    }
    
    PAGE_status(MenuPage) = PAGE_ExitOnExec;

    sprintf(buffer, "%s %s", FORM_fieldValue(EXPR1), FORM_fieldValue(EXPR2));
    strcpy(FORM_fieldValue(ITEM), buffer);
    CUR_wattron(Win1, FORM_fieldAttr(ITEM));
    CUR_wmove(Win1, FORM_fieldY(ITEM), FORM_fieldX(ITEM));
    CUR_wprintw(Win1,
    "%-*.*s", FORM_fieldLen(ITEM), FORM_fieldLen(ITEM), 
					FORM_fieldValue(ITEM));
    CUR_wattroff(Win1, FORM_fieldAttr(ITEM));
}


PrivateFnDef void screeningItems() {
    SPR_Field *fptr;
    char *ptr;
    int i;

    if (ScreenItem == NULL)
    {
        ERR_displayPause (" No Screening Items Available");
	return;
    }
    
    i = 0;
    while (ScreenItem[i] != NULL)
    {
	SPR_addField(Sprsheet, ptr);
	fptr =  SPR_field(Sprsheet, SPR_fieldCount(Sprsheet) - 1);   
	strcpy(SPR_item(fptr), ScreenItem[i]);
	strcpy(SPR_heading(fptr), ScreenItem[i]);
	strcpy(SPR_width(fptr), FORM_fieldValue(WIDTH));
	strcpy(SPR_decimals(fptr), FORM_fieldValue(DECIMAL));
	strcpy(SPR_totalType(fptr), FORM_fieldValue(TOTAL));
	printField(fptr, SPR_fieldCount(Sprsheet));
	i++;
    }

    displayFields();
}



/***************************************************
 **********	Exec Functions		************
 ***************************************************/
#define getTotalFormat()\
{\
    n = 0;\
    format = NULL;\
    while (Table[n].type != NULL)\
    {\
        if (0 == strcmp(Table[n].type, SPR_totalType(field)))\
	    format = Table[n].format;\
	n++;\
    }\
    if (format == NULL)\
        format = ":totalBlock <- NA; ";\
}

#define centerTitle(str, columns)\
{\
    n = strlen(str);\
    width = ((columns - n) / 2) + n;\
    sprintf(buffer, "%*s%*s", width, str, COLWIDTH, " ");\
}

#define centerHeading(str, columns)\
{\
    n = strlen(str);\
    width = ((columns - n) / 2) + n;\
    sprintf(buffer, "%*s", width, str);\
}

PrivateFnDef int createReport() {
    SPR_Field *field;
    char string[RS_MaxLine], buffer[200];
    char const *format;
    int n, i, width, flipped, companyCount;
    
    RS_sendOnly("FrontEndTools StandardReport named __tmpReport send: [");
    flipped = FALSE;
    if (0 == strcmp(" Flipped ", FORM_fieldValue(FORMAT)))
    {
	sprintf(string, ":transpose <- ^global TRUE;");
	flipped = TRUE;
    }
    else
	sprintf(string, ":transpose <- ^global FALSE;");
    RS_sendOnly(string);

    centerTitle(FORM_fieldValue(TITLE1), CUR_COLS);
    sprintf(string, ":title <- \"%s", buffer);
    RS_sendOnly(string);
    centerTitle(FORM_fieldValue(TITLE2), CUR_COLS);
    sprintf(string, "%s\";", buffer);
    RS_sendOnly(string);

    if (isBlank(FORM_fieldValue(SORT)))
	sprintf(string, ":sortBlock <- ^global NA;");
    else
	sprintf(string, ":sortBlock <- [ %s ];" , FORM_fieldValue(SORT));
    RS_sendOnly(string);

    if (0 == strncmp(" A", FORM_fieldValue(ORDER), 2))
	sprintf(string, ":sortDefaultDirection <- ^global TRUE;");
    else
	sprintf(string, ":sortDefaultDirection <- ^global FALSE;");
    RS_sendOnly(string);

    if (isBlank(FORM_fieldValue(GROUP1)))
	sprintf(string, ":groupBlock1 <- ^global NA;");
    else
	sprintf(string, ":groupBlock1 <- [ %s ];", FORM_fieldValue(GROUP1));
    RS_sendOnly(string);

    if (isBlank(FORM_fieldValue(GROUP2)))
	sprintf(string, ":groupBlock2 <- ^global NA;");
    else
	sprintf(string, ":groupBlock2 <- [ %s ];", FORM_fieldValue(GROUP2));
    RS_sendOnly(string);

    if (0 == strcmp(" Yes ", FORM_fieldValue(SUBTOTAL)))
	sprintf(string, ":subTotals <- ^global TRUE;");
    else
	sprintf(string, ":subTotals <- ^global FALSE;");
    RS_sendOnly(string);

    if (0 == strcmp(" Yes ", FORM_fieldValue(GRAND)))
	sprintf(string, ":grandTotals <- ^global TRUE;");
    else
	sprintf(string, ":grandTotals <- ^global FALSE;");
    RS_sendOnly(string);

/***** assign items *****/
    RS_sendOnly(":itemspecList <- ");
    for (i = 0; i < SPR_fieldCount(Sprsheet); i++)
    {
	field = SPR_field(Sprsheet, i);
        RS_sendOnly("( ^global FrontEndTools ItemSpec new do: [");
	sprintf(string, ":block <- [ %s ];", SPR_item(field));
	RS_sendOnly(string);
	if (flipped)
	    sprintf(string, ":label <- \"%-*.*s\";", 
	        COLWIDTH * 2, COLWIDTH * 2, SPR_heading(field));
	else
	{
	    centerHeading(SPR_heading(field), COLWIDTH);
	    sprintf(string, ":label <- \"%-*.*s\";", 
	        COLWIDTH, COLWIDTH, buffer);
	}
	RS_sendOnly(string);
	sprintf(string, ":width <- %s;", SPR_width(field));
	RS_sendOnly(string);
	sprintf(string, ":decimals <- %s;", SPR_decimals(field));
	RS_sendOnly(string);
	getTotalFormat();
	sprintf(string, format, SPR_item(field));
	RS_sendOnly(string);
	if (i == (SPR_fieldCount(Sprsheet) - 1))
	    RS_sendOnly("] ); ");
	else
	    RS_sendOnly("] ), ");
    }
    
    if (TRUE == RS_sendAndCheck("];", ">>>"))
    {
	ERR_displayPause(" Error assigning report format");
	return(TRUE);
    }
    if (flipped)
    {
	sprintf(string, "companyUniverse%sList count",
					 FORM_fieldValue(UNIVERSE));
	RS_sendLine(string);
	RS_getValue(string);
	companyCount = atoi(string);
	SPR_colCount(Sprsheet) = 2 + companyCount +
		((0 == strcmp(" Yes ", FORM_fieldValue(GRAND))) ? 1 : 0);
;
    }
    else
	SPR_colCount(Sprsheet) = 
        ((SPR_fieldCount(Sprsheet) + 2) > ((CUR_COLS / COLWIDTH) + 1))
	    ? (SPR_fieldCount(Sprsheet) + 2) : ((CUR_COLS / COLWIDTH) + 1);

/**** run report ****/
    sprintf(string, "FrontEndTools StandardReport named __tmpReport for: companyUniverse%sList asOf: ^today",
					 FORM_fieldValue(UNIVERSE));
    RS_sendLine(string);

    SPR_erase(Sprsheet);
    if (SPR_readSSheet(Sprsheet))
    {
	 ERR_displayPause(" Error reading spread sheet");
	 return TRUE;
    }
    return FALSE;
}

#if 0
PrivateVarDef int printReport(), downloadReport();

PrivateVarDef MENU_Choice reportChoices[] = {
 " Print ",   " Print Hardcopy Of Report",	'p', printReport, ON, 
 " Save ",    " Save Report Format",		's', NULL, ON, 
 " Download ", " Download Report To PRN File PC", 'd', downloadReport, ON, 
 static_cast<char const*>(NULL), 
};
#endif

PrivateFnDef void reportFileMenu() {
	EDIT_reportFileMenu(ReportPage,FALSE);
}

PrivateFnDef void applicFileMenu() {
	EDIT_reportFileMenu(ApplicPage,FALSE);
}

PrivateFnDef void exec() {
    if (! isBlank (FORM_fieldValue (ITEM)))
    {
        addField ();
	return;
    }

    if (SPR_fieldCount(Sprsheet) <= 0)
    {
        ERR_displayPause(" Please Enter Items To Print");
	return;
    }
    
    if (isBlank(FORM_fieldValue(UNIVERSE)))
    {
        ERR_displayPause(" No Universe Set in Parameters");
	return;
    }
    
/**** create the report ****/
    ERR_displayStr(" Writing report, please wait...",FALSE);

    if (createReport())
        return;
    ERR_clearMsg();
    didExec = TRUE;
    PAGE_ExitF8PageParent = FALSE;
    SPR_handler(Sprsheet, SprWin, PAGE_Init);
    MENU_status(AppActionMenu) = MENU_ExitOnExec;
    if( inItemsPage )
	PAGE_status(ItemsPage) = PAGE_ExitOnExec;
}

PrivateFnDef void initSprPage() {
    int i, j, longest;

    AppActionMenu.setTo (new MENU (reportChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(AppActionMenu) = " Report Writer:";
    longest += 4;
    if( (i = strlen(MENU_title(AppActionMenu))) >= longest )
	longest = i + 1;
    i = MENU_choiceCount(AppActionMenu) + 4;
    if( (longest + 55 + 4) > CUR_COLS )
	j = CUR_COLS - longest - 4;
    else
	j = 55;
    AppWin = CUR_newwin(i, longest, 5, j);

    PAGE_createPage(ApplicPage, 3, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_createElement(ApplicPage, 0, NULL, SprWin, PAGE_Init, NULL, FALSE)
    PAGE_createElement(ApplicPage, 1, AppActionMenu, AppWin, PAGE_Input, 
					    MENU_handler, TRUE);

/**** create page object ****/
    PAGE_createPage(ReportPage, 2, NULL, ApplicPage, NULL, PAGE_pageType, i);
    PAGE_fkey(ReportPage, 1) = NULL;
    PAGE_fkey(ReportPage, 4) = reportFileMenu;
    PAGE_fkey(ApplicPage, 1) = NULL;
    PAGE_fkey(ApplicPage, 4) = applicFileMenu;

    PAGE_createElement(ReportPage, 0, Sprsheet, SprWin, PAGE_Scroll, 
					    SPR_handler, TRUE);
    StatWin = CUR_newwin(1, CUR_COLS-1, CUR_LINES-2, 0);
    CUR_wattron(StatWin,CUR_A_REVERSE);
    CUR_wmove(StatWin,0,0);
    CUR_wprintw(StatWin,"%-*.*s", CUR_WIN_cols(StatWin), CUR_WIN_cols(StatWin),
    	"  Interface Menu (F5)  Application Menu (F8)  Quit (F9)");
    CUR_wattroff(StatWin,CUR_A_REVERSE);
    PAGE_createElement(ReportPage, 1, NULL, StatWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(ApplicPage, 2, NULL, StatWin, PAGE_Init, NULL, FALSE);
}



/************************************************
 **********	Misc. Functions		*********
 ***********************************************/

#if 0
PrivateFnDef void printReport()
{
    SPR_print(Sprsheet, ReportPage);
}
#endif

#if 0
PrivateFnDef void downloadReport()
{
    SPR_download(Sprsheet);
}
#endif

PrivateFnDef void initBuffer () {
    DisplayBuf = (LINEBUFFER *) malloc(sizeof(LINEBUFFER));
    BUF_maxLineSize(DisplayBuf) = CUR_COLS;
    BUF_initBuffer(DisplayBuf, (10 * BUFFERSIZE));    
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
    MENU_title(menu2) = " Items: ";

    SysWin = CUR_newwin(10, 20, 5, 5);
    
    FORM_fieldMenu(ITEM) = menu2;
    FORM_fieldMenu(SORT) = menu2;
    MENU_handler(menu2, SysWin, PAGE_Input);
    FORM_fieldMenu(ITEM) = menu1;
    FORM_fieldMenu(SORT) = menu1;
    CUR_delwin(SysWin);
}


PrivateVarDef FORM_Field getFields[] = {
 2, 8, CUR_A_NORMAL, 15, 0, 'a', "Enter Universe:", 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'a', "                  ", 
        " Enter Starting Universe, Press F1 For Menu", MENU::Reference(), NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        static_cast<char const*>(NULL), MENU::Reference(), NULL, 
-1, 
};

PrivateFnDef void initUniverse() {
	int	i;
	FORM_makeForm(ParmForm, getFields, i);
	MENU *menu = MENU_getMenu("companyUniverseListList");
	if (menu != NULL) {
	    for (i = 0; i < MENU_choiceCount(menu); i++)
		MENU_choiceHandler(menu, i) = FORM_menuToForm;
	    MENU_title(menu) = " Universe: ";
	}
	FORM_fieldMenu(UNIVERSE) = menu;
	FORM_fieldValue(UNIVERSE)[FORM_fieldLen(UNIVERSE)] = '\0';
}

PrivateVarDef int	inParamPage = FALSE;

PrivateFnDef int getUniverse(int useOuterPage) {
    int i;
    CUR_WINDOW *win, *win2;

	win = CUR_newwin(10, 50, 10, 0);
	WIN_ReverseBox(win,NULL);
	CUR_wmove(win,0,13);
	CUR_wattron(win,CUR_A_REVERSE);
	CUR_wprintw(win,"REPORT WRITER PARAMETERS");
	CUR_wattroff(win,CUR_A_REVERSE);
	win2 = CUR_subwin(win, 8, 48, 11, 1);
    if( useOuterPage )
    {
	PAGE_createPage(PPage, 5, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(PPage, 0, NULL, SprWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(PPage, 1, NULL, AppWin, PAGE_Init, NULL, FALSE);
	PAGE_createElement(PPage, 4, NULL, StatWin, PAGE_Init, NULL, FALSE);
	i = 2;
    }
    else
    {
	PAGE_createPage(PPage, 5, NULL, NULL, NULL, PAGE_noType, i);
	PAGE_createElement(PPage, 0, NULL, Win1, PAGE_Init, NULL, FALSE);
	PAGE_createElement(PPage, 1, NULL, Win2, PAGE_Init, NULL, FALSE);
	PAGE_createElement(PPage, 2, NULL, Win3, PAGE_Init, NULL, FALSE);
	i = 3;
    }    
    PAGE_createElement(PPage, i, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(PPage, (i+1), ParmForm, win2,
				 PAGE_Input, FORM_handler, TRUE);

    PAGE_fkey(PPage, 1) = execUniverse;

    inParamPage = TRUE;
    PAGE_handler(PPage);
    inParamPage = FALSE;

    CUR_delwin(win2);
    CUR_delwin(win);
    PAGE_deletePage(PPage, i);
    return(FALSE);
}

PrivateVarDef int	UnivDoExec = TRUE;

PrivateFnDef void getOuterUniverse() {
	getUniverse(TRUE);
}

PrivateFnDef void getItemsUniverse() {
	UnivDoExec = FALSE;
	getUniverse(FALSE);
	UnivDoExec = TRUE;
}

PrivateVarDef int UnivFirstTime = TRUE;

PrivateFnDef void execUniverse () {
    char buffer[80];
    
    if (isBlank(FORM_fieldValue(UNIVERSE)))
    {
        ERR_displayPause (" Please Enter a Starting Universe");
	return;
    }

   ERR_displayStr(" Validating Universe...",FALSE);
   sprintf(buffer, "companyUniverse%sList", FORM_fieldValue(UNIVERSE));

    if (RS_sendAndCheck (buffer, ">>>"))
	ERR_displayPause (" Invalid Universe");
    else
    {
    	gotInitialInput = TRUE;
	if( UnivFirstTime )
	    UnivFirstTime = FALSE;
	else if( UnivDoExec )
	{
	    FORM_fieldValue(ITEM)[0] = '\0';
	    exec();
	}
	if( inParamPage )
	    PAGE_status(PPage) = PAGE_ExitOnExec;
    }
}
