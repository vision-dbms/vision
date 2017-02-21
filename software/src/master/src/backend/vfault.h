/*****  Fault Remembering Facility Declarations Header File  *****/
#ifndef FAULT_H
#define FAULT_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

#include "m.h"

#include "verr.h"

class VPrimitiveTask;
class VString;


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/*******************************
 *****  Globals Variables  *****
 *******************************/

PublicVarDecl unsigned int
    FAULT_SessionUnrecordedCount,
    FAULT_SessionErrorCount, 
    FAULT_SessionWarningCount;

PublicFnDecl void FAULT_ForgetAllSessionFaults ();

PublicFnDecl void FAULT_RecordError (ERR_ErrorDescription *errorDescription);

PublicFnDecl void FAULT_RecordWarning (char const *msg);

PublicFnDecl M_CPD *FAULT_AccessSeverity (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessTimestamp (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessErrorCode (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessHandlerErrorCode (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessFacility (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessFileName (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessDescription (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessLineNumber (VPrimitiveTask *pTask);

PublicFnDecl M_CPD *FAULT_AccessMoreInfoAvail (VPrimitiveTask *pTask);

PublicFnDecl void FAULT_DisplayFaults (bool bShowErrors, bool bShowWarnings);

PublicFnDecl void FAULT_DumpFaults (VString &strDump, bool bShowErrors = true, bool bShowWarnings = true);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  fault.h 1 replace /users/jad/system
  880711 17:07:46 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  fault.h 2 replace /users/jck/system
  890808 16:02:13 jck Fixed bug in fault recording

 ************************************************************************/
/************************************************************************
  fault.h 3 replace /users/jck/system
  890809 10:13:39 jck Add macro to maintain compatibility for Apollo version

 ************************************************************************/
/************************************************************************
  fault.h 4 replace /users/jck/system
  890809 15:18:50 jck Redefining FAULT_RecordSelectorNotFound

 ************************************************************************/
