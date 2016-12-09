#ifndef V_VSpinlock_Interface
#define V_VSpinlock_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VLock.h"

#include "V_VCount.h"
#include "V_VSemaphore.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
  /*******************************************************************************************
   *****  This spinlock implementation is a hybrid spin/suspend lock designed to work
   *****  efficiently in both uniprocessor and multiprocessor environments.  The
   *****  assumptions used to design the algorithm are:
   *****
   *****	- lock contention will be low, so acquisition and release of uncontested
   *****	  locks should be extremely efficient.
   *****	- there is no point spinning out a time slice on a uni-processor system
   *****	  where there is no chance that the state of the spinlock will change
   *****	  because no other thread can be running to change it.
   *****
   *****  The basic design of the algorithm uses an atomic counter to record the number
   *****  of threads contending for the lock.  That counter is incremented before lock
   *****  acquisition is attempted and decremented after lock release.  The counter is
   *****  also decremented by lock contenders that fail to acquire the lock and simply
   *****  withdraw from contention.
   *****
   *****  The hybrid algorithm uses a semaphore to suspend threads that fail to acquire
   *****  the lock within the spin count limit of the lock.  On resumption from that
   *****  wait, a suspended thread repeats the spin cycle.  Resumption occurs when the
   *****  current holder of the lock increments the semaphore after releasing the lock.
   *****  To restrict use of the semaphore to only those cases that require it, the
   *****  contention count is used to suppress semaphore increment (produce) if the
   *****  lock holder is the last contender for the lock.  While the algorithm would
   *****  function correctly without the contention count, it would introduce unnecessary
   *****  semphore contention for otherwise uncontested locks and cause wasteful spinning
   *****  of contested locks running the semaphore count down to zero.
   *****
   *****  Note that the semaphore is similar to a condvar in that resumption from a
   *****  semaphore wait signals that lock acquisition may be possible but does not
   *****  guarantee it.
   *****
   *******************************************************************************************/

    class V_API VSpinlock : public VLock {
	DECLARE_FAMILY_MEMBERS (VSpinlock, VLock);

	friend class Claim_<ThisClass>;

    //  Aliases
    public:
	typedef unsigned_int32_count_t	counter_t;
	typedef counter_t::value_t	count_t;
	typedef unsigned int		lockval_t;

    //  Claim
    public:
	typedef Claim_<ThisClass> Claim;

    //  Construction
    public:
	VSpinlock ();

    //  Destruction
    public:
	~VSpinlock ();

    //  Management
    private:
	bool lock (Claim *pClaim, bool bWaiting = true);
	bool lockTry (Claim *pClaim) {
	    return lock (pClaim, false);
	}
	bool unlock (Claim *pClaim);

    //  State
    private:
	lockval_t	m_iLock;
	counter_t	m_cContenders;
	VSemaphore	m_iContentionSemaphore;
    };
}


#endif
