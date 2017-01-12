/******** Spread Sheet Header File ********/
#ifndef SPS_H
#define SPS_H

/***** Shared definitions *****/
#include "stdcurses.h"

#include "sps.d"
#include "page.d"

PublicFnDecl void SPS_refreshPage();

PublicFnDecl void SPS_clearCell();
PublicFnDecl void SPS_doClearCell(
    SPSHEET *spr, CUR_WINDOW *win
);

PublicFnDecl void SPS_clearInput();
PublicFnDecl int SPS_doClearInput(
    SPSHEET *spr, CUR_WINDOW *win
);

PublicFnDecl void SPS_transposeAxes();
PublicFnDecl void SPS_rotateAxes();
PublicFnDecl void SPS_incrementZ();
PublicFnDecl void SPS_decrementZ();

PublicFnDecl SPS_cell *SPS_cellPtrXYZ(
    SPSHEET *spr, int x, int y, int z
);
PublicFnDecl void SPS_delete(SPSHEET *spr);

PublicFnDecl int SPS_readHeadings(SPSHEET *spr, char const *list, int flag);
PublicFnDecl SPSHEET *SPS_readSSheet(char const *list);
PublicFnDecl int SPS_readCells(SPSHEET *spr, char const *list);

PublicFnDecl int SPS_writeSSheet(SPSHEET *spr, char const *list);
PublicFnDecl PAGE_Action SPS_handler(
    SPSHEET *spsheet, CUR_WINDOW *win, PAGE_Action action
);
PublicFnDecl void SPS_paintScreen(SPSHEET *spr, CUR_WINDOW *win);


#endif
