/****************************************************************************
*****									*****
*****			formHandler.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

/**** Local Includes ****/
#include "stdcurses.h"
#include "keys.h"
#include "page.h"
#include "attr.h"
#include "choices.h"

/**** Form Definitions ****/
#include "form.d"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef PAGE_Action initForm (
    FORM *			form,
    CUR_WINDOW *		win
);

PrivateFnDef PAGE_Action inputForm (
    FORM *			form,
    CUR_WINDOW *		win
);


#if 0
#define readLine()\
{\
    if (!isBuffer)\
    {\
	if (NULL == (fgets(buffer, FORM_MaxLine, fptr)))\
	    return(NULL);\
    }\
    else\
    {\
	j = 0;\
	while (NULL != (buffer[j] = filename[j])) j++;\
	filename += j;\
    }\
}

PublicFnDef FORM *FORM_read(filename, isBuffer)
char *filename;
int isBuffer;
{
    int i, j, index1, index2, k, bytes, FORM_menuToForm();
    char buffer[FORM_MaxLine], string[FORM_MaxLine], 
	 *bp, *bp2;
    FILE *fptr, *fopen();
    FORM *form;
    MENU *mptr;

    if ((form = (FORM *) malloc(sizeof(FORM))) < 0)
        return(NULL);

    if (!isBuffer)
    {
	if (NULL == (fptr = fopen(filename, "r")))
	    return(NULL);
    }
    readLine();
    
    sscanf(buffer, "%d", &form->fields);
    
    for (i = 0; i < form->fields; i++)
    {
        form->field[i] = (FORM_Field *) malloc(sizeof(FORM_Field));
   
	readLine();

	sscanf(buffer, "%d %d %d %d %d %c", &form->field[i]->y, 
					 &form->field[i]->x, 
					 &index1,
					 &form->field[i]->len, 
					 &form->field[i]->input, 
					 &form->field[i]->type);
            
	form->field[i]->attr = ATTRIBUTE[index1];

	form->field[i]->value =
	    malloc((form->field[i]->len + 1) * sizeof(char));
	
	if (form->field[i]->type != 'm')
	{
	    bp = strchr(buffer, FORM_Quote);	
	    j = 0;
	    if (bp != NULL)
	    {
		bp++;
		while (j < form->field[i]->len && *bp != FORM_Quote)
		{
		    form->field[i]->value[j++] = *bp;
		    bp++;
		}
	    }
	    form->field[i]->value[j] = '\0';
	    bp++;
	}
	else
	    bp = buffer;

/**** help message ***/	
	form->field[i]->help = malloc(81 * sizeof(char));
	
	bp = strchr(bp, FORM_Quote);	
	j = 0;
	if (bp != NULL)
	{
	    bp++;
	    while (j < 80 && *bp != FORM_Quote)
	    {
		form->field[i]->help[j++] = *bp;
		bp++;
	    }
	}
	form->field[i]->help[j] = '\0';

	if (form->field[i]->type == 'm')
	{
	    form->field[i]->menu = (MENU *)malloc(sizeof(MENU));
	    mptr = form->field[i]->menu;
	    readLine();

	    sscanf(buffer, "%d %d %d %d", &mptr->choiceCount, 
					  &mptr->currChoice, 
					  &index1, 
					  &index2);

	    MENU_normal(mptr) = ATTRIBUTE[index1];
	    MENU_hilight(mptr) = ATTRIBUTE[index2];

	    bp = strchr(buffer, FORM_Quote);
	    bp++;
	
	    j = 0;
	    while (j < 80 && *bp != FORM_Quote)
	    {
		string[j++] = *bp;
		bp++;
	    }
	    string[j] = '\0';
	    if (NULL == (mptr->title = malloc(strlen(string) + 1)))
		return(NULL);
	    strcpy(mptr->title, string);
				     
    
	    for (k = 0; k < mptr->choiceCount; k++)
	    {
		if (NULL == (mptr->choice[k] =
		    (MENU_Choice *)malloc(sizeof(MENU_Choice))))
		    return(NULL);

		readLine();

		sscanf(buffer, "%c", &mptr->choice[k]->letter);

		bp = strchr(buffer, FORM_Quote);
		bp++;
	
		j = 0;
		while (j < 80 && *bp != FORM_Quote)
		{
		    string[j++] = *bp;
		    bp++;
		}
		string[j] = '\0';
		if (NULL == (mptr->choice[k]->label = malloc(strlen(string) + 1)))
		    return(NULL);
		strcpy(mptr->choice[k]->label, string);

		bp2 = bp + 1;
	
		bp = strchr(bp2, FORM_Quote);
		bp++;
	
		j = 0;
		while (j < 80 && *bp != FORM_Quote)
		{
		    string[j++] = *bp;
		    bp++;
		}
		string[j] = '\0';
		if (NULL == (mptr->choice[k]->help = malloc(strlen(string) + 1)))
		    return(NULL);
		strcpy(mptr->choice[k]->help, string);

		mptr->choice[k]->handler = FORM_menuToForm;
	    }
	} /* if menu type */
	
    }

    if (!isBuffer) fclose(fptr);
    
    return(form);

}
#endif


