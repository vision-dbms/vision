/********************************************************
 *********	buffers.h			*********
 *******************************************************/
#ifndef BUFFERS_H
#define BUFFERS_H

#include "page.d"
#include "buffers.d"

PublicFnDecl int BUF_getColFromSCol(
    LINEBUFFER	*buffer,
    char	*row,
    int		scol,
    int		*nscol
);
PublicFnDecl int BUF_getSColFromCol(
    LINEBUFFER	*buffer,
    char	*row,
    int		col
);
PublicFnDecl int BUF_setCol(
    LINEBUFFER	*buffer,
    CUR_WINDOW	*win,
    int		col
);
PublicFnDecl void BUF_resetScreen(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);
PublicFnDecl void BUF_paintWindow(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);
PublicFnDecl void BUF_paintLine(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);

PublicFnDecl int BUF_initBuffer(
    LINEBUFFER *buffer,
    int size
);
PublicFnDecl int BUF_eraseBuffer(
    LINEBUFFER *buffer
);
PublicFnDecl void BUF_compact(
    LINEBUFFER *buffer
);

PublicFnDecl char *BUF_getLine(
    LINEBUFFER *buffer,
    int len
);
PublicFnDecl char *BUF_appendLine(
    LINEBUFFER *buffer,
    char *str
);
PublicFnDecl char *BUF_insertLine(
    LINEBUFFER *buffer,
    char *before,
    char *str
);
PublicFnDecl int BUF_deleteLine(
    LINEBUFFER *buffer,
    char *bufrow
);

PublicFnDecl int BUF_recalcCols(
    LINEBUFFER	*buffer,
    CUR_WINDOW	*win
);

PublicFnDecl int BUF_adjustRow(
    LINEBUFFER *buffer
);
PublicFnDecl int BUF_changeRow(
    LINEBUFFER *buffer,
    char *row
);

PublicFnDecl int BUF_backSpace(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);
PublicFnDecl int BUF_carrReturn(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);
PublicFnDecl int BUF_insertString(
    LINEBUFFER *buffer,
    char       *str,
    CUR_WINDOW *win
);
PublicFnDecl int BUF_insertChar(
    LINEBUFFER *buffer,
    int c,
    CUR_WINDOW *win
);
PublicFnDecl int BUF_deleteChar(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
);

PublicFnDecl int BUF_searchString(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_searchNext(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_searchPrev(LINEBUFFER *buffer, CUR_WINDOW *win);

PublicFnDecl int BUF_forwardWord(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_backwardWord(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_deleteCurrWord(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_deletePrevWord(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_replaceString(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_queryReplaceString(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl int BUF_deleteRegion(
    LINEBUFFER *buffer, char *startrow, int startcol, char *endrow, int endcol
);
PublicFnDecl int BUF_insertRegion(
    LINEBUFFER *tobuffer, CUR_WINDOW *win, LINEBUFFER *frbuffer
);
PublicFnDecl int BUF_appendRegion(
    LINEBUFFER *tobuffer, LINEBUFFER *frbuffer, char *startrow, int startcol, char *endrow, int endcol
);

PublicFnDecl int BUF_printBuffer (LINEBUFFER *buf, PAGE *page, int defaultPrinter);
PublicFnDecl int BUF_saveFile(LINEBUFFER *buffer, char const *current_file);
PublicFnDecl int BUF_appendFile(LINEBUFFER *buffer, char const *current_file);
PublicFnDecl int BUF_listFile(LINEBUFFER *buffer, char *directory);
PublicFnDecl int BUF_writeFile(LINEBUFFER *buffer, char const *filename);
PublicFnDecl int BUF_appendToFile(LINEBUFFER *buffer, char const *filename);
PublicFnDecl int BUF_writeLastOutput(LINEBUFFER *buffer, char const *filename, char const *mode);
PublicFnDecl void BUF_stripTabs(char const *source, char *dest, int len);
PublicFnDecl int BUF_readFile(LINEBUFFER *buffer, char *current_file);
PublicFnDecl int BUF_getFile(LINEBUFFER *buffer, char *current_file);
PublicFnDecl LINEBUFFER *BUF_readBuffer(char const *filename, int len, int min, int max);

PublicFnDecl PAGE_Action BUF_handler(
    LINEBUFFER *buffer, CUR_WINDOW *win, PAGE_Action actionbuffer
);

PublicFnDecl void BUF_scrollUp1(LINEBUFFER *buffer, CUR_WINDOW *win);
PublicFnDecl void BUF_scrollDown1(LINEBUFFER *buffer, CUR_WINDOW *win);


#endif


/************************************************************************
  buffers.h 2 replace /users/ees/interface
  860730 15:28:16 ees  

 ************************************************************************/
/************************************************************************
  buffers.h 3 replace /users/ees/interface
  860730 16:15:50 ees  

 ************************************************************************/
/************************************************************************
  buffers.h 4 replace /users/lis/frontend
  870202 16:03:09 lis Search commands added

 ************************************************************************/
/************************************************************************
  buffers.h 5 replace /users/lis/frontend
  870204 10:23:02 lis BUF_writeFile removed

 ************************************************************************/
/************************************************************************
  buffers.h 6 replace /users/lis/frontend
  870601 17:10:17 lis BUF_compact and BUF_adjustRow added

 ************************************************************************/
/************************************************************************
  buffers.h 7 replace /users/lis/frontend/M2/sys
  870811 17:03:15 m2 Curses, VAX conversion, and editor fixes/enhancements

 ************************************************************************/
/************************************************************************
  buffers.h 8 replace /users/lis/frontend/M2/sys
  870826 01:18:19 m2 Added Query/Replace and Global Replace for the editor

 ************************************************************************/
/************************************************************************
  buffers.h 9 replace /users/lis/frontend/M2/sys
  870902 16:41:57 m2 Added dbadmin saver

 ************************************************************************/
/************************************************************************
  buffers.h 10 replace /users/lis/frontend/M2/sys
  871116 23:53:20 m2 Added Region upload and download to PC

 ************************************************************************/
/************************************************************************
  buffers.h 11 replace /users/lis/frontend/M2/sys
  880525 18:04:02 m2 Interface form, Paste Buffers, editor enhacncements, etc.

 ************************************************************************/
/************************************************************************
  buffers.h 12 replace /users/lis/frontend/M2/sys
  880602 09:35:26 m2 Typeahead, tabs,  isprint(), casting, and other fixes

 ************************************************************************/
/************************************************************************
  buffers.h 13 replace /users/m2/frontend/sys
  891011 14:17:16 m2 Tab handling, buffer reorganization

 ************************************************************************/
