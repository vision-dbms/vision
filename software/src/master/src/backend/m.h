/*****  The Memory Manager Header File  *****/
#ifndef M_H
#define M_H

/**********************************
 **********************************
 *****  Imported Interfaces  ******
 **********************************
 **********************************/

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void M_AccumulateAllocationStatistics (
    double *pAllocationTotal, double *pMappingTotal
);

PublicFnDecl bool M_DisposeOfSessionGarbage ();

PublicFnDecl void M_FlushCachedResources ();

PublicFnDecl unsigned int M_ReclaimSegments ();

PublicFnDecl unsigned int M_ReclaimAllSegments ();

PublicFnDecl void M_ProcessGRMRequests ();

PublicFnDecl char const* M_UpdateStatusDescription (
    PS_UpdateStatus updateStatus, char const *alternateSuccessText
);

PublicFnDecl void __cdecl M_LogError (char const* pFormat, ...);


/**********************************************************
 *****  Representation Type Description Declarations  *****
 **********************************************************/

PublicVarDecl M_RTD M_RTDDB [];

/*****  RTD Array Access  *****/
#define M_RTDPtr(rtype) (M_RTDDB + (int)(rtype))


#endif


/************************************************************************
  m.h 2 replace /users/mjc/system
  860321 17:53:34 mjc Release 1 of new memory architecture

 ************************************************************************/
/************************************************************************
  m.h 3 replace /users/mjc/system
  860408 23:09:49 mjc Added standard CPD allocation capability

 ************************************************************************/
/************************************************************************
  m.h 4 replace /users/mjc/system
  860410 19:17:30 mjc Added standard CPDs, POP testing, and default printer

 ************************************************************************/
/************************************************************************
  m.h 5 replace /users/mjc/system
  860414 12:44:08 mjc Release 'M_ShiftContainerTail', delete 'M_Browser' calls

 ************************************************************************/
/************************************************************************
  m.h 6 replace /users/hjb/system
  860423 00:41:13 hjb added 'M_AttacheCPDPointers' 

 ************************************************************************/
/************************************************************************
  m.h 7 replace /users/hjb/system
  860517 19:00:21 hjb added User Space Functions

 ************************************************************************/
/************************************************************************
  m.h 8 replace /users/jck/system
  860711 16:55:32 jck Release Object Saver

 ************************************************************************/
/************************************************************************
  m.h 9 replace /users/jad/system
  860731 17:47:13 jad added DBUPDATE utilities

 ************************************************************************/
/************************************************************************
  m.h 10 replace /users/mjc/rsystem
  860813 18:23:08 mjc Implemented statistics query/display, simple optimization of CPD allocator

 ************************************************************************/
/************************************************************************
  m.h 11 replace /users/mjc/system
  860817 20:18:00 mjc New, improved CPD allocators

 ************************************************************************/
/************************************************************************
  m.h 12 replace /users/mjc/system
  860819 12:32:04 mjc Added experimental fast preamble pointer access routine

 ************************************************************************/
/************************************************************************
  m.h 13 replace /users/jad/system
  860914 11:15:18 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  m.h 14 replace /users/mjc/system
  861002 18:03:11 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  m.h 15 replace /users/mjc/system
  870215 23:05:08 lcn Release of new memory manager

 ************************************************************************/
/************************************************************************
  m.h 16 replace /users/mjc/system
  870326 19:27:57 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  m.h 17 replace /users/jck/system
  870415 09:54:52 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  m.h 18 replace /users/mjc/translation
  870524 09:39:45 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  m.h 19 replace /users/jck/system
  871007 13:39:51 jck Release implementation of Global Persistent Garbage Collector

 ************************************************************************/
/************************************************************************
  m.h 20 replace /users/mjc/Software/system
  871123 20:10:44 mjc Added experimental CPCC allocation routine and '#M displayMappingInfo' debug method

 ************************************************************************/
