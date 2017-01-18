/*******************************************
 **********	browser.c	************
 *******************************************/

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

/*****  System  *****/
#include "Vk.h"

#include "browser.h"

/*****  Module  *****/
#include "stdcurses.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "buffers.h"
#include "edit.h"

/***************************************
 *****  Display Objects and State  *****
 ***************************************/

/*******************
 *  Display Types  *
 *******************/

/*****  Extended Menu  *****/
typedef struct {
    MENU	*baseMenu;
    int		enterable,
		autoSelect,
		lastChoice;
    PAGE_Action	pendingAction;
    char const	*instructions;
} XMENU;

#define XMENU_BaseMenu(menu)		((menu)->baseMenu)
#define XMENU_Enterable(menu)		((menu)->enterable)
#define XMENU_AutoSelect(menu)		((menu)->autoSelect)
#define XMENU_LastChoice(menu)		((menu)->lastChoice)
#define XMENU_PendingAction(menu)	((menu)->pendingAction)
#define XMENU_Instructions(menu)	((menu)->instructions)

#define XMENU_currChoice(menu)		MENU_currChoice (XMENU_BaseMenu (menu))
#define XMENU_currChoiceLabel(menu)	MENU_choiceLabel (XMENU_BaseMenu (menu), XMENU_currChoice (menu))

/*******************
 *  Display State  *
 *******************/

PrivateVarDef int		HWidth,
				QWidth,
				MHeight;
PrivateVarDef FORM		*Background,
				*HelpBanner,
				*ContextForm		= NULL,
				*AddCatalogForm		= NULL,
				*AddObjectForm		= NULL,
				*AddMessageForm		= NULL;
PrivateVarDef MENU		*NullMenu,
				*F8Menu,
				*ClassMenu		= NULL,
				*LevelMenu		= NULL,
				*CTypeMenu		= NULL,
				*ETypeMenu		= NULL;
PrivateVarDef XMENU		*ObjectCatalog,
				*ObjectInstance,
				*MessageCatalog,
				*MessageInstance;
PrivateVarDef caddr_t		LastDisplayObject	= NULL;
PrivateVarDef CUR_WINDOW	*BackgroundWin,
				*HelpBannerWin,
				*ObjectCatalogWin,
				*ObjectInstanceWin,
				*MessageCatalogWin,
				*MessageInstanceWin,
				*BufferWin,
				*F8MenuWin,
				*PopupBorderWin,
				*PopupBodyWin,
				*TargetPageWindow;
PrivateVarDef PAGE		*MasterPage,
				*PopupPage;

/***********************
 *  Execution Context  *
 ***********************/

#define LevelIndividual		LevelMenuIndividualIndex
#define LevelCollection		LevelMenuCollectionIndex

#define CTypeObject		CTypeObjectIndex
#define CTypeMessage		CTypeMessageIndex

PrivateVarDef int		Level;
PrivateVarDef char		ClassName		[RS_MaxLine],
				ObjectCatalogName	[RS_MaxLine],
				MessageCatalogName	[RS_MaxLine];
PrivateVarDef char const	*ObjectCatalogAccessMsg,
				*MessageCatalogAccessMsg;

/*****************
 *  Page Layout  *
 *****************/
 
#define BackgroundPageIndex		0
#define ObjectCatalogPageIndex		1
#define ObjectInstancePageIndex		2
#define MessageCatalogPageIndex		3
#define MessageInstancePageIndex	4
#define BufferPageIndex			5
#define HelpBannerPageIndex		6
#define PopupBorderPageIndex		7
#define PopupBodyPageIndex		8

#define MasterPageElementCount		(HelpBannerPageIndex + 1)
#define PopupPageElementCount		(PopupBodyPageIndex  + 1)

/*************************************
 *  Master Display Object Templates  *
 *************************************/

PrivateVarDef FORM_Field BackgroundFields[] = {
 -1,
};

PrivateVarDef FORM_Field HelpBannerFields[] = {
 0, 0, (CUR_A_REVERSE), 65, 0, 'a',
	" Browser:  Execute(F2) Window(F3) Interface(F5) Menu(F8) Quit(F9)",
	static_cast<char const*>(NULL), NULL, NULL, 
 -1,
};

PrivateFnDef void SelectNullMenuEntry ();
PrivateVarDef MENU_Choice NullMenuChoices[] = {
    " ", static_cast<char const*>(NULL), '\0', SelectNullMenuEntry, ON, 
    static_cast<char const*>(NULL),
};

/************************************
 *  Popup Display Object Templates  *
 ************************************/

/*****  'F8' Menu  *****/
PrivateFnDef void DetermineContext ();
PrivateFnDef void AddCatalog ();
PrivateFnDef void AddToCatalog ();
PrivateVarDef MENU_Choice F8MenuChoices[] = {
    " Change Context"	, "Change browser class or level"	  , 'c', DetermineContext, ON,
    " Add to Catalog"	, "Add an object or message to a catalog" , 'a', AddToCatalog	 , ON,
    " New Catalog"      , "Create a new object or message catalog", 'n', AddCatalog      , ON,
    static_cast<char const*>(NULL),
};

/*****  Level Menu  *****/
PrivateVarDef MENU_Choice LevelMenuChoices[] = {
    "Individual", "Operate on individual class instances"	, 'i', FORM_menuToForm, ON,
    "Collection", "Operate on collections of class instances"	, 'c', FORM_menuToForm, ON,
    static_cast<char const*>(NULL),
};

#define LevelMenuIndividualIndex	0
#define LevelMenuCollectionIndex	1

/*****  Catalog Type Menu  *****/
PrivateVarDef MENU_Choice CTypeMenuChoices[] = {
    "Object" , "Operate on an object catalog"	, 'o', FORM_menuToForm, ON,
    "Message", "Operate on a message catalog"	, 'm', FORM_menuToForm, ON,
    static_cast<char const*>(NULL),
};

