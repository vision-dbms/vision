/*****  V_VLogger Implementation  *****/

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

#include "V_VLogger.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VRTTI.h"
#include "V_VString.h"
#include "V_VTime.h"


/************************
 ************************
 *****              *****
 *****  V::VLogger  *****
 *****              *****
 ************************
 ************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VLogger::VLogger (
    char const *pLogFileName, bool bAutoFlushEnabled, bool bTimestampsEnabled
) : m_bAutoFlushEnabled (bAutoFlushEnabled), m_bTimestampsEnabled (bTimestampsEnabled) {
    if (pLogFileName) {
	m_iLogFile.OpenForTextAppend (pLogFileName);
	enableLogFileAutoFlush ();
    }
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void V::VLogger::enableLogFileAutoFlush () const {
    if (m_bAutoFlushEnabled)
	m_iLogFile.enableAutoFlush ();
}

void V::VLogger::disableLogFileAutoFlush () const {
    if (m_bAutoFlushEnabled)
	m_iLogFile.disableAutoFlush ();
}


/*********************
 *********************
 *****  Logging  *****
 *********************
 *********************/

void V::VLogger::outputTimestamp () const {
    VString vsNow; {
	V::VTime iNow;
	iNow.asString (vsNow);
    }
    m_iLogFile.printf ("%s: ", vsNow.content ());
}

void V::VLogger::logDataPrefix (unsigned int xObject, char const *pWhere, std::type_info const &rWhat, size_t cElements) const {
    if (isActive ()) {
	disableLogFileAutoFlush ();
	V::VRTTI iWhat (rWhat);
	logDataPrefix (xObject, pWhere, iWhat.name (), cElements);
    }
}

void V::VLogger::logDataPrefix (unsigned int xObject, char const *pWhere, std::type_info const &rWhat) const {
    if (isActive ()) {
	disableLogFileAutoFlush ();
	V::VRTTI iWhat (rWhat);
	logDataPrefix (xObject, pWhere, iWhat.name ());
    }
}

void V::VLogger::logDataPrefix (unsigned int xObject, char const *pWhere, char const *pWhat, size_t cElements) const {
    if (isActive ()) {
	outputTimestamp ();
	m_iLogFile.printf ("%5u: %s: %s[%u][", xObject, pWhere, pWhat, cElements);
    }
}

void V::VLogger::logDataPrefix (unsigned int xObject, char const *pWhere, char const *pWhat) const {
    if (isActive ()) {
	outputTimestamp ();
	m_iLogFile.printf ("%5u: %s: %s[", xObject, pWhere, pWhat);
    }
}

void V::VLogger::logDataSuffix () const {
    if (isActive ()) {
	enableLogFileAutoFlush ();
	m_iLogFile.printf ("\n%7c]\n", ' ');
    }
}

void V::VLogger::log (unsigned int xElement, bool iValue) const {
    if (isActive ()) {
	if (0 == xElement % 40)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf ("%c", iValue ? 'T' : 'F');
    }
}

void V::VLogger::log (unsigned int xElement, int iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf ("%-10d", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, unsigned int iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf ("%-10u", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, __int64 iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf ("%-10lld", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, unsigned __int64 iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf ("%-10llu", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, float iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf (" %-10g", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, double iValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf (" %-10g", iValue);
    }
}

void V::VLogger::log (unsigned int xElement, char const *pValue) const {
    if (isActive ())
	m_iLogFile.printf ("\n%10c\"%s\"", ' ', pValue);
}

void V::VLogger::log (unsigned int xElement, VString const &rValue) const {
    log (xElement, rValue.content ());
}

void V::VLogger::log (unsigned int xElement, void const *pValue) const {
    if (isActive ()) {
	if (0 == xElement % 10)
	    m_iLogFile.printf ("\n%11c", ' ');
	m_iLogFile.printf (" %p", pValue);
    }
}

int __cdecl V::VLogger::printf (char const *pFormat, ...) const {
    V_VARGLIST (pFormatArgs, pFormat);
    return vprintf (pFormat, pFormatArgs);
}

int V::VLogger::vprintf (char const *pFormat, va_list pFormatArgs) const {
    if (isActive ()) {
	outputTimestamp ();
	return m_iLogFile.vprintf (pFormat, pFormatArgs);
    }
    return 0;
}
