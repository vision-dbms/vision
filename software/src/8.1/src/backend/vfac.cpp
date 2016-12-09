/*****  Facility Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName FAC

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

/*****  Kernel  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "viobj.h"
#include "vstdio.h"
#include "vutil.h"

#include "V_VArgList.h"

/*****  Self  *****/
#include "vfac.h"


/****************************************
 ****************************************
 *****  Name/Description Utilities  *****
 ****************************************
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return a character string description for a facility
 *****  completion code.
 *
 *  Argument:
 *	completionCode		- the completion code whose description is
 *				  desired.
 *
 *  Returns:
 *	A pointer to a read only (please) character string description of the
 *	completion code.
 *
 *****/
PublicFnDef char const *FAC_CompletionCodeName (FAC_CompletionCode completionCode) {
    switch (completionCode) {
    case FAC_RequestSucceeded:
        return "Request Succeeded";
    case FAC_RequestFailed:
        return "Request Failed";
    case FAC_RequestNotImplemented:
        return "Request Not Implemented";
    case FAC_RequestAlreadyDone:
        return "Request Already Done";
    case FAC_RequestNotMade:
        return "Request Not Made";
    default:
	return UTIL_FormatMessage (
	    "Unknown Completion Code %d", (int)completionCode
	);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to return a facility id as a string.
 *
 *  Argument:
 *	facility		- the facility whose name is desired.
 *
 *  Returns:
 *	A pointer to a read only (please) string identifying the facility.
 *
 *****/
PublicFnDef char const* FAC_FacilityIdAsString (FAC_Facility facility) {
    if (facility) {
	FAC_Request request;

	request.m_requestType	 = FAC_ReturnFacilityIdAsString;
	request.m_completionCode = FAC_RequestNotImplemented;
	(*facility)(&request);
	if (request.m_completionCode == FAC_RequestSucceeded)
	    return request.m_result.m_string;
    }

    return UTIL_FormatMessage ("Facility#%u", facility);
}


/*---------------------------------------------------------------------------
 *****  Routine to return the name of a facility.
 *
 *  Argument:
 *	facility		- the facility whose name is desired.
 *
 *  Returns:
 *	A pointer to a read only (please) string identifying the facility.
 *
 *****/
PublicFnDef char const *FAC_FacilityDescription (FAC_Facility facility) {
    if (facility) {
	FAC_Request request;

	request.m_requestType	 = FAC_ReturnFacilityDescription;
	request.m_completionCode = FAC_RequestNotImplemented;
	(*facility)(&request);
	if (request.m_completionCode == FAC_RequestSucceeded)
	    return request.m_result.m_string;
    }

    return UTIL_FormatMessage ("Unnamed Facility#%u", facility);
}


/***********************************************
 ***********************************************
 *****  Facility <--> I-Object Interfaces  *****
 ***********************************************
 ***********************************************/

/**************************************************
 *****  Standard Facility Method Definitions  *****
 **************************************************/

/*****  Method to display a facility I-Object  *****/
IOBJ_DefinePublicUnaryMethod (FAC_DisplayFacilityIObject)
{
    IO_printf (
	"#%s",
	FAC_FacilityIdAsString (
	    IOBJ_IObjectValueAsFacility (self)
	)
    );
    return self;
}

/*****  I-Type Definition for a Facility  *****/
PrivateFnDef IOBJ_ITypeFunctionHeader (FacilityIType)
{
    FAC_Request request;
    IOBJ_BeginMD (defaultMD)
	IOBJ_MDE ("qprint"	, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"	, FAC_DisplayFacilityIObject)
    IOBJ_EndMD;

    request.m_requestType	= FAC_ReturnFacilityMD;
    request.m_completionCode	= FAC_RequestNotImplemented;
    (*IOBJ_IObjectValueAsFacility (containingIObject)) (&request);
    return
	FAC_RequestSucceeded == request.m_completionCode
	? request.m_result.m_methodDictionary
	: defaultMD;
}


/*****************************************
 *****************************************
 *****  Facility I-Object Generator  *****
 *****************************************
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the I-Object associated with a facility.
 *
 *  Argument:
 *	facility		- the facility whose I-Object is desired.
 *
 *  Returns:
 *	The I-Object representing this facility.
 *
 *****/
PublicFnDef IOBJ_IObject FAC_FacilityIObject (
    FAC_Facility		facility
)
{
    return IOBJ_PackFacilityIObject (FacilityIType, facility);
}


/***************************************************
 ***************************************************
 *****  Last Completion Code Cache and Access  *****
 ***************************************************
 ***************************************************/

/******************************
 *  Private Global Variables  *
 ******************************/

PrivateVarDef FAC_CompletionCode LastCompletionCode = FAC_RequestNotMade;

/*********************
 *  Access Routines  *
 *********************/

/*---------------------------------------------------------------------------
 *****  Routine to return the completion code of the last facility request
 *****  made through one of the following standard facility request routines.
 *
 *  Returns:
 *	The completion code returned for the last facility request made by a
 *	FAC routine or 'FAC_RequestNotMade'.
 *
 *****/
PublicFnDef FAC_CompletionCode FAC_LastCompletionCode (
    void
)
{
    return LastCompletionCode;
}


/**********************************************
 **********************************************
 *****  Multi-Facility Request Utilities  *****
 **********************************************
 **********************************************/

/*---------------------------------------------------------------------------
 * The following macro expands into a function which will issue the same
 * request to a collection of facilities.  The function expects an argument
 * list consisting of a Nil terminated list of facilities and returns 0 (zero)
 * if all facilities successfully processed the request, -1 otherwise.  The
 * identity of any failed facility is returned via the first parameter to this
 * routine.  The result of this routine is the completion code returned by the
 * last facility called.
 *---------------------------------------------------------------------------
 */

PublicFnDef FAC_CompletionCode MultiFacilityRequestHelper (
    FAC_Facility *		pFailedFacilityReturn,
    FAC_RequestType		requestType,
    va_list			pArgList
) {
    FAC_Request request;
    request.m_requestType	= requestType;
    request.m_completionCode	= FAC_RequestSucceeded;

    V::VArgList iArgList (pArgList);
    FAC_Facility facility;
    while (
	request.m_completionCode == FAC_RequestSucceeded && IsntNil (
	    facility = iArgList.arg<FAC_Facility>()
	)
    ) {
	request.m_completionCode = FAC_RequestNotImplemented;
	facility (&request);
    }

    *pFailedFacilityReturn = facility;
    return request.m_completionCode;
}

#define DefineMultiFacilityRequestHandler(functionName, requestType)\
PublicFnDef FAC_CompletionCode __cdecl functionName (\
    FAC_Facility *pFailedFacilityReturn, ...\
) {\
    V_VARGLIST (ap, pFailedFacilityReturn);\
    return MultiFacilityRequestHelper (\
	pFailedFacilityReturn, requestType, ap\
    );\
}


/****************************************
 ****************************************
 *****  Standard Facility Requests  *****
 ****************************************
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a collection of facilities at system startup.
 *
 *  Arguments:
 *	pFailedFacility		- the address of a 'FAC_Facility' object set to
 *				  either the address of a failed 'FAC_Facility'
 *				  or Nil.
 *	...			- a 'NilOf (FAC_Facility)' terminated list of
 *				  additional facilities to process.
 *
 *  Returns:
 *	The completion code of the last facility processed.
 *
 *****/
DefineMultiFacilityRequestHandler (
    FAC_InitializeFacilities, FAC_DoStartupInitialization
)

/*---------------------------------------------------------------------------
 *****  Routine to shut down a collection of facilities at system shutdown.
 *
 *  Arguments:
 *	pFailedFacility		- the address of a 'FAC_Facility' object set to
 *				  either the address of a failed 'FAC_Facility'
 *				  or Nil.
 *	...			- a 'NilOf (FAC_Facility)' terminated list of
 *				  additional facilities to process.
 *
 *  Returns:
 *	The completion code of the last facility processed.
 *
 *****/
DefineMultiFacilityRequestHandler (
    FAC_ShutdownFacilities, FAC_DoShutdownCleanup
)


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (FAC);
    FAC_FDFCase_FacilityDescription ("Facility");
    default:
	break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  fac.c 1 replace /users/mjc/system
  860226 16:21:40 mjc create

 ************************************************************************/
/************************************************************************
  fac.c 2 replace /users/mjc/system
  860322 10:36:01 mjc Changed :quickPrint to :qprint

 ************************************************************************/
/************************************************************************
  fac.c 3 replace /users/mjc/system
  860327 16:27:36 mjc Deleted : from default messages selectors

 ************************************************************************/
/************************************************************************
  fac.c 4 replace /users/mjc/system
  860403 11:21:27 mjc Fixed failure to preset completion code in most service request routines

 ************************************************************************/
/************************************************************************
  fac.c 5 replace /users/jad
  860421 18:44:15 jad now uses the new io module

 ************************************************************************/
/************************************************************************
  fac.c 6 replace /users/mjc/translation
  870524 09:38:20 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
