#ifndef V_VSSID_Interface
#define V_VSSID_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VCount.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VSSID {
	DECLARE_NUCLEAR_FAMILY (VSSID);

    //  Aliases
    public:
#ifdef _WIN32
//*********************************************************************************
//*****  The 32-bit version of MSVC C++ (the only one we support at the moment)
//*****  does not offer 64-bit atomic memory operation intrinsics.  While there
//*****  are library routines that provide them, relying on them causes obscure
//*****  DLL registration errors because of the dependencies they introduce.
//*****  Rather than deal with those issues, use a 32-bit SSID generator.
//*********************************************************************************
	typedef V::counter32_t counter_t;
#else
	typedef V::counter64_t counter_t;
#endif
	typedef V::counter64_t::value_t value_t;

    //  Generator
    private:
	static counter_t g_xNextGeneratedSSID;
    public:
	static value_t NewIdentity () {
	    return g_xNextGeneratedSSID++;
	}

    //  Construction
    public:
	VSSID (VSSID const &rOther) : m_xSSID (rOther.m_xSSID) {
	}
	VSSID () : m_xSSID (0) {
	}

    //  Destruction
    public:
	~VSSID () {
	}

    //  Access
    public:
	operator value_t () const {
	    return m_xSSID;
	}
	value_t value () const {
	    return m_xSSID;
	}
	U32 u32High () const {
	    return (U32)(m_xSSID >> 32);
	}
	U32 u32Low () const {
	    return (U32)(m_xSSID & 0xffffffff);
	}

    //  Query
    public:
	bool operator <  (VSSID const &rOther) const {
	    return m_xSSID <  rOther.m_xSSID;
	}
	bool operator <= (VSSID const &rOther) const {
	    return m_xSSID <= rOther.m_xSSID;
	}
	bool operator == (VSSID const &rOther) const {
	    return m_xSSID == rOther.m_xSSID;
	}
	bool operator != (VSSID const &rOther) const {
	    return m_xSSID != rOther.m_xSSID;
	}
	bool operator >= (VSSID const &rOther) const {
	    return m_xSSID >= rOther.m_xSSID;
	}
	bool operator >  (VSSID const &rOther) const {
	    return m_xSSID >  rOther.m_xSSID;
	}

    //  Update
    public:
	VSSID &operator= (VSSID const &rOther) {
	    m_xSSID = rOther.m_xSSID;
	    return *this;
	}
	void generateSSID () {
	    m_xSSID = NewIdentity ();
	}

    //  State
    protected:
	U64 m_xSSID;
    };

    typedef VSSID::value_t ssid_t;
}


#endif
