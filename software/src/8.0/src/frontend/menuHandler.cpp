/****************************************************************************
*****									*****
*****			    menu.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

/**** Local Includes ****/
#include "stdcurses.h"
#include "misc.h"
#include "keys.h"
#include "page.h"
#include "attr.h"
#include "choices.h"
#include "vars.h"

#ifndef DBMENU
#include "rsInterface.h"
#endif

/**** Menu Definitions ****/
#include "menu.h"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef void menuInit (
    MENU *			menu,
    CUR_WINDOW *		menuWin
);

PrivateFnDef PAGE_Action run_menu (
    MENU *			menu,
    CUR_WINDOW *		menuWin
);

PrivateFnDef void menuShadow (
    MENU *			menu,
    CUR_WINDOW *		win
);

PrivateFnDef void DisplayOptions (
    MENU *			menu,
    CUR_WINDOW *		menuWin,
    int				direction
);

PrivateFnDef void scanModuleChoice (
    MENU *			menu,
    char *			line
);

PrivateFnDef void scanChoice (
    MENU *			menu,
    char *			line
);


/*****************************************
 **********	class MENU	**********
 *****************************************/

MENU::MENU (MENU_Choice *choices, int norm, int high, int &longest, int &i, int &j) {
    MENU_title(this) = (char *)NULL;
    MENU_currChoice(this) = 0;
    MENU_normal(this) = norm;
    MENU_hilight(this) = high;
    i = 0;
    while (choices[i].label.isntEmpty ()) i++;
    MENU_choiceArray(this) = (MENU_Choice **)calloc(i, sizeof(MENU_Choice *));
    longest = i = 0;
    while (choices[i].label.isntEmpty ()) {
        MENU_choice(this, i) = (MENU_Choice *)&choices[i];
	if ((j = strlen(choices[i].label)) > longest)
	    longest = j;
	i++;
    }
    m_longest = longest;

    MENU_status(this) = MENU_Normal;
    MENU_flags(this) = MENU_StaticMenu;
    MENU_choiceCount(this) = i;
}

MENU::~MENU () {
    if (MENU_choiceArray(this))
	::free(MENU_choiceArray(this));
}


#if 0
/****************************************************
 *************		MENU_read	*************
 ***************************************************/

PublicFnDef MENU *MENU_read(char const *filename, int isBuffer) {
    int i, j, index1, index2;
    char buffer[MENU_MaxLine], string[MENU_MaxLine], 
        *bp, *bp2;
    FILE *fptr, *fopen();
    MENU *menu;

    if ((menu = (MENU *) malloc(sizeof(MENU))) < 0)
        return(NULL);

    if (!isBuffer)
    {
	if (NULL == (fptr = fopen(filename, "r")))
	    return(NULL);
	if (NULL == (fgets(buffer, MENU_MaxLine, fptr)))
	    return(NULL);
    }
    else
    {
	j = 0;
        while (NULL != (buffer[j] = filename[j])) j++;
	filename += j;
    }
    
    sscanf(buffer, "%d %d %d %d", &menu->choiceCount, 
				     &menu->currChoice, 
				     &index1, 
				     &index2);

    MENU_normal(menu) = ATTRIBUTE[index1];
    MENU_hilight(menu) = ATTRIBUTE[index2];

    bp = strchr(buffer, MENU_Quote);
    bp++;
	
    j = 0;
    while (j < 80 && *bp != MENU_Quote)
    {
	string[j++] = *bp;
	bp++;
    }
    string[j] = '\0';
    if (NULL == (menu->title = malloc(strlen(string) + 1)))
	return(NULL);
    strcpy(menu->title, string);
				     
    
    for (i = 0; i < menu->choiceCount; i++)
    {
        if (NULL == (menu->choice[i] =
	   (MENU_Choice *)malloc(sizeof(MENU_Choice))))
	    return(NULL);

	if (!isBuffer)   
	{
	    if (NULL == (fgets(buffer, MENU_MaxLine, fptr)))
		return(NULL);
	}
	else
	{
	    j = 0;
	    while (NULL != (buffer[j] = filename[j])) j++;
	    filename += j;
	}
	

	sscanf(buffer, "%c", &menu->choice[i]->letter);

	bp = strchr(buffer, MENU_Quote);
	bp++;
	
	j = 0;
	while (j < 80 && *bp != MENU_Quote)
	{
	    string[j++] = *bp;
	    bp++;
	}
	string[j] = '\0';
	if (NULL == (menu->choice[i]->label = malloc(strlen(string) + 1)))
	    return(NULL);
	strcpy(menu->choice[i]->label, string);

	bp2 = bp + 1;
	
	bp = strchr(bp2, MENU_Quote);
	bp++;
	
	j = 0;
	while (j < 80 && *bp != MENU_Quote)
	{
	    string[j++] = *bp;
	    bp++;
	}
	string[j] = '\0';
	if (NULL == (menu->choice[i]->help = malloc(strlen(string) + 1)))
	    return(NULL);
	strcpy(menu->choice[i]->help, string);

	menu->choice[i]->handler = NULL;

    }

    if (!isBuffer) fclose(fptr);
    
    return(menu);

}
#endif


