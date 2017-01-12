/****************************************************************************
*****									*****
*****			pageHandler.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "mainmenu.h"
#include "buffers.h"
#include "edit.h"
#include "choices.h"
#include "form.h"
#include "vars.h"

/**** Page Definitions ****/
#include "page.d"

/**********  Forward Declarations  **********/
PrivateFnDef int pageRefresh (PAGE *page);

PrivateFnDef void pageHelp ();

PrivateFnDef void pageWindow ();

PrivateFnDef void pageApplic ();

PrivateFnDef void pagePrev ();

PrivateFnDef void pageEdit ();

PrivateFnDef void execProfile ();

/**********  Globals  **********/
PublicVarDef int PAGE_ExitSystem = FALSE, PAGE_ToNextEltOnExec = FALSE,
		 PAGE_ExitF8PageParent = FALSE;

PrivateVarDef PAGE_Action MenuAction;

PrivateVarDef PAGE	*CurrentPage = NULL;

#define initFkeys()\
{\
    if (PAGE_help(page) == NULL)\
         PAGE_fkey(page, 0) = NULL;\
    else\
         PAGE_fkey(page, 0) = pageHelp;\
    if (enterCount <= 1)\
         PAGE_fkey(page, 2) = NULL;\
    else\
         PAGE_fkey(page, 2) = pageWindow;\
    PAGE_fkey(page, 3) = pageEdit;\
    if (PAGE_obj(page) == NULL)\
         PAGE_fkey(page, 7) = NULL;\
    else\
         PAGE_fkey(page, 7) = pageApplic;\
    PAGE_fkey(page, 8) = pagePrev;\
}

#if 0
#define initSysMenu()\
{\
    for (i = 0; i < 10; i++)\
	menuChoices[i].handler = PAGE_fkey(page, i);\
    k = 0;\
    MENU_choiceArray(sysMenu) = (MENU_Choice **)calloc(11, sizeof(MENU_Choice *));\
    for (i = 0; i < 11; i++)\
        if (menuChoices[i].handler != NULL)\
	    MENU_choice(sysMenu, k++) = &menuChoices[i];\
    MENU_choiceCount(sysMenu) = k;\
    MENU_currChoice(sysMenu) = 0;\
    MENU_normal(sysMenu) = CUR_A_NORMAL;\
    MENU_hilight(sysMenu) = CUR_A_REVERSE;\
    MENU_title(sysMenu) = " System Menu: ";\
}
#endif

#ifdef DBMENU
PrivateFnDef void pageDisplayHelp(char const *helpname) {

#define SourceDirEnv 	"VisionLibrary"

   int c = -1;
   char *SrcDirName, *getenv(), helpfile[80];
   FILE *helpfd;
   CUR_WINDOW *tmpWin;

        if( (SrcDirName = getenv(SourceDirEnv)) == NULL )
        	return;
        strcpy(helpfile, SrcDirName);
        strcat(helpfile, helpname);

	tmpWin = CUR_newwin(LINES,COLS,0,0);
	CUR_werase(tmpWin);
        if (NULL != (helpfd=fopen(helpfile, "r")))
        {
           CUR_wmove(tmpWin, 0, 0);
           while (EOF != (c=fgetc(helpfd)))
              CUR_waddch(tmpWin,c);
           fclose(helpfd);
        }
        CUR_touchwin(tmpWin);
        CUR_wrefresh(tmpWin);
        KEY_getkey(FALSE);
        KEY_QuoteNextKey = FALSE;
        CUR_delwin(tmpWin);
        return;
}
#endif


/*****		Routine to manage user interaction with windows
 *
 *  Argument:
 *	page	    -pointer to a completely filled in PAGE structure
 *
 *  Returns:
 *	nothing
 *
 *****/
