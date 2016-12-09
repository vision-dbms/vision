#ifndef Vca_VServerApplication_Interface
#define Vca_VServerApplication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VApplication.h"

#include "Vca_VInterfaceEKG.h"

#include "Vca_IBuddy.h"
#include "Vca_IDirectoryEntry.h"
#include "Vca_IListener.h"
#include "Vca_IPassiveServer.h"
#include "Vca_IRequest.h"
#include "Vca_IServerApplication.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTimer.h"

template<class Actor, class Datum> class VReceiver;

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * VApplication subclass providing machinery to expose an object to the outside world; subclasses define the exposed object.
     *
     * Minimally, derived classes must override start_(), making at least one call to offerSelf() (or one of its overloads).
     * For a concise example of a VServerApplication subclass, take a look at Vca::VServer, which does the following to extend VServerApplication in a useful way:
     *  - On construction, use @ref InterfaceAggregation "interface aggregation" to expose the interfaces of the objects to be exported.
     *  - On start, use offerSelf() to expose, or register in some publically reachable location, the server and its aggregated interfaces.
     *
     * @todo Document lifetime management functionality here.
     *
     */
    class ABSTRACT Vca_Main_API VServerApplication : public VApplication {
        DECLARE_ABSTRACT_RTTLITE (VServerApplication, VApplication);
	DECLARE_ROLEPLAYER_CLASSINFO ();

    //  Aliases
    public:
        /** Timeout type, counted in minutes. */
        typedef U32 timeout_minutes_t;

        /** VReceiver templated on IListener instantiated for this class. */
        typedef VReceiver<ThisClass,IListener*> ListenerReceiver;

    //****************************************************************
    //  ControlRequest
    public:
        /**
         * Encapsulation object for IStop and IPauseResume instances, used for buddy operational management.
         *
         * Each server application that has been launched as a buddy will have one ControlRequest instance containing the control interfaces for the server application that spawned it. These are then used to propagate control requests sent to this server application.
         *
         * This code has been written to allow for multiple ControlRequest instances per server application, though it has not been used as such (yet).
         */
        class ControlRequest : public VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (ControlRequest, VRolePlayer);

        //  Construction
        public:
            ControlRequest (ThisClass *pSuccessor, IPauseResume *pIPauseResume, IStop *pIStop);

        //  Destruction
        private:
            ~ControlRequest ();

        /// @name IRequest Implementation
        //@{
        private:
            /** The IRequest role for this ControlRequest. */
            VRole<ThisClass,IRequest> m_pIRequest;
        public:
            /**
             * Retrieves the IRequest role for this VServerApplication::ControlRequest.
             *
             * @param[out] rpRole the reference that will be used to return the IRequest for this VServerApplication::ControlRequest.
             */
            void getRole (IRequest::Reference &rpRole) {
                m_pIRequest.getRole (rpRole);
            }

        //  IRequest Methods
        public:
            /**
             * Currently does nothing. But it could.
             *
             * @todo Implement IRequest::Cancel() so that control over the represented server is relinquished.
             */
            void Cancel (IRequest *pRole);
        //@}

        //  Use
        public:
            ThisClass *stop (bool bHardStop) const;
            ThisClass *pause () const;
            ThisClass *resume () const;

        //  State
        private:
            /** The successor of this ControlRequest, representing the spawning process of the server applicaiton controlled by this ControlRequest. */
            Reference m_pSuccessor;
            /** The IPauseResume instance for the server that launched this server as its buddy. */
            IPauseResume::Reference const m_pIPauseResume;
            /** The IStop instance for the server that launched this server as its buddy. */
            IStop::Reference const m_pIStop;
        };

    //****************************************************************
    //  OfferTracker
    public:
        class OfferTracker;
        friend class OfferTracker;

    //****************************************************************
    //  ServerActivity
    public:
	class ServerActivity : public Activity {
	    DECLARE_ABSTRACT_RTTLITE (ServerActivity, Activity);

	//  Construction
	protected:
	    ServerActivity (VServerApplication *pServer);

	//  Destruction
	protected:
	    ~ServerActivity ();

	//  Access
	public:
	    VServerApplication *application () const {
		return static_cast<VServerApplication*>(BaseClass::application());
	    }

	//  Callbacks
	protected:
	    void OnError_(IError *pInterface, VString const &rMessage);
	};
	friend class ServerActivity;

    //****************************************************************
    //  Callback
    public:
	class Callback;
	friend class Callback;

    //****************************************************************
    //  Listener
    public:
	class Listener : public ServerActivity {
	    DECLARE_CONCRETE_RTTLITE (Listener, ServerActivity);
	    DECLARE_ROLEPLAYER_CLASSINFO ();

	//  Aliases
	public:
	    typedef IVReceiver<IListener*> IListenerSink;

	//  Construction
	public:
	    Listener (
		VServerApplication *pServer, IListenerFactory *pFactory, VString const &rAddress
	    );

	//  Destruction
	private:
	    ~Listener ();

	//  Access
	public:
	    VString const &listenerName () const {
		return m_iListenerName;
	    }
	    VString const &registrationName () const {
		return m_iRegistrationName;
	    }
	    VString const &registrationData () const {
		return m_iRegistrationData;
	    }

	protected:
	    void getDescription_(VString &rResult) const;

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IListenerSink Role
	private:
	    VRole<ThisClass,IListenerSink> m_pIListenerSink;
	public:
	    void getRole (IListenerSink::Reference &rpRole) {
		m_pIListenerSink.getRole (rpRole);
	    }

	//  IListenerSink Methods
	public:
	    void OnData (IListenerSink *pRole, IListener *pListener);

	//  ITrigger Role
	private:
	    VRole<ThisClass,ITrigger> m_pITrigger;
	public:
	    void getRole (ITrigger::Reference &rpRole) {
		return m_pITrigger.getRole (rpRole);
	    }
	public:
	    void Process (ITrigger *pRole);

	//  Callbacks
	private:
	    void onListenerName (VString const &rName);

	//  Control
	private:
	    void startWatching ();
	    void stopListening ();
	public:
	    void retract ();

	//  Status
	protected:
	    void onFailure (IError *pInterface, VString const &rMessage);

	//  State
	private:
	    IListener::Reference m_pIListener;
	    
	    VString m_iListenerName;
	    VString m_iRegistrationName;
	    VString m_iRegistrationData;
	};
	friend class Listener;

	typedef VkSetOf<Listener::Reference, Listener*> listener_set_t;

    //****************************************************************
    //  Registration
    public:
	class Registration : public ServerActivity {
	    DECLARE_CONCRETE_RTTLITE (Registration, ServerActivity);
	    DECLARE_ROLEPLAYER_CLASSINFO ();

	//  Aliases
	public:
	    typedef IDirectory			IRegistry;
	    typedef IDirectoryEntry		IRegistration;
	    typedef IVReceiver<IRegistration*>	IRegistrationSink;

 	    typedef VGoferInterface<IRegistry> registry_gofer_t;

	    typedef VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;

	//  Construction
	public:
	    Registration (
		VServerApplication *pServer, registry_gofer_t *pRegistryGofer, VString const &rRegistrationName
	    );

	//  Destruction
	private:
	    ~Registration ();

	//  Access
	public:
	    VString const &registrationName () const {
		return m_iRegistrationName;
	    }

	protected:
	    void getDescription_(VString &rResult) const;

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IRegistrationSink Role
	private:
	    VRole<ThisClass,IRegistrationSink> m_pIRegistrationSink;
	public:
	    void getRole (IRegistrationSink::Reference &rpRole) {
		m_pIRegistrationSink.getRole (rpRole);
	    }

	//  IRegistrationSink Methods
	public:
	    void OnData (IRegistrationSink *pRole, IRegistration *pRegistration);

	//  Callbacks
	private:
	    void onDisconnect ();
	    void onRegistry (IRegistry *pRegistry);

	//  Control
	private:
	    void monitor (IVUnknown *pInterface);
	protected:
	    void onFailure (IError *pInterface, VString const &rMessage);
	public:
	    void retract ();

	//  State
	private:
	    VString const m_iRegistrationName;
	    interface_monitor_t::Reference m_pMonitor;
	    IRegistration::Reference m_pIRegistration;
	};
	friend class Registration;

	typedef VkSetOf<Registration::Reference, Registration*> registration_set_t;

    //****************************************************************
    //  Plumber
    public:
	class Plumber;
	friend class Plumber;

    //****************************************************************
    //  Construction
    protected:
        /** @copydoc Vca::VApplication::VApplication() */
        VServerApplication (Context *pContext);

    //  Destruction
    protected:
        /** @copydoc Vca::VApplication::~VApplication() */
        ~VServerApplication ();

    //  Access/Query
    public:
        /** Returns the number of offers that we currently have out. */
        count_t activeOfferCount () const {
            return m_cOffers;
        }
	count_t passiveOfferCount () const {
	    return listenerCount () + registrationCount ();
	}
	count_t listenerCount () const {
	    return  m_pListeners.cardinality ();
	}
	VString listenerName () const;
	count_t registrationCount () const {
	    return m_pRegistrations.cardinality ();
	}
	bool stoppingOnIdle () const {
	    return m_bStoppingOnIdle || passiveOfferCount () == 0;
	}
        /**
         * Returns the stop timeout of this application.
         *
         * Delegates to stopTimeout_() so that subclasses can override behavior.
         */
        timeout_minutes_t stopTimeout () const {
            return stopTimeout_();
        }
        /**
         * Returns the watch timeout of this application.
         *
         * Delegates to watchTimeout_() so that subclasses can override behavior.
         */
	timeout_minutes_t watchTimeout () const {
	    return watchTimeout_();
	}
    private:
	bool getListenerName (VString &rName) const;
        /**
         * @copybrief stopTimeout()
         * Delegate of stopTimout(), overridden by subclasses as necessary.
         */
        virtual timeout_minutes_t stopTimeout_() const {
            return m_sStopTimeout;
        }
        /**
         * @copybrief watchTimeout()
         * Delegate of watchTimout(), overridden by subclasses as necessary.
         */
        virtual timeout_minutes_t watchTimeout_() const {
            return m_sWatchTimeout;
        }

    //  Base Roles
    public:
        using BaseClass::getRole;

    /// @name IBuddy Implementation
    //@{
    private:
        /** The IBuddy role for this VServerApplication. */
        VRole<ThisClass,IBuddy> m_pIBuddy;
    public:
        /**
         * Retrieves the IBuddy role for this VServerApplication::ControlRequest.
         *
         * @param[out] rpRole the reference that will be used to return the IBuddy for this VServerApplication::ControlRequest.
         */
        void getRole (IBuddy::Reference &rpRole) {
            m_pIBuddy.getRole (rpRole);
        }

    // IBuddy Methods
    public:
        /** Implements IBuddy::ControlThese(). */
        void ControlThese (
            IBuddy *pRole, IRequestReceiver *pRequestReceiver, IPauseResume *pIPauseResume, IStop *pIStop
        );
    //@}

    /// @name IPassiveServer Implementation
    //@{
    private:
        /** The IPassiveServer role for this VServerApplication. */
        VRole<ThisClass,IPassiveServer> m_pIPassiveServer;
    public:
        /**
         * Retrieves the IPassiveServer role for this VServerApplication::ControlRequest.
         *
         * @param[out] rpRole the reference that will be used to return the IPassiveServer for this VServerApplication::ControlRequest.
         */
        void getRole (IPassiveServer::Reference &rpRole) {
            m_pIPassiveServer.getRole (rpRole);
        }

    //  IPassiveServer Methods
    public:
        /** Implements IPassiveServer::RequestCallback() */
        void RequestCallback (
            IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
        );
    //@}

    /// @name IServerApplication Implementation
    //@{
    private:
        /** The IServerApplication role for this VServerApplication. */
        VRole<ThisClass,IServerApplication> m_pIServerApplication;
    public:
        /**
         * Retrieves the IServerApplication role for this VServerApplication::ControlRequest.
         *
         * @param[out] rpRole the reference that will be used to return the IServerApplication for this VServerApplication::ControlRequest.
         */
        void getRole (IServerApplication::Reference &rpRole) {
            m_pIServerApplication.getRole (rpRole);
        }
    //@}

    //  Activity Accounting
    private:
	virtual void onActivityCountWasZero ();
	virtual void onActivityCountIsZero ();

	void onStopCondition ();

    /**
     * @name Connectivity
     * Member functions at least one of which must be called from the override of start_() of derived classes in order to make available all of the interfaces implemented by this application, including:
     *  - The application query/control interfaces provided by VApplication.
     *  - The server application query/control interfaces provided by VServerApplication.
     *  - The application interfaces provided by subclasses.
     */
    //@{
    protected:
        /**
         * Makes availabe all of this application's interfaces via a network address.
         *
         * @param rAddress the network address on which this application's interfaces should be made available.
         */
        void offerSelf (VString const &rAddress);

        /**
         * Makes availabe all of this application's interfaces via a network address.
         *
         * @param pAddress the network address on which this application's interfaces should be made available.
         */
        void offerSelf (char const* pAddress);

	/**
         * Makes available all of this application's interfaces via channels specified by the command-line.
         *
         * Currently three offer mechanisms are supported:
         *  - Via a network address: This aplication was given an 'address' parameter and will use said parameter's value as a publish address in the TCP/IP address space.
         *  - Via standard I/O channels: This application was launched with the 'plumbed' switch enabled and will use the standard input and output channels to speak Vca with its caller. This is only sensical in a programmatically launched setting.
         *  - Via callback to client: This application was given one or more 'callback' parameters, each specifying the address and callback token of a client.
         *
         * @param bPublicOfferAlreadyMade 
         * @return true if at least one offer made, false otherwise
         */
        bool offerSelf (bool bPublicOfferAlreadyMade);

        /**
         * @copybrief offerSelf(bool)
         * This is the routine that most subclasses will call in order to publish their interfaces.
         *
         * Delegates to offerSelf(bool) with its only parameter as @c false.
         */
        bool offerSelf () {
            return offerSelf(false);
        }
    //@}

    /**
     * @name Connectivity Callbacks
     * Methods used internally by this class for establishing Vca connectivity, typically initiated by a call to offerSelf().
     */
    //@{
    private:
        void onBuddy (IBuddy *pBuddy);
        void onBuddyGoferError (IError *pInterface, VString const &rMessage);

        bool onCallbackRequest (char const *pCallbackRequest);
        void onCallbackRequest (IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID);

        void onStandardChannels (VBSProducer *pStdin, VBSConsumer *pStdout);
        void onStandardChannels (VBSProducer *pStdin, VBSConsumer *pStdout, VBSConsumer *pStderr);

	bool attach (Listener *pListener);
	bool detach (Listener *pListener);

	bool attach (Registration *pRegistration);
	bool detach (Registration *pRegistration);
    //@}

    //  Transitions
    protected:
        /**
         * Gracefully stops this server, also stopping all buddies for whom we have control requests.
         *
         * @see m_pControlRequests
         * @see ControlRequest::stop()
         */
        bool stop_(bool bHardstop);

        /**
         * Propagates the pause operation to all buddies for whom we have control requests.
         *
         * @see m_pControlRequests
         * @see ControlRequest::pause()
         */
        bool pause_();

        /**
         * Propagates the reesume operation to all buddies for whom we have control requests.
         *
         * @see m_pControlRequests
         * @see ControlRequest::resume()
         */
        bool resume_();

    /// @name Triggering and Timeouts
    //@{
    private:
        /**
         * Callback triggered when the offer count is incremented.
         */
        void onOfferCountIncrement (OfferTracker *pTracker, VcaOffer* pOffer);

        /**
         * Callback triggered when the offer count is decremented.
         */
        void onOfferCountDecrement (OfferTracker *pTracker, VcaOffer* pOffer);

        /**
         * Callback triggered when the offer count is decremented to zero.
         */
        void onOfferCountIsZero ();

        /**
         * Callback triggered when the offer count is incremented from zero.
         */
        void onOfferCountWasZero ();

        /**
         * Callback triggered when the stop timer fires.
         */
        void onStopTimer (VTrigger<ThisClass> *pTrigger);

        /**
         * Cancels the stop timer altogether.
         */
        void cancelStopTimer ();
    //@}

    /// @name Tracking and Trackables
    //@{
    public:
	VTrackable_count_t const &activeOfferCountTrackable ();
    //@}

    //  State
    protected:
	/** Used to hold the 'listen' activity for this application, if one was created. */
	listener_set_t			m_pListeners;
	registration_set_t		m_pRegistrations;
        /** Used to keep track of our offer count. */
        VTrackableCounter<counter_t>    m_cOffers;
        /** The maximum number of offers that this server will handle before finding/making a buddy. */
        count_t                         m_sOfferLimit;
        /** Control mechanisms for our buddies. */
        ControlRequest::Reference       m_pControlRequests;
        /** The name within our buddy source namespace (typically the default Directory) to use to create buddies. */
        VString                         m_iBuddySource;
        /** Our buddy (list). */
        IBuddy::Reference               m_pBuddy;
        /** Used internally for buddy control. */
        IRequest::Reference             m_pBuddyControlRequest;
        /** The stop timer for this application. */
        VTimer::Reference               m_pStopTimer;
        /** The stop timeout for this application. */
        timeout_minutes_t               m_sStopTimeout;
        /** The watch timeout for this application. */
        timeout_minutes_t               m_sWatchTimeout;
        /** Used to keep track of whether or not this application will stop automatically when the offer count reaches zero. */
        bool                            m_bStoppingOnIdle;
        /** True if we're in a state in between requesting a buddy and receiving one. */
        bool                            m_bWaitingForBuddy;
    };
}


#endif
