/*********************************************************
 **********		sps.c			**********
 *********************************************************/

#include "Vk.h"

#include "sps.h"

/********** /usr/rs/lib/system **********/
#include "stdcurses.h"

/********** /usr/rs/lib/interface **********/
#include "page.h"
#include "window.h"
#include "keys.h"
#include "error.h"
#include "rsInterface.h"

/***** Spread Sheet defines *****/
#include "sps.d"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef void SPS_checkForLockedSheet(SPSHEET *spr);

PrivateFnDef int SPS_getHeadingEntry(void);
PrivateFnDef void SPS_setHeadingEntry(
    SPS_Heading *h, int flag
);

PrivateFnDef int SPS_recalcCols (
    SPSHEET *			spr,
    CUR_WINDOW *		win
);

PrivateFnDef void SPS_keyLeft (
    SPSHEET *			spr,
    CUR_WINDOW *		win
);

PrivateFnDef void SPS_keyRight (
    SPSHEET *			spr,
    CUR_WINDOW *		win
);

PrivateFnDef PAGE_Action initSprSh (
    SPSHEET *			spsheet,
    CUR_WINDOW *		win
);

PrivateFnDef PAGE_Action inputSprSh (
    SPSHEET *			spsheet,
    CUR_WINDOW *		win
);

PrivateFnDef int getDate (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    int				width,
    int				firstChar
);

PrivateFnDef int getIntOrFloat (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    SPS_DataType		type,
    int				width,
    int				firstChar
);

PrivateFnDef int getString (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    int				width,
    int				firstChar
);


/*************************************************
 **********	Constants and Globals	**********
 *************************************************/

PublicFnDecl char	*eatLeadingAndTrailingSpaces();

#define LINES_PER_PAGE	66

PrivateVarDef char	ErrorString[] = ">>> Selector";

PrivateVarDef int	MenuOptionSelected = SPS_NOACTION;
PrivateVarDef int	AlreadyHereInLeft = FALSE;
PrivateVarDef int	AlreadyHereInRight = FALSE;
PrivateVarDef int	LastXWidth = 0;
PrivateVarDef int	TotalScreenWidth = 0;
PrivateVarDef int	CenteringOffset = 0;
PublicVarDef int	SPS_MaxHeadingLen = 0;
PublicVarDef int	SPS_MaxHeadingLines = 0;
PublicVarDef char	SPS_GlobalObject[SPS_maxItemWidth+1];
PublicVarDef SPS_Heading SPS_GlobalSelectorHeading;
PublicVarDef char	*SPS_hBuf[ItemsInFullListEntry] = {
				NULL, NULL, NULL, NULL, NULL };

PublicFnDef void SPS_refreshPage(void) {
	MenuOptionSelected = SPS_REFRESHPAGE;
}

PublicFnDef void SPS_clearCell(void) {
	MenuOptionSelected = SPS_CLEARCELL;
}

PublicFnDef void SPS_doClearCell(
    SPSHEET *spr, CUR_WINDOW *win
) {
    int x, y, z;
    SPS_cell *c;

    c = SPS_cellPtrCurr(spr);
    if( (SPS_cellStatus(c) == SPS_NA) ||
	(SPS_cellStatus(c) == SPS_ModifiedNA) ||
	(SPS_cellStatus(c) == SPS_SavedNA) )
	SPS_cellStatus(c) = SPS_NA;
    else
	SPS_cellStatus(c) = SPS_Normal;
    SPS_repaintScreen(spr) = TRUE;
    SPS_sheetStatus(spr) = SPS_Normal;
    for( x=0 ; x<SPS_XCount(spr) ; x++ )
    {
	for( y=0 ; y<SPS_YCount(spr) ; y++ )
	{
	    for( z=0 ; z<SPS_ZCount(spr) ; z++ )
	    {
		if( SPS_isInputXYZ(spr,x,y,z) )
		{
		    c = SPS_cellPtrXYZ(spr,x,y,z);
		    if( (SPS_cellStatus(c) == SPS_ModifiedNA) ||
			(SPS_cellStatus(c) == SPS_Modified) )
		    {
			SPS_sheetStatus(spr) = SPS_Modified;
			return;
		    }
		}
	    }
	}
    }
}

PublicFnDef void SPS_clearInput(void) {
	MenuOptionSelected = SPS_CLEARINPUT;
}

PublicFnDef int SPS_doClearInput(
    SPSHEET *spr, CUR_WINDOW *win
) {
    int x, y, z;
    SPS_cell *c;

    for( x=0 ; x<SPS_XCount(spr) ; x++ )
    {
	for( y=0 ; y<SPS_YCount(spr) ; y++ )
	{
	    for( z=0 ; z<SPS_ZCount(spr) ; z++ )
	    {
		if( SPS_isInputXYZ(spr,x,y,z) )
		{
		    c = SPS_cellPtrXYZ(spr,x,y,z);
		    if( (SPS_cellStatus(c) == SPS_NA) ||
			(SPS_cellStatus(c) == SPS_ModifiedNA) ||
			(SPS_cellStatus(c) == SPS_SavedNA) )
			SPS_cellStatus(c) = SPS_NA;
		    else
			SPS_cellStatus(c) = SPS_Normal;
		}
	    }
	}
    }
    SPS_sheetStatus(spr) = SPS_Normal;
    SPS_repaintScreen(spr) = TRUE;
}

PublicFnDef void SPS_transposeAxes(void) {
	MenuOptionSelected = SPS_TRANSPOSE;
}

PrivateFnDef void doTranspose(
    SPSHEET *spsheet, CUR_WINDOW *win
) {
	int i;
	if( !SPS_allowTransposeAndRotate(spsheet) )
	{
	    ERR_displayStr("Operation not permitted for current worksheet",TRUE);
	    return;
	}
	i = spsheet->idx[X];
	spsheet->idx[X] = spsheet->idx[Y];
	spsheet->idx[Y] = i;
	SPS_repaintScreen(spsheet) = TRUE;
	SPS_homeY(spsheet) = SPS_screenY(spsheet) = 0;
	SPS_homeX(spsheet) = SPS_screenX(spsheet) = 0;
	SPS_recalcCols(spsheet,win);
	SPS_checkForLockedSheet(spsheet);
	if( !SPS_cellsLocked(spsheet) && SPS_isLockedCurr(spsheet) && !SPS_isInputCurr(spsheet) )
	    SPS_keyRight(spsheet,win);
}

PublicFnDef void SPS_rotateAxes(void) {
	MenuOptionSelected = SPS_ROTATE;
}

PrivateFnDef void doRotate(
    SPSHEET *spsheet, CUR_WINDOW *win
) {
	int i;
	if( !SPS_allowTransposeAndRotate(spsheet) )
	{
	    ERR_displayStr("Operation not permitted for current worksheet",TRUE);
	    return;
	}
	i = spsheet->idx[X];
	spsheet->idx[X] = spsheet->idx[Y];
	spsheet->idx[Y] = spsheet->idx[Z];
	spsheet->idx[Z] = i;
	SPS_repaintScreen(spsheet) = TRUE;
	SPS_homeZ(spsheet) = SPS_screenZ(spsheet) = 0;
	SPS_homeY(spsheet) = SPS_screenY(spsheet) = 0;
	SPS_homeX(spsheet) = SPS_screenX(spsheet) = 0;
	SPS_recalcCols(spsheet,win);
	SPS_checkForLockedSheet(spsheet);
	if( !SPS_cellsLocked(spsheet) && SPS_isLockedCurr(spsheet) && !SPS_isInputCurr(spsheet) )
	    SPS_keyRight(spsheet,win);
}

PublicFnDef void SPS_incrementZ(void) {
	MenuOptionSelected = SPS_INCREMENTZ;
}

PrivateFnDef void doIncrement(
    SPSHEET *spsheet, CUR_WINDOW *win
) {
	if( SPS_ZCount(spsheet) == 1 )
	    return;
	if( SPS_currZ(spsheet) == (SPS_ZCount(spsheet) - 1) )
	{
	    SPS_homeZ(spsheet) = 0;
	    SPS_screenZ(spsheet) = 0;
	}
	else
	{
	    SPS_homeZ(spsheet) = 0;
	    SPS_screenZ(spsheet)++;
	}
	SPS_repaintScreen(spsheet) = TRUE;
	SPS_recalcCols(spsheet,win);
	SPS_checkForLockedSheet(spsheet);
	if( !SPS_cellsLocked(spsheet) && SPS_isLockedCurr(spsheet) && !SPS_isInputCurr(spsheet) )
	    SPS_keyRight(spsheet,win);
}

PublicFnDef void SPS_decrementZ(void) {
	MenuOptionSelected = SPS_DECREMENTZ;
}

PrivateFnDef void doDecrement(
    SPSHEET *spsheet, CUR_WINDOW *win
) {
	if( SPS_ZCount(spsheet) == 1 )
	    return;
	if( SPS_currZ(spsheet) == 0 )
	{
	    SPS_homeZ(spsheet) = 0;
	    SPS_screenZ(spsheet) = (SPS_ZCount(spsheet) - 1);
	}
	else
	{
	    SPS_homeZ(spsheet) = 0;
	    SPS_screenZ(spsheet)--;
	}
	SPS_repaintScreen(spsheet) = TRUE;
	SPS_recalcCols(spsheet,win);
	SPS_checkForLockedSheet(spsheet);
	if( !SPS_cellsLocked(spsheet) && SPS_isLockedCurr(spsheet) && !SPS_isInputCurr(spsheet) )
	    SPS_keyRight(spsheet,win);
}

PublicFnDef SPS_cell *SPS_cellPtrXYZ(
    SPSHEET *spr, int x, int y, int z
) {
   int	tx, ty, tz;
   if( spr->idx[X] == X )	tx = x;
   else if( spr->idx[X] == Y )	ty = x;
   else 			tz = x;
   if( spr->idx[Y] == X )	tx = y;
   else if( spr->idx[Y] == Y )	ty = y;
   else 			tz = y;
   if( spr->idx[Z] == X )	tx = z;
   else if( spr->idx[Z] == Y )	ty = z;
   else 			tz = z;
   return( &spr->cells[tx][ty][tz] );
}

/**************************************************************
 * Recompute the number of spreadsheet columns on the screen
 * based on the width of the columns and headings.
 * Therefore, when scrolling left or right, the number of
 * columns displayed may vary.
 **************************************************************/
