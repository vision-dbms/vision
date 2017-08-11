/*****  Vxa_VClass Implementation  *****/

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

#include "Vxa_VClass.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallHandle.h"

#include "VSimpleFile.h"


/*************************
 *************************
 *****               *****
 *****  Vxa::VClass  *****
 *****               *****
 *************************
 *************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VClass::VClass () : m_cCollectionsCreated (0), m_cCollectionsDeleted (0), m_cInstancesReported (0), m_cInstancesReleased (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

namespace {
    VSimpleFile g_iLeakLog;

    VSimpleFile *LeakLogFile () {
	static bool bFirstCall = true;
	if (bFirstCall) {
	    bFirstCall = false;
            char const *const pLeakLog = getenv ("VxaTracingLeaks") ? getenv ("VxaLeakLog") : 0;
	    if (pLeakLog) {
		g_iLeakLog.OpenForTextAppend (pLeakLog);
		g_iLeakLog.enableAutoFlush ();
	    }
	}
	return g_iLeakLog.isOpen () ? &g_iLeakLog : 0;
    }
}

Vxa::VClass::~VClass () {
    static VSimpleFile *const pLeakLogFile = LeakLogFile ();
    if (pLeakLogFile && m_cCollectionsCreated > 0) {
	pLeakLogFile->printf (
	    "%-45.45s:%5llu %5llu %5llu %8llu %8llu %8llu\n",
	    m_iIdentification.content (),
	    m_cCollectionsCreated, m_cCollectionsDeleted, m_cCollectionsCreated - m_cCollectionsDeleted,
	    m_cInstancesReported , m_cInstancesReleased , m_cInstancesReported  - m_cInstancesReleased
	);
    }
}

/*******************************
 *******************************
 *****  Method Definition  *****
 *******************************
 *******************************/

bool Vxa::VClass::defineMethod (VMethod *pMethod) {
    unsigned int xElement;
    m_iDictionary.Insert (pMethod->name (), xElement);
    m_iDictionary.value(xElement).setTo (pMethod);

    m_iHelpInfo << pMethod->name () << "\n";

    return true;
}

bool Vxa::VClass::defineHelp (char const *pWhere) {
    VString iHelpInfo;
    iHelpInfo << "The class " << pWhere << " supports the following methods:\nhelp\n" << m_iHelpInfo;
    defineConstant ("help", iHelpInfo);
}

/*******************************
 *******************************
 *****  Method Invocation  *****
 *******************************
 *******************************/

bool Vxa::VClass::invokeMethod (
    VString const &rName, VCallHandle const &rCallHandle, VCollection *pCollection
) const {
    unsigned int xElement = UINT_MAX;
    return m_iDictionary.Locate (rName, xElement)
	?  rCallHandle.invoke (m_iDictionary.value(xElement), pCollection)
	:  rCallHandle.returnSNF ();
}