/************************************************************
 **********		MENU_handler		*************
 ************************************************************/

/*** global variables ****/
   
PrivateVarDef int *Ypos = NULL,   /** current y position of a label **/
	   Rows,
           LastChoice = -1, /** prior menu choice   **/
	   FirstLabel = -1, /** first label currently displayed **/
	   LastLabel = -1;  /** last label currently displayed  **/


/*****		Routine to manage user interaction with stack menus
 *
 *  Arguments:
 *  An array of pointers containing:
 *	menu	    -pointer to a completely filled in MENU structure
 *	menuWin	    -pointer to the CUR_CUR_WINDOW    
 *	action	    -pointer to a PAGE_Action
 *
 *  Returns:
 *	PAGE_Action
 *
 *****/
PublicFnDef PAGE_Action MENU_handler(
    MENU *menu, CUR_WINDOW *menuWin, PAGE_Action action
) {
    switch (action)
    {
    case PAGE_Init:
	menuInit(menu, menuWin);
	return(PAGE_Init);

    case PAGE_Input:
	menuInit(menu, menuWin);
        return(run_menu(menu, menuWin));

    case PAGE_Refresh:
	CUR_touchwin(menuWin);
	CUR_wnoutrefresh(menuWin);
        return(PAGE_Refresh);
          
    default:
	return(PAGE_Error);
    }

}

#define FORWARD		0
#define BACKWARD	1

PrivateFnDef void menuInit (
    MENU *			menu,
    CUR_WINDOW *		menuWin
)
{
    LastChoice = LastLabel = FirstLabel = -1;

    if( Ypos != NULL )
    	free((char *)Ypos);
    Ypos = (int *)calloc((MENU_choiceCount(menu)+1), sizeof(int));

    DisplayOptions (menu, menuWin, FORWARD);
    LastChoice = MENU_currChoice (menu);
    CUR_touchwin(menuWin);
    CUR_wnoutrefresh(menuWin);
}