PrivateFnDef int SPS_recalcCols (
    SPSHEET *			spr,
    CUR_WINDOW *		win
)
{
    int  w, xs, xx, yy, zz, width, xlines, oldy;
    SPS_DataType type;
    xlines = SPS_currHeadingLines(spr);
    xs = width = 0;
    xx = SPS_homeX(spr);
    yy = SPS_currY(spr);
    zz = SPS_currZ(spr);
    if( (w = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	w = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    w = maximum(SPS_maxHWidth(spr,Y),w);
    while( (w < SPS_screenCols(spr)) && (xx < SPS_XCount(spr)) )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	xx++;
	xs++;
	w += width;
    }
    if( xs == 0 )
	LastXWidth = 0;
    else
    {
        if( w > SPS_screenCols(spr) ) 
	{
	    xs--;
	    w -= width;
	}
	if( xs == 0 )
	{
	    LastXWidth = SPS_screenCols(spr) - w;
	    xs = 1;
	    w = SPS_screenCols(spr);
	}
	else
	{
	    xx = SPS_homeX(spr) + xs - 1;
	    SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	    if( w < SPS_screenCols(spr) )
		LastXWidth = width;
	    else
		LastXWidth = SPS_screenCols(spr) - (w - width);
	}
    }
    SPS_XsOnScreen(spr) = xs;
    TotalScreenWidth = w;
    CenteringOffset = (SPS_screenCols(spr) - TotalScreenWidth) / 2;
    oldy = SPS_YsOnScreen(spr);
    SPS_YsOnScreen(spr) = CUR_WIN_rows(win) - (xlines - 1) -
			minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr));
    if( oldy != SPS_YsOnScreen(spr) )
	SPS_screenY(spr) = 0;
    if( (SPS_homeY(spr) + SPS_YsOnScreen(spr)) > SPS_YCount(spr) )
	SPS_homeY(spr) = SPS_YCount(spr) - SPS_YsOnScreen(spr);
    if( SPS_homeY(spr) < 0 )
	SPS_homeY(spr) = 0;
}

PrivateFnDef void SPS_checkForLockedSheet(SPSHEET *spr) {
    int xx, yy, zz;

    SPS_cellsLocked(spr) = TRUE;
    zz = SPS_currZ(spr);
    for( xx=0 ; xx<SPS_XCount(spr) ; xx++ )
    {
	for( yy=0 ; yy<SPS_YCount(spr) ; yy++ )
	{
	    if( !SPS_isLockedXYZ(spr,xx,yy,zz) ||
		SPS_isInputXYZ(spr,xx,yy,zz) )
	    {
		SPS_cellsLocked(spr) = FALSE;
		return;
	    }
	}
    }
}


PrivateFnDef int SPS_goToEOL(spr,win,wrapFlag)
SPSHEET *spr;
CUR_WINDOW *win;
int wrapFlag;
{
    int  w, xs, xx, yy, zz, width;
    SPS_DataType type;
    xx = SPS_XCount(spr) - 1;
    yy = SPS_currY(spr);
    zz = SPS_currZ(spr);
    xs = width = 0;
    if( (w = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	w = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    w = maximum(SPS_maxHWidth(spr,Y),w);
    while( (w < SPS_screenCols(spr)) && (xx >= 0) )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	xx--;
	xs++;
	w += width;
    }
    if( xs == 0 )
    {
	LastXWidth = 0;
	SPS_homeX(spr) = SPS_XCount(spr) - 1;
	SPS_screenX(spr) = 0;
	SPS_recalcCols(spr,win);
	SPS_repaintScreen(spr) = TRUE;
    }
    else
    {
	if( (w > SPS_screenCols(spr)) && (xs > 0) )
	    xs--;
        SPS_homeX(spr) = ((xs == 0) ? SPS_XCount(spr) - 1 : SPS_XCount(spr) - xs);
	SPS_recalcCols(spr,win);
	SPS_screenX(spr) = ((SPS_XsOnScreen(spr) <= 0) ? 0 : (SPS_XsOnScreen(spr) - 1));
	SPS_repaintScreen(spr) = TRUE;
    }
    if( !SPS_cellsLocked(spr) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) && wrapFlag )
	SPS_keyLeft(spr,win);
}

PrivateFnDef int SPS_keyDown(spr,win)
SPSHEET *spr;
CUR_WINDOW *win;
{
  int oldy, oldh, olds;
  int ycount, ycountminus1;
  oldh = SPS_homeY(spr);
  olds = SPS_screenY(spr);
  oldy = SPS_currY(spr);
  ycount = SPS_YCount(spr);
  ycountminus1 = ycount - 1;
  do
  {
    if( (SPS_YsOnScreen(spr) >= ycount) && 
	 (SPS_currY(spr) == ycountminus1) )
    {
	SPS_homeY(spr) = 0;
	SPS_screenY(spr) = 0;
    }
    else if( (SPS_YsOnScreen(spr) < ycount) && 
	     (SPS_screenY(spr) == (SPS_YsOnScreen(spr) - 1)) )
    {
	SPS_repaintScreen(spr) = TRUE;
	if( SPS_currY(spr) == ycountminus1 )
	{
	    SPS_homeY(spr) = 0;
	    SPS_screenY(spr) = 0;
	}
	else
	{
	    SPS_homeY(spr)++;
	    SPS_screenY(spr) = (SPS_YsOnScreen(spr) - 1);
	}
	SPS_recalcCols(spr,win);
    }
    else
	SPS_screenY(spr)++;
  } while( !SPS_cellsLocked(spr) && (SPS_currY(spr) != oldy) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) );
  if( !SPS_cellsLocked(spr) && (SPS_currY(spr) == oldy) )
  {
    SPS_homeY(spr) = oldh;
    SPS_screenY(spr) = olds;
    SPS_recalcCols(spr,win);
    if( SPS_XCount(spr) > 1 )
	SPS_keyRight(spr, win);
  }
}

PrivateFnDef int SPS_keyUp(spr,win)
SPSHEET *spr;
CUR_WINDOW *win;
{
  int oldy, oldh, olds;
  int ycount, ycountminus1;
  oldh = SPS_homeY(spr);
  olds = SPS_screenY(spr);
  oldy = SPS_currY(spr);
  ycount = SPS_YCount(spr);
  ycountminus1 = ycount - 1;
  do
  {
    if( SPS_screenY(spr) == 0 )
    {
	if( ycount > SPS_YsOnScreen(spr) )
	    SPS_repaintScreen(spr) = TRUE;
	if( SPS_homeY(spr) == 0 )
	{
	    if (ycount > SPS_YsOnScreen(spr))
	    {
		SPS_homeY(spr) = ycount - SPS_YsOnScreen(spr);
		SPS_screenY(spr) = SPS_YsOnScreen(spr) - 1;
	    }
	    else
		SPS_screenY(spr) = ycountminus1;
	}
	else
	{
	    SPS_homeY(spr)--;
	    SPS_screenY(spr) = 0;
	}
	SPS_recalcCols(spr,win);
    }
    else
	SPS_screenY(spr)--;
  } while( !SPS_cellsLocked(spr) && (SPS_currY(spr) != oldy) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) );
  if( !SPS_cellsLocked(spr) && (SPS_currY(spr) == oldy) )
  {
    SPS_homeY(spr) = oldh;
    SPS_screenY(spr) = olds;
    SPS_recalcCols(spr,win);
    if( SPS_XCount(spr) > 1 )
	SPS_keyLeft(spr, win);
  }
}

PrivateFnDef void SPS_keyLeft (
    SPSHEET *			spr,
    CUR_WINDOW *		win
) {
  int oldx, oldh, olds;
  if( AlreadyHereInLeft )
  {
    AlreadyHereInLeft = FALSE;
    return;
  }
  oldh = SPS_homeX(spr);
  olds = SPS_screenX(spr);
  oldx = SPS_currX(spr);
  do
  {
    if( SPS_screenX(spr) == 0 )
    {
	if( SPS_homeX(spr) == 0 )
	{
	    SPS_goToEOL(spr,win,FALSE);
	}
	else
	{
	    SPS_homeX(spr)--;
	    SPS_screenX(spr) = 0;
	}
	SPS_recalcCols(spr,win);
	SPS_repaintScreen(spr) = TRUE;
    }
    else
	SPS_screenX(spr)--;
  } while( !SPS_cellsLocked(spr) && (SPS_currX(spr) != oldx) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) );
  if( !SPS_cellsLocked(spr) && (SPS_currX(spr) == oldx) )
  {
    SPS_homeX(spr) = oldh;
    SPS_screenX(spr) = olds;
    SPS_recalcCols(spr,win);
    if( SPS_YCount(spr) > 1 )
    {
	AlreadyHereInLeft = TRUE;
	SPS_keyUp(spr, win);
	AlreadyHereInLeft = FALSE;
    }
  }
}

PrivateFnDef void SPS_keyRight (
    SPSHEET *			spr,
    CUR_WINDOW *		win
)
{
  int oldx, oldh, olds;
  int xcount, xcountminus1;
  if( AlreadyHereInRight )
  {
    AlreadyHereInRight = FALSE;
    return;
  }
  oldh = SPS_homeX(spr);
  olds = SPS_screenX(spr);
  oldx = SPS_currX(spr);
  xcount = SPS_XCount(spr);
  xcountminus1 = xcount - 1;
  do
  {
    if( xcount <= SPS_XsOnScreen(spr) )
    {
	if( SPS_currX(spr) == xcountminus1 )
	    SPS_screenX(spr) = 0;
	else
	    SPS_screenX(spr)++;
    }
    else if( (SPS_screenX(spr) == (SPS_XsOnScreen(spr) - 1)) ||
	     (SPS_XsOnScreen(spr) == 0) )
    {
	if( SPS_currX(spr) == xcountminus1 )
	{
	    SPS_homeX(spr) = 0;
	    SPS_screenX(spr) = 0;
	    SPS_recalcCols(spr,win);
	}
	else
	{
	    SPS_homeX(spr)++;
	    SPS_recalcCols(spr,win);
	    SPS_screenX(spr) =
		(SPS_XsOnScreen(spr) == 0 ? 0 : SPS_XsOnScreen(spr) - 1);
	}
	SPS_repaintScreen(spr) = TRUE;
    }
    else
	SPS_screenX(spr)++;
  } while( !SPS_cellsLocked(spr) && (SPS_currX(spr) != oldx) && SPS_isLockedCurr(spr) && !SPS_isInputCurr(spr) );
  if( !SPS_cellsLocked(spr) && (SPS_currX(spr) == oldx) )
  {
    SPS_homeX(spr) = oldh;
    SPS_screenX(spr) = olds;
    SPS_recalcCols(spr,win);
    if( SPS_YCount(spr) > 1 )
    {
	AlreadyHereInRight = TRUE;
	SPS_keyDown(spr, win);
	AlreadyHereInRight = FALSE;
    }
  }
}
/*********************************************************
 *********	Cursor Movement Operations	**********
 ********************************************************/
PrivateFnDef int SPS_moveXHeadingCursor(spr,win,x,lx)
SPSHEET *spr;
CUR_WINDOW *win;
int x, lx;
{
    int  i, w, xx, yy, zz, width;
    SPS_DataType type;
    xx = SPS_homeX(spr);
    yy = SPS_homeY(spr);
    zz = SPS_currZ(spr);
    if( (w = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	w = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    w = maximum(SPS_maxHWidth(spr,Y),w);
    for( i=0 ; i<x ; i++ )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	xx++;
	w += width;
    }
    CUR_wmove(win, minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))-1+lx, w+CenteringOffset);
}

PrivateFnDef int SPS_moveYHeadingCursor(spr,win,y,ls)
SPSHEET *spr;
CUR_WINDOW *win;
int y, ls;
{
    CUR_wmove(win, y+minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))+ls-1, CenteringOffset);
}

