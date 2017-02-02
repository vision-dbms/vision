/********************************************************
 *********	sps.d				*********
 *******************************************************/
#ifndef SPS_D
#define SPS_D

#define SPS_maxItemWidth	80
#define SPS_maxHeadingWidth	40

#define LineBreakChar	'_'

#define FLOATDECIMALS	2

#define DIM	3

#define X	0
#define Y	1
#define Z	2
#define CELL	3

#define SPS_NOACTION	0
#define SPS_TRANSPOSE	1
#define SPS_ROTATE	2
#define SPS_INCREMENTZ	3
#define SPS_DECREMENTZ	4
#define SPS_CLEARINPUT	5
#define SPS_CLEARCELL	6
#define SPS_REFRESHPAGE	7

#define ItemsInFullListEntry		5

#define EntryItemName			0
#define EntryHeadingLabel		1
#define EntryItemType			2
#define EntryInputOrDisplay		3
#define EntryCellType			4

#define SPS_NOERROR			0
#define SPS_GOTPROMPT			-1
#define SPS_GOTNA			-2
#define SPS_GOTERROR			-3
#define SPS_FULLLISTNOTCOMPLETE 	-4

#define FULL		0
#define PARTIAL		1

typedef enum {	SPS_Normal, SPS_NA, SPS_Modified, SPS_ModifiedNA,
		SPS_Saved, SPS_SavedNA
} SPS_CellStatus;

typedef enum {	SPS_Variable, SPS_String, SPS_Float, SPS_Int,
		SPS_Date, SPS_RString, SPS_CString, SPS_Unknown
} SPS_DataType;

typedef enum { SPS_Display, SPS_Input, SPS_Locked
} SPS_IOType;

typedef struct {
    union {
	char	*s;
	double	f;
	int	d;
	int	i;
    } item;
    SPS_CellStatus status;
} SPS_cell;

typedef struct {
    union {
	char	*s;
	double	f;
	int	d;
	int	i;
    } item;
    SPS_CellStatus status;
    char *heading;		/* Displayed name of heading */
    SPS_DataType itemType;	/* Data type of internal name */
    int hwidth; 		 /* Width of heading */
    int hlwidth; 		 /* Width of heading */
    int owidth; 		 /* Optional Width of heading */
    int hlines;			/* Number of lines for heading */
    int cwidth; 		 /* Width of cells under heading */
    SPS_DataType cellType;	/* Data type of cells for this heading */
    SPS_IOType cellIO;		/* Input flag for cells in this heading */
} SPS_Heading;

typedef struct {
    int homeOffset[DIM]; /* Current offsets of screen home position within */
			 /* SPSHEET array */
    int screenOffset[DIM]; /* Offsets from current screen home */
    SPS_CellStatus status; /* Was spreadsheet modified ? */
    int XsOnScreen;	 /* # of SPSHEET cols on screen */
    int YsOnScreen;	 /* # of SPSHEET rows on screen */
    int ZsOnScreen;	 /* depth SPSHEET on screen */
    int screenCols;	 /* # of character positions on screen */
    int repaint;	 /* Flag to signal that screen needs repainting */
    int axisCount[DIM];  /* # of elements for each axis */
    int idx[DIM];	 /* Determines index to use in DIM'ed arrays */
    int cWidth[DIM];	 /* Maximum width of all cells */
    int hWidth[DIM];	 /* Maximum width of all headings */
    int hLines[DIM];	 /* Maximum lines of all headings */
    int cellsLocked;	 /* Flag which determines whether all cells are locked*/
    int allowTraAndRot;	 /* Flag which determines whether sheet can be */
			 /* transposed and/or rotated */
    SPS_Heading *headings[DIM]; /* Arrays of headings */
    SPS_cell	***cells; /* Array of cells in SPSHEET */
} SPSHEET;

#define maximum(a,b)	(((a)>(b))?(a):(b))
#define minimum(a,b)	(((a)<(b))?(a):(b))

