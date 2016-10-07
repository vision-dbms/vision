#ifndef Vca_MetaData_Object_Interface
#define Vca_MetaData_Object_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca_MetaData.h"

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Base class for all descriptive objects within the Vca::MetaData namespace. */
	class Vca_MetaData_API Object : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Object, VRolePlayer);

	//  Construction
	protected:
	    Object ();

	//  Destruction
	protected:
	    ~Object ();

	//  Roles
	public:
	    using BaseClass::getRole;
	};
    }
}


#endif