#define CTypeObjectIndex		0
#define CTypeMessageIndex		1

/*****  Object Evaluation Type Menu  *****/
PrivateVarDef MENU_Choice ETypeMenuChoices[] = {
    "Now"	, "Evaluate the object definition once now"	, 'n', FORM_menuToForm, ON,
    "Later"	, "Evaluate the object definition at each use"	, 'l', FORM_menuToForm, ON,
    static_cast<char const*>(NULL),
};

#define ETypeNowIndex			0
#define ETypeLaterIndex			1

/*****  Context Form  *****/
PrivateVarDef FORM_Field ContextFormFields[] = {
 1,  4, CUR_A_NORMAL, 6, 0, 'a', "Class:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 11, (CUR_A_DIM | CUR_A_REVERSE), 16, FORM_InputFlag, 'm', "",
	" Use Arrow Keys To Select Class, or F1 For Menu", NULL, NULL,
 3,  4, CUR_A_NORMAL, 6, 0, 'a', "Level:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 11, (CUR_A_DIM | CUR_A_REVERSE), 16, FORM_InputFlag, 'm', "",
	" Use Arrow Keys To Select Level, or F1 For Menu", NULL, NULL,
 -1,
};

#define ContextFormClassField	(ContextForm->field[1])
#define ContextFormLevelField	(ContextForm->field[3])

/*****  Add Catalog Form  *****/
PrivateVarDef FORM_Field AddCatalogFormFields[] = {
 1,  4, CUR_A_NORMAL, 6, 0, 'a', "Class:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 11, (CUR_A_DIM | CUR_A_REVERSE), 16, FORM_InputFlag, 'm', "",
	" Use Arrow Keys To Select Class, or F1 For Menu", NULL, NULL,
 1, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Type:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 42, (CUR_A_DIM | CUR_A_REVERSE), 16, FORM_InputFlag, 'm', "",
	" Use Arrow Keys To Select Catalog Type, or F1 For Menu", NULL, NULL,
 3,  4, CUR_A_NORMAL, 6, 0, 'a', "Level:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 11, (CUR_A_DIM | CUR_A_REVERSE), 16, FORM_InputFlag, 'm', "",
	" Use Arrow Keys To Select Level, or F1 For Menu", NULL, NULL,
 3, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Name:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 42, (CUR_A_DIM | CUR_A_REVERSE), 16, (FORM_InputFlag | FORM_ScrollFlag), 'a', "",
	" Enter the name of the catalog to be added, or F1 For Menu", NULL, NULL,
 -1,
};

#define AddCatalogFormClassField	(AddCatalogForm->field[1])
#define AddCatalogFormCTypeField	(AddCatalogForm->field[3])
#define AddCatalogFormLevelField	(AddCatalogForm->field[5])
#define AddCatalogFormCNameField	(AddCatalogForm->field[7])

/*****  Add Object Form  *****/
PrivateVarDef FORM_Field AddObjectFormFields[] = {
 1,  4, CUR_A_NORMAL, 6, 0, 'a', "Class:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 11, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 1, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Type:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 42, CUR_A_DIM, 16, 0, 'a', "Object",
	static_cast<char const*>(NULL), NULL, NULL,
 3,  4, CUR_A_NORMAL, 6, 0, 'a', "Level:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 11, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 3, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Name:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 42, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 5,  4, CUR_A_NORMAL,  6, 0, 'a', " Name:",
        static_cast<char const*>(NULL), NULL, NULL,
 5, 11, (CUR_A_DIM | CUR_A_REVERSE), 20, (FORM_InputFlag | FORM_ScrollFlag), 'a', "",
	" Enter a name for the object being cataloged", NULL, NULL,
 7,  3, CUR_A_NORMAL,  7, 0, 'a', "Object:",
        static_cast<char const*>(NULL), NULL, NULL,
 7, 11, (CUR_A_DIM | CUR_A_REVERSE), 50, (FORM_InputFlag | FORM_ScrollFlag), 'a', "",
	" Enter an expression defining the object being cataloged", NULL, NULL,
 9,  1, CUR_A_NORMAL,  9, 0, 'a', "Evaluate:",
        static_cast<char const*>(NULL), NULL, NULL,
 9, 11, (CUR_A_DIM | CUR_A_REVERSE), 6, FORM_InputFlag, 'm', "",
	" Use arrow keys to select an evaluation type, or F1 for menu", NULL, NULL,
 -1,
};

#define AddObjectFormClassField		(AddObjectForm->field[1])
#define AddObjectFormLevelField		(AddObjectForm->field[5])
#define AddObjectFormCNameField		(AddObjectForm->field[7])
#define AddObjectFormONameField		(AddObjectForm->field[9])
#define AddObjectFormODefnField		(AddObjectForm->field[11])
#define AddObjectFormETypeField		(AddObjectForm->field[13])

/*****  Add Message Form  *****/
PrivateVarDef FORM_Field AddMessageFormFields[] = {
 1,  4, CUR_A_NORMAL, 6, 0, 'a', "Class:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 11, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 1, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Type:",
        static_cast<char const*>(NULL), NULL, NULL,
 1, 42, CUR_A_DIM, 16, 0, 'a', "Message",
	static_cast<char const*>(NULL), NULL, NULL,
 3,  4, CUR_A_NORMAL, 6, 0, 'a', "Level:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 11, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 3, 28, CUR_A_NORMAL, 13, 0, 'a', "Catalog Name:",
        static_cast<char const*>(NULL), NULL, NULL,
 3, 42, CUR_A_DIM, 16, 0, 'm', "",
	static_cast<char const*>(NULL), NULL, NULL,
 5,  4, CUR_A_NORMAL,  6, 0, 'a', " Name:",
        static_cast<char const*>(NULL), NULL, NULL,
 5, 11, (CUR_A_DIM | CUR_A_REVERSE), 20, (FORM_InputFlag | FORM_ScrollFlag), 'a', "",
	" Enter the name of the message being cataloged", NULL, NULL,
 -1,
};

