#ifndef V_VSchedulable_Interface
#define V_VSchedulable_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAddin.h"
#include "V_VCount.h"
#include "V_VLinkedQueue_.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class VScheduler;

    /** Addin for objects that should support usage with a VScheduler. Supports suspension counting. */
    class V_API VSchedulable : public VAddin {
	DECLARE_FAMILY_MEMBERS (VSchedulable, VAddin);

    //  Aliases
    public:
	typedef V::unsigned_int32_count_t	counter_t;
	typedef VReference<ThisClass>		Reference;
	typedef ThisClass			Schedulable;

    //  Queue
    public:
	class Queue;
	friend class Queue;

    //  Construction
    protected:
	VSchedulable (VReferenceable *pReferenceable) : m_cSuspensions (1), BaseClass (pReferenceable) {
	}

    //  Destruction
    protected:
	~VSchedulable () {
	}

    //  Query
    public:
        /**
         * Used to determine if this VSchedulable is ready to be run.
         *
         * @return true if there are zero suspensions for this VSchedulable, false otherwise.
         */
	bool runnable () const {
	    return m_cSuspensions == 0;
	}

        /**
         * Used to determine if this VSchedulable is suspended.
         *
         * @return false if there are zero suspensions for this VSchedulable, true otherwise.
         */
	bool suspended () const {
	    return m_cSuspensions != 0;
	}

    //  Scheduling
    private:
        /** Called when the suspension count transitions to zero. */
	virtual void schedule_() = 0;

    //  Execution
    private:
        /** Called to perform the action that this object performs when it is no longer suspended. */
	virtual void run_() = 0;
    public:
        /** Attempts to perform the action that this object performs when it is no longer suspended. Does nothing if this object is suspended, delegates to run_() otherwise. */
	void run () {
	    if (m_cSuspensions.isZero ())
		run_();
	}

    protected:
        /** Used to suspend this object. */
	void suspend () {
	    m_cSuspensions.increment ();
	}
        /** Used to indicate that a previous suspension is no longer in effect. This does not cause the schedule_() method to be invoked unless there are no other suspensions still in effect. */
	void resume () {
	    if (m_cSuspensions.decrementIsZero ())
		schedule_();
	}

    //  State
    private:
        /** Linked list. */
	Reference	m_pSuccessor;
        /** The number of suspensions. */
	counter_t	m_cSuspensions;
    };

    /** Queue of VSchedulable objects. */
    class V_API VSchedulable::Queue : public VLinkedQueue_<VSchedulable> {
	DECLARE_FAMILY_MEMBERS (Queue, VLinkedQueue_<VSchedulable>);

    //  Construction
    public:
	Queue () : BaseClass (&VSchedulable::m_pSuccessor) {
	}

    //  Destruction
    public:
	~Queue () {
	}
    };
}


#endif
