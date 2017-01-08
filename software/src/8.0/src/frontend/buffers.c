/*********************************************************
 *********	buffers.c		******************
 ********************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "buffers.h"
#include "page.h"
#include "keys.h"
#include "edit.h"
#include "print.h"
#include "error.h"
#include "rsInterface.h"

#define SUCCESS 0
#define FAILURE -1

PublicVarDef char SearchString[81];
PublicVarDef char ReplaceString[81];
PrivateVarDef int BUF_wrapSearch = TRUE;
PrivateVarDef int BUF_doingReplace = FALSE; /* Flag used in search procedure */
					    /* to indicate that we are       */
					    /* searching for a string to     */
					    /* replace.			     */
PrivateVarDef int BUF_tempReplace = FALSE;  /* Flag used in search procedure */ 
					    /* to indicate that the string   */
					    /* at the current position was   */
					    /* not replaced so move forward  */
					    /* past the start of the string  */
					    /* so that we don't find it again*/
PrivateVarDef int BUF_replaceWrap = FALSE;  /* Flag used in search procedure */ 
					    /* which indicates that we have  */
					    /* already wrapped around the    */
					    /* buffer 			     */
PrivateVarDef char *ReplaceRow = NULL, *OrigRow = NULL;
PrivateVarDef int  ReplaceCol = 0;


/************************************************
 *********	Forward Declarations	*********
 ************************************************/

PrivateFnDef int BUF_doSearch (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win,
    int				Direction	/* Direction of Search */
);

PrivateFnDef int BUF_doSearchForward (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
);

PrivateFnDef int BUF_doSearchBackward (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
);

PrivateFnDef int BUF_doReplace (
    LINEBUFFER *		buffer,
    WINDOW *			win
);

PrivateFnDef PAGE_Action scrollBuffer (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
);


/*********************************************************
 *********	Window Operations	******************
 ********************************************************/

PublicFnDef int BUF_getColFromSCol(
    LINEBUFFER	*buffer,
    char	*row,
    int		scol,
    int		*nscol
) {
	int	col = 0, tscol = 0, pscol = 0;
	char	*ptr;
	
	if( BUF_row(buffer) == NULL )
	{
		*nscol = 0;
		return(0);
	}
	
	if( BUF_row(buffer) == row )
		ptr = BUF_editRow(buffer);
	else
		ptr = BUF_line(row);
	
	while( (tscol < scol) && (ptr[col] != '\0') )
	{
		pscol = tscol;	
		if( ptr[col++] == '\t' )
			tscol = ((tscol + RS_TabSpacing) / RS_TabSpacing) * RS_TabSpacing;
		else
			tscol++;
	}
	
	if( tscol > scol )
	{
		tscol = pscol;
		col--;
	}
	
	*nscol = tscol;
	return(col);
}

PublicFnDef int BUF_getSColFromCol(
    LINEBUFFER	*buffer,
    char	*row,
    int		col
) {
	int	scol = 0, tcol = 0;
	char	*ptr;
	
	if( BUF_row(buffer) == NULL )
		return(0);
	
	if( BUF_row(buffer) == row )
		ptr = BUF_editRow(buffer);
	else
		ptr = BUF_line(row);
	
	while( (tcol < col) && (ptr[tcol] != '\0') )
	{
		if( ptr[tcol++] == '\t' )
			scol = ((scol + RS_TabSpacing) / RS_TabSpacing) * RS_TabSpacing;
		else
			scol++;
	}
	
	return(scol);
}

PublicFnDef int BUF_recalcCols(
    LINEBUFFER	*buffer,
    CUR_WINDOW	*win
) {
	int	scol;
	
	if( BUF_row(buffer) == NULL )
	{
		BUF_col(buffer) = BUF_colZero(buffer) = 0;
		BUF_repaintScreen(buffer) = TRUE;
		return(TRUE);
	}
	
	scol = BUF_getSColFromCol(buffer, BUF_row(buffer), BUF_col(buffer));
	
	if( scol < BUF_colZero(buffer) )
	{
		BUF_repaintScreen(buffer) = TRUE;
		while( scol < BUF_colZero(buffer) )
			BUF_colZero(buffer) -= RS_TabSpacing;
	}
	if( scol > (BUF_colZero(buffer)+CUR_WIN_maxx(win)) )
	{
		BUF_repaintScreen(buffer) = TRUE;
		while( scol > (BUF_colZero(buffer)+CUR_WIN_maxx(win)) )
			BUF_colZero(buffer) += RS_TabSpacing;
	}
	return(FALSE);
}

PublicFnDef int BUF_setCol(
    LINEBUFFER	*buffer,
    CUR_WINDOW	*win,
    int		col
) {
	int	len;
	
	if( (BUF_row(buffer) == NULL) || (col == 0) )
	{
		BUF_col(buffer) = BUF_colZero(buffer) = 0;
		BUF_repaintScreen(buffer) = TRUE;
		return(TRUE);
	}
		
	len = strlen(BUF_editRow(buffer));
	
	if( col > len )
		col = len;
		
	BUF_col(buffer) = col;

	if( win != NULL )
		return(BUF_recalcCols(buffer, win));
}


/*********************************************************
 *********	Window Operations	******************
 ********************************************************/

PublicFnDef void BUF_resetScreen(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    int i, len;
    char *scrRow, *prev;
    

/***** set array of rows on screen to NULLs *****/
    for (i = 0; i < BUF_MaxScrLines; i++)
        BUF_bufferRow(buffer, i) = NULL;

/*** make sure that cursor is in bounds ***/
    if (BUF_screenRow(buffer) < 0)
        BUF_screenRow(buffer) = 0;
    if (BUF_screenRow(buffer) > CUR_WIN_maxy(win))
        BUF_screenRow(buffer) = CUR_WIN_maxy(win);

/***** determine which buffer row is on screen row 0 *****/
    scrRow = BUF_row(buffer);
    for (i = BUF_screenRow(buffer);
	     (i > 0 && scrRow != NULL); i--)
    {
        if (NULL == (prev = BUF_prevLine(scrRow))) break;
	scrRow = prev;
    }
/*** now that scrRow is set, reset BUF_bufferRow array ***/
    i = 0;
    while (i <= CUR_WIN_maxy(win) && scrRow != NULL)
    {
	BUF_bufferRow(buffer, i) = scrRow;
	if (scrRow == BUF_row(buffer))
	    BUF_screenRow(buffer) = i;
	scrRow = BUF_nextLine(scrRow);
	if (scrRow == NULL) break;
	i++;
    }

    BUF_setCol(buffer, win, BUF_col(buffer));

#if 0
    if (BUF_row(buffer) == NULL)
    {
        BUF_screenRow(buffer) = 0;
        len = 0;
    }
    else
	len = strlen(BUF_editRow(buffer));

/*** make sure that col is in bounds ***/
    if (BUF_col(buffer) > len) 
        BUF_col(buffer) = len;

/*** readjust horizontal margin ***/
    while( BUF_col(buffer) < BUF_colZero(buffer) )
	BUF_colZero(buffer) -= RS_TabSpacing;
    while( BUF_col(buffer) > (BUF_colZero(buffer) + CUR_WIN_maxx(win)) )
	    BUF_colZero(buffer) += RS_TabSpacing;
#endif

/*** set flag to repaint the window ***/    
    BUF_repaintScreen(buffer) = TRUE;
}

PublicFnDef void BUF_paintWindow(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    int i, col, nscol;
    char *ptr;

/*** resetScreen needs to be called to be sure parameters are set ***/
    BUF_resetScreen(buffer, win);
    
/*** blank out window ***/
    CUR_werase(win);

/*** fill the window ***/
    i = 0;
    while (BUF_bufferRow(buffer, i) != NULL && i < CUR_WIN_rows(win))
    {
	if (BUF_bufferRow(buffer, i) == BUF_row(buffer))
	    ptr = BUF_editRow(buffer);
	else
	    ptr = BUF_line(BUF_bufferRow(buffer, i));
	col = BUF_getColFromSCol(buffer, BUF_bufferRow(buffer, i), BUF_colZero(buffer), &nscol);
	if (nscol >= BUF_colZero(buffer))
	{
	    CUR_wmove(win, i, 0);
	    ERR_paintLineASCII( &ptr[col],
	    			CUR_WIN_cols(win),
	    			win,
	    			TRUE,
	    			CUR_A_REVERSE );
	}
	else
	{
	    CUR_wmove(win, i, 0);
	    CUR_wprintw(win, "%-*.*s", 
		CUR_WIN_cols(win), CUR_WIN_cols(win), "  ");
	}
	i++;
    }

    BUF_repaintScreen(buffer) = FALSE;
}

PublicFnDef void BUF_paintLine(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    int col, nscol;
    char *ptr;

/*** print the edit buffer on the current screen line ***/
    ptr = BUF_editRow(buffer);

    col = BUF_getColFromSCol(buffer, BUF_row(buffer), BUF_colZero(buffer), &nscol);

    if (nscol >= BUF_colZero(buffer))
    {
	CUR_wmove(win, BUF_screenRow(buffer), 0);
	ERR_paintLineASCII( &ptr[col],
			    CUR_WIN_cols(win),
	    		    win,
	    		    TRUE,
	    		    CUR_A_REVERSE );
    }
    else
    {
	CUR_wmove(win, BUF_screenRow(buffer), 0);
	CUR_wprintw(win, "%-*.*s", 
	    CUR_WIN_cols(win), CUR_WIN_cols(win), "   ");
    }
    
}



/*********************************************************
 *********	Buffer Operations	******************
 ********************************************************/

#define initBufferVariables(buffer)\
{\
    BUF_whitePtr(buffer) = BUF_grayPtr(buffer) = BUF_top(buffer);\
    BUF_firstLine(buffer) = NULL;\
    BUF_lastLine(buffer) = NULL;\
    BUF_row(buffer) = NULL;\
    BUF_LastOutput(buffer) = NULL;\
    BUF_screenRow(buffer) = 0;\
    BUF_colZero(buffer) = 0;\
    BUF_col(buffer) = 0;\
    BUF_repaintScreen(buffer) = TRUE;\
    for (i = 0; i < BUF_MaxScrLines; i++)\
        BUF_bufferRow(buffer, i) = NULL;\
    ED_unsetMarkers(buffer);\
}


PublicFnDef int BUF_initBuffer(
    LINEBUFFER *buffer,
    int size
) {
    int i;

    /*if ((int)(buffer->m_lines = memallc (-1, 				  */
    /*					0, 				  */
    /*					min, 				  */
    /*					max, 				  */
    /*					MEM_PRIVATE | MEM_PAGED | MEM_DATA,*/ 
    /*					MEM_R | MEM_W)) < 0)		  */

    if ( NULL == (buffer->m_lines = malloc(size)) )
    {
	return ERR_MallocError;
    }

    BUF_sizeLines(buffer) = BUF_maxSize(buffer) = size;

    initBufferVariables(buffer);

    return SUCCESS;
}

/************************************************
*
*	Arguments:	LINEBUFFER pointer
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_eraseBuffer(LINEBUFFER *buffer) {
    int i;

    initBufferVariables(buffer);
    return SUCCESS;
}


/*********************************************************
 *********	Line Operations		******************
 ********************************************************/