PrivateFnDef PAGE_Action run_menu (
    MENU *			menu,
    CUR_WINDOW *		menuWin
)
{
    int i, c, validChoice;
    
    for (;;)
    {
	if( !KEY_cready() )
	{
	    CUR_wmove(menuWin, Ypos[menu->currChoice], 2);
	    CUR_touchwin(menuWin);
	    CUR_wnoutrefresh(menuWin);
	    CUR_doupdate();
	    CUR_forceSetCursor(menuWin, Ypos[menu->currChoice], 2);
	}
	c = KEY_getkey(FALSE);

	if (ERR_msgDisplayed && !KEY_cready()) ERR_clearMsg();

	switch(c)
	{
	    case 0:
		break;

	    case KEY_SCRIPTR:
		KEY_beginScriptRead();
		break;

	    case KEY_SCRIPTW:
		KEY_beginScriptWrite();
		break;

	    case KEY_HELP:
		ERR_clearMsg();
		return(PAGE_Help);

	    case KEY_WINDOW:
		ERR_clearMsg();
		return(PAGE_Window);

	    case KEY_EDIT:
		ERR_clearMsg();
		return(PAGE_F4);

	    case KEY_FILES:
		ERR_clearMsg();
		return(PAGE_File);

	    case KEY_SMENU:
		ERR_clearMsg();
		return(PAGE_SMenu);

	    case KEY_AMENU:
		ERR_clearMsg();
		return(PAGE_AMenu);

	    case KEY_PREV:
		ERR_clearMsg();
		return(PAGE_Prev);

	    case KEY_QUIT:
		ERR_clearMsg();
		return(PAGE_Quit);

	    case KEY_TOP:
	        MENU_currChoice(menu) = 0;
		DisplayOptions(menu, menuWin, FORWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case KEY_BOTTOM:
	        MENU_currChoice(menu) = -1;
		DisplayOptions(menu, menuWin, BACKWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case KEY_SCR_U:
	        MENU_currChoice(menu) -= (Rows - 1);
	        if( MENU_currChoice(menu) < 0 )
	            MENU_currChoice(menu) = 0;
		DisplayOptions(menu, menuWin, FORWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case KEY_SCR_D:
	        MENU_currChoice(menu) += (Rows - 1);
	        if( MENU_currChoice(menu) >= MENU_choiceCount(menu) )
	            MENU_currChoice(menu) = MENU_choiceCount(menu) - 1;
		DisplayOptions(menu, menuWin, BACKWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case CUR_KEY_LEFT:
	    case CUR_KEY_UP:
	        MENU_currChoice (menu)--;
		DisplayOptions(menu, menuWin, BACKWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case CUR_KEY_RIGHT:
	    case CUR_KEY_DOWN:
	    case KEY_CR:
	        MENU_currChoice (menu)++;
		DisplayOptions(menu, menuWin, FORWARD);
		LastChoice = MENU_currChoice (menu);
	        break;

	    case KEY_EXEC:
		if (menu->currChoice == -1)
		{
		    ERR_displayError(ERR_MenuError);
		    break;
		}
		i = menu->currChoice;
		if (menu->choice[i]->handler == NULL)
		{
		    ERR_displayError(ERR_UndefFn);
		    break;
		}
		(*menu->choice[i]->handler)();
		/*ERR_clearMsg();*/
		if( MENU_status(menu) == MENU_ExitOnExec )
		{
		    MENU_status(menu) = MENU_Normal;
		    return(PAGE_Prev);
		}
		if( PAGE_ToNextEltOnExec )
		{
		    PAGE_ToNextEltOnExec = FALSE;
		    if( STD_delwinDoesNotRefresh )
			return(PAGE_RefreshAndNext);
		    else
			return(PAGE_Window);
		}
		else
		{
		    if( STD_delwinDoesNotRefresh )
			return(PAGE_Refresh);
		    else
			return(PAGE_Input);
		}

	    case KEY_REPAINT:
		CUR_clearok(CUR_curscr);
		CUR_wrefresh(CUR_curscr);
		break;

	    case KEY_QUOTE:
		KEY_QuoteNextKey = FALSE;
		ERR_displayError(ERR_UndefKey);
		break;

	    default:
		if( (c & ~0x7f) || !isprint(c) )
		{
		    ERR_displayError(ERR_InvalidChoice);
	    	    break;
	    	}
		validChoice = FALSE;
		for (i = 0; i < menu->choiceCount; i++)
		    if( (c == menu->choice[i]->letter ||
			 c == toUpper(menu->choice[i]->letter)) &&
			(menu->choice[i]->active != OFF) )
		    {
			validChoice = TRUE;
			if (menu->choice[i]->handler == NULL)
			{
			    ERR_displayError(ERR_UndefFn);
			    break;
			}
			else
			{
			    MENU_currChoice(menu) = i;
/*			    DisplayOptions(menu, menuWin, FORWARD);
			    if( !KEY_cready() )
			    	CUR_wrefresh(menuWin);*/
			    (*menu->choice[i]->handler)();
			    /*ERR_clearMsg();*/
			    if( MENU_status(menu) == MENU_ExitOnExec )
			    {
				MENU_status(menu) = MENU_Normal;
				return(PAGE_Prev);
			    }
			    if( PAGE_ToNextEltOnExec )
			    {
				PAGE_ToNextEltOnExec = FALSE;
				if( STD_delwinDoesNotRefresh )
				    return(PAGE_RefreshAndNext);
				else
				    return(PAGE_Window);
			    }
			    else
			    {
				if( STD_delwinDoesNotRefresh )
				    return(PAGE_Refresh);
				else
				    return(PAGE_Input);
			    }
			}
		    }
		if (!validChoice)
		    ERR_displayError(ERR_InvalidChoice);
	        break;
	}
    }
    
}



PrivateFnDef void ScrollWindow(MENU *menu, CUR_WINDOW *menuWin) {
    int label, row, prev, rowContent[CUR_maxScrLines+1];  /* Max number of rows on screen */

    /*****  draw the menu border on the screen  *****/
    menuShadow(menu, menuWin);
    
    /*****  find the labels to display  *****/
    if (FirstLabel == -1)
        label = 0;
    else
	label = FirstLabel;
    while (TRUE)
    {
	while ((MENU_choiceActive(menu, label) == OFF)) label++;
        FirstLabel = label;
	Ypos[label] = 2;
	rowContent[2] = label;
	while (Ypos[label] <= Rows && label < MENU_choiceCount(menu))
	{
	    prev = label;
	    do {
	        label++;
		if (label == MENU_choiceCount(menu)) break;
	    } while (MENU_choiceActive(menu, label) == OFF);
	    Ypos[label] = Ypos[prev] + 1;
	    rowContent[Ypos[label]] = label;
	}
	LastLabel = prev;
	if (MENU_currChoice(menu) >= FirstLabel && 
	    MENU_currChoice(menu) <= LastLabel)
	    break;
	label = FirstLabel + 1;
	if (label >= MENU_choiceCount(menu))
	    label = 0;
    }

    /*****  print labels until the window fills up  *****/
    row = 2;
    while (row <= Rows)
    {
	if (rowContent[row] == MENU_choiceCount(menu))
	    break;
        CUR_wmove(menuWin, row, 2);
	CUR_wprintw(menuWin, "%-*.*s", 
	    	(CUR_WIN_cols(menuWin) - 4),
	    	(CUR_WIN_cols(menuWin) - 4),
		MENU_choiceLabel (menu, rowContent[row]).content ());
	row++;
    }
}


PrivateFnDef void menuShadow (
    MENU *			menu,
    CUR_WINDOW *		win
)
{
    int	i, numActive = 0;
    char const *info;
    
    for( i=0 ; i<MENU_choiceCount(menu) ; i++ )
    	if( MENU_choiceActive(menu,i) == ON )
    	    numActive++;
    WIN_RepaintWindow(win, MENU_normal(menu));
    if( numActive > (CUR_WIN_rows(win) - 4) )
    {
    	if( CUR_WIN_cols(win) < 5 )
    	    info = "+";
    	else if( CUR_WIN_cols(win) < 9 )
    	    info = "more";
    	else
    	    info = "more...";
    }
    else
    	info = NULL;
    WIN_HighlightBox(win, MENU_hilight(menu), menu->title, info);
    CUR_wattron(win, MENU_normal(menu));
    Rows = CUR_WIN_rows(win) - 3;
}


PrivateFnDef void DisplayOptions (
    MENU *			menu,
    CUR_WINDOW *		menuWin,
    int				direction
)
{
    int i;
    
    if (MENU_currChoice(menu) >= 0 &&
	MENU_currChoice(menu) < MENU_choiceCount(menu))
    {
        i = MENU_currChoice(menu);
	if( direction == FORWARD )
	{
	    while (i < MENU_choiceCount(menu) &&
		   MENU_choiceActive(menu, i) == OFF ) i++;
	}
	else
	{
	    while (i >= 0 && MENU_choiceActive(menu, i) == OFF ) i--;
	}
	MENU_currChoice(menu) = i;
    }
    
    if (MENU_currChoice(menu) < 0)
    {
        i = MENU_choiceCount(menu) - 1;	/**** set to last active choice ****/
	while (i > 0 && MENU_choiceActive(menu, i) == OFF) i--;
	MENU_currChoice(menu) = i;
    }
    else if (MENU_currChoice(menu) >= MENU_choiceCount(menu))
    {
        i = 0;				/*** set to first active choice ****/
	while (i < MENU_choiceCount(menu) &&
		MENU_choiceActive(menu, i) == OFF) i++;
	MENU_currChoice(menu) = i;
    }
    
    i = MENU_currChoice (menu);

    CUR_wattron(menuWin, MENU_normal(menu));

    if (FirstLabel == -1)
        ScrollWindow(menu, menuWin);
    else if (i < FirstLabel)
    {
        FirstLabel--;
	ScrollWindow (menu, menuWin);
    }
    else if (i > LastLabel)
    {
        FirstLabel++;
	ScrollWindow (menu, menuWin);
    }
    else
    {
	CUR_wmove(menuWin, Ypos[LastChoice], 2);
	CUR_wprintw(menuWin, 
		  "%-*.*s", 
		  (CUR_WIN_cols(menuWin) - 4),
		  (CUR_WIN_cols(menuWin) - 4),
		  MENU_choiceLabel (menu, LastChoice).content ());
    }
    /* CUR_wattroff(menuWin, MENU_normal(menu)); */

    /***** set MENU_currChoice field to hilight video attr *****/

    CUR_wattron(menuWin, MENU_hilight(menu));
    CUR_wmove(menuWin, Ypos[i], 2);
    CUR_wprintw(menuWin, "%-*.*s", 
		(CUR_WIN_cols(menuWin) - 4),
		(CUR_WIN_cols(menuWin) - 4),
		MENU_choiceLabel (menu, i).content ());
    if( !ERR_msgDisplayed && MENU_choiceHelp (menu, i).isntEmpty () )
    {
	CUR_werase (ERR_Window);
	CUR_mvwaddstr(ERR_Window, 0, 0, MENU_choiceHelp (menu, i).content ());
	CUR_touchwin(ERR_Window);
	CUR_wnoutrefresh(ERR_Window);
    }
    CUR_wattroff(menuWin, MENU_hilight(menu));

}


PublicFnDef CUR_WINDOW *MENU_makeWindow(
    MENU *menu, int startrow, int startcol, int maxrows
) {
    int rows, cols, longest = 0, i, j;
    
    for (i = 0; i < MENU_choiceCount(menu); i++)
    {
	if ((j = MENU_choiceLabel(menu, i).length ()) > longest)
	    longest = j;
    }
    rows = (MENU_choiceCount(menu) + 4);
    if (rows > maxrows)
        rows = maxrows;
    cols = longest + 4;
    return(CUR_newwin(rows, cols, startrow, startcol));
}

#ifndef DBMENU

PrivateVarDef int	NumChoicesAllocated = 0;

PublicFnDef MENU *MENU_getMenu(char const *message) {
    int i;
    MENU *menu;
    char buffer[RS_MaxLine+1];

    RS_sendLine(message);
    
    if ((menu = (MENU *) malloc(sizeof(MENU))) == NULL)
        return(NULL);

    MENU_title(menu) = NULL;
    MENU_currChoice(menu) = 0;
    MENU_normal(menu) = CUR_A_NORMAL;
    MENU_hilight(menu) = CUR_A_REVERSE;
    MENU_choiceCount(menu) = 0;
    MENU_status(menu) = MENU_Normal;
    MENU_SetBackendMenu(menu);
    NumChoicesAllocated = MENU_Choices;
    MENU_choiceArray(menu) = (MENU_Choice **)calloc(NumChoicesAllocated, sizeof(MENU_Choice *));

    while (RS_readLine(buffer, RS_MaxLine))
    {
	if (0 == strncmp(buffer, ">>>", 3))
	{
	    while (RS_readLine(buffer, RS_MaxLine));
	    if( MENU_choiceArray(menu) != NULL )
	    	free((void*)MENU_choiceArray(menu));
	    free(menu);
	    return(NULL);
	}
	
	if( NumChoicesAllocated <= MENU_choiceCount(menu) )
	{
	    NumChoicesAllocated += MENU_Choices;
	    MENU_choiceArray(menu) = (MENU_Choice **)realloc(MENU_choiceArray(menu), NumChoicesAllocated * sizeof(MENU_Choice *));
	}
    
	scanChoice(menu, buffer);
    }

    if (MENU_choiceCount(menu) == 0)
    {
	if( MENU_choiceArray(menu) != NULL )
	    free((void*)MENU_choiceArray(menu));
	free(menu);
	return(NULL);
    }
    return(menu);
}

PrivateFnDef MENU *getMainMenu(char const *msg) {
    int i;
    MENU *menu;
    char buffer[RS_MaxLine+1];

    RS_sendLine(msg);

    if ((menu = (MENU *) malloc(sizeof(MENU))) == NULL)
        return(NULL);

    MENU_title(menu) = NULL;
    MENU_currChoice(menu) = 0;
    MENU_normal(menu) = CUR_A_NORMAL;
    MENU_hilight(menu) = CUR_A_REVERSE;
    MENU_choiceCount(menu) = 0;
    MENU_status(menu) = MENU_Normal;
    MENU_SetBackendMenu(menu);
    NumChoicesAllocated = MENU_Choices;
    MENU_choiceArray(menu) = (MENU_Choice **)calloc(NumChoicesAllocated, sizeof(MENU_Choice *));

    while (RS_readLine(buffer, RS_MaxLine))
    {
	if (0 == strncmp(buffer, ">>>", 3))
	{
	    while (RS_readLine(buffer, RS_MaxLine));
	    if( MENU_choiceArray(menu) != NULL )
	    	free((void*)MENU_choiceArray(menu));
	    free(menu);
	    return(NULL);
	}

	if( NumChoicesAllocated <= MENU_choiceCount(menu) )
	{
	    NumChoicesAllocated += MENU_Choices;
	    MENU_choiceArray(menu) = (MENU_Choice **)realloc(MENU_choiceArray(menu), NumChoicesAllocated * sizeof(MENU_Choice *));
	}
    
        scanModuleChoice(menu, buffer);
    }

    if (MENU_choiceCount(menu) == 0)
    {
	if( MENU_choiceArray(menu) != NULL )
	    free((void*)MENU_choiceArray(menu));
	free(menu);
	return(NULL);
    }

    return(menu);
}

PublicFnDef MENU *MENU_getMainMenu()
{
    MENU *menu;
    char buffer[RS_MaxLine+1];

    if( !isBlank(StartupKit) )
    {
	sprintf(buffer,"ApplicationKit %s displayModulesInKit",StartupKit);
    	if( (menu = getMainMenu(buffer)) != NULL )
    		return(menu);
    }
    sprintf(buffer,"ApplicationKit displayMainMenu");
    return(getMainMenu(buffer));
}

#endif


PrivateFnDef void scanModuleChoice (
    MENU *			menu,
    char *			line
)
{
    char buffer[128];
    int i, j, len, func;
    
    len = strlen(line);
    if (line[len - 1] == '\n')
    {
        line[len - 1] = '\0';
	len--;
    }
    j = 0;
    while (line[j] == ' ' && j < len) j++;
    i = 0;
    while (line[j] != ',' && j < len)
	buffer[i++] = line[j++];
    if( line[j] == ',' )
    	j++;
    buffer[i] = '\0';
    if (0 == strlen(buffer))
	return;

    func = VARS_funcFromModuleName(buffer);

    while (line[j] == ' ' && j < len) j++;
    i = 0;
    while (line[j] != ',' && j < len)
	buffer[i++] = line[j++];
    if( line[j] == ',' )
    	j++;
    buffer[i] = '\0';
    if (0 == (len = strlen(buffer)))
	return;

    MENU_choice(menu, MENU_choiceCount(menu)) =
	   (MENU_Choice *)calloc(1, sizeof(MENU_Choice));

    MENU_choiceLabel(menu, MENU_choiceCount(menu)).guarantee (len + 3);
    MENU_choiceLabel(menu, MENU_choiceCount(menu)).setTo (" ");
    MENU_choiceLabel(menu, MENU_choiceCount(menu)).append (buffer);
    MENU_choiceLabel(menu, MENU_choiceCount(menu)).append (" ");
    MENU_choiceLetter(menu, MENU_choiceCount(menu)) = toLower(buffer[0]);

    while (line[j] == ' ' && line[j] != '\0') j++;
    if ((len = strlen(&line[j])) > 0)
    {
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).guarantee (len + 2);
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).setTo (" ");
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).append (&line[j]);
    }
    else
    	MENU_choiceHelp(menu, MENU_choiceCount(menu)).clear ();

    if( func != -1 )    
	MENU_choiceHandler(menu, MENU_choiceCount(menu)) = VARS_menuFunc(&NameAndFunc[func]);
    else
	MENU_choiceHandler(menu, MENU_choiceCount(menu)) = NULL;
    MENU_choiceActive(menu, MENU_choiceCount(menu)) = ON;

    MENU_choiceCount(menu)++;
}

PrivateFnDef void scanChoice (
    MENU *			menu,
    char *			line
)
{
    char buffer[80];
    int i, j, len;
    
    len = strlen(line);
    if (line[len - 1] == '\n')
    {
        line[len - 1] = '\0';
	len--;
    }
    j = 0;
    while (line[j] == ' ' && j < len) j++;
    i = 0;
    while (line[j] != ' ' && line[j] != ',' && j < len)
	buffer[i++] = line[j++];
    buffer[i] = '\0';
    if (0 == (len = strlen(buffer)))
	return;

    MENU_choice(menu, MENU_choiceCount(menu)) =
	   (MENU_Choice *)calloc(1, sizeof(MENU_Choice));

    MENU_choiceLabel(menu, MENU_choiceCount(menu)).setTo (buffer);
    MENU_choiceLetter(menu, MENU_choiceCount(menu)) = toLower(buffer[0]);

    while (line[j] == ' ' && line[j] != '\0') j++;
    if ((len = strlen(&line[j])) > 0)
    {
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).guarantee (len + 2);
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).setTo (" ");
	MENU_choiceHelp(menu, MENU_choiceCount(menu)).append (&line[j]);
    }
    else
    	MENU_choiceHelp(menu, MENU_choiceCount(menu)).clear ();
    MENU_choiceActive(menu, MENU_choiceCount(menu)) = ON;
    
    MENU_choiceCount(menu)++;
}
