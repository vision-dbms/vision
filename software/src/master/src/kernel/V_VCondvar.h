/**
 * @file
 * Declares V::VCondvar class.
 */
#ifndef V_VCondvar_Interface
#define V_VCondvar_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VMutex.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class VTime;

    /**
     * Lock with wait mechanism.
     */
    class V_API VCondvar : public VMutex {
	DECLARE_FAMILY_MEMBERS (VCondvar, VMutex);

    //  Construction
    public:
        /** Default constructor takes no arguments. */
	VCondvar ();

    //  Destruction
    public:
        /** Destructor cleans up any existing lock state. */
	~VCondvar ();

    /// @name Operation
    //@{
    public:
        /**
         * Signals this lock, triggering return of any wait() or waitUntil() calls.
         */
	void signal ();

        /**
         * Waits on this lock until it has been signaled.
         *
         * @return true on success, false on error.
         */
	bool wait ();

        /**
         * Waits on this lock until it has been signaled, or until a given timeout has been reached, whichever is first.
         *
         * @param rLimit the (absolute) time that this method should be allowed to wait before it returns WaitStatus_Timeout.
         * @return the appropriate WaitStatus according to the reason that this call returned.
         */
	WaitStatus waitUntil (VTime const& rLimit);
    //@}

    /// @name State
    //@{
    private:
#   if defined(V_MUTEX_IMPLEMENTED_USING_WIN32_MUTEX)
	HANDLE m_hEvent;
#   elif defined(V_MUTEX_IMPLEMENTED_USING_POSIX_MUTEX)
	pthread_cond_t m_iCondvar;
#   endif
    //@}
    };
}


#endif
