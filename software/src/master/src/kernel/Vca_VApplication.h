#ifndef Vca_VApplication_Interface
#define Vca_VApplication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"
#include "Vca_VApplicationContext.h"

#include "V_VCommandLine.h"

#include "Vca_IApplication.h"
#include "Vca_IPauseResume.h"
#include "Vca_IStop.h"
#include "Vca_IDie.h"

#include "Vca_VcaGofer.h"
#include "Vca_VTrackableCounter.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkStatus;

#include "Vca_IConnectionFactory.h"
#include "Vca_IListenerFactory.h"
#include "Vca_IPipeFactory.h"
#include "Vca_IProcessFactory.h"
#include "Vca_IStdPipeSource.h"

/*************************
 *****  Definitions  *****
 *************************/


namespace Vca {
    class VBSProducer;
    class VBSConsumer;

    /**
     * Abstract class providing tools for creating standalone programs that expose and consume Vca-enabled objects in a distributed environment.
     *
     * @par Subclassing
     * Developers who use the functionality provided by this class will almost certainly actually derive their application classes from one of two direct subclasses of VApplication.
     *  - VServerApplication for making available, in a distributed environment, Vca-enabled objects intended for use by others.
     *  - VClientApplication for making use of Vca-enabled objects that be have been made available in a distributed environment.
     * At the very least, subclasses should provide an implementation of start_().
     *
     * @par Functionality Provided
     * Functionality provided by this class includes:
     *  - Remote object access.
     *  - Command-line and environment handling.
     *  - Program execution and control.
     *  - Logging.
     *
     * @par Interfaces Exposed
     * There are several interfaces that provide access to said functionality, which may be of interest to consumers. Any of these interfaces can be queried for by clients of this application, and subsequently used to control the applicable features of this application.
     *  - IApplication
     *  - IPauseResume
     *  - IStop
     *  - IDie
     *
     * @par Remote Object Access
     * VApplication provides standardized mechanisms by which applications can be instructed to connect to remote objects. The provided mechanisms assume that applications typically first connect to one 'master' remote object -- we'll call this the <i>server object</i> -- and subsequently use this server object to connect to other remote objects/interfaces. As such, within the context of VApplication and its subclasses, there is the notion of a server name, or the name (within some namespace, typically TCP/IP) and a gofer that will make use of the server name and namespace in order to make the appropriate connections and retrieve an interface to the server object.
     */
    class ABSTRACT Vca_Main_API VApplication : public VActivity, public VApplicationContext::ApplicationAddin {
        DECLARE_ABSTRACT_RTTLITE (VApplication, VActivity);
	DECLARE_ROLEPLAYER_CLASSINFO ();

    //  Aliases
    public:
	/** Convenience definition for nested classes.  */
	typedef Reference ApplicationReference;

        /** Convenience definition for command-line descriptor class. */
        typedef V::VCommandLine Command;
        /** The context of VApplication, including command-line and environment information. */
        typedef VApplicationContext Context;

        /**
         * Convenience definition for an IVUnknown VGofer, used to fetch interfaces from a server.
         */
        typedef VGoferInterface<IVUnknown> server_gofer_t;

    //********************************************************************************************/
    //  Activity
    public:
	class Vca_Main_API Activity : public VActivity {
	    DECLARE_ABSTRACT_RTTLITE (Activity, VActivity);

	//  Construction
	protected:
	    Activity (VApplication *pApplication);

	//  Destruction
	protected:
	    ~Activity ();

	//  Access/Query
	protected:
	    VApplication* application() const {
		return m_pApplication;
	    }

	    char const *commandStringValue (			// -xyz=StringValue
		char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
	    ) const {
		return m_pApplication->commandStringValue (pTag, pVariable, pDefaultValue);
	    }
	    bool commandSwitchValue (				// -xyz
		char const *pTag, char const *pVariable = 0
	    ) const {
		return m_pApplication->commandSwitchValue (pTag, pVariable);
	    }

	    template<typename class_t, typename interface_t> bool getServerInterface (
		class_t *pSink, void (class_t::*pValueSink)(interface_t*), void (class_t::*pErrorSink)(IError*,VString const&)
	    ) {
		return m_pApplication->getServerInterface (pSink, pValueSink, pErrorSink, false);
	    }
	    template <typename class_t, typename interface_t> bool getServerInterface (
		class_t *pSink, void (class_t::*pValueSink)(interface_t*)
	    ) {
		return m_pApplication->getServerInterface (pSink, pValueSink, false);
	    }
	    template <typename gofer_reference_t> bool getInterfaceGofer (gofer_reference_t &rpGofer) {
		return m_pApplication->getInterfaceGofer (rpGofer);
	    }