#define AddMessageFormClassField	(AddMessageForm->field[1])
#define AddMessageFormLevelField	(AddMessageForm->field[5])
#define AddMessageFormCNameField	(AddMessageForm->field[7])
#define AddMessageFormMNameField	(AddMessageForm->field[9])

/*************************************
 *****  Miscellaneous Utilities  *****
 *************************************/

PrivateFnDef char *VFormatString (char const *fmt, va_list ap) {
    static char buffer[RS_MaxLine+1];

    vsprintf (buffer, fmt, ap);
    return buffer;
}

PrivateFnDef char *FormatString (char const *fmt, ...) {
    va_list ap;
    char *result;

    va_start (ap, fmt);
    result = VFormatString (fmt, ap);
    va_end (ap);

    return result;
}

PrivateFnDef int SelectorTakesParameters (char const *selector) {
    return
	strpbrk (selector, ":+-*/&|,<>=") != NULL &&
	(
	    strpbrk (selector, ":"  ) != NULL ||
	    strcmp  (selector, "<-" ) == 0    ||
	    strcmp  (selector, "->" ) == 0    ||
	    strcmp  (selector, ","  ) == 0    ||
	    strcmp  (selector, "<"  ) == 0    ||
	    strcmp  (selector, "<=" ) == 0    ||
	    strcmp  (selector, ">=" ) == 0    ||
	    strcmp  (selector, ">"  ) == 0    ||
	    strcmp  (selector, "="  ) == 0    ||
	    strcmp  (selector, "==" ) == 0    ||
	    strcmp  (selector, "!=" ) == 0    ||
	    strcmp  (selector, "!==") == 0    ||
	    strcmp  (selector, "||" ) == 0    ||
	    strcmp  (selector, "&&" ) == 0    ||
	    strcmp  (selector, "+"  ) == 0    ||
	    strcmp  (selector, "-"  ) == 0    ||
	    strcmp  (selector, "*"  ) == 0    ||
	    strcmp  (selector, "/"  ) == 0
	);
}

PrivateFnDef char *StringAsValidSelector (char *buffer, char const *string) {
    char *result = buffer;

    if (*string && !isalpha (*string))
	*buffer++ = '\\';

    while (*buffer++ = *string++)
    {
	if (*string && !isalnum (*string))
	    *buffer++ = '\\';
    }

    return result;
}

/****************************************
 *****  Extended Menu Type Manager  *****
 ****************************************/

PrivateFnDef XMENU *XMENU_create (MENU *menu, char const *instructions) {
    XMENU *xmenu;

    if ((xmenu = (XMENU *) malloc (sizeof (XMENU))) == NULL)
	return NULL;

    XMENU_BaseMenu     (xmenu) = menu;
    XMENU_Enterable    (xmenu) = FALSE;
    XMENU_AutoSelect   (xmenu) = FALSE;
    XMENU_LastChoice   (xmenu) = -1;
    XMENU_Instructions (xmenu) = instructions;

    return xmenu;
}

PrivateFnDef PAGE_Action XMENU_handler (
    XMENU *xmenu, CUR_WINDOW *window, PAGE_Action action
) {
    int instructionsDisplayed = FALSE;
    if (PAGE_Input == action)
    {
	if (!XMENU_Enterable (xmenu))
	    return PAGE_Window;
	if (XMENU_Instructions (xmenu) != NULL)
	{
	    ERR_displayStr (XMENU_Instructions (xmenu), FALSE);
	    instructionsDisplayed = TRUE;
	}
    }

    XMENU_PendingAction (xmenu) = MENU_handler (XMENU_BaseMenu (xmenu), window, action);
    if (instructionsDisplayed)
	ERR_clearMsg ();

    if (XMENU_AutoSelect (xmenu) &&
	(PAGE_Window == XMENU_PendingAction (xmenu) || PAGE_RefreshAndNext == XMENU_PendingAction (xmenu)) &&
	XMENU_currChoice (xmenu) != XMENU_LastChoice (xmenu))
    {
	XMENU_LastChoice (xmenu) = XMENU_currChoice (xmenu);
	(*MENU_choiceHandler (XMENU_BaseMenu (xmenu), XMENU_LastChoice (xmenu)))();
    }

    return XMENU_PendingAction (xmenu);
}

PrivateFnDef void XMENU_ChangeMenu (
    CUR_WINDOW *window, XMENU *xmenu, MENU *newMenu, int enterable, int autoSelect
) {
    MENU *oldMenu;
    int i;

    if ((oldMenu = XMENU_BaseMenu (xmenu)) != newMenu)
    {
	if (oldMenu != NullMenu) MENU_deleteMenu (oldMenu, i);
	XMENU_BaseMenu   (xmenu) = newMenu;
	XMENU_LastChoice (xmenu) = -1;
	XMENU_handler    (xmenu, window, PAGE_Init);
    }

    XMENU_Enterable  (xmenu) = enterable;
    XMENU_AutoSelect (xmenu) = autoSelect;
}

/***************************
 *****  Page Managers  *****
 ***************************/

/*************
 *  General  *
 *************/

PrivateFnDef void ExecuteFileMenu () {
    EDIT_reportFileMenu(MasterPage,TRUE);
}

PrivateFnDef PAGE_Action FormPageHandler (
    FORM *form, CUR_WINDOW *window, PAGE_Action action
) {
    if (TargetPageWindow && PAGE_Input == action)
    {
	if (TargetPageWindow != window)
	    return PAGE_Window;
	TargetPageWindow = NULL;
    }

    LastDisplayObject = (caddr_t)form;
    return FORM_handler (form, window, action);
}

