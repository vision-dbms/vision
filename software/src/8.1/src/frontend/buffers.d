/********************************************************
 *********	buffers.d			*********
 ********* Documentation for this module is     *********
 ********* in doc/frontend/bufferDoc.           *********
 *******************************************************/
#ifndef BUFFERS_D
#define BUFFERS_D

#include "stdcurses.h"
#include "window.h"

#define BUF_MaxPathNameChars	1024

/*******************************************
 ******** LINEBUFFER DATA STRUCTURE ********
 ******************************************/

#define BUF_maxlinesize 1024
#define BUF_MaxScrLines CUR_maxScrLines

typedef enum { BUF_Normal, BUF_Hilight } BUF_Status;

typedef struct {
    int col_zero;	        /**** col displayed on screen col 0     ****/
    char *row;			/**** current row in buffer		****/
    int col;			/**** current col in buffer		****/
    int repaint;		/**** boolean TRUE = repaint screen	****/
    int screen_row;		/**** row on screen where row is located ***/
    char *buffer_row[BUF_MaxScrLines];	
				/**** array of rows on screen		****/
    char *first;		/**** first logical line                ****/
    char *last;			/**** last logical line                 ****/
    int size;			/**** size of line buffer               ****/
    int addsize;		/**** incremental add size		****/
    int maxsize;		/**** maximum buffer size		****/
    int maxlinesize;
    char *m_lines;		/**** array of line buffers             ****/
    BUF_Status status;		/**** multi-purpose flag		****/
    char edit[BUF_maxlinesize]; /**** edit buffer			****/
    int editted;		/**** editted flag			****/
    char *lastoutput;		/**** first line of last output to buf	****/
    char *startr;		/**** start line of marked region	****/
    char *endr;			/**** end line of marked region		****/
    int  startc;		/**** start column of marked region	****/
    int  endc;			/**** end column of marked region	****/
} LINEBUFFER;

#define BUF_status(buf)		    (buf->status)
#define BUF_screenRow(buf)	    (buf->screen_row)
#define BUF_colZero(buf) 	    (buf->col_zero)
#define BUF_row(buf)		    (buf->row)
#define BUF_col(buf)		    (buf->col)
#define BUF_repaintScreen(buf)	    (buf->repaint)
#define BUF_bufferRow(buf, n)	    (buf->buffer_row[n])
#define BUF_firstLine(buf)	    (buf->first)
#define BUF_lastLine(buf)	    (buf->last)
#define BUF_sizeLines(buf)	    (buf->size)
#define BUF_addSize(buf)	    (buf->addsize)
#define BUF_maxSize(buf)	    (buf->maxsize)
#define BUF_maxLineSize(buf)	    (buf->maxlinesize)
#define BUF_LastOutput(buf)	    (buf->lastoutput)
#define BUF_startrow(buf)	    (buf->startr)
#define BUF_endrow(buf)		    (buf->endr)
#define BUF_startcol(buf)	    (buf->startc)
#define BUF_endcol(buf)		    (buf->endc)

#define BUF_grayPtr(buf)	    *((char **)buf->m_lines)
#define BUF_whitePtr(buf)	    *((char **)(buf->m_lines + sizeof(char *)))
#define BUF_top(buf)		    (buf->m_lines + (2 * sizeof(char**)))
#define BUF_bottom(buf)		    (buf->m_lines + buf->size)
#define BUF_whiteSpace(buf)\
	    ((int)(BUF_bottom(buf) - BUF_whitePtr(buf)))

#define BUF_line(ptr)		    (ptr + sizeof(int) + (2 * sizeof(char *)))
#define BUF_lineSize(ptr)	    *((int *)(ptr + (2 * sizeof(char*))))
#define BUF_prevLine(ptr)	    *((char **)(ptr))
#define BUF_nextLine(ptr)	    *((char **)(ptr + sizeof(char *)))

#define BUF_editRow(b)		    (b->edit)
#define BUF_editted(b)		    (b->editted)

#define BUF_rowSize(ptr)\
    (((((2 * sizeof(char *)) + sizeof(int) + (BUF_lineSize(ptr) * sizeof(char)))\
         / sizeof(int)) + 1) * sizeof(int))


