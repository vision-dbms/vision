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

bool Vxa::VClass::defineMethod (VString const &rName, VMethod *pMethod) {
    unsigned int xElement;
    m_iDictionary.Insert (rName, xElement);
    m_iDictionary.value(xElement).setTo (pMethod);
    return true;
}

bool Vxa::VClass::defineDefault (VMethod *pMethod) {
    m_pDefaultMethod.setTo (pMethod);
    return true;
}

/***************************
 ***************************
 *****  Method Access  *****
 ***************************
 ***************************/

bool Vxa::VClass::getMethod (VMethod::Reference &rpMethod, VCallHandle const &rCallHandle) const {
    unsigned int xElement = UINT_MAX;
    if (m_iDictionary.Locate (rCallHandle.selectorName (), xElement)) {
	rpMethod.setTo (m_iDictionary.value(xElement));
	return true;
    }
    rpMethod.setTo (m_pDefaultMethod);
    return rpMethod.isntNil ();
}

/*******************************
 *******************************
 *****  Method Invocation  *****
 *******************************
 *******************************/

bool Vxa::VClass::invokeMethod (VCallHandle const &rCallHandle, VCollection *pCluster) const {
    VMethod::Reference pMethod;
    return getMethod (pMethod, rCallHandle)
	?  rCallHandle.invokeMethod (pMethod, pCluster)
	:  rCallHandle.returnSNF ();
}
