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

    //  Wait Status
    public:
        /**
         * Used as the return type of waitUntil() and timed wait operations; indicates the reason for returning.
         */
	enum WaitStatus {
	    WaitStatus_Success, /**< This lock was signaled. */
            WaitStatus_Failure, /**< There was an error. */
            WaitStatus_Timeout /**< The given timeout was reached while waiting. */
	};

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