/*********************************************************
 *********	Cursor Movement Operations	**********
 ********************************************************/
#define BUF_initScreen(buf, win)\
{\
    if (BUF_row(buf) == NULL)\
    {\
	BUF_screenRow(buf) = 0;\
	if (BUF_firstLine(buf) != NULL)\
	    BUF_changeRow(buf, BUF_firstLine(buf));\
	BUF_setCol(buf, win, 0);\
    }\
    /*BUF_resetScreen(buf, win);*/\
    BUF_paintWindow(buf, win);\
    CUR_wmove(win, 0, 0);\
    CUR_touchwin(win);\
    CUR_wrefresh(win);\
}

#define BUF_moveCursor(buf, win, row, col, i)\
{\
    if (BUF_row(buf) == NULL)\
    {\
        row = 0;\
	col = 0;\
    }\
    else\
    {\
	row = BUF_screenRow(buf);\
	col = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
	col -= BUF_colZero(buf);\
	if (col >= CUR_WIN_cols(win))\
	    col = CUR_WIN_maxx(win);\
    }\
    CUR_wmove(win, row, col);\
}\

#define BUF_checkCursorHLocation(buf, win)\
{\
    /** If the cursor has been moved horizontally outside of the **/\
    /** current window, then a flag is set to repaint the screen.**/\
\
   if ( (BUF_col(buf) < BUF_colZero(buf)) ||\
        (BUF_col(buf) > (BUF_colZero(buf)+CUR_WIN_maxx(win))) )\
            BUF_repaintScreen(buf) = TRUE;\
}


#define BUF_goToTop(buf, win)\
{\
    if ((BUF_row(buf) == BUF_firstLine(buf)) &&\
        (BUF_col(buf) == 0))\
    {\
	ERR_displayError(ERR_TopBuffer);\
    }\
    else if(BUF_row(buf) == BUF_firstLine(buf))\
    {\
    	BUF_setCol(buf, win, 0);\
    }\
    else\
    {\
	BUF_screenRow(buf) = 0;\
	BUF_changeRow(buf, BUF_firstLine(buf));\
	BUF_setCol(buf, win, 0);\
	BUF_repaintScreen(buf) = TRUE;\
    }\
}

#define BUF_goToBottom(buf, win, i)\
{\
    if ((BUF_row(buf) == BUF_lastLine(buf)) &&\
        (BUF_col(buf) == strlen(BUF_editRow(buf))))\
        ERR_displayError(ERR_BottomBuffer);\
    else if(BUF_row(buf) == BUF_lastLine(buf))\
    {\
    	BUF_setCol(buf, win, strlen(BUF_editRow(buf)));\
    }\
    else\
    {\
	BUF_changeRow(buf, BUF_lastLine(buf));\
	BUF_setCol(buf, win, strlen(BUF_editRow(buf)));\
	for( i=0 ; i<CUR_WIN_maxy(win) ; i++ )\
	    if( BUF_bufferRow(buf, i) == BUF_row(buf) )\
	    	break;\
	BUF_screenRow(buf) = i;\
	BUF_repaintScreen(buf) = TRUE;\
    }\
}

#define BUF_goToEOL(buf, length, win)\
{\
     /** go to the end of the line **/\
     BUF_setCol(buf, win, length);\
}

#define BUF_goToBOL(buf, win)\
{\
     /** go to the beginning of the line **/\
     BUF_setCol(buf, win, 0);\
}


#define BUF_keyDown(buf, len, win)\
{\
    int	co, osc, nsc;\
    if (BUF_row(buf) == NULL || BUF_row(buf) == BUF_lastLine(buf))\
	ERR_displayError(ERR_BottomBuffer);\
    else\
    {\
    	osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
	BUF_changeRow(buf, BUF_nextLine(BUF_row(buf)));\
	if (BUF_screenRow(buf) == CUR_WIN_maxy(win))\
		/** At bottom line of window - move window down **/\
		/** one line.                                   **/\
	    BUF_repaintScreen(buf) = TRUE;\
	else\
	    BUF_screenRow(buf)++;\
	co = BUF_getColFromSCol(buf, BUF_row(buf), osc, &nsc);\
	BUF_setCol(buf, win, co);\
    }\
}

