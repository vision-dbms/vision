/****************************************************************************
*****									*****
*****			page.d						*****
*****									*****
****************************************************************************/
#ifndef PAGE_D
#define PAGE_D

#include "stdcurses.h"
#include "keys.h"
#include "error.h"
#include "menu.d"

#ifndef  DBMENU
#include "buffers.d"
#endif

#define PAGE_Elements 10

typedef enum	{PAGE_Normal, 
		 PAGE_ExitOnExec, 
} PAGE_Status;

typedef enum   {PAGE_Init, 
		PAGE_Exec, 
		PAGE_Help, 
		PAGE_Window, 
		PAGE_Quit,
		PAGE_Error,
	        PAGE_Input,
	        PAGE_Scroll,
		PAGE_Refresh, 
		PAGE_End, 
		PAGE_Success, 
		PAGE_SMenu, 
		PAGE_AMenu, 
		PAGE_Prev, 
		PAGE_F4, 
		PAGE_File, 
		PAGE_Region, 
		PAGE_CheckList,  
		PAGE_RefreshAndNext,  
		PAGE_Profile,  
		PAGE_Editor, 
		PAGE_Module, 
} PAGE_Action;

#define	PAGE_noType	0
#define	PAGE_menuType	1
#define	PAGE_formType	2
#define	PAGE_pageType	3

typedef PAGE_Action (*PAGE_Handler)(void*,CUR_WINDOW*,PAGE_Action);

typedef struct {
    void	    *object;
    CUR_WINDOW	    *window;
    PAGE_Action	    action;
    PAGE_Handler    handler;
    unsigned int    enter;
} PAGE_Element;

#ifndef DBMENU
#define PAGE_HelpType	LINEBUFFER
#else
#define PAGE_HelpType	char
#endif

typedef struct {
    PAGE_HelpType   *help;
    int		    elementCount;
    PAGE_Element    *element[PAGE_Elements];
    char	    *obj;
    CUR_WINDOW	    *objWin;
    int		    objType;
    void	    (*fkeyFunc[10])();
    PAGE_Status	    status;
} PAGE;

#define PAGE_object(page, n)	    (page->element[n]->object)
#define PAGE_window(page, n)	    (page->element[n]->window)
#define PAGE_action(page, n)	    (page->element[n]->action)
#define PAGE_handlerFn(page, n)	    (page->element[n]->handler)
#define PAGE_handlerCall(page, n)   (*page->element[n]->handler)
#define PAGE_enter(page, n)	    (page->element[n]->enter)
#define PAGE_status(page)	    (page->status)


#define PAGE_help(page)	            (page->help)
#define PAGE_windowCount(page)	    (page->elementCount)
#define PAGE_element(page, n)	    (page->element[n])
#define PAGE_obj(page)		    (page->obj)
#define PAGE_objWin(page)	    (page->objWin)
#define PAGE_objType(page)	    (page->objType)
#define PAGE_status(page)	    (page->status)
#define PAGE_fkey(page, n)	    (page->fkeyFunc[n])
#define PAGE_pageHelp(page)	    ((*page->fkeyFunc[0])())
#define PAGE_pageExec(page)	    ((*page->fkeyFunc[1])())
#define PAGE_pageWindow(page)	    ((*page->fkeyFunc[2])())
#define PAGE_pageEdit(page)	    ((*page->fkeyFunc[3])())
#define PAGE_pageFile(page)	    ((*page->fkeyFunc[4])())
#define PAGE_pageRegion(page)	    ((*page->fkeyFunc[5])())
#define PAGE_pageSystem(page)	    ((*page->fkeyFunc[6])())
#define PAGE_pageApplic(page)	    ((*page->fkeyFunc[7])())
#define PAGE_pagePrev(page)	    ((*page->fkeyFunc[8])())
#define PAGE_pageQuit(page)	    ((*page->fkeyFunc[9])())

#define PAGE_initScreen()\
{\
    CUR_nonl();\
    CUR_cbreak();\
    CUR_noecho();\
    ERR_Window = CUR_newwin(1, CUR_COLS, CUR_LINES - 1, 0);\
}\

#define PAGE_endScreen()\
{\
    CUR_erase();\
    CUR_touchwin(CUR_stdscr);\
    CUR_refresh();\
}

#define PAGE_createPage(pge, n, hlp, obj, ow, ot, i)\
{\
    if (NULL == (pge = (PAGE *)malloc(sizeof(PAGE))))\
        ERR_fatal(" Malloc Error");\
    PAGE_windowCount(pge) = n;\
    PAGE_help(pge) = (PAGE_HelpType *)hlp;\
    PAGE_obj(pge) = (char *)obj;\
    PAGE_objWin(pge) = (CUR_WINDOW *)ow;\
    PAGE_objType(pge) = (int)ot;\
    for (i = 0; i < 10; i++)\
        PAGE_fkey(pge, i) = (void(*)())NULL;\
    PAGE_status(pge) = PAGE_Normal;\
}


#define PAGE_createElement(pge, n, obj, win, action, fn, enter)\
{\
    if (NULL == (PAGE_element(pge, n) =\
		 (PAGE_Element *)malloc(sizeof(PAGE_Element))))\
        ERR_fatal(" Malloc error");\
    PAGE_object(pge, n) = (char *)obj;\
    PAGE_window(pge, n) = (CUR_WINDOW *)win;\
    PAGE_action(pge, n) = (PAGE_Action)action;\
    PAGE_handlerFn(pge, n) = (PAGE_Handler)(fn);\
    PAGE_enter(pge, n) = (unsigned int)enter;\
}

#define PAGE_deletePage(pge, n)\
{\
    for (n = 0; n < PAGE_windowCount(pge); n++)\
        free(PAGE_element(pge, n));\
    free(pge);\
}

#endif