PublicFnDef PAGE_Action FORM_handler(form, win, action)
FORM *form;
CUR_WINDOW *win;
PAGE_Action action;
/*****		Routine to manage user interaction with forms
 *
 *  Arguments:
 *  An array of pointers containing:
 *	form	    -pointer to a completely filled in Form structure
 *	win	    -pointer to CUR_WINDOW
 *	action	    -pointer to a PAGE_Action
 *
 *  Returns:
 *	PAGE_Action
 *
 *****/
{
    switch (action)
    {
    case PAGE_Init:
        return(initForm(form, win));
          
    case PAGE_Input:
        return(inputForm(form, win));
    
    case PAGE_Refresh:
	CUR_touchwin(win);
	CUR_wnoutrefresh(win);
        return(PAGE_Refresh);
          
    default:
        return(PAGE_Error);

    }

}

PrivateFnDef PAGE_Action initForm (
    FORM *			form,
    CUR_WINDOW *		win
)
{
    int i;
    MENU *mptr;
    FORM_Field *fptr;
    
/*** construct form ***/
    if( (FORM_currField(form) < 0) ||
    	(FORM_currField(form) >= FORM_fieldCount(form)) ||
    	(FORM_fieldInput(FORM_field(form,FORM_currField(form))) == FALSE) )
	    FORM_currField(form) = -1;
    for (i = 0; i < FORM_fieldCount (form); i++)
    {
	if( (FORM_fieldType (FORM_field (form, i)) == 'm') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'M') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'S') )
	{
	    mptr = FORM_fieldMenu(FORM_field(form, i));
	    if (MENU_currChoice(mptr) < 0 || 
	        MENU_currChoice(mptr) >= MENU_choiceCount(mptr))
		MENU_currChoice(mptr) = 0;
	    strcpy(FORM_fieldValue (FORM_field (form, i)), 
			MENU_choiceLabel (mptr, MENU_currChoice(mptr)));
	}
	if( (FORM_fieldType (FORM_field (form, i)) == 'M') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'X') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'D') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'I') ||
	    (FORM_fieldType (FORM_field (form, i)) == 'A') )
		continue;
	CUR_wattron(win, FORM_fieldAttr (FORM_field (form, i)));
 	CUR_wmove(win, 
		  FORM_fieldY (FORM_field (form, i)), 
		  FORM_fieldX (FORM_field (form, i)));
	CUR_wprintw(win, 
		  "%-*.*s", 
		  FORM_fieldLen (FORM_field (form, i)), 
		  FORM_fieldLen (FORM_field (form, i)), 
		  FORM_fieldValue (FORM_field (form, i)));
	CUR_wattroff(win, FORM_fieldAttr (FORM_field (form, i)));
	if (FORM_fieldInput (FORM_field (form, i)) == TRUE
					     && FORM_currField(form) == -1)
	    FORM_currField(form) = i;
    }
    for (i = 0; i < FORM_fieldCount (form); i++)
    {
	if( (FORM_fieldType (FORM_field (form, i)) == 'X') )
	{
	    mptr = FORM_fieldMenu(FORM_field(form, i-1));
	    fptr = FORM_field(form,i+1+MENU_currChoice(mptr));
	    CUR_wattron(win, FORM_fieldAttr (fptr));
 	    CUR_wmove(win, FORM_fieldY (fptr), FORM_fieldX (fptr));
	    CUR_wprintw(win, 
		  "%-*.*s", 
		  FORM_fieldLen (fptr),
		  FORM_fieldLen (fptr),
		  FORM_fieldValue (fptr));
	    CUR_wattroff(win, FORM_fieldAttr (fptr));
	}
    }

    CUR_touchwin(win);
    CUR_wnoutrefresh(win);

    return(PAGE_Init);

}

