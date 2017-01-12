/****************************************************************************
*****									*****
*****			page.h						*****
*****									*****
****************************************************************************/

#ifndef PAGE_H
#define PAGE_H

#include "Vk.h"

/***** Shared definitions *****/
#include "page.d"

PublicVarDecl int	PAGE_ExitSystem, PAGE_ToNextEltOnExec,
			PAGE_ExitF8PageParent;

PublicFnDecl void	PAGE_runSysMenu(PAGE *opage);
PublicFnDecl void	PAGE_handler(PAGE *page);


#endif


/************************************************************************
  page.h 2 replace /users/ees/interface
  860730 15:33:25 ees  

 ************************************************************************/
/************************************************************************
  page.h 3 replace /users/ees/interface
  860730 16:16:18 ees  

 ************************************************************************/
/************************************************************************
  page.h 4 replace /users/lis/frontend/M2/sys
  870811 17:05:30 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  page.h 5 replace /users/m2/frontend/sys
  880706 11:29:30 m2 Browser, report writertime series reflow, cosmetics/

 ************************************************************************/
/************************************************************************
  page.h 6 replace /users/m2/frontend/sys
  880712 14:38:03 m2 Reentrancy fix for page handler

 ************************************************************************/
/************************************************************************
  page.h 7 replace /users/m2/frontend/sys
  880727 17:53:41 m2 Completion of Phase 1 revisions

 ************************************************************************/
