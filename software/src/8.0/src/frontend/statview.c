/*******************************************
 **********	statview.c	************
 ******************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "choices.h"

/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef int exec (
    void
);

PrivateFnDef trimString (
    char *			str
);


/*************************************************
 **********	Constants and Globals	**********
 *************************************************/

#define STANDALONE 0

PrivateVarDef FORM_Field formFields[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ",  
	NULL, NULL, NULL, 
 3, 31, (CUR_A_BOLD | CUR_A_UNDERLINE), 17, 0, 'a', "Stat Sheet - View",  
	NULL, NULL, NULL, 
 8, 24, CUR_A_NORMAL, 15, 0, 'a', "       Company:",
	NULL, NULL, NULL, 
 8, 41, (CUR_A_DIM | CUR_A_REVERSE), 6, 1, 'a', 
        "      ", " Enter Company Ticker Symbol", NULL, NULL, 
 10, 24, CUR_A_NORMAL, 15, 0, 'a', "         Model:", NULL, NULL, NULL, 
 10, 41, (CUR_A_DIM | CUR_A_REVERSE), 15,  1,  'm', "               ", 
    " Use Arrow Keys To Select Analysis Type, or F1 For Menu", NULL, NULL, 
 12, 24, CUR_A_NORMAL, 15, 0, 'a', "         Rules:", NULL, NULL, NULL, 
 12, 41, (CUR_A_DIM | CUR_A_REVERSE), 15, 1, 'm', "               ", 
    " Use Arrow Keys To Select Rules, or F1 For Menu", NULL, NULL, 
 14, 24, CUR_A_NORMAL, 15, 0, 'a', "   Assumptions:", NULL, NULL, NULL, 
 14, 41, (CUR_A_DIM | CUR_A_REVERSE), 15, 1, 'm', "               ", 
    " Use Arrow Keys To Select Assumptions, or F1 For Menu", NULL, NULL, 
 -1, 
};