	//  Use
	protected:
	    void onActivitySuccess () {
		onSuccess ();
	    }
	    void onActivityFailure (IError *pInterface, VString const &rMessage) {
		onFailure (pInterface, rMessage);
	    }
	    void onDone () {
		onSuccess ();
	    }

	    void onSuccess ();
	    void onFailure (IError *pInterface, VString const &rMessage);

	//  State
	private:
	    ApplicationReference const m_pApplication;
            /** The run state of this Activity. */
	    V::VTwiddler m_bRunning;
	};
	friend class Activity;

    //********************************************************************************************/
    //  Command Cursor
    public:
        class Vca_Main_API CommandCursor : public Command::Cursor {
            DECLARE_FAMILY_MEMBERS (CommandCursor, Command::Cursor);

        //  Construction
        public:
            CommandCursor (CommandCursor const &rOther) : BaseClass (rOther) {
            }
            CommandCursor (VApplication const *pApplication) : BaseClass (&pApplication->commandLine ()) {
            }

        //  Destruction
        public:
            ~CommandCursor() {
            }
        };
        friend class CommandCursor;

    //********************************************************************************************/
    //  StateTransition
    public:
	class StateTransition;
	friend class StateTransition;

    //********************************************************************************************/
    //  Construction
    protected:
        /**
         * Constructor expects a context within which the application is loaded.
         * Must be called by subclasses, passing in the appropriate Context.
         *
         * @param pContext the Context within which this application is loaded.
         */
        VApplication (Context *pContext);

    //  Destruction
    protected:
        /** Destructor automatically stops this application. */
        ~VApplication ();

    //  Access
    public:
	VString description () const;

    //  Base Roles
    public:
        using BaseClass::getRole;

    /**
     * @name IApplication Implementation
     * VApplication implements the IApplication interface, and these members support said implementation.
     */
    //@{

    //  IApplication Role
    private:
        /** The IApplication role for this VApplication. */
        VRole<ThisClass,IApplication> m_pIApplication;
    public:
        /**
         * Retrieves the IApplication role for this VApplication.
         *
         * @param[out] rpRole the reference that will be used to return the IApplication for this VApplication.
         */
        void getRole (IApplication::Reference &rpRole) {
            m_pIApplication.getRole (rpRole);
        }

    //  IApplication Methods
    public:
        /** Implements Vca::IApplication::GetLogFilePath(). */
        void GetLogFilePath (
            IApplication *pRole, IVReceiver<VString const &> *pClient
        );
        /** Implements Vca::IApplication::SetLogFilePath(). */
        void SetLogFilePath (
             IApplication *pRole, VString const& iValue
        );
        /** Implements Vca::IApplication::GetLogSwitch(). */
        void GetLogSwitch (
            IApplication *pRole, IVReceiver<bool> *pClient
        );
        /** Implements Vca::IApplication::SetLogSwitch(). */
        void SetLogSwitch (
             IApplication *pRole, bool iValue
        );
        /** Implements Vca::IApplication::GetLogFileSize(). */
        void GetLogFileSize (
            IApplication *pRole, IVReceiver<U32> *pClient
        );
        /** Implements Vca::IApplication::SetLogFileSize(). */
        void SetLogFileSize (
            IApplication *pRole, U32 iValue
        );
        /** Implements Vca::IApplication::GetLogFileBackups(). */
        void GetLogFileBackups (
            IApplication *pRole, IVReceiver<U32> *pClient
        );
        /** Implements Vca::IApplication::SetLogFileBackups(). */
        void SetLogFileBackups (
            IApplication *pRole, U32 iValue
        );
        /** Implements Vca::IApplication::GetStatistics(). */
        void GetStatistics (
            IApplication *pRole,  IVReceiver<VString const&> *pClient
        );
    //@} IApplication Implementation

    /** @name IPauseResume Implementation */
    //@{

