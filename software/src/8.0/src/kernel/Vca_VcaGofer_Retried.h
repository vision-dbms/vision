#ifndef Vca_VcaGofer_Retried_Interface
#define Vca_VcaGofer_Retried_Interface


/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VRTTI.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Gofer {

    /*********************************************************************************************
     *  Can be generalized to any object, not just interfaces, provided interfaces continue to
     *  inherit from VGoferInterface<T> (non-interfaces must inherit from VGoferInstance<T>).
     *********************************************************************************************/

        /**
         * Generic gofer for wrapping another gofer with (delayed) retry functionality.
         *
         * Intended to be used for retrieving interfaces only, though in theory this class could easily be generalized to handle any return data type.
         *
         * If this gofer receives an error from its delegate, a timer is started with a particular retry delay. When said timer expires, a retry is attempted using reset() followed by onNeed(). This process is repeated as necessary until either (a) a result is successfully returned via onData() or (b) the maximum retry limit has been reached.
         *
         * @tparam Interface_T the interface type being requested.
         */
	template <typename Interface_T> class Retried : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Retried<Interface_T>, VGoferInterface<Interface_T>);

	//  Aliases
	public:
	    typedef VGofer::RetrySettings RetrySettings;
	    typedef RetrySettings::timer_t timer_t;
	    typedef RetrySettings::count_t count_t;
	    typedef RetrySettings::delay_t delay_t;

	//  Construction
	public:
            /**
             * Constructor for retried gofers expecting (a gofer for) the requested interface type as well as retry settings.
             *
             * @param pResultSource the (gofer for the) requested interface.
             * @param rRetrySettings the retry settings for the newly created Retried gofer instance.
             */
	    template <typename result_source_t> Retried (
		result_source_t pResultSource, RetrySettings const& rRetrySettings
	    ) : m_pITrigger (this), m_pResultSource (pResultSource), m_iRetrySettings (rRetrySettings), m_cRetriesRemaining (rRetrySettings.retryCount ()) {
	    }

	//  Destruction
	private:
	    ~Retried () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	/**
         * @name ITrigger Implementation
         * The ITrigger interface is used by the Vca::VTimer created to handle retry delays.
         */
        //@{
	private:
            /** The Vca::ITrigger role for this Retried gofer. */
	    Vca::VRole<ThisClass,ITrigger> m_pITrigger;
	public:
            /** Retrieves the Vca::ITrigger role for this Retried gofer. */
	    void getRole (ITrigger::Reference& rpRole) {
		m_pITrigger.getRole (rpRole);
	    }
	public:
            /**
             * Implements Vca::ITrigger::Process(); throws away previous results (via resetGofer()) and starts over (via onNeed()).
             */
	    void Process (ITrigger* pRole) {
                // Throw away.
		BaseClass::resetGofer (); // We have to be explicit here because methods from superclasses specified by template parameter values are not imported into the subclass' lexical scope.
                // Start over.
		onNeed ();
	    }
        //@}

	//  Callbacks and Triggers
	private:
            /**
             * Delegates to the result source.
             *
             * @see m_pResultSource
             */
	    void onNeed () {
		m_pResultSource.materializeFor (this);
		BaseClass::onNeed ();
	    }

            /** Stops retries. */
	    void onData () {
		stopTrying ();
		BaseClass::setTo (m_pResultSource);
	    }

            /**
             * Initiates retry as necessary, stops trying when retry limit has been reached.
             */
	    void onError (IError *pInterface, VString const &rMessage) {
		if (m_cRetriesRemaining-- > 0)
		    keepTrying ();
		else {
		    stopTrying ();
		    BaseClass::onError (pInterface, rMessage);
		}
	    }

	protected:
            /** Cancels retry delay timer. */
	    void onReset () { // onGoferReset
		if (m_pDelayTimer)
		    m_pDelayTimer->stop ();
		BaseClass::onReset ();
	    }


	private:
            /** Starts automatic retries by setting up the retry delay timer. */
	    void keepTrying () {
                // Retrieve/create our timer.
		if (m_pDelayTimer.isNil ()) {
		    ITrigger::Reference pTrigger;
		    getRole (pTrigger);
		    m_iRetrySettings.getTimer (m_pDelayTimer, pTrigger);
		}
                // Start our timer.
		m_pDelayTimer->start ();
	    }

            /** Stops automatic retries. */
	    void stopTrying () {
		m_cRetriesRemaining = m_iRetrySettings.retryCount ();
		if (m_pDelayTimer) {
		    m_pDelayTimer->stop ();
		    m_pDelayTimer.clear ();
		}
	    }

	//  State
	private:
            /** The underlying gofer that we'll use to retrieve our result. */
	    VInstanceOfInterface<Interface_T>	m_pResultSource;
            /** Settings for retries (including delays). */
	    RetrySettings		const	m_iRetrySettings;
            /** Retry delay timer. */
	    timer_t::Reference			m_pDelayTimer;
            /** The number of retries that remain until a hard failure state has been reached. */
	    count_t				m_cRetriesRemaining;

	};
    }
}


#endif /* Vca_VcaGofer_Retried_Interface */
