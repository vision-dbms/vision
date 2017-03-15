#ifndef VComputationScheduler_Interface
#define VComputationScheduler_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

#include "VComputationList.h"

#include "VkRunTimes.h"

#include "Vca_VTimer.h"


/**************************
 *****  Declarations  *****
 **************************/

class IOMDriver;
class VReadEvalPrintController;
class VTopTaskBase;

/*************************
 *****  Definitions  *****
 *************************/

class VComputationScheduler : public VBenderenceable {
    DECLARE_CONCRETE_RTT (VComputationScheduler, VBenderenceable);

    friend class VReadEvalPrintController;

//  Aliases
public:
    typedef Reference SchedulerReference;
    typedef V::counter32_t counter_t;
    typedef V::U64 gc_timeout_t;

//  Manager
public:
    class Manager : virtual public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (Manager, VReferenceable);

	friend class VComputationScheduler;

    //  Event Processor
    public:
	class EventProcessor;
	friend class EventProcessor;

    //  Construction
    protected:
	Manager ();

    //  Destruction
    protected:
	~Manager () {
	}

    //  Access
    public:
	IOMDriver* activeChannel () const {
	    return m_pActiveScheduler ? m_pActiveScheduler->channel () : 0;
	}
	VComputationScheduler* activeScheduler () const {
	    return m_pActiveScheduler;
	}

    //  Query
    public:
	bool queueIsEmpty () const {
	    return m_pQueueHead.isNil ();
	}
	bool queueIsntEmpty () const {
	    return m_pQueueHead.isntNil ();
	}

    //  Debugger Control
    public:
	bool debuggerEnabled () const {
	    return m_bDebuggerEnabled;
	}
	void setDebuggerEnabledTo (bool bDebuggerEnabled) {
	    m_bDebuggerEnabled = bDebuggerEnabled;
	}
	void toggleDebuggerEnabled () {
	    m_bDebuggerEnabled = !m_bDebuggerEnabled;
	}

    //  Garbage Collection
    private:
	void onGCTimeOut (Vca::VTrigger<ThisClass> *pTrigger);

	void enableGC ();
	void disableGC ();
    public:
	void setGCTimeout (unsigned int sTimeout);

    //  Process Management
    public:
	//  ... implemented by class Batchvision:
	virtual void setExitValue (int value) = 0;
	virtual int  getExitValue () = 0;
	virtual void restartProcess () = 0;

    //  Top Task Accounting
    private:
	void incrementTopTaskCount () {
	    m_cTopTasks++;
	}
	void decrementTopTaskCount () {
	    m_cTopTasks--;
	}

    //  Tuning Parameter Access
    public:
	size_t blockingWait () const {
	    return m_sBlockingWait;
	}
	size_t pollingInterval () const {
	    return m_sPollingInterval;
	}

    //  Tuning Parameter Control
    public:
	void setBlockingWaitTo (size_t sBlockingWait) {
	    m_sBlockingWait = sBlockingWait ? sBlockingWait : 1;
	}
	void setPollingIntervalTo (size_t sPollingInterval) {
	    m_sPollingInterval = sPollingInterval ? sPollingInterval : 1;
	}

    //  Scheduling
    private:
	void schedule (VComputationScheduler* pScheduler);
	void reschedule ();

    //  Master Scheduling
    protected:
	void DoEverything ();
	void StopEverything ();

    //  State
    private:
	//  ... scheduling:
	SchedulerReference	m_pActiveScheduler;

	SchedulerReference	m_pQueueHead;
	SchedulerReference	m_pQueueTail;

	//  ... tuning parameters:
    private:
	size_t			m_sBlockingWait;
	size_t			m_sPollingInterval;
	counter_t		m_cTopTasks;
	counter_t		m_cGCInhibits;
	gc_timeout_t		m_sGCTimeout;
	Vca::VTimer::Reference	m_pGCTimer;
	bool			m_bDebuggerEnabled;
	bool			m_bStopped;
    };
    friend class Manager;