    //  IPauseResume Role
    private:
        /** The IPauseResume role for this VApplication. */
        VRole<ThisClass,IPauseResume> m_pIPauseResume;
    public:
        /**
         * Retrieves the IPauseResume role for this VApplication.
         *
         * @param[out] rpRole the reference that will be used to return the IPauseResume for this VApplication.
         */
        void getRole (IPauseResume::Reference &rpRole) {
            m_pIPauseResume.getRole (rpRole);
        }

    //  IPauseResume Methods
    public:
        /** Implements Vca::IPauseResume::Pause(). */
        void Pause  (IPauseResume *pRole);
        /** Implements Vca::IPauseResume::Resume(). */
        void Resume (IPauseResume *pRole);

    //@}

    /** @name IStop Implementation */
    //@{

    //  IStop Role
    private:
        /** The IStop role for this VApplication. */
        VRole<ThisClass,IStop> m_pIStop;
    public:
        /**
         * Retrieves the IStop role for this VApplication.
         *
         * @param[out] rpRole the reference that will be used to return the IStop for this VApplication.
         */
        void getRole (IStop::Reference &rpRole) {
            m_pIStop.getRole (rpRole);
        }

    //  IStop Methods
    public:
        /** Implements Vca::IStop::Stop(). */
        void Stop (IStop *pRole, bool bHardStop);

    //@}

    /** @name IDie Implementation */
    //@{

    //  IDie Role
    private:
        /** The IDie role for this VApplication. */
        VRole<ThisClass,IDie> m_pIDie;
    public:
        /**
         * Retrieves the IDie role for this VApplication.
         *
         * @param[out] rpRole the reference that will be used to return the IDie for this VApplication.
         */
        void getRole (IDie::Reference &rpRole) {
            m_pIDie.getRole (rpRole);
        }

    //  IDie Methods
    public:
        /** Implements Vca::IDie::Die(). */
        void Die (IDie *pRole);

    //@}

    /// Routines used by subclasses of VApplication::Activity and VApplication to register the existence and completion of the subtasks that are keeping the application alive.
    //@{
    public:
	count_t activityCount () const {
	    return m_iActivityCount;
	}

    protected:
	void incrementActivityCount();
	void decrementActivityCount();

	virtual void onActivityCountWasZero ();
	virtual void onActivityCountIsZero ();
    //@}

    /// @name Command Line Access
    /// Convenience aliases to the appropriate calls within the Context instace for this application (#m_pContext).
    //@{
    public:
        /** Returns the name used to invoke this application. */
        char const *arg0 () const {
            return m_pContext->arg0 ();
        }
        /** Returns the number of arguments that this application was invoked with. */
        unsigned int argc () const {
            return m_pContext->argc ();
        }
        /** Returns this application's argument list. */
        argv_t argv () const {
            return m_pContext->argv ();
        }

        /**
         * Returns the command-line descriptor that this application was started with.
         * @copydetails Context::commandLine()
         */
        Command const &commandLine () const {
            return m_pContext->commandLine ();
        }

        /**
         * @copydoc Context::commandStringValue()
         */
        char const *commandStringValue (                        // -xyz=StringValue
            char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
        ) const {
            return m_pContext->commandStringValue (pTag, pVariable, pDefaultValue);
        }

        /**
         * @copydoc Context::commandSwitchValue()
         */
        bool commandSwitchValue (                               // -xyz
            char const *pTag, char const *pVariable = 0
        ) const {
            return m_pContext->commandSwitchValue (pTag, pVariable);
        }

        /**
         * @copydoc Context::getCommandValue()
         */
        bool getCommandValue (                          // -xyz=size
            size_t &rValue, char const *pTag, char const *pVariable = 0
        ) const {
            return m_pContext->getCommandValue (rValue, pTag, pVariable);
        }

    //@}

    /// @name Connection Support
    //@{
    private:
        /**
         * Called by getServerName() to retrieve this application's default server name if none was specified via command-line nor environment; can be overridden by subclasses to provide a default server name for that application.
         * The stock implementation returns false and does nothing.
         *
         * @param[out] rName the VString reference that will be used to return the default server name for this application.
         * @return true if a server name was set, false otherwise.
         */
        virtual bool getDefaultServerName (VString &rName) const;

