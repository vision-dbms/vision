#ifndef Vca_VcaDirectoryBuilder_Interface
#define Vca_VcaDirectoryBuilder_Interface

/************************
 *****  Components  *****
 ************************/

#include "VSimpleFile.h"

#include "Vca_VDirectoryBuilder.h"

#include "Vca_VDirectory.h"
#include "VkRegExp.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPipeSource.h"
#include "Vca_VGoferInterface.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * General directory builder for Vca services according to the session configuration files (sessions.cfg).
     */
    class Vca_API VcaDirectoryBuilder : public VDirectoryBuilder {
	DECLARE_CONCRETE_RTTLITE (VcaDirectoryBuilder, VDirectoryBuilder);

    //  Aliases
    public:
	/** VcaDirectoryBuilder Reference */
	typedef Reference BuilderReference;

        /** Convenience definition for pipe source gofers. */
	typedef VGoferInterface<IPipeSource> PipeSourceGofer;

    //  Order Class
    public:
        /**
         * Builder for directories.
         * Called 'Order' as in 'Work Order' as in 'Contractor' as in 'Builder'.
         */
	class Vca_API Order: public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Order, VRolePlayer);

	//  Enumerations
        public:
	    enum ParseState {
		ParseState_Error,
		ParseState_ExpectingTag,
		ParseState_ExpectingUnknown,
		ParseState_ExpectingIncludeTarget,
		ParseState_ExpectingIncludeIfTag,
		ParseState_ExpectingIncludeIfTarget,
		ParseState_ExpectingSessionSentinel,

		ParseState_ExpectingSessionName,
		ParseState_ExpectingSessionPattern,

		ParseState_ExpectingUseNamespaceTarget,

		ParseState_ExpectingUseTarget,
		ParseState_ExpectingElseUseTarget,

		ParseState_ExpectingHostName,
		ParseState_ExpectingUserName,
		ParseState_ExpectingPassword,
		ParseState_ExpectingProgram,
		ParseState_ExpectingRProgram,
		ParseState_ExpectingDatabase,
		ParseState_ExpectingPortNumber,
		ParseState_ExpectingRetryCount,
		ParseState_ExpectingRetryDelay1,
		ParseState_ExpectingRetryDelay2,
		ParseState_ExpectingServerFile,
		ParseState_ExpectingURL,
		ParseState_ExpectingEntryName,
		ParseState_ExpectingImageName,
		ParseState_ExpectingTimeout,
		ParseState_ExpectingOption,

		ParseState_ExpectingTTL,
		ParseState_ExpectingInfoServer,
		ParseState_ExpectingNSTemplate,
		ParseState_ExpectingNotificationSentinel
	    };
            /**
             * Type of a session; used to control UI visibility of sessions.
             * Each session is of exactly one type.
             */
	    enum SessionType {
		SESSION_GLOBAL, /**< Global sessions not visible in directory sessoin UIs presented to clients (but shown for developers/internal users). */
		SESSION_LOCAL, /**< Local sessions visible to all. */
		SESSION_HIDDEN /**< Hidden sessions not visible to anyone in directory selection UIs. */
	    };

	//  Friends
	    friend class VcaDirectoryBuilder;

	//  Globals
	protected:
            /** Contains a search pattern matching line termination. */
	    static char const* const g_pRestOfLineBreakSet;
            /** Contains a search pattern matching any whitespace. */
	    static char const* const g_pWhitespaceBreakSet;
	public:
            /**
             * Translates a SessionType into a human-readable string.
             *
             * @param xType the SessionType that should be translated into a string.
             * @return a human-readable representation of @p xType.
             * @todo Check documentation output of above return.
             */
	    static char const *SessionTypeCode (SessionType xType);

	//  Session Class
	public:
            /**
             * Factory type for directory entries, corresponds to a single Connection_Template definition (or equivalent, e.g. PoolConnection_Template) in a sessions.cfg file.
             * Session definitions are added to a directory under construction via a call to Order::insert(Session), typically resulting in the creation of a deferred (object
	     * source) directory entry bound to the gofer returned by 'supplyObjectSource'.
             * Abstract class; meant to be specialized for particular types of Sessions/usage.
             *
             * @see supplyObjectSource()
             */
	    class Vca_API Session : public VRolePlayer {
		DECLARE_ABSTRACT_RTTLITE (Session, VRolePlayer);

	    //  Aliases
	    public:
                /** Generic interface gofer. */
		typedef VGoferInterface<IVUnknown> gofer_t;

	    //  AuthoritativeSource (the official top level gofer type added to a directory).
	    public:
		class AuthoritativeSource;

	    //  Bindings
	    public:
		class Vca_API Bindings {
		    DECLARE_NUCLEAR_FAMILY (Bindings);

		//  Construction
		protected:
		    Bindings () {
		    }

		//  Destruction
		protected:
		    ~Bindings () {
		    }

		//  Access
		public:
		    virtual bool getBinding (VString& rResult, unsigned int xBinding) const = 0;
		    virtual VString const& input () const = 0;

		//  Use
		public:
		    virtual unsigned int edit (VString const& rInput, VString& rOutput) const = 0;
		    virtual VString edit (VString const& rInput) const = 0;
		};

	    //  Fallback
	    public:
                /**
                 * Descriptor representing ElseUse in a sessions file.
                 */
		class Vca_API Fallback : public VRolePlayer {
		    DECLARE_CONCRETE_RTTLITE (Fallback, VRolePlayer);

		//  Construction
		public:
		    Fallback (VString const &rName, ThisClass *pSuccessor = 0, ThisClass *pNamespace = 0);
		    Fallback (ThisClass const* pPattern, Bindings const& rBindings);

		//  Destruction
		private:
		    ~Fallback () {
		    }

		//  Instantiation
		public:
		    ThisClass* instantiationFor (Bindings const& rOther) const;
		//  Access
		public:
		    VString const &name () const {
			return m_iName;
		    }
		    ThisClass *namespase () const {
			return m_pNamespace;
		    }
		    ThisClass *successor () const {
			return m_pSuccessor;
		    }

                    /**
                     * Returns a reversal of this linked list of Fallback instances.
                     *
                     * Because the fallbacks are listed in reverse order, this routine puts them in 'fallback order' for use with human-readable description routines.
                     *
                     * @see m_pSuccessor
                     *
                     * @param[in] rpReversed an existing Fallback list to which this reversal will be prepended.
                     * @param[out] rpReversed the reference by which the reversed list of Fallback instances will be returned.
                     */
		    void getReversedList (Reference &rpReversed) const;

		//  Use
		public:
                    /**
                     * Manufactures a gofer for the object named by m_iName within the m_pNamespace directory, optionally with a fallback specified by @p rpGofer.
                     *
                     * @param[in] rpGofer the fallback for the gofer being created. If empty, no fallback is used.
                     * @param[out] rpGofer the reference by which the newly created gofer should be returned.
                     *
                     * @todo check if the documentation was rendered correctly.
                     */
		    void makeGofer (gofer_t::Reference &rpGofer) const;

		//  State
		private:
                    /** The name of the fallback entry. */
		    VString	const m_iName;

                    /** The namespace within which the name of this fallback entry is understood. */
		    Reference	const m_pNamespace;

                    /** The fallback for this fallback. */
		    Reference	const m_pSuccessor;
		};

	    //  Retry Settings
	    public:
		typedef VGofer::RetrySettings RetrySettings;
		typedef RetrySettings::count_t retry_count_t;
		typedef RetrySettings::delay_t retry_delay_t;

	    //  TTL Settings
	    public:
		typedef RetrySettings::microseconds_t microseconds_t;

	    //  Construction
	    protected:
		Session (VString const &rName, SessionType xType);
		Session (Session const *pPattern, Bindings const& rBindings);

	    //  Destruction
	    protected:
		~Session ();

	    //  Pattern Instantiation
	    private:
		virtual Session* instantiationFor_(Bindings const& rBindings) const;
	    private:
		Session* instantiationFor (Bindings const& rBindings) const;
		Fallback* fallbackListFor (Bindings const& rBindings) const;

	    //  Access
	    protected:
		virtual U64 timeout_() const;
		virtual bool wantsTimeout_() const;
	    public:
		VString const &getName () const {
		    return m_iName;
		}
		SessionType getType () const {
		    return m_xType;
		}
		char const *getTypeCode () const {
		    return SessionTypeCode (m_xType);
		}

		microseconds_t getTTL () const {
		    return m_sTTL;
		}
 
		/**
                 * Used to retrieve a textual description of this Session. Includes Global/Local tag, status message tags for apps (e.g. VED) which needs to know about the type of session amongst other things. The format of this description is stylized to provide short and long descriptions as well as visibility information.
                 *
                 * @param rDesc the reference by which the description for this Session is returned.
                 * @todo document exact syntax for this description by taking a look at how VED uses it.
                 */
		void getDescription (VString &rDesc) const;
		virtual void getInfo (VString &rDesc) const = 0;
		using BaseClass::getInfo;

                /**
                 * Returns IObjectSource to be inserted as a directory entry.
                 * Intended to be implemented in specializations of this class by way of an override for either the supplyObjectGofer_() (for gofer-based object sources) or supplyObjectSource_() (for non-gofer-based object sources) virtuals.
                 *
                 * In addition to calling its virtual delegate(s), this routine is responsible for handling logic generic to all sessions including ElseUse (fallbacks) and retries.
                 *
                 * @todo Make this routine responsible for timeouts, refactoring timeout logic to the generic session type.
                 *
                 * @param[out] rpObjectSource reference by which the new object source is returned.
                 * @return false on error, true otherwise.
                 */
		bool supplyObjectSource (gofer_t::Reference &rpObjectSource) const;

		/**
		 * Used to retrieve the timeout (in microseconds) for this session.  Zero specifies no timeout.
		 */
		U64 timeout () const {
		    return timeout_();
		}
		bool hasFallbacks () const {
		    return m_pFallbackList.isntNil ();
		}
		bool hasTTL () const {
		    return m_sTTL > 0;
		}
		bool isPattern () const {
		    return m_bPattern;
		}
		bool wantsRetries () const {
		    return m_iRetrySettings.retryCount () > 0;
		}
		bool wantsTimeout () const {
		    return wantsTimeout_();
		}

	    private:
		virtual void getStatusMessage_(VString &rDesc) const;

                /**
                 * Used to retrieve a VGoferInterface<IVUnknown> instance whose IDataSource role is the object source that this Session is expected to supply.
                 *
                 * @param[out] rpGofer the reference by which the VGoferInterface<IVUnknown> is returned.
                 * @return false on error, true otherwise.
                 */
		virtual bool supplyObjectGofer_(gofer_t::Reference& rpGofer) const;

                /**
                 * Default implementation for supplyObjectSource_() implemented using gofers.
                 * Calls supplyObjectGofer_() to retrieve an interface gofer, which provides the IObjectSource role, which is then returned via rpObjectSource.
                 *
                 * @param[out] rpObjectSource reference by which the new object source is returned.
                 * @return false on error, true otherwise.
                 */
		virtual bool supplyObjectSource_(IObjectSource::Reference& rpObjectSource) const;

	    //  Update
	    public:
		void setFallbackListTo (Fallback *pFallbackList) {
		    m_pFallbackList.setTo (pFallbackList);
		}
		void setIsPatternTo (bool bPattern) {
		    m_bPattern = bPattern;
		}
		void setRetrySettings (retry_count_t cRetries, retry_delay_t sDelayMin, retry_delay_t sDelayMax) {
		    m_iRetrySettings.setTo (cRetries, sDelayMin, sDelayMax);
		}
		void setRetrySettings (RetrySettings const& rSettings) {
		    m_iRetrySettings.setTo (rSettings);
		}
		void setTTL (microseconds_t sTTL) {
		    m_sTTL = sTTL;
		}

	    public:
		/**
		 * Adds a session to a directory.
		 *
		 * @param pSession the directory to which the Session will be added.
		 *
		 * @return true if the directory was updated, false otherwise.
		 */
		bool addToDirectory (VDirectory* pDirectory);
		bool addToDirectory (VDirectory* pDirectory, Bindings const& rBindings) const;

	    private:
                /** The name of this session. */
		VString		const	 m_iName;
		SessionType	const	 m_xType;
		Fallback::Reference	 m_pFallbackList;
		RetrySettings		 m_iRetrySettings;
		microseconds_t           m_sTTL;
		bool			 m_bPattern;
	    };

	//  Declarations
	public:
	    class AliasSession;
	    class LinkedSession;

	    class PlumbedSession;
	    class PortSession;
	    class ProcessSession;

	//  Constructor
	public:
            /**
             * Constructor expects a VDirectory instance to populate.
             *
             * @param pDirectory the VDirectory to populate.
             */
	    Order (VcaDirectoryBuilder* pBuilder, VDirectory* pDirectory);

	//  Destructor
	protected:
            /** Empty destructor restricts scope. */
	    ~Order ();

	//  Access
	public:
	    VcaDirectoryBuilder* builder () const {
		return m_pBuilder;
	    }

	//  Implementation
	protected:
            /**
             * Initiates processing of this Order, populating the underlying directory.
             * This is the method invoked by the VDirectoryBuilder that created this order.
             * @see Vca::VcaDirectoryBuilder::build()
             */
	    void buildDirectory ();

            /**
             * Parses a sessions file, overwriting existing directory entries as necessary.
             * Contains the main parse loop for a sessions file.
             *
             * @param pConfigFile the path to the sessions file to be parsed.
             * @param iType the type of the sessions within said file.
             */
	    virtual void parseSessionsFile (char const *pConfigFile, SessionType iType);

            /**
             * Parses a session template portion of a sessions file, generating a new Session instance in the process.
             *
             * This is called after 'Connection_Template Begin' has been parsed and is expected to parse through (consuming tokens from the token stream) the corresponding 'Connection_Template End'.
             *
             * @param rSessionsFile the sessions file positioned at the template's definition
             * @param iType the type to be assigned to the sessions within said sessions file.
             * @param rSession the reference by which the newly generated session is returned.
             * @return true on success, false on failure.
             */
	    bool parseSessionTemplate (VSimpleFile &rSessionsFile, SessionType iType, Session::Reference &rSession) const;
		
	    bool parseSessionTemplate (VString &rSessionsString, SessionType iType, Session::Reference &rSession) const;
	    
	    bool extractSessionTemplateString (VSimpleFile &rSessionsFile, VString &rTemplateString) const;

            /**
             * Adds a session to the underlying directory being populated.
             *
             * @param pSession the session to be added.
             */
	    virtual bool insertSession (Session *pSession);

	//  Query
	protected:
	    void getDirectoryDescription (VString &rDescription) const;
            /**
             * Used to determine whether or not this parser supports a given tag.
             *
             * @param pTag the tag whose support should be checked for in this parser.
             * @return true if the given tag is supported, false otherwise.
             *
             * @deprecated
             */
	    virtual bool includeIfTagAccepted (char const *pTag) const;

	//  State
	protected:
	    /** The directory builder fulfilling this Order. */
	    BuilderReference	  const	m_pBuilder;
            /** The directory being populated by this Order. */
	    VDirectory::Reference const m_pDirectory;
	};
	typedef Order::Session Session;

    //  Directory Class
    public:
	class Vca_API Directory : public VDirectory {
	    DECLARE_CONCRETE_RTTLITE (Directory, VDirectory);

	//  Pattern
	public:
	    class Pattern : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Pattern,VReferenceable);

	    //  Aliases
	    public:
		typedef VkRegExp::Query QueryBase;

	    //  Query
	    public:
		class Query;

	    // Construction
	    public:
		Pattern (VString const& rPattern, Session* pImplementation);

	    // Destruction
	    private:
		~Pattern ();

	    //  Use
	    public:
		bool addToDirectory (Directory* pDirectory, Query& rQuery) const;

		bool matches (Query& rQuery) const;
	    
	    // State
	    private:
		VString		const	m_iPattern;
		VkRegExp		mutable	m_iPatternRegex;
		Session::Reference	const	m_pImplementation;
	    };

	//  PatternMap
	public:
	    typedef VkMapOf<
	    EntryKey, VString const&, VString const&, Pattern::Reference
		> PatternMap;

	//  Construction
	public:
	    Directory ();

	//  Destruction
	private:
	    ~Directory () {
	    }

	//  Access
	public:
	    bool hasPattern (VString const& rPattern) const;
	protected:
	    bool locate (Entry::Reference &rpEntry, VString const &rName) OVERRIDE;

	//  Update
	public:
	    void setPattern (VString const& rPattern, Session* pSession);

	//  State
	private:
	    PatternMap m_iPatterns;
	};

    //  Construction
    public:
        /** Empty default constructor. */
	VcaDirectoryBuilder ();

    //  Destruction
    protected:
        /** Empty constructor restricts scope. */
	~VcaDirectoryBuilder ();

    //  Directory Creation
    private:
	/**
	 * Virtual member called to create a new directory on behalf of create(VDirectory::Reference& rpDirectory).
	 * Creates a VcaDirectoryBuilder::Directory implementing regular expression entry patterns.
	 */
	VDirectory* newDirectory_() const OVERRIDE;
    public:
        /**
         * Populates a directory by delegating to Order::buildDirectory().
         *
         * @param pDirectory the directory to be populated.
         * @see Vca::VDirectoryBuider::build()
         * @see Order::buildDirectory()
         */
	void build (VDirectory *pDirectory) OVERRIDE;
    };
}

#endif
