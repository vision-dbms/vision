#ifndef V_VLock_Interface
#define V_VLock_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VSynchronizationObject.h"

#include "V_VAggregatePointer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VLock : public VSynchronizationObject {
	DECLARE_FAMILY_MEMBERS (VLock, VSynchronizationObject);

    //  Claim
    public:
	class Claim {
	    DECLARE_FAMILY_MEMBERS (Claim, void);

	//  Construction
	protected:
	    Claim () : m_bHeld (false) {
	    }

	//  Destruction
	protected:
	    ~Claim () {
	    }

	//  Query
	private:
	    virtual bool names_(VLock const *pLock) const = 0;
	public:
	    bool held () const {
		return m_bHeld;
	    }
	    bool names (VLock const *pLock) const {
		return names_(pLock);
	    }

	//  Use
	protected:
	    bool onAcquire (bool bAcquired) {
		if (bAcquired)
		    m_bHeld = true;
		return bAcquired;
	    }
	    bool onRelease (bool bReleased) {
		if (bReleased)
		    m_bHeld = false;
		return bReleased;
	    }

	//  State
	private:
	    bool m_bHeld;
	};

    //  Claim_
    public:
	template <class LockClass> class Claim_ : public Claim {
	    DECLARE_FAMILY_MEMBERS (Claim_<LockClass>, Claim);

	//  Aliases
	public:
	    typedef LockClass lock_t;

	//  Construction
	public:
	    Claim_(lock_t &rLock, bool bWait = true) : m_rLock (rLock) {
		acquire (bWait);
	    }

	//  Destruction
	public:
	    ~Claim_() {
		release ();
	    }

	//  Query
	private:
	    virtual bool names_(VLock const *pLock) const {
		return names (pLock);
	    }
	public:
	    bool names (VLock const *pLock) const {
		return &m_rLock == pLock;
	    }

	//  Use
	public:
	    bool acquire (bool bWait = true) {
		return onAcquire (m_rLock.lock (this, bWait));
	    }
	    bool acquireTry () {
		return onAcquire (m_rLock.lockTry (this));
	    }
	    bool release () {
		return onRelease (m_rLock.unlock (this));
	    }

	//  State
	private:
	    lock_t &m_rLock;
	};

    //  Construction
    protected:
	VLock () : m_cClaims (0) {
	}

    //  Destruction
    protected:
	~VLock () {
	}

    //  Query
    public:
	bool heldBy (Claim *pClaim) const {
	    return pClaim->names (this) && pClaim->held ();
	}
	bool locked () const {
//	    return m_pClaim.isntNil ();
	    return m_cClaims > 0;
	}
	bool unlocked () const {
//	    return m_pClaim.isNil ();
	    return m_cClaims == 0;
	}

    //  Update
    protected:
	bool attachClaim (Claim *pClaim) {
//	    return m_pClaim.interlockedSetIfNil (pClaim);
	    m_cClaims++;
	    return true;
	}
	bool detachClaim (Claim *pClaim) {
//	    return m_pClaim.interlockedClearIf (pClaim);
	    if (!heldBy (pClaim))
		return false;
	    m_cClaims--;
	    return true;
	}

    //  State
    private:
	unsigned int m_cClaims;
    };
}


#endif
