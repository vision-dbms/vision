/*****  Fault Remembering Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName FAULT

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "venvir.h"
#include "verr.h"
#include "verrdef.h"
#include "vfac.h"
#include "viobj.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTcharuv.h"
#include "RTintuv.h"
#include "RTptoken.h"

#include "VPrimitiveTask.h"

#include "V_VTime.h"
#include "V_VString.h"

/*****  Shared  *****/
#include "vfault.h"


/***********************
 *****  Utilities  *****
 ***********************/

/*---------------------------------------------------------------------------
 *****  Format a second resolution time stamp.
 *
 *  Arguments:
 *	tsp			- the address of the time stamp to format.
 *
 *  Returns:
 *	The address of a reused buffer containing the formatted representation
 *	of the "seconds" field of the time stamp.
 *
 *****/
PrivateFnDef char const *Ctimeval (struct time const *tsp) {
    V::VTime iTime (*tsp);
    struct tm iTimeStruct;
    struct tm *tm = iTime.localtime (iTimeStruct);

/****  using 'mm/dd/yy:hh:mm:ss' format  ****/
    static char buffer[80];
    sprintf (
	buffer,
	"%02d/%02d/%02d:%02d:%02d:%02d",
	tm->tm_mon + 1,
	tm->tm_mday,
	tm->tm_year + 1900,
	tm->tm_hour,
	tm->tm_min,
	tm->tm_sec
    );

    return buffer;
}


/*****************************************
 *****  Error Description Structure  *****
 *****************************************/

/*****  FD Types  *****/
#define FAULT_FD_TypeErrorDesc	0
#define FAULT_FD_TypeStringMsg	1

/*---------------------------------------------------------------------------
 * A Fault Error Description contains basic information about errors and
 * warnings which occurred during the session.  There are two types of Fault
 * Error Descriptions -- those that contain a full ERR_ErrorDescription and
 * those that only contain a string describing the problem.
 *
 * Fault Error Description Fields:
 *
 *	m_xSeverity		- the severity of the problem:
 *				  ERR_SeverityCode_...
 *	m_xType			- the type of Fault Description, 
 *				  either FAULT_FD_TypeErrorDesc or
 *                                FAULT_FD_TypeStringMsg
 *	content			- a union of representation formats for a
 *				  FD.  The following describes those
 *				  formats:
 *	.errorDesc		... a standard Error Description.  
 *	.stringMsg		... a pointer to a string containing the
 *                                  message describing the problem
 *	m_iTimestamp		- the time that this message occurred
 *	m_pSuccessor		- a pointer to the next FD or Nil.
 *	
 *---------------------------------------------------------------------------
 */
#define FAULT_FileNameSize		15
#define FAULT_DescriptionSize		79
#define FAULT_ErrorDescriptionSize	(sizeof (FAULT_ErrorDescription) - 1)

struct FAULT_ErrorDescription {
    ERRDEF_ErrorCode
	errorCode,
	handlerErrorCode;/* The error code to which the handler is attached */
    FAC_Facility
	facility;
    int
	lineNumber;
    char
	fileName [FAULT_FileNameSize + 1],
	description [FAULT_DescriptionSize + 1];
};

#define FAULT_ED_ErrorCode(ed)		((ed).errorCode)
#define FAULT_ED_HandlerErrorCode(ed)	((ed).handlerErrorCode)
#define FAULT_ED_Facility(ed)		((ed).facility)
#define FAULT_ED_LineNumber(ed)		((ed).lineNumber)
#define FAULT_ED_FileName(ed)		((ed).fileName)
#define FAULT_ED_Description(ed)	((ed).description)


/*************************************
 *****  Fault Description Class  *****
 *************************************/

/************
 *  Macros  *
 ************/

#define FAULT_FD_Severity(fd)		((fd)->m_xSeverity)
#define FAULT_FD_Type(fd)		((fd)->m_xType)