PublicFnDef void PAGE_handler(PAGE *page) {
    int i, k, 
	enterCount, 
	currElement, 
	notDone;
    MENU *sysMenu;
    PAGE_Action action;
    PAGE_Status status;
    PAGE	*tmpPage;
    CUR_WINDOW	*menuwin, *helpwin, *syswin;

#if defined(__hp9000s700)
    clear();
    CUR_refresh();
#endif

/***** call window handler routines to display initial window contents ***/
    enterCount = 0;
    for (i = 0; i < page->elementCount; i++)
    {
        if (page->element[i]->enter)
	    enterCount++;
	if (page->element[i]->handler != NULL)
	{
	    (*page->element[i]->handler)(page->element[i]->object, 
					 page->element[i]->window, 
					 PAGE_Init);
	}
	else
	{
	    CUR_touchwin(page->element[i]->window);
	    CUR_wnoutrefresh(page->element[i]->window);
	}
    
    }
    
    initFkeys();
#if 0
    sysMenu = (MENU *)malloc(sizeof(MENU));
    initSysMenu();
#endif
    
/**** update the screen ****/
    if( !KEY_cready() )
	CUR_doupdate();
    
    
/**** loop through windows calling window handler routines ****/

    currElement = 0;
    notDone = TRUE;
    MenuAction = PAGE_Input;
    while (notDone)
    {
    	if( PAGE_ExitSystem )
    	    break;
	if (currElement >= page->elementCount) currElement = 0;

	if (!page->element[currElement]->enter)
	{
	    currElement++;
	    continue;
	}

	CurrentPage = page;
    
	if (MenuAction == PAGE_Input)
	{
	    action = (*page->element[currElement]->handler)
		    (page->element[currElement]->object, 
		     page->element[currElement]->window, 
	             page->element[currElement]->action);
	}
	else
	    action = MenuAction;
	    
	if( ERR_msgDisplayed && !KEY_cready() ) ERR_clearMsg();

	CurrentPage = page;

	switch (action)
	{
	case PAGE_Help:
	    if (PAGE_help(page) == NULL)
	    {
		ERR_displayPause(" No more help available");
		if( STD_delwinDoesNotRefresh )
			pageRefresh(page);
	    }
	    else
	    {
#ifndef DBMENU
		helpwin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);
		BUF_handler(PAGE_help(page), helpwin, PAGE_Scroll);
		CUR_delwin(helpwin);
#else
		pageDisplayHelp(PAGE_help(page));
#endif
		if( STD_delwinDoesNotRefresh )
			pageRefresh(page);
	    }
	    MenuAction = PAGE_Input;
	    break;

	case PAGE_Exec:
	    if (PAGE_fkey(page, 1) != NULL)
	    {
		PAGE_pageExec(page);
		if (PAGE_status(page) == PAGE_ExitOnExec)
		    notDone = FALSE;
		else if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
		    pageRefresh(page);
		if( PAGE_ToNextEltOnExec )
		{
		    MenuAction = PAGE_Window;	    
		    PAGE_ToNextEltOnExec = FALSE;
		}
	    }
	    else
		ERR_displayPause(" Execute is undefined");
	    break;

	case PAGE_Window:
	    if (enterCount <= 1)
		ERR_displayPause(" Only one window");
	    else		
		currElement++;
	    MenuAction = PAGE_Input;
	    break;

#ifndef DBMENU
	case PAGE_F4:
	    if( DefaultModule != NULL )
	    	(*DefaultModule)();
	    else
	    	EDIT_main();
	    if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
	    	pageRefresh(page);
	    MenuAction = PAGE_Input;
	    break;

	case PAGE_Editor:
	    EDIT_main();
	    if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
	    	pageRefresh(page);
	    MenuAction = PAGE_Input;
	    break;
#endif

	case PAGE_File:
	    if (PAGE_fkey(page, 4) != NULL)
	    {
		PAGE_pageFile(page);
		if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
			pageRefresh(page);	    
	    }
	    else
	    {
	    	EDIT_runInterface(page);
		if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
			pageRefresh(page);	    
	    }
#if 0
		ERR_displayPause(" F5 is undefined");
#endif
	    break;

	case PAGE_Region:
	    if (PAGE_fkey(page, 5) != NULL)
	    {
		PAGE_pageRegion(page);
		if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
			pageRefresh(page);	    
	    }
	    else
		ERR_displayPause(" F6 is undefined");
	    break;

	case PAGE_SMenu:
#if 0
            if (PAGE_fkey(page, 6) != NULL) 
	    {
               MenuAction = PAGE_Input;
	       syswin = CUR_newwin(10, 20, 5, 5);
	       MENU_handler(sysMenu, syswin, PAGE_Input);
	       CUR_delwin(syswin);
	       switch (MenuAction)
	       {
	       case PAGE_Editor:
	           break;
	       case PAGE_Profile:
                   VARS_runProfile(page);
		   if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
	   		pageRefresh(page);
		   MenuAction = PAGE_Input;
	           break;
	       case PAGE_Module:
                   
		   if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
	   		pageRefresh(page);
		   MenuAction = PAGE_Input;
	           break;
	       default:
		   MenuAction = PAGE_Input;
	           break;
	       }  /** switch **/
#endif
	       PAGE_runSysMenu(page);
	       if( STD_delwinDoesNotRefresh && !PAGE_ExitSystem )
		   pageRefresh(page);
               MenuAction = PAGE_Input;
#if 0
            } /** if **/
            else 
               ERR_displayPause(" F7 is undefined");
#endif
	    break;

	case PAGE_AMenu:
	    if (PAGE_obj(page) != NULL)
	    {
	    	switch(PAGE_objType(page))
	    	{
	    	    case PAGE_menuType:
	    	    	if( PAGE_objWin(page) != NULL )
			    MENU_handler((MENU *)PAGE_obj(page), PAGE_objWin(page), PAGE_Input);
			else
			{
			    menuwin = CUR_newwin(11, 21, 5, 55);
			    MENU_handler((MENU *)PAGE_obj(page), menuwin, PAGE_Input);
			    CUR_delwin(menuwin);
			}
			break;
		    case PAGE_formType:
		        FORM_handler((FORM *)PAGE_obj(page), PAGE_objWin(page), PAGE_Input);
		        break;
		    case PAGE_pageType:
		        tmpPage = (PAGE *)PAGE_obj(page);
			status = PAGE_status(tmpPage);
		        PAGE_handler((PAGE *)PAGE_obj(page));
		        if( PAGE_ExitF8PageParent )
		        {
		        	PAGE_ExitF8PageParent = FALSE;
		        	notDone = FALSE;
		        }
			PAGE_status(tmpPage) = status;
		        break;
		    default:
			ERR_displayPause(" No application menu");
			break;
		}
		if( STD_delwinDoesNotRefresh && notDone && !PAGE_ExitSystem )
			pageRefresh(page);
	    }
	    else
		ERR_displayPause(" No application menu");
	    MenuAction = PAGE_Input;
	    break;

	case PAGE_Prev:
	    notDone = FALSE;
	    break;

	case PAGE_Quit:
	    QuitSystem();
	    if( !PAGE_ExitSystem && STD_delwinDoesNotRefresh )
	    	pageRefresh(page);
	    break;

	case PAGE_Error:
	    ERR_displayPause(" Error in window handler routine");
	    notDone = FALSE;
	    break;

	case PAGE_Refresh:
	    if (!PAGE_ExitSystem) pageRefresh(page);
	    break;

	case PAGE_RefreshAndNext:
	    if (!PAGE_ExitSystem) pageRefresh(page);
	    MenuAction = PAGE_Window;
	    PAGE_ToNextEltOnExec = FALSE;
	    break;

	default:
	    break;    
	}
	
    }
