#ifndef Vca_VDevice_Interface
#define Vca_VDevice_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vca_VStatus.h"

#include "V_VAddin.h"
#include "V_VArea.h"
#include "V_VQueue.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IConnection;
    class IListener;
    class IPipeSource;

    class VBSConsumer;
    class VBSProducer;
    class VConnection;
    class VListener;

    /**
     * Descriptor for an abstract, low-level device, logical, physical or otherwise.
     * Currently, devices can support any of the following operating interfaces:
     *  - Byte stream producer (BSReadFace)
     *  - Byte stream consumer (BSWriteFace)
     *  - Connectivity builder (ListenFace)
     *
     * There is one templated subclass of this class:
     * @code
     * template <class Implementation> VDevice_<Implementation>
     * @endcode
     * The Implementation class fills in the implementation details for the device (e.g. a socket, pipe, etc.). VDevice_<Implementation> is subclassed once more to produce a concrete VDevice. This specialization exposes a set of device interfaces appropriate for the specific device (e.g. reader, writer, listener, etc.). 
     * Currently, the following templated subclasses of VDevice_<Implementation> provide for the common cases:
     *  - VBSConsumerDevice_<Implementation>
     *  - VBSProducerDevice_<Implementation>
     *  - VBSProducerConsumerDevice_<Implementation>
     *  - VListeningDevice_<Implementation>
     * As an example, a socket device can be read from and written to, thus it is instantiated via the VBSProducerConsumerDevice_ template, providing its own Implementation template parameter.
     *
     * @todo Add dot picture here describing inheritance tree.
     *
     * Each template instantiation provides its own implementation delegate, which in turn provides implementation of the appropriate interface methods. These methods are all then statically bound by inheritance, so there is no need for providing virutals.
     */
    class Vca_API VDevice : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VDevice, VRolePlayer);

    /*-------------------------*
     *----  VDevice::Face  ----*
     *-------------------------*/
    public:
        /** The interface of the device consisting of the exposed functionality that this device offers to its consumers. */
	class Vca_API Face : public V::VAddin {
	    DECLARE_FAMILY_MEMBERS (Face, V::VAddin);

	//  User
	public:
            /**
             * VAddin providing functionality for using a particular VDevice::Face.
             *
             * In order to make use of any given VDevice::Face, a consuming class need only inherit from the User nested in said Face. This inheritance enables the consuming class to make use of any VDevice that includes said Face in its implementation. For example, VStreamSource indicates its compatibility with BSReadFace by inheriting from the User associated with (nested inside) BSReadFace: BSReader.
             *
             * User instances are in one of two states at any given time: open or closed. Closed Users are not capable of interacting with their underlying device.
             *
             * @todo document in two-part fashion: things that subclasses must define, and things that subclasses should call.
             */
	    class Vca_API User : public V::VAddin {
		DECLARE_FAMILY_MEMBERS (User, V::VAddin);

	    //  Use
	    public:
		class Vca_API Use : public VRolePlayer {
		    DECLARE_ABSTRACT_RTTLITE (Use, VRolePlayer);

		//  Friends
		    friend class User;

		//  RunState
		public:
		    enum RunState {
	/*  .------- */	RunState_Idle,
	/*  |	     */
	/*  '------> */	RunState_Busy,		/* -----. */
						/*	| */
	/*  .------- */	RunState_Data,		/* <----| */
	/*  |------- */	RunState_Error,		/* <----| */
	/*  |						| */
	/*  |   .--- */	RunState_DataKeep,	/* <----| (in operating system queue) */
	/*  |	|--- */	RunState_ErrorKeep,	/* <----' (in operating system queue) */
	/*  |	|    */
	/*  |   '--> */	RunState_RecycleKeep,	/*  ----. (in operating system queue) */
	/*  |						| */
	/*  '------> */	RunState_Recycle	/* <----' */
		    };

		//  Construction
		protected:
		    Use ();

		//  Destruction
		protected:
		    ~Use ();

		//  Access
		public:
		    RunState runState () const {
			return m_xRunState;
		    }
		    VStatus const &status () const {
			return m_iStatus;
		    }

		private:
		    virtual Use *use_() = 0;
		    virtual User *user_() = 0;
		public:
		    Use *use () {
			return use_();
		    }
		    User *user () {
			return user_();
		    }

		//  Callback
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    void triggerUser ();

		//  Execution
		private:
		    virtual void finish_() = 0;
		private:
		    bool finish (VkStatus &rStatus);	//  ... called by User::finish
		protected:
		    void onUser (User *pUser);
		    bool onData (bool bKeep);
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool onError (VkStatus const &rStatus, bool bKeep);
		private:
		    void onUsed ();			//  ... called by User::finish

		//  Recycling
		private:
		    virtual void recycle_() = 0;
		protected:
		    void recycle ();

		//  State
		private:
		    RunState	m_xRunState;
		    VStatus	m_iStatus;
		};
		friend class Use;

	    //  Use Queue
	    public:
                /** Queue of uses in progress. */
		typedef V::VQueue<Use::Reference> QUIP;

	    //  Construction
	    protected:
                /**
                 * Constructor expects referenceable delegate for reference counting.
                 *
                 * @param pContainer the referencable that will be used to keep reference counts for this addin.
                 */
		User (VReferenceable *pContainer) : BaseClass (pContainer), m_bClosed (false) {
		}

	    //  Destruction
	    protected:
                /** Empty destructor restricts scope. */
		~User () {
		}

	    //  Access
	    public:
                /**
                 * Used to determine whether or not this user is closed.
                 * Users that are closed are no longer active and can't do anything with the device.
                 *
                 * @return true if this user is closed, false otherwise.
                 */
		bool closed () const {
		    return m_bClosed;
		}
	    private:
                /**
                 * Used to retrieve the Face upon which this User acts.
                 *
                 * @return the Face upon which this User acts.
                 */
		virtual Face *face_() const = 0;
	    public:
                /** @copydoc face_() */
		Face *face () const {
		    return face_();
		}
                /**
                 * Used to retrieve the name of the VDevice via its Face upon which this User acts.
                 *
                 * @param[out] rStatus the return status of the name retrieval.
                 * @param[out] rName the reference by which the VDevice name should be returned.
                 * @return true on success, false on failure.
                 */
		bool getName (VkStatus &rStatus, VString &rName) {
		    return face ()->getName_(rStatus, rName);
		}

	    //  Callback
	    private:
		virtual void trigger_() = 0;
	    public:
		void trigger () {
		    trigger_();
		}

	    //  Use Start
	    protected:
                /**
                 * Used to determine if new operations can be started by this User.
                 * Should always be called by derived classes before performing start logic. If this method returns false, no start logic should be performed.
                 *
                 * @param[out] rStatus the return status of the start operation.
                 * @return true if new operations can be started by this User, false otherwise (the User is closed).
                 */
		bool start (VkStatus &rStatus);

                /**
                 * Enqueues a Use from this user.
                 *
                 * @param pUse the Use to be enqueued.
                 *
                 * @see m_iQUIP
                 */
		void enqueue (Use *pUse);

	    //  Use Completion
	    protected:
                /**
                 * Processes elements on the work in progress queue.
                 * Continually processes events on the queue until either (a) there's nothing left to process or (b) the work in progress at the front of the queue hasn't yet finished.
                 *
                 * @param[out] rStatus the return status of the queue processing.
                 * @return true if the queue is empty after processing, false otherwise.
                 * @see m_iQUIP
                 */
		bool finish (VkStatus &rStatus);

	    //  Control
	    protected:
                /**
                 * Closes this User.
                 *
                 * Prevents this user from doing anything else with its underlyind device.
                 *
                 * @return true if this user was not previously closed, false otherwise.
                 */
		bool close () {
		    bool bNewClose = !m_bClosed;
		    m_bClosed = true;
		    return bNewClose;
		}

	    //  State
	    private:
                /** Queue of Uses in Progress. */
		QUIP m_iQUIP;
                /** Whether or not this User is closed. */
		bool m_bClosed;
	    };
	public:
	    typedef User::Use Use;
	    friend class User;

	//  Construction
	protected:
            /**
             * Constructor expects a referenceable that manages the lifetime of this addin instance.
             *
             * @param pContainer the referenceable that manages the lifetime of this addin instance.
             */
	    Face (VReferenceable *pContainer) : BaseClass (pContainer), m_cUsers (0) {
	    }

	//  Destruction
	protected:
	    ~Face () {
	    }

	//  Access
	private:
            /**
             * Used to retrieve the device for which this is an interface.
             *
             * @return the underlying device.
             */
	    virtual VDevice *device_() = 0;

            /**
             * Used to retrieve a descriptive name for the underlying device.
             *
             * @param rStatus the return status of this call.
             * @param rName the reference by which the descriptive name for the underlying device should be returned.
             * @return true if the name retrieval succeeded, false otherwise.
             */
	    virtual bool getName_(VkStatus &rStatus, VString &rName) = 0;
	public:
            /**
             * @copydoc device_()
             */
	    VDevice *device () {
		return device_();
	    }

            /**
             * Used to retrieve the cohort to which the underlying device belongs.
             *
             * @return the cohort to which the underlying device belongs.
             */
	    VCohort *deviceCohort () {
		return device ()->cohort ();
	    }

	//  User Management
	protected:
            /**
             * Invoked when a user of this interface is created.
             *
             * @return true if transitioning from a count of no users to a positve number of users.
             */
	    bool onUserCreation () {
		return 0 == m_cUsers++;	//  true if first
	    }
            /**
             * Invoked when a user of this interface is destroyed.
             *
             * @return true if transitioning to a count of no users from a positve number of users.
             */
	    bool onUserDeletion () {
		return 0 == --m_cUsers;	//  true if last
	    }

	//  State
	private:
            /** The current number of users of this aspect of this device. */
	    unsigned int m_cUsers;
	};
    public:
	typedef Face::Use  Use;
	typedef Face::User User;


    /*---------------------------*
     *----  VDevice::BSFace  ----*
     *---------------------------*/
    public:
        /** Face subclass serves as an interface for byte-stream devices. */
	class Vca_API BSFace : public Face {
	    DECLARE_FAMILY_MEMBERS (BSFace, Face);

	//  User
	public:
            /** User that transfers bytes in a byte stream. */
	    class Vca_API BSMover : public User {
		DECLARE_FAMILY_MEMBERS (BSMover, User);

	    //  Use
                class Vca_API BSUse : public Use {
                    DECLARE_ABSTRACT_RTTLITE (BSUse, Use);

		//  User Reference
		public:
		    typedef VReference<BSMover> UserRef;

	        //  Construction
	        protected:
	            BSUse () {
	            }

	        //  Destruction
	        protected:
	            ~BSUse () {
	            }

		//  Execution
                protected:
		    void onUser (BSMover *pUser) {
			m_pUser.setTo (pUser);
			BaseClass::onUser (pUser);
		    }

		//  Access
		private:
		    User *user_() {
			return user ();
		    }
		public:
		    BSMover *user () {
			return m_pUser;
		    }

	        //  Callback
	        public:
	            void triggerUser () {
                        m_pUser->trigger ();
	            }

		//  State
		protected:
		    UserRef	m_pUser;
                };

	    public:
		class Vca_API BSMove : public BSUse {
		    DECLARE_ABSTRACT_RTTLITE (BSMove, BSUse);

		//  Construction
		protected:
		    BSMove () {
		    }

		//  Destruction
		protected:
		    ~BSMove () {
		    }

		//  Completion
		private:
		    void finish_() {
			m_pUser->onTransfer (m_sTransfer);
		    }

		//  Execution
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool onData (size_t sTransfer, bool bKeep) {
			m_sTransfer = sTransfer;
			return BaseClass::onData (bKeep);
		    }
		    bool onData (VDevice *pDevice, bool bKeep) {
			return BaseClass::onData (bKeep);
		    }

		//  Execution
		protected:
		    void recycle () {
			BaseClass::recycle ();
			m_pUser.clear ();
		    }

		//  State
		private:
		    size_t	m_sTransfer;
		};
		friend class BSMove;

            //  BSPoll
            public:
                class Vca_API BSPoll : public BSUse {
                    DECLARE_ABSTRACT_RTTLITE (BSPoll, BSUse);

		//  Construction
		protected:
		    BSPoll () {
		    }

		//  Destruction
		protected:
		    ~BSPoll () {
		    }

		//  Startup
		private:
		    virtual bool start_(VkStatus &rStatus) = 0;
		public:
		    bool start (
			VkStatus &rStatus, BSMover *pUser
		    ) {
			onUser (pUser);
			return start_(rStatus);
		    }

		//  Execution
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool onData (size_t sTransfer, bool bKeep) {
			return BaseClass::onData (bKeep);
		    }
		    bool onData (VDevice *pDevice, bool bKeep) {
			return BaseClass::onData (bKeep);
		    }

		//  Completion
		private:
                    /** Empty definition satisfies virtual requirement. */
		    void finish_() {
		    }
                };

	    //  Construction
	    protected:
		BSMover (VReferenceable *pContainer) : BaseClass (pContainer) {
		}

	    //  Destruction
	    protected:
		~BSMover () {
		}

	    //  Data Delivery
	    private:
		virtual void onTransfer_(size_t sTransfer) = 0;
		void onTransfer (size_t sTransfer) {
		    onTransfer_(sTransfer);
		}
	    };
	    typedef BSMover::BSMove BSMove;
            typedef BSMover::BSPoll BSPoll;

	//  Construction
	protected:
	    BSFace (VReferenceable *pContainer) : BaseClass (pContainer) {
	    }

	//  Destruction
	protected:
	    ~BSFace () {
	    }

	//  User Creation
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool supply (VReference<VConnection>&rpUser);
	};
    public:
	typedef BSFace::BSMove  BSMove;
	typedef BSFace::BSMover BSMover;
        typedef BSFace::BSPoll  BSPoll;


    /*-------------------------------*
     *----  VDevice::BSReadFace  ----*
     *-------------------------------*/
    public:
	class Vca_API BSReadFace : public BSFace {
	    DECLARE_FAMILY_MEMBERS (BSReadFace, BSFace);

	//  User
	public:
            /**
             * User that reads byte streams.
             */
	    class Vca_API BSReader : public BSMover {
		DECLARE_FAMILY_MEMBERS (BSReader, BSMover);

	    //  Use
	    public:
		class Vca_API BSRead : public BSMove {
		    DECLARE_ABSTRACT_RTTLITE (BSRead, BSMove);

		//  Area
		public:
		    typedef V::VArea<void*> Area;
		    typedef Area::Pointer AreaPointer;

		//  Construction
		protected:
		    BSRead () {
		    }

		//  Destruction
		protected:
		    ~BSRead () {
		    }

		//  Startup
		private:
		    virtual bool start_(VkStatus &rStatus, Area const &rArea) = 0;
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool start (
			VkStatus &rStatus, BSReader *pUser, Area const &rArea
		    ) {
			onUser (pUser);
			return start_(rStatus, rArea);
		    }
		};
		typedef BSRead::AreaPointer	AreaPointer;
		typedef BSRead::Area		Area;
		friend class BSRead;

	    //  Construction
	    protected:
		BSReader (
		    VReferenceable *pContainer, BSReadFace *pFace
		) : BaseClass (pContainer), m_pFace (pFace) {
		    m_pFace->onUserCreation ();
		}

	    //  Destruction
	    protected:
		~BSReader () {
		    close ();
		}

	    //  Access
	    private:
		Face *face_() const {
		    return face ();
		}
	    public:
		BSReadFace *face () const {
		    return m_pFace;
		}

	    //  Use
	    protected:
		bool start (VkStatus &rStatus, AreaPointer pArea, size_t sArea);
		bool start (VkStatus &rStatus, Area const &rArea);
                /** Used for polling. */
                bool start (VkStatus &rStatus);

	    //  Control
	    protected:
		void close () {
		    if (BaseClass::close ())
			m_pFace->onUserDeletion ();
		}

	    //  State
	    private:
		VReference<BSReadFace> const m_pFace;
	    };
	    typedef BSReader::AreaPointer	AreaPointer;
	    typedef BSReader::Area		Area;
	    typedef BSReader::BSRead		BSRead;
	    friend class BSReader;

	//  Construction
	protected:
	    BSReadFace (VReferenceable *pContainer) : BaseClass (pContainer) {
	    }

	//  Destruction
	protected:
	    ~BSReadFace () {
	    }

	//  User Accounting
	private:
	    virtual void onFirstUser_(ThisClass *pFace) = 0;
	    virtual void onFinalUser_(ThisClass *pFace) = 0;
	protected:
	    void onUserCreation () {
		if (BaseClass::onUserCreation ())	//  true if first
		    onFirstUser_(this);
	    }
	    void onUserDeletion () {
		if (BaseClass::onUserDeletion ())	//  true if last
		    onFinalUser_(this);
	    }

	//  User Creation
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    using BaseClass::supply;
	    bool supply (VReference<VBSProducer>&rpUser);

	//  Use
	private:
	    virtual bool start_(
		VkStatus &rStatus, BSReader *pUser, Area const &rArea
	    ) = 0;
            virtual bool start_(
		VkStatus &rStatus, BSReader *pUser
            ) = 0;
	};
	typedef BSReadFace::Area	BSReadArea;
	typedef BSReadFace::BSRead	BSRead;
	typedef BSReadFace::BSReader	BSReader;


    /*--------------------------------*
     *----  VDevice::BSWriteFace  ----*
     *--------------------------------*/
    public:
	class Vca_API BSWriteFace : public BSFace {
	    DECLARE_FAMILY_MEMBERS (BSWriteFace, BSFace);

	//  User
	public:
            /** User that writes to a byte stream. */
	    class Vca_API BSWriter : public BSMover {
		DECLARE_FAMILY_MEMBERS (BSWriter, BSMover);

	    //  Use
	    public:
		class Vca_API BSWrite : public BSMove {
		    DECLARE_ABSTRACT_RTTLITE (BSWrite, BSMove);

		//  Area
		public:
		    typedef V::VArea<void const*> Area;
		    typedef Area::Pointer AreaPointer;

		//  Construction
		protected:
		    BSWrite () {
		    }

		//  Destruction
		protected:
		    ~BSWrite () {
		    }

		//  Startup
		private:
		    virtual bool start_(VkStatus &rStatus, Area const &rArea) = 0;
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool start (
			VkStatus &rStatus, BSWriter *pUser, Area const &rArea
		    ) {
			onUser (pUser);
			return start_(rStatus, rArea);
		    }
		};
		typedef BSWrite::AreaPointer	AreaPointer;
		typedef BSWrite::Area		Area;
		friend class BSWrite;

	    //  Construction
	    protected:
		BSWriter (
		    VReferenceable *pContainer, BSWriteFace *pFace
		) : BaseClass (pContainer), m_pFace (pFace) {
		    m_pFace->onUserCreation ();
		}

	    //  Destruction
	    protected:
                /** Destructor calls close(). */
		~BSWriter () {
		    close ();
		}

	    //  Access
	    private:
		Face *face_() const {
		    return face ();
		}
	    public:
		BSWriteFace *face () const {
		    return m_pFace;
		}

	    //  Use
	    protected:
		bool start (VkStatus &rStatus, AreaPointer pArea, size_t sArea);
		bool start (VkStatus &rStatus, Area const &rArea);
                bool start (VkStatus &rStatus);

	    //  Control
	    protected:
		void close () {
		    if (BaseClass::close ())
			m_pFace->onUserDeletion ();
		}

	    //  State
	    private:
		VReference<BSWriteFace> const m_pFace;
	    };
	    typedef BSWriter::AreaPointer	AreaPointer;
	    typedef BSWriter::Area		Area;
	    typedef BSWriter::BSWrite		BSWrite;
	    friend class BSWriter;

	//  Construction
	protected:
	    BSWriteFace (VReferenceable *pContainer) : BaseClass (pContainer) {
	    }

	//  Destruction
	protected:
	    ~BSWriteFace () {
	    }

	//  User Accounting
	private:
	    virtual void onFirstUser_(ThisClass *pFace) = 0;
	    virtual void onFinalUser_(ThisClass *pFace) = 0;
	protected:
	    void onUserCreation () {
		if (BaseClass::onUserCreation ())	//  true if first
		    onFirstUser_(this);
	    }
	    void onUserDeletion () {
		if (BaseClass::onUserDeletion ())	//  true if last
		    onFinalUser_(this);
	    }

	//  User Creation
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    using BaseClass::supply;
	    bool supply (VReference<VBSConsumer>&rpUser);

	//  Use
	private:
	    virtual bool start_(
		VkStatus &rStatus, BSWriter *pUser, Area const &rArea
	    ) = 0;
            virtual bool start_(
		VkStatus &rStatus, BSWriter *pUser
            ) = 0;
	};
	typedef BSWriteFace::Area     BSWriteArea;
	typedef BSWriteFace::BSWrite  BSWrite;
	typedef BSWriteFace::BSWriter BSWriter;


    /*-------------------------------*
     *----  VDevice::ListenFace  ----*
     *-------------------------------*/
    public:
	class Vca_API ListenFace : public Face {
	    DECLARE_FAMILY_MEMBERS (ListenFace, Face);

	//  User
	public:
	    class Vca_API Listener : public User {
		DECLARE_FAMILY_MEMBERS (Listener, User);

	    //  Use
	    public:
		class Vca_API Listen : public Use {
		    DECLARE_ABSTRACT_RTTLITE (Listen, Use);

		//  User Reference
		public:
		    typedef VReference<Listener> UserRef;

		//  Construction
		protected:
		    Listen () {
		    }

		//  Destruction
		protected:
		    ~Listen () {
		    }

		//  Startup
		private:
		    virtual bool start_(VkStatus &rStatus) = 0;
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    bool start (VkStatus &rStatus, Listener *pUser) {
			onUser (pUser);
			return start_(rStatus);
		    }

		//  Callback
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    void triggerUser () {
			m_pUser->trigger ();
		    }

		//  Execution
		public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		    void onUser (Listener *pUser) {
			m_pUser.setTo (pUser);
			BaseClass::onUser (pUser);
		    }
		    bool onData (size_t sTransfer, bool bKeep) {
			return BaseClass::onData (bKeep);
		    }
		    bool onData (VDevice *pDevice, bool bKeep) {
			m_pDevice.setTo (pDevice);
			return BaseClass::onData (bKeep);
		    }

		//  Completion
		private:
		    void finish_() {
			m_pUser->onDevice (m_pDevice);
		    }

		//  Recycling
		protected:
		    void recycle ()  {
			BaseClass::recycle ();
			m_pDevice.clear ();
			m_pUser.clear ();
		    }

		//  State
		private:
		    UserRef		m_pUser;
		    VReference<VDevice> m_pDevice;
		};
		friend class Listen;

	    //  Construction
	    protected:
		Listener (
		    VReferenceable *pContainer, ListenFace *pFace
		) : BaseClass (pContainer), m_pFace (pFace) {
		    m_pFace->onUserCreation ();
		}

	    //  Destruction
	    protected:
		~Listener () {
		    close ();
		}

	    //  Access
	    private:
		Face *face_() const {
		    return face ();
		}
	    public:
		ListenFace *face () const {
		    return m_pFace;
		}

	    //  Data Delivery
	    private:
		virtual void onDevice_(VDevice *pDevice) = 0;
	    protected:
		void onDevice (VDevice *pDevice) {
		    onDevice_(pDevice);
		}

	    //  Use
	    protected:
		bool start (VkStatus &rStatus);

	    //  Control
	    protected:
		void close () {
		    if (BaseClass::close ())
			m_pFace->onUserDeletion ();
		}

	    //  State
	    private:
		VReference<ListenFace> const m_pFace;
	    };
	    typedef Listener::Listen Listen;
	    friend class Listener;

	//  Construction
	protected:
	    ListenFace (VReferenceable *pContainer) : BaseClass (pContainer) {
	    };

	//  Destruction
	protected:
	    ~ListenFace () {
	    }

	//  User Accounting
	private:
	    virtual void onFirstUser_(ThisClass *pFace) = 0;
	    virtual void onFinalUser_(ThisClass *pFace) = 0;
	protected:
	    void onUserCreation () {
		if (BaseClass::onUserCreation ())	//  true if first
		    onFirstUser_(this);
	    }
	    void onUserDeletion () {
		if (BaseClass::onUserDeletion ())	//  true if last
		    onFinalUser_(this);
	    }

	//  User Creation
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool supply (VReference<VListener>&rpUser);

	//  Use
	private:
	    virtual bool start_(VkStatus &rStatus, Listener *pUser) = 0;
	};
	typedef ListenFace::Listen	Listen;
	typedef ListenFace::Listener	Listener;


    /*-------------------*
     *----  VDevice  ----*
     *-------------------*/

    //  Construction
    protected:
	VDevice (VCohort *pCohort);

    //  Destruction
    protected:
	~VDevice ();

    //  Access
    private:
	virtual bool getName_(VkStatus &rStatus, VString &rName) = 0;