PrivateFnDef PAGE_Action XMenuPageHandler (
    XMENU *xmenu, CUR_WINDOW *window, PAGE_Action action
) {
    if (TargetPageWindow && PAGE_Input == action)
    {
	if (TargetPageWindow != window)
	    return PAGE_Window;
	TargetPageWindow = NULL;
    }

    LastDisplayObject = (caddr_t)xmenu;
    return XMENU_handler (xmenu, window, action);
}

PrivateFnDef PAGE_Action BufPageHandler (
    LINEBUFFER *buffer, CUR_WINDOW *window, PAGE_Action action
) {
    if (TargetPageWindow && (PAGE_Input == action || PAGE_Scroll == action) )
    {
	if (TargetPageWindow != window)
	    return PAGE_Window;
	TargetPageWindow = NULL;
    }

    LastDisplayObject = (caddr_t)buffer;
    return BUF_handler (buffer, window, action);
}

/*****************
 *  Master Page  *
 *****************/

PrivateFnDef void SelectInstance ();

PrivateFnDef void
    CreateMasterPage ()
{
    int i;

/*****  Create windows, ...  ****/
    HWidth = (CUR_COLS - 1) / 2;
    QWidth = HWidth / 2;
    MHeight = CUR_LINES / 3;
    MHeight = MHeight < 8 ? 8 : MHeight > 12 ? 12 : MHeight;

    BackgroundWin = CUR_newwin(CUR_LINES - 2, CUR_COLS, 0, 0);
    HelpBannerWin = CUR_newwin(2, CUR_COLS, CUR_LINES - 2, 0);

    ObjectCatalogWin  = CUR_newwin(MHeight, QWidth             , 0, 0);
    ObjectInstanceWin = CUR_newwin(MHeight, HWidth - QWidth + 1, 0, QWidth - 1);
    MessageCatalogWin = CUR_newwin(MHeight, QWidth             , 0, HWidth);
    MessageInstanceWin= CUR_newwin(MHeight, HWidth - QWidth + 1, 0, HWidth + QWidth - 1);

    BufferWin	    = CUR_newwin(CUR_LINES - 2 - MHeight, CUR_COLS-1, MHeight, 0);
/*
    if( STD_hasInsertDeleteLine )
	CUR_idlok(BufferWin,TRUE);
*/

    F8MenuWin	= CUR_newwin(7, 20, MHeight + 2, HWidth + QWidth / 2);
	
/*****  ... and the page object.  *****/
    PAGE_createPage (MasterPage, MasterPageElementCount, NULL, F8Menu, F8MenuWin, PAGE_menuType, i);

    PAGE_createElement(MasterPage, BackgroundPageIndex	   , Background     , BackgroundWin	, PAGE_Input , FormPageHandler , FALSE);
    PAGE_createElement(MasterPage, ObjectCatalogPageIndex  , ObjectCatalog  , ObjectCatalogWin  , PAGE_Input , XMenuPageHandler, TRUE);
    PAGE_createElement(MasterPage, ObjectInstancePageIndex , ObjectInstance , ObjectInstanceWin , PAGE_Input , XMenuPageHandler, TRUE);
    PAGE_createElement(MasterPage, MessageCatalogPageIndex , MessageCatalog , MessageCatalogWin , PAGE_Input , XMenuPageHandler, TRUE);
    PAGE_createElement(MasterPage, MessageInstancePageIndex, MessageInstance, MessageInstanceWin, PAGE_Input , XMenuPageHandler, TRUE);
    PAGE_createElement(MasterPage, BufferPageIndex	   , BrowserBuf     , BufferWin		, PAGE_Scroll, BufPageHandler  , TRUE);
    PAGE_createElement(MasterPage, HelpBannerPageIndex	   , HelpBanner     , HelpBannerWin	, PAGE_Input , FormPageHandler , FALSE);

/*
    PAGE_fkey(MasterPage, 1) = SelectInstance;
*/
    PAGE_fkey(MasterPage, 4) = ExecuteFileMenu;
}

PrivateFnDef void
    DeleteMasterPage ()
{
    int i;

    CUR_delwin (BackgroundWin);
    CUR_delwin (ObjectCatalogWin);
    CUR_delwin (ObjectInstanceWin);
    CUR_delwin (MessageCatalogWin);
    CUR_delwin (MessageInstanceWin);
    CUR_delwin (BufferWin);
    CUR_delwin (HelpBannerWin);
    CUR_delwin (F8MenuWin);

    PAGE_deletePage (MasterPage, i)
}

PrivateFnDef void
    ProcessMasterPage ()
{
    PAGE_handler (MasterPage);
}

/****************
 *  Popup Page  *
 ****************/

