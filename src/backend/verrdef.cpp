/*****  Error Code Definition Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName ERRDEF


/*******************************************
 *****  Header and Declarations Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "vfac.h"
#include "vutil.h"

/*****  Shared  *****/
#include "verrdef.h"


/*************************************************
 *****  Error Category and Code Definitions  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *  Error Code Description Typedef
 *---------------------------------------------------------------------------
 */

struct CodeDescriptionType {
    ERRDEF_ErrorCode		code,
				superCode;
    char const			*description;
};

/*---------------------------------------------------------------------------
 * Error code description helper macros
 *---------------------------------------------------------------------------
 */

#define BeginCodeDescriptions PrivateVarDef CodeDescriptionType CodeDescriptionArray [] = {

#define EndCodeDescriptions	};

#define CodeDescription(code, superCode, description)\
{code, superCode, description}

/*---------------------------------------------------------------------------
 * Error Code Definitions
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * The following list describes all error codes that can be generated.
 * Entries in this list define three values in the following order:
 *
 *	1) The symbolic name of the error code.
 *	2) The symbolic name of the super error code for the error code.
 *	3) A string describing the error code.
 *
 * Be particular careful not to reverse the first and second error codes in a
 * definition - the system won't catch it and it could throw the error handler
 * into an infinite loop.
 *---------------------------------------------------------------------------
 */
BeginCodeDescriptions
/*  All Errors  */
    CodeDescription
	(EC__AnError,
	 EC__AnError,
	 "An Error"),

/**  System Faults  **/
    CodeDescription
	(EC__SystemFault,
	 EC__AnError,
	 "A System Fault"),

/***  Unknown Errors  ***/
    CodeDescription
	(EC__AnUnknownError,
	 EC__SystemFault,
	 "An Unknown Error"),

/***  Unimplemented Case  ***/
    CodeDescription
	(EC__UnimplementedCase,
	 EC__SystemFault,
	 "Unimplemented Case or Procedure"),

/***  Error Handler Errors  ***/
    CodeDescription
	(EC__ErrorHandlerError,
	 EC__SystemFault,
	 "An Error Handler Error"),

    CodeDescription
	(EC__InvalidErrorCode,
	 EC__ErrorHandlerError,
	 "Invalid Error Code"),

    CodeDescription
	(EC__HandlerStackEmpty,
	 EC__ErrorHandlerError,
	 "Error Code Handler Stack Empty"),

    CodeDescription
	(EC__CantReturn,
	 EC__ErrorHandlerError,
	 "Can\'t Return To Error Signaller"),

/***  I-Object Errors  ***/
    CodeDescription
	(EC__IObjectError,
	 EC__SystemFault,
	 "An I-Object Manager Error"),

    CodeDescription
	(EC__IObjectMethodNotFound,
	 EC__IObjectError,
	 "I-Type does not define a method for the message sent"),

/***  File Errors  ***/
    CodeDescription
	(EC__FileError,
	 EC__SystemFault,
	 "A File Error"),

    CodeDescription
	(EC__FileStatusAccessError,
	 EC__FileError,
	 "File Status Access Error"),

/***  Memory Errors  ***/
    CodeDescription
	(EC__MemoryError,
	 EC__SystemFault,
	 "A Memory Error"),

    CodeDescription
	(EC__SegmentationFault,
	 EC__MemoryError,
	 "A Segmentation Fault"),

    CodeDescription
	(EC__PrivateMemoryError,
	 EC__MemoryError,
	 "A Private Memory Space Error"),

    CodeDescription
	(EC__MallocError,
	 EC__PrivateMemoryError,
	 "\'malloc\' Error"),

    CodeDescription
	(EC__ReallocError,
	 EC__PrivateMemoryError,
	 "\'realloc\' Error"),

    CodeDescription
	(EC__FreeError,
	 EC__PrivateMemoryError,
	 "\'free\' Error"),

    CodeDescription
	(EC__SharedMemoryError,
	 EC__MemoryError,
	 "A Shared Memory Space Error"),

    CodeDescription
	(EC__FileMappingError,
	 EC__SharedMemoryError,
	 "A File Mapping Error"),

    CodeDescription
	(EC__MError,
	 EC__MemoryError,
	 "A Memory Manager (M) Error"),

    CodeDescription
        (EC__ContainerFramingError,
	 EC__MError,
	 "Container Framing Error"),

    CodeDescription
        (EC__MemoryManagerLimit,
	 EC__MError,
	 "A Memory Manager Limit Has Been Reached"),

/**  Representation Type Errors  **/
    CodeDescription
	(EC__RTypeFault,
	 EC__SystemFault,
	 "Representation Type Fault"),

    CodeDescription
	(EC__UnknownRType,
	 EC__RTypeFault,
	 "Unknown Representation Type Code"),

    CodeDescription
	(EC__UnknownRTypeHandlerOp,
	 EC__RTypeFault,
	 "Unknown Representation Type Handler Operation Requested"),

    CodeDescription
	(EC__RTypeHandlerFault,
	 EC__RTypeFault,
	 "Representation Type Handler Fault"),

