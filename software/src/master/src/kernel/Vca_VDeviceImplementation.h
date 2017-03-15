#ifndef Vca_VDeviceImplementation_Interface
#define Vca_VDeviceImplementation_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAddin.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VDevice.h"
#include "VkStatus.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Abstract base class for all device implementations.
     *
     * This is intended to be subclassed to create suitable template parameters for abstract device implementations deriving from VDevice_.
     * @see VDevice_
     * @see VBSProducerConsumerDevice_
     * @see VListeningDevice_
     *
     * For example, if one were to create a new device that was capable of writes as well as reads, one would subclass VDeviceImplementation implementing the calls made from VBSProducerConsumerDevice_ and then use said subclass as a template parameter for VBSProducerConsumerDevice_.
     *
     * @todo Finish documenting trivial-ish methods.
     */
    class Vca_API VDeviceImplementation : public V::VAddin {
	DECLARE_FAMILY_MEMBERS (VDeviceImplementation, V::VAddin);

    //  Aliases
	typedef VDeviceBSReadArea	BSReadArea;
	typedef VDeviceBSWriteArea	BSWriteArea;
        typedef VReference<ThisClass>   DeviceImplReference;

    //  AbstractUse
    public:
	class AbstractUse : public V::VAddin {
	    DECLARE_FAMILY_MEMBERS (AbstractUse, V::VAddin);

	//  Construction
	protected:
	    AbstractUse (VReferenceable *pContainer) : BaseClass (pContainer) {
	    }

	//  Destruction
	protected:
	    ~AbstractUse () {
	    }

	//  Access
	private:
	    virtual VDevice::Use *use_() = 0;
	    virtual VDevice::User *user_() = 0;
	public:
	    VDevice::Use *use () {
		return use_();
	    }
	    VDevice::User *user () {
		return user_();
	    }

	//  Completion
	private:
	    virtual bool onData_(size_t sTransfer, bool bKeep) = 0;
	    virtual bool onData_(VDevice *pDevice, bool bKeep) = 0;
	    virtual bool onError_(VkStatus const &rStatus, bool bKeep) = 0;
	protected:
	    bool onData (size_t sTransfer, bool bKeep = false) {
		return onData_(sTransfer, bKeep);
	    }
	    bool onData (VDevice *pDevice, bool bKeep = false) {
		return onData_(pDevice, bKeep);
	    }
	    bool onEnd (bool bKeep = false) {
		VkStatus iStatus;
		iStatus.MakeClosedStatus ();
		return onError (iStatus, bKeep);
	    }
	    bool onError (VkStatus const &rStatus, bool bKeep = false) {
		return onError_(rStatus, bKeep);
	    }

	//  Callback
	private:
	    virtual void triggerUser_() = 0;
	protected:
	    void triggerUser () {
		triggerUser_();
	    }
	};

    //  Poll
    public:
        class UnimplementedPoll : public AbstractUse {
	    DECLARE_FAMILY_MEMBERS (UnimplementedPoll, AbstractUse);

	//  Construction
	protected:
	    UnimplementedPoll (VReferenceable *pContainer, VDeviceImplementation *pDevice) : BaseClass (pContainer), m_pDevice (pDevice) {
	    }

	//  Destruction
	protected:
	    ~UnimplementedPoll () {
	    }

	public:
            bool start (VkStatus &rStatus) {
                return rStatus.MakeUnimplementedStatus ();
            }

        //  State
        protected:
            DeviceImplReference const m_pDevice;
        };
        typedef UnimplementedPoll ReadPoll;
        typedef UnimplementedPoll WritePoll;

    //  Construction
    protected:
	VDeviceImplementation (VReferenceable *pContainer, VDeviceImplementation const &rOther);
	VDeviceImplementation (VReferenceable *pContainer);

    //  Destruction
    protected:
	~VDeviceImplementation ();

    //  Access
    private:
	virtual VCohort *cohort_() const {
	    return VCohort::Here ();
	}
    public:
	VCohort *cohort () const {
	    return cohort_();
	}

	//  Override in derived class if implemented
	bool getName (VkStatus &rStatus, VString &rName) const {
	    return rStatus.MakeUnimplementedStatus ();
	}

#ifndef __VMS
	//  Override in derived class if implemented
	bool getSocket (VkStatus &rStatus, SOCKET& rSocket) const {
	    return rStatus.MakeUnimplementedStatus ();
	}
#endif

	unsigned int useCount () const {
	    return m_cUses;
	}
	unsigned int userCount () const {
	    return m_cUsers;
	}

    //  Device Control
    public:
	//  Override in derived class if necessary
	bool makeDeviceFriendly () {
	    return true;
	}

    //  Error Accounting
    protected:
	virtual void onErrorDetect_(AbstractUse *pUse, VkStatus const &rStatus);
    public:
	void onErrorDetect (AbstractUse *pUse, VkStatus const &rStatus);

    /// @name User Accounting
    /// These methods should actually be 'protected', but they are instead public to avoid an MSVC6 bug.
    //@{
    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	void onFirstReader ();		//  ... override in derived class if necessary
	void onFinalReader ();		//  ... override in derived class if necessary

	void onFirstWriter ();		//  ... override in derived class if necessary
	void onFinalWriter ();		//  ... override in derived class if necessary

	void onFirstListener ();	//  ... override in derived class if necessary
	void onFinalListener ();	//  ... override in derived class if necessary
    //@}
    private:
        /**
         * Called when a User requires the use of this Device.
         */
	void incrementUserCount () {
	    if (0 == m_cUsers++)
		onUserCountWasZero ();
	}

        /**
         * Called when a User no longer requires the use of this Device.
         */
	void decrementUserCount () {
	    if (0 == --m_cUsers)
		onUserCountIsZero ();
	}

        /**
         * Non-virtual psas-through method for onUserCountWasZero_().
         */
	void onUserCountWasZero ();

        /**
         * Non-virtual pass-through method for onUserCountIsZero_().
         *
         * In the case that there are outstanding Use instances, this will also call onUseCountIsZero_(). For details, see onUseCountIsZero().
         */
	void onUserCountIsZero ();
    protected:
        /**
         * Called where the User count increments from zero.
         *
         * Default implementation does nothing.
         */
	virtual void onUserCountWasZero_();

        /**
         * Called when there are no User instances (I.E. handles) referencing this device.
         *
         * Default implementation does nothing.
         */
	virtual void onUserCountIsZero_();

    //  Use Accounting
    protected:
        /**
         * To be called when an active use begins processing.
         *
         * Typically called from an implementing subclass when it begins handling device operations for a given Use.
         */
	void incrementUseCount () {
	    if (0 == m_cUses++)
		onUseCountWasZero ();
	}

        /**
         * To be called when an active use completes.
         *
         * Typically called from a Use when it knows that its actions have wrapped up.
         */
	void decrementUseCount () {
	    if (0 == --m_cUses)
		onUseCountIsZero ();
	}
    private:
        /**
         * Non-virtual pass-through method for onuseCountWasZero_().
         *
         * In the case that there are zero User instances referencing this Device, onUseCountWasZero_() is not actually called in order to avoid a bug described in onUseCountIsZero().
         */
	void onUseCountWasZero ();

        /**
         * Non-virtual pass-through method for onUseCountIsZero_().
         *
         * Does not call onUseCountIsZero_() in the case that there are no users. Instead, logs a message indicating said situation. In some circumstances on certain OS's (*cough* VMS *cough*) we have found that some I/O operations fail to report their termination. To deal with this issue, we pretend that all Uses have completed by calling onUseCountIsZero_() directly from onUserCountIsZero() in said situation. In the case of a sane OS, the Uses will actually report their termination and wrap up; in this case we don't want to duplicate our call of onUseCountIsZero_(), hence the need for this extra check.
         */
	void onUseCountIsZero ();
    protected:
        /**
         * Called when the Use count increments from zero (new active operations).
         *
         * Default implementation does nothing.
         */
	virtual void onUseCountWasZero_();

        /**
         * Called when the Use count drops to zero (no active operations).
         *
         * Default implementation does nothing.
         */
	virtual void onUseCountIsZero_();

    //  State
    private:
        /** The number of User instances referencing this device (basically, open handles). */
	unsigned int m_cUsers;
        /** The number of Use instances referencing this device (basically, operations in progress). */
	unsigned int m_cUses;
    };
}


#endif