#define hilightField(w, f, index)\
{\
    CUR_wattron(w, (CUR_A_BOLD | FORM_fieldAttr(f)));\
    CUR_wmove(w, FORM_fieldY(f), FORM_fieldX(f));\
    CUR_wprintw(w,\
	"%-*.*s", FORM_fieldLen(f), FORM_fieldLen(f), &FORM_fieldValue(f)[index]);\
    CUR_wattroff(w, CUR_A_BOLD);\
}\

#define unhilightField(w, f, index)\
{\
    CUR_wattron(w, FORM_fieldAttr(f));\
    CUR_wmove(w, FORM_fieldY(f), FORM_fieldX(f));\
    CUR_wprintw(w,\
	"%-*.*s", FORM_fieldLen(f), FORM_fieldLen(f), &FORM_fieldValue(f)[index]);\
}\

#define addChar()\
{\
    len = strlen(FORM_fieldValue(Xfptr));\
    if( (!firstKey || FORM_fieldExtKey(Xfptr)) &&\
        ((FORM_fieldScroll(Xfptr) && (len >= FORM_ValueMaxLen)) ||\
    	 (!FORM_fieldScroll(Xfptr) && (len >= FORM_fieldLen(Xfptr)))) )\
	ERR_displayStr(" Field is full", TRUE);\
    else\
    {\
    	if( firstKey )\
    	{\
    	    firstKey = FALSE;\
    	    if( !FORM_fieldExtKey(Xfptr) )\
    	    {\
    	    	for( len=0 ; len<=FORM_ValueMaxLen ; len++ )\
    	    	    FORM_fieldValue(Xfptr)[len] = '\0';\
    	    }\
    	}\
    	strcpy(tbuf,&FORM_fieldValue(Xfptr)[xpos]);\
	FORM_fieldValue(Xfptr)[xpos++] = c;\
	strcpy(&FORM_fieldValue(Xfptr)[xpos],tbuf);\
	if( (xpos - idx) > FORM_fieldLen(Xfptr) )\
		idx++;\
	hilightField(win, Xfptr, idx);\
    }\
}

#define limitChoices()\
{\
    if (FORM_fieldChoiceArray (Xfptr) != NULL)\
	CHOICES_LimitOptions (win, form,\
			      FORM_fieldChoiceArray(Xfptr)[Pos[Xfld]], Pos);\
}

#define resetForm()\
{\
    for (j = 0; j < inputFields; j++)\
	FORM_fieldSetInput (FORM_field (form, input[j]));\
    for (i = 0; i < FORM_fieldCount(form); i++)\
    {\
	fptr = FORM_field(form, i);\
	if( (FORM_fieldType(fptr) == 'm') ||\
	    (FORM_fieldType(fptr) == 'M') ||\
	    (FORM_fieldType(fptr) == 'S') )\
	{\
	    mptr = FORM_fieldMenu(fptr);\
	    MENU_currChoice(mptr) = Pos[i];\
	}\
    }\
}

PrivateVarDef int Pos[FORM_Fields];
PrivateVarDef FORM *Form;
PrivateVarDef CUR_WINDOW *Win;