PrivateFnDef int SPS_moveZHeadingCursor(spr,win,iz,lz)
SPSHEET *spr;
CUR_WINDOW *win;
int iz, lz;
{
    if( iz == 0 )
	CUR_wmove(win, minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))-1+lz, CenteringOffset);
    else
	CUR_wmove(win, minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))-1-iz, (iz*4)+CenteringOffset);
}

PrivateFnDef int SPS_moveCellCursor(spr,win,x,y,ls)
SPSHEET *spr;
CUR_WINDOW *win;
int x, y, ls;
{
    int  i, w, xx, yy, zz, width;
    SPS_DataType type;
    xx = SPS_homeX(spr);
    yy = SPS_homeY(spr) + y;
    zz = SPS_currZ(spr);
    if( (w = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	w = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    w = maximum(SPS_maxHWidth(spr,Y),w);
    for( i=0 ; i<x ; i++ )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	xx++;
	w += width;
    }
    CUR_wmove(win, y+minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))+ls-1, w+CenteringOffset);
}

PrivateFnDef int SPS_displayCellCursor(spr,win,x,y,ls)
SPSHEET *spr;
CUR_WINDOW *win;
int x, y, ls;
{
    int  i, w, xx, yy, zz, width;
    SPS_DataType type;
    xx = SPS_homeX(spr);
    yy = SPS_homeY(spr) + y;
    zz = SPS_currZ(spr);
    if( (w = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	w = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    w = maximum(SPS_maxHWidth(spr,Y),w);
    for( i=0 ; i<x ; i++ )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	xx++;
	w += width;
    }
    if( x == (SPS_XsOnScreen(spr) - 1) )
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	w += (LastXWidth / 2);
    }
    else
    {
	SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);
	w += (width / 2);
    }
    CUR_wmove(win, y+minimum(SPS_ZsOnScreen(spr),SPS_ZCount(spr))+ls-1, w+CenteringOffset);
}

/**************************************************************
 * Free all memory associated with the given SPSHEET.
 **************************************************************/
PublicFnDef void SPS_delete(SPSHEET *spr) {
    int i, j, k, width;
    SPS_DataType type;
    SPS_cell *c;
    SPS_Heading *h;

    SPS_resetIndices(spr);
    for( i=0 ; i<SPS_XCount(spr) ; i++ )
    {
	for( j=0 ; j<SPS_YCount(spr) ; j++ )
	{
	    for( k=0 ; k<SPS_ZCount(spr) ; k++ )
	    {
		SPS_xyzTypeWidth(spr,i,j,k,type,width);
		c = SPS_cellPtrXYZ(spr,i,j,k);
		if( ((type == SPS_String) ||
		     (type == SPS_RString) ||
		     (type == SPS_CString)) && 
		    (SPS_cellString(c) != NULL) )
		    free(SPS_cellString(c));
	    }
	}
    }
    for( i=0 ; i<SPS_XCount(spr) ; i++ )
    {
	for( j=0 ; j<SPS_YCount(spr) ; j++ )
	    free(spr->cells[i][j]);
	free(spr->cells[i]);
    }
    free(spr->cells);
    for( i=0 ; i<SPS_XCount(spr) ; i++ )
    {
	h = SPS_headingPtr(spr,i,X);
	if( ((SPS_headingType(h) == SPS_String) || 
	     (SPS_headingType(h) == SPS_RString) || 
	     (SPS_headingType(h) == SPS_CString)) && 
	    (SPS_headingString(h) != NULL) )
	    free(SPS_headingString(h));
	if( SPS_displayHeading(h) != NULL )
	    free(SPS_displayHeading(h));
    }
    for( i=0 ; i<SPS_YCount(spr) ; i++ )
    {
	h = SPS_headingPtr(spr,i,Y);
	if( ((SPS_headingType(h) == SPS_String) || 
	     (SPS_headingType(h) == SPS_RString) || 
	     (SPS_headingType(h) == SPS_CString)) && 
	    (SPS_headingString(h) != NULL) )
	    free(SPS_headingString(h));
	if( SPS_displayHeading(h) != NULL )
	    free(SPS_displayHeading(h));
    }
    for( i=0 ; i<SPS_ZCount(spr) ; i++ )
    {
	h = SPS_headingPtr(spr,i,Z);
	if( ((SPS_headingType(h) == SPS_String) || 
	     (SPS_headingType(h) == SPS_RString) || 
	     (SPS_headingType(h) == SPS_CString)) && 
	    (SPS_headingString(h) != NULL) )
	    free(SPS_headingString(h));
	if( SPS_displayHeading(h) != NULL )
	    free(SPS_displayHeading(h));
    }
    free(spr->headings[spr->idx[X]]);
    free(spr->headings[spr->idx[Y]]);
    free(spr->headings[spr->idx[Z]]);
    free(spr);
    SPS_freeHeadingBuf(SPS_hBuf);
}

/**************************************************************
 * Based on the given string and width, this procedure will
 * center the string within this range and display it.
 **************************************************************/
PrivateFnDef int SPS_centeredPrint(win,str,width)
CUR_WINDOW	*win;
char		*str;
int		width;
{
    char buf[256];
    int i, len;

    len = strlen(str);
    if( len >= width )
    {
	len = width;
	i = 0;
    }
    else
	i = (width - len) / 2;
    sprintf(buf,"%*.*s",width,width," ");
    strncpy(&buf[i],str,len);
    buf[i+len] = '\0';
    CUR_wprintw(win,"%-*.*s ",width-1,width-1,buf);
}

PrivateVarDef char DateBuf[81];
PrivateVarDef char HeadingBuf[81];

/**************************************************************
 * Format the date in a string based on the format of the
 * internal number.
 **************************************************************/
PublicFnDef char *SPS_formatDate(int date) {
	int	len;
	static char    buf[32];

	sprintf(buf,"%d",date);
	len = strlen(buf);
	switch(len)
	{
		case 1: /* Assume Y */
			sprintf(DateBuf,"%1.1s",buf);
			break;
		case 2: /* Assume YY */
			sprintf(DateBuf,"%2.2s",buf);
			break;
		case 3: /* Assume YMM */
			sprintf(DateBuf,"%2.2s/0%1.1s",&buf[1],buf);
			break;
		case 4: /* Assume YYMM */
			sprintf(DateBuf,"%2.2s/%2.2s",&buf[2],buf);
			break;
		case 5: /* Assume YMMDD */
			sprintf(DateBuf,"%2.2s/%2.2s/0%1.1s",&buf[1],&buf[3],buf);
			break;
		case 6: /* Assume YYMMDD */
			sprintf(DateBuf,"%2.2s/%2.2s/%2.2s",&buf[2],&buf[4],buf);
			break;
		case 7: /* Assume YYYMMDD */
			sprintf(DateBuf,"%2.2s/%2.2s/0%3.3s",&buf[3],&buf[5],buf);
			break;
		case 8: /* Assume YYYYMMDD */
			sprintf(DateBuf,"%2.2s/%2.2s/%4.4s",&buf[4],&buf[6],buf);
			break;
		case 9: /* Assume YYYYYMMDD */
			sprintf(DateBuf,"%2.2s/%2.2s/%5.5s",&buf[5],&buf[7],buf);
			break;
		default:
			strcpy(DateBuf,"NA");
			break;
	}
	return(DateBuf);
}

PublicFnDef char *SPS_formatHeading(SPS_Heading *h) {
	HeadingBuf[0] = '\0';
	if( SPS_headingStatus(h) == SPS_NA )
	{
	    sprintf(HeadingBuf,"NA");
	    return(HeadingBuf);
	}
	switch(SPS_headingType(h))
	{
	    case SPS_String:
	    case SPS_RString:
	    case SPS_CString:
		sprintf(HeadingBuf,"%s",SPS_headingString(h));
		break;
	    case SPS_Float:
		sprintf(HeadingBuf,"%.*f",FLOATDECIMALS,SPS_headingFloat(h));
		break;
	    case SPS_Int:
		sprintf(HeadingBuf,"%d",SPS_headingInt(h));
		break;
	    case SPS_Date:
		return(SPS_formatDate(SPS_headingDate(h)));
	}
	return(HeadingBuf);
}

PublicFnDef char *SPS_cellAsString(SPS_cell *c, SPS_DataType type) {
	HeadingBuf[0] = '\0';
	if( SPS_cellStatus(c) == SPS_ModifiedNA )
	{
	    sprintf(HeadingBuf,"NA");
	    return(HeadingBuf);
	}
	switch(type)
	{
	    case SPS_String:
	    case SPS_RString:
	    case SPS_CString:
		sprintf(HeadingBuf,"\"%s\"",SPS_cellString(c));
		break;
	    case SPS_Float:
		sprintf(HeadingBuf,"%.*f",6,SPS_cellFloat(c));
		break;
	    case SPS_Int:
		sprintf(HeadingBuf,"%d",SPS_cellInt(c));
		break;
	    case SPS_Date:
		sprintf(HeadingBuf,"%d asDate",SPS_cellDate(c));
		break;
	}
	return(HeadingBuf);
}

PublicFnDef char *SPS_itemAsString(SPS_Heading *h) {
	HeadingBuf[0] = '\0';
	if( SPS_headingStatus(h) == SPS_NA )
	{
	    sprintf(HeadingBuf,"NA");
	    return(HeadingBuf);
	}
	switch(SPS_headingType(h))
	{
	    case SPS_String:
	    case SPS_RString:
	    case SPS_CString:
		return(SPS_headingString(h));
	    case SPS_Float:
		sprintf(HeadingBuf,"%.*f",6,SPS_headingFloat(h));
		break;
	    case SPS_Int:
		sprintf(HeadingBuf,"%d",SPS_headingInt(h));
		break;
	    case SPS_Date:
		sprintf(HeadingBuf,"%d asDate",SPS_headingDate(h));
		break;
	}
	return(HeadingBuf);
}

/**************************************************************
 * Print the value of the cell at (x,y,z).
 **************************************************************/
