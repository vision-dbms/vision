/********************************************************
 *********	spsheet.d			*********
 *******************************************************/
#ifndef SPR_D
#define SPR_D

#define SPR_initsize    200
#define SPR_addsize     200

typedef enum { SPR_Empty, SPR_Normal, SPR_Error, SPR_Modified, 
} SPR_CellStatus;

typedef enum {SPR_Norm, SPR_InitBottom, 
} SPR_SheetStatus;

typedef enum {SPR_Item, SPR_Line, 
} SPR_FieldType;

typedef struct {
    char item[256];
    char heading[20];
    char width[10];
    char decimals[10];
    char total[15];
    SPR_FieldType type;
} SPR_Field;

typedef struct {
    int rowOnScreen[CUR_maxScrLines];    /**** array on rows on screen ****/
    int colOnScreen[CUR_maxScrCols];    /**** array cols on screen ****/
    int currRow;	    /**** current screen row ****/
    int currCol;	    /**** current screen col ****/
    int screenCols;
    int lastCol;
    int titleRows;
    int fixedRows;
    int fixedCols;
    int repaint;
    int rowCount;
    int colCount;
    int cellSize;
    int cellLen;
    char *rows;
    int rowsSize;
    SPR_SheetStatus status;
    int fieldsSize;
    SPR_Field **fields;
    int fieldCount;
} SPRSHEET;

#define SPR_rowOnScreen(spr, n)	    (spr->rowOnScreen[n])
#define SPR_colOnScreen(spr, n)	    (spr->colOnScreen[n])
#define SPR_currRow(spr)	    (spr->currRow)
#define SPR_currCol(spr)	    (spr->currCol)
#define SPR_screenCols(spr)	    (spr->screenCols)
#define SPR_lastCol(spr)	    (spr->lastCol)
#define SPR_usedCols(spr)	    (spr->lastCol + 1)
#define SPR_fixedRows(spr)	    (spr->fixedRows + spr->titleRows)
#define SPR_scrollRows(spr)	    (spr->fixedRows)
#define SPR_titleRows(spr)	    (spr->titleRows)
#define SPR_fixedCols(spr)	    (spr->fixedCols)
#define SPR_repaintScreen(spr)	    (spr->repaint)
#define SPR_rowCount(spr)	    (spr->rowCount)
#define SPR_colCount(spr)	    (spr->colCount)
#define SPR_cellSize(spr)	    (spr->cellSize)
#define SPR_cellLen(spr)	    (spr->cellLen)

#define SPR_rowsSize(spr)	    (spr->rowsSize)
#define SPR_status(spr)		    (spr->status)

#define SPR_cellPtr(spr, row, col)\
    (spr->rows + (row * (spr->colCount * spr->cellLen)) + (col * spr->cellLen))
#define SPR_cellValue(c)	    (c + sizeof(SPR_CellStatus))
#define SPR_cellStatus(c)	    *((SPR_CellStatus *)c)

#define SPR_fieldsSize(spr)	    (spr->fieldsSize)
#define SPR_field(spr, n)	    (spr->fields[n])
#define SPR_fieldCount(spr)	    (spr->fieldCount)
#define SPR_item(field)		    (field->item)
#define SPR_heading(field)	    (field->heading)
#define SPR_width(field)	    (field->width)
#define SPR_decimals(field)	    (field->decimals)
#define SPR_totalType(field)	    (field->total)
#define SPR_fieldType(field)	    (field->type)


#define SPR_makeSheet(spr, size, cols, fcols, trows, frows)\
{\
    spr = (SPRSHEET *)calloc(1, sizeof(SPRSHEET));\
    SPR_cellSize(spr) = size;\
    SPR_cellLen(spr) = (sizeof(SPR_CellStatus) +\
         ((((SPR_cellSize(spr) + 1) / sizeof(int)) + 1) * sizeof(int)));\
    SPR_colCount(spr) = cols;\
    SPR_fixedCols(spr) = SPR_currCol(spr) = fcols;\
    SPR_titleRows(spr) = trows;\
    SPR_scrollRows(spr) = frows;\
    SPR_currRow(spr) = SPR_fixedRows(spr);\
    SPR_status(spr) = SPR_Norm;\
}


