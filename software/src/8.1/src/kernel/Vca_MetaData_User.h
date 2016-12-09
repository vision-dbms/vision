#ifndef Vca_MetaData_User_Interface
#define Vca_MetaData_User_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Object.h"

#include "Vca_MetaData_DomainObject.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents a user. */
	class Vca_MetaData_API User : public Object, public DomainObject {
	    DECLARE_CONCRETE_RTTLITE (User, Object);

	//  Construction
	public:
	    User (Domain* pDomain);

	//  Destruction
	private:
	    ~User ();
        
	//  Roles
	public:
	    using BaseClass::getRole;
//	    using DomainObject::getRole;

	//  State
	private:
	};
    }
}


#endif
