/*******************************************
 **********	statmenu.c	************
 ******************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "page.h"
#include "menu.h"
#include "form.h"
#include "choices.h"
#include "rsInterface.h"
          
PublicFnDecl int statview();

PrivateVarDef MENU_Choice menuChoices[] = {
 " View ",	    " View analysis; enter new assumptions",	'v', statview, ON, 
 " Report ",	    " Print report in hard copy",		'r', NULL, ON, 
 " Submit ", " Send private esitmate data to shared data base", 's', NULL, ON, 
 " Maintain rules ", " View, edit or create calculation rules", 'm', NULL, ON, 
 NULL, 
};

PrivateVarDef FORM_Field formFields[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ", 
	NULL, NULL, NULL, 
 3, 35, (CUR_A_UNDERLINE | CUR_A_BOLD), 10, 0, 'a', "Stat Sheet", NULL, NULL, NULL, 
 4, 35, (CUR_A_UNDERLINE | CUR_A_BOLD), 9, 0, 'a', "Main Menu",	NULL, NULL, NULL, 
 -1, 
};

PublicVarDef int	inSM = FALSE;

PublicFnDef int statmenu()
{
    MENU *menu;
    FORM *form;
    PAGE *page;
    CUR_WINDOW *menuWin, *formWin;
    int i, j, longest, rows, cols, startrow, startcol;
    static int alreadyCentered = FALSE;

    if( inSM )
    {
    	ERR_displayPause("Models already running");
    	return(0);
    }
    inSM = TRUE;
    
/*** create menu object ***/
    MENU_makeMenu(menu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	
/*** create menu window ***/
    rows = MENU_choiceCount(menu) + 4;
    cols = longest + 4;
    startrow = 4 + ((20 - rows) / 2);
    startcol = (CUR_COLS - cols) / 2;
    menuWin = CUR_newwin(rows, cols, startrow, startcol);

/**** create form object ****/
    FORM_makeForm(form, formFields, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form, CUR_COLS);
    }
    strcpy(form->field[0]->value, RS_CompanyName);

/*** create menu window ***/
    formWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_createElement(page, 0, form, formWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(page, 1, menu, menuWin, PAGE_Input, 
						 MENU_handler, TRUE);
    
/**** call page handler ****/
    PAGE_handler(page);

    MENU_deleteMenu(menu, i);
    free(form);
    CUR_delwin(formWin);
    CUR_delwin(menuWin);
    PAGE_deletePage(page, i);

    inSM = FALSE;    
    return(0);

}
