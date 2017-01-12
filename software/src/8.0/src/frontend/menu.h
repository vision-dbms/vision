/******	Menu Header File *******/
#ifndef MENU_H
#define MENU_H

#include "stdcurses.h"
#include "page.d"

/***** Shared Definitions *****/
#include "menu.d"

/***** Function Declarations *****/

PublicFnDecl MENU *MENU_read(char const *filename, int isBuffer);
PublicFnDecl PAGE_Action MENU_handler(
    MENU *menu, CUR_WINDOW *menuWin, PAGE_Action action
);
PublicFnDecl CUR_WINDOW *MENU_makeWindow(
    MENU *menu, int startrow, int startcol, int maxrows
);

PublicFnDecl MENU *MENU_getMenu(char const *message);
PublicFnDecl MENU *MENU_getMainMenu();

#endif


/************************************************************************
  menu.h 2 replace /users/ees/interface
  860730 15:32:29 ees  

 ************************************************************************/
/************************************************************************
  menu.h 3 replace /users/ees/interface
  860730 16:16:05 ees  

 ************************************************************************/
/************************************************************************
  menu.h 4 replace /users/lis/frontend/M2/sys
  870811 17:05:07 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  menu.h 5 replace /users/lis/frontend/M2/sys
  880614 14:21:09 m2 Printer form fix, read mainmenu from backend, adding skeleton for profile variables

 ************************************************************************/
