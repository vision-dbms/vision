/*********************************************************
 **********		spsheet.c		**********
 *********************************************************/

#include "Vk.h"

/********** /usr/rs/lib/system **********/
#include "stdcurses.h"

/********** /usr/rs/lib/interface **********/
#include "misc.h"
#include "page.h"
#include "window.h"
#include "keys.h"
#include "error.h"
#include "rsInterface.h"
#include "print.h"

/***** Spread Sheet defines *****/
#include "spsheet.d"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef PAGE_Action initSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
);

PrivateFnDef PAGE_Action scrollSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
);

PrivateFnDef PAGE_Action inputSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
);

PrivateFnDef int systemPrint (
    SPRSHEET *			spr
);


/*************************************************
 **********	Constants and Globals	**********
 *************************************************/

#define LINES_PER_PAGE	66

PublicVarDef SPRSHEET	*SPR_OldSPR = NULL, *SPR_CurrSPR = NULL;
PublicVarDef int	SPR_simulateApplic = FALSE;

PublicFnDef void SPR_delete(spr)
SPRSHEET *spr;
{
    int col;
    
    if( spr == SPR_CurrSPR )
    	SPR_CurrSPR = NULL;
    if (SPR_rowsSize(spr) > 0) free(spr->rows);

    for (col = 0; col < SPR_fieldCount(spr); col++)
	free(SPR_field(spr, col));
    if (SPR_fieldsSize(spr) > 0) free(spr->fields);

    free(spr);
}

PublicFnDef void SPR_erase(spr)
SPRSHEET *spr;
{
    if (SPR_rowsSize(spr) > 0) free(spr->rows);
    spr->rows = NULL;
    SPR_rowCount(spr) = SPR_rowsSize(spr) = 0;
    SPR_lastCol(spr) = 0;
    SPR_currCol(spr) = SPR_fixedCols(spr);
    SPR_currRow(spr) = SPR_fixedRows(spr);

}

PublicFnDef void SPR_eraseFields(spr)
SPRSHEET *spr;
{
    int col;

    for (col = 0; col < SPR_fieldCount(spr); col++)
	free(SPR_field(spr, col));
    SPR_fieldCount(spr) = 0;
}


PublicFnDef PAGE_Action SPR_handler(spsheet, win, action)
SPRSHEET *spsheet;
CUR_WINDOW *win;
PAGE_Action action;
/*****		Routine to manage user interaction with spsheet 
 *
 *  Arguments:
 *	spsheet	    -pointer to a spsheet object 
 *	win	    -pointer to the CUR_WINDOW    
 *	action	    -pointer to a PAGE_Action
 *
 *  Returns:
 *	PAGE_Action
 *
 *****/
{
    switch (action)
    {
    case PAGE_Init:
        return(initSprSh(spsheet, win));

    case PAGE_Input:
        return(inputSprSh(spsheet, win));
          
    case PAGE_Scroll:
        return(scrollSprSh(spsheet, win));

    case PAGE_End:
        CUR_delwin(win);
	return(action);

    case PAGE_Refresh:
	CUR_touchwin(win);
	CUR_wnoutrefresh(win);
        return(PAGE_Refresh);
          
    default:
        return(PAGE_Error);

    }
}

PrivateFnDef PAGE_Action initSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
)
{
    int i;
    
    SPR_screenCols(spsheet) = (CUR_WIN_cols(win) - 1) / SPR_cellSize(spsheet);
    
/*** fill the window with the spsheet data ***/
    if ((SPR_status(spsheet) == SPR_InitBottom) &&
	(SPR_rowCount(spsheet) > CUR_WIN_rows(win)))
    {
	for (i = 0; i < CUR_WIN_rows(win); i++)
	    SPR_rowOnScreen(spsheet, i) = i;
	for (i = SPR_fixedRows(spsheet);
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spsheet); i++)
	    SPR_rowOnScreen(spsheet, i) =
		 SPR_rowCount(spsheet) - (CUR_WIN_rows(win) - i);
    }
    else
    {
	for (i = 0; i < CUR_WIN_rows(win); i++)
	    SPR_rowOnScreen(spsheet, i) = i;
    }
    
    for (i = 0; i < SPR_screenCols(spsheet); i++)
	SPR_colOnScreen(spsheet, i) = i;

    SPR_currRow(spsheet) = SPR_fixedRows(spsheet);
    SPR_currCol(spsheet) = SPR_fixedCols(spsheet);

    SPR_paintScreen(spsheet, win);

