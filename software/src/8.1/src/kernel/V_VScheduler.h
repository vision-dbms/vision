#ifndef V_VScheduler_Interface
#define V_VScheduler_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VSchedulable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VScheduler : public VTransient {
	DECLARE_FAMILY_MEMBERS (VScheduler, VTransient);

    //  Construction
    public:
	VScheduler ();

    //  Destruction
    public:
	~VScheduler ();

    //  Scheduling
    public:
        /**
         * Schedules an element for execution and runs scheduled events if the event was scheduled as the first event to be processed by this scheduler.
         *
         * @param pSchedulable the event to be scheduled and run when appropriate.
         *
         * @see Queue::dequeue(Queue::ElementLink&, bool&)
         */
	void schedule (VSchedulable *pSchedulable);

    //  State
    private:
        /** Event queue. */
	VSchedulable::Queue m_iQueue;
    };
}


#endif