//  Queue
public:
    class Queue : protected VComputationList {
	DECLARE_FAMILY_MEMBERS (Queue, VComputationList);

    //  Construction
    public:
	Queue () : m_pEnqueueReference (this) {
	}

    //  Query
    public:
	operator VComputationUnit* () const {
	    return VComputationList::operator VComputationUnit* ();
	}

	bool isEmpty () const {
	    return VComputationList::isEmpty ();
	}
	bool isntEmpty () const {
	    return VComputationList::isntEmpty ();
	}

    //  Update
    public:
	inline void enqueue (VComputationUnit* pCU);
	inline void dequeue (VReference<VComputationUnit>& rpCU);

	void resetEnqueueReference () {
	    m_pEnqueueReference = this;
	}

    //  State
    protected:
	VReference<VComputationUnit>* m_pEnqueueReference;
    };

//  Friends
    friend class VComputationUnit;
    friend class VTopTaskBase;

//  Tuning Parameter Access
public:
    size_t blockingWait () {
	return m_pTheMan->blockingWait ();
    }
    size_t pollingInterval () {
	return m_pTheMan->pollingInterval ();
    }

//  Tuning Parameter Control
public:
    void setBlockingWaitTo (size_t sBlockingWait) {
	m_pTheMan->setBlockingWaitTo (sBlockingWait);
    }
    void setPollingIntervalTo (size_t sPollingInterval) {
	m_pTheMan->setPollingIntervalTo (sPollingInterval);
    }

//  Construction
public:
    VComputationScheduler ();

//  Destruction
protected:
    ~VComputationScheduler ();

//  Query
public:
    bool QueueIsEmpty () const {
	return m_iActiveQueue.isEmpty ();
    }
    bool QueueIsntEmpty () const {
	return m_iActiveQueue.isntEmpty ();
    }

//  Access
public:
    IOMDriver* channel () const;

    void getTimeSliceStart (VkRunTimes& rRunTimes) const {
	rRunTimes = m_iTimeSliceStart;
    }
    void getCumulativeTime (VkRunTimes& rRunTimes) const {
	rRunTimes = m_iCumulativeTime;
    }
    void getRefreshedCumulativeTime (VkRunTimes& rRunTimes) const {
	rRunTimes.refresh ();
	rRunTimes -= m_iTimeSliceStart;
	rRunTimes += m_iCumulativeTime;
    }

    size_t pollingResidue () const {
	return m_sPollingResidue;
    }

    //  Debugger Control
public:
    bool debuggerEnabled () const {
	return m_pTheMan->debuggerEnabled ();
    }
    void setDebuggerEnabledTo (bool bDebuggerEnabled) const {
	m_pTheMan->setDebuggerEnabledTo (bDebuggerEnabled);
    }
    void toggleDebuggerEnabled () const {
	m_pTheMan->toggleDebuggerEnabled ();
    }

//  Process Management
public:
    void setExitValue (int value) const {
	m_pTheMan->setExitValue (value);
    }
    int getExitValue () const {
	return m_pTheMan->getExitValue ();
    }
    void restartProcess () const {
	m_pTheMan->restartProcess ();
    }

//  Garbage Collection
private:
    void enableGC () const {
	m_pTheMan->enableGC ();
    }
    void disableGC () const {
	m_pTheMan->disableGC ();
    }
public:
    void setGCTimeout (unsigned int sTimeout) const {
	m_pTheMan->setGCTimeout (sTimeout);
    }

//  Time Slice Management
protected:
    void startTimeSlice (VkRunTimes const& rCurrentTimes) {
	m_iTimeSliceStart = rCurrentTimes;
    }
    void endTimeSlice (VkRunTimes& rCurrentTimes);

//  Top Task Accounting
private:
    void incrementTopTaskCount () {
	m_pTheMan->incrementTopTaskCount ();
    }
    void decrementTopTaskCount () {
	m_pTheMan->decrementTopTaskCount ();
    }

//  Execution
protected:
    void processCUnits ();

//  Scheduling
protected:
    inline void schedule (VComputationUnit* pUnit);

    void schedule () {
	m_pTheMan->schedule (this);
    }

private:
    void setPollingResidueTo (size_t sPollingResidue) {
	m_sPollingResidue = sPollingResidue;
    }

protected:
    void yield ();

//  State
protected:
    Manager::Reference const		m_pTheMan;
    Reference				m_pSuccessor;
    Queue				m_iActiveQueue;
    VReference<VComputationUnit>	m_pActiveUnit;
    VkRunTimes				m_iTimeSliceStart;
    VkRunTimes				m_iCumulativeTime;
    size_t				m_sPollingResidue;
};


#endif