PrivateFnDef void CreatePopupPage (
    char const*			title,
    void			(*execfn)(),
    PAGE_Action			(*handler)(FORM*,CUR_WINDOW*,PAGE_Action),
    FORM *			object,
    int				ysize,
    int				xsize
)
{
    int i;

    i = MHeight+3 < CUR_LINES-5 - ysize ? MHeight+3 : CUR_LINES-5-ysize;
    PopupBorderWin = CUR_newwin(ysize + 2, xsize + 2, i, 2);

    WIN_ReverseBox(PopupBorderWin,NULL);
    CUR_wmove(PopupBorderWin,0,2);
    CUR_wattron(PopupBorderWin,CUR_A_REVERSE);
    CUR_wprintw(PopupBorderWin,title);
    CUR_wmove(PopupBorderWin,CUR_WIN_maxy(PopupBorderWin),2);
    CUR_wprintw(PopupBorderWin,"Execute(F2)  Quit(F9)");
    CUR_wattroff(PopupBorderWin,CUR_A_REVERSE);

    PopupBodyWin = CUR_subwin(PopupBorderWin, ysize, xsize, i + 1, 3);

    PAGE_createPage (PopupPage, PopupPageElementCount, NULL, NULL, NULL, PAGE_noType, i);

    PAGE_createElement (PopupPage, BackgroundPageIndex	   , NULL  , BackgroundWin     , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, ObjectCatalogPageIndex  , NULL  , ObjectCatalogWin  , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, ObjectInstancePageIndex , NULL  , ObjectInstanceWin , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, MessageCatalogPageIndex , NULL  , MessageCatalogWin , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, MessageInstancePageIndex, NULL  , MessageInstanceWin, PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, BufferPageIndex	   , NULL  , BufferWin	       , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, HelpBannerPageIndex     , NULL  , HelpBannerWin     , PAGE_Init , NULL   , FALSE);

    PAGE_createElement (PopupPage, PopupBorderPageIndex    , NULL  , PopupBorderWin    , PAGE_Init , NULL   , FALSE);
    PAGE_createElement (PopupPage, PopupBodyPageIndex      , object, PopupBodyWin      , PAGE_Input, handler, TRUE);

    PAGE_fkey(PopupPage, 1) = execfn;

    PAGE_status(PopupPage) = PAGE_ExitOnExec;
}

PrivateFnDef void
    DeletePopupPage ()
{
    int i;

    CUR_delwin (PopupBodyWin);
    CUR_delwin (PopupBorderWin);
    PAGE_deletePage (PopupPage, i);
}

PrivateFnDef void
    ProcessPopupPage ()
{
    PAGE_handler (PopupPage);
}

/***************************
 *****  Menu Managers  *****
 ***************************/

/*************
 *  General  *
 *************/

PrivateFnDef MENU *GetMenu (
    char const *		title,
    void			(*handler)(),
    char const*			genformat,
    ...
)
{
    va_list ap;
    MENU *menu;
    int i;

    va_start (ap, genformat);

    if ( (menu = MENU_getMenu (VFormatString (genformat, ap) ) ) != NULL )
    {
	for( i=0 ; i<MENU_choiceCount(menu) ; i++ )
	    MENU_choiceHandler(menu, i) = handler;
	MENU_title(menu) = title;
    }

    va_end (ap);

    return menu;
}

/***************
 *  Null Menu  *
 ***************/

PrivateFnDef void SelectNullMenuEntry () {
    ERR_displayPause("Select an entry from previous menu first");
}

/*******************
 *  Instance Menu  *
 *******************/

PrivateFnDef MENU *GetInstanceMenu (
    int				catalogType,
    char const*			catalog,
    void			(*handler)()
)
{
    char const *title, *catalogAccessMsg;
    char classSelector  [RS_MaxLine + 1],
	 catalogSelector[RS_MaxLine + 1];

/*****  Decode catalog type, ...  *****/
    if (CTypeObject  == catalogType)
    {
	catalogAccessMsg = ObjectCatalogAccessMsg;
	title = LevelIndividual == Level ? " Object:" : " Collection:";
    }
    else
    {
	catalogAccessMsg = MessageCatalogAccessMsg;
	title = " Message:";
    }

    return GetMenu (title,
		    handler,
		    "FrontEndTools ClassInfo %s %s %s displayMessages",
		    StringAsValidSelector (classSelector, ClassName),
		    catalogAccessMsg,
		    StringAsValidSelector (catalogSelector, catalog));
}

PrivateFnDef void SelectInstance () {
    char classSelector  [RS_MaxLine + 1],
	 catalogSelector[RS_MaxLine + 1],
	 objectSelector [RS_MaxLine + 1],
	 messageSelector[RS_MaxLine + 1];

    if (XMENU_BaseMenu (MessageInstance) == NullMenu)
    {
    	TargetPageWindow = MessageCatalogWin;
	return;
    }

    if (XMENU_BaseMenu (ObjectInstance) == NullMenu)
    {
    	TargetPageWindow = ObjectCatalogWin;
	return;
    }

    if (SelectorTakesParameters (XMENU_currChoiceLabel (ObjectInstance)))
    {
	ERR_displayPause ("Parameters not implemented for object selectors");
	return;
    }

    if (SelectorTakesParameters (XMENU_currChoiceLabel (MessageInstance)))
    {
	ERR_displayPause ("Parameters not implemented for message selectors");
	return;
    }

    ERR_displayMsg (ERR_ExecBuffer);

    BUF_eraseBuffer (BrowserInput);
    BUF_appendLine
    (
	BrowserInput,
	FormatString
	(
	    "FrontEndTools ClassInfo %s %s %s %s %s",
	    StringAsValidSelector (classSelector  , ClassName),
	    ObjectCatalogAccessMsg,
	    StringAsValidSelector (catalogSelector, ObjectCatalogName),
	    StringAsValidSelector (objectSelector , XMENU_currChoiceLabel (ObjectInstance)),
	    StringAsValidSelector (messageSelector, XMENU_currChoiceLabel (MessageInstance))
	)
    );

#if RSATTACH
    EDIT_browserIO ();
#endif

    BUF_screenRow (BrowserBuf) = 0;
    ERR_clearMsg ();
    BUF_handler (BrowserBuf, BufferWin, PAGE_Init);

    TargetPageWindow = BufferWin;
}

/******************
 *  Catalog Menu  *
 ******************/