/**** The following temporary pointers are used by any procedure which 
 **** contains a row pointer to the buffer and wants to preserve it AFTER
 **** a call to BUF_getLine() (which MAY call BUF_compact()).  If the buffer
 **** is compacted then all old pointers are invalid.  An example calling 
 **** sequence is:
 ****  	BUF_tempRow = oldrow;
 ****  	newrow = BUF_getLine(buffer, len);
 ****  	oldrow = BUF_tempRow;
 ****	BUF_tempRow = NULL;
 **** Also, a call to BUF_changeRow() uses the first two temporary pointers so
 **** if you need to save a pointer before calling this procedure then use
 **** a new variable like BUF_tempRow3.
 ****/

PrivateVarDef char *BUF_tempRow = NULL;	 /* Used in BUF_changeRow() */
PrivateVarDef char *BUF_tempRow2 = NULL; /* Used in BUF_adjustRow() */
PrivateVarDef char *BUF_tempRow3 = NULL; /* Used in others functions */
PrivateVarDef char *BUF_tempRow4 = NULL; /* Used in deleteRegion */

/************************************************
*
*	Arguments:	LINEBUFFER pointer
*
*	Function: 	Compacts buffer if possible
*
*	Returns:	NULL
*
************************************************/
PublicFnDef void BUF_compact(LINEBUFFER *buffer) {
   char *start, *end, *prevLine, *nextLine, *ptr;
   int	len;

    /**** if there is gray space, compact it ****/
    if (BUF_grayPtr(buffer) < BUF_whitePtr(buffer))
    {
	ERR_displayStr("Compacting Buffer...",FALSE);

        /*** start = first nonblank line after gray space ***/
	start = BUF_grayPtr(buffer);
	while (start < BUF_whitePtr(buffer))
	{
	    /* Move start to the first line which contains real information. */
	    /* After while loop, the space between BUF_grayPtr(buffer) and   */
	    /* start will be a contiguous empty space.			     */
	    while (BUF_prevLine(start) == NULL && BUF_nextLine(start) == NULL
	       && start < BUF_whitePtr(buffer))
		start += BUF_rowSize(start);
	    if (start >= BUF_whitePtr(buffer))
	    {
		BUF_whitePtr(buffer) = BUF_grayPtr(buffer);
		break;
	    }

	    /* Since we use memcpy(), pointers of the linked lists must */
	    /* be changed after each line is moved.  Also, the tempRow, */
	    /* firstLine, lastLine, and row pointers must be adjusted   */
	    /* in the buffer structure if one of these lines was moved. */
	    end = start;
	    while((BUF_prevLine(end) != NULL || BUF_nextLine(end) != NULL)
	       && end < BUF_whitePtr(buffer))
	    {
		prevLine = BUF_prevLine(end);
		nextLine = BUF_nextLine(end);
		len = BUF_rowSize(end);
		end += len;
		ptr = BUF_grayPtr(buffer);
		memcpy(ptr, start, len);
		BUF_prevLine(ptr) = prevLine;
		BUF_nextLine(ptr) = nextLine;
		if( BUF_tempRow == start )
			BUF_tempRow = ptr;
		if( BUF_tempRow2 == start )
			BUF_tempRow2 = ptr;
		if( BUF_tempRow3 == start )
			BUF_tempRow3 = ptr;
		if( BUF_tempRow4 == start )
			BUF_tempRow4 = ptr;
		if( BUF_row(buffer) == start )
			BUF_row(buffer) = ptr;
		if( BUF_LastOutput(buffer) == start )
			BUF_LastOutput(buffer) = ptr;
		if( BUF_startrow(buffer) == start )
			BUF_startrow(buffer) = ptr;
		if( BUF_endrow(buffer) == start )
			BUF_endrow(buffer) = ptr;
		if( ReplaceRow == start )
			ReplaceRow = ptr;
		if( OrigRow == start )
			OrigRow = ptr;
		if( nextLine != NULL )
			BUF_prevLine(nextLine) = ptr;
		else
			BUF_lastLine(buffer) = ptr;
		if( prevLine != NULL )
			BUF_nextLine(prevLine) = ptr;
		else
			BUF_firstLine(buffer) = ptr;
		BUF_grayPtr(buffer) += len;
		start = end;
	    }
	}
	BUF_whitePtr(buffer) = BUF_grayPtr(buffer);
	ERR_clearMsg();
    }
}



/************************************************
*
*	Arguments:	LINEBUFFER pointer
*
*	Returns:	ptr to line of size "len"
*
************************************************/
PublicFnDef char *BUF_getLine(
    LINEBUFFER *buffer,
    int len
) {
    char *ptr;
    int need, diff, i;
    int dotemp = FALSE, dotemp2 = FALSE, dotemp3 = FALSE, dotemp4 = FALSE, 
    	dolast = FALSE, doreprow = FALSE, doorigrow = FALSE, dostart = FALSE, doend = FALSE;

    /**** need = sizeof(prev *) + sizeof(next *) + sizeof(int) + sizeof(text+1) ****/
    /****        rounded to the nearest word                                    ****/

    need = ((((2 * sizeof(char *)) + sizeof(int) + ((len + 1) * sizeof(char)))
		/ sizeof(int)) + 1) * sizeof(int);

    if (need <= BUF_whiteSpace(buffer))     /* take line from end,  */
    { 					    /* adjust white ptr.    */
	ptr = BUF_whitePtr(buffer);
        BUF_whitePtr(buffer) += need;
	if (BUF_grayPtr(buffer) == ptr)
	     BUF_grayPtr(buffer) = BUF_whitePtr(buffer);
	BUF_lineSize(ptr) = len + 1;
	return(ptr);
    }
    BUF_compact(buffer);   /*  Compact creates faulty pointers in the linked */
                             /** list of lines.                            **/

    /**** if free space is still less than needed space, ****/
    /**** then generate error message                    ****/
    if (BUF_whiteSpace(buffer) < need)
    {
    	if( (BUF_tempRow >= BUF_top(buffer)) && (BUF_tempRow < BUF_bottom(buffer)) )
    		dotemp = TRUE;
    	if( (BUF_tempRow2 >= BUF_top(buffer)) && (BUF_tempRow2 < BUF_bottom(buffer)) )
    		dotemp2 = TRUE;
    	if( (BUF_tempRow3 >= BUF_top(buffer)) && (BUF_tempRow3 < BUF_bottom(buffer)) )
    		dotemp3 = TRUE;
    	if( (BUF_tempRow4 >= BUF_top(buffer)) && (BUF_tempRow4 < BUF_bottom(buffer)) )
    		dotemp4 = TRUE;
    	if( (BUF_LastOutput(buffer) >= BUF_top(buffer)) && (BUF_LastOutput(buffer) < BUF_bottom(buffer)) )
    		dolast = TRUE;
    	if( (BUF_startrow(buffer) >= BUF_top(buffer)) && (BUF_startrow(buffer) < BUF_bottom(buffer)) )
    		dostart = TRUE;
    	if( (BUF_endrow(buffer) >= BUF_top(buffer)) && (BUF_endrow(buffer) < BUF_bottom(buffer)) )
    		doend = TRUE;
    	if( (ReplaceRow >= BUF_top(buffer)) && (ReplaceRow < BUF_bottom(buffer)) )
    		doreprow = TRUE;
    	if( (OrigRow >= BUF_top(buffer)) && (OrigRow< BUF_bottom(buffer)) )
    		doorigrow = TRUE;
	ERR_displayStr("Trying to reallocate memory.  Please wait...",FALSE);
	ptr = buffer->m_lines;
	if ( NULL == (buffer->m_lines = realloc(buffer->m_lines,(BUF_maxSize(buffer)*2))) )
	{
	    buffer->m_lines = ptr;
	    ERR_displayPause("Unable to obtain more memory");
	    ERR_displayStr("Please wait...",FALSE);
	    return(NULL);
	}

	BUF_sizeLines(buffer) = BUF_maxSize(buffer) = BUF_maxSize(buffer)*2;
	diff = (int)(buffer->m_lines - ptr);
	if( dotemp && (BUF_tempRow != NULL) )
	    BUF_tempRow += diff;
	if( dotemp2 && (BUF_tempRow2 != NULL) )
	    BUF_tempRow2 += diff;
	if( dotemp3 && (BUF_tempRow3 != NULL) )
	    BUF_tempRow3 += diff;
	if( dotemp4 && (BUF_tempRow4 != NULL) )
	    BUF_tempRow4 += diff;
	if( BUF_row(buffer) != NULL )
	    BUF_row(buffer) += diff;
	if( BUF_firstLine(buffer) != NULL )
	    BUF_firstLine(buffer) += diff;
	if( BUF_lastLine(buffer) != NULL )
	    BUF_lastLine(buffer) += diff;
	if( dolast && (BUF_LastOutput(buffer) != NULL) )
	    BUF_LastOutput(buffer) += diff;
	if( dostart && (BUF_startrow(buffer) != NULL) )
	    BUF_startrow(buffer) += diff;
	if( doend && (BUF_endrow(buffer) != NULL) )
	    BUF_endrow(buffer) += diff;
	if( doreprow && (ReplaceRow != NULL) )
	    ReplaceRow += diff;
	if( doorigrow && (OrigRow != NULL) )
	    OrigRow += diff;
	if( BUF_whitePtr(buffer) != NULL )
	    BUF_whitePtr(buffer) += diff;
	if( BUF_grayPtr(buffer) != NULL )
	    BUF_grayPtr(buffer) += diff;
	for (i = 0; i < BUF_MaxScrLines; i++)
	    if( BUF_bufferRow(buffer, i) != NULL )
		BUF_bufferRow(buffer, i) += diff;
	ptr = BUF_firstLine(buffer);
	while( ptr != NULL /*&& ptr < BUF_whitePtr(buffer)*/ )
	{
	    if( BUF_prevLine(ptr) != NULL )
		BUF_prevLine(ptr) += diff;
	    if( BUF_nextLine(ptr) != NULL )
		BUF_nextLine(ptr) += diff;
#if 0
	    i = BUF_rowSize(ptr);
	    ptr += i;
#endif
	    ptr = BUF_nextLine(ptr);
	}
	ERR_displayStr("Continuing...",FALSE);
	return(BUF_getLine(buffer,len));
    }

    ptr = BUF_whitePtr(buffer);     /** get line from end of text **/
    BUF_whitePtr(buffer) += need;   /** adjust white pointer.     **/
    if (BUF_grayPtr(buffer) == ptr)
	BUF_grayPtr(buffer) = BUF_whitePtr(buffer);
    BUF_lineSize(ptr) = len + 1;
    return(ptr);
}