#define BUF_keyUp(buf, len, win)\
{\
    int	co, osc, nsc;\
    if (BUF_row(buf) == NULL || BUF_row(buf) == BUF_firstLine(buf))\
        ERR_displayError(ERR_TopBuffer);\
    else\
    {\
    	osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
	BUF_changeRow(buf, BUF_prevLine(BUF_row(buf)));\
	if (BUF_screenRow(buf) == 0)\
		/** At bottom line of window - move window down **/\
		/** one line.                                   **/\
	    BUF_repaintScreen(buf) = TRUE;\
	else\
	    BUF_screenRow(buf)--;\
	co = BUF_getColFromSCol(buf, BUF_row(buf), osc, &nsc);\
	BUF_setCol(buf, win, co);\
    }\
}

#define BUF_keyLeft(buf, len, win)\
{\
        /** At top left of buffer - can't move left    **/\
    if ( (BUF_row(buf) == NULL) || (BUF_row(buf) == BUF_firstLine(buf) && BUF_col(buf) == 0) )\
	    ERR_displayError(ERR_TopBuffer);\
\
	/** At top left of window - cursor put at last **/\
	/** character of previous line and screen is   **/\
	/** reset.                                     **/\
    else if (BUF_screenRow(buf) == 0 && BUF_col(buf) == 0)\
    {\
	BUF_changeRow(buf, BUF_prevLine(BUF_row(buf)));\
	BUF_setCol(buf, win, strlen(BUF_editRow(buf)));\
	BUF_repaintScreen(buf) = TRUE;\
    }\
\
	/** At beginning of line - cursor put at last  **/\
        /** character of previous line and screen is   **/\
	/** reset if cursor is now in a different      **/\
	/** window.                                    **/\
    else if (BUF_col(buf) == 0)\
    {\
	BUF_changeRow(buf, BUF_prevLine(BUF_row(buf)));\
	BUF_screenRow(buf)--;\
	BUF_setCol(buf, win, strlen(BUF_editRow(buf)));\
    }\
    else\
    {\
	BUF_setCol(buf, win, (BUF_col(buf)-1));\
    }\
}

#define BUF_keyRight(buf, len, win)\
{\
\
          /** At bottom right end of buffer - can't move right **/\
    if( (BUF_row(buf) == NULL) || ((BUF_row(buf) == BUF_lastLine(buf) && BUF_col(buf) == len)) )\
        ERR_displayError(ERR_BottomBuffer);\
\
    else\
          /** At bottom right of last line in window - cursor **/\
          /** is put at column 0 on next line and screen is   **/\
          /** reset.                                          **/\
    if (BUF_screenRow(buf) == CUR_WIN_maxy(win) && BUF_col(buf) == len)\
    {\
	BUF_changeRow(buf, BUF_nextLine(BUF_row(buf)));\
	BUF_setCol(buf, win,  0);\
	BUF_repaintScreen(buf) = TRUE;\
    }\
\
         /** At end of line - cursor is put at column 0 on next **/\
         /** line and screen is reset if cursor is now in a     **/\
         /** different window.                                  **/\
    else if (BUF_col(buf) == len)\
    {\
	BUF_changeRow(buf, BUF_nextLine(BUF_row(buf)));\
	BUF_screenRow(buf)++;\
	BUF_setCol(buf, win, 0);\
    }\
    else\
    {\
 	BUF_setCol(buf, win, (BUF_col(buf)+1));\
    }\
}


#define BUF_scrollDown(buf, win, i)\
{\
    int	osc, co, nsc, ocz;\
    if (BUF_row(buf) != BUF_lastLine(buf))\
    {\
    	ocz = BUF_colZero(buf);\
	osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
	for(i = 0; i < CUR_WIN_rows(win); i++)\
	{\
	    if (BUF_nextLine(BUF_row(buf)) == NULL) break;\
	    BUF_changeRow(buf, BUF_nextLine(BUF_row(buf)));\
	}\
	co = BUF_getColFromSCol(buf, BUF_row(buf), osc, &nsc);\
	BUF_colZero(buf) = ocz;\
	BUF_setCol(buf, win, co);\
	BUF_repaintScreen(buf) = TRUE;\
    }\
    else\
	ERR_displayError(ERR_BottomBuffer);\
}

