/*****  Machine Specific Terminal IO Services Header File  *****/
#ifndef siTERMIO_H
#define siTERMIO_H

PublicVarDecl int	STD_delwinDoesNotRefresh,
			STD_doNotUseNodelay,
			STD_hasInsertDeleteLine;
			
PublicFnDecl void	STD_checkTerminalCapabilities();
PublicFnDecl int	STD_checkInputStream();
PublicFnDecl void	STD_cleanupTerminal();

#endif /* siTERMIO_H */