/*** refresh the current screen ***/
    CUR_touchwin(win);
    CUR_wnoutrefresh(win);
}

PublicFnDef SPR_paintScreen(spr, win)
SPRSHEET *spr;
CUR_WINDOW *win;
{
    int i, j, row, col;
    char *cell;
    
    CUR_werase(win);

    for (i = 0; (i < CUR_WIN_rows(win) && i < SPR_rowCount(spr)); i++)
    {
        row = SPR_rowOnScreen(spr, i);
	for (j = 0; (j < SPR_screenCols(spr) && j < SPR_colCount(spr)); j++)
	{
	    if( i < SPR_titleRows(spr) )
	    	col = j;
	    else
		col = SPR_colOnScreen(spr, j);
	    cell = SPR_cellPtr(spr, row, col);
	    CUR_wmove(win, i, (j * SPR_cellSize(spr)));
	    CUR_wprintw(win, "%*s", SPR_cellSize(spr), SPR_cellValue(cell));
	}
    }
    SPR_repaintScreen(spr) = FALSE;
}


PrivateFnDef PAGE_Action scrollSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
)
{
    int c, row, col, i, j, not_done;
    PAGE_Action action;

    SPR_CurrSPR = spsheet;
        
    row = SPR_currRow(spsheet);
    col = SPR_currCol(spsheet);
    
    not_done = TRUE;
    while (not_done)
    {
	if (!KEY_cready())
	{
	    if (SPR_repaintScreen(spsheet))
		SPR_paintScreen(spsheet, win);
	    SPR_moveCursor(win, spsheet, row, col);
	    CUR_touchwin(win);
	    CUR_wnoutrefresh(win);
	    CUR_doupdate();
	    CUR_forceSetCursor(win, row, col);
	}

	if( SPR_simulateApplic )
	{
	    SPR_simulateApplic = FALSE;
	    c = KEY_AMENU;
	}
	else
	    c = KEY_getkey(FALSE);

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

    	case KEY_HELP:
	    action = PAGE_Help;
	    not_done = FALSE;
	    break;
		
	case KEY_WINDOW:
	    action = PAGE_Window;
	    not_done = FALSE;
	    break;

	case KEY_EDIT:
	    action = PAGE_F4;
	    not_done = FALSE;
	    break;

    	case KEY_EXEC:
	    action = PAGE_Exec;
	    not_done = FALSE;
	    break;
		
	case KEY_PREV:
	    action = PAGE_Prev;
	    not_done = FALSE;
	    break;

	case KEY_FILES:
	    action = PAGE_File;
	    not_done = FALSE;
	    break;

	case KEY_QUIT:
	    action = PAGE_Quit;
	    not_done = FALSE;
	    break;

	case KEY_AMENU:
	    action = PAGE_AMenu;
	    not_done = FALSE;
	    break;

	case KEY_SMENU:
	    action = PAGE_SMenu;
	    not_done = FALSE;
	    break;

	case KEY_TOP:
	    SPR_goToTop(spsheet, win, i, row, col);
	    break;

	case KEY_BOTTOM:
	    SPR_goToBottom(spsheet, win, i, row, col);
	    break;

	case KEY_SCR_D:
	    SPR_scrollDown(spsheet, win, i, j);
	    break;
       
	case KEY_SCR_U:
	    SPR_scrollUp(spsheet, win, i, j);
	    break;
       
	case KEY_SCROLLDN1:
	    SPR_scrollDown1(spsheet, win, row, j);
	    break;
       
	case KEY_SCROLLUP1:
	    SPR_scrollUp1(spsheet, win, row, j);
	    break;
  
	case KEY_SCR_R:
	case KEY_TAB:
	    SPR_scrollRight(spsheet, win, i, j);
	    break;
       
	case KEY_SCR_L:
	case KEY_BTAB:
	    SPR_scrollLeft(spsheet, win, i, j);
	    break;

	case KEY_UPL:       
	case CUR_KEY_UP:
	    SPR_keyUp(spsheet, win, row, i);
	    break;

	case KEY_NL:
	case CUR_KEY_DOWN:
	    SPR_keyDown(spsheet, win, row, i);
	    break;

	case KEY_BKCH:
	case CUR_KEY_LEFT:
	    SPR_keyLeft(spsheet, win, col, i);
	    break;

	case KEY_FCH:
	case CUR_KEY_RIGHT:
	    SPR_keyRight(spsheet, win, col, i);
	    break;

	case KEY_REPAINT:
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    break;

	case KEY_QUOTE:
	    KEY_QuoteNextKey = FALSE;
	    ERR_displayError(ERR_UndefKey);
	    break;

	default:
	    ERR_displayError(ERR_UndefKey);
	    break;

	} /* switch */

    } /* while not_done*/

    SPR_currRow(spsheet) = row;
    SPR_currCol(spsheet) = col;

    return(action);
}


