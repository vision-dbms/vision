#ifndef Vca_VTimer_Interface
#define Vca_VTimer_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "V_VTime.h"
#include "V_VTwiddler.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_ITrigger.h"

#include "Vca_VDeviceManager.h"


/*************************
 *****  Definitions  *****
 *************************/

/******************************************************************************
 * VTimer object is constructed with an ITrigger,a microsecond time
 * value after which the ITrigger will be triggered and a boolean indicating
 * whether it is repeatable or not. VTimer could be started/stopped resulting
 * in the timer being in Running/Stopped state. Upon creation it is in
 * Init state, explicit start () needs to be called.
 ******************************************************************************/

namespace Vca {
    /**
     * Generic timer class supporting the automatic triggering of events (via Vca::ITrigger) after a given delay.
     *
     * By the way that Vca::VCohort has been implemented with respect to timer handling, timers do not represent an interrupt, and thus are not guaranteed to fire precisely after a given delay. Rather, they are implemented so that they fire sometime after the given delay, subject to event loop lag/congestion.
     */
    class Vca_API VTimer : public VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (VTimer, VRolePlayer);

        friend class VCohort;

    //  Aliases
    public:
        /** Convenience definition for absolute time. */
        typedef V::VTime VTime;

        /** Convenience definition for relative time. */
        typedef U64 microseconds_t;

    //  Constants
    private:
        /** The default purpose string, used when a purpose has not been provided. */
        static VString const g_sDefaultPurpose;

