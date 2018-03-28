/******	Menu Header File *******/
#ifndef MENU_H
#define MENU_H

/*****  Components  ****/
#include "VReferenceable.h"
#include "V_VString.h"

/*****  Declarations  *****/
#include "menu.d"
#include "page.d"

/***** Classes and Structures *****/
struct MENU_Choice {
    V::VString label;
    V::VString help;
    char letter;	 
    void (*handler)();
    char active;
};

struct MENU : public VReferenceable {
    DECLARE_CONCRETE_RTTLITE (MENU,VReferenceable);
    
//  Construction
public:
    MENU (MENU_Choice *choices, int norm, int high, int &longest, int &i, int &j);

//  Destruction
private:
    ~MENU ();

//  Access
public:
    size_t longest () const {
	return m_longest;
    }

//  State
private:
    size_t m_longest;
public:
    char const *title;
    int choiceCount;
    int currChoice;
    int normalAttr;
    int hilightAttr;
    int status;
    int flags;
    MENU_Choice **choice;
};

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