PrivateFnDef PAGE_Action inputForm (
    FORM *			form,
    CUR_WINDOW *		win
)
{
    int c, i, j, fld, len, Xfld, idx,
	inputFields, firstKey, 
	xpos, isS, isX,
	notDone, notExit, 
	input[FORM_Fields];	/** array of input fields **/

    FORM_Field	*fptr, *Xfptr, *tfptr;
    MENU	*mptr;
    CUR_WINDOW	*menuWin;
    PAGE_Action action;
    char 	tbuf[FORM_ValueMaxLen+1];
           
    Form = form;
    Win = win;

/*** contruct array of input field indices ****/       

    j = inputFields = 0;
    for (i = 0; i < FORM_fieldCount(form); i++)
    {
	fptr = FORM_field(form, i);
	mptr = FORM_fieldMenu(fptr);
	if (FORM_fieldInput(fptr))
	    input[j++] = i;
	if( (FORM_fieldType(fptr) == 'm') ||
	    (FORM_fieldType(fptr) == 'M') ||
	    (FORM_fieldType(fptr) == 'S') )
	    Pos[i] = MENU_currChoice(mptr);
	else
	    Pos[i] = 0;	    /** used to remember choice for menu fields **/
    }
    
    inputFields = j;

    if (inputFields == 0)
        return(PAGE_Error);	/** there are no input fields **/

/*** get data ***/
    
    j = 0;
    while (input[j] != FORM_currField(form) && j < inputFields)
        j++;
    notExit = TRUE;
    while (notExit)
    {
	if (j == -1)		/*** go to last field ***/
	    j = inputFields - 1;
	if (j >= inputFields)	/*** go to first field ***/
	    j = 0;

	fld = FORM_currField(form) = input[j];
	fptr = FORM_field(form, fld);
	if( FORM_fieldType(fptr) == 'S' )
		isS = TRUE;
	else
		isS = FALSE;
	if( FORM_fieldType(fptr) == 'X' )
		isX = TRUE;
	else
		isX = FALSE;
	if( isX )
	{
		Xfld = fld+1+Pos[fld-1];
		Xfptr = FORM_field(form,Xfld);
	}
	else
	{
		Xfld = fld;
		Xfptr = fptr;
	}
	mptr = FORM_fieldMenu(Xfptr);
	idx = xpos = 0;
	if( isBlank(FORM_fieldValue(Xfptr)) )
	{
	    for( i=0 ; i<=FORM_ValueMaxLen ; i++ )
		FORM_fieldValue(Xfptr)[i] = '\0';
	}
	hilightField(win, Xfptr, idx);
	limitChoices();
	if( !ERR_msgDisplayed && !KEY_cready() && (FORM_fieldHelp(Xfptr) != NULL) )
	    ERR_displayStr(FORM_fieldHelp(Xfptr),FALSE);

	firstKey = TRUE;
	notDone = TRUE;

	while (notDone)
	{
	    if( !KEY_cready() )
	    {
	    	CUR_touchwin(win);
	    	CUR_wmove(win, FORM_fieldY(Xfptr), (xpos - idx) + FORM_fieldX(Xfptr));
	    	CUR_wnoutrefresh(win);
	    	CUR_doupdate();
	    	CUR_forceSetCursor(win, FORM_fieldY(Xfptr), (xpos-idx)+FORM_fieldX(Xfptr));
	    }
	         
	    c = KEY_getkey(FALSE);

	    if (ERR_msgDisplayed && !KEY_cready()) ERR_clearMsg();

	    switch(c) {

	    case 0:
		break;

	    case KEY_SCRIPTR:
		KEY_beginScriptRead();
		break;

	    case KEY_SCRIPTW:
		KEY_beginScriptWrite();
		break;

    	    case KEY_HELP:
		if (FORM_fieldMenu(Xfptr) == NULL)
		    action = PAGE_Help;
		else
		{ 
		    menuWin = CUR_newwin(10, 20, 1, (CUR_COLS - 25));
		    if (PAGE_Help ==
		    MENU_handler(FORM_fieldMenu(Xfptr), menuWin, PAGE_Input))
			action = PAGE_Help;
		    else if( STD_delwinDoesNotRefresh )
			action = PAGE_Refresh;
		    else
		    	action = PAGE_Input;
		    CUR_delwin(menuWin);
		}
		notExit = FALSE;		
		break;
		
	    case KEY_WINDOW:
		unhilightField(win, Xfptr, 0);
		CUR_touchwin(win);
		CUR_wrefresh(win);
		action = PAGE_Window;
		notExit = FALSE;
		break;

	    case KEY_EDIT:
		action = PAGE_F4;
		notExit = FALSE;
		break;

    	    case KEY_EXEC:
		action = PAGE_Exec;
		notExit = FALSE;
		break;
		
	    case KEY_QUIT:
		action = PAGE_Quit;
		notExit = FALSE;
		break;

	    case KEY_PREV:
		action = PAGE_Prev;
		notExit = FALSE;
		break;

	    case KEY_SMENU:
		action = PAGE_SMenu;
		notExit = FALSE;
		break;

	    case KEY_AMENU:
		action = PAGE_AMenu;
		notExit = FALSE;
		break;

	    case KEY_FILES:
		action = PAGE_File;
		notExit = FALSE;
		break;

	    case KEY_REGIONS:
		action = PAGE_Region;
		notExit = FALSE;
		break;

	    case KEY_BTAB:
	    case KEY_BKCH:
		j--;
		unhilightField(win, Xfptr, 0);
		while ((j >= 0) && !FORM_fieldInput (FORM_field (form, input[j])))
			j--;
		notDone = FALSE;
		break;

	    case KEY_FCH:
	    case KEY_TAB:
	    case KEY_CR:
		j++;
		unhilightField(win, Xfptr, 0);
		while ((j < inputFields) && !FORM_fieldInput (FORM_field (form, input[j])))
			j++;
		notDone = FALSE;
		break;

	    case CUR_KEY_LEFT:
	    case CUR_KEY_UP:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( (c == CUR_KEY_UP) || !FORM_fieldExtKey(Xfptr) )
		    {
			ERR_displayError(ERR_NotMenuField);
			break;
		    }
		    if( xpos == 0 )
		    	ERR_displayStr(" Beginning of field",TRUE);
		    else
		    {
		    	xpos--;
		    	if( xpos < idx )
		    	    idx = xpos;
		    }
		    hilightField(win, Xfptr, idx);
		}
		else
		{
		    Pos[Xfld]--;
		    if (Pos[Xfld] < 0)
			Pos[Xfld] = MENU_choiceCount(mptr) - 1;
		    while (MENU_choiceActive (mptr, Pos[Xfld]) == OFF)
		    {
			Pos[Xfld]--;
			if (Pos[Xfld] < 0)
			    Pos[Xfld] = MENU_choiceCount(mptr) - 1;
		    }
		    
		    strcpy(FORM_fieldValue(Xfptr), 
			MENU_choiceLabel(mptr, Pos[Xfld]));
		    if( isS )
		    {
		    	tfptr = FORM_field(form,Xfld+2+Pos[Xfld]);
		    	unhilightField(win, tfptr, 0);
		    }
		    hilightField(win, Xfptr, 0);
		    limitChoices();
		}
	        break;

	    case CUR_KEY_RIGHT:
	    case CUR_KEY_DOWN:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( (c == CUR_KEY_DOWN) || !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_NotMenuField);
			    break;
		    }
		    len = strlen(FORM_fieldValue(Xfptr));
		    if( xpos >= len )
		    	ERR_displayStr(" End of field",TRUE);
		    else
		    {
		    	xpos++;
		    	if( (xpos - idx) > FORM_fieldLen(Xfptr) )
		    	    idx++;
			hilightField(win, Xfptr, idx);
		    }
		}
		else
		{
		    Pos[Xfld]++;
		    if (Pos[Xfld] >= MENU_choiceCount(mptr))
		        Pos[Xfld] = 0;
		    while (MENU_choiceActive (mptr, Pos[Xfld]) == OFF)
		    {
			Pos[Xfld]++;
			if (Pos[Xfld] >= MENU_choiceCount(mptr))
			    Pos[Xfld] = 0;
		    }
		    strcpy(FORM_fieldValue(Xfptr), 
			    MENU_choiceLabel(mptr, Pos[Xfld]));
		    if( isS )
		    {
		    	tfptr = FORM_field(form,Xfld+2+Pos[Xfld]);
		    	unhilightField(win, tfptr, 0);
		    }
		    hilightField(win, Xfptr, 0);
		    limitChoices();
		}
	        break;

	    case KEY_BKSP:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    len = strlen(FORM_fieldValue(Xfptr));
		    if( len == 0 )
			ERR_displayStr(" Nothing to delete", TRUE);
		    else if( xpos == 0 )
		    	ERR_displayStr(" Beginning of field", TRUE);
		    else
		    {
		    	strcpy(tbuf,&FORM_fieldValue(Xfptr)[xpos]);
		    	xpos--;
		    	strcpy(&FORM_fieldValue(Xfptr)[xpos],tbuf);
		    	if( xpos < idx )
		    		idx = xpos;
			hilightField(win, Xfptr, idx);
		    }
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;

	    case KEY_DEL:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_UndefKey);
			    break;
		    }
		    len = strlen(FORM_fieldValue(Xfptr));
		    if( len == 0 )
			ERR_displayStr(" Nothing to delete", TRUE);
		    else if( xpos >= len )
		    	ERR_displayStr(" End of field", TRUE);
		    else
		    {
		    	strcpy(tbuf,&FORM_fieldValue(Xfptr)[xpos+1]);
		    	strcpy(&FORM_fieldValue(Xfptr)[xpos],tbuf);
			hilightField(win, Xfptr, idx);
		    }
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;

	    case KEY_VEOL:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_UndefKey);
			    break;
		    }
		    len = strlen(FORM_fieldValue(Xfptr));
		    xpos = len;
		    if( (xpos - idx) > FORM_fieldLen(Xfptr) )
		    {
		    	idx = xpos - FORM_fieldLen(Xfptr);
		    	if( idx < 0 )
		    		idx = 0;
		    }
		    hilightField(win, Xfptr, idx);
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;