PrivateVarDef MENU_Choice typeChoices[] = {
 " Earnings ",	    " Earnings Analysis",	'e', FORM_menuToForm, ON, 
 " Flow of Funds ", " Flow of Funds Analysis",	'f', FORM_menuToForm, ON, 
 " Reinvest Rate ", " Reinvestment Rate Analysis",  'r', FORM_menuToForm, ON, 
 " Dividends ",     "Dividend Analysis", 'd', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice assmptChoices[] = {
 " Right to Left ", " Calculate Right To Left",   'r', FORM_menuToForm, ON, 
 " Left to Right ", " Calculate Left To Right",   'l', FORM_menuToForm, ON, 
 " Tony's Rules  ", " Calculate Using Tony's Rules",  't', 
                                                       FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef MENU_Choice ruleChoices[] = {
 " Basic ",	    " Basic Assumptions",	    'b', FORM_menuToForm, ON, 
 " Optimistic ",    " Optimistic Assumptions",	    'o', FORM_menuToForm, ON, 
 " Pessimistic ",   " Pessimistic Assumptions",	    'p', FORM_menuToForm, ON, 
 NULL, 
};

PrivateVarDef char const *Rules;

PrivateVarDef FORM *Form;

/*---------------------------------------------------------------------------
 *	Note: the following defines determine which fields in the form
 *	      are used in the analysis, these MUST correspond to the 
 *	      correct fields
 *___________________________________________________________________________
 */

#define CompanyField	(Form->field[3]->value)
#define TypeField	(&Form->field[5]->value[1])
#define RulesField	(&Form->field[7]->value[1])
#define DataField	(&Form->field[9]->value[1])

PublicFnDef PAGE_Action statview()
{
    MENU *menu1, *menu2, *menu3;
    PAGE *page;
    CUR_WINDOW *formWin;
    int i, longest, j;
    static int alreadyCentered = FALSE;


/**** create form object ****/
    FORM_makeForm(Form, formFields, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(Form, CUR_COLS);
    }
    strcpy(Form->field[0]->value, RS_CompanyName);
    MENU_makeMenu(menu1, typeChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_makeMenu(menu2, assmptChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_makeMenu(menu3, ruleChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    Form->field[5]->menu = menu1;
    Form->field[5]->menu->title = " Model Type:";
    Form->field[7]->menu = menu2;
    Form->field[7]->menu->title = " Rules:";
    Form->field[9]->menu = menu3;
    Form->field[9]->menu->title = " Assumptions:";

/*** create form window ****/
    formWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 1, NULL, NULL, NULL, PAGE_noType, i);
    PAGE_fkey(page, 1) = exec;
    
    PAGE_createElement(page, 0, Form, formWin, PAGE_Input, FORM_handler, TRUE);

/**** call page handler ****/
    PAGE_handler(page);

/*** cleanup ***/
    free(Form);
    MENU_deleteMenu(menu1, i);
    MENU_deleteMenu(menu2, i);
    MENU_deleteMenu(menu3, i);
    CUR_delwin(formWin);
    PAGE_deletePage(page, i);
    
    return(PAGE_Input);
}

PrivateFnDef int exec (
    void
)
{
    char buffer[80],
        companyCaps[20], companyLower[20];
    int result, i, len;
    
    if (*RulesField == 'R' && *TypeField == 'E')
        Rules = "eRulesRightToLeft";
    else if (*RulesField == 'L' && *TypeField == 'E')
        Rules = "eRulesLeftToRight";
    else if (*RulesField == 'T' && *TypeField == 'E')
        Rules = "eRulesTony";

    else if (*RulesField == 'R' && *TypeField == 'F')
        Rules = "fRulesRightToLeft";
    else if (*RulesField == 'L' && *TypeField == 'F')
        Rules = "fRulesLeftToRight";
    else if (*RulesField == 'T' && *TypeField == 'F')
        Rules = "fRulesTony";

    else if (*RulesField == 'R' && *TypeField == 'R')
        Rules = "rRulesTony";
    else if (*RulesField == 'L' && *TypeField == 'R')
        Rules = "rRulesLeftToRight";
    else if (*RulesField == 'T' && *TypeField == 'R')
        Rules = "rRulesTony";
  
    else if (*RulesField == 'R' && *TypeField == 'D')
        Rules = "dRulesTony";
    else if (*RulesField == 'L' && *TypeField == 'D')
        Rules = "dRulesLeftToRight";
    else if (*RulesField == 'T' && *TypeField == 'D')
        Rules = "dRulesTony";


#if RSATTACH
    RS_dumpOutput();	/**** need to clear buffer of old stuff ****/
#endif

    trimString(CompanyField);
    trimString(DataField);

#if RSATTACH
    len = strlen(CompanyField);
    for (i = 0; i < len; i++)
        companyCaps[i] = toUpper(CompanyField[i]);
    sprintf(buffer, "Named Company %s isaCompany", companyCaps);
    if (FALSE == RS_sendAndCheck(buffer, "True"))
    {
        ERR_displayPause(" Invalid company");
	return(TRUE);
    }

    len = strlen(CompanyField);
    for (i = 0; i < len; i++)
        companyLower[i] = toLower(CompanyField[i]);
    sprintf(buffer, "%s%s", companyLower, DataField);
    result = RS_sendAndCheck(buffer, ">>>");
    if (result == TRUE)
    {
	sprintf(buffer, " %s assumptions do not exist; create new dataset", 
				    DataField);
	result = ERR_promptYesNo(buffer);
	if (result == FALSE)
	    return FALSE;
	sprintf(buffer, "!%s%s <- Named Company %s extendBy: ssDataset",
				companyLower, DataField, companyCaps);
	if (TRUE == RS_sendAndCheck(buffer, ">>>"))
	{
	    ERR_displayPause(" Invalid assumptions dataset");
	    return TRUE;
	}
	sprintf(buffer, "%s%s initializePlugs", 
				companyLower, DataField);
	if (TRUE == RS_sendAndCheck(buffer, ">>>"))
	{
	    ERR_displayPause(" Error initializing assumptions dataset");
	    return TRUE;
	}
    }
/**** create symbolic name for extended company ****/    
    sprintf(buffer, "!__tmpCompany <- %s%s extendBy: %s", 
					companyLower, DataField, Rules);
    if (TRUE == RS_sendAndCheck(buffer, ">>>"))
    {
	ERR_displayPause(" Error creating temporary company");
	return TRUE;
    }

#endif    
    statsheet(CompanyField, TypeField, Rules, DataField);    
    return FALSE;
}

PrivateFnDef trimString (
    char *			str
)
{
    char *ptr;
     
    ptr = str;
    while ((*ptr != '\0') && (*ptr == ' ')) ptr++;
    while ((*ptr != '\0') && (*ptr != ' ')) ptr++;
    *ptr = '\0';
	
}


#if STANDALONE
/**** to test standalone *****/
main ()
{
    CUR_initscr();
    PAGE_initScreen();
    statview();
    PAGE_endScreen();
    CUR_endwin();
}
#endif