/**  Virtual Machine Errors  **/
    CodeDescription
	(EC__VirtualMachineError,
	 EC__SystemFault,
	 "A Virtual Machine Error"),

/**  Internal Inconsistencies  **/
    CodeDescription
	(EC__InternalInconsistency,
	 EC__SystemFault,
	 "An Internal Inconsistency"),

/**  External Interface Errors  **/
    CodeDescription
	(EC__ExternalInterfaceError,
	 EC__SystemFault,
	 "An External Interface Error"),

/**  Usage Errors  **/
    CodeDescription
	(EC__UsageError,
	 EC__AnError,
	 "A Usage Error"),

/**  Unknown UVector RType  **/
    CodeDescription
	(EC__UnknownUVectorRType, 
	 EC__AnError, 
	 "Unknown UVector RType"), 

/**  Interrupts  **/
    CodeDescription
	(EC__AnInterrupt,
	 EC__AnError,
	 "An Interrupt"),

    CodeDescription
	(EC__HangupSignal,
	 EC__AnInterrupt,
	 "Hangup Signal"),

    CodeDescription
	(EC__InterruptSignal,
	 EC__AnInterrupt,
	 "User Interrupt Signal"),

    CodeDescription
	(EC__TerminateSignal,
	 EC__AnInterrupt,
	 "Software Terminate Signal"),

    CodeDescription
	(EC__QuitSignal,
	 EC__AnInterrupt,
	 "Quit Signal"),

    CodeDescription
	(EC__FPESignal,
	 EC__AnInterrupt,
	 "Floating Point Exception Signal"),

    CodeDescription
	(EC__UserSignal1,
	 EC__AnInterrupt,
	 "User Signal 1"),

    CodeDescription
	(EC__UserSignal2,
	 EC__AnInterrupt,
	 "User Signal 2"),

    CodeDescription
	(EC__AlarmSignal,
	 EC__AnInterrupt,
	 "Alarm Signal"),

    CodeDescription
	(EC__VTAlarmSignal,
	 EC__AnInterrupt,
	 "Virtual Timer Alarm Signal"),

    CodeDescription
	(EC__PTAlarmSignal,
	 EC__AnInterrupt,
	 "Profiling Timer Alarm Signal"),

/**  The Largest Possible Error  **/
    CodeDescription
	(EC__MaxError,
	 EC__AnError,
	 "The Largest Possible Error Code")
EndCodeDescriptions


/*****  Error Code Index Arrays  *****/
/*---------------------------------------------------------------------------
 * The error code description array defined above is not constrained
 * to be in any particular order.  To enable efficient access, the following
 * array indexed by (int)code is initialized as part of the the startup
 * processing required by this facility.
 *---------------------------------------------------------------------------
 */

PrivateVarDef CodeDescriptionType *CodeDescriptionPtrArray [(int)EC__MaxError + 1];


/***********************************************
 *****  Error Code Interrogation Routines  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return a character string description for an error code.
 *
 *  Argument:
 *	code			- the error code whose description is
 *				  desired.
 *
 *  Returns:
 *	The address of a read only (please) text string describing the
 *	code.
 *
 *****/
PublicFnDef char const *ERRDEF_CodeDescription (
    ERRDEF_ErrorCode		code
)
{
    CodeDescriptionType *codeDescription;

    if ((int)code < 0 || (int)code > (int)EC__MaxError ||
	    IsNil (codeDescription = CodeDescriptionPtrArray[(int)code]))
        return UTIL_FormatMessage ("Unknown Error Code#%d", (int)code);
    else
	return codeDescription->description;
}

/*---------------------------------------------------------------------------
 *****  Routine to return the 'super' code for an error code.
 *
 *  Argument:
 *	code			- the error code whose superior is desired.
 *
 *  Returns:
 *	The super code for 'code'
 *
 *****/
PublicFnDef ERRDEF_ErrorCode ERRDEF_SuperCode (
    ERRDEF_ErrorCode		code
)
{
    CodeDescriptionType *codeDescription;

    return
	(int)code < 0 || (int)code > (int)EC__MaxError ||
	    IsNil (codeDescription = CodeDescriptionPtrArray[(int)code])
	? EC__AnUnknownError
	: codeDescription->superCode;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    int i, n;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (ERRDEF);
    FAC_FDFCase_FacilityDescription ("Error Code Definitions");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    case FAC_DoStartupInitialization:
/*****  Initialize the error code description pointer array  *****/
	for (i = 0; i <= (int)EC__MaxError; i++)
	    CodeDescriptionPtrArray[i] = NilOf (CodeDescriptionType *);

	for (i = 0,
	    n = sizeof CodeDescriptionArray / sizeof (CodeDescriptionType);
	    i < n;
	    i++)
	    CodeDescriptionPtrArray[(int)CodeDescriptionArray[i].code] =
		CodeDescriptionArray + i;

	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}
/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  errdef.c 1 replace /users/mjc/system
  860226 16:18:39 mjc create

 ************************************************************************/
/************************************************************************
  errdef.c 2 replace /users/mjc/translation
  870524 09:38:03 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