#if 0    
    MENU_deleteMenu(sysMenu, i);
#endif
    CurrentPage = NULL;
    return;					 
}

PrivateFnDef int pageRefresh (
    PAGE *			page
)
{
    int i;

    for (i = 0; i < page->elementCount; i++)
    {
	if (page->element[i]->handler != NULL)
	{
	    (*page->element[i]->handler)(page->element[i]->object, 
					 page->element[i]->window, 
					 PAGE_Refresh);
	}
	else
	{
	    CUR_touchwin(page->element[i]->window);
	    CUR_wnoutrefresh(page->element[i]->window);
	}
    }
    
    if( !KEY_cready() )
	CUR_doupdate();
}

PrivateFnDef void pageHelp () {
    MenuAction = PAGE_Help;
}

PrivateFnDef void pageWindow () {
    MenuAction = PAGE_Window;
}

PrivateFnDef void pageApplic () {
    MenuAction = PAGE_AMenu;
}

PrivateFnDef void pagePrev () {
    MenuAction = PAGE_Prev;
}

PrivateFnDef void pageEdit () {
    MenuAction = PAGE_Editor;
}

#if 0
PrivateFnDef void pageModule() {
    MenuAction = PAGE_Module;
}
#endif

PrivateVarDef PAGE	*SysPage = NULL;

