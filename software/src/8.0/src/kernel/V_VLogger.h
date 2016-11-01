#ifndef V_VLogger_Interface
#define V_VLogger_Interface

/************************
 *****  Components  *****
 ************************/

#include "VSimpleFile.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "VkDynamicArrayOf.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VLogger {
	DECLARE_FAMILY_MEMBERS (VLogger,void);

    //  Construction
    public:
	VLogger (char const *pLogFileName, bool bAutoFlushEnabled = true, bool bTimestampsEnabled = true);

    //  Destruction
    public:
	~VLogger () {
	}

    //  Access/Query
    public:
	bool isActive () const {
	    return m_iLogFile.isOpen ();
	}
	bool isntActive () const {
	    return m_iLogFile.isntOpen ();
	}

	bool autoFlushEnabled () const {
	    return m_bAutoFlushEnabled;
	}

	bool timestampsEnabled () const {
	    return m_bTimestampsEnabled;
	}

    //  Control
    public:
	void enableAutoFlush () {
	    m_bAutoFlushEnabled = true;
	}
	void disableAutoFlush () {
	    m_bAutoFlushEnabled = false;
	}
	void flush () {
	    m_iLogFile.flush ();
	}

	void enableTimestamps () {
	    m_bTimestampsEnabled = true;
	}
	void disableTimestamps () {
	    m_bTimestampsEnabled = false;
	}

    //  Logging
    private:
	void outputTimestamp () const;

	void enableLogFileAutoFlush () const;
	void disableLogFileAutoFlush () const;

	void logDataPrefix (unsigned int xObject, char const *pWhere, std::type_info const &rWhat, size_t cElements) const;
	void logDataPrefix (unsigned int xObject, char const *pWhere, std::type_info const &rWhat) const;
	void logDataPrefix (unsigned int xObject, char const *pWhere, char const *pWhat, size_t cElements) const;
	void logDataPrefix (unsigned int xObject, char const *pWhere, char const *pWhat) const;
	void logDataSuffix () const;
	void log (unsigned int xElement, bool iValue) const;
	void log (unsigned int xElement, int iValue) const;
	void log (unsigned int xElement, unsigned int iValue) const;
	void log (unsigned int xElement, __int64 iValue) const;
	void log (unsigned int xElement, unsigned __int64 iValue) const;
	void log (unsigned int xElement, float iValue) const;
	void log (unsigned int xElement, double iValue) const;
	void log (unsigned int xElement, char const *pValue) const;
	void log (unsigned int xElement, VString const &rValue) const;
	void log (unsigned int xElement, void const *pValue) const;
    public:
	template <typename value_t> void log (unsigned int xObject, char const *pWhere, value_t const &rValue) const {
	    if (isActive ()) {
		logDataPrefix (xObject, pWhere, typeid (value_t));
		log (0, rValue);
		logDataSuffix ();
	    }
	}
	template <typename value_t> void log (unsigned int xObject, char const *pWhere, value_t const *pArray, unsigned int sArray) const {
	    if (isActive ()) {
		logDataPrefix (xObject, pWhere, typeid (value_t), sArray);
		for (unsigned int xElement = 0; xElement < sArray; xElement++)
		    log (xElement, pArray[xElement]);
		logDataSuffix ();
	    }
	}
	template <typename value_t> void log (unsigned int xObject, char const *pWhere, VkDynamicArrayOf<value_t> const &rValues) const {
	    if (isActive ()) {
		log (xObject, pWhere, rValues.elementArray (), rValues.elementCount ());
	    }
	}
	int __cdecl printf (char const *pFormat, ...) const;
	int vprintf (char const *pFormat, va_list pFormatArgs) const;

    //  State
    private:
	VSimpleFile mutable m_iLogFile;
	bool m_bAutoFlushEnabled;
	bool m_bTimestampsEnabled;
    };
}


#endif
