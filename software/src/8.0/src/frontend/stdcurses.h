/***************************************************************/
/* CURses.h                                                    */
/* Mask for system package: curses.                            */
/***************************************************************/

#ifndef STDCURSES_H
#define STDCURSES_H

#include "SItermio.h"

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#include <curses.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE
#endif


/**********************
 ***   Invariants   ***
 **********************/

/* Control Macro  */
#define STD_CTRL(c) ((c)&037)

/* Window structure */
typedef WINDOW CUR_WINDOW;

#define CUR_maxScrLines	67
#define CUR_maxScrCols	256

/* Curses' Variables */
#define CUR_stdscr	stdscr
#define CUR_curscr 	curscr
#define CUR_COLS  	COLS
#define CUR_LINES	LINES

#define CUR_clearok(win)		clearok(win,TRUE)
#define CUR_delwin(win)			delwin(win)
#define CUR_endwin()			endwin()
#define CUR_erase()			erase()
#define CUR_wclear(win)			wclear(win)
#define CUR_getyx(win,y,x)		getyx(win,y,x)
#define CUR_initscr()			initscr()
#define CUR_mvwaddch(win,y,x,ch)	mvwaddch(win,y,x,ch)
#define CUR_mvwaddstr(win,y,x,str)	mvwaddstr(win,y,x,str)
#define CUR_wprintw			wprintw
#define CUR_newwin(num_lines,num_cols,beg_y,beg_x)\
					newwin(num_lines,num_cols,beg_y,beg_x)
#define CUR_noecho()			noecho()
#define CUR_echo()			echo()
#define CUR_nonl()			nonl()
#define CUR_nl()			nl()
#define CUR_refresh()			refresh()
#define CUR_subwin(win,lines,cols,beg_y,beg_x)\
					subwin(win,lines,cols,beg_y,beg_x)
#define CUR_touchwin(win)		touchwin(win)
#define CUR_waddch(win,ch)		waddch(win,ch)
#define CUR_waddstr(win,str)		waddstr(win,str)
#define CUR_werase(win)			werase(win)
#define CUR_wmove(win,y,x)		wmove(win,y,x)

/***************************************************************/
/* CURses.h                                                    */
/* Mask for system package: curses.                            */
/***************************************************************/

/*********************************
 *****  Solaris 1.x (SunOS)  *****
 *********************************/

#if defined(solaris_1)

/* Curses' Attributes */
#define CUR_A_NORMAL		0
#define CUR_A_REVERSE   	_STANDOUT
#define CUR_A_BOLD		CUR_A_NORMAL
#define CUR_A_BLINK		CUR_A_NORMAL
#define CUR_A_UNDERLINE		CUR_A_NORMAL
#define CUR_A_DIM		CUR_A_NORMAL
#define CUR_A_ALTCHARSET	CUR_A_NORMAL
#define CUR_A_ALLATTRS		(CUR_A_REVERSE)

/* Access to window structure */
#define CUR_WIN_cury(win)	((win)->_cury)
#define CUR_WIN_curx(win)	((win)->_curx)
#define CUR_WIN_rows(win)	((win)->_maxy)
#define CUR_WIN_cols(win)	((win)->_maxx)
#define CUR_WIN_maxy(win)	((win)->_maxy - 1)
#define CUR_WIN_maxx(win)	((win)->_maxx - 1)

/* Curses' functions */
#undef CUR_initscr
#define CUR_initscr()                   SUN_initscr()

#define CUR_beep()			/* beep() */
#define CUR_tgetnum(id)			tgetnum(id)
#define CUR_wgetch(scr)			wgetch(scr)
#define CUR_nodelay(scr,flag)           /* nodelay(scr, flag) */
#define CUR_keypad(scr,flag)		/* keypad(scr,flag) */
#define CUR_cbreak()			cbreak()
#define CUR_nocbreak()			nocbreak()
#define CUR_noraw()			noraw()
#define CUR_doupdate()			1 /* doupdate() */
#define CUR_resetty()			resetty()
#define CUR_savetty()			savetty()
#define CUR_wattroff(win,attrs)		wstandend(win)
#define CUR_wattron(win,attrs)		((attrs == CUR_A_NORMAL) ?\
					 wstandend(win) :\
					 wstandout(win))
#define CUR_wnoutrefresh(win)		wrefresh(win)
#define CUR_wrefresh(win)		wrefresh(win)
#define CUR_forceSetCursor(win,row,col)	/* Not needed on Hp */
#define	CUR_saveterm()			SUN_def_prog_mode()
#define	CUR_fixterm()			SUN_reset_prog_mode()
#define	CUR_resetterm()			SUN_reset_shell_mode()
#define	CUR_idlok(win,flag)		idlok(win,flag)
#define	CUR_has_il()			has_il()

