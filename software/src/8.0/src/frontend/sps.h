/******** Spread Sheet Header File ********/
#ifndef SPS_H
#define SPS_H

/***** Shared definitions *****/
#include "stdcurses.h"

#include "sps.d"
#include "page.d"

PublicFnDecl void SPS_refreshPage(void);

PublicFnDecl void SPS_clearCell(void);
PublicFnDecl void SPS_doClearCell(
    SPSHEET *spr, CUR_WINDOW *win
);

PublicFnDecl void SPS_clearInput(void);
PublicFnDecl int SPS_doClearInput(
    SPSHEET *spr, CUR_WINDOW *win
);

PublicFnDecl void SPS_transposeAxes(void);
PublicFnDecl void SPS_rotateAxes(void);
PublicFnDecl void SPS_incrementZ(void);
PublicFnDecl void SPS_decrementZ(void);

PublicFnDecl SPS_cell *SPS_cellPtrXYZ(
    SPSHEET *spr, int x, int y, int z
);
PublicFnDecl void SPS_delete(SPSHEET *spr);

PublicFnDef int SPS_readHeadings(SPSHEET *spr, char *list, int flag);
PublicFnDecl SPSHEET *SPS_readSSheet(char *list);
PublicFnDecl int SPS_readCells(SPSHEET *spr, char *list);

PublicFnDecl int SPS_writeSSheet(SPSHEET *spr, char *list);
PublicFnDecl PAGE_Action SPS_handler(
    SPSHEET *spsheet, CUR_WINDOW *win, PAGE_Action action
);
PublicFnDecl void SPS_paintScreen(SPSHEET *spr, CUR_WINDOW *win);


#endif
