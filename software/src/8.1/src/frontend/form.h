/******	Form Header File *******/
#ifndef FORM_H
#define FORM_H

/***** Shared Definitions *****/
#include "form.d"

/***** Function Declarations *****/
PublicFnDecl FORM *FORM_read();

PublicFnDecl PAGE_Action FORM_handler();

PublicFnDecl int FORM_menuToForm();

PublicFnDecl int FORM_centerFormElts();

#endif


/************************************************************************
  form.h 2 replace /users/ees/interface
  860730 15:33:11 ees  

 ************************************************************************/
/************************************************************************
  form.h 3 replace /users/lis/frontend/M2/sys
  870811 17:04:30 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  form.h 4 replace /users/lis/frontend/M2/sys
  880505 11:04:15 m2 Apollo and VAX port

 ************************************************************************/