    //  Construction
    public:
        /**
         * Constructor for VTimer expecting cohort, purpose, trigger and delay range.
         *
         * @copydetail VTimer(VCohort*,ITrigger*,microseconds_t,microseconds_t,bool)
         * @param rPurpose a human-readable descriptive string about this timer's purpose/use.
         */
        VTimer (VCohort *pCohort, VString const &rPurpose, ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting cohort, purpose, trigger and delay.
         *
         * @copydetail VTimer(VCohort*,ITrigger*,microseconds_t,bool)
         * @param rPurpose a human-readable descriptive string about this timer's purpose/use.
         */
        VTimer (VCohort *pCohort, VString const &rPurpose, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting cohort, trigger and delay range.
         *
         * @copydetail VTimer(ITrigger*,microseconds_t,microseconds_t,bool)
         * @param pCohort the cohort to which this timer should belong.
         */
        VTimer (VCohort *pCohort, ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting cohort, trigger and delay.
         *
         * @copydetail VTimer(ITrigger*,microseconds_t,bool)
         * @param pCohort the cohort to which this timer should belong.
         */
        VTimer (VCohort *pCohort, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting purpose, trigger and delay range.
         * @copydetail VTimer(ITrigger*,microseconds_t,microseconds_t,bool)
         * @param rPurpose a human-readable descriptive string about this timer's purpose/use.
         */
        VTimer (VString const &rPurpose, ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting purpose, trigger and delay.
         *
         * @copydetail VTimer(ITrigger*,microseconds_t,bool)
         * @param rPurpose a human-readable descriptive string about this timer's purpose/use.
         */
        VTimer (VString const &rPurpose, ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting trigger and delay range.
         *
         * @param pTrigger the trigger that should be fired when this timer has elapsed.
         * @param sDelay1 one end of the delay range to use when calculating how long the actual timer delay should be.
         * @param sDelay2 another end of the delay range to use when calculating how long the actual timer delay should be.
         * @param bRepeating true if this timer should automatically restart after firing.
         */
        VTimer (ITrigger *pTrigger, microseconds_t sDelay1, microseconds_t sDelay2, bool bRepeating = false);

        /**
         * Constructor for VTimer expecting trigger and delay.
         *
         * @param pTrigger the trigger that should be fired when this timer has elapsed.
         * @param sDelay the amount of time that should elapse between starting this timer and it firing.
         * @param bRepeating true if this timer should automatically restart after firing.
         */
        VTimer (ITrigger *pTrigger, microseconds_t sDelay, bool bRepeating = false);

    //  Destruction
    private:
        ~VTimer ();

    //  Access
    public:
        /** Returns this timer's purpose. */
        VString const& purpose () const {
            return m_iPurpose;
        }

        /** Returns this timer's start time. */
        VTime const &startTime () const {
            return m_iStartTime;
        }

        /** Returns this timer's expiration time. */
        VTime const &expirationTime () const {
            return m_iExpirationTime;
        }

        /** Returns true if this timer will automatically reset after firing. */
        bool isRepeating () const {
            return m_bRepeating;
        }

    //  Query
    public:
        /**
         * Returns the minimum of (a) the amount of time until this timer will expire and (b) a provided timeout.
         *
         * @param sTimeout the timeout against which the time that this timer will expire should be compared, expressed in milliseconds.
         * @return the minimum of (a) the amount of time until this timer will expire and (b) a provided timeout, expressed in milliseconds.
         */
        size_t adjustedTimeout (size_t sTimeout) const;

        /**
         * Returns true if this timer expires before another timer.
         *
         * @param pOther the timer whose expiration time this timer's expiration time should be compared to.
         * @return true if this timer expires before @c pOther does.
         */
        bool expiresBefore (VTimer const *pOther) const {
            return m_iExpirationTime < pOther->expirationTime ();
        }

        /**
         * Returns true if this timer expires before a given time.
         *
         * @param rTime the time that this timer's expiration time should be compared to.
         * @return true if this timer expires before @c rTime.
         */
        bool expiresBefore (VTime const &rTime) const {
            return m_iExpirationTime < rTime;
        }

        /**
         * Returns true if this timer expires after another timer.
         *
         * @param pOther the timer whose expiration time this timer's expiration time should be compared to.
         * @return true if this timer expires after @c pOther does.
         */
        bool expiresAfter (VTimer const *pOther) const {
            return m_iExpirationTime > pOther->expirationTime ();
        }

        /**
         * Returns true if this timer expires after a given time.
         *
         * @param rTime the time that this timer's expiration time should be compared to.
         * @return true if this timer expires after @c rTime.
         */
        bool expiresAfter (VTime const &rTime) const {
            return m_iExpirationTime > rTime;
        }

        /** Returns true if this timer has expired. */
        bool hasExpired () const {
            return m_iExpirationTime.isInThePast ();
        }

        /** Returns true if this timer is running. */
        bool isRunning () const {
            return m_bRunning;
        }

        /** Returns true if this timer is @e not running. */
        bool isntRunning () const {
            return !m_bRunning;
        }

        /** Returns the amount of time until this timer will expire. */
        microseconds_t timeToExpire () const;

    //  Update
    public:
        /**
         * Sets the purpose of this timer.
         *
         * @param rPurpose the new purpose for this timer.
         */
        void setPurposeTo (VString const& rPurpose) {
            m_iPurpose.setTo (rPurpose);
        }

        /**
         * Sets the delay range of this timer.
         * Does not affect an already running timer.
         *
         * @param sDelay1 one end of the new delay range.
         * @param sDelay2 another end of the new delay range.
         */
        void setDelays (microseconds_t sDelay1, microseconds_t sDelay2) {
            m_sDelay1 = sDelay1;
            m_sDelay2 = sDelay2;
        }

        /**
         * Sets the delay of this timer.
         * Does not affect an already running timer.
         *
         * @param sDelay the new delay for this timer.
         */
        void setDelay (microseconds_t sDelay) {
            setDelays (sDelay, sDelay);
        }

        /**
         * Sets the repeat option for this timer.
         *
         * @param bRepeating if true, this timer will be set to restart automatically after firing; if false, this timer will be set to @e not automatically restart after firing.
         */
        void setRepeating (bool bRepeating) {
            m_bRepeating = bRepeating;
        }

    /// @name Control
    //@{
    private:
	/**
	 * Internal routine to start this timer if it is not currently running.
	 * @param bRequested true if this request came from the user, false if an internal bookkeeping request.
	 */
	void start (bool bRequested);

	/**
	 * Internal routine to stop this timer if it is currently running.
	 * @param bRequested true if this request came from the user, false if an internal bookkeeping request.
	 */
	void stop  (bool bRequested);
    public:
        /**
         * Starts this timer if it is not already running, does nothing if it already is running.
         * Adds this timer to its cohort's timer list.
         */
        void start () {
	    start (true);
	}

        /**
         * Stops this timer if it is running.
         * Removes this timer from its cohort's timer list.
         */
        void stop () {
	    stop (true);
	}

        /** Restarts this timer if it is running, starts it if it isn't running. */
        void restart ();

        /**
         * Stops this timer if it is running, and also releases the reference to its trigger.
         */
        void cancel ();
    //@}

    //  Linkage
    private:
        /**
         * Attaches this timer to a list of timers, retaining the sort order of the list (by expiration time).
         */
        bool attach (Reference& rpListHead);
        /**
         * Detaches this timer from a list of timers.
         */
        bool detach (Reference& rpListHead);

    //  Triggering
    public:
        /**
         * Fires this timer's trigger as necessary. Does nothing if this timer has not yet expired.
         */
        bool triggerIfExpired ();

    //  State
    private:
        /** Human-readable descriptive string for use with logging and debugging tools. */
        VString                 m_iPurpose;
        /** The trigger to fire when this timer has elapsed. */
        ITrigger::Reference     m_pTrigger;
        /** Actual (absolute) time that this timer was started. */
        VTime                   m_iStartTime;
        /** Actual (absolute) time that this timer is due to expire. */
        VTime                   m_iExpirationTime;
        /**
         * Doubly-linked list support connecting this timer to other active timers from the same cohort.
         * This list is sorted by the order of expiration time, with the timer to expire first at the head of the list.
         */
        Reference               m_pSuccessor;
        /** @copydoc m_pSuccessor */
        Reference               m_pPredecessor;
        /** The first delay parameter used to define a delay range. */
        microseconds_t          m_sDelay1;
        /** The second delay parameter used to define a delay range. */
        microseconds_t          m_sDelay2;
        /** The actual delay in effect for this timer run. */
        microseconds_t          m_sDelay;
        /** True if this timer is ticking. */
        V::VTwiddler            m_bRunning;
	/** True if this timer should be running because its user wants it to be running. */
	bool			m_bExpected;
        /** True if this timer should automatically restart after firing. */
        bool                    m_bRepeating;
    };
}


#endif
