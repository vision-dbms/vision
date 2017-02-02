/****************************************************************************
*****									*****
*****			window.h					*****
*****									*****
****************************************************************************/
#ifndef WINDOW_H
#define WINDOW_H

#include "window.d"

PublicFnDecl void WIN_LineBox(
    CUR_WINDOW *win, char const *title, char const *info
);

PublicFnDecl void WIN_RepaintWindow(CUR_WINDOW *win, int attr);

PublicFnDecl void WIN_HighlightBox(
    CUR_WINDOW *win, int attr, char const *title, char const *info
);

PublicFnDecl void WIN_ReverseBox(CUR_WINDOW *win, char const *title);

PublicFnDecl void WIN_ShadowBox(CUR_WINDOW *win);
		  

#endif


/************************************************************************
  window.h 2 replace /users/ees/interface
  860730 15:28:31 ees  

 ************************************************************************/
/************************************************************************
  window.h 3 replace /users/ees/interface
  860808 14:00:47 ees added function decls

 ************************************************************************/
/************************************************************************
  window.h 4 replace /users/lis/frontend/M2/sys
  870811 17:07:02 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
