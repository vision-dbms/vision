#ifndef Vca_MetaData_Process_Interface
#define Vca_MetaData_Process_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_HostObject.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents a process on some machine. */
	class Vca_MetaData_API Process : public HostObject {
	    DECLARE_CONCRETE_RTTLITE (Process, HostObject);

	//  Construction
	public:
	    Process (Host* pHost);

	//  Destruction
	private:
	    ~Process ();
        
	//  Roles
	public:
	    using BaseClass::getRole;

	//  State
	private:
	};
    }
}


#endif