#ifndef __VMS
        virtual bool getSocket_ (VkStatus &rStatus, SOCKET& rSocket) = 0;
#endif
    public:
	bool getName (VString &rName, VkStatus &rStatus) {
	    return getName_(rStatus, rName);
	}
#ifndef __VMS
        bool getSocket (VkStatus &rStatus, SOCKET& rSocket) {
            return getSocket_(rStatus, rSocket);
        }
#endif

    //  Face Access
    public:
        virtual operator BSReadFace* () { return 0; }
        virtual operator BSWriteFace* () { return 0; }

    //  User Creation
    private:
	virtual bool supplyBSProducerConsumer_(
	    VReference<VBSProducer>&rpBSProducer, VReference<VBSConsumer>&rpBSConsumer
	);
	virtual bool supplyBSProducer_(VReference<VBSProducer>&rpUser);
	virtual bool supplyBSConsumer_(VReference<VBSConsumer>&rpUser);
	virtual bool supplyConnection_(VReference<VConnection>&rpUser);
	virtual bool supplyListener_(VReference<VListener>&rpUser);

    public:
	bool supply (VReference<IConnection>&rpInterface);
	bool supply (VReference<IListener>  &rpInterface);
	bool supply (VReference<IPipeSource>&rpInterface);

	bool supply (
	    VReference<VBSProducer>&rpBSProducer, VReference<VBSConsumer>&rpBSConsumer
	) {
	    return supplyBSProducerConsumer_(rpBSProducer, rpBSConsumer);
	}
	bool supply (VReference<VBSProducer>&rpUser) {
	    return supplyBSProducer_(rpUser);
	}
	bool supply (VReference<VBSConsumer>&rpUser) {
	    return supplyBSConsumer_(rpUser);
	}
	bool supply (VReference<VConnection>&rpUser) {
	    return supplyConnection_(rpUser);
	}
	bool supply (VReference<VListener>&rpUser) {
	    return supplyListener_(rpUser);
	}
    };

    typedef VDevice::BSReadArea		VDeviceBSReadArea;
    typedef VDevice::BSReadFace		VDeviceBSReadFace;
    typedef VDevice::BSReader		VDeviceBSReader;
    typedef VDevice::BSRead		VDeviceBSRead;

    typedef VDevice::BSWriteArea	VDeviceBSWriteArea;
    typedef VDevice::BSWriteFace	VDeviceBSWriteFace;
    typedef VDevice::BSWriter		VDeviceBSWriter;
    typedef VDevice::BSWrite		VDeviceBSWrite;

    typedef VDevice::BSPoll             VDevicePoll;

    typedef VDevice::ListenFace		VDeviceListenFace;
    typedef VDevice::Listener		VDeviceListener;
    typedef VDevice::Listen		VDeviceListen;

    typedef VDevice::Use		VDeviceUse;
    typedef VDevice::User		VDeviceUser;
}


#endif