/* Curses's KEY definitions */
#define CUR_KEY_DOWN 	1000
#define CUR_KEY_UP 	1001
#define CUR_KEY_LEFT	1002
#define CUR_KEY_RIGHT	1003
#define CUR_KEY_F1	1004
#define CUR_KEY_F2	1005
#define CUR_KEY_F3	1006
#define CUR_KEY_F4	1007
#define CUR_KEY_F5	1008
#define CUR_KEY_F6	1009
#define CUR_KEY_F7	1010
#define CUR_KEY_F8	1011
#define CUR_KEY_F9	1012
#define CUR_KEY_F10	1013
#define KEY_HOME	1014
#define KEY_LL		1015
#define KEY_DC		1016
#define KEY_NPAGE	1017
#define KEY_PPAGE	1018
#define KEY_SF		1019

#else

/*********************************
 *****  All Other Platforms  *****
 *********************************/

/* Curses' Attributes */
#define CUR_A_NORMAL		A_NORMAL
#define CUR_A_REVERSE   	A_REVERSE
#define CUR_A_BOLD		A_BOLD
#define CUR_A_BLINK		A_BLINK
#define CUR_A_UNDERLINE		A_UNDERLINE
#define CUR_A_DIM		A_DIM
#define CUR_A_ALTCHARSET	A_ALTCHARSET
#define CUR_A_ALLATTRS		(A_ALTCHARSET | A_DIM | A_UNDERLINE | A_BOLD | A_REVERSE)

/* Access to window structure */
#define CUR_WIN_cury(win)	((win)->_cury)
#define CUR_WIN_curx(win)	((win)->_curx)
#define CUR_WIN_rows(win)	((win)->_maxy)
#define CUR_WIN_cols(win)	((win)->_maxx)
#define CUR_WIN_maxy(win)	((win)->_maxy - 1)
#define CUR_WIN_maxx(win)	((win)->_maxx - 1)

/* Curses' functions */
#if defined(__hp9000s700)
#undef CUR_delwin
#define CUR_delwin(win) STD_delwin (win)
#endif

#define CUR_beep()			beep()
#define CUR_tgetnum(id)			tgetnum(id)
#define CUR_wgetch(scr)			wgetch(scr)
#define CUR_nodelay(scr,flag)		nodelay(scr,flag)
#define CUR_keypad(scr,flag)		keypad(scr,flag)
#define CUR_cbreak()			cbreak()
#define CUR_nocbreak()			nocbreak()
#define CUR_noraw()			noraw()
#define CUR_doupdate()			doupdate()
#define CUR_resetty()			resetty()
#define CUR_savetty()			savetty()
#define CUR_wattroff(win,attrs)		wattroff(win,attrs)
#define CUR_wattron(win,attrs)		((attrs == CUR_A_NORMAL) ? \
					 wattroff(win,CUR_A_ALLATTRS) : \
					 wattron(win,attrs))
#define CUR_wnoutrefresh(win)		wnoutrefresh(win)
#define CUR_wrefresh(win)		wrefresh(win)
#define CUR_forceSetCursor(win,row,col)	/* Not needed on Hp */
#define	CUR_saveterm()			saveterm()
#define	CUR_fixterm()			fixterm()
#define	CUR_resetterm()			resetterm()
#define	CUR_idlok(win,flag)		idlok(win,flag)
#define	CUR_has_il()			has_il()

/* Curses's KEY definitions */
#define CUR_KEY_DOWN 	KEY_DOWN
#define CUR_KEY_UP 	KEY_UP
#define CUR_KEY_LEFT	KEY_LEFT
#define CUR_KEY_RIGHT	KEY_RIGHT
#define CUR_KEY_F1	KEY_F(1)
#define CUR_KEY_F2	KEY_F(2)
#define CUR_KEY_F3	KEY_F(3)
#define CUR_KEY_F4	KEY_F(4)
#define CUR_KEY_F5	KEY_F(5)
#define CUR_KEY_F6	KEY_F(6)
#define CUR_KEY_F7	KEY_F(7)
#define CUR_KEY_F8	KEY_F(8)
#define CUR_KEY_F9	KEY_F(9)
#define CUR_KEY_F10	KEY_F(10)

#endif  /* defined(__hp9000s700)||defined(solaris_2) */

#endif  /* STDCURSES_H */
