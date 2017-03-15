#ifndef Vca_MetaData_ProcessObject_Interface
#define Vca_MetaData_ProcessObject_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Object.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_MetaData_Process.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /**
         * Base class for classes of process-specific objects.
         *
         * @see Vca::MetaData::Site
         */
	class Vca_MetaData_API ProcessObject : public Object {
	    DECLARE_ABSTRACT_RTTLITE (ProcessObject, Object);

	//  Construction
	protected:
	    ProcessObject (Process* pProcess);

	//  Destruction
	protected:
	    ~ProcessObject ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
	    Process* process () const {
		return m_pProcess;
	    }

	//  State
	private:
	    Process::Reference const m_pProcess;
	};
    }
}


#endif
