#ifndef Vca_VDeviceManager_Interface
#define Vca_VDeviceManager_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace OS {
	class DeviceManager;
	class ManagedDevice;
    }

    /**
     * Abstract base class for device managers. Platform-specific concrete subclasses are implemented in Vca::VDeviceFactory.
     */
    class VDeviceManager : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VDeviceManager, VRolePlayer);

	friend class OS::ManagedDevice;

    //  Aliases
    public:
	typedef OS::DeviceManager DeviceManager;

    //  Construction
    protected:
	VDeviceManager (VCohort *pCohort);

    //  Destruction
    protected:
	~VDeviceManager ();

    //  Cohort Management
    protected:
	bool joinCohort ();
	bool exitCohort ();
    public:
	static bool Supply (Reference &rpInstance, VCohort *pCohort);
	static bool Supply (VReference<DeviceManager> &rpInstance, VCohort *pCohort);

    //  Event Processing
    private:
        /**
         * Implemented by subclasses in order to interrupt this VDeviceManager's event wait.
         *
         * @return true on success, false otherwise.
         */
	virtual bool postInterrupt_() = 0;

        /**
         * Implemented by subclasses in order to attempt event processing.
         *
         * @param[in] sTimeout the maximum wait timeout for processing the I/O event.
         * @return true if an event was processed, false otherwise.
         */
	virtual bool processEvent_(size_t sTimeout) = 0;
    public:
        /**
         * Interrupts this VDeviceManager's event wait.
         * Delegates to postInterrupt_()
         *
         * @return true on success, false otherwise.
         */
	bool postInterrupt () {
	    return postInterrupt_();
	}

        /**
         * Invoked in order to attempt processing of an I/O event.
         * Delegates to processEvent_()
         *
         * @param[in] sTimeout the maximum wait timeout for processing the I/O event.
         * @return true if an event was processed, false otherwise.
         */
	bool processEvent (size_t sTimeout) {
	    return processEvent_(sTimeout);
	}

        /**
         * Invoked to attempt to process I/O events.
         *
         * @param[in] sTimeout the maximum wait timeout for processing the I/O event.
         * @param[out] rbEventsHandled true if events were handled, false if no events were handled.
         * @return true if this VDeviceManager still has outstanding I/O operations across any of its devices after attempting to process an event, false otherwise.
         */
	bool processEvents (size_t sTimeout, bool &rbEventsHandled);

    //  Use Management
    protected:
        /** Increments the use counter. */
	void incrementUseCount ();
        /** Decrements the use counter. */
	void decrementUseCount () {
	    m_cUses.decrement ();
	}
    public:
        /**
         * Used to determine if there are outstanding (or even just mundane) I/O operations across all devices managed by this VDeviceManager.
         *
         * @see m_cUses
         * @return true if there are, false if there aren't.
         */
	bool hasUses () const {
	    return m_cUses > 0;
	}
        /**
         * Returns the number of in-progress I/O operations across all devices manged by this VDeviceManager.
         */
	count_t useCount () const {
	    return m_cUses;
	}

    //  State
    private:
        /** The number of in-progress I/O operations across all devices manged by this VDeviceManager. */
	counter_t m_cUses;
    };
}


#endif
