#ifndef Vca_MetaData_DomainObject_Interface
#define Vca_MetaData_DomainObject_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_VRolePlayerAddin.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_MetaData_Domain.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /**
         * Base class for classes of domain-specific objects.
         *
         * @see Vca::MetaData::User
         */
	class Vca_MetaData_API DomainObject : public VRolePlayerAddin {
	    DECLARE_FAMILY_MEMBERS (DomainObject, VRolePlayerAddin);

	//  Construction
	protected:
	    DomainObject (VRolePlayer* pContainer, Domain* pDomain);

	//  Destruction
	protected:
	    ~DomainObject ();

	//  Roles
	public:

	//  Access
	public:
	    Domain* domain () const {
		return m_pDomain;
	    }

	//  State
	private:
	    Domain::Reference const m_pDomain;
	};
    }
}


#endif
