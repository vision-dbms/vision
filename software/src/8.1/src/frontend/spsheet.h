/******** Spread Sheet Header File ********/
#ifndef SPR_H
#define SPR_H

/***** Shared definitions *****/
#include "spsheet.d"
#include "page.d"

/***** Function Declarations ******/

PublicFnDecl SPRSHEET *SPR_readSpreadSheet();

PublicFnDecl int SPR_readSSheet(), SPR_makeDummySheet();

PublicFnDecl PAGE_Action SPR_handler();

PublicFnDecl void SPR_delete();

PublicVarDecl SPRSHEET	*SPR_CurrSPR, *SPR_OldSPR;
PublicVarDecl int	SPR_simulateApplic;

#endif


/************************************************************************
  spsheet.h 2 replace /users/ees/interface
  860730 15:31:09 ees  

 ************************************************************************/
/************************************************************************
  spsheet.h 3 replace /users/ees/interface
  860730 16:15:33 ees  

 ************************************************************************/
/************************************************************************
  spsheet.h 4 replace /users/ees/interface
  860808 13:58:23 ees added decl for readSSheet()

 ************************************************************************/
/************************************************************************
  spsheet.h 5 replace /users/lis/frontend/M2/sys
  870811 17:06:29 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  spsheet.h 6 replace /users/lis/frontend/M2/sys
  880617 19:35:56 m2 Report writer, etc. redefinitions

 ************************************************************************/