PrivateFnDef PAGE_Action inputSprSh (
    SPRSHEET *			spsheet,
    CUR_WINDOW *		win
)
{
    int c, row, col, i, j, not_done, firstKey;
    char *cell, *lastCell;
    PAGE_Action action;
    
    SPR_CurrSPR = spsheet;
    
    row = SPR_currRow(spsheet);
    col = SPR_currCol(spsheet);
    lastCell = SPR_cellPtr(spsheet, SPR_rowOnScreen(spsheet, row),
			    		SPR_colOnScreen(spsheet, col));
    firstKey = TRUE;
    
    not_done = TRUE;
    while (not_done)
    {
	cell = SPR_cellPtr(spsheet, SPR_rowOnScreen(spsheet, row),
			    		SPR_colOnScreen(spsheet, col));

	if (cell != lastCell)
	{
	    lastCell = cell;
	    firstKey = TRUE;
	}
	
	if (!KEY_cready())
	{
	    if (SPR_repaintScreen(spsheet))
		SPR_paintScreen(spsheet, win);
	    SPR_moveCursor(win, spsheet, row, col);
	    CUR_touchwin(win);
	    CUR_wrefresh(win);
	    CUR_forceSetCursor(win, row, col);
	}
	if( SPR_simulateApplic )
	{
	    SPR_simulateApplic = FALSE;
	    c = KEY_AMENU;
	}
	else
	    c = KEY_getkey(FALSE);

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

    	case KEY_HELP:
	    action = PAGE_Help;
	    not_done = FALSE;
	    break;
		
	case KEY_WINDOW:
	    action = PAGE_Window;
	    not_done = FALSE;
	    break;

	case KEY_EDIT:
	    action = PAGE_F4;
	    not_done = FALSE;
	    break;

    	case KEY_EXEC:
	    action = PAGE_Exec;
	    not_done = FALSE;
	    break;
		
	case KEY_QUIT:
	    action = PAGE_Quit;
	    not_done = FALSE;
	    break;

	case KEY_PREV:
	    action = PAGE_Prev;
	    not_done = FALSE;
	    break;

	case KEY_FILES:
	    action = PAGE_File;
	    not_done = FALSE;
	    break;

	case KEY_AMENU:
	    action = PAGE_AMenu;
	    not_done = FALSE;
	    break;

	case KEY_SMENU:
	    action = PAGE_SMenu;
	    not_done = FALSE;
	    break;

	case KEY_TOP:
	    SPR_goToTop(spsheet, win, i, row, col);
	    break;

	case KEY_BOTTOM:
	    SPR_goToBottom(spsheet, win, i, row, col);
	    break;

	case KEY_SCR_D:
	    SPR_scrollDown(spsheet, win, i, j);
	    break;
       
	case KEY_SCR_U:
	    SPR_scrollUp(spsheet, win, i, j);
	    break;
       
	case KEY_SCR_R:
	case KEY_TAB:
	    SPR_scrollRight(spsheet, win, i, j);
	    break;
       
	case KEY_SCR_L:
	    SPR_scrollLeft(spsheet, win, i, j);
	    break;
       
	case CUR_KEY_UP:
	    SPR_keyUp(spsheet, win, row, i);
	    break;

	case CUR_KEY_DOWN:
	    SPR_keyDown(spsheet, win, row, i);
	    break;

	case CUR_KEY_LEFT:
	    SPR_keyLeft(spsheet, win, col, i);
	    break;

	case CUR_KEY_RIGHT:
	    SPR_keyRight(spsheet, win, col, i);
	    break;


	case KEY_REPAINT:
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    break;

	case KEY_BKSP:
	    i = strlen(SPR_cellValue(cell));
	    if ((!firstKey) && i > 0)
	    {
		if (i == 1)
		{
		    strcpy(SPR_cellValue(cell), "NA");
		    firstKey = TRUE;
		}
		else
		    SPR_cellValue(cell)[i - 1] = '\0';
		SPR_repaintScreen(spsheet) = TRUE;
	    }
	    else
		ERR_displayStr(" Nothing to delete", TRUE);
	    break;

	case KEY_QUOTE:
	    KEY_QuoteNextKey = FALSE;
	    ERR_displayError(ERR_UndefKey);
	    break;

	default:
	    if (isdigit(c) || 
	        (c == '.' && (NULL == strchr(SPR_cellValue(cell), '.'))))
	    {
		i = strlen(SPR_cellValue(cell));
		if ((!firstKey) && (i >= SPR_cellSize(spsheet)))
		    ERR_displayStr(" Cell is full", TRUE);
		else
		{    
		    if (firstKey)
		    {
			firstKey = FALSE;
		        i = 0;
			SPR_cellStatus(cell) = SPR_Modified;
		    }
		    SPR_cellValue(cell)[i] = c;
		    SPR_cellValue(cell)[i + 1] = '\0';
		    SPR_repaintScreen(spsheet) = TRUE;
		}
	    }
	    else
		ERR_displayError(ERR_UndefKey);
	    break;

	} /* switch */

    } /* while not_done */

    SPR_currRow(spsheet) = row;
    SPR_currCol(spsheet) = col;

    return(action);
}