#define SPS_XsOnScreen(spr)	(spr->XsOnScreen)
#define SPS_YsOnScreen(spr)	(spr->YsOnScreen)
#define SPS_ZsOnScreen(spr)	(spr->ZsOnScreen)
#define SPS_homeX(spr)		(spr->homeOffset[spr->idx[X]])
#define SPS_homeY(spr)		(spr->homeOffset[spr->idx[Y]])
#define SPS_homeZ(spr)		(spr->homeOffset[spr->idx[Z]])
#define SPS_screenX(spr)	(spr->screenOffset[spr->idx[X]])
#define SPS_screenY(spr)	(spr->screenOffset[spr->idx[Y]])
#define SPS_screenZ(spr)	(spr->screenOffset[spr->idx[Z]])
#define SPS_currX(spr)		(spr->homeOffset[spr->idx[X]] +\
				 spr->screenOffset[spr->idx[X]])
#define SPS_currY(spr)		(spr->homeOffset[spr->idx[Y]] +\
				 spr->screenOffset[spr->idx[Y]])
#define SPS_currZ(spr)		(spr->homeOffset[spr->idx[Z]] +\
				 spr->screenOffset[spr->idx[Z]])
#define SPS_screenCols(spr)	(spr->screenCols)
#define SPS_repaintScreen(spr)	(spr->repaint)
#define SPS_allowTransposeAndRotate(spr)	(spr->allowTraAndRot)
#define SPS_cellsLocked(spr)	(spr->cellsLocked)
#define SPS_sheetStatus(spr)	(spr->status)
#define SPS_XCount(spr) 	(spr->axisCount[spr->idx[X]])
#define SPS_YCount(spr) 	(spr->axisCount[spr->idx[Y]])
#define SPS_ZCount(spr) 	(spr->axisCount[spr->idx[Z]])
#define SPS_maxCellWidth(spr,axis) (spr->cWidth[spr->idx[axis]])
#define SPS_maxHWidth(spr,axis)    (spr->hWidth[spr->idx[axis]])
#define SPS_maxHLines(spr,axis)    (spr->hLines[spr->idx[axis]])
#define SPS_XType(spr,index)	(spr->headings[spr->idx[X]][index].itemType)
#define SPS_YType(spr,index)	(spr->headings[spr->idx[Y]][index].itemType)
#define SPS_ZType(spr,index)	(spr->headings[spr->idx[Z]][index].itemType)
#define SPS_displayHeading(h)	((h)->heading)
#define SPS_headingHWidth(h)	((h)->hwidth)
#define SPS_headingHLWidth(h)	((h)->hlwidth)
#define SPS_headingOWidth(h)	((h)->owidth)
#define SPS_headingLines(h)	((h)->hlines)
#define SPS_headingCWidth(h)	((h)->cwidth)
#define SPS_headingType(h)	((h)->itemType)
#define SPS_headingStatus(h)	((h)->status)
#define SPS_cellTypeH(h)	((h)->cellType)
#define SPS_headingPtr(spr,index,axis)  \
	(&spr->headings[spr->idx[axis]][index])
#define SPS_cellType(spr,index,axis) \
	(spr->headings[spr->idx[axis]][index].cellType)
#define SPS_xyzTypeWidth(spr,x,y,z,type,width) \
{\
   if( (width = spr->headings[spr->idx[X]][x].owidth) > 0 )\
   {\
       if( (type = SPS_cellType(spr,x,X)) != SPS_Variable ) ;\
       else if( (type = SPS_cellType(spr,z,Z)) != SPS_Variable ) ;\
       else if( (type = SPS_cellType(spr,y,Y)) != SPS_Variable ) ;\
       else type = SPS_cellTypeH(&SPS_GlobalSelectorHeading);\
   }\
   else\
   {\
       if( (type = SPS_cellType(spr,x,X)) != SPS_Variable ) \
	 width = spr->headings[spr->idx[X]][x].cwidth;\
       else if( (type = SPS_cellType(spr,z,Z)) != SPS_Variable ) \
	 width = SPS_headingCWidth(SPS_headingPtr(spr,z,Z));\
       else if( (type = SPS_cellType(spr,y,Y)) != SPS_Variable ) \
	 width = SPS_maxCellWidth(spr,Y);\
       else\
       {\
	type = SPS_cellTypeH(&SPS_GlobalSelectorHeading);\
	width = SPS_headingCWidth(&SPS_GlobalSelectorHeading);\
       }\
       width = maximum(width,spr->headings[spr->idx[X]][x].hlwidth);\
   }\
}
#define SPS_currTypeWidth(spr,type,width)\
  SPS_xyzTypeWidth(spr,SPS_currX(spr),SPS_currY(spr),SPS_currZ(spr),type,width)