#if 0
	    case KEY_BOL:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_UndefKey);
			    break;
		    }
		    xpos = idx = 0;
		    hilightField(win, Xfptr, idx);
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;
#endif

	    case KEY_DELEOL:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
			xpos = idx = 0;
	    	    for( i=xpos ; i<=FORM_ValueMaxLen ; i++ )
			FORM_fieldValue(Xfptr)[i] = '\0';
		    hilightField(win, Xfptr, idx);
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;
    
	    case KEY_SCR_R:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_UndefKey);
			    break;
		    }
		    len = strlen(FORM_fieldValue(Xfptr));
		    if( (len <= FORM_fieldLen(Xfptr)) || (xpos == len) )
		    {
		    	xpos = len;
		    	break;
		    }
		    xpos += (FORM_fieldLen(Xfptr) / 2);
		    idx += (FORM_fieldLen(Xfptr) / 2);
		    if( xpos > len )
		    	xpos = len;
		    if( idx >= xpos )
		    	idx = xpos - (FORM_fieldLen(Xfptr) / 2);
		    if( idx < 0 )
		    	idx = 0;
		    hilightField(win, Xfptr, idx);
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;

	    case KEY_SCR_L:
		if (FORM_fieldType(fptr) == 'a' ||
		    (FORM_fieldType(fptr) == 'X' && FORM_fieldType(Xfptr) != 'M') ||
		    FORM_fieldType(fptr) == 'n')
		{
		    if( !FORM_fieldExtKey(Xfptr) )
		    {
			    ERR_displayError(ERR_UndefKey);
			    break;
		    }
		    xpos -= (FORM_fieldLen(Xfptr) / 2);
		    idx -= (FORM_fieldLen(Xfptr) / 2);
		    if( xpos < 0 )
		    	xpos = 0;
		    if( idx < 0 )
		    	idx = 0;
		    hilightField(win, Xfptr, idx);
		}
		else
		    ERR_displayStr(" Not applicable in a menu field", TRUE);
		break;

	    case KEY_TOP:
		j = 0;
		unhilightField(win, Xfptr, 0);
		notDone = FALSE;
		break;
		
	    case KEY_BOTTOM:
		j = inputFields - 1;
		unhilightField(win, Xfptr, 0);
		notDone = FALSE;
		break;
		
	    case KEY_REPAINT:
		CUR_clearok(CUR_curscr);
		CUR_wrefresh(CUR_curscr);
		break;

