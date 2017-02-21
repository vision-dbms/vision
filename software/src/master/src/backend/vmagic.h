/*****  Magic Word Facility Header File  *****/
#ifndef MAGIC_Interface
#define MAGIC_Interface

/*****  String <->  Magic Word Index Conversion Routines  *****/

PublicFnDecl int MAGIC_StringToMWI (char const* pString);

PublicFnDecl char const* MAGIC_MWIToString (unsigned int mwi);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  magic.h 1 replace /users/jck/system
  860411 14:39:53 jck Translate Magic Words from ascii to code
and vice versa.

 ************************************************************************/
/************************************************************************
  magic.h 2 replace /users/mjc/translation
  870524 09:39:54 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