    protected:
        /**
         * Retrieves the external object/server that this application will use.
         * There are four mechanisms by which the server name can be supplied. They are attempted in the following order until a server name can be found. If one is still not found after the last mechanism has been attempted, a call to getDefaultServerName() concludes the server name retreival process.
         *
         * @param[out] rServerName the reference that should be used to return the server name.
         * @param pServerTag the name of the command-line parameter supplying the server name.
         * @param pServerEnv the name of the environment variable supplying the server name in the event that the above mechanism fails.
         * @param pServerFileTag the name of the command-line parameter supplying the server file name that contains the server name, in the event that the above mechanisms fail.
         * @param pServerFileEnv the name of the environment variable supplying the server file name that contains the server name, in the event that the above mechanisms fail.
         */
        bool getServerName (
            VString &rServerName,
            char const *pServerTag, char const *pServerEnv,
            char const *pServerFileTag, char const *pServerFileEnv
        ) const;

        /**
         * @copydoc getServerName(VString &)const
         * @param pServerTag the name of the command-line parameter supplying the server name.
         * @param pServerEnv the name of the environment variable supplying the server name in the event that the above mechanism fails.
         */
        bool getServerName (
            VString &rServerName, char const *pServerTag, char const *pServerEnv
        ) const;

        /**
         * @copybrief getServerName(VString &rServerName, char const *pServerTag, char const *pServerEnv, char const *pServerFileTag, char const *pServerFileEnv)const
         *
         * Implemented via a call to #getServerName(VString &rServerName, char const *pServerTag, char const *pServerEnv, char const *pServerFileTag, char const *pServerFileEnv)const
         *
         * @param[out] rServerName the reference that should be used to return the server name.
         * @return true if a server name was set, false otherwise.
         */
        bool getServerName (VString &rServerName) const;

    public:
        /**
         * Retrieves an interface of a given type from this application's server object.
         *
         * Uses VGoferInterface to query for interfaces.
         *
         * @tparam class_t the type of the object that should receive the requested interface. Must derive from VReferenceable.
         * @tparam interface_t the type of the interface being requested.
         * @param pSink pointer to an instance of the class that should receive the requested interface.
         * @param pSinkDataMember pointer to the member function within pSink that should be used to return an instance of the requested interface.
         * @param pSinkErrorMember pointer to the member function within pSink that should be used to indicate an error while retreiving the requested interface.
         * @return true if the parameters of this application identify a server object, false otherwise.
         */
        template <typename class_t, typename interface_t> bool getServerInterface (
            class_t *pSink,
            void (class_t::*pSinkDataMember)(interface_t*),
            void (class_t::*pSinkErrorMember)(IError*,VString const&),
	    bool bNewActivity = true
        ) {
            typename VGoferInterface<interface_t>::Reference pGofer;
            if (!getInterfaceGofer (pGofer))
                return false;

	    if (bNewActivity)
		incrementActivityCount ();
            pGofer->supplyMembers (pSink, pSinkDataMember, pSinkErrorMember);
            return true;
        }

        /**
         * Retrieves an interface of a given type from this application's server object.
         *
         * Uses VGoferInterface to query for interfaces.
         *
         * @tparam class_t the type of the object that should receive the requested interface. Must derive from VReferenceable and must implement a member function whose name and signature matches: <code>OnConnectError(IError*, VString const &)</code>.
         * @tparam interface_t the type of the interface being requested.
         * @param pSink pointer to an instance of the class that should receive the requested interface.
         * @param pSinkDataMember pointer to the member function within pSink that should be used to return an instance of the requested interface.
         * @return true if the parameters of this application identify a server object, false otherwise.
         */
        template <typename class_t, typename interface_t> bool getServerInterface (
            class_t *pSink, void (class_t::*pSinkDataMember)(interface_t*), bool bNewActivity = true
        ) {
            typedef void (class_t::*sink_member_t)(IError*,VString const&);
            sink_member_t const pSinkErrorMember = &class_t::onConnectError;
	    return getServerInterface (pSink, pSinkDataMember, pSinkErrorMember, bNewActivity);
        }