#if 0
	    case 'x':
	    case 'X':
		if (FORM_fieldType(fptr) == 'l')
		{
		    MENU_choiceSelect(mptr, MENU_currChoice(mptr)) = 'X';
		    CUR_wattron(win, (CUR_A_BOLD | FORM_fieldAttr(fptr)));
		    CUR_wmove(win, FORM_fieldY(fptr), FORM_fieldX(fptr));
		    CUR_wprintw(win, "%c", 'X');
		    CUR_wattroff(win, CUR_A_BOLD);
		    /* CUR_wattroff(win, (CUR_A_BOLD | FORM_fieldAttr(fptr)));*/
		    break;
		}

	    case ' ':
		if (FORM_fieldType(fptr) == 'l')
		{
		    MENU_choiceSelect(mptr, MENU_currChoice(mptr)) = 'X';
		    CUR_wattron(win, (CUR_A_BOLD | FORM_fieldAttr(fptr)));
		    CUR_wmove(win, FORM_fieldY(fptr), FORM_fieldX(fptr));
		    CUR_wprintw(win, "%c", ' ');
		    CUR_wattroff(win, CUR_A_BOLD);
		    /* CUR_wattroff(win, (CUR_A_BOLD | FORM_fieldAttr(fptr)));*/
		    break;
		}
