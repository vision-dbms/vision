#ifndef VARS_H
#define VARS_H

#include "vars.d"

PublicVarDecl char	StartupKit[];
PublicVarDecl int	(*StartupModule)();
PublicVarDecl int	(*DefaultModule)();

PublicVarDecl VARS_Menu	NameAndFunc[];

PublicFnDecl int	VARS_initProfileVariables();
PublicFnDecl int	VARS_funcFromModuleName();
PublicFnDecl int	VARS_runProfile();

#endif /* VARS_H */