        /**
         * Used to retrieve a gofer for an interface of a particular type.
         * Used by getServerInterface().
         *
         * @tparam gofer_reference_t must be of type Vca::VGoferInterface<interface_t> where interface_t is the interface type that's being queried for.
         * @param[out] rpGofer the reference that will be used to return a gofer for the requested interface.
         * @return true if a gofer could be constructed for the server object, false otherwise.
         */
        template <typename gofer_reference_t> bool getInterfaceGofer (gofer_reference_t &rpGofer) {
	    server_gofer_t::Reference pServerGofer;
            rpGofer.setTo (
                getServerGofer (pServerGofer) ? (
                    new Gofer::Queried<typename gofer_reference_t::ReferencedClass::interface_t> (
                        pServerGofer
                    )
                ) : 0
            );
            return rpGofer.isntNil ();
        }
    public:
        /**
         * Used to obtain an uncached gofer for the server object.
         *
         * @param rpServerGofer the reference that should be used to return the generated gofer.
         * @param rServerName the name of the remote object for which a gofer should be generated.
         * @return true if a server gofer could be generated, false otherwise.
         */
        bool getServerGofer (server_gofer_t::Reference &rpServerGofer, VString const &rServerName) const;
    private:
        /**
         * Used to obtain a gofer for the server object.  The gofer returned from this routine will be cached
	 * if it didn't come from the directory.
         *
         * @param rpServerGofer the reference that should be used to return the generated gofer.
         * @return true if a server gofer could be generated, false otherwise.
         */
        bool getServerGofer (server_gofer_t::Reference &rpServerGofer);

        /**
         * Used to cache a gofer for the server object.  The gofer returned from this routine will be cached
	 * if it didn't come from the directory.
         *
         * @param pServerGofer the gofer to be cached.
         * @return true if a server gofer exists, false otherwise.
         */
	bool setServerGofer (server_gofer_t *pServerGofer);

    protected:
        /**
         * Generic callback for pipe connection errors.
         *
         * @param[in] pError interface to the error generated.
         * @param[in] rMessage the error message.
         */
        virtual void onConnectError (IError *pError, VString const &rMessage);

    public:
        /** Returns true if this application was invoked with the @c -directory switch. */
        bool usingDirectory () const;

    private:
        /**
         * Asynchronous return mechanism used for passive callback gofers.
         *
         * @param[in] pCallback the IPassiveCallback returned.
         */
        void onGoferCallbackValue (IPassiveCallback *pCallback);

        /** @copydoc onConnectError() */
        void onGoferCallbackError (IError *pError, VString const &rMessage);

    //@}

    /// @name Device Factory Access
    /// Convenience aliases for the appropriate factory mechanisms.
    //@{
    protected:
        /** @copydoc VConnectionFactory::Supply() */
        bool supply (IConnectionFactory::Reference  &rpInterface) const;
        /** @copydoc VListenerFactory::Supply() */
        bool supply (IListenerFactory::Reference    &rpInterface) const;
        /** @copydoc VConnectionFactory::Supply() */
        bool supply (IPipeFactory::Reference        &rpInterface) const;
        /** @copydoc VProcessFactory::Supply() */
        bool supply (IProcessFactory::Reference     &rpInterface) const;
        /** @copydoc VStdPipeSource::Supply() */
        bool supply (IStdPipeSource::Reference      &rpInterface) const;
    //@}

    /// @name Standard Channel Access
    /// Undocumented because these methods will probably change in order to allow for output redirections and multiple output processing layers within a VApplication subclass heirarchy.
    //@{
    protected:
	/**
	 *  Request a byte stream attached to this process' standard input.
	 *
	 *  The requested byte stream is returned via a callback to onStandardInput.  Override that virtual
	 *  in a subclass to receive the byte stream.  Errors that occur are returned via a callback to the
	 *  onStandardChannelAccessError virtual.  Override that member as well to receive those notifications.
	 *
	 *  @see onStandardInput
	 *  @see onStandardChannelAccessError
	 */
	void getStandardInput ();

	/**
	 *  Request a byte stream attached to this process' standard output.
	 *
	 *  The requested byte stream is returned via a callback to onStandardOutput.  Override that virtual
	 *  in a subclass to receive the byte stream.  Errors that occur are returned via a callback to the
	 *  onStandardChannelAccessError virtual.  Override that member as well to receive those notifications.
	 *
	 *  @see onStandardOutput
	 *  @see onStandardChannelAccessError
	 */
	void getStandardOutput	();

	/**
	 *  Request a byte stream attached to this process' standard error.
	 *
	 *  The requested byte stream is returned via a callback to onStandardError.  Override that virtual
	 *  in a subclass to receive the byte stream.  Errors that occur are returned via a callback to the
	 *  onStandardChannelAccessError virtual.  Override that member as well to receive those notifications.
	 *
	 *  @see onStandardError
	 *  @see onStandardChannelAccessError
	 */
	void getStandardError ();