PrivateFnDef void SPS_printCell(
    SPSHEET *spr, CUR_WINDOW *win, int x, int y
) {
    SPS_cell	*c;
    int width, xx, yy, zz, inp, xlines;
    SPS_DataType type;
    char	buf[81];

    xlines = SPS_currHeadingLines(spr);
    SPS_moveCellCursor(spr,win,x,y,xlines);
    xx = SPS_homeX(spr) + x;
    yy = SPS_homeY(spr) + y;
    zz = SPS_currZ(spr);
    c = SPS_cellPtrXYZ(spr,xx,yy,zz);
    SPS_xyzTypeWidth(spr,xx,yy,zz,type,width);

    if( x == (SPS_XsOnScreen(spr) - 1) )
	width = LastXWidth;
    if( (inp = SPS_isInputXYZ(spr,xx,yy,zz)) )
    {
	if( (SPS_cellStatus(c) == SPS_NA) || (SPS_cellStatus(c) == SPS_Normal) )
	{
	    CUR_wprintw(win,"%*.*s",width,width," ");
	    return;
	}
	CUR_wattron(win,CUR_A_BOLD);
    }

    if( (SPS_cellStatus(c) == SPS_NA) || 
	(SPS_cellStatus(c) == SPS_ModifiedNA) ||
	(SPS_cellStatus(c) == SPS_SavedNA) )
	CUR_wprintw(win,"%*.*s ",width-1,width-1,"NA");
    else
    {
	switch(type)
	{
	    case SPS_String:
		CUR_wprintw(win,"%-*.*s",width,width,SPS_cellString(c));
		break;
	    case SPS_RString:
		CUR_wprintw(win,"%*.*s",width,width,SPS_cellString(c));
		break;
	    case SPS_CString:
		SPS_centeredPrint(win,SPS_cellString(c),width);
		break;
	    case SPS_Float:
		sprintf(buf,"%*.*f",width-1,FLOATDECIMALS,SPS_cellFloat(c));
		CUR_wprintw(win,"%*.*s ",width-1,width-1,buf);
		break;
	    case SPS_Int:
		sprintf(buf,"%d",SPS_cellInt(c));
		CUR_wprintw(win,"%*.*s ",width-1,width-1,buf);
		break;
	    case SPS_Date:
		CUR_wprintw(win,"%*.*s ",width-1,width-1,
			SPS_formatDate(SPS_cellDate(c)));
		break;
	}
    }
    if( inp )
	CUR_wattroff(win,CUR_A_BOLD);
}

/**************************************************************
 * Print the headings for the Z axis.
 * The current Z value (index == 0) can only be as wide as
 * the current Y headings.
 **************************************************************/

PrivateFnDef int SPS_printZHeading (
    SPSHEET *			spr,
    CUR_WINDOW *		win,
    int				screenIndex,
    int				index
)
{
    SPS_Heading *h;
    int width, idx, xlines, l, hlen;
    char buf[81], *s, *ptr;

    xlines = SPS_currHeadingLines(spr);
    idx = SPS_homeZ(spr) + index;
    h = SPS_headingPtr(spr,idx,Z);
    if( (width = SPS_headingOWidth(h)) <= 0 )
	width = SPS_headingHLWidth(h);
    width = maximum(SPS_maxHWidth(spr,Y),width);
    if( screenIndex == 0 )
    {
	if( SPS_headingStatus(h) == SPS_NA )
	{
	    SPS_moveZHeadingCursor(spr,win,screenIndex,0);
	    CUR_wprintw(win,"%-*.*s ",width-1,width-1,"NA");
	    for( l=1 ; l<xlines ; l++ )
	    {
		SPS_moveZHeadingCursor(spr,win,screenIndex,l);
		CUR_wattron(win,CUR_A_REVERSE);
		CUR_wprintw(win,"%-*.*s",width,width," ");
		CUR_wattroff(win,CUR_A_REVERSE);
	    }
	}
	else
	{
	    ptr = SPS_displayHeading(h);
	    if( ptr == NULL )
	    {
		ptr = buf;
		buf[0] = '\0';
	    }
	    for( l=0 ; l<xlines ; l++ )
	    {
		SPS_moveZHeadingCursor(spr,win,screenIndex,l);
		CUR_wattron(win,CUR_A_REVERSE);
		s = strchr(ptr,LineBreakChar);
		if( s == NULL )
		{
		    CUR_wprintw(win,"%-*.*s",width,width,ptr);
		    ptr += strlen(ptr);
		}
		else
		{
		    hlen = ((int)(s - ptr));
		    strncpy(buf,ptr,hlen);
		    buf[hlen] = '\0';
		    CUR_wprintw(win,"%-*.*s ",width-1,width-1,buf);
		    ptr = s + 1;
		}
		CUR_wattroff(win,CUR_A_REVERSE);
	    }
	}
    }
    else
    {
	width = minimum((SPS_screenCols(spr)-(screenIndex*4)),TotalScreenWidth);
	if( (width + CenteringOffset + (screenIndex*4)) > SPS_screenCols(spr) )
	    width = SPS_screenCols(spr) - CenteringOffset - (screenIndex*4);
	SPS_moveZHeadingCursor(spr,win,screenIndex,0);
	CUR_wattron(win,CUR_A_REVERSE);
	if( SPS_headingStatus(h) == SPS_NA )
		CUR_wprintw(win,"%-*.*s ",width-1,width-1,"NA");
	else if( SPS_displayHeading(h) != NULL )
	{
	    strcpy(buf,SPS_displayHeading(h));
	    ptr = buf;
	    while( (s = strchr(ptr,LineBreakChar)) != NULL )
	    {
		*s = ' ';
		ptr = s;
	    }
	    CUR_wprintw(win,"%-*.*s",width,width,buf);
	}
	else
	    CUR_wprintw(win,"%-*.*s",width,width,"WHAT!");
	CUR_wattroff(win,CUR_A_REVERSE);
    }
}

/**************************************************************
 * Print the headings on the Y axis.
 **************************************************************/
PrivateFnDef int SPS_printYHeading(spr,win,index)
SPSHEET *spr;
CUR_WINDOW *win;
int index;
{
    SPS_Heading *h;
    int width, idx, xlines, zz;
    char buf[81], *s, *ptr;

    xlines = SPS_currHeadingLines(spr);
    SPS_moveYHeadingCursor(spr,win,index,xlines);
    idx = SPS_homeY(spr) + index;
    h = SPS_headingPtr(spr,idx,Y);
    zz = SPS_currZ(spr);
    if( (width = SPS_headingOWidth(SPS_headingPtr(spr,zz,Z))) <= 0 )
	width = SPS_headingHLWidth(SPS_headingPtr(spr,zz,Z));
    width = maximum(SPS_maxHWidth(spr,Y),width);
    CUR_wattron(win,CUR_A_REVERSE);
    if( SPS_headingStatus(h) == SPS_NA )
	CUR_wprintw(win,"%-*.*s ",width-1,width-1,"NA");
    else if( SPS_displayHeading(h) != NULL )
    {
	strcpy(buf,SPS_displayHeading(h));
	ptr = buf;
	while( (s = strchr(ptr,LineBreakChar)) != NULL )
	{
	    *s = ' ';
	    ptr = s;
	}
	CUR_wprintw(win,"%-*.*s ",width-1,width-1,buf);
    }
    else
	CUR_wprintw(win,"%-*.*s",width,width,"WHAT!!!");
    CUR_wattroff(win,CUR_A_REVERSE);
}

/**************************************************************
 * Print the headings on the X axis. They will be centered
 * within their respective widths.
 **************************************************************/
PrivateFnDef int SPS_printXHeading(spr,win,index)
SPSHEET *spr;
CUR_WINDOW *win;
int index;
{
    SPS_Heading *h;
    SPS_DataType type;
    int width, idx, xlines, l, hlen;
    char buf[81], *s, *ptr;

    xlines = SPS_currHeadingLines(spr);
    idx = SPS_homeX(spr) + index;
    h = SPS_headingPtr(spr,idx,X);
    if( index == (SPS_XsOnScreen(spr) - 1) )
	width = LastXWidth;
    else
    {
	SPS_xyzTypeWidth(spr,idx,SPS_currY(spr),SPS_currZ(spr),type,width);
	/*
	width = maximum(SPS_headingHWidth(h),SPS_headingCWidth(h));
	if( SPS_cellTypeH(h) == SPS_Variable )
		width = maximum(SPS_maxCellWidth(spr,Y),width);
	*/
    }
    if( SPS_headingStatus(h) == SPS_NA )
    {
	SPS_moveXHeadingCursor(spr,win,index,0);
	strcpy(buf,"NA");
	CUR_wattron(win,CUR_A_REVERSE);
	SPS_centeredPrint(win,buf,width);
	CUR_wattroff(win,CUR_A_REVERSE);
	strcpy(buf," ");
	for( l=1 ; l<xlines ; l++ )
	{
	    SPS_moveXHeadingCursor(spr,win,index,l);
	    CUR_wattron(win,CUR_A_REVERSE);
	    SPS_centeredPrint(win,buf,width);
	    CUR_wattroff(win,CUR_A_REVERSE);
	}
    }
    else
    {
	ptr = SPS_displayHeading(h);
	if( ptr == NULL )
	{
	    ptr = buf;
	    buf[0] = '\0';
	}
	for( l=0 ; l<xlines ; l++ )
	{
	    SPS_moveXHeadingCursor(spr,win,index,l);
	    CUR_wattron(win,CUR_A_REVERSE);
	    s = strchr(ptr,LineBreakChar);
	    if( s == NULL )
	    {
		buf[0] = '\0';
		strcpy(buf,ptr);
		SPS_centeredPrint(win,buf,width);
		ptr += strlen(ptr);
	    }
	    else
	    {
		hlen = ((int)(s - ptr));
		strncpy(buf,ptr,hlen);
		buf[hlen] = '\0';
		SPS_centeredPrint(win,buf,width);
		ptr += (hlen + 1);
	    }
	    CUR_wattroff(win,CUR_A_REVERSE);
	}
    }
}

/*****		Routine to manage user interaction with spsheet
 *
 *  Arguments:
 *	spsheet     -pointer to a spsheet object
 *	win	    -pointer to the CUR_WINDOW
 *	action	    -pointer to a PAGE_Action
 *
 *  Returns:
 *	PAGE_Action
 *
 *****/
PublicFnDef PAGE_Action SPS_handler(
    SPSHEET *spsheet, CUR_WINDOW *win, PAGE_Action action
) {
    switch (action)
    {
	case PAGE_Init:
	    return(initSprSh(spsheet, win));

	case PAGE_Input:
	case PAGE_Scroll:
	    return(inputSprSh(spsheet, win));

	case PAGE_End:
	    CUR_delwin(win);
	    return(action);

	case PAGE_Refresh:
	    SPS_paintScreen(spsheet, win);
	    CUR_touchwin(win);
	    CUR_wnoutrefresh(win);
	    return(PAGE_Refresh);
	      
	default:
	    break;
    }
    return(PAGE_Error);
}

