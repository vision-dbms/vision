/*****  Vxa_VCallHandle Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vxa_VCallHandle.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "V_VRTTI.h"
#include "V_VTime.h"


/******************************
 ******************************
 *****                    *****
 *****  Vxa::VCallHandle  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallHandle::VCallHandle (
    VCollection *pCluster, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask, bool bIntensional
) : BaseClass (pCluster, rMethodName, cParameters, cTask, bIntensional) {
}

Vxa::VCallHandle::VCallHandle (ThisClass const &rOther) : BaseClass (rOther) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallHandle::~VCallHandle () {
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

namespace {
    Vxa::cardinality_t g_xCardinalityReportingThreshold = UINT_MAX;
    Vxa::cardinality_t g_xParameterCountReportingThreshold = UINT_MAX;

    bool Reporting () {
	char const *const pCRT = getenv ("VxaCRT");
	if (pCRT)
	    g_xCardinalityReportingThreshold = atoi (pCRT);
	char const *const pPCRT = getenv ("VxaPCRT");
	if (pPCRT)
	    g_xParameterCountReportingThreshold = atoi (pPCRT);
	return pCRT || pPCRT;
    }
    bool g_bReporting = Reporting ();
}

bool Vxa::VCallHandle::makingReport () const {
    return g_bReporting && cardinality () >= g_xCardinalityReportingThreshold  && parameterCount () >= g_xParameterCountReportingThreshold;
}

void Vxa::VCallHandle::report (char const *pFormat, ...) const {
    VString iNowString; {
	V::VTime iNow;
	iNow.asString (iNowString);
    }

    V_VARGLIST (ap, pFormat);
    VString iMessage;
    iMessage.vprintf (pFormat, ap);

    fprintf (stderr, "+++ %p: %s: %s\n", caller (), iNowString.content (), iMessage.content ());
}

void Vxa::VCallHandle::reportInvocation () const {
    if (makingReport ()) {
	report ("On Invocation PC:%u TC:%u", parameterCount (), cardinality ());
    }
}

void Vxa::VCallHandle::reportParameterRequest (unsigned int xParameter) const {
    if (makingReport ()) {
	report ("On Parameter Request %u", xParameter);
    }
}

void Vxa::VCallHandle::reportParameterReceipt (unsigned int xParameter) const {
    if (makingReport ()) {
	report ("On Parameter Receipt %u", xParameter);
    }
}

void Vxa::VCallHandle::reportCompletion () const {
    if (makingReport ()) {
	report ("On Completion");
    }
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vxa::VCallHandle::raiseTypeException (
    std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType, char const *pWhere
) const {
    V::VRTTI iOriginatorRTTI (rOriginatorType);
    V::VRTTI iUnexpectedRTTI (rUnexpectedType);

    VString iMessage (128);
    iMessage << iOriginatorRTTI.name () << ": Unsupported " << pWhere << " Type: " << iUnexpectedRTTI.name ();

    return returnError (iMessage);
}

bool Vxa::VCallHandle::raiseParameterTypeException (
    std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
) const {
    return raiseTypeException (rOriginatorType, rUnexpectedType, "Parameter");
}

bool Vxa::VCallHandle::raiseResultTypeException (
    std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
) const {
    return raiseTypeException (rOriginatorType, rUnexpectedType, "Result");
}

bool Vxa::VCallHandle::raiseUnimplementedOperationException (
    std::type_info const &rOriginatorType, char const *pWhere
) const {
    V::VRTTI iOriginatorRTTI (rOriginatorType);
    VString iMessage (128);
    iMessage << iOriginatorRTTI.name () << "::" << pWhere << ": Not Implemented";

    return returnError (iMessage);
}

bool Vxa::VCallHandle::returnNA () const {
    return returnError ("No Result");
}