PrivateFnDef void doEdit() {
	EDIT_main();
	SysPage = NULL;
}

PrivateFnDef void doProfile() {
	VARS_runProfile(SysPage);
	SysPage = NULL;
}

PublicFnDecl void statmenu(), queries(), financeSt(), dataEntryModule(), browser();
PublicFnDecl void MAIN_getCompany(), MAIN_getUniverse(), timeSeries();

PrivateVarDef MENU_Choice menuChoices[] = {
 " Financial Statements ", " Financial Statement Analysis", 'f', financeSt, ON, 
 " Corporate Profile    ", " Summary Information",  'c',  MAIN_getCompany, ON, 
 " Models               ", " Stat Sheet and Other Modeling Applications", 'm',	statmenu, ON,
 " Screening            ", " Ad Hoc Screening   ", 's', queries, ON, 
 " Report Writer        ", " User Defined Reports", 'r',    MAIN_getUniverse, ON, 
 " Time Series Reports  ", " User Defined Reports Over Time", 't',    timeSeries, ON, 
 " Data Entry           ", " Data Entry module", 'd', dataEntryModule, ON,
 " Browser              ", " Browse Through Objects in the System", 'b', browser, ON,
 " Editor               ", " Run the System Editor", 'e', EDIT_main, ON,
 NULL, 
};