/************************************************
*
*	Function:	Takes buffer and string and adds a line
*			to the end of the logical list of lines
*                       Leave cursor on new line.
*
*	Arguments:	LINEBUFFER pointer and string
*
*	Returns:	FAILURE or index of row added
*
************************************************/
PublicFnDef char *BUF_appendLine(
    LINEBUFFER *buffer,
    char *str
) {
    int len;
    char *row;
     
    if( BUF_adjustRow(buffer) == ERR_Memory )
    	return(NULL);
    if (str == NULL)
        len = 0;
    else
	len = strlen(str);
    if (len > 0)
    {
	if (str[len - 1] == '\n')
	{
	    str[len - 1] = '\0';
	    len--;
	}
    }

    if (len > BUF_maxLineSize(buffer))
    {
        len = BUF_maxLineSize(buffer);
	str[len] = '\0';
    }
	
    row = BUF_getLine(buffer, len);
    if (NULL == row)
	return NULL;

    if (len == 0)
        BUF_line(row)[0] = '\0';
    else
	strcpy(BUF_line(row), str);     /** put str into new row **/

    if (BUF_row(buffer) == NULL)  /** this is the first line to **/
    {					/** be put in the buffer.     **/
	BUF_firstLine(buffer) = row;
	BUF_prevLine(row) =  NULL;
    }
    else
    {
	BUF_nextLine(BUF_lastLine(buffer)) = row;   /** append row to end **/
	BUF_prevLine(row) =  BUF_lastLine(buffer);  /** of buffer lines.  **/
    }
    
    BUF_nextLine(row) = NULL;		/** adjust pointers **/
    BUF_lastLine(buffer) = row;

    if (BUF_row(buffer) == NULL)	/** leave cursor on row just added **/
        BUF_changeRow(buffer, row);
	
    return (row);
}


/************************************************
*
*	Function:	Takes a buffer and string and inserts a line
*			into the logical list of lines before "before"
*
*	Arguments:	LINEBUFFER pointer, row to insert before, string
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef char *BUF_insertLine(
    LINEBUFFER *buffer,
    char *before,
    char *str
) {
    int len;
    char *row;

    if( BUF_adjustRow(buffer) == ERR_Memory )
    	return(NULL);
    if (str == NULL)
        len = 0;
    else
	len = strlen(str);
    if (len > 0)
    {
	if (str[len - 1] == '\n')
	{
	    str[len - 1] = '\0';
	    len--;
	}
    }

    if (len > BUF_maxLineSize(buffer))
    {
        len = BUF_maxLineSize(buffer);
	str[len] = '\0';
    }
	
    BUF_tempRow = before;
    if (NULL == (row = BUF_getLine(buffer, len)))
	return (NULL);
    before = BUF_tempRow; BUF_tempRow = NULL;

    if (len == 0)
        BUF_line(row)[0] = '\0';
    else
	strcpy(BUF_line(row), str);
    BUF_prevLine(row) = BUF_prevLine(before);
    if (BUF_prevLine(row) == NULL)
        BUF_firstLine(buffer) = row;
    else
	BUF_nextLine(BUF_prevLine(row)) = row;
    BUF_prevLine(before) = row;
    BUF_nextLine(row) = before;
    if (BUF_nextLine(row) == NULL)
        BUF_lastLine(buffer) = row;
    return (row);
}

/************************************************
*
*	Function:	Deletes a line from the logical list of lines
*			Leave cursor at same column position in 
*			next line.
*
*	Arguments:	LINEBUFFER pointer, row to delete
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_deleteLine(
    LINEBUFFER *buffer,
    char *bufrow
) {
    if (bufrow == BUF_row(buffer))
	BUF_editted(buffer) = FALSE;

    if (bufrow == NULL)
        return (ERR_InvalidRow);
    else 
    if (bufrow == BUF_firstLine(buffer) && bufrow == BUF_lastLine(buffer))
    {
	BUF_eraseBuffer(buffer);
    }
    else
    {
	if (bufrow == BUF_firstLine(buffer))
	{
	    BUF_firstLine(buffer) = BUF_nextLine(bufrow);
	    BUF_prevLine(BUF_nextLine(bufrow)) =
				     BUF_prevLine(bufrow);
	    if (BUF_row(buffer) == bufrow)
	    {
		BUF_changeRow(buffer, BUF_nextLine(bufrow));
		BUF_setCol(buffer, (CUR_WINDOW *)NULL, 0);
	    }
	}
	else
	if (bufrow == BUF_lastLine(buffer))
	{
	    BUF_lastLine(buffer) = BUF_prevLine(bufrow);
	    BUF_nextLine(BUF_prevLine(bufrow)) =
				     BUF_nextLine(bufrow);
	    if (BUF_row(buffer) == bufrow)
	    {
		BUF_changeRow(buffer, BUF_prevLine(bufrow));
		BUF_setCol(buffer, (CUR_WINDOW *)NULL, 0);
	    }
	}
	else
	{
	    BUF_prevLine(BUF_nextLine(bufrow)) =
				     BUF_prevLine(bufrow);
	    BUF_nextLine(BUF_prevLine(bufrow)) =
				     BUF_nextLine(bufrow);
	    if (BUF_row(buffer) == bufrow)
	    {
		BUF_changeRow(buffer, BUF_nextLine(bufrow));
		BUF_setCol(buffer, (CUR_WINDOW *)NULL, 0);
	    }
	}
    }

    BUF_prevLine(bufrow) = BUF_nextLine(bufrow) = NULL;
    if( bufrow == BUF_LastOutput(buffer) )
    	BUF_LastOutput(buffer) = NULL;
    if( bufrow == BUF_startrow(buffer) || bufrow == BUF_endrow(buffer) )
    {
    	ED_unsetMarkers(buffer);
	ERR_displayMsg(ERR_ClearMarkers);
    }
    if (bufrow < BUF_grayPtr(buffer))   
        BUF_grayPtr(buffer) = bufrow;  /** makes grayPtr point to row that **/
 				       /** was just deleted.               **/
    return SUCCESS;
}

/************************************************
*
*	Function:	If the current line has been editted,
*			then updates the appropriate row in
*			lines array so it contains the same
*                       text as editRow.
*
*	Arguments:	LINEBUFFER pointer
*
*	Returns: 	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_adjustRow(
    LINEBUFFER *buffer
) {
    char *new, *old;
    int len, do3 = FALSE, do4 = FALSE, doL = FALSE, doR = FALSE, doO = FALSE;
   
    old = BUF_row(buffer);
    if (old != NULL && BUF_editted(buffer))
    {
	if (old == BUF_startrow(buffer) || old == BUF_endrow(buffer)) {
	    ED_unsetMarkers(buffer);
	    ERR_displayMsg(ERR_ClearMarkers);
	}
	len = strlen(BUF_editRow(buffer));
	if( len < BUF_lineSize(old) )
	    strcpy(BUF_line(old), BUF_editRow(buffer));
	else
	{
	    if (BUF_tempRow3 == old)
		do3 = TRUE;
	    if (BUF_tempRow4 == old)
		do4 = TRUE;
	    if (BUF_LastOutput(buffer) == old)
		doL = TRUE;
	    if (ReplaceRow == old)
		doR = TRUE;
	    if (OrigRow == old)
		doO = TRUE;
	    BUF_tempRow = old;
	    if (NULL == (new = BUF_getLine(buffer, len)))
		return(ERR_Memory);
	    old = BUF_tempRow; BUF_tempRow = NULL;
	    if (do3)
		BUF_tempRow3 = new;
	    if (do4)
		BUF_tempRow4 = new;
	    if (doL)
		BUF_LastOutput(buffer) = new;
	    if (doR)
		ReplaceRow = new;
	    if (doO)
		OrigRow = new;
	    strcpy(BUF_line(new), BUF_editRow(buffer));

/****  set pointer in array of pointers on screen to new pointer ****/
	    BUF_bufferRow(buffer, BUF_screenRow(buffer)) = new;

	    BUF_prevLine(new) = BUF_prevLine(old);
	    if (BUF_prevLine(new) == NULL)
		BUF_firstLine(buffer) = new;
	    else
		BUF_nextLine(BUF_prevLine(new)) = new;

	    BUF_nextLine(new) = BUF_nextLine(old);
	    if (BUF_nextLine(new) == NULL)
		BUF_lastLine(buffer) = new;
	    else
		BUF_prevLine(BUF_nextLine(new)) = new;

	    BUF_prevLine(old) = BUF_nextLine(old) = NULL;
	    if (old < BUF_grayPtr(buffer))
		BUF_grayPtr(buffer) = old;   /** moves gray ptr to empty row **/
	    BUF_row(buffer) = new;
	}  /** else **/
    }
    BUF_editted(buffer) = FALSE;

    return SUCCESS;
}



/************************************************
*
*	Function:	Moves to "row" in "buffer".
*			Initializes BUF_editRow to be the
*			contents of BUF_line(row).
*
*	Arguments:	LINEBUFFER pointer, row to move to
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_changeRow(
    LINEBUFFER *buffer,
    char *row
) {
    int err;
    
    if (row == NULL)
    {
        ERR_displayStr("Illegal Row Change!!!", TRUE);
	return(TRUE);
    }

    if (row == BUF_row(buffer))
	return(FALSE);

    BUF_tempRow2 = row;
    if ( (err=BUF_adjustRow(buffer)) != SUCCESS )
	return(err);
    row = BUF_tempRow2; BUF_tempRow2 = NULL;

    BUF_row(buffer) = row;    /** Change rows **/

    if (row != NULL)
	strcpy(BUF_editRow(buffer), BUF_line(BUF_row(buffer)));
    else
        BUF_editRow(buffer)[0] = '\0';

    return(SUCCESS);

}



/****************************************************************
 ************	    Line Edit Operations	*****************
 ***************************************************************/
PublicFnDef int BUF_backSpace(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    char *ptr, buf1[BUF_maxlinesize],
        *prevrow, *bufrow;
    int len1, len2, n;

    BUF_tempRow3 = bufrow = BUF_row(buffer);
    if (bufrow == NULL)    
	return (ERR_TopBuffer);
    else
    if (BUF_col(buffer) == 0 && bufrow == BUF_firstLine(buffer))
    {
	return (ERR_TopBuffer);
    }
    else
    if (BUF_col(buffer) > 0)
    {
        ptr = BUF_editRow(buffer);
	strcpy(buf1, &ptr[BUF_col(buffer)]);
	ptr[BUF_col(buffer) - 1] = '\0';
	strcat(ptr, buf1);
	BUF_setCol(buffer, win, (BUF_col(buffer)-1));
	BUF_editted(buffer) = TRUE;
	BUF_paintLine(buffer, win);
    }
    else
    {
	prevrow = BUF_prevLine(bufrow);
	if ((prevrow == BUF_startrow(buffer)) || (prevrow == BUF_endrow(buffer)) ||
	    (bufrow  == BUF_startrow(buffer)) || (bufrow  == BUF_endrow(buffer))) 
	{
	    ED_unsetMarkers(buffer);
	    ERR_displayMsg(ERR_ClearMarkers);
	}
	len1 = strlen(BUF_line(prevrow));
	len2 = strlen(BUF_editRow(buffer));
	if( (len1+len2) <= BUF_maxLineSize(buffer) )
	{
	    strcpy(buf1, BUF_editRow(buffer));  /*** save row ***/
	    BUF_editted(buffer) = FALSE;    /*** don't bother to replace ***/
	    BUF_changeRow(buffer, prevrow);
	    strcat(BUF_editRow(buffer), buf1);
	    BUF_editted(buffer) = TRUE;
	    BUF_deleteLine(buffer, bufrow);
	}
	else
	{
	    return (ERR_FullLine);
	}
	bufrow = BUF_tempRow3;

	if (BUF_bufferRow(buffer, 0) == bufrow)
	{
	    BUF_setCol(buffer, win, len1);
	    BUF_repaintScreen(buffer) = TRUE;
	}
	else
	{
	    BUF_screenRow(buffer)--;
	    BUF_setCol(buffer, win, len1);
	    BUF_repaintScreen(buffer) = TRUE;
	}
    }
    BUF_tempRow3 = NULL;
    return SUCCESS;
}