#define FAULT_FD_IsErrorDesc(fd) (FAULT_FD_Type (fd) == FAULT_FD_TypeErrorDesc)
#define FAULT_FD_IsStringMsg(fd) (FAULT_FD_Type (fd) == FAULT_FD_TypeStringMsg)

#define FAULT_FD_Content(fd)		((fd)->content)
#define FAULT_FD_ErrorDescription(fd)	(FAULT_FD_Content (fd).errorDesc)

#define FAULT_FD_ErrorCode(fd)\
    FAULT_ED_ErrorCode (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_HandlerErrorCode(fd)\
    FAULT_ED_HandlerErrorCode (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_Facility(fd)\
    FAULT_ED_Facility (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_Description(fd)\
    FAULT_ED_Description (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_FileName(fd)\
    FAULT_ED_FileName (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_LineNumber(fd)\
    FAULT_ED_LineNumber (FAULT_FD_ErrorDescription (fd))

#define FAULT_FD_StringMsg(fd)		(FAULT_FD_Content (fd).stringMsg)
#define FAULT_FD_Timestamp(fd)		((fd)->m_iTimestamp)


/***********
 *  Types  *
 ***********/

class FAULT_FaultDescription : public VTransient {
//  Construction
public:
    FAULT_FaultDescription (ERR_ErrorDescription *errDesc);
    FAULT_FaultDescription (char const* stringMsg, int xSeverity);

//  Destruction

//  Access
public:
    FAULT_FaultDescription *Successor () const {
	return m_pSuccessor;
    }

//  Query
public:
    bool IsError () const {
	return m_xType == FAULT_FD_TypeErrorDesc;
    }
    bool IsMessage () const {
	return m_xType == FAULT_FD_TypeStringMsg;
    }

//  Field Accessors 
public:
    rtINTUV_ElementType LineNumber () const {
	return IsError () ? FAULT_FD_LineNumber (this) : 0;
    }
    rtINTUV_ElementType ErrorInfoAvailable () const {
	return IsError ();
    }

    char const *SeverityString () const {
	return ERR_SeverityCodeString (m_xSeverity);
    }

    char const *TimestampString () const {
	return Ctimeval (&m_iTimestamp);
    }

    char const *ErrorCodeDescription () const {
	return IsError () ? ERRDEF_CodeDescription (FAULT_FD_ErrorCode (this)) : "";
    }

    char const *HandlerErrorCodeDescription () const {
	return IsError () ? ERRDEF_CodeDescription (FAULT_FD_HandlerErrorCode (this)) : "";
    }

    char const *FacilityNameString () const {
	return IsError () ? FAC_FacilityDescription (FAULT_FD_Facility (this)) : "";
    }

    char const *FileName () const {
	return IsError () ? FAULT_FD_FileName (this) : "";
    }

    char const *DescriptionString () const {
	return IsError () ? FAULT_FD_Description (this) : FAULT_FD_StringMsg (this);
    }

//  State
public:
    int const			m_xSeverity,
				m_xType;
    union content_t {
	FAULT_ErrorDescription	    errorDesc;
	char			    stringMsg [FAULT_ErrorDescriptionSize + 1];
    }				content;
    struct time			m_iTimestamp;
    FAULT_FaultDescription	*m_pSuccessor;
};

typedef rtINTUV_ElementType (FAULT_FaultDescription::*FAULT_IntegerAccessor)() const;
typedef char const *(FAULT_FaultDescription::*FAULT_StringAccessor)() const;


/*************************************************
 *****  Global Definitions and Declarations  *****
 *************************************************/

PublicVarDef unsigned int
    FAULT_SessionUnrecordedCount	= 0,
    FAULT_SessionErrorCount		= 0,
    FAULT_SessionWarningCount		= 0;
PrivateVarDef struct time const
    NilTimestamp = {0, 0};
PrivateVarDef FAULT_FaultDescription
    *FirstSessionFault   = NilOf (FAULT_FaultDescription *),
    *CurrentSessionFault = NilOf (FAULT_FaultDescription *);
PrivateVarDef bool
    fFaultDescriptionAllocInProgress	= false;


/*********************************************
 *****  'FaultDescription' Construction  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to allocate and initialize a new FaultDescription
 *****  from an ERR_ErrorDescription.
 *
 *  Arguments:
 *	errDesc		- the ErrorDescription to use to initialize
 *			  'faultDesc'.
 *
 *  Returns:
 *	An initialized fault description.
 *
 *****/
FAULT_FaultDescription::FAULT_FaultDescription (ERR_ErrorDescription *errDesc)
: m_xType	(FAULT_FD_TypeErrorDesc)
, m_xSeverity	(ERR_ED_Severity (errDesc))
, m_pSuccessor	(0)
{
    FAULT_FD_ErrorCode		(this) = ERR_ED_ErrorCode(errDesc);
    FAULT_FD_HandlerErrorCode	(this) = ERR_ED_HandlerErrorCode(errDesc);
    FAULT_FD_Facility		(this) = ERR_ED_Facility(errDesc);
    FAULT_FD_LineNumber		(this) = ERR_ED_LineNumber(errDesc);

    strncpy (
	FAULT_FD_Description (this), ERR_ED_Description (errDesc), FAULT_DescriptionSize
    );
    FAULT_FD_Description (this)[FAULT_DescriptionSize] = '\0';

    strncpy (
	FAULT_FD_FileName (this), ERR_ED_FileName (errDesc), FAULT_FileNameSize
    );
    FAULT_FD_FileName (this)[FAULT_FileNameSize] = '\0';

    FAULT_FD_Timestamp		(this) = NilTimestamp;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to allocate and initialize a new Fault Description
 *****  given the a string message and the severity.
 *
 *  Arguments:
 *	stringMsg	- a string pointer to the fault message.
 *	xSeverity	- the error severity code for the fault.
 *
 *
 *  Returns:
 *	An initialized fault description.
 *
 *****/
FAULT_FaultDescription::FAULT_FaultDescription (char const* stringMsg, int xSeverity)
: m_xType	(FAULT_FD_TypeStringMsg)
, m_xSeverity	(xSeverity)
, m_iTimestamp	(NilTimestamp)
, m_pSuccessor	(0)
{
    strncpy (FAULT_FD_StringMsg (this), stringMsg, FAULT_ErrorDescriptionSize);
    FAULT_FD_StringMsg (this) [FAULT_ErrorDescriptionSize] = '\0';
}


/*---------------------------------------------------------------------------
 *****  Routine to discard all Fault Descriptions contained in the
 *****  linked list pointed to by CurrentSessionFault.
 *
 *  Argument:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *****/
PublicFnDef void FAULT_ForgetAllSessionFaults () {
    while (FirstSessionFault) {
	FAULT_FaultDescription *pCurrentFault = FirstSessionFault;
	FirstSessionFault = pCurrentFault->Successor ();

	delete pCurrentFault;
    }
    CurrentSessionFault = NilOf (FAULT_FaultDescription *);

    FAULT_SessionUnrecordedCount    =
    FAULT_SessionErrorCount	    =
    FAULT_SessionWarningCount	    = 0;
}


/***************************************
 *  Public Functions to Record Faults  *
 ***************************************/

/*---------------------------------------------------------------------------
 ***** Function to record a fault from an Error Description.
 *
 *  Arguments:
 *	errorDescription 	- a pointer to the Error Description for the
 *				  error to be recorded.
 *
 *  Returns:
 *	NOTHING
 *****/
PublicFnDef void FAULT_RecordError (ERR_ErrorDescription *errorDescription) {
    FAULT_FaultDescription *previousFaults = CurrentSessionFault;

/*****  Increment the appropriate counter ...  *****/
    if (ERR_ED_Severity (errorDescription) == ERR_SeverityCode_Error)
	FAULT_SessionErrorCount++;
    else if (ERR_ED_Severity (errorDescription) == ERR_SeverityCode_Warning)
	FAULT_SessionWarningCount++;

/*****  Log the fault ...  *****/
    if (ERR_SeverityCode_Error	== ERR_ED_Severity  (errorDescription) &&
	EC__HangupSignal	!= ERR_ED_ErrorCode (errorDescription) &&
	EC__InterruptSignal	!= ERR_ED_ErrorCode (errorDescription) &&
	EC__QuitSignal		!= ERR_ED_ErrorCode (errorDescription) &&
	EC__AlarmSignal		!= ERR_ED_ErrorCode (errorDescription) &&
	EC__TerminateSignal	!= ERR_ED_ErrorCode (errorDescription)
    ) ERR_ReportError (M_LogError, "Log Entry", errorDescription, FAULT_SessionErrorCount);

/*****  Add a FaultDescription for this error ...  *****/
    if (false == fFaultDescriptionAllocInProgress) {
	// If the error is a memory allocation error, then we could be in a nested
	// allocation error. Only attempt to allocate a fault description once.
	fFaultDescriptionAllocInProgress = true;
	CurrentSessionFault = new FAULT_FaultDescription (errorDescription);
	fFaultDescriptionAllocInProgress = false; // If we got here, it worked!
	gettime (&FAULT_FD_Timestamp (CurrentSessionFault));
	if (previousFaults)
	    previousFaults->m_pSuccessor = CurrentSessionFault;
	else  
	    FirstSessionFault = CurrentSessionFault;
    }

}


/*---------------------------------------------------------------------------
 *****  Function to record a fault given the string message.
 *
 *  Arguments:
 *	msg	- a string containing the fault message.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void FAULT_RecordWarning (char const *msg) {
    FAULT_FaultDescription *previousFaults = CurrentSessionFault;

    CurrentSessionFault = new FAULT_FaultDescription (msg, ERR_SeverityCode_Warning);
    gettime (&FAULT_FD_Timestamp (CurrentSessionFault));
    if (previousFaults)
	previousFaults->m_pSuccessor = CurrentSessionFault;
    else
	FirstSessionFault = CurrentSessionFault;

    FAULT_SessionWarningCount++;
}


/*************************************
 *  Session Faults Access Utilities  *
 *************************************/

PrivateFnDef M_CPD *IntegerField (VPrimitiveTask *pTask, FAULT_IntegerAccessor field) {
/*****  Create the uvector ...  *****/
    rtPTOKEN_Handle::Reference ptoken (
	pTask->NewCodPToken (
	    FAULT_SessionErrorCount + FAULT_SessionWarningCount - FAULT_SessionUnrecordedCount
	)
    );
    M_CPD *result = pTask->NewIntUV (ptoken);

/*****  Fill the uvector ...  *****/
    rtINTUV_ElementType *ptr = rtINTUV_CPD_Array (result);
    for (
	FAULT_FaultDescription *cfd = FirstSessionFault;
	cfd;
	cfd = cfd->Successor ()
    ) {
	*ptr++ = (cfd->*field)();
    }

    return result;
}

PrivateFnDef M_CPD *StringField (VPrimitiveTask *pTask, FAULT_StringAccessor field) {
/*****  Count the number of characters ... *****/
    unsigned int count = 0;
    FAULT_FaultDescription *cfd;
    for (cfd = FirstSessionFault; cfd; cfd = cfd->Successor ()) {
	count += strlen ((cfd->*field)()) + 1;
    }

/*****  Create the uvector ...  *****/
    rtPTOKEN_Handle::Reference ptoken (pTask->NewCodPToken (count));
    M_CPD *result = pTask->NewCharUV (ptoken);

/*****  Fill the uvector ...  *****/
    rtCHARUV_ElementType *ptr = rtCHARUV_CPD_Array (result);
    for (cfd = FirstSessionFault; cfd; cfd = cfd->Successor ()) {
	char const *pValue = ((cfd->*field)());
	strcpy (ptr, pValue);
	ptr += strlen (pValue) + 1;
    }

    return result;
}


/*---------------------------------------------------------------------------
 *  The following functions are used to access the fault history.
 *  Each one returns a uvector containing the value for the requested
 *  field for every system fault.  For example, if 5 faults have occurred,
 *  calling FAULT_AccessSeverity will return a character uvector containing
 *  5 strings.  The first string correspounds to the the severity of the
 *  first fault, the second string corresponds to the severity of the second
 *  fault, etc.
 *---------------------------------------------------------------------------
 */

PublicFnDef M_CPD *FAULT_AccessSeverity (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::SeverityString);
}

PublicFnDef M_CPD *FAULT_AccessTimestamp (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::TimestampString);
}

PublicFnDef M_CPD *FAULT_AccessErrorCode (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::ErrorCodeDescription);
}

PublicFnDef M_CPD *FAULT_AccessHandlerErrorCode (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::HandlerErrorCodeDescription);
}

PublicFnDef M_CPD *FAULT_AccessFacility (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::FacilityNameString);
}

PublicFnDef M_CPD *FAULT_AccessFileName (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::FileName);
}