PublicFnDef int SPR_addRow(spr, buffer)
SPRSHEET *spr;
char *buffer;
{
    char *ptr, *new, *cell, buffer2[80];
    int row, col, len, i;
    
    row = SPR_rowCount(spr);    

    if (SPR_rowsSize(spr) == 0)
    {
	SPR_rowsSize(spr) = 25;
        spr->rows =
	    malloc(SPR_rowsSize(spr) * SPR_colCount(spr) * SPR_cellLen(spr));
    }

    if (row >= SPR_rowsSize(spr))
    {
	SPR_rowsSize(spr) += 25;
	new = realloc (spr->rows, 
	    (SPR_rowsSize(spr) * SPR_colCount(spr) * SPR_cellLen(spr)));
	if (new == NULL) return TRUE;
	spr->rows = new;
    }

    if (buffer == NULL)
        len = 0;
    else
	len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        len--;
	buffer[len] = '\0';
    }
    if( (buffer != NULL) && ((i = (len % SPR_cellSize(spr))) != 0) )
    {
    	sprintf(buffer2,"%*s",(SPR_cellSize(spr)-i)," ");
    	strcat(buffer,buffer2);
    	len = strlen(buffer);
    }
    
    ptr = buffer;
    for (col = 0; col < SPR_colCount(spr); col++)
    {
	cell = SPR_cellPtr(spr, row, col);
	if (len > 0 && len > (int)(ptr - buffer))
	{
	    memcpy(SPR_cellValue(cell), ptr, SPR_cellSize(spr));
	    SPR_cellValue(cell)[SPR_cellSize(spr)] = '\0';
	    SPR_cellStatus(cell) = SPR_Normal;
	    if (col > SPR_lastCol(spr) && (!isBlank(ptr)))
		SPR_lastCol(spr) = col;
	    ptr += SPR_cellSize(spr);
	}
	else
	{
	    SPR_cellStatus(cell) = SPR_Empty;
	    SPR_cellValue(cell)[0] = '\0';
	}
    }

    SPR_rowCount(spr)++;

    return(FALSE);
}

PublicFnDef int SPR_makeDummySheet(spr)
SPRSHEET *spr;
{
    char *buffer;
    int width, j;

    width = SPR_colCount(spr) * SPR_cellSize(spr) * sizeof(char) * 2;
    buffer = malloc(width);
    if (buffer == NULL) return(TRUE);	

    buffer[0] = '\0';    
    for( j=0 ; j<=SPR_fixedRows(spr) ; j++ )
    {
	if (SPR_addRow(spr, buffer)) return (TRUE);
    }   

    free(buffer);
    return FALSE;
}

#if RSATTACH

PublicFnDef int SPR_readSSheet(spr)
SPRSHEET *spr;
{
    char *buffer;
    int lastIsBlank, width, i;

    width = SPR_colCount(spr) * SPR_cellSize(spr) * sizeof(char) * 2;
    buffer = malloc(width);
    if (buffer == NULL) return(TRUE);	

    i = SPR_rowCount(spr);
    while (RS_readLine(buffer, width))
    {
	lastIsBlank = isBlank(buffer);
	if (SPR_addRow(spr, buffer)) return (TRUE);
	sprintf(buffer, " Report Lines: %d", i++);
	ERR_displayStr(buffer,FALSE);
    }   

    if (lastIsBlank)
	SPR_rowCount(spr)--;        

    free(buffer);
    return FALSE;
}

