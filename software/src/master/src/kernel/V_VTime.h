#ifndef V_VTime_Interface
#define V_VTime_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

/******************************************************************************
 *---- V::VTime objects are 64 bit, microsecond resolution, time stamps.  ----*
 ******************************************************************************/

namespace V {
    class V_API VTime : public VTransient {
	DECLARE_FAMILY_MEMBERS (VTime, VTransient);

    //  Constants
    public:
	enum {
	    OneM  = 1000,
	    OneMM = 1000000
	};

    //  Utilities
    public:
	static U64 encode (struct time const &rTime);
	static U64 encodeNow ();

    //  Construction
    public:
	VTime (struct time const &rTime) : m_iValue (encode (rTime)) {
	}
	VTime (VTime const &rOther) : m_iValue (rOther.m_iValue) {
	}
	VTime () : m_iValue (encodeNow ()) {
	}
	VTime (U64 iValue) : m_iValue (iValue) {
	}

    //  Destruction
    public:
	~VTime () {
	}

    //  Access
    public:
	operator time_t () const {
	    return static_cast<time_t> (m_iValue/OneMM);
	}
	bool asString (VString &rTimestring) const;

	void asValue (struct time& rResult) const;
	void asValue (struct timespec& rResult) const;

	char *ctime (char *pResult) const;
	struct tm *localtime (struct tm &rResult) const;

	U64 encodedValue () const {
	    return m_iValue;
	}

    //  Query
    public:
	bool operator <  (VTime const &rOther) const {
	    return m_iValue < rOther.m_iValue;
	}
	bool operator <= (VTime const &rOther) const {
	    return m_iValue <= rOther.m_iValue;
	}
	bool operator == (VTime const &rOther) const {
	    return m_iValue == rOther.m_iValue;
	}
	bool operator != (VTime const &rOther) const {
	    return m_iValue != rOther.m_iValue;
	}
	bool operator >= (VTime const &rOther) const {
	    return m_iValue >= rOther.m_iValue;
	}
	bool operator >  (VTime const &rOther) const {
	    return m_iValue > rOther.m_iValue;
	}

	bool isInTheFuture () const {
	    return m_iValue > encodeNow ();
	}
	bool isInThePast () const {
	    return m_iValue < encodeNow ();
	}

    //  Arithmetic
    public:
	VTime operator+ (S64 sDelta) const {
	    return VTime (m_iValue + sDelta);
	}
	VTime operator+ (U64 sDelta) const {
	    return VTime (m_iValue + sDelta);
	}
	VTime operator- (S64 sDelta) const {
	    return VTime (m_iValue - sDelta);
	}
	VTime operator- (U64 sDelta) const {
	    return VTime (m_iValue - sDelta);
	}

	S64 operator- (VTime const &rOther) const {
	    return m_iValue - rOther.m_iValue;
	}

    //  Update
    public:
	void setTo (VTime const &rOther) {
	    m_iValue = rOther.m_iValue;
	}
	void setTo (struct time const &rTime) {
	    m_iValue = encode (rTime);
	}
	void setTo (U64 rOther) {
	    m_iValue = rOther;
	}
	void setToNow () {
	    m_iValue = encodeNow ();
	}

	VTime &operator= (VTime const &rOther) {
	    setTo (rOther);
	    return *this;
	}
	VTime &operator= (struct time const &rTime) {
	    setTo (rTime);
	    return *this;
	}
	VTime &operator= (U64 rOther) {
	    setTo (rOther);
	    return *this;
	}

	VTime &operator+= (S64 sDelta) {
	    m_iValue += sDelta;
	    return *this;
	}
	VTime &operator+= (U64 sDelta) {
	    m_iValue += sDelta;
	    return *this;
	}
	VTime &operator-= (S64 sDelta) {
	    m_iValue -= sDelta;
	    return *this;
	}
	VTime &operator-= (U64 sDelta) {
	    m_iValue -= sDelta;
	    return *this;
	}

    //  State
    protected:
	U64 m_iValue;
    };
}


#endif
