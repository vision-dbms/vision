#ifndef Vca_VGofer_Interface
#define Vca_VGofer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "V_VTwiddler.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTimer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VGofering;

    /**
     * Generic deferred function call system.
     * The basic function of a Gofer is to encapsulate the mechanism by which a particular operation or value is executed or retrieved, so as to facilitate deferred execution.
     *
     * Supports nifty things like metadata (descriptions) and the like, but the basic functionality here is implemented by the onData() and onNeed() virtuals.
     */
    class Vca_API VGofer : public VRolePlayer {
        DECLARE_ABSTRACT_RTTLITE (VGofer, VRolePlayer);

    //  Aliases
    public:
	typedef VGofering Gofering;

    //********************************************************************************************/
    //  RetrySettings
    public:
        /**
         * Specifies settings to be used with Vca::Gofer::Retried instances.
         *
         * This class provides three parameters:
         *  -# A maximum number of retries. If this many retries have continually failed, a hard failure will be declared and retrying will cease.
         *  -# Two delay values. A random value between these two values will be selected as a delay amount (in milliseconds) between each retry. If both delay values are equal, this value is used as an absolute delay with no randomness/choice.
         *
         * After construction/initialization, the getTimer() method should be used to retrieve a timer with the correct specifications/configuration for retries according to the provided parameters.
         */
	class Vca_API RetrySettings {
	    DECLARE_NUCLEAR_FAMILY (RetrySettings);

	//  Aliases
	public:
            /** Convenience definition for timer type. */
	    typedef VTimer timer_t;
            /** Convenience definition for relative time. */
	    typedef VTimer::microseconds_t microseconds_t;
            /** Convenience definition for relative time. */
	    typedef unsigned int milliseconds_t;
            /** Convenience definition for counter type. */
	    typedef unsigned int count_t;
            /** Convenience definition for delay type. */
	    typedef milliseconds_t delay_t;

	//  Construction
	public:
            /**
             * Constructor for RetrySettings expecting parameter values.
             *
             * @param cRetries the maximum number of times to retry after failure.
             * @param sDelay1 retry delay (minimum or maximum).
             * @param sDelay2 secondary retry delay (minimum or maximum).
             */
	    RetrySettings (count_t cRetries, delay_t sDelay1, delay_t sDelay2);

            /**
             * Copy constructor.
             *
             * @param rOther another RetrySettings instance whose values should be copied.
             */
	    RetrySettings (ThisClass const& rOther);

            /**
             * Empty constructor provides default values that disable retries.
             */
	    RetrySettings ();

	//  Destruction
	public:
	    ~RetrySettings ();

	//  Access
	public:
            /** Returns the retry count specified by this RetrySettings instance. */
	    count_t retryCount () const {
		return m_cRetries;
	    }
            /** Returns the first delay parameter specified by this RetrySettings instance. */
	    delay_t delay1 () const {
		return m_sDelay1;
	    }
            /** Returns the second delay parameter specified by this RetrySettings instance. */
	    delay_t delay2 () const {
		return m_sDelay2;
	    }

	//  Update
	public:
            /** @copydoc setTo(ThisClass const& rOther) */
	    ThisClass& operator= (ThisClass const& rOther) {
		setTo (rOther);
		return *this;
	    }

            /**
             * Copy values from another RetrySettings instance.
             *
             * @param rOther another RetrySettings instance whose values should be copied.
             */
	    void setTo (ThisClass const& rOther);

            /**
             * Used to set the parameter values for this RetrySettings instance.
             *
             * @param cRetries the maximum number of times to retry after failure.
             * @param sDelay1 retry delay (minimum or maximum).
             * @param sDelay2 secondary retry delay (minimum or maximum).
             */
	    void setTo (count_t cRetries, delay_t sDelay1, delay_t sDelay2);

	//  Use
            /**
             * Generates and returns a timer with the apprpriate delay (according to parameter values).
             * The generated timer is not started immediately; it is the responsability of the consumer to do so.
             *
             * @param[out] rpTimer the timer reference that should be used to return the newly generated timer.
             * @param pTrigger the trigger that should be invoked when the timer has elapsed.
             */
	    void getTimer (timer_t::Reference& rpTimer, ITrigger* pTrigger) const;

	//  State
	private:
            /** The maximum number of retries parameter. */
	    count_t m_cRetries;
            /** The first delay parameter, one end of the delay range. */
	    delay_t m_sDelay1;
            /** The second delay parameter, another end of the delay range. */
	    delay_t m_sDelay2;
	};

    //********************************************************************************************/
    /**
     * @name Error Rememberance
     * These methods are used to control whether or not errors are treated as final. In the case the errors are not treated as final, re-querying a failed gofer for its value will cause the previous error to be ignored and a re-fetch to be initiated.
     * By default, all gofer errors are not final (retrying enabled), unless the environment variable @c VcaGofersKeepErrors is set to @c 1.
     */
    //@{
    public:
        /**
         * Used to enable or disable error rememberance for all gofers.
         *
         * @param bKeepingErrors provide true if error rememberance should be enabled, false otherwise.
         */
        static void SetKeepingErrorsTo (bool bKeepingErrors) {
            g_bKeepingErrors = bKeepingErrors;
        }

        /** Enables error rememberance for all gofers. */
        static void SetKeepingErrors () {
            SetKeepingErrorsTo (true);
        }

        /** Disables error rememberance for all gofers. */
        static void ClearKeepingErrors () {
            SetKeepingErrorsTo (false);
        }

        /**
         * Indicates whether or not error rememberance is enabled for all gofers.
         *
         * @return true if error rememberance is enabled, false otherwise.
         */
        static bool KeepingErrors () {
            return g_bKeepingErrors;
        }
    private:
        /** State of error rememberance for all gofers. */
        static bool g_bKeepingErrors;
    //@}

    /**
     * @name Tracing
     * These members are used to control whether or not tracing will be done on gofer operations. Subclasses should implement their own tracing calls conditioned on TracingGofers() returning @c true.
     * By default, all gofer tracing is disabled, unless specified by the environment variable @c TracingVcaGofers is set to @c 1.
     */
    //@{
    public:
        /**
         * Used to determine if gofer tracing is enabled.
         *
         * @return true if gofer tracing is enabled, false otherwise.
         */
        static bool TracingGofers () {
            return g_bTracingGofers;
        }
    private:
        /** State of gofer tracing for all gofers. */
        static bool g_bTracingGofers;
    //@}

    //  Construction
    protected:
        /**
         * Constructor initializes state with one dependency by default.
         */
        VGofer ();

    //  Destruction
    protected:
        /** Empty destructor. */
        ~VGofer ();

    //  Access/Query
    public:
        /**
         * Used to determine if this gofer has been triggered (via onNeed()).
         *
         * @return true if this gofer has been triggered, false otherwise.
         */
        bool triggered () const {
            return m_bTriggered;
        }

        /**
         * Used to retrieve the human-readable description of this gofer.
         * Delegates to getDescription_()
         *
         * @param[out] rDescription the reference that should be used to return the description of this gofer.
         */
        void getDescription (VString &rDescription) const {
            getDescription_(rDescription);
        }
    private:
        /**
         * Delegate for getDescription().
         * Intended to be implemented by subclasses in order to provide a custom description for their gofer.
         * By default, returns the run-time type name obtained via rttName().
         */
        virtual void getDescription_(VString &rDescription) const;

    //  Activity
    private:
	virtual Gofering* newGofering_();
    public:
	Gofering* newGofering ();
    protected:
	void onGoferingError (IError* pInterface, VString const& rMessage);
	void onGoferingFailure (IError* pInterface, VString const& rMessage);
	void onGoferingSuccess ();

    //  Execution
    public:
        /**
         * Called when one of this Gofer's dependencies is being evaluated.
         * Increments our dependency count.
         */
        void suspend ();

        /**
         * Called when one of this Gofer's dependencies is ready.
         * Decrements our dependency count.
         * If the dependency count drops to zero post-decrement, onData() is called.
         */
        void resume ();

        /**
         * Method to reinitialize a gofer, for example, after an error is encountered during its onNeed phase.
         *  When a gofer is reset, three things happen:
         *   -# The gofer is marked as unevaluated (<code>m_bTriggered.clearIfSet ()</code>).
         *   -# The gofer has its suspension count reset to its initial value of one.
         *   -# The gofer needs to do whatever other prep work needs to happen to reinitialize itself (onReset()).
         */
        void resetGofer ();

    //  Triggers and Callbacks
    private:
        /**
         * Called when this Gofer has all of its prerequisite data needed to compute its result or produce its effect.
         * Subclasses @e must provide a definition for this method.
         */
        virtual void onData () = 0;
    protected:
        //  VRolePlayer::OnError_ override...
        virtual void OnError_(IError *pInterface, VString const &rMessage);
    public:
	/**
	 * Called to notify this gofer that an error occurred computing it's result or one of its prerequisites.
	 * @param pInterface an interface for the error. Can be null.
	 * @param rMessage a human readable message describing the error.
	 */
        virtual void onError (IError *pInterface, VString const &rMessage) = 0;

    protected:
        /**
         * Called when this Gofer's effect/result is needed by the consumer.
         * This is actually responsible for kicking off the desired process (data acquisition, etc.). This process begins by acquiring any prerequisite data needed by this Gofer. Once that prerequisite data is available, the onData() member of this class will be called. In any subclass that involves prerequisite information, this member must be overridden so that all child-class-specific dependencies/parameters are satisfied, @e followed by a call to BaseClass::onNeed().
         *
         * Examples of subclasses that redefine this method:
         *  - Vca::Gofer::Named::onNeed()
         */
        virtual void onNeed ();

        /**
         * Called when this gofer has been reset.
         *
         * Intended to be implemented by subclasses in order to perform any additional operations that need to be done before a subsequent gofer query is made.
         * Default implementation does nothing.
         */
        virtual void onReset ();

        /**
         * Internal convenience wrapper for onNeed().
         * Used by Vca::VGoferInstance (via Vca::VInstanceSource).
         * @internal
         */
        void onValueNeeded ();
        /**
         * Internal convenience wrapper for resetGofer().
         * Used by Vca::VGoferInstance (via Vca::VInstanceSource).
         * @internal
         */
        void onResetNeeded ();

    //  Validation
    protected:
        /**
         * Used to determine if a given error should be remembered or ignored.
         *
         * @param pInterface the IError for the raised error.
         * @param rMessage the error message.
         * @return true if the error should be remembered, false otherwise.
         */
        bool validatesError (IError *pInterface, VString const &rMessage) const;

    //  State
    private:
	VReference<Gofering> m_pGofering;
        /**
         * Dependency count for dependencies with deferred execution.
         * Starts off at one, representing this Gofer's dependency on the operation it should perform.
         */
        counter_t m_cDependencies;

        /** Indicates whether or not this gofer has ever been triggered (via call to onNeed()). */
        V::VTwiddler m_bTriggered;
    };
}


#endif
