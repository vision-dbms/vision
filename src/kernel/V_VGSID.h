#ifndef V_VGSID_Interface
#define V_VGSID_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkUUID.h"
#include "V_VSSID.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VGSID {
	DECLARE_NUCLEAR_FAMILY (VGSID);

    //  Construction
    public:
	static VGSID NewGSID ();

	VGSID (uuid_t const& rUUID, VSSID const& rSSID);
	VGSID (ThisClass const& rOther);
	VGSID ();

    //  Destruction
    public:
	~VGSID ();

    //  Access
    public:
	VSSID const& ssid () const {
	    return m_iSSID;
	}
	VkUUID const& uuid () const {
	    return m_iUUID;
	}

    //  Comparison
    public:
	bool operator < (ThisClass const& rOther) const {
	    return m_iUUID < rOther.uuid () || (
		m_iUUID == rOther.uuid () && m_iSSID < rOther.ssid ()
	    );
	}
	bool operator <= (ThisClass const& rOther) const {
	    return m_iUUID < rOther.uuid () || (
		m_iUUID == rOther.uuid () && m_iSSID <= rOther.ssid ()
	    );
	}
	bool operator == (ThisClass const& rOther) const {
	    return m_iUUID == rOther.uuid () && m_iSSID == rOther.ssid ();
	}
	bool operator != (ThisClass const& rOther) const {
	    return m_iUUID != rOther.uuid () || m_iSSID != rOther.ssid ();
	}
	bool operator >= (ThisClass const& rOther) const {
	    return m_iUUID > rOther.uuid () || (
		m_iUUID == rOther.uuid () && m_iSSID >= rOther.ssid ()
	    );
	}
	bool operator > (ThisClass const& rOther) const {
	    return m_iUUID > rOther.uuid () || (
		m_iUUID == rOther.uuid () && m_iSSID > rOther.ssid ()
	    );
	}

    //  Update
    public:
	ThisClass& operator= (ThisClass const& rOther) {
	    setTo (rOther);
	    return *this;
	}
	void setTo (ThisClass const& rOther) {
	    m_iUUID = rOther.uuid ();
	    m_iSSID = rOther.ssid ();
	}
	void setTo (uuid_t const& rUUID, VSSID const& rSSID) {
	    m_iUUID = rUUID;
	    m_iSSID = rSSID;
	}

	//  State
    private:
	VkUUIDHolder	m_iUUID;
	VSSID		m_iSSID;
    };
}


#endif