#define SPS_currHeadingLines(spr)\
    maximum((spr->headings[spr->idx[Z]][SPS_currZ(spr)].hlines),\
	    (spr->hLines[spr->idx[X]]))

#define SPS_controllingHeaderXYZ(spr,x,y,z,h,axis) \
{\
   if( SPS_cellType(spr,x,X) != SPS_Variable ) \
   {\
     h = &spr->headings[spr->idx[X]][x];\
     axis = X;\
   }\
   else if( SPS_cellType(spr,z,Z) != SPS_Variable ) \
   {\
     h = &spr->headings[spr->idx[Z]][z];\
     axis = Z;\
   }\
   else if( SPS_cellType(spr,y,Y) != SPS_Variable ) \
   {\
     h = &spr->headings[spr->idx[Y]][y];\
     axis = Y;\
   }\
   else\
   {\
     h = &SPS_GlobalSelectorHeading;\
     axis = -1;\
   }\
}

#define SPS_controllingHeaderCurr(spr,h,axis)\
  SPS_controllingHeaderXYZ(spr,SPS_currX(spr),SPS_currY(spr),SPS_currZ(spr),h,axis)

#define SPS_cellPtrCurr(spr)\
	SPS_cellPtrXYZ(spr,SPS_currX(spr),SPS_currY(spr),SPS_currZ(spr))
#define SPS_cellString(c)	((c)->item.s)
#define SPS_cellFloat(c)	((c)->item.f)
#define SPS_cellDate(c) 	((c)->item.d)
#define SPS_cellInt(c)		((c)->item.i)
#define SPS_cellStatus(c)	((c)->status)
#define SPS_headingString(h)	((h)->item.s)
#define SPS_headingFloat(h)	((h)->item.f)
#define SPS_headingDate(h)	((h)->item.d)
#define SPS_headingInt(h)	((h)->item.i)

#define SPS_headingCellIO(h)	((h)->cellIO)
#define SPS_cellIO(spr,index,axis) \
	(spr->headings[spr->idx[axis]][index].cellIO)
#define SPS_isInputXYZ(spr,x,y,z)\
	((SPS_cellIO(spr,x,X) == SPS_Input) ||\
	 (SPS_cellIO(spr,y,Y) == SPS_Input) ||\
	 (SPS_cellIO(spr,z,Z) == SPS_Input))
#define SPS_isInputCurr(spr)\
	SPS_isInputXYZ(spr,SPS_currX(spr),SPS_currY(spr),SPS_currZ(spr))
#define SPS_isLockedXYZ(spr,x,y,z)\
	((SPS_cellIO(spr,x,X) == SPS_Locked) ||\
	 (SPS_cellIO(spr,y,Y) == SPS_Locked) ||\
	 (SPS_cellIO(spr,z,Z) == SPS_Locked))
#define SPS_isLockedCurr(spr)\
	SPS_isLockedXYZ(spr,SPS_currX(spr),SPS_currY(spr),SPS_currZ(spr))


#define SPS_resetIndices(spr)\
{\
    spr->idx[X] = X;\
    spr->idx[Y] = Y;\
    spr->idx[Z] = Z;\
}

