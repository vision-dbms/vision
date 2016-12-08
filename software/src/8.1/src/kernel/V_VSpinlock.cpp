/*****  V::VSpinlock Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "V_VSpinlock.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  V::VSpinlock  *****
 *****                *****
 **************************
 **************************/

/************************
 ************************
 *****  Spin Count  *****
 ************************
 ************************/

/*******************************************************************************************
 *****  Currently, all spinlocks suspend the current thread after a small number of lock
 *****  spins.  On uni-processor systems that implement atomic operations using a
 *****  single, uninterruptable instruction (x86, Sparc, etc.), this number can be one.
 *****  For processor architectures that use cancelable memory locks and interruptable
 *****  multi-instruction sequences (Alphas), this number should be small but greater than
 *****  one to guard against spurious uncontended lock acquisition failures.  With these
 *****  processors, failback to a semaphore wait should not be attempted if the spinlock
 *****  was not acquired but no other lock contenders are present.  For these processors,
 *****  this situation is simply a case of an interrupted (and hence canceled) lock
 *****  sequence.
 *****
 *****  On uniprocessor systems or in effectively single-threaded environments (the
 *****  VMS online system), the low spin count strategy is preferred since no other
 *****  thread will ever be running to change the state of the spinlock anyway.  The number
 *****  of lock spins should probably be raised for use on multi-processor architectures
 *****  and in threading systems that support > 1 concurrent thread per process (Windows,
 *****  Linux, Solaris, etc.).
 *******************************************************************************************/

#ifdef __VMS
#define InterruptibleInterlocking
#define EffectivelySingleThreaded
#endif

enum {
#ifdef InterruptibleInterlocking
  NonblockingSpinCount = 1	//  2 tries = 1 default + 1 extra try
#else
  NonblockingSpinCount = 0	//  1 try = 1 default + 0 extra tries
#endif
};

enum {
#ifdef EffectivelySingleThreaded
  BlockingSpinCount = NonblockingSpinCount
#else
  BlockingSpinCount = 100
#endif
};

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VSpinlock::VSpinlock () : m_iLock (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VSpinlock::~VSpinlock () {
}


/*************************
 *************************
 *****  Acquisition  *****
 *************************
 *************************/

bool V::VSpinlock::lock (Claim *pClaim, bool bWaiting) {
    m_cContenders.increment ();		//  become a lock contender

    unsigned int xSpin = bWaiting ? (unsigned int)BlockingSpinCount : (unsigned int)NonblockingSpinCount;
    bool bNotDone = true;
    do {
	do {
	    if (VAtomicMemoryOperations_<sizeof (lockval_t)>::interlockedSetIf (&m_iLock, 0, 1)) {
		attachClaim (pClaim);
		// This barrier insures that all lock protected memory accesses issued by this
		// processor (reads in particular) do not occur before acquisition of the lock:
		VAtomicMemoryOperations::MemoryBarrierAcquire ();
		return true;
	    }
	} while (xSpin-- > 0);
	if (!bWaiting) {
	    // This lock was never acquired so quietly withdraw from contention...
	    m_cContenders.decrement ();
	    return false;
	}
	//  This test is necessary since interruptable atomic update architectures (Alpha)
	//  can fail through to here...
	if (m_cContenders > 1) {
	    //  Stop spinning and suspend, somebody else has the lock...
	    m_iContentionSemaphore.consume ();
	}
	xSpin = BlockingSpinCount;
    } while (bNotDone);
    return false;
}


/*********************
 *********************
 *****  Release  *****
 *********************
 *********************/

bool V::VSpinlock::unlock (Claim *pClaim) {
    if (!detachClaim (pClaim))
	return false;

    // processor (writes in particular) do not occur after release of the lock:
    VAtomicMemoryOperations::MemoryBarrierRelease ();
    m_iLock = 0;

    //  Just released the lock, if contenders exist, allow one of them to run:
    if (!m_cContenders.decrementIsZero ()) {
	m_iContentionSemaphore.produce ();
    }

    return true;
}
