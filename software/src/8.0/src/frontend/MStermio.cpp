/*****  Machine Specific Terminal IO *****/

#include "Vk.h"

#include "V_VString.h"

#include <curses.h>
#include <term.h>

#include "SItermio.h"


/*******************************************
 *****  CURSES compatibility routines  *****
 *******************************************/

PublicVarDef int	STD_hasInsertDeleteLine = FALSE;
PublicVarDef int	STD_doNotUseNodelay = FALSE;
PublicVarDef int	STD_delwinDoesNotRefresh = TRUE;

#if defined(__hp9000s700)
PublicFnDef int STD_delwin (WINDOW *win)
{
    int	    result;
    result = delwin (win);
    clearok (curscr, TRUE);
    return result;
}
#endif

PublicFnDef void STD_checkTerminalCapabilities() {
    static char entry[1024];
    static int  entryIsntValid = TRUE;
    char *areap, area[1024];

    if (entryIsntValid)
    {
	char const *term = getenv ("TERM");
	VString iTerm (term ? term : "vt100", false);

	entryIsntValid = tgetent (entry, iTerm.storage ()) != 1;
    }

    char dl[] = "dl"; char DL[] = "DL";
    char al[] = "al"; char AL[] = "AL";
    areap = area;
    if( ((NULL != tgetstr(dl,&areap)) || (NULL != tgetstr(DL,&areap))) &&
	((NULL != tgetstr(al,&areap)) || (NULL != tgetstr(AL,&areap))) )
    	STD_hasInsertDeleteLine = TRUE;
    else
    	STD_hasInsertDeleteLine = FALSE;
}

PublicFnDef void
STD_cleanupTerminal()
{
}


#if 0 // defined(sun)
#include <sgtty.h>

PrivateVarDef struct sgttyb	ShellModeTerminalState,
				ProgramModeTerminalState;

PublicFnDef int
    SUN_def_shell_mode		()
{
    gtty (0, &ShellModeTerminalState);
}

PublicFnDef int
    SUN_reset_shell_mode	()
{
    stty (0, &ShellModeTerminalState);
}

PublicFnDef int
    SUN_def_prog_mode		()
{
    gtty (0, &ProgramModeTerminalState);
}

PublicFnDef int
    SUN_reset_prog_mode		()
{
    stty (0, &ProgramModeTerminalState);
}

PublicFnDef int
    SUN_initscr			()
{
    SUN_def_shell_mode	();
    initscr		();
}
#endif

#if defined (sun)
PublicFnDef int STD_checkInputStream()
{
    STD_doNotUseNodelay = TRUE;

    return(FALSE);
}

#else

PublicFnDef int STD_checkInputStream()
{
    return(FALSE);
}
#endif /* sun */