#define SPR_addField(spr, ptr)\
{\
    if (SPR_fieldsSize(spr) == 0)\
    {\
	if (NULL ==\
	(spr->fields = (SPR_Field **) calloc(10, sizeof(SPR_Field *))))\
	    ERR_fatal(" Error allocating space");\
	SPR_fieldsSize(spr) = 10;\
    }\
    if (SPR_fieldCount(spr) >= SPR_fieldsSize(spr))\
    {\
        if (NULL == (ptr = (char*)realloc(spr->fields,\
	(SPR_fieldsSize(spr) + 10) * sizeof(SPR_Field *))))\
	    ERR_fatal(" Error allocating space");\
	spr->fields = (SPR_Field **)ptr;\
	SPR_fieldsSize(spr) += 10;\
    }\
    if (NULL == (SPR_field(spr, SPR_fieldCount(spr)) =\
			    (SPR_Field *)calloc(1, sizeof(SPR_Field))))\
        ERR_fatal(" Error allocating space");\
    SPR_fieldCount(spr)++;\
}


/*********************************************************
 *********	Cursor Movement Operations	**********
 ********************************************************/
#define SPR_moveCursor(win, spr, row, col)\
{\
    wmove(win, row , ((col + 1) * SPR_cellSize(spr)) - 1);\
}

#define SPR_goToTop(spr, win, i, row, col)\
{\
    if (SPR_rowCount(spr) > CUR_WIN_rows(win))\
    {\
	for (i = 0; i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) = i;\
    }\
    if (SPR_usedCols(spr) > SPR_screenCols(spr))\
    {\
	for (i = 0; i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) = i;\
    }\
    row = SPR_fixedRows(spr);\
    col = SPR_fixedCols(spr);\
    SPR_repaintScreen(spr) = TRUE;\
}


#define SPR_goToBottom(spr, win, i, row, col)\
{\
    if (SPR_rowCount(spr) > CUR_WIN_rows(win))\
    {\
	for (i = SPR_fixedRows(spr);\
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) = SPR_rowCount(spr) - (CUR_WIN_rows(win) - i);\
    }\
    if (SPR_usedCols(spr) > SPR_screenCols(spr))\
    {\
	for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) = \
	    SPR_usedCols(spr) - (SPR_screenCols(spr) - i);\
    }\
    row = CUR_WIN_rows(win) - 1;\
    col = SPR_screenCols(spr) - 1;\
    SPR_repaintScreen(spr) = TRUE;\
}


