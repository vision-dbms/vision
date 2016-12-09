/*****  Selector Facility Header File  *****/
#ifndef SELECTOR_Interface
#define SELECTOR_Interface


/*****  Shared Definitions  *****/
#include "selector.d"

/*****  String <-> Known Selector Index Conversion Routines  *****/
PublicFnDecl int SELECTOR_StringToKSI (
    char const *		string
);

PublicFnDecl char const *KS__ToString (
    unsigned int		ksi
);

/*****  Known Selector Valence Access Routine  *****/
PublicFnDecl int SELECTOR_KSValence (
    unsigned int		ksi
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  selector.h 1 replace /users/mjc/system
  860404 09:14:05 mjc Selector services (including known selectors)

 ************************************************************************/
/************************************************************************
  selector.h 2 replace /users/mjc/system
  860407 17:02:22 mjc Added KS valence access, altered selector descriptor type

 ************************************************************************/
/************************************************************************
  selector.h 3 replace /users/mjc/translation
  870524 09:41:42 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
