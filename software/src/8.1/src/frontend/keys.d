/****************************************************************************
*****									*****
*****			keys.d		         			*****
*****	The keys not defined here are defined in "curses.h".		*****
****************************************************************************/
#ifndef KEYS_D
#define KEYS_D

#include "stdcurses.h"

#define KEY_CR	    015    /* These numbers are octal ascii codes */
#define KEY_ESC	    033
#define KEY_SPACE   040
#define KEY_BKSP    127

#ifdef KEY_HELP
#undef KEY_HELP
#endif
#define KEY_HELP    CUR_KEY_F1
#define KEY_EXEC    CUR_KEY_F2
#define KEY_WINDOW  CUR_KEY_F3
#define KEY_EDIT    CUR_KEY_F4
#define KEY_FILES   CUR_KEY_F5
#define KEY_REGIONS CUR_KEY_F6
#define KEY_SMENU   CUR_KEY_F7
#define KEY_AMENU   CUR_KEY_F8
#define KEY_PREV    CUR_KEY_F9
#define KEY_QUIT    CUR_KEY_F10
/* Don't use KEY_EOL because curses has a different meaning for this */
#define KEY_VEOL    STD_CTRL('E')
#define KEY_BOL	    STD_CTRL('A')
#define KEY_DEL	    STD_CTRL('G')
#define KEY_DELEOL  STD_CTRL('K')
#define KEY_FCH	    STD_CTRL('F')
#define KEY_BKCH    STD_CTRL('B')
#ifdef KEY_BTAB
#undef KEY_BTAB
#endif
#define KEY_BTAB    STD_CTRL('A')
#define KEY_TAB	    STD_CTRL('I')
#define KEY_NL	    STD_CTRL('N')
#define KEY_UPL	    STD_CTRL('P')
#define KEY_REPAINT STD_CTRL('W')

#define KEY_SCR_L   STD_CTRL('L')
#define KEY_SCR_R   STD_CTRL('R')
#define KEY_SCR_U   STD_CTRL('U')
#define KEY_SCR_D   STD_CTRL('D')
#define KEY_TOGGLEWIN   STD_CTRL('T')

#define KEY_INCREMENTZ  STD_CTRL('N')
#define KEY_DECREMENTZ  STD_CTRL('P')
#define KEY_ROTATE      STD_CTRL('F')
#define KEY_TRANSPOSE   STD_CTRL('T')

#define KEY_META    STD_CTRL('X')
#define KEY_QUOTE   STD_CTRL('V')

#define KEY_TOP	    KEY_HOME
#define KEY_BOTTOM  KEY_LL
#define KEY_RECALL  0541
#define KEY_ZEROROW 0542
#define KEY_SEARCH  0543	
#ifdef KEY_SNEXT
#undef KEY_SNEXT
#endif
#define KEY_SNEXT   0544
#define KEY_SPREV   0545
#define KEY_BEGREGION   0546
#define KEY_ENDREGION   0547
#ifdef KEY_REPLACE
#undef KEY_REPLACE
#endif
#define KEY_REPLACE     0550
#define KEY_QUERYR      0551
#define KEY_READFILE    0552
#define KEY_INSERTREG   0553
#define KEY_WORDFORW	0554
#define KEY_WORDBACK	0555
#define KEY_WORDDELC	0556
#define KEY_WORDDELP	0557
#define KEY_SCROLLUP1	0560
#define KEY_SCROLLDN1	0561
#define KEY_ONEWINDOW	0562
#define KEY_TWOWINDOW	0563
#define KEY_ERASEWIN	0564
#define KEY_SAVEFILE	0565
#define KEY_WRITEFILE	0566
#define KEY_SCRIPTR	0567
#define KEY_SCRIPTW	0570
#define KEY_SUBSHELL	0571
#define KEY_DELREGION	0572
#define KEY_DESCRIBE	0573
#define KEY_REPEAT	0574
#define KEY_COPYPB	0575

#endif
