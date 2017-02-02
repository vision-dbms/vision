#ifndef V_VSynchronizationObject_Interface
#define V_VSynchronizationObject_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class VSynchronizationObject : public VTransient {
	DECLARE_FAMILY_MEMBERS (VSynchronizationObject, VTransient);

    //  Construction
    protected:
	VSynchronizationObject () {
	}

    //  Destruction
    protected:
	~VSynchronizationObject () {
	}
    };
}


#endif