#endif


PublicFnDef int SPR_print (spr, page)
SPRSHEET *spr;
PAGE *page;
{
#if 0
    int c;

    c = ERR_promptForChar("Print to System or PC (s/p) ? ", "sSpP");
	
    if (c == 'p' || c == 'P')
	pcPrint(spr);
    else
    if (c == 's' || c == 'S')
	systemPrint(spr);
#endif

    print (page,-1);

#if 0
    if (0 == strncmp(PRINT_Command, "pc", 2))
	pcPrint(spr);
    else
#endif
    if (PRINT_Lines)
	systemPrint (spr);
    

    return (FALSE);
}

#define printLine(l)\
{\
    ptr = buffer;\
    for (col = 0; col < SPR_usedCols(spr); col++)\
    {\
	cell = SPR_cellPtr(spr, l, col);\
	sprintf(ptr, "%*s", SPR_cellSize(spr), SPR_cellValue(cell));\
	ptr += SPR_cellSize(spr);\
    }\
    strcat(buffer, "\n\0");\
    fputs(buffer, fd);\
    lineCount++;\
}

PublicFnDef int SPR_writeToFile(spr,filename)
SPRSHEET *spr;
char	*filename;
{
    int row,  col, i, lineCount = 0;
    char *cell;
    char *buffer, *ptr;
    FILE *fd, *fopen();

    ERR_displayStr(" Please wait...",FALSE);
    if( (fd = fopen(filename, "w")) == NULL )
    	return(TRUE);
    if( (buffer = malloc((SPR_cellSize(spr) * SPR_colCount(spr) * sizeof(char) * 2) + 2)) == NULL )
    {
    	fclose(fd);
    	return(TRUE);
    }

    for (row = 0; row < SPR_rowCount(spr); row++)
	printLine(row);
    fclose(fd);

    free(buffer);
    return(FALSE);
}

#if 0
PrivateVarDef int	doPC = FALSE;
#endif

PrivateFnDef int systemPrint (
    SPRSHEET *			spr
)
{
    int row,
        col,
        i,
        lineCount;
    char *cell;
    char *buffer,
         filename[80],
         cmd[128],
         *ptr;
    FILE *fd,
         *fopen();

    if (isBlank (PRINT_Command.command))
        {
	ERR_clearMsg ();
	return;
        }

/*    ERR_displayStr("Printing report, please wait...",FALSE);*/

    sprintf (filename, "/tmp/vision%d", getpid());

    fd = fopen (filename, "w");
    buffer = malloc ((SPR_cellSize (spr) * SPR_colCount (spr) * sizeof (char) * 2) + 2);

    lineCount = 0;
    for (row = 0; row < SPR_rowCount (spr); row++)
        {
	if (lineCount >= (PRINT_Lines - 6))
            {
	    for (i = 0; i < 6; i++)
		fputs ("\n\0", fd);
	    lineCount = 0;
	    for (i = 0; i < SPR_fixedRows (spr); i++)
		printLine(i);
	}
	printLine (row);
    }
    fclose (fd);

#if 0
    sprintf(cmd, "/bin/csh -if -c \"%s %c %s\"", 
	PRINT_Command, 
	(doPC ? ' ' : '<'),
	filename);

    RS_system(cmd);
#endif
    printFile (filename);
    remove (filename);

    free (buffer);
    ERR_clearMsg ();
    EDIT_displayErrorMessage ();
    CUR_clearok (CUR_curscr);	/*** need to repaint the screen ***/
    CUR_wrefresh (CUR_curscr);
}

#define printLine2(l)\
{\
    ptr = buffer;\
    for (col = 0; col < SPR_usedCols(spr); col++)\
    {\
	cell = SPR_cellPtr(spr, l, col);\
	sprintf(ptr, "%*s", SPR_cellSize(spr), SPR_cellValue(cell));\
	ptr += SPR_cellSize(spr);\
    }\
    puts(buffer);\
    lineCount++;\
}