/************************************************
*
*	Function:	Inserts CR and leaves cursor at 
*			beginning of new line.
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_carrReturn(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    char *new, *ptr;
    
    if (BUF_row(buffer) == NULL)   /** buffer is empty **/
    {
        if (NULL == (new = BUF_appendLine(buffer, NULL)))
	    return(ERR_Memory);
        if (NULL == (new = BUF_appendLine(buffer, NULL)))
	    return(ERR_Memory);
	BUF_changeRow(buffer, new);   /** cursor sits on second line **/
    }				      /** which is blank.            **/
    else if ((size_t)BUF_col(buffer) >= strlen(BUF_editRow(buffer)))
    {
	if (BUF_row(buffer) != BUF_lastLine(buffer))
	{
	    if (NULL == (new = BUF_insertLine(buffer,
			     BUF_nextLine(BUF_row(buffer)), NULL)))
		return(ERR_Memory);
	    BUF_changeRow(buffer, new);
	}
	else
	{
	    if (NULL == (new = BUF_appendLine(buffer, NULL)))
		return(ERR_Memory);
	    BUF_changeRow(buffer, new);
	}
    }
    else    /***** we're in the middle of the line *****/
    {
	if (BUF_row(buffer) != BUF_lastLine(buffer))
	{
	    ptr = BUF_editRow(buffer);
	    if (NULL == (new = BUF_insertLine(buffer,
	     BUF_nextLine(BUF_row(buffer)), &ptr[BUF_col(buffer)])))
		return(ERR_Memory);
	    ptr[BUF_col(buffer)] = '\0';
	    BUF_editted(buffer) = TRUE;
	    BUF_changeRow(buffer, new);
	}
	else
	{
	    ptr = BUF_editRow(buffer);
	    if (NULL == (new = 
	        BUF_appendLine(buffer, &ptr[BUF_col(buffer)])))
		return(ERR_Memory);
	    ptr[BUF_col(buffer)] = '\0';
	    BUF_editted(buffer) = TRUE;
	    BUF_changeRow(buffer, new);
	}
    }

    if ((win != NULL) && (BUF_screenRow(buffer) < CUR_WIN_maxy(win)))
	BUF_screenRow(buffer)++;      /** cursor is 1 row lower on screen **/
    BUF_setCol(buffer, win, 0);	      /** cursor is left at beg. of new line **/
    BUF_repaintScreen(buffer) = TRUE;

    return SUCCESS;
}

/************************************************
*
*	Function:	Inserts "str" at cursor's current location
*			in "buffer".  Cursor is left just to the 
*			right of "str".
*
*	Arguments:	LINEBUFFER pointer, String to be inserted, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_insertString(
    LINEBUFFER *buffer,
    char       *str,
    CUR_WINDOW *win
) {
    char *ptr, buf1[BUF_maxlinesize], *new;

    if (BUF_row(buffer) == NULL)	/** first character in buffer **/
    {
	if (NULL == (new = BUF_appendLine(buffer, str)))
	    return(ERR_Memory);
	BUF_editted(buffer) = FALSE;
	BUF_changeRow(buffer, new);
    }
    else	
    if (strlen(BUF_editRow(buffer)) + strlen(str) >
				     (size_t)BUF_maxLineSize(buffer))
        return (ERR_FullLine);
    else
    {
	ptr = BUF_editRow(buffer);		/** ptr = complete line **/
	strcpy(buf1, &ptr[BUF_col(buffer)]);    /** buf1 = points to current **/
	ptr[BUF_col(buffer)] = '\0';   		/**     position.            **/
	strcat(ptr, str);
	strcat(ptr, buf1);                      /** ptr = ptr | str | buf1 **/
	BUF_editted(buffer) = TRUE;
    }
    BUF_setCol(buffer, win, (BUF_col(buffer)+strlen(str)));
    if( win != NULL )
    {
	BUF_paintLine(buffer, win);
    }
    else
    	BUF_repaintScreen(buffer) = TRUE;
    return SUCCESS;
}

/************************************************
*
*	Function:	Inserts character "c" at cursor's current location
*			in "buffer".  Cursor is left just to the 
*			right of "c".
*
*	Arguments:	LINEBUFFER pointer, Character to be inserted, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PublicFnDef int BUF_insertChar(
    LINEBUFFER *buffer,
    int c,
    CUR_WINDOW *win
) {
    char *ptr, buf1[BUF_maxlinesize], *new;

    if (BUF_row(buffer) == NULL)   	/** put first character into buffer **/
    {
	buf1[0] = c;
	buf1[1] = '\0';
	if (NULL == (new = BUF_appendLine(buffer, buf1)))
	    return(ERR_Memory);
	
	BUF_changeRow(buffer, new);
	BUF_setCol(buffer, win, (BUF_col(buffer)+1));
    }
    else
    {
	if (strlen(BUF_editRow(buffer)) + 1 >
				 (size_t)BUF_maxLineSize(buffer))
	{
	    return (ERR_FullLine);
	}
	else		/** insert character into middle of buffer **/
	{
	    ptr = BUF_editRow(buffer);
	    strcpy(buf1, &ptr[BUF_col(buffer)]);
	    ptr[BUF_col(buffer)] = c;
	    ptr[BUF_col(buffer) + 1] = '\0';
	    strcat(ptr, buf1);
	    BUF_editted(buffer) = TRUE;
	    BUF_setCol(buffer, win, (BUF_col(buffer)+1));
	}
    }
    BUF_paintLine(buffer, win);
    return SUCCESS;
}

PublicFnDef int BUF_deleteChar(
    LINEBUFFER *buffer,
    CUR_WINDOW *win
) {
    char *ptr, buf1[BUF_maxlinesize];
    int	len, error;

    if (BUF_row(buffer) == NULL)
	return (ERR_BottomBuffer);
    len = strlen(BUF_editRow(buffer));
    if ((BUF_row(buffer) == BUF_lastLine(buffer)) && (BUF_col(buffer) == len)) {
	if (len == 0)
	    return (BUF_backSpace(buffer, win));
	else
	    return SUCCESS;
    }
    else if (BUF_col(buffer) == len) {
	BUF_keyRight(buffer, len, win);
	error = BUF_backSpace(buffer, win);
	if( error == ERR_FullLine )
	    BUF_keyLeft(buffer, len, win);
	return(error);
    }
	
    ptr = BUF_editRow(buffer);
    strcpy(buf1, &ptr[BUF_col(buffer) + 1]);
    ptr[BUF_col(buffer)] = '\0';   /** blank current position **/
    strcat(ptr, buf1);
    BUF_editted(buffer) = TRUE;
    BUF_paintLine(buffer, win);
    return SUCCESS;
}


/****************************************************************
 ************	    Search Operations		*****************
 ***************************************************************/

PublicFnDef int BUF_searchString(buffer,win)
/************************************************
*
*	Function:	Searches for the SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
CUR_WINDOW *win;
{

int Direction = 1;  /* forward search */
int len;

   BUF_doingReplace = FALSE;
   BUF_tempReplace = FALSE;
   if ( (len = strlen(SearchString)) !=0 )
	memset(SearchString,'\0',len);	  /* blank SearchString */
   if (!ERR_promptForString(" Enter Search String: ", SearchString, FALSE)
        && strlen(SearchString))     	  /* valid string entered  */
        BUF_doSearch( buffer, win, Direction);	

   return SUCCESS;
}


PublicFnDef int BUF_searchNext(buffer,win)
/************************************************
*
*	Function:	Searches for next occurrence 
*			of SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
int Direction = 1;   		/* forward search */

   BUF_doingReplace = FALSE;
   BUF_tempReplace = FALSE;
   ERR_displaySearchStr(SearchString);
   BUF_doSearch(buffer, win, Direction);

return SUCCESS;
}


PublicFnDef int BUF_searchPrev(buffer,win)
/************************************************
*
*	Function:	Searches for previous
*			occurrence of SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
int Direction = -1;		/* backward search */

   BUF_doingReplace = FALSE;
   BUF_tempReplace = FALSE;
   ERR_displaySearchStr(SearchString);
   BUF_doSearch(buffer, win, Direction);

return SUCCESS;
}


/************************************************
*
*	Function:	Searches for a the SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PrivateFnDef int BUF_doSearch (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win,
    int				Direction	/* Direction of Search */
)
{

#define BEEP TRUE

	BUF_wrapSearch = TRUE;
	if( Direction == 1 )
		return (BUF_doSearchForward(buffer,win));
	else
		return (BUF_doSearchBackward(buffer,win));
}


