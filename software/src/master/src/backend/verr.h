/*****  Error Handler Declarations Header File  *****/
#ifndef ERR_H
#define ERR_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

#include "verrdef.h"
#include "vfac.h"


/**************************
 **************************
 *****  Declaration  ******
 **************************
 **************************/

class VString;

/*********************
 *********************
 *****  Helpers  *****
 *********************
 *********************/

/******************************
 *****  Declare Facility  *****
 ******************************/

#ifdef FacilityName
FAC_DeclareFacility (FacilityName);
#define ERR_ThisFacility FacilityName
#else
#define ERR_ThisFacility NilOf (FAC_Facility)
#endif

/****************************
 *****  Severity Codes  *****
 ****************************/

#define ERR_SeverityCode_Warning    0
#define ERR_SeverityCode_Error	    1

#define ERR_SeverityCodeString(severityCode) (\
    (severityCode == ERR_SeverityCode_Error) ? "Error" : "Warning"\
)

/**************************************************
 *****  'ERR_BasicSignalError' Helper Macros  *****
 **************************************************/

#define ERR_SignalError(errorCode, description, localInfoIObject) ERR_BasicSignalError (\
    (errorCode)		, ERR_ThisFacility	, __LINE__,\
    (description)	, false			, ERR_SeverityCode_Error\
)

#define ERR_SignalException(errorCode, description) ERR_BasicSignalError (\
    (errorCode)		, ERR_ThisFacility	, __LINE__,\
    (description)	, false			, ERR_SeverityCode_Error\
)

#define ERR_SignalFault(errorCode, description) ERR_BasicSignalError (\
    (errorCode)		, ERR_ThisFacility	, __LINE__,\
    (description)	, true			, ERR_SeverityCode_Error\
)

#define ERR_SignalWarning(errorCode, description) ERR_BasicSignalError (\
    (errorCode)		, ERR_ThisFacility	, __LINE__,\
    (description)	, false			, ERR_SeverityCode_Warning\
)

#define ERR_SignalWarningNoReturn(errorCode, description) ERR_BasicSignalError (\
    (errorCode)		, ERR_ThisFacility	, __LINE__,\
    (description)	, true			, ERR_SeverityCode_Warning\
)

/*********************************************
 *****  Error Handler Error Description  *****
 *********************************************/

struct ERR_ErrorDescription {
    int			severity;
    ERRDEF_ErrorCode	errorCode,
			handlerErrorCode;
    FAC_Facility	facility;
    char const*		fileName;
    char const*		description;
    int			lineNumber,
			noReturn;
};

/*****  Access Macros  *****/
/*---------------------------------------------------------------------------
 * The following macros expect to be given the address of an error description
 * (the usual situation inside an error handler).
 *---------------------------------------------------------------------------
 */

#define ERR_ED_Severity(ed)		(ed)->severity
#define ERR_ED_ErrorCode(ed)		(ed)->errorCode
#define ERR_ED_HandlerErrorCode(ed)	(ed)->handlerErrorCode
#define ERR_ED_Facility(ed)		(ed)->facility
#define ERR_ED_Description(ed)		(ed)->description
#define ERR_ED_FileName(ed)		(ed)->fileName
#define ERR_ED_LineNumber(ed)		(ed)->lineNumber
#define ERR_ED_NoReturn(ed)		(ed)->noReturn

/***************************
 *****  Error Handler  *****
 ***************************/

typedef int (*ERR_ErrorHandler) (ERR_ErrorDescription *errorDescription);


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void ERR_ReportError (
    void			(__cdecl *writer)(char const*, ...),
    char const*			banner,
    ERR_ErrorDescription	*errorDescription,
    unsigned int		counter = 0
);

PublicFnDecl void ERR_ReportErrorOnStderr (
    char const*			banner,
    ERR_ErrorDescription	*errorDescription
);

PublicFnDecl void ERR_ReportErrorOnStdout (
    char const*			banner,
    ERR_ErrorDescription	*errorDescription
);

PublicFnDecl void ERR_DumpErrorToString (
    VString                     &strDump, 
    char const*                 banner, 
    ERR_ErrorDescription        *errorDescription, 
    unsigned int                counter = 0
);

PublicFnDecl int ERR_BasicSignalError (
    ERRDEF_ErrorCode		errorCode,
    FAC_Facility		facility,
    int				lineNumber,
    char const*			description,
    int				noReturn, 
    int				severity
);

PublicFnDecl void __cdecl ERR_EstablishErrorFrame (
    void (*actionFn) (va_list), ...
);

PublicFnDecl void ERR_SetErrorHandler (
    ERRDEF_ErrorCode		errorCode,
    ERR_ErrorHandler		handlerFn
);

PublicFnDecl void ERR_PushErrorHandler (
    ERRDEF_ErrorCode		errorCode,
    ERR_ErrorHandler		handlerFn
);

PublicFnDecl void ERR_PopErrorHandler (
    ERRDEF_ErrorCode		errorCode
);

PublicFnDecl void ERR_ClearErrorHandler (
    ERRDEF_ErrorCode		errorCode
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  err.h 1 replace /users/mjc/system
  860226 16:18:15 mjc create

 ************************************************************************/
/************************************************************************
  err.h 2 replace /users/mjc/translation
  870524 09:37:59 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  err.h 3 replace /users/jad/system
  880711 16:11:14 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
