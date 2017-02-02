/*****  Facility Interface  *****/
#ifndef FAC_Interface
#define FAC_Interface

/*---------------------------------------------------------------------------
 * Facilities are the modules of which a system is built.  Aside from the
 * specific services performed by the facility (mapping files, handling
 * errors, implementing object memory, etc.), a facility must also provide a
 * standard, uniform set of services for identifying the facility,
 * implementing start-up and shut-down processing, interactively interrogating
 * the state of the facility, and a variety of other miscellaneous functions.
 * Providing that standard set of services in a uniform manner is the purpose
 * of the FACility facility.
 *
 * In order to support its standardized interface to the rest of the system,
 * each facility implements a Facility Definition Function whose address
 * serves as an active unique identifier for the facility.  The Facility
 * Definition Function is expected to respond to certain fixed requests to
 * perform specific tasks or provide specific information about the Facility.
 * Requests are made by constructing an object of type 'FAC_Request' and
 * applying the Facility Definition Function to its address.  The definitions
 * that follow specify in more detail the types and format of requests along
 * with utilities that standardize the decomposition of requests and the
 * definition and declaration of facilities.
 *---------------------------------------------------------------------------
 */

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

#include "viobj.h"


/******************************
 ******************************
 *****  Type Definitions  *****
 ******************************
 ******************************/

/*---------------------------------------------------------------------------
 *****     Facility Definition Function Request Set Type Definition	*****
 * The following enumerated type defines the set of requests to which every
 * Facility Definition Function must respond.
 *---------------------------------------------------------------------------
 */
typedef enum FAC_RequestType {
    FAC_ReturnFacilityIdAsString,
    FAC_ReturnFacilityDescription,
    FAC_ReturnFacilitySccsId,
    FAC_ReturnFacilityMD,
    FAC_DoStartupInitialization,
    FAC_DoShutdownCleanup
} FAC_RequestType;

/*---------------------------------------------------------------------------
 ******  Facility Definition Function Completion Code Type Definition	*****
 * The following enumerated type defines the set of completion status codes
 * that a Facility Definition Function can return.
 *---------------------------------------------------------------------------
 */
typedef enum FAC_CompletionCode {
    FAC_RequestSucceeded,
    FAC_RequestFailed,
    FAC_RequestNotImplemented,
    FAC_RequestAlreadyDone,
    FAC_RequestNotMade
} FAC_CompletionCode;

/*---------------------------------------------------------------------------
 *****		Facility Definition Function Request Definition		*****
 * The following type definition defines the structure of a request as
 * presented to a Facility Definition Function.  Minimally, the facility
 * definition function can expect 'Request' to be set to the desired request
 * and 'CompletionCode' to be set to the value FAC_RequestNotImplemented.
 *---------------------------------------------------------------------------
 */
struct FAC_Request {
    FAC_RequestType		m_requestType;
    FAC_CompletionCode		m_completionCode;
    union request_data_t {
	char const *		    m_string;
	IOBJ_MD			    m_methodDictionary;
    }				m_result;
};


/********************************************************
 ********************************************************
 *****  Facility Definition And Declaration Macros  *****
 ********************************************************
 ********************************************************/

/*---------------------------------------------------------------------------
 *****			Facility Definition Macro			*****
 * Macro to define a facility.  This macro serves as the header for the
 * facility definition function.  Before this macro is expanded in the body of
 * a facility, the macro 'FacilityName' should be defined to be the name of
 * the facility.  Because macro accesses to other routines in other facilities
 * may need to know the facility from which they are being invoked (i.e., the
 * error handler), it is good form to define 'FacilityName' at the beginning
 * of the '.c' file for the facility.  This macro is intended to be followed
 * by the body of the facility definition function:
 *
 *	#define FacilityName ERR
 *
 *	...
 *
 *	FAC_DefineFacility
 *	{
 *	    switch (FAC_RequestTypeField)
 *	    {
 *	    case FAC_ReturnFacilityName:
 *		FAC_StringResultField = "Error Handler";
 *		FAC_CompletionCodeField = FAC_RequestSucceeded;
 *		break;
 *	    case FAC_ReturnSccsId:
 *		FAC_StringResultField = "%W%:%G%:%H%:%T%";
 *		FAC_CompletionCodeField = FAC_RequestSucceeded;
 *		break;
 *	    default:
 *		break;
 *	    }
 *	}
 *
 * Note that this example is accurate.  The symbols used to designate
 * components of the request and the destinations of results are defined below.
 *---------------------------------------------------------------------------
 */
#define FAC_DefineFacility void FacilityName (\
    FAC_Request *requestAddress\
)

/*---------------------------------------------------------------------------
 *			Facility Declaration Macro			*****
 * Macro to declare a facility.  Use this macro to declare any facility that
 * must be referenced from within another facility.
 *---------------------------------------------------------------------------
 */
#define FAC_DeclareFacility(facilityName) PublicFnDecl void facilityName (\
    FAC_Request *requestAddress\
)

/*---------------------------------------------------------------------------
 *****			Request Decomposition Macros			*****
 * Use these macros to access the fields of a request in the body of a
 * facility definition function.
 *---------------------------------------------------------------------------
 */
#define FAC_RequestTypeField		((requestAddress)->m_requestType)
#define FAC_CompletionCodeField		((requestAddress)->m_completionCode)
#define FAC_StringResultField		((requestAddress)->m_result.m_string)
#define FAC_MDResultField		((requestAddress)->m_result.m_methodDictionary)


/************************************************************
 ************************************************************
 *****  Prepackaged Facility Definition Function Cases  *****
 ************************************************************
 ************************************************************/

#define FAC_FDFCase_FacilityIdAsString(facilityId)\
    case FAC_ReturnFacilityIdAsString:\
        FAC_StringResultField = #facilityId;\
	FAC_CompletionCodeField = FAC_RequestSucceeded;\
        break

#define FAC_FDFCase_FacilityDescription(description)\
    case FAC_ReturnFacilityDescription:\
        FAC_StringResultField = description;\
	FAC_CompletionCodeField = FAC_RequestSucceeded;\
        break

#define FAC_FDFCase_FacilitySccsId(sccsIdString)\
    case FAC_ReturnFacilitySccsId:\
        FAC_StringResultField = sccsIdString;\
	FAC_CompletionCodeField = FAC_RequestSucceeded;\
        break

#define FAC_FDFCase_FacilityMD(methodDictionary)\
    case FAC_ReturnFacilityMD:\
        FAC_MDResultField = methodDictionary;\
	FAC_CompletionCodeField = FAC_RequestSucceeded;\
        break


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl char const *FAC_CompletionCodeName (
    FAC_CompletionCode		completionCode
);

PublicFnDecl char const *FAC_FacilityIdAsString (
    FAC_Facility		facility
);

PublicFnDecl char const *FAC_FacilityDescription (
    FAC_Facility		facility
);

IOBJ_DeclarePublicMethod (FAC_DisplayFacilityIObject);

PublicFnDecl IOBJ_IObject FAC_FacilityIObject (
    FAC_Facility		facility
);

PublicFnDecl FAC_CompletionCode __cdecl FAC_InitializeFacilities (
    FAC_Facility *		pFailedFacilityReturn,
    ...
);

PublicFnDecl FAC_CompletionCode __cdecl FAC_ShutdownFacilities (
    FAC_Facility *		pFailedFacilityReturn,
    ...
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  fac.h 1 replace /users/mjc/system
  860226 16:21:44 mjc create

 ************************************************************************/
/************************************************************************
  fac.h 2 replace /users/mjc/translation
  870524 09:38:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