#define BUF_scrollUp(buf, win)\
{\
    int	osc, co, nsc, ocz;\
    if (BUF_bufferRow(buf, 0) != BUF_firstLine(buf))\
    {\
    	ocz = BUF_colZero(buf);\
	osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
	for(i = 0; i < CUR_WIN_rows(win); i++)\
	{\
	    if (BUF_prevLine(BUF_row(buf)) == NULL) break;\
	    BUF_changeRow(buf, BUF_prevLine(BUF_row(buf)));\
	    if (BUF_row(buf) == BUF_firstLine(buf)) break;\
	}\
	co = BUF_getColFromSCol(buf, BUF_row(buf), osc, &nsc);\
	BUF_colZero(buf) = ocz;\
	BUF_setCol(buf, win, co);\
	BUF_repaintScreen(buf) = TRUE;\
    }\
    else\
	ERR_displayError(ERR_TopBuffer);\
}


#define BUF_scrollRight(buf, win, i)\
{\
    int	co, osc, nsc;\
    i = strlen(BUF_editRow(buf));\
    if (BUF_col(buf) >= i)\
    {\
        ERR_displayError(ERR_EndOfLine);\
	break;\
    }\
    osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
    i = (CUR_WIN_maxx(win)/(2*RS_TabSpacing))*RS_TabSpacing;\
    co = BUF_getColFromSCol(buf, BUF_row(buf), (osc+i), &nsc);\
    BUF_colZero(buf) += i;\
    BUF_setCol(buf, win, co);\
    BUF_repaintScreen(buf) = TRUE;\
}

#define BUF_scrollLeft(buf, win)\
{\
    int	i, co, osc, nsc;\
    if (BUF_colZero(buf) == 0)\
    {\
        ERR_displayError(ERR_BegOfLine);\
	break;\
    }\
    osc = BUF_getSColFromCol(buf, BUF_row(buf), BUF_col(buf));\
    i = (CUR_WIN_maxx(win)/(2*RS_TabSpacing))*RS_TabSpacing;\
    if( (osc-i) < 0 )\
    	BUF_setCol(buf, win, 0);\
    else\
    {\
	co = BUF_getColFromSCol(buf, BUF_row(buf), (osc-i), &nsc);\
	BUF_colZero(buf) -= i;\
	if( BUF_colZero(buf) < 0 ) BUF_colZero(buf) = 0;\
	BUF_setCol(buf, win, co);\
    }\
    BUF_repaintScreen(buf) = TRUE;\
}


/*********************************************************
 *********	Edit Operations			**********
 ********************************************************/
#define BUF_deleteEOL(buf, len, win, ptr)\
{\
    if (BUF_row(buf) == NULL)\
        ERR_displayError(ERR_BottomBuffer);\
    if ((strlen(BUF_editRow(buf)) == 0) && (BUF_row(buf) == BUF_lastLine(buf)))\
        ERR_displayError(ERR_BottomBuffer);\
    else if ((BUF_col(buf) == 0) && (BUF_row(buf) != BUF_lastLine(buf)))\
    {\
        BUF_deleteLine(buf, BUF_row(buf));\
	BUF_repaintScreen(buf) = TRUE;\
    }\
    else if (BUF_col(buf) < len)\
    {\
        ptr = BUF_editRow(buf);\
	ptr[BUF_col(buf)] = '\0';\
	BUF_editted(buf) = TRUE;\
	BUF_paintLine(buf, win);\
    }\
    else\
    	BUF_deleteChar(buf, win);\
}

#define BUF_deleteBuffer(buf)\
{\
    if (BUF_sizeLines(buf) > 0)\
        free(buf->m_lines);\
    free(buf);\
    buf = (LINEBUFFER *)NULL;\
}

#endif
