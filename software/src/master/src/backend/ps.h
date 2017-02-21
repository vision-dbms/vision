/*****  The Persistent Storage Manager Declarations  *****/
#ifndef PS_H
#define PS_H

/**********************************
 **********************************
 *****  Imported Interfaces  ******
 **********************************
 **********************************/

#include "PS_ASD.h"


/***********************************************
 ***********************************************
 *****  Usage Counters and Trace Switches  *****
 ***********************************************
 ***********************************************/

PublicVarDecl bool		PS_TracingSegmentScans,
				PS_TracingNetworkAccess,
				PS_ValidatingChecksums;

PublicVarDecl unsigned int	PS_SegmentScanCount,
				PS_SegmentReclamationCount,
				PS_SegmentReclamationMinimum,
				PS_SegmentReclamationMaximum,
				PS_PCTPrivatizations;


/**********************************************
 **********************************************
 *****  Global Resource Manager Routines  *****
 **********************************************
 **********************************************/

PublicFnDecl int PS_GRMRequestsArePending ();

/*******************************
 *******************************
 *****  Creation Routines  *****
 *******************************
 *******************************/

PublicFnDecl void PS_CreateNetwork (
    char const*			networkDirectoryFilePath,
    char const*			newSpaceDirectoryPath,
    int				lookingForExistingSpaces
);

/***********************************
 ***********************************
 *****  Module Initialization  *****
 ***********************************
 ***********************************/

PublicFnDecl void PS_Initialize ();

PublicFnDecl void PS_SetMappingLimit (
    int		limit
);

PublicFnDecl void PS_SetAddressThreshold (
    unsigned int	threshold
);

PublicFnDecl void PS_SetNSyncRetries (
    int		retries
);

PublicFnDecl void PS_SetSSyncRetries (
    int		retries
);

PublicFnDecl void PS_SetSOpenRetries (
    int		retries
);

PublicFnDecl void PS_SetSORetryDelay (
    int		delay
);

PublicFnDecl void PS_SetNetOpenTrace (
    int		onOff
);

PublicFnDecl void PS_SetMaxWriteChunk (unsigned int iMaxWriteChunk);

PublicFnDecl void PS_DisableCommits (bool value);
#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  ps.h 1 replace /users/mjc/system
  870215 22:56:29 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  ps.h 2 replace /users/mjc/system
  870326 19:28:11 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  ps.h 3 replace /users/cmm/system
  870519 16:11:58 cmm Release compactor

 ************************************************************************/
/************************************************************************
  ps.h 4 replace /users/mjc/translation
  870524 09:40:59 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  ps.h 5 replace /users/cmm/system
  870821 11:02:47 cmm compactor: added omitlist, minseg formula

 ************************************************************************/
/************************************************************************
  ps.h 6 replace /users/cmm/system
  871006 14:43:30 cmm added stats-only option to compactor, initial MSS back to orig rt. seg of previous SVD, mjc's MSS override

 ************************************************************************/