#define SPS_makeSheet(spr, x, y, z)\
{\
    int i, j;\
    if( NULL == (spr = (SPSHEET *)calloc(1, sizeof(SPSHEET))) )\
	ERR_fatal(" Error allocating space (make sheet)");\
    SPS_resetIndices(spr);\
    SPS_XCount(spr) = x;\
    SPS_YCount(spr) = y;\
    SPS_ZCount(spr) = z;\
    spr->cWidth[X] = 1;\
    spr->cWidth[Y] = 1;\
    spr->cWidth[Z] = 1;\
    spr->hWidth[X] = 1;\
    spr->hWidth[Y] = 1;\
    spr->hWidth[Z] = 1;\
    spr->hLines[X] = 1;\
    spr->hLines[Y] = 1;\
    spr->hLines[Z] = 1;\
    spr->cellsLocked = FALSE;\
    if( NULL == \
	(spr->headings[spr->idx[X]] = \
	 (SPS_Heading *)calloc(x,sizeof(SPS_Heading))) )\
	    ERR_fatal(" Error allocating space (make sheet)");\
    if( NULL == \
	(spr->headings[spr->idx[Y]] = \
	 (SPS_Heading *)calloc(y,sizeof(SPS_Heading))) )\
	    ERR_fatal(" Error allocating space (make sheet)");\
    if( NULL == \
	(spr->headings[spr->idx[Z]] = \
	 (SPS_Heading *)calloc(z,sizeof(SPS_Heading))) )\
	    ERR_fatal(" Error allocating space (make sheet)");\
    if( NULL == \
	(spr->cells = (SPS_cell ***)calloc(x,sizeof(SPS_cell **))) )\
	    ERR_fatal(" Error allocating space (make sheet)");\
    for( i=0 ; i<x ; i++ )\
    {\
	if( NULL == \
	    (spr->cells[i] = (SPS_cell **)calloc(y,sizeof(SPS_cell *))) )\
		ERR_fatal(" Error allocating space (make sheet)");\
	for( j=0 ; j<y ; j++ )\
	    if( NULL == \
		(spr->cells[i][j] = (SPS_cell *)calloc(z,sizeof(SPS_cell))) )\
		    ERR_fatal(" Error allocating space (make sheet)");\
    }\
    if( SPS_hBuf[0] == NULL )\
        SPS_makeHeadingBuf(SPS_hBuf);\
}


#define SPS_addString(dest,str)\
{\
    if( (str == NULL) || (strlen(str) == 0) )\
    {\
	if( NULL == (dest = (char *)calloc(1,sizeof(char))) )\
		ERR_fatal(" Error allocating space (addString)");\
	dest[0] = '\0';\
    }\
    else\
    {\
	if( NULL == (dest = (char *)calloc(strlen(str)+1,sizeof(char))) )\
		ERR_fatal(" Error allocating space (addString)");\
	strcpy(dest,str);\
    }\
}

#define	SPS_makeHeadingBuf(buf)\
{\
    int	ii;\
    for( ii=0 ; ii<ItemsInFullListEntry ; ii++ )\
	if( NULL == (buf[ii] = (char *)calloc(SPS_maxItemWidth+1,sizeof(char))) )\
	    ERR_fatal(" Error allocating space (make "#buf")");\
}

#define	SPS_freeHeadingBuf(buf)\
{\
    int	ii;\
    for( ii=0 ; ii<ItemsInFullListEntry ; ii++ )\
	if( buf[ii] != NULL )\
	{\
	    free(buf[ii]);\
	    buf[ii] = NULL;\
	}\
}


#define SPS_goToBOL(spr, win)\
{\
    if( SPS_homeX(spr) > 0 )\
    {\
	SPS_repaintScreen(spr) = TRUE;\
    }\
    SPS_homeX(spr) = 0;\
    SPS_screenX(spr) = 0;\
    SPS_recalcCols(spr,win);\
    if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) )\
	SPS_keyRight(spr,win);\
}

#define SPS_goToTop(spr, win)\
{\
    if( (SPS_homeY(spr) > 0) || (SPS_homeX(spr) > 0) )\
    {\
	SPS_repaintScreen(spr) = TRUE;\
    }\
    SPS_homeY(spr) = 0;\
    SPS_homeX(spr) = 0;\
    SPS_screenY(spr) = 0;\
    SPS_screenX(spr) = 0;\
    SPS_recalcCols(spr,win);\
    if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) )\
	SPS_keyRight(spr,win);\
}


#define SPS_goToBottom(spr,win)\
{\
    if (SPS_YCount(spr) > SPS_YsOnScreen(spr))\
    {\
	SPS_homeY(spr) = SPS_YCount(spr) - SPS_YsOnScreen(spr);\
	SPS_screenY(spr) = SPS_YsOnScreen(spr) - 1;\
	SPS_repaintScreen(spr) = TRUE;\
    }\
    else\
	SPS_screenY(spr) = SPS_YCount(spr) - 1;\
    SPS_goToEOL(spr,win,TRUE);\
}


