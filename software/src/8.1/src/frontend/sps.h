/******** Spread Sheet Header File ********/
#ifndef SPS_H
#define SPS_H

/***** Shared definitions *****/
#include "sps.d"

PublicFnDecl int  SPS_transposeAxes(), SPS_rotateAxes();
PublicFnDecl int  SPS_incrementZ(), SPS_decrementZ();
PublicFnDecl int  SPS_clearInput();
PublicFnDecl int  SPS_clearCell();
PublicFnDecl int  SPS_refreshPage();
PublicFnDecl int  SPS_doClearInput();
PublicFnDecl SPSHEET *SPS_readSSheet();
PublicFnDecl int SPS_writeSSheet();
PublicFnDecl int SPS_readHeadings();
PublicFnDecl int SPS_readCells();
PublicFnDecl PAGE_Action SPS_handler();

#endif