PrivateFnDef PAGE_Action initSprSh (
    SPSHEET *			spsheet,
    CUR_WINDOW *		win
)
{
    int maxl, ys;

    SPS_homeX(spsheet) = SPS_screenX(spsheet) = 0;
    SPS_homeY(spsheet) = SPS_screenY(spsheet) = 0;
    SPS_homeZ(spsheet) = SPS_screenZ(spsheet) = 0;

    maxl = maximum(SPS_maxHLines(spsheet,X),SPS_maxHLines(spsheet,Z));
    if( maxl >= CUR_WIN_rows(win) )
    {
	ERR_displayPause("Not enough rows in window to display sheet");
	KEY_putc(KEY_PREV);
	return(PAGE_Error);
    }
    if( SPS_maxHLines(spsheet,Y) >= CUR_WIN_rows(win) )
	SPS_allowTransposeAndRotate(spsheet) = FALSE;
    else
    {
	SPS_allowTransposeAndRotate(spsheet) = TRUE;
	maxl = maximum(SPS_maxHLines(spsheet,Y),maxl);
    }
    ys = CUR_WIN_rows(win) - maxl;
    if( ys > 11 )
	SPS_ZsOnScreen(spsheet) = 4;
    else if( ys > 7 )
	SPS_ZsOnScreen(spsheet) = 3;
    else if( ys > 3 )
	SPS_ZsOnScreen(spsheet) = 2;
    else
	SPS_ZsOnScreen(spsheet) = 1;
    SPS_screenCols(spsheet) = CUR_WIN_cols(win) - 1;
    SPS_YsOnScreen(spsheet) = CUR_WIN_rows(win) - 
			(SPS_currHeadingLines(spsheet) - 1) -
			minimum(SPS_ZsOnScreen(spsheet),SPS_ZCount(spsheet));

    /*MenuOptionSelected = SPS_NOACTION;*/

    SPS_recalcCols(spsheet,win);

    SPS_checkForLockedSheet(spsheet);
    if( !SPS_cellsLocked(spsheet) && SPS_isLockedCurr(spsheet) && !SPS_isInputCurr(spsheet) )
	SPS_keyRight(spsheet,win);

    if( STD_hasInsertDeleteLine )
	CUR_idlok(win,TRUE);

    SPS_paintScreen(spsheet, win);

/*** refresh the current screen ***/
    CUR_touchwin(win);
    CUR_wrefresh(win);
    return(PAGE_Init);
}

/**************************************************************
 * Redraw the spreadsheet.
 **************************************************************/
PublicFnDef void SPS_paintScreen(SPSHEET *spr, CUR_WINDOW *win) {
    int x, y, z, iz;

    SPS_recalcCols(spr,win);

    CUR_werase(win);

    z = SPS_screenZ(spr);
    for( iz=0 ; ((iz<SPS_ZsOnScreen(spr)) && (iz<SPS_ZCount(spr))) ; iz++ )
    {
	SPS_printZHeading(spr,win,iz,z);
	z = (z + 1) % SPS_ZCount(spr);
    }

    for( y=0 ; ((y<SPS_YsOnScreen(spr)) && (y<SPS_YCount(spr))) ; y++ )
    {
	SPS_printYHeading(spr,win,y);
	for( x=0 ; ((x<SPS_XsOnScreen(spr)) && (x<SPS_XCount(spr))) ; x++ )
	{
	    if( y == 0 )
		SPS_printXHeading(spr,win,x);
	    SPS_printCell(spr,win,x,y);
	}
    }
    SPS_repaintScreen(spr) = FALSE;
}

PrivateFnDef PAGE_Action inputSprSh (
    SPSHEET *			spsheet,
    CUR_WINDOW *		win
)
{
    int c, row, col, not_done, rr, cc, len, axis, xlines;
    SPS_cell *cell;
    SPS_DataType type;
    int width;
    PAGE_Action action;
    SPS_Heading *ch;

    ERR_clearMsg();

    switch( MenuOptionSelected )
    {
	case SPS_REFRESHPAGE:
	    MenuOptionSelected = SPS_NOACTION;
	    return(PAGE_Refresh);

	case SPS_CLEARCELL:
	    SPS_doClearCell(spsheet,win);
	    break;

	case SPS_CLEARINPUT:
	    SPS_doClearInput(spsheet,win);
	    break;

	case SPS_TRANSPOSE:
	    doTranspose(spsheet,win);
	    break;

	case SPS_ROTATE:
	    doRotate(spsheet,win);
	    break;

	case SPS_INCREMENTZ:
	    doIncrement(spsheet,win);
	    break;

	case SPS_DECREMENTZ:
	    doDecrement(spsheet,win);
	    break;

	default:
	    break;
    }
    SPS_recalcCols(spsheet,win);
    MenuOptionSelected = SPS_NOACTION;
    not_done = TRUE;
    while (not_done)
    {
	if (!KEY_cready())
	{
	    if (SPS_repaintScreen(spsheet))
		SPS_paintScreen(spsheet, win);
	    SPS_highlightCell(spsheet, win);
	    SPS_displayCellCursor(spsheet, win, 
		SPS_screenX(spsheet), 
		SPS_screenY(spsheet), 
		SPS_currHeadingLines(spsheet));
	    CUR_touchwin(win);
	    CUR_wrefresh(win);
	    CUR_forceSetCursor(win, CUR_WIN_cury(win), CUR_WIN_curx(win));
	}

	c = KEY_getkey(FALSE);

	if( ERR_msgDisplayed )
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

	case KEY_FILES:
	    action = PAGE_File;
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

	case KEY_AMENU:
	    action = PAGE_AMenu;
	    not_done = FALSE;
	    break;

	case KEY_SMENU:
	    action = PAGE_SMenu;
	    not_done = FALSE;
	    break;

	case KEY_TOP:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_goToTop(spsheet, win);
	    break;

	case KEY_BOTTOM:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_goToBottom(spsheet, win);
	    break;

	case KEY_VEOL:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_goToEOL(spsheet, win, TRUE);
	    break;

	case KEY_BOL:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_goToBOL(spsheet, win);
	    break;

	case KEY_SCR_D:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_scrollDown(spsheet,win);
	    break;

	case KEY_SCR_U:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_scrollUp(spsheet,win);
	    break;

	case KEY_SCR_R:
	case KEY_TAB:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_scrollRight(spsheet, win);
	    break;

	case KEY_SCR_L:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_scrollLeft(spsheet, win);
	    break;

	case CUR_KEY_UP:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_keyUp(spsheet,win);
	    break;

	case CUR_KEY_DOWN:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_keyDown(spsheet,win);
	    break;

	case CUR_KEY_LEFT:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_keyLeft(spsheet, win);
	    break;

	case KEY_CR:
	case CUR_KEY_RIGHT:
	    SPS_unhighlightCell(spsheet, win);
	    SPS_keyRight(spsheet, win);
	    break;

	case KEY_REPAINT:
	    CUR_clearok(CUR_curscr);
	    CUR_wrefresh(CUR_curscr);
	    break;

	case KEY_TRANSPOSE:
	    doTranspose(spsheet,win);
	    break;

	case KEY_ROTATE:
	    doRotate(spsheet,win);
	    break;

	case KEY_INCREMENTZ:
	    doIncrement(spsheet,win);
	    break;

	case KEY_DECREMENTZ:
	    doDecrement(spsheet,win);
	    break;

	case KEY_QUOTE:
	    KEY_QuoteNextKey = FALSE;
	    CUR_beep();
	    break;

	default:
	    if( SPS_isInputCurr(spsheet) )
	    {
		row = SPS_screenY(spsheet);
		col = SPS_screenX(spsheet);
		xlines = SPS_currHeadingLines(spsheet);

		cell = SPS_cellPtrCurr(spsheet);
		SPS_currTypeWidth(spsheet,type,width);
		SPS_moveCellCursor(spsheet,win,col,row,xlines);
		rr = CUR_WIN_cury(win);
		cc = CUR_WIN_curx(win);
		switch(type)
		{
		    case SPS_Float:
		    case SPS_Int:
			getIntOrFloat(win,cell,rr,cc,type,width,c);
			ERR_clearMsg();
			break;
		    case SPS_String:
		    case SPS_RString:
		    case SPS_CString:
			if( !getString(win,cell,rr,cc,width,c) )
			{
			    if( SPS_cellStatus(cell) == SPS_ModifiedNA )
				break;
			    SPS_controllingHeaderCurr(spsheet,ch,axis);
			    if( (len = strlen(SPS_cellString(cell))) >= width )
			    {
#if 0
				SPS_headingCWidth(ch) = len + 1;
				if( axis != -1 )
				    if( (len+1) > SPS_maxCellWidth(spsheet,axis) )
					SPS_maxCellWidth(spsheet,axis) = len + 1;
				SPS_repaintScreen(spsheet) = TRUE;
#endif
				ERR_displayStr("String not completely displayed",FALSE);
			    }
			}
			else
			    ERR_clearMsg();
			break;
		    case SPS_Date:
			getDate(win,cell,rr,cc,width,c);
			ERR_clearMsg();
			break;
		    default:
			break;
		}
		if( (SPS_cellStatus(cell) == SPS_Modified) ||
		    (SPS_cellStatus(cell) == SPS_ModifiedNA) )
		    SPS_sheetStatus(spsheet) = SPS_Modified;
	    }
	    else
		ERR_displayStr(" Not an input cell",TRUE);
	    break;

	} /* switch */

    } /* while not_done */

    SPS_unhighlightCell(spsheet, win);
    CUR_wrefresh(win);

    return(action);
}

PublicFnDef int SPS_readHeadingList(
    SPSHEET *spr, int count, int axis, int flag
) {
	int	i;
	SPS_Heading	*h;
	char buf[256];

	SPS_MaxHeadingLen = 1;
	SPS_MaxHeadingLines = 1;

	for( i=0 ; i<count ; i++ )
	{
		h = SPS_headingPtr(spr,i,axis);
		if( SPS_getHeadingEntry() )
			return(TRUE);	
		SPS_setHeadingEntry(h,flag);
	}

	SPS_maxHWidth(spr,axis) = SPS_MaxHeadingLen + 1;
	SPS_maxHLines(spr,axis) = SPS_MaxHeadingLines;

	while( RS_readLine(buf,256) ) ;

	return(FALSE);	
}

PublicFnDef int SPS_readStringOnly(char *buf, int len) {
	int	errlen, errval;
	char	*ptr;

	errlen = strlen(ErrorString);
	errval = SPS_NOERROR;
	if( !RS_readLine(buf,len) )
		return( SPS_GOTPROMPT );
	ptr = eatLeadingAndTrailingSpaces(buf);
	if( strcmp(ptr,"NA") == 0 )
	    errval = SPS_GOTNA;
	if( strncmp(ptr,ErrorString,errlen) == 0 )
	    errval = SPS_GOTERROR;
	return( errval );
}

PublicFnDef int SPS_readString(char *buf, int len) {
	int	errlen, errval;
	char	*ptr, buf2[256];

	errlen = strlen(ErrorString);
	errval = SPS_NOERROR;
	if( !RS_readLine(buf,len) )
		return( SPS_GOTPROMPT );
	ptr = eatLeadingAndTrailingSpaces(buf);
	if( strcmp(ptr,"NA") == 0 )
	{
	    while( RS_readLine(buf2,256) ) ;
	    return( SPS_GOTNA );
	}
	while( RS_readLine(buf2,256) )
	{
	    ptr = eatLeadingAndTrailingSpaces(buf2);
	    if( strncmp(ptr,ErrorString,errlen) == 0 )
		errval = SPS_GOTERROR;
	}
	return( errval );
}

PublicFnDef int SPS_readInteger() {
	int	i;
	char	buf[SPS_maxItemWidth+1], *ptr;

	if( !RS_readLine(buf,SPS_maxItemWidth) )
		return( SPS_GOTPROMPT );
	ptr = eatLeadingAndTrailingSpaces(buf);
	if( strcmp(ptr,"NA") == 0 )
		return( SPS_GOTNA );
	i = (int)atoi(ptr);
	return( i );
}

