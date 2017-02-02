#ifndef V_VUnmanagedThread_Interface
#define V_VUnmanagedThread_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "V_VThread.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /** Descriptor for a single thread that has @e not been created by classes in the V library. */
    class VUnmanagedThread : public VThread {
	DECLARE_CONCRETE_RTT (VUnmanagedThread, VThread);

    //  Construction
    public:
	VUnmanagedThread ();

    //  Destruction
    private:
	~VUnmanagedThread ();
    };
}


#endif