	/**
	 *  Request byte streams attached to this process' standard input and output.
	 *
	 *  The requested byte stream is returned via a callback to onStandardChannels.  Override that virtual
	 *  in a subclass to receive the byte streams.  Errors that occur are returned via a callback to the
	 *  onStandardChannelAccessError virtual.  Override that member as well to receive those notifications.
	 *
	 *  @see onStandardChannels
	 *  @see onStandardChannelAccessError
	 */
	void getStandardChannels2 ();

	/**
	 *  Request byte streams attached to this process' standard input, output, and error.
	 *
	 *  The requested byte stream is returned via a callback to onStandardChannels.  Override that virtual
	 *  in a subclass to receive the byte streams.  Errors that occur are returned via a callback to the
	 *  onStandardChannelAccessError virtual.  Override that member as well to receive those notifications.
	 *
	 *  @see onStandardChannels
	 *  @see onStandardChannelAccessError
	 */
	void getStandardChannels3 ();
    private:
        virtual void onStandardInput (VBSProducer *pI);
        virtual void onStandardOutput (VBSConsumer *pO);
        virtual void onStandardError (VBSConsumer *pE);
        virtual void onStandardChannels (VBSProducer *pI, VBSConsumer *pO);
        virtual void onStandardChannels (VBSProducer *pI, VBSConsumer *pO, VBSConsumer *pE);

	virtual void onStandardChannelAccessError (IError *pError, VString const &rMessage);
    private:
	void onStandardChannelAccessStatus (VkStatus const& rStatus);
    //@}

    /// @name Usage Reporting
    //@{
    protected:
        void displayUsagePattern (char const *pPattern) const;
        void __cdecl displayUsagePatterns (char const *pPattern, ...) const;
        static void __cdecl displayOptions (char const *pPattern, ... );
        void displayStandardOptions ();
    //@}

    //  Query
    public:
        /** Returns true if this application has been invoked using the @c -background switch. */
        bool isBackground () const;

    /// @name Exit Status
    //@{
    public:
        /** Returns the exit status of this application. */
        int getExitStatus () const {
          return m_pContext->getExitStatus ();
        }
    protected:
        /**
         * Sets the exit status of this application.
         *
         * @param xExitStatus the new exit status of this application.
         */
        void setExitStatus (int xExitStatus) {
            m_pContext->setExitStatus (xExitStatus);
        }
    //@}

    /** @name Run State Access/Query */
    //@{
    protected:
	void getDescription_(VString& rResult) const OVERRIDE;
    public:
        /**
	 * Returns the name of a run state.
	 *
	 * @param xRunState the run state whose name is wanted.
	 *
	 * @return a null terminated string or null if the run state is unknown.
	 */
	static char const* RunStateName (RunState xRunState);
	static void GetRunStateName (VString& rResult, RunState xRunState);

        /** Returns the current run state of this application. */
        RunState runState () const {
            return m_xRunState;
        }
        /** Returns the name of the current run state of this application. */
	char const* runStateName () const {
	    return RunStateName (m_xRunState);
	}
        /** Returns true if this application is currently awaiting start. */
        bool isAwaitingStart () const {
            return m_xRunState == RunState_AwaitingStart;
        }
        /** Returns true if this application is currently starting. */
        bool isStarting () const {
            return m_xRunState == RunState_Starting;
        }
        /** Returns true if this application is currently running. */
        bool isRunning () const {
            return m_xRunState == RunState_Running;
        }
        /** Returns true if this application is currently pausing. */
        bool isPausing () const {
            return m_xRunState == RunState_Pausing;
        }
        /** Returns true if this application is currently paused. */
        bool isPaused () const {
            return m_xRunState == RunState_Paused;
        }
        /** Returns true if this application is currently resuming. */
        bool isResuming () const {
            return m_xRunState == RunState_Resuming;
        }
        /** Returns true if this application is currently stopping. */
        bool isStopping () const {
            return m_xRunState == RunState_Stopping;
        }
        /** Returns true if this application is currently awaiting stop.. */
        bool isAwaitingStop () const {
            return m_xRunState == RunState_AwaitingStop;
        }
        /** Returns true if this application is currently stopped. */
        bool isStopped () const {
            return m_xRunState == RunState_Stopped;
        }
        /** Returns true if this application is @e not stopping. */
        bool isntStopping () const {
            return m_xRunState != RunState_Stopping;
        }
        /** Returns true if this application is currently @e not awaiting stop. */
        bool isntAwaitingStop () const {
            return m_xRunState != RunState_AwaitingStop;
        }
        /** Returns true if this application is currently @e not stopped. */
        bool isntStopped () const {
            return m_xRunState != RunState_Stopped;
        }
        /**
         * Returns true if this application is currently stopping or optionally if this application is @e not stopped.
         *
         * @param bHardStop if false, this method will return true only if this application is stopped; if true, this method will additionally return true if this application is @e not stopped.
         */
        bool isStopping (bool bHardStop) const {
            return isStopping () || bHardStop && isntStopped ();
        }
    //@}