PrivateFnDef void doModule() {
	MENU		*menu;
	int		i, j, longest, rows, cols, startrow, startcol;
	CUR_WINDOW	*MenuWin;

	MENU_makeMenu(menu, menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Module Menu: ";
	rows = MENU_choiceCount(menu) + 4;
	cols = longest + 4;
	if( (size_t)cols <= strlen(MENU_title(menu)) )
		cols = strlen(MENU_title(menu)) + 1;
	startrow = 4 + ((20 - rows) / 2);
	startcol = (CUR_COLS - cols) / 2;
	MenuWin = CUR_newwin(rows, cols, startrow, startcol);
	MENU_handler(menu, MenuWin, PAGE_Input);
	CUR_delwin(MenuWin);
	MENU_deleteMenu(menu, i);
	SysPage = NULL;
}

PrivateVarDef MENU_Choice sysChoices[] = {
     " Module ",     " Execute a Module",		'm',	doModule, ON, 
     " Editor ",     " Run the System Editor",		'e',	doEdit, ON, 
     " Profile ",    " Profile Variables Editor",	'p',	doProfile, ON, 
     NULL,
};

PublicFnDef void PAGE_runSysMenu(PAGE *opage) {
	MENU	*menu;
	int	i, j, longest;
	CUR_WINDOW	*MenuWin;
	PAGE	*tpage = CurrentPage;

	CurrentPage = SysPage = opage;	
	MENU_makeMenu(menu, sysChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " System Menu: ";
	j = longest + 4;
	if( (size_t)j <= strlen(MENU_title(menu)) )
		j = strlen(MENU_title(menu)) + 1;
	MenuWin = CUR_newwin(7, j, 5, 5);
	MENU_handler(menu, MenuWin, PAGE_Input);
	CUR_delwin(MenuWin);
	MENU_deleteMenu(menu, i);
	CurrentPage = tpage;
	SysPage = NULL;
}
	
PublicVarDef int	inQueries = FALSE;

PublicFnDef void queries() {
    PAGE	*tpage = CurrentPage;
    if( inQueries )
    {
    	ERR_displayPause("Screening already running");
    	return;
    }
    inQueries = TRUE;
    queriesReal(CurrentPage);
    inQueries = FALSE;
    CurrentPage = tpage;
}



/******************************************************
 ******************	getCompany	***************
 *****************************************************/
#define COMPANY (Form->field[1]->value)

PrivateVarDef FORM_Field companyFields[] = {
 2, 4, CUR_A_NORMAL, 19, 0, 'a', "Company To Profile:", 
        NULL, NULL, NULL, 
 2, 24, (CUR_A_DIM | CUR_A_REVERSE), 18, 1, 'a', "                  ", 
        " Enter Company Ticker Symbol", NULL, NULL, 
 7, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" , 
        NULL, NULL, NULL, 
-1, 
};

PublicVarDef  int	inProfile = FALSE;
PrivateVarDef int	firstProfile = TRUE;
PrivateVarDef PAGE	*Page;
PrivateVarDef FORM	*Form;

PublicFnDef int MAIN_getCompany()
{
    PAGE	*tpage = CurrentPage;
    int i;
    CUR_WINDOW *win, *win2, *tmpWin = NULL;
    
    if( !firstProfile )
    {
    	if( inProfile )
    	{
    		ERR_displayPause("Corporate Profile already running");
    		return(0);
    	}
    	inProfile = TRUE;
    	profile(NULL);
    	inProfile = FALSE;
	CurrentPage = tpage;
    	return;
    }
    if( inProfile )
    {
    	ERR_displayPause("Corporate Profile already running");
    	return(0);
    }

    inProfile = TRUE;
    win = CUR_newwin(10, 50, 10, 0);
    WIN_ReverseBox(win,NULL);
    win2 = CUR_subwin(win, 8, 48, 11, 1);
    FORM_makeForm(Form, companyFields, i);

    if( CurrentPage == NULL )
    {
    	tmpWin = CUR_newwin(CUR_LINES-1,CUR_COLS,0,0);
	PAGE_createPage(Page, 3, NULL, NULL, NULL, PAGE_noType, i);
	i = 1;
    	PAGE_createElement(Page, 0, NULL, tmpWin, PAGE_Init, NULL, FALSE);
    }
    else
    {
	PAGE_createPage(Page, (PAGE_windowCount(CurrentPage)+2), NULL, NULL, NULL, PAGE_noType, i);

	for( i=0 ; i<PAGE_windowCount(CurrentPage) ; i++ )
	{
    	    PAGE_createElement(Page, i, NULL, PAGE_window(CurrentPage, i), PAGE_Init, NULL, FALSE);
	}
    }    
    PAGE_fkey(Page, 1) = execProfile;
    PAGE_createElement(Page, i, NULL, win, PAGE_Init, NULL, FALSE);
    PAGE_createElement(Page, (i+1), Form, win2, PAGE_Input, FORM_handler, TRUE);
    PAGE_handler(Page);
    
    PAGE_deletePage(Page, i);
    CUR_delwin(win2);
    CUR_delwin(win);
    if( tmpWin != NULL )
    	CUR_delwin(tmpWin);
    free(Form);
    inProfile = FALSE;
    CurrentPage = tpage;
    return(FALSE);

}

PrivateFnDef void execProfile () {
    char buffer[80];
    
    if (strlen (COMPANY) == 0)
    {
        ERR_displayPause (" Please Enter a Company Ticker Symbol");
	return;
    }

    ERR_displayStr(" Validating Company...",FALSE);
    strToUpper (COMPANY);
    
    sprintf (buffer, "Named Company %s", COMPANY);
    if (RS_sendAndCheck (buffer, ">>>"))
	ERR_displayPause (" Invalid Company");
    else
    {
    	firstProfile = FALSE;
	profile(COMPANY);
	PAGE_status(Page) = PAGE_ExitOnExec;
    }
}

PublicFnDef int MAIN_getUniverse()
{
    report(NULL,NULL);
    return(FALSE);
}
