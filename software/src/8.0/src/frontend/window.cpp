/****************************************************************************
*****									*****
*****			window.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"

PublicFnDef void WIN_LineBox(
    CUR_WINDOW *win, char const *title, char const *info
) {
    int i;
    char tbuf[81];

    CUR_wattron(win, CUR_A_ALTCHARSET);
    
/**** horizontal lines *****/
    CUR_wmove(win, 0, 1);
    for (i = 1; i < CUR_WIN_maxx(win); i++)
	CUR_waddch(win, 'q');
    CUR_wmove(win, CUR_WIN_maxy(win), 1);
    for (i = 1; i < CUR_WIN_maxx(win); i++)
	CUR_waddch(win, 'q');

/**** vertical lines *****/
    for (i = 1; i < CUR_WIN_maxy(win); i++)
	CUR_mvwaddch(win, i, 0, 'x');
    for (i = 1; i < CUR_WIN_maxy(win); i++)
	CUR_mvwaddch(win, i, CUR_WIN_maxx(win), 'x');

/**** corners ****/
    CUR_mvwaddch(win, 0, 0, 'l');
    CUR_mvwaddch(win, 0, CUR_WIN_maxx(win), 'k');
    CUR_mvwaddch(win, CUR_WIN_maxy(win), 0, 'm');
    CUR_mvwaddch(win, CUR_WIN_maxy(win), CUR_WIN_maxx(win), 'j');

    CUR_wattroff(win, CUR_A_ALTCHARSET);
    if (title != NULL)
    {
        CUR_wmove(win, 0, 0);
	sprintf(tbuf, "%-.*s",(CUR_WIN_cols(win)-1),title);
	CUR_wprintw(win, "%s",tbuf);
    }
    if (info != NULL)
    {
        CUR_wmove(win, CUR_WIN_maxy(win), 1);
	sprintf(tbuf, "%-.*s",(CUR_WIN_cols(win)-2),info);
	CUR_wprintw(win, "%s",tbuf);
    }	
}

PublicFnDef void WIN_RepaintWindow(CUR_WINDOW *win, int attr) {
    static int i, j;

    if( attr == CUR_A_NORMAL )
    {
    	CUR_werase(win);
    	return;
    }

    CUR_wattron(win, attr);

    for (j = 0; j < CUR_WIN_rows(win); j++) {
	CUR_wmove(win, j, 0);
	for (i = 0; i < CUR_WIN_cols(win); i++)
		CUR_waddch(win, ' ');
    }
	
    CUR_wattroff(win, attr);
}

PublicFnDef void WIN_HighlightBox(
    CUR_WINDOW *win, int attr, char const *title, char const *info
) {
    static int i;
    static int first = TRUE;
    static int special = FALSE;
    char *tmp, tbuf[81];

    if( first ) {
	first = FALSE;
	tmp = getenv("TERM");
	if( (tmp != NULL) && (strcmp("leedata",tmp) == 0) )
		special = TRUE;
	else if( (tmp != NULL) && (strcmp("leedata2",tmp) == 0) )
		special = TRUE;
    }

    if( special ) {
	WIN_LineBox(win,title,info);
	return;
    }

    CUR_wattron(win, attr);

/**** horizontal lines *****/
    CUR_wmove(win, 0, 0);
    for (i = 0; i <= CUR_WIN_maxx(win); i++)
	CUR_waddch(win, ' ');
    CUR_wmove(win, CUR_WIN_maxy(win), 0);
    for (i = 0; i <= CUR_WIN_maxx(win); i++)
	CUR_waddch(win, ' ');

/**** vertical lines *****/
    for (i = 1; i < CUR_WIN_maxy(win); i++)
	CUR_mvwaddch(win, i, 0, ' ');
    for (i = 1; i < CUR_WIN_maxy(win); i++)
	CUR_mvwaddch(win, i, CUR_WIN_maxx(win), ' ');

    if (title != NULL)
    {
        CUR_wmove(win, 0, 0);
	sprintf(tbuf, "%-.*s",(CUR_WIN_cols(win)-1),title);
	CUR_wprintw(win, "%s",tbuf);
    }
    if (info != NULL)
    {
        CUR_wmove(win, CUR_WIN_maxy(win), 1);
	sprintf(tbuf, "%-.*s",(CUR_WIN_cols(win)-2),info);
	CUR_wprintw(win, "%s",tbuf);
    }	
	
    CUR_wattroff(win, attr);
}

PublicFnDef void WIN_ReverseBox(CUR_WINDOW *win, char const *title) {
	WIN_HighlightBox(win, CUR_A_REVERSE, title, NULL);
}

PublicFnDef void WIN_ShadowBox(CUR_WINDOW *win) {
    int i;

    CUR_wattron(win, CUR_A_ALTCHARSET);
    for (i = 0; i < CUR_WIN_cols(win); i++)	/**** top of box ***/
	CUR_mvwaddch(win, 0, i, 'q');
    for (i = 0; i < CUR_WIN_rows(win); i++)	/**** left side ****/
	CUR_mvwaddch(win, i, 0, 'x');
    CUR_mvwaddch(win, 0, 0, 'l');		/**** upper left corner ****/
    for (i = 0; i < CUR_WIN_cols(win); i++)	/**** bottom of box ****/
	CUR_mvwaddch(win, CUR_WIN_maxy(win), i, 'q');    
    CUR_mvwaddch(win, CUR_WIN_maxy(win), 0, 'm');
    CUR_wattroff(win, CUR_A_ALTCHARSET);

    CUR_wattron(win, CUR_A_REVERSE);
    for (i = 0; i < CUR_WIN_rows(win); i++)	/**** right side ****/
	CUR_mvwaddch(win, i, CUR_WIN_maxx(win), ' ');
    CUR_wattroff(win, CUR_A_REVERSE);

}
