#ifndef Vca_MetaData_Domain_Interface
#define Vca_MetaData_Domain_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Object.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents a set of systems that have been configured to operate as a unit. */
	class Vca_MetaData_API Domain : public Object {
	    DECLARE_ABSTRACT_RTTLITE (Domain, Object);

	//  Construction
	protected:
	    Domain (Domain* pParent);

	//  Destruction
	protected:
	    ~Domain ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
	    Domain* parent () const {
		return m_pParent;
	    }

	//  State
	private:
	    Domain::Reference const m_pParent;
	};
    }
}


#endif
