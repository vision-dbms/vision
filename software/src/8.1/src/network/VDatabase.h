#ifndef VDatabase_Interface
#define VDatabase_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

#include "VkUUID.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VDatabase : public VBenderenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VDatabase, VBenderenceable);

//  Construction
public:
    VDatabase (uuid_t const &rDatabaseUUID) : m_iUUID (rDatabaseUUID) {
    }

//  Destruction
private:
    ~VDatabase () {
    }

//  Access
public:
    VkUUID const &uuid () const {
	return m_iUUID;
    }

//  State
protected:
    VkUUIDHolder const m_iUUID;
};


#endif
