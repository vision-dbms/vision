#ifndef Vca_MetaData_HostObject_Interface
#define Vca_MetaData_HostObject_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Object.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_MetaData_Host.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /**
         * Base class for classes of host-specific objects.
         *
         * @see Vca::MetaData::Process
         */
	class Vca_MetaData_API HostObject : public Object {
	    DECLARE_ABSTRACT_RTTLITE (HostObject, Object);

	//  Construction
	protected:
	    HostObject (Host* pHost);

	//  Destruction
	protected:
	    ~HostObject ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
	    Host* host () const {
		return m_pHost;
	    }

	//  State
	private:
	    Host::Reference const m_pHost;
	};
    }
}


#endif
