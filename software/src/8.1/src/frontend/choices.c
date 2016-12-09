
#include "Vk.h"

#include "stdcurses.h"
#include "page.h"
#include "form.h"
#include "menu.h"
#include "choices.d"

PublicFnDef void CHOICES_LimitOptions (win, form, dependencyL, Pos)
CUR_WINDOW *win;
FORM *form;
CHOICE_MenuChoice *dependencyL;
int *Pos;
/*****  Routine decides which menu choices may be selected by the user.
 *****	  It also turns on and off the field choices depending upon the number
 *****    of active menu choices.
 *
 *  Arguments:
 *	win		- pointer to a curses WINDOW.
 *	form		- pointer to a form structure.
 *	dependencyL	- an array used for activating and deactivating a
 *			  field's menu choices.
 *	Pos		- an array containing the current choice position
 *			  of each menu.
 *
 *  Returns:
 *	NOTHING - Executed for side effects on the form and on its menu.
 *
 *****/
{
    int count, 
	fieldNumber, 
	menuChoice, 
	firstTime;

    char *choiceP;
    
    CHOICE_MenuChoice
	*dArrayP = dependencyL;
	
    FORM_Field *fptr;
    
    MENU *submenu;
    
    /*****  extract the field number to access next from the structure  *****/
    while ((fieldNumber = CHOICES_Subfield (dArrayP)) != -1)
    {
	/**  extract the activating/deactivating string from the structure **/
	choiceP = CHOICES_Choices (dArrayP++);

	/*****  get the menu and the form field to modify  *****/
	fptr = FORM_field (form, fieldNumber);
	submenu = FORM_fieldMenu (fptr);
	
	firstTime = TRUE;
	count = 0;
	/*****  activate and deactivate menu choices  *****/
	for (menuChoice = 0;
	     menuChoice != MENU_choiceCount (submenu);
	     menuChoice++)
	{
	    MENU_choiceActive (submenu, menuChoice) = *choiceP;
	    if (*choiceP == ON)
	        count++;
		
	    /*****  menu's default value is the first active menu choice  ****/
	    if (*choiceP == ON && firstTime)
	    {
		firstTime = FALSE;
		/*****  write new default to screen  *****/
		Pos[fieldNumber] = menuChoice;
		strcpy(FORM_fieldValue (fptr), 
		    MENU_choiceLabel (FORM_fieldMenu (fptr), menuChoice));
		CUR_wattron(win, FORM_fieldAttr(fptr));
		CUR_wmove (win, FORM_fieldY (fptr), FORM_fieldX (fptr));
		CUR_wprintw(win,
		    "%-*s", FORM_fieldLen (fptr), FORM_fieldValue (fptr));
		CUR_wattroff(win, FORM_fieldAttr(fptr));
	    }
	    choiceP++;
	}	    

	/*****  turn form field off if only one option exists  *****/
	if (count <= 1)
	    FORM_fieldClearInput (fptr);
	else
	    FORM_fieldSetInput (fptr);

    }
}