#endif

	    case KEY_QUOTE:
		KEY_QuoteNextKey = FALSE;
		ERR_displayError(ERR_UndefKey);
		break;

	    default:
		if ((c & ~0x7f) || !isprint(c))
		    ERR_displayError(ERR_UndefKey);
		else 
		if( (FORM_fieldType(Xfptr) == 'a') ||
		    (FORM_fieldType(Xfptr) == 'A') )
		{
		    addChar();
		}
		else 
		if( (FORM_fieldType(Xfptr) == 'n') ||
		    (FORM_fieldType(Xfptr) == 'N') )
		{
		    if (isdigit(c) ||
		       (c == '.' && (NULL == strchr(FORM_fieldValue(fptr), '.'))))
		    {
			addChar();
		    }
		    else
			ERR_displayStr(" Only digits and a decimal point allowed in a Number field", TRUE);
		}
		else
		if( (FORM_fieldType(Xfptr) == 'I') ||
		    (FORM_fieldType(Xfptr) == 'D') )
		{
		    if (isdigit(c)) 
		    {
			addChar();
		    }
		    else if( FORM_fieldType(Xfptr) == 'I' ) 
			ERR_displayStr(" Only digits allowed in an Integer field",TRUE);
		    else
			ERR_displayStr(" Only digits allowed in a Date field",TRUE);
		}
		else
		    ERR_displayError(ERR_MenuField);
		break;

	    } /* switch */
	    
	    if (notExit == FALSE) break;

	} /* while notDone  */

    } /*  while notExit  */
    
    resetForm();
    return(action);
}


PublicFnDef int FORM_menuToForm()
{
    FORM_Field *fptr, *Xfptr, *tfptr;
    MENU *mptr;
    int fld, isS, isX, Xfld;
    
    fld = FORM_currField(Form);
    fptr = FORM_field(Form, fld);
	if( FORM_fieldType(fptr) == 'S' )
		isS = TRUE;
	else
		isS = FALSE;
	if( FORM_fieldType(fptr) == 'X' )
		isX = TRUE;
	else
		isX = FALSE;
	if( isX )
	{
		Xfld = fld+1+Pos[fld-1];
		Xfptr = FORM_field(Form,Xfld);
	}
	else
	{
		Xfld = fld;
		Xfptr = fptr;
	}
    mptr = FORM_fieldMenu(Xfptr);
    strcpy(FORM_fieldValue(Xfptr),
         MENU_choiceLabel(mptr, MENU_currChoice(mptr)));
    if( isS )
    {
    	tfptr = FORM_field(Form,Xfld+2+MENU_currChoice(mptr));
    	unhilightField(Win, tfptr, 0);
    }
    hilightField(Win, Xfptr, 0);
    Pos[Xfld] = MENU_currChoice(mptr);
    if (FORM_fieldChoiceArray (Xfptr) != NULL)
	CHOICES_LimitOptions (Win, Form,
			      FORM_fieldChoiceArray(Xfptr)[Pos[Xfld]], Pos);
    return(0);
}

PublicFnDef int FORM_centerFormElts(fptr,width)
FORM	*fptr;
int	width;
{
	int	i;
	
	for( i=0 ; i<FORM_fieldCount(fptr) ; i++ )
		FORM_fieldX(FORM_field(fptr,i)) += ((width - 80) / 2);
}
