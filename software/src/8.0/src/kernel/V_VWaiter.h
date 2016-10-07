#ifndef V_VWaiter_Interface
#define V_VWaiter_Interface

#include "V_VCondvar.h"
#include "V_VTwiddler.h"
#include "V_VTime.h"

namespace V {
    class V_API VWaiter {
        DECLARE_NUCLEAR_FAMILY (VWaiter);

    // Construction
    protected:
        /** Default constructor. */
        VWaiter() {}

    // Destruction
    protected:
        /** Empty destructor. */
        ~VWaiter() {}

    // Exposed functionality.
    public:
        /**
         * Initiates waiting until a given timeout. Subclasses must call signal() in order for waiting to cease.
         * If a result already exists, waiting is skipped entirely and @c true is returned.
         *
         * @return true if a result exists, false otherwise (timeout or error).
         * @see V::VCondvar::waitUntil()
         */
        bool waitUntil (V::VTime const & cLimit);

        /**
         * Initiates waiting. Subclasses must call signal() in order for waiting to cease.
         * If a result already exists, waiting is skipped entirely and @c true is returned.
         *
         * @return true if a result exists, false otherwise (error).
         * @see V::VCondvar::wait()
         */
        bool wait ();

        /**
         * Used to check whether or not a result exists.
         *
         * @return true if a result exists, false otherwise.
         */
        bool hasResult ();

    // Subclass call-ins.
    protected:
        /**
         * Signals the lock; intended to be called by subclasses when the lock should be released and waiting should cease (e.g. upon receipt of result).
         *
         * @param[in] bWithResult true if a result now exists, false otherwise (error).
         */
        void signal (bool bWithResult = true);

        /** Resets the history; intended to be called by subclasses when the history should be cleared. and new incoming wait requests should not return immediately. */
        void reset ();

    private:
        /** Wait lock. */
        V::VCondvar m_pWaitLock;
        /** History. */
        V::VTwiddler m_bHasResult;
    };
}

#endif

