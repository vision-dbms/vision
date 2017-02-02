/****************************************************************************
*****									*****
*****			window.d					*****
*****									*****
****************************************************************************/
#ifndef WINDOW_D
#define WINDOW_D

#include "stdcurses.h"

#define WIN_DrawLine(win, row, c, i)\
{\
    CUR_wmove(win, row, 0);\
    for (i = 0; i < CUR_WIN_cols(win); i++)\
        CUR_waddch(win, c);\
}\

#endif