/************************************************
*
*	Function:	Searches for a the SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PrivateFnDef int BUF_doSearchForward (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
)
{

#define NextRow\
		if( row == BUF_lastLine(buffer) && BUF_wrapSearch ) {\
			BUF_replaceWrap = TRUE;\
			BUF_changeRow(buffer,BUF_firstLine(buffer));\
			MiddleOfScreen = TRUE;\
		}\
		else if( row == BUF_lastLine(buffer) && !BUF_wrapSearch )\
		{\
			OrigRow = NULL;\
			return FAILURE;\
		}\
		else\
			BUF_changeRow(buffer,BUF_nextLine(row));\
		if( !MiddleOfScreen )\
		{\
			if( BUF_screenRow(buffer) < CUR_WIN_maxy(win) )\
				BUF_screenRow(buffer)++;\
			else\
				MiddleOfScreen = TRUE;\
		}\
		row = BUF_row(buffer);\
		i = 0;\
		ptr = beginbuf = BUF_editRow(buffer);\
		len = strlen(beginbuf);

	char firstchar, *ptr, *beginbuf;
	int Found = FALSE, Done, NotFirst = FALSE, MiddleOfScreen = FALSE;
	int Slen, len, i;
	char *row;
	int origcol, origscreenrow, origcolzero;

	if (BUF_row(buffer) == NULL) {
	    if ( !BUF_doingReplace )
		ERR_displayStrMsg(SearchString," not found",BEEP);
	    return FAILURE;
	}

	BUF_adjustRow(buffer);
	row = OrigRow = BUF_row(buffer);
	i = origcol = BUF_col(buffer);
	origscreenrow = BUF_screenRow(buffer);	
	origcolzero = BUF_colZero(buffer);	
	firstchar = SearchString[0];
	Slen = strlen(SearchString);

	beginbuf = BUF_editRow(buffer);
	len = strlen(beginbuf);

	if( BUF_doingReplace && BUF_replaceWrap )
		NotFirst = TRUE;
	if( !BUF_tempReplace )
		i++;
	if( i >= len ) {
		NextRow;
		NotFirst = TRUE;
	}
	else {
		ptr = &beginbuf[i];
	}

	do {
	    while( i < len ) {
		if( (ptr = strchr(ptr,firstchar)) == NULL ) {
		    i = BUF_maxlinesize;
		    break;
		}
		i = (int)(ptr - beginbuf);
		if( (Found = (strncmp(ptr,SearchString,Slen) == 0)) )
		    break;
		i++;
		ptr++;
	    }

	    Done = (NotFirst && 
		    ((!BUF_doingReplace && row == OrigRow && i >= origcol) ||
		     (BUF_doingReplace && row == ReplaceRow && i >= ReplaceCol)));
	    NotFirst = TRUE;
	    if( !Found && !Done ) {
		NextRow;
	    }
	} while ( !Found && !Done );

	if ( Found && Done ) {
	    if ( BUF_doingReplace )
	    {
	    	OrigRow = NULL;
		return FAILURE;
	    }
	    else
	    {
	    	BUF_screenRow(buffer) = origscreenrow;
		BUF_colZero(buffer) = origcolzero;
	    	MiddleOfScreen = FALSE;
		ERR_displayStrMsg(SearchString," occurs once",BEEP);
	    }
	}

	if ( Found ) {
	    BUF_setCol(buffer, win, i);
	    if( MiddleOfScreen )
	    {
		BUF_screenRow(buffer) = (CUR_WIN_maxy(win) / 2);
		BUF_repaintScreen(buffer) = TRUE;
	    }
	    OrigRow = NULL;
	}
	else {
	    if ( !BUF_doingReplace )
	    {
		BUF_changeRow(buffer, OrigRow);
		BUF_colZero(buffer) = origcolzero;
		BUF_setCol(buffer, win, origcol);
		BUF_screenRow(buffer) = origscreenrow;
		ERR_displayStrMsg(SearchString," not found",BEEP);
	    }
	    OrigRow = NULL;
	    BUF_repaintScreen(buffer) = TRUE;
	    return FAILURE;
	}

        return SUCCESS;
}

PrivateFnDef char *BUF_strrchr(buf,i,c)
char	*buf;
int	i;
char	c;
{
	char	*ptr;
	for( ptr = &buf[i] ; i >= 0 ; i--, ptr-- )
		if( *ptr == c )
			return( ptr );
	return( NULL );
}

/************************************************
*
*	Function:	Searches for a the SearchString
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PrivateFnDef int BUF_doSearchBackward (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
)
{

#define PrevRow\
		if( row == BUF_firstLine(buffer) )\
		{\
			BUF_changeRow(buffer,BUF_lastLine(buffer));\
			MiddleOfScreen = TRUE;\
		}\
		else\
			BUF_changeRow(buffer,BUF_prevLine(row));\
		if( !MiddleOfScreen )\
		{\
			if( BUF_screenRow(buffer) > 0 )\
				BUF_screenRow(buffer)--;\
			else\
				MiddleOfScreen = TRUE;\
		}\
		row = BUF_row(buffer);\
		beginbuf = BUF_editRow(buffer);\
		i = strlen(beginbuf);

	char firstchar, *ptr, *beginbuf;
	int Found = FALSE, Done, NotFirst = FALSE, MiddleOfScreen = FALSE;
	int Slen, i;
	char *row;
	int origcol, origscreenrow, origcolzero;

	if (BUF_row(buffer) == NULL) {
		ERR_displayStrMsg(SearchString," not found",BEEP);
		return FAILURE;
	}

	BUF_adjustRow(buffer);
	row = OrigRow = BUF_row(buffer);
	i = origcol = BUF_col(buffer);
	origscreenrow = BUF_screenRow(buffer);	
	origcolzero = BUF_colZero(buffer);	
	firstchar = SearchString[0];
	Slen = strlen(SearchString);

	beginbuf = BUF_editRow(buffer);

	if( --i < 0 ) {
		PrevRow;
		NotFirst = TRUE;
	}

	do {
	    while( i >= 0 ) {
		if( (ptr = BUF_strrchr(beginbuf,i,firstchar)) == NULL ) {
		    i = -1;
		    break;
		}
		i = (int)(ptr - beginbuf);
		if( (Found = (strncmp(ptr,SearchString,Slen) == 0)) )
		    break;
		i--;
	    }

	    Done = (NotFirst && row == OrigRow && i <= origcol);
	    NotFirst = TRUE;
	    if( !Found && !Done ) {
		PrevRow;
	    }
	} while ( !Found && !Done );

	if ( Found && Done )
	{
	    BUF_screenRow(buffer) = origscreenrow;
	    BUF_colZero(buffer) = origcolzero;
	    MiddleOfScreen = FALSE;
	    ERR_displayStrMsg(SearchString," occurs once",BEEP);
	}

	if ( Found ) {
	    BUF_setCol(buffer, win, i);
	    if( MiddleOfScreen )
	    {
		BUF_screenRow(buffer) = (CUR_WIN_maxy(win) / 2);
		BUF_repaintScreen(buffer) = TRUE;
	    }
	    OrigRow = NULL;
	}
	else {
	    BUF_changeRow(buffer, OrigRow);
	    BUF_colZero(buffer) = origcolzero;
	    BUF_setCol(buffer, win, origcol);
	    BUF_screenRow(buffer) = origscreenrow;
	    ERR_displayStrMsg(SearchString," not found",BEEP);
	    OrigRow = NULL;
	    BUF_repaintScreen(buffer) = TRUE;
	    return FAILURE;
	}

        return SUCCESS;
}

#define charInWord(ch)	(((c = ch) == '_') ||\
			 (c == '#') ||\
			 (isalnum(c)))
 
PublicFnDef int
BUF_forwardWord(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	char *beginbuf;
	int len, i;
	char *row;
	int c, doRepaint = FALSE;

	BUF_adjustRow(buffer);
	if( (row = BUF_row(buffer)) == NULL )
		return(FALSE);
	i = BUF_col(buffer); 
	beginbuf = BUF_editRow(buffer);
	len = strlen(beginbuf);

	for(;;)
	{
		while( (i < len) && !charInWord(beginbuf[i]) )
			i++;
		if( i >= len ) 
		{
			if( BUF_nextLine(row) == NULL )
			{
				BUF_setCol(buffer, win, i);
				if( doRepaint )
					BUF_repaintScreen(buffer) = TRUE;
				return(FALSE);
			}
			if( BUF_screenRow(buffer) < CUR_WIN_maxy(win) )
				BUF_screenRow(buffer)++;
			else
				doRepaint = TRUE;
			BUF_changeRow(buffer,BUF_nextLine(row));
			row = BUF_row(buffer);
			i = 0;
			beginbuf = BUF_editRow(buffer);
			len = strlen(beginbuf);
		}
		else
			break;
	}

	while( (i < len) && charInWord(beginbuf[i]) )
		i++;
	BUF_setCol(buffer, win, i);
	if( doRepaint )
		BUF_repaintScreen(buffer) = TRUE;
	return(FALSE);
}	

PublicFnDef int
BUF_backwardWord(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	char *beginbuf;
	int i;
	char *row;
	int c, doRepaint = FALSE;

	BUF_adjustRow(buffer);
	if( (row = BUF_row(buffer)) == NULL )
		return(FALSE);
	i = BUF_col(buffer); 
	beginbuf = BUF_editRow(buffer);
	if( i > 0 )
		i--;
	else
	{
		if( BUF_prevLine(row) == NULL )
		{
			BUF_setCol(buffer, win, 0);
			if( doRepaint )
				BUF_repaintScreen(buffer) = TRUE;
			return(FALSE);
		}
		if( BUF_screenRow(buffer) > 0 )
			BUF_screenRow(buffer)--;
		else
			doRepaint = TRUE;
		BUF_changeRow(buffer,BUF_prevLine(row));
		row = BUF_row(buffer);
		beginbuf = BUF_editRow(buffer);
		i = strlen(beginbuf);
	}

	for(;;)
	{
		while( (i > 0) && !charInWord(beginbuf[i]) )
			i--;
		if( (i == 0) && !charInWord(beginbuf[i]) ) 
		{
			if( BUF_prevLine(row) == NULL )
			{
				BUF_setCol(buffer, win, 0);
				if( doRepaint )
					BUF_repaintScreen(buffer) = TRUE;
				return(FALSE);
			}
			if( BUF_screenRow(buffer) > 0 )
				BUF_screenRow(buffer)--;
			else
				doRepaint = TRUE;
			BUF_changeRow(buffer,BUF_prevLine(row));
			row = BUF_row(buffer);
			beginbuf = BUF_editRow(buffer);
			i = strlen(beginbuf);
		}
		else
			break;
	}
	while( (i > 0) && charInWord(beginbuf[i]) )
		i--;
	if( !charInWord(beginbuf[i]) && (strlen(beginbuf) > (size_t)0) )
		i++;
	BUF_setCol(buffer, win, i);
	if( doRepaint )
		BUF_repaintScreen(buffer) = TRUE;
	return(FALSE);
}

PublicFnDef int
BUF_deleteCurrWord(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	int	scol, ecol, oldscreenrow, oldcolzero;
	char	*srow, *erow;

	BUF_adjustRow(buffer);
	if( BUF_row(buffer) == NULL )
		return(FALSE);
	scol = BUF_col(buffer);
	srow = BUF_row(buffer);
	oldscreenrow = BUF_screenRow(buffer);
	oldcolzero = BUF_colZero(buffer);
	BUF_forwardWord(buffer,win);
	ecol = BUF_col(buffer);
	erow = BUF_row(buffer);
	BUF_changeRow(buffer,srow);
	BUF_colZero(buffer) = oldcolzero;
	BUF_screenRow(buffer) = oldscreenrow;
	BUF_setCol(buffer, win, scol);
	BUF_deleteRegion(buffer,srow,scol,erow,ecol);
	BUF_repaintScreen(buffer) = TRUE;
	return(FALSE);
}

PublicFnDef int
BUF_deletePrevWord(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	int	scol, ecol;
	char	*srow, *erow;

	BUF_adjustRow(buffer);
	if( BUF_row(buffer) == NULL )
		return(FALSE);
	ecol = BUF_col(buffer);
	erow = BUF_row(buffer);
	BUF_backwardWord(buffer,win);
	scol = BUF_col(buffer);
	srow = BUF_row(buffer);
	BUF_deleteRegion(buffer,srow,scol,erow,ecol);
	BUF_repaintScreen(buffer) = TRUE;
	return(FALSE);
}

PrivateFnDef int promptYesNoAbort(prompt, buffer, win)
char *prompt;
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
    int c, row, col, i;
    
    CUR_werase(ERR_Window);
    CUR_wattron(ERR_Window, CUR_A_BOLD);
    CUR_wmove(ERR_Window, 0, 0);
    CUR_wprintw(ERR_Window, "%s", prompt);
    CUR_wattroff(ERR_Window, CUR_A_BOLD);
    CUR_touchwin(ERR_Window);
    CUR_wrefresh(ERR_Window);
    BUF_moveCursor(buffer, win, row, col, i);
    CUR_wrefresh(win);
    CUR_forceSetCursor(win, row, col);
    c = ' ';
    while (c != 'y' && c != 'Y' && 
	   c != 'n' && c != 'N' &&
	   c != KEY_PREV && c != KEY_QUIT)
    {
	c = KEY_getkey(FALSE);
	KEY_QuoteNextKey = FALSE;
    }

    return(c);
}

PublicFnDef int BUF_replaceString(buffer,win)
/************************************************
*
*	Function:	Searches for the SearchString and replaces 
*			with ReplaceString (NOT prompted by user each time)
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
CUR_WINDOW *win;
{

int len, count = 0, origscreenrow, origcolzero;
char msg[80];

   BUF_doingReplace = TRUE;
   BUF_tempReplace = TRUE;
   BUF_replaceWrap = FALSE;
   if (BUF_row(buffer) == NULL)
   {
	ERR_displayStr("Buffer empty",TRUE);
	return SUCCESS;
   }
   if ( (len = strlen(SearchString)) !=0 )
	memset(SearchString,'\0',len);	  /* blank SearchString */
   if ( (len = strlen(ReplaceString)) !=0 )
	memset(ReplaceString,'\0',len);	  /* blank SearchString */
   if (ERR_promptForString(" Enter String to Replace: ", SearchString, FALSE)
        || !strlen(SearchString))     	  /* invalid string entered  */
   {
	ERR_clearMsg();
	return SUCCESS;
   }
   if (ERR_promptForString(" Enter Replace String: ", ReplaceString, FALSE))
	return SUCCESS;
   BUF_wrapSearch = FALSE;
   BUF_adjustRow(buffer);
   ERR_clearMsg();
   ERR_displayStr("Searching ... ",FALSE);
   ReplaceRow = BUF_row(buffer);
   ReplaceCol = BUF_col(buffer);
   origscreenrow = BUF_screenRow(buffer);
   origcolzero = BUF_colZero(buffer);
   while (BUF_doSearchForward(buffer, win) != FAILURE)
   {
	BUF_doReplace(buffer,win);
	count++;
   }
   BUF_wrapSearch = TRUE;
   BUF_changeRow(buffer, ReplaceRow);
   BUF_colZero(buffer) = origcolzero;
   BUF_setCol(buffer, win, ReplaceCol);
   BUF_screenRow(buffer) = origscreenrow;
   BUF_paintWindow(buffer, win);
   ReplaceRow = NULL;
   ReplaceCol = 0;
   ERR_clearMsg();
   if( count == 0 )
	ERR_displayStrMsg(SearchString," not found",BEEP);
   else
   {
	sprintf(msg,"Replaced %d occurences",count);
	ERR_displayStr(msg,FALSE);
   }
   return SUCCESS;
}