PrivateFnDef SPS_DataType SPS_getDataType(str)
char	*str;
{
	if( strncmp("VARIES",str,6) == 0 )
		return(SPS_Variable);
	if( strncmp("aDouble",str,7) == 0 )
		return(SPS_Float);
	if( strncmp("aFloat",str,6) == 0 )
		return(SPS_Float);
	if( strncmp("anInteger",str,9) == 0 )
		return(SPS_Int);
	if( strncmp("aDate",str,5) == 0 )
		return(SPS_Date);
	if( strncmp("aString",str,7) == 0 )
		return(SPS_String);
	if( strncmp("aRString",str,8) == 0 )
		return(SPS_RString);
	if( strncmp("aLString",str,8) == 0 )
		return(SPS_String);
	if( strncmp("aCString",str,8) == 0 )
		return(SPS_CString);
	return(SPS_Float);
}

PublicFnDef void SPS_setCellValue(
    SPS_cell *c, SPS_DataType type, char *str
) {
	double	atof();

	if( strcmp(str,"NA") == 0 )
	{
		SPS_cellStatus(c) = SPS_NA;
		if( (type == SPS_String) ||
		    (type == SPS_RString) ||
		    (type == SPS_CString) )
		{
		    if( SPS_cellString(c) != NULL )
			free(SPS_cellString(c));
		    str = " ";
		    SPS_addString(SPS_cellString(c),str);
		}
		return;
	}
	SPS_cellStatus(c) = SPS_Normal;
	switch( type )
	{
		case SPS_String:
		case SPS_RString:
		case SPS_CString:
			if( SPS_cellString(c) != NULL )
			    free(SPS_cellString(c));
			SPS_addString(SPS_cellString(c),str);
			break;
		case SPS_Date:
			SPS_cellDate(c) = (int)atoi(str);
			break;
		case SPS_Int:
			SPS_cellInt(c) = (int)atoi(str);
			break;
		case SPS_Float:
			SPS_cellFloat(c) = (double)atof(str);
			break;
		default:
			break;
	}
}

PublicFnDef int SPS_readCell(
    SPS_cell *c, SPS_DataType type
) {
	char	buf[SPS_maxItemWidth+1], *ptr;
	int	errlen;

	errlen = strlen(ErrorString);
	if( !RS_readLine(buf,SPS_maxItemWidth) )
	{
		SPS_cellStatus(c) = SPS_NA;
		return(SPS_GOTPROMPT);
	}
	ptr = eatLeadingAndTrailingSpaces(buf);

	if( strncmp(ptr,ErrorString,errlen) == 0 )
	{
	    /* Now eat the "NA" after the error message */
	    if( !RS_readLine(buf,SPS_maxItemWidth) )
	    {
		    SPS_cellStatus(c) = SPS_NA;
		    return(SPS_GOTPROMPT);
	    }
	    return( SPS_GOTERROR );
	}
		
	SPS_setCellValue(c,type,ptr);
	return(FALSE);
}

PublicFnDef void SPS_setItemAndStatus(
    SPS_Heading *h, char *str
) {
	double	atof();

	if( strcmp(str,"NA") == 0 )
	{
	    if( (SPS_headingType(h) == SPS_String) ||
	        (SPS_headingType(h) == SPS_RString) ||
	        (SPS_headingType(h) == SPS_CString) )
	    {
		if( SPS_headingString(h) != NULL )
		    free(SPS_headingString(h));
		str = "";
		SPS_addString(SPS_headingString(h),str);
	    }
	    SPS_headingStatus(h) = SPS_NA;
	    return;
	}
	SPS_headingStatus(h) = SPS_Normal;
	switch( SPS_headingType(h) )
	{
		case SPS_String:
		case SPS_RString:
		case SPS_CString:
			if( SPS_headingString(h) != NULL )
			    free(SPS_headingString(h));
			SPS_addString(SPS_headingString(h),str);
			break;
		case SPS_Date:
			SPS_headingDate(h) = (int)atoi(str);
			break;
		case SPS_Int:
			SPS_headingInt(h) = (int)atoi(str);
			break;
		case SPS_Float:
			SPS_headingFloat(h) = (double)atof(str);
			break;
		default:
			break;
	}
}

PrivateFnDef int SPS_getHeadingEntry(void) {
	char	buf2[SPS_maxItemWidth+1];
	int	i, errlen, gotprompt = FALSE;

	if( SPS_hBuf[0] == NULL )
	    SPS_makeHeadingBuf(SPS_hBuf);
	errlen = strlen(ErrorString);
	for( i=0 ; i<ItemsInFullListEntry ; i++ )
		SPS_hBuf[i][0] = '\0';
	i = 0;
	while( i < ItemsInFullListEntry )
	{
		if( !RS_readLine(buf2,SPS_maxItemWidth) )
		{
			gotprompt = TRUE;
			break;
		}
		strcpy(SPS_hBuf[i],eatLeadingAndTrailingSpaces(buf2));
		if( strncmp(SPS_hBuf[i],ErrorString,errlen) == 0 )
		{
			SPS_hBuf[i][0] = '\0';
			if( i > 0 )
				i--;
			continue;
		}
		i++;
	}
	return( gotprompt );
}

PrivateFnDef void SPS_setHeadingEntry(
    SPS_Heading *h, int flag
) {
	int	len, hlen, hlines;
	char	*s, *ptr ,dummy[81];

/*
	if( flag == FULL )
	{
*/
	    SPS_headingType(h) = SPS_getDataType(SPS_hBuf[EntryItemType]);
	    SPS_cellTypeH(h) = SPS_getDataType(SPS_hBuf[EntryCellType]);
/*
	}
*/
	SPS_setItemAndStatus(h,SPS_hBuf[EntryItemName]);
	if( SPS_displayHeading(h) != NULL )
	    free(SPS_displayHeading(h));
	if( strlen(SPS_hBuf[EntryHeadingLabel]) == 0 )
	{
	    SPS_addString(SPS_displayHeading(h),SPS_formatHeading(h));
	}
	else
	{
	    SPS_addString(SPS_displayHeading(h),SPS_hBuf[EntryHeadingLabel]);
	}

/*
	if( flag == FULL )
	{
*/
	    if( (SPS_hBuf[EntryInputOrDisplay][0] == 'i') ||
		(SPS_hBuf[EntryInputOrDisplay][0] == 'I') )
		    SPS_headingCellIO(h) = SPS_Input;
	    else if( (SPS_hBuf[EntryInputOrDisplay][0] == 'l') ||
		     (SPS_hBuf[EntryInputOrDisplay][0] == 'L') )
		SPS_headingCellIO(h) = SPS_Locked;
	    else
		SPS_headingCellIO(h) = SPS_Display;
/*
	}
*/
	if( (sscanf(SPS_hBuf[EntryItemType],"%s %d",dummy,&len) != 2) ||
	    (len <= 0) )
	    SPS_headingOWidth(h) = 0;
	else
	    SPS_headingOWidth(h) = len;
	if( (len = strlen(SPS_displayHeading(h))) > SPS_MaxHeadingLen )
	    SPS_MaxHeadingLen = len;
	SPS_headingHWidth(h) = len + 1;
	if( SPS_headingHWidth(h) >= SPS_maxHeadingWidth )
	    SPS_headingHWidth(h) = SPS_maxHeadingWidth;
	hlen = 1;
	hlines = 1;
	ptr = SPS_displayHeading(h);
	while( (s = strchr(ptr,LineBreakChar)) != NULL )
	{
	    hlines++;
	    if( hlen <= ((int)(s - ptr)) )
		hlen = ((int)(s - ptr)) + 1;
	    ptr = s + 1;
	}
	if( hlen <= ((int)(&SPS_displayHeading(h)[len] - ptr)) )
	    hlen = ((int)(&SPS_displayHeading(h)[len] - ptr)) + 1;
	SPS_headingHLWidth(h) = hlen;
	if( SPS_headingHLWidth(h) >= SPS_maxHeadingWidth )
	    SPS_headingHLWidth(h) = SPS_maxHeadingWidth;
	SPS_headingLines(h) = hlines;
	if( hlines > SPS_MaxHeadingLines )
		SPS_MaxHeadingLines = hlines;
}

PublicFnDef int SPS_readHeadings(SPSHEET *spr, char *list, int flag) {
    char buf[256];
    int oldx, oldy, oldz;

    oldx = spr->idx[X];
    oldy = spr->idx[Y];
    oldz = spr->idx[Z];
    SPS_resetIndices(spr);

    ERR_displayStr("Reading in worksheet headings.  Please wait...",FALSE);
    sprintf(buf,"%s headingListX",list);
    RS_sendLine(buf);
    if( SPS_readHeadingList(spr,SPS_XCount(spr),X,flag) )
	    return(TRUE);
    sprintf(buf,"%s headingListY",list);
    RS_sendLine(buf);
    if( SPS_readHeadingList(spr,SPS_YCount(spr),Y,flag) )
	    return(TRUE);
    sprintf(buf,"%s headingListZ",list);
    RS_sendLine(buf);
    if( SPS_readHeadingList(spr,SPS_ZCount(spr),Z,flag) )
	    return(TRUE);
    spr->idx[X] = oldx;
    spr->idx[Y] = oldy;
    spr->idx[Z] = oldz;
    return(FALSE);
}

PublicFnDef SPSHEET *SPS_readSSheet(char *list) {
    char buf[256];
    int xcount, ycount, zcount;
    SPSHEET *spr;

    sprintf(buf,"%s axisCounts",list);
    RS_dumpOutput();
    RS_sendLine(buf);
    xcount = SPS_readInteger();
    if( xcount <= 0 )
	    return((SPSHEET *)NULL);
    ycount = SPS_readInteger();
    if( ycount <= 0 )
	    return((SPSHEET *)NULL);
    zcount = SPS_readInteger();
    if( zcount <= 0 )
	    return((SPSHEET *)NULL);
    while( RS_readLine(buf,256) ) ;

    SPS_makeSheet(spr,xcount,ycount,zcount);

    if( SPS_readHeadings(spr,list,FULL) )
	return((SPSHEET *)NULL);

    if( SPS_readCells(spr,list) )
	return((SPSHEET *)NULL);

    return(spr);
}

