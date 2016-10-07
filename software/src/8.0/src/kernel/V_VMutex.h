#ifndef V_VMutex_Interface
#define V_VMutex_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VLock.h"

#if defined(_WIN32)
#define V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX

#elif defined(__VMS) || defined(sun) || defined(__linux__)
#include <pthread.h>
#define V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX

#endif

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VMutex : public VLock {
	DECLARE_FAMILY_MEMBERS (VMutex, VLock);

	friend class Claim_<ThisClass>;

    //  Claim
    public:
	typedef Claim_<ThisClass> Claim;

    //  Construction
    public:
	VMutex ();

    //  Destruction
    public:
	~VMutex ();

    //  Management
    private:
	bool lock (Claim *pClaim, bool bWait = true);
	bool lockTry (Claim *pClaim) {
	    return lock (pClaim, false);
	}
	bool unlock (Claim *pClaim);

    //  State
    protected:
#   if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
	HANDLE m_hMutex;
#   elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
	pthread_mutex_t	m_iMutex;
#   endif
    };
}


#endif
