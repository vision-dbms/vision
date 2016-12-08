#ifndef V_VAtom_Interface
#define V_VAtom_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "VkMapOf.h"
#include "VkUUID.h"
#include "V_VString.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VAtom : public VReferenceable {
	DECLARE_CONCRETE_RTT (VAtom, VReferenceable);

    //  Globals
    private:
	typedef VkMapOf<VkUUIDHolder, uuid_t const&, uuid_t const&, VAtom*> AtomTable;
	static AtomTable g_iAtomTable;

    //  Construction
    private:
	VAtom (uuid_t const &rUUID);

    public:
	VAtom *instance (uuid_t const &rUUID);
	VAtom *instance (char const *pUUID);
	VAtom *instance (
	    unsigned int    iD1,
	    unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1,
	    unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5,
	    unsigned char iD4_6, unsigned char iD4_7
	);

    //  Destruction
    private:
	~VAtom ();

    //  Access
    public:
	VString const &description () const {
	    return m_iDescription;
	}
	VkUUID const &uuid () const {
	    return m_iUUID;
	}

    //  Update
    public:
	void setDescriptionTo (char const *pDescription, bool bCopy = true) {
	    if (bCopy)
		m_iDescription.setTo (pDescription);
	    else
		m_iDescription.setToStatic (pDescription);
	}
	void setDescriptionTo (VString const &rDescription) {
	    m_iDescription.setTo (rDescription);
	}

    //  State
    private:
	VkUUIDHolder const	m_iUUID;
	VString			m_iDescription;
    };
}


#endif