PublicFnDef int SPS_readCells(SPSHEET *spr, char *list) {
    char buff[256];
    SPS_cell	*c;
    SPS_DataType type;
    int i, width, x, y, z, cwidth, len, axis, error;
    int oldx, oldy, oldz;
    SPS_Heading *xh, *yh, *zh, *ch;

    oldx = spr->idx[X];
    oldy = spr->idx[Y];
    oldz = spr->idx[Z];
    SPS_resetIndices(spr);

    ERR_displayStr("Reading in worksheet cells.  Please wait...",FALSE);
    sprintf(buff,"%s printCells",list);
    RS_dumpOutput();
    RS_sendLine(buff);

    for( z=0 ; z<SPS_ZCount(spr) ; z++ )
    {
	for( y=0 ; y<SPS_YCount(spr) ; y++ )
	{
	    for( x=0 ; x<SPS_XCount(spr) ; x++ )
	    {
		c = SPS_cellPtrXYZ(spr,x,y,z);
		SPS_xyzTypeWidth(spr,x,y,z,type,width);
		error = SPS_readCell(c,type);
		switch(error)
		{
		    case FALSE:
			break;
		    case SPS_GOTPROMPT:
			return(TRUE);
		    case SPS_GOTERROR:
			if( x == 0 )
			{
			    if( y == 0 )
			    {
				if( z == 0 )
				{
				    SPS_cellStatus(c) = SPS_NA;
				    break;
				}
				c = SPS_cellPtrXYZ(spr,
					SPS_XCount(spr)-1,
					SPS_YCount(spr)-1,
					z-1);
				SPS_cellStatus(c) = SPS_NA;
				x--;
				continue;
			    }
			    c = SPS_cellPtrXYZ(spr,
				    SPS_XCount(spr)-1,
				    y-1,
				    z);
			    SPS_cellStatus(c) = SPS_NA;
			    x--;
			    continue;
			}
			c = SPS_cellPtrXYZ(spr,
				x-1,
				y,
				z);
			SPS_cellStatus(c) = SPS_NA;
			x--;
			continue;
		    default:
			break;
		}
		SPS_controllingHeaderXYZ(spr,x,y,z,ch,axis);
		if( (type == SPS_String) ||
		    (type == SPS_RString) ||
		    (type == SPS_CString) )
		{
		    len = strlen(SPS_cellString(c)) + 1;
		    if( len > SPS_headingCWidth(ch) )
		    {
			if( len >= SPS_maxHeadingWidth )
			    SPS_headingCWidth(ch) = SPS_maxHeadingWidth;
			else
			    SPS_headingCWidth(ch) = len;
		    }
		}
		else
		    SPS_headingCWidth(ch) = 12;
		sprintf(buff,"reading cell (%d,%d,%d): type=%d",x,y,z,type);
		ERR_displayStr(buff,FALSE);
	    }
	}
    }

    while( RS_readLine(buff,256) ) ;

    for( i=0 ; i<SPS_XCount(spr) ; i++ )
    {
	xh = SPS_headingPtr(spr,i,X);
	cwidth = SPS_headingCWidth(xh);
	if( cwidth > SPS_maxCellWidth(spr,X) )
	    SPS_maxCellWidth(spr,X) = cwidth;
    }

    for( i=0 ; i<SPS_YCount(spr) ; i++ )
    {
	yh = SPS_headingPtr(spr,i,Y);
	cwidth = SPS_headingCWidth(yh);
	if( cwidth > SPS_maxCellWidth(spr,Y) )
	    SPS_maxCellWidth(spr,Y) = cwidth;
    }

    for( i=0 ; i<SPS_ZCount(spr) ; i++ )
    {
	zh = SPS_headingPtr(spr,i,Z);
	cwidth = SPS_headingCWidth(zh);
	if( cwidth > SPS_maxCellWidth(spr,Z) )
	    SPS_maxCellWidth(spr,Z) = cwidth;
    }

    SPS_repaintScreen(spr) = TRUE;
    ERR_clearMsg();
    spr->idx[X] = oldx;
    spr->idx[Y] = oldy;
    spr->idx[Z] = oldz;

    SPS_sheetStatus(spr) = SPS_Normal;
    return(FALSE);
}

PrivateFnDef int getDate (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    int				width,
    int				firstChar
)
{
    int	i = 0, j, len = 0, c;
    int NotDone, isNA = FALSE;
    char buffer[81], buf[81];

    memset(buffer,0,81);

    CUR_wattron(win,(CUR_A_BOLD | CUR_A_REVERSE));
    for(;;)
    {
	NotDone = TRUE;

	while( NotDone )
	{
	    CUR_wmove(win,row,col);
	    if( isNA )
		CUR_wprintw(win,"%*.*s ",width-1,width-1,"NA");
	    else
		CUR_wprintw(win,"%-*.*s",width,width,buffer);
	    CUR_wmove(win,row,col+i);
	    CUR_wrefresh(win);
	    CUR_forceSetCursor(win,row,col+i);

	    if( firstChar != 0 )
	    {
		c = firstChar;
		firstChar = 0;
	    }
	    else
		c = KEY_getkey(FALSE);

	    switch (c)
	    {
		case 0:
		    break;

		case KEY_PREV:
		case KEY_QUIT:
		    CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
		    return(TRUE);

		case KEY_CR:
		case KEY_EXEC:
		case CUR_KEY_UP:
		case CUR_KEY_DOWN:
		case KEY_SCR_L:
		case KEY_SCR_R:
		case KEY_SCR_U:
		case KEY_SCR_D:
		case KEY_TOP:
		case KEY_BOTTOM:
		case KEY_HELP:
		case KEY_WINDOW:
		case KEY_EDIT:
		case KEY_FILES:
		case KEY_AMENU:
		case KEY_SMENU:
		case CUR_KEY_RIGHT:
		case CUR_KEY_LEFT:
		    NotDone = FALSE;
		    break;

		case KEY_DEL:
		    i = 0;
		    memset(buffer,0,len);
		    len = 0;
		    isNA = TRUE;
		    NotDone = FALSE;
		    break;

		case KEY_BOL:
		    i = 0;
		    break;

		case KEY_VEOL:
		    i = len;
		    break;

		case KEY_REPAINT:
		    CUR_clearok(CUR_curscr);
		    CUR_wrefresh(CUR_curscr);
		    break;

		case KEY_BKSP:
		    if (i > 0)
		    {
			for( j=i ; j<=len ; j++ )
			    buffer[j-1] = buffer[j];
			i--;
			len--;
		    }
		    else
			CUR_beep();
		    break;

		case KEY_QUOTE:
		    KEY_QuoteNextKey = FALSE;
		    CUR_beep();
		    break;

		default:
		    if( isdigit(c) && (len < 9) && !(c & ~0x7f) )
		    {
			buffer[len+1] = '\0';
			for( j=len-1 ; j>=i ; j-- )
			    buffer[j+1] = buffer[j];
			buffer[i++] = c;
			len++;
			isNA = FALSE;
		    }
		    else
			ERR_displayStr("Enter date. (Formats are: YYYYMMDD, YYMMDD, YYMM, YY) (^G = NA, F9 = no change)",FALSE);
		    break;
		    
	    }   /*** switch ***/

	}   /*** while ***/

	if( isNA )
	{
	    SPS_cellStatus(cell) = SPS_ModifiedNA;
	    break;
	}

	switch(len)
	{
	    case 1:
		sprintf(buf,"0%1.1s asDate",buffer);
		break;
	    case 2:
		sprintf(buf,"%2.2s asDate",buffer);
		break;
	    case 3:
		sprintf(buf,"0%1.1s%2.2s asDate",buffer,&buffer[1]);
		break;
	    case 4:
		sprintf(buf,"%2.2s%2.2s asDate",buffer,&buffer[2]);
		break;
	    case 5:
		sprintf(buf,"0%1.1s%4.4s asDate",buffer,&buffer[1]);
		break;
	    case 6:
		sprintf(buf,"%2.2s%4.4s asDate",buffer,&buffer[2]);
		break;
	    case 7:
		sprintf(buf,"%03.3s%4.4s asDate",buffer,&buffer[3]);
		break;
	    case 8:
		sprintf(buf,"%4.4s%4.4s asDate",buffer,&buffer[4]);
		break;
	    case 9:
		sprintf(buf,"%5.5s%4.4s asDate",buffer,&buffer[5]);
		break;
	    default:
		sprintf(buf,"NA asDate");
		break;
	}

	ERR_displayStr(" Validating date...",FALSE);
	if (RS_sendAndCheck(buf, "       NA"))
	{
	    ERR_displayStr(" Invalid date.",TRUE);
	    sleep(1);
	    ERR_displayStr("Enter date. (Formats are: YYYYMMDD, YYMMDD, YYMM, YY) (^G = NA, F9 = no change)",FALSE);
	    continue;
	}

	SPS_cellDate(cell) = atoi(buf);
	SPS_cellStatus(cell) = SPS_Modified;
	if( c != KEY_DEL )
	    KEY_putc(c);
	break;
    }
    CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
    return(FALSE);
}

PrivateFnDef int getIntOrFloat (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    SPS_DataType		type,
    int				width,
    int				firstChar
)
{
    int	i = 0, j, len = 0, c;
    int NotDone, isNA = FALSE, gotDot = FALSE, gotMinus = FALSE;
    char buffer[81];
    double	atof();

    memset(buffer,0,81);

    CUR_wattron(win,(CUR_A_BOLD | CUR_A_REVERSE));

    NotDone = TRUE;

    while( NotDone )
    {
	CUR_wmove(win,row,col);
	if( isNA )
	    CUR_wprintw(win,"%*.*s ",width-1,width-1,"NA");
	else
	    CUR_wprintw(win,"%-*.*s ",width-1,width-1,buffer);
	CUR_wmove(win,row,col+i);
	CUR_wrefresh(win);
	CUR_forceSetCursor(win,row,col+i);

	if( firstChar != 0 )
	{
	    c = firstChar;
	    firstChar = 0;
	}
	else
	    c = KEY_getkey(FALSE);

	switch (c)
	{
	    case 0:
		break;

	    case KEY_PREV:
	    case KEY_QUIT:
		CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
		return(TRUE);

	    case KEY_CR:
	    case KEY_EXEC:
	    case CUR_KEY_UP:
	    case CUR_KEY_DOWN:
	    case KEY_SCR_L:
	    case KEY_SCR_R:
	    case KEY_SCR_U:
	    case KEY_SCR_D:
	    case KEY_TOP:
	    case KEY_BOTTOM:
	    case KEY_HELP:
	    case KEY_WINDOW:
	    case KEY_FILES:
	    case KEY_EDIT:
	    case KEY_AMENU:
	    case KEY_SMENU:
	    case CUR_KEY_RIGHT:
	    case CUR_KEY_LEFT:
		if (len == 0)
		{
		    ERR_displayStr(" Invalid number.",TRUE);
		    sleep(1);
		    if( type == SPS_Int )
			ERR_displayStr("Enter an integer. (^G = NA, F9 = no change)",FALSE);
		    else
			ERR_displayStr("Enter a floating point number. (^G = NA, F9 = no change)",FALSE);
		}
		else
		    NotDone = FALSE;
		break;

	    case KEY_DEL:
		i = 0;
		memset(buffer,0,len);
		len = 0;
		isNA = TRUE;
		NotDone = FALSE;
		break;

	    case KEY_BOL:
		i = 0;
		break;

	    case KEY_VEOL:
		i = len;
		break;

	    case KEY_REPAINT:
		CUR_clearok(CUR_curscr);
		CUR_wrefresh(CUR_curscr);
		break;

	    case KEY_BKSP:
		if (i > 0)
		{
		    if( buffer[i-1] == '.' )
			gotDot = FALSE;
		    if( buffer[i-1] == '-' )
			gotMinus = FALSE;
		    for( j=i ; j<=len ; j++ )
			buffer[j-1] = buffer[j];
		    i--;
		    len--;
		}
		else
		    CUR_beep();
		break;

	    case KEY_QUOTE:
		KEY_QuoteNextKey = FALSE;
		CUR_beep();
		break;

	    default:
		if( (isdigit(c) || 
		     ((c == '-') && (i == 0) && !gotMinus) ||
		     ((type == SPS_Float) && (c == '.') && !gotDot)) && 
		    (len < (width-1)) && !(c & ~0xff) )
		{
		    if( c == '-' )
			gotMinus = TRUE;
		    if( c == '.' )
			gotDot = TRUE;
		    buffer[len+1] = '\0';
		    for( j=len-1 ; j>=i ; j-- )
			buffer[j+1] = buffer[j];
		    buffer[i++] = c;
		    len++;
		    isNA = FALSE;
		}
		else
		{
		    if( type == SPS_Int )
			ERR_displayStr("Enter an integer. (^G = NA, F9 = no change)",FALSE);
		    else
			ERR_displayStr("Enter a floating point number. (^G = NA, F9 = no change)",FALSE);
		}
		break;
		
	}   /*** switch ***/

    }   /*** while ***/

    if( isNA )
	SPS_cellStatus(cell) = SPS_ModifiedNA;
    else
    {
	if( type == SPS_Int )
	    SPS_cellInt(cell) = atoi(buffer);
	else
	    SPS_cellFloat(cell) = atof(buffer);
	SPS_cellStatus(cell) = SPS_Modified;
    }

    if( c != KEY_DEL )
	KEY_putc(c);
    CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
    return(FALSE);
}