#define SPS_scrollDown(spr,win)\
{\
    if( (SPS_YCount(spr) > SPS_YsOnScreen(spr)) &&\
	(SPS_homeY(spr) < (SPS_YCount(spr) - SPS_YsOnScreen(spr))) )\
    {\
	if( (SPS_homeY(spr) + SPS_YsOnScreen(spr)) <= \
	    (SPS_YCount(spr) - SPS_YsOnScreen(spr)) )\
	{\
	    SPS_homeY(spr) += SPS_YsOnScreen(spr);\
	}\
	else\
	{\
	    SPS_homeY(spr) = SPS_YCount(spr) - SPS_YsOnScreen(spr);\
	}\
	SPS_screenY(spr) = 0;\
	SPS_repaintScreen(spr) = TRUE;\
	SPS_recalcCols(spr,win);\
	if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) )\
	    SPS_keyRight(spr,win);\
    }\
}


#define SPS_scrollUp(spr,win)\
{\
    if( (SPS_YCount(spr) > SPS_YsOnScreen(spr)) && (SPS_homeY(spr) > 0) )\
    {\
	SPS_homeY(spr) -= SPS_YsOnScreen(spr);\
	if( SPS_homeY(spr) < 0 )\
	    SPS_homeY(spr) = 0;\
	SPS_screenY(spr) = 0;\
	SPS_repaintScreen(spr) = TRUE;\
	SPS_recalcCols(spr,win);\
	if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) )\
	    SPS_keyLeft(spr,win);\
    }\
}

#define SPS_scrollRight(spr, win)\
{\
    if( (SPS_XCount(spr) > SPS_XsOnScreen(spr)) && \
	((SPS_homeX(spr) + SPS_XsOnScreen(spr)) < SPS_XCount(spr)) )\
    {\
	if( SPS_XsOnScreen(spr) == 0 )\
	    SPS_homeX(spr)++;\
	else\
	{\
	    SPS_homeX(spr) += SPS_XsOnScreen(spr);\
	    SPS_recalcCols(spr, win);\
	    if( (SPS_homeX(spr) + SPS_XsOnScreen(spr)) >= SPS_XCount(spr) )\
	    {\
		while( (SPS_homeX(spr) + SPS_XsOnScreen(spr)) >= SPS_XCount(spr) )\
		{\
		    SPS_homeX(spr)--;\
		    if( SPS_homeX(spr) < 0 )\
			    break;\
		    SPS_recalcCols(spr, win);\
		}\
		SPS_homeX(spr)++;\
	    }\
	    if( SPS_homeX(spr) >= SPS_XCount(spr) )\
		SPS_homeX(spr) = SPS_XCount(spr) - 1;\
	}\
	SPS_screenX(spr) = 0;\
	SPS_recalcCols(spr, win);\
	SPS_repaintScreen(spr) = TRUE;\
	if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) )\
	    SPS_keyDown(spr,win);\
    }\
}

#define SPS_scrollLeft(spr, win)\
{\
    int	oldx;\
    if( (SPS_XCount(spr) > SPS_XsOnScreen(spr)) || \
        (SPS_homeX(spr) > 0) ) \
    {\
	oldx = SPS_homeX(spr);\
	while( (SPS_homeX(spr) + SPS_XsOnScreen(spr)) > oldx )\
	{\
	    SPS_homeX(spr)--;\
	    if( SPS_homeX(spr) < 0 )\
		    break;\
	    SPS_recalcCols(spr, win);\
	}\
	if( SPS_homeX(spr) <= 0 )\
	    SPS_homeX(spr) = 0;\
	SPS_recalcCols(spr,win);\
	SPS_screenX(spr) = 0;\
	SPS_repaintScreen(spr) = TRUE;\
    }\
}

#define SPS_setWidth(width,type,str)\
switch(type)\
{\
	case SPS_String:\
	case SPS_RString:\
	case SPS_CString:\
		width = strlen(str) + 1;\
		if( width >= SPS_maxHeadingWidth )\
			width = SPS_maxHeadingWidth;\
		break;\
	default:\
		width = 12;\
		break;\
}

#define SPS_setCellWidth(h,type,str) \
	SPS_setWidth(SPS_headingCWidth(h),type,str)

#define	SPS_highlightCell(spr,win)\
{\
	CUR_wattron(win,CUR_A_REVERSE);\
	SPS_printCell(spr,win,SPS_screenX(spr),SPS_screenY(spr));\
	CUR_wattroff(win,CUR_A_REVERSE);\
}

#define	SPS_unhighlightCell(spr,win)\
{\
	CUR_wattroff(win,CUR_A_REVERSE);\
	SPS_printCell(spr,win,SPS_screenX(spr),SPS_screenY(spr));\
}

#endif
