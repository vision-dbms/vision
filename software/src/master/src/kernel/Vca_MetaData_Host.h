#ifndef Vca_MetaData_Host_Interface
#define Vca_MetaData_Host_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Domain.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents a machine. */
	class Vca_MetaData_API Host : public Domain {
	    DECLARE_CONCRETE_RTTLITE (Host, Domain);

	//  Construction
	public:
	    Host (Domain* pParent);

	//  Destruction
	private:
	    ~Host ();
        
	//  Roles
	public:
	    using BaseClass::getRole;

	//  State
	private:
	};
    }
}


#endif
