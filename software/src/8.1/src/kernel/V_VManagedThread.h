#ifndef V_VManagedThread_Interface
#define V_VManagedThread_Interface

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
    /** Descriptor for a single thread that has been created by and is to be managed exclusively by classes in the V library. */
    class V_API VManagedThread : public VThread {
	DECLARE_ABSTRACT_RTT (VManagedThread, VThread);

    //  Construction
    protected:
        /** Creates (but does not actually spawn) a new thread. */
	VManagedThread ();

    //  Destruction
    protected:
	~VManagedThread ();

    //  Execution
    private:
	static pthread_procedure_result_t Run (ThisClass *pThis);
        /**
         * The entry point for this managed thread.
         * Overridden by subclasses to perform appropriate processing.
         */
	virtual pthread_procedure_result_t run_() = 0;
	pthread_procedure_result_t run ();
    public:
        /** Spawns a thread for this descriptor. */
	bool start ();
        /**
         * @copybrief BaseClass::stop()
         * Delegates to VThread::stop().
         */
	bool stop () {
	    return BaseClass::stop ();
	}
    };
}


#endif
