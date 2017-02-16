#ifndef VARS_H
#define VARS_H

#include "vars.d"

PublicVarDecl char	StartupKit[];
PublicVarDecl void	(*StartupModule)(void);
PublicVarDecl void	(*DefaultModule)(void);

PublicVarDecl VARS_Menu	NameAndFunc[];

PublicFnDecl int	VARS_initProfileVariables();
PublicFnDecl int	VARS_funcFromModuleName(char const *name);
PublicFnDecl int 	VARS_moduleNameFromFunc(void (*funcAddr)());
PublicFnDecl int	VARS_runProfile(PAGE *page);

#endif /* VARS_H */