#if 0
PrivateFnDef int pcPrint(spr)
SPRSHEET *spr;
{
 #if 0
    int row,  col, i, lineCount;
    char *cell;
    char *buffer, *ptr;
 #endif

    if( isBlank(PRINT_Command) )
    {
	ERR_clearMsg();
	return;
    }

    ERR_displayPause("Please Turn On PC Printer");
    doPC = TRUE;
    systemPrint(spr);
    doPC = FALSE;
 #if 0
    ERR_displayStr("Printing report, please wait...",FALSE);

    CUR_nl();
    putchar('\020');	/*** toggle printer ***/

    buffer = malloc(SPR_cellSize(spr) * SPR_colCount(spr) * sizeof(char) * 2);

    lineCount = 0;
    for (row = 0; row < SPR_rowCount(spr); row++)
    {
	if (lineCount >= (PRINT_Lines - 6))
	{
	    for (i = 0; i < 6; i++)
		puts("\n\0");
	    lineCount = 0;
	    for (i = 0; i < SPR_fixedRows(spr); i++)
		printLine2(i);
	}
	printLine2(row);
    }

    free(buffer);
    putchar('\020');	/*** toggle printer ***/

    CUR_nonl();
    CUR_werase(ERR_Window);
    CUR_clearok(CUR_curscr);	/*** need to repaint the screen ***/
    CUR_wrefresh(CUR_curscr);
 #endif
}
#endif

PrivateFnDef int isStringCell(str)
char *str;
{
    int  i, len;

    len = strlen(str);
    for( i=0 ; i<len ; i++ )
	if( isdigit(str[i]) || isspace(str[i]) || (str[i] == '.') || (str[i] == '-') )
		continue;
	else
		return(TRUE);
    return(FALSE);
}


PublicFnDef int SPR_download(spr)
SPRSHEET *spr;
{
    int row,  col;
    char *cell;
    char buffer[1024], filename[80], *ptr;
    char pcname[128], cmd[128];
    FILE *fd, *fopen();

    sprintf(filename, "/tmp/vision%d", getpid());

    fd = fopen(filename, "w");
    /*
    buffer = malloc(SPR_cellSize(spr) * SPR_colCount(spr) * sizeof(char) * 2);
    */

    for (row = 0; row < SPR_rowCount(spr); row++)
    {
	if( row < 5 )
	    continue;
	ptr = buffer;
	for (col = 0; col < SPR_usedCols(spr); col++)
	{
	    cell = SPR_cellPtr(spr, row, col);
	    if( col == 0 )
	    {
		if( strlen(SPR_cellValue(cell)) == 0 )
		    break;
		sprintf(ptr, "\"%s", SPR_cellValue(cell));
		ptr += (strlen(SPR_cellValue(cell)) + 1);
	    }
	    else if( col == 1 )
	    {
		sprintf(ptr, "%s\" ", SPR_cellValue(cell));
		ptr += (strlen(SPR_cellValue(cell)) + 2);
	    }
	    else if( isStringCell(SPR_cellValue(cell)) )
	    {
		sprintf(ptr, "\"%s\" ", SPR_cellValue(cell));
		ptr += (strlen(SPR_cellValue(cell)) + 3);
	    }
	    else
	    {
		sprintf(ptr, "%s ", SPR_cellValue(cell));
		ptr += (strlen(SPR_cellValue(cell)) + 1);
	    }
	}
	if( (col == 0) &&  (col < SPR_usedCols(spr)) )
	    continue;
	strcat(buffer, "\n\0");
	fputs(buffer, fd);
    }
    fclose(fd);

    if (ERR_promptForString("Enter name of file (.PRN added automatically) (F9 to abort): ", pcname, FALSE))
    {
	remove(filename);
	/*
	free(buffer);
	*/
	return;
    }
    else if (!strlen(pcname))
    {
	remove(filename);
	/*
	free(buffer);
	*/
	return;
    }
    strcat(pcname,".PRN");

    sprintf(cmd,"/bin/csh -if -c \"pcdownload %s %s\"",filename,pcname);
    ERR_displayStr("Please wait...",FALSE);
    CUR_saveterm();
    CUR_resetterm();
    RS_system(cmd);
    CUR_fixterm();
    CUR_noecho();
    CUR_nonl();
    CUR_cbreak();
    ERR_clearMsg();
    CUR_clearok(CUR_curscr);
    CUR_wrefresh(CUR_curscr);
    ERR_displayStr("Done!",TRUE);
    remove(filename);
	/*
    free(buffer);
	*/
}
