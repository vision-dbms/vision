/*****  Error Code Definition Facility Exported Declarations  *****/
#ifndef ERRDEF_Interface
#define ERRDEF_Interface

#ifndef Vk_Interface
#include "Vk.h"
#endif

/*****  Error Code Declarations  *****/
#include "verrdef.d"

/*****  Function Declarations  *****/
PublicFnDecl char const *ERRDEF_CodeDescription (
    ERRDEF_ErrorCode		code
);

PublicFnDecl ERRDEF_ErrorCode ERRDEF_SuperCode (
    ERRDEF_ErrorCode		code
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  errdef.h 1 replace /users/mjc/system
  860226 16:18:43 mjc create

 ************************************************************************/
/************************************************************************
  errdef.h 2 replace /users/mjc/translation
  870524 09:38:11 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
