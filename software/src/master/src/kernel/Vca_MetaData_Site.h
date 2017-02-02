#ifndef Vca_MetaData_Site_Interface
#define Vca_MetaData_Site_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_ProcessObject.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents the Vca state inside of a process. */
	class Vca_MetaData_API Site : public ProcessObject {
	    DECLARE_CONCRETE_RTTLITE (Site, ProcessObject);

	//  Construction
	public:
	    Site (Process* pProcess, uuid_t const &rUUID);

	//  Destruction
	private:
	    ~Site ();
        
	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
	    uuid_t const &uuid () const {
		return m_iUUID;
	    }

	//  State
	private:
	    VkUUIDHolder const m_iUUID;
	};
    }
}


#endif