PublicFnDef int BUF_queryReplaceString(buffer,win)
/************************************************
*
*	Function:	Searches for the SearchString and replaces 
*			with ReplaceString (prompted by user each time)
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
CUR_WINDOW *win;
{

int len, c, count = 0, done = FALSE, foundAny = FALSE, origscreenrow, origcolzero;
char msg[80];

   BUF_doingReplace = TRUE;
   BUF_tempReplace = TRUE;
   BUF_replaceWrap = FALSE;
   if (BUF_row(buffer) == NULL)
   {
	ERR_displayStr("Buffer empty",TRUE);
	return SUCCESS;
   }
   if ( (len = strlen(SearchString)) !=0 )
	memset(SearchString,'\0',len);	  /* blank SearchString */
   if ( (len = strlen(ReplaceString)) !=0 )
	memset(ReplaceString,'\0',len);	  /* blank SearchString */
   if (ERR_promptForString(" Enter String to Replace: ", SearchString, FALSE)
        || !strlen(SearchString))     	  /* invalid string entered  */
   {
	ERR_clearMsg();
	return SUCCESS;
   }
   if (ERR_promptForString(" Enter Replace String: ", ReplaceString, FALSE))
	return SUCCESS;
   BUF_wrapSearch = FALSE;
   BUF_adjustRow(buffer);
   ReplaceRow = BUF_row(buffer);
   ReplaceCol = BUF_col(buffer);
   origscreenrow = BUF_screenRow(buffer);
   origcolzero = BUF_colZero(buffer);
   while (!done)
   {
        if (BUF_doSearchForward( buffer, win ) == FAILURE)
		break;
	foundAny = TRUE;
	if (!KEY_cready() && BUF_repaintScreen(buffer))
		BUF_paintWindow(buffer, win);
	c = promptYesNoAbort("Replace String (y/n) (F9 to abort) ? ", buffer, win);
	switch (c)
	{
		case 'y':
		case 'Y':
			BUF_doReplace(buffer,win);
			BUF_paintLine(buffer,win);
			CUR_wrefresh(win);
			BUF_tempReplace = TRUE;
			count++;
			break;
		case KEY_QUIT:
		case KEY_PREV:
			done = TRUE;
			break;
		case 'n':
		case 'N':
			/* If we don't replace the found string then we must */
			/* set this to FALSE so that the next occurance of   */
			/* the SearchString is found.			     */
			BUF_tempReplace = FALSE;
			break;
		default:
			BUF_tempReplace = FALSE;
			break;
	}
   }
   BUF_wrapSearch = TRUE;
   BUF_changeRow(buffer, ReplaceRow);
   BUF_colZero(buffer) = origcolzero;
   BUF_setCol(buffer, win, ReplaceCol);
   BUF_screenRow(buffer) = origscreenrow;
   BUF_paintWindow(buffer, win);
   ReplaceRow = NULL;
   ReplaceCol = 0;
   if( count == 0 && !foundAny )
	ERR_displayStrMsg(SearchString," not found",BEEP);
   else
   {
	sprintf(msg,"Replaced %d occurences",count);
	ERR_displayStr(msg,FALSE);
   }

   return SUCCESS;
}

/************************************************
*
*	Function:	Replaces the string in SearchString with the string
*			in ReplaceString at current cursor position
*
*	Arguments:	LINEBUFFER pointer, Window
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
PrivateFnDef int BUF_doReplace (
    LINEBUFFER *		buffer,
    WINDOW *			win
)
{

int searchlen, linelen, replacelen;
char *row, *ptr, buf1[BUF_maxlinesize];

   if ((row = BUF_row(buffer)) == NULL)
	return SUCCESS;
   searchlen = strlen(SearchString);
   replacelen = strlen(ReplaceString);
   ptr = BUF_editRow(buffer);
   linelen = strlen(ptr);
   if ((linelen + replacelen - searchlen) > BUF_maxLineSize(buffer)) 
   {
	ERR_displayError(ERR_FullLine);
	return FAILURE;
   }
   strcpy(buf1,&ptr[BUF_col(buffer) + searchlen]);
   ptr[BUF_col(buffer)] = '\0';
   strcat(ptr, ReplaceString);
   strcat(ptr, buf1);
   if( row == ReplaceRow ) 
   {
   	if( ReplaceCol >= (BUF_col(buffer) + searchlen) )
		ReplaceCol += (replacelen - searchlen);
	else if( ReplaceCol > BUF_col(buffer) &&
		 ReplaceCol < (BUF_col(buffer) + searchlen) )
		ReplaceCol = BUF_col(buffer) + replacelen;
   }
   BUF_setCol(buffer, win, (BUF_col(buffer)+replacelen));
   BUF_editted(buffer) = TRUE;

   return SUCCESS;
}

/*********************************************************
 *********	Region Operations	    **************
 ********************************************************/

PublicFnDef int BUF_deleteRegion(buffer, startrow, startcol, endrow, endcol)
/************************************************
*
*	Function:	Delete and free region of line buffer
*
*	Arguments:	LINEBUFFER pointer, starting row and column
*			ending row and column
*
*	Returns:	FAILURE or SUCCESS
*
************************************************/
LINEBUFFER *buffer;
char *startrow;
int startcol;
char *endrow;
int endcol;
{
    char *row, *next;
    char *starttext, *endtext, tbuf[BUF_maxlinesize];
    int i, slen, elen, delEndRow = TRUE;

    /** If insertion or deletion occurs after region set then **/
    /** the region becomes invalid.				  **/
    if (BUF_editted(buffer) && 
       ((BUF_row(buffer) == startrow) || (BUF_row(buffer) == endrow)))
	return(ERR_BadRegion);
    if( (startrow == endrow) && (startcol == endcol) )
    	return(ERR_BadRegion);

    BUF_changeRow(buffer,startrow);
    starttext = BUF_editRow(buffer);
    endtext = BUF_line(endrow);
    slen = strlen(starttext);
    elen = strlen(endtext);
    if( startcol > slen )
    	startcol = slen;
    if( endcol > elen )
    	endcol = elen;

    if ((startrow == BUF_firstLine(buffer) && startcol == 0) &&
	(endrow == BUF_lastLine(buffer) && endcol >= elen) )
    {					/** region = entire buffer **/
        BUF_eraseBuffer(buffer);
        return SUCCESS;
    }
    
    if (startrow == endrow)    
    /** Cursor sits on startrow; therefore want to modify EditRow **/
    /** and not the lines array directly.                         **/
    {						
        starttext[startcol] = '\0';
	strcat(starttext, &starttext[endcol]);
	BUF_editted(buffer) = TRUE;	/** causes lines array to be updated **/
	BUF_setCol(buffer, (CUR_WINDOW *)NULL, startcol);
	BUF_repaintScreen(buffer) = TRUE;
	return SUCCESS;
    }

	if( startcol + strlen (&endtext[endcol]) >=
	    (size_t)BUF_maxLineSize(buffer)
	) return(ERR_FullLine);
#if 0
	{
        	starttext[startcol] = '\0';
		BUF_editted(buffer) = TRUE;     /** causes lines array to be updated **/
		BUF_tempRow3 = startrow;
		BUF_tempRow4 = endrow;
		strncat( starttext,
			 &endtext[endcol],
			 (i = (BUF_maxLineSize(buffer) - strlen(starttext))) );
		starttext[BUF_maxLineSize(buffer)] = '\0';
		strcpy(tbuf,&endtext[endcol+i]);
		strcpy(endtext,tbuf);
		BUF_adjustRow(buffer);
		startrow = BUF_nextLine(BUF_tempRow3);
		endrow = BUF_tempRow4;
		delEndRow = FALSE;
	}
#endif
	else
	{
        	starttext[startcol] = '\0';
		BUF_editted(buffer) = TRUE;     /** causes lines array to be updated **/
		BUF_tempRow3 = startrow;
		BUF_tempRow4 = endrow;
		strcat(starttext, &endtext[endcol]);
		BUF_adjustRow(buffer);
		startrow = BUF_nextLine(BUF_tempRow3);
		endrow = BUF_tempRow4;
	}

    row = startrow;
    while (row != endrow) 
    {
	next = BUF_nextLine(row);
	BUF_deleteLine(buffer, row);
	row = next;
    }
    if( delEndRow )
    	BUF_deleteLine(buffer,endrow);
    BUF_setCol(buffer, (CUR_WINDOW *)NULL, startcol);
    BUF_repaintScreen(buffer) = TRUE;
    BUF_tempRow3 = BUF_tempRow4 = NULL;
    return SUCCESS;
}

/*----------------------------------------------------------------------*/
PublicFnDef int BUF_insertRegion(tobuffer, win, frbuffer)
LINEBUFFER *tobuffer;
CUR_WINDOW *win;
LINEBUFFER *frbuffer;
{
    char *frrow, *row;
    int origcol, origscreenrow, origcolzero;

    if (BUF_row(tobuffer) == NULL)
/*	(BUF_row(tobuffer) == BUF_lastLine(tobuffer) && 
	 BUF_col(tobuffer) >= strlen(BUF_editRow(tobuffer))))*/
    {
	frrow = BUF_firstLine(frbuffer);
	while (frrow != NULL)
	{
	    if (NULL == (row = BUF_appendLine(tobuffer, BUF_line(frrow))))
		return(ERR_Memory);
	    if (BUF_row(tobuffer) == NULL)
		BUF_changeRow(tobuffer, row);
	    frrow = BUF_nextLine(frrow);
	}
	return SUCCESS;
    }

    BUF_tempRow3 = BUF_row(tobuffer);
    origcol = BUF_col(tobuffer);
    origscreenrow = BUF_screenRow(tobuffer);
    origcolzero = BUF_colZero(tobuffer);
    frrow = BUF_firstLine(frbuffer);
    BUF_insertString(tobuffer,BUF_line(frrow),win);
    if( BUF_nextLine(frrow) != NULL )
	BUF_carrReturn(tobuffer, win);
    frrow = BUF_nextLine(frrow);

    while (frrow != NULL)
    {
	if( BUF_nextLine(frrow) == NULL )
	    BUF_insertString(tobuffer,BUF_line(frrow),win);
	else if( NULL == BUF_insertLine(tobuffer, BUF_row(tobuffer), BUF_line(frrow)) )
	    return(ERR_Memory);
	frrow = BUF_nextLine(frrow);
    }
    
    BUF_changeRow(tobuffer, BUF_tempRow3);   /*** change to first line read ***/
    BUF_tempRow3 = NULL;
    BUF_colZero(tobuffer) = origcolzero;
    BUF_setCol(tobuffer, win, origcol);
    BUF_screenRow(tobuffer) = origscreenrow;

    return SUCCESS;
}

