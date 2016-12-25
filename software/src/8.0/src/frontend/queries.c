/****************************************************************************
*****									*****
*****			queries.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "choices.h"

#include "queries.h"

PrivateFnDef void screen1 (PAGE *mpage);
PrivateFnDef void screen0 () {
    screen1 (NULL);
}

PrivateFnDef void getUniverse (PAGE *mpage);

PrivateFnDef void execUniverse ();

PrivateVarDef CUR_WINDOW *MenuWin, *FormWin;
PrivateVarDef FORM *Form;
PrivateVarDef PAGE *Page;


/*************************************************
 **********	Menu Declarations	**********
 *************************************************/

PrivateVarDef MENU_Choice menuChoices[] = {
    " Company ",
 	    " Company Screens", 'c', screen0, ON, 
    " Account ",
           " Portfolio Screens", 'p', NULL, ON, 
    " Industry ",
           " Industry Screens",  'i',  NULL, ON, 
    NULL, 
};

PrivateVarDef FORM_Field formFields[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ", 
	NULL, NULL, NULL, 
 3, 34, (CUR_A_UNDERLINE | CUR_A_BOLD), 12, 0, 'a', "Query Sheets",
	NULL, NULL, NULL, 
 4, 35, (CUR_A_UNDERLINE | CUR_A_BOLD), 9, 0, 'a', "Main Menu", 
	NULL, NULL, NULL, 
 -1, 
};



/*************************************************
 **********	Screening Choices	**********
 ************************************************/

PublicFnDecl int companyScr(char *universe, int count);

typedef struct {
  int (*function)(char*,int);
    char const *msgFormat;
} SCREENTYPE;

PrivateVarDef SCREENTYPE ScreenType[] = {
    companyScr, "companyUniverse%sList", 
    NULL, 
};

PrivateVarDef int Index;


PublicFnDef void queriesReal(PAGE *mpage) {
    MENU *menu;
    FORM *form;
    PAGE *page;
    int i, j, cols, startcol, longest;
    static int alreadyCentered = FALSE;

#if 1
/****  Assume company screens only for now...  *****/
    screen1 (mpage);
#else
/*** create menu object ***/
    MENU_makeMenu(menu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    
/*** create menu window ***/
    cols = longest + 4;
    startcol = (CUR_COLS - cols) / 2;
    MenuWin =CUR_newwin(10, cols, 8, startcol);

/**** create form object ****/
    FORM_makeForm(form, formFields, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form, CUR_COLS);
    }
    strcpy(form->field[0]->value, RS_CompanyName);

/*** create menu window ***/
    FormWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_createElement(page, 0, form, FormWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(page, 1, menu, MenuWin, PAGE_Input, 
						 MENU_handler, TRUE);
    
/**** call page handler ****/
    PAGE_handler(page);

/**** cleanup page ****/
    CUR_delwin(MenuWin);
    CUR_delwin(FormWin);
    MENU_deleteMenu(menu, i);
    free(form);
    PAGE_deletePage(page, i);
#endif
}


/******************************************************
 **********	Exec Functions		***************
 *****************************************************/

PrivateFnDef void screen1 (PAGE *mpage) {
    Index = 0;
    getUniverse(mpage);
}


/******************************************************
 ******************	getUniverse	***************
 *****************************************************/
#define UNIVERSE (Form->field[1])

PrivateVarDef FORM_Field getFields[] = {
 2, 8, CUR_A_NORMAL, 15, 0, 'a', "Enter Universe:", 
        NULL, NULL, NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'a', "                  ", 
        " Enter Starting Universe, Press F1 For Menu", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PrivateVarDef int	queriesFirstTime = TRUE;

PrivateFnDef void getUniverse (PAGE *mpage) {
    int i;
    char buffer[80];
    CUR_WINDOW *win, *win2, *tmpWin = NULL;
    MENU *menu;

    if( queriesFirstTime )
    {
    	queriesFirstTime = FALSE;
	ERR_displayStr(" Getting Universes...",FALSE);    
	FORM_makeForm(Form, getFields, i);

	sprintf(buffer, ScreenType[Index].msgFormat, "List");
	menu = MENU_getMenu(buffer);
	if (menu != NULL)
	{
	    for (i = 0; i < MENU_choiceCount(menu); i++)
		MENU_choiceHandler(menu, i) = FORM_menuToForm;
	    MENU_title(menu) = " Universe: ";
	}
	FORM_fieldMenu(UNIVERSE) = menu;
    }

    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    CUR_wmove(win,0,15);
    CUR_wattron(win,CUR_A_REVERSE);
    CUR_wprintw(win,"SCREENING PARAMETERS");
    CUR_wattroff(win,CUR_A_REVERSE);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
#if 1
    if( mpage == NULL )
    {
    	tmpWin = CUR_newwin(CUR_LINES-1,CUR_COLS,0,0);
	PAGE_createPage(Page, 3, NULL, NULL, NULL, PAGE_noType, i);
	i = 1;
    	PAGE_createElement(Page, 0, NULL, tmpWin, PAGE_Init, NULL, FALSE);
    }
    else
    {
	PAGE_createPage(Page, (PAGE_windowCount(mpage)+2), NULL, NULL, NULL, PAGE_noType, i);
    
	for( i=0 ; i< PAGE_windowCount(mpage) ; i++ )
	    PAGE_createElement(Page, i, NULL, PAGE_window(mpage,i), PAGE_Init, NULL, FALSE);
    }
    PAGE_createElement(Page, i, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(Page, (i+1), Form, win2,
				 PAGE_Input, FORM_handler, TRUE);
    PAGE_fkey(Page, 1) = execUniverse;
#else
    PAGE_createPage(Page, 4, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(Page, 1) = execUniverse;
    
    PAGE_createElement(Page, 0, NULL, FormWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(Page, 1, NULL, MenuWin, PAGE_Init, NULL, FALSE);
    PAGE_createElement(Page, 2, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(Page, 3, Form, win2,
				 PAGE_Input, FORM_handler, TRUE);
#endif
    PAGE_handler(Page);

/*    MENU_deleteMenu(menu, i);*/
    PAGE_deletePage(Page, i);
    CUR_delwin(win2);
    CUR_delwin(win);
    if( tmpWin != NULL )
    	CUR_delwin(tmpWin);
/*    free(Form);*/

}

PrivateFnDef void execUniverse () {
    char buffer[80];
    int count, len;
    
    if (isBlank(FORM_fieldValue(UNIVERSE)))
    {
        ERR_displayPause (" Please Enter a Starting Universe");
	return;
    }

    ERR_displayStr(" Executing...",FALSE);

    strcpy(buffer, "!__subset0 <- ");
    len = strlen(buffer);
    sprintf(&buffer[len], ScreenType[Index].msgFormat,
					 FORM_fieldValue(UNIVERSE));
    
    if (RS_sendAndCheck(buffer, ">>>"))
    {
	ERR_displayPause(" Invalid Universe");
	return;
    }

    strcpy(buffer, "__subset0 count");
    RS_sendLine(buffer);
    RS_getValue(buffer);
    count = atoi(buffer);

    if (count <= 0)
	ERR_displayPause (" Invalid Universe");
    else
    {
	(*ScreenType[Index].function)(FORM_fieldValue(UNIVERSE), count);
	PAGE_status(Page) = PAGE_ExitOnExec;
    }
}