PrivateFnDef MENU *GetCatalogMenu (
    int				catalogType,
    char const*			cLass,
    char const*			catalogAccessMsg,
    void			(*handler)()
) {
    char const *title;
    char classSelector[RS_MaxLine + 1];

/*****  Decode catalog type, ...  *****/
    if (CTypeObject == catalogType)
	title = " Object Catalog:";
    else
	title = " Message Catalog:";

    return GetMenu (title,
		    handler,
		    "FrontEndTools ClassInfo %s %s displayMessages",
		    StringAsValidSelector (classSelector, cLass),
		    catalogAccessMsg);
}


PrivateFnDef void SelectCatalog (
    int				catalogType,
    XMENU *			sourceXMenu,
    char *			targetCatalogName,
    XMENU *			targetXMenu,
    CUR_WINDOW *		targetWindow,
    void			(*handler)(),
    char const*			errorMessage
)
{
    char const *const catalog = XMENU_currChoiceLabel (sourceXMenu);
    MENU *const instances = GetInstanceMenu (catalogType, catalog, handler);
    if (instances == NULL)
    {
	ERR_displayPause(errorMessage);
	XMENU_LastChoice (sourceXMenu) = -1;
	XMENU_PendingAction (sourceXMenu) = PAGE_Input;
	return;
    }

    strcpy (targetCatalogName, catalog);

    XMENU_ChangeMenu (targetWindow, targetXMenu, instances, TRUE, FALSE);

    TargetPageWindow = targetWindow;
}

PrivateFnDef void SelectObjectCatalog () {
    SelectCatalog
	(CTypeObject,
	 ObjectCatalog,
	 ObjectCatalogName,
	 ObjectInstance,
	 ObjectInstanceWin,
	 SelectInstance,
	 "Error reading object instance catalog");
}

PrivateFnDef void SelectMessageCatalog () {
    SelectCatalog
	(CTypeMessage,
	 MessageCatalog,
	 MessageCatalogName,
	 MessageInstance,
	 MessageInstanceWin,
	 SelectInstance,
	 "Error reading message instance catalog");
}

/*********************************
 *****  Popup Form Managers  *****
 *********************************/

/*************
 *  General  *
 *************/

PrivateFnDef void ProcessPopupForm (
    char const *title, void (*execfn)(), FORM *form, int ysize, int xsize
) {
    CreatePopupPage (title, execfn, FORM_handler, form, ysize, xsize);
    ProcessPopupPage ();
    DeletePopupPage ();
}


/******************
 *  Context Form  *
 ******************/

PrivateFnDef void CreateContextForm () {
    int i;

    if (NULL == ClassMenu || NULL == LevelMenu)
	return;

    if (NULL != ContextForm)
	return;

    FORM_makeForm (ContextForm, ContextFormFields, i);

    FORM_fieldMenu (ContextFormClassField) = ClassMenu;
    FORM_fieldMenu (ContextFormLevelField) = LevelMenu;
}


PrivateFnDef void ExecuteContextForm () {
    int level, i;
    MENU *objectCatalog, *messageCatalog;
    char const *className, *objectCatalogAccessMsg, *messageCatalogAccessMsg;

    level = MENU_currChoice (FORM_fieldMenu (ContextFormLevelField));
    className = FORM_fieldValue (ContextFormClassField);
    if (LevelIndividual == level)
    {
	objectCatalogAccessMsg  = "individualObjectCatalog";
	messageCatalogAccessMsg = "individualMessageCatalog";
    }
    else
    {
	objectCatalogAccessMsg  = "collectionObjectCatalog";
	messageCatalogAccessMsg = "collectionMessageCatalog";
    }

    objectCatalog = 
	GetCatalogMenu (CTypeObject, className, objectCatalogAccessMsg, SelectObjectCatalog);
    if (objectCatalog == NULL)
    {
	ERR_displayPause("Error reading object catalog");
	PAGE_status(PopupPage) = PAGE_Normal;
	return /*FALSE*/;
    }

    messageCatalog =
	GetCatalogMenu (CTypeMessage, className, messageCatalogAccessMsg, SelectMessageCatalog);
    if (messageCatalog == NULL)
    {
	MENU_deleteMenu (objectCatalog, i);
	ERR_displayPause("Error reading message catalog");
	PAGE_status(PopupPage) = PAGE_Normal;
	return /*FALSE*/;
    }

    Level			= level;
    ObjectCatalogAccessMsg	= objectCatalogAccessMsg;
    MessageCatalogAccessMsg	= messageCatalogAccessMsg;
    strcpy (ClassName		, FORM_fieldValue (ContextFormClassField));

    XMENU_ChangeMenu (ObjectCatalogWin  , ObjectCatalog  , objectCatalog  , TRUE , TRUE );
    XMENU_ChangeMenu (ObjectInstanceWin , ObjectInstance , NullMenu       , FALSE, FALSE);
    XMENU_ChangeMenu (MessageCatalogWin , MessageCatalog , messageCatalog , TRUE , TRUE );
    XMENU_ChangeMenu (MessageInstanceWin, MessageInstance, NullMenu       , FALSE, FALSE);

    PAGE_status(PopupPage) = PAGE_ExitOnExec;
    TargetPageWindow = ObjectCatalogWin;
}

PrivateFnDef void DetermineContext () {
    CreateContextForm ();

    if (NULL != ContextForm)
	ProcessPopupForm 
	    ("Browsing Context:", ExecuteContextForm, ContextForm, 5, 32);
}

PrivateFnDef int
    ContextDetermined ()
{
    return XMENU_BaseMenu (ObjectCatalog) != NullMenu && XMENU_BaseMenu (MessageCatalog) != NullMenu;
}

/**********************
 *  Add Catalog Form  *
 **********************/