/*----------------------------------------------------------------------*/
PublicFnDef int BUF_appendRegion(tobuffer, frbuffer, startrow, startcol, 
						endrow, endcol)
LINEBUFFER *tobuffer, *frbuffer;
char *startrow;
int startcol;
char *endrow;
int endcol;
{
    char *rowtext, buffer[BUF_maxlinesize+1], *row;
    int slen, elen;

    if (startrow == BUF_row(frbuffer) && BUF_editted(frbuffer))
	rowtext = BUF_editRow(frbuffer);
    else
	rowtext = BUF_line(startrow);

    slen = strlen(rowtext);
    if( startcol > slen )
    	startcol = slen;
    if (startrow == endrow)
    {
	if( endcol > slen )
	    endcol = slen;
	strncpy(buffer, &rowtext[startcol], endcol - startcol);
	buffer[endcol-startcol] = '\0';
	if (NULL == BUF_appendLine(tobuffer, buffer))
	    return(ERR_Memory);
	return SUCCESS;
    }

/*
    if (startcol >= strlen(rowtext))
    {
        startcol = 0;
	startrow = BUF_nextLine(startrow);
    }
*/
    
    row = startrow;
    /*endrow = (endcol == 0) ? endrow : BUF_nextLine(endrow);*/

    while (row != BUF_nextLine(endrow))
    {
	if (row == BUF_row(frbuffer) && BUF_editted(frbuffer))
	    rowtext = BUF_editRow(frbuffer);
	else 
	    rowtext = BUF_line(row);

	if (row == startrow)
	{
	    if (NULL == BUF_appendLine(tobuffer, &rowtext[startcol]))
		return(ERR_Memory);
	}
	else if (row == endrow)
	{
/*
	    if (endcol == 0)
		break;
*/
	    elen = strlen(rowtext);
	    if( endcol > elen )
		endcol = elen;
	    strncpy(buffer, rowtext, endcol);
	    buffer[endcol] = '\0';
	    if (NULL == BUF_appendLine(tobuffer, buffer))
		return(ERR_Memory);
	}
	else
	    if (NULL == BUF_appendLine(tobuffer, rowtext))
		return(ERR_Memory);

	row = BUF_nextLine(row);
    }
    return SUCCESS;    
}


/*************************************************
 **********	Print Functions		**********
 *************************************************/

PrivateFnDef void systemPrint (buf)
LINEBUFFER *buf;
{
    char buffer[80], filename[80], *row;
    FILE *fd, *fopen();

    ERR_displayStr ("Printing buffer, please wait...",FALSE);
    sprintf(filename, "/tmp/vision%d", getpid());

    fd = fopen (filename, "w");

    row = BUF_firstLine (buf);

    if (!isBlank (PRINT_Command.initString))
        fprintf (fd, "%s", PRINT_Command.initString);

    while (row != NULL)
        {
	fprintf (fd, "%s\n", BUF_line(row));
        row = BUF_nextLine (row);
        }

    fclose(fd);

/***    sprintf(buffer, "lp -c -s -oc < %s", filename);	***/
    sprintf (buffer, "%s %s", PRINT_Command.command, filename);
    RS_system (buffer);
    remove (filename);
    ERR_clearMsg ();
}

#if 0
PrivateFnDef void pcPrint(buf)
LINEBUFFER *buf;
{
    char buffer[80], filename[80];
    FILE *fd, *fopen();
    char *row;
    
    ERR_displayPause ("Please Turn On PC Printer");
    ERR_displayStr ("Printing buffer, please wait...",FALSE);

#if 0
    putchar('\020');	/*** toggle printer ***/
    
    row = BUF_firstLine(buf);
    while (row != NULL)
    {
	puts(BUF_line(row));
        row = BUF_nextLine(row);
    }

    putchar('\020');	/*** toggle printer ***/
#endif

    sprintf (filename, "/tmp/vision%d", getpid());

    fd = fopen (filename, "w");

    row = BUF_firstLine (buf);
    while (row != NULL)
        {
	fprintf (fd, "%s\n", BUF_line (row));
        row = BUF_nextLine (row);
        }

    fclose (fd);

    sprintf (buffer, "%s %s", PRINT_Command.command, filename);
    RS_system (buffer);
    remove (filename);

    CUR_werase (ERR_Window);
    CUR_clearok (CUR_curscr);	/*** need to repaint the screen ***/
    CUR_wrefresh (CUR_curscr);
}
#endif

PublicFnDef int BUF_printBuffer (buf, page, defaultPrinter)
LINEBUFFER *buf;
PAGE *page;
int  defaultPrinter;
{
#if 0
    int c;
    
    c = ERR_promptForChar("Print to System or PC (s/p) ? ", "sSpP");
	
    if (c == 'p' || c == 'P')    
	pcPrint(buf);
    else
    if (c == 's' || c == 'S')    
	systemPrint(buf);
#endif

    print (page,defaultPrinter);

#if 0
    if (0 == strncmp(PRINT_Command.command, "pc", 2))
	pcPrint (buf);
    else
    if (PRINT_Lines)
#endif
    if (!isBlank (PRINT_Command.command))
        systemPrint (buf);
    

    return (FALSE);
}
#if 0
/*********************************************************
 *********	File / Buffer Operations    **************
 ********************************************************/
PublicFnDef int BUF_saveFile(buffer, current_file)
LINEBUFFER *buffer;
char *current_file;
{
    char string[BUF_MaxPathNameChars], prompt[BUF_MaxPathNameChars + 80];
    int	error;

    if (!strlen(current_file))
    {
	strcpy(prompt, " No current file,  enter file to write: ");
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	else
	{
	    if (strlen(string))
	        strcpy(current_file, string);
	    else
		return(FALSE);
	}
    }
    else
    {
	sprintf(prompt,
	 " Enter filename or <CR> to update %s: ", current_file);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	else
	    if (strlen(string))
	        strcpy(current_file, string);
    }
    
    return(file_write(buffer, current_file, "w"));
}

PublicFnDef int BUF_appendFile(buffer, current_file)
LINEBUFFER *buffer;
char *current_file;
{
    char string[BUF_MaxPathNameChars], prompt[BUF_MaxPathNameChars + 80];
    int	error, mode;

    if (!strlen(current_file))
    {
	strcpy(prompt, " No current file, enter file to append to: ");
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	else
	{
	    if (strlen(string))
	        strcpy(current_file, string);
	    else
		return(FALSE);
	}
    }
    else
    {
	sprintf(prompt,
	 " Enter filename or <CR> to append to %s: ", current_file);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	else
	    if (strlen(string))
	        strcpy(current_file, string);
    }
    
    return(file_write(buffer, current_file, "a"));
}
#endif

PublicFnDef int BUF_listFile(buffer,directory)
LINEBUFFER *buffer;
char *directory;
{

   char string[BUF_MaxPathNameChars];

   if (ERR_promptForString(" Enter directory: ", string, FALSE))
  	return(TRUE);
   else
	strcpy(directory,string);
   return(FALSE);

}

PrivateFnDef int file_write(buffer, filename, mode)
LINEBUFFER *buffer;
char *filename, *mode;
{
    char *row;
    FILE *fd;
    
    if ((fd = fopen(filename, mode)) == NULL)
	return (ERR_OpenError);

    BUF_adjustRow(buffer);
    row = BUF_firstLine(buffer);
    while (row != NULL)
    {
	if (row == BUF_row(buffer))
	    fputs(BUF_editRow(buffer), fd);
	else
	    fputs(BUF_line(row), fd);
	fputc('\n', fd);
	row = BUF_nextLine(row);
    }
	
    fclose(fd);
    return SUCCESS;
}

PublicFnDef int BUF_writeFile( buffer, filename )
LINEBUFFER *buffer;
char	   *filename;
{
	return( file_write( buffer, filename, "w" ) );
}

PublicFnDef int BUF_appendToFile( buffer, filename )
LINEBUFFER *buffer;
char	   *filename;
{
	return( file_write( buffer, filename, "a" ) );
}

PublicFnDef int BUF_writeLastOutput( buffer, filename, mode )
LINEBUFFER *buffer;
char	   *filename, *mode;
{
    char *row;
    FILE *fd;
    
    if ((fd = fopen(filename, mode)) == NULL)
    {
	return (ERR_OpenError);
    }

    row = BUF_LastOutput(buffer);
    while (row != NULL)
    {
	if (row == BUF_row(buffer))
	    fputs(BUF_editRow(buffer), fd);
	else
	    fputs(BUF_line(row), fd);
	fputc('\n', fd);
	row = BUF_nextLine(row);
    }

    fclose(fd);
    return SUCCESS;
}

PublicFnDef int BUF_stripTabs(source, dest, len)
/***************************************************
 * Strip tabs ('\t') from source into dest and copy
 * no more than len characters (including the '\0')
 ***************************************************/
char	*source, *dest;
int	len;
{
	int	i, j, k, l, slen;

	slen = strlen(source);
#if 0
	for( i=0, j=0 ; i<slen ; i++ ) {
		if( j >= len-1 ) {
			ERR_displayError(ERR_FullLine);
			break;
		}
		if( source[i] == '\t' ) {
			k = (((j + RS_TabSpacing) / RS_TabSpacing) *
				RS_TabSpacing) - j;
			for( l=0 ; l<k ; l++ ) {
				dest[j++] = ' ';
				if( j >= len-1 )
					break;
			}
		}
		else
			dest[j++] = source[i];
	}
	dest[j] = '\0';
#endif
	if( slen >= len )
	{
		strncpy(dest, source, (len-1));
		dest[len-1]  ='\0';
	}
	else
		strcpy(dest,source);
}

