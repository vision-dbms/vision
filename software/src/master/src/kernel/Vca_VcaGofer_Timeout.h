#ifndef Vca_VcaGofer_Timeout_Interface
#define Vca_VcaGofer_Timeout_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTimer.h"

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
         * Generic gofer for wrapping another gofer with a timeout.
         *
         * @tparam Interface_T the interface type being requested.
         */
	template <typename Interface_T> class Timeout : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Timeout<Interface_T>, VGoferInterface<Interface_T>);

	//  Aliases
	public:

	//  Construction
	public:
            /**
             * Constructor for retried gofers expecting (a gofer for) the requested interface type as well as retry settings.
             *
             * @param pResultSource the (gofer for the) requested interface.
             * @param sTimeout the timeout in microseconds.
             */
	    template <typename result_source_t> Timeout (
		result_source_t pResultSource, U64 sTimeout
	    ) : m_pITrigger (this), m_pResultSource (pResultSource), m_sTimeout (sTimeout) {
	    }

	//  Destruction
	private:
	    ~Timeout () {
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
            /** The Vca::ITrigger role for this Timeout gofer. */
	    Vca::VRole<ThisClass,ITrigger> m_pITrigger;
	public:
            /** Retrieves the Vca::ITrigger role for this Timeout gofer. */
	    void getRole (ITrigger::Reference& rpRole) {
		m_pITrigger.getRole (rpRole);
	    }
	public:
            /**
             * Implements Vca::ITrigger::Process(); throws away previous results (via resetGofer()) and starts over (via onNeed()).
             */
	    void Process (ITrigger* pRole) {
		stopTimer ();
		BaseClass::onError (0, "Timeout");
	    }
        //@}

	//  Callbacks and Triggers
	private:
            /**
             * Delegates to the result source.
             *
             * @see m_pResultSource
             */
	    void onNeed () OVERRIDE {
		startTimer ();
		m_pResultSource.materializeFor (this);
		BaseClass::onNeed ();
	    }

            /** Stops retries. */
	    void onData () OVERRIDE {
		stopTimer ();
		BaseClass::setTo (m_pResultSource);
	    }

            /**
             * Initiates retry as necessary, stops trying when retry limit has been reached.
             */
	    void onError (IError *pInterface, VString const &rMessage) OVERRIDE {
		stopTimer ();
		BaseClass::onError (pInterface, rMessage);
	    }

	private:
            /** Starts automatic retries by setting up the retry delay timer. */
	    void startTimer () {
                // Retrieve/create our timer.
		if (m_pDelayTimer.isNil ()) {
		    ITrigger::Reference pTrigger;
		    getRole (pTrigger);
		    m_pDelayTimer.setTo (new VTimer ("Gofer", pTrigger, m_sTimeout));
		}
                // Start our timer.
		m_pDelayTimer->start ();
	    }

            /** Stops automatic retries. */
	    void stopTimer () {
		if (m_pDelayTimer) {
		    m_pDelayTimer->stop ();
		    m_pDelayTimer.clear ();
		}
	    }

	//  State
	private:
            /** The underlying gofer that we'll use to retrieve our result. */
	    VInstanceOfInterface<Interface_T>	m_pResultSource;
            /** The timeout in microsecondss. */
	    U64 const				m_sTimeout;
            /** Retry delay timer. */
	    VTimer::Reference			m_pDelayTimer;
	};
    }
}


#endif /* Vca_VcaGofer_Timeout_Interface */