PublicFnDef M_CPD *FAULT_AccessDescription (VPrimitiveTask *pTask) {
    return StringField (pTask, &FAULT_FaultDescription::DescriptionString);
}

PublicFnDef M_CPD *FAULT_AccessLineNumber (VPrimitiveTask *pTask) {
    return IntegerField (pTask, &FAULT_FaultDescription::LineNumber);
}

PublicFnDef M_CPD *FAULT_AccessMoreInfoAvail (VPrimitiveTask *pTask) {
    return IntegerField (pTask, &FAULT_FaultDescription::ErrorInfoAvailable);
}


/*****************************************
 *  Utilities to Display Session Faults  *
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Function to display the faults encountered in this session.
 *
 *  Arguments:
 *	bShowErrors	- if true all past errors will be displayed.
 *	bShowWarnings	- if true all past warnings will be displayed.
 *
 *  Note:
 *  	Writes to stdout.
 *****/
PublicFnDef void FAULT_DisplayFaults (bool bShowErrors, bool bShowWarnings) {
    IO_printf ("\n%u Faults not recorded during this session\n",
	FAULT_SessionUnrecordedCount);
    IO_printf ("\n%u Errors occurred during this session\n",
	FAULT_SessionErrorCount);
    IO_printf ("%u Warnings occurred during this session\n\n",
	FAULT_SessionWarningCount);

    for (FAULT_FaultDescription *faultDesc = FirstSessionFault;
	 IsntNil (faultDesc);
	 faultDesc = faultDesc->Successor ()
    )
    {
	if ((FAULT_FD_Severity (faultDesc) == ERR_SeverityCode_Warning && bShowWarnings) ||
	    (FAULT_FD_Severity (faultDesc) == ERR_SeverityCode_Error && bShowErrors))
	{
	    char banner[130];
	    sprintf (banner, "occurred at %s", Ctimeval (&FAULT_FD_Timestamp (faultDesc)));
	    if (FAULT_FD_IsErrorDesc (faultDesc)) {
		ERR_ErrorDescription	ed;

		ERR_ED_Severity		(&ed) = FAULT_FD_Severity(faultDesc);
		ERR_ED_ErrorCode	(&ed) = FAULT_FD_ErrorCode(faultDesc);
		ERR_ED_HandlerErrorCode	(&ed) = FAULT_FD_HandlerErrorCode(faultDesc);
		ERR_ED_Facility		(&ed) = FAULT_FD_Facility(faultDesc);
		ERR_ED_LineNumber	(&ed) = FAULT_FD_LineNumber(faultDesc);
		ERR_ED_FileName		(&ed) = FAULT_FD_FileName(faultDesc);
		ERR_ED_Description	(&ed) = FAULT_FD_Description(faultDesc);

		ERR_ReportErrorOnStdout (banner, &ed);
	    }
	    else if (FAULT_FD_IsStringMsg (faultDesc)) {
		IO_printf (">>>  %s ", ERR_SeverityCodeString (FAULT_FD_Severity (faultDesc)));
		IO_printf ("%s  <<<\n", banner);
		IO_printf ("\t%s\n", FAULT_FD_StringMsg (faultDesc));
	    }
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Function to dump the faults encountered in this session.
 *
 *  Arguments:
 *	bShowErrors	- if true all past errors will be dumped.
 *	bShowWarnings	- if true all past warnings will be dumped.
 *
 *  Note:
 *  	Saved to a string.
 *****/
PublicFnDef void FAULT_DumpFaults (VString &strDump, bool bShowErrors, bool bShowWarnings) {
    strDump.printf ("\n%u Faults not recorded during this session\n",
	FAULT_SessionUnrecordedCount);
    strDump.printf ("\n%u Errors occurred during this session\n",
	FAULT_SessionErrorCount);
    strDump.printf ("%u Warnings occurred during this session\n\n",
	FAULT_SessionWarningCount);

    for (FAULT_FaultDescription *faultDesc = FirstSessionFault;
	 IsntNil (faultDesc);
	 faultDesc = faultDesc->Successor ()
    )
    {
	if ((FAULT_FD_Severity (faultDesc) == ERR_SeverityCode_Warning && bShowWarnings) ||
	    (FAULT_FD_Severity (faultDesc) == ERR_SeverityCode_Error && bShowErrors))
	{
	    char banner[130];
	    strDump.printf (banner, "occurred at %s", Ctimeval (&FAULT_FD_Timestamp (faultDesc)));
	    if (FAULT_FD_IsErrorDesc (faultDesc)) {
		ERR_ErrorDescription	ed;

		ERR_ED_Severity		(&ed) = FAULT_FD_Severity(faultDesc);
		ERR_ED_ErrorCode	(&ed) = FAULT_FD_ErrorCode(faultDesc);
		ERR_ED_HandlerErrorCode	(&ed) = FAULT_FD_HandlerErrorCode(faultDesc);
		ERR_ED_Facility		(&ed) = FAULT_FD_Facility(faultDesc);
		ERR_ED_LineNumber	(&ed) = FAULT_FD_LineNumber(faultDesc);
		ERR_ED_FileName		(&ed) = FAULT_FD_FileName(faultDesc);
		ERR_ED_Description	(&ed) = FAULT_FD_Description(faultDesc);

		ERR_DumpErrorToString (strDump, banner, &ed);
	    }
	    else if (FAULT_FD_IsStringMsg (faultDesc)) {
		strDump.printf (">>>  %s ", ERR_SeverityCodeString (FAULT_FD_Severity (faultDesc)));
		strDump.printf ("%s  <<<\n", banner);
		strDump.printf ("\t%s\n", FAULT_FD_StringMsg (faultDesc));
	    }
	}
    }
}

/***************************************
 *****  Facility I-Object Methods  *****
 ***************************************/

IOBJ_DefineUnaryMethod (DisplayErrors) {
    FAULT_DisplayFaults (true, false);
    return self;
}

IOBJ_DefineUnaryMethod (DisplayWarnings) {
    FAULT_DisplayFaults (false, true);
    return self;
}

IOBJ_DefineUnaryMethod (DisplayFaults) {
    FAULT_DisplayFaults (true, true);
    return self;
}

IOBJ_DefineUnaryMethod (ForgetAllFaults) {
    FAULT_ForgetAllSessionFaults ();
    return self;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("displayErrors"	, DisplayErrors)
	IOBJ_MDE ("displayWarnings"	, DisplayWarnings)
	IOBJ_MDE ("displayFaults"	, DisplayFaults)
	IOBJ_MDE ("forgetAllFaults"	, ForgetAllFaults)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (FAULT);
    FAC_FDFCase_FacilityDescription ("Fault Remembering Facility");
    case FAC_ReturnFacilityMD:
        FAC_MDResultField = methodDictionary;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  fault.c 1 replace /users/jad/system
  880711 17:06:54 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  fault.c 2 replace /users/m2/backend
  890503 15:07:05 m2 Fix lint complaint

 ************************************************************************/
/************************************************************************
  fault.c 3 replace /users/jck/system
  890808 15:49:04 jck Fixed bug in fault recording

 ************************************************************************/