PrivateFnDef void
    CreateAddCatalogForm ()
{
    int i;

    if (NULL == ClassMenu || NULL == LevelMenu || NULL == CTypeMenu)
	return;

    if (NULL == AddCatalogForm)
    {
	FORM_makeForm  (AddCatalogForm, AddCatalogFormFields, i);
	FORM_fieldMenu (AddCatalogFormClassField) = ClassMenu;
	FORM_fieldMenu (AddCatalogFormLevelField) = LevelMenu;
	FORM_fieldMenu (AddCatalogFormCTypeField) = CTypeMenu;
    }
    strcpy (FORM_fieldValue (AddCatalogFormCNameField), "");
}

PrivateFnDef void ExecuteAddCatalogForm () {
    char cLass	[RS_MaxLine],
	 catalog[RS_MaxLine],
	 *expression;
    int  selectedLevel,
	 selectedCType;
    MENU *newCatalog;

    if (strlen (FORM_fieldValue (AddCatalogFormCNameField)) == 0)
    {
	ERR_displayPause ("Catalog Name Not Specified");
	return /*FALSE*/;
    }

    selectedLevel = MENU_currChoice (FORM_fieldMenu (AddCatalogFormLevelField));
    selectedCType = MENU_currChoice (FORM_fieldMenu (AddCatalogFormCTypeField));
    expression =
	FormatString
	    ("FrontEndTools ClassInfo %s create%s%sCatalog: \"%s\"",
	     StringAsValidSelector (cLass, FORM_fieldValue (AddCatalogFormClassField)),
	     LevelIndividual == selectedLevel ? "Individual" : "Collection",
	     CTypeObject == selectedCType ? "Object" : "Message",
	     StringAsValidSelector (catalog, FORM_fieldValue (AddCatalogFormCNameField)));

    if (RS_sendAndCheck (expression, ">>>"))
	ERR_displayPause ("Error adding catalog");
    else if (Level != selectedLevel || 
	     strcmp (cLass, FORM_fieldValue (AddCatalogFormClassField)) != 0)
    {
    }
    else if (CTypeObject == selectedCType)
    {
	newCatalog = 
	    GetCatalogMenu (CTypeObject, ClassName, ObjectCatalogAccessMsg, SelectObjectCatalog);
	if (newCatalog == NULL)
	{
	    ERR_displayPause("Error re-reading object catalog");
	}
	else
	{
	    XMENU_ChangeMenu (ObjectCatalogWin , ObjectCatalog , newCatalog, TRUE , TRUE );
	    XMENU_ChangeMenu (ObjectInstanceWin, ObjectInstance, NullMenu  , FALSE, TRUE );
	    TargetPageWindow = ObjectCatalogWin;
	}
    }
    else
    {
	newCatalog =
	    GetCatalogMenu (CTypeMessage, ClassName, MessageCatalogAccessMsg, SelectMessageCatalog);
	if (newCatalog == NULL)
	{
	    ERR_displayPause("Error re-reading message catalog");
	}
	else
	{
	    XMENU_ChangeMenu (MessageCatalogWin , MessageCatalog , newCatalog, TRUE , TRUE );
	    XMENU_ChangeMenu (MessageInstanceWin, MessageInstance, NullMenu  , FALSE, FALSE);
	    TargetPageWindow = MessageCatalogWin;
	}
    }
}

PrivateFnDef void AddCatalog () {
    CreateAddCatalogForm ();

    if (NULL != AddCatalogForm)
	ProcessPopupForm
	    ("Add object or message catalog:",
	     ExecuteAddCatalogForm, AddCatalogForm, 5, 62);
}

/********************************
 *  Add Object / Message Forms  *
 ********************************/

/*****  Add Object Form  *****/
PrivateFnDef void CreateAddObjectForm () {
    int i;

    if (NULL == ClassMenu || NULL == LevelMenu || NULL == ETypeMenu)
	return;

    if (NULL == AddObjectForm)
    {
	FORM_makeForm  (AddObjectForm, AddObjectFormFields, i);

	FORM_fieldMenu (AddObjectFormClassField) = ClassMenu;
	FORM_fieldMenu (AddObjectFormLevelField) = LevelMenu;
	FORM_fieldMenu (AddObjectFormETypeField) = ETypeMenu;
    }
    FORM_fieldMenu (AddObjectFormCNameField) = XMENU_BaseMenu (ObjectCatalog);
    strcpy (FORM_fieldValue (AddObjectFormONameField), "");
    strcpy (FORM_fieldValue (AddObjectFormODefnField), "");
}

PrivateFnDef void ExecuteAddObjectForm () {
    char cLass	[RS_MaxLine],
	 catalog[RS_MaxLine],
	 *expression;
    int  deferringExecution;

    if (strlen (FORM_fieldValue (AddObjectFormONameField)) == 0)
    {
	ERR_displayPause ("Object name not specified");
	return /*FALSE*/;
    }
    if (SelectorTakesParameters (FORM_fieldValue (AddObjectFormONameField)))
    {
	ERR_displayPause ("Object name cannot require parameters");
	return /*FALSE*/;
    }

    if (strlen (FORM_fieldValue (AddObjectFormODefnField)) == 0)
    {
	ERR_displayPause ("Object definition not specified");
	return /*FALSE*/;
    }

    deferringExecution =
	ETypeLaterIndex == MENU_currChoice (FORM_fieldMenu (AddObjectFormETypeField));
    expression =
	FormatString
	    ("FrontEndTools ClassInfo %s %s %s add: \"%s\" as: %c%s%c;",
	     StringAsValidSelector (cLass, ClassName),
	     ObjectCatalogAccessMsg,
	     StringAsValidSelector (catalog, FORM_fieldValue (AddObjectFormCNameField)),
	     FORM_fieldValue (AddObjectFormONameField),
	     deferringExecution ? '[' : '(',
	     FORM_fieldValue (AddObjectFormODefnField),
	     deferringExecution ? ']' : ')');

    if (RS_sendAndCheck (expression, ">>>"))
	ERR_displayPause ("Error adding object to catalog");
    else if (strcmp (ObjectCatalogName, FORM_fieldValue (AddObjectFormCNameField)) == 0)
    {
	SelectCatalog
	    (CTypeObject,
	     ObjectCatalog,
	     ObjectCatalogName,
	     ObjectInstance,
	     ObjectInstanceWin,
	     SelectInstance,
	     "Error re-reading object instance catalog");
    }
}