    /// @name Run State Control
    //@{
    public:
        /**
         * Starts this application.
         */
        bool start ();
	/**
	 * Stops this application gracefully via its activity count.
	 */
        /**
         * Stops this application, optionally gracefully.
         *
         * @param bHardStop if false, this application will be stopped gracefully.
         */
        bool stop (bool bHardStop = false);
        /**
         * Stops this application.
         * This is an alias to stop() with @c true given as the @c bHardStop parameter.
         */
        bool hardstop () {
            return stop (true);
        }
        /** @copydoc pause() */
        bool suspend () {
            return pause ();
        }
        /**
         * Pauses this application.
         */
        bool pause ();
        /**
         * Resumes this application.
         */
        bool resume ();
    //@}

    /// @name Run State Transition
    /// When overriding these methods, you must @e always invoke the BaseClass implementation and continue state transition only if the return value thereof is true. Overrides should also return the appropriate run state query call. For examples, see Vsa::VPoolApplication.
    /// @todo Example code.
    //@{
    private:
	RunState makeRunState (RunState xNewState);
    protected:
        /**
         * Sets the run state if and only if the current run state is as expected.
         *
         * @param xOldState the expected current run state.
         * @param xNewState the new run state to enter.
         * @return false on failure, true otherwise.
         */
        bool setRunStateIf (RunState xOldState, RunState xNewState);

        /**
         * Sets the run state to stopped.
         *
         * @return false on failure, true otherwise.
         */
        bool setRunStateToStopped ();

        /**
         * Handles application startup; must be implemented by concrete subclasses.
         * Stock implementation does nothing, so this application will exit unless this method is overridden.
         *
         * @return true if the state transition was successful.
         */
        virtual bool start_();

        /**
         * Handles application teardown.
         * Can be overridden by subclasses to provide custom teardown processing.
         *
         * @param bHardStop if false, this application should be stopped gracefully.
         * @return true if the state transition was successful.
         */
        virtual bool stop_(bool bHardstop);

        /**
         * Handles application pausing.
         * Can be overridden by subclasses to provide custom pause processing.
         *
         * @return true if the state transition was successful.
         */
        virtual bool pause_();

        /**
         * Handles application resuming.
         * Can be overriden by subclasses to provide custom resume processing.
         *
         * @return true if the state transition was successful.
         */
        virtual bool resume_();
    //@}

    //  Run State Change Callback
    public:
        /**
         * Used to supply this class with a IRunStateReceiver delegate that should be informed when this application's run state has changed.
         *
         * @param pReceiver the new IRunStateReceiver that should be be notified of run state changes.
         */
        void onRunStateChangeNotify (IRunStateReceiver *pReceiver);

    //  Statistics
    public:
        /**
         * Used to retrieve run-time statistics about this application.
         *
         * @param[out] rResult the reference by which runtime statistics should be returned.
         */
        void getStatistics (VString &rResult) const;

    /// @name Tracking and Trackables
    //@{
    public:
	VTrackable_count_t const &activityCountTrackable ();
    //@}

    /// @name State
    //@{
    private:
        /** The context of this application, including environment and command-line information. */
        Context::Reference const m_pContext;
        /** The run state of this application. */
        RunState m_xRunState;
        /** This application's IRunStateReceiver. */
        IRunStateReceiver::Reference m_pRunStateReceiver;
        /** The server object gofer for this application. */
        server_gofer_t::Reference m_pServerGofer;
        /** The number of activities that this client application still has to process. */
	VTrackableCounter<counter_t> m_iActivityCount;
    //@}
    };
}


#endif