PrivateFnDef int getString (
    CUR_WINDOW *		win,
    SPS_cell *			cell,
    int				row,
    int				col,
    int				width,
    int				firstChar
)
{
    int	i = 0, j, offset = 0, len = 0, c;
    int NotDone, isNA = FALSE;
    char buffer[SPS_maxItemWidth+1];
    double	atof();

    memset(buffer,0,SPS_maxItemWidth+1);

    CUR_wattron(win,(CUR_A_BOLD | CUR_A_REVERSE));

    NotDone = TRUE;

    while( NotDone )
    {
	CUR_wmove(win,row,col);
	if( isNA )
	    CUR_wprintw(win,"%*.*s ",width-1,width-1,"NA");
	else
	    CUR_wprintw(win,"%-*.*s",width,width,&buffer[offset]);

	CUR_wmove(win,row,col+(i-offset));
	CUR_wrefresh(win);
	CUR_forceSetCursor(win,row,col+(i-offset));

	if( firstChar != 0 )
	{
	    c = firstChar;
	    firstChar = 0;
	}
	else
	    c = KEY_getkey(FALSE);

	switch (c)
	{
	    case 0:
		break;

	    case KEY_PREV:
	    case KEY_QUIT:
		CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
		return(TRUE);

	    case KEY_CR:
	    case KEY_EXEC:
	    case CUR_KEY_UP:
	    case CUR_KEY_DOWN:
	    case KEY_SCR_L:
	    case KEY_SCR_R:
	    case KEY_SCR_U:
	    case KEY_SCR_D:
	    case KEY_TOP:
	    case KEY_BOTTOM:
	    case KEY_HELP:
	    case KEY_WINDOW:
	    case KEY_FILES:
	    case KEY_EDIT:
	    case KEY_AMENU:
	    case KEY_SMENU:
	    case CUR_KEY_RIGHT:
	    case CUR_KEY_LEFT:
		NotDone = FALSE;
		break;

	    case KEY_DEL:
		i = 0;
		offset = 0;
		memset(buffer,0,len);
		len = 0;
		isNA = TRUE;
		NotDone = FALSE;
		break;

	    case KEY_BOL:
		i = offset = 0;
		break;

	    case KEY_VEOL:
		i = offset = len;
		break;

	    case KEY_REPAINT:
		CUR_clearok(CUR_curscr);
		CUR_wrefresh(CUR_curscr);
		break;

	    case KEY_BKSP:
		if (i > 0)
		{
		    for( j=i ; j<=len ; j++ )
			buffer[j-1] = buffer[j];
		    if( i == offset )
			offset--;
		    i--;
		    len--;
		}
		else
		    CUR_beep();
		break;

	    case KEY_QUOTE:
		KEY_QuoteNextKey = FALSE;
		CUR_beep();
		break;

	    default:
		if( isprint(c) && (len < (SPS_maxItemWidth-1)) && !(c & ~0xff) )
		{
		    buffer[len+1] = '\0';
		    for( j=len-1 ; j>=i ; j-- )
			buffer[j+1] = buffer[j];
		    if( (i - offset) >= width )
			offset++;
		    buffer[i++] = c;
		    len++;
		    isNA = FALSE;
		}
		else
		    ERR_displayStr("Enter a string. (^G = NA, F9 = no change)",FALSE);
		break;
		
	}   /*** switch ***/

    }   /*** while ***/

    if( isNA )
	SPS_cellStatus(cell) = SPS_ModifiedNA;
    else
    {
	if( SPS_cellString(cell) != NULL )
	{
	    free( SPS_cellString(cell) );
	    SPS_cellString(cell) = NULL;
	}
	SPS_addString(SPS_cellString(cell),buffer);
	SPS_cellStatus(cell) = SPS_Modified;
    }

    if( c != KEY_DEL )
	KEY_putc(c);
    CUR_wattroff(win,(CUR_A_BOLD | CUR_A_REVERSE));
    return(FALSE);
}

PrivateFnDef int getAxis(char *buf) {
    switch(toLower (buf[0]))
    {
	case 'x':	return(X);
	case 'y':	return(Y);
	case 'z':	return(Z);
	case 'c':	return(CELL);
	default:	return(-1);
    }
}

PublicFnDef int SPS_writeSSheet(SPSHEET *spr, char *list) {
    int x, y, z, Arg1Axis, Arg2Axis, Arg3Axis, Arg4Axis, width;
    int oldx, oldy, oldz;
    char InitString[256], CleanupString[256], FormatString[256];
    char Arg1buf[81], Arg2buf[81], Arg3buf[81], Arg4buf[81];
    char buf[256];
    SPS_cell *c;
    SPS_DataType type;
    SPS_Heading *xh, *yh, *zh;

    ERR_displayStr("Saving modifications to the worksheet.  Please wait...",FALSE);
    RS_dumpOutput();
    sprintf(buf,"%s initString",list);
    RS_sendLine(buf);
    if( SPS_readString(InitString,256) != SPS_NOERROR )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }

    sprintf(buf,"%s cleanupString",list);
    RS_sendLine(buf);
    if( SPS_readString(CleanupString,256) != SPS_NOERROR )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }

    sprintf(buf,"%s formatString",list);
    RS_sendLine(buf);
    if( SPS_readStringOnly(FormatString,256) != SPS_NOERROR )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }
    if( (SPS_readStringOnly(Arg1buf,80) != SPS_NOERROR) ||
	((Arg1Axis = getAxis(Arg1buf)) < 0) )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }
    if( (SPS_readStringOnly(Arg2buf,80) != SPS_NOERROR) ||
	((Arg2Axis = getAxis(Arg2buf)) < 0) )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }
    if( (SPS_readStringOnly(Arg3buf,80) != SPS_NOERROR) ||
	((Arg3Axis = getAxis(Arg3buf)) < 0) )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }
    if( (SPS_readStringOnly(Arg4buf,80) != SPS_NOERROR) ||
	((Arg4Axis = getAxis(Arg4buf)) < 0) )
    {
	ERR_displayPause("Unable to write worksheet");
	return(TRUE);
    }
    while( RS_readLine(buf,256) ) ;

    oldx = spr->idx[X];
    oldy = spr->idx[Y];
    oldz = spr->idx[Z];
    SPS_resetIndices(spr);

    RS_sendOnly(InitString);
    for( z=0 ; z<SPS_ZCount(spr) ; z++ )
    {
	zh = SPS_headingPtr(spr,z,Z);
	if( Arg1Axis == Z )	 strcpy(Arg1buf,SPS_itemAsString(zh));
	else if( Arg2Axis == Z ) strcpy(Arg2buf,SPS_itemAsString(zh));
	else if( Arg3Axis == Z ) strcpy(Arg3buf,SPS_itemAsString(zh));
	else if( Arg4Axis == Z ) strcpy(Arg4buf,SPS_itemAsString(zh));
	for( y=0 ; y<SPS_YCount(spr) ; y++ )
	{
	    yh = SPS_headingPtr(spr,y,Y);
	    if( Arg1Axis == Y )	     strcpy(Arg1buf,SPS_itemAsString(yh));
	    else if( Arg2Axis == Y ) strcpy(Arg2buf,SPS_itemAsString(yh));
	    else if( Arg3Axis == Y ) strcpy(Arg3buf,SPS_itemAsString(yh));
	    else if( Arg4Axis == Y ) strcpy(Arg4buf,SPS_itemAsString(yh));
	    for( x=0 ; x<SPS_XCount(spr) ; x++ )
	    {
		xh = SPS_headingPtr(spr,x,X);
		if( Arg1Axis == X )      strcpy(Arg1buf,SPS_itemAsString(xh));
		else if( Arg2Axis == X ) strcpy(Arg2buf,SPS_itemAsString(xh));
		else if( Arg3Axis == X ) strcpy(Arg3buf,SPS_itemAsString(xh));
		else if( Arg4Axis == X ) strcpy(Arg4buf,SPS_itemAsString(xh));

		c = SPS_cellPtrXYZ(spr,x,y,z);
		if( (SPS_cellStatus(c) == SPS_Normal) ||
		    (SPS_cellStatus(c) == SPS_NA) ||
		    (SPS_cellStatus(c) == SPS_SavedNA) ||
		    (SPS_cellStatus(c) == SPS_Saved) )
		    continue;
		SPS_xyzTypeWidth(spr,x,y,z,type,width);
		if( Arg1Axis == CELL )      strcpy(Arg1buf,SPS_cellAsString(c,type));
		else if( Arg2Axis == CELL ) strcpy(Arg2buf,SPS_cellAsString(c,type));
		else if( Arg3Axis == CELL ) strcpy(Arg3buf,SPS_cellAsString(c,type));
		else if( Arg4Axis == CELL ) strcpy(Arg4buf,SPS_cellAsString(c,type));

		sprintf(buf,FormatString,Arg1buf,Arg2buf,Arg3buf,Arg4buf);
		RS_sendOnly(buf);
		if( SPS_cellStatus(c) == SPS_ModifiedNA )
		    SPS_cellStatus(c) = SPS_SavedNA;
		else
		    SPS_cellStatus(c) = SPS_Saved;
	    }
	}
    }
    RS_dumpOutput();
    if( RS_sendAndCheck(CleanupString,">>>") )
    {
	ERR_displayPause("Error writing worksheet");
	return(TRUE);
    }

    spr->idx[X] = oldx;
    spr->idx[Y] = oldy;
    spr->idx[Z] = oldz;
    SPS_repaintScreen(spr) = TRUE;
    SPS_sheetStatus(spr) = SPS_Normal;
    ERR_clearMsg();
    RS_DataWasUpdated = TRUE;
    return(FALSE);
}