PrivateFnDef int AddObject () {
    CreateAddObjectForm ();

    if (NULL != AddObjectForm)
	ProcessPopupForm
	    ("Add object to catalog:",
	     ExecuteAddObjectForm, AddObjectForm, 11, 67);

    return (FALSE);
}

/*****   Add Message Form  *****/
PrivateFnDef void CreateAddMessageForm () {
    int i;

    if (NULL == ClassMenu || NULL == LevelMenu)
	return;

    if (NULL == AddMessageForm)
    {
	FORM_makeForm  (AddMessageForm, AddMessageFormFields, i);

	FORM_fieldMenu (AddMessageFormClassField) = ClassMenu;
	FORM_fieldMenu (AddMessageFormLevelField) = LevelMenu;
    }
    FORM_fieldMenu (AddMessageFormCNameField) = XMENU_BaseMenu (MessageCatalog);
    strcpy (FORM_fieldValue (AddMessageFormMNameField), "");
}

PrivateFnDef void ExecuteAddMessageForm () {
    char cLass	[RS_MaxLine],
	 catalog[RS_MaxLine],
	 *expression;

    if (strlen (FORM_fieldValue (AddMessageFormMNameField)) == 0)
    {
	ERR_displayPause ("Message name not specified");
	return /*FALSE*/;
    }
    if (SelectorTakesParameters (FORM_fieldValue (AddMessageFormMNameField)))
    {
	ERR_displayPause ("Message cannot require parameters");
	return /*FALSE*/;
    }

    expression =
	FormatString
	    ("FrontEndTools ClassInfo %s %s %s add: \"%s\" as: NA;",
	     StringAsValidSelector (cLass, ClassName),
	     MessageCatalogAccessMsg,
	     StringAsValidSelector (catalog, FORM_fieldValue (AddMessageFormCNameField)),
	     FORM_fieldValue (AddMessageFormMNameField));

    if (RS_sendAndCheck (expression, ">>>"))
	ERR_displayPause ("Error adding message to catalog");
    else if (strcmp (MessageCatalogName, FORM_fieldValue (AddMessageFormCNameField)) == 0)
    {
	SelectCatalog
	    (CTypeMessage,
	     MessageCatalog,
	     MessageCatalogName,
	     MessageInstance,
	     MessageInstanceWin,
	     SelectInstance,
	     "Error re-reading messageinstance catalog");
    }
}

PrivateFnDef int AddMessage () {
    CreateAddMessageForm ();

    if (NULL != AddMessageForm)
	ProcessPopupForm
	    ("Add message to catalog:",
	     ExecuteAddMessageForm, AddMessageForm, 7, 67);

    return (FALSE);
}
/*****  Add Object/Message Form Dispatcher  *****/
PrivateFnDef void AddToCatalog () {
    if (LastDisplayObject == (caddr_t)ObjectCatalog || LastDisplayObject == (caddr_t)ObjectInstance)
	AddObject ();
    else if (LastDisplayObject == (caddr_t)MessageCatalog|| LastDisplayObject == (caddr_t)MessageInstance)
	AddMessage ();
    else
	ERR_displayPause ("Please select an object or message catalog first");
}

/*******************************************
 *****  Fixed Display Object Managers  *****
 *******************************************/

PrivateFnDef void
    CreateFixedDisplayObjects ()
{
    int longest, i, j;

    MENU_makeMenu (NullMenu, NullMenuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    ObjectCatalog   = XMENU_create (NullMenu, "Select object catalog");
    ObjectInstance  = XMENU_create (NullMenu, "Select object");
    MessageCatalog  = XMENU_create (NullMenu, "Select message catalog");
    MessageInstance = XMENU_create (NullMenu, "Select message");

    MENU_makeMenu (F8Menu, F8MenuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title    (F8Menu) = " Browser:";

    if( (ClassMenu = GetMenu(" Class:", FORM_menuToForm, "FrontEndTools ClassInfo displayMessages")) == NULL )
    {
	ERR_displayPause("Error reading class menu");
	return;
    }

    MENU_makeMenu (LevelMenu, LevelMenuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title    (LevelMenu) = " Level:";

    MENU_makeMenu (CTypeMenu, CTypeMenuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title    (CTypeMenu) = " Catalog Type:";

    MENU_makeMenu (ETypeMenu, ETypeMenuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title    (ETypeMenu) = " Evaluation Type:";

    FORM_makeForm (Background , BackgroundFields , i);
    FORM_makeForm (HelpBanner , HelpBannerFields , i);
}

/*********************
 *****  Browser  *****
 *********************/
PublicVarDef int	inBrowser = FALSE;

PublicFnDef void browser() {
    static int firstEntry = TRUE;

/*****  Guard against recursive invocation (the browser is not re-entrant), ...  *****/
    if (inBrowser)
    {
	ERR_displayPause ("Browser already running");
	return;
    }
    inBrowser = TRUE;

/*****  ... perform one-time only initializations, ...  *****/
    if (firstEntry)
    {
	CreateFixedDisplayObjects ();
	firstEntry = FALSE;
    }

/*****  ... run the browser, ...  *****/
    CreateMasterPage ();

    if (!ContextDetermined ())
	DetermineContext ();
    if (ContextDetermined ())
	ProcessMasterPage ();

/*****  ... cleanup, ...  *****/
    DeleteMasterPage ();
    inBrowser = FALSE;

/*****  ... and exit.  *****/
    return;
}
