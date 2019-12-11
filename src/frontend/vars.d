#ifndef VARS_D
#define VARS_D

#include "V_VString.h"
#include "menu.h"

#define	VARS_maxLen	256

#define	VARS_noType	0
#define	VARS_intType	1
#define VARS_stringType	2
#define	VARS_dateType	3
#define	VARS_floatType	4
#define	VARS_funcType	5
#define	VARS_boolType	6

typedef struct {
	char const *name;
	void	(*func)();
} VARS_Menu;

#define VARS_menuName(m)	((m)->name)
#define VARS_menuFunc(m)	((m)->func)

typedef struct {
	char const	*name;
	int		type;
	caddr_t		p;
	V::VString	menustring;
	MENU::Reference m;
} VARS_Type;

#define	VARS_varName(v)		((v)->name)
#define	VARS_valueType(v)	((v)->type)
#define VARS_menuString(v)	((v)->menustring)
#define VARS_menu(v)		((v)->m)

#define	VARS_boolean(v)		(*(int *)((v)->p))
#define	VARS_int(v)		(*(int *)((v)->p))
#define	VARS_string(v)		((char *)((v)->p))
#define	VARS_date(v)		(*(int *)((v)->p))
#define	VARS_float(v)		(*(float *)((v)->p))
#define	VARS_func(v)		(*(void (**)())((v)->p))

#endif /* VARS_D */
