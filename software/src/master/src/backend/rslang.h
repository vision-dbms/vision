/*****  Research System Language Parser Header File  *****/
#ifndef RSLANG_H
#define RSLANG_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

class VString;

/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *RSLANG_Compile (
    M_ASD	*pContainerSpace,
    char const	*source,
    M_CPD	*dictionary,
    char	*pMessageBuffer	= 0,
    unsigned int sMessageBuffer	= 0,
    int		*errorLine	= 0,
    int		*errorCharacter	= 0
);

PublicFnDecl void RSLANG_Decompile (
    VString	&rSourceReturn,
    M_CPD	*pProgram,
    unsigned int*pDecompiledPC = 0,
    unsigned int xPCOffset = UINT_MAX
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rslang.h 1 replace /users/mjc/system
  860324 11:09:16 mjc Add header file for compiler

 ************************************************************************/
/************************************************************************
  rslang.h 2 replace /users/mjc/translation
  870524 09:41:08 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
