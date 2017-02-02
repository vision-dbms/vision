#ifndef V_VNullock_Interface
#define V_VNullock_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VLock.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicMemoryOperations_.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VNullock : public VLock {
	DECLARE_FAMILY_MEMBERS (VNullock, VLock);

	friend class Claim_<ThisClass>;

    //  Claim
    public:
	typedef Claim_<ThisClass> Claim;

    //  Construction
    public:
	VNullock () : m_bLocked (false) {
	}

    //  Destruction
    public:
	~VNullock () {
	}

    //  Query
    public:
	bool heldBy (Claim *pClaim) const {
	    return m_bLocked;
	}
	bool locked () const {
	    return m_bLocked;
	}
	bool unlocked () const {
	    return !m_bLocked;
	}

    //  Use
    public:
	bool lock (Claim *pClaim, bool bWait = true) {
	    m_bLocked = true;
	    return true;
	}
	bool lockTry (Claim *pClaim) {
	    return lock (pClaim, false);
	}
	bool unlock (Claim *pClaim) {
	    m_bLocked = false;
	    return true;
	}

    //  State
    private:
	bool m_bLocked;
    };
}


#endif
