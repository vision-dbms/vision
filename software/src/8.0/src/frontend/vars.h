#ifndef VARS_H
#define VARS_H

#include "vars.d"

PublicVarDecl char	StartupKit[];
PublicVarDecl void	(*StartupModule)(void);
PublicVarDecl void	(*DefaultModule)(void);

PublicVarDecl VARS_Menu	NameAndFunc[];

PublicFnDecl int	VARS_initProfileVariables();
PublicFnDecl int	VARS_funcFromModuleName();
PublicFnDecl int	VARS_runProfile();

#endif /* VARS_H */
