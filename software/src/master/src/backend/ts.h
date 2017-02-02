/*****  The Transient Storage Manager Declarations  *****/
#ifndef TS_H
#define TS_H

/****************************************
 ****************************************
 *****  Definitions And Signatures  *****
 ****************************************
 ****************************************/

#include "m.h"

/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDef M_CPreamble* TS_AllocateContainer (
    RTYPE_Type type, size_t size, unsigned int xOS, unsigned int xCTE
);

PublicFnDef M_CPreamble* TS_AllocateContainer (
    RTYPE_Type type, size_t size, M_POP iPOP
);

PublicFnDecl M_CPreamble* TS_CopyContainer (
    M_CPreamble*		oldPreamblePtr
);

PublicFnDecl M_CPreamble* TS_ReallocateContainer (
    M_CPreamble*		containerAddress,
    size_t			newSize
);

PublicFnDecl void TS_DeallocateContainer (
    M_CPreamble*		containerAddress
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  ts.h 1 replace /users/mjc/system
  870216 00:08:46 lcn Release of new memory manager

 ************************************************************************/
/************************************************************************
  ts.h 2 replace /users/mjc/translation
  870524 09:42:11 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