PrivateFnDef int file_read(buffer, current_file)
LINEBUFFER *buffer;
char *current_file;
{
    char string[BUF_maxlinesize], str[BUF_maxlinesize];
    FILE *fd;
    int emptyBuffer, error, len, origcol = 0, origscreenrow, origcolzero;

    if (!strlen(current_file))
	return(ERR_OpenError);
    if (NULL == (fd = fopen(current_file, "r")))
        return (ERR_OpenError);

    BUF_adjustRow(buffer);
    origscreenrow = BUF_screenRow(buffer);
    origcolzero = BUF_colZero(buffer);
    if (BUF_firstLine(buffer) == NULL)
    {
	emptyBuffer = TRUE;
	if ((fgets(string, BUF_maxlinesize, fd)) != NULL)
	{
	    BUF_stripTabs(string, str, BUF_maxlinesize);
	    if (NULL == (BUF_tempRow3 = BUF_appendLine(buffer, str)))
	    {
	    	fclose(fd);
		return(ERR_Memory);
	    }
	}
	else
	{
	    fclose(fd);
	    return(ERR_FileEmpty);
	}
    }
    else
    {
    	origcol = BUF_col(buffer);
	emptyBuffer = FALSE;
	if ((fgets(string, BUF_maxlinesize, fd)) != NULL)
	{
	    BUF_stripTabs(string, str, BUF_maxlinesize);
	    if (str == NULL)
	        len = 0;
	    else
		len = strlen(str);
	    if( (len > 0) && (str[len - 1] == '\n') )
		str[len - 1] = '\0';
	    BUF_tempRow3 = BUF_row(buffer);
	    if( error = BUF_insertString(buffer, str, NULL) )
	    {
	    	fclose(fd);
		return(error);
	    }
	    BUF_carrReturn(buffer, NULL);
	}
	else
	{
	    fclose(fd);
	    return(ERR_FileEmpty);
	}
    }

    while ((fgets(string, BUF_maxlinesize, fd)) != NULL)
    {	
	BUF_stripTabs(string, str, BUF_maxlinesize);
	if (emptyBuffer)
	{
	    if (NULL == BUF_appendLine(buffer, str))
	    {
	        fclose(fd);
		return(ERR_Memory);
	    }
	}
	else
	{
	    if (NULL == BUF_insertLine(buffer, BUF_row(buffer), str))
	    {
	        fclose(fd);
		return(ERR_Memory);
	    }
	    
	}
    }

    BUF_changeRow(buffer, BUF_tempRow3);   /*** change to first line read ***/
    BUF_tempRow3 = NULL;
    BUF_colZero(buffer) = origcolzero;
    BUF_setCol(buffer, (CUR_WINDOW *)NULL, origcol);
    BUF_screenRow(buffer) = origscreenrow;

    fclose(fd);

    return SUCCESS;
}

PublicFnDef int BUF_readFile(buffer, current_file)
LINEBUFFER *buffer;
char *current_file;
{
    char string[BUF_maxlinesize], prompt[BUF_MaxPathNameChars + 80];
    int  error;

    if( !strlen(current_file) )
    {     
	if (error = ERR_promptForString(" Enter file to read: ", string, TRUE))
		return(error);
	if (strlen(string))
		strcpy(current_file, string);
	else
		return(FALSE);
    }
    else
    {
	sprintf(prompt,
	 " Enter filename or <CR> to read from %s: ", current_file);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	if (strlen(string))
	        strcpy(current_file, string);
    }
    return( file_read(buffer, current_file) );
}

PublicFnDef int BUF_getFile(buffer, current_file)
LINEBUFFER *buffer;
char *current_file;
{
    return( file_read(buffer, current_file) );
}

PublicFnDef LINEBUFFER *BUF_readBuffer(filename, len, min, max)
char *filename;
int len, min, max;
{
    FILE *fptr, *fopen();
    LINEBUFFER *buffer;
    char string[BUF_maxlinesize], str[BUF_maxlinesize];
    
    if (NULL == (buffer = (LINEBUFFER *) malloc(sizeof(LINEBUFFER))))
        return NULL;
    if (NULL == (fptr = fopen(filename, "r")))
        return NULL;

    BUF_initBuffer(buffer,max);
    BUF_maxLineSize(buffer) = len;

    while (fgets(string, len, fptr))	
    {
	BUF_stripTabs(string, str, BUF_maxlinesize);
	if (NULL == BUF_appendLine(buffer, str))
	{
	    fclose(fptr);
	    return(NULL);
	}
    }

    fclose(fptr);
    
    return(buffer);
}


PublicFnDef PAGE_Action BUF_handler(buffer, win, action)
LINEBUFFER *buffer;
CUR_WINDOW *win;
PAGE_Action action;
/*****		Routine to manage user interaction with buffers
 *
 *  Arguments:
 *  An array of pointers containing:
 *	buffer	    -pointer to a LINEBUFFER structure
 *	win	    -pointer to CUR_WINDOW
 *	action	    -pointer to a PAGE_Action
 *
 *  Returns:
 *	PAGE_Action
 *
 *****/
{
    PAGE_Action initBuffer();

    switch (action)
    {
    case PAGE_Init:
	BUF_initScreen(buffer, win);
        return(PAGE_Init);
          
    case PAGE_Scroll:
        return(scrollBuffer(buffer, win));
    
    case PAGE_Refresh:
	CUR_touchwin(win);
	CUR_wnoutrefresh(win);
        return(PAGE_Refresh);
          
    default:
        return(PAGE_Error);

    }
}


#define hilightRow(b, w)\
{\
    CUR_wattron(w, CUR_A_REVERSE);\
    ptr = BUF_editRow(b);\
    tcol = BUF_getColFromSCol(buffer, BUF_row(buffer), BUF_colZero(buffer), &nscol);\
    CUR_wmove(w, BUF_screenRow(b), 0);\
    ERR_paintLineASCII( &ptr[tcol],\
    			CUR_WIN_cols(w),\
    			w,\
    			FALSE,\
    			0 );\
    CUR_wattroff(w, CUR_A_REVERSE);\
}

#define unhilightRow(b, w)\
{\
    CUR_wattroff(w, CUR_A_ALLATTRS);\
    CUR_wmove(w, BUF_screenRow(b), 0);\
    ERR_paintLineASCII( &ptr[tcol],\
    			CUR_WIN_cols(w),\
    			w,\
    			TRUE,\
    			CUR_A_REVERSE );\
}

PrivateFnDef PAGE_Action scrollBuffer (
    LINEBUFFER *		buffer,
    CUR_WINDOW *		win
)
{
    int c, i, length, row, col, tcol, nscol;
    char *ptr;

/***** initialize, display the window *****/

    BUF_initScreen(buffer, win);

/***** the input loop *****/

    for (;;)
    {
	if (!KEY_cready())
	{
	    if (BUF_repaintScreen(buffer))
		BUF_paintWindow(buffer, win);
	    if (BUF_status(buffer) == BUF_Hilight)
	        hilightRow(buffer, win);
	    BUF_moveCursor(buffer, win, row, col, i);
/*	    CUR_touchwin(win);*/
	    CUR_wrefresh(win);
	    if (BUF_status(buffer) == BUF_Hilight)
	        unhilightRow(buffer, win);
	    CUR_forceSetCursor(win, row, col);
	}

	c = KEY_getkey(FALSE);

	if (BUF_row(buffer) == NULL)
	    length = 0;
	else		
	    length = strlen(BUF_editRow(buffer));
        
	if (ERR_msgDisplayed)
	    ERR_clearMsg();
     
	switch(c) {
	
	case 0:
	    break;

	case KEY_SCRIPTR:
	    KEY_beginScriptRead();
	    break;

	case KEY_SCRIPTW:
	    KEY_beginScriptWrite();
	    break;

	case KEY_VEOL:
	    BUF_goToEOL(buffer, length, win);
	    break;

	case KEY_BOL:
	    BUF_goToBOL(buffer, win);
	    break;

	case KEY_WORDFORW:
	    BUF_forwardWord(buffer,win);
	    break;

	case KEY_WORDBACK:
	    BUF_backwardWord(buffer,win);
	    break;

	case KEY_SCROLLUP1:
	    BUF_scrollUp1(buffer, win);
	    break;

	case KEY_SCROLLDN1:
	    BUF_scrollDown1(buffer, win);
	    break;

	case KEY_EXEC:
	    return(PAGE_Exec);

	case KEY_HELP:
	    return(PAGE_Help);

	case KEY_WINDOW: 
	    return(PAGE_Window);

	case KEY_EDIT:
	    return(PAGE_F4);

	case KEY_FILES:
	    return(PAGE_File);
	    
	case KEY_REGIONS:
	    return(PAGE_Region);
	    
	case KEY_SMENU:
	    return(PAGE_SMenu);

	case KEY_AMENU:
	    return(PAGE_AMenu);

	case KEY_QUIT:
	    return(PAGE_Quit);
	    
	case KEY_PREV:
	    return(PAGE_Prev);

	case KEY_ZEROROW:
	    BUF_screenRow(buffer) = 0;
	    BUF_resetScreen(buffer, win);
	    break;

	case KEY_REPAINT:
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    break;

	case KEY_NL:
	case CUR_KEY_DOWN:
	    BUF_keyDown(buffer, length, win);
	    break;
	    
	case KEY_UPL:
	case CUR_KEY_UP:
	    BUF_keyUp(buffer, length, win);
	    break;

	case KEY_BKCH:
	case CUR_KEY_LEFT:
	    BUF_keyLeft(buffer, length, win);
	    break;
   
	case KEY_FCH:
	case CUR_KEY_RIGHT:
	    BUF_keyRight(buffer, length, win);
	    break;

	case KEY_SCR_R:
	    BUF_scrollRight(buffer, win, i);
	    break;

	case KEY_SCR_L:
	    BUF_scrollLeft(buffer, win);
	    break;
	
	case KEY_SCR_U:
	    BUF_scrollUp(buffer, win);
	    break;

	case KEY_SCR_D:
	    BUF_scrollDown(buffer, win, i);
	    break;

	case KEY_TOP:
	    BUF_goToTop(buffer, win);
	    break;

	case KEY_BOTTOM:
	    BUF_goToBottom(buffer, win, i);
	    break;

	case KEY_SEARCH:
	    BUF_searchString(buffer,win);
	    break;

	case KEY_SNEXT:
	    BUF_searchNext(buffer,win);
	    break;

	case KEY_SPREV:
	    BUF_searchPrev(buffer,win);
	    break;

	default:
	    ERR_displayError(ERR_UndefKey);
	    KEY_QuoteNextKey = FALSE;
	    break;
	}
    }    
}

PublicFnDef int
BUF_scrollUp1(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	BUF_adjustRow(buffer);
	BUF_resetScreen(buffer,win);
	if( (BUF_row(buffer) == NULL) ||
	    (BUF_prevLine(BUF_row(buffer)) == NULL) )
	{
		return(FALSE);
	}
	if( BUF_row(buffer) == BUF_bufferRow(buffer,CUR_WIN_maxy(win)) )
	{
		BUF_changeRow(buffer,BUF_prevLine(BUF_row(buffer)));
		BUF_setCol(buffer, win, 0);
	}
	else
		BUF_screenRow(buffer)++;
	BUF_resetScreen(buffer,win);
	return(FALSE);
}

PublicFnDef int
BUF_scrollDown1(buffer,win)
LINEBUFFER *buffer;
CUR_WINDOW *win;
{
	BUF_adjustRow(buffer);
	BUF_resetScreen(buffer,win);
	if( (BUF_row(buffer) == NULL) ||
	    (BUF_nextLine(BUF_row(buffer)) == NULL) )
	{
		return(FALSE);
	}
	if( BUF_row(buffer) == BUF_bufferRow(buffer,0) )
	{
		BUF_changeRow(buffer,BUF_nextLine(BUF_row(buffer)));
		BUF_setCol(buffer, win, 0);
	}
	else
		BUF_screenRow(buffer)--;
	BUF_resetScreen(buffer,win);
	return(FALSE);
}