#define SPR_keyDown(spr, win, row, i)\
{\
    if (row == (CUR_WIN_rows(win) - 1))\
    {\
	if (SPR_rowCount(spr) <= CUR_WIN_rows(win))\
	    row = SPR_fixedRows(spr);\
	else\
	if (SPR_rowOnScreen(spr, row) == (SPR_rowCount(spr) - 1))\
	{\
	    for (i = SPR_fixedRows(spr);\
		 i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
		SPR_rowOnScreen(spr, i) = i;\
	    row = SPR_fixedRows(spr);\
	    SPR_repaintScreen(spr) = TRUE;\
	}\
	else\
	{\
	    for (i = SPR_fixedRows(spr);\
		 i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
		SPR_rowOnScreen(spr, i)++;\
		SPR_repaintScreen(spr) = TRUE;\
	}\
    }\
    else\
	row++;\
}


#define SPR_scrollDown1(spr, win, row, i)\
{\
    if( SPR_rowCount(spr) > CUR_WIN_rows(win) ) \
    {\
	for (i = SPR_fixedRows(spr);\
		 i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    if( SPR_rowOnScreen(spr, i) == (SPR_rowCount(spr) - 1) )\
	    	break;\
	if( (i >= CUR_WIN_rows(win)) || (i >= SPR_rowCount(spr)) )\
	{\
	    for (i = SPR_fixedRows(spr); i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
		SPR_rowOnScreen(spr, i)++;\
	    if( row > SPR_fixedRows(spr) )\
	    	row--;\
	    SPR_repaintScreen(spr) = TRUE;\
	}\
    }\
}

#define SPR_scrollUp1(spr, win, row, i)\
{\
    if( (SPR_rowCount(spr) > CUR_WIN_rows(win)) &&\
        (SPR_rowOnScreen(spr, SPR_fixedRows(spr)) > SPR_fixedRows(spr)) )\
    {\
	for (i = SPR_fixedRows(spr); i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i)--;\
	if( row < (CUR_WIN_rows(win) - 1) )\
	    row++;\
	SPR_repaintScreen(spr) = TRUE;\
    }\
}


#define SPR_keyUp(spr, win, row, i)\
{\
    if (row == SPR_fixedRows(spr))\
    {\
	if (SPR_rowCount(spr) <= CUR_WIN_rows(win))\
	    row = CUR_WIN_rows(win) - 1;\
	else\
	if (SPR_rowOnScreen(spr, row) != SPR_fixedRows(spr))\
	{\
	    for (i = SPR_fixedRows(spr);\
		 i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
		SPR_rowOnScreen(spr, i)--;\
		SPR_repaintScreen(spr) = TRUE;\
	}\
	else\
	{\
	    for (i = SPR_fixedRows(spr);\
		 i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) = SPR_rowCount(spr) - (CUR_WIN_rows(win) - i);\
	    row = CUR_WIN_rows(win) - 1;\
	    SPR_repaintScreen(spr) = TRUE;\
	}\
    }\
    else\
    {\
	row--;\
    }\
}


#define SPR_keyLeft(spr, win, col, i)\
{\
    if (col == SPR_fixedCols(spr))\
    {\
	if (SPR_usedCols(spr) <= SPR_screenCols(spr))\
	    col = SPR_screenCols(spr) - 1;\
	else\
	if (SPR_colOnScreen(spr, col) == SPR_fixedCols(spr))\
	{\
	    for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
		SPR_colOnScreen(spr, i) = \
		SPR_usedCols(spr) - (SPR_screenCols(spr) - i);\
	    col = SPR_screenCols(spr) - 1;\
	    SPR_repaintScreen(spr) = TRUE;\
	}\
	else\
	{\
	    for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
		SPR_colOnScreen(spr, i)--;\
		SPR_repaintScreen(spr) = TRUE;\
	}\
    }\
    else\
	col--;\
}


#define SPR_keyRight(spr, win, col, i)\
{\
    if (col == (SPR_screenCols(spr) - 1))\
    {\
	if (SPR_usedCols(spr) <= SPR_screenCols(spr))\
	    col = SPR_fixedCols(spr);\
	else\
	if (SPR_colOnScreen(spr, col) == SPR_lastCol(spr))\
	{\
	    for (i = 0; i < SPR_screenCols(spr); i++)\
		SPR_colOnScreen(spr, i) = i;\
	    col = SPR_fixedCols(spr);\
	    SPR_repaintScreen(spr) = TRUE;\
	}\
	else\
	{\
	    for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
		SPR_colOnScreen(spr, i)++;\
		SPR_repaintScreen(spr) = TRUE;\
	}\
    }\
    else\
	col++;\
}


#define SPR_scrollDown(spr, win, i, j)\
{\
    if (SPR_rowCount(spr) <= CUR_WIN_rows(win))\
        break;\
    else\
    if (SPR_rowOnScreen(spr, CUR_WIN_rows(win) - 1) != \
	    (SPR_rowCount(spr) - 1))\
    {\
	j = (SPR_rowCount(spr) - 1) -\
	    SPR_rowOnScreen(spr, CUR_WIN_rows(win) - 1);\
	if (j > (CUR_WIN_rows(win) - SPR_fixedRows(spr)))\
	    j = (CUR_WIN_rows(win) - SPR_fixedRows(spr));\
	for (i = SPR_fixedRows(spr);\
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) += j;\
	    SPR_repaintScreen(spr) = TRUE;\
    }\
    else\
    {\
	for (i = SPR_fixedRows(spr);\
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) = i;\
	SPR_repaintScreen(spr) = TRUE;\
    }\
}


#define SPR_scrollUp(spr, win, i, j)\
{\
    if (SPR_rowCount(spr) <= CUR_WIN_rows(win))\
        break;\
    else\
    if (SPR_rowOnScreen(spr, SPR_fixedRows(spr)) != SPR_fixedRows(spr))\
    {\
	j = SPR_rowOnScreen(spr, SPR_fixedRows(spr)) - SPR_fixedRows(spr);\
	if (j > (CUR_WIN_rows(win) - SPR_fixedRows(spr)))\
	    j = (CUR_WIN_rows(win) - SPR_fixedRows(spr));\
	for (i = SPR_fixedRows(spr);\
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) -= j;\
	    SPR_repaintScreen(spr) = TRUE;\
    }\
    else\
    {\
	for (i = SPR_fixedRows(spr);\
	     i < CUR_WIN_rows(win) && i < SPR_rowCount(spr); i++)\
	    SPR_rowOnScreen(spr, i) = SPR_rowCount(spr) - (CUR_WIN_rows(win) - i);\
	SPR_repaintScreen(spr) = TRUE;\
    }\
}

#define SPR_scrollRight(spr, win, i, j)\
{\
    if (SPR_usedCols(spr) <= SPR_screenCols(spr))\
        break;\
    else\
    if (SPR_colOnScreen(spr, SPR_screenCols(spr) - 1) \
	!= SPR_lastCol(spr))\
    {\
	j = SPR_lastCol(spr) - \
	    SPR_colOnScreen(spr, SPR_screenCols(spr) - 1);\
	if (j > (SPR_screenCols(spr) - SPR_fixedCols(spr)))\
	    j = (SPR_screenCols(spr) - SPR_fixedCols(spr));\
	for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) += j;\
	    SPR_repaintScreen(spr) = TRUE;\
    }\
    else\
    {\
	for (i = 0; i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) = i;\
	SPR_repaintScreen(spr) = TRUE;\
    }\
}

#define SPR_scrollLeft(spr, win, i, j)\
{\
    if (SPR_usedCols(spr) <= SPR_screenCols(spr))\
        break;\
    else\
    if (SPR_colOnScreen(spr, SPR_fixedCols(spr)) != SPR_fixedCols(spr))\
    {\
	j = SPR_colOnScreen(spr, SPR_fixedCols(spr)) - SPR_fixedCols(spr);\
	if (j > (SPR_screenCols(spr) - SPR_fixedCols(spr)))\
	    j = (SPR_screenCols(spr) - SPR_fixedCols(spr));\
	for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) -= j;\
	    SPR_repaintScreen(spr) = TRUE;\
    }\
    else\
    {\
	for (i = SPR_fixedCols(spr); i < SPR_screenCols(spr); i++)\
	    SPR_colOnScreen(spr, i) = \
	    SPR_usedCols(spr) - (SPR_screenCols(spr) - i);\
	col = SPR_screenCols(spr) - 1;\
	SPR_repaintScreen(spr) = TRUE;\
    }\
}

#define SPR_resetCells(spr, cell, i, j)\
{\
    for (i = 0; i < SPR_rowCount(spr); i++)\
    {\
        for (j = 0; j < SPR_colCount(spr); j++)\
	{\
	    cell = SPR_cellPtr(spr, i, j);\
	    if (SPR_cellStatus(cell) == SPR_Modified)\
	        SPR_cellStatus(cell) == SPR_Normal;\
	}\
    }\
}

#endif
